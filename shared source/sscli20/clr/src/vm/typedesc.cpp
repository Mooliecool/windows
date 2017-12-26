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
// File: typedesc.cpp
//
// ===========================================================================
// This file contains definitions for methods in the TypeDesc class and its
// subclasses ParamTypeDesc, ArrayTypeDesc, TyVarTypeDesc and FnPtrTypeDesc
// @todo akenn: complete this. Right now we've just got TypeVarTypeDesc and FnPtrTypeDesc methods
// ===========================================================================
//

#include "common.h"
#include "typedesc.h"
#include "typestring.h"
#include "array.h"
#include "stackprobe.h"


#ifndef DACCESS_COMPILE
#ifdef _DEBUG

BOOL ParamTypeDesc::Verify() {

    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    _ASSERTE(m_TemplateMT == 0 || m_TemplateMT->SanityCheck());
    _ASSERTE(!GetTypeParam().IsNull());
    BAD_FORMAT_NOTHROW_ASSERT(!(GetTypeParam().IsUnsharedMT() && GetTypeParam().AsMethodTable()->IsArray()));
    BAD_FORMAT_NOTHROW_ASSERT(CorTypeInfo::IsModifier(GetInternalCorElementType()));
    GetTypeParam().Verify();
    return(true);
}

BOOL ArrayTypeDesc::Verify() {

    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    // m_TemplateMT == 0 may be null when building types involving TypeVarTypeDesc's
    BAD_FORMAT_NOTHROW_ASSERT(m_TemplateMT == 0 || m_TemplateMT->IsArray());
    BAD_FORMAT_NOTHROW_ASSERT(CorTypeInfo::IsArray(GetInternalCorElementType()));
    ParamTypeDesc::Verify();
    return(true);
}

#endif

#endif // #ifndef DACCESS_COMPILE

Module* TypeDesc::GetLoaderModule()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    Module *retVal = NULL;
    BOOL fFail = FALSE;
    
    if (HasTypeParam())
    {
        TypeHandle param = GetTypeParam();
        _ASSERTE(!param.IsNull());
        INTERIOR_STACK_PROBE_NOTHROW_CHECK_THREAD(fFail = TRUE);
        if (! fFail)
        {
        retVal = ClassLoader::ComputeLoaderModule(NULL,0,&param,1,NULL,0);
        }
        END_INTERIOR_STACK_PROBE;
        return retVal;
    }
    else if (IsGenericVariable())
    {
        TypeVarTypeDesc* asVar =
            PTR_TypeVarTypeDesc(PTR_HOST_TO_TADDR(this));
        return asVar->GetModule();
    }
    else
    {
        _ASSERTE(GetInternalCorElementType() == ELEMENT_TYPE_FNPTR);
        FnPtrTypeDesc* asFnPtr = (FnPtrTypeDesc*) this;
        INTERIOR_STACK_PROBE_NOTHROW_CHECK_THREAD(fFail = TRUE);
        if (! fFail)
        {
        retVal =  ClassLoader::ComputeLoaderModule(NULL,0,
                                                asFnPtr->GetRetAndArgTypesPointer(),
                                                asFnPtr->GetNumArgs()+1, NULL, 0);
        }                                              
        END_INTERIOR_STACK_PROBE;
        return retVal;
    }
}


Module* TypeDesc::GetZapModule()
{
    WRAPPER_CONTRACT;
    return ExecutionManager::FindZapModule(PTR_HOST_TO_TADDR(this));
}

BaseDomain* TypeDesc::GetDomain()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    Module *pZapModule = GetZapModule();
    if (pZapModule != NULL)
    {
        return pZapModule->GetDomain();
    }

    if (HasTypeParam())
    {
        TypeHandle param = GetTypeParam();
        _ASSERTE(!param.IsNull());
        return param.GetDomain();
    }
    if (IsGenericVariable())
    {
        TypeVarTypeDesc* asVar = (TypeVarTypeDesc*) this;
        return asVar->GetModule()->GetDomain();
    }
    _ASSERTE(GetInternalCorElementType() == ELEMENT_TYPE_FNPTR);
    FnPtrTypeDesc* asFnPtr = (FnPtrTypeDesc*) this;
    return BaseDomain::ComputeBaseDomain(asFnPtr->GetRetAndArgTypesPointer()[0].GetDomain(),
                                         asFnPtr->GetNumArgs(),
                                         asFnPtr->GetRetAndArgTypesPointer()+1);
}

Module* TypeDesc::GetModule() {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
        // Function pointer types belong to no module
        //PRECONDITION(GetInternalCorElementType() != ELEMENT_TYPE_FNPTR);
    }
    CONTRACTL_END

    // Note here we are making the assumption that a typeDesc lives in
    // the classloader of its element type.

    if (HasTypeParam())
    {
        TypeHandle param = GetTypeParam();
        _ASSERTE(!param.IsNull());
        return(param.GetModule());
    }

    if (IsGenericVariable())
    {
        TypeVarTypeDesc* asVar = (TypeVarTypeDesc*) this;
        return asVar->GetModule();
    }

    _ASSERTE(GetInternalCorElementType() == ELEMENT_TYPE_FNPTR);

    return GetLoaderModule();
}

BOOL TypeDesc::IsDomainNeutral()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    return GetDomain()->IsSharedDomain();
}

BOOL ParamTypeDesc::OwnsTemplateMethodTable()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // This set of checks matches precisely those in Module::CreateArrayMethodTable
    // and ParamTypeDesc::ComputeNeedsRestore
    //
    // They indicate if an array TypeDesc is non-canonical (in much the same a a generic
    // method table being non-canonical), i.e. it is not the primary
    // owner of the m_TemplateMT (the primary owner is the TypeDesc for object[])
    //
    CorElementType elemType = m_Arg.GetSignatureCorElementType();
    CorElementType kind = GetInternalCorElementType();
    if (CorTypeInfo::IsArray(kind) &&
        CorTypeInfo::IsObjRef(elemType) &&
        elemType != ELEMENT_TYPE_SZARRAY &&
        m_Arg.GetMethodTable() != g_pObjectClass)
    {
        return FALSE;
    }

    // The m_TemplateMT for pointer types is UIntPtr
    if (!CorTypeInfo::IsArray(kind))
    {
        return FALSE;
    }

    if (CorTypeInfo::IsGenericVariable(elemType))
    {
        return FALSE;
    }

    return TRUE;
}

Assembly* TypeDesc::GetAssembly() {
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    Module *pModule = GetModule();
    PREFIX_ASSUME(pModule!=NULL);
    return pModule->GetAssembly();
}

void TypeDesc::GetName(SString &ssBuf)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    CorElementType kind = GetInternalCorElementType();
    TypeHandle th;
    int rank;

    if (CorTypeInfo::IsModifier(kind))
        th = GetTypeParam();
    else
        th = TypeHandle(this);

    if (kind == ELEMENT_TYPE_ARRAY)
        rank = ((ArrayTypeDesc*) this)->GetRank();
    else if (kind == ELEMENT_TYPE_VAR || kind == ELEMENT_TYPE_MVAR)
        rank = ((TypeVarTypeDesc*) this)->GetIndex();
    else
        rank = 0;

    ConstructName(kind, th, rank, ssBuf);
}

void TypeDesc::ConstructName(CorElementType kind,
                             TypeHandle param,
                             int rank,
                             SString &ssBuff)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM()); // SString operations can allocate.
    }
    CONTRACTL_END

    if (CorTypeInfo::IsModifier(kind))
    {
        param.GetName(ssBuff);
    }

    switch(kind) {
    case ELEMENT_TYPE_BYREF:
        ssBuff.Append(L'&');
        break;
    case ELEMENT_TYPE_PTR:
        ssBuff.Append(L'*');
        break;
    case ELEMENT_TYPE_SZARRAY:
        ssBuff.Append(L"[]");
        break;
    case ELEMENT_TYPE_ARRAY: {
        ssBuff.Append(L'[');

        if (rank == 1)
        {
            ssBuff.Append(L'*');
        }
        else
        {
            while(--rank > 0)
            {
                ssBuff.Append(L',');
            }
        }

        ssBuff.Append(L']');
        break;
    }
    case ELEMENT_TYPE_VAR:
    case ELEMENT_TYPE_MVAR:
    {
        if (kind == ELEMENT_TYPE_VAR)
        {
            ssBuff.Printf(L"!%d", rank);
        }
        else
        {
            ssBuff.Printf(L"!!%d", rank);
        }
        break;
    }

    case ELEMENT_TYPE_FNPTR:
    default:
        LPCUTF8 namesp = CorTypeInfo::GetNamespace(kind);
        if(namesp && *namesp) {
            ssBuff.AppendUTF8(namesp);
            ssBuff.Append(L'.');
        }

        LPCUTF8 name = CorTypeInfo::GetName(kind);
        BAD_FORMAT_NOTHROW_ASSERT(name);
        if (name && *name) {
            ssBuff.AppendUTF8(name);
        }
    }
}

BOOL TypeDesc::IsArray()
{
    WRAPPER_CONTRACT;
    return CorTypeInfo::IsArray(GetInternalCorElementType());
}

BOOL TypeDesc::IsGenericVariable()
{
    WRAPPER_CONTRACT;
    return CorTypeInfo::IsGenericVariable(GetInternalCorElementType());
}

BOOL TypeDesc::HasTypeParam()
{
    WRAPPER_CONTRACT;
    return CorTypeInfo::IsModifier(GetInternalCorElementType());
}

#ifndef DACCESS_COMPILE

BOOL TypeDesc::CanCastTo(TypeHandle toType)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    if (TypeHandle(this) == toType)
        return TRUE;

    //A boxed variable type can be cast to any of its constraints, or object, if none are specified
    if (GetInternalCorElementType() == ELEMENT_TYPE_VAR ||
        GetInternalCorElementType() == ELEMENT_TYPE_MVAR)
    {
        TypeVarTypeDesc *tyvar = (TypeVarTypeDesc*) this;
        DWORD numConstraints;
        TypeHandle *constraints = tyvar->GetConstraints(&numConstraints, CLASS_DEPENDENCIES_LOADED);
        if (toType == g_pObjectClass)
            return TRUE;

        if (toType == g_pValueTypeClass) 
        {
            mdGenericParam genericParamToken = tyvar->GetToken();
            DWORD flags;
            tyvar->GetModule()->GetMDImport()->GetGenericParamProps(genericParamToken, NULL, &flags, NULL, NULL, NULL);
            DWORD specialConstraints = flags & gpSpecialConstraintMask;
            if ((specialConstraints & gpNotNullableValueTypeConstraint) != 0) 
                return TRUE;
        }

        if (constraints == NULL)
            return FALSE;
        else
        {
            for (DWORD i = 0; i < numConstraints; i++)
            {
                if (constraints[i].CanCastTo(toType))
                    return TRUE;
            }
            return FALSE;
        }
    }

    // If we're not casting to a TypeDesc (i.e. not to a reference array type, variable type etc.)
    // then we must be trying to cast to a class or interface type.
    if (!toType.IsTypeDesc())
    {
        MethodTable *pMT = GetMethodTable();
        if (pMT == 0) {
            // I don't have an underlying method table, therefore I'm
            // a variable type, pointer type, function pointer type
            // etc.  I am not an object or value type.  Therefore
            // I can't be cast to an object or value type.
            return FALSE;
        }

        // This does the right thing if 'type' == System.Array or System.Object, System.Clonable ...
        if (pMT->CanCastToClassOrInterface(toType.AsMethodTable()) != 0)
        {
            return TRUE;
        }

        if (IsArray() && toType.AsMethodTable()->IsInterface())
        {
            if (ArraySupportsBizarreInterfaceStatic((ArrayTypeDesc*)this, toType.AsMethodTable()))
            {
                return TRUE;
            }

        }

        return FALSE;
    }

    TypeDesc* toTypeDesc = toType.AsTypeDesc();

    CorElementType toKind = toTypeDesc->GetInternalCorElementType();
    CorElementType fromKind = GetInternalCorElementType();

    // The element kinds must match, only exception is that SZARRAY matches a one dimension ARRAY
    if (!(toKind == fromKind || (CorTypeInfo::IsArray(toKind) && fromKind == ELEMENT_TYPE_SZARRAY)))
        return FALSE;

    // Is it a parameterized type?
    if (CorTypeInfo::IsModifier(toKind)) {
        if (toKind == ELEMENT_TYPE_ARRAY) {
            ArrayTypeDesc* fromArray = (ArrayTypeDesc*) this;
            ArrayTypeDesc* toArray = (ArrayTypeDesc*) toTypeDesc;

            if (fromArray->GetRank() != toArray->GetRank())
                return FALSE;
        }

            // While boxed value classes inherit from object their
            // unboxed versions do not.  Parameterized types have the
            // unboxed version, thus, if the from type parameter is value
            // class then only an exact match works.
        TypeHandle fromParam = GetTypeParam();
        TypeHandle toParam = toTypeDesc->GetTypeParam();
        if (fromParam == toParam)
            return TRUE;

            // Object parameters dont need an exact match but only inheritance, check for that
        CorElementType fromParamCorType = fromParam.GetVerifierCorElementType();
        if (CorTypeInfo::IsObjRef(fromParamCorType))
        {
            return fromParam.CanCastTo(toParam);
        }
        else if (CorTypeInfo::IsGenericVariable(fromParamCorType))
        {
            TypeVarTypeDesc* varFromParam = fromParam.AsGenericVariable();
            
            if (!varFromParam->ConstraintsLoaded())
                varFromParam->LoadConstraints();

            if (!varFromParam->ConstrainedAsObjRef())
                return FALSE;
            
            return fromParam.CanCastTo(toParam);
        }
        else if(CorTypeInfo::IsPrimitiveType(fromParamCorType))
        {
            CorElementType toParamCorType = toParam.GetVerifierCorElementType();
            if(CorTypeInfo::IsPrimitiveType(toParamCorType))
            {
                // Primitive types such as E_T_I4 and E_T_U4 are interchangeable
                // BOOL is NOT interchangeable with I1/U1, neither CHAR -- with I2/U2
                BOOL interchangeable = (toParamCorType == fromParamCorType);
                if(!interchangeable)
                {
                    if((toParamCorType != ELEMENT_TYPE_BOOLEAN)
                       &&(fromParamCorType != ELEMENT_TYPE_BOOLEAN)
                       &&(toParamCorType != ELEMENT_TYPE_CHAR)
                       &&(fromParamCorType != ELEMENT_TYPE_CHAR))
                        interchangeable = interchangeable ||
                           ((CorTypeInfo::Size(toParamCorType) == CorTypeInfo::Size(fromParamCorType))
                            && (CorTypeInfo::IsFloat(toParamCorType) == CorTypeInfo::IsFloat(fromParamCorType)));
                }

                // Enums with interchangeable underlying types are interchangable
                if (interchangeable)
                {
                    MethodTable* pFromMT= fromParam.GetMethodTable();
                    MethodTable* pToMT= toParam.GetMethodTable();
                    if (pFromMT && (pFromMT->IsEnum() || pFromMT->IsTruePrimitive()) &&
                        pToMT && (pToMT->IsEnum()   || pToMT->IsTruePrimitive()))
                            return TRUE;
                }
            } // end if(CorTypeInfo::IsPrimitiveType(toParamCorType))
        } // end if(CorTypeInfo::IsPrimitiveType(fromParamCorType)) 

            // Anything else is not a match.
        return FALSE;
    }


    if (toKind == ELEMENT_TYPE_VAR || toKind == ELEMENT_TYPE_MVAR ||
        toKind == ELEMENT_TYPE_FNPTR)
    {
        return FALSE;
    };


    BAD_FORMAT_NOTHROW_ASSERT(toKind == ELEMENT_TYPE_TYPEDBYREF || CorTypeInfo::IsPrimitiveType(toKind));
    return TRUE;
}
TypeHandle::CastResult TypeDesc::CanCastToNoGC(TypeHandle toType)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

    if (TypeHandle(this) == toType)
        return TypeHandle::CanCast;

    // If we're not casting to a TypeDesc (i.e. not to a reference array type, variable type etc.)
    // then we must be trying to cast to a class or interface type.
    if (!toType.IsTypeDesc())
    {

        //A boxed variable type can be cast to any of its constraints, or object, if none are specified
        if (GetInternalCorElementType() == ELEMENT_TYPE_VAR ||
            GetInternalCorElementType() == ELEMENT_TYPE_MVAR)
        {
            TypeVarTypeDesc *tyvar = (TypeVarTypeDesc*) this;
            DWORD numConstraints;
            TypeHandle *constraints = tyvar->GetCachedConstraints(&numConstraints);
            if (constraints == NULL)
                return (toType == g_pObjectClass) ? TypeHandle::CanCast : TypeHandle::CannotCast;
            else
            {
                for (DWORD i = 0; i < numConstraints; i++)
                {
                    if (constraints[i].CanCastToNoGC(toType) == TypeHandle::CanCast)
                        return TypeHandle::CanCast;
                }
                return TypeHandle::MaybeCast;
            }
        };

        MethodTable *pMT = GetMethodTable();
        if (pMT == 0) {
            // I don't have an underlying method table, therefore I'm
            // a variable type, pointer type, function pointer type
            // etc.  I am not an object or value type.  Therefore
            // I can't be cast to an object or value type.
            return TypeHandle::CannotCast;
        }

        // This does the right thing if 'type' == System.Array or System.Object, System.Clonable ...
        return pMT->CanCastToClassOrInterfaceNoGC(toType.AsMethodTable());
    }

    TypeDesc* toTypeDesc = toType.AsTypeDesc();

    CorElementType toKind = toTypeDesc->GetInternalCorElementType();
    CorElementType fromKind = GetInternalCorElementType();

    // The element kinds must match, only exception is that SZARRAY matches a one dimension ARRAY
    if (!(toKind == fromKind || (CorTypeInfo::IsArray(toKind) && fromKind == ELEMENT_TYPE_SZARRAY)))
        return TypeHandle::CannotCast;

    // Is it a parameterized type?
    if (CorTypeInfo::IsModifier(toKind)) {
        if (toKind == ELEMENT_TYPE_ARRAY) {
            ArrayTypeDesc* fromArray = (ArrayTypeDesc*) this;
            ArrayTypeDesc* toArray = (ArrayTypeDesc*) toTypeDesc;

            if (fromArray->GetRank() != toArray->GetRank())
                return TypeHandle::CannotCast;
        }

            // While boxed value classes inherit from object their
            // unboxed versions do not.  Parameterized types have the
            // unboxed version, thus, if the from type parameter is value
            // class then only an exact match works.
        TypeHandle fromParam = GetTypeParam();
        TypeHandle toParam = toTypeDesc->GetTypeParam();
        if (fromParam == toParam)
            return TypeHandle::CanCast;

            // Object parameters dont need an exact match but only inheritance, check for that
        CorElementType fromParamCorType = fromParam.GetVerifierCorElementType();
        if (CorTypeInfo::IsObjRef(fromParamCorType))
        {
            return fromParam.CanCastToNoGC(toParam);
        }
        else if (CorTypeInfo::IsGenericVariable(fromParamCorType))
        {
            TypeVarTypeDesc* varFromParam = fromParam.AsGenericVariable();
            
            if (!varFromParam->ConstraintsLoaded())
                return TypeHandle::MaybeCast;

            if (!varFromParam->ConstrainedAsObjRef())
                return TypeHandle::CannotCast;
            
            return fromParam.CanCastToNoGC(toParam);
        }
        else if (CorTypeInfo::IsPrimitiveType(fromParamCorType))
        {
            CorElementType toParamCorType = toParam.GetVerifierCorElementType();
            if(CorTypeInfo::IsPrimitiveType(toParamCorType))
            {
                // Primitive types such as E_T_I4 and E_T_U4 are interchangeable
                // BOOL is NOT interchangeable with I1/U1, neither CHAR -- with I2/U2
                BOOL interchangeable = (toParamCorType == fromParamCorType);
                if(!interchangeable)
                {
                    if((toParamCorType != ELEMENT_TYPE_BOOLEAN)
                       &&(fromParamCorType != ELEMENT_TYPE_BOOLEAN)
                       &&(toParamCorType != ELEMENT_TYPE_CHAR)
                       &&(fromParamCorType != ELEMENT_TYPE_CHAR))
                        interchangeable = interchangeable ||
                           ((CorTypeInfo::Size(toParamCorType) == CorTypeInfo::Size(fromParamCorType))
                            && (CorTypeInfo::IsFloat(toParamCorType) == CorTypeInfo::IsFloat(fromParamCorType)));
                }

                // Enums with interchangeable underlying types are interchangable
                if (interchangeable)
                {
                    MethodTable* pFromMT= fromParam.GetMethodTable();
                    MethodTable* pToMT= toParam.GetMethodTable();
                    if (pFromMT && (pFromMT->IsEnum() || pFromMT->IsTruePrimitive()) &&
                        pToMT && (pToMT->IsEnum()   || pToMT->IsTruePrimitive()))
                            return TypeHandle::CanCast;
                }
            } // end if(CorTypeInfo::IsPrimitiveType(toParamCorType))
        } // end if(CorTypeInfo::IsPrimitiveType(fromParamCorType)) 

            // Anything else is not a match.
        return TypeHandle::CannotCast;
    }


    if (toKind == ELEMENT_TYPE_VAR || toKind == ELEMENT_TYPE_MVAR ||
        toKind == ELEMENT_TYPE_FNPTR)
    {
        return TypeHandle::CannotCast;
    };


    BAD_FORMAT_NOTHROW_ASSERT(toKind == ELEMENT_TYPE_TYPEDBYREF || CorTypeInfo::IsPrimitiveType(toKind));
    return TypeHandle::CanCast;
}

#endif // #ifndef DACCESS_COMPILE


TypeHandle TypeDesc::GetParent() {

    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    CorElementType kind = GetInternalCorElementType();
    if (CorTypeInfo::IsArray(kind)) {
        _ASSERTE(IsArray());
        BAD_FORMAT_NOTHROW_ASSERT(kind == ELEMENT_TYPE_SZARRAY || kind == ELEMENT_TYPE_ARRAY);
        return ((ArrayTypeDesc*)this)->GetParent();
    }
    if (CorTypeInfo::IsPrimitiveType(kind))
        return (MethodTable*)g_pObjectClass;
    return TypeHandle();
}

#ifndef DACCESS_COMPILE

OBJECTREF ParamTypeDesc::GetManagedClassObject()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;

        INJECT_FAULT(COMPlusThrowOM());

        PRECONDITION(GetInternalCorElementType() == ELEMENT_TYPE_ARRAY ||
                     GetInternalCorElementType() == ELEMENT_TYPE_SZARRAY ||
                     GetInternalCorElementType() == ELEMENT_TYPE_BYREF ||
                     GetInternalCorElementType() == ELEMENT_TYPE_PTR);
    }
    CONTRACTL_END;

    if (m_ExposedClassObject == NULL) {
        REFLECTCLASSBASEREF  refClass = NULL;
        GCPROTECT_BEGIN(refClass);
        if (GetAssembly()->IsIntrospectionOnly())
            refClass = (REFLECTCLASSBASEREF) AllocateObject(g_Mscorlib.GetClass(CLASS__CLASS_INTROSPECTION_ONLY));
        else
            refClass = (REFLECTCLASSBASEREF) AllocateObject(g_Mscorlib.GetClass(CLASS__CLASS));
        ((ReflectClassBaseObject*)OBJECTREFToObject(refClass))->SetType(TypeHandle(this));

        // Let all threads fight over who wins using InterlockedCompareExchange.
        // Only the winner can set m_ExposedClassObject from NULL.
        OBJECTREF *exposedClassObject;
        exposedClassObject = GetDomain()->AllocateObjRefPtrsInLargeTable(1);
        SetObjectReference(exposedClassObject, refClass, IsDomainNeutral() ? NULL : (AppDomain*)GetDomain());

        if (FastInterlockCompareExchangePointer((void**)&m_ExposedClassObject, *(void**)&exposedClassObject, NULL))
            SetObjectReference(exposedClassObject, NULL, NULL);

        GCPROTECT_END();
    }
    return *m_ExposedClassObject;

}

#endif // #ifndef DACCESS_COMPILE

BOOL TypeDesc::IsRestored()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    return IsRestored_NoLogging();
}

BOOL TypeDesc::IsRestored_NoLogging()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    return (m_typeAndFlags & TypeDesc::enum_flag_Unrestored) == 0;
}

ClassLoadLevel TypeDesc::GetLoadLevel()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (m_typeAndFlags & TypeDesc::enum_flag_UnrestoredTypeKey)
    {
        return CLASS_LOAD_UNRESTOREDTYPEKEY;
    }
    else if (m_typeAndFlags & TypeDesc::enum_flag_Unrestored)
    {
        return CLASS_LOAD_UNRESTORED;
    }
    else if (m_typeAndFlags & TypeDesc::enum_flag_IsNotFullyLoaded)
    {
        if (m_typeAndFlags & TypeDesc::enum_flag_DependenciesLoaded)
        {
            return CLASS_DEPENDENCIES_LOADED;
        }
        else
        {
            return CLASS_LOAD_EXACTPARENTS;
        }
    }

    return CLASS_LOADED;
}


// Recursive worker that pumps the transitive closure of a type's dependencies to the specified target level.
// Dependencies include:
//
//   - parent
//   - interfaces
//   - canonical type, for non-canonical instantiations
//   - typical type, for non-typical instantiations
//
// Parameters:
//
//   pVisited - used to prevent endless recursion in the case of cyclic dependencies
//
//   level    - target level to pump to - must be CLASS_DEPENDENCIES_LOADED or CLASS_LOADED
//
//              if CLASS_DEPENDENCIES_LOADED, all transitive dependencies are resolved to their
//                 exact types.
//
//              if CLASS_LOADED, all type-safety checks are done on the type and all its transitive
//                 dependencies. Note that for the CLASS_LOADED case, some types may be left
//                 on the pending list rather that pushed to CLASS_LOADED in the case of cyclic
//                 dependencies - the root caller must handle this.
//
//
//   pfBailed - if we or one of our depedencies bails early due to cyclic dependencies, we
//              must set *pfBailed to TRUE. Otherwise, we must *leave it unchanged* (thus, the
//              boolean acts as a cumulative OR.)
//
//   pPending - if one of our dependencies bailed, the type cannot yet be promoted to CLASS_LOADED
//              as the dependencies will be checked later and may fail a security check then.
//              Instead, DoFullyLoad() will add the type to the pending list - the root caller
//              is responsible for promoting the type after the full transitive closure has been
//              walked. Note that it would be just as correct to always defer to the pending list -
//              however, that is a little less performant.
//
void TypeDesc::DoFullyLoad(TypeHandleList *pVisited, ClassLoadLevel level, DFLPendingList *pPending, BOOL *pfBailed)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END

    _ASSERTE(level == CLASS_LOADED || level == CLASS_DEPENDENCIES_LOADED);
    _ASSERTE(pfBailed != NULL);
    _ASSERTE(!(level == CLASS_LOADED && pPending == NULL));


#ifndef DACCESS_COMPILE

    if (TypeHandleList::Exists(pVisited, TypeHandle(this)))
    {
        *pfBailed = TRUE;
        return;
    }

    if (GetLoadLevel() >= level)
    {
        return;
    }

    if (level == CLASS_LOADED)
    {
        UINT numTH = pPending->Count();
        TypeHandle *pTypeHndPending = pPending->Table();
        for (UINT idxPending = 0; idxPending < numTH; idxPending++)
        {
            if (pTypeHndPending[idxPending].IsTypeDesc() && pTypeHndPending[idxPending].AsTypeDesc() == this)
            {
                *pfBailed = TRUE;
                return;
            }
        }

    }


    BOOL fBailed = FALSE;

    // First ensure that we're loaded to just below CLASS_LOADED
    ClassLoader::EnsureLoaded(TypeHandle(this), (ClassLoadLevel) (level-1));
    
    TypeHandleList newVisited(TypeHandle(this), pVisited);

    if (HasTypeParam())
    {
        // Fully load the type parameter
        GetTypeParam().DoFullyLoad(&newVisited, level, pPending, &fBailed);

        ParamTypeDesc* pPTD = (ParamTypeDesc*) this;

        // Fully load the template method table
        if (pPTD->m_TemplateMT != NULL)
        {
            pPTD->m_TemplateMT->DoFullyLoad(&newVisited, level, pPending, &fBailed);
        }
    }

    switch (level)
    {
        case CLASS_DEPENDENCIES_LOADED:
            FastInterlockOr(&m_typeAndFlags, TypeDesc::enum_flag_DependenciesLoaded);
            break;

        case CLASS_LOADED:
            if (fBailed)
            {
                // We couldn't complete security checks on some dependency because he is already being processed by one of our callers.
                // Do not mark this class fully loaded yet. Put him on the pending list and he will be marked fully loaded when
                // everything unwinds.

                *pfBailed = TRUE;

                TypeHandle* pthPending = pPending->AppendThrowing();
                *pthPending = TypeHandle(this);
            }
            else
            {
                // Finally, mark this method table as fully loaded
                SetIsFullyLoaded();
            }
            break;

        default:
            _ASSERTE(!"Can't get here.");
            break;
    }
#endif
}




#ifndef DACCESS_COMPILE


TypeHandle* TypeVarTypeDesc::GetCachedConstraints(DWORD *pNumConstraints)
{
    LEAF_CONTRACT;
    PRECONDITION(CheckPointer(pNumConstraints));
    PRECONDITION(m_numConstraints != (DWORD) -1);

    *pNumConstraints = m_numConstraints;
    return m_constraints;
}




TypeHandle* TypeVarTypeDesc::GetConstraints(DWORD *pNumConstraints, ClassLoadLevel level /* = CLASS_LOADED */)
{
    WRAPPER_CONTRACT;
    PRECONDITION(CheckPointer(pNumConstraints));
    PRECONDITION(level == CLASS_DEPENDENCIES_LOADED || level == CLASS_LOADED);

    if (m_numConstraints == (DWORD) -1)
        LoadConstraints(level);

    *pNumConstraints = m_numConstraints;
    return m_constraints;
}


void TypeVarTypeDesc::LoadConstraints(ClassLoadLevel level /* = CLASS_LOADED */)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        INJECT_FAULT(COMPlusThrowOM());

        PRECONDITION(level == CLASS_DEPENDENCIES_LOADED || level == CLASS_LOADED);
    }
    CONTRACTL_END;

    if (m_numConstraints == (DWORD) -1)
    {
        IMDInternalImport* pInternalImport = GetModule()->GetMDImport();

        HENUMInternalHolder hEnum(pInternalImport);
        mdGenericParamConstraint tkConstraint;
        
        SigTypeContext typeContext;
        mdToken defToken = GetTypeOrMethodDef();

        MethodTable *pMT = NULL;
        if (TypeFromToken(defToken) == mdtMethodDef)
        {
            MethodDesc *pMD = m_pModule->LookupMethodDef(defToken);
            _ASSERTE(pMD != NULL);
            PREFIX_ASSUME(pMD!= NULL);
            _ASSERTE(pMD->IsGenericMethodDefinition());
            SigTypeContext::InitTypeContext(pMD,&typeContext);
            _ASSERTE(typeContext.m_methInst != NULL);
            pMT = pMD->GetMethodTable();
        }
        else
        {
            _ASSERTE(TypeFromToken(defToken) == mdtTypeDef);
            TypeHandle genericType = m_pModule->LookupTypeDef(defToken);

            _ASSERTE(!genericType.IsNull() || GetAppDomain()->IsCompilationDomain());
            if (genericType.IsNull())
                GetModule()->GetAssembly()->ThrowTypeLoadException(pInternalImport, defToken, IDS_CLASSLOAD_GENERAL);

            _ASSERTE(genericType.IsGenericTypeDefinition());
            SigTypeContext::InitTypeContext(genericType,&typeContext);
        }
        
        hEnum.EnumInit(mdtGenericParamConstraint, GetToken());
        DWORD numConstraints = pInternalImport->EnumGetCount(&hEnum);
        if (numConstraints != 0)
        {
            // If there is a single class constraint we put in in element 0 of the array
            AllocMemHolder<TypeHandle> constraints 
                (GetDomain()->GetLowFrequencyHeap()->AllocMem(numConstraints * sizeof(TypeHandle)));

            DWORD i = 0;
            constraints[0] = TypeHandle();
            while (pInternalImport->EnumNext(&hEnum, &tkConstraint))
            {
                _ASSERTE(i <= numConstraints);
                mdToken tkConstraintType, tkParam;
                pInternalImport->GetGenericParamConstraintProps(tkConstraint, &tkParam, &tkConstraintType);
                _ASSERTE(tkParam == GetToken());
                TypeHandle thConstraint = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(GetModule(), tkConstraintType, 
                                                                                      &typeContext, 
                                                                                      ClassLoader::ThrowIfNotFound, 
                                                                                      ClassLoader::FailIfUninstDefOrRef,
                                                                                      ClassLoader::LoadTypes,
                                                                                      level);

                constraints[i++] = thConstraint;

                // Method type constraints behave contravariantly
                // (cf Bounded polymorphism e.g. see
                //     Cardelli & Wegner, On understanding types, data abstraction and polymorphism, Computing Surveys 17(4), Dec 1985)                
                if (pMT != NULL && pMT->HasVariance() && TypeFromToken(tkConstraintType) == mdtTypeSpec)
                {
                    ULONG cSig;
                    PCCOR_SIGNATURE pSig;
                    pInternalImport->GetTypeSpecFromToken(tkConstraintType, &pSig, &cSig);
                    if (!pMT->GetClass()->CheckVarianceInSig(pMT->GetNumGenericArgs(),
                                                             pMT->GetClass()->GetVarianceInfo(), 
                                                             SigPointer(pSig), gpContravariant))
                    {
                        GetModule()->GetAssembly()->ThrowTypeLoadException(pInternalImport, pMT->GetCl(), IDS_CLASSLOAD_VARIANCE_IN_CONSTRAINT);
                    }
                }
            }


            constraints.SuppressRelease();

            m_constraints = constraints;
        }
        
        m_numConstraints = numConstraints;
    }

    for (DWORD i = 0; i < m_numConstraints; i++)
    {
        ClassLoader::EnsureLoaded(m_constraints[i], level);
    }
}

BOOL TypeVarTypeDesc::ConstrainedAsObjRef()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    IMDInternalImport* pInternalImport = GetModule()->GetMDImport();
    mdGenericParam genericParamToken = GetToken();
    DWORD flags;
    pInternalImport->GetGenericParamProps(genericParamToken, NULL, &flags, NULL, NULL, NULL);
    DWORD specialConstraints = flags & gpSpecialConstraintMask;

    if ((specialConstraints & gpReferenceTypeConstraint) != 0)
        return TRUE;

    DWORD dwNumConstraints = 0;
    TypeHandle* constraints = GetCachedConstraints(&dwNumConstraints);

    for (DWORD i = 0; i < dwNumConstraints; i++)
    {
        TypeHandle constraint = constraints[i];

        if (constraint.IsGenericVariable() && constraint.AsGenericVariable()->ConstrainedAsObjRef())
            return TRUE;
        
        if (!constraint.IsInterface() && CorTypeInfo::IsObjRef(constraint.GetInternalCorElementType()))
            return TRUE;
    }

    return FALSE;
}




// Forward decl (if I put the main body of the fcn here, it windiff's very badly for some reason...)
static
BOOL SatisfiesConstraintsRecHelper(TypeVarTypeDesc *pTyVar, DWORD specialConstraint, SigTypeContext *pTypeContext);



BOOL TypeVarTypeDesc::SatisfiesConstraints(SigTypeContext *pTypeContext, TypeHandle thArg)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(!thArg.IsNull());
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

   IMDInternalImport* pInternalImport = GetModule()->GetMDImport();
   mdGenericParamConstraint tkConstraint;
        
   INDEBUG(mdToken defToken = GetTypeOrMethodDef());
   _ASSERTE(TypeFromToken(defToken) == mdtMethodDef || TypeFromToken(defToken) == mdtTypeDef);

   // First check special constraints (must-be-reference-type, must-be-value-type, and must-have-default-constructor)
   mdGenericParam genericParamToken = GetToken();
   DWORD flags;
   pInternalImport->GetGenericParamProps(genericParamToken, NULL, &flags, NULL, NULL, NULL);
   DWORD specialConstraints = flags & gpSpecialConstraintMask;

   if (thArg.IsGenericVariable())
   {
       if ( (specialConstraints & gpNotNullableValueTypeConstraint)  != 0 )
       {
           if (!SatisfiesConstraintsRecHelper(thArg.AsGenericVariable(), gpNotNullableValueTypeConstraint, pTypeContext))
           {
               return FALSE;
           }
       }

       if ( (specialConstraints & gpReferenceTypeConstraint)  != 0 )
       {
           if (!SatisfiesConstraintsRecHelper(thArg.AsGenericVariable(), gpReferenceTypeConstraint, pTypeContext))
           {
               return FALSE;
           }
       }

       if ( (specialConstraints & gpDefaultConstructorConstraint)  != 0 )
       {
           if (!SatisfiesConstraintsRecHelper(thArg.AsGenericVariable(), gpDefaultConstructorConstraint, pTypeContext))
           {
               return FALSE;
           }
       }


   }
   else
   {

       if ((specialConstraints & gpNotNullableValueTypeConstraint) != 0)
       { 
           if (!thArg.IsValueType()) 
               return FALSE;        
           else
           {
               // the type argument is a value type, however if it is any kind of Nullable we want to fail
               // as the constraint accepts any value type except Nullable types (Nullable itself is a value type)
               if (thArg.AsMethodTable()->HasSameTypeDefAs(g_Mscorlib.FetchClass(CLASS__NULLABLE))) 
                   return FALSE;
           }
       }
       
       if ((specialConstraints & gpReferenceTypeConstraint) != 0)
       {
           if (thArg.IsValueType())
               return FALSE;
       }
    
       if ((specialConstraints & gpDefaultConstructorConstraint) != 0)
       {
           if (thArg.IsTypeDesc() || (!thArg.AsMethodTable()->HasExplicitOrImplicitPublicDefaultConstructor()))
               return FALSE;
       }
   }
       
   // Now check general subtype constraints
   HENUMInternalHolder hEnum(pInternalImport);
   hEnum.EnumInit(mdtGenericParamConstraint, genericParamToken);
   while (pInternalImport->EnumNext(&hEnum, &tkConstraint))
   {
       mdToken tkConstraintType, tkParam;
       pInternalImport->GetGenericParamConstraintProps(tkConstraint, &tkParam, &tkConstraintType);
       _ASSERTE(tkParam == GetToken());
       TypeHandle thConstraint = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(GetModule(), tkConstraintType, pTypeContext, 
                                                                             ClassLoader::ThrowIfNotFound, 
                                                                             ClassLoader::FailIfUninstDefOrRef,
                                                                             ClassLoader::LoadTypes,
                                                                             CLASS_DEPENDENCIES_LOADED);
       if (!(thArg.CanCastTo(thConstraint)))
           return FALSE;
   }	   
   return TRUE;
}


//---------------------------------------------------------------------------------------------------------------------
// We come here only if a type parameter with a special constraint is instantiated by an argument that is itself
// a type parameter. In this case, we'll need to examine *its* constraints to see if the range of types that would satisfy its
// constraints is a subset of the range of types that would satisfy the special constraint.
//
// This routine will return TRUE if it can prove that argument "pTyArg" has a constraint that will satisfy the special constraint.
//
// (NOTE: It does not check against anything other than one specific specialConstraint (it doesn't even know what they are.) This is
// just one step in the checking of constraints.)
//---------------------------------------------------------------------------------------------------------------------
static
BOOL SatisfiesConstraintsRecHelper(TypeVarTypeDesc *pTyArg, DWORD specialConstraint, SigTypeContext *pTypeContext)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END;

    // The caller must invoke for all special constraints that apply - this fcn can only reliably test against one
    // constraint at a time.
    _ASSERTE(specialConstraint == gpNotNullableValueTypeConstraint
          || specialConstraint == gpReferenceTypeConstraint
          || specialConstraint == gpDefaultConstructorConstraint);

    IMDInternalImport* pInternalImport = pTyArg->GetModule()->GetMDImport();

    // Get the argument type's own special constraints
    DWORD argFlags;
    pTyArg->GetModule()->GetMDImport()->GetGenericParamProps(pTyArg->GetToken(),NULL,&argFlags,NULL,NULL,NULL);
    DWORD argSpecialConstraints = argFlags & gpSpecialConstraintMask;


    // First, if the argument's own special constraints match the parameter's special constraints,
    // we can safely conclude the constraint is satisfied.
    if (specialConstraint == gpNotNullableValueTypeConstraint)
    {
        if ((argSpecialConstraints & gpNotNullableValueTypeConstraint) != 0)
        {
            return TRUE;
        }
    }
    
    if (specialConstraint == gpReferenceTypeConstraint)
    {
        if ((argSpecialConstraints & gpReferenceTypeConstraint) != 0)
        {
            return TRUE;
        }
    }
 
    if (specialConstraint == gpDefaultConstructorConstraint)
    {
        // Note: we no longer accept gpNotNullableValueType here - it is possible to declare a valuetype's default ctor private.
        if ((argSpecialConstraints & (gpDefaultConstructorConstraint | gpNotNullableValueTypeConstraint)) != 0)
        {
            return TRUE;
        }
    }


    // The special constraints did not match. However, we may find a primary type constraint
    // that would always satisfy the special constraint.
    HENUMInternalHolder hEnum(pInternalImport);
    hEnum.EnumInit(mdtGenericParamConstraint, pTyArg->GetToken());

    mdGenericParamConstraint tkConstraint;
    while (pInternalImport->EnumNext(&hEnum, &tkConstraint))
    {
        mdToken tkConstraintType, tkParam;
        pInternalImport->GetGenericParamConstraintProps(tkConstraint, &tkParam, &tkConstraintType);
        _ASSERTE(tkParam == pTyArg->GetToken());
        TypeHandle thConstraint = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(pTyArg->GetModule(),
                                                                              tkConstraintType,
                                                                              pTypeContext,
                                                                              ClassLoader::ThrowIfNotFound, 
                                                                              ClassLoader::FailIfUninstDefOrRef,
                                                                              ClassLoader::LoadTypes,
                                                                              CLASS_DEPENDENCIES_LOADED);
        if (thConstraint.IsGenericVariable())
        {
            //
        }
        else if (thConstraint.IsInterface())
        {
            // This is a secondary constraint - this tells us nothing about the eventual instantiation that
            // we can use here.
        }
        else 
        {
            // This is a type constraint. Remember that the eventual instantiation is only guaranteed to be
            // something *derived* from this type, not the actual type itself. To emphasize, we rename the local.

            TypeHandle thAncestorOfType = thConstraint;

            if (specialConstraint == gpNotNullableValueTypeConstraint)
            {
                if (thAncestorOfType.IsValueType() && !(thAncestorOfType.AsMethodTable()->HasSameTypeDefAs(g_Mscorlib.FetchClass(CLASS__NULLABLE))))
                {
                    return TRUE;
                }
            }
            
            if (specialConstraint == gpReferenceTypeConstraint)
            {

                if (thAncestorOfType.IsUnsharedMT())
                {
                    MethodTable *pAncestorMT = thAncestorOfType.AsMethodTable();

                    if ( (!(pAncestorMT->IsValueType())) && pAncestorMT != g_pObjectClass && pAncestorMT != g_pValueTypeClass )
                    {
                        BOOL fIsValueTypeAnAncestor = FALSE;
                        MethodTable *pParentMT = pAncestorMT->GetParentMethodTable();
                        while (pParentMT)
                        {
                            if (pParentMT == g_pValueTypeClass)
                            {
                                fIsValueTypeAnAncestor = TRUE;
                                break;
                            }
                            pParentMT = pParentMT->GetParentMethodTable();
                        }

                        if (!fIsValueTypeAnAncestor)
                        {
                            return TRUE;
                        }
                    }
                }
            }
         
            if (specialConstraint == gpDefaultConstructorConstraint)
            {
                // If a valuetype, just check to ensure that doesn't have a private default ctor.
                // If not a valuetype, not much we can conclude knowing just an ancestor class.
                if (thAncestorOfType.IsValueType() && thAncestorOfType.GetMethodTable()->HasExplicitOrImplicitPublicDefaultConstructor())
                {
                    return TRUE;
                }
            }
            
        }
    }


    // If we got here, we found no evidence that the argument's constraints are strict enough to satisfy the parameter's constraints.
    return FALSE;
}





OBJECTREF TypeVarTypeDesc::GetManagedClassObject()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;

        INJECT_FAULT(COMPlusThrowOM());
        
        PRECONDITION(GetInternalCorElementType() == ELEMENT_TYPE_VAR ||
                     GetInternalCorElementType() == ELEMENT_TYPE_MVAR);
    }
    CONTRACTL_END;
            
    if (m_ExposedClassObject == NULL) {
        REFLECTCLASSBASEREF  refClass = NULL;
        GCPROTECT_BEGIN(refClass);
        if (GetAssembly()->IsIntrospectionOnly())
            refClass = (REFLECTCLASSBASEREF) AllocateObject(g_Mscorlib.GetClass(CLASS__CLASS_INTROSPECTION_ONLY));
        else
            refClass = (REFLECTCLASSBASEREF) AllocateObject(g_Mscorlib.GetClass(CLASS__CLASS));
        ((ReflectClassBaseObject*)OBJECTREFToObject(refClass))->SetType(TypeHandle(this));

        // Let all threads fight over who wins using InterlockedCompareExchange.
        // Only the winner can set m_ExposedClassObject from NULL.      
        OBJECTREF *exposedClassObject = GetDomain()->AllocateObjRefPtrsInLargeTable(1);
        SetObjectReference(exposedClassObject, refClass, IsDomainNeutral() ? NULL : (AppDomain*)GetDomain());
        
        if (FastInterlockCompareExchangePointer((void**)&m_ExposedClassObject, *(void**)&exposedClassObject, NULL)) 
            SetObjectReference(exposedClassObject, NULL, NULL);

        GCPROTECT_END();
    }
    return *m_ExposedClassObject;
        
}







#endif // #ifndef DACCESS_COMPILE

TypeHandle* FnPtrTypeDesc::GetRetAndArgTypes()
{ 
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    // Decode encoded type handles on demand

    return m_RetAndArgTypes;
}


#ifdef DACCESS_COMPILE

void
ParamTypeDesc::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_DTHIS();

    if (m_TemplateMT.IsValid())
    {
        m_TemplateMT->EnumMemoryRegions(flags);
    }

    m_Arg.EnumMemoryRegions(flags);
}

void
TypeVarTypeDesc::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_DTHIS();

    if (m_pModule.IsValid())
    {
        m_pModule->EnumMemoryRegions(flags, true);
    }

    if (m_numConstraints != (DWORD)-1)
    {
        PTR_TypeHandle constraint = m_constraints;
        for (DWORD i = 0; i < m_numConstraints; i++)
        {
            if (constraint.IsValid())
            {
                constraint->EnumMemoryRegions(flags);
            }
            constraint++;
        }
    }
}

void
FnPtrTypeDesc::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_DTHIS();

    for (DWORD i = 0; i < m_NumArgs; i++)
    {
        m_RetAndArgTypes[i].EnumMemoryRegions(flags);
    }
}

#endif // #ifdef DACCESS_COMPILE
