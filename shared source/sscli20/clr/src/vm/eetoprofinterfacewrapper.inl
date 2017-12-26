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

#ifndef _EETOPROFEXCEPTIONINTERFACEWRAPPER_INL_
#define _EETOPROFEXCEPTIONINTERFACEWRAPPER_INL_

#include "common.h"


// A wrapper for the profiler.  Various events to signal different phases of exception
// handling.
class EEToProfilerExceptionInterfaceWrapper
{
  public:

#if defined(PROFILING_SUPPORTED)
    //
    // Exception creation
    //

    static inline void ExceptionThrown(Thread* pThread)
    {
        WRAPPER_CONTRACT;
        if (CORProfilerTrackExceptions())
        {
            _ASSERTE(pThread->PreemptiveGCDisabled());

            // Get a reference to the object that won't move
            OBJECTREF thrown = pThread->GetThrowable();

            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ExceptionThrown(
                reinterpret_cast<ThreadID>(pThread),
                reinterpret_cast<ObjectID>((*(BYTE**)&thrown)));
        }
    }

    //
    // Search phase
    //

    static inline void ExceptionSearchFunctionEnter(Thread* pThread, MethodDesc* pFunction)
    {
        WRAPPER_CONTRACT;
        // Notify the profiler of the function being searched for a handler.
        if (CORProfilerTrackExceptions())
        {
            if (!pFunction->IsNoMetadata())
            {
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->ExceptionSearchFunctionEnter(
                    reinterpret_cast<ThreadID>(pThread),
                    (CodeID) pFunction);
            }
        }
    }

    static inline void ExceptionSearchFunctionLeave(Thread* pThread)
    {
        WRAPPER_CONTRACT;
        // Notify the profiler of the function being searched for a handler.
        if (CORProfilerTrackExceptions())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ExceptionSearchFunctionLeave(
                reinterpret_cast<ThreadID>(pThread));

        }
    }

    static inline void ExceptionSearchFilterEnter(Thread* pThread, MethodDesc* pFunc)
    {
        WRAPPER_CONTRACT;
        // Notify the profiler of the filter.
        if (CORProfilerTrackExceptions())
        {
            if (!pFunc->IsNoMetadata())
            {
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->ExceptionSearchFilterEnter(
                    reinterpret_cast<ThreadID>(pThread),
                    (CodeID) pFunc);
            }
        }
    }

    static inline void ExceptionSearchFilterLeave(Thread* pThread)
    {
        WRAPPER_CONTRACT;
        // Notify the profiler of the filter.
        if (CORProfilerTrackExceptions())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ExceptionSearchFilterLeave(
                reinterpret_cast<ThreadID>(pThread));

        }
    }

    static inline void ExceptionSearchCatcherFound(Thread* pThread, MethodDesc* pFunc)
    {
        WRAPPER_CONTRACT;
        if (CORProfilerTrackExceptions())
        {
            if (!pFunc->IsNoMetadata())
            {
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->ExceptionSearchCatcherFound(
                    reinterpret_cast<ThreadID>(pThread),
                    (CodeID) pFunc);
            }
        }
    }

    static inline void ExceptionOSHandlerEnter(Thread* pThread, MethodDesc** ppNotify, MethodDesc* pFunc)
    {
        WRAPPER_CONTRACT;
        // If this is first managed function seen in this crawl, notify profiler.
        if (CORProfilerTrackExceptions())
        {
            _ASSERTE(ppNotify);

            if (*ppNotify == NULL)
            {
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->ExceptionOSHandlerEnter(
                    reinterpret_cast<ThreadID>(pThread),
                    (CodeID) pFunc);
            }
            *ppNotify = pFunc;
        }
    }

    static inline void ExceptionOSHandlerLeave(Thread* pThread, MethodDesc** ppNotify)
    {
        WRAPPER_CONTRACT;
        if (CORProfilerTrackExceptions())
        {
            if (*ppNotify != NULL)
            {
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->ExceptionOSHandlerLeave(
                    reinterpret_cast<ThreadID>(pThread),
                    (CodeID)*ppNotify);
            }
        }
    }

    //
    // Unwind phase
    //
    static inline void ExceptionUnwindFunctionEnter(Thread* pThread, MethodDesc* pFunc)
    {
        WRAPPER_CONTRACT;
        // Notify the profiler of the function being searched for a handler.
        if (CORProfilerTrackExceptions())
        {
            if (!pFunc->IsNoMetadata())
            {
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->ExceptionUnwindFunctionEnter(
                    reinterpret_cast<ThreadID>(pThread),
                    (CodeID) pFunc);
            }
        }
    }

    static inline void ExceptionUnwindFunctionLeave(Thread* pThread)
    {
        WRAPPER_CONTRACT;
        // Notify the profiler that searching this function is over.
        if (CORProfilerTrackExceptions())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ExceptionUnwindFunctionLeave(
                reinterpret_cast<ThreadID>(pThread));
        }
    }

    static inline void ExceptionUnwindFinallyEnter(Thread* pThread, MethodDesc* pFunc)
    {
        WRAPPER_CONTRACT;
        // Notify the profiler of the function being searched for a handler.
        if (CORProfilerTrackExceptions())
        {
            if (!pFunc->IsNoMetadata())
            {
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->ExceptionUnwindFinallyEnter(
                    reinterpret_cast<ThreadID>(pThread),
                    (CodeID) pFunc);
            }
        }
    }

    static inline void ExceptionUnwindFinallyLeave(Thread* pThread)
    {
        WRAPPER_CONTRACT;
        // Notify the profiler of the function being searched for a handler.
        if (CORProfilerTrackExceptions())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ExceptionUnwindFinallyLeave(
                reinterpret_cast<ThreadID>(pThread));
        }
    }

    static inline void ExceptionCatcherEnter(Thread* pThread, MethodDesc* pFunc)
    {
        WRAPPER_CONTRACT;
        // Notify the profiler.
        if (CORProfilerTrackExceptions())
        {
            if (!pFunc->IsNoMetadata())
            {
    
                // Note that the callee must be aware that the ObjectID
                // passed CAN change when gc happens.
                OBJECTREF thrown = NULL;
                GCPROTECT_BEGIN(thrown);
                thrown = pThread->GetThrowable();
                {
                    PROFILER_CALL;
                    g_profControlBlock.pProfInterface->ExceptionCatcherEnter(
                        reinterpret_cast<ThreadID>(pThread),
                        (CodeID) pFunc,
                        reinterpret_cast<ObjectID>((*(BYTE**)&thrown)));
                }
                GCPROTECT_END();
            }
        }
    }

    static inline void ExceptionCatcherLeave(Thread* pThread)
    {
        WRAPPER_CONTRACT;
        // Notify the profiler of the function being searched for a handler.
        if (CORProfilerTrackExceptions())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ExceptionCatcherLeave(
                reinterpret_cast<ThreadID>(pThread));

        }
    }
        

#else // !PROFILING_SUPPORTED
    static inline void ExceptionThrown(Thread* pThread) { LEAF_CONTRACT;}
    static inline void ExceptionSearchFunctionEnter(Thread* pThread, MethodDesc* pFunction) { LEAF_CONTRACT;}
    static inline void ExceptionSearchFunctionLeave(Thread* pThread) { LEAF_CONTRACT;}
    static inline void ExceptionSearchFilterEnter(Thread* pThread, MethodDesc* pFunc) { LEAF_CONTRACT;}
    static inline void ExceptionSearchFilterLeave(Thread* pThread) { LEAF_CONTRACT;}
    static inline void ExceptionSearchCatcherFound(Thread* pThread, MethodDesc* pFunc) { LEAF_CONTRACT;}
    static inline void ExceptionOSHandlerEnter(Thread* pThread, MethodDesc** ppNotify, MethodDesc* pFunc) { LEAF_CONTRACT;}
    static inline void ExceptionOSHandlerLeave(Thread* pThread, MethodDesc** ppNotify) { LEAF_CONTRACT;}
    static inline void ExceptionUnwindFunctionEnter(Thread* pThread, MethodDesc* pFunc) { LEAF_CONTRACT;}
    static inline void ExceptionUnwindFunctionLeave(Thread* pThread) { LEAF_CONTRACT;}
    static inline void ExceptionUnwindFinallyEnter(Thread* pThread, MethodDesc* pFunc) { LEAF_CONTRACT;}
    static inline void ExceptionUnwindFinallyLeave(Thread* pThread) { LEAF_CONTRACT;}
    static inline void ExceptionCatcherEnter(Thread* pThread, MethodDesc* pFunc) { LEAF_CONTRACT;}
    static inline void ExceptionCatcherLeave(Thread* pThread) { LEAF_CONTRACT;}
#endif // !PROFILING_SUPPORTED
};


#endif // _EETOPROFEXCEPTIONINTERFACEWRAPPER_INL_
