// File:	HLRTest_DrawableEdgeTool.cxx
// Created:	Wed Oct 14 11:16:16 1992
// Author:	Christophe MARION
//		<cma@sdsun1>

#include <HLRTest_DrawableEdgeTool.ixx>
#include <HLRTest_ShapeData.hxx>
#include <HLRBRep_Data.hxx>
#include <HLRAlgo_EdgeIterator.hxx>
#include <HLRBRep_ShapeBounds.hxx>

//=======================================================================
//function : HLRTest_DrawableEdgeTool
//purpose  : 
//=======================================================================

HLRTest_DrawableEdgeTool::HLRTest_DrawableEdgeTool
  (const Handle(HLRBRep_Algo)& Alg,
   const Standard_Boolean Visible,
   const Standard_Boolean IsoLine,
   const Standard_Boolean Rg1Line,
   const Standard_Boolean RgNLine,
   const Standard_Integer ViewId) :
  myAlgo(Alg),
  myVisible(Visible),
  myIsoLine(IsoLine),
  myRg1Line(Rg1Line),
  myRgNLine(RgNLine),
  myViewId(ViewId)
{}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void HLRTest_DrawableEdgeTool::DrawOn(Draw_Display& D) const
{
  if (myViewId == D.ViewId()) {
    if (myIsoLine) InternalDraw(D,1);
    InternalDraw(D,2);
    InternalDraw(D,3);
  }
}

//=======================================================================
//function : InternalDraw
//purpose  : 
//=======================================================================

void 
HLRTest_DrawableEdgeTool::InternalDraw (Draw_Display& D,
					const Standard_Integer typ) const
{
  Handle(HLRBRep_Data) DS = myAlgo->DataStructure();

  if (!DS.IsNull()) {
//    Standard_Real sta,end;
//    Standard_ShortReal tolsta,tolend;
//    Standard_Integer ie,v1,v2,e1,e2,f1,f2;
    Standard_Integer ie,e2;
    Standard_Integer iCB = 1;
    Standard_Integer nCB = myAlgo->NbShapes();
    Standard_Integer ne = DS->NbEdges();
    Standard_Integer nf = DS->NbFaces();
    HLRBRep_EdgeData* ed = &(DS->EDataArray().ChangeValue(0));
    ed++;
    e2 = 0;
    
    for (ie = 1; ie <= ne; ie++) {
      if (ed->Selected() && !ed->Vertical()) ed->Used(Standard_False);
      else                                   ed->Used(Standard_True);
      ed++;
    }

    for (Standard_Integer iface = 1; iface <= nf; iface++)
      DrawFace(D,typ,nCB,iface,e2,iCB,DS);
    
    if (typ >= 3) {
      iCB = 1;
      e2 = 0;
      HLRBRep_EdgeData* ed = &(DS->EDataArray().ChangeValue(0));
      ed++;
      
      for (Standard_Integer ie = 1; ie <= ne; ie++) {
	if (!ed->Used()) {
	  DrawEdge(D,Standard_False,typ,nCB,ie,e2,iCB,*ed);
	  ed->Used(Standard_True);
	}
	ed++;
      }
    }
  }
}

//=======================================================================
//function : DrawFace
//purpose  : 
//=======================================================================

void 
HLRTest_DrawableEdgeTool::DrawFace (Draw_Display& D,
				    const Standard_Integer typ,
				    const Standard_Integer nCB,
				    const Standard_Integer iface,
				    Standard_Integer& e2,
				    Standard_Integer& iCB,
				    Handle(HLRBRep_Data)& DS) const
{
  HLRBRep_FaceIterator Itf;

  for (Itf.InitEdge(DS->FDataArray().ChangeValue(iface));
       Itf.MoreEdge();
       Itf.NextEdge()) {               
    Standard_Integer ie = Itf.Edge();
    HLRBRep_EdgeData& edf = DS->EDataArray().ChangeValue(ie);
    if (!edf.Used()) {
      Standard_Boolean todraw;
      if ((!myRg1Line &&
	   !Itf.OutLine() &&
	   edf.Rg1Line()) ||
	  (!myRgNLine &&
	   !Itf.OutLine() &&
	   edf.RgNLine())) todraw =  Standard_False;
      else if (typ == 1) todraw =  Itf.IsoLine();
      else if (typ == 2) todraw =  Itf.OutLine() || Itf.Internal();
      else               todraw = !(Itf.IsoLine() ||
				    (Itf.OutLine() || Itf.Internal()));
      if (todraw) DrawEdge(D,Standard_True,typ,nCB,ie,e2,iCB,edf);
      edf.Used(Standard_True);
    }
  }
}

//=======================================================================
//function : DrawEdge
//purpose  : 
//=======================================================================

void 
HLRTest_DrawableEdgeTool::DrawEdge (Draw_Display& D,
				    const Standard_Boolean inFace,
				    const Standard_Integer typ,
				    const Standard_Integer nCB,
				    const Standard_Integer ie,
				    Standard_Integer& e2,
				    Standard_Integer& iCB,
				    HLRBRep_EdgeData& ed) const
{
  Standard_Boolean todraw = Standard_True;
  if (!inFace && 
      ((!myRg1Line && ed.Rg1Line()) ||
       (!myRgNLine && ed.RgNLine()))) todraw = Standard_False;
  if (todraw) {
    Standard_Real sta,end;
    Standard_ShortReal tolsta,tolend;
    Standard_Integer v1,v2,e1,f1,f2;
    HLRAlgo_EdgeIterator It;
    if (myVisible) {
      
      while (ie > e2 && iCB <= nCB) {
	HLRBRep_ShapeBounds& ShB 
	  = myAlgo->ShapeBounds(iCB);
	ShB.Bounds(v1,v2,e1,e2,f1,f2);
	Handle(HLRTest_ShapeData) ShData =
	  Handle(HLRTest_ShapeData)::DownCast(ShB.ShapeData());
	if      (typ == 1) D.SetColor(ShData->VisibleIsoColor());
	else if (typ == 2) D.SetColor(ShData->VisibleOutLineColor());
	else               D.SetColor(ShData->VisibleColor());
	iCB++;
      }
      
      const HLRBRep_Curve& ec = ed.Geometry();
      
      for (It.InitVisible(ed.Status());
	   It.MoreVisible();
	   It.NextVisible()) {
	It.Visible(sta,tolsta,end,tolend);
	D.MoveTo(ec.Value3D(sta));
	if (!ec.GetType() == GeomAbs_Line) {
	  Standard_Integer nbPnt = 100;
	  Standard_Real step = (end-sta)/(nbPnt+1);
	  
	  for (Standard_Integer i = 1; i <= nbPnt; i++) {
	    sta += step;
	    D.DrawTo(ec.Value3D(sta));
	  }
	}
	D.DrawTo(ec.Value3D(end));
      }
    }
    else {
      
      while (ie > e2 && iCB <= nCB) {
	HLRBRep_ShapeBounds& ShB
	  = myAlgo->ShapeBounds(iCB);
	ShB.Bounds(v1,v2,e1,e2,f1,f2);
	Handle(HLRTest_ShapeData) ShData =
	  Handle(HLRTest_ShapeData)::DownCast(ShB.ShapeData());
	if      (typ == 1) D.SetColor(ShData->HiddenIsoColor());
	else if (typ == 2) D.SetColor(ShData->HiddenOutLineColor());
	else               D.SetColor(ShData->HiddenColor());
	iCB++;
      }
      
      const HLRBRep_Curve& ec = ed.Geometry();
      
      for (It.InitHidden(ed.Status());
	   It.MoreHidden();
	   It.NextHidden()) {
	It.Hidden(sta,tolsta,end,tolend);
	D.MoveTo(ec.Value3D(sta));
	if (!ec.GetType() == GeomAbs_Line) {
	  Standard_Integer nbPnt = 100;
	  Standard_Real step = (end-sta)/(nbPnt+1);
	  
	  for (Standard_Integer i = 1; i <= nbPnt; i++) {
	    sta += step;
	    D.DrawTo(ec.Value3D(sta));
	  }
	}
	D.DrawTo(ec.Value3D(end));
      }
    }
  }
}

