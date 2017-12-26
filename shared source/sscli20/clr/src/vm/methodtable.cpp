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
// File: METHODTABLE.CPP
//

#include "common.h"

#include "clsload.hpp"
#include "method.hpp"
#include "class.h"
#include "object.h"
#include "field.h"
#include "util.hpp"
#include "excep.h"
#include "siginfo.hpp"
#include "threads.h"
#include "stublink.h"
#include "ecall.h"
#include "dllimport.h"
#include "gcdesc.h"
#include "verifier.hpp"
#include "jitinterface.h"
#include "eeconfig.h"
#include "log.h"
#include "fieldmarshaler.h"
#include "cgensys.h"
#include "gc.h"
#include "security.h"
#include "comstringbuffer.h"
#include "dbginterface.h"
#include "comdelegate.h"
#include "remoting.h"
#include "eeprofinterfaces.h"
#include "dllimportcallback.h"
#include "listlock.h"
#include "methodimpl.h"
#include "guidfromname.h"
#include "stackprobe.h"
#include "encee.h"
#include "encee.h"
#include "comsynchronizable.h"
#include "customattribute.h"
#include "virtualcallstub.h"
#include "contractimpl.h"

#include "hostexecutioncontext.h"



#include "listlock.inl"
#include "generics.h"
#include "genericdict.h"
#include "typestring.h"
#include "typedesc.h"
#include "crossdomaincalls.h"
#include "array.h"

#ifndef DACCESS_COMPILE

//==========================================================================================-----------
// The following is needed to monitor RVA fields overlapping in InitializeFieldDescs
//
#define RVA_FIELD_VALIDATION_ENABLED
//#define RVA_FIELD_OVERLAPPING_VALIDATION_ENABLED
#include "../ildasm/dynamicarray.h"

#define UNPLACED_NONVTABLE_SLOT_NUMBER ((WORD) -2)

#include "assembly.hpp"

// Typedef for string comparition functions.
typedef int (__cdecl *UTF8StringCompareFuncPtr)(const char *, const char *);


MetaSig    *MethodTable::s_cctorSig;

MethodTable *MethodTable::s_ISerializableMT = NULL;
MethodTable *MethodTable::s_IDeserializationCBMT = NULL;
MethodTable *MethodTable::s_IObjectReferenceMT = NULL;

MethodDataCache *MethodTable::s_pMethodDataCache = NULL;
BOOL MethodTable::s_fUseMethodDataCache = FALSE;
BOOL MethodTable::s_fUseParentMethodData = FALSE;
BOOL MethodTable::s_fUseMethodDataCacheInFindDispatchImpl = FALSE;

#ifdef _DEBUG
extern unsigned g_dupMethods;
#endif

#endif // !DACCESS_COMPILE

#ifndef DACCESS_COMPILE
//==========================================================================================
class MethodDataCache
{
    typedef MethodTable::MethodData MethodData;

  public:    // Ctor. Allocates cEntries entries. Throws.
    static UINT32 GetObjectSize(UINT32 cEntries);
    MethodDataCache(UINT32 cEntries);

    MethodData *Find(MethodTable *pMT);
    MethodData *Find(MethodTable *pMTDecl, MethodTable *pMTImpl);
    void Insert(MethodData *pMData);
    void Clear();

  protected:
    // This describes each entry in the cache.
    struct Entry
    {
        MethodData *m_pMData;
        UINT32      m_iTimestamp;
    };

    MethodData *FindHelper(MethodTable *pMTDecl, MethodTable *pMTImpl, UINT32 idx);

    inline UINT32 GetNextTimestamp()
        { return ++m_iCurTimestamp; }

    inline UINT32 NumEntries()
        { LEAF_CONTRACT; return m_cEntries; }

    inline void TouchEntry(UINT32 i)
        { WRAPPER_CONTRACT; m_iLastTouched = i; GetEntry(i)->m_iTimestamp = GetNextTimestamp(); }

    inline UINT32 GetLastTouchedEntryIndex()
        { WRAPPER_CONTRACT; return m_iLastTouched; }

    // The end of this object contains an array of Entry
    inline Entry *GetEntryData()
        { LEAF_CONTRACT; return (Entry *)(this + 1); }

    inline Entry *GetEntry(UINT32 i)
        { WRAPPER_CONTRACT; return GetEntryData() + i; }

  private:
    // This serializes access to the cache
    SimpleRWLock    m_lock;

    // This allows ageing of entries to decide which to punt when
    // inserting a new entry.
    UINT32 m_iCurTimestamp;

    // The number of entries in the cache
    UINT32 m_cEntries;
    UINT32 m_iLastTouched;

};

//==========================================================================================
UINT32 MethodDataCache::GetObjectSize(UINT32 cEntries)
{
    LEAF_CONTRACT;
    return sizeof(MethodDataCache) + (sizeof(Entry) * cEntries);
}

//==========================================================================================
MethodDataCache::MethodDataCache(UINT32 cEntries)
    : m_lock(COOPERATIVE_OR_PREEMPTIVE, LOCK_TYPE_DEFAULT),
      m_iCurTimestamp(0),
      m_cEntries(cEntries),
      m_iLastTouched(0)
{
    WRAPPER_CONTRACT;
    ZeroMemory(GetEntryData(), cEntries * sizeof(Entry));
}

//==========================================================================================
MethodTable::MethodData *MethodDataCache::FindHelper(
    MethodTable *pMTDecl, MethodTable *pMTImpl, UINT32 idx)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
    } CONTRACTL_END;

    MethodData *pEntry = GetEntry(idx)->m_pMData;
    if (pEntry != NULL) {
        MethodTable *pMTDeclEntry = pEntry->GetDeclMethodTable();
        MethodTable *pMTImplEntry = pEntry->GetImplMethodTable();
        if (pMTDeclEntry == pMTDecl && pMTImplEntry == pMTImpl) {
            return pEntry;
        }
        else if (pMTDecl == pMTImpl) {
            if (pMTDeclEntry == pMTDecl) {
                return pEntry->GetDeclMethodData();
            }
            if (pMTImplEntry == pMTDecl) {
                return pEntry->GetImplMethodData();
            }
        }
    }

    return NULL;
}

//==========================================================================================
MethodTable::MethodData *MethodDataCache::Find(MethodTable *pMTDecl, MethodTable *pMTImpl)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
    } CONTRACTL_END;

#ifdef LOGGING
    g_sdStats.m_cCacheLookups++;
#endif

    SimpleReadLockHolder lh(&m_lock);

    // Check the last touched entry.
    MethodData *pEntry = FindHelper(pMTDecl, pMTImpl, GetLastTouchedEntryIndex());

    // Now search the entire cache.
    if (pEntry == NULL) {
        for (UINT32 i = 0; i < NumEntries(); i++) {
            pEntry = FindHelper(pMTDecl, pMTImpl, i);
            if (pEntry != NULL) {
                TouchEntry(i);
                break;
            }
        }
    }

    if (pEntry != NULL) {
        pEntry->AddRef();
    }

#ifdef LOGGING
    else {
        // Failure to find the entry in the cache.
        g_sdStats.m_cCacheMisses++;
    }
#endif // LOGGING

    return pEntry;
}

//==========================================================================================
MethodTable::MethodData *MethodDataCache::Find(MethodTable *pMT)
{
    WRAPPER_CONTRACT;
    return Find(pMT, pMT);
}

//==========================================================================================
void MethodDataCache::Insert(MethodData *pMData)
{
    CONTRACTL {
        NOTHROW; // for now, because it does not yet resize.
        GC_NOTRIGGER;
        INSTANCE_CHECK;
    } CONTRACTL_END;

    SimpleWriteLockHolder hLock(&m_lock);

    UINT32 iMin = UINT32_MAX;
    UINT32 idxMin = UINT32_MAX;
    for (UINT32 i = 0; i < NumEntries(); i++) {
        if (GetEntry(i)->m_iTimestamp < iMin) {
            idxMin = i;
            iMin = GetEntry(i)->m_iTimestamp;
        }
    }
    Entry *pEntry = GetEntry(idxMin);
    if (pEntry->m_pMData != NULL) {
        pEntry->m_pMData->Release();
    }
    pMData->AddRef();
    pEntry->m_pMData = pMData;
    pEntry->m_iTimestamp = GetNextTimestamp();
}

//==========================================================================================
void MethodDataCache::Clear()
{
    CONTRACTL {
        NOTHROW; // for now, because it does not yet resize.
        GC_NOTRIGGER;
        INSTANCE_CHECK;
    } CONTRACTL_END;

    // Taking the lock here is just a precaution. Really, the runtime
    // should be suspended because this is called while unloading an
    // AppDomain at the SysSuspendEE stage. But, if someone calls it
    // outside of that context, we should be extra cautious.
    SimpleWriteLockHolder lh(&m_lock);

    for (UINT32 i = 0; i < NumEntries(); i++) {
        Entry *pEntry = GetEntry(i);
        if (pEntry->m_pMData != NULL) {
            pEntry->m_pMData->Release();
        }
    }
    ZeroMemory(GetEntryData(), NumEntries() * sizeof(Entry));
    m_iCurTimestamp = 0;
}

#endif // !DACCESS_COMPILE

//==========================================================================================
#ifdef _MSC_VER
#ifndef _DEBUG
// Optimization intended for MethodTable::GetModule only
#pragma optimize("t", on)
#endif
#endif
Module* MethodTable::GetModule()
{
    WRAPPER_CONTRACT;
    Module *pModule = NULL;

    if (!HasInstantiation() && !IsArray())
    {
        pModule = GetLoaderModule();
    }
    else
    {
        pModule = GetClass()->GetModule();
    }

    PREFIX_ASSUME(pModule != NULL);
    CONSISTENCY_CHECK(pModule == GetClass()->GetModule());
    return pModule;
}
#ifdef _MSC_VER
#ifndef _DEBUG
#pragma optimize("", on)
#endif
#endif

//==========================================================================================
BOOL MethodTable::ValidateWithPossibleAV()
{
    CANNOT_HAVE_CONTRACT;


    //
    //
    //
    EEClass* pEEClass = this->GetClassWithPossibleAV();
    return ((this == pEEClass->GetMethodTableWithPossibleAV()) ||
        ((pEEClass->HasInstantiation()) &&
        (pEEClass->GetMethodTableWithPossibleAV()->GetClassWithPossibleAV() == pEEClass)));
}

//==========================================================================================
BOOL  MethodTable::IsClassInited(AppDomain* pAppDomain /* = NULL */)
{
    WRAPPER_CONTRACT;

    if (IsClassPreInited())
        return TRUE;

    DomainLocalModule *pLocalModule = GetDomainLocalModule(pAppDomain);
    _ASSERTE(pLocalModule != NULL);

    return pLocalModule->IsClassInitialized(this);
}

BOOL  MethodTable::IsClassInitedHaveIndex(DWORD iClassIndex)
{
    WRAPPER_CONTRACT;

    if (IsClassPreInited())
        return TRUE;

    DomainLocalModule *pLocalModule = GetDomainLocalModule();
    _ASSERTE(pLocalModule != NULL);

    return pLocalModule->IsClassInitialized(this, iClassIndex);
}


//==========================================================================================
BOOL  MethodTable::IsInitError()
{
    WRAPPER_CONTRACT;

    DomainLocalModule *pLocalModule = GetDomainLocalModule();
    _ASSERTE(pLocalModule != NULL);

    return pLocalModule->IsClassInitError(this);
}

#ifndef DACCESS_COMPILE

//==========================================================================================
// mark the class as having no class init logic required (this includes
// running .cctor and/or setting up any statics).
void MethodTable::SetClassPreInited()
{
    LEAF_CONTRACT;
    // We require certain conditions to be able to mark a class as preinited
    // Make sure we are only called on MTs that meet them.
    _ASSERTE(ComputeIsPreInit());
    m_wFlags2 |= enum_flag2_ClassPreInited;
}

//==========================================================================================
// mark the class as having its .cctor run
void MethodTable::SetClassInited()
{
    WRAPPER_CONTRACT;
    _ASSERTE(!IsClassPreInited() || g_Mscorlib.IsClass(this, CLASS__SHARED_STATICS));
    GetDomainLocalModule()->SetClassInitialized(this);
}

//==========================================================================================
void MethodTable::SetClassInitError()
{
    WRAPPER_CONTRACT;
    GetDomainLocalModule()->SetClassInitError(this);
}

//==========================================================================================
// mark the class as having been restored.
void MethodTable::SetIsRestored()
{
    LEAF_CONTRACT;
    PRECONDITION(!IsFullyLoaded());
    g_IBCLogger.LogMethodTableWriteableDataWriteAccess(this);
    FastInterlockAnd(&(GetWriteableData()->m_dwFlags), ~MethodTableWriteableData::enum_flag_Unrestored);
}

//==========================================================================================
// mark as transparent proxy type
void MethodTable::SetTransparentProxyType()
{
    LEAF_CONTRACT;
    if (!(m_wFlags & enum_flag_TransparentProxy)) {
        g_IBCLogger.LogMethodTableWriteAccess(this);
    m_wFlags |= enum_flag_TransparentProxy;
    }

}


//==========================================================================================
MethodDesc* MethodTable::GetBoxedEntryPointMD(MethodDesc *pMD)
{
    // Unboxing method descriptors are stored in the class.  They are
    // exact-instantiation-neutral, because all the necessary instantiation
    // information is held in the object being invoked.  Thus we can simply
    // use the shared unboxing descriptor in the canonical EEClass.
    return GetClass()->GetBoxedEntryPointMD(pMD);
}

#endif // !DACCESS_COMPILE

//==========================================================================================
BaseDomain *MethodTable::GetDomain()
{
    WRAPPER_CONTRACT;
    g_IBCLogger.LogMethodTableAccess(this);

    return GetLoaderModule()->GetDomain();
}

//==========================================================================================
//
// Background: the ComputedDomainNeutrality of a type is a computation
// based on the constituents of a type, e.g. List<C> is domain-neutral if
// both List and C are domain-neutral types.  The CompileTimeDomainNeutrality
// of a type is whether the type was saved into an image generated with "/shared".
//
// The good news is that these two are identical for:
//     1. non-generic, non-array types
//     2  array types saved in their PreferredZapModule
//     3. generic instantiations saved in their PreferredZapModule
//
// The bad news is that they are not a-prioiri the same for
//     4. array types saved outside their PreferredZapModule
//     5. generic instantiations saved outside their PreferredZapModule
//
// (Note 4 & 5 are controlled by the switches FullArraysNGen and FullGenericsNGEN)
//
// However by the time we have actually loaded code these are guaranteed to match up.
// In particular, a constructed type will only actually be used at runtime if
// ComputedDomainNeutrality of the image matches the CompileTimeDomainNeutrality.
//
// The flag stored in the MethodTable is the CompileTimeDomainNeutrality
// of the type, and in the NGEN case must be the same as the CompileTimeDomainNeutrality
// of the image we are generating.
BOOL MethodTable::IsDomainNeutral()
{
    WRAPPER_CONTRACT;

    BOOL fDomainNeutral = IsCompiledDomainNeutral();

    // Even if a type was compiled domain neutral during ngen time, it might be
    // loaded into an app domain.  Thus we always check the container flag first in
    // case it was changed.
    if (fDomainNeutral)
    {
        if (IsZapped())
            return GetZapModule()->GetAssembly()->IsDomainNeutral();
    }
    return fDomainNeutral;
}

//==========================================================================================
void MethodTable::SetCompiledDomainNeutral(BOOL fDomainNeutral)
{
    LEAF_CONTRACT;


    if (fDomainNeutral)
        m_wFlags |= enum_flag_CompiledDomainNeutral;
    else
        m_wFlags &= ~enum_flag_CompiledDomainNeutral;
}


//==========================================================================================
BOOL MethodTable::HasSameTypeDefAs(MethodTable *pMT)
{
    WRAPPER_CONTRACT;
    return (this == pMT)
        || (this->GetClass() == pMT->GetClass())
        || this->GetClass()->HasSameTypeDefAs(pMT->GetClass());
}

//==========================================================================================
BOOL MethodTable::InterfaceMapIterator::InterfaceEquals(MethodTable *pMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(pMT->IsRestored());
    }
    CONTRACTL_END;

    if (GetInterfaceInfo()->m_pMethodTable == pMT)
        return TRUE;

#ifndef DACCESS_COMPILE
#endif // !DACCESS_COMPILE

    return FALSE;
}

#ifndef DACCESS_COMPILE

//==========================================================================================
void MethodTable::SetMarshaledByRef()
{
    LEAF_CONTRACT;
    m_wFlags2 |= enum_flag2_MarshaledByRef;

#if defined(CHECK_APP_DOMAIN_LEAKS) || defined(_DEBUG)
    GetClass()->SetAuxFlags(EEClass::AUXFLAG_CLASS_IS_MARSHALED_BY_REF);
#endif
}



//==========================================================================================
// get the method desc given the interface method desc
/* static */ MethodDesc *MethodTable::GetMethodDescForInterfaceMethodAndServer(
                            TypeHandle ownerType, MethodDesc *pItfMD, OBJECTREF *pServer)
{
    CONTRACT(MethodDesc*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pItfMD));
        PRECONDITION(pItfMD->IsInterface());
        PRECONDITION(!ownerType.IsNull());
        PRECONDITION(ownerType.GetMethodTable()->HasSameTypeDefAs(pItfMD->GetMethodTable()));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    VALIDATEOBJECTREF(*pServer);

#ifdef _DEBUG
    MethodTable * pItfMT =  ownerType.GetMethodTable();
    PREFIX_ASSUME(pItfMT != NULL);
#endif // _DEBUG

    MethodTable *pServerMT = (*pServer)->GetMethodTable();
    PREFIX_ASSUME(pServerMT != NULL);

    if (pServerMT->IsThunking())
    {
        // If pServer is a TP, then the interface method desc is the one to
        // use to dispatch the call.
        RETURN(pItfMD);
    }
    else if (!pServerMT->IsComObjectType())
    {
        // Handle pure COM+ types.
        RETURN (pServerMT->GetMethodDescForInterfaceMethod(ownerType, pItfMD));
    }
    RETURN(NULL);
}


//==========================================================================================
// This is a helper routine to get the address of code from the server and method descriptor
// It is used by remoting to figure out the address to which the method call needs to be
// dispatched.
const BYTE *MethodTable::GetTargetFromMethodDescAndServer(TypeHandle ownerType,
                                                          MethodDesc *pMD,
                                                          OBJECTREF *ppServer,
                                                          BOOL fContext,
                                                          MethodDesc **ppTargetMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(!pMD->ContainsGenericVariables());
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    const BYTE* pTarget = NULL;

    // Instantions of virtual generic methods get handled by ResolveGenericVirtualMethod
    if (pMD->HasMethodInstantiation() && (pMD->IsInterface() || pMD->IsVtableMethod()))
    {
        MethodDesc *pResultMD = pMD->ResolveGenericVirtualMethod(ppServer);
        _ASSERTE(pResultMD);
        _ASSERTE(pResultMD->HasMethodInstantiation());

        pTarget = pResultMD->GetMultiCallableAddrOfCode(); // GetAddrofCode();
        if (ppTargetMD)
        {
            *ppTargetMD = pResultMD;
        }
        return pTarget;
    }

    MethodDesc *pPartlyResolvedMD = pMD;
    if(pMD->GetMethodTable()->IsInterface() && !pMD->IsStatic())
    {
        _ASSERTE(*ppServer != NULL);

        // NOTE: This method can trigger GC
        pPartlyResolvedMD = (*ppServer)->GetMethodTable()->GetMethodDescForInterfaceMethodAndServer(ownerType, pMD, ppServer);
        if(!pPartlyResolvedMD)
        {
            LPCWSTR szClassName;
            DefineFullyQualifiedNameForClassW();
            szClassName = GetFullyQualifiedNameForClassW(pMD->GetClass());

            MAKE_WIDEPTR_FROMUTF8(szMethodName, pMD->GetName());

            COMPlusThrow(kMissingMethodException, IDS_EE_MISSING_METHOD, szClassName, szMethodName);
        }
    }

    _ASSERTE(pPartlyResolvedMD != NULL);

    if (ppTargetMD)
    {
        *ppTargetMD = pPartlyResolvedMD;
    }

    // get the target depending on whether the method is virtual or non-virtual
    // like a constructor, private or final method

    if (pPartlyResolvedMD->GetMethodTable()->IsInterface())
    {
        // Handle the special cases where the invoke is happening through an interface class
        // (typically for COM interop).
            pTarget = pPartlyResolvedMD->GetSafeAddrofCode();
    }
    else
    {
        if (pPartlyResolvedMD->IsVtableMethod())
        {
            pTarget = pPartlyResolvedMD->GetMultiCallableAddrOfVirtualizedCode(ppServer, ownerType);
        }
        else
        {
            pTarget = pPartlyResolvedMD->GetMultiCallableAddrOfCode(); // GetSafeAddrofCode();
        }
    }

    _ASSERTE(NULL != pTarget);

    return pTarget;
}

//==========================================================================================
// Static helper to create a new method table. This is the only
// way to allocate a new MT. Don't try calling new / ctor.
// Called from SetupMethodTable
// This needs to be kept consistent with GetExtent()
MethodTable * MethodTable::AllocateNewMT(EEClass *pClass,
                                         DWORD dwVtableSlots,
                                         DWORD dwGCSize,
                                         DWORD dwNumInterfaces,
                                         DWORD numGenericArgs,
                                         DWORD dwNumDicts,
                                         DWORD cbDict,
                                         ClassLoader *pClassLoader,
                                         BaseDomain *pDomain,
                                         BOOL isInterface,
                                         BOOL fHasGenericsStaticsInfo,
                                         BOOL fNeedsRemotableMethodInfo,
                                         BOOL fNeedsRemotingVtsInfo,
                                         BOOL fHasThreadOrContextStatics
        , AllocMemTracker *pamTracker
    )
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // GCSize must be aligned
    _ASSERTE(IS_ALIGNED(dwGCSize, sizeof(void*)));

    size_t size = sizeof(MethodTable);

    // size without the interface map
    //@GENERICS: reserve space for the dictionary pointers
    DWORD cbTotalSize = 0;
    DWORD cbDicts = 0;
    if (!ClrSafeInt<DWORD>::multiply(dwNumDicts, sizeof(TypeHandle*), cbDicts) ||
        !ClrSafeInt<DWORD>::addition((DWORD)size, cbDicts, cbTotalSize) ||
        !ClrSafeInt<DWORD>::addition(cbTotalSize, dwGCSize, cbTotalSize))
        ThrowHR(COR_E_OVERFLOW);

    DWORD extra = 0;
    if (!ClrSafeInt<DWORD>::multiply(dwVtableSlots, sizeof(SLOT), extra) ||
        !ClrSafeInt<DWORD>::addition(cbTotalSize, extra, cbTotalSize))
        ThrowHR(COR_E_OVERFLOW);

    // Add space for optional members here. Same as GetOptionalMembersSize()
    if (!ClrSafeInt<DWORD>::addition(
        cbTotalSize,
        MethodTable::GetOptionalMembersAllocationSize(dwNumInterfaces,
                                                      fNeedsRemotableMethodInfo,
                                                      fHasGenericsStaticsInfo,
                                                      fNeedsRemotingVtsInfo,
                                                      dwNumDicts,
                                                      fHasThreadOrContextStatics),
        cbTotalSize))
        ThrowHR(E_INVALIDARG);

    //
    // the pointer to the interface map must be pointer-size aligned
    //
    DWORD cbTotalSizeAligned = (DWORD)ALIGN_UP(cbTotalSize, sizeof(void*));
    if (cbTotalSize > cbTotalSizeAligned)
        ThrowHR(E_INVALIDARG);
    cbTotalSize = cbTotalSizeAligned;

    // size with the interface map and fixed-size dictionary. DynamicInterfaceMap
    // have an extra DWORD added to the end of the normal interface
    // map. This will be used to store the count of dynamically added interfaces
    // (the ones that are not in  the metadata but are QI'ed for at runtime).
    DWORD newSize = cbTotalSize;
    DWORD dwInterfacesSize = 0;


    if (!ClrSafeInt<DWORD>::multiply(dwNumInterfaces, sizeof(InterfaceInfo_t), dwInterfacesSize) ||
        !ClrSafeInt<DWORD>::addition(newSize, dwInterfacesSize, newSize) ||
        !ClrSafeInt<DWORD>::addition(newSize, cbDict, newSize))
        ThrowHR(E_INVALIDARG);

    //
    // on 64-bit we want to make sure that these guys are allocated such that
    // the pMT can be cacheline size aligned, this is especially important as the MT
    // is == sizeof a cache line, so aligning it ensures that we will manage to 
    // fit the whole thing into a cache line.
    //
    // It it not important how big cache_line_size is (if you care about it) as long as 
    // it is > sizeof(MethodTable) and < actual cache line size (e.g. IA64 is actually 128).
    //
#if defined(_X86_)
    // For x86 right now we'll just align these guys on pointer size alignment
    const int cache_line_size = sizeof(void*);
#else
    const int cache_line_size = sizeof(void*);
#endif

    int padding = 0;
    if (!ClrSafeInt<int>::addition((int)ALIGN_UP((size_t)dwGCSize, cache_line_size), -(int)dwGCSize, padding))
        ThrowHR(E_INVALIDARG);

    _ASSERTE(padding >= 0 && padding < cache_line_size);
    _ASSERTE(IS_ALIGNED(padding, sizeof(size_t)));

    size_t extra1 = 0;

    
    // padding is used to ensure that pMT will end up cache_line_size aligned
    // so first we allocate a block that is cache_line_size aligned and then we move
    // pData into the block enough so that (pData + dwGCSize) is cache_line_size aligned.
    BYTE *pData = (BYTE *)pamTracker->Track(pDomain->GetHighFrequencyHeap()->AllocAlignedMem(newSize + padding, cache_line_size, &extra1));
    pData += padding;

    MethodTable* pMT = (MethodTable*)(pData + dwGCSize);
    _ASSERTE(IS_ALIGNED(pMT, cache_line_size));
    _ASSERTE(IS_ALIGNED(pData, sizeof(size_t)));


    BYTE* pMTWriteableData = (BYTE *) pamTracker->Track(pDomain->GetHighFrequencyHeap()->AllocAlignedMem(sizeof(MethodTableWriteableData), sizeof(MethodTableWriteableData), &extra1));
    // Note: Memory allocated on loader heap is zero filled
    pMT->SetWriteableData((PTR_MethodTableWriteableData)pMTWriteableData);

    pMT->ClearFlags();   // clear flags without touching m_ComponentSize

#ifdef _DEBUG
    pClassLoader->m_dwGCSize += dwGCSize;
    pClassLoader->m_dwInterfaceMapSize += (dwNumInterfaces * sizeof(InterfaceInfo_t));
    pClassLoader->m_dwMethodTableSize += (DWORD)size;
    pClassLoader->m_dwVtableData += (dwVtableSlots * sizeof(SLOT));
#endif // _DEBUG

    // initialize the total number of slots
    pMT->SetNumMethods(dwVtableSlots);


    // the dictionary pointers precede the interface map
    if (dwNumDicts)
    {
        pMT->SetPerInstInfo (((Dictionary**) (pData + cbTotalSize)) - dwNumDicts);

        // Fill in the dictionary for this type, if it's instantiated
        if (cbDict)
        {
            if (dwNumDicts-1 > dwNumDicts)
            {
                // This would cause an underflow on the next statement,
                // so we have to throw
                ThrowHR(E_INVALIDARG);
            }

            PREFIX_ASSUME(dwNumDicts-1 < dwNumDicts);

            *(pMT->GetPerInstInfo() + (dwNumDicts-1)) = (Dictionary*) (pData + newSize - cbDict);
            memset(pData + newSize - cbDict, 0, cbDict);
        }
    }
    else
        pMT->SetPerInstInfo(NULL);

    {
        // interface map is at the end of the vtable
        pMT->SetInterfaceMap ((WORD) dwNumInterfaces, (InterfaceInfo_t *)(pData+cbTotalSize));
    }

    // we use the debug version of the IMap ptr here because
    // we haven't set the special debug only interface info at the beginging
    // of the map. If we use GetInterfaceMap() instead of the m_pIMapDEBUG here then
    // we need to set the invalid slot first (done below)
    _ASSERTE(IS_ALIGNED(pMT->m_pIMapDEBUG, sizeof(void*)));

    _ASSERTE(((WORD) dwNumInterfaces) == dwNumInterfaces);

#ifdef _DEBUG
    pMT->m_dwLastVerifedGCCnt = (DWORD)-1;
#endif // _DEBUG

    pMT->SetClass(pClass);

    RETURN(pMT);
}

//==========================================================================================
void MethodTable::SetupRemotableMethodInfo(BaseDomain *pDomain, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    // Make RMI for a method table.
    CrossDomainOptimizationInfo *pRMIBegin = NULL;
    if (GetNumMethods() > 0)
    {
        DWORD requiredSize = RemotableMethodInfo::GetSizeOfRemotableMethodInfo(this);
        pRMIBegin = (CrossDomainOptimizationInfo*) pamTracker->Track(pDomain->GetLowFrequencyHeap()->AllocMem(requiredSize));
        _ASSERTE(IS_ALIGNED(pRMIBegin, sizeof(void*)));
    }
    *(GetRemotableMethodInfoPtr()) = pRMIBegin;
}

//==========================================================================================
PTR_RemotingVtsInfo MethodTable::AllocateRemotingVtsInfo(BaseDomain *pDomain, AllocMemTracker *pamTracker, DWORD dwNumFields)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Size the data structure to contain enough bit flags for all the
    // instance fields.
    DWORD cbInfo = RemotingVtsInfo::GetSize(dwNumFields);
    RemotingVtsInfo *pInfo = (RemotingVtsInfo*)pamTracker->Track(pDomain->GetLowFrequencyHeap()->AllocMem(cbInfo));

    // Note: Memory allocated on loader heap is zero filled
    // ZeroMemory(pInfo, cbInfo);

#ifdef _DEBUG
    pInfo->m_dwNumFields = dwNumFields;
#endif

    *(GetRemotingVtsInfoPtr()) = pInfo;

    return pInfo;
}




//==========================================================================================
void MethodTable::SetInterfaceMap(WORD wNumInterfaces, InterfaceInfo_t* iMap)
{
    LEAF_CONTRACT;
    if (wNumInterfaces == 0)
    {
#ifdef _DEBUG
        m_pIMapDEBUG = NULL;
#endif
        _ASSERTE(!HasInterfaceMap());
        return;
    }
#ifdef _DEBUG
    m_pIMapDEBUG = iMap;
#endif
    m_wNumInterfaces = wNumInterfaces;
    *(GetInterfaceMapPtr()) = iMap;
    CONSISTENCY_CHECK(IS_ALIGNED(GetInterfaceMap(), sizeof(void*)));
}



void MethodTable::SetupGenericsStaticsInfo(FieldDesc* pStaticFieldDescs)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // No need to generate IDs for open types.  Indeed since we don't save them
    // in the NGEN image it would be actively incorrect to do so.  However
    // we still leave the optional member in the MethodTable holding the value 0 for the ID.

    GenericsStaticsInfo *pInfo = GetGenericsStaticsInfo();
    if (!ContainsGenericVariables())
    {
        pInfo->m_pModuleForStatics = GetLoaderModule();

        pInfo->m_DynamicTypeID = pInfo->m_pModuleForStatics->AllocateDynamicEntry(this);
    }
    else
    {
        pInfo->m_pModuleForStatics = NULL;
        pInfo->m_DynamicTypeID = (SIZE_T)-1;
    }

    pInfo->m_pFieldDescs = pStaticFieldDescs;
}

#endif // !DACCESS_COMPILE

//==========================================================================================
Module* MethodTable::GetModuleForStatics()
{
    WRAPPER_CONTRACT;

    if (HasGenericsStaticsInfo())
    {
        return GetGenericsStaticsInfo()->m_pModuleForStatics;
    }
    else
    {
        return GetLoaderModule();
    }
}

//==========================================================================================
DWORD  MethodTable::GetModuleDynamicEntryID()
{
    WRAPPER_CONTRACT;

    _ASSERTE(IsDynamicStatics() && "Only memory reflection emit types and generics can have a dynamic ID");

    if (HasGenericsStaticsInfo())
    {
        return (DWORD) (GetGenericsStaticsInfo()->m_DynamicTypeID);
    }
    else
    {
        return GetClass()->GetModuleDynamicID();
    }
}

//==========================================================================================
BOOL MethodTable::ParentEquals(MethodTable *pMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    g_IBCLogger.LogMethodTableAccess(this);
    if (m_pParentMethodTable == pMT)
        return TRUE;

#ifndef DACCESS_COMPILE
#endif // !DACCESS_COMPILE

    return FALSE;
}

#ifndef DACCESS_COMPILE

//==========================================================================================
// Notice whether this class requires finalization
void MethodTable::MaybeSetHasFinalizer()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE(!HasFinalizer());      // one shot

    // This method is called after we've built the MethodTable.  Since we always
    // load parents before children, this also guarantees that g_pObjectClass is
    // loaded (though the variable may not have been initialized yet if we are
    // just finishing the load of "Object".
    if (g_pObjectClass && !IsInterface() && !IsValueClass())
    {
        WORD    slot = g_pObjectFinalizerMD->GetSlot();

        // Structs and other objects not derived from Object will get marked as
        // having a finalizer, if they have sufficient virtual methods.  This will
        // only be an issue if they can be allocated in the GC heap (which will
        // cause all sorts of other problems).
        //
        // We are careful to check that we have a method that is distinct from both
        // the JITted and unJITted (prestub) addresses of Object's Finalizer.
        if (GetNumVirtuals() >= slot &&
            !(g_pObjectFinalizerMD->HasTemporaryEntryPoint() && GetSlot(slot) == (PBYTE)g_pObjectFinalizerMD->GetTemporaryEntryPoint()) &&
            !(g_pObjectFinalizerMD->HasStableEntryPoint()    && GetSlot(slot) == (PBYTE)g_pObjectFinalizerMD->GetStableEntryPoint()))
        {
            m_wFlags |= enum_flag_HasFinalizer;
        }

        // The need for a critical finalizer can be inherited from a parent.
        // Since we set this automatically for CriticalFinalizerObject
        // elsewhere, the code below is the means by which any derived class
        // picks up the attribute.
        if (GetParentMethodTable() != NULL && GetParentMethodTable()->HasCriticalFinalizer())
            SetHasCriticalFinalizer();
    }
}


void CallFinalizerOnThreadObject(Object *obj)
{
    THREADBASEREF   refThis = (THREADBASEREF)ObjectToOBJECTREF(obj);
    Thread*         thread  = refThis->GetInternal();

    // Prevent multiple calls to Finalize
    // Objects can be resurrected after being finalized.  However, there is no
    // race condition here.  We always check whether an exposed thread object is
    // still attached to the internal Thread object, before proceeding.
    if (thread)
    {
        refThis->SetDelegate(NULL);

        if ((g_fEEShutDown & ShutDown_Finalize2) == 0)
        {
            if (GetThread() != thread)
            {
                refThis->SetInternal(NULL);
            }

            _ASSERTE(thread->m_pDLSHash == NULL);

            FastInterlockOr ((ULONG *)&thread->m_State, Thread::TS_Finalized);
            Thread::SetCleanupNeededForFinalizedThread();
        }
    }
}

//==========================================================================================
// From the GC finalizer thread, invoke the Finalize() method on an object.
void MethodTable::CallFinalizer(Object *obj)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    MethodTable *pMT = obj->GetMethodTable();
    if (pMT == g_pThreadClass)
    {
        // Finalizing Thread object requires ThreadStoreLock.  It is expensive if 
        // we keep taking ThreadStoreLock.  This is very bad if we have high retiring
        // rate of Thread objects.
        // To avoid taking ThreadStoreLock multiple times, we mark Thread with TS_Finalized
        // and clean up a batch of them when we take ThreadStoreLock next time.

        // To avoid possible hierarchy requirement between critical finalizers, we call cleanup
        // code directly.
        CallFinalizerOnThreadObject(obj);
        return;
    }

    // Notify the host to setup the restricted context before finalizing each object
    HostExecutionContextManager::SetHostRestrictedContext();

    // Determine if the object has a critical or normal finalizer.
    BOOL fCriticalFinalizer = pMT->HasCriticalFinalizer();

    // There's no reason to actually set up a frame here.  If we crawl out of the
    // Finalize() method on this thread, we will see FRAME_TOP which indicates
    // that the crawl should terminate.  This is analogous to how KickOffThread()
    // starts new threads in the runtime.
    SLOT funcPtr = pMT->GetSlot(g_pObjectFinalizerMD->GetSlot());
#ifdef DEBUGGING_SUPPORTED
    if (CORDebuggerTraceCall())
        g_pDebugInterface->TraceCall((const BYTE *) funcPtr);
#endif // DEBUGGING_SUPPORTED

    OBJECTREF orThis = ObjectToOBJECTREF(obj);
    GCPROTECT_BEGIN(orThis);
    MethodDescCallSite objectFinalizer(g_pObjectFinalizerMD, &orThis, TypeHandle(), fCriticalFinalizer);

    ARG_SLOT arg = ObjToArgSlot(orThis);
    objectFinalizer.Call(&arg);

    GCPROTECT_END();

}

//==========================================================================================
BOOL MethodTable::CanCastToInterface(MethodTable *pTargetMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INSTANCE_CHECK;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pTargetMT));
        PRECONDITION(pTargetMT->IsInterface());
        PRECONDITION(!IsThunking());
        PRECONDITION(IsRestored());
    }
    CONTRACTL_END

    if (!pTargetMT->HasVariance())
    {
        return CanCastToNonVariantInterface(pTargetMT);
    }
    else
    {
        if (CanCastByVarianceToInterfaceOrDelegate(pTargetMT))
            return TRUE;

        InterfaceMapIterator it = IterateInterfaceMap();
        while (it.Next())
        {
            if (it.GetInterface()->CanCastByVarianceToInterfaceOrDelegate(pTargetMT))
                return TRUE;
        }
    }
    return FALSE;
}

//==========================================================================================
BOOL MethodTable::CanCastByVarianceToInterfaceOrDelegate(MethodTable *pTargetMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INSTANCE_CHECK;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pTargetMT));
        PRECONDITION(pTargetMT->HasVariance());
        PRECONDITION(pTargetMT->IsInterface() || pTargetMT->IsAnyDelegateClass());
        PRECONDITION(IsRestored());
    }
    CONTRACTL_END

    BOOL returnValue = FALSE;

    EEClass *pClass = NULL;
    TypeHandle *pInst = NULL;
    TypeHandle *pTargetInst = NULL;

    BEGIN_ENTRYPOINT_THROWS;

    if (GetCl() != pTargetMT->GetCl() || GetModule() != pTargetMT->GetModule())
    {
        goto Exit;
    }

    pClass = pTargetMT->GetClass();
    pInst = GetInstantiation();
    _ASSERTE(pInst != NULL);
    pTargetInst = pTargetMT->GetInstantiation();
    _ASSERTE(pTargetInst != NULL);

    for (DWORD i = 0; i < GetNumGenericArgs(); i++)
    {
        // If argument types do not match exactly, test them for compatibility
        // in accordance with the the variance annotation
        if (pInst[i] != pTargetInst[i])
        {
            switch (pClass->GetVarianceOfTypeParameter(i))
            {
            case gpCovariant :
                if (!pInst[i].IsBoxedAndCanCastTo(pTargetInst[i]))
                    goto Exit;
                break;

            case gpContravariant :
                if (!pTargetInst[i].IsBoxedAndCanCastTo(pInst[i]))
                    goto Exit;
                break;

            case gpNonVariant :
                goto Exit;
                break;

            default : _ASSERTE(!"Illegal variance annotation");
            }
        }
    }

    returnValue = TRUE;

Exit:    
    ;
    END_ENTRYPOINT_THROWS;
    
    return returnValue;
}

//==========================================================================================
BOOL MethodTable::CanCastToClass(MethodTable *pTargetMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INSTANCE_CHECK;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pTargetMT));
        PRECONDITION(!pTargetMT->IsArray());
        PRECONDITION(!pTargetMT->IsInterface());
    }
    CONTRACTL_END

    MethodTable *pMT = this;

    // If the target type has variant type parameters, we take a slower path
    if (pTargetMT->HasVariance())
    {
        // At present, we support variance only on delegates and interfaces
        CONSISTENCY_CHECK(pTargetMT->IsAnyDelegateClass());

        // First chase inheritance hierarchy until we hit a class that only differs in its instantiation
        do {
            // Cheap check for equality
            if (pMT == pTargetMT)
                return TRUE;

            g_IBCLogger.LogMethodTableAccess(pMT);

            if (pMT->CanCastByVarianceToInterfaceOrDelegate(pTargetMT))
                return TRUE;

            pMT = pMT->GetParentMethodTable();
        } while (pMT);
    }

    // If there are no variant type parameters, just chase the hierarchy
    else
    {
        do {
            if (pMT == pTargetMT)
                return TRUE;

            g_IBCLogger.LogMethodTableAccess(pMT);

            pMT = pMT->GetParentMethodTable();
        } while (pMT);
    }

    return FALSE;
}

//==========================================================================================
BOOL MethodTable::CanCastToNonVariantInterface(MethodTable *pTargetMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INSTANCE_CHECK;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pTargetMT));
        PRECONDITION(pTargetMT->IsInterface());
        PRECONDITION(!pTargetMT->HasVariance());
        PRECONDITION(!IsThunking());
        PRECONDITION(IsRestored());
    }
    CONTRACTL_END

    // Check to see if the current class is for the interface passed in.
    if (this == pTargetMT)
        return TRUE;

    // Check to see if the static class definition indicates we implement the interface.
    InterfaceMapIterator it = IterateInterfaceMap();

    while (it.Next())
    {
        if (it.InterfaceEquals(pTargetMT))
            return TRUE;
    }

    return FALSE;
}

//==========================================================================================
TypeHandle::CastResult MethodTable::CanCastToInterfaceNoGC(MethodTable *pTargetMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INSTANCE_CHECK;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pTargetMT));
        PRECONDITION(pTargetMT->IsInterface());
        PRECONDITION(!IsThunking());
        PRECONDITION(IsRestored());
    }
    CONTRACTL_END

    if (!pTargetMT->HasVariance() && !IsArray())
    {
        return CanCastToNonVariantInterface(pTargetMT) ? TypeHandle::CanCast : TypeHandle::CannotCast;
    }
    else
    {
        // We're conservative on variant interfaces
        return TypeHandle::MaybeCast;
    }
}

//==========================================================================================
TypeHandle::CastResult MethodTable::CanCastToClassNoGC(MethodTable *pTargetMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INSTANCE_CHECK;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pTargetMT));
        PRECONDITION(!pTargetMT->IsArray());
        PRECONDITION(!pTargetMT->IsInterface());
    }
    CONTRACTL_END

    // We're conservative on variant classes
    if (pTargetMT->HasVariance())
    {
        return TypeHandle::MaybeCast;
    }

    // If there are no variant type parameters, just chase the hierarchy
    else
    {
        MethodTable *pMT = this;
        do {
            if (pMT == pTargetMT)
                return TypeHandle::CanCast;

            g_IBCLogger.LogMethodTableAccess(pMT);

            pMT = pMT->GetParentMethodTable();
        } while (pMT);
    }

    return TypeHandle::CannotCast;
}

#ifndef DACCESS_COMPILE
BOOL MethodTable::IsExternallyVisible()
{
    CONTRACTL
    {
        NOTHROW;
        MODE_ANY;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BOOL bIsVisible = TRUE;

    DWORD dwAttrClass = GetClass()->GetAttrClass();

    if (!IsTdPublic(dwAttrClass))
    {
        if (!IsTdNestedPublic(dwAttrClass))
            return FALSE;

        _ASSERTE(GetClass()->IsNested());
        IMDInternalImport *pInternalImport = GetModule()->GetMDImport();

        DWORD dwAttrEnclosing;

        mdTypeDef tdCurrent = GetCl();
        do
        {
            mdTypeDef tdEnclosing = mdTypeDefNil;

            if (!SUCCEEDED(pInternalImport->GetNestedClassProps(tdCurrent, &tdEnclosing)))
                return FALSE;

            tdCurrent = tdEnclosing;

            // We stop searching as soon as we hit the first non NestedPublic type.
            // So logically, we can't possibly fall off the top of the hierarchy.
            _ASSERTE(tdEnclosing != mdTypeDefNil);

            mdToken tkJunk = mdTokenNil;

            pInternalImport->GetTypeDefProps(tdEnclosing, &dwAttrEnclosing, &tkJunk);
        }
        while (IsTdNestedPublic(dwAttrEnclosing));

        bIsVisible = IsTdPublic(dwAttrEnclosing);
    }

    if (bIsVisible && HasInstantiation() && !IsGenericTypeDefinition())
    {
        for (COUNT_T i = 0; i < GetNumGenericArgs(); i ++)
        {
            if (!GetInstantiation()[i].IsExternallyVisible())
                return FALSE;
        }
    }

    return bIsVisible;
}
#endif

void MethodTable::DebugDumpVtable(LPCUTF8 pszClassName, BOOL debug)
{
    //diag functions shouldn't affect normal behavior
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    CQuickBytes qb;
    const size_t cchBuff = MAX_CLASSNAME_LENGTH + 30;
    LPWSTR buff = debug ? (LPWSTR) qb.AllocNoThrow(cchBuff * sizeof(WCHAR)) : NULL;

    if (buff==NULL && debug)
    {
        WszOutputDebugString(L"OOM when dumping VTable - falling back to logging");
        debug=false;
    };

    if(debug) {
        swprintf_s(buff, cchBuff, L"Vtable (with interface dupes) for '%S':\n", pszClassName);
#ifdef _DEBUG
        swprintf_s(&buff[wcslen(buff)], cchBuff - wcslen(buff) , L"Total duplicate slots = %d\n", g_dupMethods);
#endif
        WszOutputDebugString(buff);
    }
    else {
        //LF_ALWAYS allowed here because this is controlled by special env var ShouldDumpOnClassLoad
        LOG((LF_ALWAYS, LL_ALWAYS, "Vtable (with interface dupes) for '%s':\n", pszClassName));
        LOG((LF_ALWAYS, LL_ALWAYS, "Total duplicate slots = %d\n", g_dupMethods));
    }


    HRESULT hr;
    BEGIN_EXCEPTION_GLUE(&hr, NULL);
    {
        MethodIterator it(this);
        for (; it.IsValid(); it.Next())
        {
            MethodDesc *pMD = it.GetMethodDesc();
            LPCUTF8      pszName = pMD->GetName((USHORT) it.GetSlotNumber());
            DWORD       dwAttrs = pMD->GetAttrs();

            if(debug)
            {
                DefineFullyQualifiedNameForClass();
                LPCUTF8 name = GetFullyQualifiedNameForClass(pMD->GetClass());
                // We need to do all this stuff so we don't get a compiler error or a prefast error
                int pAddr = ((__int64)(pMD->ContainsGenericVariables() ? NULL: (intptr_t)pMD->GetAddrofCode()) & 0xffffffff);
                swprintf_s(buff, cchBuff,
                           L"slot %2d: %S::%S%S  0x%X (slot = %2d)\n",
                           it.GetSlotNumber(),
                           name,
                           pszName,
                           IsMdFinal(dwAttrs) ? " (final)" : "",
                           pAddr,
                           pMD->GetSlot()
                          );
                WszOutputDebugString(buff);
            }
            else
            {
                //LF_ALWAYS allowed here because this is controlled by special env var ShouldDumpOnClassLoad
                LOG((LF_ALWAYS, LL_ALWAYS,
                     "slot %2d: %s::%s%s  0x%X (slot = %2d)\n",
                     it.GetSlotNumber(),
                     pMD->GetClass()->GetDebugClassName(),
                     pszName,
                     IsMdFinal(dwAttrs) ? " (final)" : "",
                     pMD->ContainsGenericVariables() ? 0: pMD->GetAddrofCode(),
                     pMD->GetSlot()
                    ));
            }
            if (it.GetSlotNumber() == (DWORD)(GetNumMethods()-1)) {
                if(debug) {
                    WszOutputDebugString(L"<-- vtable ends here\n");
                }
                else {
                    //LF_ALWAYS allowed here because this is controlled by special env var ShouldDumpOnClassLoad
                    LOG((LF_ALWAYS, LL_ALWAYS, "<-- vtable ends here\n"));
                }
            }
        }
    }
    END_EXCEPTION_GLUE;


    if(debug) {
        WszOutputDebugString(L"\n");
    } else {
        //LF_ALWAYS allowed here because this is controlled by special env var ShouldDumpOnClassLoad
        LOG((LF_ALWAYS, LL_ALWAYS, "\n"));
    }
}

//==========================================================================================
InterfaceInfo_t* MethodTable::FindInterface(MethodTable *pInterface)
{
    CONTRACT(InterfaceInfo_t*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(pInterface->IsRestored());
        PRECONDITION(!IsInterface()); // we can't be an interface ourselves
        PRECONDITION(pInterface->GetClass()->IsInterface() != FALSE); // class we are looking up should be an interface
        PRECONDITION(!IsThunking());
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    InterfaceMapIterator it = IterateInterfaceMap();

    while (it.Next())
    {

        if (it.InterfaceEquals(pInterface) ||
            (!g_pConfig->ExactInterfaceCalls() &&
             it.GetInterfaceInfo()->m_pMethodTable->GetCanonicalMethodTable() == pInterface->GetCanonicalMethodTable()))
        {
            // Extensible RCW's need to be handled specially because they can have interfaces
            // in their map that are added at runtime. These interfaces will have a start offset
            // of -1 to indicate this. We cannot take for granted that every instance of this
            // COM object has this interface so FindInterface on these interfaces is made to fail.
            //
            // However, we are only considering the statically available slots here
            // (m_wNumInterface doesn't contain the dynamic slots), so we can safely
            // ignore this detail.
            RETURN(it.GetInterfaceInfo());
        }
    }

    RETURN(NULL);
}

//==========================================================================================
MethodDesc *MethodTable::GetMethodDescForInterfaceMethod(MethodDesc *pInterfaceMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(!pInterfaceMD->HasClassOrMethodInstantiation());
    }
    CONTRACTL_END;
    WRAPPER_CONTRACT;

    return GetMethodDescForInterfaceMethod(TypeHandle(pInterfaceMD->GetMethodTable()), pInterfaceMD);
}

//==========================================================================================
MethodDesc *MethodTable::GetMethodDescForInterfaceMethod(TypeHandle ownerType, MethodDesc *pInterfaceMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(!ownerType.IsNull());
        PRECONDITION(ownerType.GetMethodTable()->IsInterface());
        PRECONDITION(ownerType.GetMethodTable()->HasSameTypeDefAs(pInterfaceMD->GetMethodTable()));
        PRECONDITION(IsArray() || ImplementsInterface(ownerType.GetMethodTable()));
    }
    CONTRACTL_END;

    MethodDesc *pMD = NULL;

    MethodTable *pInterfaceMT = ownerType.GetMethodTable();


    BYTE *pTgt = (BYTE *) VirtualCallStubManager::GetTarget(
        DispatchToken::CreateDispatchToken(pInterfaceMT->GetTypeID(), pInterfaceMD->GetSlot()).To_SIZE_T(),
        this);
    pMD = MethodTable::GetUnknownMethodDescForSlotAddress((SLOT)pTgt);
#ifdef _DEBUG
    MethodDesc *pDispSlotMD = FindDispatchSlotForInterfaceMD(ownerType, pInterfaceMD).GetMethodDesc();
    _ASSERTE(pDispSlotMD == pMD);
#endif // _DEBUG

    pMD->CheckRestore();

    return pMD;
}

//==========================================================================================
FieldDesc *MethodTable::GetFieldDescByIndex(DWORD fieldIndex)
{

    WRAPPER_CONTRACT;

    if (HasGenericsStaticsInfo() &&
        fieldIndex >= GetNumIntroducedInstanceFields())
    {
        return GetGenericsStaticFieldDescs() + (fieldIndex - GetNumIntroducedInstanceFields());
    }
    else
    {
        return GetApproxFieldDescListRaw() + fieldIndex;
    }
}

//==========================================================================================
DWORD MethodTable::GetIndexForFieldDesc(FieldDesc *pField)
{
    WRAPPER_CONTRACT;
    if (pField->IsStatic() && HasGenericsStaticsInfo())
    {
        FieldDesc *pStaticFields = GetGenericsStaticFieldDescs();

        return GetNumIntroducedInstanceFields() + DWORD(pField - pStaticFields);

    }
    else
    {
        FieldDesc *pFields = GetApproxFieldDescListRaw();

        return DWORD(pField - pFields);
    }
}

//==========================================================================================
void MethodTable::AllocateStaticBoxes()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(!ContainsGenericVariables());
        PRECONDITION(GetNumBoxedStatics() > 0);
        MODE_ANY;
    }
    CONTRACTL_END;

    LOG((LF_CLASSLOADER, LL_INFO10000, "STATICS: Instantiating static handles for %s\n", GetDebugClassName()));

    TADDR pStaticBase = GetGCStaticsBasePointer();

    // In ngened case, we have cached array with boxed statics MTs. In JITed case, we have just the FieldDescs
    ClassCtorInfoEntry *pClassCtorInfoEntry = GetClassCtorInfoIfExists();
    if (pClassCtorInfoEntry != NULL)
    {
        OBJECTREF* pStaticSlots = (OBJECTREF*)(pStaticBase + pClassCtorInfoEntry->firstBoxedStaticOffset);

        MethodTable **ppMTs = GetLoaderModule()->GetZapModuleCtorInfo()->
            GetGCStaticMTs(pClassCtorInfoEntry->firstBoxedStaticMTIndex);
        MethodTable **ppMTsEnd = ppMTs + pClassCtorInfoEntry->numBoxedStatics;

        GCX_COOP();

        while (ppMTs < ppMTsEnd)
        {
            _ASSERTE(*ppMTs != NULL);

            MethodTable *pMT = (MethodTable*)*ppMTs;

            _ASSERTE(pMT);

            LOG((LF_CLASSLOADER, LL_INFO10000, "\tInstantiating static of type %s\n", pMT->GetDebugClassName()));
            OBJECTREF obj = AllocateStaticBox(pMT);

            SetObjectReference( pStaticSlots, obj, GetAppDomain() );

            pStaticSlots++;
            ppMTs++;
        }
    }
    else
    {
        // We should never take this codepath in zapped images.
        _ASSERTE(!IsZapped());

        FieldDesc *pField = HasGenericsStaticsInfo() ? 
            GetGenericsStaticFieldDescs() : (GetApproxFieldDescListRaw() + GetNumIntroducedInstanceFields());
        FieldDesc *pFieldEnd = pField + GetNumStaticFields();

        GCX_COOP();

        while (pField < pFieldEnd)
        {
            _ASSERTE(pField->IsStatic());

            if (!pField->IsSpecialStatic() && pField->GetFieldType() == ELEMENT_TYPE_VALUETYPE)
            {
                TypeHandle  th = pField->GetFieldTypeHandleThrowing();
                MethodTable* pMT = th.GetMethodTable();

                LOG((LF_CLASSLOADER, LL_INFO10000, "\tInstantiating static of type %s\n", pMT->GetDebugClassName()));
                OBJECTREF obj = AllocateStaticBox(pMT);

                SetObjectReference( (OBJECTREF*)(pStaticBase + pField->GetOffset()), obj, GetAppDomain() );
            }

            pField++;
        }
    }
}

//==========================================================================================
OBJECTREF MethodTable::AllocateStaticBox(MethodTable* pFieldMT, OBJECTHANDLE* pHandle)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        CONTRACTL_END;
    }

    // Activate any dependent modules if necessary
    pFieldMT->EnsureInstanceActive();

    OBJECTREF obj = AllocateObject(pFieldMT);

    // Pin the object if necessary
    if (this->IsFixedAddressVTStatics() && pFieldMT->IsValueType())
    {
        LOG((LF_CLASSLOADER, LL_INFO10000, "\tSTATICS:Pinning static (VT fixed address attribute) of type %s\n", pFieldMT->GetDebugClassName()));
        OBJECTHANDLE oh = GetAppDomain()->CreatePinningHandle(obj);
        if (pHandle)
        {
            *pHandle = oh;
        }
    }
    else
    {
        if (pHandle)
        {
            *pHandle = NULL;
        }
    }

    return obj;
}

//==========================================================================================
// The class initializer.  We only impact working set if there is a strong likelihood that
// the class needs <clinit> to be run.

HRESULT MethodTable::CheckRunClassInitNT(OBJECTREF* pThrowable)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;

    HRESULT hr=S_OK;

    // Check to see if we have already run the .cctor for this class.
    // We do the extra check here because it would be great to avoid
    // the exception glue setup if we've already inited the class.
    g_IBCLogger.LogMethodTableWriteableDataAccess(this);
    if (IsClassInited())
        return S_OK;

    {
    BEGIN_EXCEPTION_GLUE(&hr,pThrowable);
    CheckRunClassInitThrowing();
    END_EXCEPTION_GLUE;
    }
    return hr;
}

//==========================================================================================
void MethodTable::CheckRunClassInitThrowing()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(IsFullyLoaded());
        PRECONDITION(CheckActivated());
    }
    CONTRACTL_END;

    // To find GC hole easier...
    TRIGGERSGC();
    g_IBCLogger.LogMethodTableWriteableDataAccess(this);

    if (IsClassPreInited())
        return;

    DomainLocalModule *pLocalModule = GetDomainLocalModule();
    DWORD iClassIndex = GetClassIndex();

    _ASSERTE(pLocalModule);

    // Check to see if we have already run the .cctor for this class.
    if (!pLocalModule->IsClassAllocated(this, iClassIndex))
        pLocalModule->PopulateClass(this);

    if (!pLocalModule->IsClassInitialized(this, iClassIndex))
        DoRunClassInitThrowing();

    return;

}

//==========================================================================================
OBJECTREF MethodTable::Allocate()
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        THROWS;
    }
    CONTRACTL_END;

    CONSISTENCY_CHECK(IsFullyLoaded());

    EnsureInstanceActive();

    // We get the class token by directly accessing EEClass, instead of calling GetClassIndex().
    // This is typically not good from working set standpoint because EEClass is
    // supposed to be cold. But MT::Allocate is not heavily called, and when it _is_
    // called, GetClassIndex is an execution perf bottleneck. So we are trading space for speed.
    if (!IsClassInitedHaveIndex(GetClassIndexFromToken(GetClass()->GetCl())))
        CheckRunClassInitThrowing();

    return AllocateObject(this);
}

//==========================================================================================
// box 'data' creating a new object and return it.  This routine understands the special
// handling needed for Nullable values.  

OBJECTREF MethodTable::Box(void* data, BOOL mayContainRefs)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsValueClass());
    }
    CONTRACTL_END;

    OBJECTREF ref;

    GCPROTECT_BEGININTERIOR (data);
    ref = FastBox(&data);
    GCPROTECT_END ();
    return ref;
}

OBJECTREF MethodTable::FastBox(void** data)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsValueClass());
    }
    CONTRACTL_END;

    if (Nullable::IsNullableType(TypeHandle(this)))
        return Nullable::Box(*data, this);

    OBJECTREF ref = Allocate();
    CopyValueClass(ref->UnBox(), *data, this, ref->GetAppDomain());
    return ref;
}

#endif // !DACCESS_COMPILE

//==========================================================================================
BOOL MethodTable::IsSharedByGenericInstantiations()
{
    LEAF_CONTRACT;
    return HasInstantiation() && IsCanonicalMethodTable() && GetClass()->IsSharedByGenericInstantiations();
}


//==========================================================================================
// Similar to MethodTable::IsSharedByGenericInstantiations()
//  but will also return true for subtypes that contain a Canonical type:
//
// A<__Canon>    is the canonical MethodTable (aka "representative" generic MT)
// A<B<__Canon>> is a subtype that contains a Canonical type
//
BOOL MethodTable::IsSharedByGenericCanonicalSubtype()
{
    LEAF_CONTRACT;

    if (!HasInstantiation())
        return FALSE;

    if (!GetClass()->IsSharedByGenericInstantiations())
        return FALSE;

    if (IsCanonicalMethodTable())
        return TRUE;

    // Recursively search the type arguments pof the TypeHandle and if
    // one of the type arguments is Canon then return TRUE

    TypeHandle th = TypeHandle(this);
    return th.IsCanonicalSubtype();
}


//==========================================================================================
// This needs to stay consistent with AllocateNewMT()
//
//
void MethodTable::GetExtent(TADDR *pStart, TADDR *pEnd, BOOL classAvailable)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    TADDR start;

    if (ContainsPointers())
        start = PTR_HOST_TO_TADDR(this) - CGCDesc::GetCGCDescFromMT(this)->GetSize();
    else
        start = PTR_HOST_TO_TADDR(this);

    TADDR end;

    // Last-but-one piece in the MethodTable is the Interface Map
    InterfaceInfo_t *pInterfaceMap = GetInterfaceMap();
    if (pInterfaceMap)
    {
        _ASSERTE(GetNumInterfaces());
        end = PTR_HOST_TO_TADDR(pInterfaceMap + GetNumInterfaces());
    }
    else
    {
        end = PTR_HOST_TO_TADDR(this) + GetEndOffsetOfOptionalMembers();
        end += GetPerInstInfoSize();
    }

    // Last piece in the MethodTable is the instantiation/dictionary (if any)
    if (classAvailable) {
    end += GetInstAndDictSize();
    }

    CONSISTENCY_CHECK(m_pPerInstInfo == NULL || (UINT_PTR)m_pPerInstInfo < (UINT_PTR)end);
    _ASSERTE(start && end && (start < end));
    *pStart = start;
    *pEnd = end;
}

#ifndef DACCESS_COMPILE

//==========================================================================
// If the MethodTable doesn't yet know the Exposed class that represents it via
// Reflection, acquire that class now.  Regardless, return it to the caller.
//==========================================================================
OBJECTREF MethodTable::GetManagedClassObject()
{
    CONTRACT(OBJECTREF) {

        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(!CRemotingServices::IsRemotingStarted() || this != CTPMethodTable::GetMethodTable());
        POSTCONDITION(GetWriteableData()->m_pExposedClassObject != NULL);
        //REENTRANT
    }
    CONTRACT_END;

    if (GetWriteableData()->m_pExposedClassObject == NULL)
    {
        // Make sure that we have been restored
        CheckRestore();

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

        g_IBCLogger.LogMethodTableWriteableDataWriteAccess(this);

        if (FastInterlockCompareExchangePointer((void**)&(GetWriteableData()->m_pExposedClassObject), *(void**)&exposedClassObject, NULL))
        {
            SetObjectReference(exposedClassObject, NULL, NULL);
        }

        GCPROTECT_END();
    }
    RETURN(*(GetWriteableData()->m_pExposedClassObject));
}

#endif //!DACCESS_COMPILE


#ifndef DACCESS_COMPILE

//==========================================================================================
void MethodTable::SetModule (Module *pModule)
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClass());
    GetClass()->SetModule (pModule);
}

#endif // !DACCESS_COMPILE

//==========================================================================================
void MethodTable::CheckRestore()
{
    LEAF_CONTRACT;
}



BOOL SatisfiesClassConstraints(TypeHandle instanceTypeHnd, TypeHandle typicalTypeHnd);



#ifndef DACCESS_COMPILE
static VOID DoAccessibilityCheck(MethodTable *pAskingMT, MethodTable *pTargetMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (!ClassLoader::CanAccessClass(NULL,                      //currentMD (NULL in this case, we're asking on behalf of the entire type)
                                     pAskingMT,                 //the class that wants access
                                     pAskingMT->GetAssembly(),  //the assembly that wants access
                                     pTargetMT,                 //the desired class
                                     pTargetMT->GetAssembly()   //the desired class's assembly
                                    ))
    {
        SString targetName;
        TypeString::AppendType(targetName, TypeHandle(pTargetMT));


        // Not the most informative message but...
        //  - I don't want to add another localization cost at this late stage.
        //  - I don't want Snap aborting the job because of a trifling resource.h conflict
        //  - the only people likely to see this are malicious hackers.
        //
        COMPlusThrow(kTypeLoadException, E_ACCESSDENIED, targetName.GetUnicode());
    }

}


VOID DoAccessibilityCheckForConstraint(MethodTable *pAskingMT, TypeHandle thConstraint)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (thConstraint.IsGenericVariable())
    {
        // since the metadata respresents a generic type param constraint as an index into
        // the declaring type's list of generic params, it is structurally impossible
        // to express a violation this way. So there's no check to be done here.
    }
    else if (thConstraint.IsUnsharedMT())
    {
        DoAccessibilityCheck(pAskingMT, thConstraint.GetMethodTable());
    }
    else
    {
        TypeDesc *pTypeDesc = thConstraint.AsTypeDesc();

        if (pTypeDesc->HasTypeParam())
        {
            DoAccessibilityCheckForConstraint(pAskingMT, pTypeDesc->GetTypeParam());
        }
        else
        {
            COMPlusThrow(kTypeLoadException, E_ACCESSDENIED);
        }
    
    }

}

VOID DoAccessibilityCheckForConstraints(MethodTable *pAskingMT, TypeVarTypeDesc *pTyVar)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    DWORD numConstraints;
    TypeHandle *pthConstraints = pTyVar->GetCachedConstraints(&numConstraints);
    for (DWORD cidx = 0; cidx < numConstraints; cidx++)
    {
        TypeHandle thConstraint = pthConstraints[cidx];

        DoAccessibilityCheckForConstraint(pAskingMT, thConstraint);
    }
}

#endif


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
//   fDoConstraintChecks - almost always TRUE - used internally by this function to prevent false
//              positives on the hidden canonical methodtable for generics. No one else should
//              pass FALSE here.
//   
void MethodTable::DoFullyLoad(TypeHandleList *pVisited, ClassLoadLevel level, DFLPendingList *pPending, BOOL *pfBailed, BOOL fDoConstraintChecks /*=TRUE*/)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

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
            if (pTypeHndPending[idxPending].IsUnsharedMT() && pTypeHndPending[idxPending].AsMethodTable() == this)
            {
                *pfBailed = TRUE;
                return;
            }
        }

    }

    BEGIN_SO_INTOLERANT_CODE(GetThread());
    // First ensure that we're loaded to just below CLASS_DEPENDENCIES_LOADED
    ClassLoader::EnsureLoaded(this, (ClassLoadLevel) (level-1));

    CONSISTENCY_CHECK(IsRestored());
    CONSISTENCY_CHECK(!HasApproxParent());

    TypeHandleList newVisited(this, pVisited);

    BOOL fBailed = FALSE;


    BOOL fNeedAccessChecks = (level == CLASS_LOADED) &&
                             !( HasInstantiation() && !IsTypicalTypeDefinition() );


    // Fully load the parent
    MethodTable *pParentMT = GetParentMethodTable();
    if (pParentMT)
    {
        pParentMT->DoFullyLoad(&newVisited, level, pPending, &fBailed);

        if (fNeedAccessChecks)
        {
            if (!IsComObjectType()) //RCW's are special - they are manufactured by the runtime and derive from the non-public type System.__ComObject
            {
                DoAccessibilityCheck(this, pParentMT);
            }
        }

    }

    // Fully load the interfaces
    MethodTable::InterfaceMapIterator it = IterateInterfaceMap();
    while (it.Next())
    {
        it.GetInterface()->DoFullyLoad(&newVisited, level, pPending, &fBailed);
        if (fNeedAccessChecks)
        {
            if (it.IsDeclaredOnClass()) // only test directly implemented interfaces (it's legal for an inherited interface to be private.)
            {
                DoAccessibilityCheck(this, it.GetInterface());
            }
        }
    }

    // Fully load the generic arguments
    TypeHandle *pInst = GetInstantiation();
    for (DWORD i = 0; i < GetNumGenericArgs(); i++)
    {
        pInst[i].DoFullyLoad(&newVisited, level, pPending, &fBailed);
    }

    // Fully load the canonical methodtable
    if (!IsCanonicalMethodTable())
    {
        GetCanonicalMethodTable()->DoFullyLoad(&newVisited, level, pPending, &fBailed, FALSE);
    }

    TypeHandle typicalTypeHnd;
    // Fully load the typical instantiation
    if (!IsTypicalTypeDefinition())
    {
        typicalTypeHnd = ClassLoader::LoadTypeDefThrowing(GetModule(), GetCl(),
                                                          ClassLoader::ThrowIfNotFound, ClassLoader::PermitUninstDefOrRef, tdNoTypes,
                                                          (ClassLoadLevel) (level-1));
        CONSISTENCY_CHECK(!typicalTypeHnd.IsNull());
        typicalTypeHnd.DoFullyLoad(&newVisited, level, pPending, &fBailed);
    }

    // Fully load the exact field types for value type fields
    // Note that MethodTableBuilder::InitializeFieldDescs() loads the type of the
    // field only upto level CLASS_LOAD_APPROXPARENTS.
    FieldDesc *pField = GetApproxFieldDescListRaw();
    FieldDesc *pFieldEnd = pField + GetNumStaticFields() + GetNumIntroducedInstanceFields();
    while (pField < pFieldEnd)
    {
        if (pField->GetFieldType() == ELEMENT_TYPE_VALUETYPE)
        {
            TypeHandle th = pField->GetFieldTypeHandleThrowing((ClassLoadLevel) (level-1));
            CONSISTENCY_CHECK(!th.IsNull());

            th.DoFullyLoad(&newVisited, level, pPending, &fBailed);

            if (fNeedAccessChecks)
            {
                DoAccessibilityCheck(this, th.GetMethodTable());
            }

        }
        pField++;
    }

    // Fully load the exact field types for generic value type fields
    FieldDesc *pGenStaticField = GetGenericsStaticFieldDescs();
    if (pGenStaticField != NULL)
    {
        FieldDesc *pGenStaticFieldEnd = pGenStaticField + GetNumStaticFields();
        while (pGenStaticField < pGenStaticFieldEnd)
        {
            if (pGenStaticField->GetFieldType() == ELEMENT_TYPE_VALUETYPE)
            {
                TypeHandle th = pGenStaticField->GetFieldTypeHandleThrowing((ClassLoadLevel) (level-1));
                CONSISTENCY_CHECK(!th.IsNull());

                th.DoFullyLoad(&newVisited, level, pPending, &fBailed);

                // The accessibility check is not necessary for generic fields. The generic fields are copy
                // of the regular fields, the only difference is that they have the exact type.
            }
            pGenStaticField++;
        }
    }

    if (level == CLASS_LOADED && IsTypicalTypeDefinition() )
    {
        // Check for cyclical class constraints
        {
            TypeHandle* formalParams = GetInstantiation();
            DWORD numGenericArgs = GetNumGenericArgs();
        
            for (DWORD i = 0; i < numGenericArgs; i++)
            {
                BOOL Bounded(TypeVarTypeDesc *tyvar, DWORD depth);
    
                TypeVarTypeDesc *pTyVar = formalParams[i].AsGenericVariable();
                pTyVar->LoadConstraints(CLASS_DEPENDENCIES_LOADED);
                if (!Bounded(pTyVar, numGenericArgs))
                {
                    COMPlusThrow(kTypeLoadException, VER_E_CIRCULAR_VAR_CONSTRAINTS);
                }

                DoAccessibilityCheckForConstraints(this, pTyVar);
            }
        }

        // Check for cyclical method constraints
        {

            HENUMInternalHolder hEnumDeclaredMethods(GetMDImport());
            if (GetCl() != mdTypeDefNil)  // Make sure this is actually a metadata type!
            {
                hEnumDeclaredMethods.EnumInit(mdtMethodDef, GetCl());
                ULONG numMethods =  GetMDImport()->EnumGetCount(&hEnumDeclaredMethods);
                for (ULONG methidx = 0; methidx < numMethods; methidx++)
                {             
                    mdMethodDef tok = 0;
                    GetMDImport()->EnumNext(&hEnumDeclaredMethods, &tok);
                    _ASSERTE(TypeFromToken(tok) == mdtMethodDef);
                    MethodDesc *pMD = GetModule()->LookupMethodDef(tok);
                    if (pMD)  // vtable spacers have methoddefs but no methoddescs
                    {
                        if (pMD->IsGenericMethodDefinition() && pMD->IsTypicalMethodDefinition())
                        {
                            BOOL fHasCircularClassConstraints = TRUE;
                            BOOL fHasCircularMethodConstraints = TRUE;
            
                            pMD->LoadConstraintsForTypicalMethodDefinition(&fHasCircularClassConstraints, &fHasCircularMethodConstraints, CLASS_DEPENDENCIES_LOADED);
            
                            if (fHasCircularClassConstraints)
                            {
                                COMPlusThrow(kTypeLoadException, VER_E_CIRCULAR_VAR_CONSTRAINTS);
                            }
                            if (fHasCircularMethodConstraints)
                            {
                                COMPlusThrow(kTypeLoadException, VER_E_CIRCULAR_MVAR_CONSTRAINTS);
                            }
                        }
                    }
    
                }
            }
    
        }

    }


#ifdef _DEBUG
    if (LoggingOn(LF_CLASSLOADER, LL_INFO10000))
    {
        SString name;
        TypeString::AppendTypeDebug(name, this);
        LOG((LF_CLASSLOADER, LL_INFO10000, "PHASEDLOAD: Completed full dependency load of type %S\n", name.GetUnicode()));
    }
#endif

    switch (level)
    {
        case CLASS_DEPENDENCIES_LOADED:
            SetIsDependenciesLoaded();
            break;

        case CLASS_LOADED:
            if (fDoConstraintChecks && !IsTypicalTypeDefinition())
            {
                TypeHandle thThis = TypeHandle(this);
    
                // If we got here, we about to mark a generic instantiation as fully loaded. Before we do so,
                // check to see if has constraints that aren't being satisfied.
                SatisfiesClassConstraints(thThis, typicalTypeHnd);
    
            }

            if (fBailed)
            {
                // We couldn't complete security checks on some dependency because he is already being processed by one of our callers.
                // Do not mark this class fully loaded yet. Put him on the pending list and he will be marked fully loaded when
                // everything unwinds.

                *pfBailed = TRUE;

                TypeHandle *pTHPending = pPending->AppendThrowing();
                *pTHPending = TypeHandle(this);
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

    END_SO_INTOLERANT_CODE;
    
#endif
}


#ifndef DACCESS_COMPILE




#endif // !DACCESS_COMPILE

//==========================================================================================
// Return a pointer to the dictionary for an instantiated type
// Return NULL if not instantiated
Dictionary* MethodTable::GetDictionary()
{
    WRAPPER_CONTRACT;
    if (HasInstantiation())
    {
        // The instantiation for this class is stored in the type slots table
        // *after* any inherited slots
        return GetPerInstInfo()[GetNumDicts()-1];
    }
    else
    {
        return NULL;
    }
}

//==========================================================================================
// As above, but assert if an instantiated type is not restored
TypeHandle* MethodTable::GetInstantiation()
{
    WRAPPER_CONTRACT;
    if (HasInstantiation())
    {
        return (TypeHandle *) (
                        PTR_READ(PTR_HOST_TO_TADDR(GetDictionary()->GetInstantiation()),
                                 GetClass()->GetNumGenericArgs() * sizeof(TypeHandle)));
    }
    else
    {
        return NULL;
    }
}

//==========================================================================================
// Obtain instantiation from an instantiated type or a pointer to the
// element type of an array
TypeHandle* MethodTable::GetClassOrArrayInstantiation()
{
    WRAPPER_CONTRACT;
    if (IsArray()) {
        ArrayClass *pClass = (ArrayClass*) GetClass();
        return pClass->GetInstantiation();
    }
    else {
        return GetInstantiation();
    }
}

#ifndef DACCESS_COMPILE

//==========================================================================================
BOOL MethodTable::RunClassInitEx(OBJECTREF *pThrowable)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsFullyLoaded());
        PRECONDITION(IsProtectedByGCFrame(pThrowable));
    }
    CONTRACTL_END;


    // A somewhat unusual function, can both return throwable and throw.
    // The difference is, we throw on restartable operations and just return throwable
    // on exceptions fatal for the .cctor
    // (Of course in the latter case the caller is supposed to throw pThrowable)
    // Doing the opposite ( i.e. throwing on fatal and returning on nonfatal)
    // would be more intuitive but it's more convenient the way it is

    BOOL fRet = FALSE;

    if (s_cctorSig == NULL)
    {
        // Allocate a metasig to use for all class constructors.
        void *tempSpace = SystemDomain::System()->GetHighFrequencyHeap()->AllocMem(sizeof(MetaSig));

        PCCOR_SIGNATURE pSig;
        DWORD cbSigSize;

        gsig_SM_RetVoid.GetBinarySig(&pSig, &cbSigSize);

        s_cctorSig = new (tempSpace) MetaSig(pSig,
                                             cbSigSize,
                                             SystemDomain::SystemModule(),
                                             NULL,
                                             NULL);
    }

    // Find the init method (in the canonical MT)
    MethodDesc *pCLInitMethod = GetCanonicalMethodTable()->GetMethodDescForSlot(GetClassConstructorSlot());

    // Apply the instantiation implicit in the type of our method
    // table (if any).  This may produce an instantiating stub.
    pCLInitMethod = MethodDesc::FindOrCreateAssociatedMethodDesc(pCLInitMethod,
                                                                 this,
                                                                 FALSE /* no BoxedEntryPointStub */,
                                                                 0, NULL, /* no method instantiation */
                                                                 FALSE /* no allowInstParam */);

    // During the <clinit>, this thread must not be asynchronously
    // stopped or interrupted.  That would leave the class unavailable
    // and is therefore a security hole.  We don't have to worry about
    // multithreading, since we only manipulate the current thread's count.
    ThreadPreventAsyncHolder preventAsync(TRUE);

    // If the static initialiser throws an exception that it doesn't catch, it has failed
    EX_TRY
    {
        // Activate our module if necessary
        pCLInitMethod->EnsureActive();

        fRet = RunClassInitWorker(pCLInitMethod, pThrowable);

    }
    EX_CATCH
    {
        // Exception set by parent
        UpdateThrowable(pThrowable);
        _ASSERTE(fRet == FALSE);
    }
    EX_END_CATCH(SwallowAllExceptions)

    return fRet;
}

//==========================================================================================
BOOL MethodTable::RunClassInitWorker(MethodDesc* pInitMethod, OBJECTREF *pThrowable)
{
    //CONTRACTL
    //{
    //    THROWS;
    TRIGGERSGC();   //    GC_TRIGGERS;
    _ASSERTE(GetThread()->PreemptiveGCDisabled());    //    MODE_COOPERATIVE;
    //}
    //CONTRACTL_END;

    _ASSERTE(NULL != pInitMethod);
    _ASSERTE(!pInitMethod->RequiresInstArg());

    STRESS_LOG1(LF_CLASSLOADER, LL_INFO1000, "RunClassInit: Calling class contructor for type %pT\n", this);

    // We want to give the debugger a chance to handle any unhandled exceptions
    // that occur during class initialization, so we need to have filter
    MethodDescCallSite initMethod(pInitMethod, s_cctorSig);
    initMethod.Call((ARG_SLOT*)NULL);

    STRESS_LOG1(LF_CLASSLOADER, LL_INFO1000, "RunClassInit: Returned Successfully from class contructor for type %pT\n", this);

    return TRUE;
}


//==========================================================================================
void MethodTable::DoRunClassInitThrowing()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    // This is a fairly aggressive policy. Merely asking that the class be initialized is grounds for kicking you out.
    // Alternately, we could simply NOP out the class initialization. Since the aggressive policy is also the more secure
    // policy, keep this unless it proves intractable to remove all premature classinits in the system.
    EnsureActive();

    Thread *pThread;
    pThread = GetThread();
    _ASSERTE(pThread);
    INTERIOR_STACK_PROBE_FOR(pThread, 8);

    AppDomain *pDomain = GetAppDomain();

    HRESULT hrResult = E_FAIL;
    const char *description;
    STRESS_LOG2(LF_CLASSLOADER, LL_INFO1000, "DoRunClassInit: Request to init %pT in appdomain %p\n", this, pDomain);

    //
    // Take the global lock
    //

    ListLock *_pLock = pDomain->GetClassInitLock();

    ListLockHolder pInitLock(_pLock);

    // Check again
    if (IsClassInited())
        goto Exit;


    //
    // Handle cases where the .cctor has already tried to run but failed.
    //


    if (IsInitError())
    {
        // Some error occurred trying to init this class
        ListLockEntry*     pEntry= (ListLockEntry *) _pLock->Find(this);
        _ASSERTE(pEntry!=NULL);

        // If this isn't a TypeInitializationException, then its creation failed
        // somehow previously, so we should make one last attempt to create it. If
        // that fails, just throw the exception that was originally thrown.
        // Primarily, this deals with the problem that the exception is a
        // ThreadAbortException, because this must be executing on a different
        // thread. If in fact this thread is also aborting, then rethrowing the
        // other thread's exception will not do any worse.

        // If we need to create the type init exception object, we'll need to
        // GC protect these, so might as well create the structure now.
        struct _gc {
            OBJECTREF pInitException;
            OBJECTREF pNewInitException;
            OBJECTREF pThrowable;
        } gc;

        gc.pInitException = ObjectFromHandle(pEntry->m_hInitException);
        gc.pNewInitException = NULL;
        gc.pThrowable = NULL;

        GCPROTECT_BEGIN(gc);

        // We need to release this lock because CreateTypeInitializationExceptionObject and fetching the TypeLoad exception can cause
        // managed code to re-enter into this codepath, causing a locking order violation.
        pInitLock.Release();

        if (g_Mscorlib.GetException(kTypeInitializationException) != gc.pInitException->GetMethodTable())
        {
            DefineFullyQualifiedNameForClassWOnStack();
            LPCWSTR wszName = GetFullyQualifiedNameForClassW(GetClass());

            CreateTypeInitializationExceptionObject(wszName, &gc.pInitException, &gc.pNewInitException, &gc.pThrowable);

            // Now put the new init exception in the handle. If another thread beat us (because we released the
            // lock above), then we'll just let the extra init exception object get collected later.
            InterlockedCompareExchangeObjectInHandle(pEntry->m_hInitException, gc.pNewInitException, gc.pInitException);
        }
        else {
            gc.pThrowable = gc.pInitException;
        }

        GCPROTECT_END();

        //
        if (IsException(gc.pThrowable->GetMethodTable()))
        {
            ((EXCEPTIONREF)(gc.pThrowable))->ClearStackTraceForThrow();
        }

        COMPlusThrow(gc.pThrowable);
    }

    description = ".cctor lock";
#if _DEBUG
    description = GetDebugClassName();
#endif

    // Take the lock
    {
        //nontrivial holder, might take a lock in destructor
        ListLockEntryHolder pEntry(ListLockEntry::Find(pInitLock, this, description));

        ListLockEntryLockHolder pLock(pEntry, FALSE);

        // We have a list entry, we can release the global lock now
        pInitLock.Release();

        if (pLock.DeadlockAwareAcquire())
        {
            if (pEntry->m_hrResultCode == S_FALSE)
            {
                if (GetNumBoxedStatics() > 0)
                {
                    // First, instantiate any objects needed for value type statics
                    AllocateStaticBoxes();
                }

                // Nobody has run the .cctor yet
                if (HasClassConstructor())
                {
                    struct _gc {
                        OBJECTREF pInnerException;
                        OBJECTREF pInitException;
                        OBJECTREF pThrowable;
                    } gc;
                    gc.pInnerException = NULL;
                    gc.pInitException = NULL;
                    gc.pThrowable = NULL;
                    GCPROTECT_BEGIN(gc);

                    if (!RunClassInitEx(&gc.pInnerException))
                    {
                        // The .cctor failed and we want to store the exception that resulted
                        // in the entry. Increment the ref count to keep the entry alive for
                        // subsequent attempts to run the .cctor.
                        pEntry->AddRef();

                        _ASSERTE(g_pThreadAbortExceptionClass == g_Mscorlib.GetException(kThreadAbortException));

                        if(gc.pInnerException->GetMethodTable() == g_pThreadAbortExceptionClass)
                        {
                            gc.pThrowable = gc.pInnerException;
                            gc.pInitException = gc.pInnerException;
                            gc.pInnerException = NULL;
                        }
                        else
                        {
                            DefineFullyQualifiedNameForClassWOnStack();
                            LPCWSTR wszName = GetFullyQualifiedNameForClassW(GetClass());

                            // Note that this may not succeed due to problems creating the exception
                            // object. On failure, it will first try to
                            CreateTypeInitializationExceptionObject(
                                wszName, &gc.pInnerException, &gc.pInitException, &gc.pThrowable);
                        }

                        // CreateHandle can throw due to OOM. We need to catch this so that we make sure to set the
                        // init error. Whatever exception was thrown will be rethrown below, so no worries.
                        EX_TRY {
                            // Save the exception object, and return to caller as well.
                            pEntry->m_hInitException = pDomain->CreateHandle(gc.pInitException);
                        } EX_CATCH {
                            // If we failed to create the handle (due to OOM), we'll just store the preallocated OOM
                            // handle here instead.
                            pEntry->m_hInitException = CLRException::GetPreallocatedOutOfMemoryExceptionHandle();
                        } EX_END_CATCH(SwallowAllExceptions);
                        
                        pEntry->m_hrResultCode = E_FAIL;
                        SetClassInitError();

                        COMPlusThrow(gc.pThrowable);
                    }

                    GCPROTECT_END();
                }

                pEntry->m_hrResultCode = S_OK;

                // Set the initialization flags in the DLS and on domain-specific types.
                // Note we also set the flag for dynamic statics, which use the DynamicStatics part
                // of the DLS irrespective of whether the type is domain neutral or not.
                SetClassInited();

            }
            else
            {
                // Use previous result

                hrResult = pEntry->m_hrResultCode;
                if(FAILED(hrResult))
                {
                    // An exception may have occurred in the cctor. DoRunClassInit() should return FALSE in that
                    // case.
                    _ASSERTE(pEntry->m_hInitException);
                    _ASSERTE(IsInitError());

                    //
                    if (IsException(ObjectFromHandle(pEntry->m_hInitException)->GetMethodTable()))
                    {
                        ((EXCEPTIONREF)(ObjectFromHandle(pEntry->m_hInitException)))->ClearStackTraceForThrow();
                    }
                    COMPlusThrow(ObjectFromHandle(pEntry->m_hInitException));
                }
            }
        }
    }

    //
    // Notify any entries waiting on the current entry and wait for the required entries.
    //

    // We need to take the global lock before we play with the list of entries.

    STRESS_LOG2(LF_CLASSLOADER, LL_INFO1000, "DoRunClassInit: returning SUCCESS for init %pT in appdomain %p\n", this, pDomain);
    // No need to set pThrowable in case of error it will already have been set.

    g_IBCLogger.LogMethodTableAccess(this);
Exit:
    ;
    END_INTERIOR_STACK_PROBE;
}

#endif // !DACCESS_COMPILE

#ifndef DACCESS_COMPILE

//==========================================================================================
void MethodTable::SetInternalCorElementType (CorElementType _NormType, BOOL isTruePrimitive)
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClass());
    GetClass()->SetInternalCorElementType(_NormType);
    if (_NormType == ELEMENT_TYPE_SZARRAY)
    {
        m_wFlags |= enum_flag_IfArrayThenSzArray;
    }
    else if (_NormType == ELEMENT_TYPE_ARRAY)
    {
        m_wFlags &= ~enum_flag_IfArrayThenSzArray;
    }
    else
    {
        _ASSERTE(!IsArray());
        m_wFlags &= ~enum_flag_InternalCorElementTypeExtraInfoMask;
        if (_NormType == ELEMENT_TYPE_CLASS)
        {
            m_wFlags |= enum_flag_InternalCorElementTypeExtraInfo_IfNotArrayThenClass;
        }
        else if (_NormType == ELEMENT_TYPE_VALUETYPE)
        {
            m_wFlags |= enum_flag_InternalCorElementTypeExtraInfo_IfNotArrayThenValueType;
        }
        if (isTruePrimitive)
        {
            m_wFlags |= enum_flag_InternalCorElementTypeExtraInfo_IfNotArrayThenTruePrimitive;
        }
    }
}

#endif // !DACCESS_COMPILE




//==========================================================================================
BOOL MethodTable::FindEncodedMapDispatchEntry(UINT32 typeID,
                                              UINT32 slotNumber,
                                              DispatchMapEntry *pEntry,
                                              BOOL fIgnoreVirtualMappings)
{
    CONTRACTL {
        // NOTE: LookupDispatchMapType may or may not throw. Currently, it
        // should never throw because lazy interface restore is disabled.
        if (typeID == TYPE_ID_THIS_CLASS) NOTHROW; else WRAPPER(THROWS);
        if (typeID == TYPE_ID_THIS_CLASS) GC_NOTRIGGER; else GC_TRIGGERS;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pEntry));
        PRECONDITION(typeID != TYPE_ID_THIS_CLASS);
    } CONTRACTL_END;

    CONSISTENCY_CHECK(HasDispatchMap());

    UINT32 flags = typeID == TYPE_ID_THIS_CLASS ? DispatchMap::e_IT_FLAG_THIS_CLASS_ONLY : 0;
    MethodTable *dispatchTokenType =
        (typeID == TYPE_ID_THIS_CLASS) ? this : GetThread()->GetDomain()->LookupType(typeID);

    // Search for an exact type match.
    {
        DispatchMap::EncodedMapIterator it(this, flags);
        for (; it.IsValid(); it.Next())
        {
            DispatchMapEntry *pCurEntry = it.Entry();
            if (pCurEntry->GetSlotNumber() == slotNumber)
            {
                if (!(pCurEntry->IsVirtuallyMapped() && fIgnoreVirtualMappings))
                {
                    MethodTable *pCurEntryType = LookupDispatchMapType(pCurEntry->GetTypeID());
                    if (pCurEntryType == dispatchTokenType)
                    {
                        *pEntry = *pCurEntry;
                        return TRUE;
                    }
                    if (!g_pConfig->ExactInterfaceCalls() &&
                        pCurEntryType->GetCanonicalMethodTable() == dispatchTokenType->GetCanonicalMethodTable())
                    {
                        *pEntry = *pCurEntry;
                        return TRUE;
                    }

                }
            }
        }
    }

    // Repeat the search if any variance is involved, allowing a CanCastTo match.  (We do
    // this in a separate pass because we want to avoid touching the type
    // to see if it has variance or not)
    //
    // NOTE: CERs are not guaranteed for interfaces with co- and contra-variance involved.
    if (dispatchTokenType->HasVariance())
    {
        DispatchMap::EncodedMapIterator it(this, flags);
        for (; it.IsValid(); it.Next())
        {
            DispatchMapEntry *pCurEntry = it.Entry();
            if (pCurEntry->GetSlotNumber() == slotNumber)
            {
                if (!(pCurEntry->IsVirtuallyMapped() && fIgnoreVirtualMappings))
                {
                    // Workaround for bug in static scanner - there is no actual contract bug here.
                    CONTRACT_VIOLATION(ThrowsViolation|GCViolation);

#ifndef DACCESS_COMPILE
                    MethodTable *pCurEntryType = LookupDispatchMapType(pCurEntry->GetTypeID());
                    if (pCurEntryType->CanCastByVarianceToInterfaceOrDelegate(dispatchTokenType))
                    {
                        *pEntry = *pCurEntry;
                        return TRUE;
                    }
#endif // !DACCESS_COMPILE
                }
            }
        }
    }
    return FALSE;
}


//==========================================================================================
BOOL MethodTable::FindDispatchEntryForCurrentType(UINT32 typeID,
                                                  UINT32 slotNumber,
                                                  DispatchMapEntry *pEntry,
                                                  BOOL fIgnoreVirtualMappings,
                                                  BOOL fVirtualMethodsOnly)
{
    CONTRACTL {
        if (typeID == TYPE_ID_THIS_CLASS) NOTHROW; else THROWS;
        if (typeID == TYPE_ID_THIS_CLASS) GC_NOTRIGGER; else GC_TRIGGERS;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pEntry));
        PRECONDITION(typeID != TYPE_ID_THIS_CLASS);
    } CONTRACTL_END;

    BOOL fRes = FALSE;

    if (HasDispatchMap())
    {
        g_IBCLogger.LogDispatchMapAccess(this);
        fRes = FindEncodedMapDispatchEntry(
            typeID, slotNumber, pEntry, fIgnoreVirtualMappings);
    }

    return fRes;
}

//==========================================================================================
BOOL MethodTable::FindDispatchEntry(UINT32 typeID,
                                    UINT32 slotNumber,
                                    DispatchMapEntry *pEntry,
                                    BOOL fIgnoreVirtualMappings)
{
    CONTRACT (BOOL) {
        INSTANCE_CHECK;
        MODE_ANY;
        if (typeID == TYPE_ID_THIS_CLASS) NOTHROW; else THROWS;
        if (typeID == TYPE_ID_THIS_CLASS) GC_NOTRIGGER; else GC_TRIGGERS;
        PRECONDITION(!fIgnoreVirtualMappings || typeID == TYPE_ID_THIS_CLASS);
        POSTCONDITION(!RETVAL || pEntry->IsValid());
        PRECONDITION(typeID != TYPE_ID_THIS_CLASS);
    } CONTRACT_END;

    // Start at the current type and work up the inheritance chain
    MethodTable *pCurMT = this;
    UINT32 iCurInheritanceChainDelta = 0;
    while (pCurMT != NULL)
    {
        g_IBCLogger.LogMethodTableAccess(pCurMT);
        if (pCurMT->FindDispatchEntryForCurrentType(
                typeID, slotNumber, pEntry, fIgnoreVirtualMappings, pCurMT != this))
        {
            RETURN (TRUE);
        }
        pCurMT = pCurMT->GetParentMethodTable();
        iCurInheritanceChainDelta++;
    }
    RETURN (FALSE);
}

//==========================================================================================
// Possible cases:
//      1. Typed (interface) contract
//          a. To non-virtual implementation (NYI). Just
//             return the DispatchSlot as the implementation
//          b. Mapped virtually to virtual slot on 'this'. Need to
//             further resolve the new 'this' virtual slot.
//      2. 'this' contract
//          a. To non-virtual implementation. Return the DispatchSlot
//             as the implementation.
//          b. Mapped virtually to another virtual slot. Need to further
//             resolve the new slot on 'this'.
BOOL MethodTable::FindDispatchImpl(UINT32 typeID,
                                   UINT32 slotNumber,
                                   DispatchSlot *pDeclSlot,
                                   DispatchSlot *pImplSlot)
{
    CONTRACT (BOOL) {
        INSTANCE_CHECK;
        MODE_ANY;
        if (typeID == TYPE_ID_THIS_CLASS) NOTHROW; else THROWS;
        if (typeID == TYPE_ID_THIS_CLASS) GC_NOTRIGGER; else GC_TRIGGERS;
        PRECONDITION(CheckPointer(pImplSlot));
        POSTCONDITION(!RETVAL || !pImplSlot->IsNull() || IsComObjectType());
        POSTCONDITION(!RETVAL || pDeclSlot == NULL || !pDeclSlot->IsNull() || IsComObjectType());
        PRECONDITION(pDeclSlot == NULL);
    } CONTRACT_END;

    LOG((LF_LOADER, LL_INFO10000, "SD: MT::FindDispatchImpl: searching %s.\n", GetClass()->GetDebugClassName()));


    ///////////////////////////////////
    // 1. Typed (interface) contract

    INDEBUG(MethodTable *dbg_pMTTok = NULL; dbg_pMTTok = this;)
    DispatchMapEntry declEntry;
    DispatchMapEntry implEntry;

#ifndef DACCESS_COMPILE
    if (typeID != TYPE_ID_THIS_CLASS)
    {
        INDEBUG(dbg_pMTTok = GetThread()->GetDomain()->LookupType(typeID));
        DispatchMapEntry e;
        if (!FindDispatchEntry(typeID, slotNumber, &e, FALSE))
        {
            // A call to an array thru IList<T> (or IEnumerable<T> or ICollection<T>) has to be handled specially.
            // These interfaces are "magic" (mostly due to working set concerned - they are created on demand internally
            // even though semantically, these are static interfaces.)
            //
            // NOTE: CERs are not currently supported with generic array interfaces.
            if (IsArray())
            {

                CONTRACT_VIOLATION(ThrowsViolation|GCViolation); // Workaround for bug in static scanner - there is no actual contract bug here.

                // At this, we know that we're trying to cast an array to an interface and that the normal static lookup failed.

                // FindDispatchImpl assumes that the cast is legal so we should be able to assume now that it is a valid
                // IList<T> call thru an array.

                // Get the MT of IList<T>
                MethodTable *pIfcMT = GetThread()->GetDomain()->LookupType(typeID);

                // Quick sanity check
                if (!(pIfcMT->HasInstantiation()))
                {
                    _ASSERTE(!"Should not have gotten here. If you did, it's probably because multiple interface instantiation hasn't been checked in yet. This code only works on top of that.");
                    RETURN(FALSE);
                }

                // Get the type of T (as in IList<T>)
                TypeHandle theT = pIfcMT->GetInstantiation()[0];

                // Figure out which method of IList<T> the caller requested.
                MethodDesc *pIfcMD = pIfcMT->GetMethodDescForSlot(slotNumber);

                // Retrieve the corresponding method of SZArrayHelper. This is the guy that will actually execute.
                // This method will be an instantiation of a generic method. I.e. if the caller requested
                // IList<T>.Meth(), he will actually be diverted to SZArrayHelper.Meth<T>().
                MethodDesc  *pActualImplementor = GetActualImplementationForArrayGenericIListMethod(pIfcMD, theT);

                // Now, construct a DispatchSlot to return in *pDeclSlot and *pImplSlot
                DispatchSlot ds(pActualImplementor->GetAddrOfSlot());

                if (pDeclSlot)
                {
                   *pDeclSlot = ds;
                }
                if (pImplSlot)
                {
                   *pImplSlot = ds;
                }


                RETURN( TRUE );

            }

            // This contract is not implemented by this class or any parent class.
            RETURN (FALSE);
        }

        /////////////////////////////////
        // 1.a. Currently, all interface contracts are virtually mapped
        CONSISTENCY_CHECK(e.IsVirtuallyMapped());

        /////////////////////////////////
        // 1.b. Update the typeID and slotNumber so that the full search can commense below
        typeID = TYPE_ID_THIS_CLASS;
        slotNumber = e.GetTargetSlotNumber();
    }
#endif // !DACCESS_COMPILE

    //////////////////////////////////
    // 2. 'this' contract


    // Just grab the target out of the vtable
    *pImplSlot = PTR_TADDR(PTR_TO_TADDR(GetRestoredSlotPtr(slotNumber)));


    // Successfully determined the target for the given target
    RETURN (TRUE);
}

//==========================================================================================
DispatchSlot MethodTable::FindDispatchSlot(UINT32 typeID, UINT32 slotNumber)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    DispatchSlot implSlot(NULL);
    FindDispatchImpl(typeID, slotNumber, NULL, &implSlot);
    return implSlot;
}

//==========================================================================================
DispatchSlot MethodTable::FindDispatchSlot(UINT32 slotNumber)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return FindDispatchSlot(TYPE_ID_THIS_CLASS, slotNumber);
}

//==========================================================================================
DispatchSlot MethodTable::FindDispatchSlot(DispatchToken tok)
{
    WRAPPER_CONTRACT;
    return FindDispatchSlot(tok.GetTypeID(), tok.GetSlotNumber());
}

#ifndef DACCESS_COMPILE

//==========================================================================================
DispatchSlot MethodTable::FindDispatchSlotForInterfaceMD(MethodDesc *pMD)
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(CheckPointer(pMD));
    CONSISTENCY_CHECK(pMD->IsInterface());
    return FindDispatchSlotForInterfaceMD(TypeHandle(pMD->GetMethodTable()), pMD);
}

//==========================================================================================
DispatchSlot MethodTable::FindDispatchSlotForInterfaceMD(TypeHandle ownerType, MethodDesc *pMD)
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(!ownerType.IsNull());
    CONSISTENCY_CHECK(CheckPointer(pMD));
    CONSISTENCY_CHECK(pMD->IsInterface());
    return FindDispatchSlot(ownerType.GetMethodTable()->GetTypeID(), pMD->GetSlot());
}

//==========================================================================================
DispatchSlot MethodTable::FindRepresentativeDispatchSlot(UINT32 slotNumber)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
        PRECONDITION(!IsTransparentProxyType());
    } CONTRACTL_END;

    return DispatchSlot((PTR_TADDR)GetRestoredSlotPtr(slotNumber));
}

//==========================================================================================
// This is used for reverse methodimpl lookups by ComPlusMethodCall MDs.
// This assumes the following:
//      The methodimpl is for an interfaceToken->slotNumber
//      There is ONLY ONE such mapping for this slot number
//      The mapping exists in this type, not a parent type.
MethodDesc *MethodTable::ReverseInterfaceMDLookup(UINT32 slotNumber)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;
    DispatchMap::Iterator it(this);
    for (; it.IsValid(); it.Next()) {
        if (it.Entry()->IsVirtuallyMapped() && it.Entry()->GetTargetSlotNumber() == slotNumber) {
            if (!it.Entry()->GetTypeID().IsThisClass()) {
                DispatchMapTypeID typeID = it.Entry()->GetTypeID();
                UINT32 slotNum = it.Entry()->GetSlotNumber();
                MethodTable *pMTItf = LookupDispatchMapType(typeID);
                CONSISTENCY_CHECK(CheckPointer(pMTItf));

                return pMTItf->GetMethodDescForSlot((DWORD)slotNum);
            }
        }
    }
    CONSISTENCY_CHECK_MSG(FALSE, "Stub Dispatch: should not get here.");
        return NULL;
}

//==========================================================================================
UINT32 MethodTable::GetTypeID()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    PTR_MethodTable pMT = PTR_MethodTable(this);

    return GetDomain()->GetTypeID(pMT);
}

//==========================================================================================
UINT32 MethodTable::LookupTypeID()
{
    WRAPPER_CONTRACT;
    PTR_MethodTable pMT = PTR_MethodTable(this);

    return GetDomain()->LookupTypeID(pMT);
}

#endif // !DACCESS_COMPILE

//==========================================================================================
MethodTable *MethodTable::LookupDispatchMapType(DispatchMapTypeID typeID)
{
    CONTRACTL {
        WRAPPER(THROWS);
        GC_TRIGGERS;
    } CONTRACTL_END;

    if (typeID.IsThisClass())
    {
        return this;
    }
    else
    {
        InterfaceMapIterator intIt = IterateInterfaceMapFrom(typeID.GetInterfaceNum());
        return intIt.GetInterface();
    }
}


//==========================================================================================
// There is a case where a method declared in a type can be explicitly
// overridden by a methodImpl on another method within the same type. In
// this case, we need to call the methodImpl target, and this will map
// things appropriately for us.
MethodDesc *MethodTable::MapMethodDeclToMethodImpl(MethodDesc *pMDDecl)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    MethodTable *pMT = pMDDecl->GetMethodTable();

    //
    // Fast negative case check
    //

    // If it's not virtual, then it could not have been methodImpl'd.
    if (!pMDDecl->IsVirtual() ||
        // Is it a non-virtual call to the instantiating stub
        (pMT->IsValueType() && !pMDDecl->IsUnboxingStub()))
    {
        return pMDDecl;
    }


    MethodDesc *pMDImpl = pMT->GetMethodDescForSlot(pMDDecl->GetSlot());


    // If the method is instantiated, then we need to resolve to the corresponding
    // instantiated MD for the new slot number.
    if (pMDDecl->HasMethodInstantiation())
    {
        if (pMDDecl->GetSlot() != pMDImpl->GetSlot())
        {
            if (!pMDDecl->IsGenericMethodDefinition())
            {
#ifndef DACCESS_COMPILE
                pMDImpl = pMDDecl->FindOrCreateAssociatedMethodDesc(
                                        pMDImpl,
                                        pMT,
                                        pMDDecl->IsUnboxingStub(),
                                        pMDDecl->GetNumGenericMethodArgs(),
                                        pMDDecl->GetMethodInstantiation(),
                                        pMDDecl->IsInstantiatingStub());
#else
                DacNotImpl();
#endif
            }
        }
        else
        {
            // Since the generic method definition is always in the actual
            // slot for the method table, and since the slot numbers for
            // the Decl and Impl MDs are the same, then the call to
            // FindOrCreateAssociatedMethodDesc would just result in the
            // same pMDDecl being returned. In this case, we can skip all
            // the work.
            pMDImpl = pMDDecl;
        }
    }

    CONSISTENCY_CHECK(CheckPointer(pMDImpl));
    CONSISTENCY_CHECK(!pMDImpl->IsGenericMethodDefinition());
    return pMDImpl;
}

#ifndef DACCESS_COMPILE

//==========================================================================================
void MethodTable::GetGuid(GUID *pGuid, BOOL bGenerateIfNotFound)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;


    SIZE_T      cchName = 0;            // Length of the name (possibly after decoration).
    SIZE_T      cbCur;                  // Current offset.
    LPCWSTR     szName = NULL;          // Name to turn to a guid.
    CQuickArray<BYTE> rName;            // Buffer to accumulate signatures.
    BOOL        bGenerated = FALSE;     // A flag indicating if we generated the GUID from name.

    _ASSERTE(pGuid != NULL);

    // First check to see if we have already cached the guid for this type.
    // We currently only cache guids on interfaces.
    if (IsInterface() && GetGuidInfo())
    {
        if (GetGuidInfo()->m_bGeneratedFromName)
        {
            // If the GUID was generated from the name then only return it
            // if bGenerateIfNotFound is set.
            if (bGenerateIfNotFound)
                *pGuid = GetGuidInfo()->m_Guid;
            else
                *pGuid = GUID_NULL;
            }
        else
        {
            *pGuid = GetGuidInfo()->m_Guid;
        }
        return;
    }

    if (GetClass()->HasNoGuid())
        *pGuid = GUID_NULL;
    else
    {
        // If there is a GUID in the metadata then return that.
        GetMDImport()->GetItemGuid(GetCl(), pGuid);

        if (*pGuid == GUID_NULL)
        {
            // Remember that we didn't find the GUID, so we can skip looking during
            // future checks. (Note that this is a very important optimization in the
            // prejit case.)
            GetClass()->SetHasNoGuid();
        }
    }

    if (*pGuid == GUID_NULL && bGenerateIfNotFound)
    {
        // For interfaces, concatenate the signatures of the methods and fields.
        if (!IsNilToken(GetCl()) && IsInterface())
        {
            // Retrieve the stringized interface definition.
            cbCur = GetStringizedItfDef(TypeHandle(this), rName);

            // Pad up to a whole WCHAR.
            if (cbCur % sizeof(WCHAR))
            {
                SIZE_T cbDelta = sizeof(WCHAR) - (cbCur % sizeof(WCHAR));
                rName.ReSizeThrows(cbCur + cbDelta);
                memset(rName.Ptr() + cbCur, 0, cbDelta);
                cbCur += cbDelta;
            }

            // Point to the new buffer.
            cchName = cbCur / sizeof(WCHAR);
            szName = reinterpret_cast<LPWSTR>(rName.Ptr());
        }
        else
        {
            // Get the name of the class.
            DefineFullyQualifiedNameForClassW();
            szName = GetFullyQualifiedNameForClassNestedAwareW(GetClass());
            if (szName == NULL)
                return;
            cchName = wcslen(szName);

            // Enlarge buffer for class name.
            cbCur = cchName * sizeof(WCHAR);
            rName.ReSizeThrows(cbCur + sizeof(WCHAR));
            wcscpy_s(reinterpret_cast<LPWSTR>(rName.Ptr()), cchName + 1, szName);

            // Add the assembly guid string to the class name.
            ULONG cbCurOUT = (ULONG)cbCur;
            IfFailThrow(GetStringizedTypeLibGuidForAssembly(GetAssembly(), rName, (ULONG)cbCur, &cbCurOUT));
            cbCur = (SIZE_T) cbCurOUT;

            // Pad to a whole WCHAR.
            if (cbCur % sizeof(WCHAR))
            {
                rName.ReSizeThrows(cbCur + sizeof(WCHAR)-(cbCur%sizeof(WCHAR)));
                while (cbCur % sizeof(WCHAR))
                    rName[cbCur++] = 0;
            }

            // Point to the new buffer.
            szName = reinterpret_cast<LPWSTR>(rName.Ptr());
            cchName = cbCur / sizeof(WCHAR);
            // Dont' want to have to pad.
            _ASSERTE((sizeof(GUID) % sizeof(WCHAR)) == 0);
        }

        // Generate guid from name.
        CorGuidFromNameW(pGuid, szName, cchName);

        // Remeber we generated the guid from the type name.
        bGenerated = TRUE;
    }

    // Cache the guid in the type, if not already cached.
    // We currently only do this for interfaces.
    if (IsInterface() && !GetGuidInfo() && *pGuid != GUID_NULL)
    {
        // Allocate the guid information.
        GuidInfo *pInfo =
            (GuidInfo*)(void*)GetDomain()->GetHighFrequencyHeap()->AllocMem(sizeof(GuidInfo));
        pInfo->m_Guid = *pGuid;
        pInfo->m_bGeneratedFromName = bGenerated;

        // Set in in the interface method table.
        SetGuidInfo (pInfo);
    }
}

#endif // !DACCESS_COMPILE

//==========================================================================================
PTR_SLOT MethodTable::GetRestoredSlotPtr(DWORD slot)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_TOLERANT;
    } CONTRACTL_END;

    PTR_SLOT addr = NULL;


    // Grab the value out of the vtable.
    addr = GetSlotPtr(slot);


    CONSISTENCY_CHECK(CheckPointer(addr));


    CONSISTENCY_CHECK(CheckPointer(addr));
    return addr;
}

//==========================================================================================
MethodDesc* MethodTable::GetUnknownMethodDescForSlotAddress(SLOT addr, BOOL fSpeculative /*=FALSE*/)
{
    CONTRACT(MethodDesc *)
    {
        GC_NOTRIGGER;
        NOTHROW;
        SO_TOLERANT;
        POSTCONDITION(CheckPointer(RETVAL, NULL_NOT_OK));
        POSTCONDITION(RETVAL->m_pDebugMethodTable == NULL || // We must be in BuildMethdTableThrowing()
                      RETVAL->SanityCheck());
    }
    CONTRACT_END;

    // If we see shared fcall implementation as an argument to this
    // function, it means that a vtable slot for the shared fcall
    // got backpatched when it shouldn't have.  The reason we can't
    // backpatch this method is that it is an FCall that has many
    // MethodDescs for one implementation.  If we backpatch delegate
    // constructors, this function will not be able to recover the
    // MethodDesc for the method.
    //
    _ASSERTE_IMPL(!ECall::IsSharedFCallImpl((TADDR)addr) &&
                  "someone backpatched shared fcall implementation -- "
                  "see comment in code");

    MethodDesc* pMethodDesc = NULL;

    const BYTE *addrOfCode = (const BYTE*)(addr);


    IJitManager * pJM = ExecutionManager::FindJitMan(addr);

    if (pJM)
    {
        // Since we are walking in the class these should be methods so the cast should be valid
        pJM->JitCodeToMethodInfo(addr, &pMethodDesc);

        // method entry points have JitManagers, but JitCodeToMethodInfo does not return a MethodDesc
        if (NULL != pMethodDesc)
        {
            goto lExit;
        }
    }

    // Is it an FCALL?
    pMethodDesc = ECall::MapTargetBackToMethod((TADDR) addr);
    if (pMethodDesc != 0)
    {
        _ASSERTE(pMethodDesc->GetNativeCode() == (TADDR) addr);
        goto lExit;
    }

    pMethodDesc = MethodDesc::GetMethodDescFromStubAddr((TADDR)addrOfCode, fSpeculative);

lExit:

    RETURN(pMethodDesc);
}

//==========================================================================================
/* static*/
BOOL MethodTable::ComputeContainsGenericVariables(DWORD nGenericClassArgs, TypeHandle *pClassArgs)
{
    WRAPPER_CONTRACT;

    for (DWORD j = 0; j < nGenericClassArgs; j++)
    {
        if (pClassArgs[j].ContainsGenericVariables())
        {
            return TRUE;
        }
    }
    return FALSE;
}

#ifndef DACCESS_COMPILE

//==========================================================================================
BOOL MethodTable::SanityCheck()
{
    LEAF_CONTRACT;

    // strings have component size2, all other non-arrays should have 0
    _ASSERTE((GetComponentSize() <= 2) || IsArray());

    if (GetClassIfPresent() == NULL)
    {
        if (IsAsyncPinType())
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    // Can only easily sanity check method tables not representing
    // instantiated generic types.  For generics, its possible to be restoring
    // a MethodTable from one module, but the EEClass is defined in another and
    // may not yet have been restored.  However, let's at least try to make 
    // sure we have a valid EEClass pointer. 
    if (!HasInstantiation() && GetClass()->GetMethodTable() == NULL)
        return FALSE;
    if (GetNumGenericArgs() != 0)
        return TRUE;
    else
        return (GetClass()->GetMethodTable() == this ||
                CTPMethodTable::IsTPMethodTable(GetClass()->GetMethodTable()));
}

#endif // !DACCESS_COMPILE

//==========================================================================================
mdTypeDef MethodTable::GetCl()
{
    WRAPPER_CONTRACT;

    ClassCtorInfoEntry *pClassCtorInfoEntry = GetClassCtorInfoIfExists();
    if (pClassCtorInfoEntry)
    {
        _ASSERTE(pClassCtorInfoEntry->clTok == GetClass()->GetCl());
        return pClassCtorInfoEntry->clTok;
    }
    else
        return GetClass()->GetCl();
}

#ifndef DACCESS_COMPILE

//==========================================================================================
BOOL MethodTable::HasClassConstructor()
{
    WRAPPER_CONTRACT;

    ClassCtorInfoEntry *pClassCtorInfoEntry = GetClassCtorInfoIfExists();
    if (pClassCtorInfoEntry)
    {
        _ASSERTE((pClassCtorInfoEntry->CCtorSlot != MethodTable::NO_SLOT) == GetClass()->HasClassConstructor());
        return pClassCtorInfoEntry->CCtorSlot != MethodTable::NO_SLOT;
    }
    else
        return GetClass()->HasClassConstructor();
}

//==========================================================================================
void MethodTable::SetClassConstructorSlot (WORD wCCtorSlot)
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClass());
    _ASSERTE(GetClassCtorInfoIfExists() == NULL);
    GetClass()->SetClassConstructorSlot(wCCtorSlot);
}

#endif // !DACCESS_COMPILE

//==========================================================================================
WORD MethodTable::GetClassConstructorSlot()
{
    WRAPPER_CONTRACT;
    _ASSERTE_IMPL(HasClassConstructor());

    ClassCtorInfoEntry *pClassCtorInfoEntry = GetClassCtorInfoIfExists();
    if (pClassCtorInfoEntry)
    {
        _ASSERTE(pClassCtorInfoEntry->CCtorSlot == GetClass()->GetClassConstructorSlot());
        return pClassCtorInfoEntry->CCtorSlot;
    }
    else
        return GetClass()->GetClassConstructorSlot();
}

//==========================================================================================
WORD MethodTable::GetNumHandleStatics ()
{
    WRAPPER_CONTRACT;
    return GetClass()->GetNumHandleStatics();
}

//==========================================================================================
WORD MethodTable::GetNumBoxedStatics ()
{
    WRAPPER_CONTRACT;

    ClassCtorInfoEntry *pClassCtorInfoEntry = GetClassCtorInfoIfExists();
    if (pClassCtorInfoEntry)
    {
        _ASSERTE(pClassCtorInfoEntry->numBoxedStatics == GetClass()->GetNumBoxedStatics());
        return pClassCtorInfoEntry->numBoxedStatics;
    }
    else
    {
        g_IBCLogger.LogEEClassAndMethodTableAccess( GetClass() );
        return GetClass()->GetNumBoxedStatics();
    }
}

//==========================================================================================
ClassCtorInfoEntry* MethodTable::GetClassCtorInfoIfExists()
{
    WRAPPER_CONTRACT;
    g_IBCLogger.LogCCtorInfoReadAccess(this);

    if (IsZapped())
    {
        ModuleCtorInfo *pModuleCtorInfo = GetZapModule()->GetZapModuleCtorInfo();
        DPTR(PTR_MethodTable) ppMT = pModuleCtorInfo->ppMT;
        PTR_DWORD hotHashOffsets = pModuleCtorInfo->hotHashOffsets;
        PTR_DWORD coldHashOffsets = pModuleCtorInfo->coldHashOffsets;

        if (pModuleCtorInfo->numHotHashes)
        {
            DWORD hash = pModuleCtorInfo->GenerateHash(PTR_MethodTable(this), ModuleCtorInfo::HOT);
            _ASSERTE(hash < pModuleCtorInfo->numHotHashes);

            for (DWORD i = hotHashOffsets[hash]; i != hotHashOffsets[hash + 1]; i++)
            {
                _ASSERTE(ppMT[i]);
                if (PTR_TO_TADDR(ppMT[i]) == PTR_HOST_TO_TADDR(this))
                {
                    return pModuleCtorInfo->cctorInfoHot + i;
                }
            }
        }

        if (pModuleCtorInfo->numColdHashes)
        {
            DWORD hash = pModuleCtorInfo->GenerateHash(PTR_MethodTable(this), ModuleCtorInfo::COLD);
            _ASSERTE(hash < pModuleCtorInfo->numColdHashes);

            for (DWORD i = coldHashOffsets[hash]; i != coldHashOffsets[hash + 1]; i++)
            {
                _ASSERTE(ppMT[i]);
                if (PTR_TO_TADDR(ppMT[i]) == PTR_HOST_TO_TADDR(this))
                {
                    return pModuleCtorInfo->cctorInfoCold + (i - pModuleCtorInfo->numElementsHot);
                }
            }
        }
    }

    return NULL;
}

//==========================================================================================
// Given D<T> : C<List<T>> and a type handle D<string> we sometimes
// need to find the corresponding type handle
// C<List<string>> (C may also be some type
// further up the inheritance hierarchy).  GetInstantiationOfParentClass
// helps us do this by getting the corresponding instantiation of C, i.e.
// <List<string>>.
//
// pWhichParent: this is used identify which parent type we're interested in.
// It must be a canonical EEClass, e.g. for C<ref>.  This is used as a token for
// C<List<T>>.
//
// Note this only works for parent classes, not parent interfaces.
TypeHandle* MethodTable::GetInstantiationOfParentClass(MethodTable *pWhichParent)
{
    CONTRACT(TypeHandle*) {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pWhichParent));
        PRECONDITION(IsRestored());
        PRECONDITION(pWhichParent->IsRestored());
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    } CONTRACT_END;

    if (pWhichParent->IsSharedByGenericInstantiations())
        RETURN (GetPerInstInfo()[pWhichParent->GetNumDicts()-1]->GetInstantiation());
    else
        RETURN (pWhichParent->GetInstantiation());

}

#ifndef DACCESS_COMPILE


//==========================================================================================
ULONG MethodTable::MethodData::Release()
{
    LEAF_CONTRACT;
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
    ULONG cRef = (ULONG) InterlockedDecrement((volatile LONG *)&m_cRef);
    if (cRef == 0) {
        delete this;
    }
    return (cRef);
}


//==========================================================================================
void MethodTable::MethodData::ProcessMap(DispatchMapTypeID typeID,
                                         MethodTable *pMT,
                                         UINT32 iCurrentChainDepth,
                                         MethodDataEntry *rgWorkingData)
{
    CONTRACTL {
        // Can throw and GC due to DispatchMap::Iterator calling MapZapTypeID
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        CONSISTENCY_CHECK(!typeID.IsThisClass());
    } CONTRACTL_END;

    // Only iterate the non-interface entries. These are guaranteed to be first in the map.
    UINT32 itFlags = (typeID.IsThisClass() ? DispatchMap::e_IT_FLAG_THIS_CLASS_ONLY : 0);
    for (DispatchMap::EncodedMapIterator it(pMT, itFlags); it.IsValid(); it.Next()) {
        if (it.Entry()->GetTypeID() == typeID) {
            UINT32 curSlot = it.Entry()->GetSlotNumber();
            // If we're processing an interface, or it's for a virtual, or it's for a non-virtual
            // for the most derived type, we want to process the entry. In other words, we
            // want to ignore non-virtuals for parent classes.
            if (!typeID.IsThisClass() || curSlot < pMT->GetNumVirtuals() || iCurrentChainDepth == 0) {
                MethodDataEntry *pCurEntry = &rgWorkingData[curSlot];
                if (it.Entry()->IsVirtuallyMapped()) {
                    CONSISTENCY_CHECK(!typeID.IsThisClass() || curSlot < pMT->GetNumVirtuals());
                    if (!pCurEntry->IsDeclInit() && !pCurEntry->IsImplInit()) {
                        pCurEntry->SetImplData(it.Entry()->GetTargetSlotNumber());
                    }
                }
                CONSISTENCY_CHECK(it.Entry()->IsVirtuallyMapped());
            }
        }
    }
    if (typeID.IsThisClass()) {
        UINT32 iIntroducedMethodsStart = pMT->GetNumParentVirtuals();
        UINT32 cIntroducedMethods = (iCurrentChainDepth == 0 ? pMT->GetNumMethods() : pMT->GetNumVirtuals())
                                    - pMT->GetNumParentVirtuals();
        for (UINT32 i = 0; i < cIntroducedMethods; i++) {
            MethodDataEntry *pEntry = rgWorkingData + i+iIntroducedMethodsStart;
            if (!pEntry->IsDeclInit()) {
                pEntry->SetDeclData(iCurrentChainDepth, i);
            }
        }
    }
}

//==========================================================================================
UINT32 MethodTable::MethodDataObject::GetObjectSize(MethodTable *pMT)
{
    WRAPPER_CONTRACT;
    UINT32 cb = sizeof(MethodTable::MethodDataObject);
    cb += pMT->GetCanonicalMethodTable()->GetNumMethods() * sizeof(MethodDataObjectEntry);
    return cb;
}

//==========================================================================================
// This will fill in all the MethodEntry slots present in the current MethodTable
void MethodTable::MethodDataObject::Init(MethodTable *pMT, MethodData *pParentData)
{
    CONTRACTL {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(CheckPointer(pParentData, NULL_OK));
        PRECONDITION(!pMT->IsInterface());
        PRECONDITION(pParentData == NULL ||
                     (pMT->ParentEquals(pParentData->GetDeclMethodTable()) &&
                      pMT->ParentEquals(pParentData->GetImplMethodTable())));
    } CONTRACTL_END;

    m_pMT = pMT;

    ZeroMemory(GetEntryData(), sizeof(MethodDataObjectEntry) * GetNumMethods());
}


//==========================================================================================
MethodDesc *MethodTable::MethodDataObject::GetDeclMethodDesc(UINT32 slotNumber)
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(slotNumber < GetNumMethods());

    MethodDataObjectEntry *pEntry = GetEntry(slotNumber);
    MethodDesc *pMDRet = pEntry->GetDeclMethodDesc();
    if (pMDRet == NULL)
    {
        pMDRet = GetImplMethodDesc(slotNumber)->GetDeclMethodDesc(slotNumber);
        CONSISTENCY_CHECK(CheckPointer(pMDRet));
        pEntry->SetDeclMethodDesc(pMDRet);
    }
    return pMDRet;
}


//==========================================================================================
DispatchSlot MethodTable::MethodDataObject::GetImplSlot(UINT32 slotNumber)
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(slotNumber < GetNumMethods());
    return DispatchSlot(PTR_TADDR(m_pMT->GetSlotPtr(slotNumber)));
}

//==========================================================================================
MethodDesc *MethodTable::MethodDataObject::GetImplMethodDesc(UINT32 slotNumber)
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(slotNumber < GetNumMethods());
    MethodDataObjectEntry *pEntry = GetEntry(slotNumber);
    MethodDesc *pMDRet = pEntry->GetImplMethodDesc();
    if (pMDRet == NULL)
    {
        pMDRet = m_pMT->GetMethodDescForSlot(slotNumber);
        CONSISTENCY_CHECK(CheckPointer(pMDRet));
        pEntry->SetImplMethodDesc(pMDRet);
    }
    return pMDRet;
}

//==========================================================================================
MethodDesc *MethodTable::MethodDataInterface::GetDeclMethodDesc(UINT32 slotNumber)
{
    WRAPPER_CONTRACT;
//@TODO: For METHOD_ENTRY_CHUNKS, GetMethodDescFromStubAddr is insufficient, but
//@TODO: GetUnknownMethodDescForSlot is overkill. What's the right way to do this? </STRIP>
    MethodDesc *pMDRet;
    // Interface virtual slots must always point to the prepad of the MethodDesc
    // for that slot. Non-virtuals (statics are the only types allowed) may point
    // to real code, so we have to resort to slower lookup.
    if (slotNumber < GetNumVirtuals())
    {
        pMDRet = MethodDesc::GetMethodDescFromStubAddr((TADDR)m_pMT->GetSlot(slotNumber));
    }
    else
    {
        pMDRet = m_pMT->GetUnknownMethodDescForSlot(slotNumber);
    }
    return pMDRet;
}

//==========================================================================================
MethodDesc *MethodTable::MethodDataInterface::GetImplMethodDesc(UINT32 slotNumber)
{
    WRAPPER_CONTRACT;
    return MethodTable::MethodDataInterface::GetDeclMethodDesc(slotNumber);
}

//==========================================================================================
UINT32 MethodTable::MethodDataInterfaceImpl::GetObjectSize(MethodTable *pMTDecl)
{
    WRAPPER_CONTRACT;
    UINT32 cb = sizeof(MethodDataInterfaceImpl);
    cb += pMTDecl->GetNumMethods() * sizeof(MethodDataEntry);
    return cb;
}

//==========================================================================================
// This will fill in all the MethodEntry slots present in the current MethodTable
void MethodTable::MethodDataInterfaceImpl::Init(DispatchMapTypeID declTypeID,
                                                MethodData *pDecl,
                                                MethodData *pImpl)
{
    CONTRACTL {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        PRECONDITION(CheckPointer(pDecl));
        PRECONDITION(CheckPointer(pImpl));
        PRECONDITION(pDecl->GetDeclMethodTable()->IsInterface());
        PRECONDITION(!pImpl->GetDeclMethodTable()->IsInterface());
        PRECONDITION(pDecl->GetDeclMethodTable() == pDecl->GetImplMethodTable());
        PRECONDITION(pImpl->GetDeclMethodTable() == pImpl->GetImplMethodTable());
        PRECONDITION(pDecl == pImpl || declTypeID != DispatchMapTypeID::InterfaceNotImplementedID());
    } CONTRACTL_END;

    // Store and AddRef the decl and impl data.
    m_pDecl = pDecl;
    m_pDecl->AddRef();
    m_pImpl = pImpl;
    m_pImpl->AddRef();

    m_iNextChainDepth = 0;
    // Need side effects of the calls, but not the result.
    /* MethodTable *pDeclMT = */ pDecl->GetDeclMethodTable();
    /* MethodTable *pImplMT = */ pImpl->GetImplMethodTable();
    m_declTypeID = declTypeID;

    // Initialize each entry.
    for (UINT32 i = 0; i < GetNumMethods(); i++) {
        // Initialize the entry
        GetEntry(i)->Init();
    }
}

//==========================================================================================
MethodTable::MethodDataInterfaceImpl::MethodDataInterfaceImpl(DispatchMapTypeID declTypeID,
                                                              MethodData *pDecl,
                                                              MethodData *pImpl)
{
    WRAPPER_CONTRACT;
    Init(declTypeID, pDecl, pImpl);
}

//==========================================================================================
MethodTable::MethodDataInterfaceImpl::~MethodDataInterfaceImpl()
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(CheckPointer(m_pDecl));
    CONSISTENCY_CHECK(CheckPointer(m_pImpl));
    m_pDecl->Release();
    m_pImpl->Release();
}

//==========================================================================================
BOOL MethodTable::MethodDataInterfaceImpl::PopulateNextLevel()
{
    WRAPPER_CONTRACT;

    // Get the chain depth to next decode.
    UINT32 iChainDepth = GetNextChainDepth();

    // If the chain depth is MAX_CHAIN_DEPTH, then we've already parsed every parent.
    if (iChainDepth == MAX_CHAIN_DEPTH) {
        return FALSE;
    }

    // Now move up the chain to the target.
    MethodTable *pMTCur = m_pImpl->GetImplMethodTable();
    for (UINT32 i = 0; pMTCur != NULL && i < iChainDepth; i++) {
        pMTCur = pMTCur->GetParentMethodTable();
    }

    // If we reached the end, then we're done.
    if (pMTCur == NULL) {
        SetNextChainDepth(MAX_CHAIN_DEPTH);
        return FALSE;
    }

    ProcessMap(m_declTypeID, pMTCur, iChainDepth, GetEntryData());

    SetNextChainDepth(iChainDepth + 1);

    return TRUE;
}

//==========================================================================================
UINT32 MethodTable::MethodDataInterfaceImpl::MapToImplSlotNumber(UINT32 slotNumber)
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(slotNumber < GetNumMethods());

    MethodDataEntry *pEntry = GetEntry(slotNumber);
    while (!pEntry->IsImplInit() && PopulateNextLevel()) {}
    if (pEntry->IsImplInit()) {
        return pEntry->GetImplSlotNum();
    }
    else {
        return INVALID_SLOT_NUMBER;
    }
}

//==========================================================================================
DispatchSlot MethodTable::MethodDataInterfaceImpl::GetImplSlot(UINT32 slotNumber)
{
    WRAPPER_CONTRACT;
    UINT32 implSlotNumber = MapToImplSlotNumber(slotNumber);
    if (implSlotNumber == INVALID_SLOT_NUMBER) {
        return DispatchSlot(NULL);
    }
    return m_pImpl->GetImplSlot(implSlotNumber);
}

//==========================================================================================
MethodDesc *MethodTable::MethodDataInterfaceImpl::GetImplMethodDesc(UINT32 slotNumber)
{
    WRAPPER_CONTRACT;
    UINT32 implSlotNumber = MapToImplSlotNumber(slotNumber);
    if (implSlotNumber == INVALID_SLOT_NUMBER) {
        return NULL;
    }
    return m_pImpl->GetImplMethodDesc(MapToImplSlotNumber(slotNumber));
}

//==========================================================================================
void MethodTable::CheckInitMethodDataCache()
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    } CONTRACTL_END;
    if (s_pMethodDataCache == NULL)
    {
        UINT32 cb = MethodDataCache::GetObjectSize(8);
        NewHolder<BYTE> hb(new BYTE[cb]);
        MethodDataCache *pCache = new (hb.GetValue()) MethodDataCache(8);
        if (InterlockedCompareExchangePointer(
                (volatile PVOID *)&s_pMethodDataCache, pCache, NULL) == NULL)
        {
            hb.SuppressRelease();
        }
        // If somebody beat us, return and allow the holders to take care of cleanup.
        else
        {
            return;
        }
    }
}

//==========================================================================================
void MethodTable::ClearMethodDataCache()
{
    LEAF_CONTRACT;
    if (s_pMethodDataCache != NULL) {
        s_pMethodDataCache->Clear();
    }
}

//==========================================================================================
MethodTable::MethodData *MethodTable::FindMethodDataHelper(MethodTable *pMTDecl, MethodTable *pMTImpl)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        CONSISTENCY_CHECK(s_fUseMethodDataCache);
    } CONTRACTL_END;

    return s_pMethodDataCache->Find(pMTDecl, pMTImpl);
}

//==========================================================================================
MethodTable::MethodData *MethodTable::FindParentMethodDataHelper(MethodTable *pMT)
{
    WRAPPER_CONTRACT;
    MethodData *pData = NULL;
    if (s_fUseMethodDataCache && s_fUseParentMethodData) {
        if (!pMT->IsInterface()) {
            MethodTable *pMTParent = pMT->GetParentMethodTable();
            if (pMTParent != NULL) {
                pData = FindMethodDataHelper(pMTParent, pMTParent);
            }
        }
    }
    return pData;
}

//==========================================================================================
// This method does not cache the resulting MethodData object in the global MethodDataCache.
MethodTable::MethodData *MethodTable::GetMethodDataHelper(DispatchMapTypeID declTypeID,
                                                          MethodTable *     pMTDecl,
                                                          MethodTable *     pMTImpl)
{
    CONTRACTL {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        PRECONDITION(declTypeID != DispatchMapTypeID::InterfaceNotImplementedID());
        PRECONDITION(CheckPointer(pMTDecl, NULL_OK));
        PRECONDITION(CheckPointer(pMTImpl));
    } CONTRACTL_END;

    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;

    if (pMTDecl == NULL)
    {
        pMTDecl = pMTImpl->GetInterfaceMap()[declTypeID.GetInterfaceNum()].m_pMethodTable;
    }
    CONSISTENCY_CHECK(CheckPointer(pMTDecl));
    CONSISTENCY_CHECK(pMTDecl->IsInterface() && !pMTImpl->IsInterface());

    // Can't cache, since this is a custom method used in BuildMethodTable
    MethodDataWrapper hDecl(GetMethodData(pMTDecl, FALSE));
    MethodDataWrapper hImpl(GetMethodData(pMTImpl, FALSE));

    UINT32 cb = MethodDataInterfaceImpl::GetObjectSize(pMTDecl);
    NewHolder<BYTE> pb(new BYTE[cb]);
    MethodDataInterfaceImpl *pData = new (pb.GetValue()) MethodDataInterfaceImpl(declTypeID, hDecl, hImpl);
    pb.SuppressRelease();

    return pData;
}

//==========================================================================================
// The fCanCache argument determines if the resulting MethodData object can
// be added to the global MethodDataCache. This is used when requesting a
// MethodData object for a type currently being built.
MethodTable::MethodData *MethodTable::GetMethodDataHelper(MethodTable *pMTDecl,
                                                          MethodTable *pMTImpl,
                                                          BOOL fCanCache)
{
    CONTRACTL {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        PRECONDITION(CheckPointer(pMTDecl));
        PRECONDITION(CheckPointer(pMTImpl));
        PRECONDITION(pMTDecl == pMTImpl ||
                     (pMTDecl->IsInterface() && !pMTImpl->IsInterface()));
    } CONTRACTL_END;

    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;

    if (s_fUseMethodDataCache) {
        MethodData *pData = FindMethodDataHelper(pMTDecl, pMTImpl);
        if (pData != NULL) {
            return pData;
        }
    }

    // If we get here, there are no entries in the cache.
    MethodData *pData = NULL;
    if (pMTDecl == pMTImpl) {
        if (pMTDecl->IsInterface()) {
            pData = new MethodDataInterface(pMTDecl);
        }
        else {
            UINT32 cb = MethodDataObject::GetObjectSize(pMTDecl);
            NewHolder<BYTE> pb(new BYTE[cb]);
            MethodDataHolder h(FindParentMethodDataHelper(pMTDecl));
            pData = new (pb.GetValue()) MethodDataObject(pMTDecl, h.GetValue());
            pb.SuppressRelease();
        }
    }
    else {
        pData = GetMethodDataHelper(
            ComputeDispatchMapTypeID(pMTDecl, pMTImpl),
            pMTDecl,
            pMTImpl);
    }

    // Insert in the cache if it is active.
    if (fCanCache && s_fUseMethodDataCache) {
        s_pMethodDataCache->Insert(pData);
    }

    // Do not AddRef, already initialized to 1.
    return pData;
}

//==========================================================================================
// The fCanCache argument determines if the resulting MethodData object can
// be added to the global MethodDataCache. This is used when requesting a
// MethodData object for a type currently being built.
MethodTable::MethodData *MethodTable::GetMethodData(MethodTable *pMTDecl,
                                                    MethodTable *pMTImpl,
                                                    BOOL fCanCache)
{
    CONTRACTL {
        THROWS;
        WRAPPER(GC_TRIGGERS);
    } CONTRACTL_END;

    MethodDataWrapper hData(GetMethodDataHelper(pMTDecl, pMTImpl, fCanCache));
    hData.SuppressRelease();
    return hData;
}

//==========================================================================================
// This method does not cache the resulting MethodData object in the global MethodDataCache.
MethodTable::MethodData *MethodTable::GetMethodData(DispatchMapTypeID declTypeID,
                                                    MethodTable *pMTDecl,
                                                    MethodTable *pMTImpl)
{
    CONTRACTL {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        PRECONDITION(pMTDecl != pMTImpl);
        PRECONDITION(pMTDecl->IsInterface());
        PRECONDITION(!pMTImpl->IsInterface());
        PRECONDITION(declTypeID != DispatchMapTypeID::InterfaceNotImplementedID());
    } CONTRACTL_END;

    MethodDataWrapper hData(GetMethodDataHelper(declTypeID, pMTDecl, pMTImpl));
    hData.SuppressRelease();
    return hData;
}

//==========================================================================================
// The fCanCache argument determines if the resulting MethodData object can
// be added to the global MethodDataCache. This is used when requesting a
// MethodData object for a type currently being built.
MethodTable::MethodData *MethodTable::GetMethodData(MethodTable *pMT,
                                                    BOOL fCanCache)
{
    WRAPPER_CONTRACT;
    return GetMethodData(pMT, pMT, fCanCache);
}

//==========================================================================================
MethodTable::MethodIterator::MethodIterator(MethodTable *pMTDecl, MethodTable *pMTImpl)
{
    WRAPPER_CONTRACT;
    Init(pMTDecl, pMTImpl);
}

//==========================================================================================
MethodTable::MethodIterator::MethodIterator(MethodTable *pMT)
{
    WRAPPER_CONTRACT;
    Init(pMT, pMT);
}

//==========================================================================================
MethodTable::MethodIterator::MethodIterator(MethodData *pMethodData)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pMethodData));
    } CONTRACTL_END;

    m_pMethodData = pMethodData;
    m_pMethodData->AddRef();
    m_iCur = 0;
    m_iMethods = (INT32)m_pMethodData->GetNumMethods();
}

//==========================================================================================
MethodTable::MethodIterator::MethodIterator(const MethodIterator &it)
{
    WRAPPER_CONTRACT;
    m_pMethodData = it.m_pMethodData;
    m_pMethodData->AddRef();
    m_iCur = it.m_iCur;
    m_iMethods = it.m_iMethods;
}

//==========================================================================================
void MethodTable::MethodIterator::Init(MethodTable *pMTDecl, MethodTable *pMTImpl)
{
    CONTRACTL {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pMTDecl));
        PRECONDITION(CheckPointer(pMTImpl));
    } CONTRACTL_END;

    LOG((LF_LOADER, LL_INFO10000, "SD: MT::MethodIterator created for %s.\n", pMTDecl->GetDebugClassName()));

    m_pMethodData = MethodTable::GetMethodData(pMTDecl, pMTImpl);
    CONSISTENCY_CHECK(CheckPointer(m_pMethodData));
    m_iCur = 0;
    m_iMethods = (INT32)m_pMethodData->GetNumMethods();
}

//==========================================================================================
// Non-canonical types share the method bodies with the canonical type. So the canonical
// type can be said to own the method bodies. Hence, by default, IntroducedMethodIterator
// only lets you iterate methods of the canonical type. You have to pass in
// restrictToCanonicalTypes=FALSE to iterate methods through a non-canonical type.

MethodTable::IntroducedMethodIterator::IntroducedMethodIterator(
        MethodTable *pMT, 
        BOOL restrictToCanonicalTypes /* = TRUE */ )
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(pMT->IsCanonicalMethodTable() || !restrictToCanonicalTypes);
    m_pMethodDescChunk = pMT->GetClass()->GetChunks();
    m_iCurMethodInChunk = 0;
}

//==========================================================================================
BOOL MethodTable::IntroducedMethodIterator::Next()
{
    WRAPPER_CONTRACT;

    if (IsValid())
    {
        INT32 maxIndexInCurChunk = m_pMethodDescChunk->GetCount() -1;
        if (m_iCurMethodInChunk < maxIndexInCurChunk)
        {
            // Just skip to the next method in the same chunk
            m_iCurMethodInChunk++;
        }
        else
        {
            // We have walked all the methods in the current chunk. Move on
            // to the next chunk.
            m_pMethodDescChunk = m_pMethodDescChunk->GetNextChunk();
            m_iCurMethodInChunk = 0;
        }
    }

    return IsValid();
}

#endif // !DACCESS_COMPILE

//==========================================================================================
/*static*/
MethodTable *MethodTable::GetISerializableMT()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END
#ifdef DACCESS_COMPILE
    return NULL;
#else // !DACCESS_COMPILE
    if (s_ISerializableMT != NULL)
        return s_ISerializableMT;
    BEGIN_SO_INTOLERANT_CODE(GetThread())
    s_ISerializableMT = g_Mscorlib.GetClass(CLASS__ISERIALIZABLE);
    END_SO_INTOLERANT_CODE
    return s_ISerializableMT;
#endif // !DACCESS_COMPILE
}

//==========================================================================================
/*static*/
MethodTable *MethodTable::GetIObjectReferenceMT()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END
#ifdef DACCESS_COMPILE
    return NULL;
#else // !DACCESS_COMPILE
    if (s_IObjectReferenceMT != NULL)
        return s_IObjectReferenceMT;

    s_IObjectReferenceMT = g_Mscorlib.GetClass(CLASS__IOBJECTREFERENCE);
    return s_IObjectReferenceMT;
#endif // !DACCESS_COMPILE
}

//==========================================================================================
/*static*/
MethodTable *MethodTable::GetIDeserializationCBMT()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END
#ifdef DACCESS_COMPILE
    return NULL;
#else // !DACCESS_COMPILE
    if (s_IDeserializationCBMT != NULL)
        return s_IDeserializationCBMT;

    s_IDeserializationCBMT = g_Mscorlib.GetClass(CLASS__IDESERIALIZATIONCB);
    return s_IDeserializationCBMT;
#endif // !DACCESS_COMPILE
}


//==========================================================================================
GuidInfo *MethodTable::GetGuidInfo()
{
    LEAF_CONTRACT;
    _ASSERTE(IsInterface());
    _ASSERTE(GetClass());
    return GetClass()->GetGuidInfo();
}

//==========================================================================================
void MethodTable::SetGuidInfo(GuidInfo* pGuidInfo)
{
    LEAF_CONTRACT;
    _ASSERTE(GetClass());
    GetClass()->SetGuidInfo (pGuidInfo);
}

//==========================================================================================
CHECK MethodTable::CheckActivated()
{
    WRAPPER_CONTRACT;

    if (!IsArray())
    {
        CHECK(GetModule()->CheckActivated());
    }


    CHECK_OK;
}

#ifdef _MSC_VER
#ifndef _DEBUG
// Optimization intended for EnsureInstanceActive, IsIntrospectionOnly, EnsureActive only
#pragma optimize("t", on)
#endif
#endif // _MSC_VER
//==========================================================================================
VOID MethodTable::EnsureInstanceActive()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    EnsureActive();

    g_IBCLogger.LogEEClassAndMethodTableAccess(GetClass());

    if (GetClass()->HasModuleDependencies())
    {
        Module::DependencySetIterator i = GetClass()->IterateModuleDependencies();

        while (i.Next())
        {
            i.GetDependency()->EnsureActive();
        }
    }

    if (HasInstantiation())
    {
        INTERIOR_STACK_PROBE(GetThread());
        TypeHandle * pInstantiation = GetInstantiation();
        for (DWORD i = 0; i < GetNumGenericArgs(); i++)
            if (!pInstantiation[i].IsTypeDesc())
                pInstantiation[i].AsMethodTable()->EnsureInstanceActive();
        END_INTERIOR_STACK_PROBE;
    }

}

//==========================================================================================
BOOL MethodTable::IsIntrospectionOnly()
{
    WRAPPER_CONTRACT;
    return GetAssembly()->IsIntrospectionOnly();
}

//==========================================================================================
VOID MethodTable::EnsureActive()
{
    WRAPPER_CONTRACT;

    GetModule()->EnsureActive();
}

#ifdef _MSC_VER
#ifndef _DEBUG
#pragma optimize("", on)
#endif
#endif // _MSC_VER

//==========================================================================================
CHECK MethodTable::CheckInstanceActivated()
{
    WRAPPER_CONTRACT;

    CHECK(CheckActivated());

    if (GetClass()->HasModuleDependencies())
    {
        Module::DependencySetIterator i = GetClass()->IterateModuleDependencies();

        while (i.Next())
        {
            CHECK(i.GetDependency()->CheckActivated());
        }
    }

    CHECK_OK;
}

#ifdef DACCESS_COMPILE

//==========================================================================================
void
MethodTable::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    DAC_CHECK_ENUM_THIS();
    EMEM_OUT(("MEM: %p MethodTable\n", PTR_HOST_TO_TADDR(this)));

    DWORD size = GetEndOffsetOfOptionalMembers() + GetPerInstInfoSize() + GetInterfaceMapSize() + GetInstAndDictSize();
    DacEnumMemoryRegion(PTR_HOST_TO_TADDR(this), size);

    if (m_pEEClass.IsValid())
    {
        if (IsArray())
        {
            DacEnumMemoryRegion(PTR_HOST_TO_TADDR(m_pEEClass), sizeof(ArrayClass));
        }
        m_pEEClass->EnumMemoryRegions(flags);
    }
    if (m_pParentMethodTable.IsValid())
    {
        m_pParentMethodTable->EnumMemoryRegions(flags);
    }
    if (flags != CLRDATA_ENUM_MEM_MINI)
    {

        if (m_pWriteableData.IsValid())
        {
            m_pWriteableData.EnumMem();
        }

        if (HasDispatchMap())
        {
            DispatchMap *pMap = GetDispatchMap();
            pMap->EnumMemoryRegions(flags);
        }
    }
}

#endif // DACCESS_COMPILE

//==========================================================================================
BOOL MethodTable::ContainsGenericMethodVariables()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    TypeHandle* inst = GetInstantiation();
    for (DWORD i = 0; i < GetNumGenericArgs(); i++)
    {
        CONSISTENCY_CHECK(!inst[i].IsEncodedFixup());
        if (inst[i].ContainsGenericVariables(TRUE))
            return TRUE;
    }

    return FALSE;
}

//==========================================================================================
Module *MethodTable::GetDefiningModuleForOpenType()
{
    CONTRACT(Module*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        POSTCONDITION((ContainsGenericVariables() != 0) == (RETVAL != NULL));
    }
    CONTRACT_END

    if (ContainsGenericVariables())
    {
        TypeHandle* inst = GetInstantiation();
        for (DWORD i = 0; i < GetNumGenericArgs(); i++)
        {
            // Encoded fixups are never open types
            if (!inst[i].IsEncodedFixup())
            {
                Module *pModule = inst[i].GetDefiningModuleForOpenType();
                if (pModule != NULL)
                    RETURN pModule;
            }
        }
    }

    RETURN NULL;
}

//==========================================================================================
PTR_SLOT MethodTable::GetSlotPtr(UINT32 slotNumber)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;
    MethodTable *pMT = this;
    if (!(slotNumber < pMT->GetNumMethods())) {
        CONSISTENCY_CHECK(!IsCanonicalMethodTable());
        pMT = pMT->GetCanonicalMethodTable();
    }
    CONSISTENCY_CHECK(slotNumber < pMT->GetNumMethods());
    return pMT->GetVtable() + slotNumber;
}

//==========================================================================================
SLOT MethodTable::GetRestoredSlot(DWORD slotNumber)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;
    CONSISTENCY_CHECK(slotNumber < GetNumMethods());
    return *GetRestoredSlotPtr(slotNumber);
}

#ifndef DACCESS_COMPILE

//==========================================================================================
void MethodTable::SetSlot(UINT32 slotNumber, SLOT slotCode)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    GetVtable()[slotNumber] = slotCode;
}

//==========================================================================================
void MethodTable::SetSlot_Log(UINT32 slotNumber, SLOT slotCode)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    g_IBCLogger.LogMethodTableWriteAccess(this);
    SetSlot(slotNumber, slotCode);
}

#endif // !DACCESS_COMPILE

#ifndef DACCESS_COMPILE


//==========================================================================================
BOOL MethodTable::HasPublicDefaultConstructor()
{
    WRAPPER_CONTRACT;

    MethodDesc *pMD = NULL;
    if (HasDefaultConstructor())
        pMD = GetDefaultConstructor();
    return (pMD != NULL && pMD->IsPublic());
}


//==========================================================================================
BOOL MethodTable::HasExplicitOrImplicitPublicDefaultConstructor()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END

    MethodDesc *pCanonMD = NULL;
    if (HasDefaultConstructor())
    {
        MethodTable *pCanonMT = GetCanonicalMethodTable();
        pCanonMD = pCanonMT->GetMethodDescForSlot(pCanonMT->GetDefaultConstructorSlot());
    }

    if (IsValueType())
    {
        // valuetypes have public default ctors implicitly
        return TRUE;
    }
    else
    {
        return pCanonMD != NULL && pCanonMD->IsPublic();
    }
}

//==========================================================================================
MethodDesc *MethodTable::GetDefaultConstructor()
{
    WRAPPER_CONTRACT;
    _ASSERTE(HasDefaultConstructor());
    // The default constructor in a generic class is shared amongst
    // compatible instantiations, i.e. the slot is only valid within the
    // canonical method table.
    MethodTable *pCanonMT = GetCanonicalMethodTable();
    MethodDesc *pCanonMD = pCanonMT->GetMethodDescForSlot(pCanonMT->GetDefaultConstructorSlot());
    // The default constructor for a value type is an instantiating stub.
    // The easiest way to find the right stub is to use the following function,
    // which in the simlpe case of the default constructor for a class simply
    // returns pCanonMD immediately.
    return MethodDesc::FindOrCreateAssociatedMethodDesc(pCanonMD,
                                                        this,
                                                        FALSE /* no BoxedEntryPointStub */,
                                                        0, NULL, /* no method instantiation */
                                                        FALSE /* no allowInstParam */);
}


//==========================================================================================
// TryResolveConstraintMethodApprox
// finds the (non-unboxing) MethodDesc that implements the interface
// method pInterfaceMD.
//
// Note our ability to resolve constraint methods is affected by the
// degree of code sharing we are performing for generic code.
MethodDesc *MethodTable::TryResolveConstraintMethodApprox(TypeHandle thInterfaceType,
                                                          MethodDesc *pInterfaceMD)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    MethodDesc *pRes = NULL;

    // We can't resolve constraint calls effectively for reference types, and there's
    // not a lot of perf. benefit in doing it anyway.
    //
    if (!IsValueType())
    {
        LOG((LF_JIT, LL_INFO10000, "TryResolveConstraintmethodApprox: not a value type %s\n", GetDebugClassName()));
        return NULL;
    }

    // 1. Find the (possibly generic) method that would implement the
    // constraint if we were making a call on a boxed value type.

    MethodTable *pCanonMT = GetCanonicalMethodTable();

    MethodDesc *pGenInterfaceMD = pInterfaceMD->StripMethodInstantiation();
    MethodDesc *pGenMD = NULL;
    if (pGenInterfaceMD->IsInterface())
    {
        // Sometimes (when compiling shared generic code)
        // we don't have enough exact type information at JIT time
        // even to decide whether we will be able to resolve to an unboxed entry point...
        // To cope with this case we always go via the helper function if there's any
        // chance of this happening by checking for all interfaces which might possibly
        // be compatible with the call (verification will have ensured that
        // at least one of them will be)
        INDEBUG(BOOL foundAtLeastOne = false;)
        MethodTable::InterfaceMapIterator it = pCanonMT->IterateInterfaceMap();
        while (it.Next())
        {
            TypeHandle thPotentialInterfaceType(it.GetInterface());
            if (thPotentialInterfaceType.GetMethodTable()->GetCanonicalMethodTable() == thInterfaceType.GetMethodTable()->GetCanonicalMethodTable())
            {
                INDEBUG(foundAtLeastOne = true;)
                pGenMD = pCanonMT->GetMethodDescForInterfaceMethod(thPotentialInterfaceType, pGenInterfaceMD);

                if (pGenMD && !pGenMD->GetMethodTable()->IsValueType())
                {
                    LOG((LF_JIT, LL_INFO10000, "TryResolveConstraintMethodApprox: %s::%s not a value type method\n",
                         pGenMD->m_pszDebugClassName, pGenMD->m_pszDebugMethodName));
                    return NULL;
                }
            }
        }

        _ASSERTE(foundAtLeastOne);

        // If we can resolve the interface exactly then do so (e.g. when doing the exact
        // lookup at runtime, or when not sharing generic code).  We will have found a candidate above,
        // but this will make sure we fetch exactly the right one.
        if (pCanonMT->CanCastToInterface(thInterfaceType.GetMethodTable()))
        {
            pGenMD = pCanonMT->GetMethodDescForInterfaceMethod(thInterfaceType, pGenInterfaceMD);
            if (pGenMD == NULL)
            {
                    LOG((LF_JIT, LL_INFO10000, "TryResolveConstraintMethodApprox: failed to find method desc for interface method\n"));
            }
        }
    }
    else if (pGenInterfaceMD->IsVirtual())
    {
        pGenMD = GetMethodDescForSlot(pGenInterfaceMD->GetSlot());
    }
    else
    {
        // The pStaticGenMD will be NULL if calling a non-virtual instance
        // methods on System.Object, i.e. when these are used as a constraint.
        pGenMD = NULL;
    }

    // Only return a method if the value type itself declares the method,
    // otherwise we might get a method from Object or System.ValueType
    if (pGenMD && !pGenMD->GetMethodTable()->IsValueType())
        pGenMD = NULL;

    // We've resolved the method, ignoring its generic method arguments
    // If the method is a generic method then go and get the instantiated descriptor
    if (pGenMD)
    {
        pRes = MethodDesc::FindOrCreateAssociatedMethodDesc(
            pGenMD,
            this,
            FALSE /* no BoxedEntryPointStub */ ,
            pInterfaceMD->GetNumGenericMethodArgs(),
            pInterfaceMD->GetMethodInstantiation(),
            FALSE /* no allowInstParam */ );

        // FindOrCreateAssociatedMethodDesc won't return an BoxedEntryPointStub.
        _ASSERTE(pRes);
        _ASSERTE(!pRes->IsUnboxingStub());

    }
    else
    {
        pRes = NULL;
    }

    return pRes;
}

//==========================================================================================
// Make best-case effort to obtain an image name for use in an error message.
//
// This routine must expect to be called before the this object is fully loaded.
// It can return an empty if the name isn't available or the object isn't initialized
// enough to get a name, but it mustn't crash.
//==========================================================================================
void MethodTable::GetPathForErrorMessages(SString & result)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    EEClass *pClass = GetClass();
    if (pClass)
    {
        pClass->GetPathForErrorMessages(result);
    }
    else
    {
        result = L"";
    }
}

//==========================================================================================
// context and thread static functions
void MethodTable::SetupThreadOrContextStatics(AllocMemTracker *pamTracker, WORD wThreadStaticsSize, WORD wContextStaticsSize)
{
    WRAPPER_CONTRACT;

    ThreadAndContextStaticsBucket* pTCSInfo = (ThreadAndContextStaticsBucket*) pamTracker->Track(GetDomain()->GetLowFrequencyHeap()->AllocMem(sizeof(ThreadAndContextStaticsBucket)));
    *(GetThreadOrContextStaticsBucketPtr()) = pTCSInfo;

    pTCSInfo->m_dwThreadStaticsOffset = (DWORD)-1; // Initialized lazily
    pTCSInfo->m_dwContextStaticsOffset = (DWORD)-1; // Initialized lazily

    pTCSInfo->m_wThreadStaticsSize = wThreadStaticsSize;
    pTCSInfo->m_wContextStaticsSize = wContextStaticsSize;
}

//==========================================================================================
DWORD MethodTable::AllocateThreadOrContextStaticsOffset(BOOL fContextStatics)
{
    WRAPPER_CONTRACT;

    g_IBCLogger.LogMethodTableWriteableDataWriteAccess(this);

    BaseDomain* pDomain = IsDomainNeutral() ?  SystemDomain::System() : GetDomain();

    ThreadAndContextStaticsBucket* pTCSInfo = GetThreadOrContextStaticsBucket();
    DWORD* pOffsetSlot = fContextStatics ? &pTCSInfo->m_dwContextStaticsOffset : &pTCSInfo->m_dwThreadStaticsOffset;

    return pDomain->AllocateThreadOrContextStaticsOffset(pOffsetSlot, fContextStatics);
}

#define ASSERT_AND_CHECK(x) {_ASSERTE(x); if (!(x)) return FALSE;}

BOOL MethodTable::Validate()
{
    WRAPPER_CONTRACT;

    ASSERT_AND_CHECK(SanityCheck());        
    
#ifdef _DEBUG    
    DWORD dwLastVerifiedGCCnt = m_dwLastVerifedGCCnt;
    _ASSERTE(dwLastVerifiedGCCnt == (DWORD)-1 || dwLastVerifiedGCCnt <= GCHeap::GetGCHeap()->GetGcCount());
    if (g_pConfig->FastGCStressLevel () > 1 && dwLastVerifiedGCCnt == GCHeap::GetGCHeap()->GetGcCount())
        return TRUE;
#endif //_DEBUG         

    if (IsArray())
    {
        if (!IsAsyncPinType())
        {
            if (GetClass()->IsArrayClass() == FALSE)
            {
                ASSERT_AND_CHECK(!"Detected use of a corrupted OBJECTREF. Possible GC hole.");
            }

            if (!SanityCheck())
            {
                ASSERT_AND_CHECK(!"Detected use of a corrupted OBJECTREF. Possible GC hole.");
            }
        }
    }
    else if (!IsCanonicalMethodTable())
    {
        // The special case where this can happen is Context proxies, where we
        // build a small number of large VTables and share them.
        // Also: instantiated classes
        if ((!IsThunking() ||!GetClass()->IsThunking()) && GetInstantiation() == NULL)
        {
            ASSERT_AND_CHECK(!"Detected use of a corrupted OBJECTREF. Possible GC hole.");
        }
    }
    
#ifdef _DEBUG    
    //it's OK to race here. m_dwLastVerifedGCCnt <= GCHeap::GetGCHeap()->GetGcCount()
    //should always hold
    m_dwLastVerifedGCCnt = GCHeap::GetGCHeap()->GetGcCount();
#endif //_DEBUG

    return TRUE;
}

#endif // !DACCESS_COMPILE
