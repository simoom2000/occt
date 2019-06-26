// Author: Kirill Gavrilov
// Copyright (c) 2016-2019 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include "RWGltf_GltfJsonParser.pxx"

#include <BRep_Builder.hxx>
#include <gp_Quaternion.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressSentry.hxx>
#include <OSD_File.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Path.hxx>
#include <OSD_ThreadPool.hxx>
#include <Precision.hxx>
#include <FSD_Base64Decoder.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

#include <fstream>

#ifdef HAVE_RAPIDJSON
namespace
{
  //! Material extension.
  const char THE_KHR_materials_common[] = "KHR_materials_common";
  const char THE_KHR_binary_glTF[]      = "KHR_binary_glTF";
}

//! Find member of the object in a safe way.
inline const RWGltf_JsonValue* findObjectMember (const RWGltf_JsonValue& theObject,
                                                 const RWGltf_JsonValue& theName)
{
  if (!theObject.IsObject()
   || !theName.IsString())
  {
    return NULL;
  }

  rapidjson::Document::ConstMemberIterator anIter = theObject.FindMember (theName);
  return anIter != theObject.MemberEnd()
       ? &anIter->value
       : NULL;
}

//! Find member of the object in a safe way.
inline const RWGltf_JsonValue* findObjectMember (const RWGltf_JsonValue& theObject,
                                                 const char*  theName)
{
  if (!theObject.IsObject())
  {
    return NULL;
  }

  rapidjson::Document::ConstMemberIterator anIter = theObject.FindMember (theName);
  return anIter != theObject.MemberEnd()
       ? &anIter->value
       : NULL;
}

// =======================================================================
// function : RWGltf_GltfJsonParser::FormatParseError
// purpose  :
// =======================================================================
const char* RWGltf_GltfJsonParser::FormatParseError (rapidjson::ParseErrorCode theCode)
{
  switch (theCode)
  {
    case rapidjson::kParseErrorNone:                          return "";
    case rapidjson::kParseErrorDocumentEmpty:                 return "Empty Document";
    case rapidjson::kParseErrorDocumentRootNotSingular:       return "The document root must not follow by other values";
    case rapidjson::kParseErrorValueInvalid:                  return "Invalid value";
    case rapidjson::kParseErrorObjectMissName:                return "Missing a name for object member";
    case rapidjson::kParseErrorObjectMissColon:               return "Missing a colon after a name of object member";
    case rapidjson::kParseErrorObjectMissCommaOrCurlyBracket: return "Missing a comma or '}' after an object member";
    case rapidjson::kParseErrorArrayMissCommaOrSquareBracket: return "Missing a comma or ']' after an array element";
    case rapidjson::kParseErrorStringUnicodeEscapeInvalidHex: return "Incorrect hex digit after \\u escape in string";
    case rapidjson::kParseErrorStringUnicodeSurrogateInvalid: return "The surrogate pair in string is invalid";
    case rapidjson::kParseErrorStringEscapeInvalid:           return "Invalid escape character in string";
    case rapidjson::kParseErrorStringMissQuotationMark:       return "Missing a closing quotation mark in string";
    case rapidjson::kParseErrorStringInvalidEncoding:         return "Invalid encoding in string";
    case rapidjson::kParseErrorNumberTooBig:                  return "Number is too big to be stored in double";
    case rapidjson::kParseErrorNumberMissFraction:            return "Miss fraction part in number";
    case rapidjson::kParseErrorNumberMissExponent:            return "Miss exponent in number";
    case rapidjson::kParseErrorTermination:                   return "Parsing was terminated";
    case rapidjson::kParseErrorUnspecificSyntaxError:         return "Unspecific syntax error";
  }
  return "UNKOWN syntax error";
}

// =======================================================================
// function : GltfElementMap::Init
// purpose  :
// =======================================================================
void RWGltf_GltfJsonParser::GltfElementMap::Init (const TCollection_AsciiString& theRootName,
                                                  const RWGltf_JsonValue* theRoot)
{
  myRoot = theRoot;
  myChildren.Clear();
  if (theRoot == NULL)
  {
    return;
  }

  if (theRoot->IsObject())
  {
    // glTF 1.0
    for (ConstMemberIterator aChildIter = theRoot->MemberBegin(); aChildIter != theRoot->MemberEnd(); ++aChildIter)
    {
      if (!aChildIter->name.IsString())
      {
        continue;
      }

      const TCollection_AsciiString aKey (aChildIter->name.GetString());
      if (!myChildren.Bind (aKey, &aChildIter->value))
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString ("Invalid glTF syntax - key '")
                                         + aKey + "' is already defined in '" + theRootName + "'.", Message_Warning);
      }
    }
  }
  else if (theRoot->IsArray())
  {
    // glTF 2.0
    int aChildIndex = 0;
    for (rapidjson::Value::ConstValueIterator aChildIter = theRoot->Begin(); aChildIter != theRoot->End(); ++aChildIter, ++aChildIndex)
    {
      myChildren.Bind (TCollection_AsciiString (aChildIndex), aChildIter);
    }
  }
}
#endif

// Auxiliary macros for formatting message.
#define reportGltfError(theMsg)   reportGltfSyntaxProblem(TCollection_AsciiString() + theMsg, Message_Fail);
#define reportGltfWarning(theMsg) reportGltfSyntaxProblem(TCollection_AsciiString() + theMsg, Message_Warning);

// =======================================================================
// function : reportGltfSyntaxProblem
// purpose  :
// =======================================================================
void RWGltf_GltfJsonParser::reportGltfSyntaxProblem (const TCollection_AsciiString& theMsg,
                                                    Message_Gravity theGravity)
{
  Message::DefaultMessenger()->Send (myErrorPrefix + theMsg, theGravity);
}

// =======================================================================
// function : RWGltf_GltfJsonParser
// purpose  :
// =======================================================================
RWGltf_GltfJsonParser::RWGltf_GltfJsonParser (TopTools_SequenceOfShape& theRootShapes)
: myRootShapes(&theRootShapes),
  myAttribMap (NULL),
  myExternalFiles (NULL),
  myBinBodyOffset (0),
  myBinBodyLen (0),
  myIsBinary (false),
  myIsGltf1 (false),
  myToSkipEmptyNodes (true),
  myToProbeHeader (false)
{
  myCSTrsf.SetInputLengthUnit (1.0); // meters
  myCSTrsf.SetInputCoordinateSystem (RWMesh_CoordinateSystem_glTF);
}

// =======================================================================
// function : SetFilePath
// purpose  :
// =======================================================================
void RWGltf_GltfJsonParser::SetFilePath (const TCollection_AsciiString& theFilePath)
{
  myFilePath = theFilePath;
  // determine file location to load associated files
  TCollection_AsciiString aFileName;
  OSD_Path::FolderAndFileFromPath (theFilePath, myFolder, aFileName);
}

#ifdef HAVE_RAPIDJSON
// =======================================================================
// function : gltfParseRoots
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParseRoots()
{
  // find glTF root elements for smooth navigation
  RWGltf_JsonValue aNames[RWGltf_GltfRootElement_NB];
  for (int aRootNameIter = 0; aRootNameIter < RWGltf_GltfRootElement_NB; ++aRootNameIter)
  {
    aNames[aRootNameIter] = rapidjson::StringRef (RWGltf_GltfRootElementName ((RWGltf_GltfRootElement )aRootNameIter));
  }

  for (ConstMemberIterator aRootIter = MemberBegin();
       aRootIter != MemberEnd(); ++aRootIter)
  {
    for (int aRootNameIter = 0; aRootNameIter < RWGltf_GltfRootElement_NB; ++aRootNameIter)
    {
      if (myGltfRoots[aRootNameIter].IsNull()
            && aNames[aRootNameIter] == aRootIter->name)
      {
        // we will not modify JSON document, thus it is OK to keep the pointers
        myGltfRoots[aRootNameIter].Init (RWGltf_GltfRootElementName ((RWGltf_GltfRootElement )aRootNameIter), &aRootIter->value);
        break;
      }
    }
  }

  for (int aRootNameIter = 0; aRootNameIter < RWGltf_GltfRootElement_NB_MANDATORY; ++aRootNameIter)
  {
    if (myGltfRoots[aRootNameIter].IsNull())
    {
      reportGltfError ("Member '" + RWGltf_GltfRootElementName ((RWGltf_GltfRootElement )aRootNameIter) + "' is not found.");
      return false;
    }
  }
  return true;
}

// =======================================================================
// function : gltfParseAsset
// purpose  :
// =======================================================================
void RWGltf_GltfJsonParser::gltfParseAsset()
{
  const RWGltf_JsonValue* anAsset = myGltfRoots[RWGltf_GltfRootElement_Asset].Root();
  if (anAsset == NULL)
  {
    return;
  }

  if (const RWGltf_JsonValue* aVersion = findObjectMember (*anAsset, "version"))
  {
    if (aVersion->IsString())
    {
      TCollection_AsciiString aVerStr (aVersion->GetString());
      myIsGltf1 = aVerStr.StartsWith ("1.");
    }
  }

  if (myMetadata == NULL)
  {
    return;
  }

  if (const RWGltf_JsonValue* aGenerator = findObjectMember (*anAsset, "generator"))
  {
    if (aGenerator->IsString())
    {
      myMetadata->Add ("generator", aGenerator->GetString());
    }
  }
  if (const RWGltf_JsonValue* aCopyRight = findObjectMember (*anAsset, "copyright"))
  {
    if (aCopyRight->IsString())
    {
      myMetadata->Add ("copyright", aCopyRight->GetString());
    }
  }
}

// =======================================================================
// function : gltfParseMaterials
// purpose  :
// =======================================================================
void RWGltf_GltfJsonParser::gltfParseMaterials()
{
  const RWGltf_JsonValue* aMatList = myGltfRoots[RWGltf_GltfRootElement_Materials].Root();
  if (aMatList == NULL)
  {
    return;
  }
  else if (aMatList->IsObject())
  {
    // glTF 1.0
    for (ConstMemberIterator aMatIter = aMatList->MemberBegin();
         aMatIter != aMatList->MemberEnd(); ++aMatIter)
    {
      Handle(RWGltf_MaterialCommon) aMat;
      const RWGltf_JsonValue& aMatNode = aMatIter->value;
      const RWGltf_JsonValue& aMatId   = aMatIter->name;
      const RWGltf_JsonValue* aNameVal = findObjectMember (aMatNode, "name");
      if (!gltfParseCommonMaterial (aMat, aMatNode))
      {
        if (!gltfParseStdMaterial (aMat, aMatNode))
        {
          continue;
        }
      }

      if (aNameVal != NULL
       && aNameVal->IsString())
      {
        aMat->Name = aNameVal->GetString();
      }
      aMat->Id = aMatId.GetString();
      myMaterialsCommon.Bind (aMat->Id, aMat);
    }
  }
  else if (aMatList->IsArray())
  {
    // glTF 2.0
    int aMatIndex = 0;
    for (rapidjson::Value::ConstValueIterator aMatIter = aMatList->Begin(); aMatIter != aMatList->End(); ++aMatIter, ++aMatIndex)
    {
      Handle(RWGltf_MaterialMetallicRoughness) aMatPbr;
      const RWGltf_JsonValue& aMatNode = *aMatIter;
      const RWGltf_JsonValue* aNameVal = findObjectMember (aMatNode, "name");
      if (gltfParsePbrMaterial (aMatPbr,  aMatNode))
      {
        if (aNameVal != NULL
         && aNameVal->IsString())
        {
          aMatPbr->Name = aNameVal->GetString();
        }
        aMatPbr->Id = TCollection_AsciiString ("mat_") + aMatIndex;
        myMaterialsPbr.Bind (TCollection_AsciiString (aMatIndex), aMatPbr);
      }

      Handle(RWGltf_MaterialCommon) aMatCommon;
      if (gltfParseCommonMaterial(aMatCommon, aMatNode)
       || gltfParseStdMaterial   (aMatCommon, aMatNode))
      {
        if (aNameVal != NULL
         && aNameVal->IsString())
        {
          aMatCommon->Name = aNameVal->GetString();
        }
        aMatCommon->Id = TCollection_AsciiString ("mat_") + aMatIndex;
        myMaterialsCommon.Bind (TCollection_AsciiString (aMatIndex), aMatCommon);
      }
    }
  }
}

// =======================================================================
// function : gltfParseStdMaterial
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParseStdMaterial (Handle(RWGltf_MaterialCommon)& theMat,
                                                  const RWGltf_JsonValue& theMatNode)
{
  //const RWGltf_JsonValue* aTechVal = findObjectMember (theMatNode, "technique");
  const RWGltf_JsonValue* aValues  = findObjectMember (theMatNode, "values");
  if (aValues == NULL)
  {
    return false;
  }

  const RWGltf_JsonValue* anAmbVal = findObjectMember (*aValues, "ambient");
  const RWGltf_JsonValue* aDiffVal = findObjectMember (*aValues, "diffuse");
  const RWGltf_JsonValue* anEmiVal = findObjectMember (*aValues, "emission");
  const RWGltf_JsonValue* aSpecVal = findObjectMember (*aValues, "specular");
  const RWGltf_JsonValue* aShinVal = findObjectMember (*aValues, "shininess");
  if (anAmbVal == NULL
   && aDiffVal == NULL
   && anEmiVal == NULL
   && aSpecVal == NULL
   && aShinVal == NULL)
  {
    return false;
  }

  theMat = new RWGltf_MaterialCommon();

  Graphic3d_Vec4d anAmb, aDiff, anEmi, aSpec;
  if (anAmbVal != NULL
   && anAmbVal->IsString())
  {
    gltfParseTexture (theMat->AmbientTexture, anAmbVal);
  }
  else if (gltfReadVec4   (anAmb, anAmbVal)
        && validateColor4 (anAmb))
  {
    theMat->AmbientColor = Quantity_Color (anAmb.r(), anAmb.g(), anAmb.b(), Quantity_TOC_RGB);
  }

  if (aDiffVal != NULL
   && aDiffVal->IsString())
  {
    gltfParseTexture (theMat->DiffuseTexture, aDiffVal);
  }
  else if (gltfReadVec4   (aDiff, aDiffVal)
        && validateColor4 (aDiff))
  {
    theMat->DiffuseColor = Quantity_Color (aDiff.r(), aDiff.g(), aDiff.b(), Quantity_TOC_RGB);
    theMat->Transparency = float(1.0 - aDiff.a());
  }

  if (gltfReadVec4   (anEmi, anEmiVal)
   && validateColor4 (anEmi))
  {
    theMat->EmissiveColor = Quantity_Color (anEmi.r(), anEmi.g(), anEmi.b(), Quantity_TOC_RGB);
  }

  if (aSpecVal != NULL
   && aSpecVal->IsString())
  {
    gltfParseTexture (theMat->SpecularTexture, aSpecVal);
  }
  if (gltfReadVec4   (aSpec, aSpecVal)
   && validateColor4 (aSpec))
  {
    theMat->SpecularColor = Quantity_Color (aSpec.r(), aSpec.g(), aSpec.b(), Quantity_TOC_RGB);
  }

  if (aShinVal != NULL
   && aShinVal->IsNumber())
  {
    const double aSpecular = aShinVal->GetDouble();
    if (aSpecular >= 0)
    {
      theMat->Shininess = (float )Min (aSpecular / 1000.0, 1.0);
    }
  }
  return true;
}

// =======================================================================
// function : gltfParsePbrMaterial
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParsePbrMaterial (Handle(RWGltf_MaterialMetallicRoughness)& theMat,
                                                  const RWGltf_JsonValue& theMatNode)
{
  /*if (const RWGltf_JsonValue* anExtVal = findObjectMember (theMatNode, "extensions"))
  {
    if (const RWGltf_JsonValue* anExtDefVal = findObjectMember (*anExtVal, "KHR_materials_pbrSpecularGlossiness"))
    {
      const RWGltf_JsonValue* aDiffTexVal = findObjectMember (*anExtDefVal, "diffuseTexture");
      const RWGltf_JsonValue* aSpecTexVal = findObjectMember (*anExtDefVal, "specularGlossinessTexture");
    }
  }*/

  const RWGltf_JsonValue* aMetalRoughVal    = findObjectMember (theMatNode, "pbrMetallicRoughness");
  const RWGltf_JsonValue* aNormTexVal       = findObjectMember (theMatNode, "normalTexture");
  const RWGltf_JsonValue* anEmissFactorVal  = findObjectMember (theMatNode, "emissiveFactor");
  const RWGltf_JsonValue* anEmissTexVal     = findObjectMember (theMatNode, "emissiveTexture");
  const RWGltf_JsonValue* anOcclusionTexVal = findObjectMember (theMatNode, "occlusionTexture");
  if (aMetalRoughVal == NULL)
  {
    return false;
  }

  theMat = new RWGltf_MaterialMetallicRoughness();
  const RWGltf_JsonValue* aBaseColorFactorVal = findObjectMember (*aMetalRoughVal, "baseColorFactor");
  const RWGltf_JsonValue* aBaseColorTexVal    = findObjectMember (*aMetalRoughVal, "baseColorTexture");
  const RWGltf_JsonValue* aMetallicFactorVal  = findObjectMember (*aMetalRoughVal, "metallicFactor");
  const RWGltf_JsonValue* aRoughnessFactorVal = findObjectMember (*aMetalRoughVal, "roughnessFactor");
  const RWGltf_JsonValue* aMetalRoughTexVal   = findObjectMember (*aMetalRoughVal, "metallicRoughnessTexture");

  if (aBaseColorTexVal != NULL
   && aBaseColorTexVal->IsObject())
  {
    if (const RWGltf_JsonValue* aTexIndexVal = findObjectMember (*aBaseColorTexVal, "index"))
    {
      gltfParseTexture (theMat->BaseColorTexture, aTexIndexVal);
    }
  }

  Graphic3d_Vec4d aBaseColorFactor;
  if (gltfReadVec4   (aBaseColorFactor, aBaseColorFactorVal)
   && validateColor4 (aBaseColorFactor))
  {
    theMat->BaseColor = Quantity_ColorRGBA (Graphic3d_Vec4 (aBaseColorFactor));
  }

  Graphic3d_Vec3d anEmissiveFactor;
  if (gltfReadVec3   (anEmissiveFactor, anEmissFactorVal)
   && validateColor3 (anEmissiveFactor))
  {
    theMat->EmissiveFactor = Graphic3d_Vec3 (anEmissiveFactor);
  }

  if (aMetalRoughTexVal != NULL
   && aMetalRoughTexVal->IsObject())
  {
    if (const RWGltf_JsonValue* aTexIndexVal = findObjectMember (*aMetalRoughTexVal, "index"))
    {
      gltfParseTexture (theMat->MetallicRoughnessTexture, aTexIndexVal);
    }
  }

  if (aMetallicFactorVal != NULL
   && aMetallicFactorVal->IsNumber())
  {
    theMat->Metallic = (float )aMetallicFactorVal->GetDouble();
  }

  if (aRoughnessFactorVal != NULL
   && aRoughnessFactorVal->IsNumber())
  {
    theMat->Roughness = (float )aRoughnessFactorVal->GetDouble();
  }

  if (aNormTexVal != NULL
   && aNormTexVal->IsObject())
  {
    if (const RWGltf_JsonValue* aTexIndexVal = findObjectMember (*aNormTexVal, "index"))
    {
      gltfParseTexture (theMat->NormalTexture, aTexIndexVal);
    }
  }

  if (anEmissTexVal != NULL
   && anEmissTexVal->IsObject())
  {
    if (const RWGltf_JsonValue* aTexIndexVal = findObjectMember (*anEmissTexVal, "index"))
    {
      gltfParseTexture (theMat->EmissiveTexture, aTexIndexVal);
    }
  }

  if (anOcclusionTexVal != NULL
   && anOcclusionTexVal->IsObject())
  {
    if (const RWGltf_JsonValue* aTexIndexVal = findObjectMember (*anOcclusionTexVal, "index"))
    {
      gltfParseTexture (theMat->OcclusionTexture, aTexIndexVal);
    }
  }
  return true;
}

// =======================================================================
// function : gltfParseCommonMaterial
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParseCommonMaterial (Handle(RWGltf_MaterialCommon)& theMat,
                                                     const RWGltf_JsonValue& theMatNode)
{
  const RWGltf_JsonValue* anExtVal = findObjectMember (theMatNode, "extensions");
  if (anExtVal == NULL)
  {
    return false;
  }

  const RWGltf_JsonValue* aMatCommon = findObjectMember (*anExtVal, THE_KHR_materials_common);
  if (aMatCommon == NULL)
  {
    return false;
  }

  if (!gltfParseStdMaterial (theMat, *aMatCommon))
  {
    return false;
  }
  return true;
}

// =======================================================================
// function : gltfParseTexture
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParseTexture (Handle(Image_Texture)& theTexture,
                                              const RWGltf_JsonValue* theTextureId)
{
  if (theTextureId == NULL
  ||  myGltfRoots[RWGltf_GltfRootElement_Textures].IsNull()
  ||  myGltfRoots[RWGltf_GltfRootElement_Images].IsNull())
  {
    return false;
  }

  const TCollection_AsciiString aTextureId = getKeyString (*theTextureId);
  const RWGltf_JsonValue* aTexNode = myGltfRoots[RWGltf_GltfRootElement_Textures].FindChild (*theTextureId);
  if (aTexNode == NULL)
  {
    reportGltfWarning ("Texture node '" + aTextureId + "' is not found.");
    return false;
  }

  const RWGltf_JsonValue* aSrcVal  = findObjectMember (*aTexNode, "source");
  const RWGltf_JsonValue* aTargVal = findObjectMember (*aTexNode, "target");
  if (aSrcVal == NULL)
  {
    reportGltfWarning ("Invalid texture node '" + aTextureId + "' without a 'source' property.");
    return false;
  }
  if (aTargVal != NULL
   && aTargVal->IsNumber()
   && aTargVal->GetInt() != 3553) // GL_TEXTURE_2D
  {
    return false;
  }

  const RWGltf_JsonValue* anImgNode = myGltfRoots[RWGltf_GltfRootElement_Images].FindChild (*aSrcVal);
  if (anImgNode == NULL)
  {
    reportGltfWarning ("Invalid texture node '" + aTextureId + "' points to non-existing image '" + getKeyString (*aSrcVal) + "'.");
    return false;
  }

  if (myIsBinary)
  {
    const RWGltf_JsonValue* aBinVal = NULL;
    const RWGltf_JsonValue* aBufferViewName = findObjectMember (*anImgNode, "bufferView");
    if (aBufferViewName != NULL)
    {
      aBinVal = anImgNode;
    }
    else if (myIsGltf1)
    {
      const RWGltf_JsonValue* anExtVal = findObjectMember (*anImgNode, "extensions");
      if (anExtVal != NULL)
      {
        aBinVal = findObjectMember (*anExtVal, THE_KHR_binary_glTF);
        if (aBinVal != NULL)
        {
          aBufferViewName = findObjectMember (*aBinVal, "bufferView");
        }
      }
    }

    if (aBinVal != NULL)
    {
      //const RWGltf_JsonValue* aMimeTypeVal = findObjectMember (*aBinVal, "mimeType");
      //const RWGltf_JsonValue* aWidthVal    = findObjectMember (*aBinVal, "width");
      //const RWGltf_JsonValue* aHeightVal   = findObjectMember (*aBinVal, "height");
      if (aBufferViewName == NULL)
      {
        reportGltfWarning ("Invalid texture node '" + aTextureId + "' points to invalid data source.");
        return false;
      }

      const RWGltf_JsonValue* aBufferView = myGltfRoots[RWGltf_GltfRootElement_BufferViews].FindChild (*aBufferViewName);
      if (aBufferView == NULL
      || !aBufferView->IsObject())
      {
        reportGltfWarning ("Invalid texture node '" + aTextureId + "' points to invalid buffer view '" + getKeyString (*aBufferViewName) + "'.");
        return false;
      }

      const RWGltf_JsonValue* aBufferName = findObjectMember (*aBufferView, "buffer");
      const RWGltf_JsonValue* aByteLength = findObjectMember (*aBufferView, "byteLength");
      const RWGltf_JsonValue* aByteOffset = findObjectMember (*aBufferView, "byteOffset");
      if (aBufferName != NULL
      &&  aBufferName->IsString()
      && !IsEqual (aBufferName->GetString(), "binary_glTF"))
      {
        reportGltfError ("BufferView '" + getKeyString (*aBufferViewName) + "' does not define binary_glTF buffer.");
        return false;
      }

      RWGltf_GltfBufferView aBuffView;
      aBuffView.ByteOffset = aByteOffset != NULL && aByteOffset->IsNumber()
                           ? (int64_t )aByteOffset->GetDouble()
                           : 0;
      aBuffView.ByteLength = aByteLength != NULL && aByteLength->IsNumber()
                           ? (int64_t )aByteLength->GetDouble()
                           : 0;
      if (aBuffView.ByteLength < 0)
      {
        reportGltfError ("BufferView '" + getKeyString (*aBufferViewName) + "' defines invalid byteLength.");
        return false;
      }
      else if (aBuffView.ByteOffset < 0)
      {
        reportGltfError ("BufferView '" + getKeyString (*aBufferViewName) + "' defines invalid byteOffset.");
        return false;
      }


      const int64_t anOffset = myBinBodyOffset + aBuffView.ByteOffset;
      theTexture = new Image_Texture (myFilePath, anOffset, aBuffView.ByteLength);
      return true;
    }
  }

  const RWGltf_JsonValue* anUriVal = findObjectMember (*anImgNode, "uri");
  if (anUriVal == NULL
  || !anUriVal->IsString())
  {
    return false;
  }

  const char* anUriData = anUriVal->GetString();
  if (::strncmp (anUriData, "data:", 5) == 0) // data:image/png;base64
  {
    // uncompressing base64 here is inefficient, because the same image can be shared by several nodes
    const char* aDataStart = anUriData + 5;
    for (const char* aDataIter = aDataStart; *aDataIter != '\0'; ++aDataIter)
    {
      if (::memcmp (aDataIter, ";base64,", 8) == 0)
      {
        const char* aBase64End  = anUriData + anUriVal->GetStringLength();
        const char* aBase64Data = aDataIter + 8;
        const size_t aBase64Len = size_t(aBase64End - aBase64Data);
        //const TCollection_AsciiString aMime (aDataStart, aDataIter - aDataStart);
        Handle(NCollection_Buffer) aData = FSD_Base64Decoder::Decode ((const Standard_Byte* )aBase64Data, aBase64Len);
        theTexture = new Image_Texture (aData, myFilePath + "@" + getKeyString (*aSrcVal));
        return true;
      }
    }
    Message::DefaultMessenger()->Send ("glTF reader - embedded image has been skipped", Message_Warning);
    return false;
  }

  TCollection_AsciiString anImageFile = myFolder + anUriVal->GetString();
  theTexture = new Image_Texture (anImageFile);
  if (myExternalFiles != NULL)
  {
    myExternalFiles->Add (anImageFile);
  }
  return true;
}

// =======================================================================
// function : gltfParseScene
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParseScene (const Handle(Message_ProgressIndicator)& theProgress)
{
  // search default scene
  const RWGltf_JsonValue* aDefScene = myGltfRoots[RWGltf_GltfRootElement_Scenes].FindChild (*myGltfRoots[RWGltf_GltfRootElement_Scene].Root());
  if (aDefScene == NULL)
  {
    reportGltfError ("Default scene is not found.");
    return false;
  }

  const RWGltf_JsonValue* aSceneNodes = findObjectMember (*aDefScene, "nodes");
  if (aSceneNodes == NULL
  || !aSceneNodes->IsArray())
  {
    reportGltfError ("Empty scene '" + getKeyString (*myGltfRoots[RWGltf_GltfRootElement_Scene].Root()) + "'.");
    return false;
  }

  return gltfParseSceneNodes (*myRootShapes, *aSceneNodes, theProgress);
}

// =======================================================================
// function : gltfParseSceneNodes
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParseSceneNodes (TopTools_SequenceOfShape& theShapeSeq,
                                                 const RWGltf_JsonValue& theSceneNodes,
                                                 const Handle(Message_ProgressIndicator)& theProgress)
{
  if (!theSceneNodes.IsArray())
  {
    reportGltfError ("Scene nodes is not array.");
    return false;
  }

  Message_ProgressSentry aPSentry (theProgress, "Reading scene nodes", 0, theSceneNodes.Size(), 1);
  for (rapidjson::Value::ConstValueIterator aSceneNodeIter = theSceneNodes.Begin();
       aSceneNodeIter != theSceneNodes.End() && aPSentry.More(); ++aSceneNodeIter, aPSentry.Next())
  {
    const RWGltf_JsonValue* aSceneNode = myGltfRoots[RWGltf_GltfRootElement_Nodes].FindChild (*aSceneNodeIter);
    if (aSceneNode == NULL)
    {
      reportGltfWarning ("Scene refers to non-existing node '" + getKeyString (*aSceneNodeIter) + "'.");
      return true;
    }

    TopoDS_Shape aNodeShape;
    if (!gltfParseSceneNode (aNodeShape, getKeyString (*aSceneNodeIter), *aSceneNode, theProgress))
    {
      return false;
    }

    if (aNodeShape.IsNull())
    {
      continue;
    }
    else if (myToSkipEmptyNodes
         && !TopExp_Explorer (aNodeShape, TopAbs_FACE).More())
    {
      continue;
    }

    theShapeSeq.Append (aNodeShape);
  }
  return true;
}

// =======================================================================
// function : gltfParseSceneNode
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParseSceneNode (TopoDS_Shape& theNodeShape,
                                                const TCollection_AsciiString& theSceneNodeId,
                                                const RWGltf_JsonValue& theSceneNode,
                                                const Handle(Message_ProgressIndicator)& theProgress)
{
  const RWGltf_JsonValue* aName         = findObjectMember (theSceneNode, "name");
  //const RWGltf_JsonValue* aJointName    = findObjectMember (theSceneNode, "jointName");
  const RWGltf_JsonValue* aChildren     = findObjectMember (theSceneNode, "children");
  const RWGltf_JsonValue* aMeshes_1     = findObjectMember (theSceneNode, "meshes");
  const RWGltf_JsonValue* aMesh_2       = findObjectMember (theSceneNode, "mesh");
  //const RWGltf_JsonValue* aCamera       = findObjectMember (theSceneNode, "camera");
  const RWGltf_JsonValue* aTrsfMatVal   = findObjectMember (theSceneNode, "matrix");
  const RWGltf_JsonValue* aTrsfRotVal   = findObjectMember (theSceneNode, "rotation");
  const RWGltf_JsonValue* aTrsfScaleVal = findObjectMember (theSceneNode, "scale");
  const RWGltf_JsonValue* aTrsfTransVal = findObjectMember (theSceneNode, "translation");
  if (findNodeShape (theNodeShape, theSceneNodeId))
  {
    return true;
  }

  TopLoc_Location aNodeLoc;
  const bool hasTrs = aTrsfRotVal   != NULL
                   || aTrsfScaleVal != NULL
                   || aTrsfTransVal != NULL;
  if (aTrsfMatVal != NULL)
  {
    if (hasTrs)
    {
      reportGltfError ("Scene node '" + theSceneNodeId + "' defines ambiguous transformation.");
      return false;
    }
    else if (!aTrsfMatVal->IsArray()
           || aTrsfMatVal->Size() != 16)
    {
      reportGltfError ("Scene node '" + theSceneNodeId + "' defines invalid transformation matrix array.");
      return false;
    }

    Graphic3d_Mat4d aMat4;
    for (int aColIter = 0; aColIter < 4; ++aColIter)
    {
      for (int aRowIter = 0; aRowIter < 4; ++aRowIter)
      {
        const RWGltf_JsonValue& aGenVal = (*aTrsfMatVal)[aColIter * 4 + aRowIter];
        if (!aGenVal.IsNumber())
        {
          reportGltfError ("Scene node '" + theSceneNodeId + "' defines invalid transformation matrix.");
          return false;
        }
        aMat4.SetValue (aRowIter, aColIter, aGenVal.GetDouble());
      }
    }

    if (!aMat4.IsIdentity())
    {
      gp_Trsf aTrsf;
      aTrsf.SetValues (aMat4.GetValue (0, 0), aMat4.GetValue (0, 1), aMat4.GetValue (0, 2), aMat4.GetValue (0, 3),
                       aMat4.GetValue (1, 0), aMat4.GetValue (1, 1), aMat4.GetValue (1, 2), aMat4.GetValue (1, 3),
                       aMat4.GetValue (2, 0), aMat4.GetValue (2, 1), aMat4.GetValue (2, 2), aMat4.GetValue (2, 3));
      myCSTrsf.TransformTransformation (aTrsf);
      if (aTrsf.Form() != gp_Identity)
      {
        aNodeLoc = TopLoc_Location (aTrsf);
      }
    }
  }
  else if (hasTrs)
  {
    gp_Trsf aTrsf;
    if (aTrsfRotVal != NULL)
    {
      if (!aTrsfRotVal->IsArray()
        || aTrsfRotVal->Size() != 4)
      {
        reportGltfError ("Scene node '" + theSceneNodeId + "' defines invalid rotation quaternion.");
        return false;
      }

      Graphic3d_Vec4d aRotVec4;
      for (int aCompIter = 0; aCompIter < 4; ++aCompIter)
      {
        const RWGltf_JsonValue& aGenVal = (*aTrsfRotVal)[aCompIter];
        if (!aGenVal.IsNumber())
        {
          reportGltfError ("Scene node '" + theSceneNodeId + "' defines invalid rotation.");
          return false;
        }
        aRotVec4[aCompIter] = aGenVal.GetDouble();
      }
      const gp_Quaternion aQuaternion (aRotVec4.x(), aRotVec4.y(), aRotVec4.z(), aRotVec4.w());
      if (Abs (aQuaternion.X())       > gp::Resolution()
       || Abs (aQuaternion.Y())       > gp::Resolution()
       || Abs (aQuaternion.Z())       > gp::Resolution()
       || Abs (aQuaternion.W() - 1.0) > gp::Resolution())
      {
        aTrsf.SetRotation (aQuaternion);
      }
    }

    if (aTrsfTransVal != NULL)
    {
      if (!aTrsfTransVal->IsArray()
        || aTrsfTransVal->Size() != 3)
      {
        reportGltfError ("Scene node '" + theSceneNodeId + "' defines invalid translation vector.");
        return false;
      }

      gp_XYZ aTransVec;
      for (int aCompIter = 0; aCompIter < 3; ++aCompIter)
      {
        const RWGltf_JsonValue& aGenVal = (*aTrsfTransVal)[aCompIter];
        if (!aGenVal.IsNumber())
        {
          reportGltfError ("Scene node '" + theSceneNodeId + "' defines invalid translation.");
          return false;
        }
        aTransVec.SetCoord (aCompIter + 1, aGenVal.GetDouble());
      }
      aTrsf.SetTranslationPart (aTransVec);
    }

    if (aTrsfScaleVal != NULL)
    {
      Graphic3d_Vec3d aScaleVec;
      if (!aTrsfScaleVal->IsArray()
        || aTrsfScaleVal->Size() != 3)
      {
        reportGltfError ("Scene node '" + theSceneNodeId + "' defines invalid scale vector.");
        return false;
      }
      for (int aCompIter = 0; aCompIter < 3; ++aCompIter)
      {
        const RWGltf_JsonValue& aGenVal = (*aTrsfScaleVal)[aCompIter];
        if (!aGenVal.IsNumber())
        {
          reportGltfError ("Scene node '" + theSceneNodeId + "' defines invalid scale.");
          return false;
        }
        aScaleVec[aCompIter] = aGenVal.GetDouble();
        if (Abs (aScaleVec[aCompIter]) <= gp::Resolution())
        {
          reportGltfError ("Scene node '" + theSceneNodeId + "' defines invalid scale.");
          return false;
        }
      }

      if (Abs (aScaleVec.x() - aScaleVec.y()) > Precision::Confusion()
       || Abs (aScaleVec.y() - aScaleVec.z()) > Precision::Confusion()
       || Abs (aScaleVec.x() - aScaleVec.z()) > Precision::Confusion())
      {
        Graphic3d_Mat4d aScaleMat;
        aScaleMat.SetDiagonal (aScaleVec);

        Graphic3d_Mat4d aMat4;
        aTrsf.GetMat4 (aMat4);

        aMat4 = aMat4 * aScaleMat;
        aTrsf = gp_Trsf();
        aTrsf.SetValues (aMat4.GetValue (0, 0), aMat4.GetValue (0, 1), aMat4.GetValue (0, 2), aMat4.GetValue (0, 3),
                         aMat4.GetValue (1, 0), aMat4.GetValue (1, 1), aMat4.GetValue (1, 2), aMat4.GetValue (1, 3),
                         aMat4.GetValue (2, 0), aMat4.GetValue (2, 1), aMat4.GetValue (2, 2), aMat4.GetValue (2, 3));

        Message::DefaultMessenger()->Send (TCollection_AsciiString ("glTF reader, scene node '")
                                         + theSceneNodeId + "' defines unsupported scaling "
                                         + aScaleVec.x() + " " + aScaleVec.y() + " " + aScaleVec.z(), Message_Warning);
      }
      else if (Abs (aScaleVec.x() - 1.0) > Precision::Confusion())
      {
        aTrsf.SetScaleFactor (aScaleVec.x());
      }
    }

    myCSTrsf.TransformTransformation (aTrsf);
    if (aTrsf.Form() != gp_Identity)
    {
      aNodeLoc = TopLoc_Location (aTrsf);
    }
  }

  BRep_Builder aBuilder;
  TopoDS_Compound aNodeShape;
  aBuilder.MakeCompound (aNodeShape);
  TopTools_SequenceOfShape aChildShapes;
  int aNbSubShapes = 0;
  if (aChildren != NULL
  && !gltfParseSceneNodes (aChildShapes, *aChildren, theProgress))
  {
    theNodeShape = aNodeShape;
    bindNodeShape (theNodeShape, aNodeLoc, theSceneNodeId, aName);
    return false;
  }
  for (TopTools_SequenceOfShape::Iterator aChildShapeIter (aChildShapes); aChildShapeIter.More(); aChildShapeIter.Next())
  {
    aBuilder.Add (aNodeShape, aChildShapeIter.Value());
    ++aNbSubShapes;
  }

  if (aMeshes_1 != NULL
   && aMeshes_1->IsArray())
  {
    // glTF 1.0
    Message_ProgressSentry aPSentry (theProgress, "Reading scene meshes", 0, aMeshes_1->Size(), 1);
    for (rapidjson::Value::ConstValueIterator aMeshIter = aMeshes_1->Begin();
         aMeshIter != aMeshes_1->End() && aPSentry.More(); ++aMeshIter, aPSentry.Next())
    {
      const RWGltf_JsonValue* aMesh = myGltfRoots[RWGltf_GltfRootElement_Meshes].FindChild (*aMeshIter);
      if (aMesh == NULL)
      {
        theNodeShape = aNodeShape;
        bindNodeShape (theNodeShape, aNodeLoc, theSceneNodeId, aName);
        reportGltfError ("Scene node '" + theSceneNodeId + "' refers to non-existing mesh.");
        return false;
      }

      TopoDS_Shape aMeshShape;
      if (!gltfParseMesh (aMeshShape, getKeyString (*aMeshIter), *aMesh, theProgress))
      {
        theNodeShape = aNodeShape;
        bindNodeShape (theNodeShape, aNodeLoc, theSceneNodeId, aName);
        return false;
      }
      if (!aMeshShape.IsNull())
      {
        aBuilder.Add (aNodeShape, aMeshShape);
        ++aNbSubShapes;
      }
    }
  }
  if (aMesh_2 != NULL)
  {
    // glTF 2.0
    const RWGltf_JsonValue* aMesh = myGltfRoots[RWGltf_GltfRootElement_Meshes].FindChild (*aMesh_2);
    if (aMesh == NULL)
    {
      theNodeShape = aNodeShape;
      bindNodeShape (theNodeShape, aNodeLoc, theSceneNodeId, aName);
      reportGltfError ("Scene node '" + theSceneNodeId + "' refers to non-existing mesh.");
      return false;
    }

    TopoDS_Shape aMeshShape;
    if (!gltfParseMesh (aMeshShape, getKeyString (*aMesh_2), *aMesh, theProgress))
    {
      theNodeShape = aNodeShape;
      bindNodeShape (theNodeShape, aNodeLoc, theSceneNodeId, aName);
      return false;
    }
    if (!aMeshShape.IsNull())
    {
      aBuilder.Add (aNodeShape, aMeshShape);
      ++aNbSubShapes;
    }
  }

  if (aNbSubShapes == 1)
  {
    theNodeShape = TopoDS_Iterator (aNodeShape).Value();
  }
  else
  {
    theNodeShape = aNodeShape;
  }
  bindNodeShape (theNodeShape, aNodeLoc, theSceneNodeId, aName);
  return true;
}

// =======================================================================
// function : gltfParseMesh
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParseMesh (TopoDS_Shape& theMeshShape,
                                           const TCollection_AsciiString& theMeshId,
                                           const RWGltf_JsonValue& theMesh,
                                           const Handle(Message_ProgressIndicator)& theProgress)
{
  const RWGltf_JsonValue* aName  = findObjectMember (theMesh, "name");
  const RWGltf_JsonValue* aPrims = findObjectMember (theMesh, "primitives");
  if (!aPrims->IsArray())
  {
    reportGltfError ("Primitive array attributes within Mesh '" + theMeshId + "' is not an array.");
    return false;
  }

  if (findMeshShape (theMeshShape, theMeshId))
  {
    return true;
  }

  BRep_Builder aBuilder;
  TopoDS_Compound aMeshShape;
  int aNbFaces = 0;
  for (rapidjson::Value::ConstValueIterator aPrimArrIter = aPrims->Begin();
       aPrimArrIter != aPrims->End(); ++aPrimArrIter)
  {
    TCollection_AsciiString aUserName;
    if (aName != NULL
     && aName->IsString())
    {
      aUserName = aName->GetString();
    }

    Handle(RWGltf_GltfLatePrimitiveArray) aMeshData = new RWGltf_GltfLatePrimitiveArray (theMeshId, aUserName);
    if (!gltfParsePrimArray (aMeshData, theMeshId, *aPrimArrIter, theProgress))
    {
      return false;
    }

    if (!aMeshData->Data().IsEmpty())
    {
      if (aMeshShape.IsNull())
      {
        aBuilder.MakeCompound (aMeshShape);
      }

      TopoDS_Face aFace;
      aBuilder.MakeFace (aFace, aMeshData);
      aBuilder.Add (aMeshShape, aFace);
      if (myAttribMap != NULL
       && aMeshData->HasStyle())
      {
        RWMesh_NodeAttributes aShapeAttribs;
        aShapeAttribs.RawName = aUserName;
        aShapeAttribs.Style.SetColorSurf (aMeshData->BaseColor());
        myAttribMap->Bind (aFace, aShapeAttribs);
      }
      myFaceList.Append (aFace);
      ++aNbFaces;
    }
  }

  if (aNbFaces == 1)
  {
    theMeshShape = TopoDS_Iterator (aMeshShape).Value();
  }
  else
  {
    theMeshShape = aMeshShape;
  }
  bindMeshShape (theMeshShape, theMeshId, aName);
  return true;
}

// =======================================================================
// function : gltfParsePrimArray
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParsePrimArray (const Handle(RWGltf_GltfLatePrimitiveArray)& theMeshData,
                                                const TCollection_AsciiString& theMeshId,
                                                const RWGltf_JsonValue& thePrimArray,
                                                const Handle(Message_ProgressIndicator)& /*theProgress*/)
{
  const RWGltf_JsonValue* anAttribs = findObjectMember (thePrimArray, "attributes");
  const RWGltf_JsonValue* anIndices = findObjectMember (thePrimArray, "indices");
  const RWGltf_JsonValue* aMaterial = findObjectMember (thePrimArray, "material");
  const RWGltf_JsonValue* aModeVal  = findObjectMember (thePrimArray, "mode");
  RWGltf_GltfPrimitiveMode aMode = RWGltf_GltfPrimitiveMode_Triangles;
  if (anAttribs == NULL
  || !anAttribs->IsObject())
  {
    reportGltfError ("Primitive array within Mesh '" + theMeshId + "' defines no attributes.");
    return false;
  }
  else if (aModeVal != NULL)
  {
    aMode = RWGltf_GltfPrimitiveMode_UNKNOWN;
    if (aModeVal->IsInt())
    {
      aMode = (RWGltf_GltfPrimitiveMode )aModeVal->GetInt();
    }
    if (aMode < RWGltf_GltfPrimitiveMode_Points
     || aMode > RWGltf_GltfPrimitiveMode_TriangleFan)
    {
      reportGltfError ("Primitive array within Mesh '" + theMeshId + "' has unknown mode.");
      return false;
    }
  }
  if (aMode != RWGltf_GltfPrimitiveMode_Triangles)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString() + "Primitive array within Mesh '"
                                                   + theMeshId + "' skipped due to unsupported mode.", Message_Warning);
    return true;
  }
  theMeshData->SetPrimitiveMode (aMode);

  // assign material
  if (aMaterial != NULL)
  {
    Handle(RWGltf_MaterialMetallicRoughness) aMatPbr;
    if (myMaterialsPbr.Find (getKeyString (*aMaterial), aMatPbr))
    {
      theMeshData->SetMaterialPbr (aMatPbr);
    }

    Handle(RWGltf_MaterialCommon) aMatCommon;
    if (myMaterialsCommon.Find (getKeyString (*aMaterial), aMatCommon))
    {
      theMeshData->SetMaterialCommon (aMatCommon);
    }
  }

  bool hasPositions = false;
  for (rapidjson::Value::ConstMemberIterator anAttribIter = anAttribs->MemberBegin();
       anAttribIter != anAttribs->MemberEnd(); ++anAttribIter)
  {
    const TCollection_AsciiString anAttribId = getKeyString (anAttribIter->value);
    if (anAttribId.IsEmpty())
    {
      reportGltfError ("Primitive array attribute accessor key within Mesh '" + theMeshId + "' is not a string.");
      return false;
    }

    RWGltf_GltfArrayType aType = RWGltf_GltfParseAttribType (anAttribIter->name.GetString());
    if (aType == RWGltf_GltfArrayType_UNKNOWN)
    {
      // just ignore unknown attributes
      continue;
    }

    const RWGltf_JsonValue* anAccessor = myGltfRoots[RWGltf_GltfRootElement_Accessors].FindChild (anAttribIter->value);
    if (anAccessor == NULL
    || !anAccessor->IsObject())
    {
      reportGltfError ("Primitive array attribute accessor key '" + anAttribId + "' points to non-existing object.");
      return false;
    }
    else if (!gltfParseAccessor (theMeshData, anAttribId, *anAccessor, aType))
    {
      return false;
    }
    else if (aType == RWGltf_GltfArrayType_Position)
    {
      hasPositions = true;
    }
  }
  if (!hasPositions)
  {
    reportGltfError ("Primitive array within Mesh '" + theMeshId + "' does not define vertex positions.");
    return false;
  }

  if (anIndices != NULL)
  {
    const TCollection_AsciiString anIndicesId = getKeyString (*anIndices);
    const RWGltf_JsonValue* anAccessor = myGltfRoots[RWGltf_GltfRootElement_Accessors].FindChild (*anIndices);
    if (anAccessor == NULL
    || !anAccessor->IsObject())
    {
      reportGltfError ("Primitive array indices accessor key '" + anIndicesId + "' points to non-existing object.");
      return false;
    }
    else if (!gltfParseAccessor (theMeshData, anIndicesId, *anAccessor, RWGltf_GltfArrayType_Indices))
    {
      return false;
    }
  }

  return true;
}

// =======================================================================
// function : gltfParseAccessor
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParseAccessor (const Handle(RWGltf_GltfLatePrimitiveArray)& theMeshData,
                                               const TCollection_AsciiString& theName,
                                               const RWGltf_JsonValue& theAccessor,
                                               const RWGltf_GltfArrayType theType)
{
  RWGltf_GltfAccessor aStruct;
  const RWGltf_JsonValue* aTypeStr        = findObjectMember (theAccessor, "type");
  const RWGltf_JsonValue* aBufferViewName = findObjectMember (theAccessor, "bufferView");
  const RWGltf_JsonValue* aByteOffset     = findObjectMember (theAccessor, "byteOffset");
  const RWGltf_JsonValue* aByteStride     = findObjectMember (theAccessor, "byteStride");
  const RWGltf_JsonValue* aCompType       = findObjectMember (theAccessor, "componentType");
  const RWGltf_JsonValue* aCount          = findObjectMember (theAccessor, "count");
  if (aTypeStr == NULL
  || !aTypeStr->IsString())
  {
    reportGltfError ("Accessor '" + theName + "' does not define type.");
    return false;
  }
  aStruct.Type = RWGltf_GltfParseAccessorType (aTypeStr->GetString());
  if (aStruct.Type == RWGltf_GltfAccessorLayout_UNKNOWN)
  {
    reportGltfError ("Accessor '" + theName + "' has invalid type.");
    return false;
  }

  if (aBufferViewName == NULL)
  {
    reportGltfError ("Accessor '" + theName + "' does not define bufferView.");
    return false;
  }
  if (aCompType == NULL
  || !aCompType->IsInt())
  {
    reportGltfError ("Accessor '" + theName + "' does not define componentType.");
    return false;
  }
  aStruct.ComponentType = (RWGltf_GltfAccessorCompType )aCompType->GetInt();
  if (aStruct.ComponentType != RWGltf_GltfAccessorCompType_Int8
   && aStruct.ComponentType != RWGltf_GltfAccessorCompType_UInt8
   && aStruct.ComponentType != RWGltf_GltfAccessorCompType_Int16
   && aStruct.ComponentType != RWGltf_GltfAccessorCompType_UInt16
   && aStruct.ComponentType != RWGltf_GltfAccessorCompType_UInt32
   && aStruct.ComponentType != RWGltf_GltfAccessorCompType_Float32)
  {
    reportGltfError ("Accessor '" + theName + "' defines invalid componentType value.");
    return false;
  }

  if (aCount == NULL
  || !aCount->IsNumber())
  {
    reportGltfError ("Accessor '" + theName + "' does not define count.");
    return false;
  }

  aStruct.ByteOffset = aByteOffset != NULL && aByteOffset->IsNumber()
                     ? (int64_t )aByteOffset->GetDouble()
                     : 0;
  aStruct.ByteStride = aByteStride != NULL && aByteStride->IsInt()
                     ? aByteStride->GetInt()
                     : 0;
  aStruct.Count = (int64_t )aCount->GetDouble();

  if (aStruct.ByteOffset < 0)
  {
    reportGltfError ("Accessor '" + theName + "' defines invalid byteOffset.");
    return false;
  }
  else if (aStruct.ByteStride < 0
        || aStruct.ByteStride > 255)
  {
    reportGltfError ("Accessor '" + theName + "' defines invalid byteStride.");
    return false;
  }
  else if (aStruct.Count < 1)
  {
    reportGltfError ("Accessor '" + theName + "' defines invalid count.");
    return false;
  }

  // Read Min/Max values for POSITION type. It is used for bounding boxes
  if (theType == RWGltf_GltfArrayType_Position)
  {
    const RWGltf_JsonValue* aMin = findObjectMember (theAccessor, "min");
    const RWGltf_JsonValue* aMax = findObjectMember (theAccessor, "max");
    if (aMin != NULL && aMax != NULL)
    {
      // Note: Min/Max values can be not defined in glTF file.
      // In this case it is not used only.
      if (!aMin->IsArray()   || !aMax->IsArray() ||
           aMin->Size() != 3 ||  aMax->Size() != 3)
      {
        reportGltfWarning ("Accessor '" + theName + "' defines invalid min/max values.");
      }
      else
      {
        bool isValidMinMax = true;
        gp_Pnt aMinPnt, aMaxPnt;
        for (int anIter = 0; anIter < 3; ++anIter)
        {
          const RWGltf_JsonValue& aMinVal = (*aMin)[anIter];
          const RWGltf_JsonValue& aMaxVal = (*aMax)[anIter];
          if (!aMinVal.IsNumber() || !aMaxVal.IsNumber())
          {
            reportGltfWarning ("Accessor '" + theName + "' defines invalid min/max value.");
            isValidMinMax = false;
            break;
          }
          aMinPnt.SetCoord (anIter + 1, aMinVal.GetDouble());
          aMinPnt.SetCoord (anIter + 1, aMaxVal.GetDouble());
        }
        if (isValidMinMax)
        {
          myCSTrsf.TransformPosition (aMinPnt.ChangeCoord());
          myCSTrsf.TransformPosition (aMaxPnt.ChangeCoord());

          Bnd_Box aBox;
          aBox.Add (aMinPnt);
          aBox.Add (aMaxPnt);

          theMeshData->SetBoundingBox (aBox);
        }
      }
    }
  }

  const RWGltf_JsonValue* aBufferView = myGltfRoots[RWGltf_GltfRootElement_BufferViews].FindChild (*aBufferViewName);
  if (aBufferView == NULL
  || !aBufferView->IsObject())
  {
    reportGltfError ("Accessor '" + theName + "' refers to non-existing bufferView.");
    return false;
  }

  return gltfParseBufferView (theMeshData, getKeyString (*aBufferViewName), *aBufferView, aStruct, theType);
}

// =======================================================================
// function : gltfParseBufferView
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParseBufferView (const Handle(RWGltf_GltfLatePrimitiveArray)& theMeshData,
                                                 const TCollection_AsciiString& theName,
                                                 const RWGltf_JsonValue& theBufferView,
                                                 const RWGltf_GltfAccessor& theAccessor,
                                                 const RWGltf_GltfArrayType theType)
{
  RWGltf_GltfBufferView aBuffView;
  const RWGltf_JsonValue* aBufferName = findObjectMember (theBufferView, "buffer");
  const RWGltf_JsonValue* aByteLength = findObjectMember (theBufferView, "byteLength");
  const RWGltf_JsonValue* aByteOffset = findObjectMember (theBufferView, "byteOffset");
  const RWGltf_JsonValue* aTarget     = findObjectMember (theBufferView, "target");
  if (aBufferName == NULL)
  {
    reportGltfError ("BufferView '" + theName + "' does not define buffer.");
    return false;
  }

  aBuffView.ByteOffset = aByteOffset != NULL && aByteOffset->IsNumber()
                       ? (int64_t )aByteOffset->GetDouble()
                       : 0;
  aBuffView.ByteLength = aByteLength != NULL && aByteLength->IsNumber()
                       ? (int64_t )aByteLength->GetDouble()
                       : 0;
  if (aTarget != NULL && aTarget->IsInt())
  {
    aBuffView.Target = (RWGltf_GltfBufferViewTarget )aTarget->GetInt();
    if (aBuffView.Target != RWGltf_GltfBufferViewTarget_ARRAY_BUFFER
     && aBuffView.Target != RWGltf_GltfBufferViewTarget_ELEMENT_ARRAY_BUFFER)
    {
      reportGltfError ("BufferView '" + theName + "' defines invalid target.");
      return false;
    }
  }

  if (aBuffView.ByteLength < 0)
  {
    reportGltfError ("BufferView '" + theName + "' defines invalid byteLength.");
    return false;
  }
  else if (aBuffView.ByteOffset < 0)
  {
    reportGltfError ("BufferView '" + theName + "' defines invalid byteOffset.");
    return false;
  }

  const RWGltf_JsonValue* aBuffer = myGltfRoots[RWGltf_GltfRootElement_Buffers].FindChild (*aBufferName);
  if (aBuffer == NULL
  || !aBuffer->IsObject())
  {
    reportGltfError ("BufferView '" + theName + "' refers to non-existing buffer.");
    return false;
  }

  return gltfParseBuffer (theMeshData, getKeyString (*aBufferName), *aBuffer, theAccessor, aBuffView, theType);
}

// =======================================================================
// function : gltfParseBuffer
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::gltfParseBuffer (const Handle(RWGltf_GltfLatePrimitiveArray)& theMeshData,
                                             const TCollection_AsciiString& theName,
                                             const RWGltf_JsonValue& theBuffer,
                                             const RWGltf_GltfAccessor&     theAccessor,
                                             const RWGltf_GltfBufferView&   theView,
                                             const RWGltf_GltfArrayType     theType)
{
  //const RWGltf_JsonValue* aType       = findObjectMember (theBuffer, "type");
  //const RWGltf_JsonValue* aByteLength = findObjectMember (theBuffer, "byteLength");
  const RWGltf_JsonValue* anUriVal      = findObjectMember (theBuffer, "uri");

  int64_t anOffset = theView.ByteOffset + theAccessor.ByteOffset;
  bool isBinary = false;
  if (myIsBinary)
  {
    isBinary = IsEqual ("binary_glTF", theName) // glTF 1.0
            || anUriVal == NULL;                // glTF 2.0
  }
  if (isBinary)
  {
    anOffset += myBinBodyOffset;

    RWGltf_GltfPrimArrayData& aData = theMeshData->AddPrimArrayData (theType);
    aData.Accessor = theAccessor;
    aData.StreamOffset = anOffset;
    aData.StreamUri = myFilePath;
    return true;
  }

  if (anUriVal == NULL
  || !anUriVal->IsString())
  {
    reportGltfError ("Buffer '" + theName + "' does not define uri.");
    return false;
  }

  const char* anUriData = anUriVal->GetString();
  if (::strncmp (anUriData, "data:application/octet-stream;base64,", 37) == 0)
  {
    RWGltf_GltfPrimArrayData& aData = theMeshData->AddPrimArrayData (theType);
    aData.Accessor = theAccessor;
    aData.StreamOffset = anOffset;
    if (!myDecodedBuffers.Find (theName, aData.StreamData))
    {
      // it is better decoding in multiple threads
      aData.StreamData = FSD_Base64Decoder::Decode ((const Standard_Byte* )anUriData + 37, anUriVal->GetStringLength() - 37);
      myDecodedBuffers.Bind (theName, aData.StreamData);
    }
    return true;
  }
  else
  {
    TCollection_AsciiString anUri = anUriData;
    if (anUri.IsEmpty())
    {
      reportGltfError ("Buffer '" + theName + "' does not define uri.");
      return false;
    }

    TCollection_AsciiString aPath = myFolder + anUri;
    bool isFileExist = false;
    if (!myProbedFiles.Find (aPath, isFileExist))
    {
      isFileExist = OSD_File (aPath).Exists();
      myProbedFiles.Bind (aPath, isFileExist);
    }
    if (!isFileExist)
    {
      reportGltfError ("Buffer '" + theName + "' refers to non-existing file '" + anUri + "'.");
      return false;
    }

    RWGltf_GltfPrimArrayData& aData = theMeshData->AddPrimArrayData (theType);
    aData.Accessor = theAccessor;
    aData.StreamOffset = anOffset;
    aData.StreamUri = myFolder + anUri;
    if (myExternalFiles != NULL)
    {
      myExternalFiles->Add (aData.StreamUri);
    }
    return true;
  }
}

// =======================================================================
// function : bindNamedShape
// purpose  :
// =======================================================================
void RWGltf_GltfJsonParser::bindNamedShape (TopoDS_Shape& theShape,
                                            ShapeMapGroup theGroup,
                                            const TopLoc_Location& theLoc,
                                            const TCollection_AsciiString& theId,
                                            const RWGltf_JsonValue* theUserName)
{
  if (theShape.IsNull())
  {
    return;
  }

  if (!theLoc.IsIdentity())
  {
    theShape.Location (theLoc);
  }

  TCollection_AsciiString aUserName;
  if (theUserName != NULL
   && theUserName->IsString())
  {
    aUserName = theUserName->GetString();
  }
  else if (myIsGltf1)
  {
    aUserName = theId;
  }

  myShapeMap[theGroup].Bind (theId, theShape);
  if (myAttribMap != NULL)
  {
    RWMesh_NodeAttributes aShapeAttribs;
    aShapeAttribs.Name    = aUserName;
    aShapeAttribs.RawName = theId;
    if (theShape.ShapeType() == TopAbs_FACE)
    {
      TopLoc_Location aDummy;
      if (Handle(RWGltf_GltfLatePrimitiveArray) aLateData = Handle(RWGltf_GltfLatePrimitiveArray)::DownCast (BRep_Tool::Triangulation (TopoDS::Face (theShape), aDummy)))
      {
        if (aLateData->HasStyle())
        {
          aShapeAttribs.Style.SetColorSurf (aLateData->BaseColor());
        }
      }
    }
    myAttribMap->Bind (theShape, aShapeAttribs);
  }
}
#endif

// =======================================================================
// function : Parse
// purpose  :
// =======================================================================
bool RWGltf_GltfJsonParser::Parse (const Handle(Message_ProgressIndicator)& theProgress)
{
  Message_ProgressSentry aPSentry (theProgress, "Reading Gltf", 0, 2, 1);
#ifdef HAVE_RAPIDJSON
  {
    if (!gltfParseRoots())
    {
      return false;
    }

    gltfParseAsset();
    gltfParseMaterials();
    if (!gltfParseScene (theProgress))
    {
      return false;
    }
  }
  aPSentry.Next();
  if (!aPSentry.More())
  {
    return false;
  }
  return true;
#else
  Message::DefaultMessenger()->Send ("Error: glTF reader is unavailable - OCCT has been built without RapidJSON support.", Message_Fail);
  return false;
#endif
}
