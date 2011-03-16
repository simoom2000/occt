// File:      ShapeAnalysis_BoxBndTree.cxx
// Created:   14.02.05 12:39:31
// Author:    Alexey MORENOV
// Copyright: Open CASCADE  2005

#include <ShapeAnalysis_BoxBndTree.hxx>
#include <Standard_NoSuchObject.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>
#include <ShapeAnalysis.hxx>
#include <gp_Pnt.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <BRep_Tool.hxx>

//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_BoxBndTreeSelector::
  Reject (const Bnd_Box& theBnd) const
{
  Standard_Boolean fch = myFBox.IsOut(theBnd);
  Standard_Boolean lch = myLBox.IsOut(theBnd);
  if (fch == Standard_False || lch == Standard_False) return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : Accept
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_BoxBndTreeSelector::
  Accept (const Standard_Integer& theObj)
{
  if (theObj < 1 || theObj > mySeq->Length())
    Standard_NoSuchObject::Raise
      ("ShapeAnalysis_BoxBndTreeSelector::Accept : no such object for current index");
  Standard_Boolean IsAccept = Standard_False;
  if (myList.Contains(theObj))
    return Standard_False;
  
   
  TopoDS_Wire W = TopoDS::Wire (mySeq->Value (theObj));
  TopoDS_Vertex V1,V2;                         
  ShapeAnalysis::FindBounds (W,V1,V2);
  if(myShared){
    if (myLVertex.IsSame(V1)){      
      myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
      IsAccept = Standard_True;
    }
    else {
      if (myLVertex.IsSame(V2)){
        myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE2);
        IsAccept = Standard_True;
      }
      else {
        if (myFVertex.IsSame(V2)){
          myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE3);
          IsAccept = Standard_True;
        }
        else {
          if (myFVertex.IsSame(V1)){
            myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE4);
            IsAccept = Standard_True;
          }
          else myStatus = ShapeExtend::EncodeStatus (ShapeExtend_FAIL2);

          
        }
      }
    }
    
    if (IsAccept){
      SetNb(theObj);
      myStop = Standard_True;
      return Standard_True;
    }
    else myStop = Standard_False;
  }
  
  else{
    gp_Pnt p1 = BRep_Tool::Pnt(V1);
    gp_Pnt p2 = BRep_Tool::Pnt(V2);
    
    Standard_Real tailhead, tailtail, headhead, headtail;
    tailhead = p1.Distance(myLPnt);
    tailtail = p2.Distance(myLPnt);
    headhead = p1.Distance(myFPnt);
    headtail = p2.Distance(myFPnt);
    Standard_Real dm1 = tailhead, dm2 = headtail;
    Standard_Integer res1 = 0, res2 = 0;
    if (tailhead > tailtail) {res1 = 1; dm1 = tailtail;}
    if (headtail > headhead) {res2 = 1; dm2 = headhead;}
    Standard_Integer result = res1;
    Standard_Real min3d;
    min3d = Min (dm1, dm2);
    if (min3d > myMin3d)
      return Standard_False;

    myMin3d = min3d;
    if (min3d > myTol)
    {
       myStatus = ShapeExtend::EncodeStatus (ShapeExtend_FAIL2);
       return Standard_False;
    }
   
    SetNb(theObj);
    
    if (min3d == 0)
      myStop = Standard_True;
    
    if (dm1 > dm2) 
    {
      dm1 = dm2; 
      result = res2 + 2;
    }
    
   
    switch (result) {
        case 0: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1); break; 
        case 1: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE2);  break;
        case 2: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE3);  break;
        case 3: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE4);  break;
      }
    
      return Standard_True;
    
  }  
   
  return Standard_False;
}

