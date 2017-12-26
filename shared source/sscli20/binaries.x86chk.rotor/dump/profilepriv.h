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
//*****************************************************************************
#ifndef _ProfilePriv_h_
#define _ProfilePriv_h_


// Forward declarations
class EEToProfInterface;
class Object;
struct ScanContext;

#ifdef PROFILING_SUPPORTED

#define PROFILING_SUPPORTED_DATA 1

#include "corprof.h"

/*
 * A struct to contain the status of profiling in the VM
 */
struct ProfControlBlock
{
    DWORD              dwSig;
    DWORD              dwControlFlags;
    EEToProfInterface *pProfInterface;
    BYTE              *pReservedMem;

    FunctionEnter     *pEnter;
    FunctionEnter     *pLeave;
    FunctionEnter     *pTailcall;

    FunctionEnter2    *pEnter2;
    FunctionLeave2    *pLeave2;
    FunctionTailcall2 *pTailcall2;

    BOOL               fGCInProgress;

    // no constructor - global structure
    void Init()
    {
        LEAF_CONTRACT;

        dwSig = 0;
        dwControlFlags = COR_PRF_MONITOR_NONE;
        pProfInterface = NULL;
        pReservedMem = NULL;        
        pEnter = NULL;
        pLeave = NULL;
        pTailcall = NULL;
        pEnter2 = NULL;
        pLeave2 = NULL;
        pTailcall2 = NULL;
        fGCInProgress = FALSE;
    }
};

/*
 * Enumerates the various init states of profiling.
 */
enum ProfilerStatus
{
    profNone   = 0x0,               // No profiler running.
    profCoInit = 0x1,               // Profiler has called CoInit.
    profInit   = 0x2,               // profCoInit and profiler running.
    profInInit = 0x4                // the profiler is initializing
};

extern ProfilerStatus     g_profStatus;
extern ProfControlBlock   g_profControlBlock;

//
// Use IsProfilerPresent() to check whether or not a CLR Profiler is
// attached.
//
#define IsProfilerPresent() (g_profStatus & (profInit | profInInit))
#define IsProfilerInInit() (g_profStatus & profInit)
#define CORProfilerPresent() IsProfilerPresent()
#define CORProfilerTrackJITInfo() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_JIT_COMPILATION))
#define CORProfilerTrackCacheSearches() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_CACHE_SEARCHES))
#define CORProfilerTrackModuleLoads() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_MODULE_LOADS))
#define CORProfilerTrackAssemblyLoads() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_ASSEMBLY_LOADS))
#define CORProfilerTrackAppDomainLoads() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_APPDOMAIN_LOADS))
#define CORProfilerTrackThreads() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_THREADS))
#define CORProfilerTrackClasses() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_CLASS_LOADS))
#define CORProfilerTrackGC() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_GC))
#define CORProfilerTrackAllocationsEnabled() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_ENABLE_OBJECT_ALLOCATED))
#define CORProfilerTrackAllocations() \
    (CORProfilerTrackAllocationsEnabled() && \
    (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_OBJECT_ALLOCATED))
#define CORProfilerAllowRejit() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_ENABLE_REJIT))
#define CORProfilerTrackExceptions() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_EXCEPTIONS))
#define CORProfilerTrackCLRExceptions() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_CLR_EXCEPTIONS))
#define CORProfilerTrackTransitions() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_CODE_TRANSITIONS))
#define CORProfilerTrackEnterLeave() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_ENTERLEAVE))
#define CORProfilerTrackCCW() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_CCW))
#define CORProfilerTrackRemoting() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_REMOTING))
#define CORProfilerTrackRemotingCookie() \
    (IsProfilerPresent() && ((g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_REMOTING_COOKIE) \
                             == COR_PRF_MONITOR_REMOTING_COOKIE))
#define CORProfilerTrackRemotingAsync() \
    (IsProfilerPresent() && ((g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_REMOTING_ASYNC) \
                             == COR_PRF_MONITOR_REMOTING_ASYNC))
#define CORProfilerTrackSuspends() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_SUSPENDS))
#define CORProfilerDisableInlining() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_DISABLE_INLINING))
#define CORProfilerJITMapEnabled() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_ENABLE_JIT_MAPS))
#define CORProfilerDisableOptimizations() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_DISABLE_OPTIMIZATIONS))
#define CORProfilerFunctionArgsEnabled() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_ENABLE_FUNCTION_ARGS))
#define CORProfilerFunctionReturnValueEnabled() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_ENABLE_FUNCTION_RETVAL))
#define CORProfilerFrameInfoEnabled() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_ENABLE_FRAME_INFO))
#define CORProfilerStackSnapshotEnabled() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_ENABLE_STACK_SNAPSHOT))

#define CORProfilerFunctionInfoEnabled() \
     (CORProfilerFunctionArgsEnabled() || CORProfilerFunctionReturnValueEnabled())     



#define CORProfilerUseProfileImages() \
    (IsProfilerPresent() && (g_profControlBlock.dwControlFlags & COR_PRF_USE_PROFILE_IMAGES))

//---------------------------------------------------------------
// Bit flags used to track profiler callback execution state, such as which
// ICorProfilerCallback method we're currently executing.  These help us
// enforce the invariants of which calls a profiler is allowed to make at
// given times.  These flags are stored in Thread::m_profilerCallbackState.
//
// For now, we do bare-bones enforcement, which amounts to ensuring only
// asynchronous-safe calls are made asynchronously (i.e., are made from
// outside of profiler callbacks).  This means we only need one bucket of
// callbacks for now, which is the bucket of all callbacks.  When we choose
// to enforce more refined call trees, those trees will determine how we
// explode the set of flags
#define COR_PRF_CALLBACKSTATE_INCALLBACK                 0x1
//
//---------------------------------------------------------------

#endif // PROFILING_SUPPORTED

// This is the helper callback that the gc uses when walking the heap.
BOOL HeapWalkHelper(Object* pBO, void* pv);
void ScanRootsHelper(Object*& o, ScanContext *pSC, DWORD dwUnused);
BOOL AllocByClassHelper(Object* pBO, void* pv);

#endif  // _ProfilePriv_h_
