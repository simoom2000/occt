#include <StepShape_LoopAndPath.ixx>

#include <StepShape_Loop.hxx>

#include <StepShape_Path.hxx>


StepShape_LoopAndPath::StepShape_LoopAndPath ()  {}

void StepShape_LoopAndPath::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_LoopAndPath::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_Loop)& aLoop,
	const Handle(StepShape_Path)& aPath)
{
	// --- classe own fields ---
	loop = aLoop;
	path = aPath;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_LoopAndPath::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfOrientedEdge)& aEdgeList)
{
	// --- classe inherited fields ---

	StepRepr_RepresentationItem::Init(aName);

	// --- ANDOR componant fields ---

	loop = new StepShape_Loop();
	loop->Init(aName);

	// --- ANDOR componant fields ---

	path = new StepShape_Path();
	path->Init(aName, aEdgeList);
}


void StepShape_LoopAndPath::SetLoop(const Handle(StepShape_Loop)& aLoop)
{
	loop = aLoop;
}

Handle(StepShape_Loop) StepShape_LoopAndPath::Loop() const
{
	return loop;
}

void StepShape_LoopAndPath::SetPath(const Handle(StepShape_Path)& aPath)
{
	path = aPath;
}

Handle(StepShape_Path) StepShape_LoopAndPath::Path() const
{
	return path;
}

	//--- Specific Methods for AND classe field access ---


	//--- Specific Methods for AND classe field access ---


void StepShape_LoopAndPath::SetEdgeList(const Handle(StepShape_HArray1OfOrientedEdge)& aEdgeList)
{
	path->SetEdgeList(aEdgeList);
}

Handle(StepShape_HArray1OfOrientedEdge) StepShape_LoopAndPath::EdgeList() const
{
	return path->EdgeList();
}

Handle(StepShape_OrientedEdge) StepShape_LoopAndPath::EdgeListValue(const Standard_Integer num) const
{
	return path->EdgeListValue(num);
}

Standard_Integer StepShape_LoopAndPath::NbEdgeList () const
{
	return path->NbEdgeList();
}
