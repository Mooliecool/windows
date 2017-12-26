// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
// ===========================================================================
// File: TYPEHANDLE.CPP
//
// ===========================================================================

#include "common.h"
#include "class.h"
#include "typehandle.h"
#include "eeconfig.h"
#include "generics.h"
#include "typedesc.h"
#include "typekey.h"
#include "typestring.h"
#include "classloadlevel.h"

// This method is not being called by all the constructors of TypeHandle
// because of the following reason. SystemDomain::LoadBaseSystemClasses() 
// loads TYPE__OBJECT_ARRAY which causes the following issues:
//
// If mscorlib is JIT-compiled, Module::CreateArrayMethodTable calls
// TypeString::AppendName() with a TypeHandle that wraps the MethodTable
// being created.
// If mscorlib is ngenned, Module::RestoreMethodTablePointer() needs 
// a TypeHandle to call ClassLoader::EnsureLoaded().
//


#ifdef _DEBUG_IMPL

BOOL TypeHandle::Verify() 
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (IsNull())
        return(TRUE);

    if (IsUnsharedMT())
    {
        // If you try to do IBC logging of a type being created, the type
        // will look inconsistent. IBC logging knows to filter out such types.
        if (g_IBCLogger.InstrEnabled())
            return TRUE;

        _ASSERTE(AsMethodTable()->SanityCheck());

        //
    }
    else
    {
        if (!IsRestored())
            return TRUE;

        if (IsArray())
            AsArray()->Verify();
    }
    return(TRUE);
}

#endif // _DEBUG_IMPL

unsigned TypeHandle::GetSize()  const
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;        
    }
    CONTRACTL_END

    CorElementType type = GetInternalCorElementType();

    if (type == ELEMENT_TYPE_VALUETYPE)
    {

        return(GetMethodTable()->GetNumInstanceFieldBytes());
    }

    return(GetSizeForCorElementType(type));
}

Module* TypeHandle::GetModule() const { 
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsTypeDesc())
        return AsTypeDesc()->GetModule();
    return(AsMethodTable()->GetModule());
}

Assembly* TypeHandle::GetAssembly() const { 
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsTypeDesc())
        return AsTypeDesc()->GetAssembly();
    return(AsMethodTable()->GetAssembly());
}

BOOL TypeHandle::IsArray() const { 
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    return(IsTypeDesc() && AsTypeDesc()->IsArray());
}

BOOL TypeHandle::IsArrayType() const { 
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    return ( IsTypeDesc() && AsTypeDesc()->IsArray()) || 
           (!IsTypeDesc() && AsMethodTable()->IsArray());
}


BOOL TypeHandle::IsGenericVariable() const { 
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    return(IsTypeDesc() && CorTypeInfo::IsGenericVariable(AsTypeDesc()->GetInternalCorElementType()));
}

BOOL TypeHandle::HasTypeParam() const {
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    return(IsTypeDesc() && CorTypeInfo::IsModifier(AsTypeDesc()->GetInternalCorElementType()));
}

Module *TypeHandle::GetDefiningModuleForOpenType() const
{
    Module* returnValue = NULL;
   
    INTERIOR_STACK_PROBE_NOTHROW_CHECK_THREAD(goto Exit;);
    
    if (IsGenericVariable())
    { 
        TypeVarTypeDesc* pTyVar = (TypeVarTypeDesc*) (AsTypeDesc());
        returnValue = pTyVar->GetModule();
        goto Exit;
    }
    
    if (HasTypeParam())
    {
        returnValue = GetTypeParam().GetDefiningModuleForOpenType();
    }
    else if (HasInstantiation())
    {
        returnValue = GetMethodTable()->GetDefiningModuleForOpenType();
    }
Exit:
    ;
    END_INTERIOR_STACK_PROBE;

    return returnValue;
}

BOOL TypeHandle::ContainsGenericVariables(BOOL methodOnly /*=FALSE*/) const
{
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsTypeDesc())
    {
        if (IsGenericVariable())
        { 
            if (!methodOnly)
                return TRUE;

            TypeVarTypeDesc* pTyVar = (TypeVarTypeDesc*) (AsTypeDesc());
            return TypeFromToken(pTyVar->GetTypeOrMethodDef()) == mdtMethodDef;
        }

        if (HasTypeParam())
        {
            return GetTypeParam().ContainsGenericVariables(methodOnly);
        }
    }
    else if (HasInstantiation())
    {
        if (GetMethodTable()->ContainsGenericVariables(methodOnly))
            return TRUE;
    }

    return FALSE;
}

// Recursively search the type arguments and if
// one of the type arguments is Canon then return TRUE
//
// A<__Canon>    is the canonical TypeHandle (aka "representative" generic MT)
// A<B<__Canon>> is a subtype that contains a Canonical type
//
BOOL TypeHandle::IsCanonicalSubtype() const {
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (GetMethodTable() == g_pHiddenMethodTableClass)
        return TRUE;

    if (HasInstantiation())
    {
        TypeHandle * pInstantiation = GetInstantiation();

        for (DWORD i = 0; i < GetNumGenericArgs(); i++)
        {
            if (pInstantiation[i].IsCanonicalSubtype())
                return TRUE;
        }
    }
    else if (HasTypeParam())
    {
        return GetTypeParam().IsCanonicalSubtype();
    }

    return FALSE;
}


//@GENERICS:
// Return the number of type parameters in the instantiation of an instantiated type
// or the number of type parameters to a generic type
// Return 0 otherwise.
DWORD TypeHandle::GetNumGenericArgs() const {
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsTypeDesc())
        return 0;
    else
        return GetMethodTable()->GetNumGenericArgs();
}

BOOL TypeHandle::IsGenericTypeDefinition() const {
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (!IsTypeDesc())
        return AsMethodTable()->IsGenericTypeDefinition();
    else 
        return FALSE;
}

MethodTable* TypeHandle::GetCanonicalMethodTable() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsUnsharedMT()) 
        return AsMethodTable()->GetCanonicalMethodTable();
    else
    {
        MethodTable *pMT = AsTypeDesc()->GetMethodTable();
        if (pMT != NULL) 
            pMT = pMT->GetCanonicalMethodTable();
        return pMT;
    }
}

// Obtain instantiation from an instantiated type or a pointer to the
// element type of an array or pointer type                     
TypeHandle* TypeHandle::GetClassOrArrayInstantiation() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsTypeDesc())
    {
        return AsTypeDesc()->GetClassOrArrayInstantiation();
    }
    else if (IsArrayType())
    {
        ArrayClass * pArrayClass = (ArrayClass *)AsMethodTable()->GetClass();
        return pArrayClass->GetInstantiation();
    }
    else
    {
        return GetInstantiation();
    }
}

TypeHandle* TypeHandle::GetInstantiationOfParentClass(MethodTable *pWhichParent) const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    return GetMethodTable()->GetInstantiationOfParentClass(pWhichParent);
}

// Obtain element type from an array or pointer type            
TypeHandle TypeHandle::GetTypeParam() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

  if (IsTypeDesc())
    return AsTypeDesc()->GetTypeParam();
  else 
    return TypeHandle();
}

#ifndef DACCESS_COMPILE

// If you modify this you must also modify TypeHandle::GetTypeArgumentSharingInfo
/* static */

TypeHandle ClassLoader::LoadCanonicalGenericArg(TypeHandle thGenericArg,
                                                LoadTypesFlag fLoadTypes,
                                                ClassLoadLevel level)
{

  CONTRACT(TypeHandle)
  {
      THROWS;
      GC_TRIGGERS;
      POSTCONDITION(CheckPointer(RETVAL, ((fLoadTypes == LoadTypes) ? NULL_NOT_OK : NULL_OK)));
      POSTCONDITION(RETVAL.IsNull() || RETVAL.CheckLoadLevel(level));
      STATIC_CONTRACT_SO_TOLERANT;
  }
  CONTRACT_END

#if defined(FEATURE_SHARE_GENERIC_CODE)  
    if (!g_pConfig->ShareGenericCode())
        RETURN(thGenericArg);

    CorElementType et = thGenericArg.GetSignatureCorElementType();

    if (CorTypeInfo::IsObjRef(et) || CorTypeInfo::IsObjRef(thGenericArg.GetInternalCorElementType()) )
        RETURN(TypeHandle(g_pHiddenMethodTableClass));

    if (CorTypeInfo::IsGenericVariable(et))
        RETURN(thGenericArg);

    if (et == ELEMENT_TYPE_VALUETYPE)
    {
        // Don't share structs. But sharability must be propagated through 
        // them (i.e. struct<object> * shares with struct<string> *)

        if (thGenericArg.HasInstantiation())
        {
            TypeHandle* inst  = thGenericArg.GetInstantiation();


            S_SIZE_T allocSize = S_SIZE_T(thGenericArg.GetNumGenericArgs()) * S_SIZE_T(sizeof(TypeHandle));
            if (allocSize.IsOverflow())
            {
                ThrowHR(COR_E_OVERFLOW);
            }

            CQuickBytesSpecifySize<16> cqbss;

            TypeHandle *repInst = (TypeHandle*) cqbss.AllocThrows(allocSize.Value());
            for (DWORD i = 0; i < thGenericArg.GetNumGenericArgs(); i++)
            {
                repInst[i] = ClassLoader::LoadCanonicalGenericArg(inst[i], fLoadTypes, level);
                if (repInst[i].IsNull())
                    return TypeHandle();
            }
            RETURN(ClassLoader::LoadGenericInstantiationThrowing(thGenericArg.GetModule(), thGenericArg.GetCl(), thGenericArg.GetNumGenericArgs(), repInst, 
                                                                 fLoadTypes,
                                                                 level));
        }
        else 
            RETURN(thGenericArg);
    }

    _ASSERTE(et != ELEMENT_TYPE_PTR && et != ELEMENT_TYPE_FNPTR);
    RETURN(thGenericArg);
#else
    RETURN (thGenericArg);
#endif // FEATURE_SHARE_GENERIC_CODE
}

TypeHandle TypeHandle::LoadCanonicalGenericArg() const
{

    STATIC_CONTRACT_WRAPPER;
    STATIC_CONTRACT_SO_TOLERANT;
    return ClassLoader::LoadCanonicalGenericArg(*this, ClassLoader::LoadTypes);
}


TypeHandle TypeHandle::Instantiate(TypeHandle* inst, INT32 cInst) const
{
    STATIC_CONTRACT_WRAPPER;
    return ClassLoader::LoadGenericInstantiationThrowing(GetModule(), GetCl(), cInst, inst);
}

TypeHandle TypeHandle::MakePointer() const
{ 
    STATIC_CONTRACT_WRAPPER;
    return ClassLoader::LoadPointerOrByrefTypeThrowing(ELEMENT_TYPE_PTR, *this);
}

TypeHandle TypeHandle::MakeByRef() const
{
    STATIC_CONTRACT_WRAPPER;
    return ClassLoader::LoadPointerOrByrefTypeThrowing(ELEMENT_TYPE_BYREF, *this);
}

TypeHandle TypeHandle::MakeSZArray() const
{
    STATIC_CONTRACT_WRAPPER;
    return ClassLoader::LoadArrayTypeThrowing(*this);
}

TypeHandle TypeHandle::MakeArray(int rank) const
{
    STATIC_CONTRACT_WRAPPER;
    return ClassLoader::LoadArrayTypeThrowing(*this, ELEMENT_TYPE_ARRAY, rank);
}

#endif // #ifndef DACCESS_COMPILE

Module* TypeHandle::GetLoaderModule() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsTypeDesc())
        return AsTypeDesc()->GetLoaderModule();
    else
        return AsMethodTable()->GetLoaderModule();   
}

// Is this type the principal owner of its EEClass?
// This is true for 
// - ordinary classes
// - canonical instantiated types (e.g. List<object> but not List<string>)
// - array types that are the principal owner of their template method table (e.g. object[] and int[] but not string[])
// Otherwise false
BOOL TypeHandle::OwnsEEClass() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsTypeDesc())
    {
        if (IsArray())
        {
            ArrayTypeDesc *pTD = AsArray();
            return pTD->OwnsTemplateMethodTable();
        }
        else
            return FALSE;
    }
    else
        return GetMethodTable()->IsCanonicalMethodTable();
}

// Is the type always visible from pContainingModule. ie. Is it fully
// defined within pContainingModule and its dependencies
// For eg. System.Collections.Generics.List<System.String> is fully defined
// within mscorlib.dll, but List<foo> is not (where foo is not defined
// in mscorlib.dll)
// The result is conservative. ie. It may return FALSE even though the
// type may actually be scoped by pContainingModule.
    
BOOL TypeHandle::IsScopedByModule(Module * pContainingModule) const
{
    WRAPPER_CONTRACT;
    
    
    if (GetModule() != pContainingModule)
        return FALSE;

    if (IsTypeDesc())
    {
        TypeDesc *desc = AsTypeDesc();
        if (desc->HasTypeParam())
            return desc->GetTypeParam().IsScopedByModule(pContainingModule);
        else
            return FALSE; // Ignore FnPtrTypeDesc, ELEMENT_TYPE_VAR, ELEMENT_TYPE_MVAR, etc. for now
    }
    else 
    {
        MethodTable * pMT = AsMethodTable();
        if (pMT->IsArray())
        {
            return pMT->GetApproxArrayElementTypeHandle().IsScopedByModule(pContainingModule);
        }
    }

    // We need no further check for non-generic types
    if (!HasInstantiation())
        return TRUE;

    MethodTable * pMT = AsMethodTable();
    TypeHandle * pInstantiation = pMT->GetInstantiation();
    for (DWORD i = 0; i < pMT->GetNumGenericArgs(); i++)
    {
        _ASSERTE(!pInstantiation[i].IsEncodedFixup());
        if (!pInstantiation[i].IsScopedByModule(pContainingModule))
            return FALSE;
    }

    return TRUE;
}


Module* TypeHandle::GetZapModule() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsTypeDesc())
        return AsTypeDesc()->GetZapModule();
    else
        return AsMethodTable()->GetZapModule();   
}

BaseDomain* TypeHandle::GetDomain() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsTypeDesc())
        return AsTypeDesc()->GetDomain();
    else
        return AsMethodTable()->GetDomain();
    
}


// Given the current ShareGenericCode setting, is the specified type 
// representation-sharable as a type parameter to a generic type or method ?
/* static */ TypeHandle::GenericSharingInfo TypeHandle::GetInstantiationSharingInfo(DWORD ntypars, TypeHandle *inst) 
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

#if defined(FEATURE_SHARE_GENERIC_CODE)  
    if (!g_pConfig->ShareGenericCode())
        return NonShared;

    GenericSharingInfo info = NonShared;
    for (DWORD i = 0; i < ntypars; i++)
    {
        GenericSharingInfo argInfo = inst[i].GetTypeArgumentSharingInfo();
        if (argInfo == NonCanonicalShared)
            return argInfo;
        if (argInfo == CanonicalShared)
            info = CanonicalShared;
    }
    return info;
#else
    return NonShared;
#endif // FEATURE_SHARE_GENERIC_CODE
}


TypeHandle::GenericSharingInfo TypeHandle::GetTypeArgumentSharingInfoBaseCase() const
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

#if defined(FEATURE_SHARE_GENERIC_CODE)  
    if (!g_pConfig->ShareGenericCode())
        return NonShared;

    // System.Object is the canonical for all reference types
    if (GetMethodTable() == g_pHiddenMethodTableClass)
        return CanonicalShared;

    // All other reference types share
    CorElementType t = GetSignatureCorElementType();
    if (CorTypeInfo::IsObjRef(t) || CorTypeInfo::IsObjRef(GetInternalCorElementType()))
        return NonCanonicalShared;

    // Otherwise assume non-shared; but GetTypeArgumentSharingInfo will deal with value types and param types
#endif // FEATURE_SHARE_GENERIC_CODE
    return NonShared;
}

// Given the current ShareGenericCode setting, 
// is the specified type representation-sharable as a type 
// parameter to a generic type or method ?
//
// If you modify this please make sure you modify ClassLoader::LoadCanonicalGenericArg
TypeHandle::GenericSharingInfo TypeHandle::GetTypeArgumentSharingInfo() const
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

#if defined(FEATURE_SHARE_GENERIC_CODE)  
    if (!g_pConfig->ShareGenericCode())
        return NonShared;

    GenericSharingInfo info = GetTypeArgumentSharingInfoBaseCase();
    if (info != NonShared)
        return info;

    // Note that generic variables do not share
    CorElementType t = GetSignatureCorElementType();
    switch(t)
    {
    case ELEMENT_TYPE_VALUETYPE :
        // Instantiated generic value types share through their arguments
        if (HasInstantiation())
        {
            return GetInstantiationSharingInfo(GetNumGenericArgs(),GetInstantiation());
        }
        break;

        // Pointer types and generic structs are parametric in other types
        // so sharability must be "preserved" through it
    case ELEMENT_TYPE_PTR :
    case ELEMENT_TYPE_FNPTR :
        return GetTypeParam().GetTypeArgumentSharingInfo();

    default :
        return NonShared;
    }

#endif // FEATURE_SHARE_GENERIC_CODE
    return NonShared;
}


// Obtain instantiation from an instantiated type.                     
// Return NULL if it's not one.                                         
TypeHandle* TypeHandle::GetInstantiation() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;
    
  if (!IsTypeDesc()) return AsMethodTable()->GetInstantiation();
  else return NULL;
}


// Helper functions that just delegate through the method table
BOOL TypeHandle::IsValueType()  const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    return !IsTypeDesc() && AsMethodTable()->IsValueType();
}

BOOL TypeHandle::IsInterface() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    return !IsTypeDesc() && AsMethodTable()->IsInterface(); 
}

BOOL TypeHandle::IsAbstract() const
{
    WRAPPER_CONTRACT;
    PREFIX_ASSUME(GetMethodTable() != NULL);
    return GetMethodTable()->IsAbstract();
}

DWORD TypeHandle::IsThunking() const
{
    WRAPPER_CONTRACT;
    return IsUnsharedMT() &&  GetMethodTable()->IsThunking();
}


WORD TypeHandle::GetNumVirtuals() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    PREFIX_ASSUME(GetMethodTable() != NULL);
    return GetMethodTable()->GetNumVirtuals(); 
}

#ifndef DACCESS_COMPILE

BOOL TypeHandle::IsBlittable() const
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END

    if (IsUnsharedMT())
    {
        // This is a simple type (not an array, ptr or byref) so if 
        // simply check to see if the type is blittable.
        return AsMethodTable()->IsBlittable();
    }
    else if (IsArray())
    {
        // Single dimentional array's of blittable types are also blittable.
        if (AsArray()->GetRank() == 1)
        {
            if (AsArray()->GetArrayElementTypeHandle().IsBlittable())
                return TRUE;
        }
    }

    return FALSE;
}

BOOL TypeHandle::HasLayout() const
{
    WRAPPER_CONTRACT;
    MethodTable *pMT = GetMethodTable();
    return pMT ? pMT->HasLayout() : FALSE;
}


//--------------------------------------------------------------------------------------
// CanCastTo is necessary but not sufficient, as it assumes that any valuetype
// involved is in its boxed form.

BOOL TypeHandle::IsBoxedAndCanCastTo(TypeHandle type) const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        SO_TOLERANT;
        // The caller should check for an exact match.
        // That will cover the cast of a (unboxed) valuetype to itself.
        PRECONDITION(*this != type);
    }
    CONTRACTL_END


    CorElementType fromParamCorType = GetVerifierCorElementType();

    if (CorTypeInfo::IsObjRef(fromParamCorType))
    {
        // fromParamCorType is a reference type. We can just use CanCastTo
        return CanCastTo(type);
    }
    else if (CorTypeInfo::IsGenericVariable(fromParamCorType))
    {
        TypeVarTypeDesc* varFromParam = AsGenericVariable();
            
        if (!varFromParam->ConstraintsLoaded())
            varFromParam->LoadConstraints(CLASS_DEPENDENCIES_LOADED);

        // A generic type parameter cannot be compatible with another type
        // as it could be substitued with a valuetype. However, if it is
        // constrained to a reference type, then we can use CanCastTo.
        if (varFromParam->ConstrainedAsObjRef())
            return CanCastTo(type);
    }

    return FALSE;
}

//--------------------------------------------------------------------------------------
// CanCastTo is necessary but not sufficient, as it assumes that any valuetype
// involved is in its boxed form. See IsBoxedAndCanCastTo() if the valuetype
// is not guaranteed to be in its boxed form.

BOOL TypeHandle::CanCastTo(TypeHandle type)  const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        SO_TOLERANT;
    }
    CONTRACTL_END

    if (*this == type)
        return(true);

    if (IsTypeDesc())
        return AsTypeDesc()->CanCastTo(type);
                
    if (type.IsTypeDesc())
        return(false);

    if (AsMethodTable()->IsThunking())
        return (false);
        
    return AsMethodTable()->CanCastToClassOrInterface(type.AsMethodTable());
}

TypeHandle::CastResult TypeHandle::CanCastToNoGC(TypeHandle type)  const
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

    if (*this == type)
        return(CanCast);

    if (IsTypeDesc())
        return AsTypeDesc()->CanCastToNoGC(type);
                
    if (type.IsTypeDesc())
        return(CannotCast);

    if (AsMethodTable()->IsThunking())
        return (CannotCast);
        
    return AsMethodTable()->CanCastToClassOrInterfaceNoGC(type.AsMethodTable());
}

#endif // #ifndef DACCESS_COMPILE

void TypeHandle::GetName(SString &result) const
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    TypeHandle *inst = NULL;

    INTERIOR_STACK_PROBE_NOTHROW_CHECK_THREAD(goto Exit;);

    if (IsTypeDesc())
    {
        AsTypeDesc()->GetName(result);
        goto Exit;
    }

    AsMethodTable()->GetClass()->_GetFullyQualifiedNameForClass(result);

    // Tack the instantiation on the end
    inst = GetInstantiation();
    if (inst != NULL)
        TypeString::AppendInst(result, GetNumGenericArgs(), inst);
    
Exit:
    ;
    END_INTERIOR_STACK_PROBE;
}

TypeHandle TypeHandle::GetPossiblyEncodedParent()  const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsTypeDesc())
        return(AsTypeDesc()->GetParent());
    else
        return TypeHandle(AsMethodTable()->GetPossiblyEncodedParentMethodTable());
}

TypeHandle TypeHandle::GetParent()  const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsTypeDesc())
        return(AsTypeDesc()->GetParent());
    else
        return TypeHandle(AsMethodTable()->GetParentMethodTable());
}
#ifndef DACCESS_COMPILE

/* static */
TypeHandle TypeHandle::MergeClassWithInterface(TypeHandle tClass, TypeHandle tInterface)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    MethodTable *pMTClass = tClass.AsMethodTable(); 

    // Check if the class implements the interface
    MethodTable::InterfaceMapIterator classIt = pMTClass->IterateInterfaceMap();
    while (classIt.Next())
    {
        if (classIt.InterfaceEquals(tInterface.GetMethodTable()))
        {
            // The class implements the interface, so our merged state should be the interface
            return tInterface;
        }
    }

    // Check if the class and the interface implement a common interface
    MethodTable *pMTInterface = tInterface.AsMethodTable();
    MethodTable::InterfaceMapIterator intIt = pMTInterface->IterateInterfaceMap();
    while (intIt.Next())
    {
        classIt = pMTClass->IterateInterfaceMap();
        MethodTable *pMT = intIt.GetInterface();
        while (classIt.Next())
        {
            if (classIt.InterfaceEquals(pMT))
            {
                // Found a common interface.  If there are multiple common interfaces, then
                // the problem is ambiguous so we'll just take the first one--it's the best
                // we can do.  If an ensuing code path relies on another common interface,
                // the verifier will think the code is unverifiable, but it would require a
                // major redesign of the verifier to fix that.
                return TypeHandle(pMT);
            }
        }
    }

    // No compatible merge found - using Object
    return TypeHandle(g_pObjectClass);
}

/* static */
TypeHandle TypeHandle::MergeTypeHandlesToCommonParent(TypeHandle ta, TypeHandle tb)
{
    CONTRACTL
    {
      THROWS;
      GC_TRIGGERS;
      INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    _ASSERTE(!ta.IsNull() && !tb.IsNull());

    if (ta == tb)
        return ta;

    // Handle the array case
    if (ta.IsArray()) 
    {
        if (tb.IsArray())
            return MergeArrayTypeHandlesToCommonParent(ta, tb);
        ta = TypeHandle(g_pArrayClass);         // keep merging from here. 
    }
    else if (tb.IsArray())
        tb = TypeHandle(g_pArrayClass);


    // If either is a (by assumption boxed) type variable 
    // return the supertype, if they are related, or object if they are incomparable.
    if (ta.IsGenericVariable() || tb.IsGenericVariable())
    {
        if (ta.CanCastTo(tb))
            return tb;
        if (tb.CanCastTo(ta))
            return ta;
        return TypeHandle(g_pObjectClass);
    }


    _ASSERTE(ta.IsUnsharedMT() && tb.IsUnsharedMT());


    MethodTable *pMTa = ta.AsMethodTable(); 
    MethodTable *pMTb = tb.AsMethodTable();

    if (pMTb->IsInterface())
    {

        if (pMTa->IsInterface())
        {
            //
            // Both classes are interfaces.  Check that if one 
            // interface extends the other.
            //
            // Does tb extend ta ?
            //

            {
                MethodTable::InterfaceMapIterator itB = pMTb->IterateInterfaceMap();
                while (itB.Next())
                {
                    if (itB.InterfaceEquals(pMTa))
                    {
                        // tb extends ta, so our merged state should be ta
                        return ta;
                    }
                }
            }

            //
            // Does tb extend ta ?
            //
            {
                MethodTable::InterfaceMapIterator itA = pMTa->IterateInterfaceMap();
                while (itA.Next())
                {
                    if (itA.InterfaceEquals(pMTb))
                    {
                        // ta extends tb, so our merged state should be tb
                        return tb;
                    }
                }
                
            }
InterfaceMerge:
            MethodTable::InterfaceMapIterator itB = pMTb->IterateInterfaceMap();
            while (itB.Next())
            {
                MethodTable::InterfaceMapIterator itA = pMTa->IterateInterfaceMap();
                while (itA.Next())
                {
                    if (itA.GetInterface() == itB.GetInterface())
                    {
                        return TypeHandle(itA.GetInterface());
                    }
                }
            }

        
            // No compatible merge found - using Object
            return TypeHandle(g_pObjectClass);
        }
        else
            return MergeClassWithInterface(ta, tb);
    }
    else if (pMTa->IsInterface())
        return MergeClassWithInterface(tb, ta);

    DWORD   aDepth = 0;
    DWORD   bDepth = 0;
    TypeHandle tSearch;

    // find the depth in the class hierarchy for each class
    for (tSearch = ta; (!tSearch.IsNull()); tSearch = tSearch.GetParent())
        aDepth++;

    for (tSearch = tb; (!tSearch.IsNull()); tSearch = tSearch.GetParent())
        bDepth++;
    
    // for whichever class is lower down in the hierarchy, walk up the superclass chain
    // to the same level as the other class
    while (aDepth > bDepth)
    {
        ta = ta.GetParent();
        aDepth--;
    }

    while (bDepth > aDepth)
    {
        tb = tb.GetParent();
        bDepth--;
    }

    while (ta != tb)
    {
        ta = ta.GetParent();
        tb = tb.GetParent();
    }

    if (ta == TypeHandle(g_pObjectClass))
    {
        goto InterfaceMerge;
    }

    // If no compatible merge is found, we end up using Object

    _ASSERTE(!ta.IsNull());

    return ta;
}

/* static */
TypeHandle TypeHandle::MergeArrayTypeHandlesToCommonParent(TypeHandle ta, TypeHandle tb)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    CorElementType taKind = ta.GetInternalCorElementType();
    CorElementType tbKind = tb.GetInternalCorElementType();
    _ASSERTE(CorTypeInfo::IsArray(taKind) && CorTypeInfo::IsArray(tbKind));

    TypeHandle taElem;
    TypeHandle tMergeElem;

    // If they match we are good to go.
    if (ta == tb)
        return ta;

    if (ta == TypeHandle(g_pArrayClass))
        return ta;
    else if (tb == TypeHandle(g_pArrayClass))
        return tb;

    // Get the rank and kind of the first array
    DWORD rank = ta.AsArray()->GetRank();
    CorElementType mergeKind = taKind;

    // if no match on the rank the common ancestor is System.Array
    if (rank != tb.AsArray()->GetRank())
        return TypeHandle(g_pArrayClass);

    if (tbKind != taKind)
    {        
        if (CorTypeInfo::IsArray(tbKind) && 
            CorTypeInfo::IsArray(taKind) && rank == 1)
            mergeKind = ELEMENT_TYPE_ARRAY;
        else
        return TypeHandle(g_pArrayClass);
    }

    // If both are arrays of reference types, return an array of the common
    // ancestor.
    taElem = ta.AsArray()->GetArrayElementTypeHandle();
    if (taElem == tb.AsArray()->GetArrayElementTypeHandle())
    {
        // The element types match, so we are good to go.
        tMergeElem = taElem;
    }
    else if (taElem.IsArray() && tb.AsArray()->GetArrayElementTypeHandle().IsArray())
    {
        // Arrays - Find the common ancestor of the element types.
        tMergeElem = MergeArrayTypeHandlesToCommonParent(taElem, tb.AsArray()->GetArrayElementTypeHandle());
    }
    else if (CorTypeInfo::IsObjRef(taElem.GetSignatureCorElementType()) &&
            CorTypeInfo::IsObjRef(tb.AsArray()->GetArrayElementTypeHandle().GetSignatureCorElementType()))
    {
        // Find the common ancestor of the element types.
        tMergeElem = MergeTypeHandlesToCommonParent(taElem, tb.AsArray()->GetArrayElementTypeHandle());
    }
    else
    {
        // The element types have nothing in common.
        return TypeHandle(g_pArrayClass);
    }    


    {
        // This should just result in resolving an already loaded type.
        ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();    
        // == FailIfNotLoadedOrNotRestored
        TypeHandle result = ClassLoader::LoadArrayTypeThrowing(tMergeElem, mergeKind, rank, ClassLoader::DontLoadTypes);  
        _ASSERTE(!result.IsNull());

        return result;
    }
}

#endif // #ifndef DACCESS_COMPILE

BOOL TypeHandle::IsEnum()  const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (!IsTypeDesc() && AsMethodTable()->IsEnum());
}

BOOL TypeHandle::IsFnPtrType() const
{
    WRAPPER_CONTRACT;
    return (IsTypeDesc() && GetSignatureCorElementType() == ELEMENT_TYPE_FNPTR);
}

// Is this type part of an assembly loaded for introspection?
BOOL TypeHandle::IsIntrospectionOnly() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

#ifndef DACCESS_COMPILE
    if (IsFnPtrType())
    {
        // FnPtrs have no Module so have to do this differently
        FnPtrTypeDesc *pFnPtrTypeDesc = AsFnPtrType();
        return pFnPtrTypeDesc->GetRetAndArgTypesPointer()[0].IsIntrospectionOnly();
    }
    else if (HasTypeParam())
    {
        return GetTypeParam().IsIntrospectionOnly();
    }
    else
    {
        return !!(GetModule()->IsIntrospectionOnly());
    }
#else
    return FALSE;
#endif
}

// Is this type part of an assembly loaded for introspection?
BOOL TypeKey::IsIntrospectionOnly()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

#ifndef DACCESS_COMPILE
    switch (m_kind)
    {
        case ELEMENT_TYPE_CLASS:
            return !!(u.asClass.m_pModule->IsIntrospectionOnly());

        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_SZARRAY:
        case ELEMENT_TYPE_PTR:
        case ELEMENT_TYPE_BYREF:
            return !!(TypeHandle::FromPtr(u.asParamType.m_paramType).IsIntrospectionOnly());

        case ELEMENT_TYPE_FNPTR:
            return !!(u.asFnPtr.m_pRetAndArgTypes[0].IsIntrospectionOnly());

        default:
            UNREACHABLE_MSG("Corrupted typekey");
    }
#else
    return FALSE;
#endif
}



EEClass* TypeHandle::GetClass() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    MethodTable* pMT = GetMethodTable();
    return(pMT ? pMT->GetClass() : 0);
}

BOOL TypeHandle::IsRestored() const
{ 
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (!IsTypeDesc())
    {
        return GetMethodTable()->IsRestored();
    }
    else
    {
        return AsTypeDesc()->IsRestored();
    }
}

BOOL TypeHandle::IsEncodedFixup() const
{
    STATIC_CONTRACT_SO_TOLERANT;
    return CORCOMPILE_IS_TOKEN_TAGGED(m_asTAddr);
}

BOOL TypeHandle::HasUnrestoredTypeKey()  const
{
    WRAPPER_CONTRACT;

    if (IsTypeDesc())
        return AsTypeDesc()->HasUnrestoredTypeKey();
    else
        return AsMethodTable()->HasUnrestoredTypeKey();
}


void TypeHandle::CheckRestore() const
{ 
    CONTRACTL
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        SO_TOLERANT;
        PRECONDITION(!IsEncodedFixup());
    }
    CONTRACTL_END

    if (!IsRestored())
    {
        if (!IsTypeDesc())
        {
            GetMethodTable()->CheckRestore();
        }
        else
        {
            ClassLoader::EnsureLoaded(*this);
        }
    }

    _ASSERTE(IsRestored());
}


#ifndef DACCESS_COMPILE

BOOL TypeHandle::IsExternallyVisible() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsGenericVariable())
        return TRUE;

    TypeHandle elementType = GetElementType();
    _ASSERTE(!elementType.IsNull());

    if (elementType.IsGenericVariable())
        return TRUE;

    _ASSERTE(elementType.IsUnsharedMT());
    return elementType.AsMethodTable()->IsExternallyVisible();
}

OBJECTREF TypeHandle::GetManagedClassObject() const
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;

        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    if (IsUnsharedMT())
    {
        if (AsMethodTable()->IsTransparentProxyType())
            return NULL;
        else
            return AsMethodTable()->GetManagedClassObject();
    }
    else
    {
        switch(GetInternalCorElementType()) {
        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_SZARRAY:
        case ELEMENT_TYPE_BYREF:
        case ELEMENT_TYPE_PTR:
            return ((ParamTypeDesc*)AsTypeDesc())->GetManagedClassObject();
            
        case ELEMENT_TYPE_VAR:
        case ELEMENT_TYPE_MVAR:
            return ((TypeVarTypeDesc*)AsTypeDesc())->GetManagedClassObject();
            
            // for this release a function pointer is mapped into an IntPtr. This result in a loss of information. Fix next release
        case ELEMENT_TYPE_FNPTR:
            return TheIntPtrClass()->GetManagedClassObject();
            
        default:
        _ASSERTE(!"Bad Element Type");
        return NULL;
        }
    }
}


OBJECTREF TypeHandle::GetManagedClassObjectIfExists() const
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;

        FORBID_FAULT;
    }
    CONTRACTL_END;

    OBJECTREF o = NULL;

    g_IBCLogger.LogTypeMethodTableAccess(this);
    if (IsUnsharedMT()) {
        o = AsMethodTable()->GetManagedClassObjectIfExists();
    }
    else {
    switch(GetInternalCorElementType()) {
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:
    case ELEMENT_TYPE_BYREF:
    case ELEMENT_TYPE_PTR:
        o = ((ParamTypeDesc*)AsTypeDesc())->GetManagedClassObjectIfExists();
        break;

    case ELEMENT_TYPE_VAR:
    case ELEMENT_TYPE_MVAR:
        o = ((TypeVarTypeDesc*)AsTypeDesc())->GetManagedClassObjectIfExists();
        break;

    // for this release a function pointer is mapped into an IntPtr. This result in a loss of information. Fix next release
    case ELEMENT_TYPE_FNPTR:
        // because TheFnPtrClass() can throw we return NULL for now. That is not a major deal because it just means we will
        // not take advantage of this optimization, but the case is rather rare. 
        //o = TheFnPtrClass()->GetManagedClassObjectIfExists();
        break;

    default:
        o = AsMethodTable()->GetManagedClassObjectIfExists();
        break;
        }
    }
    
    return o;
}

#endif // #ifndef DACCESS_COMPILE

#if CHECK_APP_DOMAIN_LEAKS

BOOL TypeHandle::IsAppDomainAgile() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsUnsharedMT())
    {
        MethodTable *pMT = AsMethodTable();
        return pMT->GetClass()->IsAppDomainAgile();
    }
    else if (IsArray())
    {
        TypeHandle th = AsArray()->GetArrayElementTypeHandle();
        return th.IsArrayOfElementsAppDomainAgile();
    }
    else
    {
        return FALSE;
    }
}

BOOL TypeHandle::IsCheckAppDomainAgile() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsUnsharedMT())
    {
        MethodTable *pMT = AsMethodTable();
        return pMT->GetClass()->IsCheckAppDomainAgile();
    }
    else if (IsArray())
    {
        TypeHandle th = AsArray()->GetArrayElementTypeHandle();  
        return th.IsArrayOfElementsCheckAppDomainAgile();
    }
    else
    {
        return FALSE;
    }
}

BOOL TypeHandle::IsArrayOfElementsAppDomainAgile() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsUnsharedMT())
    {
        MethodTable *pMT = AsMethodTable();
        return (pMT->GetClass()->IsSealed()) && pMT->GetClass()->IsAppDomainAgile();
    }
    else
    {
        // I'm not sure how to prove a typedesc is sealed, so
        // just bail and return FALSE here rather than recursing.

        return FALSE;
    }
}

BOOL TypeHandle::IsArrayOfElementsCheckAppDomainAgile() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsUnsharedMT())
    {
        MethodTable *pMT = AsMethodTable();
        return (pMT->GetClass()->IsAppDomainAgile()
                && (pMT->GetClass()->IsSealed()) == 0)
          || pMT->GetClass()->IsCheckAppDomainAgile();
    }
    else
    {
        // I'm not sure how to prove a typedesc is sealed, so
        // just bail and return FALSE here rather than recursing.

        return FALSE;
    }
}
#endif

BOOL TypeHandle::IsByRef()  const
{ 
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    return(IsTypeDesc() && AsTypeDesc()->IsByRef());

}

BOOL TypeHandle::IsPointer()  const
{ 
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    return(IsTypeDesc() && AsTypeDesc()->IsPointer());

}

CorElementType TypeHandle::GetInternalCorElementType()  const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsUnsharedMT())
        return AsMethodTable()->GetInternalCorElementType();
    else
        return AsTypeDesc()->GetInternalCorElementType();
 }

BOOL TypeHandle::IsDomainNeutral() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsUnsharedMT()) 
        return GetMethodTable()->IsDomainNeutral();
    return (AsTypeDesc())->IsDomainNeutral();
}

BOOL TypeHandle::HasInstantiation()  const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (IsTypeDesc()) return false;
    if (IsNull()) return false;
    return AsMethodTable()->HasInstantiation();
}

ClassLoadLevel TypeHandle::GetLoadLevel()    
{
    LEAF_CONTRACT;

    if (IsTypeDesc())
    {
        return AsTypeDesc()->GetLoadLevel();
    }
    else
    {
        return AsMethodTable()->GetLoadLevel();
    }
}

BOOL TypeHandle::IsFullyLoaded()
{
    LEAF_CONTRACT;

    if (IsTypeDesc())
    {
        return AsTypeDesc()->IsFullyLoaded();
    }
    else
    {
        return AsMethodTable()->IsFullyLoaded();
    }
}

void TypeHandle::DoFullyLoad(TypeHandleList *pVisited, ClassLoadLevel level, DFLPendingList *pPending, BOOL *pfBailed)
{
    WRAPPER_CONTRACT;

    _ASSERTE(level == CLASS_LOADED || level == CLASS_DEPENDENCIES_LOADED);
    _ASSERTE(pfBailed != NULL);
    _ASSERTE(!(level == CLASS_LOADED && pPending == NULL));


    if (IsTypeDesc())
    {
        return AsTypeDesc()->DoFullyLoad(pVisited, level, pPending, pfBailed);
    }
    else
    {
        return AsMethodTable()->DoFullyLoad(pVisited, level, pPending, pfBailed);
    }
}


CorElementType TypeHandle::GetSignatureCorElementType() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

/*
    // The following naive implementation is inlined to avoid redundant calls to MethodTable::IsEnum
    if (IsEnum())
        return(ELEMENT_TYPE_VALUETYPE);
 
    return GetVerifierCorElementType();
*/


    if (IsUnsharedMT())
    {
        MethodTable *pMT = AsMethodTable();
        // Check the (new) use of MethodTable::IsValueType is the same as the
        // old IsValueClass
        _ASSERTE((pMT->GetClass()->IsValueClass() == 0) == (AsMethodTable()->IsValueType() == 0));

        // Undo the normalization of the element type stored in the method table
        if ((pMT->IsValueType() && !pMT->IsTruePrimitive()) || pMT->IsEnum())
        {
            return(ELEMENT_TYPE_VALUETYPE);
        }
        else
        {
            return pMT->GetInternalCorElementType();
        }
    }
    else
    {
        return AsTypeDesc()->GetInternalCorElementType();
    }
}

CorElementType TypeHandle::GetVerifierCorElementType() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;
    // This gets used by 
    //     MethodTable::Restore() --> 
    //     Pending::Pending(...) -->
    //     TypeHandle::GetSignatureCorElementType() -->
    //     TypeHandle::GetVerifierCorElementType() -->
    // early on during the process of restoring, i.e. after the EEClass for the 
    // MT is restored but not the parent method table.  Thus we cannot 
    // assume that the parent method table is even yet a valid pointer.
    // However both MethodTable::GetClass and MethodTable::IsValueType work
    // even if the parent method table pointer has not been restored.


    if (IsUnsharedMT())
    {
        MethodTable *pMT = AsMethodTable();
        // Check the (new) use of MethodTable::IsValueType is the same as the
        // old IsValueClass
        _ASSERTE((pMT->GetClass()->IsValueClass() == 0) == (AsMethodTable()->IsValueType() == 0));

        // Undo the normalization of the element type stored in the method table
        if ((pMT->IsValueType() && !pMT->IsTruePrimitive()) && !pMT->IsEnum())
        {
            return(ELEMENT_TYPE_VALUETYPE);
        }
        else
        {
            return pMT->GetInternalCorElementType();
        }
    }
    else
    {
        return AsTypeDesc()->GetInternalCorElementType();
    }
}


#ifdef DACCESS_COMPILE

FnPtrTypeDesc*
TypeHandle::AsFnPtrType()
{ 
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
  
    _ASSERTE(IsFnPtrType());

    FnPtrTypeDesc* result = PTR_FnPtrTypeDesc(m_asTAddr & ~2);
    PREFIX_ASSUME(result != NULL);
    return result;
}

TypeVarTypeDesc*
TypeHandle::AsGenericVariable() const
{ 
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    _ASSERTE(IsGenericVariable() && IsTypeDesc());

    TypeVarTypeDesc* result = PTR_TypeVarTypeDesc(m_asTAddr & ~2);
    PREFIX_ASSUME(result != NULL);
    return result;
}

void
TypeHandle::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    if (!m_asTAddr)
    {
        return;
    }
    
    PAL_TRY
    {
        if (IsArray())
        {
            AsArray()->EnumMemoryRegions(flags);
        }
        else if (IsGenericVariable())
        {
            AsGenericVariable()->EnumMemoryRegions(flags);
        }
        else if (IsFnPtrType())
        {
            AsFnPtrType()->EnumMemoryRegions(flags);
        }
        else if (IsTypeDesc())
        {
            DacEnumMemoryRegion(m_asTAddr & ~2, sizeof(TypeDesc));
        }
        else
        {
            GetMethodTable()->EnumMemoryRegions(flags);
        }
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
    }
    PAL_ENDTRY
}

#endif



//
//
//
//
//
//
// 
BOOL SatisfiesClassConstraints(TypeHandle instanceTypeHnd, TypeHandle typicalTypeHnd)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    GCX_COOP();

#ifndef DACCESS_COMPILE 

    TypeHandle* formalParams = typicalTypeHnd.GetInstantiation();
    TypeHandle* actualParams = instanceTypeHnd.GetInstantiation();
    DWORD numGenericArgs = typicalTypeHnd.GetNumGenericArgs();
    _ASSERTE(numGenericArgs == instanceTypeHnd.GetNumGenericArgs());

    for (DWORD i = 0; i < numGenericArgs; i++)
    {
        SigTypeContext typeContext;
        SigTypeContext::InitTypeContext(instanceTypeHnd, &typeContext);

        BOOL bSatisfiesConstraints = formalParams[i].AsGenericVariable()->SatisfiesConstraints(&typeContext, actualParams[i]);

        if (!bSatisfiesConstraints)
        {
            // If we got here, it's because the constraints weren't satisfied. Most of the time, we will throw at this point.
            // But there is one internal group of types that are exempted: the instantiation we use to generate the canonical
            // method bodies has to be exempted because it's the prototype for all instantiations. Because the methodtable
            // we use for the canonical methodtable isn't available outside of mscorlib, no user code can instantiate with it.
            BOOL fExempt = FALSE;

            TypeHandle* inst = instanceTypeHnd.GetInstantiation();
            for (DWORD arg = 0; arg < numGenericArgs; arg++)
            {
                if (inst[arg].GetTypeArgumentSharingInfo() == TypeHandle::CanonicalShared)
                {
                    fExempt = TRUE;
                    break;
                }
            }

            if (!fExempt)
            {
                SString argNum;
                argNum.Printf("%d", i);

                SString typicalTypeHndName;
                TypeString::AppendType(typicalTypeHndName, typicalTypeHnd);

                SString actualParamName;
                TypeString::AppendType(actualParamName, actualParams[i]);

                SString formalParamName;
                TypeString::AppendType(formalParamName, formalParams[i]);

                COMPlusThrow(kTypeLoadException,
                             IDS_EE_CLASS_CONSTRAINTS_VIOLATION,
                             argNum,
                             actualParamName,
                             typicalTypeHndName,
                             formalParamName
                            );

            }

            return FALSE;
        }
        

    }

    return TRUE;
  
#else
    return TRUE;
#endif
}




#ifndef DACCESS_COMPILE
BOOL TypeHandle::SatisfiesClassConstraints() const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_INTOLERANT;

        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;
    
    BOOL returnValue = FALSE;
    TypeHandle* classInst;
    TypeHandle thCanonical;
    TypeHandle* typicalInst;
    SigTypeContext typeContext;
    TypeHandle thParent;
    DWORD numGenericArgs = 0;
    
    INTERIOR_STACK_PROBE_CHECK_THREAD;

    
    thParent = GetParent();
   
    if (!thParent.IsNull() && !thParent.SatisfiesClassConstraints()) 
    {
        returnValue = FALSE;
        goto Exit;
    }
    
    if (!HasInstantiation()) 
    {
        returnValue = TRUE;
        goto Exit;
    }

    numGenericArgs = GetNumGenericArgs();
    classInst = GetInstantiation(); 
    thCanonical = ClassLoader::LoadTypeDefThrowing(
                                    GetModule(), 
                                    GetCl(),
                                    ClassLoader::ThrowIfNotFound,
                                    ClassLoader::PermitUninstDefOrRef);
    typicalInst = thCanonical.GetInstantiation();

    SigTypeContext::InitTypeContext(*this, &typeContext);
    
    for (DWORD i = 0; i < numGenericArgs; i++)
    {   
        TypeHandle thArg = classInst[i];
        _ASSERTE(!thArg.IsNull());

        TypeVarTypeDesc* tyvar = (TypeVarTypeDesc*) (typicalInst[i].AsTypeDesc());
        _ASSERTE(tyvar != NULL);
        _ASSERTE(TypeFromToken(tyvar->GetTypeOrMethodDef()) == mdtTypeDef);        

        tyvar->LoadConstraints(); //TODO: is this necessary for anything but the typical class?

        if (!tyvar->SatisfiesConstraints(&typeContext,thArg)) 
        {
            returnValue = FALSE;
            goto Exit;
        }

    }    
    returnValue = TRUE;
Exit:    
    ;
    END_INTERIOR_STACK_PROBE;
    
    return returnValue;
}

TypeKey TypeHandle::GetTypeKey() const
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    PRECONDITION(!IsGenericVariable());

    if (IsTypeDesc())
    {
        TypeDesc *pTD = AsTypeDesc();
        CorElementType etype = pTD->GetInternalCorElementType();
        if (CorTypeInfo::IsArray(etype))
        {
            TypeKey tk(etype, pTD->GetTypeParam(), FALSE, pTD->GetMethodTable()->GetRank());
            return tk;
        }
        else if (CorTypeInfo::IsModifier(etype))
        {
            TypeKey tk(etype, pTD->GetTypeParam());
            return tk;
        }
        else
        {
            CONSISTENCY_CHECK(etype == ELEMENT_TYPE_FNPTR);
            FnPtrTypeDesc* pFTD = (FnPtrTypeDesc*) pTD;
            TypeKey tk(pFTD->GetCallConv(), pFTD->GetNumArgs(), pFTD->GetRetAndArgTypesPointer());
            return tk;
        }
    }
    else
    {
        MethodTable *pMT = AsMethodTable();
        if (pMT->IsArray())
        {
            TypeKey tk(pMT->GetInternalCorElementType(), pMT->GetApproxArrayElementTypeHandle(), TRUE, pMT->GetRank());
            return tk;
        }
        else if (pMT->IsTypicalTypeDefinition())
        {
            TypeKey tk(pMT->GetModule(), pMT->GetCl());
            return tk;
        }
        else
        {
            TypeKey tk(pMT->GetModule(), pMT->GetCl(), pMT->GetNumGenericArgs(), pMT->GetInstantiation());
            return tk;
        }
    }
}


#ifdef _DEBUG
// Check that a type handle matches the key provided
CHECK TypeHandle::CheckMatchesKey(TypeKey *pKey) const
{
    LEAF_CONTRACT;
    CONSISTENCY_CHECK(!IsGenericVariable());

    // Check first to avoid creating debug name
    if (!GetTypeKey().Equals(pKey))
    {
        StackSString typeKeyString;
        CONTRACT_VIOLATION(GCViolation|ThrowsViolation);
        TypeString::AppendTypeKeyDebug(typeKeyString, pKey);
        if (!IsTypeDesc() && AsMethodTable()->IsArray())
        {
            MethodTable *pMT = AsMethodTable();
            CHECK_MSGF(pMT->GetInternalCorElementType() == pKey->GetKind(), 
                       ("CorElementType %d of Array MethodTable does not match key %S", pMT->GetArrayElementType(), typeKeyString.GetUnicode()));
            
            CHECK_MSGF(pMT->GetApproxArrayElementTypeHandle() == pKey->GetElementType(), 
                       ("Element type of Array MethodTable does not match key %S",typeKeyString.GetUnicode()));

            CHECK_MSGF(pMT->GetRank() == pKey->GetRank(), 
                       ("Rank %d of Array MethodTable does not match key %S", pMT->GetRank(), typeKeyString.GetUnicode()));
        }
        else
        if (IsTypeDesc())
        {
            TypeDesc *pTD = AsTypeDesc();
            CHECK_MSGF(pTD->GetInternalCorElementType() == pKey->GetKind(), 
                       ("CorElementType %d of TypeDesc does not match key %S", pTD->GetInternalCorElementType(), typeKeyString.GetUnicode()));
            
            if (CorTypeInfo::IsModifier(pKey->GetKind()))
            {
                CHECK_MSGF(pTD->GetTypeParam() == pKey->GetElementType(), 
                           ("Element type of TypeDesc does not match key %S",typeKeyString.GetUnicode()));
            }
            if (CorTypeInfo::IsArray(pKey->GetKind()))
            {
                CHECK_MSGF(pTD->GetMethodTable()->GetRank() == pKey->GetRank(), 
                           ("Rank %d of array TypeDesc does not match key %S", pTD->GetMethodTable()->GetRank(), typeKeyString.GetUnicode()));
            }
        }
        else
        {
            MethodTable *pMT = AsMethodTable();
            CHECK_MSGF(pMT->GetModule() == pKey->GetModule(), ("Module of MethodTable does not match key %S", typeKeyString.GetUnicode()));
            CHECK_MSGF(pMT->GetCl() == pKey->GetTypeToken(), 
                       ("TypeDef %x of Methodtable does not match TypeDef %x of key %S", pMT->GetCl(), pKey->GetTypeToken(),
                        typeKeyString.GetUnicode()));
            
            if (pMT->IsTypicalTypeDefinition())
            {
                CHECK_MSGF(pKey->GetNumGenericArgs() == 0 && !pKey->HasInstantiation(), 
                           ("Key %S for Typical MethodTable has non-zero number of generic arguments", typeKeyString.GetUnicode()));
            }
            else
            {
                CHECK_MSGF(pMT->GetNumGenericArgs() == pKey->GetNumGenericArgs(), 
                           ("Number of generic params %d in MethodTable does not match key %S", pMT->GetNumGenericArgs(), typeKeyString.GetUnicode()));
                if (pKey->HasInstantiation())
                {
                    for (DWORD i = 0; i < pMT->GetNumGenericArgs(); i++)
                    {
                        CHECK_MSGF(pMT->GetInstantiation()[i] == pKey->GetInstantiation()[i],
                               ("Generic argument %d in MethodTable does not match key %S", i, typeKeyString.GetUnicode()));
                    }
                }
            }
        }
    }
    CHECK_OK;
}

const char * const classLoadLevelName[] =
{
    "BEGIN",
    "UNRESTOREDTYPEKEY",
    "UNRESTORED",
    "APPROXPARENTS",
    "EXACTPARENTS",
    "DEPENDENCIES_LOADED",
    "LOADED",
};

// Check that this type is loaded up to the level indicated
// Also check that it is non-null
CHECK TypeHandle::CheckLoadLevel(ClassLoadLevel requiredLevel)
{
    CHECK(!IsNull());
    //    CHECK_MSGF(!IsNull(), ("Type is null, required load level is %s", classLoadLevelName[requiredLevel]));
    C_ASSERT(NumItems(classLoadLevelName) == (1 + CLASS_LOAD_LEVEL_FINAL));

    // Quick check to avoid creating debug string
    ClassLoadLevel actualLevel = GetLoadLevel();
    if (actualLevel < requiredLevel)
    {
        //        SString debugTypeName;
        //        TypeString::AppendTypeDebug(debugTypeName, *this);
        CHECK(actualLevel >= requiredLevel);
        //        CHECK_MSGF(actualLevel >= requiredLevel,
        //                   ("Type has not been sufficiently loaded (actual level is %d, required level is %d)",
        //                    /* debugTypeName.GetUnicode(), */ actualLevel, requiredLevel /* classLoadLevelName[actualLevel], classLoadLevelName[requiredLevel] */));
    }
    CONSISTENCY_CHECK((actualLevel > CLASS_LOAD_UNRESTORED) == IsRestored());
    CONSISTENCY_CHECK((actualLevel == CLASS_LOAD_UNRESTOREDTYPEKEY) == HasUnrestoredTypeKey());
    CHECK_OK;
}

// Check that this type is fully loaded (i.e. to level CLASS_LOADED)
CHECK TypeHandle::CheckFullyLoaded()
{
    if (IsGenericVariable())
    {
        CHECK_OK;
    }
    CheckLoadLevel(CLASS_LOADED);
    CHECK_OK;
}

#endif

#endif //DACCESS_COMPILE

