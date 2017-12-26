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
//*****************************************************************************
// File: request.cpp
//
// CorDataAccess::Request implementation.
//
//*****************************************************************************

#include "stdafx.h"
#include <win32threadpool.h>

#include <gceewks.cpp>
#include <handletablepriv.h>
#include "typestring.h"
#include <virtualcallstub.h>


// GC headers define these to EE-specific stuff that we don't want.
#undef EnterCriticalSection
#undef LeaveCriticalSection

#define PTR_CDADDR(ptr)   TO_CDADDR(PTR_TO_TADDR(ptr))
#define HOST_CDADDR(host) TO_CDADDR(PTR_HOST_TO_TADDR(host))

// Use this when you don't want to instantiate an Object * in the host.
TADDR DACGetMethodTableFromObjectPointer(TADDR objAddr,ICLRDataTarget* target)
{
    ULONG32 returned = sizeof(TADDR);
    TADDR Value = NULL;
    if(target->ReadVirtual(objAddr,
                           (PBYTE)&Value, sizeof(TADDR),
                           &returned) != S_OK)
    {
        return NULL;
    }

    Value = Value & ~3; // equivalent to Object::GCSafeMethodTable()
    return Value;
}

BOOL DacValidateEEClass(EEClass *pEEClass)
{
    BOOL retval = TRUE;
    PAL_CPP_TRY
    {
        MethodTable *pMethodTable = pEEClass->GetMethodTable();
        if (!pMethodTable)
        {
            // PREfix.
            return FALSE;
        }
        if (pEEClass != pMethodTable->GetClass())
        {
            retval = FALSE;
        }
    }
    PAL_CPP_CATCH_ALL
    {
        retval = FALSE;
    }
    PAL_CPP_ENDTRY
    return retval;

}

BOOL DacValidateMethodTable(MethodTable *pMT, BOOL &bIsFree)
{
    // Verify things are right.
    BOOL retval = FALSE;
    PAL_CPP_TRY
    {
        bIsFree = FALSE;
        EEClass *pEEClass = pMT->GetClass();
        if (pEEClass==NULL)
        {
            // Okay to have a NULL EEClass if this is a free methodtable
            CLRDATA_ADDRESS MethTableAddr = HOST_CDADDR(pMT);
            CLRDATA_ADDRESS FreeObjMethTableAddr = HOST_CDADDR(g_pFreeObjectMethodTable);
            if (MethTableAddr != FreeObjMethTableAddr)
                goto BadMethodTable;

            bIsFree = TRUE;
        }
        else
        {
            // Methodtables should be linked properly
            if (pEEClass->GetMethodTable() != pMT)
            {
                // May be a generic type
                if (pMT->HasInstantiation())
                {
                    MethodTable *pCanonical = pMT->GetCanonicalMethodTable();
                    if (pCanonical->GetClass() != pEEClass)
                    {
                        goto BadMethodTable;
                    }
                }
                else
                {
                    goto BadMethodTable;
                }
            }

            // In rare cases, we've seen the standard check above pass when it shouldn't.
            // Insert additional/ad-hoc tests below.

            // Metadata token should look valid for a class
            mdTypeDef td = pEEClass->GetCl();
            if (td != mdTokenNil && TypeFromToken(td) != mdtTypeDef)
                goto BadMethodTable;

            // BaseSize should always be greater than 0 for valid objects (unless it's an interface)
            if (!pMT->IsInterface())
            {
                if (pMT->GetBaseSize() == 0 || !IS_ALIGNED(pMT->GetBaseSize(), sizeof(void *)))
                    goto BadMethodTable;
            }
        }

        retval = TRUE;

BadMethodTable: ;
    }
    PAL_CPP_CATCH_ALL
    {
        retval = FALSE;
    }
    PAL_CPP_ENDTRY
    return retval;

}

BOOL DacValidateMD(LPCVOID pMD)
{
    return DacValidateMD((MethodDesc *)pMD, (HINSTANCE) DacGlobalBase());
}

BOOL DacValidateMD(MethodDesc *pMD, HINSTANCE hMscorwks)
{
    if (pMD == NULL)
    {
        return FALSE;
    }

    // Verify things are right.
    BOOL retval = TRUE;
    PAL_CPP_TRY
    {
        EEClass *pEEClass = pMD->GetClass();
        MethodTable *pMethodTable = pMD->GetMethodTable();
        if (pEEClass->GetMethodTable() != pMethodTable)
        {
            // May be a generic type
            if (pMethodTable->HasInstantiation())
            {
                MethodTable *pCanonical = pMethodTable->GetCanonicalMethodTable();
                if (pCanonical->GetClass() != pEEClass)
                {
                    retval = FALSE;
                }
            }
            else
            {
                retval = FALSE;
            }
        }

        if (retval && (pMD->GetSlot() > pMethodTable->GetNumMethods() && !pMD->HasNonVtableSlot()))
        {
            retval = FALSE;
        }

        if (retval && pMD->HasTemporaryEntryPoint())
        {
            MethodDesc *pMDCheck = MethodDesc::GetMethodDescFromStubAddr(pMD->GetTemporaryEntryPoint(), TRUE);

            if (PTR_HOST_TO_TADDR(pMD) != PTR_HOST_TO_TADDR(pMDCheck))
            {
                retval = FALSE;
            }
        }

        if (retval && pMD->HasNativeCode())
        {
            TADDR jitCodeAddr = pMD->GetNativeCode();

            // FindJitMan will check that the address is in a JIT code heap
            IJitManager *pJIT = ExecutionManager::FindJitMan((SLOT)jitCodeAddr);
            if (pJIT)
            {
                // Check that the given MethodDesc matches the MethodDesc from
                // the CodeHeaader
                MethodDesc *pMDCheck = NULL;
                pJIT->JitCodeToMethodInfo((SLOT)jitCodeAddr, &pMDCheck);

                if (PTR_HOST_TO_TADDR(pMD) != PTR_HOST_TO_TADDR(pMDCheck)
                    )
                {
                    retval = FALSE;
                }
            }
            else if (!pMD->IsFCall()
                || (ECall::MapTargetBackToMethod(jitCodeAddr) != pMD)
                )
            {
                // If it is _not_ an FCall and not Jitted code then it's bogus!!!
                // If it is an FCall but the code address isn't findable by
                // MapTargetBackToMethod then it's bogus!!!
                retval = FALSE;
            }
        }
    }
    PAL_CPP_CATCH_ALL
    {
        retval = FALSE;
    }
    PAL_CPP_ENDTRY
    return retval;
}

VOID GetJITMethodInfo (IJitManager *jitMan, METHODTOKEN mToken, JITTypes *pJITType, CLRDATA_ADDRESS *pGCInfo)
{
    DWORD dwType = jitMan->GetCodeType();
    if (IsMiIL(dwType))
    {
        *pJITType = TYPE_JIT;
    }
    else if (IsMiNative(dwType))
    {
        *pJITType = TYPE_PJIT;
    }
    else if ((dwType & (miManaged_IL_EJIT |
                        miCodeTypeMask)) == miManaged_IL_EJIT)
    {
        *pJITType = TYPE_EJIT;
    }
    else
    {
        *pJITType = TYPE_UNKNOWN;
    }

    *pGCInfo = (CLRDATA_ADDRESS) (ULONG_PTR)jitMan->GetGCInfo(mToken);
}


HRESULT
ClrDataAccess::RequestWorkRequestData(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpWorkRequestData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS *) inBuffer;
    WorkRequest *pRequest = PTR_WorkRequest(TO_TADDR(addr));
    if (!pRequest)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    DacpWorkRequestData* workRequestData = (DacpWorkRequestData*)outBuffer;

    workRequestData->Function = (TADDR)(pRequest->Function);
    workRequestData->Context = (TADDR)(pRequest->Context);
    workRequestData->NextWorkRequest = (TADDR)(pRequest->next);

    return S_OK;
}

HRESULT
ClrDataAccess::RequestThreadpoolData(IN ULONG32 inBufferSize,
                                     IN BYTE* inBuffer,
                                     IN ULONG32 outBufferSize,
                                     OUT BYTE* outBuffer)
{
    if ((inBufferSize != 0) ||
        (inBuffer != NULL) ||
        (outBufferSize != sizeof(DacpThreadpoolData)))
    {
        return E_INVALIDARG;
    }

    DacpThreadpoolData* threadpoolData = (DacpThreadpoolData*)outBuffer;

    threadpoolData->cpuUtilization = ThreadpoolMgr::cpuUtilization;
    threadpoolData->NumWorkerThreads  = ThreadpoolMgr::NumWorkerThreads;
    threadpoolData->MinLimitTotalWorkerThreads = ThreadpoolMgr::MinLimitTotalWorkerThreads;
    threadpoolData->MaxLimitTotalWorkerThreads = ThreadpoolMgr::MaxLimitTotalWorkerThreads;
    threadpoolData->NumRunningWorkerThreads = ThreadpoolMgr::NumWorkerThreads - ThreadpoolMgr::NumIdleWorkerThreads;
    threadpoolData->NumIdleWorkerThreads = ThreadpoolMgr::NumIdleWorkerThreads;
    threadpoolData->NumQueuedWorkRequests = ThreadpoolMgr::NumQueuedWorkRequests;

    threadpoolData->FirstWorkRequest = HOST_CDADDR(ThreadpoolMgr::WorkRequestHead);

    threadpoolData->NumCPThreads = ThreadpoolMgr::NumCPThreads;
    threadpoolData->NumFreeCPThreads = ThreadpoolMgr::NumFreeCPThreads;
    threadpoolData->MaxFreeCPThreads  = ThreadpoolMgr::MaxFreeCPThreads;
    threadpoolData->NumRetiredCPThreads = ThreadpoolMgr::NumRetiredCPThreads;
    threadpoolData->MaxLimitTotalCPThreads = ThreadpoolMgr::MaxLimitTotalCPThreads;
    threadpoolData->CurrentLimitTotalCPThreads = ThreadpoolMgr::CurrentLimitTotalCPThreads;
    threadpoolData->MinLimitTotalCPThreads = ThreadpoolMgr::MinLimitTotalCPThreads;

    TADDR pEntry = DacGetTargetAddrForHostAddr(&ThreadpoolMgr::TimerQueue,true);
    ThreadpoolMgr::LIST_ENTRY entry;
    DacReadAll(pEntry,&entry,sizeof(ThreadpoolMgr::LIST_ENTRY),true);
    TADDR node = (TADDR) entry.Flink;
    threadpoolData->NumTimers = 0;
    while (node && node != pEntry)
    {
        threadpoolData->NumTimers++;
        DacReadAll(node,&entry,sizeof(ThreadpoolMgr::LIST_ENTRY),true);
        node = (TADDR) entry.Flink;
    }
    
    threadpoolData->QueueUserWorkItemCallbackFPtr = (CLRDATA_ADDRESS) GFN_TADDR(QueueUserWorkItemCallback);
    threadpoolData->AsyncCallbackCompletionFPtr = (CLRDATA_ADDRESS) GFN_TADDR(ThreadpoolMgr__AsyncCallbackCompletion);
    threadpoolData->AsyncTimerCallbackCompletionFPtr = (CLRDATA_ADDRESS) GFN_TADDR(ThreadpoolMgr__AsyncTimerCallbackCompletion);
    return S_OK;
}

HRESULT
ClrDataAccess::RequestThreadStoreData(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{
    if ((inBufferSize != 0) ||
        (inBuffer != NULL) ||
        (outBufferSize != sizeof(DacpThreadStoreData)))
    {
        return E_INVALIDARG;
    }

    DacpThreadStoreData* threadStoreData = (DacpThreadStoreData*)outBuffer;
    ThreadStore* threadStore = ThreadStore::s_pThreadStore;
    if (!threadStore)
    {
        return E_INVALIDARG;
    }

    threadStoreData->threadCount =
        threadStore->m_ThreadCount;
    threadStoreData->unstartedThreadCount =
        threadStore->m_UnstartedThreadCount;
    threadStoreData->backgroundThreadCount =
        threadStore->m_BackgroundThreadCount;
    threadStoreData->pendingThreadCount =
        threadStore->m_PendingThreadCount;
    threadStoreData->deadThreadCount =
        threadStore->m_DeadThreadCount;

    threadStoreData->fHostConfig = g_fHostConfig;

    threadStoreData->firstThread =
        HOST_CDADDR(threadStore->m_ThreadList.GetHead());

    threadStoreData->finalizerThread = HOST_CDADDR(GCHeap::FinalizerThread);
    threadStoreData->gcThread = HOST_CDADDR(GCHeap::GcThread);
    return S_OK;
}

HRESULT
ClrDataAccess::RequestDACUnitTestData(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{
    if ((inBufferSize != 0) ||
        (inBuffer != NULL) ||
        (outBufferSize != sizeof(DacpDACUnitTest)))
    {
        return E_INVALIDARG;
    }

    DacpDACUnitTest* unitTestData = (DacpDACUnitTest*)outBuffer;

    unitTestData->threadStoreAddr = HOST_CDADDR(ThreadStore::s_pThreadStore);
    unitTestData->threadClassAddr = HOST_CDADDR(g_pThreadClass);
    unitTestData->fProcessDetach = g_fProcessDetach;
    return S_OK;
}

#ifdef STRESS_LOG
HRESULT
ClrDataAccess::RequestStressLogData(IN ULONG32 inBufferSize,
                                    IN BYTE* inBuffer,
                                    IN ULONG32 outBufferSize,
                                    OUT BYTE* outBuffer)
{
    if ((inBufferSize != 0) ||
        (inBuffer != NULL) ||
        (outBufferSize != sizeof(DacpStressLogData)))
    {
        return E_INVALIDARG;
    }

    DacpStressLogData* stressLogData = (DacpStressLogData*)outBuffer;

    if (!g_pStressLog.IsValid())
    {
        return E_FAIL;
    }

    stressLogData->StressLogAddress = HOST_CDADDR(g_pStressLog);
    return S_OK;
}
#endif // STRESS_LOG

#ifdef _DEBUG
#endif // _DEBUG

HRESULT
ClrDataAccess::RequestJitList(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{
    if ((inBufferSize != 0) ||
        (inBuffer != NULL) ||
        (outBufferSize != sizeof(DacpJitListInfo)))
    {
        return E_INVALIDARG;
    }

    DacpJitListInfo* jitInfo = (DacpJitListInfo*)outBuffer;
    LONG countList = 0;
    IJitManager *managerPtr = ExecutionManager::m_pJitList;
    while (managerPtr)
    {
        countList++;
        managerPtr = managerPtr->m_next;
    }

    jitInfo->JitCount = countList;
    return S_OK;
}

HRESULT
ClrDataAccess::RequestJitManagerList(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{
    if ((inBufferSize != 0) ||
        (inBuffer != NULL) ||
        (outBufferSize == NULL))
    {
        return E_INVALIDARG;
    }

    DacpJitManagerInfo *infoArray = (DacpJitManagerInfo *) outBuffer;
    LONG countAllowed = outBufferSize / sizeof(DacpJitManagerInfo);

    IJitManager *managerPtr = (ExecutionManager::m_pJitList);
    int i = 0;
    while ((managerPtr != NULL) && (i < countAllowed))
    {
        DacpJitManagerInfo *currentPtr = infoArray + i;
        currentPtr->managerAddr = HOST_CDADDR(managerPtr);
        currentPtr->codeType = managerPtr->GetCodeType();
        if (IsMiIL(managerPtr->GetCodeType())
#ifdef FJIT
            || (managerPtr->GetCodeType() & (miManaged_IL_EJIT | miCodeTypeMask)) == miManaged_IL_EJIT
#endif
        )
        {
            EEJitManager *eeJitManager = PTR_EEJitManager(PTR_HOST_TO_TADDR(managerPtr));
            currentPtr->ptrHeapList = HOST_CDADDR(eeJitManager->m_pCodeHeap);
        }

        // visit next DacpJitManagerInfo *

        i++;
        managerPtr = managerPtr->m_next;
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestJitHeapList(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(DacpJitHeapList)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpJitHeapList)) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    DacpJitHeapList* jitHeapList = (DacpJitHeapList*)outBuffer;
    if (jitHeapList->eeJitManager == NULL)
    {
        return E_INVALIDARG;
    }

    EEJitManager *jitManager = PTR_EEJitManager(TO_TADDR(jitHeapList->eeJitManager));
    LONG countList = 0;
    HeapList *heapList = jitManager->m_pCodeHeap;
    while (heapList)
    {
        countList++;
        heapList = heapList->hpNext;
    }

    jitHeapList->heapListCount = countList;
    return S_OK;
}

HRESULT
ClrDataAccess::RequestMethodTableSlot(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(DacpSlotArgs)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS *outAddr = (CLRDATA_ADDRESS *) outBuffer;
    DacpSlotArgs *args = (DacpSlotArgs *) inBuffer;
    MethodTable* mTable = PTR_MethodTable(TO_TADDR(args->mtaddr));
    if (!mTable)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    BOOL bIsFree;
    if (!DacValidateMethodTable(mTable,bIsFree))
    {
        return E_INVALIDARG;
    }

    if (args->SlotNumber >= mTable->GetNumMethods())
    {
        return E_INVALIDARG;
    }

    // Now get the slot:
    PTR_SLOT slot = mTable->GetRestoredSlotPtr(args->SlotNumber);
    *outAddr = PTR_CDADDR(slot);

    return S_OK;
}

HRESULT
ClrDataAccess::RequestCodeHeapList(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(DacpJitHeapList)) ||
        (inBuffer == NULL) ||
        (outBufferSize == NULL))
    {
        return E_INVALIDARG;
    }

    DacpJitHeapList* jitHeapList = (DacpJitHeapList*)inBuffer;
    if (jitHeapList->eeJitManager == NULL)
    {
        return E_INVALIDARG;
    }

    EEJitManager *jitManager = PTR_EEJitManager(TO_TADDR(jitHeapList->eeJitManager));

    DacpJitCodeHeapInfo *infoArray = (DacpJitCodeHeapInfo *) outBuffer;
    LONG countAllowed = outBufferSize / sizeof(DacpJitCodeHeapInfo);

    int i = 0;
    HeapList *heapList = jitManager->m_pCodeHeap;
    while ((heapList != NULL) && (i < countAllowed))
    {
        // What type of CodeHeap pointer do we have?
        CodeHeap *codeHeap = heapList->pHeap;
        TADDR ourVTablePtr = VPTR_HOST_VTABLE_TO_TADDR(*(LPVOID*)codeHeap);
        if (ourVTablePtr == LoaderCodeHeap::VPtrTargetVTable())
        {
            LoaderCodeHeap *loaderCodeHeap = PTR_LoaderCodeHeap(PTR_HOST_TO_TADDR(codeHeap));
            infoArray[i].codeHeapType = CODEHEAP_LOADER;
            infoArray[i].LoaderHeap = HOST_CDADDR(loaderCodeHeap->pLoaderHeap);
        }
        else if (ourVTablePtr == HostCodeHeap::VPtrTargetVTable())
        {
            HostCodeHeap *hostCodeHeap = PTR_HostCodeHeap(PTR_HOST_TO_TADDR(codeHeap));
            infoArray[i].codeHeapType = CODEHEAP_HOST;
            infoArray[i].HostData.baseAddr = PTR_CDADDR(hostCodeHeap->m_pBaseAddr);
            infoArray[i].HostData.currentAddr = PTR_CDADDR(hostCodeHeap->m_pLastAvailableCommittedAddr);
        }
        else
        {
            infoArray[i].codeHeapType = CODEHEAP_UNKNOWN;
        }
        heapList = heapList->hpNext;
        i++;
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestThreadFromThinlock(IN ULONG32 inBufferSize,
                                 IN BYTE* inBuffer,
                                 IN ULONG32 outBufferSize,
                                 OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(UINT)) ||
        (outBufferSize != sizeof(CLRDATA_ADDRESS *)))
    {
        return E_INVALIDARG;
    }

    UINT thinlockId = *((UINT *) inBuffer);
    CLRDATA_ADDRESS* outThread = (CLRDATA_ADDRESS *)outBuffer;

    Thread *pThread = g_pThinLockThreadIdDispenser->IdToThread(thinlockId);
    *outThread = PTR_HOST_TO_TADDR(pThread);
    return S_OK;
}

HRESULT
ClrDataAccess::RequestThreadData(IN ULONG32 inBufferSize,
                                 IN BYTE* inBuffer,
                                 IN ULONG32 outBufferSize,
                                 OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(DacpThreadArgs)) ||
        (outBufferSize != sizeof(DacpThreadData)))
    {
        return E_INVALIDARG;
    }

    DacpThreadArgs *args = (DacpThreadArgs *) inBuffer;
    DacpThreadData* threadData = (DacpThreadData*)outBuffer;
    Thread* thread = PTR_Thread(TO_TADDR(args->addrThread));

    ZeroMemory (threadData, sizeof(DacpThreadData));
    threadData->corThreadId = thread->m_ThreadId;
    threadData->osThreadId = thread->m_OSThreadId;
    threadData->state = thread->m_State;
    threadData->preemptiveGCDisabled = thread->m_fPreemptiveGCDisabled;
    threadData->allocContextPtr =
        TO_CDADDR(thread->m_alloc_context.alloc_ptr);
    threadData->allocContextLimit =
        TO_CDADDR(thread->m_alloc_context.alloc_limit);

    threadData->fiberData = (CLRDATA_ADDRESS)
        PTR_HOST_MEMBER_TADDR(Thread, thread, m_pFiberData);

    ULONG32 returned = sizeof(TADDR);
    TADDR Value = NULL;
    if(m_target->ReadVirtual(PTR_HOST_MEMBER_TADDR(Thread, thread, m_pFiberData),
                           (PBYTE)&Value, sizeof(TADDR),
                           &returned) == S_OK)
    {
        threadData->fiberData = (CLRDATA_ADDRESS) Value;
    }

    // Don't try to fill these in if we are debugging a mini-dump, they are not saved.
    if (args->isFullDump)
    {
        EX_TRY
        {
            threadData->pSharedStaticData = HOST_CDADDR(thread->GetSharedStaticData());
            threadData->pUnsharedStaticData = HOST_CDADDR(thread->GetUnsharedStaticData());
        }
        EX_CATCH
        {
            threadData->pSharedStaticData = NULL;
            threadData->pUnsharedStaticData = NULL;
        }
        EX_END_CATCH(SwallowAllExceptions)
    }

    threadData->pFrame = PTR_CDADDR(thread->m_pFrame);
    threadData->context = PTR_CDADDR(thread->m_Context);
    threadData->domain = PTR_CDADDR(thread->m_pDomain);
    threadData->lockCount = thread->m_dwLockCount;
    threadData->lastThrownObjectHandle =
        TO_CDADDR(thread->m_LastThrownObjectHandle);
    threadData->nextThread =
        HOST_CDADDR(ThreadStore::s_pThreadStore->m_ThreadList.GetNext(thread));
    threadData->firstNestedException = PTR_HOST_TO_TADDR(
        thread->m_ExceptionState.m_currentExInfo.m_pPrevNestedInfo);
    return S_OK;
}

HRESULT
ClrDataAccess::RequestMethodDescData(IN ULONG32 inBufferSize,
                                 IN BYTE* inBuffer,
                                 IN ULONG32 outBufferSize,
                                 OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (outBufferSize != sizeof(DacpMethodDescData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    DacpMethodDescData* methodDescData = (DacpMethodDescData*)outBuffer;

    MethodDesc* pMD = PTR_MethodDesc(TO_TADDR(addr));

    if (!DacValidateMD(pMD, (HINSTANCE)m_globalBase))
    {
        return E_INVALIDARG;
    }

    ZeroMemory(methodDescData,sizeof(DacpMethodDescData));
    
    methodDescData->bHasNativeCode = pMD->HasNativeCode();
    methodDescData->bIsDynamic = (pMD->IsLCGMethod()) ? TRUE : FALSE;
    methodDescData->wSlotNumber = pMD->m_wSlotNumber;
    methodDescData->NativeCodeAddr = pMD->HasNativeCode() ?
        TO_CDADDR(pMD->GetNativeAddrofCode()) : (CLRDATA_ADDRESS)-1;
    methodDescData->AddressOfNativeCodeSlot = PTR_CDADDR(pMD->GetAddrOfNativeCodeSlot());
    methodDescData->PreStubAddr = TO_CDADDR(pMD->GetCallablePreStubAddr());
    methodDescData->MDToken = pMD->GetMemberDef();
    methodDescData->MethodDescPtr = addr;
    methodDescData->EEClassPtr = HOST_CDADDR(pMD->GetClass());
    methodDescData->MethodTablePtr = HOST_CDADDR(pMD->GetMethodTable());
    methodDescData->ModulePtr = HOST_CDADDR(pMD->GetClass()->GetModule());


    // Set this above Dario since you know how to tell if dynamic
    if (methodDescData->bIsDynamic)
    {
        DynamicMethodDesc *pDynamicMethod = PTR_DynamicMethodDesc(TO_TADDR(addr));
        if (pDynamicMethod)
        {
            LCGMethodResolver *pResolver = pDynamicMethod->GetLCGMethodResolver();
            if (pResolver)
            {
                OBJECTREF value = ObjectFromHandle(pResolver->m_managedResolver);
                if (value)
                {
                    FieldDesc *pField = (&g_Mscorlib)->GetField(FIELD__DYNAMICRESOLVER__DYNAMIC_METHOD);
                    _ASSERTE(pField);
                    value = pField->GetRefValue(value);
                    if (value)
                    {
                        methodDescData->managedDynamicMethodObject = PTR_HOST_TO_TADDR(value);
                    }
                }
            }
        }            
    }
    
    return S_OK;
}


HRESULT
ClrDataAccess::RequestCodeHeaderData(IN ULONG32 inBufferSize,
                                 IN BYTE* inBuffer,
                                 IN ULONG32 outBufferSize,
                                 OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (outBufferSize != sizeof(DacpCodeHeaderData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    DacpCodeHeaderData* codeHeaderData = (DacpCodeHeaderData*)outBuffer;
    IJitManager* jitMan =
        ExecutionManager::FindJitMan(TO_SLOT(TO_TADDR(addr)),
                                     IJitManager::ScanNoReaderLock);
    if (!jitMan)
    {
        // We may be able to walk stubs to find a method desc if it's not a jitted method.
        MethodDesc *methodDescI = MethodTable::GetUnknownMethodDescForSlotAddress(TO_SLOT(TO_TADDR(addr)));
        if (methodDescI == NULL)
        {
            return E_INVALIDARG;
        }
        codeHeaderData->MethodDescPtr = HOST_CDADDR(methodDescI);
        codeHeaderData->JITType = TYPE_UNKNOWN;
        codeHeaderData->GCInfo = NULL;
        codeHeaderData->MethodStart = NULL;
        codeHeaderData->MethodSize = 0;
        codeHeaderData->ColdRegionStart = NULL;
        return S_OK;
    }

    MethodDesc* methodDesc;
    METHODTOKEN mToken;
    DWORD codeOffs;

    jitMan->JitCodeToMethodInfo(TO_SLOT(TO_TADDR(addr)),
                                &methodDesc, &mToken, &codeOffs,
                                IJitManager::ScanNoReaderLock);
    if (!methodDesc)
    {
        return E_INVALIDARG;
    }

    codeHeaderData->MethodDescPtr = HOST_CDADDR(methodDesc);

    GetJITMethodInfo(jitMan, mToken, &codeHeaderData->JITType, &codeHeaderData->GCInfo);

    codeHeaderData->MethodStart = (CLRDATA_ADDRESS) (ULONG_PTR) jitMan->JitTokenToStartAddress(mToken);
    codeHeaderData->MethodSize = jitMan->GetCodeManager()->GetFunctionSize(jitMan->GetGCInfo(mToken));

    IJitManager::MethodRegionInfo methodRegionInfo = {NULL, 0, NULL, 0};
    jitMan->JitTokenToMethodRegionInfo(mToken, &methodRegionInfo);

    codeHeaderData->HotRegionSize = (DWORD) methodRegionInfo.hotSize;
    codeHeaderData->ColdRegionSize = (DWORD) methodRegionInfo.coldSize;
    codeHeaderData->ColdRegionStart = (CLRDATA_ADDRESS) (ULONG_PTR) methodRegionInfo.coldStartAddress;

    return S_OK;
}

HRESULT
ClrDataAccess::RequestMethodDescFrameData(IN ULONG32 inBufferSize,
                                 IN BYTE* inBuffer,
                                 IN ULONG32 outBufferSize,
                                 OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (outBufferSize != sizeof(DacpMethodDescData)) ||
        (inBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    Frame *pFrame = PTR_Frame(TO_TADDR(addr));

    if (pFrame == NULL)
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS methodDescAddr = HOST_CDADDR(pFrame->GetFunction());
    return RequestMethodDescData(sizeof(CLRDATA_ADDRESS),(PBYTE) &methodDescAddr,outBufferSize,outBuffer);
}

HRESULT
ClrDataAccess::RequestMethodDescIPData(IN ULONG32 inBufferSize,
                                 IN BYTE* inBuffer,
                                 IN ULONG32 outBufferSize,
                                 OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (outBufferSize != sizeof(DacpMethodDescData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    DacpMethodDescData* methodDescData = (DacpMethodDescData*)outBuffer;
    IJitManager* jitMan =
        ExecutionManager::FindJitMan(TO_SLOT(TO_TADDR(addr)),
                                     IJitManager::ScanNoReaderLock);
    if (!jitMan)
    {
        return E_INVALIDARG;
    }

    MethodDesc* methodDesc;
    METHODTOKEN mToken;
    DWORD codeOffs;

    jitMan->JitCodeToMethodInfo(TO_SLOT(TO_TADDR(addr)),
                                &methodDesc, &mToken, &codeOffs,
                                IJitManager::ScanNoReaderLock);
    if (!methodDesc)
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS methodDescAddr = HOST_CDADDR(methodDesc);
    HRESULT hr = RequestMethodDescData(sizeof(CLRDATA_ADDRESS),(PBYTE) &methodDescAddr,outBufferSize,outBuffer);
    if (S_OK != hr)
        return hr;


    JITTypes JITType;
    GetJITMethodInfo(jitMan, mToken, &JITType, &methodDescData->GCInfo);

    methodDescData->JITType = JITType;

    return hr;
}

HRESULT
ClrDataAccess::RequestMethodName(IN ULONG32 inBufferSize,
                                 IN BYTE* inBuffer,
                                 IN ULONG32 outBufferSize,
                                 OUT BYTE* outBuffer)
{
    if (inBufferSize != sizeof(CLRDATA_ADDRESS))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    MethodDesc* pMD = PTR_MethodDesc(TO_TADDR(addr));

    StackSString str;

    PAL_CPP_TRY
    {
        TypeString::AppendMethodInternal(str, pMD, TypeString::FormatSignature|TypeString::FormatNamespace|TypeString::FormatFullInst);
    }
    PAL_CPP_CATCH_ALL
    {
        bool handled = false;
        if (pMD->IsDynamicMethod())
        {
            if (pMD->IsLCGMethod()
               )
            {
                // In heap dumps, trying to format the signature can fail 
                // in certain cases because StoredSigMethodDesc::m_pSig points
                // to the IMAGE_MAPPED layout (in the PEImage::m_pLayouts array).
                // We save only the IMAGE_LOADED layout to the heap dump. Rather
                // than bloat the dump, we just drop the signature in these
                // cases.
                
                str.Clear();
                TypeString::AppendMethodInternal(str, pMD, TypeString::FormatNamespace|TypeString::FormatFullInst);
                handled = true;
            }
        }
        else
        {
            str.Clear();
            Module* pModule = pMD->GetModule();
            if (pModule)
            {
                WCHAR path[MAX_PATH];
                COUNT_T nChars = 0;
                if (pModule->GetPath().DacGetUnicode(NumItems(path), path, &nChars) &&
                    nChars > 0 && nChars <= NumItems(path))
                {
                    WCHAR* pFile = path + nChars - 1;
                    while ((pFile >= path) && (*pFile != L'\\'))
                    {
                        pFile--;
                    }
                    pFile++;
                    if (*pFile)
                    {
                        str.Append(pFile);
                        str.Append(L"!Unknown");
                        handled = true;
                    }
                }
            }
        }

        if (!handled)
        {
            PAL_CPP_RETHROW;
        }
    }
    PAL_CPP_ENDTRY


    ULONG32 outWstrLen = outBufferSize/sizeof(WCHAR);
    wcsncpy_s((WCHAR*)outBuffer, outWstrLen, str.GetUnicode(), _TRUNCATE);
    outBuffer[outWstrLen - 1] = L'\0';

    return S_OK;
}

HRESULT
ClrDataAccess::RequestContextData(IN ULONG32 inBufferSize,
                                  IN BYTE* inBuffer,
                                  IN ULONG32 outBufferSize,
                                  OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (outBufferSize != sizeof(DacpContextData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    DacpContextData* contextData = (DacpContextData*)outBuffer;
    Context* context = PTR_Context(TO_TADDR(addr));
    if (!context)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    contextData->domain = HOST_CDADDR(context->GetDomain());

    return S_OK;
}


HRESULT
ClrDataAccess::RequestObjectStringData(IN ULONG32 inBufferSize,
                                  IN BYTE* inBuffer,
                                  IN ULONG32 outBufferSize,
                                  OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }


    TADDR mtTADDR = DACGetMethodTableFromObjectPointer(addr,m_target);
    if (mtTADDR==NULL)
    {
        return E_INVALIDARG;
    }
    MethodTable *mt = PTR_MethodTable(mtTADDR);

    BOOL bFree;
    if (!DacValidateMethodTable(mt, bFree))
    {
        return E_INVALIDARG;
    }

    ULONG32 NameLength = outBufferSize;
    LPWSTR pwszBuffer = (LPWSTR) outBuffer;

    // Object must be a string
    if (HOST_CDADDR(mt) != HOST_CDADDR(g_pStringClass))
    {
        return E_INVALIDARG;
    }

    // Return as much of the string as the caller asked for.
    CLRDATA_ADDRESS pszStr = addr+offsetof(StringObject,m_Characters);
    ULONG32 returned = NameLength;
    m_target->ReadVirtual(pszStr, (PBYTE)pwszBuffer, NameLength,&returned);
    if (returned != NameLength)
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT
ClrDataAccess::RequestObjectClassName(IN ULONG32 inBufferSize,
                                  IN BYTE* inBuffer,
                                  IN ULONG32 outBufferSize,
                                  OUT BYTE* outBuffer)
{
    HRESULT hr = S_OK;

    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    // Don't turn the Object into a pointer, it is too costly on
    // scans of the gc heap.
    TADDR mtTADDR = DACGetMethodTableFromObjectPointer(addr,m_target);
    if (mtTADDR==NULL)
    {
        return E_INVALIDARG;
    }
    MethodTable *mt = PTR_MethodTable(mtTADDR);

    BOOL bFree;
    if (!DacValidateMethodTable(mt, bFree))
    {
        return E_INVALIDARG;
    }

    EX_TRY
    {
        // There is a case where metadata was unloaded and the AppendType call will fail.
        // This is when an AppDomain has been unloaded but not yet collected.

        ULONG32 outWstrLen = outBufferSize/sizeof(WCHAR);
        PEFile *pPEFile = mt->GetModule()->GetFile();
        if (pPEFile->GetNativeImage() == NULL && pPEFile->GetILimage() == NULL)
        {
            wcsncpy_s((WCHAR*)outBuffer, outWstrLen, L"<Unloaded Type>", _TRUNCATE);
        }
        else
        {
            StackSString s;
            TypeString::AppendType(s, TypeHandle(mt), TypeString::FormatNamespace|TypeString::FormatFullInst);
            wcsncpy_s((WCHAR*)outBuffer, outWstrLen, s.GetUnicode(), _TRUNCATE);
        }
        outBuffer[outWstrLen - 1] = L'\0';
    }
    EX_CATCH
    {
        hr = E_FAIL;
    }
    EX_END_CATCH(SwallowAllExceptions)

    return hr;
}

HRESULT
ClrDataAccess::RequestModuleTokenData(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(DacpModuleTokenData)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpModuleTokenData)))
    {
        return E_INVALIDARG;
    }

    DacpModuleTokenData *inputData = (DacpModuleTokenData*)inBuffer;
    DacpModuleTokenData *outputData = (DacpModuleTokenData *)outBuffer;
    Module* pModule = PTR_Module(TO_TADDR(inputData->Module));

    TypeHandle th;
    switch (TypeFromToken(inputData->Token))
    {
        case mdtFieldDef:
            outputData->ReturnValue = HOST_CDADDR(pModule->LookupFieldDef(inputData->Token));
            break;
        case mdtMethodDef:
            outputData->ReturnValue = HOST_CDADDR(pModule->LookupMethodDef(inputData->Token));
            break;
        case mdtTypeDef:
            th = pModule->LookupTypeDef(inputData->Token);
            outputData->ReturnValue = th.AsTAddr();
            break;
        case mdtTypeRef:
            th = pModule->LookupTypeDef(inputData->Token);
            outputData->ReturnValue = th.AsTAddr();
            break;
        default:
            return E_INVALIDARG;
            break;
    }

    outputData->Token = inputData->Token;
    outputData->Module = inputData->Module;
    return S_OK;
}

HRESULT
ClrDataAccess::RequestModuleMapTraverse(IN ULONG32 inBufferSize,
                                 IN BYTE* inBuffer,
                                 IN ULONG32 outBufferSize,
                                 OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(DacpModuleMapTraverseArgs)) ||
        (inBuffer == NULL) ||
        (outBufferSize != 0))
    {
        return E_INVALIDARG;
    }

    DacpModuleMapTraverseArgs *args = (DacpModuleMapTraverseArgs*)inBuffer;
    if (args == NULL)
    {
        return E_INVALIDARG;
    }

    Module* pModule = PTR_Module(TO_TADDR(args->Module));

    if (!pModule)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    // We want to traverse these two tables, passing callback information
    LookupMap *pMap = NULL;
    switch (args->mmt)
    {
        case TYPEDEFTOMETHODTABLE:
            pMap = &pModule->m_TypeDefToMethodTableMap;
            break;
        case TYPEREFTOMETHODTABLE:
            pMap = &pModule->m_TypeRefToMethodTableMap;
            break;
        default:
            return E_INVALIDARG;
    }

    LookupMap::Iterator typeIter(pMap);
    for (int i = 0; typeIter.Next(); i++)
    {
        if (typeIter.GetElement())
        {
            MethodTable* pMT = PTR_MethodTable(typeIter.GetElement());
            (args->Callback)(i,PTR_HOST_TO_TADDR(pMT),args->token);
        }
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestModuleData(IN ULONG32 inBufferSize,
                                 IN BYTE* inBuffer,
                                 IN ULONG32 outBufferSize,
                                 OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpModuleData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    DacpModuleData* ModuleData = (DacpModuleData*)outBuffer;
    Module* pModule = PTR_Module(TO_TADDR(addr));

    if (!pModule)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    ZeroMemory(ModuleData,sizeof(DacpModuleData));

    ModuleData->File = HOST_CDADDR(pModule->GetFile());
    COUNT_T metadataSize = 0;
    if (pModule->GetFile()->HasNativeImage())
    {
        ModuleData->ilBase = (CLRDATA_ADDRESS)(ULONG_PTR) pModule->GetFile()->GetLoadedNative()->GetBase();
    }
    else if (!pModule->GetFile()->IsDynamic())
    {
        ModuleData->ilBase = (CLRDATA_ADDRESS)(ULONG_PTR) pModule->GetFile()->GetIJWBase();
    }

    ModuleData->metadataStart = (CLRDATA_ADDRESS)(ULONG_PTR) pModule->GetFile()->GetMetadata(&metadataSize);
    ModuleData->metadataSize = (SIZE_T) metadataSize;

    ModuleData->bIsReflection = pModule->IsReflection();
    ModuleData->bIsPEFile = pModule->IsPEFile();
    ModuleData->Assembly = HOST_CDADDR(pModule->GetAssembly());
    ModuleData->ModuleDefinition = new ClrDataModule(this, pModule);
    ModuleData->dwDomainNeutralIndex = pModule->GetModuleID();
    ModuleData->dwTransientFlags = pModule->m_dwTransientFlags;

    EX_TRY
    {
        //
        // In minidump's case, these data structure is not avaiable.
        //
        ModuleData->TypeDefToMethodTableMap = HOST_CDADDR(pModule->m_TypeDefToMethodTableMap.pTable);
        ModuleData->TypeRefToMethodTableMap = HOST_CDADDR(pModule->m_TypeRefToMethodTableMap.pTable);
        ModuleData->MethodDefToDescMap = HOST_CDADDR(pModule->m_MethodDefToDescMap.pTable);
        ModuleData->FieldDefToDescMap = HOST_CDADDR(pModule->m_FieldDefToDescMap.pTable);
        ModuleData->MemberRefToDescMap = HOST_CDADDR(pModule->m_MemberRefToDescMap.pTable);
        ModuleData->FileReferencesMap = HOST_CDADDR(pModule->m_FileReferencesMap.pTable);
        ModuleData->ManifestModuleReferencesMap = HOST_CDADDR(pModule->m_ManifestModuleReferencesMap.pTable);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions)


    return S_OK;
}

HRESULT
ClrDataAccess::RequestModuleILData(IN ULONG32 inBufferSize,
                                 IN BYTE* inBuffer,
                                 IN ULONG32 outBufferSize,
                                 OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(DacpModuleFindILArgs)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    DacpModuleFindILArgs *pArgs = (DacpModuleFindILArgs *)inBuffer;

    CLRDATA_ADDRESS *pilAddr = (CLRDATA_ADDRESS*)outBuffer;
    if (pilAddr == NULL)
    {
        return E_INVALIDARG;
    }

    Module* pModule = PTR_Module(TO_TADDR(pArgs->Module));

    if (!pModule)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    TADDR il = (TADDR) pModule->GetIL((DWORD)pArgs->rva);
    *pilAddr = (CLRDATA_ADDRESS) il;

    return S_OK;
}

HRESULT
ClrDataAccess::RequestMethodTableData(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpMethodTableData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    DacpMethodTableData* MTData = (DacpMethodTableData*)outBuffer;
    MethodTable* pMT = PTR_MethodTable(TO_TADDR(addr));
    if (!pMT)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    BOOL bIsFree;
    if (!DacValidateMethodTable(pMT,bIsFree))
    {
        return E_INVALIDARG;
    }

    ZeroMemory(MTData,sizeof(DacpMethodTableData));
    MTData->BaseSize = pMT->GetBaseSize();
    MTData->ComponentSize = pMT->GetComponentSize();
    MTData->bIsFree = bIsFree;
    if(!bIsFree)
    {
        MTData->Class = HOST_CDADDR(pMT->GetClass());
        MTData->ParentMethodTable = HOST_CDADDR(pMT->GetParentMethodTable());;
        MTData->wNumInterfaces = pMT->GetNumInterfaces();
        MTData->wTotalVtableSlots = pMT->GetNumMethods();
        MTData->bContainsPointers = pMT->ContainsPointers();
        MTData->bIsShared = (pMT->IsDomainNeutral() ? TRUE : FALSE); // flags & enum_flag_DomainNeutral
        MTData->sizeofMethodTable = sizeof(*pMT);
        MTData->bIsDynamic = pMT->IsDynamicStatics();
    }
    return S_OK;
}

HRESULT
ClrDataAccess::RequestMethodTableName(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{
    HRESULT hr = S_OK;
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    MethodTable* pMT = PTR_MethodTable(TO_TADDR(addr));
    if (!pMT)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    BOOL bIsFree;
    if (!DacValidateMethodTable(pMT,bIsFree))
    {
        return E_INVALIDARG;
    }

    EX_TRY
    {
        // There is a case where metadata was unloaded and the AppendType call will fail.
        // This is when an AppDomain has been unloaded but not yet collected.

        ULONG32 outWstrLen = outBufferSize/sizeof(WCHAR);
        PEFile *pPEFile = pMT->GetModule()->GetFile();
        if (pPEFile->GetNativeImage() == NULL && pPEFile->GetILimage() == NULL)
        {
            wcsncpy_s((WCHAR*)outBuffer, outWstrLen, L"<Unloaded Type>", _TRUNCATE);
        }
        else
        {
            StackSString s;
            TypeString::AppendType(s, TypeHandle(pMT), TypeString::FormatNamespace|TypeString::FormatFullInst);
            if (s.IsEmpty())
            {
                return E_OUTOFMEMORY;
            }
            wcsncpy_s((WCHAR*)outBuffer, outWstrLen, s.GetUnicode(), _TRUNCATE);
        }
        outBuffer[outWstrLen - 1] = L'\0';
    }
    EX_CATCH
    {
        hr = E_FAIL;
    }
    EX_END_CATCH(SwallowAllExceptions)

    return hr;
}

HRESULT
ClrDataAccess::RequestManagedStaticAddr(IN ULONG32 inBufferSize,
                                        IN BYTE* inBuffer,
                                        IN ULONG32 outBufferSize,
                                        OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(DacpManagedStaticArgs)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(CLRDATA_ADDRESS)))
    {
        return E_INVALIDARG;
    }

    DacpManagedStaticArgs *pArgs = (DacpManagedStaticArgs *)inBuffer;

    CLRDATA_ADDRESS *pOutAddr = (CLRDATA_ADDRESS *)outBuffer;
    if (pOutAddr == NULL)
    {
        return E_INVALIDARG;
    }

    Thread* thread = PTR_Thread(TO_TADDR(pArgs->ThreadAddr));
    if (!thread)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    // TADDR objPtr = PTR_HOST_TO_TADDR(ObjectFromHandle(thread->m_ExposedObject));
    // TADDR objPtr = PTR_HOST_TO_TADDR(thread->GetExposedObjectRaw());
    OBJECTREF objRef = thread->GetExposedObjectRaw();

    TADDR objPtr = PTR_HOST_TO_TADDR(objRef);

    ULONG32 returned = sizeof(TADDR);
    TADDR BucketArray = NULL;
    if(m_target->ReadVirtual(objPtr+offsetof(ThreadBaseObject,m_ThreadStaticsBuckets),
        (PBYTE)&BucketArray, sizeof(TADDR),&returned)!=S_OK)
    {
        return E_FAIL;
    }

    // Get Address of bucket
    TADDR Bucket = BucketArray+offsetof(PtrArray,m_Array)+(sizeof(size_t)*(pArgs->Slot/THREAD_STATICS_BUCKET_SIZE));

    TADDR Value = NULL;
    if(m_target->ReadVirtual(Bucket,
        (PBYTE)&Value, sizeof(TADDR),&returned)!=S_OK)
    {
        return E_FAIL;
    }

    // Get the final address of our static
    *pOutAddr = Value+offsetof(PtrArray,m_Array)+(sizeof(size_t)*(pArgs->Slot%THREAD_STATICS_BUCKET_SIZE));
    return S_OK;
}

HRESULT
ClrDataAccess::RequestFieldDescData(IN ULONG32 inBufferSize,
                                    IN BYTE* inBuffer,
                                    IN ULONG32 outBufferSize,
                                    OUT BYTE* outBuffer)
{
    HRESULT hr = S_OK;

    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpFieldDescData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    DacpFieldDescData* FieldDescData = (DacpFieldDescData*)outBuffer;
    FieldDesc* pFieldDesc = PTR_FieldDesc(TO_TADDR(addr));

    if (!pFieldDesc)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    FieldDescData->Type = pFieldDesc->GetFieldType();
    FieldDescData->sigType = FieldDescData->Type;

    EX_TRY
    {
        // minidump case, we do not have the field's type's type handle!
        // Strike should be able to form name based on the metadata token in
        // the field desc. Find type is using look up map which is huge. We cannot
        // drag in this data structure in minidump's case.
        //
        TypeHandle th = pFieldDesc->LookupFieldTypeHandle();
        MethodTable *pMt = th.GetMethodTable();
        if (pMt)
        {
            FieldDescData->MTOfType = HOST_CDADDR(th.GetMethodTable());
        }
        else
        {
            FieldDescData->MTOfType = NULL;
        }
    }
    EX_CATCH
    {
        FieldDescData->MTOfType = NULL;
    }
    EX_END_CATCH(SwallowAllExceptions)



    MetaSig tSig(pFieldDesc);
    tSig.NextArg();
    SigPointer sp1 = tSig.GetArgProps();
    CorElementType et;
    IfFailRet(sp1.GetElemType(&et)); // throw away the value, we just need to walk past.
    if (et == ELEMENT_TYPE_CLASS || et == ELEMENT_TYPE_VALUETYPE)   // any other follows token?
    {
        IfFailRet(sp1.GetToken(&(FieldDescData->TokenOfType)));
    }
    else
    {
        // There is no encoded token of field type
        FieldDescData->TokenOfType = mdTypeDefNil;
        if (FieldDescData->MTOfType == NULL)
        {
            // If there is no encoded token (that is, it is primitive type) and no MethodTable for it, remember the
            // element_type from signature
            //
            FieldDescData->sigType = et;
        }
    }

    FieldDescData->ModuleOfType = HOST_CDADDR(pFieldDesc->GetModule());
    FieldDescData->mb = pFieldDesc->GetMemberDef();
    FieldDescData->MTOfEnclosingClass = HOST_CDADDR(pFieldDesc->GetApproxEnclosingMethodTable());
    FieldDescData->dwOffset = pFieldDesc->GetOffset();
    FieldDescData->bIsThreadLocal = pFieldDesc->IsThreadStatic();
    FieldDescData->bIsContextLocal = pFieldDesc->IsContextStatic();;
    FieldDescData->bIsStatic = pFieldDesc->IsStatic();
    FieldDescData->NextField = HOST_CDADDR(PTR_FieldDesc(PTR_HOST_TO_TADDR(pFieldDesc) + sizeof(FieldDesc)));
    return S_OK;
}

HRESULT
ClrDataAccess::RequestEEClassData(IN ULONG32 inBufferSize,
                                  IN BYTE* inBuffer,
                                  IN ULONG32 outBufferSize,
                                  OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpEEClassData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    DacpEEClassData* classData = (DacpEEClassData*)outBuffer;
    EEClass* pClass = PTR_EEClass(TO_TADDR(addr));
    if (!pClass)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    if (!DacValidateEEClass(pClass))
    {
        return E_INVALIDARG;
    }

    classData->MethodTable = HOST_CDADDR(pClass->GetMethodTable());
    classData->Module = HOST_CDADDR(pClass->GetModule());;
    classData->wNumVtableSlots = pClass->GetMethodTable()->GetNumVtableSlots();
    classData->wNumMethodSlots = pClass->GetMethodTable()->GetNumMethods();
    classData->wNumInstanceFields = pClass->GetNumInstanceFields();
    classData->wNumStaticFields = pClass->GetNumStaticFields();
    classData->dwAttrClass = pClass->GetAttrClass(); // cached metadata
    classData->cl = pClass->GetCl(); // Metadata token

    classData->FirstField = (TADDR)(pClass->m_pFieldDescList_UseAccessor);

    BOOL hasThreadOrContextStatics = pClass->GetMethodTable()->HasThreadOrContextStatics();
    classData->wThreadStaticsSize = (hasThreadOrContextStatics) ? pClass->GetMethodTable()->GetThreadStaticsSize() : 0;
    classData->wThreadStaticOffset = (hasThreadOrContextStatics) ? pClass->GetMethodTable()->GetThreadStaticsOffset() : 0;

    classData->wContextStaticsSize = (hasThreadOrContextStatics) ? pClass->GetMethodTable()->GetContextStaticsSize() : 0;
    classData->wContextStaticOffset = (hasThreadOrContextStatics) ? pClass->GetMethodTable()->GetContextStaticsOffset() : 0;
    return S_OK;
}

HRESULT
ClrDataAccess::RequestFrameNameData(IN ULONG32 inBufferSize,
                                 IN BYTE* inBuffer,
                                 IN ULONG32 outBufferSize,
                                 OUT BYTE* outBuffer)
{

    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS vtAddr = *(CLRDATA_ADDRESS*)inBuffer;
    if (!vtAddr)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    PWSTR pszName = DacGetVtNameW(vtAddr);
    if (pszName == NULL)
    {
        return E_INVALIDARG;
    }

    // Turn from bytes to wide characters
    ULONG32 NameLength = outBufferSize / sizeof(WCHAR);
    LPWSTR pwszBuffer = (LPWSTR) outBuffer;

    wcsncpy_s (pwszBuffer, NameLength, pszName, _TRUNCATE);
    pwszBuffer[NameLength - 1] = L'\0';

    return S_OK;
}

HRESULT
ClrDataAccess::RequestPEFileName(IN ULONG32 inBufferSize,
                                 IN BYTE* inBuffer,
                                 IN ULONG32 outBufferSize,
                                 OUT BYTE* outBuffer)
{

    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    PEFile* pPEFile = PTR_PEFile(TO_TADDR(addr));
    if (!pPEFile)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    // Turn from bytes to wide characters
    ULONG32 NameLength = outBufferSize / sizeof(WCHAR);
    LPWSTR pwszBuffer = (LPWSTR) outBuffer;
    pwszBuffer[0] = L'\0';

    if (!pPEFile->GetPath().IsEmpty())
    {
        if (!pPEFile->GetPath().DacGetUnicode(NameLength, pwszBuffer, NULL))
        {
            return E_FAIL;
        }
    }
    else
    {
        if (!pPEFile->IsDynamic())
        {
            PEAssembly *pAssembly = pPEFile->GetAssembly();
            StackSString displayName;
            pAssembly->GetDisplayName(displayName, 0);
            if (!displayName.IsEmpty())
            {
                wcsncpy_s(pwszBuffer, NameLength, displayName.GetUnicode(), _TRUNCATE);
                // If the name
            }
        }
    }

    pwszBuffer[NameLength-1] = L'\0';

    return S_OK;
}

HRESULT
ClrDataAccess::RequestPEFileData(IN ULONG32 inBufferSize,
                                  IN BYTE* inBuffer,
                                  IN ULONG32 outBufferSize,
                                  OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpPEFileData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    DacpPEFileData* pefileData = (DacpPEFileData*)outBuffer;
    PEFile* pPEFile = PTR_PEFile(TO_TADDR(addr));
    if (!pPEFile)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    // More fields later?
    if (pPEFile->HasNativeImage())
    {
        pefileData->Base = TO_CDADDR(pPEFile->GetLoadedNative()->GetBase());
    }
    else if (!pPEFile->IsDynamic())
    {
        pefileData->Base = TO_CDADDR(pPEFile->GetIJWBase());
    }
    else
    {
        pefileData->Base = NULL;
    }
    return S_OK;
}

DWORD DACGetNumComponents(TADDR addr,ICLRDataTarget* target)
{
    // For an object pointer, this attempts to read the number of
    // array components.
    addr+=sizeof(size_t);
    ULONG32 returned = sizeof(DWORD);
    DWORD Value = NULL;
    if(target->ReadVirtual(addr,
                           (PBYTE)&Value, sizeof(DWORD),
                           &returned) != S_OK)
    {
        return 0;
    }
    return Value;
}

HRESULT
ClrDataAccess::RequestObjectData(IN ULONG32 inBufferSize,
                                  IN BYTE* inBuffer,
                                  IN ULONG32 outBufferSize,
                                  OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpObjectData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }


    DacpObjectData* objectData = (DacpObjectData*)outBuffer;
    ZeroMemory (objectData, sizeof(DacpObjectData));
    TADDR mtTADDR = DACGetMethodTableFromObjectPointer(addr,m_target);
    if (mtTADDR==NULL)
    {
        return E_INVALIDARG;
    }
    MethodTable *mt = PTR_MethodTable(mtTADDR);

    BOOL bFree;
    if (!DacValidateMethodTable(mt, bFree))
    {
        return E_INVALIDARG;
    }

    objectData->MethodTable = HOST_CDADDR(mt);
    objectData->EEClass = HOST_CDADDR(mt->GetClass());
    objectData->Size = mt->GetBaseSize();
    if (mt->GetComponentSize())
    {
        objectData->Size += (DACGetNumComponents(addr,m_target) * mt->GetComponentSize());
        objectData->dwComponentSize = mt->GetComponentSize();
    }

    if (bFree)
    {
        objectData->ObjectType = OBJ_FREE;
    }
    else
    {
        if (objectData->MethodTable == HOST_CDADDR(g_pStringClass))
        {
            objectData->ObjectType = OBJ_STRING;
        }
        else if (objectData->MethodTable == HOST_CDADDR(g_pObjectClass))
        {
            objectData->ObjectType = OBJ_OBJECT;
        }
        else if (mt->GetClass()->IsArrayClass())
        {
            objectData->ObjectType = OBJ_ARRAY;

            ArrayBase *pArrayObj = PTR_ArrayBase(TO_TADDR(addr));
            ArrayClass *pArrayClass = PTR_ArrayClass(PTR_HOST_TO_TADDR(mt->GetClass()));
            objectData->ElementType = pArrayClass->GetArrayElementType();

            // The TypeHandle of the elements could either be a MethodTable or an ArrayTypeDesc.
            // If it is the latter (i.e. if IsUnsharedMT() is false), then we need to loop until
            // we get to a MethodTable which we will then validate.
            TypeHandle thElem;
            if (mt->IsSharedByReferenceArrayTypes())
            {
                thElem = *(PTR_TypeHandle(PTR_HOST_TO_TADDR(pArrayObj) + sizeof(*pArrayObj)));
            }
            else
            {
                thElem = pArrayClass->GetApproxArrayElementTypeHandle();
            }
            TypeHandle thCur  = thElem;
            while (!thCur.IsUnsharedMT())
            {
                thCur = thCur.AsArray()->GetArrayElementTypeHandle();
            }
            TADDR mtCurTADDR = thCur.AsTAddr();
            if (!DacValidateMethodTable(PTR_MethodTable(mtCurTADDR), bFree))
            {
                return E_INVALIDARG;
            }
            objectData->ElementTypeHandle = (CLRDATA_ADDRESS)(thElem.AsTAddr());

            objectData->dwRank = pArrayClass->GetRank();
            objectData->dwNumComponents = pArrayObj->GetNumComponents ();
            objectData->ArrayDataPtr = TO_CDADDR(pArrayObj->GetDataPtr (TRUE));
            objectData->ArrayBoundsPtr = HOST_CDADDR(pArrayObj->GetBoundsPtr());
            objectData->ArrayLowerBoundsPtr = HOST_CDADDR(pArrayObj->GetLowerBoundsPtr());
        }
        else
        {
            objectData->ObjectType = OBJ_OTHER;
        }
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestAppDomainList(IN ULONG32 inBufferSize,
                                    IN BYTE* inBuffer,
                                    IN ULONG32 outBufferSize,
                                    OUT BYTE* outBuffer)
{
    if ((inBufferSize != 0) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS *pAddrArray = (CLRDATA_ADDRESS *) outBuffer;
    LONG CountAllowed = outBufferSize / sizeof(CLRDATA_ADDRESS);

    // Walk the list, putting unique pointers into pAddrArray
    AppDomainIterator ai(FALSE);
    int i=0;
    while (ai.Next() && (i < CountAllowed))
    {
        pAddrArray[i++] = HOST_CDADDR(ai.GetDomain());
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestAppDomainStoreData(IN ULONG32 inBufferSize,
                                         IN BYTE* inBuffer,
                                         IN ULONG32 outBufferSize,
                                         OUT BYTE* outBuffer)
{
    if ((inBufferSize != 0) ||
        (outBufferSize != sizeof(DacpAppDomainStoreData)))
    {
        return E_INVALIDARG;
    }

    DacpAppDomainStoreData* adsData = (DacpAppDomainStoreData*)outBuffer;

    adsData->systemDomain = HOST_CDADDR(SystemDomain::System());
    adsData->sharedDomain = HOST_CDADDR(SharedDomain::GetDomain());

    // Get an accurate count of appdomains.
    adsData->DomainCount = 0;
    AppDomainIterator ai(FALSE);
    while (ai.Next())
    {
        adsData->DomainCount++;
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestAppDomainData(IN ULONG32 inBufferSize,
                                    IN BYTE* inBuffer,
                                    IN ULONG32 outBufferSize,
                                    OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpAppDomainData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    DacpAppDomainData* appdomainData = (DacpAppDomainData*)outBuffer;
    AppDomain* pAppDomain = PTR_AppDomain(TO_TADDR(addr));
    if (!pAppDomain)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    ZeroMemory(appdomainData,sizeof(DacpAppDomainData));
    appdomainData->AppDomainPtr = HOST_CDADDR(pAppDomain);
    appdomainData->AppSecDesc = HOST_CDADDR(pAppDomain->GetSecurityDescriptor());
    appdomainData->pHighFrequencyHeap = HOST_CDADDR(pAppDomain->GetHighFrequencyHeap());
    appdomainData->pLowFrequencyHeap = HOST_CDADDR(pAppDomain->GetLowFrequencyHeap());
    appdomainData->pStubHeap = HOST_CDADDR(pAppDomain->GetStubHeap());
    appdomainData->appDomainStage = STAGE_OPEN;

    if (PTR_BaseDomain(pAppDomain) == PTR_BaseDomain(SharedDomain::GetDomain()))
    {
        SharedDomain::SharedAssemblyIterator i;
        while (i.Next())
        {
            appdomainData->AssemblyCount++;
        }
    }
    else if (PTR_BaseDomain(pAppDomain) != PTR_BaseDomain(SystemDomain::System()))
    {
        appdomainData->DomainLocalBlock = appdomainData->AppDomainPtr +
            offsetof(AppDomain, m_sDomainLocalBlock);
        appdomainData->pDomainLocalModules = PTR_CDADDR(pAppDomain->m_sDomainLocalBlock.m_pModuleSlots);

        appdomainData->dwId = pAppDomain->GetId().m_dwId;
        appdomainData->appDomainStage = (DacpAppDomainDataStage) pAppDomain->m_Stage;
        if (pAppDomain->IsActive())
        {
            // The assembly list is not valid in a closed appdomain.
            AppDomain::AssemblyIterator i = pAppDomain->IterateAssembliesEx((AssemblyIterationFlags)(kIncludeLoading | kIncludeLoaded | kIncludeExecution));
            while (i.Next())
            {
                DomainAssembly *pDA = i.GetDomainAssembly();
                if (pDA->IsLoaded())
                {
                    appdomainData->AssemblyCount++;
                }
            }

            AppDomain::FailedAssemblyIterator j = pAppDomain->IterateFailedAssembliesEx();
            while (j.Next())
            {
                appdomainData->FailedAssemblyCount++;
            }
        }
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestAssemblyList(IN ULONG32 inBufferSize,
                                   IN BYTE* inBuffer,
                                   IN ULONG32 outBufferSize,
                                   OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    AppDomain* pAppDomain = PTR_AppDomain(TO_TADDR(addr));
    if (!pAppDomain)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS *pAddrArray = (CLRDATA_ADDRESS *) outBuffer;
    LONG CountAllowed = outBufferSize / sizeof(CLRDATA_ADDRESS);

    int n=0;
    if (PTR_BaseDomain(pAppDomain) ==
        PTR_BaseDomain(SharedDomain::GetDomain()))
    {
        SharedDomain::SharedAssemblyIterator i;
        while (i.Next() && n<CountAllowed)
        {
            pAddrArray[n++] = HOST_CDADDR(i.GetAssembly());
        }
    }
    else
    {
        AppDomain::AssemblyIterator i = pAppDomain->IterateAssembliesEx((AssemblyIterationFlags)(kIncludeLoading | kIncludeLoaded | kIncludeExecution));
        while (i.Next() && n<CountAllowed)
        {
            DomainAssembly *pDA = i.GetDomainAssembly();
            if (pDA->IsLoaded())
            {
                pAddrArray[n++] = HOST_CDADDR(i.GetAssembly());
            }
        }
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestFailedAssemblyList(IN ULONG32 inBufferSize,
                                         IN BYTE* inBuffer,
                                         IN ULONG32 outBufferSize,
                                         OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    AppDomain* pAppDomain = PTR_AppDomain(TO_TADDR(addr));
    if (!pAppDomain)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS *pAddrArray = (CLRDATA_ADDRESS *) outBuffer;
    LONG CountAllowed = outBufferSize / sizeof(CLRDATA_ADDRESS);

    int n=0;
    AppDomain::FailedAssemblyIterator i = pAppDomain->IterateFailedAssembliesEx();
    while (i.Next() && n<=CountAllowed)
    {
        pAddrArray[n++] = HOST_CDADDR(i.GetFailedAssembly());
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestAppDomainName(IN ULONG32 inBufferSize,
                                  IN BYTE* inBuffer,
                                  IN ULONG32 outBufferSize,
                                  OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    AppDomain* pAppDomain = PTR_AppDomain(TO_TADDR(addr));

    // Turn from bytes to wide characters
    ULONG32 NameLength = outBufferSize / sizeof(WCHAR);
    LPWSTR pwszBuffer = (LPWSTR) outBuffer;
    pwszBuffer[0] = L'\0';

    if ((PTR_BaseDomain(pAppDomain) == PTR_BaseDomain(SharedDomain::GetDomain())) ||
        (PTR_BaseDomain(pAppDomain) == PTR_BaseDomain(SystemDomain::System())))
    {
        // Shared domain and SystemDomain don't have this field.
        return S_OK;
    }

    if (!pAppDomain->m_friendlyName.IsEmpty())
    {
        if (!pAppDomain->m_friendlyName.
            DacGetUnicode(NameLength, pwszBuffer, NULL))
        {
            return E_FAIL;
        }
    }
    pwszBuffer[NameLength-1] = L'\0';
    return S_OK;
}

HRESULT
ClrDataAccess::RequestAppDomainAppBase(IN ULONG32 inBufferSize,
                                       IN BYTE* inBuffer,
                                       IN ULONG32 outBufferSize,
                                       OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    AppDomain* pAppDomain = PTR_AppDomain(TO_TADDR(addr));

    // Turn from bytes to wide characters
    ULONG32 NameLength = outBufferSize / sizeof(WCHAR);
    LPWSTR pwszBuffer = (LPWSTR) outBuffer;
    pwszBuffer[0] = L'\0';

    if ((PTR_BaseDomain(pAppDomain) == PTR_BaseDomain(SharedDomain::GetDomain())) ||
        (PTR_BaseDomain(pAppDomain) == PTR_BaseDomain(SystemDomain::System())))
    {
        // Shared domain and SystemDomain don't have this field.
        return S_OK;
    }

    if (!pAppDomain->m_applicationBase.IsEmpty())
    {
        if (!pAppDomain->m_applicationBase.
            DacGetUnicode(NameLength, pwszBuffer, NULL))
        {
            return E_FAIL;
        }
    }
    pwszBuffer[NameLength-1] = L'\0';
    return S_OK;
}

HRESULT
ClrDataAccess::RequestAppDomainPrivateBinPaths(IN ULONG32 inBufferSize,
                                               IN BYTE* inBuffer,
                                               IN ULONG32 outBufferSize,
                                               OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    AppDomain* pAppDomain = PTR_AppDomain(TO_TADDR(addr));

    // Turn from bytes to wide characters
    ULONG32 NameLength = outBufferSize / sizeof(WCHAR);
    LPWSTR pwszBuffer = (LPWSTR) outBuffer;
    pwszBuffer[0] = L'\0';

    if ((PTR_BaseDomain(pAppDomain) == PTR_BaseDomain(SharedDomain::GetDomain())) ||
        (PTR_BaseDomain(pAppDomain) == PTR_BaseDomain(SystemDomain::System())))
    {
        // Shared domain and SystemDomain don't have this field.
        return S_OK;
    }

    if (!pAppDomain->m_privateBinPaths.IsEmpty())
    {
        if (!pAppDomain->m_privateBinPaths.
            DacGetUnicode(NameLength, pwszBuffer, NULL))
        {
            return E_FAIL;
        }
    }
    pwszBuffer[NameLength-1] = L'\0';
    return S_OK;
}

HRESULT
ClrDataAccess::RequestAppDomainConfigFile(IN ULONG32 inBufferSize,
                                          IN BYTE* inBuffer,
                                          IN ULONG32 outBufferSize,
                                          OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    AppDomain* pAppDomain = PTR_AppDomain(TO_TADDR(addr));

    // Turn from bytes to wide characters
    ULONG32 NameLength = outBufferSize / sizeof(WCHAR);
    LPWSTR pwszBuffer = (LPWSTR) outBuffer;
    pwszBuffer[0] = L'\0';

    if ((PTR_BaseDomain(pAppDomain) == PTR_BaseDomain(SharedDomain::GetDomain())) ||
        (PTR_BaseDomain(pAppDomain) == PTR_BaseDomain(SystemDomain::System())))
    {
        // Shared domain and SystemDomain don't have this field.
        return S_OK;
    }

    if (!pAppDomain->m_configFile.IsEmpty())
    {
        if (!pAppDomain->m_configFile.
            DacGetUnicode(NameLength, pwszBuffer, NULL))
        {
            return E_FAIL;
        }
    }
    pwszBuffer[NameLength-1] = L'\0';
    return S_OK;
}

HRESULT
ClrDataAccess::RequestAssemblyData(IN ULONG32 inBufferSize,
                                   IN BYTE* inBuffer,
                                   IN ULONG32 outBufferSize,
                                   OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpAssemblyData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    DacpAssemblyData* assemblyData = (DacpAssemblyData*)outBuffer;
    Assembly* pAssembly = PTR_Assembly(TO_TADDR(addr));
    if (!pAssembly)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    
    AppDomain* pDomain = NULL;
    if (assemblyData->AppDomainPtr != NULL)
        pDomain = PTR_AppDomain(TO_TADDR(assemblyData->AppDomainPtr));

    assemblyData->AssemblyPtr = HOST_CDADDR(pAssembly);
    assemblyData->ClassLoader = HOST_CDADDR(pAssembly->GetLoader());
    assemblyData->ParentDomain = HOST_CDADDR(pAssembly->GetDomain());
    if (pDomain != NULL)
        assemblyData->AssemblySecDesc = HOST_CDADDR(pAssembly->GetSecurityDescriptor(pDomain));
    assemblyData->isDynamic = pAssembly->IsDynamic();
    assemblyData->ModuleCount = 0;
    assemblyData->isDomainNeutral = pAssembly->IsDomainNeutral();

    if (pAssembly->GetManifestFile())
    {
        assemblyData->LoadContext = pAssembly->GetManifestFile()->GetLoadContext();
        assemblyData->dwLocationFlags = pAssembly->GetManifestFile()->GetLocationFlags();
    }

    ModuleIterator mi = pAssembly->IterateModules();
    while (mi.Next())
    {
        assemblyData->ModuleCount++;
    }
    return S_OK;
}

HRESULT
ClrDataAccess::RequestAssemblyName(IN ULONG32 inBufferSize,
                                   IN BYTE* inBuffer,
                                   IN ULONG32 outBufferSize,
                                   OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    Assembly* pAssembly = PTR_Assembly(TO_TADDR(addr));
    if (!pAssembly)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    // Turn from bytes to wide characters
    ULONG32 NameLength = outBufferSize / sizeof(WCHAR);
    LPWSTR pwszBuffer = (LPWSTR) outBuffer;
    pwszBuffer[0] = L'\0';

    if (!pAssembly->GetManifestFile()->GetPath().IsEmpty())
    {
        if (!pAssembly->GetManifestFile()->GetPath().
            DacGetUnicode(NameLength, pwszBuffer, NULL))
        {
            return E_FAIL;
        }
    }
    else
    {
        if (!pAssembly->GetManifestFile()->IsDynamic())
        {
            StackSString displayName;
            pAssembly->GetManifestFile()->GetDisplayName(displayName, 0);
            if (!displayName.IsEmpty())
            {
                wcsncpy_s(pwszBuffer, NameLength, displayName.GetUnicode(), _TRUNCATE);
            }
        }
    }

    pwszBuffer[NameLength-1] = L'\0';
    return S_OK;
}

HRESULT
ClrDataAccess::RequestAssemblyLocation(IN ULONG32 inBufferSize,
                                       IN BYTE* inBuffer,
                                       IN ULONG32 outBufferSize,
                                       OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    Assembly* pAssembly = PTR_Assembly(TO_TADDR(addr));
    if (!pAssembly)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    // Turn from bytes to wide characters
    ULONG32 NameLength = outBufferSize / sizeof(WCHAR);
    LPWSTR pwszBuffer = (LPWSTR) outBuffer;
    pwszBuffer[0] = L'\0';

    if (!pAssembly->GetManifestFile()->GetPath().IsEmpty())
    {
        if (!pAssembly->GetManifestFile()->GetPath().
            DacGetUnicode(NameLength, pwszBuffer, NULL))
        {
            return E_FAIL;
        }
    }

    pwszBuffer[NameLength-1] = L'\0';
    return S_OK;
}

HRESULT
ClrDataAccess::RequestAssemblyDisplayName(IN ULONG32 inBufferSize,
                                          IN BYTE* inBuffer,
                                          IN ULONG32 outBufferSize,
                                          OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    Assembly* pAssembly = PTR_Assembly(TO_TADDR(addr));
    if (!pAssembly)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    // Turn from bytes to wide characters
    ULONG32 NameLength = outBufferSize / sizeof(WCHAR);
    LPWSTR pwszBuffer = (LPWSTR) outBuffer;
    pwszBuffer[0] = L'\0';

    StackSString displayName;
    pAssembly->GetManifestFile()->GetDisplayName(displayName, 0);
    if (!displayName.IsEmpty())
    {
        wcsncpy_s(pwszBuffer, NameLength, displayName.GetUnicode(), _TRUNCATE);
    }

    pwszBuffer[NameLength-1] = L'\0';
    return S_OK;
}

HRESULT
ClrDataAccess::RequestAssemblyModuleList(IN ULONG32 inBufferSize,
                                         IN BYTE* inBuffer,
                                         IN ULONG32 outBufferSize,
                                         OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    Assembly* pAssembly = PTR_Assembly(TO_TADDR(addr));
    if (!pAssembly)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS *pAddrArray = (CLRDATA_ADDRESS *) outBuffer;
    LONG CountAllowed = outBufferSize / sizeof(CLRDATA_ADDRESS);

    ModuleIterator mi = pAssembly->IterateModules();
    LONG n = 0;
    while (mi.Next() && n<=CountAllowed)
    {
        pAddrArray[n++] = HOST_CDADDR(mi.GetModule());
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestFailedAssemblyData(IN ULONG32 inBufferSize,
                                         IN BYTE* inBuffer,
                                         IN ULONG32 outBufferSize,
                                         OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpFailedAssemblyData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    DacpFailedAssemblyData* assemblyData = (DacpFailedAssemblyData*)outBuffer;
    FailedAssembly* pAssembly = PTR_FailedAssembly(TO_TADDR(addr));
    if (!pAssembly)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    assemblyData->AssemblyPtr = HOST_CDADDR(pAssembly);
    assemblyData->LoadContext = pAssembly->context;
    assemblyData->hResult = pAssembly->error;

    return S_OK;
}

HRESULT
ClrDataAccess::RequestFailedAssemblyLocation(IN ULONG32 inBufferSize,
                                             IN BYTE* inBuffer,
                                             IN ULONG32 outBufferSize,
                                             OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    FailedAssembly* pAssembly = PTR_FailedAssembly(TO_TADDR(addr));
    if (!pAssembly)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    // Turn from bytes to wide characters
    ULONG32 NameLength = outBufferSize / sizeof(WCHAR);
    LPWSTR pwszBuffer = (LPWSTR) outBuffer;
    pwszBuffer[0] = L'\0';

    if (!pAssembly->location.IsEmpty())
    {
        if (!pAssembly->location.
            DacGetUnicode(NameLength, pwszBuffer, NULL))
        {
            return E_FAIL;
        }
    }

    pwszBuffer[NameLength-1] = L'\0';
    return S_OK;
}

HRESULT
ClrDataAccess::RequestFailedAssemblyDisplayName(IN ULONG32 inBufferSize,
                                                IN BYTE* inBuffer,
                                                IN ULONG32 outBufferSize,
                                                OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    FailedAssembly* pAssembly = PTR_FailedAssembly(TO_TADDR(addr));
    if (!pAssembly)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    // Turn from bytes to wide characters
    ULONG32 NameLength = outBufferSize / sizeof(WCHAR);
    LPWSTR pwszBuffer = (LPWSTR) outBuffer;
    pwszBuffer[0] = L'\0';

    if (!pAssembly->displayName.IsEmpty())
    {
        if (!pAssembly->displayName.
            DacGetUnicode(NameLength, pwszBuffer, NULL))
        {
            return E_FAIL;
        }
    }

    pwszBuffer[NameLength-1] = L'\0';
    return S_OK;
}

HRESULT
ClrDataAccess::RequestGCHeapDetails(IN ULONG32 inBufferSize,
                                  IN BYTE* inBuffer,
                                  IN ULONG32 outBufferSize,
                                  OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpGcHeapDetails)))
    {
        return E_INVALIDARG;
    }

#ifndef GC_SMP
    DacpGcHeapDetails* detailsData = (DacpGcHeapDetails*)outBuffer;
#endif
    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    if (!GCHeap::IsServerHeap())
    {
        return E_FAIL; // doesn't make sense to call this on WKS mode
    }

#ifdef GC_SMP
    _ASSERTE(0);
#else // !GC_SMP
    HRESULT hr = ServerGCHeapDetails(addr, detailsData);
    if (FAILED(hr))
    {
        return hr;
    }
#endif // !GC_SMP

    return S_OK;
}

HRESULT
ClrDataAccess::RequestGCHeapDetailsStatic(IN ULONG32 inBufferSize,
                                  IN BYTE* inBuffer,
                                  IN ULONG32 outBufferSize,
                                  OUT BYTE* outBuffer)
{
#ifndef GC_SMP
    if ((inBufferSize != 0) ||
        (inBuffer != NULL) ||
        (outBufferSize != sizeof(DacpGcHeapDetails)))
    {
        return E_INVALIDARG;
    }

    int i;

    DacpGcHeapDetails* detailsData = (DacpGcHeapDetails*)outBuffer;
    detailsData->lowest_address = PTR_CDADDR(g_lowest_address);
    detailsData->highest_address = PTR_CDADDR(g_highest_address);
    detailsData->card_table = PTR_CDADDR(g_card_table);

    detailsData->heapAddr = NULL;

    detailsData->alloc_allocated = PTR_CDADDR(WKS::gc_heap::alloc_allocated);
    detailsData->ephemeral_heap_segment = PTR_CDADDR(WKS::gc_heap::ephemeral_heap_segment);
    for (i=0;i<NUMBERGENERATIONS;i++)
    {
        detailsData->generation_table[i].start_segment = (CLRDATA_ADDRESS)(ULONG_PTR) WKS::generation_table[i].start_segment;
        detailsData->generation_table[i].allocation_start = (CLRDATA_ADDRESS)(ULONG_PTR) WKS::generation_table[i].allocation_start;
        detailsData->generation_table[i].allocContextPtr = (CLRDATA_ADDRESS)(ULONG_PTR) WKS::generation_table[i].allocation_context.alloc_ptr;
        detailsData->generation_table[i].allocContextLimit = (CLRDATA_ADDRESS)(ULONG_PTR) WKS::generation_table[i].allocation_context.alloc_limit;
    }

    TADDR pFillPointerArray = TO_TADDR(WKS::gc_heap::finalize_queue.GetAddr()) + offsetof(WKS::CFinalize,m_FillPointers);
    for(i=0;i<(NUMBERGENERATIONS+WKS::CFinalize::ExtraSegCount);i++)
    {
        ULONG32 returned = sizeof(size_t);
        size_t pValue;
        m_target->ReadVirtual(pFillPointerArray+(i*sizeof(size_t)), (PBYTE)&pValue, sizeof(size_t),&returned);
        if (returned != sizeof(size_t))
        {
            return E_FAIL;
        }

        detailsData->finalization_fill_pointers[i] = (CLRDATA_ADDRESS) pValue;
    }
    return S_OK;
#else // GC_SMP
    return E_NOTIMPL;
#endif // GC_SMP
}

HRESULT
ClrDataAccess::RequestGCHeapSegment(IN ULONG32 inBufferSize,
                                    IN BYTE* inBuffer,
                                    IN ULONG32 outBufferSize,
                                    OUT BYTE* outBuffer)
{
    HRESULT status = E_FAIL;

    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpHeapSegmentData)))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;
    if (addr == NULL)
    {
        return E_INVALIDARG;
    }

    DacpHeapSegmentData* heapSegment = (DacpHeapSegmentData*)outBuffer;

    if (GCHeap::IsServerHeap())
    {
#ifdef GC_SMP
        _ASSERTE(0);
#else // !GC_SMP
        status = GetServerHeapData(addr, heapSegment);
#endif // !GC_SMP
    }
    else
    {
        WKS::heap_segment *pSegment =
            __DPtr<WKS::heap_segment>(TO_TADDR(addr));
        if (!pSegment)
        {
            // PREfix.
            return E_INVALIDARG;
        }

        heapSegment->segmentAddr = addr;
        heapSegment->allocated = (CLRDATA_ADDRESS)(ULONG_PTR) pSegment->allocated;
        heapSegment->committed = (CLRDATA_ADDRESS)(ULONG_PTR) pSegment->committed;
        heapSegment->reserved = (CLRDATA_ADDRESS)(ULONG_PTR) pSegment->reserved;
        heapSegment->used = (CLRDATA_ADDRESS)(ULONG_PTR) pSegment->used;
        heapSegment->mem = (CLRDATA_ADDRESS)(ULONG_PTR) pSegment->mem;
        heapSegment->next = (CLRDATA_ADDRESS)(ULONG_PTR) pSegment->next;
        heapSegment->gc_heap = NULL;
        status = S_OK;
    }
    return status;
}

HRESULT
ClrDataAccess::RequestGCHeapList(IN ULONG32 inBufferSize,
                                  IN BYTE* inBuffer,
                                  IN ULONG32 outBufferSize,
                                  OUT BYTE* outBuffer)
{

    if ((inBufferSize != 0) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

#ifndef GC_SMP
    CLRDATA_ADDRESS *pAddrArray = (CLRDATA_ADDRESS *) outBuffer;
    LONG CountAllowed = outBufferSize / sizeof(CLRDATA_ADDRESS);
#endif // !GC_SMP

    if (GCHeap::IsServerHeap())
    {
#ifdef GC_SMP
        _ASSERTE(0);
#else // !GC_SMP
        if (CountAllowed != GCHeapCount())
        {
            return E_INVALIDARG;
        }
        HRESULT hr = GetServerHeaps(pAddrArray,m_target);
        if (FAILED(hr))
        {
            return hr;
        }
#endif // !GC_SMP
    }
    else
    {
        return E_FAIL; // doesn't make sense to call this on WKS mode
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestGCHeapData(IN ULONG32 inBufferSize,
                                  IN BYTE* inBuffer,
                                  IN ULONG32 outBufferSize,
                                  OUT BYTE* outBuffer)
{
    if ((inBufferSize != 0) ||
        (inBuffer != NULL) ||
        (outBufferSize != sizeof(DacpGcHeapData)))
    {
        return E_INVALIDARG;
    }

    TADDR gcHeapLocation = g_pGCHeap.GetAddrRaw ();
    size_t gcHeapValue = 0;
    ULONG32 returned = sizeof(gcHeapValue);
    m_target->ReadVirtual (gcHeapLocation, (PBYTE)&gcHeapValue, sizeof (gcHeapValue), &returned);
    if (gcHeapValue == 0)
        return E_FAIL;

    DacpGcHeapData* gcheapData = (DacpGcHeapData*)outBuffer;
    gcheapData->g_max_generation = GCHeap::GetMaxGeneration();
    gcheapData->bServerMode = GCHeap::IsServerHeap();
    gcheapData->bGcStructuresValid = CNameSpace::GetGcRuntimeStructuresValid();
    if (GCHeap::IsServerHeap())
    {
#ifdef GC_SMP
        _ASSERTE(0);
        gcheapData->HeapCount = 1;
#else // !GC_SMP
        gcheapData->HeapCount = GCHeapCount();
#endif // !GC_SMP
    }
    else
    {
        gcheapData->HeapCount = 1;
    }
    return S_OK;
}

HRESULT
ClrDataAccess::RequestIsStub(IN ULONG32 inBufferSize,
                             IN BYTE* inBuffer,
                             IN ULONG32 outBufferSize,
                             OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpIsStub)))
    {
        return E_INVALIDARG;
    }

    DacpIsStub* pData = reinterpret_cast<DacpIsStub*> (outBuffer);
    CLRDATA_ADDRESS addr = * reinterpret_cast<CLRDATA_ADDRESS *> (inBuffer);
    TADDR ip = TO_TADDR(addr);

    if (ip == NULL)
    {
        pData->fIsStub = FALSE;
    }
    else
    {
        pData->fIsStub = StubManager::IsStub(ip);
    }

    // If it's in Mscorwks, count that as a stub too.
    if (pData->fIsStub == FALSE)
    {
        _ASSERTE(0); // I wanna see if this gets hit
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestUsefulGlobals(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{

    if ((inBufferSize != 0) ||
        (inBuffer != NULL) ||
        (outBufferSize != sizeof(DacpUsefulGlobalsData)))
    {
        return E_INVALIDARG;
    }

    DacpUsefulGlobalsData* globalsData = (DacpUsefulGlobalsData*)outBuffer;
    globalsData->ArrayMethodTable = HOST_CDADDR(g_pPredefinedArrayTypes[ELEMENT_TYPE_OBJECT]->GetMethodTable());
    globalsData->StringMethodTable = HOST_CDADDR(g_pStringClass);
    globalsData->ObjectMethodTable = HOST_CDADDR(g_pObjectClass);
    globalsData->ExceptionMethodTable = HOST_CDADDR(g_pExceptionClass);
    globalsData->FreeMethodTable = HOST_CDADDR(g_pFreeObjectMethodTable);
    return S_OK;
}


HRESULT
ClrDataAccess::RequestNestedException(IN ULONG32 inBufferSize,
                                      IN BYTE* inBuffer,
                                      IN ULONG32 outBufferSize,
                                      OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpNestedExceptionInfo)))
    {
        return E_INVALIDARG;
    }

    DacpNestedExceptionInfo* nestedInfo =
        reinterpret_cast<DacpNestedExceptionInfo*> (outBuffer);
    ZeroMemory(nestedInfo,sizeof(DacpNestedExceptionInfo));

    CLRDATA_ADDRESS addr = * reinterpret_cast<CLRDATA_ADDRESS *> (inBuffer);
    ExInfo *pExData = PTR_ExInfo(TO_TADDR(addr));

    if (!pExData)
    {
        return E_INVALIDARG;
    }

    nestedInfo->exceptionObject = TO_CDADDR(*PTR_TADDR(pExData->m_hThrowable));
    nestedInfo->nextNestedException = PTR_HOST_TO_TADDR(pExData->m_pPrevNestedInfo);
    return S_OK;
}


HRESULT
ClrDataAccess::RequestDomainLocalModuleData(IN ULONG32 inBufferSize,
                                            IN BYTE* inBuffer,
                                            IN ULONG32 outBufferSize,
                                            OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpDomainLocalModuleData)))
    {
        return E_INVALIDARG;
    }

    DacpDomainLocalModuleData* pLocalModuleData =
        reinterpret_cast<DacpDomainLocalModuleData*> (outBuffer);
    CLRDATA_ADDRESS addr = * reinterpret_cast<CLRDATA_ADDRESS *> (inBuffer);

    DomainLocalModule* pLocalModule = PTR_DomainLocalModule(TO_TADDR(addr));
    if (!pLocalModule)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    pLocalModuleData->pGCStaticDataStart    = TO_CDADDR(pLocalModule->GetPrecomputedGCStaticsBasePointer());
    pLocalModuleData->pNonGCStaticDataStart = TO_CDADDR(pLocalModule->GetPrecomputedNonGCStaticsBasePointer());
    pLocalModuleData->pDynamicClassTable    = PTR_CDADDR(pLocalModule->m_pDynamicClassTable);
    pLocalModuleData->pClassData            = (TADDR) (PTR_HOST_MEMBER_TADDR(DomainLocalModule, pLocalModule, m_pDataBlob));

    return S_OK;
}


HRESULT
ClrDataAccess::RequestDomainLocalModuleFromModule(IN ULONG32 inBufferSize,
                                                  IN BYTE* inBuffer,
                                                  IN ULONG32 outBufferSize,
                                                  OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpDomainLocalModuleData)))
    {
        return E_INVALIDARG;
    }

    DacpDomainLocalModuleData* pLocalModuleData =
        reinterpret_cast<DacpDomainLocalModuleData*> (outBuffer);
    CLRDATA_ADDRESS addr = * reinterpret_cast<CLRDATA_ADDRESS *> (inBuffer);
    Module* pModule = PTR_Module(TO_TADDR(addr));
    if (!pModule)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    DomainLocalModule* pLocalModule =
        PTR_DomainLocalModule(pModule->
                              GetDomainLocalModule(DacGetAppDomain()));
    if (!pLocalModule)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    pLocalModuleData->pGCStaticDataStart    = TO_CDADDR(pLocalModule->GetPrecomputedGCStaticsBasePointer());
    pLocalModuleData->pNonGCStaticDataStart = TO_CDADDR(pLocalModule->GetPrecomputedNonGCStaticsBasePointer());
    pLocalModuleData->pDynamicClassTable    = PTR_CDADDR(pLocalModule->m_pDynamicClassTable);
    pLocalModuleData->pClassData            = (TADDR) (PTR_HOST_MEMBER_TADDR(DomainLocalModule, pLocalModule, m_pDataBlob));

    return S_OK;
}

HRESULT
ClrDataAccess::RequestDomainLocalModuleFromAppDomainData(IN ULONG32 inBufferSize,
                             IN BYTE* inBuffer,
                             IN ULONG32 outBufferSize,
                             OUT BYTE* outBuffer)
{
    if ((inBufferSize != 0) ||
        (inBuffer != NULL) ||
        (outBufferSize != sizeof(DacpDomainLocalModuleData)))
    {
        return E_INVALIDARG;
    }

    DacpDomainLocalModuleData* pLocalModuleData = reinterpret_cast<DacpDomainLocalModuleData*> (outBuffer);
    CLRDATA_ADDRESS AppDomainAddr = pLocalModuleData->appDomainAddr;

    AppDomain *pAppDomain = PTR_AppDomain(TO_TADDR(AppDomainAddr));
    DomainLocalModule* pLocalModule =
        pAppDomain->GetDomainLocalBlock()->
        GetModuleSlot(pLocalModuleData->ModuleID);
    if (!pLocalModule)
    {
        // PREfix.
        return E_INVALIDARG;
    }

    pLocalModuleData->pGCStaticDataStart    = TO_CDADDR(pLocalModule->GetPrecomputedGCStaticsBasePointer());
    pLocalModuleData->pNonGCStaticDataStart = TO_CDADDR(pLocalModule->GetPrecomputedNonGCStaticsBasePointer());
    pLocalModuleData->pDynamicClassTable    = PTR_CDADDR(pLocalModule->m_pDynamicClassTable);
    pLocalModuleData->pClassData            = (TADDR) (PTR_HOST_MEMBER_TADDR(DomainLocalModule, pLocalModule, m_pDataBlob));

    return S_OK;
}

HRESULT ClrDataAccess::VisitOneHandleTable(VISITHANDLE pFunc,LPVOID token,HHANDLETABLE Table)
{
    size_t buffer[HANDLE_HANDLES_PER_BLOCK];
    HandleTable *pTable = (HandleTable *) Table;
    PTR__TableSegmentHeader pCurrentSegment = PTR__TableSegmentHeader(pTable->pSegmentList);
    TADDR BlockRead = PTR_TO_TADDR(pCurrentSegment) + offsetof(TableSegment,rgValue);

    while (pCurrentSegment)
    {
        for (int i=0;i<pCurrentSegment->bEmptyLine;i++)
        {
            int start=i*HANDLE_HANDLES_PER_BLOCK;
            // Read one block at a time.
            DacReadAll(BlockRead+start*sizeof(size_t),buffer,
                       HANDLE_HANDLES_PER_BLOCK*sizeof(size_t),true);

            for (int j = 0; j < HANDLE_HANDLES_PER_BLOCK; j ++)
            {
                if (buffer[j] != NULL)
                {
                    AppDomain *pDomain = SystemDomain::GetAppDomainAtIndex(pTable->uADIndex);
                    if ((pFunc)(BlockRead+((start+j)*sizeof(size_t)),
                                buffer[j],
                                pCurrentSegment->rgBlockType[i],
                                PTR_HOST_TO_TADDR(pDomain),
                                token) == FALSE)
                    {
                        // abort the traverse
                        return E_ABORT;
                    }
                }
            }
        }
        pCurrentSegment = PTR__TableSegmentHeader(pCurrentSegment->pNextSegment);
    }
    return S_OK;
}

HRESULT
ClrDataAccess::RequestHandleTableTraverse(IN ULONG32 inBufferSize,
                             IN BYTE* inBuffer,
                             IN ULONG32 outBufferSize,
                             OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(DacpHandleTableTraverseArgs)) ||
        (inBuffer == NULL) ||
        (outBufferSize != 0))
    {
        return E_INVALIDARG;
    }

    DacpHandleTableTraverseArgs *pArgs = (DacpHandleTableTraverseArgs *) inBuffer;
    VISITHANDLE pFunc = pArgs->Callback;

    int NumSlots = 1;
    if (GCHeap::IsServerHeap())
    {
#ifdef GC_SMP
        _ASSERTE(0);
#else // !GC_SMP
        NumSlots = GCHeapCount();
#endif // !GC_SMP
    }

    HandleTableMap *pWalk = (HandleTableMap *)&g_HandleTableMap;
    while(pWalk)
    {
        for(int i=0;i<INITIAL_HANDLE_TABLE_ARRAY_SIZE;i++)
        {
            HandleTableBucket *pBucket = PTR_HandleTableBucket(pWalk->pBuckets[i]);
            if (pBucket)
            {
                TADDR *pTables = (TADDR *)PTR_READ((TADDR)pBucket->pTable,
                                                   sizeof(HandleTable *)*NumSlots);
                for(int j=0;j<NumSlots;j++)
                {
                    HandleTable *pTable = PTR_HandleTable(pTables[j]);
                    HRESULT hRet = VisitOneHandleTable(pFunc,pArgs->token,(HHANDLETABLE) pTable);
                    if (hRet != S_OK)
                    {
                        return hRet;
                    }
                }
            }
        }

        pWalk = pWalk->pNext;
    }


    return S_OK;

}

HRESULT
ClrDataAccess::RequestEHInfoTraverse(IN ULONG32 inBufferSize,
                             IN BYTE* inBuffer,
                             IN ULONG32 outBufferSize,
                             OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(DacpEHInfoTraverseArgs)) ||
        (inBuffer == NULL) ||
        (outBufferSize != 0))
    {
        return E_INVALIDARG;
    }

    DacpEHInfoTraverseArgs *pArgs = (DacpEHInfoTraverseArgs *) inBuffer;
    DUMPEHINFO pFunc = pArgs->Callback;

    IJitManager* jitMan =
        ExecutionManager::FindJitMan(TO_SLOT(TO_TADDR(pArgs->IPAddress)),
                                     IJitManager::ScanNoReaderLock);
    if (!jitMan)
    {
        return E_INVALIDARG;
    }

    MethodDesc* methodDesc;
    METHODTOKEN mToken;
    DWORD codeOffs;

    jitMan->JitCodeToMethodInfo(TO_SLOT(TO_TADDR(pArgs->IPAddress)),
                                &methodDesc, &mToken, &codeOffs,
                                IJitManager::ScanNoReaderLock);
    if (!methodDesc)
    {
        return E_INVALIDARG;
    }

    EH_CLAUSE_ENUMERATOR    EnumState;
    EE_ILEXCEPTION_CLAUSE*  pEHClause        = NULL;
    unsigned                EHCount;

    EHCount = jitMan->InitializeEHEnumeration(mToken, &EnumState);
    for (unsigned i = 0; i < EHCount; i++)
    {
        pEHClause = jitMan->GetNextEHClause(mToken, &EnumState, NULL);

        DACEHInfo deh;
        ZeroMemory(&deh,sizeof(DACEHInfo));

        if (IsFault(pEHClause))
        {
            deh.clauseType = EHFault;
        }
        else if (IsFinally(pEHClause))
        {
            deh.clauseType = EHFinally;
        }
        else if (IsFilterHandler(pEHClause))
        {
            deh.clauseType = EHFilter;
            deh.filterOffset = pEHClause->FilterOffset;
        }
        else if (IsTypedHandler(pEHClause))
        {
            deh.clauseType = EHTyped;
            deh.isCatchAllHandler = (pEHClause->TypeHandle == (void*)(size_t)mdTypeRefNil);
        }
        else
        {
            deh.clauseType = EHUnknown;
        }

        deh.tryStartOffset = pEHClause->TryStartPC;
        deh.tryEndOffset = pEHClause->TryEndPC;
        deh.handlerStartOffset = pEHClause->HandlerStartPC;
        deh.handlerEndOffset = pEHClause->HandlerEndPC;

        deh.isDuplicateClause = IsDuplicateClause(pEHClause);

        if (!(pFunc)(i, EHCount, &deh, pArgs->token))
        {
            // User wants to stop the enumeration
            return E_ABORT;
        }
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestRcwCleanupTraverse(IN ULONG32 inBufferSize,
                             IN BYTE* inBuffer,
                             IN ULONG32 outBufferSize,
                             OUT BYTE* outBuffer)
{
    return E_NOTIMPL;
}

HRESULT
ClrDataAccess::RequestLoaderHeapTraverse(IN ULONG32 inBufferSize,
                             IN BYTE* inBuffer,
                             IN ULONG32 outBufferSize,
                             OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(DacpLoaderHeapTraverseArgs)) ||
        (inBuffer == NULL) ||
        (outBufferSize != 0))
    {
        return E_INVALIDARG;
    }

    DacpLoaderHeapTraverseArgs *pArgs = (DacpLoaderHeapTraverseArgs *)inBuffer;
    VISITHEAP pFunc = pArgs->Callback;
    LoaderHeap *pLoaderHeap = PTR_LoaderHeap(TO_TADDR(pArgs->LoaderHeapAddr));

    PTR_LoaderHeapBlock block = pLoaderHeap->m_pFirstBlock;
    while (block.IsValid())
    {
        TADDR addr = (TADDR)block->pVirtualAddress;
        size_t size = block->dwVirtualSize;

        BOOL bCurrentBlock = (PTR_HOST_TO_TADDR(block) == PTR_HOST_TO_TADDR(pLoaderHeap->m_pCurBlock));
        pFunc(addr,size,bCurrentBlock);

        block = block->pNext;
    }


    return S_OK;
}

HRESULT
ClrDataAccess::RequestVirtCallStubHeapTraverse(IN ULONG32 inBufferSize,
                             IN BYTE* inBuffer,
                             IN ULONG32 outBufferSize,
                             OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(DacpVirtualCallStubHeapTraverseArgs)) ||
        (inBuffer == NULL) ||
        (outBufferSize != 0))
    {
        return E_INVALIDARG;
    }

    DacpVirtualCallStubHeapTraverseArgs *pArgs = (DacpVirtualCallStubHeapTraverseArgs *)inBuffer;
    VISITHEAP pFunc = pArgs->Callback;

    AppDomain* pAppDomain = PTR_AppDomain(TO_TADDR(pArgs->pAppDomain));
    if (!pAppDomain)
    {
        return E_POINTER;
    }

    VirtualCallStubManager *pVcsMgr = PTR_VirtualCallStubManager(TO_TADDR(pAppDomain->GetVirtualCallStubManager()));
    if (!pVcsMgr)
    {
        return E_POINTER;
    }

    LoaderHeap *pLoaderHeap = NULL;
    switch(pArgs->heaptype)
    {
        case IndcellHeap:
            pLoaderHeap = pVcsMgr->indcell_heap;
            break;
        case LookupHeap:
            pLoaderHeap = pVcsMgr->lookup_heap;
            break;
        case ResolveHeap:
            pLoaderHeap = pVcsMgr->resolve_heap;
            break;
        case DispatchHeap:
            pLoaderHeap = pVcsMgr->dispatch_heap;
            break;
        case CacheEntryHeap:
            pLoaderHeap = pVcsMgr->cache_entry_heap;
            break;
        default:
            return E_INVALIDARG;
    }

    PTR_LoaderHeapBlock block = pLoaderHeap->m_pFirstBlock;
    while (block.IsValid())
    {
        TADDR addr = (TADDR)block->pVirtualAddress;
        size_t size = block->dwVirtualSize;

        BOOL bCurrentBlock = (PTR_HOST_TO_TADDR(block) == PTR_HOST_TO_TADDR(pLoaderHeap->m_pCurBlock));
        pFunc(addr,size,bCurrentBlock);

        block = block->pNext;
    }

    return S_OK;
}


HRESULT
ClrDataAccess::RequestSyncBlockData(IN ULONG32 inBufferSize,
                             IN BYTE* inBuffer,
                             IN ULONG32 outBufferSize,
                             OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(UINT)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpSyncBlockData)))
    {
        return E_INVALIDARG;
    }

    DacpSyncBlockData* pSyncBlockData = reinterpret_cast<DacpSyncBlockData*> (outBuffer);
    UINT SBNumber = * reinterpret_cast<UINT *> (inBuffer);

    ZeroMemory(pSyncBlockData,sizeof(DacpSyncBlockData));
    pSyncBlockData->SyncBlockCount = (SyncBlockCache::s_pSyncBlockCache->m_FreeSyncTableIndex) - 1;
    PTR_SyncTableEntry ste = PTR_SyncTableEntry(PTR_HOST_TO_TADDR(g_pSyncTable)+(sizeof(SyncTableEntry) * SBNumber));
    pSyncBlockData->bFree = (((TADDR)ste->m_Object) & 1);

    if (pSyncBlockData->bFree == FALSE)
    {
        pSyncBlockData->Object = (CLRDATA_ADDRESS)(ULONG_PTR) ste->m_Object;

        if (ste->m_SyncBlock != NULL)
        {
            SyncBlock *pBlock = PTR_SyncBlock(ste->m_SyncBlock);
            pSyncBlockData->SyncBlockPointer = HOST_CDADDR(pBlock);

            pSyncBlockData->MonitorHeld = pBlock->m_Monitor.m_MonitorHeld;
            pSyncBlockData->Recursion = pBlock->m_Monitor.m_Recursion;
            pSyncBlockData->HoldingThread = HOST_CDADDR(pBlock->m_Monitor.m_HoldingThread);

            if (pBlock->GetAppDomainIndex().m_dwIndex)
            {
                pSyncBlockData->appDomainPtr = PTR_HOST_TO_TADDR(
                        SystemDomain::TestGetAppDomainAtIndex(pBlock->GetAppDomainIndex()));
            }

            pSyncBlockData->AdditionalThreadCount = 0;

            if (pBlock->m_Link.m_pNext != NULL)
            {
                PTR_SLink pLink = pBlock->m_Link.m_pNext;
                do
                {
                    pSyncBlockData->AdditionalThreadCount++;
                    pLink = pBlock->m_Link.m_pNext;
                }
                while ((pLink != NULL) &&
                    (pSyncBlockData->AdditionalThreadCount < 1000));
            }
        }
    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestSyncBlockCleanupData(IN ULONG32 inBufferSize,
                             IN BYTE* inBuffer,
                             IN ULONG32 outBufferSize,
                             OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpSyncBlockCleanupData)))
    {
        return E_INVALIDARG;
    }

    DacpSyncBlockCleanupData* syncBlockCData = reinterpret_cast<DacpSyncBlockCleanupData*> (outBuffer);
    CLRDATA_ADDRESS syncBlock = * reinterpret_cast<CLRDATA_ADDRESS *> (inBuffer);

    ZeroMemory (syncBlockCData, sizeof(DacpSyncBlockCleanupData));
    SyncBlock *pBlock = NULL;

    if (syncBlock == NULL && SyncBlockCache::s_pSyncBlockCache->m_pCleanupBlockList)
    {
        pBlock = (SyncBlock *) PTR_SyncBlock(
            PTR_HOST_TO_TADDR(SyncBlockCache::s_pSyncBlockCache->m_pCleanupBlockList) - offsetof(SyncBlock, m_Link));
    }
    else
    {
        pBlock = PTR_SyncBlock(TO_TADDR(syncBlock));
    }

    if (pBlock)
    {
        syncBlockCData->SyncBlockPointer = HOST_CDADDR(pBlock);
        if (pBlock->m_Link.m_pNext)
        {
            syncBlockCData->nextSyncBlock = (CLRDATA_ADDRESS)
                (PTR_HOST_TO_TADDR(pBlock->m_Link.m_pNext) - offsetof(SyncBlock, m_Link));
        }

    }

    return S_OK;
}

HRESULT
ClrDataAccess::RequestJitHelperFunctionName(IN ULONG32 inBufferSize,
                             IN BYTE* inBuffer,
                             IN ULONG32 outBufferSize,
                             OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CLRDATA_ADDRESS)) ||
        (inBuffer == NULL) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CLRDATA_ADDRESS addr = *(CLRDATA_ADDRESS*)inBuffer;

    // Turn from bytes to characters
    ULONG32 NameLength = outBufferSize;
    LPSTR pszBuffer = (LPSTR) outBuffer;
    pszBuffer[0] = L'\0';

    enum JitHelpers
    {
#define JITHELPER(code,fn) __##code,
#include <jithelpers.h>

        NUM_HELPERS,
    };

    const static PCSTR s_rgHelperNames[] = {
#define JITHELPER(code,fn) #code,
#include <jithelpers.h>
    };

    for (int i = 0; i < NUM_HELPERS; i++)
    {

        // While this isn't obvious to PREFast given the way _DacGlobals is defined
        // dac__rgJITHelperRVAs is guaranteed to be of size NUM_HELPERS so we can
        // safely disable this warning.        
        ULONG32 rva = g_dacGlobals.dac__rgJITHelperRVAs[i];


        if (ULONG_MAX != rva)
        {
            TADDR CurHelperAddress = DacGlobalBase() + rva;

            if (CurHelperAddress == addr)
            {
                PCSTR pszHelperName = s_rgHelperNames[i];
                ULONG32 cchHelperName = strlen(pszHelperName);

                ULONG32 cchToCopy = min(NameLength-1, cchHelperName);

                CopyMemory(pszBuffer, pszHelperName, cchToCopy);
                pszBuffer[cchToCopy] = '\0';

                return S_OK;
            }
        }
    }

    // Check if its a dynamically generated JIT helper

    const CorInfoHelpFunc s_rgDynamicHCallIds[] = {
#define DYNAMICJITHELPER(code, fn) code,
#define JITHELPER(code, fn)
#include <jithelpers.h>
    };

    for (unsigned d = 0; d < DYNAMIC_CORINFO_HELP_COUNT; d++)
    {
        if (addr == (TADDR)(hlpDynamicFuncTable[d].pfnHelper))
        {
            CorInfoHelpFunc code = s_rgDynamicHCallIds[d];
            sprintf_s(pszBuffer, outBufferSize, s_rgHelperNames[code]);
            return S_OK;
        }
    }

#ifdef _X86_
#ifdef HARDBOUND_DYNAMIC_CALLS

    JIT_Writeable_Thunks * pThunks = 
            (JIT_Writeable_Thunks *) GVAL_ADDR(JIT_Writeable_Thunks_Buf).GetAddr();

    // Check if its a dynamically generated JIT helper that the ngen image hardbound
    // to via JIT_Writeable_Thunks_Buf

    JIT_DynamicCall_Thunk * pHCalls = &pThunks->DynamicHCall[0];
    for (unsigned h = 0; h < DYNAMIC_CORINFO_HELP_COUNT; h++)
    {
        if (addr == (TADDR)pHCalls[h].Bytes)
        {
            CorInfoHelpFunc code = s_rgDynamicHCallIds[h];
            sprintf_s(pszBuffer, outBufferSize, s_rgHelperNames[code]);
            return S_OK;
        }
    }

    // Check if its a dynamically generated FCall that the ngen image hardbound
    // to via JIT_Writeable_Thunks_Buf

#undef DYNAMICALLY_ASSIGNED_FCALL_IMPL
#define DYNAMICALLY_ASSIGNED_FCALL_IMPL(id, defaultimpl) #id,
    const static PCSTR s_rgDynamicFCallNames[] = {
            DYNAMICALLY_ASSIGNED_FCALLS()
    };

    JIT_DynamicCall_Thunk * pFCalls = &pThunks->DynamicFCall[0];
    for (unsigned f = 0; f < ECall::NUM_DYNAMICALLY_ASSIGNED_FCALL_IMPLEMENTATIONS; f++)
    {
        if (addr == (TADDR)pFCalls[f].Bytes)
        {
            sprintf_s(pszBuffer, outBufferSize, s_rgDynamicFCallNames[f]);
            return S_OK;
        }
    }

#endif // HARDBOUND_DYNAMIC_CALLS
#endif // _X86_

    return E_FAIL;
};


HRESULT
ClrDataAccess::RequestJumpThunkTarget(IN ULONG32 inBufferSize,
                             IN BYTE* inBuffer,
                             IN ULONG32 outBufferSize,
                             OUT BYTE* outBuffer)
{
    if ((inBufferSize != sizeof(CONTEXT)) ||
        (inBuffer == NULL) ||
        (outBufferSize != sizeof(DacpJumpThunkData)) ||
        (outBuffer == NULL))
    {
        return E_INVALIDARG;
    }

    CONTEXT *pctx; pctx = (CONTEXT*)inBuffer;
    DacpJumpThunkData *pdata; pdata = (DacpJumpThunkData*)outBuffer;

    PAL_CPP_TRY
    {
    }
    PAL_CPP_CATCH_ALL
    {
        // Do nothing, just return E_FAIL below
    }
    PAL_CPP_ENDTRY

    return E_FAIL;
}


STDMETHODIMP
ClrDataAccess::Request(IN ULONG32 reqCode,
                       IN ULONG32 inBufferSize,
                       IN BYTE* inBuffer,
                       IN ULONG32 outBufferSize,
                       OUT BYTE* outBuffer)
{
    HRESULT status;

    DAC_ENTER();

    EX_TRY
    {
        switch(reqCode)
        {
        case CLRDATA_REQUEST_REVISION:
            if (inBufferSize != 0 ||
                inBuffer ||
                outBufferSize != sizeof(ULONG32))
            {
                status = E_INVALIDARG;
            }
            else
            {
                *(ULONG32*)outBuffer = 8;
                status = S_OK;
            }
            break;

        case DACPRIV_REQUEST_THREAD_STORE_DATA:
            status = RequestThreadStoreData(inBufferSize, inBuffer,
                                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_THREAD_DATA:
            status = RequestThreadData(inBufferSize, inBuffer,
                                       outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_THREAD_THINLOCK_DATA:
            status = RequestThreadFromThinlock(inBufferSize, inBuffer,
                                       outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_CONTEXT_DATA:
            status = RequestContextData(inBufferSize, inBuffer,
                                        outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_METHODDESC_DATA:
            status = RequestMethodDescData(inBufferSize, inBuffer,
                                        outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_METHODDESC_IP_DATA:
            status = RequestMethodDescIPData(inBufferSize, inBuffer,
                                        outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_METHODDESC_NAME:
            status = RequestMethodName(inBufferSize, inBuffer,
                                        outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_CODEHEADER_DATA:
            status = RequestCodeHeaderData(inBufferSize, inBuffer,
                                        outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_THREADPOOL_DATA:
            status = RequestThreadpoolData(inBufferSize, inBuffer,
                                        outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_WORKREQUEST_DATA:
            status = RequestWorkRequestData(inBufferSize, inBuffer,
                                        outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_OBJECT_DATA:
            status = RequestObjectData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_OBJECT_STRING_DATA:
            status = RequestObjectStringData(inBufferSize, inBuffer,
                                        outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_OBJECT_CLASS_NAME:
            status = RequestObjectClassName(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_METHODTABLE_NAME:
            status = RequestMethodTableName(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_METHODTABLE_DATA:
            status = RequestMethodTableData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_MODULE_DATA:
            status = RequestModuleData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_MODULEMAP_TRAVERSE:
            status = RequestModuleMapTraverse(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_EECLASS_DATA:
            status = RequestEEClassData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_FIELDDESC_DATA:
            status = RequestFieldDescData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_MANAGEDSTATICADDR:
            status = RequestManagedStaticAddr(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_MODULETOKEN_DATA:
            status = RequestModuleTokenData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
#ifdef _DEBUG
#endif // _DEBUG
        case DACPRIV_REQUEST_METHODDESC_FRAME_DATA:
            status = RequestMethodDescFrameData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_FRAME_NAME:
            status = RequestFrameNameData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_PEFILE_DATA:
            status = RequestPEFileData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_PEFILE_NAME:
            status = RequestPEFileName(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_APPDOMAIN_STORE_DATA:
            status = RequestAppDomainStoreData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_APPDOMAIN_LIST:
            status = RequestAppDomainList(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_APPDOMAIN_DATA:
            status = RequestAppDomainData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_APPDOMAIN_NAME:
            status = RequestAppDomainName(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_APPDOMAIN_APP_BASE:
            status = RequestAppDomainAppBase(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_APPDOMAIN_PRIVATE_BIN_PATHS:
            status = RequestAppDomainPrivateBinPaths(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_APPDOMAIN_CONFIG_FILE:
            status = RequestAppDomainConfigFile(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_ASSEMBLY_LIST:
            status = RequestAssemblyList(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_FAILED_ASSEMBLY_LIST:
            status = RequestFailedAssemblyList(inBufferSize, inBuffer,
                outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_ASSEMBLY_DATA:
            status = RequestAssemblyData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_ASSEMBLY_NAME:
            status = RequestAssemblyName(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_ASSEMBLY_DISPLAY_NAME:
            status = RequestAssemblyDisplayName(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_ASSEMBLY_LOCATION:
            status = RequestAssemblyLocation(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_FAILED_ASSEMBLY_DATA:
            status = RequestFailedAssemblyData(inBufferSize, inBuffer,
                outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_FAILED_ASSEMBLY_DISPLAY_NAME:
            status = RequestFailedAssemblyDisplayName(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_FAILED_ASSEMBLY_LOCATION:
            status = RequestFailedAssemblyLocation(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_ASSEMBLYMODULE_LIST:
            status = RequestAssemblyModuleList(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_HEAPSEGMENT_DATA:
            status = RequestGCHeapSegment(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_GCHEAPDETAILS_DATA:
            status = RequestGCHeapDetails(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_GCHEAPDETAILS_STATIC_DATA:
            status = RequestGCHeapDetailsStatic(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_GCHEAP_DATA:
            status = RequestGCHeapData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_GCHEAP_LIST:
            status = RequestGCHeapList(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_UNITTEST_DATA:
            status = RequestDACUnitTestData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;

        case DACPRIV_REQUEST_ISSTUB:
            status = RequestIsStub(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;

        case DACPRIV_REQUEST_DOMAINLOCALMODULE_DATA:
            status = RequestDomainLocalModuleData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_DOMAINLOCALMODULEFROMAPPDOMAIN_DATA:
            status = RequestDomainLocalModuleFromAppDomainData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_DOMAINLOCALMODULE_DATA_FROM_MODULE:
            status = RequestDomainLocalModuleFromModule(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_SYNCBLOCK_DATA:
            status = RequestSyncBlockData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_HANDLETABLE_TRAVERSE:
            status = RequestHandleTableTraverse(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
#ifdef STRESS_LOG
        case DACPRIV_REQUEST_STRESSLOG_DATA:
            status = RequestStressLogData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
#endif // STRESS_LOG
        case DACPRIV_REQUEST_JITLIST:
            status = RequestJitList(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;

        case DACPRIV_REQUEST_JIT_HELPER_FUNCTION_NAME:
            status = RequestJitHelperFunctionName(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;

        case DACPRIV_REQUEST_JUMP_THUNK_TARGET:
            status = RequestJumpThunkTarget(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;

        case DACPRIV_REQUEST_LOADERHEAP_TRAVERSE:
            status = RequestLoaderHeapTraverse(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_MANAGER_LIST:
            status = RequestJitManagerList(inBufferSize, inBuffer,
                outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_JITHEAPLIST:
            status = RequestJitHeapList(inBufferSize, inBuffer,
                outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_CODEHEAP_LIST:
            status = RequestCodeHeapList(inBufferSize, inBuffer,
                outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_METHODTABLE_SLOT:
            status = RequestMethodTableSlot(inBufferSize, inBuffer,
                outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_VIRTCALLSTUBHEAP_TRAVERSE:
            status = RequestVirtCallStubHeapTraverse(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_SYNCBLOCK_CLEANUP_DATA:
            status = RequestSyncBlockCleanupData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_RCWCLEANUP_TRAVERSE:
            status = RequestRcwCleanupTraverse(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_EHINFO_TRAVERSE:
            status = RequestEHInfoTraverse(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
        case DACPRIV_REQUEST_NESTEDEXCEPTION_DATA:
            status = RequestNestedException(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;

        case DACPRIV_REQUEST_USEFULGLOBALS:
            status = RequestUsefulGlobals(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;
            
        case DACPRIV_REQUEST_CLRTLSDATA_INDEX:
            status = RequestCLRTLSDataIndex(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;

        case DACPRIV_REQUEST_MODULE_FINDIL:
            status = RequestModuleILData(inBufferSize, inBuffer,
                            outBufferSize, outBuffer);
            break;

    
        default:
            status = E_INVALIDARG;
            break;
        }
    }
    EX_CATCH
    {
        if (!DacExceptionFilter(GET_EXCEPTION(), this, &status))
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions)

    DAC_LEAVE();
    return status;
}

void
ClrDataAccess::EnumWksGlobalMemoryRegions(CLRDataEnumMemoryFlags flags)
{
#ifndef GC_SMP
    WKS::gc_heap::ephemeral_heap_segment.EnumMem();
    WKS::gc_heap::alloc_allocated.EnumMem();
    WKS::gc_heap::finalize_queue.EnumMem();
    WKS::generation_table.EnumMem();

    if (WKS::generation_table.IsValid())
    {
            // enumerating the generations from max (which is normally gen2) to max+1 gives you
            // the segment list for all the normal segements plus the large heap segment (max+1)
            // this is the convention in the GC so it is repeated here
            for (ULONG i = GCHeap::GetMaxGeneration(); i <= GCHeap::GetMaxGeneration()+1; i++)
            {
                __DPtr<WKS::heap_segment> seg = (TADDR)WKS::generation_table[i].start_segment;
                while (seg)
                {
                        DacEnumMemoryRegion(PTR_HOST_TO_TADDR(seg), sizeof(WKS::heap_segment));

                        seg = (TADDR)(seg->next);
                }
            }
    }

#endif // !GC_SMP
}


HRESULT ClrDataAccess::RequestCLRTLSDataIndex(IN ULONG32 inBufferSize,
                                   IN BYTE* inBuffer,
                                   IN ULONG32 outBufferSize,
                                   OUT BYTE* outBuffer)
{
    if ((outBuffer == NULL) || (outBufferSize != sizeof(DWORD)))
    {
        return E_INVALIDARG;
    }

    if (CExecutionEngine::GetTlsIndex () == TLS_OUT_OF_INDEXES)
    {
        return S_FALSE;
    }
    
    *(DWORD*)outBuffer = CExecutionEngine::GetTlsIndex ();
    
    return S_OK;
}



