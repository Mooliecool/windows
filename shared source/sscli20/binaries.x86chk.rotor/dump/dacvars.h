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
// This file contains the globals and statics that are visible to DAC.
// It is used for the following:
// 1. in daccess.h for definitions
// 2. in enummem.cpp to dump out the related memory of static and globals 
//    in heap dump,
// 3. in DacUpdateDll and toolbox\DacTablenGen\main.cs

//
// To use this, define one of the following macro & include the file like so:
//
// #define DEFINE_DACVAR(type, size, id)  type id;
// #include "dacvars.h"
// The second column "true_type" is the true size of the static/global variable. 
// That is used in enummem.cpp to write out proper size of memory. 
//

#ifndef DEFINE_DACVAR
#define DEFINE_DACVAR(type, true_type, id)
#endif

// Use this macro to define a static var that is known to DAC, but not captured in a dump.                         
#ifndef DEFINE_DACVAR_NO_DUMP
#define DEFINE_DACVAR_NO_DUMP(type, true_type, id)
#endif

DEFINE_DACVAR(ULONG, PTR_RangeSection, ExecutionManager__m_CodeRangeList)
DEFINE_DACVAR(ULONG, PTR_RangeSection, ExecutionManager__m_DataRangeList)
DEFINE_DACVAR(ULONG, PTR_IJitManager, ExecutionManager__m_pJitList)
DEFINE_DACVAR(ULONG, PTR_EECodeManager, ExecutionManager__m_pDefaultCodeMan)
DEFINE_DACVAR(ULONG, LONG, ExecutionManager__m_dwReaderCount)
DEFINE_DACVAR(ULONG, LONG, ExecutionManager__m_dwWriterLock)

DEFINE_DACVAR(ULONG, VMHELPDEF *, dac__hlpDynamicFuncTable)
#ifdef _X86_
// #ifdef HARDBOUND_DYNAMIC_CALLS
DEFINE_DACVAR(ULONG, JIT_Writeable_Thunks, dac__JIT_Writeable_Thunks_Buf)
// #endif
#endif

DEFINE_DACVAR(ULONG, PTR_StubManager, StubManager__g_pFirstManager)
DEFINE_DACVAR(ULONG, TADDR, IJWNOADThunk__s_AddrOfMakeCall)
DEFINE_DACVAR(ULONG, ULONG *, IJWNOADThunkStubManager__g_pManager)  // IJWNOADThunk is not visible in ROTOR build
DEFINE_DACVAR(ULONG, PTR_MethodDescPrestubManager, MethodDescPrestubManager__g_pManager)
DEFINE_DACVAR(ULONG, PTR_StubLinkStubManager, StubLinkStubManager__g_pManager)
DEFINE_DACVAR(ULONG, PTR_ThunkHeapStubManager, ThunkHeapStubManager__g_pManager)
DEFINE_DACVAR(ULONG, PTR_JumpStubStubManager, JumpStubStubManager__g_pManager)
DEFINE_DACVAR(ULONG, PTR_EntryPointStubManager, EntryPointStubManager__g_pManager)
DEFINE_DACVAR(ULONG, PTR_ILStubManager, ILStubManager__g_pManager)
DEFINE_DACVAR(ULONG, PTR_NDirectDispatchStubManager, NDirectDispatchStubManager__g_pManager)
DEFINE_DACVAR(ULONG, PTR_ComPlusDispatchStubManager, ComPlusDispatchStubManager__g_pManager)
DEFINE_DACVAR(ULONG, PTR_ReverseInteropStubManager, ReverseInteropStubManager__g_pManager)
DEFINE_DACVAR(ULONG, PTR_DelegateInvokeStubManager, DelegateInvokeStubManager__g_pManager)
DEFINE_DACVAR(ULONG, PTR_VirtualCallStubManagerManager, VirtualCallStubManagerManager__g_pManager)

DEFINE_DACVAR(ULONG, PTR_ThreadStore, ThreadStore__s_pThreadStore)

DEFINE_DACVAR(ULONG, long, ThreadpoolMgr__cpuUtilization)
DEFINE_DACVAR(ULONG, int, ThreadpoolMgr__NumWorkerThreads)
DEFINE_DACVAR(ULONG, int, ThreadpoolMgr__MinLimitTotalWorkerThreads)
DEFINE_DACVAR(ULONG, DWORD, ThreadpoolMgr__MaxLimitTotalWorkerThreads)
DEFINE_DACVAR(ULONG, int, ThreadpoolMgr__NumIdleWorkerThreads)
DEFINE_DACVAR(ULONG, LONG, ThreadpoolMgr__NumQueuedWorkRequests)
DEFINE_DACVAR(ULONG, ULONG*, ThreadpoolMgr__WorkRequestHead) // PTR_WorkRequest is not defined. So use a pointer type
DEFINE_DACVAR(ULONG, ULONG*, ThreadpoolMgr__WorkRequestTail) // 
DEFINE_DACVAR(ULONG, DWORD, ThreadpoolMgr__NumTimers)    
DEFINE_DACVAR(ULONG, LONG, ThreadpoolMgr__NumCPThreads)
DEFINE_DACVAR(ULONG, LONG, ThreadpoolMgr__NumFreeCPThreads)
DEFINE_DACVAR(ULONG, LONG, ThreadpoolMgr__MaxFreeCPThreads)
DEFINE_DACVAR(ULONG, LONG, ThreadpoolMgr__NumRetiredCPThreads)
DEFINE_DACVAR(ULONG, LONG, ThreadpoolMgr__MaxLimitTotalCPThreads)
DEFINE_DACVAR(ULONG, LONG, ThreadpoolMgr__CurrentLimitTotalCPThreads)
DEFINE_DACVAR(ULONG, LONG, ThreadpoolMgr__MinLimitTotalCPThreads)        
DEFINE_DACVAR(ULONG, LIST_ENTRY, ThreadpoolMgr__TimerQueue)        

DEFINE_DACVAR(ULONG, ULONG, GCHeap__gcHeapType)
DEFINE_DACVAR(ULONG, PTR_Thread, GCHeap__FinalizerThread)
DEFINE_DACVAR(ULONG, PTR_Thread, GCHeap__GcThread)

DEFINE_DACVAR(ULONG, SIZE_T, WKS__gc_heap__alloc_allocated)
DEFINE_DACVAR(ULONG, SIZE_T, WKS__gc_heap__ephemeral_heap_segment)
DEFINE_DACVAR(ULONG, SIZE_T, WKS__gc_heap__finalize_queue)
DEFINE_DACVAR(ULONG, ULONG, SVR__gc_heap__n_heaps)
DEFINE_DACVAR(ULONG, SIZE_T, SVR__gc_heap__g_heaps)

DEFINE_DACVAR(ULONG, PTR_SystemDomain, SystemDomain__m_pSystemDomain)
DEFINE_DACVAR(ULONG, ArrayListStatic, SystemDomain__m_appDomainIndexList)
DEFINE_DACVAR(ULONG, PTR_SharedDomain, SharedDomain__m_pSharedDomain)

DEFINE_DACVAR(ULONG, DWORD, CExecutionEngine__TlsIndex)

DEFINE_DACVAR(ULONG, BOOL, CNameSpace__m_GcStructuresInvalidCnt)
DEFINE_DACVAR(ULONG, ULONG, dac__g_ECustomDumpFlavor)

DEFINE_DACVAR(ULONG, PTR_SString, SString__s_Empty)

DEFINE_DACVAR(ULONG, SIZE_T, CTPMethodTable__s_pTPStub)

DEFINE_DACVAR(ULONG, ULONG, ArrayBase__s_arrayBoundsZero)

DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pNotificationTable)
DEFINE_DACVAR(ULONG, ULONG, dac__g_dacNotificationFlags)

DEFINE_DACVAR(ULONG, ULONG, dac__gThreadTLSIndex)
DEFINE_DACVAR(ULONG, ULONG, dac__gAppDomainTLSIndex)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pConfig)
DEFINE_DACVAR(ULONG, ULONG, dac__g_Mscorlib)

DEFINE_DACVAR(ULONG, SIZE_T, dac__generation_table)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_card_table)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_lowest_address)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_highest_address)

DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pGCHeap)

DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pThinLockThreadIdDispenser)    
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pObjectClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pHiddenMethodTableClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pStringClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pArrayClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pExceptionClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pThreadAbortExceptionClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pOutOfMemoryExceptionClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pStackOverflowExceptionClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pExecutionEngineExceptionClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pDelegateClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pMultiDelegateClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pFreeObjectMethodTable)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pOverlappedDataClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pValueTypeClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pEnumClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pSharedStaticsClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pThreadClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pCriticalFinalizerObjectClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pAsyncFileStream_AsyncResultClass)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pPredefinedArrayTypes)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pPrepareConstrainedRegionsMethod)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pPrepareConstrainedRegionsNoOpMethod)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pExecuteBackoutCodeHelperMethod)

DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pObjectCtorMD)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pObjectFinalizerMD)

DEFINE_DACVAR(ULONG, PTR_Stub, dac__g_preStub)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_UMThunkPreStub)

DEFINE_DACVAR(ULONG, ULONG, dac__g_fProcessDetach)
DEFINE_DACVAR(ULONG, ULONG, dac__g_fHostConfig)

DEFINE_DACVAR(ULONG, ULONG, dac__g_CORDebuggerControlFlags)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pDebugger)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pDebugInterface)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pEEDbgInterfaceImpl)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pDebugInfoStore)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pEEInterface)

DEFINE_DACVAR(ULONG, SIZE_T, dac__gLowestFCall)
DEFINE_DACVAR(ULONG, SIZE_T, dac__gHighestFCall)
DEFINE_DACVAR(ULONG, SIZE_T, dac__gFCallMethods)

DEFINE_DACVAR(ULONG, PTR_SyncTableEntry, dac__g_pSyncTable)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pRCWCleanupList)

DEFINE_DACVAR(ULONG, SIZE_T, dac__g_mscorwksLoadedBaseAddress)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_mscorwksPreferredBaseAddress)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_mscorwksVirtualSize)
DEFINE_DACVAR(ULONG, SyncBlockCache *, SyncBlockCache__s_pSyncBlockCache)

DEFINE_DACVAR(ULONG, HandleTableMap, dac__g_HandleTableMap)
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pStressLog)

DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pMdaStaticHeap)

DEFINE_DACVAR(ULONG, SIZE_T, dac__g_pIPCManagerInterface)
    
DEFINE_DACVAR(ULONG, SIZE_T, dac__g_FCDynamicallyAssignedImplementations)

DEFINE_DACVAR_NO_DUMP(ULONG, PTR_FrameTypeName, Frame__s_pFrameTypeNames)

#undef DEFINE_DACVAR
#undef DEFINE_DACVAR_NO_DUMP
