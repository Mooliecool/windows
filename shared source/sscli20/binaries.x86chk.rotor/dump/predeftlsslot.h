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

#ifndef __PREDEFTLSSLOT_H__
#define __PREDEFTLSSLOT_H__


// And here are the predefined slots for accessing TLS from various DLLs of the CLR.
// Note that we want to support combinations of Debug and Retail DLLs for testing
// purposes, so we burn the slots into the retail EE even if a debug CLR dll needs
// them.
enum PredefinedTlsSlots
{
    TlsIdx_StrongName,
    TlsIdx_JitPerf,
    TlsIdx_JitX86Perf,
    TlsIdx_JitLogEnv,
    TlsIdx_IceCap,
    TlsIdx_StressLog,
    TlsIdx_StackProbe,
    TlsIdx_Check,
    TlsIdx_ForbidGCLoaderUseCount,
    TlsIdx_ClrDebugState,         // Pointer to ClrDebugState* structure
    TlsIdx_StressThread,
    TlsIdx_MemoryReportBucket,
    #ifndef _X86_
    TlsIdx_SimulatedFS0,            // simulated FS:[0] for win64
    #endif // _X86_

    // Add more indices here.
    TlsIdx_ThreadType, // bit flags to indicate special thread's type 
    TlsIdx_CantStopCount, // Can't-stop counter for any thread
    TlsIdx_OwnedCrstsChain, // slot to store the Crsts owned by this thread
    TlsIdx_AppDomainAgilePendingTable,
    TlsIdx_CantAllocCount, //Can't allocate memory on heap in this thread

    MAX_PREDEFINED_TLS_SLOT
};

enum TlsThreadTypeFlag // flag used for thread type in Tls data
{
    ThreadType_GC = 0x00000001,
    ThreadType_Timer = 0x00000002,
    ThreadType_Gate = 0x00000004,
    ThreadType_DbgHelper = 0x00000008,
    ThreadType_Shutdown = 0x00000010,
    ThreadType_DynamicSuspendEE = 0x00000020,
    ThreadType_Finalizer = 0x00000040,
    ThreadType_ADUnloadHelper = 0x00000200,
    ThreadType_ShutdownHelper = 0x00000400,
    ThreadType_Threadpool_IOCompletion = 0x00000800,
    ThreadType_Threadpool_Worker = 0x00001000,
    ThreadType_Wait = 0x00002000,
};

#endif

