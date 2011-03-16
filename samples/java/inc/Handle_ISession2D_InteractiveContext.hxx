// File generated by CPPExt (Transient)
//
//                     Copyright (C) 1991,1995 by
//  
//                      MATRA DATAVISION, FRANCE
//  
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of the above copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any other person. No title to an ownership of the
// software is hereby transferred.
//  
// At the termination of the contract, the software and all copies of this
// software must be deleted.

#ifndef _Handle_ISession2D_InteractiveContext_HeaderFile
#define _Handle_ISession2D_InteractiveContext_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_MMgt_TShared_HeaderFile
#include <Handle_MMgt_TShared.hxx>
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(MMgt_TShared);
class ISession2D_InteractiveContext;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(ISession2D_InteractiveContext);

class Handle(ISession2D_InteractiveContext) : public Handle(MMgt_TShared) {
  public:
    void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }
    Handle(ISession2D_InteractiveContext)():Handle(MMgt_TShared)() {} 
    Handle(ISession2D_InteractiveContext)(const Handle(ISession2D_InteractiveContext)& aHandle) : Handle(MMgt_TShared)(aHandle) 
     {
     }

    Handle(ISession2D_InteractiveContext)(const ISession2D_InteractiveContext* anItem) : Handle(MMgt_TShared)((MMgt_TShared *)anItem) 
     {
     }

    Handle(ISession2D_InteractiveContext)& operator=(const Handle(ISession2D_InteractiveContext)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(ISession2D_InteractiveContext)& operator=(const ISession2D_InteractiveContext* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    ISession2D_InteractiveContext* operator->() 
     {
      return (ISession2D_InteractiveContext *)ControlAccess();
     }

    ISession2D_InteractiveContext* operator->() const 
     {
      return (ISession2D_InteractiveContext *)ControlAccess();
     }

   Standard_EXPORT ~Handle(ISession2D_InteractiveContext)();
 
   Standard_EXPORT static const Handle(ISession2D_InteractiveContext) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif
