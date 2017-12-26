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
// File: Field.cpp
//
// ===========================================================================
// This file contains the implementation for FieldDesc methods.
// ===========================================================================
//

#include "common.h"

#include "encee.h"
#include "field.h"
#include "remoting.h"
#include "generics.h"

VOID FieldDesc::Init(mdFieldDef mb, CorElementType FieldType, DWORD dwMemberAttrs, BOOL fIsStatic, BOOL fIsRVA, BOOL fIsThreadLocal, BOOL fIsContextLocal, LPCSTR pszFieldName)
{ 
    LEAF_CONTRACT;
    
    // We allow only a subset of field types here - all objects must be set to TYPE_CLASS
    // By-value classes are ELEMENT_TYPE_VALUETYPE
    _ASSERTE(
        FieldType == ELEMENT_TYPE_I1 ||
        FieldType == ELEMENT_TYPE_BOOLEAN ||
        FieldType == ELEMENT_TYPE_U1 ||
        FieldType == ELEMENT_TYPE_I2 ||
        FieldType == ELEMENT_TYPE_U2 ||
        FieldType == ELEMENT_TYPE_CHAR ||
        FieldType == ELEMENT_TYPE_I4 ||
        FieldType == ELEMENT_TYPE_U4 ||
        FieldType == ELEMENT_TYPE_I8 ||
        FieldType == ELEMENT_TYPE_I  ||
        FieldType == ELEMENT_TYPE_U  ||
        FieldType == ELEMENT_TYPE_U8 ||
        FieldType == ELEMENT_TYPE_R4 ||
        FieldType == ELEMENT_TYPE_R8 ||
        FieldType == ELEMENT_TYPE_CLASS ||
        FieldType == ELEMENT_TYPE_VALUETYPE ||
        FieldType == ELEMENT_TYPE_PTR ||
        FieldType == ELEMENT_TYPE_FNPTR
        );
    _ASSERTE(fIsStatic || (!fIsRVA && !fIsThreadLocal && !fIsContextLocal));
    _ASSERTE(fIsRVA + fIsThreadLocal + fIsContextLocal <= 1);
    
    m_mb = RidFromToken(mb);
    m_type = FieldType;
    m_prot = fdFieldAccessMask & dwMemberAttrs;
    m_isDangerousAppDomainAgileField = 0;
    m_isStatic = fIsStatic != 0;
    m_isRVA = fIsRVA != 0;
    m_isThreadLocal = fIsThreadLocal != 0;
    m_isContextLocal = fIsContextLocal != 0;
    
#ifdef _DEBUG
    m_debugName = (LPUTF8)pszFieldName;
#endif
    _ASSERTE(GetMemberDef() == mb);                 // no truncation
    _ASSERTE(GetFieldType() == FieldType);
    _ASSERTE(GetFieldProtection() == (fdFieldAccessMask & dwMemberAttrs));
    _ASSERTE((BOOL) IsStatic() == (fIsStatic != 0));
}

// Return whether the field is a GC ref type
BOOL FieldDesc::IsObjRef()
{
    WRAPPER_CONTRACT;
    return CorTypeInfo::IsObjRef(GetFieldType());
}

#ifndef DACCESS_COMPILE //we don't require DAC to special case simple types
// Return the type of the field, as a class, but only if it's loaded.
TypeHandle FieldDesc::LookupFieldTypeHandle(ClassLoadLevel level, BOOL dropGenericArgumentLevel)
{

    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

    // This function is called during GC promotion.
    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

    // Caller should have handled all the non-class cases, already.
    _ASSERTE(GetFieldType() == ELEMENT_TYPE_CLASS ||
             GetFieldType() == ELEMENT_TYPE_VALUETYPE);

    MetaSig        sig(this);
    CorElementType type;

    type = sig.NextArg();

    // This may be the real type which includes other things
    //  beside class and value class such as arrays
    _ASSERTE(type == ELEMENT_TYPE_CLASS ||
             type == ELEMENT_TYPE_VALUETYPE ||
             type == ELEMENT_TYPE_STRING ||
             type == ELEMENT_TYPE_VALUEARRAY ||
             type == ELEMENT_TYPE_SZARRAY ||
             type == ELEMENT_TYPE_VAR
             );

    // == FailIfNotLoaded, can also assert that the thing is restored
    return sig.GetLastTypeHandleThrowing(ClassLoader::DontLoadTypes, level, dropGenericArgumentLevel);
}
#else //simplified version
TypeHandle FieldDesc::LookupFieldTypeHandle(ClassLoadLevel level, BOOL dropGenericArgumentLevel)
{
    WRAPPER_CONTRACT;
    MetaSig        sig(this);
    CorElementType type;
    type = sig.NextArg();
    return sig.GetLastTypeHandleThrowing(ClassLoader::DontLoadTypes, level, dropGenericArgumentLevel);
}
#endif //DACCESS_COMPILE 

TypeHandle FieldDesc::GetFieldTypeHandleThrowing(ClassLoadLevel level/*=CLASS_LOADED*/, 
                                                 BOOL dropGenericArgumentLevel /*=FALSE*/)
{
    WRAPPER_CONTRACT;

    MetaSig sig(this);
    sig.NextArg();
    
    return sig.GetLastTypeHandleThrowing(ClassLoader::LoadTypes, level, dropGenericArgumentLevel);
}

#ifndef DACCESS_COMPILE

void* FieldDesc::GetStaticAddress(void *base)
{

    WRAPPER_CONTRACT;

    void* ret = GetStaticAddressHandle(base);       // Get the handle

        // For value classes, the handle points at an OBJECTREF
        // which holds the boxed value class, so derefernce and unbox.  
    if (GetFieldType() == ELEMENT_TYPE_VALUETYPE && !IsRVA())
    {
        OBJECTREF obj = ObjectToOBJECTREF(*(Object**) ret);
        ret = obj->GetData();
    }
    return ret;
}

TypeHandle FieldDesc::GetExactDeclaringType(TypeHandle ownerOrSubType)
{
    WRAPPER_CONTRACT;
    return Generics::GetFieldDeclaringType(ownerOrSubType, this);
}

#endif // #ifndef DACCESS_COMPILE

// returns true if accesses to this field should make the assembly non-verifiable because
// of dangerous overlap in an explicit type.
//
// (NOTE: This implementation is quite conservative. It doesn't track nonverifiability on
// a field-by-field basis. Rather if the type has any nonverifiable fields, the whole type is
// convicted.)
BOOL FieldDesc::IsNonVerifiableExplicitField()
{
    WRAPPER_CONTRACT;
    return GetApproxEnclosingMethodTable()->GetClass()->HasNonVerifiablyOverLayedField();
}


    // static value classes are actually stored in their boxed form.  
    // this means that their address moves.  
void* FieldDesc::GetStaticAddressHandle(void *base)
{

    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_TOLERANT;
        PRECONDITION(IsStatic());
        PRECONDITION(GetEnclosingMethodTable()->IsRestored());
    }
    CONTRACTL_END

     g_IBCLogger.LogFieldDescsAccess(GetEnclosingClass());

    _ASSERTE(IsStatic());


    if (IsRVA()) 
    {        
        Module* pModule = GetModule();
        void *ret = pModule->GetRvaField(GetOffset());

        _ASSERTE(!pModule->IsPEFile() || !pModule->IsRvaFieldTls(GetOffset()));

        return(ret);
    }

    CONSISTENCY_CHECK(CheckPointer(base));

    void *ret = ((BYTE *) base + GetOffset());


    return ret;
}

// These routines encapsulate the operation of getting and setting
// fields.
void    FieldDesc::GetInstanceField(LPVOID o, VOID * pOutVal)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED() ) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED() ) GC_NOTRIGGER; else GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
        if (FORBIDGC_LOADER_USE_ENABLED() ) FORBID_FAULT; else INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    Object* obj = PTR_Object((TADDR)o);
      // We know that it isn't going to be null here.  Tell PREFIX that we know it.
    PREFIX_ASSUME(obj!= NULL);
  
    // Check whether we are getting a field value on a proxy. If so, then ask
    // remoting services to extract the value from the instance.
    if (obj->IsThunking())
    {
#ifndef DACCESS_COMPILE
        _ASSERTE(CTPMethodTable::IsTPMethodTable(obj->GetMethodTable()));

        Object *puo = (Object *) CRemotingServices::AlwaysUnwrap(obj);
        OBJECTREF unwrapped = ObjectToOBJECTREF(puo);
            
#ifdef PROFILING_SUPPORTED

        GCPROTECT_BEGIN(unwrapped); // protect from RemotingClientInvocationStarted

        BOOL fIsRemoted = FALSE;

        // If profiling is active, notify it that remoting stuff is kicking in,
        // if AlwaysUnwrap returned an identical object pointer which means that
        // we are definitely going through remoting for this access.
        if (CORProfilerTrackRemoting())
        {
            fIsRemoted = ((LPVOID)puo == o);
            if (fIsRemoted)
            {
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->RemotingClientInvocationStarted(
                        reinterpret_cast<ThreadID>(GetThread()));
            }
        }
#endif // PROFILING_SUPPORTED

        CRemotingServices::FieldAccessor(this, unwrapped, pOutVal, TRUE);

#ifdef PROFILING_SUPPORTED
        if (CORProfilerTrackRemoting() && fIsRemoted)
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->RemotingClientInvocationFinished(
                    reinterpret_cast<ThreadID>(GetThread()));
        }

        GCPROTECT_END();           // protect from RemotingClientInvocationStarted
            
#endif // PROFILING_SUPPORTED
        return;
#else
    DacNotImpl();
#endif // #ifndef DACCESS_COMPILE
    }

    _ASSERTE(IsFieldOfValueType() || !obj->IsThunking());

    // Unbox the value class
    if(IsFieldOfValueType())
    {
        o = ObjectToOBJECTREF(obj)->UnBox();
    }
    TADDR pFieldAddress = (TADDR)GetAddress(o);
    UINT cbSize = GetSize();
           
    switch (cbSize)
    {
    case 1:
        *(BYTE*)pOutVal = *PTR_BYTE(pFieldAddress);
        break;
        
    case 2:
        *(USHORT*)pOutVal = *PTR_USHORT(pFieldAddress);
        break;
        
    case 4:
        *(UINT32*)pOutVal = *PTR_UINT32(pFieldAddress);
        break;
        
    case 8:
        *(ULONG64*)pOutVal = *PTR_ULONG64(pFieldAddress);
        break;
        
    default:
        UNREACHABLE();
        break;
    }
}

#ifndef DACCESS_COMPILE

void    FieldDesc::SetInstanceField(LPVOID o, const VOID * pInVal)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END


    // Check whether we are setting a field value on a proxy or a marshalbyref
    // class. If so, then ask remoting services to set the value on the 
    // instance

    if(((Object*)o)->IsThunking())
    {
        Object *puo = (Object *) CRemotingServices::AlwaysUnwrap((Object*) o);
        OBJECTREF unwrapped = ObjectToOBJECTREF(puo);

#ifdef PROFILING_SUPPORTED

        GCPROTECT_BEGIN(unwrapped);

        BOOL fIsRemoted = FALSE;

        // If profiling is active, notify it that remoting stuff is kicking in,
        // if AlwaysUnwrap returned an identical object pointer which means that
        // we are definitely going through remoting for this access.

        if (CORProfilerTrackRemoting())
        {
            fIsRemoted = ((LPVOID)puo == o);
            if (fIsRemoted)
            {
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->RemotingClientInvocationStarted(
                    reinterpret_cast<ThreadID>(GetThread()));
            }
        }
#endif // PROFILING_SUPPORTED

        CRemotingServices::FieldAccessor(this, unwrapped, (void *)pInVal, FALSE);

#ifdef PROFILING_SUPPORTED
        if (CORProfilerTrackRemoting() && fIsRemoted)
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->RemotingClientInvocationFinished(
                reinterpret_cast<ThreadID>(GetThread()));
        }
        GCPROTECT_END();

#endif // PROFILING_SUPPORTED


    }
    else
    {
        _ASSERTE(IsFieldOfValueType() || !((Object*) o)->IsThunking());

#ifdef _DEBUG
        //
        // assert that o is derived from MT of enclosing class
        //
        // walk up o's inheritence chain to make sure m_pMTOfEnclosingClass is along it
        //
        MethodTable* pCursor = ((Object *)o)->GetMethodTable();

        while (pCursor && (GetApproxEnclosingMethodTable()->HasSameTypeDefAs(pCursor)))
        {
            pCursor = pCursor->GetParentMethodTable();
        }
        _ASSERTE(pCursor != NULL);
#endif // _DEBUG

        // Keep a hold of the MT of the in param in case we unbox just below.
        Object *oParamIn;
        oParamIn = (Object*)o;

        // Unbox the value class
        if(IsFieldOfValueType())
        {
            o = ObjectToOBJECTREF((Object *)o)->UnBox();
        }
        LPVOID pFieldAddress = GetAddress(o);
    
    
        CorElementType fieldType = GetFieldType();
    
        if (fieldType == ELEMENT_TYPE_CLASS)
        {
            OBJECTREF ref = ObjectToOBJECTREF(*(Object**)pInVal);

            SetObjectReference((OBJECTREF*)pFieldAddress, ref, 
                               (oParamIn)->GetAppDomain());
        }
        else if (fieldType == ELEMENT_TYPE_VALUETYPE)
        {
            CONSISTENCY_CHECK(!LookupFieldTypeHandle().IsNull());
            // The Approximate MT is enough to do the copy
            CopyValueClass(pFieldAddress,
                           (void*)pInVal,
                           LookupFieldTypeHandle().GetMethodTable(),
                           (oParamIn)->GetAppDomain());
        }
        else
        {
            UINT cbSize = LoadSize();
    
            switch (cbSize)
            {
                case 1:
                    *(volatile INT8*)pFieldAddress = *(INT8*)pInVal;
                    break;
    
                case 2:
                    *(volatile INT16*)pFieldAddress = *(INT16*)pInVal;
                    break;
    
                case 4:
                    *(volatile INT32*)pFieldAddress = *(INT32*)pInVal;
                    break;
    
                case 8:
                    *(volatile INT64*)pFieldAddress = *(INT64*)pInVal;
                    break;
    
                default:
                    UNREACHABLE();
                    break;
            }
        }
    }
}

#endif // #ifndef DACCESS_COMPILE

// This function is used for BYREF support of fields.  Since it generates
// interior pointers, you really have to watch the lifetime of the pointer
// so that GCs don't happen while you have the reference active
void *FieldDesc::GetAddressNoThrowNoGC( void *o)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        // This can be called by Server GC thread, so can't force cooperative
        if (GetThread() != NULL) {MODE_COOPERATIVE;} else {MODE_ANY;}
        SO_TOLERANT;
        PRECONDITION(!IsEnCNew());
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (IsFieldOfValueType())
    {
        return ((*((BYTE**) &o)) + GetOffset());
    }
    return ((*((BYTE**) &o)) + GetOffset() + sizeof(Object));
}
 
void *FieldDesc::GetAddress( void *o)
{
    STATIC_CONTRACT_SO_TOLERANT;
    CONTRACTL
    {
        if(IsEnCNew()) {THROWS;} else {DISABLED(THROWS);};
        if(IsEnCNew()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);};
        SO_TOLERANT;
    }
    CONTRACTL_END;

    g_IBCLogger.LogFieldDescsAccess(GetEnclosingClass());


    return GetAddressNoThrowNoGC(o);
}

// And here's the equivalent, when you are guaranteed that the enclosing instance of
// the field is in the GC Heap.  So if the enclosing instance is a value type, it had
// better be boxed.  We ASSERT this.
void *FieldDesc::GetAddressGuaranteedInHeap(void *o, BOOL doValidate)
{
    WRAPPER_CONTRACT;

#ifdef _DEBUG_IMPL
    Object *pObj = (Object *)o;
    if (doValidate)
        pObj->Validate();
#endif
    return ((*((BYTE**) &o)) + GetOffset() + sizeof(Object));
}


DWORD   FieldDesc::GetValue32(OBJECTREF o)
{
    WRAPPER_CONTRACT;

    DWORD val;
    GetInstanceField(o, (LPVOID)&val);
    return val;
}

#ifndef DACCESS_COMPILE

VOID    FieldDesc::SetValue32(OBJECTREF o, DWORD dwValue)
{
    WRAPPER_CONTRACT;

    SetInstanceField(o, (LPVOID)&dwValue);
}

#endif // #ifndef DACCESS_COMPILE

void*   FieldDesc::GetValuePtr(OBJECTREF o)
{
    WRAPPER_CONTRACT;

    void* val;
    GetInstanceField(o, (LPVOID)&val);
    return val;
}

#ifndef DACCESS_COMPILE

VOID    FieldDesc::SetValuePtr(OBJECTREF o, void* pValue)
{
    WRAPPER_CONTRACT;

    SetInstanceField(o, (LPVOID)&pValue);
}

#endif // #ifndef DACCESS_COMPILE

OBJECTREF FieldDesc::GetRefValue(OBJECTREF o)
{
    WRAPPER_CONTRACT;

    OBJECTREF val = NULL;

#ifdef PROFILING_SUPPORTED
    GCPROTECT_BEGIN(val);
#endif

    GetInstanceField(o, (LPVOID)&val);

#ifdef PROFILING_SUPPORTED
    GCPROTECT_END();
#endif

    return val;
}

#ifndef DACCESS_COMPILE

VOID    FieldDesc::SetRefValue(OBJECTREF o, OBJECTREF orValue)
{
    WRAPPER_CONTRACT;
    VALIDATEOBJECTREF(o);
    VALIDATEOBJECTREF(orValue);

    SetInstanceField(o, (LPVOID)&orValue);
}

#endif // #ifndef DACCESS_COMPILE

USHORT  FieldDesc::GetValue16(OBJECTREF o)
{
    WRAPPER_CONTRACT;

    USHORT val;
    GetInstanceField(o, (LPVOID)&val);
    return val;
}

#ifndef DACCESS_COMPILE

VOID    FieldDesc::SetValue16(OBJECTREF o, DWORD dwValue)
{
    WRAPPER_CONTRACT;

    USHORT val = (USHORT)dwValue;
    SetInstanceField(o, (LPVOID)&val);
}

#endif // #ifndef DACCESS_COMPILE

BYTE    FieldDesc::GetValue8(OBJECTREF o)
{
    WRAPPER_CONTRACT;

    BYTE val;
    GetInstanceField(o, (LPVOID)&val);
    return val;

}

#ifndef DACCESS_COMPILE

VOID    FieldDesc::SetValue8(OBJECTREF o, DWORD dwValue)
{
    WRAPPER_CONTRACT;

    BYTE val = (BYTE)dwValue;
    SetInstanceField(o, (LPVOID)&val);
}

#endif // #ifndef DACCESS_COMPILE

__int64 FieldDesc::GetValue64(OBJECTREF o)
{
    WRAPPER_CONTRACT;
    __int64 val;
    GetInstanceField(o, (LPVOID)&val);
    return val;

}

#ifndef DACCESS_COMPILE

VOID    FieldDesc::SetValue64(OBJECTREF o, __int64 value)
{
    WRAPPER_CONTRACT;
    SetInstanceField(o, (LPVOID)&value);
}


#endif // #ifndef DACCESS_COMPILE

UINT FieldDesc::LoadSize()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    CorElementType type = GetFieldType();
    UINT size = GetSizeForCorElementType(type);
    if (size == (UINT) -1)
    {
        //        LOG((LF_CLASSLOADER, LL_INFO10000, "FieldDesc::LoadSize %s::%s\n", GetApproxEnclosingMethodTable()->GetDebugClassName(), m_debugName));
        CONSISTENCY_CHECK(GetFieldType() == ELEMENT_TYPE_VALUETYPE);
        size = GetApproxFieldTypeHandleThrowing().GetMethodTable()->GetNumInstanceFieldBytes();
    }

    return size;
}

UINT FieldDesc::GetSize()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

    CorElementType type = GetFieldType();
    UINT size = GetSizeForCorElementType(type);
    if (size == (UINT) -1)
    {
        LOG((LF_CLASSLOADER, LL_INFO10000, "FieldDesc::GetSize %s::%s\n", GetApproxEnclosingMethodTable()->GetDebugClassName(), m_debugName));
        CONSISTENCY_CHECK(GetFieldType() == ELEMENT_TYPE_VALUETYPE);
        TypeHandle t = LookupApproxFieldTypeHandle();
        if (!t.IsNull())
        {
            size = t.GetMethodTable()->GetNumInstanceFieldBytes();
        }
    }

    return size;
}

// See field.h for details
TypeHandle* FieldDesc::GetExactClassInstantiation(TypeHandle possibleObjType)
{
    WRAPPER_CONTRACT;
    
    // We know that it isn't going to be null here.  Tell PREFIX that we know it.
    PREFIX_ASSUME(GetApproxEnclosingMethodTable()!=NULL);
    if (possibleObjType.IsNull())
    {
        return GetApproxEnclosingMethodTable()->GetInstantiation();
    }
    else
    {
        PREFIX_ASSUME(GetApproxEnclosingMethodTable()!=NULL);
        return possibleObjType.GetInstantiationOfParentClass(GetApproxEnclosingMethodTable());
    }
}

// Given, { List<String>, List<__Canon>._items } where _items is of type T[],
// it returns String[].

TypeHandle FieldDesc::GetExactFieldType(TypeHandle owner)
{
    CONTRACT(TypeHandle)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(owner, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END

    MethodTable *pApproxEnclosingMT = GetApproxEnclosingMethodTable();
    TypeHandle thApproxFieldType = GetApproxFieldTypeHandleThrowing();
    if (!IsSharedByGenericInstantiations() ||
        // Could be "T[]" which does not need to be resolved further
        // Also, SigTypeContext below did not seem to be handling arrays properly.
        !thApproxFieldType.IsUnsharedMT())
    {
        RETURN(GetFieldTypeHandleThrowing());
    }
    else
    {
        // We need to figure out the precise type of the field.
        // First, get the signature of the field
        PCCOR_SIGNATURE pSig;
        DWORD cSig;
        GetSig(&pSig, &cSig);
        SigPointer sig(pSig, cSig);

        // Get the generics information
        TypeHandle *inst = GetExactClassInstantiation(owner);
        SigTypeContext sigTypeContext(pApproxEnclosingMT->GetNumGenericArgs(), inst, 0, NULL);

        // Load the exact type
        RETURN (sig.GetTypeHandleThrowing(thApproxFieldType.GetModule(), &sigTypeContext));
    }
}    

Module* FieldDesc::GetZapModule() 
{
    WRAPPER_CONTRACT;
    return ExecutionManager::FindZapModule(PTR_HOST_TO_TADDR(this));
}

#ifndef DACCESS_COMPILE
#endif // DACCESS_COMPILE


