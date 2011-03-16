// File:	QANewBRepNaming_Box.cxx
// Created:	Fri Sep 24 16:21:09 1999
// Author:	Sergey ZARITCHNY
//		<szy@shamox.nnov.matra-dtv.fr>
// Copyright:	Matra Datavision 1999

#include <QANewBRepNaming_Box.ixx>

#include <QANewBRepNaming_Loader.hxx>
#include <TNaming_Builder.hxx>
#include <TDF_Label.hxx>
#include <Standard_NullObject.hxx>
#include <TopoDS_Solid.hxx>

//=======================================================================
//function : QANewBRepNaming_Box
//purpose  : 
//=======================================================================

QANewBRepNaming_Box::QANewBRepNaming_Box() {}

//=======================================================================
//function : QANewBRepNaming_Box
//purpose  : 
//=======================================================================

QANewBRepNaming_Box::QANewBRepNaming_Box(const TDF_Label& Label)
     : QANewBRepNaming_TopNaming(Label) {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void QANewBRepNaming_Box::Init(const TDF_Label& Label) {
  if(Label.IsNull())
    Standard_NullObject::Raise("QANewBRepNaming_Box::Init The Result label is Null ..."); 
  myResultLabel = Label;
}  

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void QANewBRepNaming_Box::Load (BRepPrimAPI_MakeBox& MS, const QANewBRepNaming_TypeOfPrimitive3D Type) const {
  //Load the faces of the box :
  TopoDS_Face BottomFace = MS.BottomFace ();
  TNaming_Builder BottomFaceIns (Bottom ()); 
  BottomFaceIns.Generated (BottomFace);
 
  TopoDS_Face TopFace = MS.TopFace ();
  TNaming_Builder TopFaceIns (Top ()); 
  TopFaceIns.Generated (TopFace); 

  TopoDS_Face FrontFace = MS.FrontFace ();
  TNaming_Builder FrontFaceIns (Front ()); 
  FrontFaceIns.Generated (FrontFace); 

  TopoDS_Face RightFace = MS.RightFace ();
  TNaming_Builder RightFaceIns (Right ()); 
  RightFaceIns.Generated (RightFace); 

  TopoDS_Face BackFace = MS.BackFace ();
  TNaming_Builder BackFaceIns (Back ()); 
  BackFaceIns.Generated (BackFace); 

  TopoDS_Face LeftFace = MS.LeftFace ();
  TNaming_Builder LeftFaceIns (Left ()); 
  LeftFaceIns.Generated (LeftFace); 

  TNaming_Builder Builder (ResultLabel());
  if (Type == QANewBRepNaming_SOLID) Builder.Generated (MS.Solid());
  else if (Type == QANewBRepNaming_SHELL) Builder.Generated (MS.Shell());
  else {
#ifdef MDTV_DEB
    cout<<"QANewBRepNaming_Box::Load(): Unexpected type of result"<<endl;
    Builder.Generated (MS.Shape());
#endif
  }
}

//=======================================================================
//function : Back
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Box::Back () const {
  return ResultLabel().FindChild(1,Standard_True); 
}

//=======================================================================
//function : Front
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Box::Front () const {
  return ResultLabel().FindChild(2,Standard_True); 
}

//=======================================================================
//function : Left
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Box::Left () const {
  return ResultLabel().FindChild(3,Standard_True); 
}

//=======================================================================
//function : Right
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Box::Right () const {
  return ResultLabel().FindChild(4,Standard_True); 
}

//=======================================================================
//function : Bottom
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Box::Bottom () const {
  return ResultLabel().FindChild(5,Standard_True); 
}

//=======================================================================
//function : Top
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Box::Top () const {
  return ResultLabel().FindChild(6,Standard_True); 
}

