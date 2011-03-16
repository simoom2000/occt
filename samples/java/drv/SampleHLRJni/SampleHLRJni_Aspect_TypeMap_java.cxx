//
//                     Copyright (C) 1991 - 2000 by  
//                      Matra Datavision SA.  All rights reserved.
//  
//                     Copyright (C) 2001 - 2004 by
//                     Open CASCADE SA.  All rights reserved.
// 
// This file is part of the Open CASCADE Technology software.
//
// This software may be distributed and/or modified under the terms and
// conditions of the Open CASCADE Public License as defined by Open CASCADE SA
// and appearing in the file LICENSE included in the packaging of this file.
//  
// This software is distributed on an "AS IS" basis, without warranty of any
// kind, and Open CASCADE SA hereby disclaims all such warranties,
// including without limitation, any warranties of merchantability, fitness
// for a particular purpose or non-infringement. Please see the License for
// the specific terms and conditions governing rights and limitations under the
// License.

#include <SampleHLRJni_Aspect_TypeMap.h>
#include <Aspect_TypeMap.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_TypeMapEntry.hxx>
#include <Standard_Integer.hxx>
#include <Aspect_LineStyle.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1TypeMap_Aspect_1TypeMap_1Create_10 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_TypeMap)* theret = new Handle(Aspect_TypeMap);
*theret = new Aspect_TypeMap();
jcas_SetHandle(env,theobj,theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1TypeMap_Aspect_1TypeMap_1AddEntry_11 (JNIEnv *env, jobject theobj, jobject AnEntry)
{

jcas_Locking alock(env);
{
try {
Aspect_TypeMapEntry* the_AnEntry = (Aspect_TypeMapEntry*) jcas_GetHandle(env,AnEntry);
if ( the_AnEntry == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_AnEntry = new Aspect_TypeMapEntry ();
 // jcas_SetHandle ( env, AnEntry, the_AnEntry );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Aspect_TypeMap) the_this = *((Handle(Aspect_TypeMap)*) jcas_GetHandle(env,theobj));
the_this->AddEntry(*the_AnEntry);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1TypeMap_Aspect_1TypeMap_1AddEntry_12 (JNIEnv *env, jobject theobj, jobject aStyle)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* the_aStyle = (Aspect_LineStyle*) jcas_GetHandle(env,aStyle);
if ( the_aStyle == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aStyle = new Aspect_LineStyle ();
 // jcas_SetHandle ( env, aStyle, the_aStyle );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Aspect_TypeMap) the_this = *((Handle(Aspect_TypeMap)*) jcas_GetHandle(env,theobj));
 thejret = the_this->AddEntry(*the_aStyle);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1TypeMap_Size (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_TypeMap) the_this = *((Handle(Aspect_TypeMap)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Size();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1TypeMap_Index (JNIEnv *env, jobject theobj, jint aTypemapIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_TypeMap) the_this = *((Handle(Aspect_TypeMap)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Index((Standard_Integer) aTypemapIndex);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1TypeMap_Dump (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_TypeMap) the_this = *((Handle(Aspect_TypeMap)*) jcas_GetHandle(env,theobj));
the_this->Dump();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1TypeMap_Entry (JNIEnv *env, jobject theobj, jint AnIndex)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_TypeMap) the_this = *((Handle(Aspect_TypeMap)*) jcas_GetHandle(env,theobj));
Aspect_TypeMapEntry* theret = new Aspect_TypeMapEntry(the_this->Entry((Standard_Integer) AnIndex));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_TypeMapEntry",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}


}
