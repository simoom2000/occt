// File:	Geom2dAPI_ProjectPointOnCurve.cxx
// Created:	Wed Mar 23 15:16:37 1994
// Author:	Bruno DUMORTIER
//		<dub@fuegox>

#include <Geom2dAPI_ProjectPointOnCurve.ixx>

#include <Geom2dAdaptor_Curve.hxx>


//=======================================================================
//function : Geom2dAPI_ProjectPointOnCurve
//purpose  : 
//=======================================================================

Geom2dAPI_ProjectPointOnCurve::Geom2dAPI_ProjectPointOnCurve()
{
  myIsDone = Standard_False;
}


//=======================================================================
//function : Geom2dAPI_ProjectPointOnCurve
//purpose  : 
//=======================================================================

Geom2dAPI_ProjectPointOnCurve::Geom2dAPI_ProjectPointOnCurve
  (const gp_Pnt2d&             P, 
   const Handle(Geom2d_Curve)& Curve)
{
  Init(P,Curve);
}


//=======================================================================
//function : Geom2dAPI_ProjectPointOnCurve
//purpose  : 
//=======================================================================

Geom2dAPI_ProjectPointOnCurve::Geom2dAPI_ProjectPointOnCurve
  (const gp_Pnt2d&             P, 
   const Handle(Geom2d_Curve)& Curve,
   const Standard_Real         Umin,
   const Standard_Real         Usup)
{
  Init(P,Curve,Umin,Usup);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Geom2dAPI_ProjectPointOnCurve::Init
  (const gp_Pnt2d&             P,
   const Handle(Geom2d_Curve)& Curve)
{
  Init(P,Curve,Curve->FirstParameter(),Curve->LastParameter());
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Geom2dAPI_ProjectPointOnCurve::Init
  (const gp_Pnt2d&             P,
   const Handle(Geom2d_Curve)& Curve,
   const Standard_Real         Umin,
   const Standard_Real         Usup )
{
  myC.Load(Curve,Umin,Usup);

  Extrema_ExtPC2d theExtPC2d(P, myC);

  myExtPC = theExtPC2d;
  
  myIsDone = myExtPC.IsDone() && ( myExtPC.NbExt() > 0);


  // evaluate the lower distance and its index;

  if ( myIsDone) {
    Standard_Real Dist2, Dist2Min = myExtPC.SquareDistance(1);
    myIndex = 1;
    
    for ( Standard_Integer i = 2; i <= myExtPC.NbExt(); i++) {
      Dist2 = myExtPC.SquareDistance(i);
      if ( Dist2 < Dist2Min) {
	Dist2Min = Dist2;
	myIndex = i;
      }
    }
  }
}


//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================

Standard_Integer Geom2dAPI_ProjectPointOnCurve::NbPoints() const 
{
  if ( myIsDone)
    return myExtPC.NbExt();
  else
    return 0;
}


//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

gp_Pnt2d Geom2dAPI_ProjectPointOnCurve::Point
  (const Standard_Integer Index) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbPoints(),
			       "Geom2dAPI_ProjectPointOnCurve::Point");
  return (myExtPC.Point(Index)).Value();
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real Geom2dAPI_ProjectPointOnCurve::Parameter
  (const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbPoints(),
			       "Geom2dAPI_ProjectPointOnCurve::Parameter");
  return (myExtPC.Point(Index)).Parameter();
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Geom2dAPI_ProjectPointOnCurve::Parameter
  (const Standard_Integer Index,
         Standard_Real&   U     ) const
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbPoints(),
			       "Geom2dAPI_ProjectPointOnCurve::Parameter");
  U = (myExtPC.Point(Index)).Parameter();
}


//=======================================================================
//function : Distance
//purpose  : 
//=======================================================================

Standard_Real Geom2dAPI_ProjectPointOnCurve::Distance
  (const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbPoints(),
			       "Geom2dAPI_ProjectPointOnCurve::Distance");
  return sqrt(myExtPC.SquareDistance(Index));
}


//=======================================================================
//function : NearestPoint
//purpose  : 
//=======================================================================

gp_Pnt2d Geom2dAPI_ProjectPointOnCurve::NearestPoint() const 
{
  StdFail_NotDone_Raise_if
    (!myIsDone, "Geom2dAPI_ProjectPointOnCurve:NearestPoint");

  return (myExtPC.Point(myIndex)).Value();
}


//=======================================================================
//function : Standard_Integer
//purpose  : 
//=======================================================================

Geom2dAPI_ProjectPointOnCurve::operator Standard_Integer() const
{
  return NbPoints();
}


//=======================================================================
//function : gp_Pnt2d
//purpose  : 
//=======================================================================

Geom2dAPI_ProjectPointOnCurve::operator gp_Pnt2d() const
{
  return NearestPoint();
}


//=======================================================================
//function : LowerDistanceParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2dAPI_ProjectPointOnCurve::LowerDistanceParameter() const
{
  StdFail_NotDone_Raise_if
    (!myIsDone, "Geom2dAPI_ProjectPointOnCurve:LowerDistanceParameter");

  return (myExtPC.Point(myIndex)).Parameter();
}


//=======================================================================
//function : LowerDistance
//purpose  : 
//=======================================================================

Standard_Real Geom2dAPI_ProjectPointOnCurve::LowerDistance() const
{
  StdFail_NotDone_Raise_if
    (!myIsDone,"Geom2dAPI_ProjectPointOnCurve:LowerDistance");

  return sqrt (myExtPC.SquareDistance(myIndex));
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

Geom2dAPI_ProjectPointOnCurve::operator Standard_Real() const
{
  return LowerDistance();
}
