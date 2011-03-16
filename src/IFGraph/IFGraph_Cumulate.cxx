#include <IFGraph_Cumulate.ixx>
#include <IFGraph_AllShared.hxx>
#include <Interface_InterfaceModel.hxx>



// Calcul de cumul
// Tres simple, on note les entites demandees, et a la fin
// on a le cumul lui-meme, et comme infos derivees, les doubles et les oublis
// Chaque recouvrement correspond a une augmentation de UN du status
// Les status demarrent a 2, ainsi a l ajout d une entite, on distingue bien
// entre les entites nouvelles, liees a cet appel (statut temporaire 1) et les
// autres (statut superieur ou egal a 2)

    IFGraph_Cumulate::IFGraph_Cumulate (const Interface_Graph& agraph)
      : thegraph (agraph)    {  }

    void  IFGraph_Cumulate::GetFromEntity
  (const Handle(Standard_Transient)& ent)
{
  IFGraph_AllShared iter(thegraph.Model(),ent);
  GetFromIter (iter);
}

    void  IFGraph_Cumulate::ResetData ()
      {  Reset();  thegraph.Reset();  }

    void  IFGraph_Cumulate::GetFromIter (const Interface_EntityIterator& iter)
{
  thegraph.GetFromIter(iter,1,1,Standard_True);
  thegraph.ChangeStatus (1,2);   // une fois le calcul fait
}

    void  IFGraph_Cumulate::Evaluate ()
{
  Reset();  GetFromGraph(thegraph);    // evaluation deja faite dans le graphe
}

    Interface_EntityIterator  IFGraph_Cumulate::Overlapped () const
{
  Interface_EntityIterator iter;
  Standard_Integer nb = thegraph.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thegraph.IsPresent(i) && thegraph.Status(i) > 2)
      iter.GetOneItem(thegraph.Entity(i));
  }
  return iter;
}

    Interface_EntityIterator  IFGraph_Cumulate::Forgotten () const
{
  Interface_EntityIterator iter;
  Standard_Integer nb = thegraph.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (!thegraph.IsPresent(i))
      iter.GetOneItem(thegraph.Model()->Value(i));
  }
  return iter;
}

    Interface_EntityIterator  IFGraph_Cumulate::PerCount
  (const Standard_Integer count) const
{
  Interface_EntityIterator iter;
  Standard_Integer nb = thegraph.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thegraph.IsPresent(i) && thegraph.Status(i) == (count + 1))
      iter.GetOneItem(thegraph.Model()->Value(i));
  }
  return iter;
}


    Standard_Integer  IFGraph_Cumulate::NbTimes
  (const Handle(Standard_Transient)& ent) const
{
  Standard_Integer num = thegraph.EntityNumber(ent);
  if (num == 0) return 0;
  Standard_Integer stat = thegraph.Status(num);
  return stat-1;
}

    Standard_Integer  IFGraph_Cumulate::HighestNbTimes () const
{
  Standard_Integer max = 0;
  Standard_Integer nb = thegraph.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (!thegraph.IsPresent(i)) continue;
    Standard_Integer count = thegraph.Status(i) - 1;
    if (count > max) max = count;
  }
  return max;
}
