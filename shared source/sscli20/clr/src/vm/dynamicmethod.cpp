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

#include "common.h"
#include "dynamicmethod.h"
#include "object.h"
#include "method.hpp"
#include "comdelegate.h"
#include "security.h"
#include "field.h"
#include "contractimpl.h"
#include "nibblemapmacros.h"

#ifndef DACCESS_COMPILE 

inline static DynamicResolver* GET_RESOLVER(MethodDesc* pMD)
{
    return pMD->GetDynamicMethodDesc()->GetResolver();
}

InfoAccessType CEEDynamicCodeInfo::constructStringLiteral(
    CORINFO_MODULE_HANDLE moduleHnd,
    mdToken metaTok,
    void **ppInfo)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsDynamicScope(moduleHnd));
    }
    CONTRACTL_END;

    _ASSERTE(ppInfo != NULL);
    *ppInfo = NULL;

    DynamicResolver* pResolver = GetDynamicResolver(moduleHnd);

    OBJECTHANDLE string = NULL;
    STRINGREF       strRef = ObjectToSTRINGREF(pResolver->GetStringLiteral(metaTok));

    GCPROTECT_BEGIN(strRef);

    if (strRef != NULL)
    {
        MethodDesc* pMD = pResolver->GetDynamicMethod();
        string = (OBJECTHANDLE)pMD->GetModule()->GetAssembly()->Parent()->GetOrInternString(&strRef);
    }

    GCPROTECT_END();

    *ppInfo = (LPVOID)string;
    return IAT_PVALUE;
}

void CEEDynamicCodeInfo::getEHinfo(
    CORINFO_METHOD_HANDLE ftn,
    unsigned EHnumber,
    CORINFO_EH_CLAUSE* clause)
{
    CONTRACTL
{
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(GetMethod(ftn)->IsDynamicMethod());
}
    CONTRACTL_END;

    GetMethod(ftn)->GetDynamicMethodDesc()->GetResolver()->GetEHInfo(EHnumber, clause);
}

CORINFO_CLASS_HANDLE CEEDynamicCodeInfo::findClassInternal(
    CORINFO_MODULE_HANDLE module,
    unsigned metaTOK,
    CORINFO_METHOD_HANDLE context,
    BOOL permitUninstantiatedType,
    OBJECTREF *pThrowable)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsDynamicScope(module));
    }
    CONTRACTL_END;

    TypeHandle classHandle;

    classHandle = TypeHandle::FromPtr(GetDynamicResolver(module)->ResolveToken(metaTOK));

#ifndef DACCESS_COMPILE
    if (!permitUninstantiatedType && !classHandle.IsNull() && classHandle.IsGenericTypeDefinition())
    {
        classHandle = TypeHandle(); // caller will throw we don't fill the throwable (should we?)
    }
#endif
    
    return CORINFO_CLASS_HANDLE(classHandle.AsPtr());
}

FieldDesc* CEEDynamicCodeInfo::findField(
    CORINFO_MODULE_HANDLE module,
    unsigned metaTOK,
    const SigTypeContext *context,
    MethodTable **ppOwner, 
    DWORD *pdwSecurityControlFlags, 
    OBJECTREF *pThrowable)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsDynamicScope(module));
        PRECONDITION(CheckPointer(ppOwner));
        PRECONDITION(CheckPointer(pdwSecurityControlFlags));
        PRECONDITION(CheckPointer(pThrowable));
    }
    CONTRACTL_END;

        FieldDesc *pField;
    DynamicResolver* pResolver = GetDynamicResolver(module);

    pField = (FieldDesc*)pResolver->ResolveToken(metaTOK);

    TypeHandle typeOwner;
    pResolver->GetJitContext(pdwSecurityControlFlags, &typeOwner);
    if (!typeOwner.IsNull()) *ppOwner = typeOwner.GetMethodTable();
    if (pField&& pField->IsStatic() && (
                            pField->GetModule()!=GetModule(module)||
                            pField->GetEnclosingMethodTable()->GetNumGenericArgs()!=0))
        pField->GetEnclosingMethodTable()->EnsureInstanceActive();
    return pField;
}

MethodDesc* CEEDynamicCodeInfo::findMethod(CORINFO_MODULE_HANDLE module,
                                                     unsigned metaTOK,
                                           const SigTypeContext *context,
                                           MethodTable **ppOwner, 
                                           DWORD *pdwSecurityControlFlags, 
                                           OBJECTREF *pThrowable)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsDynamicScope(module));
    }
    CONTRACTL_END;

    DynamicResolver* pResolver = GetDynamicResolver(module);
    MethodDesc* pCallee = (MethodDesc*)pResolver->ResolveToken(metaTOK);

    TypeHandle typeOwner;
    pResolver->GetJitContext(pdwSecurityControlFlags, &typeOwner);

    if (!typeOwner.IsNull()) *ppOwner = typeOwner.GetMethodTable();

    // if this is a BoxedEntryPointStub get the UnboxedEntryPoint one
    if (pCallee && pCallee->IsUnboxingStub())
    {
        _ASSERTE(!pCallee->ContainsGenericVariables());
        pCallee = pCallee->GetMethodTable()->GetUnboxedEntryPointMD(pCallee);
    }
    if (pCallee && 
        (pCallee->GetNumGenericClassArgs()!=0 ||
        pCallee->GetNumGenericMethodArgs()!=0 ||
        pCallee->GetModule()!=GetModule(module)))
            pCallee->EnsureActive();
    return pCallee;
}

unsigned CEEDynamicCodeInfo::getMemberParent(
    CORINFO_MODULE_HANDLE scopeHnd,
    unsigned metaTOK)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsDynamicScope(scopeHnd));
    }
    CONTRACTL_END;

        if (TypeFromToken(metaTOK) == mdtTypeDef)
            return metaTOK;

    return GetDynamicResolver(scopeHnd)->ResolveParentToken(metaTOK);
}

CORINFO_CLASS_HANDLE CEEDynamicCodeInfo::findMethodClass(
    CORINFO_MODULE_HANDLE module,
    mdToken methodTok,
    CORINFO_METHOD_HANDLE context)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsDynamicScope(module));
    }
    CONTRACTL_END;

    MethodDesc* pMethod = GetMethod((CORINFO_METHOD_HANDLE)GetDynamicResolver(module)->ResolveToken(methodTok));
    MethodTable *pMT = pMethod->GetMethodTable();
    return CORINFO_CLASS_HANDLE(pMT);
}

PCCOR_SIGNATURE CEEDynamicCodeInfo::findSig(
    CORINFO_MODULE_HANDLE *module,
    unsigned *sigTOK,
    CORINFO_METHOD_HANDLE context
    )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsDynamicScope(*module));
    }
    CONTRACTL_END;

    DynamicResolver* pResolver = GetDynamicResolver(*module);
    MethodDesc*      pMethod   = GetMethod(context);

    PCCOR_SIGNATURE pSig = pResolver->ResolveSignature(*sigTOK);
    *sigTOK = mdTokenNil;
    *module = GetScopeHandle(pMethod->GetModule());
    return pSig;
}

BOOL CEEDynamicCodeInfo::isValidToken(
    CORINFO_MODULE_HANDLE module,
    unsigned metaTOK)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsDynamicScope(module));
    }
    CONTRACTL_END;

    return GetDynamicResolver(module)->IsValidToken(metaTOK);
}


BOOL CEEDynamicCodeInfo::isValidStringRef(
    CORINFO_MODULE_HANDLE module,
    unsigned metaTOK)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsDynamicScope(module));
    }
    CONTRACTL_END;

    STRINGREF retStr = ObjectToSTRINGREF(GetDynamicResolver(module)->GetStringLiteral(metaTOK));
        return (retStr != NULL) ? TRUE : FALSE;
    }

PCCOR_SIGNATURE CEEDynamicCodeInfo::findCallSiteSig(
    CORINFO_MODULE_HANDLE *module,
    unsigned *methTOK,
    CORINFO_METHOD_HANDLE *context)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        CONSISTENCY_CHECK(IsDynamicScope(*module));
    }
    CONTRACTL_END;

    DynamicResolver* pResolver = GetDynamicResolver(*module);
    PCCOR_SIGNATURE pSig = NULL;

    if (TypeFromToken(*methTOK) == mdtMemberRef)
    {
        pSig = pResolver->ResolveSignatureForVarArg(*methTOK);
        *module = GetScopeHandle(GetMethod(*context)->GetModule());
        }
    else
    {
        _ASSERTE(TypeFromToken(*methTOK) == mdtMethodDef);
            // in this case a method is asked for its sig. Resolve the method token and get the sig
        CORINFO_METHOD_HANDLE calleeHnd = (CORINFO_METHOD_HANDLE)pResolver->ResolveToken(*methTOK);
        pSig = GetMethod(calleeHnd)->GetSig();
        *context = calleeHnd;
        *module = GetScopeHandle(GetMethod(calleeHnd)->GetModule());
    }

    *methTOK = mdTokenNil;

    return pSig; 
}

size_t CEEDynamicCodeInfo::findNameOfToken(
    CORINFO_MODULE_HANDLE module,
    mdToken metaTOK,
    __out_ecount (FQNameCapacity) char * szFQName,
    size_t FQNameCapacity
    )
{
    LEAF_CONTRACT;
    //TBI: used in debug mode. Implement at some point
    strncpy_s (szFQName, FQNameCapacity, "DynamicToken", FQNameCapacity - 1);
    return strlen (szFQName);
}

#endif // !DACCESS_COMPILE

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// DynamicMethodTable related code
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

#ifndef DACCESS_COMPILE 

// get the method table for dynamic methods
DynamicMethodTable* DomainFile::GetDynamicMethodTable()
{
    CONTRACT (DynamicMethodTable*)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        POSTCONDITION(CheckPointer(m_pDynamicMethodTable));
    }
    CONTRACT_END;

    if (!m_pDynamicMethodTable)
        DynamicMethodTable::CreateDynamicMethodTable(&m_pDynamicMethodTable, GetModule(), GetAppDomain());

    RETURN m_pDynamicMethodTable;
}

void ReleaseDynamicMethodTable(DynamicMethodTable *pDynMT)
{
    WRAPPER_CONTRACT;
    if (pDynMT)
    {
        pDynMT->Destroy();
    }
}

void DynamicMethodTable::CreateDynamicMethodTable(DynamicMethodTable **ppLocation, Module *pModule, BaseDomain *pDomain)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(ppLocation));
        PRECONDITION(CheckPointer(pModule));
        POSTCONDITION(CheckPointer(*ppLocation));
    }
    CONTRACT_END;

    AllocMemTracker amt;

    LoaderHeap* pHeap = pDomain->GetHighFrequencyHeap();
    _ASSERTE(pHeap);

    if (*ppLocation) RETURN;

    DynamicMethodTable* pDynMT = (DynamicMethodTable*)
            amt.Track(pHeap->AllocMem(sizeof(DynamicMethodTable)));

    // Note: Memory allocated on loader heap is zero filled
    // memset((void*)pDynMT, 0, sizeof(DynamicMethodTable));

    if (*ppLocation) RETURN;

    LOG((LF_BCL, LL_INFO100, "Level2 - Creating DynamicMethodTable {0x%p}...\n", pDynMT));

    Holder<DynamicMethodTable*, DoNothing, ReleaseDynamicMethodTable> dynMTHolder(pDynMT);
    pDynMT->m_Crst.Init("Dynamic Method Generation", CrstDynamicMT, CRST_DEFAULT);
    pDynMT->m_Module = pModule;
    pDynMT->m_pDomain = pDomain;
    pDynMT->MakeMethodTable(&amt);

    if (*ppLocation) RETURN;

    if (FastInterlockCompareExchangePointer((void**)ppLocation, *(void**)&pDynMT, NULL) != NULL)
    {
        LOG((LF_BCL, LL_INFO100, "Level2 - Another thread got here first - deleting DynamicMethodTable {0x%p}...\n", pDynMT));
        RETURN;
    }

    dynMTHolder.SuppressRelease();

    amt.SuppressRelease();
    LOG((LF_BCL, LL_INFO10, "Level1 - DynamicMethodTable created {0x%p}...\n", pDynMT));
    RETURN;
}

MethodTable* CreateMinimalMethodTable(Module* pContainingModule, 
                                      LoaderHeap* pCreationHeap,
                                      AllocMemTracker* pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    MethodTable* pMT = NULL;

    WORD wNumVtableSlots = (WORD)g_pObjectClass->GetNumVirtuals();

    // GC info
    size_t cbMT = sizeof(MethodTable);

    cbMT += (wNumVtableSlots * sizeof(SLOT));

    // Inherit top level class's interface map
    cbMT += g_pObjectClass->GetNumInterfaces() * sizeof(InterfaceInfo_t);

    // Note: Memory allocated on loader heap is zero filled
    // memset(pMemory, 0, sizeof(EEClass) + cbMT);

    EEClass* pClass;
    MethodTableBuilder::CreateMinimalClass(pCreationHeap,pContainingModule,pamTracker,cbMT,&pClass);
    pMT = (MethodTable*)(((BYTE*)pClass)+ sizeof(EEClass));
    LOG((LF_BCL, LL_INFO100, "Level2 - Creating MethodTable {0x%p}...\n", pClass));
    
    // Allocate the private data block ("private" during runtime in the ngen'ed case).
    BYTE* pMTWriteableData = (BYTE *)
        pamTracker->Track(pCreationHeap->AllocMem(sizeof(MethodTableWriteableData)));
    pMT->SetWriteableData((PTR_MethodTableWriteableData)pMTWriteableData);

    //
    // Set up the EEClass
    //
    pClass->SetMethodTable (pMT); // in the EEClass set the pointer to this MethodTable
    pClass->SetAttrClass(tdPublic | tdSealed);
    pClass->Setcl(mdTypeDefNil);
    pClass->SetVMFlags(0);
    pClass->SetClassConstructorSlot (MethodTable::NO_SLOT);
    pClass->SetDefaultConstructorSlot (MethodTable::NO_SLOT);

    //
    // Set up the MethodTable
    //
    pMT->SetParentMethodTable(g_pObjectClass);
    pMT->SetClass(pClass);
    pMT->SetModule(pContainingModule);
    pMT->SetLoaderModule(pContainingModule);
    pMT->SetInternalCorElementType(ELEMENT_TYPE_CLASS);
    pMT->SetBaseSize(ObjSizeOf(Object));
    pMT->SetInterfaceMap(g_pObjectClass->GetNumInterfaces(), g_pObjectClass->GetInterfaceMap());
    pMT->SetNumVirtuals(g_pObjectClass->GetNumVirtuals());
    pMT->SetNumMethods(wNumVtableSlots);

    // Copy top level MT's vtable - note, vtable is contained within the MethodTable
    memcpy(pMT->GetVtable(), g_pObjectClass->GetVtable(), wNumVtableSlots * sizeof(SLOT));

    pMT->SetClassPreInited();

#ifdef _DEBUG
    pClass->SetDebugClassName("dynamicClass");
    pMT->SetDebugClassName("dynamicClass");
#endif

    LOG((LF_BCL, LL_INFO10, "Level1 - MethodTable created {0x%p}\n", pClass));
    
    return pMT;
}

void DynamicMethodTable::MakeMethodTable(AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    m_pMethodTable = CreateMinimalMethodTable(m_Module, m_pDomain->GetHighFrequencyHeap(), pamTracker);
}

void DynamicMethodTable::Destroy()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Go over all DynamicMethodDescs and make sure that they are destroyed

    if (m_pMethodTable != NULL)
    {
        MethodDescChunk* pChunk = m_pMethodTable->GetClass()->GetChunks();
        while (pChunk != NULL)
        {
            int n = pChunk->GetCount();
            for (int i = 0; i < n; i++)
            {
                DynamicMethodDesc *pMD = (DynamicMethodDesc*)pChunk->GetMethodDescAt(i);
                pMD->Destroy(FALSE);
            }
            pChunk = pChunk->GetNextChunk();
        }
    }

    m_Crst.Destroy();
    LOG((LF_BCL, LL_INFO10, "Level1 - DynamicMethodTable destroyed {0x%p}\n", this));
}

void DynamicMethodTable::AddMethodsToList()
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACT_END;

    AllocMemTracker amt;

    LoaderHeap* pHeap = m_pDomain->GetHighFrequencyHeap();
    _ASSERTE(pHeap);

    int methodCount = MethodDescChunk::GetMaxMethodDescs(mcDynamic);

    //
    // allocate as many chunks as needed to hold the methods
    //
    MethodDescChunk* pChunk = MethodDescChunk::CreateChunk(pHeap, methodCount, mcDynamic, 0, m_pMethodTable, &amt);
    if (m_DynamicMethodList) RETURN;


    int cbAlloc = 0;
    if (!ClrSafeInt<int>::multiply(sizeof(LCGMethodResolver), methodCount, cbAlloc))
        COMPlusThrow(kNotSupportedException);

    BYTE* pResolvers = (BYTE*)amt.Track(pHeap->AllocMem(cbAlloc));
    if (m_DynamicMethodList) RETURN;

    DynamicMethodDesc *pPrevMD = NULL;
    // now go through all the methods in the chunk and link them
    for(int i = 0; i < methodCount; i++)
    {
        DynamicMethodDesc *pNewMD = (DynamicMethodDesc*)pChunk->GetMethodDescAt(i);
        _ASSERTE(pNewMD);
        _ASSERTE(pNewMD->GetClassification() == mcDynamic);

        pNewMD->SetMemberDef(0);
        pNewMD->SetSlot(0xFFFF);       // we can't ever use the slot for dynamic methods
        pNewMD->SetStatic();

        pNewMD->m_dwExtendedFlags = mdPublic | mdStatic | DynamicMethodDesc::nomdLCGMethod;

        _ASSERTE(pNewMD->ComputeMayHaveNativeCode());
        pNewMD->SetMayHaveNativeCode();

        pNewMD->SetTemporaryEntryPoint(m_pDomain, &amt);

#ifdef _DEBUG 
        pNewMD->m_pDebugMethodTable = m_pMethodTable;
        pNewMD->InitPrestubCallChecking();
#endif

        LCGMethodResolver* pResolver = new (pResolvers) LCGMethodResolver();
        pResolver->m_pDynamicMethod = pNewMD;
        pResolver->m_DynamicMethodTable = this;
        pNewMD->m_pResolver = pResolver;

        if (pPrevMD)
        {
            pPrevMD->GetLCGMethodResolver()->m_next = pNewMD;
        }
        pPrevMD = pNewMD;
        pResolvers += sizeof(LCGMethodResolver);
    }

    if (m_DynamicMethodList) RETURN;

    {
        // publish method list and method table
        LockHolder lh(this);
        if (m_DynamicMethodList) RETURN;

        // publish the new method descs on the method table
        m_pMethodTable->GetClass()->AddChunk(pChunk);
        m_DynamicMethodList = (DynamicMethodDesc*)pChunk->GetFirstMethodDesc();
    }

    amt.SuppressRelease();
}

DynamicMethodDesc* DynamicMethodTable::GetDynamicMethod(BYTE *psig, DWORD sigSize, PTR_CUTF8 name)
{
    CONTRACT (DynamicMethodDesc*)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(psig));
        PRECONDITION(sigSize > 0);
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    LOG((LF_BCL, LL_INFO10000, "Level4 - Getting DynamicMethod\n"));

    DynamicMethodDesc *pNewMD = NULL;

    for (;;)
    {
        {
            LockHolder lh(this);
            pNewMD = m_DynamicMethodList;
            if (pNewMD)
            {
                m_DynamicMethodList = pNewMD->GetLCGMethodResolver()->m_next;
#ifdef _DEBUG 
                m_Used++;
#endif
                break;
            }
        }

        LOG((LF_BCL, LL_INFO1000, "Level4 - DynamicMethod unavailable\n"));

        // need to create more methoddescs
        AddMethodsToList();
    }
    _ASSERTE(pNewMD != NULL);

    // Reset the method desc into pristine state

    // Note: Reset has THROWS contract since it may allocate jump stub. It will never throw here 
    // since it will always reuse the existing jump stub.
    pNewMD->Reset();

    LOG((LF_BCL, LL_INFO1000, "Level3 - DynamicMethod obtained {0x%p} (used %d)\n", pNewMD, m_Used));

    // the store sig part of the method desc
    pNewMD->SetStoredMethodSig((PCCOR_SIGNATURE)psig, sigSize);
    // the dynamic part of the method desc
    pNewMD->m_pszMethodName = name;

    pNewMD->m_dwExtendedFlags = mdPublic | mdStatic | DynamicMethodDesc::nomdLCGMethod;

#ifdef _DEBUG 
    pNewMD->m_pszDebugMethodName = name;
    pNewMD->m_pszDebugClassName  = (LPUTF8)"dynamicclass";
    pNewMD->m_pszDebugMethodSignature = "DynamicMethod Signature not available";
#endif // _DEBUG
    pNewMD->SetNotInline(TRUE);
    pNewMD->GetLCGMethodResolver()->m_next  = NULL;

    RETURN pNewMD;
}

void DynamicMethodTable::LinkMethod(DynamicMethodDesc *pMethod)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMethod));
    }
    CONTRACT_END;

    LOG((LF_BCL, LL_INFO10000, "Level4 - Returning DynamicMethod to free list {0x%p} (used %d)\n", pMethod, m_Used));
    {
        LockHolder lh(this);
        pMethod->GetLCGMethodResolver()->m_next = m_DynamicMethodList;
        m_DynamicMethodList = pMethod;
#ifdef _DEBUG 
        m_Used--;
#endif
    }

    RETURN;
}

#endif // !DACCESS_COMPILE


//
// CodeHeap implementation
//
HeapList* HostCodeHeap::CreateCodeHeap(CodeHeapRequestInfo *pInfo, EEJitManager *pJitManager)
{
    CONTRACT (HeapList*)
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    size_t MaxCodeHeapSize  = pInfo->getRequestSize();
    size_t CacheSize        = pInfo->getCacheSize();
    size_t ReserveBlockSize = MaxCodeHeapSize + sizeof(HeapList) + CacheSize;

    ReserveBlockSize += sizeof(TrackAllocation) + PAGE_SIZE; // make sure we have enough for the allocation
    // take a conservative size for the nibble map, we may change that later if appropriate
    size_t nibbleMapSize = ROUND_UP_TO_PAGE(HEAP2MAPSIZE(ROUND_UP_TO_PAGE(ALIGN_UP(ReserveBlockSize, VIRTUAL_ALLOC_RESERVE_GRANULARITY))));
    size_t heapListSize = (sizeof(HeapList) + CODE_SIZE_ALIGN - 1) & (~(CODE_SIZE_ALIGN - 1));
    size_t otherData = heapListSize;
    // in a conservative fashion allocate room for one extra cache entry so that if we end up spilling on another block we have the space
    otherData += CacheSize;
    // make conservative estimate of the memory needed for otherData
    size_t reservedData = otherData + sizeof(TrackAllocation);
    reservedData = (otherData + CODEHEAP_SIZE_ALIGN - 1) & (~(CODEHEAP_SIZE_ALIGN - 1));
    
    NewHolder<HostCodeHeap> pCodeHeap(new HostCodeHeap(ReserveBlockSize + nibbleMapSize + reservedData, pJitManager, pInfo));
    LOG((LF_BCL, LL_INFO10, "Level2 - CodeHeap creation {0x%p} - requested 0x%p, size available 0x%p, private data 0x%p, nibble map 0x%p\n", 
                            (HostCodeHeap*)pCodeHeap, ReserveBlockSize, pCodeHeap->m_TotalBytesAvailable, reservedData, nibbleMapSize));

    BYTE *pBuffer = pCodeHeap->InitCodeHeapPrivateData(ReserveBlockSize, reservedData, nibbleMapSize);
    _ASSERTE(((size_t)pBuffer & PAGE_MASK) == 0);
    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap creation {0x%p} - base addr 0x%p, size available 0x%p, nibble map ptr 0x%p\n",
                            (HostCodeHeap*)pCodeHeap, pCodeHeap->m_pBaseAddr, pCodeHeap->m_TotalBytesAvailable, pBuffer));

    void* pHdrMap = pBuffer;

    HeapList *pHp = (HeapList*)pCodeHeap->AllocMemory(otherData, 0);
    pHp->pHeap = (PTR_CodeHeap)pCodeHeap;
    // wire it back
    pCodeHeap->m_pHeapList = (PTR_HeapList)pHp;
    // assign beginning of nibble map
    pHp->pHdrMap = (PTR_DWORD)(DWORD*)pHdrMap;

    TrackAllocation *pTracker = *((TrackAllocation**)pHp - 1);
    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap creation {0x%p} - size available 0x%p, private data ptr [0x%p, 0x%p]\n",
                            (HostCodeHeap*)pCodeHeap, pCodeHeap->m_TotalBytesAvailable, pTracker, pTracker->size));

    // need to update the reserved data
    pCodeHeap->m_ReservedData += pTracker->size;

    pHp->pCacheSpacePtr = (BYTE*)pHp + heapListSize;
    pHp->bCacheSpaceSize = (pHp->pCacheSpacePtr == NULL) ? 0 : CacheSize;
    pCodeHeap->m_ReservedData -= pHp->bCacheSpaceSize;
    pHp->mapBase         = ROUND_DOWN_TO_PAGE((void*)(pCodeHeap->m_pBaseAddr + pTracker->size));  // round down to next lower page align

    pHp->startAddress    = (TADDR)(void*)(pCodeHeap->m_pBaseAddr + pTracker->size);
    pHp->endAddress      = pHp->startAddress;

    pHp->maxCodeHeapSize = pCodeHeap->m_TotalBytesAvailable - pTracker->size;
    _ASSERTE(pHp->maxCodeHeapSize >= MaxCodeHeapSize);

    // We do not need to memset this memory, since ClrVirtualAlloc() guarantees that the memory is zero.
    // Furthermore, if we avoid writing to it, these pages don't come into our working set

    pHp->changeStart     = NULL;
    pHp->changeEnd       = NULL;
    pHp->cBlocks         = 0;
#ifndef DACCESS_COMPILE 
#endif // !DACCESS_COMPILE

    // zero the ref count as now starts the real counter
    pCodeHeap->m_AllocationCount = 0;

    pCodeHeap.SuppressRelease();

    LOG((LF_BCL, LL_INFO10, "Level1 - CodeHeap created {0x%p}\n", (HostCodeHeap*)pCodeHeap));
    RETURN pHp;
}

HostCodeHeap::HostCodeHeap(size_t ReserveBlockSize, EEJitManager *pJitManager, CodeHeapRequestInfo *pInfo)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    // reserve ReserveBlockSize rounded-up to VIRTUAL_ALLOC_RESERVE_GRANULARITY of memory
    ReserveBlockSize = ALIGN_UP(ReserveBlockSize, VIRTUAL_ALLOC_RESERVE_GRANULARITY);

    if (pInfo->m_loAddr != NULL || pInfo->m_hiAddr != NULL)
    {
        const BYTE *pStart = NULL;

        HRESULT hr = ClrVirtualAllocWithinRange(pStart, pInfo->m_loAddr, pInfo->m_hiAddr,
                                                ReserveBlockSize, MEM_RESERVE, PAGE_NOACCESS);

        if (FAILED(hr))
            ThrowHR(hr);

        m_pBaseAddr = (PTR_BYTE)(BYTE*)pStart;
    }
    else
    {
        m_pBaseAddr = (PTR_BYTE)(BYTE*)ClrVirtualAlloc(NULL, ReserveBlockSize, MEM_RESERVE, PAGE_NOACCESS);
        if (!m_pBaseAddr)
            ThrowOutOfMemory();
    }

    m_pLastAvailableCommittedAddr = m_pBaseAddr;
    m_TotalBytesAvailable = ReserveBlockSize;
    m_AllocationCount = 0;
    m_ReservedData = 0;
    m_pJitManager = (PTR_EEJitManager)pJitManager;
    m_pFreeList = NULL;
    m_pDomain = pInfo->m_pDomain;
    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap creation {0x%p, vt(0x%x)} - base addr 0x%p, total size 0x%p\n", 
                            this, *(size_t*)this, m_pBaseAddr, m_TotalBytesAvailable));
}

HostCodeHeap::~HostCodeHeap()
{
#ifndef DACCESS_COMPILE 
    WRAPPER_CONTRACT;
    {
        CrstHolder ch(&HostCodeHeap::s_CleanupCrst);
        HostCodeHeap *pHeap = HostCodeHeap::s_cleanupList;
        HostCodeHeap *pPrevHeap = NULL;
        while (pHeap)
        {
            if (pHeap == this)
            {
                if (pPrevHeap)
                {
                    // remove current heap from list
                    pPrevHeap->m_pNextHeapToRelease = pHeap->m_pNextHeapToRelease;
                }
                else
                {
                    HostCodeHeap::s_cleanupList = pHeap->m_pNextHeapToRelease;
                }
                break;
            }
            pPrevHeap = pHeap;
            pHeap = pHeap->m_pNextHeapToRelease;
        }
    }
    if (m_pBaseAddr)
        ClrVirtualFree(m_pBaseAddr, 0, MEM_RELEASE);
    LOG((LF_BCL, LL_INFO10, "Level1 - CodeHeap destroyed {0x%p}\n", this));
#endif // !DACCESS_COMPILE
}

BYTE* HostCodeHeap::InitCodeHeapPrivateData(size_t ReserveBlockSize, size_t otherData, size_t nibbleMapSize)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    size_t nibbleNewSize = ROUND_UP_TO_PAGE(HEAP2MAPSIZE(ROUND_UP_TO_PAGE(m_TotalBytesAvailable)));
    if (m_TotalBytesAvailable - nibbleNewSize < ReserveBlockSize + otherData)
    {
        // the new allocation for the nibble map would notleave enough room for the requested memory, bail out
        nibbleNewSize = nibbleMapSize;
    }

    BYTE *pAddress = (BYTE*)ROUND_DOWN_TO_PAGE(PTR_TO_TADDR(m_pLastAvailableCommittedAddr) + 
                                               m_TotalBytesAvailable - nibbleNewSize);
    _ASSERTE(m_pLastAvailableCommittedAddr + m_TotalBytesAvailable >= pAddress + nibbleNewSize);
    if (NULL == ClrVirtualAlloc(pAddress, nibbleNewSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE))
        ThrowOutOfMemory();
    m_TotalBytesAvailable = pAddress - m_pLastAvailableCommittedAddr;
    _ASSERTE(m_TotalBytesAvailable >= ReserveBlockSize + otherData);
    return pAddress;
}

 // used to flag a block that is too small
#define UNUSABLE_BLOCK      ((size_t)-1)
 
size_t HostCodeHeap::GetPadding(TrackAllocation *pCurrent, size_t size, DWORD alignment)
{
    LEAF_CONTRACT;
    if (pCurrent->size < size)
        return UNUSABLE_BLOCK;
    size_t pointer = (size_t)((BYTE*)pCurrent + sizeof(TrackAllocation));
    size_t padding = 0;
    if (alignment)
        padding = ((pointer + (size_t)alignment - 1) & (~((size_t)alignment - 1))) - pointer;
    if (pCurrent->size < size + padding)
        return UNUSABLE_BLOCK;
    return padding;
}

void* HostCodeHeap::AllocFromFreeList(size_t size, DWORD alignment)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (m_pFreeList)
    {
        LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Alloc size corrected 0x%X for free list\n", this, size));
        // walk the list looking for a block with enough capacity
        TrackAllocation *pCurrent = m_pFreeList;
        TrackAllocation *pPrevious = NULL;
        while (pCurrent)
        {
            size_t padding = GetPadding(pCurrent, size, alignment);
            if (UNUSABLE_BLOCK != padding)
            {
                // found a block
                LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Block found, size 0x%X\n", this, pCurrent->size));
                size_t realSize = size + padding;
                BYTE *pPointer = (BYTE*)pCurrent + sizeof(TrackAllocation) + padding;
                _ASSERTE((size_t)(pPointer - (BYTE*)pCurrent) >= sizeof(TrackAllocation));

                // update current
                if (pCurrent->size - realSize == 0)
                {
                    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Item removed %p, size 0x%X\n", this, pCurrent, pCurrent->size));
                    // remove current
                    if (pPrevious)
                    {
                        pPrevious->pNext = pCurrent->pNext;
                    }
                    else
                    {
                        m_pFreeList = pCurrent->pNext;
                    }
                }
                else
                {
                    // create new current item
                    TrackAllocation *pNewCurrent = (TrackAllocation*)((BYTE*)pCurrent + realSize);
                    pNewCurrent->pNext = pCurrent->pNext;
                    pNewCurrent->size = pCurrent->size - realSize;
                    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Item changed %p, new size 0x%X\n", this, pNewCurrent, pNewCurrent->size));
                    if (pPrevious)
                    {
                        pPrevious->pNext = pNewCurrent;
                    }
                    else
                    {
                        m_pFreeList = pNewCurrent;
                    }
                }

                // now fill all the padding data correctly
                TrackAllocation *pTracker = (TrackAllocation*)pCurrent;
                pTracker->pHeap = this;
                pTracker->size = realSize;
                *((void**)pPointer - 1) = pTracker;

                LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Allocation returned %p, size 0x%X - data -> %p\n", this, pTracker, pTracker->size, pPointer));
                return pPointer;
            }
            pPrevious = pCurrent;
            pCurrent = pCurrent->pNext;
        }
    }
    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - No block in free list for size 0x%X\n", this, size));
    return NULL;
}

void HostCodeHeap::AddToFreeList(TrackAllocation *pBlockToInsert)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Add to FreeList [%p, 0x%X]\n", this, pBlockToInsert, pBlockToInsert->size));

    // append to the list in the proper position and coalesce if needed
    if (m_pFreeList)
    {
        TrackAllocation *pCurrent = m_pFreeList;
        TrackAllocation *pPrevious = NULL;
        while (pCurrent)
        {
            if (pCurrent > pBlockToInsert)
            {
                // found the point of insertion
                pBlockToInsert->pNext = pCurrent;
                if (pPrevious)
                {
                    pPrevious->pNext = pBlockToInsert;
                    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Insert block [%p, 0x%X] -> [%p, 0x%X] -> [%p, 0x%X]\n", this,
                                                                        pPrevious, pPrevious->size,
                                                                        pBlockToInsert, pBlockToInsert->size,
                                                                        pCurrent, pCurrent->size));
                }
                else
                {
                    m_pFreeList = pBlockToInsert;
                    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Insert block [%p, 0x%X] to head\n", this, pBlockToInsert, pBlockToInsert->size));
                }

                // check for coalescing
                if ((BYTE*)pBlockToInsert + pBlockToInsert->size == (BYTE*)pCurrent)
                {
                    // coalesce with next
                    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Coalesce block [%p, 0x%X] with [%p, 0x%X] - new size 0x%X\n", this,
                                                                        pBlockToInsert, pBlockToInsert->size,
                                                                        pCurrent, pCurrent->size,
                                                                        pCurrent->size + pBlockToInsert->size));
                    pBlockToInsert->pNext = pCurrent->pNext;
                    pBlockToInsert->size += pCurrent->size;
                }

                if (pPrevious && (BYTE*)pPrevious + pPrevious->size == (BYTE*)pBlockToInsert)
                {
                    // coalesce with previous
                    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Coalesce block [%p, 0x%X] with [%p, 0x%X] - new size 0x%X\n", this,
                                                                        pPrevious, pPrevious->size,
                                                                        pBlockToInsert, pBlockToInsert->size,
                                                                        pPrevious->size + pBlockToInsert->size));
                    pPrevious->pNext = pBlockToInsert->pNext;
                    pPrevious->size += pBlockToInsert->size;
                }

                return;
            }
            pPrevious = pCurrent;
            pCurrent = pCurrent->pNext;
        }
        _ASSERTE(pPrevious && pCurrent == NULL);
        pBlockToInsert->pNext = NULL;
        // last in the list
        if ((BYTE*)pPrevious + pPrevious->size == (BYTE*)pBlockToInsert)
        {
            // coalesce with previous
            LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Coalesce block [%p, 0x%X] with [%p, 0x%X] - new size 0x%X\n", this,
                                                                pPrevious, pPrevious->size,
                                                                pBlockToInsert, pBlockToInsert->size,
                                                                pPrevious->size + pBlockToInsert->size));
            pPrevious->size += pBlockToInsert->size;
        }
        else
        {
            pPrevious->pNext = pBlockToInsert;
            LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Insert block [%p, 0x%X] to end after [%p, 0x%X]\n", this,
                                                                pBlockToInsert, pBlockToInsert->size,
                                                                pPrevious, pPrevious->size));
        }

        return;

    }
    // first in the list
    pBlockToInsert->pNext = m_pFreeList;
    m_pFreeList = pBlockToInsert;
    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Insert block [%p, 0x%X] to head\n", this,
                                                        m_pFreeList, m_pFreeList->size));
}

void* HostCodeHeap::AllocMemory(size_t size, DWORD alignment)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    void *pAllocation = AllocMemory_NoThrow(size, alignment);
    if (!pAllocation)
        ThrowOutOfMemory();
    return pAllocation;
}

void* HostCodeHeap::AllocMemory_NoThrow(size_t size, DWORD alignment)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

#ifdef _DEBUG 
    if (g_pConfig->ShouldInjectFault(INJECTFAULT_DYNAMICCODEHEAP))
    {
        char *a = new (nothrow) char;
        if (a == NULL)
            return NULL;
        delete a;
    }
#endif // _DEBUG

    // honor alignment (should assert the value is proper)
    if (alignment)
        size = (size + (size_t)alignment - 1) & (~((size_t)alignment - 1));
    // align size to CODEHEAP_SIZE_ALIGN always
    size = (size + CODEHEAP_SIZE_ALIGN - 1) & (~(CODEHEAP_SIZE_ALIGN - 1));

    size += sizeof(TrackAllocation);

    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - Allocation requested 0x%X\n", this, size));

    void *pAddr = AllocFromFreeList(size, alignment);
    if (!pAddr)
    {
    // walk free list to end to find available space
        size_t availableInFreeList = 0;
        TrackAllocation *pCurrentBlock = m_pFreeList;
        TrackAllocation *pLastBlock = NULL;
    while (pCurrentBlock)
    {
        pLastBlock = pCurrentBlock;
        pCurrentBlock = pCurrentBlock->pNext;
    }
    if (pLastBlock && (BYTE*)pLastBlock + pLastBlock->size == m_pLastAvailableCommittedAddr)
    {
        availableInFreeList = pLastBlock->size;
    }
        _ASSERTE(size > availableInFreeList);
        size_t sizeToCommit = size - availableInFreeList; 
        sizeToCommit = (size + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)); // round up to page

    if (m_pLastAvailableCommittedAddr + sizeToCommit <= m_pBaseAddr + m_TotalBytesAvailable)
    {
        if (NULL == ClrVirtualAlloc(m_pLastAvailableCommittedAddr, sizeToCommit, MEM_COMMIT, PAGE_EXECUTE_READWRITE))
            {
                LOG((LF_BCL, LL_ERROR, "CodeHeap [0x%p] - VirtualAlloc failed\n", this));
            return NULL;
            }

            TrackAllocation *pBlockToInsert = (TrackAllocation*)(void*)m_pLastAvailableCommittedAddr;
            pBlockToInsert->pNext = NULL;
        pBlockToInsert->size = sizeToCommit;
        m_pLastAvailableCommittedAddr += sizeToCommit;
        AddToFreeList(pBlockToInsert);
            pAddr = AllocFromFreeList(size, alignment);
    }
    else
    {
            LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - allocation failed:\n\tm_pLastAvailableCommittedAddr: 0x%X\n\tsizeToCommit: 0x%X\n\tm_pBaseAddr: 0x%X\n\tm_TotalBytesAvailable: 0x%X\n", this, m_pLastAvailableCommittedAddr, sizeToCommit, m_pBaseAddr, m_TotalBytesAvailable));
        return NULL;
    }
    }

    _ASSERTE(pAddr);
    // ref count the whole heap
    m_AllocationCount++;
    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p] - ref count %d\n", this, m_AllocationCount));
    return pAddr;
}

void* HostCodeHeap::GetHeapStartAddress()
{
    LEAF_CONTRACT;
    return m_pBaseAddr;
}

#ifdef _DEBUG 
size_t HostCodeHeap::GetReservedPrivateData()
{
    LEAF_CONTRACT;
    return m_ReservedData;
}
#endif // _DEBUG

#ifdef DACCESS_COMPILE 
void HostCodeHeap::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    DAC_ENUM_DTHIS();

    TADDR addr = PTR_TO_TADDR(m_pBaseAddr);
    size_t size = PTR_TO_TADDR(m_pLastAvailableCommittedAddr) - addr;

    while (size)
    {
        ULONG32 enumSize;

        if (size > 0x80000000)
        {
            enumSize = 0x80000000;
        }
        else
        {
            enumSize = (ULONG32)size;
        }

        DacEnumMemoryRegion(addr, enumSize);

        addr += enumSize;
        size -= enumSize;
    }
}
#endif // DACCESS_COMPILE

// static
struct HostCodeHeap::TrackAllocation * HostCodeHeap::GetTrackAllocation(CodeHeader* pCodeHdr)
{
    LEAF_CONTRACT;
    
    BYTE* allocationStart = (BYTE*) pCodeHdr;

    // if we're using indirect codeheaders then we know in the LCG case
    // there is a RealCodeHeader immediately preceeding the CodeHeader
    // and there might be some alignment fudge preceding that so that our 
    // code achieved the specified CODE_SIZE_ALIGN
    //
    // SEE: codeman.cpp, allocCode()
#ifdef USE_INDIRECT_CODEHEADER
    size_t alignmentFudge = (sizeof(RealCodeHeader) + sizeof(CodeHeader)) % CODE_SIZE_ALIGN;
    allocationStart -= sizeof(RealCodeHeader);
#else
    size_t alignmentFudge = sizeof(CodeHeader) % CODE_SIZE_ALIGN;
#endif  // USE_INDIRECT_CODEHEADER

    // there might be some alignmentFudge that was used to achieve
    // CODE_SIZE_ALIGN'd code, if the CodeHeader is sized as a 
    // multiple of CODE_SIZE_ALIGN this will be 0 and the compiled
    // will get rid of it.
    allocationStart -= alignmentFudge;

    TrackAllocation ** ppTracker = (TrackAllocation **)(allocationStart - sizeof(TrackAllocation*));
    return *ppTracker;
}

HostCodeHeap* HostCodeHeap::GetCodeHeap(CodeHeader * pCodeHdr)
{
    WRAPPER_CONTRACT;
    return HostCodeHeap::GetTrackAllocation(pCodeHdr)->pHeap;
}

void HostCodeHeap::ReleaseReferenceToHeap(MethodDesc* pMD)
{
#ifndef DACCESS_COMPILE 
    WRAPPER_CONTRACT;

    _ASSERTE(pMD->IsDynamicMethod());
    DynamicMethodDesc *pDynamicMethod = (DynamicMethodDesc*)pMD;
    LCGMethodResolver *pPrivateData = pDynamicMethod->GetLCGMethodResolver();

    //
    // clean up the NibbleMap
    //

    // Scope the CantStopHolder
    {
        // Prevents a thread suspension AND prevents a hijacking profiler from doing
        // a stackwalk.
        ForbidSuspendThreadHolder suspend;

        // Currently all callers to this method ensure EEJitManager::m_CodeHeapCritSec
        // is held.  This crst has the CRST_DEBUGGER_THREAD flag, which prevents
        // debuggers from halting this thread after if has incremented changeStart, but
        // before it has incremented changeEnd; otherwise it would loop forever when it
        // does a stack walk and tries to access the nibble map.
        //
        // However, it is not easy in this function to verify or enforce that the crst
        // was indeed taken before this has been called.  So we're forcing the
        // cantstop counter to increment with this holder, which will also prevent
        // the debugger from causing infinite loops.
        CantStopHolder hCantStop;

        m_pHeapList->changeStart++;
        size_t delta = (size_t)((BYTE*)(pPrivateData->m_recordCodePointer) - m_pHeapList->mapBase);
        m_pJitManager->NibbleMapSet(m_pHeapList->pHdrMap, delta, FALSE);
        m_pHeapList->cBlocks--;
        m_pHeapList->changeEnd++;
    }

    CodeHeader* pCodeHdr = ((CodeHeader*)(pPrivateData->m_recordCodePointer)) - 1;
    TrackAllocation *pTracker = HostCodeHeap::GetTrackAllocation(pCodeHdr);
    AddToFreeList(pTracker);

    m_AllocationCount--;
    LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap released [0x%p, vt(0x%x)] - ref count %d\n", this, *(size_t*)this, m_AllocationCount));
    if (m_AllocationCount == 0)
    {
        // it may happen that the current heap count goes to 0 and later on, before it is destroyed, it gets reused 
        // for another dynamic method. 
        // It's then possible that the ref count reaches 0 multiple times. If so we simply don't add it again
        // Also on cleanup we check the the ref count is actually 0.
        CrstHolder ch(&HostCodeHeap::s_CleanupCrst);
        HostCodeHeap *pHeap = HostCodeHeap::s_cleanupList;
        while (pHeap)
        {
            if (pHeap == this)
            {
                LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p, vt(0x%x)] - Already in list\n", this, *(size_t*)this));
                break;
            }
            pHeap = pHeap->m_pNextHeapToRelease;
        }
        if (pHeap == NULL)
        {
            m_pNextHeapToRelease = HostCodeHeap::s_cleanupList;
            HostCodeHeap::s_cleanupList = this;
            LOG((LF_BCL, LL_INFO100, "Level2 - CodeHeap [0x%p, vt(0x%x)] - ref count %d - Adding to cleanup list\n", this, *(size_t*)this, m_AllocationCount));
        }
    }
#endif // !DACCESS_COMPILE
}

// static init
HostCodeHeap *HostCodeHeap::s_cleanupList = NULL;
CrstStatic HostCodeHeap::s_CleanupCrst;

void HostCodeHeap::Init()
{
#ifndef DACCESS_COMPILE
    WRAPPER_CONTRACT;
    HostCodeHeap::s_CleanupCrst.Init("Code Heap free list", CrstCodeHeapList, CRST_UNSAFE_COOPGC);
#endif
}

void HostCodeHeap::CleanupCodeHeaps()
{
#ifndef DACCESS_COMPILE
    WRAPPER_CONTRACT;
    _ASSERTE (g_fProcessDetach || (GCHeap::IsGCInProgress() && ::IsGCThread()));
    if (HostCodeHeap::s_cleanupList == NULL)
        return;
    HostCodeHeap *pHeap = NULL;
    {
        CrstHolder ch(&HostCodeHeap::s_CleanupCrst);
        pHeap = HostCodeHeap::s_cleanupList;
        HostCodeHeap::s_cleanupList = NULL;
    }
    while (pHeap)
    {
        HostCodeHeap *pNextHeap = pHeap->m_pNextHeapToRelease;
        if (pHeap->m_AllocationCount == 0)
        {
            LOG((LF_BCL, LL_INFO100, "Level2 - Destryoing CodeHeap [0x%p, vt(0x%x)] - ref count %d\n", pHeap, *(size_t*)pHeap, pHeap->m_AllocationCount));
            ExecutionManager::ReleaseHeap(pHeap->m_pJitManager, pHeap);
        }
        else
        {
            LOG((LF_BCL, LL_INFO100, "Level2 - Restoring CodeHeap [0x%p, vt(0x%x)] - ref count %d\n", pHeap, *(size_t*)pHeap, pHeap->m_AllocationCount));
        }
        pHeap = pNextHeap;
    }
#endif
}

void HostCodeHeap::DestroyHeap()
{
#ifndef DACCESS_COMPILE
    WRAPPER_CONTRACT;
    _ASSERTE (g_fProcessDetach || (GCHeap::IsGCInProgress()  && ::IsGCThread()));
    m_pJitManager->RemoveCodeHeapFromDomainList(this, m_pDomain);
    m_pJitManager->DeleteCodeHeap(m_pHeapList);
#endif
}


#ifndef DACCESS_COMPILE

//
// Implementation for DynamicMethodDesc declared in method.hpp
//
void DynamicMethodDesc::Destroy(BOOL fRecycle)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE(IsDynamicMethod());
    LOG((LF_BCL, LL_INFO1000, "Level3 - Destroying DynamicMethod {0x%p}\n", this));
    if (m_pSig)
    {
        delete[] (BYTE*)m_pSig;
        m_pSig = NULL;
    }
    m_cSig = 0;
    if (m_pszMethodName)
    {
        delete[] m_pszMethodName;
        m_pszMethodName = NULL;
    }

    GetLCGMethodResolver()->Destroy(fRecycle);
}

//
// LCGMethodResolver
//
//  a jit resolver for managed dynamic methods
//

void LCGMethodResolver::Destroy(BOOL fRecycle)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    LOG((LF_BCL, LL_INFO100, "Level2 - Resolver - Destroying Resolver {0x%p}\n", this));
    if (m_Code)
    {
        delete[] m_Code;
        m_Code = NULL;
    }
    m_CodeSize = 0;
    if (m_LocalSig)
    {
        delete[] m_LocalSig;
        m_LocalSig = NULL;
    }
    m_LocalSigSize = 0;

    m_jitMetaHeap.Delete();
    m_jitTempData.Delete();

    // Resources reclaimed automatically during appdomain shutdown need to be released only
    // if we are recycling.
    if (fRecycle)
    {
        if (m_managedResolver)
        {
            ::DestroyLongWeakHandle(m_managedResolver);
            m_managedResolver = NULL;
        }
        if (m_recordCodePointer)
        {
            HostCodeHeap *pHeap = HostCodeHeap::GetCodeHeap((CodeHeader *)m_recordCodePointer-1);
            LOG((LF_BCL, LL_INFO1000, "Level3 - Resolver {0x%p} - Release reference to heap {%p, vt(0x%x)} \n", this, pHeap, *(size_t*)pHeap));
            ExecutionManager::ReleaseReferenceToHeap(pHeap->m_pJitManager, pHeap, m_pDynamicMethod);
            m_recordCodePointer = NULL;
        }
        m_DynamicMethodTable->LinkMethod(m_pDynamicMethod);
    }
}

void LCGMethodResolver::FreeCompileTimeState()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    //m_jitTempData.Delete();
}

#endif // !DACCESS_COMPILE


void LCGMethodResolver::GetJitContext(DWORD *securityControlFlags, TypeHandle *typeOwner)
{
#ifndef DACCESS_COMPILE 
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;
    _ASSERTE(securityControlFlags && typeOwner);

    MethodDescCallSite getJitContext(METHOD__RESOLVER__GET_JIT_CONTEXT, m_managedResolver);

    OBJECTREF resolver = ObjectFromHandle(m_managedResolver);
    _ASSERTE(resolver); // gc root must be up the stack

    ARG_SLOT args[] =
    {
        ObjToArgSlot(resolver),
        PtrToArgSlot(securityControlFlags),
        PtrToArgSlot(typeOwner)
    };
    getJitContext.Call(args);

#else // DACCESS_COMPILE
    DacNotImpl();
#endif // DACCESS_COMPILE
}

ChunkAllocator* LCGMethodResolver::GetJitMetaHeap()
{
    LEAF_CONTRACT;
    return &m_jitMetaHeap;
}

BYTE* LCGMethodResolver::GetCodeInfo(unsigned *pCodeSize, unsigned short *pStackSize, CorInfoOptions *pOptions, unsigned short *pEHSize)
{
#ifndef DACCESS_COMPILE 
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    _ASSERTE(pCodeSize);

    if (!m_Code)
    {
        LOG((LF_BCL, LL_INFO100000, "Level5 - DM-JIT: Getting CodeInfo on resolver 0x%p...\n", this));
        // get the code - Byte[] Resolver.GetCodeInfo(ref ushort stackSize, ref int EHCount)
        MethodDescCallSite getCodeInfo(METHOD__RESOLVER__GET_CODE_INFO, m_managedResolver);

        OBJECTREF resolver = ObjectFromHandle(m_managedResolver);
        VALIDATEOBJECTREF(resolver); // gc root must be up the stack

        DWORD stackSize = 0, initLocals = 0, EHSize = 0;
        ARG_SLOT args[] =
        {
            ObjToArgSlot(resolver),
            PtrToArgSlot(&stackSize),
            PtrToArgSlot(&initLocals),
            PtrToArgSlot(&EHSize),
        };
        U1ARRAYREF dataArray = (U1ARRAYREF) getCodeInfo.Call_RetOBJECTREF(args);
        DWORD codeSize = dataArray->GetNumComponents();
        NewHolder<BYTE> code(new BYTE[codeSize]);
        memcpy(code, dataArray->GetDataPtr(), codeSize);
        m_CodeSize = codeSize;
        m_StackSize = stackSize;
        m_Options = (initLocals) ? CORINFO_OPT_INIT_LOCALS : (CorInfoOptions)0;
        m_EHSize = EHSize;
        m_Code = (BYTE*)code;
        code.SuppressRelease();
        LOG((LF_BCL, LL_INFO100000, "Level5 - DM-JIT: CodeInfo {0x%p} on resolver %p\n", m_Code, this));
    }

    *pCodeSize = m_CodeSize;
    if (pStackSize)
        *pStackSize = m_StackSize;
    if (pOptions)
        *pOptions = m_Options;
    if (pEHSize)
        *pEHSize = m_EHSize;
    return m_Code;

#else // DACCESS_COMPILE
    DacNotImpl();
    return NULL;
#endif // DACCESS_COMPILE
}

PCCOR_SIGNATURE LCGMethodResolver::GetLocalSig(DWORD *pSigSize)
{
#ifndef DACCESS_COMPILE 
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    if (!m_LocalSig)
    {
        LOG((LF_BCL, LL_INFO100000, "Level5 - DM-JIT: Getting LocalSig on resolver 0x%p...\n", this));

        MethodDescCallSite getLocalsSignature(METHOD__RESOLVER__GET_LOCALS_SIGNATURE, m_managedResolver);

        OBJECTREF resolver = ObjectFromHandle(m_managedResolver);
        VALIDATEOBJECTREF(resolver); // gc root must be up the stack

        ARG_SLOT args[] =
        {
            ObjToArgSlot(resolver)
        };
        U1ARRAYREF dataArray = (U1ARRAYREF) getLocalsSignature.Call_RetOBJECTREF(args);
        DWORD localSigSize = dataArray->GetNumComponents();
        NewHolder<COR_SIGNATURE> localSig(new COR_SIGNATURE[localSigSize]);
        memcpy((void*)localSig, dataArray->GetDataPtr(), localSigSize);
        m_LocalSigSize = localSigSize;
        m_LocalSig = (PCCOR_SIGNATURE)localSig;
        localSig.SuppressRelease();
        LOG((LF_BCL, LL_INFO100000, "Level5 - DM-JIT: LocalSig {0x%p} on resolver %p\n", m_LocalSig, this));
    }

    if (pSigSize)
        *pSigSize = m_LocalSigSize;
    return m_LocalSig;
#else // DACCESS_COMPILE
    DacNotImpl();
    return NULL;
#endif // DACCESS_COMPILE
}

StringObject* LCGMethodResolver::GetStringLiteral(mdToken token)
{
#ifndef DACCESS_COMPILE 
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    STRINGREF retStr = NULL;

    MethodDescCallSite getStringLiteral(METHOD__RESOLVER__GET_STRING_LITERAL, m_managedResolver);

    OBJECTREF resolver = ObjectFromHandle(m_managedResolver);
    VALIDATEOBJECTREF(resolver); // gc root must be up the stack

    ARG_SLOT args[] = {
        ObjToArgSlot(resolver),
        token,
    };
    retStr = getStringLiteral.Call_RetSTRINGREF(args);
    return STRINGREFToObject(retStr);
#else // DACCESS_COMPILE
    DacNotImpl();
    return NULL;
#endif // DACCESS_COMPILE
}

void* LCGMethodResolver::ResolveToken(mdToken token)
{
#ifndef DACCESS_COMPILE 
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    MethodDescCallSite resolveToken(METHOD__RESOLVER__RESOLVE_TOKEN, m_managedResolver);

    OBJECTREF resolver = ObjectFromHandle(m_managedResolver);
    VALIDATEOBJECTREF(resolver); // gc root must be up the stack

    ARG_SLOT args[] = {
        ObjToArgSlot(resolver),
        token,
    };
    void *pHandle = resolveToken.Call_RetLPVOID(args);
    return pHandle;
#else // DACCESS_COMPILE
    DacNotImpl();
    return NULL;
#endif // DACCESS_COMPILE
}

unsigned LCGMethodResolver::ResolveParentToken(mdToken token)
{
#ifndef DACCESS_COMPILE 
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    unsigned tk = (unsigned)-1;

    MethodDescCallSite parentToken(METHOD__RESOLVER__PARENT_TOKEN, m_managedResolver);

    OBJECTREF resolver = ObjectFromHandle(m_managedResolver);
    VALIDATEOBJECTREF(resolver); // gc root must be up the stack

    ARG_SLOT args[] = {
        ObjToArgSlot(resolver),
        token,
    };
    tk = parentToken.Call_RetI4(args);
    _ASSERTE(tk != (unsigned)-1);
    return tk;
#else // DACCESS_COMPILE
    DacNotImpl();
    return 0;
#endif // DACCESS_COMPILE
}

PCCOR_SIGNATURE LCGMethodResolver::ResolveSignature(mdToken token)
{
#ifndef DACCESS_COMPILE 
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    U1ARRAYREF dataArray = NULL;

    MethodDescCallSite resolveSignature(METHOD__RESOLVER__RESOLVE_SIGNATURE, m_managedResolver);

    OBJECTREF resolver = ObjectFromHandle(m_managedResolver);
    VALIDATEOBJECTREF(resolver); // gc root must be up the stack

    ARG_SLOT args[] = {
        ObjToArgSlot(resolver),
        token,
        0
    };
    dataArray = (U1ARRAYREF) resolveSignature.Call_RetOBJECTREF(args);

    DWORD SigSize = dataArray->GetNumComponents();
    PCCOR_SIGNATURE pSig = (PCCOR_SIGNATURE)m_jitTempData.New(SigSize);
    memcpy((void*)pSig, dataArray->GetDataPtr(), SigSize);
    return pSig;
#else // DACCESS_COMPILE
    DacNotImpl();
    return NULL;
#endif // DACCESS_COMPILE
}

PCCOR_SIGNATURE LCGMethodResolver::ResolveSignatureForVarArg(mdToken token)
{
#ifndef DACCESS_COMPILE 
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    U1ARRAYREF dataArray = NULL;

    MethodDescCallSite resolveSignature(METHOD__RESOLVER__RESOLVE_SIGNATURE, m_managedResolver);

    OBJECTREF resolver = ObjectFromHandle(m_managedResolver);
    VALIDATEOBJECTREF(resolver); // gc root must be up the stack

    ARG_SLOT args[] = {
        ObjToArgSlot(resolver),
        token,
        1 // vararg case
    };
    dataArray = (U1ARRAYREF) resolveSignature.Call_RetOBJECTREF(args);

    DWORD SigSize = dataArray->GetNumComponents();
    PCCOR_SIGNATURE pSig = (PCCOR_SIGNATURE)m_jitTempData.New(SigSize);
    memcpy((void*)pSig, dataArray->GetDataPtr(), SigSize);
    return pSig;
#else // DACCESS_COMPILE
    DacNotImpl();
    return NULL;
#endif // DACCESS_COMPILE
}

void LCGMethodResolver::GetEHInfo(unsigned EHnumber, CORINFO_EH_CLAUSE* clause)
{
#ifndef DACCESS_COMPILE 
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    // attempt to get the raw EHInfo first
    MethodDescCallSite getRawEHInfo(METHOD__RESOLVER__GET_RAW_EH_INFO, m_managedResolver);

    OBJECTREF resolver = ObjectFromHandle(m_managedResolver);
    VALIDATEOBJECTREF(resolver); // gc root must be up the stack
    COR_ILMETHOD_SECT_EH* pEH = NULL;

    ARG_SLOT args[] = {
        ObjToArgSlot(resolver),
    };
    U1ARRAYREF dataArray = (U1ARRAYREF) getRawEHInfo.Call_RetOBJECTREF(args);

    if (dataArray != NULL)
    {
        pEH = (COR_ILMETHOD_SECT_EH*)dataArray->GetDataPtr();

        COR_ILMETHOD_SECT_EH_CLAUSE_FAT ehClause;
        const COR_ILMETHOD_SECT_EH_CLAUSE_FAT* ehInfo;
        ehInfo = (COR_ILMETHOD_SECT_EH_CLAUSE_FAT*)pEH->EHClause(EHnumber, &ehClause);

        clause->Flags = (CORINFO_EH_CLAUSE_FLAGS)ehInfo->GetFlags();
        clause->TryOffset = ehInfo->GetTryOffset();
        clause->TryLength = ehInfo->GetTryLength();
        clause->HandlerOffset = ehInfo->GetHandlerOffset();
        clause->HandlerLength = ehInfo->GetHandlerLength();
        clause->ClassToken = ehInfo->GetClassToken();
        clause->FilterOffset = ehInfo->GetFilterOffset();
        return;
    }

    // failed, get the info off the ilgenerator
    MethodDescCallSite getEHInfo(METHOD__RESOLVER__GET_EH_INFO, m_managedResolver);

    resolver = ObjectFromHandle(m_managedResolver);
    VALIDATEOBJECTREF(resolver); // gc root must be up the stack
    ARG_SLOT args1[] = {
        ObjToArgSlot(resolver),
        EHnumber,
        PtrToArgSlot(clause)
    };
    getEHInfo.Call(args1);
#else // DACCESS_COMPILE
    DacNotImpl();
#endif // DACCESS_COMPILE
}

BOOL LCGMethodResolver::IsValidToken(mdToken token)
{
#ifndef DACCESS_COMPILE 
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    MethodDescCallSite isValidToken(METHOD__RESOLVER__IS_VALID_TOKEN, m_managedResolver);

    OBJECTREF resolver = ObjectFromHandle(m_managedResolver);
    VALIDATEOBJECTREF(resolver); // gc root must be up the stack

    ARG_SLOT args[] = {
        ObjToArgSlot(resolver),
        token
    };
    return (isValidToken.Call_RetI4(args)) ? TRUE : FALSE;
#else // DACCESS_COMPILE
    DacNotImpl();
    return FALSE;
#endif // DACCESS_COMPILE

}

void* LCGMethodResolver::GetInstantiationInfo(mdToken token)
{
#ifndef DACCESS_COMPILE 
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    MethodDescCallSite getInstantiationInfo(METHOD__RESOLVER__GET_INSTANTIATION_INFO, m_managedResolver);

    OBJECTREF resolver = ObjectFromHandle(m_managedResolver);
    VALIDATEOBJECTREF(resolver); // gc root must be up the stack

    ARG_SLOT args[] = {
        ObjToArgSlot(resolver),
        token
    };
    return getInstantiationInfo.Call_RetLPVOID(args);
#else // DACCESS_COMPILE
    DacNotImpl();
    return NULL;
#endif // DACCESS_COMPILE
}

//
// ChunkAllocator implementation
//
ChunkAllocator::~ChunkAllocator()
{
    Delete();
}

void ChunkAllocator::Delete()
{
    BYTE *next = NULL;
    LOG((LF_BCL, LL_INFO10, "Level1 - DM - Allocator [0x%p] - deleting...\n", this));
    while (m_pData)
    {
        LOG((LF_BCL, LL_INFO10, "Level1 - DM - Allocator [0x%p] - delete block {0x%p}\n", this, m_pData));
        next = ((BYTE**)m_pData)[0];
        delete[] m_pData;
        m_pData = next;
    }
}

void* ChunkAllocator::New(size_t size)
{
    BYTE *pNewBlock = NULL;
    LOG((LF_BCL, LL_INFO100, "Level2 - DM - Allocator [0x%p] - allocation requested 0x%X, available 0x%X\n", this, size, (m_pData) ? ((size_t*)m_pData)[1] : 0));
    if (m_pData)
    {
        // we may have room available
        size_t available = ((size_t*)m_pData)[1];
        if (size <= available)
        {
            LOG((LF_BCL, LL_INFO100, "Level2 - DM - Allocator [0x%p] - reusing block {0x%p}\n", this, m_pData));
            ((size_t*)m_pData)[1] = available - size;
            pNewBlock = (m_pData + CHUNK_SIZE - available);
            LOG((LF_BCL, LL_INFO100, "Level2 - DM - Allocator [0x%p] - ptr -> 0x%p, available 0x%X\n", this, pNewBlock, ((size_t*)m_pData)[1]));
            return pNewBlock;
        }
    }

    // no available - need to allocate a new buffer
    if (size + (sizeof(void*) * 2) < CHUNK_SIZE)
    {
        // make the allocation
        NewHolder<BYTE> newBlock(new BYTE[CHUNK_SIZE]);
        pNewBlock = (BYTE*)newBlock;
        ((size_t*)pNewBlock)[1] = CHUNK_SIZE - size - (sizeof(void*) * 2); 
        LOG((LF_BCL, LL_INFO10, "Level1 - DM - Allocator [0x%p] - new block {0x%p}\n", this, pNewBlock));
        newBlock.SuppressRelease();
    }
    else
    {
        // request bigger than default size this is going to be a single block
        NewHolder<BYTE> newBlock(new BYTE[size + (sizeof(void*) * 2)]);
        pNewBlock = (BYTE*)newBlock;
        ((size_t*)pNewBlock)[1] = 0; // no available bytes left
        LOG((LF_BCL, LL_INFO10, "Level1 - DM - Allocator [0x%p] - new BIG block {0x%p}\n", this, pNewBlock));
        newBlock.SuppressRelease();
    }

    // all we have left to do is to link the block.
    // We leave at the top the block with more bytes available
    if (m_pData)
    {
        if (((size_t*)pNewBlock)[1] > ((size_t*)m_pData)[1])
        {
            ((BYTE**)pNewBlock)[0] = m_pData;
            m_pData = pNewBlock;
        }
        else
        {
            ((BYTE**)pNewBlock)[0] = ((BYTE**)m_pData)[0];
            ((BYTE**)m_pData)[0] = pNewBlock;
        }
    }
    else
    {
        // this is the first allocation
        m_pData = pNewBlock;
        ((BYTE**)m_pData)[0] = NULL;
    }

    pNewBlock += (sizeof(void*) * 2);
    LOG((LF_BCL, LL_INFO100, "Level2 - DM - Allocator [0x%p] - ptr -> 0x%p, available 0x%X\n", this, pNewBlock, ((size_t*)m_pData)[1]));
    return pNewBlock;
}

