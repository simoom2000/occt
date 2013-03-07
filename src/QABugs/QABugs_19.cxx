// Created on: 2002-05-21
// Created by: QA Admin
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <QABugs.hxx>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Ax1.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <DrawTrSurf.hxx>

#include <Precision.hxx>

#include <PCollection_HAsciiString.hxx>

#include <cstdio>
#include <cmath>
#include <iostream>
#include <OSD_PerfMeter.hxx>
#include <OSD_Timer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepAlgo_Cut.hxx>

static Standard_Integer OCC230 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 4) {
    di << "ERROR OCC230: Usage : " << argv[0] << " TrimmedCurve Pnt2d Pnt2d" << "\n";
    return 1;
  }

  gp_Pnt2d P1, P2;
  if ( !DrawTrSurf::GetPoint2d(argv[2],P1)) {
    di << "ERROR OCC230: " << argv[2] << " is not Pnt2d" << "\n";
    return 1;
  }
  if ( !DrawTrSurf::GetPoint2d(argv[3],P2)) {
    di << "ERROR OCC230: " << argv[3] << " is not Pnt2d" << "\n";
    return 1;
  }

  GCE2d_MakeSegment MakeSegment(P1,P2);
  Handle(Geom2d_TrimmedCurve) TrimmedCurve = MakeSegment.Value();
  DrawTrSurf::Set(argv[1], TrimmedCurve);
  return 0;
}

static Standard_Integer OCC142 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  for(Standard_Integer i= 0;i <= 20;i++){
    Handle(PCollection_HAsciiString) pstr = new PCollection_HAsciiString("TEST");
    pstr->Clear();
  }
  di << "OCC142: OK" << "\n";
  return 0;
}

static Standard_Integer OCC23361 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  gp_Pnt p(0, 0, 2);
  
  gp_Trsf t1, t2;
  t1.SetRotation(gp_Ax1(p, gp_Dir(0, 1, 0)), -0.49328285294022267);
  t2.SetRotation(gp_Ax1(p, gp_Dir(0, 0, 1)), 0.87538474718473880);

  gp_Trsf tComp = t2 * t1;

  gp_Pnt p1(10, 3, 4);
  gp_Pnt p2 = p1.Transformed(tComp);
  gp_Pnt p3 = p1.Transformed(t1);
  p3.Transform(t2);

  // points must be equal
  if ( ! p2.IsEqual(p3, Precision::Confusion()) )
    di << "ERROR OCC23361: equivalent transformations does not produce equal points" << "\n";
  else 
    di << "OCC23361: OK" << "\n";

  return 0;
}

static Standard_Integer OCC23237 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char** /*argv*/)
{
  OSD_PerfMeter aPM("TestMeter",0);
  OSD_Timer aTM;
  
  // run some operation in cycle for about 2 seconds to have good values of times to compare
  int count = 0;
  printf("OSD_PerfMeter test.\nRunning Boolean operation on solids in loop.\n");
  for (; aTM.ElapsedTime() < 2.; count++)
  {
    aPM.Start();
    aTM.Start();

    // do some operation that will take considerable time compared with time or starting / stopping timers
    BRepPrimAPI_MakeBox aBox (10., 10., 10.);
    BRepPrimAPI_MakeSphere aSphere (10.);
    BRepAlgo_Cut (aBox.Shape(), aSphere.Shape());

    aTM.Stop();
    aPM.Stop();
  }
 
  int aNbEnters = 0;
  Standard_Real aPerfMeter_CPUtime = 0., aTimer_ElapsedTime = aTM.ElapsedTime();

  perf_get_meter("TestMeter", &aNbEnters, &aPerfMeter_CPUtime);

  Standard_Real aTimeDiff = (fabs(aTimer_ElapsedTime - aPerfMeter_CPUtime) / aTimer_ElapsedTime);

  printf("\nMeasurement results (%d cycles):\n", count);
  printf("\nOSD_PerfMeter CPU time: %lf\nOSD_Timer elapsed time: %lf\n", aPerfMeter_CPUtime, aTimer_ElapsedTime);
  printf("Time delta is: %.3lf %%\n", aTimeDiff * 100);

  if (aTimeDiff > 0.2)
    di << "OCC23237: Error: too much difference between CPU and elapsed times";
  else if (aNbEnters != count)
    di << "OCC23237: Error: counter reported by PerfMeter (" << aNbEnters << ") does not correspond to actual number of cycles";
  else
    di << "OCC23237: OK";

  return 0;
}

#ifdef HAVE_TBB

#include <Standard_Atomic.hxx>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

class IncrementerDecrementer
{
public:
    IncrementerDecrementer (Standard_Integer* theVal, Standard_Boolean thePositive) : myVal (theVal), myPositive (thePositive)
    {}
    void operator() (const tbb::blocked_range<size_t>& r) const
    {
        if (myPositive)
            for (size_t i = r.begin(); i != r.end(); ++i)
                Standard_Atomic_Increment (myVal);
        else
            for (size_t i = r.begin(); i != r.end(); ++i)
                Standard_Atomic_Decrement (myVal);
    }
private:
    Standard_Integer*   myVal;
    Standard_Boolean   myPositive;
};
#endif

#define QCOMPARE(val1, val2) \
  di << "Checking " #val1 " == " #val2 << \
        ((val1) == (val2) ? ": OK\n" : ": Error\n")

#ifdef HAVE_TBB
static Standard_Integer OCC22980 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  int aSum = 0;

  //check returned value
  QCOMPARE (Standard_Atomic_Decrement (&aSum), -1);
  QCOMPARE (Standard_Atomic_Increment (&aSum), 0);
  QCOMPARE (Standard_Atomic_Increment (&aSum), 1);
  QCOMPARE (Standard_Atomic_Increment (&aSum), 2);
//  QCOMPARE (Standard_Atomic_DecrementTest (&aSum), 0);
//  QCOMPARE (Standard_Atomic_DecrementTest (&aSum), 1);

  //check atomicity 
  aSum = 0;
  const int N = 1 << 24; //big enough to ensure concurrency

  //increment
  tbb::parallel_for (tbb::blocked_range<size_t> (0, N), IncrementerDecrementer (&aSum, true));
  QCOMPARE (aSum, N);

  //decrement
  tbb::parallel_for (tbb::blocked_range<size_t> (0, N), IncrementerDecrementer (&aSum, false));
  QCOMPARE (aSum, 0);

  return 0;
}

#else /* HAVE_TBB */

static Standard_Integer OCC22980 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char **argv)
{
  di << "Test skipped: command " << argv[0] << " requires TBB library\n";
  return 0;
}

#endif /* HAVE_TBB */

#include <TDocStd_Application.hxx>
#include <XCAFApp_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>

static Standard_Integer OCC23595 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char **argv)
{
  const Handle(TDocStd_Application)& anApp = XCAFApp_Application::GetApplication();
  Handle(TDocStd_Document) aDoc;
  anApp->NewDocument ("XmlXCAF", aDoc);
  QCOMPARE (!aDoc.IsNull(), Standard_True);

  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool (aDoc->Main());

  //check default value
  Standard_Boolean aValue = XCAFDoc_ShapeTool::AutoNaming();
  QCOMPARE (aValue, Standard_True);

  //true
  XCAFDoc_ShapeTool::SetAutoNaming (Standard_True);
  TopoDS_Shape aShape = BRepPrimAPI_MakeBox (100., 200., 300.).Shape();
  TDF_Label aLabel = aShTool->AddShape (aShape);
  Handle(TDataStd_Name) anAttr;
  QCOMPARE (aLabel.FindAttribute (TDataStd_Name::GetID(), anAttr), Standard_True);

  //false
  XCAFDoc_ShapeTool::SetAutoNaming (Standard_False);
  aShape = BRepPrimAPI_MakeBox (300., 200., 100.).Shape();
  aLabel = aShTool->AddShape (aShape);
  QCOMPARE (!aLabel.FindAttribute (TDataStd_Name::GetID(), anAttr), Standard_True);

  //restore
  XCAFDoc_ShapeTool::SetAutoNaming (aValue);

  return 0;
}

#include <ExprIntrp_GenExp.hxx>
Standard_Integer OCC22611 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{

  if (argc != 3) {
    di << "Usage : " << argv[0] << " string nb\n";
    return 1;
  }

  TCollection_AsciiString aToken = argv[1];
  Standard_Integer aNb = atoi(argv[2]);

  Handle(ExprIntrp_GenExp) aGen = ExprIntrp_GenExp::Create();
  for (Standard_Integer i=0; i < aNb; i++)
  {
    aGen->Process(aToken);
    Handle(Expr_GeneralExpression) aExpr = aGen->Expression();
  }

  return 0;
}

Standard_Integer OCC22595 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  gp_Mat M0;
  di << "M0 = "
  << " {" << M0(1,1) << "} {" << M0(1,2) << "} {" << M0(1,3) <<"}"
  << " {" << M0(2,1) << "} {" << M0(2,2) << "} {" << M0(2,3) <<"}"
  << " {" << M0(1,1) << "} {" << M0(1,2) << "} {" << M0(1,3) <<"}";
  return 0;
}

void QABugs::Commands_19(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add ("OCC230", "OCC230 TrimmedCurve Pnt2d Pnt2d", __FILE__, OCC230, group);
  theCommands.Add ("OCC142", "OCC142", __FILE__, OCC142, group);
  theCommands.Add ("OCC23361", "OCC23361", __FILE__, OCC23361, group);
  theCommands.Add ("OCC23237", "OCC23237", __FILE__, OCC23237, group); 
  theCommands.Add ("OCC22980", "OCC22980", __FILE__, OCC22980, group);
  theCommands.Add ("OCC23595", "OCC23595", __FILE__, OCC23595, group);
  theCommands.Add ("OCC22611", "OCC22611 string nb", __FILE__, OCC22611, group);
  theCommands.Add ("OCC22595", "OCC22595", __FILE__, OCC22595, group);

  return;
}
