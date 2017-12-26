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
// File: debugger.cpp
//
// Debugger runtime controller routines.
//
//*****************************************************************************

#include "stdafx.h"
#include "comsystem.h"
#include "debugdebugger.h"
#include "ipcmanagerinterface.h"
#include "../inc/common.h"
#include "comstring.h"
#include "perflog.h"
#include "eeconfig.h" // This is here even for retail & free builds...
#include "../../dlls/mscorrc/resource.h"
#include "remoting.h"
#include "context.h"
#include "vars.hpp"
#include <limits.h>
#include "ilformatter.h"
#include "memoryreport.h"
#include "typeparse.h"
#include "debuginfostore.h"
#include "generics.h"
#include "../../vm/security.h"
#include "../../vm/methoditer.h"
#include "../../vm/encee.h"


class CCLRSecurityAttributeManager;
extern CCLRSecurityAttributeManager s_CLRSecurityAttributeManager;

#ifdef DEBUGGING_SUPPORTED

#define COMPLUS_MB_SERVICE_NOTIFICATION MB_SERVICE_NOTIFICATION


#ifdef _DEBUG
// Reg key. We can set this and then any debugger-lazy-init code will assert.
// This helps track down places where we're caching in debugger stuff in a
// non-debugger scenario.
bool g_DbgShouldntUseDebugger = false;
#endif

const char* g_ppszAttachStateToSZ[]=
    {
    "SYNC_STATE_0" ,
    "SYNC_STATE_1" ,
    "SYNC_STATE_2" ,
    "SYNC_STATE_3" ,
    "SYNC_STATE_10",
    "SYNC_STATE_11",
    "SYNC_STATE_20",
    };


/* ------------------------------------------------------------------------ *
 * Global variables
 * ------------------------------------------------------------------------ */

GPTR_IMPL(Debugger,         g_pDebugger);
GPTR_IMPL(EEDebugInterface, g_pEEInterface);


bool g_EnableSIS = false;

#ifndef DACCESS_COMPILE

DebuggerRCThread        *g_pRCThread = NULL;

// Do some compile time checking on the events in DbgIpcEventTypes.h
// No one ever calls this. But the compiler should still compile it,
// and that should be sufficient.
void DoCompileTimeCheckOnDbgIpcEventTypes()
{
    _ASSERTE(!"Don't call this function. It just does compile time checking\n");

    // We use the C_ASSERT macro here to get a compile-time assert.

    // Make sure we don't have any duplicate numbers.
    // The switch statements in the main loops won't always catch this
    // since we may not switch on all events.

    // store Type-0 in const local vars, so we can use them for bounds checking
    // Create local vars with the val from Type1 & Type2. If there are any
    // collisions, then the variables' names will collide at compile time.
    #define IPC_EVENT_TYPE0(type, val)  const int e_##type = val;
    #define IPC_EVENT_TYPE1(type, val)  int T_##val; T_##val = 0;
    #define IPC_EVENT_TYPE2(type, val)  int T_##val; T_##val = 0;
    #include "dbgipceventtypes.h"
    #undef IPC_EVENT_TYPE2
    #undef IPC_EVENT_TYPE1
    #undef IPC_EVENT_TYPE0

    // Ensure that all identifiers are unique and are matched with
    // integer values.
    #define IPC_EVENT_TYPE0(type, val)  int T2_##type; T2_##type = val;
    #define IPC_EVENT_TYPE1(type, val)  int T2_##type; T2_##type = val;
    #define IPC_EVENT_TYPE2(type, val)  int T2_##type; T2_##type = val;
    #include "dbgipceventtypes.h"
    #undef IPC_EVENT_TYPE2
    #undef IPC_EVENT_TYPE1
    #undef IPC_EVENT_TYPE0

    // Make sure all values are subset of the bits specified by DB_IPCE_TYPE_MASK
    #define IPC_EVENT_TYPE0(type, val)
    #define IPC_EVENT_TYPE1(type, val)  C_ASSERT((val & e_DB_IPCE_TYPE_MASK) == val);
    #define IPC_EVENT_TYPE2(type, val)  C_ASSERT((val & e_DB_IPCE_TYPE_MASK) == val);
    #include "dbgipceventtypes.h"
    #undef IPC_EVENT_TYPE2
    #undef IPC_EVENT_TYPE1
    #undef IPC_EVENT_TYPE0

    // Make sure that no value is DB_IPCE_INVALID_EVENT
    #define IPC_EVENT_TYPE0(type, val)
    #define IPC_EVENT_TYPE1(type, val)  C_ASSERT(val != e_DB_IPCE_INVALID_EVENT);
    #define IPC_EVENT_TYPE2(type, val)  C_ASSERT(val != e_DB_IPCE_INVALID_EVENT);
    #include "dbgipceventtypes.h"
    #undef IPC_EVENT_TYPE2
    #undef IPC_EVENT_TYPE1
    #undef IPC_EVENT_TYPE0

    // Make sure first-last values are well structured.
    C_ASSERT(e_DB_IPCE_RUNTIME_FIRST < e_DB_IPCE_RUNTIME_LAST);
    C_ASSERT(e_DB_IPCE_DEBUGGER_FIRST < e_DB_IPCE_DEBUGGER_LAST);

    // Make sure that event ranges don't overlap.
    // This check is simplified because L->R events come before R<-L
    C_ASSERT(e_DB_IPCE_RUNTIME_LAST < e_DB_IPCE_DEBUGGER_FIRST);


    // Make sure values are in the proper ranges
    // Type1 should be in the Runtime range, Type2 in the Debugger range.
    #define IPC_EVENT_TYPE0(type, val)
    #define IPC_EVENT_TYPE1(type, val)  C_ASSERT((e_DB_IPCE_RUNTIME_FIRST <= val) && (val < e_DB_IPCE_RUNTIME_LAST));
    #define IPC_EVENT_TYPE2(type, val)  C_ASSERT((e_DB_IPCE_DEBUGGER_FIRST <= val) && (val < e_DB_IPCE_DEBUGGER_LAST));
    #include "dbgipceventtypes.h"
    #undef IPC_EVENT_TYPE2
    #undef IPC_EVENT_TYPE1
    #undef IPC_EVENT_TYPE0

    // Make sure that events are in increasing order
    // It's ok if the events skip numbers.
    // This is a more specific check than the range check above.

    /* Expands to look like this:
    const bool f = (
    first <=
    10) && (10 <
    11) && (11 <
    12) && (12 <
    last)
    C_ASSERT(f);
    */

    const bool f1 = (
        (e_DB_IPCE_RUNTIME_FIRST <=
        #define IPC_EVENT_TYPE0(type, val)
        #define IPC_EVENT_TYPE1(type, val)  val) && (val <
        #define IPC_EVENT_TYPE2(type, val)
        #include "dbgipceventtypes.h"
        #undef IPC_EVENT_TYPE2
        #undef IPC_EVENT_TYPE1
        #undef IPC_EVENT_TYPE0
        e_DB_IPCE_RUNTIME_LAST)
    );
    C_ASSERT(f1);

    const bool f2 = (
        (e_DB_IPCE_DEBUGGER_FIRST <=
        #define IPC_EVENT_TYPE0(type, val)
        #define IPC_EVENT_TYPE1(type, val)
        #define IPC_EVENT_TYPE2(type, val) val) && (val <
        #include "dbgipceventtypes.h"
        #undef IPC_EVENT_TYPE2
        #undef IPC_EVENT_TYPE1
        #undef IPC_EVENT_TYPE0
        e_DB_IPCE_DEBUGGER_LAST)
    );
    C_ASSERT(f2);

} // end checks

//-----------------------------------------------------------------------------
// Is the guard page missing on this thread?
// Should only be called for managed threads handling a managed exception.
// If we're handling a stack overflow (ie, missing guard page), then another
// stack overflow will instantly terminate the process. In that case, do stack
// intensive stuff on the helper thread (which has lots of stack space). Only
// problem is that if the faulting thread has a lock, the helper thread may
// get stuck.
// Serves as a hint whether we want to do a favor on the
// faulting thread (preferred) or the helper thread (if low stack).
// See whidbey bug 127436.
//----------------------------------------------------------------------------- </STRIP>
bool IsGuardPageGone()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return false;
}


// This is called from AppDomainEnumerationIPCBlock::Lock and Unlock
void BeginThreadAffinityHelper()
{
    WRAPPER_CONTRACT;

    Thread::BeginThreadAffinity();
}
void EndThreadAffinityHelper()
{
    WRAPPER_CONTRACT;
    Thread::EndThreadAffinity();
}


class TypeHandleDummyPtr
{
private:
    TypeHandleDummyPtr() { }; // should never actually create this.
    void * data;
};

// Convert: LSPTR_TYPEHANDLE --> TypeHandle
TypeHandle GetTypeHandle(LSPTR_TYPEHANDLE ptr)
{
    return TypeHandle::FromPtr(ptr.UnWrap());
}

// Convert: TypeHandle --> LSPTR_TYPEHANDLE
LSPTR_TYPEHANDLE WrapTypeHandle(TypeHandle th)
{
    return LSPTR_TYPEHANDLE::MakePtr(reinterpret_cast<TypeHandleDummyPtr*> (th.AsPtr()));
}

extern void WaitForEndOfShutdown();


// Get the Canary structure which can sniff if the helper thread is safe to run.
HelperCanary * Debugger::GetCanary()
{
    return g_pRCThread->GetCanary();
}
    
// IMPORTANT!!!!!
// Do not call Lock and Unlock directly. Because you might not unlock
// if exception takes place. Use DebuggerLockHolder instead!!!
// Only AcquireDebuggerLock can call directly.
//
void Debugger::DoNotCallDirectlyPrivateLock(void)
{
    WRAPPER_CONTRACT;

    LOG((LF_CORDB,LL_INFO10000, "D::Lock aquire attempt by 0x%x\n",
        GetCurrentThreadId()));

    // Debugger lock is larger than both Controller & debugger-data locks.
    // So we should never try to take the D lock if we hold either of the others.


    // Lock becomes no-op in late shutdown.
    if (g_fProcessDetach)
    {
        return;
    }


    //
    // If the debugger has been disabled by the runtime, this means that it should block
    // all threads that are trying to travel thru the debugger.  We do this by blocking
    // threads as they try and take the debugger lock.
    //
    if (m_fDisabled)
    {
        __SwitchToThread(INFINITE);
        _ASSERTE (!"Can not reach here");
    }

    m_mutex.Enter();

    //
    // If we were blocked on the lock and the debugging facilities got disabled
    // while we were waiting, release the lock and park this thread.
    //
    if (m_fDisabled)
    {
        m_mutex.Leave();
        __SwitchToThread(INFINITE);
        _ASSERTE (!"Can not reach here");
    }

    //
    // Now check if we are in a shutdown case...
    //
    Thread * pThread = g_pEEInterface->GetThread();
    bool fIsCooperative = (pThread != NULL) && pThread->PreemptiveGCDisabled();
    if (m_fShutdownMode && !fIsCooperative)
    {
        // The big fear is that some other random thread will take the debugger-lock and then block on something else,
        // and thus prevent the helper/finalizer threads from taking the debugger-lock in shutdown scenarios.
        //
        // If we're in shutdown mode, then some locks (like the Thread-Store-Lock) get special semantics.
        // Only helper / finalizer / shutdown threads can actually take these locks.
        // Other threads that try to take them will just get parked and block forever.
        // This is ok b/c the only threads that need to run at this point are the Finalizer and Helper threads.
        //
        // We need to be in preemptive to block for shutdown, so we don't do this block in Coop mode.
        // Fortunately, it's safe to take this lock in coop mode because we know the thread can't block
        // on anything interesting because we're in a GC-forbid region (see crst flags).
        m_mutex.ReleaseAndBlockForShutdownIfNotSpecialThread();
    }



#ifdef _DEBUG
    _ASSERTE(m_mutexCount >= 0);

    if (m_mutexCount>0)
    {
        if (pThread)
        {
            // mamaged thread
            _ASSERTE(m_mutexOwner == GetThreadIdHelper(pThread));
        }
        else
        {
            // unmanaged thread
            _ASSERTE(m_mutexOwner == GetCurrentThreadId());
        }
    }

    m_mutexCount++;
    if (pThread)
    {
        m_mutexOwner = GetThreadIdHelper(pThread);
    }
    else
    {
        // unmanaged thread
        m_mutexOwner = GetCurrentThreadId();
    }

    if (m_mutexCount == 1)
    {
        LOG((LF_CORDB,LL_INFO10000, "D::Lock aquired by 0x%x\n", m_mutexOwner));
    }
#endif

}

// See comment above.
// Only ReleaseDebuggerLock can call directly.
void Debugger::DoNotCallDirectlyPrivateUnlock(void)
{
    WRAPPER_CONTRACT;

    // Controller lock is "smaller" than debugger lock.


    if (!g_fProcessDetach)
    {
#ifdef _DEBUG
        if (m_mutexCount == 1)
            LOG((LF_CORDB,LL_INFO10000, "D::Unlock released by 0x%x\n",
                m_mutexOwner));

        if(0 == --m_mutexCount)
            m_mutexOwner = 0;

        _ASSERTE( m_mutexCount >= 0);
#endif
        m_mutex.Leave();

        //
        // If the debugger has been disabled by the runtime, this means that it should block
        // all threads that are trying to travel thru the debugger.  We do this by blocking
        // threads also as they leave the debugger lock.
        //
        if (m_fDisabled)
        {
            __SwitchToThread(INFINITE);
            _ASSERTE (!"Can not reach here");
        }

    }
}

#if _DEBUG
static DebugEventCounter g_debugEventCounter;
static int g_iDbgRuntimeCounter[DBG_RUNTIME_MAX];
static int g_iDbgDebuggerCounter[DBG_DEBUGGER_MAX];

void DoAssertOnType(DebuggerIPCEventType event, int count)
{
    WRAPPER_CONTRACT;

    // check to see if we need fire the assertion or not.
    if ((event & 0x0300) == 0x0100)
    {
        // use the Runtime array
        if (g_iDbgRuntimeCounter[event & 0x00ff] == count)
        {
            char        tmpStr[256];
            sprintf(tmpStr, "%s == %d, break now!",
                     IPCENames::GetName(event), count);

            // fire the assertion
            DbgAssertDialog(__FILE__, __LINE__, tmpStr);
        }
    }
    // check to see if we need fire the assertion or not.
    else if ((event & 0x0300) == 0x0200)
    {
        // use the Runtime array
        if (g_iDbgDebuggerCounter[event & 0x00ff] == count)
        {
            char        tmpStr[256];
            sprintf(tmpStr, "%s == %d, break now!",
                     IPCENames::GetName(event), count);

            // fire the assertion
            DbgAssertDialog(__FILE__, __LINE__, tmpStr);
        }
    }

}
void DbgLogHelper(DebuggerIPCEventType event)
{
    WRAPPER_CONTRACT;

    switch (event)
    {
// we don't need to handle event type 0
#define IPC_EVENT_TYPE0(type, val)
#define IPC_EVENT_TYPE1(type, val)  case type: {\
                                        g_debugEventCounter.m_iDebugCount_##type++; \
                                        DoAssertOnType(type, g_debugEventCounter.m_iDebugCount_##type); \
                                        break; \
                                    }
#define IPC_EVENT_TYPE2(type, val)  case type: { \
                                        g_debugEventCounter.m_iDebugCount_##type++; \
                                        DoAssertOnType(type, g_debugEventCounter.m_iDebugCount_##type); \
                                        break; \
                                    }
#include "dbgipceventtypes.h"
#undef IPC_EVENT_TYPE2
#undef IPC_EVENT_TYPE1
#undef IPC_EVENT_TYPE0
            default:
                break;
    }
}
#endif // _DEBUG


#if defined(_DEBUG)
// Return true if a debugger is "invovled". See comments at prototype
// for what "invovled" means.
// We wnat to assert if codepaths should only be invoked if a
// debugger is / will be attached.
bool DebuggerIsInvolved()
{
    LEAF_CONTRACT;

    DebuggerAttachState eState = SYNC_STATE_0;
    if (g_pDebugger != NULL)
        eState = g_pDebugger->m_syncingForAttach;

    return (
        // trivial If we're already fully attached
        (CORDebuggerAttached() != 0) |

        // If we're launched by a debugger, then we know we'll be attached
        // and so it's fair game to preinitialize debugger-only structures.
        (CORLaunchedByDebugger() != 0) |

        // If we're in the process of attaching
        (eState > SYNC_STATE_0)
    );
}

#endif







/* ------------------------------------------------------------------------ *
 * DLL export routine
 * ------------------------------------------------------------------------ */

Debugger *CreateDebugger(void)
{
    Debugger *pDebugger = NULL;

    EX_TRY
    {
        pDebugger = new (nothrow) Debugger();
    }
    EX_CATCH
    {
        if (pDebugger != NULL)
        {
            delete pDebugger;
            pDebugger = NULL;
        }
    }
    EX_END_CATCH(RethrowTerminalExceptions);

    return pDebugger;
}

//
// CorDBGetInterface is exported to the Runtime so that it can call
// the Runtime Controller.
//
extern "C"{
HRESULT __cdecl CorDBGetInterface(DebugInterface** rcInterface)
{
    CONTRACT(HRESULT)
    {
        NOTHROW; // use HRESULTS instead
        GC_NOTRIGGER;
        POSTCONDITION(FAILED(RETVAL) || (rcInterface == NULL) || (*rcInterface != NULL));
    }
    CONTRACT_END;

    HRESULT hr = S_OK;

    if (rcInterface != NULL)
    {
        if (g_pDebugger == NULL)
        {
            LOG((LF_CORDB, LL_INFO10,
                 "CorDBGetInterface: initializing debugger.\n"));

            g_pDebugger = CreateDebugger();
            TRACE_ALLOC(g_pDebugger);

            if (g_pDebugger == NULL)
                hr = E_OUTOFMEMORY;
        }

        *rcInterface = g_pDebugger;
    }

    RETURN hr;
}
}

//-----------------------------------------------------------------------------
// Helpers to send IPC Notification Events (not IPC replies)
// See SENDIPCEVENT_BEGIN & SENDIPCEVENT_END for details on usage.
//-----------------------------------------------------------------------------
void PreSendEvent_Internal(SendEventData & event)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        CALLED_IN_DEBUGGERDATALOCK_HOLDER_SCOPE_MAY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    // Our caller will ensure the process is alive before sending the actual IPC event.
    // If we're in shutdown, then skip presend stuff and fall into our caller's check.
    // If we're the shutdown-thread, then this will avoid us blocking (good).
    // If we're not the shutdown-thread, it doesn't matter what we do b/c the shutdown
    // thread will asynchronously kill us.
    if (!CORDebuggerAttached())
    {
        return;
    }

    if (GetThread() != NULL)
    {
        event.fPreGCDisabled = g_pEEInterface->IsPreemptiveGCDisabled();

        if (event.fPreGCDisabled)
        {
            g_pEEInterface->EnablePreemptiveGC();
        }
    }

    g_pDebugger->LockForEventSending(event.pDbgLockHolder);
}

void PostSendEvent_Internal(SendEventData & event)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        CALLED_IN_DEBUGGERDATALOCK_HOLDER_SCOPE_MAY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    if (!CORDebuggerAttached())
    {
        return;
    }

    // Let other Runtime threads handle their events.
    g_pDebugger->UnlockFromEventSending(event.pDbgLockHolder);

    if (GetThread() != NULL)
    {
        // We'd better not be holding the debugger lock here. That's b/c we'll block
        // here waiting for the helper to handle events; and the helper thread will take the
        // lock, so it we held it here, we'd deadlock.
        _ASSERTE(!g_pDebugger->ThreadHoldsLock() || g_fProcessDetach);

        // If we called TrapAllRuntimeThreads, we would have tried to suspend the EE, thus
        // any thread entering cooperative mode (like this one) will block.
        // We'll resume when we get the continue call.
        g_pEEInterface->DisablePreemptiveGC();

        // Restore GC mode to whatever it was before.
        if (!event.fPreGCDisabled)
        {
            g_pEEInterface->EnablePreemptiveGC();
        }
    }
}

//-----------------------------------------------------------------------------
// Send a pre-init IPC event and block.
// We assume the IPC event has already been initialized. There's nothing special
// here; it just used the standard formula for sending an IPC event to the RS.
// This should match up w/ the description in SENDIPCEVENT_BEGIN.
//-----------------------------------------------------------------------------
void Debugger::SendSimpleIPCEventAndBlock(
    DebuggerLockHolder *pdbgLockHolder,
    IpcTarget iTarget) // = IPC_TARGET_OUTOFPROC by default
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    // BEGIN will acquire the lock (END will release it). While blocking, the
    // debugger may have detached though, so we need to check for that.
    _ASSERTE(ThreadHoldsLock());

    if (CORDebuggerAttached())
    {
        m_pRCThread->SendIPCEvent(iTarget);

        // Stop all Runtime threads
        this->TrapAllRuntimeThreads(pdbgLockHolder, NULL);
    }
}

//-----------------------------------------------------------------------------
// Get context from a thread in managed code.
// See header for exact semantics.
//-----------------------------------------------------------------------------
CONTEXT * GetManagedStoppedCtx(Thread * pThread)
{
    WRAPPER_CONTRACT;

    _ASSERTE(pThread != NULL);

    // We may be stopped or live.

    // If we're stopped at an interop-hijack, we'll have a filter context,
    // but we'd better not be redirected for a managed-suspension hijack.
    if (pThread->GetInteropDebuggingHijacked())
    {
        _ASSERTE(!ISREDIRECTEDTHREAD(pThread));
        return NULL;
    }

    // Check if we have a filter ctx. This should only be for managed-code.
    // We're stopped at some exception (likely an int3 or single-step).
    // Can't have both filter ctx + redirected ctx.
    CONTEXT *pCtx = g_pEEInterface->GetThreadFilterContext(pThread);
    if (pCtx != NULL)
    {
        _ASSERTE(!ISREDIRECTEDTHREAD(pThread));
        return pCtx;
    }

    if (ISREDIRECTEDTHREAD(pThread))
    {
        pCtx = GETREDIRECTEDCONTEXT(pThread);
        _ASSERTE(pCtx != NULL);
        return pCtx;
    }

    // Not stopped somewhere in managed code.
    return NULL;
}

//-----------------------------------------------------------------------------
// See header for exact semantics.
// Never NULL. (Caller guarantees this is active.)
//-----------------------------------------------------------------------------
CONTEXT * GetManagedLiveCtx(Thread * pThread)
{
    LEAF_CONTRACT;

    _ASSERTE(pThread != NULL);

    _ASSERTE(GetThread() == pThread);

    CONTEXT *pCtx = g_pEEInterface->GetThreadFilterContext(pThread);

    // Note that we may be in a M2U hijack. So we can't assert !pThread->GetInteropDebuggingHijacked()
    _ASSERTE(!ISREDIRECTEDTHREAD(pThread));
    _ASSERTE(pCtx);

    return pCtx;
}

// Attempt to validate a GC handle.
HRESULT ValidateGCHandle(OBJECTHANDLE oh)
{

    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    EX_TRY
    {
        // Use AVInRuntimeImplOkHolder.
        AVInRuntimeImplOkayHolder AVOkay(TRUE);

        // This may throw if the Object Handle is invalid.
        Object * objPtr = *((Object**) oh);

        // NULL is certinally valid...
        if (objPtr != NULL)
        {
            if (!objPtr->ValidateObjectWithPossibleAV())
            {
                LOG((LF_CORDB, LL_INFO10000, "GAV: object methodtable-class invariant doesn't hold.\n"));
                hr = E_INVALIDARG;
                goto LExit;
            }
        }

    LExit: ;
    }
    EX_CATCH
    {
        LOG((LF_CORDB, LL_INFO10000, "GAV: exception indicated ref is bad.\n"));
        hr = E_INVALIDARG;
    }
    EX_END_CATCH(SwallowAllExceptions);

    return hr;
}


// Validate an object. Returns E_INVALIDARG or S_OK.
HRESULT ValidateObject(Object *objPtr)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    EX_TRY
    {
        // Use AVInRuntimeImplOkHolder.
        AVInRuntimeImplOkayHolder AVOkay(TRUE);

        // NULL is certinally valid...
        if (objPtr != NULL)
        {
            if (!objPtr->ValidateObjectWithPossibleAV())
            {
                LOG((LF_CORDB, LL_INFO10000, "GAV: object methodtable-class invariant doesn't hold.\n"));
                hr = E_INVALIDARG;
                goto LExit;
            }
        }

    LExit: ;
    }
    EX_CATCH
    {
        LOG((LF_CORDB, LL_INFO10000, "GAV: exception indicated ref is bad.\n"));
        hr = E_INVALIDARG;
    }
    EX_END_CATCH(SwallowAllExceptions);

    return hr;
}   // ValidateObject


/* ------------------------------------------------------------------------ *
 * Debugger routines
 * ------------------------------------------------------------------------ */

//
// a Debugger object represents the global state of the debugger program.
//

//
// Constructor & Destructor
//

/******************************************************************************
 *
 ******************************************************************************/
Debugger::Debugger()
  :
#ifdef _DEBUG
    m_mutexCount(0),
#endif //_DEBUG
    m_pRCThread(NULL),
    m_trappingRuntimeThreads(FALSE),
    m_stopped(FALSE),
    m_unrecoverableError(FALSE),
    m_ignoreThreadDetach(FALSE),
    m_pMethodInfos(NULL),
    m_mutex("DebuggerMutex", CrstDebuggerMutex, (CrstFlags)(CRST_UNSAFE_ANYMODE | CRST_REENTRANCY | CRST_DEBUGGER_THREAD)),
#ifdef _DEBUG
    m_mutexOwner(0),
    m_tidLockedForEventSending(0),
#endif //_DEBUG
    m_syncingForAttach(SYNC_STATE_0),
    m_threadsAtUnsafePlaces(0),
    m_attachingForException(FALSE),
    m_exLock(0),
    m_LoggingEnabled(TRUE),
    m_pAppDomainCB(NULL),
    m_dClassLoadCallbackCount(0),
    m_pModules(NULL),
    m_debuggerAttached(FALSE),
    m_pIDbgThreadControl(NULL),
    m_forceNonInterceptable(FALSE),
    m_pLazyData(NULL)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        CONSTRUCTOR_CHECK;
    }
    CONTRACTL_END;

    m_fShutdownMode = false;
    m_fDisabled = false;

#ifdef _DEBUG
    InitDebugEventCounting();
#endif

    m_processId = GetCurrentProcessId();

    // Initialize these in ctor because we free them in dtor.
    // And we can't set them to some safe uninited value (like NULL).

}

/******************************************************************************
 *
 ******************************************************************************/
Debugger::~Debugger()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        DESTRUCTOR_CHECK;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    // We explicitly leak the debugger object on shutdown. See Debugger::StopDebugger for details.
    _ASSERTE(!"Debugger dtor should not be called.");   
}

// For debug-only builds, we'll have a debugging feature to count
// the number of ipc events and break on a specific number.
// Initialize the stuff to do that.
void Debugger::InitDebugEventCounting()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
#ifdef _DEBUG
    // initialize the debug event counter structure to zero
    memset(&g_debugEventCounter, 0, sizeof(DebugEventCounter));
    memset(&g_iDbgRuntimeCounter, 0, DBG_RUNTIME_MAX*sizeof(int));
    memset(&g_iDbgDebuggerCounter, 0, DBG_DEBUGGER_MAX*sizeof(int));

    // retrieve the possible counter for break point
    LPWSTR      wstrValue = NULL;
    // The string value is of the following format
    // <Event Name>=Count;<Event Name>=Count;....;
    // The string must end with ;
    if ((wstrValue = REGUTIL::GetConfigString(L"DebuggerBreakPoint")) != NULL)
    {
        LPSTR   strValue;
        int     cbReq;
        cbReq = WszWideCharToMultiByte(CP_UTF8, 0, wstrValue,-1, 0,0, 0,0);

        strValue = new (nothrow) char[cbReq+1];
        // This is a debug only thingy, if it fails, not worth taking
        // down the process.
        if (strValue == NULL)
            return;


        // now translate the unicode to ansi string
        WszWideCharToMultiByte(CP_UTF8, 0, wstrValue, -1, strValue, cbReq+1, 0,0);
        char *szEnd = (char *)strchr(strValue, ';');
        char *szStart = strValue;
        while (szEnd != NULL)
        {
            // Found a key value
            char    *szNameEnd = strchr(szStart, '=');
            int     iCount;
            DebuggerIPCEventType eventType;
            if (szNameEnd != NULL)
            {
                // This is a well form key
                *szNameEnd = '\0';
                *szEnd = '\0';

                // now szStart is the key name null terminated. Translate the counter into integer.
                iCount = atoi(szNameEnd+1);
                if (iCount != 0)
                {
                    eventType = IPCENames::GetEventType(szStart);

                    if (eventType < DB_IPCE_DEBUGGER_FIRST)
                    {
                        // use the runtime one
                        g_iDbgRuntimeCounter[eventType & 0x00ff] = iCount;
                    }
                    else if (eventType < DB_IPCE_DEBUGGER_LAST)
                    {
                        // use the debugger one
                        g_iDbgDebuggerCounter[eventType & 0x00ff] = iCount;
                    }
                    else
                        _ASSERTE(!"Unknown Event Type");
                }
            }
            szStart = szEnd + 1;
            // try to find next key value
            szEnd = (char *)strchr(szStart, ';');
        }

        // free the ansi buffer
        delete [] strValue;
        REGUTIL::FreeConfigString(wstrValue);
    }
#endif // _DEBUG
}


// This is a notification from the EE it's about to go to fiber mode.
// This is given *before* it actually goes to fiber mode.
HRESULT Debugger::SetFiberMode(bool isFiberMode)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;

        // Notifications from EE never come on helper worker.
        PRECONDITION(!ThisIsHelperThreadWorker());
    }
    CONTRACTL_END;


    Thread * pThread = ::GetThread();

    m_pRCThread->m_rgDCB[IPC_TARGET_OUTOFPROC]->m_bHostingInFiber = isFiberMode;

    // If there is a debugger already attached, then we have a big problem. As of V2.0, the debugger
    // does not support debugging processes with fibers in them. We set the unrecoverable state to
    // indicate that we're in a bad state now. The debugger will notice this, and take appropiate action.
    if (isFiberMode && CORDebuggerAttached())
    {
        LOG((LF_CORDB, LL_INFO10, "Thread has entered fiber mode while debugger attached.\n"));

        EX_TRY
        {

            SString szName(L"DebuggerFiberModeNotSupported");
            SString szDescription;
            szDescription.LoadResource( MDARC_DEBUGGER_FIBER_MODE_NOT_SUPPORTED );
            SString szXML(L"");

            // Sending any debug event will be a GC violation.
            // However, if we're enabling fiber-mode while a debugger is attached, we're already doomed.
            // Deadlocks and AVs are just around the corner. A Gc-violation is the least of our worries.
            // We want to at least notify the debugger at all costs.
            CONTRACT_VIOLATION(GCViolation);

            // As soon as we set unrecoverable error in the LS,  the RS will pick it up and basically shut down.
            // It won't dispatch any events. So we fire the MDA first, and then set unrecoverable error.
            SendMDANotification(pThread, &szName, &szDescription, &szXML, (CorDebugMDAFlags) 0, FALSE);

            CORDBDebuggerSetUnrecoverableError(this, CORDBG_E_CANNOT_DEBUG_FIBER_PROCESS, false);

            // Fire the MDA again just to force the RS to sniff the LS and pick up that we're in an unrecoverable error.
            // No harm done from dispatching an MDA twice. And
            SendMDANotification(pThread, &szName, &szDescription, &szXML, (CorDebugMDAFlags) 0, FALSE);

        }
        EX_CATCH
        {
            LOG((LF_CORDB, LL_INFO10, "Error sending MDA regarding fiber mode.\n"));
        }
        EX_END_CATCH(SwallowAllExceptions);
    }

    return S_OK;
}

// Checks if the MethodInfos table has been allocated, and if not does so.
// Throw on failure, so we always return
HRESULT Debugger::CheckInitMethodInfoTable()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (m_pMethodInfos == NULL)
    {
        DebuggerMethodInfoTable *pMethodInfos = NULL;

        EX_TRY
        {
            pMethodInfos = new (interopsafe) DebuggerMethodInfoTable();
        }
        EX_CATCH
        {
            pMethodInfos = NULL;
        }
        EX_END_CATCH(RethrowTerminalExceptions);


        if (pMethodInfos == NULL)
        {
            return E_OUTOFMEMORY;
        }

        if (InterlockedCompareExchangePointer((PVOID *)&m_pMethodInfos, (PVOID)pMethodInfos, NULL) != NULL)
        {
            DeleteInteropSafe(pMethodInfos);
        }
    }

    return S_OK;
}

// Checks if the m_pModules table has been allocated, and if not does so.
HRESULT Debugger::CheckInitModuleTable()
{
    CONTRACT(HRESULT)
    {
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(m_pModules != NULL);
    }
    CONTRACT_END;

    if (m_pModules == NULL)
    {
        DebuggerModuleTable *pModules = new (interopsafe, nothrow) DebuggerModuleTable();

        if (pModules == NULL)
        {
            RETURN (E_OUTOFMEMORY);
        }

        if (InterlockedCompareExchangePointer((PVOID *)&m_pModules, (PVOID)pModules, NULL) != NULL)
        {
            DeleteInteropSafe(pModules);
        }
    }

    RETURN (S_OK);
}

// Checks if the m_pModules table has been allocated, and if not does so.
HRESULT Debugger::CheckInitPendingFuncEvalTable()
{
    CONTRACT(HRESULT)
    {
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(GetPendingEvals() != NULL);
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE

    if (GetPendingEvals() == NULL)
    {
        DebuggerPendingFuncEvalTable *pPendingEvals = new (interopsafe, nothrow) DebuggerPendingFuncEvalTable();

        if (pPendingEvals == NULL)
        {
            RETURN(E_OUTOFMEMORY);
        }

        // Since we're setting, we need an LValue and not just an accessor.
        if (InterlockedCompareExchangePointer((PVOID *)&(GetLazyData()->m_pPendingEvals), (PVOID)pPendingEvals, NULL) != NULL)
        {
            DeleteInteropSafe(pPendingEvals);
        }
    }
#endif

    RETURN (S_OK);
}


#ifdef _DEBUG_DMI_TABLE
// Returns the number of (official) entries in the table
ULONG DebuggerMethodInfoTable::CheckDmiTable(void)
{
    LEAF_CONTRACT;

    ULONG cApparant = 0;
    ULONG cOfficial = 0;

    if (NULL != m_pcEntries)
    {
        DebuggerMethodInfoEntry *dcp;
        int i = 0;
        while (i++ <m_iEntries)
        {
            dcp = (DebuggerMethodInfoEntry*)&(((DebuggerMethodInfoEntry *)m_pcEntries)[i]);
            if(dcp->pFD != 0 &&
               dcp->pFD != (MethodDesc*)0xcdcdcdcd &&
               dcp->mi != NULL)
            {
                cApparant++;

                _ASSERTE( dcp->pFD == dcp->mi->m_fd );
                LOG((LF_CORDB, LL_INFO1000, "DMIT::CDT:Entry:0x%p mi:0x%p\nPrevs:\n",
                    dcp, dcp->mi));
                DebuggerMethodInfo *dmi = dcp->mi->m_prevMethodInfo;

                while(dmi != NULL)
                {
                    LOG((LF_CORDB, LL_INFO1000, "\t0x%p\n", dmi));
                    dmi = dmi->m_prevMethodInfo;
                }
                dmi = dcp->mi->m_nextMethodInfo;

                LOG((LF_CORDB, LL_INFO1000, "Nexts:\n", dmi));
                while(dmi != NULL)
                {
                    LOG((LF_CORDB, LL_INFO1000, "\t0x%p\n", dmi));
                    dmi = dmi->m_nextMethodInfo;
                }

                LOG((LF_CORDB, LL_INFO1000, "DMIT::CDT:DONE\n",
                    dcp, dcp->mi));
            }
        }

        if (m_piBuckets == 0)
        {
            LOG((LF_CORDB, LL_INFO1000, "DMIT::CDT: The table is officially empty!\n"));
            return cOfficial;
        }

        LOG((LF_CORDB, LL_INFO1000, "DMIT::CDT:Looking for official entries:\n"));

        ULONG iNext = m_piBuckets[0];
        ULONG iBucket = 1;
        HASHENTRY   *psEntry = NULL;
        while (TRUE)
        {
            while (iNext != UINT32_MAX)
            {
                cOfficial++;

                psEntry = EntryPtr(iNext);
                dcp = ((DebuggerMethodInfoEntry *)psEntry);

                LOG((LF_CORDB, LL_INFO1000, "\tEntry:0x%p mi:0x%p @idx:0x%x @bucket:0x%x\n",
                    dcp, dcp->mi, iNext, iBucket));

                iNext = psEntry->iNext;
            }

            // Advance to the next bucket.
            if (iBucket < m_iBuckets)
                iNext = m_piBuckets[iBucket++];
            else
                break;
        }

        LOG((LF_CORDB, LL_INFO1000, "DMIT::CDT:Finished official entries: ****************"));
    }

    return cOfficial;
}
#endif // _DEBUG_DMI_TABLE


#ifdef _DEBUG
// Thread proc for interop stress coverage. Have an unmanaged thread
// that just loops throwing native exceptions. This can test corner cases
// such as getting an native exception while the runtime is synced.
DWORD WINAPI DbgInteropStressProc(void * lpParameter)
{
    LEAF_CONTRACT;

    int i = 0;
    int zero = 0;


    // This will ensure that the compiler doesn't flag our 1/0 exception below at compile-time.
    if (lpParameter != NULL)
    {
        zero = 1;
    }

    // Note that this thread is a non-runtime thread. So it can't take any CLR locks
    // or do anything else that may block the helper thread.
    // (Log statements take CLR locks).
    while(true)
    {
        i++;

        if ((i % 10) != 0)
        {
            // Generate an in-band event.
            PAL_CPP_TRY
            {
                // Throw a handled exception. Don't use an AV since that's pretty special.
                *(int*)lpParameter = 1 / zero;
            }
            PAL_CPP_CATCH_ALL
            {
            }
            PAL_CPP_ENDTRY
        }
        else
        {
            // Generate the occasional oob-event.
            WszOutputDebugString(L"Ping from DbgInteropStressProc");
        }

        // This helps parallelize if we have a lot of threads, and keeps us from
        // chewing too much CPU time.
        ClrSleepEx(2000,FALSE);
        ClrSleepEx(rand() % 1000, FALSE);
    }

    return 0;
}

// ThreadProc that does everything in a can't stop region.
DWORD WINAPI DbgInteropCantStopStressProc(void * lpParameter)
{
    WRAPPER_CONTRACT;

    // This will mark us as a can't stop region.
    ClrFlsSetThreadType (ThreadType_DbgHelper);

    return DbgInteropStressProc(lpParameter);
}

// Generate lots of OOB events.
DWORD WINAPI DbgInteropDummyStressProc(void * lpParameter)
{
    LEAF_CONTRACT;

    ClrSleepEx(1,FALSE);
    return 0;
}

DWORD WINAPI DbgInteropOOBStressProc(void * lpParameter)
{
    WRAPPER_CONTRACT;

    int i = 0;
    while(true)
    {
        i++;
        if (i % 10 == 1)
        {
            // Create a dummy thread. That generates 2 oob events
            // (1 for create, 1 for destroy)
            DWORD id;
            ::CreateThread(NULL, 0, DbgInteropDummyStressProc, NULL, 0, &id);
        }
        else
        {
            // Generate the occasional oob-event.
            WszOutputDebugString(L"OOB ping from ");
        }

        ClrSleepEx(3000, FALSE);
    }
}

// List of the different possible stress procs.
LPTHREAD_START_ROUTINE g_pStressProcs[] =
{
    DbgInteropOOBStressProc,
    DbgInteropCantStopStressProc,
    DbgInteropStressProc
};
#endif


DebuggerHeap * Debugger::GetInteropSafeHeap()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Lazily initialize our heap.
    if (!m_heap.IsInit())
    {
        _ASSERTE(!"InteropSafe Heap should have already been initialized in LazyInit");

        // Just in case we miss it in retail, convert to OOM here:
        ThrowOutOfMemory();
    }

    return &m_heap;
}

DebuggerHeap * Debugger::GetInteropSafeHeap_NoThrow()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Lazily initialize our heap.
    if (!m_heap.IsInit())
    {
        _ASSERTE(!"InteropSafe Heap should have already been initialized in LazyInit");

        // Just in case we miss it in retail, convert to OOM here:
        return NULL;        
    }
    return &m_heap;
}

//
// Startup initializes any necessary debugger objects, including creating
// and starting the Runtime Controller thread. Once the RC thread is started
// and we return successfully, the Debugger object can expect to have its
// event handlers called.
/*******************************************************************************/


HRESULT Debugger::Startup(void)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("Debugger");

    HRESULT hr = S_OK;
    _ASSERTE(g_pEEInterface != NULL);

    DebuggerLockHolder dbgLockHolder(this);


    // Stubs in Stacktraces are always enabled.
    g_EnableSIS = true;

    // We can get extra Interop-debugging test coverage by having some auxillary unmanaged
    // threads running and throwing debug events. Keep these stress procs separate so that
    // we can focus on certain problem areas.
#ifdef _DEBUG

    g_DbgShouldntUseDebugger = REGUTIL::GetConfigDWORD(L"DbgNoDebugger", 0) != 0;


    // Creates random thread procs.
    DWORD n = REGUTIL::GetConfigDWORD(L"DbgExtraThreads", 0);
    if (n > 0)
    {
        for(DWORD i = 0; i < n; i++)
        {
            int iProc = rand() % NumItems(g_pStressProcs);
            LPTHREAD_START_ROUTINE p = g_pStressProcs[iProc];
            DWORD id;
            ::CreateThread(NULL, 0, p, NULL, 0, &id);
            LOG((LF_CORDB, LL_INFO1000, "Created random thread (%d) with tid=0x%x\n", i, id));
        }
    }

    DWORD n1 = REGUTIL::GetConfigDWORD(L"DbgExtraThreadsIB", 0);
    if (n1 > 0)
    {
        for(DWORD i = 0; i < n1; i++)
        {
            DWORD id;
            ::CreateThread(NULL, 0, DbgInteropStressProc, NULL, 0, &id);
            LOG((LF_CORDB, LL_INFO1000, "Created extra thread (%d) with tid=0x%x\n", i, id));
        }
    }

    DWORD n2 = REGUTIL::GetConfigDWORD(L"DbgExtraThreadsCantStop", 0);
    if (n2 > 0)
    {
        for(DWORD i = 0; i < n2; i++)
        {
            DWORD id;
            ::CreateThread(NULL, 0, DbgInteropCantStopStressProc, NULL, 0, &id);
            LOG((LF_CORDB, LL_INFO1000, "Created extra thread 'can't-stop' (%d) with tid=0x%x\n", i, id));
        }
    }

    DWORD n3 = REGUTIL::GetConfigDWORD(L"DbgExtraThreadsOOB", 0);
    if (n3 > 0)
    {
        for(DWORD i = 0; i < n3; i++)
        {
            DWORD id;
            ::CreateThread(NULL, 0, DbgInteropOOBStressProc, NULL, 0, &id);
            LOG((LF_CORDB, LL_INFO1000, "Created extra thread OOB (%d) with tid=0x%x\n", i, id));
        }
    }
#endif


    // Lazily initialize the interop-safe heap

    // Must be done before the RC thread is initialized.
    // The helper thread will be able to determine if someone was trying
    // to attach before the runtime was loaded and set the appropriate
    // flags that will cause CORDebuggerAttached to return true
    if (CORLaunchedByDebugger())
    {
        LazyInit();
        DebuggerController::Initialize();
    }

    // Create the runtime controller thread, a.k.a, the debug helper thread.
    // Don't use the interop-safe heap b/c we don't want to lazily create it.
    m_pRCThread = new DebuggerRCThread(this);
    _ASSERTE(m_pRCThread != NULL); // throws on oom
    TRACE_ALLOC(m_pRCThread);

    hr = m_pRCThread->Init();
    _ASSERTE(SUCCEEDED(hr)); // throws on error

    // See if we need to spin up the helper thread now, rather than later.
    DebuggerIPCControlBlock* dcb = m_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC);
    if (!dcb->m_rightSideShouldCreateHelperThread)
    {
        // Create the win32 thread for the helper and let it run free.
        hr = m_pRCThread->Start();

        // convert failure to exception as with old contract
        if (FAILED(hr))
        {
            ThrowHR(hr);
        }

        LOG((LF_CORDB, LL_EVERYTHING, "Start was successful\n"));
    }

    // Also initialize the AppDomainEnumerationIPCBlock
    m_pAppDomainCB = g_pIPCManagerInterface->GetAppDomainBlock();
    if (m_pAppDomainCB == NULL)
    {
       LOG((LF_CORDB, LL_INFO100, "D::S: Failed to get AppDomain IPC block from IPCManager.\n"));
       ThrowHR(E_FAIL);
    }

    hr = InitAppDomainIPC();
    _ASSERTE(SUCCEEDED(hr)); // throws on error.

    // We don't bother changing this process's permission.
    // A managed debugger will have the SE_DEBUG permission which will allow it to open our process handle,
    // even if we're a guest account.


    // If we're launching, then sync now so that the RS gets an early chance to dispatch the CreateProcess event.
    // This is especially important b/c certain portions of the ICorDebugAPI (like setting ngen flags) are only
    // valid during the CreateProcess callback in the launch case.
    // We need to send the callback early enough so those APIs can set the flags before they're actually used.
    // Note that CORLaunchedByDebugger errs on the side or returning true (it effectively checks an env var,
    // and that may have been inherited) so we also ensure the debugger is actually attached.
    if (SUCCEEDED(hr) && CORLaunchedByDebugger() && CORDebuggerAttached())
    {
        DebuggerLockHolder lockHolder(this, FALSE);
        SENDIPCEVENT_BEGIN(&lockHolder);

        // Send a nop event. This will let the RS know that the IPC block is up + ready,
        // and then the RS can read it.
        // The RS will then update the DCB with enough information so that we can send the sync-complete.
        // This nop event ensures that the sync-complete is not the first event we send over.
        // We don't send a create-process event here because the RS alread has that event pre-queued.
        DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce, DB_IPCE_PING_ON_LAUNCH, NULL, LSPTR_APPDOMAIN::NullPtr());
        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

        // The RS has already queued the CreateProcess event, so we just need to sync the CLR.
        TrapAllRuntimeThreads(&lockHolder, NULL);

        SENDIPCEVENT_END;
    }

    // Must release the lock (which would be done at the end of this method anyways) so that
    // the helper thread can do the jit-attach.
    dbgLockHolder.Release();


#ifdef _DEBUG
    // Give chance for stress harnesses to launch a managed debugger when a managed app starts up.
    // This lets us run a set of managed apps under a debugger.
    if (!CORDebuggerAttached())
    {
        #define DBG_ATTACH_ON_STARTUP_ENV_VAR L"COMPlus_DbgAttachOnStartup"

        // We explicitly just check the env because we don't want a switch this invasive to be global.
        DWORD fAttach = WszGetEnvironmentVariable(DBG_ATTACH_ON_STARTUP_ENV_VAR, NULL, 0) > 0;
        if (fAttach)
        {
            // Remove the env var from our process so that the debugger we spin up won't inherit it.
            // Else, if the debugger is managed, we'll have an infinite recursion.
            BOOL fOk = WszSetEnvironmentVariable(DBG_ATTACH_ON_STARTUP_ENV_VAR, NULL);
            if (fOk)
            {
                // We've already created the helper thread (which can service the attach request)
                // So just do a normal jit-attach now.

                SString szName(L"DebuggerStressStartup");
                SString szDescription(L"MDA used for debugger-stress scenario. This is fired to trigger a jit-attach"
                    L"to allow us to attach a debugger to any managed app that starts up."
                    L"This MDA is only fired when the 'DbgAttachOnStartup' COM+ knob/reg-key is set on checked builds.");
                SString szXML(L"<xml>See the description</xml>");

                SendMDANotification(
                    NULL, // NULL b/c we don't have a thread yet
                    &szName,
                    &szDescription,
                    &szXML,
                    ((CorDebugMDAFlags) 0 ),
                    TRUE // this will force the jit-attach
                );
            }
        }
    }
#endif


    return hr;
}

/******************************************************************************
Lazy initialize stuff once we know we are debugging.
This reduces the startup cost in the non-debugging case.

We can do this at a bunch of random strategic places.
 ******************************************************************************/

HRESULT Debugger::LazyInitWrapper()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(ThisMaybeHelperThread());
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    // Do lazy initialization now.
    EX_TRY
    {
        LazyInit(); // throws on errors.
    }
    EX_CATCH
    {
        Exception *_ex = GET_EXCEPTION();
        hr = _ex->GetHR();
        STRESS_LOG1(LF_CORDB, LL_ALWAYS, "LazyInit failed w/ hr:0x%08x\n", hr);
    }
    EX_END_CATCH(SwallowAllExceptions);

    return hr;
}

void Debugger::LazyInit()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(ThreadHoldsLock()); // ensure we're serialized, requires GC_NOTRIGGER

        PRECONDITION(ThisMaybeHelperThread());
    }
    CONTRACTL_END;

    // Have knob that catches places where we lazy init.
    _ASSERTE(!g_DbgShouldntUseDebugger);

    // If we're already init, then bail.
    if (m_pLazyData != NULL)
    {
        return;
    }




    // Lazily create our heap.
    HRESULT hr = m_heap.Init();
    IfFailThrow(hr);

    m_pLazyData = new (interopsafe) DebuggerLazyInit();
    _ASSERTE(m_pLazyData != NULL); // throws on oom.

    m_pLazyData->Init();

}

HelperThreadFavor::HelperThreadFavor() :
    m_fpFavor(NULL),
    m_pFavorData(NULL),
    m_FavorReadEvent(NULL),
    m_FavorLock("FavorLock", CrstDebuggerFavorLock, CRST_DEFAULT),
    m_FavorAvailableEvent(NULL)
{
}

void HelperThreadFavor::Init()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(ThisMaybeHelperThread());
    }
    CONTRACTL_END;

    // Create events for managing favors.
    m_FavorReadEvent      = CreateWin32EventOrThrow(NULL, kAutoResetEvent, FALSE);
    m_FavorAvailableEvent = CreateWin32EventOrThrow(NULL, kAutoResetEvent, FALSE);
}



DebuggerLazyInit::DebuggerLazyInit() :
    m_pPendingEvals(NULL),
    //
    m_DebuggerDataLock("DebuggerDataLock", CrstDebuggerJitInfo, (CrstFlags)(CRST_UNSAFE_ANYMODE | CRST_REENTRANCY | CRST_DEBUGGER_THREAD)),
    m_CtrlCMutex(NULL),
    m_exAttachEvent(NULL),
    m_exUnmanagedAttachEvent(NULL),
    m_exAttachAbortEvent(NULL),
    m_DebuggerHandlingCtrlC(NULL)
{
}

void DebuggerLazyInit::Init()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(ThisMaybeHelperThread());
    }
    CONTRACTL_END;

    // Caller ensures this isn't double-called.

    // Create some synchronization events...

    m_exAttachEvent         = CreateWin32EventOrThrow(NULL, kManualResetEvent, FALSE);
    m_exAttachAbortEvent    = CreateWin32EventOrThrow(NULL, kManualResetEvent, FALSE);
    m_CtrlCMutex            = CreateWin32EventOrThrow(NULL, kAutoResetEvent, FALSE);

    //
    SECURITY_ATTRIBUTES* pSA = NULL;
    m_exUnmanagedAttachEvent = CreateWin32EventOrThrow(pSA, kManualResetEvent, FALSE);

    m_DebuggerHandlingCtrlC = FALSE;


    // Let the helper thread lazy init stuff too.
    m_RCThread.Init();
}


DebuggerLazyInit::~DebuggerLazyInit()
{
    {
        USHORT cBlobs = m_pMemBlobs.Count();
        BYTE **rgpBlobs = m_pMemBlobs.Table();

        for (int i = 0; i < cBlobs; i++)
        {
            g_pDebugger->ReleaseRemoteBuffer(rgpBlobs[i], false);
        }
    }

    if (m_pPendingEvals)
    {
        DeleteInteropSafe(m_pPendingEvals);
        m_pPendingEvals = NULL;
    }

    if (m_CtrlCMutex != NULL)
    {
        CloseHandle(m_CtrlCMutex);
    }

    if (m_exAttachEvent != NULL)
    {
        CloseHandle(m_exAttachEvent);
    }

    if (m_exUnmanagedAttachEvent != NULL)
    {
        CloseHandle(m_exUnmanagedAttachEvent);
    }

    if (m_exAttachAbortEvent != NULL)
    {
        CloseHandle(m_exAttachAbortEvent);
    }

}


//
// RequestFavor gets the debugger helper thread to call a function. It's
// typically called when the current thread can't call the function directly,
// e.g, there isn't enough stack space.
//
// RequestFavor can be called in stack-overflow scenarios and thus explicitly
// avoids any lazy initialization.
// It blocks until the favor callback completes. 
//
// Parameters:
//   fp    - a non-null Favour callback function
//   pData - the parameter passed to the favor callback function. This can be any value.
//
// Return values:
//   S_OK if the function succeeds, else a failure HRESULT
//   

HRESULT Debugger::RequestFavor(FAVORCALLBACK fp, void * pData)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(fp != NULL);
    }
    CONTRACTL_END;
    
    if (m_pRCThread == NULL || 
        m_pRCThread->GetRCThreadId() == GetCurrentThreadId())
    {
        // Since favors are only used internally, we know that the helper should alway be up and ready
        // to handle them. Also, since favors can be used in low-stack scenarios, there's not any 
        // extra initialization needed for them.
        _ASSERTE(!"Helper not initialized for favors.");
        return E_UNEXPECTED;
    }

    m_pRCThread->DoFavor(fp, pData);
    return S_OK;
}

/******************************************************************************
// Called to set the interface that the Runtime exposes to us.
 ******************************************************************************/
void Debugger::SetEEInterface(EEDebugInterface* i)
{
    LEAF_CONTRACT;

    // @@@

    // Implements DebugInterface API

    g_pEEInterface = i;

}


/******************************************************************************
// Called to shut down the debugger. This stops the RC thread and cleans
// the object up.
 ******************************************************************************/
void Debugger::StopDebugger(void)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
   
    // Leak almost everything on process exit. The OS will clean it up anyways and trying to 
    // clean it up ourselves is just one more place we may AV / deadlock.

    // Ping the helper thread to exit. This will also prevent the helper from servicing new requests.
    if (m_pRCThread != NULL)
    {
        m_pRCThread->AsyncStop();
    }


    // Also clean up the AppDomain stuff since this is cross-process.
    TerminateAppDomainIPC ();


    //
    // Tell the VM to clear out all references to the debugger before we start cleaning up, 
    // so that nothing will reference (accidentally) through the partially cleaned up debugger.
    //
    // NOTE: we cannot clear out g_pDebugger before the delete call because the 
    // stuff in delete (particularly deleteinteropsafe) needs to look at it.
    // 
    g_pEEInterface->ClearAllDebugInterfaceReferences();
    g_pDebugger = NULL;
}


/* ------------------------------------------------------------------------ *
 * JIT Interface routines
 * ------------------------------------------------------------------------ */


/******************************************************************************
 *
 ******************************************************************************/
DebuggerMethodInfo *Debugger::CreateMethodInfo(Module *module, mdMethodDef md)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;

        PRECONDITION(HasDebuggerDataLock());
    }
    CONTRACTL_END;


    //
    DebuggerMethodInfo *mi = new (interopsafe) DebuggerMethodInfo(module, md);
    _ASSERTE(mi != NULL); // throws on oom error

    TRACE_ALLOC(mi);

    LOG((LF_CORDB, LL_INFO100000, "D::CreateMethodInfo module=%p, token=0x%08x, info=%p\n",
        module, md, mi));

    //
    HRESULT hr;
    hr =InsertToMethodInfoList(mi);

    if (FAILED(hr))
    {
        LOG((LF_CORDB, LL_EVERYTHING, "IAHOL Failed!!\n"));
        DeleteInteropSafe(mi);
        return NULL;
    }
    return mi;

}





/******************************************************************************
//                                                               
 ******************************************************************************/
void Debugger::JITBeginning(MethodDesc* fd, bool trackJITInfo)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS_FROM_GETJITINFO;
    }
    CONTRACTL_END;
}



/******************************************************************************
// void Debugger::JITComplete():   JITComplete is called by
// the jit interface when the JIT completes, successfully or not.
//
// MethodDesc* fd:  MethodDesc of the code that's been JITted
// BYTE* newAddress:  The address of that the method begins at.
//          If newAddress is NULL then the JIT failed. Remember that this
//          gets called before the start address of the MethodDesc gets set,
//          and so methods like GetFunctionAddress & GetFunctionSize won't work.
// SIZE_T sizeOfCode: size of the generated code
//
//                                                                       
 ******************************************************************************/
void Debugger::JITComplete(MethodDesc* fd, BYTE* newAddress, SIZE_T sizeOfCode,
                           DebugInterface::JITCodeInfo jitCodeInfo)
{

    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        PRECONDITION(!HasDebuggerDataLock());
        CALLED_IN_DEBUGGERDATALOCK_HOLDER_SCOPE_MAY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    // @@@
    // Can be called on managed thread only
    // This API Implements DebugInterface

    // Jit failed. Don't create a new DJI or map patches.
    // Since we don't even have a partially initialized DJI,we don't need to do much in this failure case.
    if ((newAddress == 0) && (sizeOfCode == 0))
    {
        goto Exit;
    }


    if (CORDebuggerAttached())
    {
        // Populate the debugger's cache of DJIs. Normally we can do this lazily,
        // the only reason we do it here is b/c the MethodDesc is not yet officially marked as "jitted",
        // and so we can't lazily create it yet. Furthermore, the binding operations may need the DJIs.
        //
        // This also gives the debugger a chance to know if new JMC methods are coming.
        DebuggerMethodInfo * dmi = GetOrCreateMethodInfo(fd->GetModule(), fd->GetMemberDef());
        if (dmi == NULL)
        {
            goto Exit;
        }
        DebuggerJitInfo * ji = dmi->CreateInitAndAddJitInfo(fd, (TADDR) newAddress, sizeOfCode);

        // Bind any IL patches to the newly jitted native code.
        HRESULT hr;
        hr = MapAndBindFunctionPatches(ji, fd, (CORDB_ADDRESS_TYPE *)newAddress);
        _ASSERTE(SUCCEEDED(hr));
    }

    LOG((LF_CORDB, LL_EVERYTHING, "JitComplete completed successfully\n"));

Exit:
    ;
}

/******************************************************************************
// void Debugger::PitchCode()  This is called when the
// FJIT tosses some code out - we should do all the work
// we need to in order to prepare the function to have
// it's native code removed
// MethodDesc * fd:  MethodDesc of the method to be pitched
// CORDB_ADDRESS_TYPE *pbAddr:
 ******************************************************************************/
void Debugger::PitchCode( MethodDesc *fd, const BYTE *pbAddr )
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_TRIGGERS_FROM_GETJITINFO;
        PRECONDITION(CheckPointer(fd));
    }
    CONTRACTL_END;

    LOG((LF_CORDB,LL_INFO10000,"D:PC: Pitching method %s::%s 0x%p\n",
        fd->m_pszDebugClassName, fd->m_pszDebugMethodName,fd ));

    //ask for the JitInfo no matter what it's state
    DebuggerJitInfo *ji = GetJitInfo( fd, (const BYTE *)pbAddr );

    if ( ji != NULL )
    {
        LOG((LF_CORDB,LL_INFO10000,"De::PiCo: For addr 0x%p, got "
            "DJI 0x%p, from 0x%p, size:0x%p\n",pbAddr, ji,
            ji->m_addrOfCode, ji->m_sizeOfCode));

        DebuggerController::UnbindFunctionPatches( fd );
        ji->m_jitComplete = false;
        ji->m_codePitched = true;
        ji->m_addrOfCode = (CORDB_ADDRESS)NULL;
        ji->m_sizeOfCode = 0;
    }
}

/******************************************************************************
// void Debugger::MovedCode():  This is called when the
// code has been moved.  Currently, code that the FJIT doesn't
// pitch, it moves to another spot, and then tells us about here.
// This method should be called after the code has been copied
// over, but while the original code is still present, so we can
// change the original copy (removing patches & such).
// Note that since the code has already been moved, we need to
// save the opcodes for the rebind.
// MethodDesc * fd:  MethodDesc of the method to be pitched
// CORDB_ADDRESS_TYPE * pbNewAddress:  Address that it's being moved to
 ******************************************************************************/
void Debugger::MovedCode( MethodDesc *fd, CORDB_ADDRESS_TYPE *pbOldAddress,
    CORDB_ADDRESS_TYPE *pbNewAddress)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_TRIGGERS_FROM_GETJITINFO;
        PRECONDITION(CheckPointer(fd));
        PRECONDITION(CheckPointer(pbOldAddress));
        PRECONDITION(CheckPointer(pbNewAddress));
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000, "De::MoCo: %s::%s moved from 0x%8x to 0x%8x\n",
        fd->m_pszDebugClassName, fd->m_pszDebugMethodName,
        pbOldAddress, pbNewAddress ));

    DebuggerJitInfo *ji = GetJitInfo(fd, (const BYTE *)pbOldAddress);
    if( ji != NULL )
    {
        LOG((LF_CORDB,LL_INFO10000,"De::MoCo: For code 0x%p, got DJI 0x%p, "
            "from 0x%p to 0x%p\n", pbOldAddress, ji, ji->m_addrOfCode,
            ji->m_addrOfCode+ji->m_sizeOfCode));

        ji->m_addrOfCode = (ULONG_PTR)PTR_TO_CORDB_ADDRESS(pbNewAddress);
    }

    DebuggerController::UnbindFunctionPatches( fd, true);
    DebuggerController::BindFunctionPatches(fd, (CORDB_ADDRESS_TYPE *)pbNewAddress);
}

/******************************************************************************
 *
 ******************************************************************************/
SIZE_T Debugger::GetArgCount(MethodDesc *fd,BOOL *fVarArg)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Create a MetaSig for the given method's sig. (Eaiser than
    // picking the sig apart ourselves.)
    PCCOR_SIGNATURE pCallSig;
    DWORD cbCallSigSize;

    fd->GetSig(&pCallSig, &cbCallSigSize);

    if (pCallSig == NULL)
    {
        // Sig should only be null if the image is corrupted. (Even for lightweight-codegen)
        // We expect the jit+verifier to catch this, so that we never land here.
        // But just in case ...
        CONSISTENCY_CHECK_MSGF(false, ("Corrupted image, null sig.(%s::%s)", fd->m_pszDebugClassName, fd->m_pszDebugMethodName));
        return 0;
    }

    MetaSig msig(pCallSig, cbCallSigSize, g_pEEInterface->MethodDescGetModule(fd), NULL, NULL, MetaSig::sigMember);

    // Get the arg count.
    UINT32 NumArguments = msig.NumFixedArgs();

    // Account for the 'this' argument.
    if (!(g_pEEInterface->MethodDescIsStatic(fd)))
        NumArguments++;

    // Is this a VarArg's function?
    if (msig.IsVarArg() && fVarArg != NULL)
    {
        NumArguments++;
        *fVarArg = true;
    }

    return NumArguments;
}

#endif // #ifndef DACCESS_COMPILE





/******************************************************************************
    DebuggerJitInfo * Debugger::GetJitInfo():   GetJitInfo
    will return a pointer to a DebuggerJitInfo.  If the DJI
    doesn't exist, or it does exist, but the method has actually
    been pitched (and the caller wants pitched methods filtered out),
    then we'll return NULL.

    Note: This will also create a DMI for if one does not exist for this DJI.

    MethodDesc* fd:  MethodDesc for the method we're interested in.
    CORDB_ADDRESS_TYPE * pbAddr:  Address within the code, to indicate which
            version we want.  If this is NULL, then we want the
            head of the DebuggerJitInfo list, whether it's been
            JITted or not.
 ******************************************************************************/


// Get a DJI from an address.
DebuggerJitInfo *Debugger::GetJitInfoFromAddr(TADDR addr)
{
    WRAPPER_CONTRACT;

    MethodDesc *fd;
    fd = g_pEEInterface->GetNativeCodeMethodDesc((const BYTE *)addr);
    _ASSERTE(fd);

    return GetJitInfo(fd, (const BYTE*) addr, NULL);
}

DebuggerJitInfo *Debugger::GetLatestJitInfoFromMethodDesc(MethodDesc * pMethodDesc)
{
    WRAPPER_CONTRACT;

    _ASSERTE(pMethodDesc != NULL);
    // We'd love to assert that we're jitted; but since this may be in the JitComplete
    // callback path, we can't be sure.

    return GetJitInfoWorker(pMethodDesc, NULL, NULL);
}


DebuggerJitInfo *Debugger::GetJitInfo(MethodDesc *fd, const BYTE *pbAddr, DebuggerMethodInfo **pMethInfo )
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(!g_pDebugger->HasDebuggerDataLock());
    }
    CONTRACTL_END;

    // Address should be non-null and in range of MethodDesc. This lets us tell which EnC version.
    _ASSERTE(pbAddr != NULL); 

    return GetJitInfoWorker(fd, pbAddr, pMethInfo);

}

// Internal worker to GetJitInfo. Doesn't validate parameters.
DebuggerJitInfo *Debugger::GetJitInfoWorker(MethodDesc *fd, const BYTE *pbAddr, DebuggerMethodInfo **pMethInfo)
{

    DebuggerMethodInfo *dmi = NULL;
    DebuggerJitInfo *dji = NULL;

    // If we have a null MethodDesc - we're not going to get a jit-info. Do this check once at the top
    // rather than littered throughout the rest of this function.
    if (fd == NULL)
    {
        LOG((LF_CORDB, LL_EVERYTHING, "Debugger::GetJitInfo, addr=0x%p - null fd - returning null\n", pbAddr));
        return NULL;
    }
    else
    {
        CONSISTENCY_CHECK_MSGF(!fd->IsWrapperStub(), ("Can't get Jit-info for wrapper MDesc,'%s'", fd->m_pszDebugMethodName));
    }

    // The debugger doesn't track Lightweight-codegen methods b/c they have no metadata.
    if (fd->IsDynamicMethod())
    {
        return NULL;
    }


    // initialize our out param
    if (pMethInfo)
    {
        *pMethInfo = NULL;
    }

    LOG((LF_CORDB, LL_EVERYTHING, "Debugger::GetJitInfo called\n"));
//    CHECK_DJI_TABLE_DEBUGGER;

    // Find the DJI via the DMI
    //
    // One way to improve the perf, both in terms of memory usage, number of allocations
    // and lookup speeds would be to have the first JitInfo inline in the MethodInfo
    // struct.  After all, we never want to have a MethodInfo in the table without an
    // associated JitInfo, and this should bring us back very close to the old situation
    // in terms of perf.  But correctness comes first, and perf later...
    //        CHECK_DMI_TABLE;
    dmi = GetOrCreateMethodInfo(fd->GetModule(), fd->GetMemberDef());

    if (dmi == NULL)
    {
        // If we can't create the DMI, we won't be able to create the DJI.
        return NULL;
    }


    // This may take the lock and lazily create an entry, so we do it up front.
    dji = dmi->GetLatestJitInfo(fd);


    DebuggerDataLockHolder debuggerDataLockHolder(this);

    // Note the call to GetLatestJitInfo() will lazily create the first DJI if we don't already have one.
    for (; dji != NULL; dji = dji->m_prevJitInfo)
    {
        if (PTR_TO_TADDR(dji->m_fd) == PTR_HOST_TO_TADDR(fd))
        {
            break;
        }
    }
    LOG((LF_CORDB, LL_INFO1000, "D::GJI: for md:0x%x (%s::%s), got dmi:0x%x.\n",
         fd, fd->m_pszDebugClassName, fd->m_pszDebugMethodName,
         dmi));




    // Log stuff - fd may be null; so we don't want to AV in the log.

    LOG((LF_CORDB, LL_INFO1000, "D::GJI: for md:0x%x (%s::%s), got dmi:0x%x, dji:0x%x, latest dji:0x%x, latest fd:0x%x, prev dji:0x%x\n",
        fd, fd->m_pszDebugClassName, fd->m_pszDebugMethodName,
        dmi, dji, (dmi ? dmi->Logging_GetLatestJitInfo() : 0),
        ((dmi && dmi->Logging_GetLatestJitInfo()) ? dmi->Logging_GetLatestJitInfo()->m_fd:0),
        (dji?dji->m_prevJitInfo:0)));

    if ((dji != NULL) && (pbAddr != NULL))
    {
        dji = dji->GetJitInfoByAddress(pbAddr);

        // XXX drewb - dac doesn't support stub tracing
        // so this just results in not-impl exceptions.
#ifndef DACCESS_COMPILE
        if (dji == NULL) //may have been given address of a thunk
        {
            LOG((LF_CORDB,LL_INFO1000,"Couldn't find a DJI by address 0x%p, "
                "so it might be a stub or thunk\n", pbAddr));
            TraceDestination trace;

            g_pEEInterface->TraceStub((const BYTE *)pbAddr, &trace);

            if ((trace.GetTraceType() == TRACE_MANAGED) && (pbAddr != (const BYTE *)trace.GetAddress()))
            {
                LOG((LF_CORDB,LL_INFO1000,"Address thru thunk"
                    ": 0x%p\n", trace.GetAddress()));
                dji = GetJitInfo(fd,(const BYTE *)trace.GetAddress());
            }
#ifdef LOGGING
            else
            {
                _ASSERTE(trace.GetTraceType() != TRACE_UNJITTED_METHOD ||
                    (fd == trace.GetMethodDesc()));
                LOG((LF_CORDB,LL_INFO1000,"Address not thunked - "
                    "must be to unJITted method, or normal managed "
                    "method lacking a DJI!\n"));
            }
#endif //LOGGING
        }
#endif // #ifndef DACCESS_COMPILE
    }

    if (pMethInfo)
    {
        *pMethInfo = dmi;
    }

    // DebuggerDataLockHolder out of scope - release implied

    return dji;
}

DebuggerMethodInfo *Debugger::GetOrCreateMethodInfo(Module *pModule, mdMethodDef token)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    DebuggerMethodInfo *info = NULL;

    // In case we don't have already, take it now.
    DebuggerDataLockHolder debuggerDataLockHolder(this);

    if (m_pMethodInfos != NULL)
    {
        info = m_pMethodInfos->GetMethodInfo(pModule, token);
    }

    // dac checks ngen'ed image content first, so
    // if we didn't find information it doesn't exist.
#ifndef DACCESS_COMPILE
    if (info == NULL)
    {
        info = CreateMethodInfo(pModule, token);

        LOG((LF_CORDB, LL_INFO1000, "D::GOCMI: created DMI for mdToken:0x%x, dmi:0x%x\n",
            token, info));
    }
#endif // #ifndef DACCESS_COMPILE


    if (info == NULL)
    {
        // This should only happen in an oom scenario. It would be nice to throw here.
        STRESS_LOG2(LF_CORDB, LL_EVERYTHING, "OOM - Failed to allocate DJI (0x%p, 0x%x)\n", pModule, token);
    }

    // DebuggerDataLockHolder out of scope - release implied
    return info;
}


#ifndef DACCESS_COMPILE

/******************************************************************************
 * GetILToNativeMapping returns a map from IL offsets to native
 * offsets for this code. An array of COR_PROF_IL_TO_NATIVE_MAP
 * structs will be returned, and some of the ilOffsets in this array
 * may be the values specified in CorDebugIlToNativeMappingTypes.
 ******************************************************************************/
HRESULT Debugger::GetILToNativeMapping(MethodDesc *pMD, ULONG32 cMap,
                                       ULONG32 *pcMap, COR_DEBUG_IL_TO_NATIVE_MAP map[])
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_TRIGGERS_FROM_GETJITINFO;
    }
    CONTRACTL_END;

    // At this point, we're pulling in the debugger.
    if (!HasLazyData())
    {
        DebuggerLockHolder lockHolder(this);
        LazyInit(); // throws
    }

    // Get the JIT info by functionId.

    // This function is unsafe to use during EnC because the MethodDesc doesn't tell
    // us which version is being requested.
    // However, this function is only used by the profiler, and you can't profile with EnC,
    // which means that getting the latest jit-info is still correct.
    _ASSERTE(CORProfilerPresent());

    DebuggerJitInfo *pDJI = GetLatestJitInfoFromMethodDesc(pMD);

    if (pDJI == NULL)
        return (E_FAIL);

    // If they gave us space to copy into...
    if (map != NULL)
    {
        // Only copy as much as either they gave us or we have to copy.
        SIZE_T cpyCount = min(cMap, pDJI->GetSequenceMapCount());

        // Read the map right out of the Left Side.
        if (cpyCount > 0)
            ExportILToNativeMap(cpyCount,
                        map,
                        pDJI->GetSequenceMap(),
                        pDJI->m_sizeOfCode);
    }

    // Return the true count of entries
    if (pcMap)
    {
        *pcMap = pDJI->GetSequenceMapCount();
    }

    return (S_OK);
}






#endif // #ifndef DACCESS_COMPILE

/******************************************************************************
 *
 ******************************************************************************/
CodeRegionInfo CodeRegionInfo::GetCodeRegionInfo(DebuggerJitInfo *dji, MethodDesc *md, CORDB_ADDRESS_TYPE *addr)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (dji && dji->m_addrOfCode)
    {
        LOG((LF_CORDB, LL_EVERYTHING, "CRI::GCRI: simple case\n"));
        return dji->m_codeRegionInfo;
    }
    else
    {
        LOG((LF_CORDB, LL_EVERYTHING, "CRI::GCRI: more complex case\n"));
        CodeRegionInfo codeRegionInfo;

        // Use method desc from dji if present
        if (dji && dji->m_fd)
        {
            _ASSERTE(!md || md == dji->m_fd);
            md = dji->m_fd;
        }

        if (!addr)
        {
            _ASSERTE(md);
            addr = (CORDB_ADDRESS_TYPE *) g_pEEInterface->GetFunctionAddress(md);
        }
        else
        {
            _ASSERTE(!md ||
                     (addr == (CORDB_ADDRESS_TYPE *)g_pEEInterface->GetFunctionAddress(md)));
        }

        if (addr)
        {
            codeRegionInfo.InitializeFromStartAddress(addr);
        }

        return codeRegionInfo;
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void * Debugger::allocateArray(ULONG cBytes)
{
#ifndef DACCESS_COMPILE
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(!ThisIsHelperThread());
    }
    CONTRACTL_END;

    void *ret;

    _ASSERTE(cBytes > 0);

    // Don't user interop-safe heap b/c it's lazily initialized.
    ret = (void *) new BYTE [cBytes];
    _ASSERTE(ret != NULL); // throws on oom error

    return ret;
#else
    DacNotImpl();
    return NULL;
#endif
}

/******************************************************************************
 *
 ******************************************************************************/
void Debugger::freeArray(void *array)
{
#ifndef DACCESS_COMPILE
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(!ThisIsHelperThread());
    }
    CONTRACTL_END;

    if (array != NULL)
    {
        delete [] ((BYTE*) array);
    }

#else
    DacNotImpl();
#endif
}


#ifndef DACCESS_COMPILE
/******************************************************************************
//  Helper function for getBoundaries to get around AMD64 compiler and
// contract holders with PAL_TRY in the same function.
 ******************************************************************************/
void Debugger::getBoundariesHelper(CORINFO_METHOD_HANDLE ftnHnd,
                                   unsigned int *cILOffsets,
                                   DWORD **pILOffsets)
{
    //
    // CANNOT ADD A CONTRACT HERE.  Contract is in getBoundaries
    //

    MethodDesc *md = GetMethod(ftnHnd);

    //
    // Grab the JIT info struct for this method.  Create if needed, as this
    // may be called before JITBeginning.
    //
    DebuggerMethodInfo *dmi = NULL;
    dmi = GetOrCreateMethodInfo(md->GetModule(), md->GetMemberDef());

    if (dmi != NULL)
    {
        LOG((LF_CORDB,LL_INFO10000,"De::NGB: Got dmi 0x%x\n",dmi));

        {
            // Note: we need to make sure to enable preemptive GC here just in case we block in the symbol reader.
            bool disabled = g_pEEInterface->IsPreemptiveGCDisabled();

            if (disabled)
                g_pEEInterface->EnablePreemptiveGC();

            Module *pModule = md->GetModule();
            _ASSERTE(pModule != NULL);

            ISymUnmanagedReader *pReader = pModule->GetISymUnmanagedReader();

            // If we got a reader, use it.
            if (pReader != NULL)
            {
                // Grab the sym reader's method.
                ISymUnmanagedMethod *pISymMethod;

                HRESULT hr = pReader->GetMethod(md->GetMemberDef(),
                                                &pISymMethod);

                ULONG32 n = 0;

                if (SUCCEEDED(hr))
                {
                    // Get the count of sequence points.
                    hr = pISymMethod->GetSequencePointCount(&n);
                    _ASSERTE(SUCCEEDED(hr));


                    LOG((LF_CORDB, LL_INFO100000,
                         "D::NGB: Reader seq pt count is %d\n", n));

                    ULONG32 *p;

                    if (n > 0)
                    {
                        ULONG32 dummy;

                        p = (ULONG32*) allocateArray(sizeof(ULONG32)*n);
                        _ASSERTE(p != NULL); // throws on oom errror

                        hr = pISymMethod->GetSequencePoints(n, &dummy,
                                                            p, NULL, NULL, NULL,
                                                            NULL, NULL);
                        _ASSERTE(SUCCEEDED(hr));
                        _ASSERTE(dummy == n);

                        *pILOffsets = (DWORD*)p;

                        // Translate the IL offets based on an
                        // instrumented IL map if one exists.
                        if (dmi->HasInstrumentedILMap())
                        {
                            for (SIZE_T i = 0; i < n; i++)
                            {
                                long origOffset = *p;

                                *p = dmi->TranslateToInstIL(
                                                      origOffset,
                                                      bOriginalToInstrumented);

                                LOG((LF_CORDB, LL_INFO100000,
                                     "D::NGB: 0x%04x (Real IL:0x%x)\n",
                                     origOffset, *p));

                                p++;
                            }
                        }
#ifdef LOGGING
                        else
                        {
                            for (SIZE_T i = 0; i < n; i++)
                            {
                                LOG((LF_CORDB, LL_INFO100000,
                                     "D::NGB: 0x%04x \n", *p));
                                p++;
                            }
                        }
#endif
                    }
                    else
                        *pILOffsets = NULL;

                    pISymMethod->Release();
                }
                else
                {

                    *pILOffsets = NULL;

                    LOG((LF_CORDB, LL_INFO10000,
                         "De::NGB: failed to find method 0x%x in sym reader.\n",
                         md->GetMemberDef()));
                }

                *cILOffsets = n;
            }
            else
            {
                LOG((LF_CORDB, LL_INFO100000, "D::NGB: no reader.\n"));
            }

            // Re-disable preemptive GC if we enabled it above.
            if (disabled)
                g_pEEInterface->DisablePreemptiveGC();
        }


    }

    LOG((LF_CORDB, LL_INFO100000, "D::NGB: cILOffsets=%d\n", *cILOffsets));
    return;
}
#endif

/******************************************************************************
// Use an ISymUnmanagedReader to get method sequence points.
 ******************************************************************************/
void Debugger::getBoundaries(CORINFO_METHOD_HANDLE ftnHnd,
                             unsigned int *cILOffsets,
                             DWORD **pILOffsets,
                             ICorDebugInfo::BoundaryTypes *implicitBoundaries)
{
#ifndef DACCESS_COMPILE
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    // May be here even when a debugger is not attached.

    // @@@
    // Implements DebugInterface API

    *cILOffsets = 0;
    *pILOffsets = NULL;
    *implicitBoundaries = DEFAULT_BOUNDARIES;
    // If there has been an unrecoverable Left Side error, then we
    // just pretend that there are no boundaries.
    if (CORDBUnrecoverableError(this))
    {
        return;
    }

    MethodDesc *md = GetMethod(ftnHnd);

    // If JIT optimizations are allowed for the module this function
    // lives in, then don't grab specific boundaries from the symbol
    // store since any boundaries we give the JIT will be pretty much
    // ignored anyway.
    if (!CORDisableJITOptimizations(md->GetModule()->GetDebuggerInfoBits()))
    {
        *implicitBoundaries  = BoundaryTypes(STACK_EMPTY_BOUNDARIES |
                                         CALL_SITE_BOUNDARIES);

        return;
    }

    Module* pModule = md->GetModule();
    DWORD dwBits = pModule->GetDebuggerInfoBits();
    if ((dwBits & DACF_IGNORE_PDBS) != 0)
    {
        //
        // If told to explicitly ignore PDBs for this function, then bail now.
        //
        return;
    }

    if( !pModule->IsSymbolReadingEnabled() )
    {
        // Symbol reading is disabled for this module, so bail out early (for efficiency only)
        return;
    }

    if (pModule == SystemDomain::SystemModule())
    {
        // We don't look up PDBs for mscorlib.  This is not quite right, but avoids
        // a bootstrapping problem.  When an EXE loads, it has the option of setting
        // the COM appartment model to STA if we need to.  It is important that no
        // other Coinitialize happens before this.  Since loading the PDB reader uses
        // com we can not come first.  However managed code IS run before the COM
        // appartment model is set, and thus we have a problem since this code is
        // called for when JITTing managed code.    We avoid the problem by just
        // bailing for mscorlib.
        return;
    }

        // At this point, we're pulling in the debugger.
    if (!HasLazyData())
    {
        DebuggerLockHolder lockHolder(this);
        LazyInit(); // throws
    }

    getBoundariesHelper(ftnHnd, cILOffsets, pILOffsets);

#else
    DacNotImpl();
#endif // #ifndef DACCESS_COMPILE
}

/******************************************************************************
// void Debugger::setBoundaries(), setVars():
// Part of the ICorDebugInfo interface which is called by the JIT.
// The Jit now never sets on the debugger directly. Rather it sets on an
// IDebugInfoStore specifically for the jit, and then the debugger accesses
// the jit info from there.
//
// The jit still calls getXXX() on the debugger b/c that may control
// some details of the jit-info.
 ******************************************************************************/
void Debugger::setBoundaries(CORINFO_METHOD_HANDLE ftn, ULONG32 cMap,
                             OffsetMapping *pMap)
{
    _ASSERTE(false || !"Shouldn't be called");
}

void Debugger::setVars(CORINFO_METHOD_HANDLE ftn, ULONG32 cVars, NativeVarInfo *vars)
{
    _ASSERTE(false || !"Shouldn't be called");
}


/******************************************************************************
 *
 ******************************************************************************/
void Debugger::getVars(CORINFO_METHOD_HANDLE ftnHnd, ULONG32 *cVars, ILVarInfo **vars,
                       bool *extendOthers)
{
#ifndef DACCESS_COMPILE
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_TRIGGERS_FROM_GETJITINFO;
        PRECONDITION(!ThisIsHelperThreadWorker());
    }
    CONTRACTL_END;



    // At worst return no information
    *cVars = 0;
    *vars = NULL;

    // Just tell the JIT to extend everything.
    // Note that if optimizations are enabled, the native compilers are
    // free to ingore *extendOthers
    *extendOthers = true;

    DWORD bits = (GetMethod(ftnHnd))->GetModule()->GetDebuggerInfoBits();

    if (CORDBUnrecoverableError(this))
        goto Exit;

    if (CORDisableJITOptimizations(bits))
//    if (!CORDebuggerAllowJITOpts(bits))
    {
        //

        BOOL fVarArg = false;
        GetArgCount(GetMethod(ftnHnd), &fVarArg);

        if (fVarArg)
        {
            // It is, so we need to tell the JIT to give us the
            // varags handle.
            ILVarInfo *p = new ILVarInfo[1];
            _ASSERTE(p != NULL); // throws on oom error

            COR_ILMETHOD_DECODER header(g_pEEInterface->MethodDescGetILHeader(GetMethod(ftnHnd)));
            unsigned int ilCodeSize = header.GetCodeSize();

            p->startOffset = 0;
            p->endOffset = ilCodeSize;
            p->varNumber = (DWORD) ICorDebugInfo::VARARGS_HND_ILNUM;

            *cVars = 1;
            *vars = p;
        }
    }

    LOG((LF_CORDB, LL_INFO100000, "D::gV: cVars=%d, extendOthers=%d\n",
         *cVars, *extendOthers));

Exit:
    ;
#else
    DacNotImpl();
#endif // #ifndef DACCESS_COMPILE
}


#ifndef DACCESS_COMPILE

// We want to keep the 'worst' HRESULT - if one has failed (..._E_...) & the
// other hasn't, take the failing one.  If they've both/neither failed, then
// it doesn't matter which we take.
// Note that this macro favors retaining the first argument
#define WORST_HR(hr1,hr2) (FAILED(hr1)?hr1:hr2)
/******************************************************************************
 *
 ******************************************************************************/
HRESULT Debugger::SetIP( bool fCanSetIPOnly, Thread *thread,Module *module,
                         mdMethodDef mdMeth, DebuggerJitInfo* dji,
                         SIZE_T offsetILTo, BOOL fIsIL, void *firstExceptionHandler)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(thread));
        PRECONDITION(CheckPointer(module));
        PRECONDITION(mdMeth != mdMethodDefNil);
    }
    CONTRACTL_END;

#ifdef _DEBUG
    static ConfigDWORD breakOnSetIP;
    if (breakOnSetIP.val(L"DbgBreakOnSetIP", 0)) _ASSERTE(!"DbgBreakOnSetIP");
#endif

    HRESULT hr = S_OK;
    HRESULT hrAdvise = S_OK;

    DWORD offsetILFrom;
    CorDebugMappingResult map;
    DWORD whichIgnore;

    ControllerStackInfo csi;

    BOOL exact;
    SIZE_T offsetNatTo;

    BYTE    *pbDest = NULL;
    BYTE    *pbBase = NULL;
    CONTEXT *pCtx   = NULL;
    DWORD    dwSize = 0;
    SIZE_T  *rgVal1 = NULL;
    SIZE_T  *rgVal2 = NULL;
    BYTE **pVCs   = NULL;

    LOG((LF_CORDB, LL_INFO1000, "D::SIP: In SetIP ==> fCanSetIPOnly:0x%x <==!\n", fCanSetIPOnly));

    pCtx = GetManagedStoppedCtx(thread);

    // If we can't get a context, then we can't possibly be a in a good place
    // to do a setip.
    if (pCtx == NULL)
    {
        return CORDBG_S_BAD_START_SEQUENCE_POINT;
    }

    // Implicit Caveat: We need to be the active frame.
    // We can safely take a stack trace because the thread is synchronized.
    StackTraceTicket ticket(thread);
    csi.GetStackInfo(ticket, thread, LEAF_MOST_FRAME, NULL);

    ULONG offsetNatFrom = csi.m_activeFrame.relOffset;

    _ASSERTE(dji != NULL);

    // On WIN64 platforms, it's important to use the total size of the
    // parent method and the funclets below (i.e. m_sizeOfCode).  Don't use
    // the size of the individual funclets or the parent method.
    pbBase = (BYTE*)CORDB_ADDRESS_TO_PTR(dji->m_addrOfCode);
    dwSize = (DWORD)dji->m_sizeOfCode;


#if defined(_X86_) || defined(_WIN64)
    // Create our structure for analyzing this.
    EHRangeTree* pEHRT = new (nothrow) EHRangeTree(csi.m_activeFrame.pIJM,
                                                   csi.m_activeFrame.MethodToken,
                                                   dwSize);

    // To maintain the current semantics, we will check the following right before SetIPFromSrcToDst() is called
    // (instead of checking them now):
    // 1) pEHRT == NULL
    // 2) FAILED(pEHRT->m_hrInit)
#endif // _X86_ || _WIN64


    {
        LOG((LF_CORDB, LL_INFO1000, "D::SIP:Got version info fine\n"));

        // Caveat: we need to start from a sequence point
        offsetILFrom = dji->MapNativeOffsetToIL(offsetNatFrom,
                                                &map, &whichIgnore);
        if ( !(map & MAPPING_EXACT) )
        {
            LOG((LF_CORDB, LL_INFO1000, "D::SIP:Starting native offset is bad!\n"));
            hrAdvise = WORST_HR(hrAdvise, CORDBG_S_BAD_START_SEQUENCE_POINT);
        }
        else
        {   // exact IL mapping

            if (!(dji->GetSrcTypeFromILOffset(offsetILFrom) & ICorDebugInfo::STACK_EMPTY))
            {
                LOG((LF_CORDB, LL_INFO1000, "D::SIP:Starting offset isn't stack empty!\n"));
                hrAdvise = WORST_HR(hrAdvise, CORDBG_S_BAD_START_SEQUENCE_POINT);
            }
        }

        // Caveat: we need to go to a sequence point
        if (fIsIL )
        {
            DebuggerJitInfo::ILToNativeOffsetIterator it;
            dji->InitILToNativeOffsetIterator(it, offsetILTo);
            offsetNatTo = it.CurrentAssertOnlyOne(&exact);

            if (!exact)
            {
                LOG((LF_CORDB, LL_INFO1000, "D::SIP:Dest (via IL offset) is bad!\n"));
                hrAdvise = WORST_HR(hrAdvise, CORDBG_S_BAD_END_SEQUENCE_POINT);
            }
        }
        else
        {
            offsetNatTo = offsetILTo;
            LOG((LF_CORDB, LL_INFO1000, "D::SIP:Dest of 0x%p (via native "
                "offset) is fine!\n", offsetNatTo));
        }

        CorDebugMappingResult mapping;
        DWORD which;
        offsetILTo = dji->MapNativeOffsetToIL(offsetNatTo, &mapping, &which);

        // We only want to perhaps return CORDBG_S_BAD_END_SEQUENCE_POINT if
        // we're not already returning CORDBG_S_BAD_START_SEQUENCE_POINT.
        if (hr != CORDBG_S_BAD_START_SEQUENCE_POINT)
        {
            if ( !(mapping & MAPPING_EXACT) )
            {
                LOG((LF_CORDB, LL_INFO1000, "D::SIP:Ending native offset is bad!\n"));
                hrAdvise = WORST_HR(hrAdvise, CORDBG_S_BAD_END_SEQUENCE_POINT);
            }
            else
            {
                // <NOTE WIN64>
                // All duplicate sequence points (ones with the same IL offset) should have the same SourceTypes.
                // </NOTE WIN64>
                if (!(dji->GetSrcTypeFromILOffset(offsetILTo) & ICorDebugInfo::STACK_EMPTY))
                {
                    LOG((LF_CORDB, LL_INFO1000, "D::SIP:Ending offset isn't a sequence"
                                                " point, or not stack empty!\n"));
                    hrAdvise = WORST_HR(hrAdvise, CORDBG_S_BAD_END_SEQUENCE_POINT);
                }
            }
        }

        // Caveat: can't setip if there's no code
        if (dji->m_codePitched)
        {
            LOG((LF_CORDB, LL_INFO1000, "D::SIP:Code has been pitched!\n"));
            hrAdvise = WORST_HR(hrAdvise, CORDBG_E_CODE_NOT_AVAILABLE);
            if (fCanSetIPOnly)
                goto LExit;
        }

        // Once we finally have a native offset, it had better be in range.
        if (offsetNatTo >= dwSize)
        {
            LOG((LF_CORDB, LL_INFO1000, "D::SIP:Code out of range! offsetNatTo = 0x%x, dwSize=0x%x\n", offsetNatTo, dwSize));
            hrAdvise = E_INVALIDARG;
            goto LExit;
        }

        pbDest = CodeRegionInfo::GetCodeRegionInfo(dji).OffsetToAddress(offsetNatTo);
        LOG((LF_CORDB, LL_INFO1000, "D::SIP:Dest is 0x%p\n", pbDest));

        // Don't allow SetIP if the source or target is cold (SetIPFromSrcToDst does not
        // correctly handle this case).
        if (!CodeRegionInfo::GetCodeRegionInfo(dji).IsOffsetHot(offsetNatTo) ||
            !CodeRegionInfo::GetCodeRegionInfo(dji).IsOffsetHot(offsetNatFrom))
        {
            hrAdvise = WORST_HR(hrAdvise, CORDBG_E_SET_IP_IMPOSSIBLE);
            goto LExit;
        }
    }

    if (!fCanSetIPOnly)
    {
        hr = ShuffleVariablesGet(dji,
                                 offsetNatFrom,
                                 pCtx,
                                 &rgVal1,
                                 &rgVal2,
                                 &pVCs);
        LOG((LF_CORDB|LF_ENC,
             LL_INFO10000,
             "D::SIP: rgVal1 0x%X, rgVal2 0x%X\n",
             rgVal1,
             rgVal2));

        if (FAILED(hr))
        {
            // This will only fail fatally, so exit.
            hrAdvise = WORST_HR(hrAdvise, hr);
            goto LExit;
        }
    }

#if defined(_X86_) || defined(_WIN64)
    if (pEHRT == NULL)
    {
        hr = E_OUTOFMEMORY;
    }
    else if (FAILED(pEHRT->m_hrInit))
    {
        hr = pEHRT->m_hrInit;
    }
    else
    {
        //
        // This is a known, ok, violation.  END_EXCEPTION_GLUE has a call to GetThrowable in it, but
        // we will never hit it because we are passing in NULL below.  This is to satisfy the static
        // contract analyzer.
        //
        CONTRACT_VIOLATION(GCViolation);

        BEGIN_EXCEPTION_GLUE(NULL, NULL)
        {
        hr =g_pEEInterface->SetIPFromSrcToDst(thread,
                                              csi.m_activeFrame.pIJM,
                                              csi.m_activeFrame.MethodToken,
                                              pbBase,
                                              offsetNatFrom,
                                              offsetNatTo,
                                              fCanSetIPOnly,
                                              &(csi.m_activeFrame.registers),
                                              pCtx,
                                              dwSize,
                                              firstExceptionHandler,
                                              (void *)dji,
                                              pEHRT);
        }
        END_EXCEPTION_GLUE;

    }

#else  // !_X86_ && !_WIN64
    hr = E_FAIL;

#endif // !_X86_ && !_WIN64

    // Get the return code, if any
    if (hr != S_OK)
    {
        hrAdvise = WORST_HR(hrAdvise, hr);
        goto LExit;
    }

    // If we really want to do this, we'll have to put the
    // variables into their new locations.
    if (!fCanSetIPOnly && !FAILED(hrAdvise))
    {
        // TODO: We should zero out any registers which have now become live GC roots,
        // but which aren't tracked variables (i.e. they are JIT temporaries).  Such registers may
        // have garbage left over in them, and we don't want the GC to try and dereference them
        // as object references.  However, we can't easily tell here which of the callee-saved regs
        // are used in this method and therefore safe to clear.

        hr = ShuffleVariablesSet(dji,
                            offsetNatTo,
                            pCtx,
                            &rgVal1,
                            &rgVal2,
                            pVCs);


        if (hr != S_OK)
        {
            hrAdvise = WORST_HR(hrAdvise, hr);
            goto LExit;
        }

        _ASSERTE(pbDest != NULL);

        ::SetIP(pCtx, pbDest);

        LOG((LF_CORDB, LL_INFO1000, "D::SIP:Set IP to be 0x%p\n", GetIP(pCtx)));
    }


LExit:
    if (rgVal1 != NULL)
    {
        DeleteInteropSafe(rgVal1);
    }

    if (rgVal2 != NULL)
    {
        DeleteInteropSafe(rgVal2);
    }

#if defined(_X86_) || defined(_WIN64)
    if (pEHRT != NULL)
    {
        delete pEHRT;
    }
#endif // _X86_ || _WIN64

    LOG((LF_CORDB, LL_INFO1000, "D::SIP:Returning 0x%x\n", hr));
    return hrAdvise;
}

#include "nativevaraccessors.h"

/******************************************************************************
 *
 ******************************************************************************/

HRESULT Debugger::ShuffleVariablesGet(DebuggerJitInfo  *dji,
                                      SIZE_T            offsetFrom,
                                      CONTEXT          *pCtx,
                                      SIZE_T          **prgVal1,
                                      SIZE_T          **prgVal2,
                                      BYTE           ***prgpVCs)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(dji));
        PRECONDITION(CheckPointer(pCtx));
        PRECONDITION(CheckPointer(prgVal1));
        PRECONDITION(CheckPointer(prgVal2));
        PRECONDITION(dji->m_sizeOfCode >= offsetFrom);
    }
    CONTRACTL_END;

    LONG cVariables = 0;
    DWORD i;

    //
    // Find the largest variable number
    //
    for (i = 0; i < dji->GetVarNativeInfoCount(); i++)
    {
        if ((LONG)(dji->GetVarNativeInfo()[i].varNumber) > cVariables)
        {
            cVariables = (LONG)(dji->GetVarNativeInfo()[i].varNumber);
        }
    }

    HRESULT hr = S_OK;

    //
    // cVariables is a zero-based count of the number of variables.  Increment it.
    //
    cVariables++;

    SIZE_T *rgVal1 = new (interopsafe, nothrow) SIZE_T[cVariables + unsigned(-ICorDebugInfo::UNKNOWN_ILNUM)];

    SIZE_T *rgVal2 = NULL;

    if (rgVal1 == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto LExit;
    }

    rgVal2 = new (interopsafe, nothrow) SIZE_T[cVariables + unsigned(-ICorDebugInfo::UNKNOWN_ILNUM)];

    if (rgVal2 == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto LExit;
    }

    memset(rgVal1, 0, sizeof(SIZE_T) * (cVariables + unsigned(-ICorDebugInfo::UNKNOWN_ILNUM)));
    memset(rgVal2, 0, sizeof(SIZE_T) * (cVariables + unsigned(-ICorDebugInfo::UNKNOWN_ILNUM)));

    LOG((LF_CORDB|LF_ENC,
         LL_INFO10000,
         "D::SVG cVariables %d, hiddens %d, rgVal1 0x%X, rgVal2 0x%X\n",
         cVariables,
         unsigned(-ICorDebugInfo::UNKNOWN_ILNUM),
         rgVal1,
         rgVal2));

    GetVariablesFromOffset(dji->m_fd,
                           dji->GetVarNativeInfoCount(),
                           dji->GetVarNativeInfo(),
                           offsetFrom,
                           pCtx,
                           rgVal1,
                           rgVal2,
                           cVariables + unsigned(-ICorDebugInfo::UNKNOWN_ILNUM),
                           prgpVCs);


LExit:
    if (!FAILED(hr))
    {
        (*prgVal1) = rgVal1;
        (*prgVal2) = rgVal2;
    }
    else
    {
        LOG((LF_CORDB, LL_INFO100, "D::SVG: something went wrong hr=0x%x!", hr));

        (*prgVal1) = NULL;
        (*prgVal2) = NULL;

        if (rgVal1 != NULL)
            delete[] rgVal1;

        if (rgVal2 != NULL)
            delete[] rgVal2;
    }

    return hr;
}

/******************************************************************************
 *
 ******************************************************************************/
HRESULT Debugger::ShuffleVariablesSet(DebuggerJitInfo  *dji,
                                   SIZE_T            offsetTo,
                                   CONTEXT          *pCtx,
                                   SIZE_T          **prgVal1,
                                   SIZE_T          **prgVal2,
                                   BYTE            **rgpVCs)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(dji));
        PRECONDITION(CheckPointer(pCtx));
        PRECONDITION(CheckPointer(prgVal1));
        PRECONDITION(CheckPointer(prgVal2));
        PRECONDITION(dji->m_sizeOfCode >= offsetTo);
    }
    CONTRACTL_END;

    LOG((LF_CORDB|LF_ENC,
         LL_INFO10000,
         "D::SVS: rgVal1 0x%X, rgVal2 0x%X\n",
         (*prgVal1),
         (*prgVal2)));

    HRESULT hr = SetVariablesAtOffset(dji->m_fd,
                                      dji->GetVarNativeInfoCount(),
                                      dji->GetVarNativeInfo(),
                                      offsetTo,
                                      pCtx,
                                      *prgVal1,
                                      *prgVal2,
                                      rgpVCs);

    LOG((LF_CORDB|LF_ENC,
         LL_INFO100000,
         "D::SVS deleting rgVal1 0x%X, rgVal2 0x%X\n",
         (*prgVal1),
         (*prgVal2)));

    DeleteInteropSafe(*prgVal1);
    (*prgVal1) = NULL;
    DeleteInteropSafe(*prgVal2);
    (*prgVal2) = NULL;
    return hr;
}

//
// This class is used by Get and SetVariablesFromOffsets to manage a frameHelper
// list for the arguments and locals corresponding to each varNativeInfo. The first
// four are hidden args, but the remainder will all have a corresponding entry
// in the argument or local signature list.
//
// The structure of the array varNativeInfo contains home information for each variable
// at various points in the function.  Thus, you have to search for the proper native offset
// (IP) in the varNativeInfo, and then find the correct varNumber in that native offset to
// find the correct home information.
//
// Important to note is that the JIT has hidden args that have varNumbers that are negative.
// Thus we cannot use varNumber as a strict index into our holder arrays, and instead shift
// indexes before indexing into our holder arrays.
//
// The hidden args are a fixed-sized array given by the value of 0-UNKNOWN_ILNUM. These are used
// to pass cookies about the arguments (var args, generics, retarg buffer etc.) to the function.
// The real arguments and locals are as one would expect.
//

class GetSetFrameHelper
{
public:
    GetSetFrameHelper();
    ~GetSetFrameHelper();

    HRESULT Init(MethodDesc* pMD);

    bool GetValueClassSizeOfVar(int varNum, ICorDebugInfo::VarLocType varType, SIZE_T* pSize);
    int ShiftIndexForHiddens(int varNum);

private:
    MethodDesc*     m_pMD;
    SIZE_T*         m_rgSize;
    CorElementType* m_rgElemType;
    ULONG           m_numArgs;
    ULONG           m_numTotalVars;

    SIZE_T  GetValueClassSize(MetaSig* pSig);

    static SIZE_T  GetSizeOfElement(CorElementType cet);
};

//
// GetSetFrameHelper::GetSetFrameHelper()
//
// This is the constructor.  It just initailizes all member variables.
//
// parameters: none
//
// return value: none
//
GetSetFrameHelper::GetSetFrameHelper() : m_pMD(NULL), m_rgSize(NULL), m_rgElemType(NULL),
                                         m_numArgs(0), m_numTotalVars(0)
{
    LEAF_CONTRACT;
}

//
// GetSetFrameHelper::Init()
//
// This method extracts the element type and the size of the arguments and locals of the method we are doing
// the SetIP on and stores this information in instance variables.
//
// parameters:   pMD - MethodDesc of the method we are doing the SetIP on
//
// return value: S_OK or E_OUTOFMEMORY
//
HRESULT GetSetFrameHelper::Init(MethodDesc *pMD)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMD));
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    m_pMD = pMD;

    // Initialize decoderOldIL before checking the method argument signature.
    COR_ILMETHOD_DECODER decoderOldIL(pMD->GetILHeader());
    mdSignature mdLocalSig = (decoderOldIL.GetLocalVarSigTok()) ? (decoderOldIL.GetLocalVarSigTok()):
                                                                  (mdSignatureNil);

    PCCOR_SIGNATURE pCallSig;
    DWORD cbCallSigSize;

    pMD->GetSig(&pCallSig, &cbCallSigSize);

    MetaSig*        pArgSig  = NULL;

    if (pCallSig)
    {
        // Yes, we do need to pass in the text because this might be generic function!
        SigTypeContext tmpContext(pMD);

        pArgSig = new (interopsafe, nothrow) MetaSig(pCallSig,
                                                     cbCallSigSize,
                                                     pMD->GetModule(),
                                                     &tmpContext,
                                                     FALSE,
                                                     MetaSig::sigMember);

        if (pArgSig == NULL)
        {
            hr = E_OUTOFMEMORY;
        }
        else
        {
            m_numArgs = pArgSig->NumFixedArgs();

            if (pArgSig->HasThis())
            {
                m_numArgs++;
            }

            /*
            if (argSig.IsVarArg())
                m_numArgs++;
            */
        }
    }

    MetaSig* pLocSig = NULL;
    // allocation of pArgSig succeeded
    if (SUCCEEDED(hr))
    {
        ULONG cbSig;
        PCCOR_SIGNATURE pLocalSig = mdLocalSig == mdSignatureNil ?
                                        NULL :
                                        pMD->GetModule()->GetMDImport()->GetSigFromToken(mdLocalSig, &cbSig);
        if (pLocalSig)
        {
            SigTypeContext tmpContext(pMD);
            pLocSig = new (interopsafe, nothrow) MetaSig(pLocalSig,
                                                         cbSig,
                                                         pMD->GetModule(),
                                                         &tmpContext,
                                                         FALSE,
                                                         MetaSig::sigLocalVars);

            if (pLocSig == NULL)
            {
                hr = E_OUTOFMEMORY;
            }
        }

        // allocation of pLocalSig succeeded
        if (SUCCEEDED(hr))
        {
            m_numTotalVars = m_numArgs + (pLocSig != NULL ? pLocSig->NumFixedArgs() : 0);

            if (m_numTotalVars > 0)
            {
                m_rgSize     = new (interopsafe, nothrow) SIZE_T[m_numTotalVars];
                m_rgElemType = new (interopsafe, nothrow) CorElementType[m_numTotalVars];

                if (m_rgSize == NULL || m_rgElemType == NULL)
                {
                    hr = E_OUTOFMEMORY;
                }
                else
                {
                    // allocation of m_rgSize and m_rgElemType succeeded
                    for (ULONG i = 0; i < m_numTotalVars; i++)
                    {
                        // Choose the correct signature to walk.
                        MetaSig* pCur = NULL;
                        if (i < m_numArgs)
                        {
                            pCur = pArgSig;
                        }
                        else
                        {
                            pCur = pLocSig;
                        }

                        // The "this" argument isn't stored in the signature, so we have to
                        // check for it manually.
                        if (i == 0 && pCur->HasThis())
                        {
                            _ASSERTE(pCur == pArgSig);

                            m_rgElemType[i] = ELEMENT_TYPE_CLASS;
                            m_rgSize[i]     = sizeof(SIZE_T);
                        }
                        else
                        {
                            m_rgElemType[i] = pCur->NextArg();

                            if (m_rgElemType[i] == ELEMENT_TYPE_VALUETYPE)
                            {
                                m_rgSize[i] = GetValueClassSize(pCur);
                            }
                            else
                            {
                                m_rgSize[i] = GetSetFrameHelper::GetSizeOfElement(m_rgElemType[i]);
                            }

                            LOG((LF_CORDB, LL_INFO10000, "GSFH::I: var 0x%x is of type %x, size:0x%x\n",
                                 i, m_rgElemType[i], m_rgSize[i]));
                        }
                    }
                } // allocation of m_rgSize and m_rgElemType succeeded
            }   // if there are variables to take care of
        }   // allocation of pLocSig succeeded
    }

    // clean up
    if (pArgSig)
    {
        DeleteInteropSafe(pArgSig);
    }

    if (pLocSig)
    {
        DeleteInteropSafe(pLocSig);
    }

    if (FAILED(hr))
    {
        if (m_rgSize)
        {
            DeleteInteropSafe(m_rgSize);
        }

        if (m_rgElemType)
        {
            DeleteInteropSafe((int*)m_rgElemType);
        }
    }

    return hr;
}

//
// GetSetFrameHelper::~GetSetFrameHelper()
//
// This is the destructor.  It checks the two arrays we have allocated and frees the memory accordingly.
//
// parameters:   none
//
// return value: none
//
GetSetFrameHelper::~GetSetFrameHelper()
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (m_rgSize)
    {
        DeleteInteropSafe(m_rgSize);
    }

    if (m_rgElemType)
    {
        DeleteInteropSafe((int*)m_rgElemType);
    }
}

//
// GetSetFrameHelper::GetSizeOfElement()
//
// Given a CorElementType, this function returns the size of this type.
// Note that this function doesn't handle ELEMENT_TYPE_VALUETYPE.  Use GetValueClassSize() instead.
//
// parameters:   cet - the CorElementType of the argument/local we are dealing with
//
// return value: the size of the argument/local
//
// static
SIZE_T GetSetFrameHelper::GetSizeOfElement(CorElementType cet)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(cet != ELEMENT_TYPE_VALUETYPE);
    }
    CONTRACTL_END;

    if (!CorIsPrimitiveType(cet))
    {
        return sizeof(SIZE_T);
    }
    else
    {
        switch (cet)
        {
        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
        case ELEMENT_TYPE_R8:
               return 8;

        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
        case ELEMENT_TYPE_I:
        case ELEMENT_TYPE_U:
        case ELEMENT_TYPE_R4:
            return 4;

        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_CHAR:
            return 2;

        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
        case ELEMENT_TYPE_BOOLEAN:
            return 1;

        case ELEMENT_TYPE_VOID:
        case ELEMENT_TYPE_END:
            _ASSERTE(!"debugger.cpp - Check this code path\n");
            return 0;

        case ELEMENT_TYPE_STRING:
            return sizeof(SIZE_T);

        default:
            _ASSERTE(!"debugger.cpp - Check this code path\n");
            return sizeof(SIZE_T);
        }
    }
}

//
// GetSetFrameHelper::GetValueClassSize()
//
// Given a MetaSig pointer to the signature of a value type, this function returns its size.
//
// parameters:   pSig - MetaSig pointer to the signature of a value type
//
// return value: the size of this value type
//
SIZE_T GetSetFrameHelper::GetValueClassSize(MetaSig* pSig)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pSig));
    }
    CONTRACTL_END;

    // We need to determine the number of bytes for this value-type.
    SigPointer sp = pSig->GetArgProps();

    TypeHandle vcType = TypeHandle();
    {
        // Lookup operations run the class loader in non-load mode.
        ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

        SigTypeContext typeContext(m_pMD);
        vcType = sp.GetTypeHandle(m_pMD->GetModule(),
                                  &typeContext,
                                  NULL,
                                  // == FailIfNotLoaded
                                  ClassLoader::DontLoadTypes);
    }
    // We need to know the size of the class in bytes. This means:
    // - we need a specific instantiation (since that affects size)
    // - but we don't care if it's shared (since it will be the same size either way)
    _ASSERTE(!vcType.IsNull() && vcType.IsValueType());

    return (vcType.GetMethodTable()->GetAlignedNumInstanceFieldBytes());
}

//
// GetSetFrameHelper::GetValueClassSizeOfVar()
//
// This method retrieves the size of the variable saved in the array m_rgSize.  Also, it returns true
// if the variable is a value type.
//
// parameters:   varNum  - the variable number (arguments come before locals)
//               varType - the type of variable home
//               pSize   - [out] the size
//
// return value: whether this variable is a value type
//
bool GetSetFrameHelper::GetValueClassSizeOfVar(int varNum, ICorDebugInfo::VarLocType varType, SIZE_T* pSize)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(varType != ICorDebugInfo::VLT_FIXED_VA);
        PRECONDITION(pSize != NULL);
    }
    CONTRACTL_END;

    // preliminary checking
    if (varNum < 0)
    {
        // Make sure this is one of the secret parameters (e.g. VASigCookie, generics context, etc.).
        _ASSERTE(varNum > (int)ICorDebugInfo::MAX_ILNUM);

        *pSize = sizeof(LPVOID);
        return false;
    } 

    // This check is only safe after we make sure that varNum is not negative.
    if ((UINT)varNum >= m_numTotalVars)
    {
        _ASSERTE(!"invalid variable index encountered during setip");
        *pSize = 0;
        return false;
    }

    CorElementType cet = m_rgElemType[varNum];
    *pSize = m_rgSize[varNum];

    if ((cet != ELEMENT_TYPE_VALUETYPE) ||
        (varType == ICorDebugInfo::VLT_REG) ||
        (varType == ICorDebugInfo::VLT_REG_REG) ||
        (varType == ICorDebugInfo::VLT_REG_STK) ||
        (varType == ICorDebugInfo::VLT_STK_REG))
    {
        return false;
    }
    else
    {
        return true;
    }
}

int GetSetFrameHelper::ShiftIndexForHiddens(int varNum)
{
    LEAF_CONTRACT;

    //
    // Need to shift them up so are appropriate index for rgVal arrays
    //
    return varNum - ICorDebugInfo::UNKNOWN_ILNUM;
}

// Helper method pair to grab all, then set all, variables at a given
// point in a routine.
// NOTE: GetVariablesFromOffset and SetVariablesAtOffset are
// very similar - modifying one will probably need to be reflected in the other...
// rgVal1 and rgVal2 are preallocated by callers with estimated size.
// We pass in the size of the allocation in rRgValeSize. The safe index will be rgVal1[0..uRgValSize - 1]
//
HRESULT Debugger::GetVariablesFromOffset(MethodDesc  *pMD,
                                         UINT varNativeInfoCount,
                                         ICorJitInfo::NativeVarInfo *varNativeInfo,
                                         SIZE_T offsetFrom,
                                         CONTEXT *pCtx,
                                         SIZE_T  *rgVal1,
                                         SIZE_T  *rgVal2,
                                         UINT    uRgValSize, // number of elements of the preallocated rgVal1 and rgVal2
                                         BYTE ***rgpVCs)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(rgpVCs));
        PRECONDITION(CheckPointer(pCtx));
        PRECONDITION(varNativeInfoCount == 0 || CheckPointer(varNativeInfo));
        PRECONDITION(varNativeInfoCount == 0 || CheckPointer(rgVal1));
        PRECONDITION(varNativeInfoCount == 0 || CheckPointer(rgVal2));
        // This may or may not be called on the helper thread.
    }
    CONTRACTL_END;

    *rgpVCs = NULL;
    // if there are no locals, well, we are done!

    if (varNativeInfoCount == 0)
    {
        return S_OK;
    }

    memset( rgVal1, 0, sizeof(SIZE_T)*uRgValSize);
    memset( rgVal2, 0, sizeof(SIZE_T)*uRgValSize);

    LOG((LF_CORDB|LF_ENC, LL_INFO10000, "D::GVFO: %s::%s, infoCount:0x%x, from:0x%p\n",
         pMD->m_pszDebugClassName,
         pMD->m_pszDebugMethodName,
         varNativeInfoCount,
         offsetFrom));

    GetSetFrameHelper frameHelper;
    HRESULT hr = frameHelper.Init(pMD);
    if (FAILED(hr))
    {
        return hr;
    }
    // preallocate enough to hold all possible valueclass args & locals
    // sure this is more than we need, but not a big deal and better
    // than having to crawl through the frameHelper and count
    ULONG cValueClasses = 0;
    BYTE **rgpValueClasses = new (interopsafe, nothrow)  BYTE *[varNativeInfoCount];
    if (rgpValueClasses == NULL)
    {
        return E_OUTOFMEMORY;
    }
    memset(rgpValueClasses, 0, sizeof(BYTE *)*varNativeInfoCount);

    hr = S_OK;

    LOG((LF_CORDB|LF_ENC,
         LL_INFO10000,
         "D::GVFO rgVal1 0x%X, rgVal2 0x%X\n",
         rgVal1,
         rgVal2));

    // Now go through the full array and save off each arg and local
    for (UINT i = 0; i< varNativeInfoCount;i++)
    {
        if ((varNativeInfo[i].startOffset > offsetFrom) ||
            (varNativeInfo[i].endOffset < offsetFrom) ||
            (varNativeInfo[i].loc.vlType == ICorDebugInfo::VLT_INVALID))
        {
            LOG((LF_CORDB|LF_ENC,LL_INFO10000, "D::GVFO [%2d] invalid\n", i));
            continue;
        }

        SIZE_T cbClass;
        bool isVC = frameHelper.GetValueClassSizeOfVar(varNativeInfo[i].varNumber,
                                                       varNativeInfo[i].loc.vlType,
                                                       &cbClass);

        if (!isVC)
        {
            int rgValIndex = frameHelper.ShiftIndexForHiddens(varNativeInfo[i].varNumber);

            _ASSERTE(rgValIndex >= 0 && rgValIndex < (int)uRgValSize);

            BOOL res = GetNativeVarVal(varNativeInfo[i].loc,
                                       pCtx,
                                       rgVal1 + rgValIndex,
                                       rgVal2 + rgValIndex
                                       WIN64_ARG(cbClass));

            LOG((LF_CORDB|LF_ENC,LL_INFO10000,
                 "D::GVFO [%2d] varnum %d, nonVC type %x, addr %8.8x: %8.8x;%8.8x\n",
                 i,
                 varNativeInfo[i].varNumber,
                 varNativeInfo[i].loc.vlType,
                 NativeVarStackAddr(varNativeInfo[i].loc, pCtx),
                 rgVal1[rgValIndex],
                 rgVal2[rgValIndex]));

            if (res == TRUE)
            {
                continue;
            }

            _ASSERTE(res == TRUE);
            hr = E_FAIL;
            break;
        }

        // it's definately a value class
        // Make space for it - note that it uses the VC index, NOT the variable index
        _ASSERTE(cbClass != 0);
        rgpValueClasses[cValueClasses] = new (interopsafe, nothrow) BYTE[cbClass];
        if (rgpValueClasses[cValueClasses] == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }
        memcpy(rgpValueClasses[cValueClasses],
               NativeVarStackAddr(varNativeInfo[i].loc, pCtx),
               cbClass);

        // Move index up.
        cValueClasses++;
#ifdef _DEBUG
        LOG((LF_CORDB|LF_ENC,LL_INFO10000,
             "D::GVFO [%2d] varnum %d, VC len %d, addr %8.8x, sample: %8.8x%8.8x\n",
             i,
             varNativeInfo[i].varNumber,
             cbClass,
             NativeVarStackAddr(varNativeInfo[i].loc, pCtx),
             (rgpValueClasses[cValueClasses-1])[0], (rgpValueClasses[cValueClasses-1])[1]));
#endif
    }

    LOG((LF_CORDB|LF_ENC, LL_INFO10000, "D::GVFO: returning %8.8x\n", hr));
    if (SUCCEEDED(hr))
    {
        (*rgpVCs) = rgpValueClasses;
        return hr;
    }

    // We failed for some reason
    if (rgpValueClasses != NULL)
    {   // free any memory we allocated for VCs here
        while(cValueClasses > 0)
        {
            --cValueClasses;
            DeleteInteropSafe(rgpValueClasses[cValueClasses]);  // OK to delete NULL
        }
        DeleteInteropSafe(rgpValueClasses);
        rgpValueClasses = NULL;
    }
    return hr;
}

// NOTE: GetVariablesFromOffset and SetVariablesAtOffset are
// very similar - modifying one will probably need to be reflected in the other...
HRESULT Debugger::SetVariablesAtOffset(MethodDesc  *pMD,
                                       UINT varNativeInfoCount,
                                       ICorJitInfo::NativeVarInfo *varNativeInfo,
                                       SIZE_T offsetTo,
                                       CONTEXT *pCtx,
                                       SIZE_T  *rgVal1,
                                       SIZE_T  *rgVal2,
                                       BYTE **rgpVCs)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pCtx));
        PRECONDITION(varNativeInfoCount == 0 || CheckPointer(rgpVCs));
        PRECONDITION(varNativeInfoCount == 0 || CheckPointer(varNativeInfo));
        PRECONDITION(varNativeInfoCount == 0 || CheckPointer(rgVal1));
        PRECONDITION(varNativeInfoCount == 0 || CheckPointer(rgVal2));
        // This may or may not be called on the helper thread.
    }
    CONTRACTL_END;

    LOG((LF_CORDB|LF_ENC, LL_INFO10000, "D::SVAO: %s::%s, infoCount:0x%x, to:0x%p\n",
         pMD->m_pszDebugClassName,
         pMD->m_pszDebugMethodName,
         varNativeInfoCount,
         offsetTo));

    if (varNativeInfoCount == 0)
    {
        return S_OK;
    }

    GetSetFrameHelper frameHelper;
    HRESULT hr = frameHelper.Init(pMD);
    if (FAILED(hr))
    {
        return hr;
    }

    ULONG iVC = 0;
    hr = S_OK;

    // Note that since we obtain all the variables in the first loop, we
    // can now splatter those variables into their new locations
    // willy-nilly, without the fear that variable locations that have
    // been swapped might accidentally overwrite a variable value.
    for (UINT i = 0;i< varNativeInfoCount;i++)
    {
        if ((varNativeInfo[i].startOffset > offsetTo) ||
            (varNativeInfo[i].endOffset < offsetTo) ||
            (varNativeInfo[i].loc.vlType == ICorDebugInfo::VLT_INVALID))
        {
            LOG((LF_CORDB|LF_ENC,LL_INFO10000, "D::SVAO [%2d] invalid\n", i));
            continue;
        }

        SIZE_T cbClass;
        bool isVC = frameHelper.GetValueClassSizeOfVar(varNativeInfo[i].varNumber,
                                                       varNativeInfo[i].loc.vlType,
                                                       &cbClass);

        if (!isVC)
        {
            int rgValIndex = frameHelper.ShiftIndexForHiddens(varNativeInfo[i].varNumber);

            _ASSERTE(rgValIndex >= 0);

            BOOL res = SetNativeVarVal(varNativeInfo[i].loc,
                                       pCtx,
                                       rgVal1[rgValIndex],
                                       rgVal2[rgValIndex]
                                       WIN64_ARG(cbClass));

            LOG((LF_CORDB|LF_ENC,LL_INFO10000,
                 "D::SVAO [%2d] varnum %d, nonVC type %x, addr %8.8x: %8.8x;%8.8x\n",
                 i,
                 varNativeInfo[i].varNumber,
                 varNativeInfo[i].loc.vlType,
                 NativeVarStackAddr(varNativeInfo[i].loc, pCtx),
                 rgVal1[rgValIndex],
                 rgVal2[rgValIndex]));

            if (res == TRUE)
            {
                continue;
            }
            _ASSERTE(res == TRUE);
            hr = E_FAIL;
            break;
        }

        // It's definately a value class.
        _ASSERTE(cbClass != 0);
        if (rgpVCs[iVC] == NULL)
        {
            // it's new in scope, so just clear it
            memset(NativeVarStackAddr(varNativeInfo[i].loc, pCtx), 0, cbClass);
            LOG((LF_CORDB|LF_ENC,LL_INFO10000, "D::SVAO [%2d] varnum %d, new VC len %d, addr %8.8x\n",
                 i,
                 varNativeInfo[i].varNumber,
                 cbClass,
                 NativeVarStackAddr(varNativeInfo[i].loc, pCtx)));
            continue;
        }
        // it's a pre-existing VC, so copy it
        memmove(NativeVarStackAddr(varNativeInfo[i].loc, pCtx), rgpVCs[iVC], cbClass);
#ifdef _DEBUG
        LOG((LF_CORDB|LF_ENC,LL_INFO10000,
             "D::SVAO [%2d] varnum %d, VC len %d, addr: %8.8x sample: %8.8x%8.8x\n",
             i,
             varNativeInfo[i].varNumber,
             cbClass,
             NativeVarStackAddr(varNativeInfo[i].loc, pCtx),
             rgpVCs[iVC][0],
             rgpVCs[iVC][1]));
#endif
        // Now get rid of the memory
        DeleteInteropSafe(rgpVCs[iVC]);
        rgpVCs[iVC] = NULL;
        iVC++;
    }

    LOG((LF_CORDB|LF_ENC, LL_INFO10000, "D::SVAO: returning %8.8x\n", hr));

    if (rgpVCs != NULL)
    {
        DeleteInteropSafe(rgpVCs);
    }

    return hr;
}



#endif // #ifndef DACCESS_COMPILE


#ifndef DACCESS_COMPILE


BOOL IsDuplicatePatch(SIZE_T *rgEntries, ULONG cEntries,
                      SIZE_T Entry )
{
    LEAF_CONTRACT;

    for( ULONG i = 0; i < cEntries;i++)
    {
        if (rgEntries[i] == Entry)
            return TRUE;
    }
    return FALSE;
}


/******************************************************************************
// HRESULT Debugger::MapAndBindFunctionBreakpoints():  For each breakpoint
//      that we've set in any version of the existing function,
//      set a correponding breakpoint in the new function if we haven't moved
//      the patch to the new version already.
//
//      This must be done _AFTER_ the MethodDesc has been udpated
//      with the new address (ie, when GetFunctionAddress pFD returns
//      the address of the new EnC code)
//
// Parameters:
// djiNew - this is the DJI created in D::JitComplete.
//   If djiNew == NULL iff we aren't tracking debug-info.
// fd - the method desc that we're binding too.
// addrOfCode - address of the native blob of code we just jitted
//
//                                                                         
//                                                                                                 
 ******************************************************************************/
HRESULT Debugger::MapAndBindFunctionPatches(DebuggerJitInfo *djiNew,
                                            MethodDesc * fd,
                                            CORDB_ADDRESS_TYPE *addrOfCode)
{
    // @@@
    // Internal helper API. Can be called from Debugger or Controller.
    //

    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        CALLED_IN_DEBUGGERDATALOCK_HOLDER_SCOPE_MAY_GC_TRIGGERS_CONTRACT;
        PRECONDITION(!djiNew || djiNew->m_fd == fd);
    }
    CONTRACTL_END;

    HRESULT     hr =                S_OK;
    HASHFIND    hf;
    SIZE_T      *pidTableEntry =    NULL;
    SIZE_T      pidInCaseTableMoves;
    Module      *pModule =          g_pEEInterface->MethodDescGetModule(fd);
    mdMethodDef md =                fd->GetMemberDef();

    LOG((LF_CORDB,LL_INFO10000,"D::MABFP: All BPs will be mapped to "
        "Ver:0x%04x (DJI:0x%08x)\n", djiNew?djiNew->m_methodInfo->GetCurrentEnCVersion():0, djiNew));

    // We need to traverse the patch list while under the controller lock (small lock).
    // But we can only send BreakpointSetErros while under the debugger lock (big lock).
    // So to avoid a lock violation, we queue any errors we find under the small lock,
    // and then send the whole list when under the big lock.
    PATCH_UNORDERED_ARRAY listUnbindablePatches;


    // First lock the patch table so it doesn't move while we're
    //  examining it.
    LOG((LF_CORDB,LL_INFO10000, "D::MABFP: About to lock patch table\n"));
    DebuggerController::ControllerLockHolder ch(&DebuggerController::g_criticalSection);

    // Manipulate tables AFTER lock's been acquired.
    DebuggerPatchTable *pPatchTable = DebuggerController::GetPatchTable();
    GetBPMappingDuplicates()->Clear(); //dups are tracked per-version

    for (DebuggerControllerPatch *dcp = pPatchTable->GetFirstPatch(&hf);
         dcp != NULL;
         dcp = pPatchTable->GetNextPatch( &hf ))
    {

        LOG((LF_CORDB, LL_INFO10000, "D::MABFP: got patch 0x%p\n", dcp));

        // Only copy over breakpoints that are in this method
        if (dcp->key.module != pModule || dcp->key.md != md)
        {
            LOG((LF_CORDB, LL_INFO10000, "Patch not in this method\n"));
            continue;
        }

        // Do not copy over slave breakpoint patches.  Instead place a new slave
        // based off the master.
        if (dcp->IsILSlavePatch())
        {
            LOG((LF_CORDB, LL_INFO10000, "Not copying over slave breakpoint patch\n"));
            continue;
        }

        // If the patch is already bound, then we don't want to try to rebind it.
        // Eg. It may be bound to a different generic method instantiation.
        if (dcp->IsBound())
        {
            LOG((LF_CORDB, LL_INFO10000, "Skipping already bound patch\n"));
            continue;
        }

        // Only apply breakpoint patches that are for this version.
        // If the patch doesn't have a particular EnCVersion available from its data then
        // we're (probably) not tracking JIT info.
        if (dcp->IsBreakpointPatch() && dcp->HasEnCVersion() && djiNew && dcp->GetEnCVersion() != djiNew->m_encVersion)
        {
            LOG((LF_CORDB, LL_INFO10000, "Not applying breakpoint patch to new version\n"));
            continue;
        }

        // Only apply breakpoint and stepper patches
        //
        // The DJI gets nuked as part of the Unbind/Rebind process in MovedCode.
        // This is to signal that we should not skip here.
        // <NICE> under exactly what scenarios (EnC, code pitching etc.) will this apply?... </NICE>
        // <NICE> can't we be a little clearer about why we don't want to bind the patch in this arcance situation?</NICE>
        if (dcp->HasDJI() && !dcp->IsBreakpointPatch() &&  !dcp->IsStepperPatch())
        {
            LOG((LF_CORDB, LL_INFO10000, "Neither stepper nor BP but we have valid a DJI (i.e. the DJI hasn't been nuked as part of the Unbind/MovedCode/Rebind mess)! - getting next patch!\n"));
            continue;
        }

        // Now check if we're tracking JIT info or not
        if (djiNew == NULL)
        {
            // This means we put a patch in a method w/ no debug info.
            _ASSERTE(dcp->IsBreakpointPatch() ||
                dcp->IsStepperPatch() ||
                dcp->controller->GetDCType() == DEBUGGER_CONTROLLER_THREAD_STARTER);

            // W/o Debug-info, We can only patch native offsets, and only at the start of the method (native offset 0).
            // We can't even do a IL-offset 0 because that's after the prolog and w/o the debug-info,
            // we don't know where the prolog ends.
            // Failing this assert is arguably an API misusage - the debugger should have enabled
            // jit-tracking if they wanted to put bps at offsets other than native:0.
            if (dcp->IsNativePatch() && (dcp->offset == 0))
            {
                DebuggerController::g_patches->BindPatch(dcp, addrOfCode);
                DebuggerController::ActivatePatch(dcp);
            }
            else
            {
                // IF a debugger calls EnableJitDebugging(true, ...) in the module-load callback,
                // we should never get here.
                *(listUnbindablePatches.AppendThrowing()) = dcp;
            }

        }
        else
        {
            pidInCaseTableMoves = dcp->pid;

            // If we've already mapped this one to the current version,
            //  don't map it again.
            LOG((LF_CORDB,LL_INFO10000,"D::MABFP: Checking if 0x%x is a dup...",
                pidInCaseTableMoves));

            if ( IsDuplicatePatch(GetBPMappingDuplicates()->Table(),
                GetBPMappingDuplicates()->Count(),
                pidInCaseTableMoves) )
            {
                LOG((LF_CORDB,LL_INFO10000,"it is!\n"));
                continue;
            }
            LOG((LF_CORDB,LL_INFO10000,"nope!\n"));

            // Attempt mapping from patch to new version of code, and
            // we don't care if it turns out that there isn't a mapping.
            hr = MapPatchToDJI( dcp, djiNew );
            if (CORDBG_E_CODE_NOT_AVAILABLE == hr )
            {
                *(listUnbindablePatches.AppendThrowing()) = dcp;
                hr = S_OK;
            }

            if (FAILED(hr))
                break;

            //Remember the patch id to prevent duplication later
            pidTableEntry = GetBPMappingDuplicates()->Append();
            if (NULL == pidTableEntry)
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            *pidTableEntry = pidInCaseTableMoves;
            LOG((LF_CORDB,LL_INFO10000,"D::MABFP Adding 0x%x to list of "
                "already mapped patches\n", pidInCaseTableMoves));
        }
    }

    ch.Release(); // unlock controller lock before sending events.    
    LOG((LF_CORDB,LL_INFO10000, "D::MABFP: Unlocked patch table\n"));


    // Now send any Breakpoint bind error events. 
    if (listUnbindablePatches.Count() > 0)
    {
        LockAndSendBreakpointSetError(&listUnbindablePatches);
    }
    
    return hr;
}

/******************************************************************************
// HRESULT Debugger::MapPatchToDJI():  Maps the given
//  patch to the corresponding location at the new address.
//  We assume that the new code has been JITTed.
// Returns:  CORDBG_E_CODE_NOT_AVAILABLE - Indicates that a mapping wasn't
//  available, and thus no patch was placed.  The caller may or may
//  not care.
 ******************************************************************************/
HRESULT Debugger::MapPatchToDJI( DebuggerControllerPatch *dcp,DebuggerJitInfo *djiTo)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        CALLED_IN_DEBUGGERDATALOCK_HOLDER_SCOPE_MAY_GC_TRIGGERS_CONTRACT;
        PRECONDITION(djiTo != NULL);
        PRECONDITION(djiTo->m_jitComplete == true);
    }
    CONTRACTL_END;

    _ASSERTE(DebuggerController::HasLock());
#ifdef _DEBUG
    static BOOL shouldBreak = -1;
    if (shouldBreak == -1)
        shouldBreak = UnsafeGetConfigDWORD(L"DbgBreakOnMapPatchToDJI", 0);

    if (shouldBreak > 0) {
        _ASSERTE(!"DbgBreakOnMatchPatchToDJI");
    }
#endif

    LOG((LF_CORDB, LL_EVERYTHING, "Calling MapPatchToDJI\n"));

    // We shouldn't have been asked to map an already bound patch
    _ASSERTE( !dcp->IsBound() );
    if ( dcp->IsBound() )
    {
        return S_OK;
    }

    // If the patch has no DJI then we're doing a UnbindFunctionPatches/RebindFunctionPatches.  Either
    // way, we simply want the most recent version.  In the absence of EnC we should have djiCur == djiTo.
    DebuggerJitInfo *djiCur = dcp->HasDJI() ? dcp->GetDJI() : djiTo;
    PREFIX_ASSUME(djiCur != NULL);

    // If the source and destination are the same version, then this method
    // decays into BindFunctionPatch's BindPatch function
    if (djiCur->m_encVersion == djiTo->m_encVersion)
    {
        // If the patch is a "master" then make a new "slave" patch instead of
        // binding the old one.  This is to stop us mucking with the master breakpoint patch
        // which we may need to bind several times for generic code.
        if (dcp->IsILMasterPatch())
        {
            LOG((LF_CORDB, LL_EVERYTHING, "Add, Bind, Activate new patch from master patch\n"));
            if (dcp->controller->AddBindAndActivateILSlavePatch(dcp, djiTo))
            {
                LOG((LF_CORDB, LL_INFO1000, "Add, Bind Activate went fine!\n" ));
                return S_OK;

            }
            else
            {
                LOG((LF_CORDB, LL_INFO1000, "Didn't work for some reason!\n"));

                // Caller can track this HR and send error.
                return CORDBG_E_CODE_NOT_AVAILABLE;
            }

        }
        else
        {
            _ASSERTE( dcp->GetKind() == PATCH_KIND_NATIVE_MANAGED );

            // We have an unbound native patch (eg. for PatchTrace), lets try to bind and activate it
            dcp->SetDJI(djiTo);
            LOG((LF_CORDB, LL_EVERYTHING, "trying to bind patch... could be problem\n"));
            if (DebuggerController::BindPatch(dcp, djiTo->m_fd, NULL))
            {
                DebuggerController::ActivatePatch(dcp);
                LOG((LF_CORDB, LL_INFO1000, "Application went fine!\n" ));
                return S_OK;
            }
            else
            {
                LOG((LF_CORDB, LL_INFO1000, "Didn't apply for some reason!\n"));

                // Caller can track this HR and send error.
                return CORDBG_E_CODE_NOT_AVAILABLE;
            }
        }
    }

    // Breakpoint patches never get mapped over
    _ASSERTE(!dcp->IsBreakpointPatch());

    return S_OK;
}

//
// Wrapper function for debugger to WaitForSingleObject. If CLR is hosted,
// notify host before we leave runtime.
//
DWORD  Debugger::WaitForSingleObjectHelper(HANDLE handle, DWORD dwMilliseconds)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    DWORD   dw = 0;
    EX_TRY
    {

        // make sure that we let host know that we are leaving runtime.
        LeaveRuntimeHolder holder((size_t)(::WaitForSingleObject));
        dw = ::WaitForSingleObject(handle,dwMilliseconds);
    }
    EX_CATCH
    {
        // Only possibility to enter here is when Thread::LeaveRuntime
        // throws exception.
        dw = WAIT_ABANDONED;
    }
    EX_END_CATCH(SwallowAllExceptions);
    return dw;

}


/* ------------------------------------------------------------------------ *
 * EE Interface routines
 * ------------------------------------------------------------------------ */

//
// SendSyncCompleteIPCEvent sends a Sync Complete event to the Right Side.
//
void Debugger::SendSyncCompleteIPCEvent()
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(ThreadHoldsLock());

        // Anyone sending the synccomplete must hold the TSL.
        PRECONDITION(ThreadStore::HoldingThreadStore() || g_fProcessDetach);

        // The sync complete is now only sent on a helper thread.
        PRECONDITION(ThisIsHelperThreadWorker());
        MODE_COOPERATIVE;

        // We had better be trapping Runtime threads and not stopped yet.
        PRECONDITION(m_stopped && m_trappingRuntimeThreads);



        // By the time we're sending a sync-complete, we should have already
        // attached (SYNC_STATE_0) or be passed SYNC_STATE_1.
        PRECONDITION(m_syncingForAttach != SYNC_STATE_1);
    }
    CONTRACTL_END;

    // @@@
    // Internal helper API.
    // This is to send Sync Complete event to RightSide.
    // We should have hold the debugger lock
    //

    STRESS_LOG0(LF_CORDB, LL_INFO10000, "D::SSCIPCE: sync complete.\n");

    // Synchronizing while in in rude shutdown should be extremely rare b/c we don't
    // TART in rude shutdown. Shutdown must have started after we started to sync.
    // We know we're not on the shutdown thread here.
    // And we also know we can't block the shutdown thread (b/c it has the TSL and will
    // get a free pass through the GC toggles that normally block threads for debugging).
    if (g_fProcessDetach)
    {
        STRESS_LOG0(LF_CORDB, LL_INFO10000, "D::SSCIPCE: Skipping for shutdown.\n");
        return;
    }

    DebuggerIPCControlBlock * pDCB = m_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC);
    PREFIX_ASSUME(pDCB != NULL); // must have DCB by the time we're sending IPC events.
    {
        STRESS_LOG0(LF_CORDB, LL_EVERYTHING, "GetIPCEventSendBuffer called in SendSyncCompleteIPCEvent\n");
        // Send the Sync Complete event to the Right Side
        DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce, DB_IPCE_SYNC_COMPLETE);

        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
    }
}

DebuggerModule* Debugger::LookupModule(Module* pModule, AppDomain *pAppDomain)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // DebuggerModules are relative to a specific AppDomain so we should always be looking up a module / 
    // AppDomain pair.
    _ASSERTE( pModule != NULL );
    _ASSERTE( pAppDomain != NULL );
    
    // This is called from all over. We just need to lock in order to lookup. We don't need
    // the lock when actually using the DebuggerModule (since it won't be unloaded as long as there is a thread
    // in that appdomain). Many of our callers already have this lock, many don't.
    // We can take the lock anyways because it's reentrant.
    DebuggerDataLockHolder ch(g_pDebugger); // need to traverse module list

    // if this is a module belonging to the system assembly, then scan
    // the complete list of DebuggerModules looking for the one
    // with a matching appdomain id
    // it.
    if (m_pModules == NULL)
    {
        return NULL;
    }

    DebuggerModule* dmod;

    _ASSERTE( SystemDomain::SystemAssembly()->IsDomainNeutral() );
    if (pModule->GetAssembly()->IsDomainNeutral())
    {
        // We have to make sure to lookup the module with the app domain parameter if the module lives in a shared assembly
        dmod = m_pModules->GetModule(pModule, pAppDomain);
    }
    else
    {
        dmod = m_pModules->GetModule(pModule);    
    }

    // The module must be in the AppDomain that was requested
    _ASSERTE( (dmod == NULL) || (dmod->GetAppDomain() == pAppDomain) );

    return dmod;    
}

//
void Debugger::TrapAllRuntimeThreads(DebuggerLockHolder *dbgLockHolder, AppDomain *pAppDomain)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;

        // We acquired the lock b/c we're in a scope between LFES & UFES.
        PRECONDITION(ThreadHoldsLock());

        // This should never be called on a Temporary Helper thread.
        PRECONDITION(IsDbgHelperSpecialThread() ||
                     (g_pEEInterface->GetThread() == NULL) ||
                     !g_pEEInterface->IsPreemptiveGCDisabled());
    }
    CONTRACTL_END;


    // If we're doing shutdown, then don't bother trying to communicate w/ the RS.
    // If we're not the thread doing shutdown, then we may be asynchronously killed by the OS.
    // If we are the thread in shutdown, don't TART b/c that may block and do complicated stuff.
    if (g_fProcessDetach)
    {
        STRESS_LOG0(LF_CORDB, LL_INFO10000, "D::TART: Skipping for shutdown.\n");
        return;
    }

    pAppDomain = NULL;

    // Only try to start trapping if we're not already trapping.
    if (m_trappingRuntimeThreads == FALSE)
    {
        bool fSuspended;

        STRESS_LOG0(LF_CORDB, LL_INFO10000, "D::TART: Trapping all Runtime threads.\n");

        // There's no way that we should be stopped and still trying to call this function.
        _ASSERTE(!m_stopped);

        // Mark that we're trapping now.
        m_trappingRuntimeThreads = TRUE;

        // Take the thread store lock.
        STRESS_LOG0(LF_CORDB,LL_INFO1000, "About to lock thread Store\n");
        ThreadStore::LockThreadStore(GCHeap::SUSPEND_FOR_DEBUGGER);
        STRESS_LOG0(LF_CORDB,LL_INFO1000, "Locked thread store\n");

        // We start the suspension here, and let the helper thread finish it.
        // If there's no helper thread, then we need to do helper duty.
        {
            SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
            fSuspended = g_pEEInterface->StartSuspendForDebug(pAppDomain, TRUE);
        }

        // We tell the RC Thread to check for other threads now and then and help them get synchronized. (This
        // is similar to what is done when suspending threads for GC with the HandledJITCase() function.)

        // This does not block.
        // Pinging this will waken the helper thread (or temp H. thread) and tell it to sweep & send
        // the sync complete.
        m_pRCThread->WatchForStragglers();

        // It's possible we may not have a real helper thread.
        // - on startup in dllmain, helper is blocked on DllMain loader lock.
        // - on shutdown, helper has been removed on us.
        // In those cases, we need somebody to send the sync-complete, and handle
        // managed events, and wait for the continue. So we pretend to be the helper thread.
        STRESS_LOG0(LF_CORDB, LL_EVERYTHING, "D::SSCIPCE: Calling IsRCThreadReady()\n");

        // We must check the helper thread status while under the lock.
        _ASSERTE(ThreadHoldsLock());
        // If we failed to suspend, then that means we must have multiple managed threads.
        // That means that our helper is not blocked on starting up, thus we can wait infinite on it.
        // Thus we don't need to do helper duty if the suspend fails.
        bool fShouldDoHelperDuty = !m_pRCThread->IsRCThreadReady() && fSuspended;
        if (fShouldDoHelperDuty && !g_fProcessDetach)
        {
            // In V1.0, we had the assumption that if the helper thread isn't ready yet, then we're in
            // a state that SuspendForDebug will succeed on the first try, and thus we'll
            // never call Sweep when doing helper thread duty.
            _ASSERTE(fSuspended);

            // This call will do a ton of work, it will toggle the lock,
            // and it will block until we receive a continue!
            DoHelperThreadDuty();

            // We will have released the TSL after the call to continue.
        }
        else
        {
            // We have a live and active helper thread which will handle events
            // from the RS now that we're stopped.
            // We need to release the TSL which we acquired above. (The helper will
            // likely take this lock while doing stuff).
            STRESS_LOG0(LF_CORDB,LL_INFO1000, "About to unlock thread store!\n");
            ThreadStore::UnlockThreadStore(FALSE, GCHeap::SUSPEND_FOR_DEBUGGER);
            STRESS_LOG0(LF_CORDB,LL_INFO1000, "TART: Unlocked thread store!\n");
        }
        _ASSERTE(ThreadHoldsLock()); // still hold the lock. (though it may have been toggled)
    }
}


//
// ReleaseAllRuntimeThreads releases all Runtime threads that may be
// stopped after trapping and sending the at safe point event.
//
void Debugger::ReleaseAllRuntimeThreads(AppDomain *pAppDomain)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;

        // We acquired the lock b/c we're in a scope between LFES & UFES.
        PRECONDITION(ThreadHoldsLock());

        // Currently, this is only done on a helper thread.
        PRECONDITION(ThisIsHelperThreadWorker());

        // Make sure that we were stopped...
        PRECONDITION(m_trappingRuntimeThreads && m_stopped);
    }
    CONTRACTL_END;

    pAppDomain = NULL;

    STRESS_LOG1(LF_CORDB, LL_INFO10000, "D::RART: Releasing all Runtime threads"
        "for AppD 0x%x.\n", pAppDomain);

    // Mark that we're on our way now...
    m_trappingRuntimeThreads = FALSE;
    m_stopped = FALSE;

    // Go ahead and resume the Runtime threads.
    g_pEEInterface->ResumeFromDebug(pAppDomain);
}

// Given a method, get's its EnC version number. 1 if the method is not EnCed.
// Note that MethodDescs are reused between versions so this will give us
// the most recent EnC number.
int Debugger::GetMethodEncNumber(MethodDesc * pMethod)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    DebuggerJitInfo * dji = GetLatestJitInfoFromMethodDesc(pMethod);
    if (dji == NULL)
    {
        // If there's no DJI, couldn't have been EnCed.
        return 1;
    }
    return (int) dji->m_encVersion;
}


bool Debugger::IsJMCMethod(Module* pModule, mdMethodDef tkMethod)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CORDebuggerAttached());
    }
    CONTRACTL_END;

#ifdef _DEBUG
    Crst crstDbg("dbg", CrstIsJMCMethod, CRST_UNSAFE_ANYMODE);
    PRECONDITION(crstDbg.IsSafeToTake());
#endif

    DebuggerMethodInfo *pInfo = GetOrCreateMethodInfo(pModule, tkMethod);

    if (pInfo == NULL)
        return false;

    return pInfo->IsJMCFunction();
}

/******************************************************************************
 * Called by Runtime when on a 1st chance Native Exception.
 * This is likely when we hit a breakpoint / single-step.
 * This is called for all native exceptions (except COM+) on managed threads,
 * regardless of whether the debugger is attached.
 ******************************************************************************/
bool Debugger::FirstChanceNativeException(EXCEPTION_RECORD *exception,
                                          CONTEXT *context,
                                          DWORD code,
                                          Thread *thread)
{

    // @@@
    // Implement DebugInterface
    // Can be called from EE exception code. Or from our M2UHandoffHijackFilter
    // must be on managed thread.

    CONTRACTL
    {
        SO_TOLERANT;
        NOTHROW;

        // No clear GC_triggers semantics here. See DispatchNativeException.
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;

        PRECONDITION(DebuggerMaybeInvolved()); // see caller for details

        PRECONDITION(CheckPointer(exception));
        PRECONDITION(CheckPointer(context));
        PRECONDITION(CheckPointer(thread));
    }
    CONTRACTL_END;

    bool retVal;

    // Don't stop for native debugging anywhere inside our inproc-Filters.
    CantStopHolder hHolder;

    if (!CORDBUnrecoverableError(this))
    {
        retVal = DebuggerController::DispatchNativeException(exception, context,
                                                           code, thread);
    }
    else
    {
        retVal = false;
    }

    return retVal;
}

/******************************************************************************
 *
 ******************************************************************************/
PRD_TYPE Debugger::GetPatchedOpcode(CORDB_ADDRESS_TYPE *ip)
{
    WRAPPER_CONTRACT;

    if (!CORDBUnrecoverableError(this))
    {
        return DebuggerController::GetPatchedOpcode(ip);
    }
    else
    {
        PRD_TYPE mt;
        InitializePRD(&mt);
        return mt;
    }
}

/******************************************************************************
 *
 ******************************************************************************/
BOOL Debugger::CheckGetPatchedOpcode(CORDB_ADDRESS_TYPE *address, /*OUT*/ PRD_TYPE *pOpcode)
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(CheckPointer(address));
    CONSISTENCY_CHECK(CheckPointer(pOpcode));

    if (CORDebuggerAttached() && !CORDBUnrecoverableError(this))
    {
        return DebuggerController::CheckGetPatchedOpcode(address, pOpcode);
    }
    else
    {
        InitializePRD(pOpcode);
        return FALSE;
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void Debugger::TraceCall(const BYTE *code)
{
    CONTRACTL
    {
        // We're being called right before we call managed code. Can't trigger
        // because there may be unprotected args on the stack.
        MODE_COOPERATIVE;
        GC_NOTRIGGER;

        NOTHROW;
    }
    CONTRACTL_END;


    Thread * pCurThread = g_pEEInterface->GetThread();
    // Ensure we never even think about running managed code on the helper thread.
    _ASSERTE(!ThisIsHelperThreadWorker() || !"You're running managed code on the helper thread");

    // One threat is that our helper thread may be forced to execute a managed DLL main.
    // In that case, it's before the helper thread proc is even executed, so our conventional
    // IsHelperThread() checks are inadequate.
    _ASSERTE((GetCurrentThreadId() != g_pRCThread->m_DbgHelperThreadOSTid) || !"You're running managed code on the helper thread");

    _ASSERTE((g_pEEInterface->GetThreadFilterContext(pCurThread) == NULL) || !"Shouldn't run managed code w/ Filter-Context set");

    if (!CORDBUnrecoverableError(this))
    {
        // There are situations where our callers can't tolerate us throwing.  
        EX_TRY
        {
            // Since we have a try catch and the debugger code can deal properly with 
            // faults occuring inside DebuggerController::DispatchTraceCall, we can safely
            // establish a FAULT_NOT_FATAL region. This is required since some callers can't
            // tolerate faults.
            FAULT_NOT_FATAL();
            
            DebuggerController::DispatchTraceCall(pCurThread, code);
        }
        EX_CATCH
        {
            // We're being called for our benefit, not our callers. So if we fail,
            // they don't care.
            // Failure for us means that some steppers may miss their notification
            // for entering managed code.
            LOG((LF_CORDB, LL_INFO10000, "Debugger::TraceCall - inside catch, %p\n", code));
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void Debugger::PossibleTraceCall(UMEntryThunk *pUMEntryThunk, Frame *pFrame)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;

        // TraceCall is NO_TRIGGERS, but PossibleTraceCall is actually triggers.
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    // Ensure we never even think about running managed code on the helper thread.
    _ASSERTE(!ThisIsHelperThreadWorker() || !"You're running managed code on the helper thread");

    // One threat is that our helper thread may be forced to execute a managed DLL main.
    // In that case, it's before the helper thread proc is even executed, so our conventional
    // IsHelperThread() checks are inadequate.
    _ASSERTE((GetCurrentThreadId() != g_pRCThread->m_DbgHelperThreadOSTid) || !"You're running managed code on the helper thread");


    if (!CORDBUnrecoverableError(this))
    {
        // This is just a notification that we've entered the runtime.
        // Our callers aren't prepared to handle us throwing, and may not even
        // have an SEH filter up, so we'll just catch and eat exceptions here.
        EX_TRY
        {
            DebuggerController::DispatchPossibleTraceCall(g_pEEInterface->GetThread(), pUMEntryThunk, pFrame);
        }
        EX_CATCH
        {
            // We're being called for our benefit, not our callers. So if we fail,
            // they don't care.
            // Failure for us means that some steppers may miss their notification
            // for entering managed code.
            LOG((LF_CORDB, LL_INFO10000, "Debugger::PossibleTraceCall - inside catch, %p,%p\n", pUMEntryThunk, pFrame));
        }
        EX_END_CATCH(SwallowAllExceptions);

    }
}

/******************************************************************************
 * For Just-My-Code (aka Just-User-Code).
 * Invoked from a probe in managed code when we enter a user method and
 * the flag (set by GetJMCFlagAddr) for that method is != 0.
 * pIP - the ip within the method, right after the prolog.
 * sp  - stack pointer (frame pointer on x86) for the managed method we're entering.
 * bsp - backing store pointer for the managed method we're entering
  ******************************************************************************/
void Debugger::OnMethodEnter(void * pIP)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000000, "D::OnMethodEnter(ip=%p)\n", pIP));

    if (!CORDebuggerAttached())
    {
        LOG((LF_CORDB, LL_INFO1000000, "D::OnMethodEnter returning since debugger attached.\n"));
        return;
    }
    FramePointer fp = LEAF_MOST_FRAME;
    DebuggerController::DispatchMethodEnter(pIP, fp);
}
/******************************************************************************
 * GetJMCFlagAddr
 * Provide an address of the flag that the JMC probes use to decide whether
 * or not to call TriggerMethodEnter.
 * Called for each method that we jit.
 * md - method desc for the JMC probe
 * returns an address of a flag that the probe can use.
 ******************************************************************************/
DWORD* Debugger::GetJMCFlagAddr(Module * pModule)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END;

    // This callback will be invoked whenever we jit debuggable code.
    // A debugger may not be attached yet, but we still need someplace
    // to store this dword.
    // Use the EE's module, because it's always around, even if a debugger
    // is attached or not.
    return &(pModule->m_dwDebuggerJMCProbeCount);
}

/******************************************************************************
 * Updates the JMC flag on all the EE modules.
 * We can do this as often as we'd like - though it's a perf hit.
 ******************************************************************************/
void Debugger::UpdateAllModuleJMCFlag(bool fStatus)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000000, "D::UpdateModuleJMCFlag to %d\n", fStatus));

    _ASSERTE(HasDebuggerDataLock());

    // Loop through each module.
    HASHFIND f;
    for (DebuggerModule * m = m_pModules->GetFirstModule(&f);
         m != NULL;
         m = m_pModules->GetNextModule(&f))
    {
        // the primary module may get called multiple times, but that's ok.
        DebuggerModule * pPrimary = m->GetPrimaryModule();

        UpdateModuleJMCFlag(pPrimary, fStatus);

    } // end for all modules.
}

/******************************************************************************
 * Updates the JMC flag on the given Primary module
 * We can do this as often as we'd like - though it's a perf hit.
 * If we've only changed methods in a single module, then we can just call this.
 * If we do a more global thing (Such as enable MethodEnter), then that could
 * affect all modules, so we use the UpdateAllModuleJMCFlag helper.
 ******************************************************************************/
void Debugger::UpdateModuleJMCFlag(DebuggerModule * pPrimary, bool fStatus)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(pPrimary->GetPrimaryModule() == pPrimary);
    _ASSERTE(HasDebuggerDataLock());


    DWORD * pFlag = &(pPrimary->GetRuntimeModule()->m_dwDebuggerJMCProbeCount);
    _ASSERTE(pFlag != NULL);

    if (pPrimary->HasAnyJMCFunctions())
    {
        // If this is a user-code module, then update the JMC flag
        // the probes look at so that we get MethodEnter callbacks.
        *pFlag = fStatus;

        LOG((LF_CORDB, LL_EVERYTHING, "D::UpdateModuleJMCFlag, module %p is user code\n", pPrimary));
    } else {
        LOG((LF_CORDB, LL_EVERYTHING, "D::UpdateModuleJMCFlag, module %p is not-user code\n", pPrimary));

        // if non-user code, flag should be 0 so that we don't waste
        // cycles in the callbacks.
        _ASSERTE(*pFlag == 0);
    }
}


/******************************************************************************
 * Called by GC to determine if it's safe to do a GC.
 ******************************************************************************/
bool Debugger::ThreadsAtUnsafePlaces(void)
{
    LEAF_CONTRACT;

    // If we're in shutdown mode, then all other threads are parked.
    // Even if they claim to be at unsafe regions, they're still safe to do a GC. They won't touch
    // their stacks.
    if (m_fShutdownMode)
    {
        if (m_threadsAtUnsafePlaces > 0)
        {
            STRESS_LOG1(LF_CORDB, LL_INFO10000, "D::TAUP: Claiming safety in shutdown mode.%d\n", m_threadsAtUnsafePlaces);
        }
        return false;
    }


    return (m_threadsAtUnsafePlaces != 0);
}

//
// SendBreakpoint is called by Runtime threads to send that they've
// hit a breakpoint to the Right Side.
//
void Debugger::SendBreakpoint(Thread *thread, CONTEXT *context,
                              DebuggerBreakpoint *breakpoint)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (CORDBUnrecoverableError(this))
        return;

#ifdef _DEBUG
    static BOOL shouldBreak = -1;
    if (shouldBreak == -1)
        shouldBreak = REGUTIL::GetConfigDWORD(L"DbgBreakOnSendBreakpoint", 0);

    if (shouldBreak > 0) {
        _ASSERTE(!"DbgBreakOnSendBreakpoint");
    }
#endif

    LOG((LF_CORDB, LL_INFO10000, "D::SB: breakpoint BP:0x%x\n", breakpoint));

    _ASSERTE((g_pEEInterface->GetThread() &&
             !g_pEEInterface->GetThread()->m_fPreemptiveGCDisabled) ||
             g_fInControlC);

    _ASSERTE(ThreadHoldsLock());

    // Send a breakpoint event to the Right Side
    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    InitIPCEvent(ipce,
                 DB_IPCE_BREAKPOINT,
                 thread,
                 thread->GetDomain());
    ipce->BreakpointData.breakpointToken.Set(breakpoint);
    _ASSERTE( breakpoint->m_pAppDomain == ipce->appDomainToken.UnWrap());

    m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
}

//****************************************************************
//
// SendRawUserBreakpoint is called by Runtime threads to send that
// they've hit a user breakpoint to the Right Side. This is the event
// send only part, since it can be called from a few different places.
//
// thread [in] : managed thread where user break point takes place
//
//****************************************************************
void Debugger::SendRawUserBreakpoint(Thread *thread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (CORDBUnrecoverableError(this))
        return;

    LOG((LF_CORDB, LL_INFO10000, "D::SRUB: user breakpoint\n"));

    if (GetThread() != NULL)
    {
        // If this function is executed on helper thread, we should be checking on
        // this condition if executed on managed thread.
        //
        _ASSERTE(!g_pEEInterface->IsPreemptiveGCDisabled());
    }
    else
    {
    }
    _ASSERTE(ThreadHoldsLock());

    // Send a breakpoint event to the Right Side
    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    InitIPCEvent(ipce,
                 DB_IPCE_USER_BREAKPOINT,
                 thread,
                 thread->GetDomain());

    m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
}

//
// SendInterceptExceptionComplete is called by Runtime threads to send that
// they've completed intercepting an exception to the Right Side. This is the event
// send only part, since it can be called from a few different places.
//
void Debugger::SendInterceptExceptionComplete(Thread *thread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (CORDBUnrecoverableError(this))
        return;

    LOG((LF_CORDB, LL_INFO10000, "D::SIEC: breakpoint\n"));

    _ASSERTE(!g_pEEInterface->IsPreemptiveGCDisabled());
    _ASSERTE(ThreadHoldsLock());

    // Send a breakpoint event to the Right Side
    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    InitIPCEvent(ipce,
                 DB_IPCE_INTERCEPT_EXCEPTION_COMPLETE,
                 thread,
                 thread->GetDomain());

    m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
}



//
// SendStep is called by Runtime threads to send that they've
// completed a step to the Right Side.
//
void Debugger::SendStep(Thread *thread, CONTEXT *context,
                        DebuggerStepper *stepper,
                        CorDebugStepReason reason)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (CORDBUnrecoverableError(this))
        return;

    LOG((LF_CORDB, LL_INFO10000, "D::SS: step:token:0x%p reason:0x%x\n",
        stepper, reason));

    _ASSERTE((g_pEEInterface->GetThread() &&
             !g_pEEInterface->GetThread()->m_fPreemptiveGCDisabled) ||
             g_fInControlC);

    _ASSERTE(ThreadHoldsLock());

    // Send a step event to the Right Side
    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    InitIPCEvent(ipce,
                 DB_IPCE_STEP_COMPLETE,
                 thread,
                 thread->GetDomain());
    ipce->StepData.stepperToken.Set(stepper);
    ipce->StepData.reason = reason;
    m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
}

//-------------------------------------------------------------------------------------------------
// Send an EnC remap opportunity and block until it is continued.
//
// dji - current method information
// currentIP - IL offset within that method 
// resumeIP - address of a SIZE_T that the RS will write to cross-process if they take the
//  remap opportunity. *resumeIP is untouched if the RS does not remap. 
//-------------------------------------------------------------------------------------------------
void Debugger::LockAndSendEnCRemapEvent(DebuggerJitInfo * dji, SIZE_T currentIP, SIZE_T *resumeIP)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS; // From SendIPCEvent
        PRECONDITION(dji != NULL);
    }
    CONTRACTL_END;


    LOG((LF_CORDB, LL_INFO10000, "D::LASEnCRE:\n"));

    if (CORDBUnrecoverableError(this))
        return;

    MethodDesc * pFD = dji->m_fd;

    // Note that the debugger lock is reentrant, so we may or may not hold it already.
    Debugger::DebuggerLockHolder dbgLockHolder(this, FALSE);
    SENDIPCEVENT_BEGIN(&dbgLockHolder);

    // Send an EnC remap event to the Right Side.
    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    Thread *thread = g_pEEInterface->GetThread();
    InitIPCEvent(ipce,
                 DB_IPCE_ENC_REMAP,
                 thread,
                 thread->GetDomain());

    ipce->EnCRemap.currentVersionNumber = dji->m_encVersion;
    ipce->EnCRemap.resumeVersionNumber = dji->m_methodInfo->GetCurrentEnCVersion();;
    ipce->EnCRemap.currentILOffset = currentIP;
    ipce->EnCRemap.resumeILOffset = resumeIP;
    ipce->EnCRemap.funcMetadataToken = pFD->GetMemberDef();

    LOG((LF_CORDB, LL_INFO10000, "D::LASEnCRE: token 0x%x, from version %d to %d\n",
    ipce->EnCRemap.funcMetadataToken, ipce->EnCRemap.currentVersionNumber, ipce->EnCRemap.resumeVersionNumber));

    Module *pRuntimeModule = pFD->GetModule();

    ipce->EnCRemap.debuggerModuleToken.Set(LookupModule(
                                            pRuntimeModule,
                                            thread->GetDomain()));

    LOG((LF_CORDB, LL_INFO10000, "D::LASEnCRE: %s::%s "
        "dmod:0x%x, methodDef:0x%x \n",
        pFD->m_pszDebugClassName, pFD->m_pszDebugMethodName,
        ipce->EnCRemap.debuggerModuleToken.UnWrap(),
        ipce->EnCRemap.funcMetadataToken));

    // IPC event is now initialized, so we can send it over.
    SendSimpleIPCEventAndBlock(&dbgLockHolder, IPC_TARGET_OUTOFPROC);

    // This will block on the continue
    SENDIPCEVENT_END;

    LOG((LF_CORDB, LL_INFO10000, "D::LASEnCRE: done\n"));

}
void Debugger::LockAndSendEnCRemapCompleteEvent(MethodDesc *pFD)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::LASEnCRE:\n"));

    if (CORDBUnrecoverableError(this))
        return;

    // Note that the debugger lock is reentrant, so we may or may not hold it already.
    Debugger::DebuggerLockHolder dbgLockHolder(this, FALSE);
    SENDIPCEVENT_BEGIN(&dbgLockHolder);

    DebuggerJitInfo *dji = NULL;

    EX_TRY
    {
        // Should always be resuming into the latest one
        dji = g_pDebugger->GetLatestJitInfoFromMethodDesc(pFD);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(RethrowTerminalExceptions);

    if (dji == NULL)
    {
        return;
    }

    // Send an EnC remap event to the Right Side.
    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    Thread *thread = g_pEEInterface->GetThread();
    InitIPCEvent(ipce,
                 DB_IPCE_ENC_REMAP_COMPLETE,
                 thread,
                 thread->GetDomain());


    ipce->EnCRemapComplete.funcMetadataToken = pFD->GetMemberDef();

    Module *pRuntimeModule = pFD->GetModule();

    ipce->EnCRemapComplete.debuggerModuleToken.Set(LookupModule(
                                            pRuntimeModule,
                                            thread->GetDomain()));

    LOG((LF_CORDB, LL_INFO10000, "D::LASEnCRC: %s::%s "
        "dmod:0x%x, methodDef:0x%x \n",
        pFD->m_pszDebugClassName, pFD->m_pszDebugMethodName,
        ipce->EnCRemap.debuggerModuleToken.UnWrap(),
        ipce->EnCRemap.funcMetadataToken));

    // IPC event is now initialized, so we can send it over.
    SendSimpleIPCEventAndBlock(&dbgLockHolder, IPC_TARGET_OUTOFPROC);

    // This will block on the continue
    SENDIPCEVENT_END;

    LOG((LF_CORDB, LL_INFO10000, "D::LASEnCRC: done\n"));

}
//
// This function sends a notification to the RS that a function has been
// updated with a new version. At this point, the EE is already stopped
// for handling an EnC ApplyChanges operation, so no need to take locks etc.
//
void Debugger::SendEnCUpdateEvent(DebuggerIPCEventType eventType, Module *pModule, mdToken memberToken, mdTypeDef classToken, SIZE_T enCVersion)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::LASEnCUFE:\n"));

    _ASSERTE(eventType == DB_IPCE_ENC_UPDATE_FUNCTION ||
                      eventType == DB_IPCE_ENC_ADD_FUNCTION ||
                      eventType== DB_IPCE_ENC_ADD_FIELD);

    if (CORDBUnrecoverableError(this))
        return;

    // Send an EnC UpdateFunction event to the Right Side.
    DebuggerIPCEvent* event = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    InitIPCEvent(event,
                 eventType,
                 NULL,
                 NULL);

    event->EnCUpdate.newVersionNumber = enCVersion;
    event->EnCUpdate.memberMetadataToken = memberToken;
    // we have to pass the class token across to the RS because we cannot look it up over
    // there based on the added field/method because the metadata on the RS will not yet
    // have the changes applied, so the token will not exist in its metadata and we have
    // no way to find it.
    event->EnCUpdate.classMetadataToken = classToken;

    _ASSERTE(pModule);
    // we don't support shared assemblies, so must have an appdomain
    _ASSERTE(pModule->GetDomain()->IsAppDomain());
    event->EnCUpdate.debuggerModuleToken.Set(LookupModule(
                                                pModule,
                                                pModule->GetDomain()->AsAppDomain()));

     LOG((LF_CORDB, LL_INFO10000, "D::LASEnCUE: "
        "dmod:0x%p, token:0x%x, class:0x%x, newVersion:0x%x \n",
        event->EnCUpdate.debuggerModuleToken.UnWrap(),
        event->EnCUpdate.memberMetadataToken,
        event->EnCUpdate.classMetadataToken,
        event->EnCUpdate.newVersionNumber));

    m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

    LOG((LF_CORDB, LL_INFO10000, "D::LASEnCUE: done\n"));

}


//
// Send a BreakpointSetError event to the Right Side if the given patch is for a breakpoint. Note: we don't care if this
// fails, there is nothing we can do about it anyway, and the breakpoint just wont hit.
//
void Debugger::LockAndSendBreakpointSetError(PATCH_UNORDERED_ARRAY * listUnbindablePatches) 
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE(listUnbindablePatches != NULL);
    
    if (CORDBUnrecoverableError(this))
        return;


    ULONG count = listUnbindablePatches->Count();
    _ASSERTE(count > 0); // must send at least 1 event.
    

    // Note that the debugger lock is reentrant, so we may or may not hold it already.
    Debugger::DebuggerLockHolder dbgLockHolder(this, FALSE);
    SENDIPCEVENT_BEGIN(&dbgLockHolder);

    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    Thread *thread = g_pEEInterface->GetThread();

    for(ULONG i =  0; i < count; i++)
    {        
        DebuggerControllerPatch *patch = listUnbindablePatches->Table()[i];
        _ASSERTE(patch != NULL);

        // Only do this for breakpoint controllers
        DebuggerController *controller = patch->controller;

        if (controller->GetDCType() != DEBUGGER_CONTROLLER_BREAKPOINT)
        {
            continue;
        }

        LOG((LF_CORDB, LL_INFO10000, "D::LASBSE:\n"));

        // Send a breakpoint set error event to the Right Side.
        InitIPCEvent(ipce, DB_IPCE_BREAKPOINT_SET_ERROR, thread, thread->GetDomain());

        ipce->BreakpointSetErrorData.breakpointToken.Set(static_cast<DebuggerBreakpoint*> (controller));

        // IPC event is now initialized, so we can send it over.
        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
    }

    // Stop all Runtime threads
    TrapAllRuntimeThreads(&dbgLockHolder, NULL);

    // This will block on the continue
    SENDIPCEVENT_END;

}

//
// Called from the controller to lock the debugger for event
// sending. This is called before controller events are sent, like
// breakpoint, step complete, and thread started.
//
// Note that it's possible that the debugger detached (and destroyed our IPC
// events) while we're waiting for our turn.
// So Callers should check for that case.
void Debugger::LockForEventSending(DebuggerLockHolder *dbgLockHolder, BOOL fNoRetry)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END;

    IncCantStopCount(StateHolderParamValue);
    _ASSERTE(IsInCantStopRegion());

    // What we need is for caller to get the debugger lock
    if (dbgLockHolder != NULL)
    {
        dbgLockHolder->Acquire();
    }

#ifdef _DEBUG
     // Track our TID. We're not re-entrant.
    //_ASSERTE(m_tidLockedForEventSending == 0);
    m_tidLockedForEventSending = GetCurrentThreadId();
#endif

}

//
// Called from the controller to unlock the debugger from event
// sending. This is called after controller events are sent, like
// breakpoint, step complete, and thread started.
//
void Debugger::UnlockFromEventSending(DebuggerLockHolder *dbgLockHolder)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END;

#ifdef _DEBUG
    //_ASSERTE(m_tidLockedForEventSending == GetCurrentThreadId());
    m_tidLockedForEventSending = 0;
#endif
    if (dbgLockHolder != NULL)
    {
        dbgLockHolder->Release();
    }
    _ASSERTE(IsInCantStopRegion());
    DecCantStopCount(StateHolderParamValue);
}


//
// Called from the controller after all events have been sent for a
// thread to sync the process.
//
void Debugger::SyncAllThreads(DebuggerLockHolder *dbgLockHolder)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    if (CORDBUnrecoverableError(this))
        return;

    STRESS_LOG0(LF_CORDB, LL_INFO10000, "D::SAT: sync all threads.\n");

    Thread *pThread = g_pEEInterface->GetThread();
    _ASSERTE((pThread &&
             !pThread->m_fPreemptiveGCDisabled) ||
              g_fInControlC);

    _ASSERTE(ThreadHoldsLock());

    // Stop all Runtime threads
    TrapAllRuntimeThreads(dbgLockHolder, pThread->GetDomain());
}

/******************************************************************************
 *
 ******************************************************************************/
HRESULT Debugger::LaunchDebuggerForUser (void)
{
    WRAPPER_CONTRACT;

    LOG((LF_CORDB, LL_INFO10000, "D::LDFU: Attaching Debugger.\n"));

    // Should we ask the user if they want to attach here?

    return(AttachDebuggerForBreakpoint(g_pEEInterface->GetThread(),
                                       L"Launch for user"));
}

/******************************************************************************
 * Get the format string for launching a jit debugging.
 * This returns a pointer to memory allocated on the interop-safe heap,
 * the caller must free this memory.
 ******************************************************************************/
WCHAR *Debugger::GetDebuggerLaunchStringTemplate(BOOL useManagedDebugger)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    WCHAR *cmd = NULL;
    DWORD len;

    // First, try the environment...
    len = WszGetEnvironmentVariable(CorDB_ENV_DEBUGGER_KEY, NULL, 0);

    if (len > 0)
    {
        // Len includes the terminating null. Note: using (interopsafe) because we may be out of other memory, not
        // because we're on the helper thread.
        cmd = new (interopsafe, nothrow) WCHAR[len];

        if (cmd != NULL)
        {
            DWORD newlen = WszGetEnvironmentVariable(CorDB_ENV_DEBUGGER_KEY, cmd, len);

            if (newlen == 0 || newlen > len)
            {
                DeleteInteropSafe(cmd);
                cmd = NULL;
            }
        }
    }

    if (cmd == NULL)
    {
        // Note: using (interopsafe) because we may be out of other memory,
        // not because we're on the helper thread.
        cmd = new (interopsafe, nothrow) WCHAR[MAX_PATH];
        if (cmd) {
            if (!PAL_FetchConfigurationStringW(TRUE,
                               CorDB_REG_DEBUGGER_KEY,
                               cmd,
                               MAX_PATH)) {
                DeleteInteropSafe(cmd);
                cmd = NULL;
            }
        }
    }

    return cmd;
}


//-----------------------------------------------------------------------------
// Get the full launch string for a jit debugger.
//
// If a jit-debugger is registed, then writes string into pStrArgsBuf and
//   return true.
//
// If no jit-debugger is registered, then return false.
//
// Throws on error (like OOM).
//-----------------------------------------------------------------------------
bool Debugger::GetCompleteDebuggerLaunchString(AppDomain* pAppDomain, LPCWSTR wszAttachReason, BOOL bUseManagedDebugger, SString * pStrArgsBuf)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;


    DWORD pid = GetCurrentProcessId();

    // Grab the ID for this appdomain.
    ULONG appId = (pAppDomain == NULL) ? 0 : (pAppDomain->GetId().m_dwId);

    // Get the debugger to launch. realDbgCmd will point to a buffer that was allocated with (interopsafe) if
    // there is a user-specified debugger command string.
    WCHAR* realDbgCmd = GetDebuggerLaunchStringTemplate(bUseManagedDebugger);

    if (realDbgCmd != NULL)
    {
        if (bUseManagedDebugger)
        {
            pStrArgsBuf->Printf(realDbgCmd, pid, appId, wszAttachReason, GetAttachAbortEvent());
        }
        else
        {
            pStrArgsBuf->Printf(realDbgCmd, pid, GetUnmanagedAttachEvent());
        }
    }
    else
    {
        // No jit-debugger available. Don't make one up.
        return false;
    }

    DeleteInteropSafe(realDbgCmd);  // DeleteInteropSafe does handle NULL safely.
    return true;
}


// Proxy code for EDA
struct EnsureDebuggerAttachedParams
{
    Debugger*                   m_pThis;
    AppDomain *                 m_pAppDomain;
    LPCWSTR                      m_wszAttachReason;
    HRESULT                     m_retval;
    BOOL                        m_useManagedDebugger;
    EnsureDebuggerAttachedParams() :
        m_pThis(NULL), m_pAppDomain(NULL), m_wszAttachReason(NULL), m_retval(E_FAIL), m_useManagedDebugger(FALSE) {LEAF_CONTRACT; }
};

// This is called by the helper thread
void EDAHelperStub(EnsureDebuggerAttachedParams * p)
{
    WRAPPER_CONTRACT;

    p->m_retval = p->m_pThis->EDAHelper(p->m_pAppDomain, p->m_wszAttachReason, p->m_useManagedDebugger);
}

// This gets called just like the normal version, but it sends the call over to the helper thread
HRESULT Debugger::EDAHelperProxy(AppDomain *pAppDomain, LPCWSTR wszAttachReason, BOOL useManagedDebugger)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE(!ThisIsHelperThreadWorker());
    _ASSERTE(ThreadHoldsLock());

    HRESULT hr = LazyInitWrapper();
    if (FAILED(hr))
    {
        // We already stress logged this case.
        return hr;
    }


    if (!IsGuardPageGone())
    {
        return EDAHelper(pAppDomain, wszAttachReason, useManagedDebugger);
    }

    EnsureDebuggerAttachedParams p;
    p.m_pThis = this;
    p.m_pAppDomain= pAppDomain;
    p.m_wszAttachReason = wszAttachReason;
    p.m_useManagedDebugger = useManagedDebugger;

    LOG((LF_CORDB, LL_INFO1000000, "D::EDAHelperProxy\n"));
    m_pRCThread->DoFavor((FAVORCALLBACK) EDAHelperStub, &p);
    LOG((LF_CORDB, LL_INFO1000000, "D::EDAHelperProxy return\n"));

    return p.m_retval;
}

// We can't have the helper thread execute all of EDA because it will deadlock.
// EDA will wait on m_exAttachEvent, which can only be set by the helper thread
// processing DB_IPCE_CONTINUE. But if the helper is stuck waiting in EDA, it
// can't handle the event and we deadlock.

// So, we factor out the stack intensive portion (CreateProcess & MessageBox)
// of EnsureDebuggerAttached. Conviently, this portion doesn't block
// and so won't cause any deadlock.
//   E_ABORT - if the attach was declined
//   S_OK    - Jit-attach successfully started
HRESULT Debugger::EDAHelper(AppDomain *pAppDomain, LPCWSTR wszAttachReason, BOOL useManagedDebugger)
{
    CONTRACTL
    {
        NOTHROW;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;

        PRECONDITION(ThisMaybeHelperThread()); // on helper if stackoverflow.
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    LOG((LF_CORDB, LL_INFO10000, "D::EDA: thread 0x%x is launching the debugger.\n", GetCurrentThreadId()));

    _ASSERTE(HasLazyData());

    // Another potential hang. This may get run on the helper if we have a stack overflow.
    // Hopefully the odds of 1 thread hitting a stack overflow while another is stuck holding the heap
    // lock is very small.
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;

    BOOL fCreateSucceeded = FALSE;

    StackSString strDbgCommand;
    const WCHAR * wszDbgCommand = NULL;
    SString strCurrentDir;
    const WCHAR * wszCurrentDir = NULL;

    EX_TRY
    {

        // Get the debugger to launch.  The returned string is via the strDbgCommand out param. Throws on error.
        bool fHasDebugger = GetCompleteDebuggerLaunchString(pAppDomain, wszAttachReason, useManagedDebugger, &strDbgCommand);
        if (fHasDebugger)
        {
            wszDbgCommand = strDbgCommand.GetUnicode();
            _ASSERTE(wszDbgCommand != NULL); // would have thrown on oom.

            LOG((LF_CORDB, LL_INFO10000, "D::EDA: launching with command [%S]\n", wszDbgCommand));

            ClrGetCurrentDirectory(strCurrentDir);
            wszCurrentDir = strCurrentDir.GetUnicode();
        }
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);

    STARTUPINFOW startupInfo = {0};
    startupInfo.cb = sizeof(STARTUPINFOW);
    PROCESS_INFORMATION processInfo = {0};

    DWORD errCreate = 0;

    if (wszDbgCommand != NULL)
    {
        // Create the debugger process
        // If we are launching an unmanaged debugger (i.e. if useManagedDebugger is FALSE), then we need
        // to let the child process inherit our handles.  This is necessary for the debugger to signal us
        // that the attach is complete.
        fCreateSucceeded = WszCreateProcess(NULL, const_cast<WCHAR*> (wszDbgCommand),
                               NULL, NULL,
                               (useManagedDebugger ? FALSE : TRUE),
                               CREATE_NEW_CONSOLE,
                               NULL, wszCurrentDir,
                               &startupInfo,
                               &processInfo);
        errCreate = GetLastError();
    }


    if (fCreateSucceeded)
    {
        LOG((LF_CORDB, LL_INFO10000, "D::EDA: debugger launched successfully.\n"));

        // We don't need a handle to the debugger process.
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    }
    else
    {
        GCX_PREEMP();

        int result;

        const UINT uCaption = IDS_DEBUG_NO_DEBUGGER_FOUND;
        const UINT uMsgBoxType = MB_RETRYCANCEL | MB_ICONEXCLAMATION | COMPLUS_MB_SERVICE_NOTIFICATION;


        if (wszDbgCommand == NULL)
        {
            // Notify that there was not even a debugger specified.
            result = MessageBox(IDS_DEBUG_NO_JIT_DEBUGGER_SPECIFIED,
                                uCaption, uMsgBoxType, TRUE);
        }
        else
        {
            // Notify that a debugger was indeed specified, but we failed to launch it.
            result = MessageBox(IDS_DEBUG_JIT_DEBUGGER_UNAVAILABLE,
                                uCaption, uMsgBoxType, TRUE,
                                errCreate, errCreate, wszDbgCommand);
        }

        // If the user wants to attach a debugger manually (they press Retry), then pretend as if the launch
        // succeeded.
        if (result == IDRETRY)
        {
            hr = S_OK;
        }
        else
        {
            hr = E_ABORT;
        }
    }

    if (FAILED(hr))
    {
        LOG((LF_CORDB, LL_INFO10000, "D::EDA: debugger did not launch successfully.\n"));

        // Make sure that any other threads that entered leave
        VERIFY(SetEvent(GetAttachAbortEvent()));
    }

    return hr;
}


//-----------------------------------------------------------------------------
// Ensure that a debugger is attached. Will jit-attach if needed.
//
// Returns:
//   E_ABORT - if the attach was declined
//   S_FALSE - if the debugger was already attached before calling this.
//   S_OK    - Jit-attach successfully started
//-----------------------------------------------------------------------------
HRESULT Debugger::EnsureDebuggerAttached(AppDomain *pAppDomain,
                                         LPCWSTR wszAttachReason,
                                         BOOL useManagedDebugger)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;

        MODE_PREEMPTIVE;

        PRECONDITION(!ThisIsHelperThreadWorker());
    }
    CONTRACTL_END;



    LOG( (LF_CORDB,LL_INFO10000,"D::EDA\n") );

    HRESULT hr = S_OK;

    DebuggerLockHolder dbgLockHolder(this);

    if (!IsDebuggerAttached())
    {

        // Remember that an exception is causing the attach.
        m_attachingForException = TRUE;

        // Only one thread throwing an exception when there is no
        // debugger attached should launch the debugger...
        m_exLock++;

        if (m_exLock == 1)
        {
            CONTRACT_VIOLATION(GCViolation);
            hr = EDAHelperProxy(pAppDomain, wszAttachReason, useManagedDebugger);
        }

        if (SUCCEEDED(hr))
        {
            // Wait for the debugger to begin attaching to us.
            LOG((LF_CORDB, LL_INFO10000, "D::EDA: waiting on m_exAttachEvent "
                 "and m_exAttachAbortEvent\n"));

            DWORD  dwHandles;
            HANDLE arrHandles[2];

            if (useManagedDebugger)
            {
                dwHandles = 2;
                arrHandles[0] = GetAttachEvent();
                arrHandles[1] = GetAttachAbortEvent();
            }
            else
            {
                dwHandles = 1;
                arrHandles[0] = GetUnmanagedAttachEvent();
            }

            // Let the helper thread does the attach logic for us and wait for the
            // attach event.  When we come back here, we should be in SYNC_STATE_3
            //
            dbgLockHolder.Release();

            // Wait for one or the other to be set
            DWORD res = WaitForMultipleObjects(dwHandles, arrHandles, FALSE, INFINITE);

            //dbgLockHolder.Acquire();

            // Indicate to the caller that the attach was aborted
            if (res == WAIT_OBJECT_0 + 1)
            {
                _ASSERTE(useManagedDebugger);

                LOG((LF_CORDB, LL_INFO10000,
                     "D::EDA: m_exAttachAbortEvent set\n"));

                hr = E_ABORT;
            }

            // Otherwise, attach was successful (Note, only 2/3 done so far!!!)
            else
            {
                if (useManagedDebugger)
                {
                // SS3 is the normal attach case.
                // SS0 is the EDA at a very early time before the process is fully up.
                _ASSERTE((m_syncingForAttach == SYNC_STATE_3) || (m_syncingForAttach == SYNC_STATE_0));
                    _ASSERTE((res == WAIT_OBJECT_0) && "WaitForMultipleObjects failed!");
                }

                // We can't reset the event here because some threads may
                // be just about to wait on it. If we reset it before the
                // other threads hit the wait, they'll block.

                // We have an innate race here that can't easily fix. The best
                // we can do is have a super small window (by moving the reset as
                // far out this making it very unlikely that a thread will
                // hit the window.

                LOG((LF_CORDB, LL_INFO10000, "D::EDA: m_exAttachEvent set\n"));
            }
        }

        // If this is the last thread, then reset the attach logic.
        m_exLock--;

        if (m_exLock == 0 && hr == E_ABORT)
        {
            // Reset the attaching logic.
            m_attachingForException = FALSE;
            VERIFY(ResetEvent(GetAttachAbortEvent()));
        }

        // Convert the "early-attach" case into an "already-attached" case.
        // See the DB_IPCE_ATTACHING even for details.
        // If the jit-attach was aborted, then hr = E_ABORT, and don't do this.
        if (SUCCEEDED(hr) && (m_syncingForAttach == SYNC_STATE_0))
        {
            hr = S_FALSE;
        }
    }
    else
    {
        // Debugger already attached
        hr = S_FALSE;
    }

    // dbgLockHolder goes out of scope - implicit Release

    LOG( (LF_CORDB, LL_INFO10000, "D::EDA:Leaving\n") );
    return hr;
}

/******************************************************************************
 *
 ******************************************************************************/
HRESULT Debugger::FinishEnsureDebuggerAttached()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;

        PRECONDITION(ThisIsHelperThread());
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    LOG( (LF_CORDB,LL_INFO10000,"D::FEDA\n") );
    if (!IsDebuggerAttached())
    {
        STRESS_LOG0(LF_CORDB, LL_INFO10000, "D::SE: sending sync complete.\n");

        _ASSERTE(m_syncingForAttach != SYNC_STATE_0);

        // Send the Sync Complete event next...
        SendSyncCompleteIPCEvent();

        LOG((LF_CORDB, LL_INFO10000,"D::FEDA: calling PAL_InitializeDebug.\n"));
        // Tell the PAL that we're trying to debug
        PAL_InitializeDebug();

        MarkDebuggerAttachedInternal();

        m_attachingForException = FALSE;
    }
    LOG( (LF_CORDB,LL_INFO10000,"D::FEDA leaving\n") );

    _ASSERTE(SUCCEEDED(hr) && "FinishEnsureDebuggerAttached failed.");
    return (hr);
}

// Proxy code for AttachDebuggerForBreakpoint
// Structure used in the proxy function callback
struct SendExceptionOnHelperThreadParams
{
    Debugger        *m_pThis;
    HRESULT         m_retval;
    Thread          *m_pThread;
    OBJECTHANDLE    m_exceptionHandle;
    bool            m_continuable;
    FramePointer    m_framePointer;
    SIZE_T          m_nOffset;
    CorDebugExceptionCallbackType m_eventType;
    DWORD           m_dwFlags;


    SendExceptionOnHelperThreadParams() :
        m_pThis(NULL),
        m_retval(S_OK),
        m_pThread(NULL)
        {LEAF_CONTRACT; }
};

//**************************************************************************
// This function will be executed on helper thread
//
// Parameters:
// p [in] : contains all data that needs to send exception info.
//**************************************************************************
void SendExceptionOnHelperThreadProxy(SendExceptionOnHelperThreadParams * p)
{
    WRAPPER_CONTRACT;

    p->m_retval = p->m_pThis->SendExceptionOnHelperThread(
        p->m_pThread,
        p->m_exceptionHandle,
        p->m_continuable,
        p->m_framePointer,
        p->m_nOffset,
        p->m_eventType,
        p->m_dwFlags);
}

//**************************************************************************
// This function will send Exception and ExceptionCallback2 event and finish the attach logic
// on helper thread.
//**************************************************************************
HRESULT Debugger::SendExceptionOnHelperThread(
    Thread      *pThread,
    OBJECTHANDLE exceptionHandle,
    bool        continuable,
    FramePointer framePointer,
    SIZE_T      nOffset,
    CorDebugExceptionCallbackType eventType,
    DWORD       dwFlags)
{
    // execute on helper thread
    _ASSERTE(IsDbgHelperSpecialThread());

    HRESULT     hr = S_OK;

    // This function is only called on helper thread and it should be still holding the
    // debugger lock since attach is not yet fully finished.
    //
    _ASSERTE(ThreadHoldsLock());

    // Send a user breakpoint event to the Right Side
    hr = SendExceptionCallBackHelper(
        pThread, exceptionHandle, continuable, framePointer, nOffset, eventType, dwFlags);

    _ASSERTE(SUCCEEDED(hr) && "SendExceptionCallBackHelper failed");

    // Stop all Runtime threads
    TrapAllRuntimeThreads(NULL, pThread->GetDomain());

    // Since we're still syncing for attach, send sync complete now and
    // mark that the debugger has completed attaching.
    return FinishEnsureDebuggerAttached();
}



//**************************************************************************
// This function sends Exception and ExceptionCallback2 event.
// It can be called on managed thread or helper thread.
//
// pThread : managed thread which exception takes place
// exptionHandle :
// continuable :
// framePointer :
// nOffset :
// eventType :
// dwFlags :
//
//**************************************************************************
HRESULT Debugger::SendExceptionCallBackHelper(
    Thread      *pThread,
    OBJECTHANDLE exceptionHandle,
    bool        continuable,
    FramePointer framePointer,
    SIZE_T      nOffset,
    CorDebugExceptionCallbackType eventType,
    DWORD       dwFlags)

{

    HRESULT     hr = S_OK;
    // This function can be called on helper thread or managed thread.
    // However, we should be holding locks upon entry

    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);

    InitIPCEvent(ipce, DB_IPCE_EXCEPTION, pThread, pThread->GetDomain());

    ipce->Exception.exceptionHandle.Set(exceptionHandle);
    ipce->Exception.firstChance = (eventType == DEBUG_EXCEPTION_FIRST_CHANCE);
    ipce->Exception.continuable = continuable;
    hr = m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

    _ASSERTE(SUCCEEDED(hr) && "D::SE: Send ExceptionCallback event failed.");

    InitIPCEvent(ipce, DB_IPCE_EXCEPTION_CALLBACK2, pThread, pThread->GetDomain());

    ipce->ExceptionCallback2.framePointer = framePointer;
    ipce->ExceptionCallback2.eventType = eventType;
    ipce->ExceptionCallback2.nOffset = nOffset;
    ipce->ExceptionCallback2.dwFlags = dwFlags;
    ipce->ExceptionCallback2.exceptionHandle.Set(exceptionHandle);

    LOG((LF_CORDB, LL_INFO10000, "D::SE: sending ExceptionCallbakc2 event from "
        "Thread:0x%x AD 0x%x.\n", ipce->threadId, ipce->appDomainToken.UnWrap()));
    hr = m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

    if (eventType == DEBUG_EXCEPTION_FIRST_CHANCE)
    {
        pThread->GetExceptionState()->GetFlags()->SetSentDebugFirstChance();
    }
    else
    {
        _ASSERTE(eventType == DEBUG_EXCEPTION_UNHANDLED);

        // When we have an unhandled exception, we may not have started processing the exception
        // (on WIN64, it means that we have not called ProcessCLRException()).  Thus, we may not
        // have created the necessary state to represent the "current" exception for the debugger.
        // We need to check for this case here.
        if (pThread->IsExceptionInProgress())
        {
            //
            // If we send the exception_unhandled, we want to surpress the unwind begin ipc event.
            //
            pThread->GetExceptionState()->GetFlags()->SetSentDebugUnwindBegin();
        }
    }

    _ASSERTE(SUCCEEDED(hr) && "D::SE: Send ExceptionCallback2 event failed.");
    return hr;

}

//
// SendException is called by Runtime threads to send that they've hit an Managed exception to the Right Side.
// This may block this thread and suspend the debuggee, and let the debugger inspect us. 
//
// The thread's throwable should be set so that the debugger can inspect the current exception.
// It does not report native exceptions in native code (which is consistent because those don't have a
// managed exception object).
//
// This may kick off a jit-attach (in which case fAttaching==true), and so may be called even when no debugger
// is yet involved.
//
// Parameters:
//    pThread - the thread throwing the exception. 
//    firstChance - true if this is a first chance exception. False if this is an unhandled exception.
//    currentIP - absolute native address of the exception if it is from managed code. If this is 0, we try to find it 
//                based off the thread's current exception state.
//    currentSP - stack pointer of the exception. This will get converted into a FramePointer and then used by the debugger
//                to identify which stack frame threw the exception.
//    currentBSP - additional information for IA64 only to identify the stack frame.
//    continuable - not used.
//    fAttaching - true iff this exception may initiate a jit-attach. In the common case, if this is true, then 
//                 CorDebuggerAttached() is false. However, since a debugger can attach at any time, it's possible
//                 for another debugger to race against the jit-attach and win. Thus this may err on the side of being true.
//    fForceNonInterceptable - This is used to determine if the exception is continuable (ie "Interceptible", 
//                  we can handle a DB_IPCE_INTERCEPT_EXCEPTION event for it). If true, then the exception can not be continued.
//                  If false, we get continuation status from the exception properties of the current thread. 
//    useManagedDebugger - Determines whether to do a managed jit-launch or native jit-launch (differen protocols).
//
// Returns: 
//    S_OK on success (common case by far).
//    propogates other errors.
//
HRESULT Debugger::SendException(Thread *pThread,
                                bool firstChance,
                                SIZE_T currentIP,
                                SIZE_T currentSP
                                IA64_ARG(SIZE_T currentBSP),
                                bool continuable, // not used by RS.
                                bool fAttaching,
                                bool fForceNonInterceptable,
                                BOOL useManagedDebugger
                               )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;

        MODE_ANY;

        PRECONDITION(HasLazyData());
        PRECONDITION(CheckPointer(pThread));

        // FilterContext is set by SecondChanceHijackFuncWorker, shouldn't be set any other time.
        PRECONDITION(pThread->GetInteropDebuggingHijacked() || (pThread->GetFilterContext() == NULL));
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::SendException\n"));

    if (CORDBUnrecoverableError(this))
    {
        return (E_FAIL);
    }

    // Mark if we're at an unsafe place.
    bool atSafePlace = g_pDebugger->IsThreadAtSafePlace(pThread);

    if (!atSafePlace)
    {
        g_pDebugger->IncThreadsAtUnsafePlaces();
    }

    // Is preemptive GC disabled on entry here?
    bool disabled = g_pEEInterface->IsPreemptiveGCDisabled();

    // We can only access the exception object while preemptive GC is disabled, so disable it if we need to.
    if (!disabled)
    {
        g_pEEInterface->DisablePreemptiveGC();
    }

    // Grab the exception name from the current exception object to pass to the JIT attach.
    OBJECTHANDLE h = g_pEEInterface->GetThreadException(pThread);
    OBJECTREF *o = ((OBJECTREF*)h);
    bool fIsInterceptable;

    if (fForceNonInterceptable)
    {
        fIsInterceptable = false;
        m_forceNonInterceptable = true;
    }
    else
    {
        fIsInterceptable = IsInterceptableException(pThread);
        m_forceNonInterceptable = false;
    }

    WCHAR *wszExceptionName = NULL;
    BOOL fIsExceptionNameAllocated = FALSE;

    {
        // Special case the System.StackOverflowException name lookup as retrieving it at runtime
        // is too costly in terms of stack space when we're executing inside the stack guard region.
        // NOTE: even the heap alloc is too costly so put it in the else.
        if ( h == CLRException::GetPreallocatedStackOverflowExceptionHandle() )
        {
            wszExceptionName = L"<System.StackOverflowException>";
        }
        else
        {
            wszExceptionName = new (interopsafe, nothrow) WCHAR[MAX_CLASSNAME_LENGTH];
            fIsExceptionNameAllocated = wszExceptionName != NULL;

            if ((o != NULL) && (*o != NULL) && (wszExceptionName != NULL))
            {
                // The WCHAR version of _GetFullyQualifiedNameForClass also used to allocate since
                // it used QuickBytes to allocate a temporary UTF8 string buffer, then transformed
                // the resulting UTF8 into the LPWSTR buffer passed in. This new version actually
                // makes a better attempt at not allocating, since all conversion routines in
                // SString use stack-based buffer strings.
                SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;

                // We know that this is not the stack overflow case, so we can allocate the string on the stack.
                InlineSString<MAX_CLASSNAME_LENGTH> ssExceptionName;
                (*o)->GetMethodTable()->GetClass()->_GetFullyQualifiedNameForClass(ssExceptionName);

                // Copy to the allocated string buffer.
                wcscpy_s(wszExceptionName, MAX_CLASSNAME_LENGTH, ssExceptionName.GetUnicode());
            }
            else
            {
                wszExceptionName = L"<Unknown exception>";
            }
        }
    }

    // We have to send enabled, so enable now.
    g_pEEInterface->EnablePreemptiveGC();

    // If no debugger is attached, then launch one to attach to us.  Ignore hr: if EDA fails, app suspends in EDA &
    // waits for a debugger to attach to us.

    HRESULT hr = S_FALSE; // Return value of EDA if debugger already attached
    bool    bStillInAttachLogic = false;

    if (fAttaching)
    {
        hr = EnsureDebuggerAttached(pThread->GetDomain(), wszExceptionName, useManagedDebugger);
        if (hr == S_OK)
        {
            bStillInAttachLogic = true;
        }

        // If a debugger is now attached, then it must have attached underneath the jit-attach.
        // No longer in the attaching stage.
        if (hr == S_FALSE)
        {
            fAttaching = false;   
        }
    }

    if (fIsExceptionNameAllocated)
    {
        CONSISTENCY_CHECK(CheckPointer(wszExceptionName));
        DeleteInteropSafe(wszExceptionName);
        wszExceptionName = NULL;
    }

    // The unmanaged debugger should have been attached by now.

    if (!useManagedDebugger)
    {
        if (hr == S_FALSE)
        {
            hr = S_OK;
        }
    }
    else
    if (SUCCEEDED(hr))
    {
        // Prevent other Runtime threads from handling events.

        // NOTE: if EnsureDebuggerAttached returned S_FALSE, this means that a debugger was already attached and
        // LockForEventSending should behave as normal.  If there was no debugger attached, then we have a special case
        // where this event is a part of debugger attaching and we've previously sent a sync complete event which means
        // that LockForEventSending will retry until a continue6929 is called - however, with attaching logic the previous
        // continue didn't enable event handling and didn't continue the process - it's waiting for this event to be
        // sent, so we do so even if the process appears to be stopped.
        // DebuggerLockHolder dbgLockHolder(this, FALSE);
        // LockForEventSending(&dbgLockHolder, hr == S_OK);

        //
        // In the JITattach case, an exception may be sent before the debugger is fully attached.
        //
        ThreadExceptionState* pExState = pThread->GetExceptionState();

        if ((CORDebuggerAttached() || fAttaching) &&
            ((!firstChance) ||
             (firstChance && (!pExState->GetFlags()->SentDebugFirstChance() || !pExState->GetFlags()->SentDebugUserFirstChance()))))
        {

            //
            // Figure out parameters to the IPC events.
            //
            const BYTE *ip;
            LPVOID stackPointer;
            SIZE_T nOffset = (SIZE_T)ICorDebugInfo::NO_MAPPING;

            DebuggerMethodInfo *pDebugMethodInfo = NULL;

            // If we're passed a zero IP or SP, then go to the ThreadExceptionState on the thread to get the data. Note:
            // we can only do this if there is a context in the pExState. There are cases (most notably the
            // EEPolicy::HandleFatalError case) where we don't have that. So we just leave the IP/SP 0.
            if ((currentIP == 0) && (pExState->GetContextRecord() != NULL))
            {
                ip = (BYTE *)GetIP(pExState->GetContextRecord());
            }
            else
            {
                ip = (BYTE *)currentIP;
            }

            if ((currentSP == 0) && (pExState->GetContextRecord() != NULL))
            {
                stackPointer = GetSP(pExState->GetContextRecord());
            }
            else
            {
                stackPointer = (LPVOID)currentSP;
            }


            if (g_pEEInterface->IsManagedNativeCode(ip))
            {

                MethodDesc *pMethodDesc = g_pEEInterface->GetNativeCodeMethodDesc(ip);
                _ASSERTE(pMethodDesc != NULL);

                if (pMethodDesc != NULL)
                {
                    DebuggerJitInfo *pDebugJitInfo = GetJitInfo(pMethodDesc, ip, &pDebugMethodInfo);

                    if (pDebugJitInfo != NULL)
                    {
                        SIZE_T nativeOffset = CodeRegionInfo::GetCodeRegionInfo(pDebugJitInfo, pMethodDesc).AddressToOffset(ip);
                        CorDebugMappingResult mapResult;
                        DWORD which;

                        nOffset = pDebugJitInfo->MapNativeOffsetToIL(nativeOffset,
                                                                     &mapResult,
                                                                     &which
                                                                    );
                    }
                }
            }


            DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);

            //
            // Send the first chance exception if we have not already.
            //
            if (firstChance)
            {

                if (!pExState->GetFlags()->SentDebugFirstChance())
                {
                    {
                        if (bStillInAttachLogic)
                        {
                            _ASSERTE(fAttaching);

                            // We are still in part of attach logic
                            // We need to let the helper thread to do this favor for us
                            // Send a user breakpoint event to the Right Side
                            SendExceptionOnHelperThreadParams p;
                            p.m_pThis = this;
                            p.m_pThread= pThread;
                            p.m_exceptionHandle = g_pEEInterface->GetThreadException(pThread);
                            p.m_continuable = continuable;
                            p.m_framePointer = FramePointer::MakeFramePointer(stackPointer  IA64_ARG(backingStorePointer));
                            p.m_nOffset = nOffset;
                            p.m_eventType = DEBUG_EXCEPTION_FIRST_CHANCE;
                            p.m_dwFlags = fIsInterceptable ? DEBUG_EXCEPTION_CAN_BE_INTERCEPTED : 0;

                            m_pRCThread->DoFavor((FAVORCALLBACK) SendExceptionOnHelperThreadProxy, &p);

                            hr = p.m_retval;

                        }
                        else
                        {
                            // This is a normal event to send from LS to RS
                            DebuggerLockHolder dbgLockHolder(this, FALSE);
                            LockForEventSending(&dbgLockHolder);

                            hr = SendExceptionCallBackHelper(
                                    pThread,
                                    g_pEEInterface->GetThreadException(pThread),
                                    continuable,
                                    FramePointer::MakeFramePointer(stackPointer  IA64_ARG(backingStorePointer)),
                                    nOffset,
                                    DEBUG_EXCEPTION_FIRST_CHANCE,
                                    fIsInterceptable ? DEBUG_EXCEPTION_CAN_BE_INTERCEPTED : 0);
                            //
                            // We now need to block this thread so that the debugger can mark any code
                            // JMC or not.
                            //
                            if (SUCCEEDED(hr))
                            {
                                // Stop all Runtime threads
                                TrapAllRuntimeThreads(&dbgLockHolder, pThread->GetDomain());

                            }

                            // Let other Runtime threads handle their events.
                            UnlockFromEventSending(&dbgLockHolder);
                        }
                    }


                    //
                    // Toggle GC to block this thread.
                    //
                    g_pEEInterface->DisablePreemptiveGC();

                    //
                    // If we weren't at a safe place when we enabled PGC, then go ahead and unmark that fact now that we've successfully
                    // disabled.
                    //
                    if (!atSafePlace)
                    {
                        g_pDebugger->DecThreadsAtUnsafePlaces();
                    }

                    ProcessAnyPendingEvals(pThread);

                    //
                    // If we weren't at a safe place, increment the unsafe count before we enable preemptive mode.
                    //
                    if (!atSafePlace)
                    {
                        g_pDebugger->IncThreadsAtUnsafePlaces();
                    }

                    //
                    // Re-enable to setup for sending USER_FIRST_CHANCE event.
                    //
                    g_pEEInterface->EnablePreemptiveGC();

                } // end if (!SentDebugFirstChance)

                //
                // If this is a JMC function, then we send a USER's first chance as well.
                //
                if ((pDebugMethodInfo != NULL) &&
                    pDebugMethodInfo->IsJMCFunction() &&
                    !pExState->GetFlags()->SentDebugUserFirstChance())
                {
                    DebuggerLockHolder dbgLockHolder(this, FALSE);
                    LockForEventSending(&dbgLockHolder);

                    InitIPCEvent(ipce, DB_IPCE_EXCEPTION_CALLBACK2, pThread, pThread->GetDomain());

                    ipce->ExceptionCallback2.framePointer =
                        FramePointer::MakeFramePointer(stackPointer  IA64_ARG(backingStorePointer));
                    ipce->ExceptionCallback2.eventType = DEBUG_EXCEPTION_USER_FIRST_CHANCE;
                    ipce->ExceptionCallback2.nOffset = nOffset;
                    ipce->ExceptionCallback2.dwFlags = fIsInterceptable ? DEBUG_EXCEPTION_CAN_BE_INTERCEPTED : 0;
                    ipce->ExceptionCallback2.exceptionHandle.Set(g_pEEInterface->GetThreadException(pThread));

                    LOG((LF_CORDB, LL_INFO10000, "D::SE: sending ExceptionCallbakc2 event from "
                        "Thread:0x%x AD 0x%x.\n", ipce->threadId, ipce->appDomainToken.UnWrap()));
                    hr = m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

                    _ASSERTE(SUCCEEDED(hr) && "D::SE: Send ExceptionCallback2 (User) event failed.");

                    if (SUCCEEDED(hr))
                    {
                        // Stop all Runtime threads
                        TrapAllRuntimeThreads(&dbgLockHolder, pThread->GetDomain());

                        _ASSERTE(!fAttaching);
                    }

                    pExState->GetFlags()->SetSentDebugUserFirstChance();

                    // Let other Runtime threads handle their events.
                    UnlockFromEventSending(&dbgLockHolder);

                } // end if (!SentDebugUserFirstChance)

            } // end if (firstChance)
            else
            {
                // unhandled exception case

                if (bStillInAttachLogic)
                {
                     _ASSERTE(fAttaching);

                    // We are still in part of attach logic
                    // We need to let the helper thread to do this favor for us
                    // Send a user breakpoint event to the Right Side
                    SendExceptionOnHelperThreadParams p;
                    p.m_pThis = this;
                    p.m_pThread= pThread;
                    p.m_exceptionHandle = g_pEEInterface->GetThreadException(pThread);
                    p.m_continuable = continuable;
                    p.m_framePointer = LEAF_MOST_FRAME;
                    p.m_nOffset = (SIZE_T)ICorDebugInfo::NO_MAPPING;
                    p.m_eventType = DEBUG_EXCEPTION_UNHANDLED;
                    p.m_dwFlags = fIsInterceptable ? DEBUG_EXCEPTION_CAN_BE_INTERCEPTED : 0;

                    m_pRCThread->DoFavor((FAVORCALLBACK) SendExceptionOnHelperThreadProxy, &p);

                    hr = p.m_retval;

                }
                else
                {
                    _ASSERTE(fAttaching == false);

                    DebuggerLockHolder dbgLockHolder(this, FALSE);
                    LockForEventSending(&dbgLockHolder);

                    hr = SendExceptionCallBackHelper(
                            pThread,
                            g_pEEInterface->GetThreadException(pThread),
                            continuable,
                            LEAF_MOST_FRAME,
                            (SIZE_T)ICorDebugInfo::NO_MAPPING,
                            DEBUG_EXCEPTION_UNHANDLED,
                            fIsInterceptable ? DEBUG_EXCEPTION_CAN_BE_INTERCEPTED : 0);

                    //
                    // We now need to block this thread so that the debugger can intercept if it wants.
                    //
                    if (SUCCEEDED(hr))
                    {
                        // Stop all Runtime threads
                        TrapAllRuntimeThreads(&dbgLockHolder, pThread->GetDomain());
                    }

                    // Let other Runtime threads handle their events.
                    UnlockFromEventSending(&dbgLockHolder);
                }
            } // end if (!firstChance)
        } // end if (!Attached)
        else
        {
            LOG((LF_CORDB,LL_INFO1000, "D:SE: Skipping SendIPCEvent because not supposed to send anything, or RS detached.\n"));
        }
     } // end if (!SUCCEEDED(attachingDebugger))

    // Disable PGC
    g_pEEInterface->DisablePreemptiveGC();

    // If we weren't at a safe place when we enabled PGC, then go ahead and unmark that fact now that we've successfully
    // disabled.
    if (!atSafePlace)
    {
        g_pDebugger->DecThreadsAtUnsafePlaces();
    }

    ProcessAnyPendingEvals(pThread);

    if (!disabled)
    {
        g_pEEInterface->EnablePreemptiveGC();
    }

    return (hr);
}


/*
 * ProcessAnyPendingEvals
 *
 * This function checks for, and then processes, any pending func-evals.
 *
 * Parameters:
 *   pThread - The thread to process.
 *
 * Returns:
 *   None.
 *
 */
void Debugger::ProcessAnyPendingEvals(Thread *pThread)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

#ifndef DACCESS_COMPILE

    // If no debugger is attached, then no evals to process.
    // We may get here in oom situations during jit-attach, so we'll check now and be safe.
    if (!CORDebuggerAttached())
    {
        return;
    }

    //
    // Note: if there is a filter context installed, we may need remove it, do the eval, then put it back. I'm not 100%
    // sure which yet... it kinda depends on whether or not we really need the filter context updated due to a
    // collection during the func eval...
    //
    // If we need to do a func eval on this thread, then there will be a pending eval registered for this thread. We'll
    // loop so long as there are pending evals registered. We block in FuncEvalHijackWorker after sending up the
    // FuncEvalComplete event, so if the user asks for another func eval then there will be a new pending eval when we
    // loop and check again.
    //
    DebuggerPendingFuncEval *pfe;    

    while (GetPendingEvals() != NULL && (pfe = GetPendingEvals()->GetPendingEval(pThread)) != NULL)
    {
        DebuggerEval *pDE = pfe->pDE;

        _ASSERTE(pDE->m_evalDuringException);
        _ASSERTE(pDE->m_thread == GetThread());

        // Remove the pending eval from the hash. This ensures that if we take a first chance exception during the eval
        // that we can do another nested eval properly.
        GetPendingEvals()->RemovePendingEval(pThread);

        // Go ahead and do the pending func eval. pDE is invalid after this. 
        void *ret;
        ret = ::FuncEvalHijackWorker(pDE);


        // The return value should be NULL when FuncEvalHijackWorker is called as part of an exception.
        _ASSERTE(ret == NULL);                
    }

    // If we need to re-throw a ThreadAbortException, go ahead and do it now.
    if (GetThread()->m_StateNC & Thread::TSNC_DebuggerReAbort)    
    {    
        // Now clear the bit else we'll see it again when we process the Exception notification
        // from this upcoming UserAbort exception.
        pThread->ResetThreadStateNC(Thread::TSNC_DebuggerReAbort);
        pThread->UserAbort(Thread::TAR_Thread, EEPolicy::TA_Safe, INFINITE, Thread::UAC_Normal);
    }

#endif

}


/*
 * FirstChanceManagedException is called by Runtime threads when crawling the managed stack frame
 * for a handler for the exception.  It is called for each managed call on the stack.
 *
 * Parameters:
 *   pThread - The thread the exception is occurring on.
 *   currentIP - the IP in the current stack frame.
 *   currentSP - the SP in the current stack frame.
 *
 * Returns:
 *   Always FALSE.
 *
 */
bool Debugger::FirstChanceManagedException(Thread *pThread, SIZE_T currentIP, SIZE_T currentSP
                                           IA64_ARG(SIZE_T currentBSP))
{

    // @@@
    // Implement DebugInterface
    // Can only be called from EE/exception
    // must be on managed thread.

    CONTRACTL
    {
        THROWS;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;

        PRECONDITION(CORDebuggerAttached());
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::FCE: First chance exception, TID:0x%x, \n", GetThreadIdHelper(pThread)));

    _ASSERTE(GetThread() != NULL);

#ifdef _DEBUG
    static ConfigDWORD d_fce;
    if (d_fce.val(L"D::FCE", 0))
        _ASSERTE(!"Stop in Debugger::FirstChanceManagedException?");
#endif

    SendException(pThread, TRUE, currentIP, currentSP  IA64_ARG(currentBSP), FALSE, FALSE, FALSE, TRUE);

    return false;
}


/*
 * FirstChanceManagedExceptionCatcherFound is called by Runtime threads when crawling the
 * managed stack frame and a handler for the exception is found.
 *
 * Parameters:
 *   pThread - The thread the exception is occurring on.
 *   pTct - Contains the function information that has the catch clause.
 *   pEHClause - Contains the native offset information of the catch clause.
 *
 * Returns:
 *   None.
 *
 */
void Debugger::FirstChanceManagedExceptionCatcherFound(Thread *pThread,
                                                       MethodDesc *pMD, TADDR pMethodAddr,
                                                       BYTE *currentSP
                                                       IA64_ARG(BYTE *currentBSP),
                                                       EE_ILEXCEPTION_CLAUSE *pEHClause)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS_FROM_GETJITINFO;
        MODE_ANY;
    }
    CONTRACTL_END;

    // @@@
    // Implements DebugInterface
    // Call by EE/exception. Must be on managed thread
    _ASSERTE(GetThread() != NULL);

    // Quick check.
    if (!CORDebuggerAttached())
    {
        return;
    }

    // Compute the offset

    DWORD nOffset = (DWORD)(SIZE_T)ICorDebugInfo::NO_MAPPING;

    if (pMD != NULL)
    {

        DebuggerJitInfo *pDebugJitInfo = GetJitInfo(pMD, (const BYTE *) pMethodAddr);

        if (pDebugJitInfo != NULL)
        {
            CorDebugMappingResult mapResult;
            DWORD which;

            nOffset = pDebugJitInfo->MapNativeOffsetToIL(
                pEHClause->HandlerStartPC,
                &mapResult,
                &which
                );
        }
    }


    bool fIsInterceptable = IsInterceptableException(pThread);
    m_forceNonInterceptable = false;
    DWORD dwFlags = fIsInterceptable ? DEBUG_EXCEPTION_CAN_BE_INTERCEPTED : 0;

    FramePointer fp = FramePointer::MakeFramePointer(currentSP  IA64_ARG(currentBSP));
    SendCatchHandlerFound(pThread, fp, nOffset, dwFlags);
}

// Filter to trigger CHF callback
// Notify of a catch-handler found callback.
LONG Debugger::NotifyOfCHFFilter(EXCEPTION_POINTERS* pExceptionPointers, PVOID pData)
{
    CONTRACTL
    {
        if ((GetThread() == NULL) || g_pEEInterface->IsThreadExceptionNull(GetThread()))
        {
            NOTHROW;
            GC_NOTRIGGER;
        }
        else
        {
            THROWS;
            MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
        }
        MODE_ANY;
    }
    CONTRACTL_END;

    SCAN_IGNORE_TRIGGER; // Scan can't handle conditional contracts.

    // @@@
    // Implements DebugInterface
    // Can only be called from EE

    // If no debugger is attached, then don't bother sending the events.
    // This can't kick off a jit-attach.
    if (!CORDebuggerAttached())
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    //
    // If this exception has never bubbled thru to managed code, then there is no
    // useful information for the debugger and, in fact, it may be a completely
    // internally handled runtime exception, so we should do nothing.
    //
    if ((GetThread() == NULL) || g_pEEInterface->IsThreadExceptionNull(GetThread()))
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    // Caller must pass in the stack address. This should match up w/ a Frame.
    BYTE * pCatcherStackAddr = (BYTE*) pData;

    // If we don't have any catcher frame, then use ebp from the context.
    if (pData == NULL)
    {
        pCatcherStackAddr = (BYTE*) GetFP(pExceptionPointers->ContextRecord);
    }
    else
    {
#ifdef _DEBUG
        _ASSERTE(pData != NULL);
        {
            // We want the CHF stack addr to match w/ the Internal Frame Cordbg sees
            // in the stacktrace.
            // The Internal Frame comes from an EE Frame. This means that the CHF stack
            // addr must match that EE Frame exactly. Let's check that now.

            Frame * pFrame = reinterpret_cast<Frame*>(pData);
            // Calling a virtual method will enforce that we have a valid Frame. ;)
            // If we got passed in a random catch address, then when we cast to a Frame
            // the vtable pointer will be bogus and this call will AV.
            Frame::ETransitionType e;
            e = pFrame->GetTransitionType();
        }
#endif
    }

    if (!g_EnableSIS)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    // Stubs don't have an IL offset.
    const SIZE_T offset = (SIZE_T)ICorDebugInfo::NO_MAPPING;
    Thread *pThread = GetThread();
    DWORD dwFlags = IsInterceptableException(pThread) ? DEBUG_EXCEPTION_CAN_BE_INTERCEPTED : 0;
    m_forceNonInterceptable = false;

    FramePointer fp = FramePointer::MakeFramePointer(pCatcherStackAddr  IA64_ARG(LEAF_MOST_FRAME_BSP));

    //
    // If we have not sent a first-chance notification, do so now.
    //
    ThreadExceptionState* pExState = pThread->GetExceptionState();

    if (!pExState->GetFlags()->SentDebugFirstChance())
    {
        SendException(pThread,
                      TRUE, // first-chance
                      (SIZE_T)(GetIP(pExceptionPointers->ContextRecord)), // IP
                      (SIZE_T)pCatcherStackAddr // SP
                      IA64_ARG((SIZE_T)(LEAF_MOST_FRAME_BSP)),
                      FALSE, // continuable
                      FALSE, // attaching
                      TRUE,  // ForceNonInterceptable since we are transition stub, the first and last place
                             // that will see this exception.
                      TRUE   // UseManagedDebugger
                     );
    }

    SendCatchHandlerFound(pThread, fp, offset, dwFlags);

#ifdef DEBUGGING_SUPPORTED


    if ( (pThread != NULL) &&
         (pThread->IsExceptionInProgress()) &&
         (pThread->GetExceptionState()->GetFlags()->DebuggerInterceptInfo()) )
    {
        //
        // The debugger wants to intercept this exception.  It may return in a failure case,
        // in which case we want to continue thru this path.
        //
        ClrDebuggerDoUnwindAndIntercept(EXCEPTION_CHAIN_END, pExceptionPointers->ExceptionRecord);
    }
#endif // DEBUGGING_SUPPORTED

    return EXCEPTION_CONTINUE_SEARCH;
}


// Actually send the catch handler found event.
// This can be used to send CHF for both regular managed catchers as well
// as stubs that catch (Func-eval, COM-Interop, AppDomains)
void Debugger::SendCatchHandlerFound(
    Thread * pThread,
    FramePointer fp,
    SIZE_T   nOffset,
    DWORD    dwFlags
)
{

    CONTRACTL
    {
        THROWS;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
        MODE_ANY;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::FirstChanceManagedExceptionCatcherFound\n"));

    if ((pThread == NULL))
    {
        _ASSERTE(!"Bad parameter");
        LOG((LF_CORDB, LL_INFO10000, "D::FirstChanceManagedExceptionCatcherFound - Bad parameter.\n"));
        return;
    }

    if (CORDBUnrecoverableError(this))
    {
        return;
    }

    //
    // Mark if we're at an unsafe place.
    //
    bool atSafePlace = g_pDebugger->IsThreadAtSafePlace(pThread);

    if (!atSafePlace)
    {
        g_pDebugger->IncThreadsAtUnsafePlaces();
    }

    //
    // Is preemptive GC disabled on entry here?
    //
    bool disabled = g_pEEInterface->IsPreemptiveGCDisabled();

    // We can only access the exception object while preemptive GC is disabled, so disable it if we need to.
    if (!disabled)
    {
        g_pEEInterface->DisablePreemptiveGC();
    }

    //
    // We have to send enabled, so enable now.
    //
    g_pEEInterface->EnablePreemptiveGC();

    //
    // Prevent other Runtime threads from handling events.
    //
    DebuggerLockHolder dbgLockHolder(this, FALSE);
    LockForEventSending(&dbgLockHolder);

    if (CORDebuggerAttached())
    {
        HRESULT hr;

        //
        // Figure out parameters to the IPC events.
        //
        DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);

        InitIPCEvent(ipce, DB_IPCE_EXCEPTION_CALLBACK2, pThread, pThread->GetDomain());

        ipce->ExceptionCallback2.framePointer = fp;
        ipce->ExceptionCallback2.eventType = DEBUG_EXCEPTION_CATCH_HANDLER_FOUND;
        ipce->ExceptionCallback2.nOffset = nOffset;
        ipce->ExceptionCallback2.dwFlags = dwFlags;
        ipce->ExceptionCallback2.exceptionHandle.Set(g_pEEInterface->GetThreadException(pThread));

        LOG((LF_CORDB, LL_INFO10000, "D::FCMECF: sending ExceptionCallback2 event from "
            "Thread:0x%x AD 0x%x.\n", ipce->threadId, ipce->appDomainToken.UnWrap()));
        hr = m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

        _ASSERTE(SUCCEEDED(hr) && "D::FCMECF: Send ExceptionCallback2 event failed.");

        //
        // If we send the catch_handler_found, we want to surpress the unwind begin ipc event.
        //
        pThread->GetExceptionState()->GetFlags()->SetSentDebugUnwindBegin();

        //
        // Stop all Runtime threads
        //
        TrapAllRuntimeThreads(&dbgLockHolder, pThread->GetDomain());

    } // end if (!Attached)
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D:FCMECF: Skipping SendIPCEvent because RS detached.\n"));
    }

    //
    // Let other Runtime threads handle their events.
    //
    UnlockFromEventSending(&dbgLockHolder);

    //
    // Disable PGC
    //
    g_pEEInterface->DisablePreemptiveGC();

    //
    // If we weren't at a safe place when we enabled PGC, then go ahead and unmark that fact now that we've successfully
    // disabled.
    //
    if (!atSafePlace)
    {
        g_pDebugger->DecThreadsAtUnsafePlaces();
    }

    ProcessAnyPendingEvals(pThread);

    //
    // If we disabled pre-emptive GC, re-enable it.
    //
    if (!disabled)
    {
        g_pEEInterface->EnablePreemptiveGC();
    }

    return;
}

/*
 * ManagedExceptionUnwindBegin is called by Runtime threads when crawling the
 * managed stack frame and unwinding them.
 *
 * Parameters:
 *   pThread - The thread the unwind is occurring on.
 *
 * Returns:
 *   None.
 *
 */
void Debugger::ManagedExceptionUnwindBegin(Thread *pThread)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    // @@@
    // Implements DebugInterface
    // Can only be called on managed threads
    //

    LOG((LF_CORDB, LL_INFO10000, "D::ManagedExceptionUnwindBegin\n"));

    if (pThread == NULL)
    {
        _ASSERTE(!"Bad parameter");
        LOG((LF_CORDB, LL_INFO10000, "D::ManagedExceptionUnwindBegin - Bad parameter.\n"));
        return;
    }

    if (CORDBUnrecoverableError(this))
    {
        return;
    }

    //
    // Mark if we're at an unsafe place.
    //
    bool atSafePlace = g_pDebugger->IsThreadAtSafePlace(pThread);

    if (!atSafePlace)
    {
        g_pDebugger->IncThreadsAtUnsafePlaces();
    }

    //
    // Is preemptive GC disabled on entry here?
    //
    bool disabled = g_pEEInterface->IsPreemptiveGCDisabled();

    //
    // We have to send enabled, so enable now.
    //
    if (disabled)
    {
        g_pEEInterface->EnablePreemptiveGC();
    }

    //
    // Prevent other Runtime threads from handling events.
    //
    DebuggerLockHolder dbgLockHolder(this, FALSE);
    LockForEventSending(&dbgLockHolder);

    if (CORDebuggerAttached() && !(pThread->GetExceptionState()->GetFlags()->SentDebugUnwindBegin()))
    {
        HRESULT hr;

        DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);

        InitIPCEvent(ipce, DB_IPCE_EXCEPTION_UNWIND, pThread, pThread->GetDomain());

        ipce->ExceptionUnwind.eventType = DEBUG_EXCEPTION_UNWIND_BEGIN;
        ipce->ExceptionUnwind.dwFlags = 0;

        LOG((LF_CORDB, LL_INFO10000, "D::MEUB: sending ExceptionUnwind event from "
            "Thread:0x%x AD 0x%x.\n", ipce->threadId, ipce->appDomainToken.UnWrap()));
        hr = m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

        _ASSERTE(SUCCEEDED(hr) && "D::MEUB: Send ExceptionUnwind event failed.");

        pThread->GetExceptionState()->GetFlags()->SetSentDebugUnwindBegin();

        //
        // Stop all Runtime threads
        //
        TrapAllRuntimeThreads(&dbgLockHolder, pThread->GetDomain());

    } // end if (!Attached)

    //
    // Let other Runtime threads handle their events.
    //
    UnlockFromEventSending(&dbgLockHolder);

    //
    // Disable PGC
    //
    g_pEEInterface->DisablePreemptiveGC();

    //
    // If we weren't at a safe place when we enabled PGC, then go ahead and unmark that fact now that we've successfully
    // disabled.
    //
    if (!atSafePlace)
    {
        g_pDebugger->DecThreadsAtUnsafePlaces();
    }

    //
    // If we disabled pre-emptive GC, re-enable it.
    //
    if (!disabled)
    {
        g_pEEInterface->EnablePreemptiveGC();
    }

    return;
}

/*
 * DeleteInterceptContext
 *
 * This function is called be the VM to release any debugger specific information for an
 * exception object.  It is called when the VM releases its internal exception stuff.
 *
 *
 * Parameters:
 *   pData - Debugger specific context.
 *
 * Returns:
 *   None.
 *
 */
void Debugger::DeleteInterceptContext(void *pContext)
{
}


// Get the frame point for an exception handler
FramePointer GetHandlerFramePointer(
    BYTE *pStack
    IA64_ARG(BYTE *pBStore))
{
    FramePointer handlerFP;

    // Refer to the comment in DispatchUnwind() to see why we have to add
    // sizeof(LPVOID) to the handler ebp.
    handlerFP = FramePointer::MakeFramePointer(LPVOID(pStack + sizeof(void*)));

    return handlerFP;
}

//
// ExceptionFilter is called by the Runtime threads when an exception
// is being processed.
// - fd - MethodDesc of filter function
// - pMethodAddr - any address inside of the method. This lets us resolve exactly which version
//                 of the method is being executed (for EnC)
// - offset - native offset to handler.
// - pStack, pBStore - stack pointers.
//
void Debugger::ExceptionFilter(MethodDesc *fd, TADDR pMethodAddr, SIZE_T offset, BYTE *pStack
                               IA64_ARG(BYTE *pBStore))
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        NOTHROW;
        GC_NOTRIGGER;

        PRECONDITION(!IsDbgHelperSpecialThread());
    }
    CONTRACTL_END;

    LOG((LF_CORDB,LL_INFO10000, "D::EF: pStack:0x%x MD: %s::%s, offset:0x%x\n",
        pStack, fd->m_pszDebugClassName, fd->m_pszDebugMethodName, offset));

    //
    // !!! Need to think through logic for when to step through filter code -
    // perhaps only during a "step in".
    //

    //
    // !!! Eventually there may be some weird mechanics introduced for
    // returning from the filter that we have to understand.  For now we should
    // be able to proceed normally.
    //

    FramePointer handlerFP;
    handlerFP = GetHandlerFramePointer(pStack IA64_ARG(pBStore));

    DebuggerJitInfo * pDJI = NULL;
    EX_TRY
    {
        pDJI = GetJitInfo(fd, (const BYTE *) pMethodAddr);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);

    if (!fd->IsDynamicMethod() && (pDJI == NULL))
    {
        // The only way we shouldn't have a DJI is from a dynamic method or from oom (which the LS doesn't handle).
        _ASSERTE(!"Debugger doesn't support OOM scenarios.");
        return;
    }

    DebuggerController::DispatchUnwind(g_pEEInterface->GetThread(),
                                       fd, pDJI, offset, handlerFP, STEP_EXCEPTION_FILTER);
}


//
// ExceptionHandle is called by Runtime threads when an exception is
// being handled.
// - fd - MethodDesc of filter function
// - pMethodAddr - any address inside of the method. This lets us resolve exactly which version
//                 of the method is being executed (for EnC)
// - offset - native offset to handler.
// - pStack, pBStore - stack pointers.
//
void Debugger::ExceptionHandle(MethodDesc *fd, TADDR pMethodAddr, SIZE_T offset, BYTE *pStack
                               IA64_ARG(BYTE *pBStore))
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        NOTHROW;
        GC_NOTRIGGER;

        PRECONDITION(!IsDbgHelperSpecialThread());
    }
    CONTRACTL_END;


    FramePointer handlerFP;
    handlerFP = GetHandlerFramePointer(pStack IA64_ARG(pBStore));

    DebuggerJitInfo * pDJI = NULL;
    EX_TRY
    {
        pDJI = GetJitInfo(fd, (const BYTE *) pMethodAddr);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);

    if (!fd->IsDynamicMethod() && (pDJI == NULL))
    {
        // The only way we shouldn't have a DJI is from a dynamic method or from oom (which the LS doesn't handle).
        _ASSERTE(!"Debugger doesn't support OOM scenarios.");
        return;
    }


    DebuggerController::DispatchUnwind(g_pEEInterface->GetThread(),
                                       fd, pDJI, offset, handlerFP, STEP_EXCEPTION_HANDLER);
}

BOOL Debugger::ShouldAutoAttach()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(!IsDebuggerAttached());

    // We're relying on the caller to determine the

    LOG((LF_CORDB, LL_INFO1000000, "D::SAD\n"));

    // Check if the user has specified a seting in the registry about what he
    // wants done when an unhandled exception occurs.
    DebuggerLaunchSetting dls = GetDbgJITDebugLaunchSetting();

    return (0 != (dls & DLS_ATTACH_DEBUGGER));


}

BOOL Debugger::FallbackJITAttachPrompt()
{
    _ASSERTE(!IsDebuggerAttached());
    return (ATTACH_YES == this->ShouldAttachDebuggerProxy(false,
                                                          DefaultDebuggerAttach));
}

BOOL Debugger::IsDebuggerAttached()
{
    // Doesn't make sense to lock here. It's a contract violation (since the lock toggles the GC)
    // and it won't actually buy us anything (since the value could change immediately after we release
    // the lock).
    return m_debuggerAttached;
}

void Debugger::MarkDebuggerAttachedInternal()
{
    LEAF_CONTRACT;

    // Attach is complete now.
    LOG((LF_CORDB, LL_INFO10000, "D::FEDA: Attach Complete!"));
    g_pEEInterface->MarkDebuggerAttached();
    m_syncingForAttach = SYNC_STATE_0;
    LOG((LF_CORDB, LL_INFO10000, "Attach state is now %s\n", g_ppszAttachStateToSZ[m_syncingForAttach]));

    _ASSERTE(HasLazyData());

    m_debuggerAttached = TRUE;
}
void Debugger::MarkDebuggerUnattachedInternal()
{
    LEAF_CONTRACT;

    _ASSERTE(HasLazyData());

    g_pEEInterface->MarkDebuggerUnattached();
    m_debuggerAttached = FALSE;
}

//-----------------------------------------------------------------------------
// Favor to do lazy initialization on helper thread.
// This is needed to allow lazy intialization in Stack Overflow scenarios.
// We may or may not already be initialized.
//-----------------------------------------------------------------------------
void LazyInitFavor(void *)
{
    CONTRACTL
    {
        NOTHROW;
        MODE_ANY;
    }
    CONTRACTL_END;
    Debugger::DebuggerLockHolder dbgLockHolder(g_pDebugger);
    HRESULT hr = g_pDebugger->LazyInitWrapper();

    // On checked builds, warn that we're hitting a scenario that debugging doesn't support.    
    _ASSERTE(SUCCEEDED(hr) || !"Couldn't initialize lazy data for LastChanceManagedException");
}

/******************************************************************************
 *
 ******************************************************************************/
LONG Debugger::LastChanceManagedException(EXCEPTION_RECORD *pExceptionRecord,
                                          CONTEXT *pContext,
                                          Thread *pThread,
                                          BOOL jitAttachRequested,
                                          BOOL useManagedDebugger)
{
    CONTRACTL
    {
        NOTHROW;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
        MODE_ANY;
    }
    CONTRACTL_END;

    // @@@
    // Implements DebugInterface.
    // Can be run only on managed thread.

    // Don't stop for native debugging anywhere inside our inproc-Filters.
    CantStopHolder hHolder;

    LOG((LF_CORDB, LL_INFO10000, "D::LastChanceManagedException\n"));

    // You're allowed to call this function with a NULL exception record and context. If you do, then its assumed
    // that we want to head right down to asking the user if they want to attach a debugger. No need to try to
    // dispatch the exception to the debugger controllers. You have to pass NULL for both the exception record and
    // the context, though. They're a pair. Both have to be NULL, or both have to be valid.
    _ASSERTE(((pExceptionRecord != NULL) && (pContext != NULL)) ||
             ((pExceptionRecord == NULL) && (pContext == NULL)));

    if (CORDBUnrecoverableError(this))
    {
        return ExceptionContinueSearch;
    }

    // We don't do anything on the second pass
    if ((pExceptionRecord != NULL) && ((pExceptionRecord->ExceptionFlags & EXCEPTION_UNWINDING) != 0))
    {
        return ExceptionContinueSearch;
    }

    // Let the controllers have a chance at it - this may be the only handler which can catch the exception if this
    // is a native patch.

    if ((pThread != NULL) &&
        (pContext != NULL) &&
        IsDebuggerAttached() &&
        DebuggerController::DispatchNativeException(pExceptionRecord,
                                                    pContext,
                                                    pExceptionRecord->ExceptionCode,
                                                    pThread))
    {
        return ExceptionContinueExecution;
    }

    // Otherwise, run our last chance exception logic
    ATTACH_ACTION action;
    action = ATTACH_NO;

    if (IsDebuggerAttached() || jitAttachRequested)
    {
        LOG((LF_CORDB, LL_INFO10000, "D::BEH ... debugger attached.\n"));

        Thread *thread = g_pEEInterface->GetThread();

        // ExceptionFlags is 0 for continuable, EXCEPTION_NONCONTINUABLE otherwise. Note that if we don't have an
        // exception record, then we assume this is a non-continuable exception.
        bool continuable = (pExceptionRecord != NULL) && (pExceptionRecord->ExceptionFlags == 0);

        LOG((LF_CORDB, LL_INFO10000, "D::BEH ... sending exception.\n"));

        HRESULT hr = E_FAIL;

        // In the jit-attach case, lazy-init. We may be in a stack-overflow, so do it via a favor to avoid
        // using this thread's stack space.
        if (jitAttachRequested)
        {
            m_pRCThread->DoFavor((FAVORCALLBACK) LazyInitFavor, NULL);                
        }
        
        // The only way we don't have lazy data at this point is in an OOM scenario, which 
        // the debugger doesn't support.
        if (!HasLazyData())
        {
            return ExceptionContinueExecution;            
        }


        EX_TRY
        {
            // We pass the attaching status to SendException so that it knows
            // whether to attach a debugger or not. We should really do the
            // attach stuff out here and not bother with the flag.
            hr = SendException(thread,
                          FALSE,
                          ((pContext != NULL) ? (SIZE_T)GetIP(pContext) : NULL),
                          ((pContext != NULL) ? (SIZE_T)GetSP(pContext) : NULL)
                          IA64_ARG(((pContext != NULL) ? (SIZE_T)GetRsBSP(pContext) : NULL)),
                          continuable,
                          !!jitAttachRequested,
                          !!jitAttachRequested,
                          useManagedDebugger);  // If we are JIT attaching on an unhandled exceptioin, we force
                                                  // the exception to be uninterceptable.
        }
        EX_CATCH
        {
        }
        EX_END_CATCH(SwallowAllExceptions);

        // The first case is for attaching a managed debugger.
        // The second case is for attaching an unmanaged debugger.
        if ( ( useManagedDebugger && (continuable && g_pEEInterface->IsThreadExceptionNull(thread))) ||
             (!useManagedDebugger && (hr == S_OK)) )
        {
            return ExceptionContinueExecution;
        }
    }
    else
    {
        // Note: we don't do anything on NO or TERMINATE. We just return to the exception logic, which will abort the
        // app or not depending on what the CLR impl decides is appropiate.
        _ASSERTE(action == ATTACH_TERMINATE || action == ATTACH_NO);
    }

    return ExceptionContinueSearch;
}



// This function checks the registry for the debug launch setting upon encountering an exception or breakpoint.
DebuggerLaunchSetting Debugger::GetDbgJITDebugLaunchSetting(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Query for the value "DbgJITDebugLaunchSetting"
    DWORD dwSetting = 0;

#ifdef PLATFORM_UNIX
    // don't launch the cordbg by default on Unix
    dwSetting = 1;
#else
    // show a dialog by default on Windows
    dwSetting = 0;
#endif

    dwSetting = UnsafeGetConfigDWORD(CorDB_REG_QUESTION_KEY, dwSetting);

    DebuggerLaunchSetting ret = (DebuggerLaunchSetting)dwSetting;

    return ret;
}


//
// NotifyUserOfFault notifies the user of a fault (unhandled exception
// or user breakpoint) in the process, giving them the option to
// attach a debugger or terminate the application.
//
int Debugger::NotifyUserOfFault(bool userBreakpoint, DebuggerLaunchSetting dls)
{
    LOG((LF_CORDB, LL_INFO1000000, "D::NotifyUserOfFault\n"));

    CONTRACTL
    {
        NOTHROW;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END;

    int result = IDCANCEL;

    if (!IsDebuggerAttached())
    {
        DWORD pid;
        DWORD tid;

        pid = GetCurrentProcessId();
        tid = GetCurrentThreadId();

        DWORD flags = 0;
        UINT resIDMessage = 0;



        if (userBreakpoint)
        {
            resIDMessage = IDS_DEBUG_USER_BREAKPOINT_MSG;
            flags |= MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION;
        }
        else
        {
            resIDMessage = IDS_DEBUG_UNHANDLED_EXCEPTION_MSG;
            flags |= MB_OKCANCEL | MB_ICONEXCLAMATION;
        }

        {
            // Another potential hang. This may get run on the helper if we have a stack overflow.
            // Hopefully the odds of 1 thread hitting a stack overflow while another is stuck holding the heap
            // lock is very small.
            SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;

            result = MessageBox(resIDMessage, IDS_DEBUG_SERVICE_CAPTION, flags, TRUE, pid, pid, tid, tid);
        }
    }

    LOG((LF_CORDB, LL_INFO1000000, "D::NotifyUserOfFault left\n"));
    return result;
}


// Proxy for ShouldAttachDebugger
struct ShouldAttachDebuggerParams {
    Debugger*                   m_pThis;
    bool                        m_fIsUserBreakpoint;
    UnhandledExceptionLocation  m_location;
    Debugger::ATTACH_ACTION     m_retval;
};

// This is called by the helper thread
void ShouldAttachDebuggerStub(ShouldAttachDebuggerParams * p)
{
    WRAPPER_CONTRACT;

    p->m_retval = p->m_pThis->ShouldAttachDebugger(p->m_fIsUserBreakpoint, p->m_location);
}

// This gets called just like the normal version, but it sends the call over to the helper thread
Debugger::ATTACH_ACTION Debugger::ShouldAttachDebuggerProxy(bool fIsUserBreakpoint, UnhandledExceptionLocation location)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (!HasLazyData())
    {
        DebuggerLockHolder lockHolder(this);
        HRESULT hr = LazyInitWrapper();
        if (FAILED(hr))
        {
            // We already stress logged this case.
            return ATTACH_NO;
        }
    }


    if (!IsGuardPageGone())
        return ShouldAttachDebugger(fIsUserBreakpoint, location);

    ShouldAttachDebuggerParams p;
    p.m_pThis = this;
    p.m_fIsUserBreakpoint = fIsUserBreakpoint;
    p.m_location = location;

    LOG((LF_CORDB, LL_INFO1000000, "D::SADProxy\n"));
    m_pRCThread->DoFavor((FAVORCALLBACK) ShouldAttachDebuggerStub, &p);
    LOG((LF_CORDB, LL_INFO1000000, "D::SADProxy return %d\n", p.m_retval));

    return p.m_retval;
}

// Returns true if the debugger is not attached and DbgJITDebugLaunchSetting is set to either ATTACH_DEBUGGER or
// ASK_USER and the user request attaching.
Debugger::ATTACH_ACTION Debugger::ShouldAttachDebugger(bool fIsUserBreakpoint, UnhandledExceptionLocation location)
{
    CONTRACTL
    {
        NOTHROW;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
        MODE_ANY;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000000, "D::SAD\n"));

    // If the debugger is already attached, not necessary to re-attach
    if (IsDebuggerAttached())
    {
        return ATTACH_NO;
    }

    // Check if the user has specified a seting in the registry about what he wants done when an unhandled exception
    // occurs.
    DebuggerLaunchSetting dls = GetDbgJITDebugLaunchSetting();

    // First, we just don't attach if the location of the exception doesn't fit what the user is looking for. Note: a
    // location of 0 indicates none specified, in which case we only let locations specified in DefaultDebuggerAttach
    // through. This is for backward compatability and convience.
    UnhandledExceptionLocation userLoc = (UnhandledExceptionLocation)(dls >> DLS_LOCATION_SHIFT);

    if ((userLoc == 0) && !(location & DefaultDebuggerAttach))
    {
        return ATTACH_NO;
    }
    else if ((userLoc != 0) && !(userLoc & location))
    {
        return ATTACH_NO;
    }

    // Now that we've passed the location test, how does the user want to attach?
    if (dls & DLS_ATTACH_DEBUGGER)
    {
        // Attach without asking the user...
        return ATTACH_YES;
    }
    else if (dls & DLS_TERMINATE_APP)
    {
        // We just want to ignore user breakpoints if the registry says to "terminate" the app.
        if (fIsUserBreakpoint)
            return ATTACH_NO;
        else
            return ATTACH_TERMINATE;
    }
    else
    {
        // Only ask the user once if they wish to attach a debugger.  This is because LastChanceManagedException can be called
        // twice, which causes ShouldAttachDebugger to be called twice, which causes the user to have to answer twice.
        static BOOL s_fHasAlreadyAsked = FALSE;
        static ATTACH_ACTION s_action;


        GCX_PREEMP();       
        DebuggerLockHolder lockHolder(this);

        // We always want to ask about user breakpoints!
        if (!s_fHasAlreadyAsked || fIsUserBreakpoint)
        {
            if (!fIsUserBreakpoint)
                s_fHasAlreadyAsked = TRUE;

            CONTRACT_VIOLATION(GCViolation);

            // Ask the user if they want to attach
            int iRes = NotifyUserOfFault(fIsUserBreakpoint, dls);

            // If it's a user-defined breakpoint, they must hit Retry to launch
            // the debugger.  If it's an unhandled exception, user must press
            // Cancel to attach the debugger.
            if ((iRes == IDCANCEL) || (iRes == IDRETRY))
                s_action = ATTACH_YES;

            else if ((iRes == IDABORT) || (iRes == IDOK))
                s_action = ATTACH_TERMINATE;

            else
                s_action = ATTACH_NO;
        }

        // dbgLockHolder goes out of scope - implicit Release
        return s_action;
    }
}

//******************************************************************************
// Proxy code for AttachDebuggerForBreakpoint
// Structure used in the proxy function callback
//******************************************************************************
struct AttachDebuggerForBreakpointOnHelperThreadParams
{
    Debugger        *m_pThis;
    Thread          *m_pThread;
    HRESULT         m_retval;

    AttachDebuggerForBreakpointOnHelperThreadParams() :
        m_pThis(NULL), m_pThread(NULL), m_retval(S_OK) {LEAF_CONTRACT; }
};


//******************************************************************************
//
// This function will be executed on helper thread to send raw user break point
// and finish attach logic. It calls AttachDebuggerForBreakpointOnHelperThread after unbundled
// parameters from a struct.
//
//******************************************************************************
void SendRawUserBreakPointOnHelperThreadProxy(AttachDebuggerForBreakpointOnHelperThreadParams * p)
{
    WRAPPER_CONTRACT;
    _ASSERTE(p->m_pThis != NULL && p->m_pThread != NULL);
    STRESS_LOG0(LF_CORDB, LL_INFO10, "SendRawUserBreakPointOnHelperThreadProxy\n");

    p->m_retval = p->m_pThis->AttachDebuggerForBreakpointOnHelperThread(p->m_pThread);
}

//******************************************************************************
// This function will send UserBreakPoint event and finish the attach logic
// on helper thread.
//******************************************************************************
HRESULT Debugger::AttachDebuggerForBreakpointOnHelperThread(Thread *pThread)
{
    // @@@
    // This function is only called on helper thread and it should be still holding the
    // debugger lock since attach is not yet fully finished.
    //
    _ASSERTE(ThreadHoldsLock());

    // Send a user breakpoint event to the Right Side
    SendRawUserBreakpoint(pThread);

    // Stop all Runtime threads
    TrapAllRuntimeThreads(NULL, pThread->GetDomain());

    // Since we're still syncing for attach, send sync complete now and
    // mark that the debugger has completed attaching.
    return FinishEnsureDebuggerAttached();
}


/******************************************************************************
 *
 ******************************************************************************/
HRESULT Debugger::AttachDebuggerForBreakpoint(Thread *thread,
                                              __in_z __in_opt WCHAR *wszLaunchReason)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (wszLaunchReason == NULL)
    {
        wszLaunchReason = L"Launch for user";
    }

    // Mark if we're at an unsafe place.
    bool atSafePlace = g_pDebugger->IsThreadAtSafePlace(thread);

    if (!atSafePlace)
        g_pDebugger->IncThreadsAtUnsafePlaces();

    // Enable preemptive GC...
    bool disabled = g_pEEInterface->IsPreemptiveGCDisabled();

    if (disabled)
    {
        g_pEEInterface->EnablePreemptiveGC();
    }

    {
        // make sure we init the lazy data whiel holder the debugger lock on this thread.
        DebuggerLockHolder dbgLockHolder(this);
        HRESULT hr = LazyInitWrapper();
        if (FAILED(hr))
        {
            // We already stress logged this case.
            return hr;
        }
    }

    // If no debugger is attached, then launch one to attach to us.
    HRESULT hr = EnsureDebuggerAttached(thread->GetDomain(), wszLaunchReason, TRUE);
    if (SUCCEEDED(hr))
    {
    bool    bStillInAttachLogic = false;
    if (hr == S_OK)
    {
        bStillInAttachLogic = true;
    }

    if (bStillInAttachLogic)
    {
        // Prevent other Runtime threads from handling events.

        // NOTE: if EnsureDebuggerAttached returned S_FALSE, this means that
        // a debugger was already attached and LockForEventSending should
        // behave as normal.  If there was no debugger attached, then we have
        // a special case where this event is a part of debugger attaching and
        // we've previously sent a sync complete event which means that
        // LockForEventSending will retry until a continue is called - however,
        // with attaching logic the previous continue didn't enable event
        // handling and didn't continue the process - it's waiting for this
        // event to be sent, so we do so even if the process appears to be
        // stopped.

        // In this case, let the helper thread to send the proper event and finish
        // the attach logic for managed thread!
        // @@@ Now the helper thread has the lock
        // @@@ Also we are stopped. We should assert that.


        // Send a user breakpoint event to the Right Side
        AttachDebuggerForBreakpointOnHelperThreadParams p;
        p.m_pThis = this;
        p.m_pThread= thread;

        m_pRCThread->DoFavor((FAVORCALLBACK) SendRawUserBreakPointOnHelperThreadProxy, &p);

        hr = p.m_retval;
    }
    else
    {
        // Someone launch the debugger during the meantime.
        // just send the event
        DebuggerLockHolder dbgLockHolder(this, FALSE);
        LockForEventSending(&dbgLockHolder, hr == S_OK);

        // Send a user breakpoint event to the Right Side
        SendRawUserBreakpoint(thread);

        // Stop all Runtime threads
        TrapAllRuntimeThreads(&dbgLockHolder, thread->GetDomain());

        // Let other Runtime threads handle their events.
        UnlockFromEventSending(&dbgLockHolder);
    }
    }

    // Set back to disabled GC
    g_pEEInterface->DisablePreemptiveGC();

    if (!disabled)
        g_pEEInterface->EnablePreemptiveGC();

    if (!atSafePlace)
        g_pDebugger->DecThreadsAtUnsafePlaces();

    return (hr);
}


//
// SendUserBreakpoint is called by Runtime threads to send that they've hit
// a user breakpoint to the Right Side.
//
void Debugger::SendUserBreakpoint(Thread *thread, __in_z __in_opt WCHAR* szMessage)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(thread != NULL);
        PRECONDITION(thread == ::GetThread());
    }
    CONTRACTL_END;


#ifdef _DEBUG
    // For testing Watson, we want a consistent way to be able to generate a
    // Fatal Execution Error
    // So we have a debug-only knob in this particular managed call that can be used
    // to artificially inject the error.
    // This is only for testing.
    static int fDbgInjectFEE = -1;

    if (fDbgInjectFEE == -1)
        fDbgInjectFEE = UnsafeGetConfigDWORD(L"DbgInjectFEE", 0);

    if (fDbgInjectFEE)
    {
        STRESS_LOG0(LF_CORDB, LL_INFO10000, "Debugger posting bogus FEE b/c knob DbgInjectFEE is set.\n");
        EEPOLICY_HANDLE_FATAL_ERROR(COR_E_EXECUTIONENGINE);
        // These never return.
    }
#endif

    if (CORDBUnrecoverableError(this))
        return;

    // Ask the user if they want to attach the debugger
    ATTACH_ACTION dbgAction;

    // If user wants to attach the debugger, do so
    if (IsDebuggerAttached() || ((dbgAction = ShouldAttachDebugger(true, ProcessWideHandler)) == ATTACH_YES))
    {
        _ASSERTE(GetManagedStoppedCtx(thread) == NULL);

        if (IsDebuggerAttached())
        {
            // A debugger is already attached, so setup a DebuggerUserBreakpoint controller to get us out of the helper
            // that got us here. The DebuggerUserBreakpoint will call AttachDebuggerForBreakpoint for us when we're out
            // of the helper. The controller will delete itself when its done its work.
            DebuggerUserBreakpoint::HandleDebugBreak(thread);
        }
        else
        {
            // No debugger attached, so go ahead and just try to send the user breakpoint
            // event. AttachDebuggerForBreakpoint will ensure that the debugger is attached before sending the event.
            HRESULT hr;
            hr = AttachDebuggerForBreakpoint(thread, szMessage);
            _ASSERTE(SUCCEEDED(hr) || hr == E_ABORT);
        }
    }
    else if (dbgAction == ATTACH_TERMINATE)
    {
        // ATTACH_TERMINATE indicates the the user wants to terminate the app.
        LOG((LF_CORDB, LL_INFO10000, "D::SUB: terminating this process due to user request\n"));

        TerminateProcess(GetCurrentProcess(), 0);
        _ASSERTE(!"Should never reach this point.");
    }
    else
    {
        _ASSERTE(dbgAction == ATTACH_NO);
    }
}


// void Debugger::ThreadCreated():  ThreadCreated is called when
// a new Runtime thread has been created, but before its ever seen
// managed code.  This is a callback invoked by the EE into the Debugger.
// This will create a DebuggerThreadStarter patch, which will set
// a patch at the first instruction in the managed code.  When we hit
// that patch, the DebuggerThreadStarter will invoke ThreadStarted, below.
//
// Thread* pRuntimeThread:  The EE Thread object representing the
//      runtime thread that has just been created.
void Debugger::ThreadCreated(Thread* pRuntimeThread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // @@@
    // This function implements the DebugInterface. But it is also called from Attach
    // logic internally.
    //

    if (CORDBUnrecoverableError(this))
        return;

    LOG((LF_CORDB, LL_INFO100, "D::TC: thread created for 0x%x. ******\n",
         GetThreadIdHelper(pRuntimeThread)));

    // Create a thread starter and enable its WillEnterManaged code
    // callback. This will cause the starter to trigger once the
    // thread has hit managed code, which will cause
    // Debugger::ThreadStarted() to be called.  NOTE: the starter will
    // be deleted automatically when its done its work.
    DebuggerThreadStarter *starter = new (interopsafe, nothrow) DebuggerThreadStarter(pRuntimeThread);

    if (starter == NULL)
    {
        CORDBDebuggerSetUnrecoverableWin32Error(this, 0, false);
        return;
    }

    starter->EnableTraceCall(LEAF_MOST_FRAME);
}


// void Debugger::ThreadStarted():  ThreadStarted is called when
// a new Runtime thread has reached its first managed code. This is
// called by the DebuggerThreadStarter patch's SendEvent method.
//
// Thread* pRuntimeThread:  The EE Thread object representing the
//      runtime thread that has just hit managed code.
void Debugger::ThreadStarted(Thread* pRuntimeThread,
                             BOOL fAttaching)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // @@@
    // This method implemented DebugInterface but it is also called from Controller

    if (CORDBUnrecoverableError(this))
        return;

    LOG((LF_CORDB, LL_INFO100, "D::TS: thread attach : ID=%#x AD:%#x isAttaching:%d.\n",
         GetThreadIdHelper(pRuntimeThread), pRuntimeThread->GetDomain(), fAttaching));

    //
    // If we're attaching, then we only need to send the event. We
    // don't need to disable event handling or lock the debugger
    // object.
    //
#ifdef _DEBUG
    if (!fAttaching)
    {
        _ASSERTE((g_pEEInterface->GetThread() &&
                 !g_pEEInterface->GetThread()->m_fPreemptiveGCDisabled) ||
                 g_fInControlC);
        _ASSERTE(ThreadHoldsLock());
    }
#endif

    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    InitIPCEvent(ipce,
                 DB_IPCE_THREAD_ATTACH,
                 pRuntimeThread,
                 pRuntimeThread->GetDomain());

    ipce->ThreadAttachData.debuggerThreadToken.Set(pRuntimeThread);
#if PLATFORM_UNIX
    ipce->ThreadAttachData.threadHandle = INVALID_HANDLE_VALUE;
#else
    if (CLRTaskHosted())
    {
        // Don't need to pass out thread hanle since it will change from time to time.
        ipce->ThreadAttachData.threadHandle = SWITCHOUT_HANDLE_VALUE;
    }
    else
    {
        ipce->ThreadAttachData.threadHandle = pRuntimeThread->GetThreadHandle();
    }
#endif
    ipce->ThreadAttachData.firstExceptionHandler = (void *)pRuntimeThread->GetExceptionListPtr();


    m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

    if (!fAttaching)
    {
        //
        // Well, if this thread got created _after_ we started sync'ing
        // then its Runtime thread flags don't have the fact that there
        // is a debug suspend pending. We need to call over to the
        // Runtime and set the flag in the thread now...
        //
        if (m_trappingRuntimeThreads)
            g_pEEInterface->MarkThreadForDebugSuspend(pRuntimeThread);
    }
}


// DetachThread is called by Runtime threads when they are completing
// their execution and about to be destroyed.
//
void Debugger::DetachThread(Thread *pRuntimeThread)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    // @@@
    // Implement DebugInterface API

    if (CORDBUnrecoverableError(this))
        return;

    if (m_ignoreThreadDetach)
        return;

    _ASSERTE (pRuntimeThread != NULL);


    if (!g_fEEShutDown && !IsDebuggerAttachedToAppDomain(pRuntimeThread))
        return;

    LOG((LF_CORDB, LL_INFO100, "D::DT: thread detach : ID=%#x AD:%#x.\n",
         GetThreadIdHelper(pRuntimeThread), pRuntimeThread->GetDomain()));


    // We may be killing a thread before the Thread-starter fired.
    // So check (and cancel) any outstanding thread-starters.
    // If we don't, this old thread starter may conflict w/ a new thread-starter
    // if AppDomains or EE Thread's get recycled.
    DebuggerController::CancelOutstandingThreadStarter(pRuntimeThread);

    // Controller lock is bigger than debugger lock.
    // Don't take debugger lock before the CancelOutStandingThreadStarter function.
    DebuggerLockHolder dbgLockHolder(this, FALSE);

    SENDIPCEVENT_BEGIN(&dbgLockHolder);

    if (CORDebuggerAttached())
    {
        // Send a detach thread event to the Right Side.
        DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce,
                     DB_IPCE_THREAD_DETACH,
                     pRuntimeThread,
                     pRuntimeThread->GetDomain());
        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

        // Stop all Runtime threads
        TrapAllRuntimeThreads(&dbgLockHolder, pRuntimeThread->GetDomain());

        // This prevents a race condition where we blocked on the Lock()
        // above while another thread was sending an event and while we
        // were blocked the debugger suspended us and so we wouldn't be
        // resumed after the suspension about to happen below.
        pRuntimeThread->ResetThreadStateNC(Thread::TSNC_DebuggerUserSuspend);
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::DT: Skipping SendIPCEvent because RS detached."));
    }

    SENDIPCEVENT_END;
}


//
// SuspendComplete is called when the last Runtime thread reaches a safe point in response to having its trap flags set.
// This may be called on either the real helper thread or someone doing helper thread duty.
//
BOOL Debugger::SuspendComplete()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;

        // This will is conceptually mode-cooperative.
        // But we haven't marked the runtime as stopped yet (m_stopped), so the contract
        // subsystem doesn't realize it yet.
        DISABLED(MODE_COOPERATIVE);
    }
    CONTRACTL_END;

    // @@@
    // Call from RCThread::MainLoop and TemporaryHelperThreadMainLoop.
    // when all threads suspended. Can happen on managed thread or helper thread.
    // If happen on managed thread, it must be doing the helper thread duty.
    //

    _ASSERTE(ThreadStore::HoldingThreadStore() || g_fProcessDetach);

    // We should be holding debugger lock m_mutex.
    _ASSERTE(ThreadHoldsLock());

    // We can't throw here (we're in the middle of the runtime suspension logic).
    // But things below us throw. So we catch the exception, but then what state are we in?

    _ASSERTE((!g_pEEInterface->GetThread() || !g_pEEInterface->GetThread()->m_fPreemptiveGCDisabled) || g_fInControlC);
    _ASSERTE(ThisIsHelperThreadWorker());

    STRESS_LOG0(LF_CORDB, LL_INFO10000, "D::SC: suspension complete\n");

    // We have suspended runtime.

    // We're stopped now. Marking m_stopped allows us to use MODE_COOPERATIVE contracts.
    _ASSERTE(!m_stopped && m_trappingRuntimeThreads);
    m_stopped = true;

    // If we're attaching, then this is the first time that all the
    // threads in the process have synchronized. Before sending the
    // sync complete event, we send back events telling the Right Side
    // which modules have already been loaded.
    if (m_syncingForAttach == SYNC_STATE_1)
    {
        // We should never be on a worker, only the real helper thread here.
        // This is b/c attach sends an event LS<--RS, and only the real helper
        // can pick it up.
        _ASSERTE(IsDbgHelperSpecialThread());

        STRESS_LOG0(LF_CORDB, LL_INFO10000, "D::SSCIPCE: syncing for attach, sending "
             "current module set.\n");

        EX_TRY
        {
            HRESULT hr;
            BOOL fAtleastOneEventSent = FALSE;

            hr = IterateAppDomainsForAttach(ONLY_SEND_APP_DOMAIN_CREATE_EVENTS,
                                            &fAtleastOneEventSent, TRUE);
            _ASSERTE (fAtleastOneEventSent == TRUE || FAILED(hr));

        }
        EX_CATCH
        {
            // We never actually expect to take an exception (maybe oom, but debugger
            // isn't robust on oom, so we can still assert)
            _ASSERTE(!"Unexpected Exception in Suspend");
        }
        EX_END_CATCH(RethrowTerminalExceptions);

        // update the state
        m_syncingForAttach = SYNC_STATE_2;
        STRESS_LOG1(LF_CORDB, LL_INFO10000, "Attach state is now %s\n", g_ppszAttachStateToSZ[m_syncingForAttach]);
    }

    // Send the sync complete event to the Right Side.
    {
        // If we fail to send the SyncComplete, what do we do?
        CONTRACT_VIOLATION(ThrowsViolation);

        SendSyncCompleteIPCEvent(); // sets m_stopped = true...
    }

    // Everything in the next scope is meant to mimic what we do UnlockForEventSending minus EnableEventHandling.
    // We do the EEH part when we get the Continue event.
    {
#ifdef _DEBUG
        //_ASSERTE(m_tidLockedForEventSending == GetCurrentThreadId());
        m_tidLockedForEventSending = 0;
#endif

        //
        // Event handling is re-enabled by the RCThread in response to a
        // continue message from the Right Side.

    }

    return TRUE;
}

ULONG inline Debugger::IsDebuggerAttachedToAppDomain(Thread *pThread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pThread));
    }
    CONTRACTL_END;

    AppDomain *pAppDomain = pThread->GetDomain();

    if (pAppDomain != NULL)
        return pAppDomain->IsDebuggerAttached();
    else
    {
        _ASSERTE (g_fEEShutDown);
        return 0;
    }
}


//
// SendCreateAppDomainEvent is called when a new AppDomain gets created.
//
void Debugger::SendCreateAppDomainEvent(AppDomain* pRuntimeAppDomain,
                                        BOOL fAttaching)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;

        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    if (CORDBUnrecoverableError(this))
        return;

    LOG((LF_CORDB, LL_INFO100, "D::SCADE: Create AppDomain 0x%08x (0x%08x) (Attaching: %s).\n",
        pRuntimeAppDomain, pRuntimeAppDomain->GetId().m_dwId,
        fAttaching?"TRUE":"FALSE"));

    STRESS_LOG3(LF_CORDB, LL_INFO10000, "D::SCADE: AppDomain creation:%#08x, %#08x, %#08x\n",
            pRuntimeAppDomain, pRuntimeAppDomain->GetId().m_dwId, fAttaching);


    bool disabled = false;
    DebuggerLockHolder dbgLockHolder(this, FALSE);

    //
    // If we're attaching, then we only need to send the event. We
    // don't need to disable event handling or lock the debugger
    // object.
    //
    if (!fAttaching)
    {
        disabled =  g_pEEInterface->IsPreemptiveGCDisabled();

        if (disabled)
            g_pEEInterface->EnablePreemptiveGC();

        // Prevent other Runtime threads from handling events.
        LockForEventSending(&dbgLockHolder);
    }

    // We may have detached while waiting in LockForEventSending,
    // in which case we can't send the event.
    // Note that CORDebuggerAttached() wont return true until we're finished
    // the attach, but if fAttaching, then there's a debugger listening
    if (CORDebuggerAttached() || fAttaching)
    {
        // Send a create appdomain event to the Right Side.
        DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(
            IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce,
                     DB_IPCE_CREATE_APP_DOMAIN,
                     g_pEEInterface->GetThread(),
                     pRuntimeAppDomain);

        ipce->AppDomainData.id = pRuntimeAppDomain->GetId().m_dwId;
        ipce->AppDomainData.fIsDefaultDomain = pRuntimeAppDomain->IsDefaultDomain();

        // No need to set the friendly name, the RS will get it via DAC.

        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::SCADE: Skipping SendIPCEvent because RS detached."));
    }

    if (!fAttaching)
    {
        // Stop all Runtime threads if we actually sent an event
        if (CORDebuggerAttached())
        {
            TrapAllRuntimeThreads(&dbgLockHolder, pRuntimeAppDomain);
        }

        // Let other Runtime threads handle their events.
        UnlockFromEventSending(&dbgLockHolder);

        g_pEEInterface->DisablePreemptiveGC();

        if (!disabled)
            g_pEEInterface->EnablePreemptiveGC();
    }
}


//
// SendExitAppDomainEvent is called when an app domain is destroyed.
//
void Debugger::SendExitAppDomainEvent(AppDomain* pRuntimeAppDomain)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    if (CORDBUnrecoverableError(this))
        return;

    LOG((LF_CORDB, LL_INFO100, "D::EAD: Exit AppDomain 0x%08x.\n",
        pRuntimeAppDomain));

    STRESS_LOG3(LF_CORDB, LL_INFO10000, "D::EAD: AppDomain exit:%#08x, %#08x, %#08x\n",
            pRuntimeAppDomain, pRuntimeAppDomain->GetId().m_dwId, CORDebuggerAttached());

    // Prevent other Runtime threads from handling events.
    DebuggerLockHolder dbgLockHolder(this, FALSE);
    SENDIPCEVENT_BEGIN(&dbgLockHolder);

    if (CORDebuggerAttached())
    {
        if (pRuntimeAppDomain->IsDefaultDomain() )
        {
            // The Debugger expects to never get an unload event for the default Domain.
            // Currently we should never get here because g_fProcessDetach will be true by
            // the time this method is called.  However, we'd like to know if this ever changes
            _ASSERTE(!"Trying to deliver notification of unload for default domain" );
            return;
        }

        // Send the exit appdomain event to the Right Side.
        DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce,
                     DB_IPCE_EXIT_APP_DOMAIN,
                     g_pEEInterface->GetThread(),
                     pRuntimeAppDomain);
        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

        // Delete any left over modules for this appdomain.
        // Note that we're doing this under the lock.
        if (m_pModules != NULL)
        {
            DebuggerDataLockHolder ch(this);
            m_pModules->RemoveModules(pRuntimeAppDomain);
        }

        // Stop all Runtime threads
        TrapAllRuntimeThreads(&dbgLockHolder, pRuntimeAppDomain);
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::EAD: Skipping SendIPCEvent because RS detached."));
    }

    SENDIPCEVENT_END;
}


//
// LoadAssembly is called when a new Assembly gets loaded.
//
void Debugger::LoadAssembly(AppDomain* pRuntimeAppDomain,
                            Assembly *pAssembly,
                            BOOL fIsSystemAssembly,
                            BOOL fAttaching)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    if (CORDBUnrecoverableError(this))
        return;

    LOG((LF_CORDB, LL_INFO100, "D::LA: Load Assembly Asy:0x%p AD:0x%p which:%ls\n",
        pAssembly, pRuntimeAppDomain, pAssembly->GetDebugName() ));

    bool disabled = false;
    DebuggerLockHolder dbgLockHolder(this, FALSE);

    //
    // If we're attaching, then we only need to send the event. We
    // don't need to disable event handling or lock the debugger
    // object.
    //
    if (!fAttaching)
    {
        disabled =  g_pEEInterface->IsPreemptiveGCDisabled();

        if (disabled)
            g_pEEInterface->EnablePreemptiveGC();

        // Prevent other Runtime threads from handling events.
        LockForEventSending(&dbgLockHolder);
    }

    if (CORDebuggerAttached() || fAttaching)
    {
        // Send a load assembly event to the Right Side.
        DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce,
                     DB_IPCE_LOAD_ASSEMBLY,
                     g_pEEInterface->GetThread(),
                     pRuntimeAppDomain);

        ipce->AssemblyData.debuggerAssemblyToken.Set(pAssembly);
        ipce->AssemblyData.fIsSystemAssembly =  fIsSystemAssembly;

        // Use the filename from the module that holds the assembly so
        // that we have the full path to the assembly and not just some
        // half-baked simple name.
        if (pAssembly->GetManifestModule()->IsIStream())
        {
            // No path for stream module
            ipce->AssemblyData.rcName.SetString(L"");
        }
        else
        {
            EX_TRY
            {
                ipce->AssemblyData.rcName.SetString(pAssembly->GetManifestModule()->GetPath());
            }
            EX_CATCH
            {
                ipce->AssemblyData.rcName.SetString(L"");
            }
            EX_END_CATCH(RethrowTransientExceptions);
        }

        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::LA: Skipping SendIPCEvent because RS detached."));
    }

    if (!fAttaching)
    {
        // Stop all Runtime threads
        if (CORDebuggerAttached())
        {
            TrapAllRuntimeThreads(&dbgLockHolder, pRuntimeAppDomain);
        }

        // Let other Runtime threads handle their events.
        UnlockFromEventSending(&dbgLockHolder);

        g_pEEInterface->DisablePreemptiveGC();

        if (!disabled)
            g_pEEInterface->EnablePreemptiveGC();
    }
}


//
// UnloadAssembly is called when a Runtime thread unloads an assembly.
//
void Debugger::UnloadAssembly(AppDomain *pAppDomain,
                              Assembly* pAssembly)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    if (CORDBUnrecoverableError(this))
        return;

    LOG((LF_CORDB, LL_INFO100, "D::UA: Unload Assembly Asy:0x%p AD:0x%p which:%ls\n",
         pAssembly, pAppDomain, pAssembly->GetDebugName() ));

    // Note that the debugger lock is reentrant, so we may or may not hold it already.
    Debugger::DebuggerLockHolder dbgLockHolder(this, FALSE);
    SENDIPCEVENT_BEGIN(&dbgLockHolder);

    // Send the unload assembly event to the Right Side.
    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);

    InitIPCEvent(ipce,
                 DB_IPCE_UNLOAD_ASSEMBLY,
                 g_pEEInterface->GetThread(),
                 pAppDomain);
    ipce->AssemblyData.debuggerAssemblyToken.Set(pAssembly);

    SendSimpleIPCEventAndBlock(&dbgLockHolder, IPC_TARGET_OUTOFPROC);

    // This will block on the continue
    SENDIPCEVENT_END;

}

// Create a new module
DebuggerModule* Debugger::AddDebuggerModule(Module* pRuntimeModule,
                                    AppDomain *pAppDomain)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
   
    _ASSERTE(ThreadHoldsLock()); // Debugger lock

    DebuggerDataLockHolder chInfo(this);

    DebuggerModule*  module = new (interopsafe) DebuggerModule(pRuntimeModule, pAppDomain);
    _ASSERTE(module != NULL); // throws on oom

    TRACE_ALLOC(module);

    if (FAILED(CheckInitModuleTable()))
    {
        DeleteInteropSafe(module);
        return (NULL);
    }
    m_pModules->AddModule(module);

    return module;
}

//
// LoadModule is called when a Runtime thread loads a new module and a debugger
// is attached.  This also includes when a domain-neutral module is "loaded" into
// a new domain.
//
void Debugger::LoadModule(Module* pRuntimeModule,
                          LPCWSTR pszModuleName, // module file name.
                          DWORD dwModuleName, // length of pszModuleName in chars, not including null.
                          Assembly *pAssembly,
                          AppDomain *pAppDomain,
                          BOOL fAttaching)
{

    CONTRACTL
    {
        THROWS;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    // @@@@
    // Implement DebugInterface but can be called internally as well.
    // This can be called by EE loading module or when we are attaching called by IteratingAppDomainForAttaching
    //

    if (CORDBUnrecoverableError(this))
        return;

    BOOL disabled = FALSE;
    DebuggerLockHolder dbgLockHolder(this, FALSE);

    //
    // If we're attaching, then we only need to send the event. We
    // don't need to disable event handling or lock the debugger
    // object.
    //
    if (!fAttaching)
    {
        disabled =  g_pEEInterface->IsPreemptiveGCDisabled();

        if (disabled)
            g_pEEInterface->EnablePreemptiveGC();

        // Prevent other Runtime threads from handling events.
        LockForEventSending(&dbgLockHolder);
    }

    if (pRuntimeModule->IsIStream() == FALSE)
    {
        SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
        pRuntimeModule->FusionCopyPDBs(pRuntimeModule->GetPath());
    }

    DebuggerModule *module = LookupModule(pRuntimeModule,pAppDomain);
    DebuggerIPCEvent* ipce = NULL;

    // Don't create new record if already loaded. We do still want to send the ModuleLoad event, however.
    // The RS has logic to ignore duplicate ModuleLoad events. We have to send what could possibly be a dup, though,
    // due to some really nasty issues with getting proper assembly and module load events from the loader when dealing
    // with shared assemblies.
    if (module)
    {
        STRESS_LOG4(LF_CORDB, LL_INFO10000, "D::LM: Existing module Mod:%#08x, %#08x, %#08x, %#08x\n",
            pRuntimeModule, pAssembly, pAppDomain, pRuntimeModule->IsIStream());

        LOG((LF_CORDB, LL_INFO100, "D::LM: module already loaded Mod:%#08x "
            "Asy:%#08x AD:%#08x isDynamic:0x%x runtimeMod:%#08x ModName:%ls\n",
             module, pAssembly, pAppDomain, pRuntimeModule->IsReflection(), pRuntimeModule, pszModuleName));
    }
    else
    {
        STRESS_LOG4(LF_CORDB, LL_INFO10000, "D::LM: new module loaded Mod:%#08x, %#08x, %#08x, %#08x\n",
            pRuntimeModule, pAssembly, pAppDomain, pRuntimeModule->IsIStream());

        module = AddDebuggerModule(pRuntimeModule, pAppDomain);

        LOG((LF_CORDB, LL_INFO100, "D::LM: load module Mod:%#08x "
            "Asy:%#08x AD:%#08x isDynamic:0x%x runtimeMod:%#08x ModName:%ls\n",
            module, pAssembly, pAppDomain, pRuntimeModule->IsReflection(), pRuntimeModule, pszModuleName));
    }

    // Send a load module event to the Right Side.
    ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    InitIPCEvent(ipce,DB_IPCE_LOAD_MODULE, g_pEEInterface->GetThread(), pAppDomain);
    ipce->LoadModuleData.debuggerModuleToken.Set(module);
    ipce->LoadModuleData.debuggerAssemblyToken.Set(pAssembly);

    if ((pszModuleName == NULL) || (*pszModuleName == L'\0'))
        ipce->LoadModuleData.fInMemory = TRUE;
    else
        ipce->LoadModuleData.fInMemory = FALSE;

    ipce->LoadModuleData.fIsDynamic = pRuntimeModule->IsReflection();

    ipce->LoadModuleData.rcFullNgenName.SetString(L"");

    if (!ipce->LoadModuleData.fIsDynamic)
    {
        COUNT_T nPESize;
        void * baseAddress = pRuntimeModule->GetFile()->GetDebuggerContents(&nPESize);

        ipce->LoadModuleData.pPEBaseAddress = baseAddress;
        ipce->LoadModuleData.nPESize = nPESize;

        COUNT_T size;
        ipce->LoadModuleData.pMetadataStart
          = (void *) pRuntimeModule->GetFile()->GetLoadedMetadata(&size);
        ipce->LoadModuleData.nMetadataSize = size;


    }
    else
    {
        ipce->LoadModuleData.pPEBaseAddress = 0;
        ipce->LoadModuleData.nPESize = 0;

        BYTE *rgb;
        DWORD cb;

        HRESULT hr = ModuleMetaDataToMemory( pRuntimeModule, &rgb, &cb);
        if (!FAILED(hr))
        {
            ipce->LoadModuleData.pMetadataStart = rgb; //get this
            ipce->LoadModuleData.nMetadataSize = cb;
        }
        else
        {
            ipce->LoadModuleData.pMetadataStart = 0; //get this
            ipce->LoadModuleData.nMetadataSize = 0;
        }
        LOG((LF_CORDB,LL_INFO10000, "D::LM: putting dynamic, new mD at 0x%x, "
             "size 0x%x\n",ipce->LoadModuleData.pMetadataStart,
             ipce->LoadModuleData.nMetadataSize));

        // Dynamic modules must receive ClassLoad callbacks in order to receive metadata updates as the module
        // evolves. So we force this on here and refuse to change it for all dynamic modules.
        module->EnableClassLoadCallbacks(TRUE);
    }

    {
        // Never give an empty module name...
        const WCHAR *moduleName = NULL;

        // SStrings may allocate.
        SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
        SString szModuleTemp;

        if (dwModuleName > 0)
        {
            // The module name is the file name of the module
            moduleName = pszModuleName;
        }
        else
        {
            // If we don't have a name for the reflection (Dynamic) modules then get the simple name.
            if (ipce->LoadModuleData.fIsDynamic)
            {
                // The simple name is the module name in the metadata
                LPCUTF8 pSimpleName = pRuntimeModule->GetSimpleName();
                _ASSERTE(pSimpleName != NULL); // should always have a simple name.

                szModuleTemp.SetUTF8(pSimpleName); // Use SString to convert to unicode.
                moduleName = szModuleTemp.GetUnicode();
                dwModuleName = (DWORD)wcslen(moduleName);
            }
            else
            {
                // If we absoutely don't have a name, then send over empty and let the RS deal with it.
                moduleName = L"";
            }
        }

        _ASSERTE(moduleName != NULL);
        ipce->LoadModuleData.rcName.SetString(moduleName);
    }

    m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

    if (fAttaching)
    {
        UpdateModuleSyms(pRuntimeModule,
                         pAppDomain,
                         fAttaching);
    }
    else // !fAttaching
    {
        // Stop all Runtime threads
        TrapAllRuntimeThreads(&dbgLockHolder, pAppDomain);

        // Let other Runtime threads handle their events.
        UnlockFromEventSending(&dbgLockHolder);

        g_pEEInterface->DisablePreemptiveGC();

        if (!disabled)
            g_pEEInterface->EnablePreemptiveGC();
    }

    // need to update pdb stream for SQL passed in pdb stream
    // regardless attach or not.
    //
    if (pRuntimeModule->IsIStream())
    {
        SendPdbStream(pRuntimeModule, pAppDomain, fAttaching);
    }
}


//
// SendPdbStream is called when we update pdb for host loaded modules.
//
HRESULT Debugger::SendPdbStream(Module* pRuntimeModule,
                                AppDomain *pAppDomain,
                                BOOL fAttaching)
{
    HRESULT     hr = NOERROR;
    IStream     *pStream = NULL;
    STATSTG     SizeData = {0};
    DWORD       streamSize = 0;
    DWORD       cbRead;
    DebuggerModule* module = NULL;
    BYTE        *bPdbBuf = NULL;
    bool        disabled = false;
    DebuggerIPCEvent* ipce = NULL;
    DebuggerLockHolder dbgLockHolder(this, FALSE);

    if (CORDBUnrecoverableError(this))
    {
        goto ErrExit;
    }

    STRESS_LOG3(LF_CORDB, LL_INFO10000, "D::SPS: SendPdbStream:%p, %p, %#08x\n",
            pRuntimeModule, pAppDomain, fAttaching);

    // No pdb stream, then we are done.
    hr = pRuntimeModule->GetHostPdbStream(&pStream);
    if (pStream == NULL)
    {
        // No symbols to update (eg, symbols are on-disk),
        STRESS_LOG1(LF_CORDB, LL_INFO10000, "D::SPS: SendPdbStream - No Pdb Stream:%p\n",
            pRuntimeModule);
        return hr;
    }

    if (!fAttaching && GetThread() != NULL)
    {
        disabled =  g_pEEInterface->IsPreemptiveGCDisabled();

        if (disabled)
            g_pEEInterface->EnablePreemptiveGC();
    }

    if (!fAttaching)
    {
        // Prevent other Runtime threads from handling events.
        LockForEventSending(&dbgLockHolder);
    }

    module = LookupModule(pRuntimeModule, pAppDomain);

    _ASSERTE(module != NULL);
    _ASSERTE(pRuntimeModule->IsIStream() == TRUE);

    if (CORDebuggerAttached() || fAttaching)
    {
        //
        hr = pStream->Stat(&SizeData, STATFLAG_NONAME);
        if (FAILED(hr))
        {
            goto ErrExit;
        }

        streamSize = SizeData.cbSize.u.LowPart;

        if (SizeData.cbSize.u.HighPart > 0)
        {
            // Too big to deal with it.
            hr = E_OUTOFMEMORY;
            goto ErrExit;
        }


        // now allocate memory for the pdb
        bPdbBuf = (BYTE *)new (interopsafe) BYTE[streamSize];
        if (bPdbBuf == NULL)
        {
            goto ErrExit;
        }
        hr = pStream->Read(bPdbBuf, streamSize, &cbRead);
        if (FAILED(hr))
        {
            goto ErrExit;
        }

        // No symbols to update (eg, symbols are on-disk),
        STRESS_LOG2(LF_CORDB, LL_INFO10000, "D::SPS: SendPdbStream - got pdb Stream:%p, %#08x\n",
            pRuntimeModule, streamSize);

        // Send a update module syns event to the Right Side.
        ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce, DB_IPCE_UPDATE_MODULE_SYMS,
                     g_pEEInterface->GetThread(),
                     pAppDomain);
        ipce->UpdateModuleSymsData.debuggerModuleToken.Set(module);
        ipce->UpdateModuleSymsData.debuggerAppDomainToken.Set(pAppDomain);
        ipce->UpdateModuleSymsData.dataBuffer.SetLsData((BYTE *)bPdbBuf, streamSize);

        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::UMS: Skipping SendIPCEvent because RS detached."));
    }

    if(!fAttaching)
    {
        // Stop all Runtime threads if we sent a message
        if (CORDebuggerAttached())
        {
            TrapAllRuntimeThreads(&dbgLockHolder, pAppDomain);
        }

        // Let other Runtime threads handle their events.
        UnlockFromEventSending(&dbgLockHolder);

        if (GetThread() != NULL)
        {
            g_pEEInterface->DisablePreemptiveGC();

            if (!disabled)
                g_pEEInterface->EnablePreemptiveGC();
        }
    }

ErrExit:
    // free the buffer since RS has made a copy eagerly.
    if (bPdbBuf != NULL)
    {
        DeleteInteropSafe(bPdbBuf);
    }

    if (pStream != NULL)
    {
        pStream->Release();
        pRuntimeModule->ClearHostPdbStream();
    }
    return hr;
}

//
// UpdateModuleSyms is called when the symbols for a module need to be
// sent to the Right Side because they've changed.
//
void Debugger::UpdateModuleSyms(Module* pRuntimeModule,
                                AppDomain *pAppDomain,
                                BOOL fAttaching)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    // The metadata objects that we use here call New and we can't stop that.
    // This is a potential deadlock in interop-debugging.
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;

    DebuggerIPCEvent* ipce = NULL;
    DebuggerLockHolder dbgLockHolder(this, FALSE);

    if (CORDBUnrecoverableError(this))
        return;

    CGrowableStream *pStream = pRuntimeModule->GetInMemorySymbolStream();

    LOG((LF_CORDB, LL_INFO10000, "D::UMS: update module syms "
         "RuntimeModule:0x%08x CGrowableStream:0x%08x\n",
         pRuntimeModule, pStream));

    DebuggerModule* module = LookupModule(pRuntimeModule, pAppDomain);
    PREFIX_ASSUME(module != NULL);

    if (pStream == NULL)
    {
        // we have loaded pdb already
        return;
    }

    STATSTG SizeData = {0};
    DWORD streamSize = 0;
    HRESULT hr = S_OK;
    bool disabled = false;

    IMetaDataImport *pImport = NULL;
    IMetaDataEmit   *pEmitter = NULL;
    EX_TRY
    {
        pImport = pRuntimeModule->GetRWImporter();
        pEmitter = pRuntimeModule->GetEmitter();
    }
    EX_CATCH
    {
        goto LExit;
    }
    EX_END_CATCH(RethrowTerminalExceptions);

    // for ISymUnmanagedWriter to work, we need to call Commit to flush PDB info into the stream
    if (pRuntimeModule->IsReflection())
    {
        ISymUnmanagedWriter* pWriter = pRuntimeModule->GetReflectionModule()->GetISymUnmanagedWriter();
        if( pWriter != NULL )
        {
            SafeComHolder<ISymUnmanagedWriter3> pWriter3;
            HRESULT thr = pWriter->QueryInterface(IID_ISymUnmanagedWriter3, (void**)&pWriter3);
            _ASSERTE( SUCCEEDED(thr) );
            if( SUCCEEDED(thr) )
            {
                thr = pWriter3->Commit();
                _ASSERTE( SUCCEEDED(thr) || thr == E_OUTOFMEMORY );
            }
        }
    }

    hr = pStream->Stat(&SizeData, STATFLAG_NONAME);

    streamSize = SizeData.cbSize.u.LowPart;
    if (FAILED(hr))
    {
        goto LExit;
    }

    if (SizeData.cbSize.u.HighPart > 0)
    {
        hr = E_OUTOFMEMORY;
        goto LExit;
    }

    if (!fAttaching && GetThread() != NULL)
    {
        disabled =  g_pEEInterface->IsPreemptiveGCDisabled();

        if (disabled)
            g_pEEInterface->EnablePreemptiveGC();
    }

    if (!fAttaching)
    {
        // Prevent other Runtime threads from handling events.
        LockForEventSending(&dbgLockHolder);
    }

    if (CORDebuggerAttached() || fAttaching)
    {
        // Send a update module syns event to the Right Side.
        ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce, DB_IPCE_UPDATE_MODULE_SYMS,
                     g_pEEInterface->GetThread(),
                     pAppDomain);
        ipce->UpdateModuleSymsData.debuggerModuleToken.Set(module);
        ipce->UpdateModuleSymsData.debuggerAppDomainToken.Set(pAppDomain);
        ipce->UpdateModuleSymsData.dataBuffer.SetLsData((BYTE *)pStream->GetBuffer(), streamSize);

        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::UMS: Skipping SendIPCEvent because RS detached."));
    }

    // We used to set HasLoadedSymbols here, but we don't really want
    // to do that in the face of the same module being in multiple app
    // domains.

    if(!fAttaching)
    {
        // Stop all Runtime threads if we sent a message
        if (CORDebuggerAttached())
        {
            TrapAllRuntimeThreads(&dbgLockHolder, pAppDomain);
        }

        // Let other Runtime threads handle their events.
        UnlockFromEventSending(&dbgLockHolder);

        if (GetThread() != NULL)
        {
            g_pEEInterface->DisablePreemptiveGC();

            if (!disabled)
                g_pEEInterface->EnablePreemptiveGC();
        }
    }
LExit:
    ; // Debugger must free buffer using RELEASE_BUFFER message!
    if( FAILED(hr) )
    {
        LOG((LF_CORDB, LL_WARNING, "D::UMS: update module syms failed for %u with hr=%x", 
            pRuntimeModule->GetPath().GetUnicode(), hr ));
    }

    // return now
}

/******************************************************************************
 * Get BYTE stream for a modules metadata.
 *
 * pMod - module to get metadata from.
 * prgb - output buffer to store metadata.
 * pcb - output count of bytes for size of prgb buffer.
 ******************************************************************************/
HRESULT Debugger::ModuleMetaDataToMemory(Module *pMod, BYTE **prgb, DWORD *pcb)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    IMetaDataEmit *pIMDE = NULL;

    EX_TRY
    {
        pIMDE = pMod->GetEmitter();
    }
    EX_CATCH
    {
        pIMDE = NULL;
    }
    EX_END_CATCH(RethrowTerminalExceptions);

    if (pIMDE == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // This isn't ideal - Making ModuleMetaDataToMemory not call new is hard,
    // but the odds of trying to load a module after a thread is stopped w/
    // the heap lock should be pretty low.
    // All of the metadata calls can violate this and call new.
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;

    HRESULT hr;


    hr = pIMDE->GetSaveSize(cssQuick, pcb);
    if (FAILED(hr))
    {
        *pcb = 0;
        return hr;
    }


    (*prgb) = new (interopsafe, nothrow) BYTE[*pcb];
    if (NULL == (*prgb))
    {
        *pcb = 0;
        return E_OUTOFMEMORY;
    }

    hr = pIMDE->SaveToMemory((*prgb), *pcb);
    if (FAILED(hr))
    {
        *pcb = 0;
        return hr;
    }

    pIMDE = NULL; // note that the emiiter SHOULD NOT be released

    LOG((LF_CORDB,LL_INFO1000, "D::MMDTM: Saved module 0x%x MD to 0x%x "
        "(size:0x%x)\n", pMod, *prgb, *pcb));

    return S_OK;
}

void Debugger::UnloadModule(Module* pRuntimeModule,
                            AppDomain *pAppDomain)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    // @@@@
    // implements DebugInterface.
    // can only called by EE on Module::NotifyDebuggerUnload
    //

    if (CORDBUnrecoverableError(this))
        return;



    LOG((LF_CORDB, LL_INFO100, "D::UM: unload module Mod:%#08x AD:%#08x runtimeMod:%#08x modName:%ls\n",
         LookupModule(pRuntimeModule, pAppDomain), pAppDomain, pRuntimeModule, pRuntimeModule->GetDebugName()));


    DebuggerLockHolder dbgLockHolder(this, FALSE);

    SENDIPCEVENT_BEGIN(&dbgLockHolder);

    if (CORDebuggerAttached())
    {

        DebuggerModule* module = LookupModule(pRuntimeModule, pAppDomain);
        if (module == NULL)
        {
            LOG((LF_CORDB, LL_INFO100, "D::UM: module already unloaded AD:%#08x runtimeMod:%#08x modName:%ls\n",
                 pAppDomain, pRuntimeModule, pRuntimeModule->GetDebugName()));
            goto LExit;
        }
        _ASSERTE(module != NULL);

        STRESS_LOG3(LF_CORDB, LL_INFO10000, "D::UM: Unloading Mod:%#08x, %#08x, %#08x\n",
            pRuntimeModule, pAppDomain, pRuntimeModule->IsIStream());

        _ASSERTE(module->GetAppDomain() == pAppDomain);

        // Send the unload module event to the Right Side.
        DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce, DB_IPCE_UNLOAD_MODULE, g_pEEInterface->GetThread(), pAppDomain);
        ipce->UnloadModuleData.debuggerModuleToken.Set(module);
        ipce->UnloadModuleData.debuggerAssemblyToken.Set(pRuntimeModule->GetClassLoader()->GetAssembly());
        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

        //
        // Cleanup the module (only for resources consumed when a debugger is attached)
        //

        // Remove all patches that apply to this module/AppDomain combination
        AppDomain* domainToRemovePatchesIn = NULL;  // all domains by default
        if( pRuntimeModule->GetAssembly()->IsDomainNeutral() )
        {
            // Deactivate all the patches specific to the AppDomain being unloaded
            domainToRemovePatchesIn = pAppDomain;
        }
        // Note that we'll explicitly NOT delete DebuggerControllers, so that
        // the Right Side can delete them later.
        DebuggerController::RemovePatchesFromModule(pRuntimeModule, domainToRemovePatchesIn);

        // Deactive all JMC functions in this module.  We don't do this for shared assemblies
        // because JMC status is not maintained on a per-AppDomain basis and we don't
        // want to change the JMC behavior of the module in other domains.
        if( !pRuntimeModule->GetAssembly()->IsDomainNeutral() )
        {
	    LOG((LF_CORDB, LL_EVERYTHING, "Setting all JMC methods to false:\n"));
            DebuggerDataLockHolder debuggerDataLockHolder(this);
            DebuggerMethodInfoTable * pTable = GetMethodInfoTable();
            if (pTable != NULL)
            {
                HASHFIND info;

                for (DebuggerMethodInfo *dmi = pTable->GetFirstMethodInfo(&info);
                    dmi != NULL;
                    dmi = pTable->GetNextMethodInfo(&info))
                {
                    if (dmi->m_module == pRuntimeModule)
                    {
                        dmi->SetJMCStatus(false);
                    }
                }
            }
            LOG((LF_CORDB, LL_EVERYTHING, "Done clearing JMC methods!\n"));
        }
    
        // Delete the Left Side representation of the module.
        if (m_pModules != NULL)
        {
            DebuggerDataLockHolder chInfo(this);
            m_pModules->RemoveModule(pRuntimeModule, pAppDomain);
        }

        // Stop all Runtime threads
        TrapAllRuntimeThreads(&dbgLockHolder, pAppDomain);
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::UM: Skipping SendIPCEvent because RS detached."));
    }

LExit:
    SENDIPCEVENT_END;
}

void Debugger::DestructModule(Module *pModule)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO100, "D::DM: destruct module runtimeMod:%#08x modName:%ls\n",
         pModule, pModule->GetDebugName()));

    // @@@
    // Implements DebugInterface.
    // It is called for Module::Destruct. We do not need to send any IPC event.

    DebuggerLockHolder dbgLockHolder(this);

    // We should have removed all patches at AD unload time (or detach time if the
    // debugger detached).   
    _ASSERTE( !DebuggerController::ModuleHasPatches(pModule) );

    // Do module clean-up that applies even when no debugger is attached.
    // Ideally, we might like to do this cleanup more eagerly and detministically,
    // but we don't currently get any early AD unload callback from the loader
    // when no debugger is attached.  Perhaps we should make the loader
    // call this callback earlier.
    RemoveModuleReferences(pModule);
}


// Internal helper to remove all the DJIs / DMIs and other references for a given Module.
// If we don't remove the DJIs / DMIs, then we're subject to recycling bugs because the underlying
// MethodDescs will get removed. Thus we'll look up a new MD and it will pull up an old DMI that matched
// the old MD. Now the DMI and MD are out of sync and it's downhill from there.
// Note that DMIs may be used (and need cleanup) even when no debugger is attached.
void Debugger::RemoveModuleReferences( Module* pModule )
{
    _ASSERTE( ThreadHoldsLock() );

    // We want to remove all references to the module from the various
    // tables.  It's not just possible, but probable, that the module
    // will be re-loaded at the exact same address, and in that case,
    // we'll have piles of entries in our DJI table that mistakenly
    // match this new module.
    // Note that this doesn't apply to domain neutral assemblies, that only
    // get unloaded when the process dies.  We won't be reclaiming their
    // DJIs/patches b/c the process is going to die, so we'll reclaim
    // the memory when the various hashtables are unloaded.

    if (m_pMethodInfos != NULL)
    {
        HRESULT hr = S_OK;
        if (!HasLazyData())
        {
            hr = LazyInitWrapper();
        }

        if (SUCCEEDED(hr))
        {
            DebuggerDataLockHolder debuggerDataLockHolder(this);

            m_pMethodInfos->ClearMethodsOfModule(pModule);

            // DebuggerDataLockHolder out of scope - release implied
        }
    } 
}

/******************************************************************************
 *
 ******************************************************************************/
void Debugger::SendClassLoadUnloadEvent (mdTypeDef classMetadataToken,
                                         DebuggerModule *classModule,
                                         Assembly *pAssembly,
                                         AppDomain *pAppDomain,
                                         BOOL fIsLoadEvent)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    LOG((LF_CORDB,LL_INFO10000, "D::SCLUE: Tok:0x%x isLoad:0x%x Mod:%#08x AD:%#08x\n",
        classMetadataToken, fIsLoadEvent, classModule, pAppDomain));

    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    BOOL isReflection = classModule->GetRuntimeModule()->IsReflection();
    if (fIsLoadEvent == TRUE)
    {
        // We need to update Metadata before Symbols (since symbols depend on metadata)
        // It's debatable which needs to come first: Class Load or Sym update.
        // V1.1 sent Sym Update first so that binding at the class load has the latest symbols.
        // However, The Class Load may need to be in sync with updating new metadata,
        // and that has to come before the Sym update.
        InitIPCEvent(ipce,
                     DB_IPCE_LOAD_CLASS,
                     g_pEEInterface->GetThread(),
                     pAppDomain);
        ipce->LoadClass.classMetadataToken = classMetadataToken;
        ipce->LoadClass.classDebuggerModuleToken.Set(classModule);
        ipce->LoadClass.classDebuggerAssemblyToken.Set(pAssembly);

        if (isReflection)
        {
            HRESULT hr;
            hr = ModuleMetaDataToMemory(classModule->GetRuntimeModule(),
                                        &(ipce->LoadClass.pNewMetaData),
                                        &(ipce->LoadClass.cbNewMetaData));

            if (FAILED(hr))
            {
                ipce->LoadClass.pNewMetaData = NULL;
                ipce->LoadClass.cbNewMetaData = 0;
            }
        }
    }
    else
    {
        InitIPCEvent(ipce,
                     DB_IPCE_UNLOAD_CLASS,
                     g_pEEInterface->GetThread(),
                     pAppDomain);
        ipce->UnloadClass.classMetadataToken = classMetadataToken;
        ipce->UnloadClass.classDebuggerModuleToken.Set(classModule);
        ipce->UnloadClass.classDebuggerAssemblyToken.Set(pAssembly);
    }

    m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

    if (fIsLoadEvent == TRUE && isReflection)
    {
        // We're not actually attaching, but it's behaviourly identical
        UpdateModuleSyms(classModule->GetRuntimeModule(), pAppDomain, TRUE);
    }

}



/******************************************************************************
 *
 ******************************************************************************/
BOOL Debugger::SendSystemClassLoadUnloadEvent(mdTypeDef classMetadataToken,
                                              Module *classModule,
                                              BOOL fIsLoadEvent)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    if (!m_dClassLoadCallbackCount)
    {
        return FALSE;
    }

    BOOL fRetVal = FALSE;

    Assembly *pAssembly = classModule->GetAssembly();

    if (!m_pAppDomainCB->Lock())
        return (FALSE);

    AppDomainInfo *pADInfo = m_pAppDomainCB->FindFirst();

    while (pADInfo != NULL)
    {
        AppDomain *pAppDomain = pADInfo->m_pAppDomain;
        _ASSERTE(pAppDomain != NULL);

        if ((pAppDomain->IsDebuggerAttached() || (pAppDomain->GetDebuggerAttached() & AppDomain::DEBUGGER_ATTACHING_THREAD)) &&
            (pAppDomain->ContainsAssembly(pAssembly) || pAssembly->IsSystem()) &&
            !(fIsLoadEvent && pAppDomain->IsUnloading()) )
        {
            // Find the Left Side module that this class belongs in.
            DebuggerModule* pModule = LookupModule(classModule, pAppDomain);
            //_ASSERTE(pModule != NULL);

            // Only send a class load event if they're enabled for this module.
            if (pModule && pModule->ClassLoadCallbacksEnabled())
            {
                SendClassLoadUnloadEvent(classMetadataToken,
                                         pModule,
                                         pAssembly,
                                         pAppDomain,
                                         fIsLoadEvent);
                fRetVal = TRUE;
            }
        }

        pADInfo = m_pAppDomainCB->FindNext(pADInfo);
    }

    m_pAppDomainCB->Unlock();

    return fRetVal;
}


//
// LoadClass is called when a Runtime thread loads a new Class.
// Returns TRUE if an event is sent, FALSE otherwise
BOOL  Debugger::LoadClass(TypeHandle th,
                          mdTypeDef  classMetadataToken,
                          Module    *classModule,
                          AppDomain *pAppDomain,
                          BOOL fSendEventToAllAppDomains,   // unused parameter
                          BOOL fAttaching)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    // @@@
    // Implements DebugInterface
    // This can be called by EE/Loader when class is loaded or called by our attach logic.
    //

    BOOL fRetVal = FALSE;

    if (CORDBUnrecoverableError(this))
        return FALSE;

    // Note that pAppDomain may be null.  The AppDomain isn't used here, and doesn't make a lot of sense since
    // we may be delivering the notification for a class in an assembly which is loaded into multiple AppDomains.  We 
    // handle this in SendSystemClassLoadUnloadEvent below by looping through all AppDomains and dispatching 
    // events for each that contain this assembly.
    
    LOG((LF_CORDB, LL_INFO10000, "D::LC: load class Tok:%#08x Mod:%#08x AD:%#08x classMod:%#08x modName:%ls\n",
         classMetadataToken, (pAppDomain == NULL) ? NULL : LookupModule(classModule, pAppDomain),
         pAppDomain, classModule, classModule->GetDebugName()));

    //
    // If we're attaching, then we only need to send the event. We
    // don't need to disable event handling or lock the debugger
    // object.
    //
    bool disabled = false;
    DebuggerLockHolder dbgLockHolder(this, FALSE);

    if (!fAttaching)
    {
        // Enable preemptive GC...
        disabled = g_pEEInterface->IsPreemptiveGCDisabled();

        if (disabled)
            g_pEEInterface->EnablePreemptiveGC();

        // Prevent other Runtime threads from handling events.
        LockForEventSending(&dbgLockHolder);
    }

    if (CORDebuggerAttached() || fAttaching)
    {
        fRetVal = SendSystemClassLoadUnloadEvent(classMetadataToken, classModule, TRUE);

        if (fRetVal == TRUE)
        {
            // Stop all Runtime threads
            TrapAllRuntimeThreads(&dbgLockHolder, pAppDomain);
        }
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::LC: Skipping SendIPCEvent because RS detached."));
    }

    if (!fAttaching)
    {
        // Let other Runtime threads handle their events.
        UnlockFromEventSending(&dbgLockHolder);

        g_pEEInterface->DisablePreemptiveGC();

        if (!disabled)
            g_pEEInterface->EnablePreemptiveGC();
    }

    return fRetVal;
}


//
// UnloadClass is called when a Runtime thread unloads a Class.
//
void Debugger::UnloadClass(mdTypeDef classMetadataToken,
                           Module *classModule,
                           AppDomain *pAppDomain,
                           BOOL fSendEventToAllAppDomains)  // Unused parameter
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    // @@@
    // Implements DebugInterface
    // Can only be called from EE

    DebuggerLockHolder dbgLockHolder(this, FALSE);

    if (CORDBUnrecoverableError(this))
    {
        return;
    }

    LOG((LF_CORDB, LL_INFO10000, "D::UC: unload class Tok:0x%08x Mod:%#08x AD:%#08x runtimeMod:%#08x modName:%ls\n",
         classMetadataToken, LookupModule(classModule, pAppDomain), pAppDomain, classModule, classModule->GetDebugName()));

    Assembly *pAssembly = classModule->GetClassLoader()->GetAssembly();
    DebuggerModule *pModule = LookupModule(classModule, pAppDomain);

    if ((pModule == NULL) || !pModule->ClassLoadCallbacksEnabled())
    {
        return;
    }

    bool toggleGC = false;

    if (GetThread() != NULL)
    {
        toggleGC =  g_pEEInterface->IsPreemptiveGCDisabled();

        if (toggleGC)
        {
            g_pEEInterface->EnablePreemptiveGC();
        }
    }

    // Prevent other Runtime threads from handling events.
    LockForEventSending(&dbgLockHolder);

    if (CORDebuggerAttached())
    {
        _ASSERTE((pAppDomain != NULL) && (pAssembly != NULL) && (pModule != NULL));

        SendClassLoadUnloadEvent(classMetadataToken, pModule, pAssembly, pAppDomain, FALSE);

        // Stop all Runtime threads
        TrapAllRuntimeThreads(&dbgLockHolder, pAppDomain);
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::UC: Skipping SendIPCEvent because RS detached."));
    }

    // Let other Runtime threads handle their events.
    UnlockFromEventSending(&dbgLockHolder);

    if (GetThread() != NULL && toggleGC)
    {
        g_pEEInterface->DisablePreemptiveGC();
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void Debugger::FuncEvalComplete(Thread* pThread, DebuggerEval *pDE)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifndef DACCESS_COMPILE

    if (CORDBUnrecoverableError(this))
        return;

    LOG((LF_CORDB, LL_INFO10000, "D::FEC: func eval complete pDE:%08x evalType:%d %s %s\n",
        pDE, pDE->m_evalType, pDE->m_successful ? "Success" : "Fail", pDE->m_aborted ? "Abort" : "Completed"));


    _ASSERTE(pDE->m_completed);
    _ASSERTE((g_pEEInterface->GetThread() && !g_pEEInterface->GetThread()->m_fPreemptiveGCDisabled) || g_fInControlC);
    _ASSERTE(ThreadHoldsLock());

    // If we need to rethrow a ThreadAbortException then set the thread's state so we remember that.
    if (pDE->m_rethrowAbortException)
    {
        pThread->SetThreadStateNC(Thread::TSNC_DebuggerReAbort);
    }        


    //
    // Get the domain that the result is valid in. The RS will cache this in the ICorDebugValue
    // Note: it's possible that the AppDomain has (or is about to be) unloaded, which could lead to a 
    // crash when we use the DebuggerModule.  Ideally we'd only be using AppDomain IDs here. 
    // We can't easily convert our ADID to an AppDomain* (SystemDomain::GetAppDomainFromId)
    // because we can't proove that that the AppDomain* would be valid (not unloaded). 
    //
    AppDomain *pDomain = pThread->GetDomain();
    AppDomain *pResultDomain = ((pDE->m_debuggerModule == NULL) ? pDomain : pDE->m_debuggerModule->GetAppDomain());
    _ASSERTE( pResultDomain->GetId() == pDE->m_appDomainId );
    
    // Send a func eval complete event to the Right Side.
    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    InitIPCEvent(ipce, DB_IPCE_FUNC_EVAL_COMPLETE, pThread, pDomain);

    ipce->FuncEvalComplete.funcEvalKey = pDE->m_funcEvalKey;
    ipce->FuncEvalComplete.successful = pDE->m_successful;
    ipce->FuncEvalComplete.aborted = pDE->m_aborted;
    ipce->FuncEvalComplete.resultAddr = &(pDE->m_result);
    ipce->FuncEvalComplete.resultAppDomainToken = LSPTR_APPDOMAIN::MakePtr(pResultDomain);
    ipce->FuncEvalComplete.objectHandle = pDE->m_objectHandle;

    LOG((LF_CORDB, LL_INFO10000, "D::FEC: TypeHandle is :%08x\n", pDE->m_resultType.AsPtr()));

    Debugger::TypeHandleToExpandedTypeInfo(pDE->m_retValueBoxing, // whether return values get boxed or not depends on the particular FuncEval we're doing...
                                           pResultDomain,
                                           pDE->m_resultType,
                                           &ipce->FuncEvalComplete.resultType,
                                           IPC_TARGET_OUTOFPROC);

    _ASSERTE(ipce->FuncEvalComplete.resultType.elementType != ELEMENT_TYPE_VALUETYPE);

    LOG((LF_CORDB, LL_INFO10000, "D::FEC: returned from call\n"));

    // We must adjust the result address to point to the right place
    unsigned size = GetSizeForCorElementType(ipce->FuncEvalComplete.resultType.elementType);
    ipce->FuncEvalComplete.resultAddr = ArgSlotEndianessFixup((ARG_SLOT*)ipce->FuncEvalComplete.resultAddr, size);

    m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

#endif
}

/* ------------------------------------------------------------------------ *
 * Right Side Interface routines
 * ------------------------------------------------------------------------ */

//
// GetFunctionInfo returns various bits of function information given
// a module and a token. The info will come from a MethodDesc, if
// one exists (and the fd will be returned) or the info will come from
// metadata.
//
HRESULT Debugger::GetFunctionInfo(Module *pModule, mdToken functionToken,
                                  ULONG *pRVA,
                                  BYTE  **pCodeStart,
                                  unsigned int *pCodeSize,
                                  mdToken *pLocalSigToken)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    DWORD implFlags;

    // Get the RVA and impl flags for this method.
    hr = g_pEEInterface->GetMethodImplProps(pModule,
                                            functionToken,
                                            pRVA,
                                            &implFlags);

    if (SUCCEEDED(hr))
    {
        // If the RVA is 0 or it's native, then the method is not IL
        if (*pRVA == 0)
        {
            LOG((LF_CORDB,LL_INFO100000, "D::GFI: Function is not IL - *pRVA == NULL!\n"));
            //return (CORDBG_E_FUNCTION_NOT_IL);
            // Sanity check this....
            MethodDesc* pFD =
                g_pEEInterface->FindLoadedMethodRefOrDef(pModule, functionToken);

            if(!pFD || !pFD->IsIL())
            {
                LOG((LF_CORDB,LL_INFO100000, "D::GFI: And the MD agrees..\n"));
                return(CORDBG_E_FUNCTION_NOT_IL);
            }
            else
            {
                LOG((LF_CORDB,LL_INFO100000, "D::GFI: But the MD says it's IL..\n"));
            }

            if (pFD != NULL && g_pEEInterface->MethodDescGetRVA(pFD) == 0)
            {
                LOG((LF_CORDB,LL_INFO100000, "D::GFI: Actually, MD says RVA is 0 too - keep going...!\n"));
            }
        }
        if (IsMiNative(implFlags))
        {
            LOG((LF_CORDB,LL_INFO100000, "D::GFI: Function is not IL - IsMiNative!\n"));
            return (CORDBG_E_FUNCTION_NOT_IL);
        }

        COR_ILMETHOD *ilMeth = (COR_ILMETHOD*) pModule->GetIL(*pRVA);
        COR_ILMETHOD_DECODER header(ilMeth);

        // Snagg the IL code info.
        *pCodeStart = const_cast<BYTE*>(header.Code);
        *pCodeSize = header.GetCodeSize();

        if (header.LocalVarSigTok != NULL)
            *pLocalSigToken = header.GetLocalVarSigTok();
        else
            *pLocalSigToken = mdSignatureNil;
    }
#ifdef LOGGING
    else
    {
        LOG((LF_CORDB,LL_INFO100000, "D::GFI: GetMethodImplProps failed!\n"));
    }
#endif
    return hr;
}


/******************************************************************************
 *
 ******************************************************************************/
bool Debugger::ResumeThreads(AppDomain* pAppDomain)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(ThisIsHelperThreadWorker());
    }
    CONTRACTL_END;

    // Okay, mark that we're not stopped anymore and let the
    // Runtime threads go...
    ReleaseAllRuntimeThreads(pAppDomain);

    // Return that we've continued the process.
    return true;
}


class CodeBuffer
{
public:

    BYTE *getCodeBuffer(DebuggerJitInfo *dji)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
        }
        CONTRACTL_END;

        CodeRegionInfo codeRegionInfo = CodeRegionInfo::GetCodeRegionInfo(dji);

        if (codeRegionInfo.getAddrOfColdCode())
        {
            _ASSERTE(codeRegionInfo.getSizeOfHotCode() != 0);
            _ASSERTE(codeRegionInfo.getSizeOfColdCode() != 0);
            S_SIZE_T totalSize = S_SIZE_T( codeRegionInfo.getSizeOfHotCode() ) +
                                                S_SIZE_T( codeRegionInfo.getSizeOfColdCode() );
            if ( totalSize.IsOverflow() )
            {
                _ASSERTE(0 && "Buffer overflow error in getCodeBuffer");
                return NULL;
            }

            BYTE *code = (BYTE *) buffer.AllocNoThrow( totalSize.Value() );
            if (code)
            {
                memcpy(code,
                       (void *) codeRegionInfo.getAddrOfHotCode(),
                       codeRegionInfo.getSizeOfHotCode());

                memcpy(code + codeRegionInfo.getSizeOfHotCode(),
                       (void *) codeRegionInfo.getAddrOfColdCode(),
                       codeRegionInfo.getSizeOfColdCode());

                // Now patch the control transfer instructions
            }

            return code;
        }
        else
        {
            return codeRegionInfo.getAddrOfHotCode();
        }
    }
private:

    CQuickBytes buffer;
};

//
// HandleIPCEvent is called by the RC thread in response to an event
// from the Debugger Interface. No other IPC events, nor any Runtime
// events will come in until this method returns. Returns true if this
// was a Continue event.
//
// If this function is called on native debugger helper thread, we will
// handle everything. However if this is called on managed thread doing
// helper thread duty, we will fail on operation since we are mainly
// waiting for CONTINUE message from the RS.
//
bool Debugger::HandleIPCEvent(DebuggerIPCEvent* event, IpcTarget iWhich)
{
    CONTRACTL
    {
        THROWS;
        if (g_pEEInterface->GetThread() != NULL) { GC_TRIGGERS; } else { GC_NOTRIGGER; }
        PRECONDITION(ThisIsHelperThreadWorker());

        if (m_stopped)
        {
            MODE_COOPERATIVE;
        }
        else
        {
            MODE_ANY;
        }
    }
    CONTRACTL_END;

    // If we're the temporary helper thread, then we may reject certain operations.
    bool temporaryHelp = ThisIsTempHelperThread();


#ifdef _DEBUG
    // Common mistake is to have both a managed & unmanaged debugger attached to the process
    // at the same time (that's what interop is for).
    // Default is to issue a assert warning.
    static int fDbgAllowBoth = -1;
    if (fDbgAllowBoth == -1)
        fDbgAllowBoth = UnsafeGetConfigDWORD(L"DbgAllowBoth", 0);

    if (!fDbgAllowBoth)
    {
        if (IsDebuggerPresent() && !g_pRCThread->GetDCB(iWhich)->m_rightSideIsWin32Debugger)
        {
            _ASSERTE(!"You can't attach a native debugger and a managed debugger to the same process.\n"
                      "Either use Interop debugging; or make sure that the native debugger doesn't steal "
                      "exceptions meant for the managed debugger.\n"
                      "You can ignore this assert; but you're voiding your warranty!\n"
                      "You can supress this assert by setting the COM+ reg key 'DbgAllowBoth' to 1.");
        }
    }

    // This reg key allows us to test our unhandled event filter installed in HandleIPCEventWrapper
    // to make sure it works properly.
    static int fDbgFaultInHandleIPCEvent = -1;
    if (fDbgFaultInHandleIPCEvent == -1)
        fDbgFaultInHandleIPCEvent = UnsafeGetConfigDWORD(L"DbgFaultInHandleIPCEvent", 0);

    // If we need to fault, let's generate an access violation.
    if (fDbgFaultInHandleIPCEvent)
        *((BYTE *)0) = 0;
#endif

    bool ret = false;
    HRESULT hr = S_OK;

    STRESS_LOG1(LF_CORDB, LL_INFO10000, "D::HIPCE: got %s\n", IPCENames::GetName(event->type));
    DbgLog((DebuggerIPCEventType)(event->type & DB_IPCE_TYPE_MASK));

    // As for runtime is considered stopped, it means that managed threads will not
    // execute anymore managed code. However, these threads may be still running for
    // unmanaged code. So it is not true that we do not need to hold the lock while processing
    // synchrnoized event.
    //
    // The worst of all, it is the special case where user break point and exception can
    // be sent as part of attach if debugger was launched by managed app.
    //
    DebuggerLockHolder dbgLockHolder(this, FALSE);
    if ((event->type & DB_IPCE_TYPE_MASK) == DB_IPCE_ASYNC_BREAK ||
        (event->type & DB_IPCE_TYPE_MASK) == DB_IPCE_ATTACHING)
    {
        dbgLockHolder.Acquire();
    }
    else
    {
        _ASSERTE(m_stopped);
        _ASSERTE(ThreadHoldsLock());
    }


    switch (event->type & DB_IPCE_TYPE_MASK)
    {
    case DB_IPCE_ASYNC_BREAK:
        {
            if (temporaryHelp)
            {
                // Don't support async break on temporary helper thread.
                // Well, this function does not return HR. So this means that
                // ASYNC_BREAK event will be swallowing silently while we are
                // doing helper thread duty!
                //
                hr = CORDBG_E_NOTREADY;
            }
            else
            {
                // not synchornized. We get debugger lock upon the function entry
                _ASSERTE(ThreadHoldsLock());

                // Simply trap all Runtime threads if we're not already trying to.
                if (!m_trappingRuntimeThreads)
                {
                    TrapAllRuntimeThreads(&dbgLockHolder, event->appDomainToken.UnWrap());
                }
            }
            break;
        }

    case DB_IPCE_CONTINUE:
        {
            GetCanary()->ClearCache();
        
            // if we receive IPCE_CONTINUE and m_syncingForAttach is != SYNC_STATE_0,
            // we send loaded assembly, modules, classes and started threads, and finally
            // another sync event. We _do_not_ release the threads in this case.

            // Here's how the attach logic works:
            // 1. Set m_syncingForAttach to SYNC_STATE_1
            // 2. Send all CreateAppDomain events to the right side
            // 3. Set m_syncingForAttach to SYNC_STATE_2
            // 4. The right side sends AttachToAppDomain events for every app domain
            //    that it wishes to attach to. Then the right side sends IPCE_CONTINUE
            // 5. Upon receiving IPCE_CONTINUE, m_syncingForAttach is SYNC_STATE_2. This
            //    indicates that we should send all the Load Assembly and Load Module
            //    events to the right side for all the app domains to which the debugger
            //    is attaching.
            // 6. Set m_syncingForAttach to SYNC_STATE_3
            // 7. Upon receiving IPCE_CONTINUE when m_syncingForAttach is in SYNC_STATE_3,
            //    send out all the LoadClass events for all the modules which the right
            //    side is interested in.
            // 8. Set m_syncingForAttach to SYNC_STATE_0. This indicates that the
            //    attach has completed!!
            if (m_syncingForAttach != SYNC_STATE_0)
            {
                _ASSERTE (m_syncingForAttach != SYNC_STATE_1);
                LOG((LF_CORDB, LL_INFO10, "D::HIPCE: Got DB_IPCE_CONTINUE.  Attach state is currently %s\n", g_ppszAttachStateToSZ[m_syncingForAttach]));

                BOOL fAtleastOneEventSent = FALSE;

            syncForAttachRetry:
                if ((m_syncingForAttach == SYNC_STATE_2) ||
                    (m_syncingForAttach == SYNC_STATE_10))
                {
                        hr = IterateAppDomainsForAttach(DONT_SEND_CLASS_EVENTS,
                                                        &fAtleastOneEventSent,
                                                        TRUE);

                        // This is for the case that we're attaching at a point where
                        // only an AppDomain is loaded, so we can't send any
                        // assembly load events but it's valid and so we should just
                        // move on to SYNC_STATE_3 and retry this stuff.  This
                        // happens in particular when we are trying to use the
                        // service to do a synchronous attach at runtime load.
                        if (FAILED(hr) || !fAtleastOneEventSent)
                        {
                            m_syncingForAttach = SYNC_STATE_3;
                            STRESS_LOG1(LF_CORDB, LL_INFO10, "D::HIPCE: Attach state is now %s\n", g_ppszAttachStateToSZ[m_syncingForAttach]);
                            goto syncForAttachRetry;
                        }
                }
                else
                {
                    _ASSERTE ((m_syncingForAttach == SYNC_STATE_3) ||
                              (m_syncingForAttach == SYNC_STATE_11));

                    hr = IterateAppDomainsForAttach(ONLY_SEND_CLASS_EVENTS,
                                                    &fAtleastOneEventSent,
                                                    TRUE);

                    // Send thread attaches...
                    if (m_syncingForAttach == SYNC_STATE_3)
                        hr = g_pEEInterface->IterateThreadsForAttach(
                                                     &fAtleastOneEventSent,
                                                     TRUE);

                    // Change the debug state of all attaching app domains to attached
                    MarkAttachingAppDomainsAsAttachedToDebugger();
                }

                // If we're attaching due to an exception, set
                // exAttachEvent, which will let all excpetion threads
                // go. They will send their events as normal, and will
                // also cause the sync complete to be sent to complete the
                // attach. Therefore, we don't need to do this here.
                if (m_attachingForException &&
                    (m_syncingForAttach == SYNC_STATE_3))
                {
                    STRESS_LOG1(LF_CORDB, LL_INFO10000,
                         "D::HIPCE: Calling SetEvent on m_exAttachEvent= %x\n",
                         GetAttachEvent());
                    // The managed thread that hit Exception or UserBreakPoint
                    // is waiting on this event.
                    //
                    VERIFY(SetEvent(GetAttachEvent()));
                }
                else
                {
                    if (fAtleastOneEventSent == TRUE)
                    {
                        // Send the Sync Complete event next...
                        SendSyncCompleteIPCEvent();
                    }

                    if ((m_syncingForAttach == SYNC_STATE_3) ||
                        (m_syncingForAttach == SYNC_STATE_11))
                    {
                        LOG((LF_CORDB, LL_INFO10000,"D::HIPCE: calling PAL_InitializeDebug.\n"));
                        // Tell the PAL that we're trying to debug
                        PAL_InitializeDebug();

                        // Attach is complete now. This will set m_syncingForAttach to SYNC_STATE_0
                        MarkDebuggerAttachedInternal();
                    }
                    else
                    {
                        _ASSERTE ((m_syncingForAttach == SYNC_STATE_2) ||
                                  (m_syncingForAttach == SYNC_STATE_10));

                        if (m_syncingForAttach == SYNC_STATE_2)
                        {
                            m_syncingForAttach = SYNC_STATE_3;
                            STRESS_LOG1(LF_CORDB, LL_INFO10, "D::HIPCE: Attach state is now %s\n", g_ppszAttachStateToSZ[m_syncingForAttach]);
                        }
                        else
                        {
                            m_syncingForAttach = SYNC_STATE_11;
                            STRESS_LOG1(LF_CORDB, LL_INFO10, "D::HIPCE: Attach state is now %s\n", g_ppszAttachStateToSZ[m_syncingForAttach]);
                        }
                    }

                    if (fAtleastOneEventSent == FALSE)
                        goto LetThreadsGo;
                }
            }
            else
            {
    LetThreadsGo:
                ret = ResumeThreads(event->appDomainToken.UnWrap());

                //
                // Go ahead and release the TSL now that we're continuing. This ensures that we've held
                // the thread store lock the entire time the Runtime was just stopped.
                //
                ThreadStore::UnlockThreadStore(FALSE, GCHeap::SUSPEND_FOR_DEBUGGER);

            }

            break;
        }
    case DB_IPCE_BREAKPOINT_ADD:
        {

            //
            // Currently, we can't create a breakpoint before a
            // function desc is available.
            // Also, we can't know if a breakpoint is ok
            // prior to the method being JITted.
            //

            _ASSERTE(hr == S_OK);
            DebuggerBreakpoint *bp = NULL;

            DebuggerModule *module = event->BreakpointData.funcDebuggerModuleToken.UnWrap();
        
            {
                BOOL fSucceed;
                // If we haven't been either JITted or EnC'd yet, then
                // we'll put a patch in by offset, implicitly relative
                // to the first version of the code.

                bp = new (interopsafe, nothrow) DebuggerBreakpoint(module->GetRuntimeModule(),
                                                                   event->BreakpointData.funcMetadataToken,
                                                                   event->appDomainToken.UnWrap(),
                                                                   event->BreakpointData.offset,
                                                                   !event->BreakpointData.isIL,
                                                                   event->BreakpointData.encVersion,
                                                                   event->BreakpointData.nativeCodeMethodDescToken.UnWrap(),
                                                                   event->BreakpointData.nativeCodeJITInfoToken.UnWrap(),
                                                                   &fSucceed);
                TRACE_ALLOC(bp);
                if (bp != NULL && !fSucceed)
                {
                    DeleteInteropSafe(bp);
                    bp = NULL;
                    hr = CORDBG_E_UNABLE_TO_SET_BREAKPOINT;
                }
            }

            if(NULL == bp && !FAILED(hr))
            {
                hr = E_OUTOFMEMORY;
            }

            LOG((LF_CORDB,LL_INFO10000,"\tBP Add: BPTOK:"
                "0x%x, tok=0x%08x, offset=0x%x, isIL=%d\n", bp,
                 event->BreakpointData.funcMetadataToken,
                 event->BreakpointData.offset,
                 event->BreakpointData.isIL));
        

            //
            // We're using a two-way event here, so we place the
            // result event into the _receive_ buffer, not the send
            // buffer.
            //

            DebuggerIPCEvent *result = m_pRCThread->GetIPCEventReceiveBuffer(
                iWhich);
            InitIPCEvent(result,
                         DB_IPCE_BREAKPOINT_ADD_RESULT,
                         g_pEEInterface->GetThread(),
                         event->appDomainToken);
            result->BreakpointData.breakpointToken.Set(bp);
            result->hr = hr;

            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_STEP:
        {
            LOG((LF_CORDB,LL_INFO10000, "D::HIPCE: stepIn:0x%x frmTok:0x%x"
                "StepIn:0x%x RangeIL:0x%x RangeCount:0x%x MapStop:0x%x "
                "InterceptStop:0x%x AppD:0x%x\n",
                event->StepData.stepIn,
                event->StepData.frameToken.GetSPValue(),
                event->StepData.stepIn,
                event->StepData.rangeIL,
                event->StepData.rangeCount,
                event->StepData.rgfMappingStop,
                event->StepData.rgfInterceptStop,
                event->appDomainToken.UnWrap()));

            Thread *thread = event->StepData.threadToken.UnWrap();
            AppDomain *pAppDomain;
            pAppDomain = event->appDomainToken.UnWrap();

            DebuggerIPCEvent *result = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(result,
                         DB_IPCE_STEP_RESULT,
                         thread,
                         event->appDomainToken);

            if (temporaryHelp)
            {
                // Can't step on the temporary helper thread.
                result->hr = CORDBG_E_NOTREADY;
            }
            else
            {
                DebuggerStepper *stepper = event->StepData.IsJMCStop ?
                    new (interopsafe, nothrow) DebuggerJMCStepper(thread,
                                                                  event->StepData.rgfMappingStop,
                                                                  event->StepData.rgfInterceptStop,
                                                                  pAppDomain) :
                    new (interopsafe, nothrow) DebuggerStepper(thread,
                                                               event->StepData.rgfMappingStop,
                                                               event->StepData.rgfInterceptStop,
                                                               pAppDomain);

                if (stepper == NULL)
                {
                    result->hr = E_OUTOFMEMORY;

                    m_pRCThread->SendIPCReply(iWhich);

                    break;
                }
                TRACE_ALLOC(stepper);

                unsigned int cRanges = event->StepData.totalRangeCount;
                _ASSERTE(cRanges == 0 || (cRanges > 0 && cRanges == event->StepData.rangeCount));

                if (!stepper->Step(event->StepData.frameToken,
                                   event->StepData.stepIn,
                                   &(event->StepData.range),
                                   cRanges,
                                   (cRanges > 0 ? event->StepData.rangeIL : FALSE)))
                {
                    result->hr = E_OUTOFMEMORY;

                    m_pRCThread->SendIPCReply(iWhich);

                    DeleteInteropSafe(stepper);
                    break;
                }

                result->StepData.stepperToken.Set(stepper);


            } // end normal step case.


            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_STEP_OUT:
        {
            Thread *thread = event->StepData.threadToken.UnWrap();
            AppDomain *pAppDomain;
            pAppDomain = event->appDomainToken.UnWrap();

            DebuggerIPCEvent *result = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(result,
                         DB_IPCE_STEP_RESULT,
                         thread,
                         pAppDomain);

            if (temporaryHelp)
            {
                // Can't step on the temporary helper thread.
                result->hr = CORDBG_E_NOTREADY;
            }
            else
            {
                DebuggerStepper *stepper = event->StepData.IsJMCStop ?
                    new (interopsafe, nothrow) DebuggerJMCStepper(thread,
                                                                  event->StepData.rgfMappingStop,
                                                                  event->StepData.rgfInterceptStop,
                                                                  pAppDomain) :
                    new (interopsafe, nothrow) DebuggerStepper(thread,
                                                               event->StepData.rgfMappingStop,
                                                               event->StepData.rgfInterceptStop,
                                                               pAppDomain);


                if (stepper == NULL)
                {
                    result->hr = E_OUTOFMEMORY;
                    m_pRCThread->SendIPCReply(iWhich);

                    break;
                }

                TRACE_ALLOC(stepper);

                // Safe to stack trace b/c we're stopped.
                StackTraceTicket ticket(thread);
                stepper->StepOut(event->StepData.frameToken, ticket);

                result->StepData.stepperToken.Set(stepper);
            }

            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_BREAKPOINT_REMOVE:
        {

            DebuggerBreakpoint *bp = event->BreakpointData.breakpointToken.UnWrap();

            bp->Delete();
        }
        break;

    case DB_IPCE_STEP_CANCEL:
        {
            LOG((LF_CORDB,LL_INFO10000, "D:HIPCE:Got STEP_CANCEL for stepper "
                "0x%p\n",event->StepData.stepperToken.UnWrap()));
            DebuggerStepper *stepper = event->StepData.stepperToken.UnWrap();

            stepper->Delete();
        }
        break;

    case DB_IPCE_STACK_TRACE:
        {
            Thread* thread = event->StackTraceData.debuggerThreadToken.UnWrap();

            //
            //
            LOG((LF_CORDB,LL_INFO1000, "Stack trace to :iWhich:0x%x\n",iWhich));

            hr =
                DebuggerThread::TraceAndSendStack(thread, m_pRCThread, iWhich);
        }
        break;

    case DB_IPCE_SET_DEBUG_STATE:
        {
            Thread* thread = event->SetDebugState.debuggerThreadToken.UnWrap();
            CorDebugThreadState debugState = event->SetDebugState.debugState;

            LOG((LF_CORDB,LL_INFO10000,"HandleIPCE:SetDebugState: thread 0x%x (ID:0x%x) to state 0x%x\n",
                thread,GetThreadIdHelper(thread), debugState));

            g_pEEInterface->SetDebugState(thread, debugState);

            STRESS_LOG1(LF_CORDB,LL_INFO10000,"HandleIPC: Got 0x%x back from SetDebugState\n", hr);

            DebuggerIPCEvent *result = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(result, DB_IPCE_SET_DEBUG_STATE_RESULT, NULL, NULL);
            result->hr = S_OK;

            m_pRCThread->SendIPCReply(iWhich);

        }
        break;

    case DB_IPCE_SET_ALL_DEBUG_STATE:
        {
            Thread* et = event->SetAllDebugState.debuggerExceptThreadToken.UnWrap();
            CorDebugThreadState debugState = event->SetDebugState.debugState;

            LOG((LF_CORDB,LL_INFO10000,"HandleIPCE: SetAllDebugState: except thread 0x%08x (ID:0x%x) to state 0x%x\n",
                et, et != NULL ? GetThreadIdHelper(et) : 0, debugState));

            if (!g_fProcessDetach)
                g_pEEInterface->SetAllDebugState(et, debugState);

            STRESS_LOG1(LF_CORDB,LL_INFO10000,"HandleIPC: Got 0x%x back from SetAllDebugState\n", hr);

            // Just send back an HR.
            DebuggerIPCEvent *result = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            PREFIX_ASSUME(result != NULL);
            InitIPCEvent(result, DB_IPCE_SET_DEBUG_STATE_RESULT, NULL, NULL);
            result->hr = S_OK;
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_GET_FUNCTION_DATA:
        {
            //
            //
            hr = GetAndSendFunctionData(
                               m_pRCThread,
                               event->GetFunctionData.funcMetadataToken,
                               event->GetFunctionData.funcDebuggerModuleToken.UnWrap(),
                               event->GetFunctionData.nVersion,
                               iWhich);
        }
        break;


    case DB_IPCE_GET_GCHANDLE_INFO:
        // Given an unvalidated GC-handle, find out all the info about it to view the object
        // at the other end
        {
            OBJECTHANDLE oh = event->GetGCHandleInfo.GCHandle.UnWrap();

            DebuggerIPCEvent *result = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            PREFIX_ASSUME(result != NULL);
            InitIPCEvent(result, DB_IPCE_GET_GCHANDLE_INFO_RESULT, NULL, NULL);

            bool fValid = SUCCEEDED(ValidateGCHandle(oh));

            AppDomain * pAppDomain = NULL;
            if(fValid)
            {
                // Get the appdomain
                ADIndex idx = HndGetHandleADIndex(oh);
                pAppDomain = SystemDomain::GetAppDomainAtIndex(idx);

                _ASSERTE(pAppDomain != NULL);
            }

            result->hr = S_OK;
            result->GetGCHandleInfoResult.pLSAppDomain.Set(pAppDomain);
            result->GetGCHandleInfoResult.fValid = fValid;

            m_pRCThread->SendIPCReply(iWhich);

        }
        break;

    case DB_IPCE_GET_OBJECT_INFO:
        {
            //
            //
            hr = GetAndSendObjectInfo(
                               m_pRCThread,
                               event->appDomainToken.UnWrap(),
                               event->GetObjectInfo.objectRefAddress,
                               event->GetObjectInfo.objectHandle.UnWrap(),
                               event->GetObjectInfo.objectRefIsValue,
                               event->GetObjectInfo.objectType,
                               iWhich);
        }
        break;

    case DB_IPCE_GET_CLASS_INFO:
        {
            //
            //
            TypeHandle thExact;
            TypeHandle thApprox;
            bool fInstantiatedType = false;

            DebuggerModule *pDebuggerModule = (DebuggerModule*) event->GetClassInfo.debuggerModuleToken.UnWrap();

            if (pDebuggerModule != NULL)
            {
                _ASSERTE(event->GetClassInfo.typeHandleExact == NULL);
                _ASSERTE(event->GetClassInfo.typeHandleApprox == NULL);

                // Find the class given its module and token. The class must be loaded.
                LOG((LF_CORDB, LL_INFO10000, "D::GASCI: getting info for 0x%08x 0x%0x8.\n",
                    pDebuggerModule, event->GetClassInfo.metadataToken));

                thExact = g_pEEInterface->FindLoadedClass(pDebuggerModule->GetRuntimeModule(), event->GetClassInfo.metadataToken);
                thApprox = thExact;
            }
            else if (event->GetClassInfo.typeHandleExact != NULL || event->GetClassInfo.typeHandleApprox != NULL)
            {
                _ASSERTE(pDebuggerModule == NULL);

                LOG((LF_CORDB, LL_INFO10000, "D::GASCI: getting info for type handle 0x%08x with approx 0x%08x.\n",
                    LsPtrToCookie(event->GetClassInfo.typeHandleExact), LsPtrToCookie(event->GetClassInfo.typeHandleApprox)));

                thExact = GetTypeHandle(event->GetClassInfo.typeHandleExact);
                thApprox = GetTypeHandle(event->GetClassInfo.typeHandleApprox);
                fInstantiatedType = true;
            }

            hr = GetAndSendClassInfo(
                               m_pRCThread,
                               thExact,
                               thApprox,
                               fInstantiatedType,
                               event->appDomainToken.UnWrap(),
                               mdFieldDefNil,
                               NULL,
                               iWhich);
        }
        break;

    case DB_IPCE_GET_SIMPLE_TYPE:
        {
            //
            //

            AppDomain *pAppDomain = event->appDomainToken.UnWrap();
            TypeHandle r = g_pEEInterface->FindLoadedElementType(event->GetSimpleType);

            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(event,
                         DB_IPCE_GET_SIMPLE_TYPE_RESULT,
                         0,
                         pAppDomain);


            if (r.IsNull())
                event->hr = CORDBG_E_CLASS_NOT_LOADED;
            else
            {
                event->GetSimpleTypeResult.metadataToken = r.GetCl();
                DebuggerModule * pDModule = LookupModule(r.GetModule(), pAppDomain);
                event->GetSimpleTypeResult.debuggerModuleToken.Set(pDModule);
                event->hr = S_OK;
            }
            LOG((LF_CORDB, LL_INFO10000, "D::STI: sending result.\n"));

            // Send off the data to the right side.
            hr = m_pRCThread->SendIPCReply(iWhich);
        }

        break;

    case DB_IPCE_GET_TYPE_HANDLE_PARAMS:
        {
            hr = GetAndSendTypeHandleParams(
                               m_pRCThread,
                               event->appDomainToken.UnWrap(),
                               GetTypeHandle(event->GetTypeHandleParams.typeHandle),
                               iWhich);
        }
        break;

    case DB_IPCE_GET_EXPANDED_TYPE_INFO:
        {
            hr = GetAndSendExpandedTypeInfo(
                               m_pRCThread,
                               event->appDomainToken.UnWrap(),
                               GetTypeHandle(event->ExpandType.typeHandle),
                               iWhich);
        }
        break;

    case DB_IPCE_GET_APPROX_TYPE_HANDLE:
        {
            hr = GetAndSendApproxTypeHandle(
                               m_pRCThread,
                               event->appDomainToken.UnWrap(),
                               event->GetApproxTypeHandle.typeDataNodeCount,
                               (DebuggerIPCE_TypeArgData *) event->GetApproxTypeHandle.typeDataBuffer,
                               iWhich);
        }
        break;

    case DB_IPCE_GET_TYPE_HANDLE:
        {
            hr = GetAndSendTypeHandle(
                               m_pRCThread,
                               event->appDomainToken.UnWrap(),
                               &event->GetTypeHandle.typeData,
                               event->GetTypeHandle.genericArgsCount,
                               (DebuggerIPCE_BasicTypeData *) event->GetTypeHandle.genericArgsBuffer,
                               iWhich);
        }
        break;

    case DB_IPCE_GET_METHOD_DESC_PARAMS:
        {
            hr = GetAndSendMethodDescParams(
                               m_pRCThread,
                               event->appDomainToken.UnWrap(),
                               event->GetMethodDescParams.methodDesc.UnWrap(),
                               event->GetMethodDescParams.exactGenericArgsToken,
                               iWhich);
        }
        break;

    case DB_IPCE_GET_SPECIAL_STATIC:
        {
            hr = GetAndSendSpecialStaticInfo(
                               m_pRCThread,
                               event->GetSpecialStatic.fldDebuggerToken.UnWrap(),
                               event->GetSpecialStatic.debuggerThreadToken.UnWrap(),
                               iWhich);
        }
        break;

    case DB_IPCE_GET_JIT_INFO:
        {
            //
            //

            hr = GetAndSendJITInfo(
                               m_pRCThread,
                               event->GetJITInfo.nativeCodeJITInfoToken.UnWrap(),
                               event->appDomainToken.UnWrap(),
                               iWhich);
        }
        break;

    case DB_IPCE_GET_BUFFER:
        {
            GetAndSendBuffer(m_pRCThread, event->GetBuffer.bufSize);
        }
        break;

    case DB_IPCE_RELEASE_BUFFER:
        {
            SendReleaseBuffer(m_pRCThread, (BYTE *)event->ReleaseBuffer.pBuffer);
        }
        break;

    case DB_IPCE_SET_CLASS_LOAD_FLAG:
        {
            DebuggerModule *pModule =
                    event->SetClassLoad.debuggerModuleToken.UnWrap();
            _ASSERTE(pModule != NULL);

            
            LOG((LF_CORDB, LL_INFO10000,
                 "D::HIPCE: class load flag is %d for module 0x%p\n",
                 event->SetClassLoad.flag, pModule));

            pModule->EnableClassLoadCallbacks((BOOL)event->SetClassLoad.flag);            
        }
        break;

    case DB_IPCE_ATTACHING:
        {
            // Perform some initialization necessary for debugging
            LOG((LF_CORDB,LL_INFO10000, "D::HIPCE: Attach begins!\n"));

            // We should never attach using the temporary helper thread.
            // Temp. only exists when we're already debugging.
            _ASSERTE(!temporaryHelp);

            // It's possible that a debugger may be involved before this point.
            // For example, we may have been launched by a debugger, and then
            // someone attached to us.

            // Remember that we're attaching now...
            m_syncingForAttach = SYNC_STATE_1;
            _ASSERTE(DebuggerIsInvolved());

            LazyInit();
            DebuggerController::Initialize();


            LOG((LF_CORDB, LL_INFO10000, "Attach state is now %s\n", g_ppszAttachStateToSZ[m_syncingForAttach]));

            // Simply trap all Runtime threads...
            // This is an 'attach to process' msg, so it's not
            // unreasonable that we stop all the appdomains
            if (!m_trappingRuntimeThreads)
            {


                {
                    if (m_pAppDomainCB->Lock())
                    {
                        AppDomainInfo *pADInfo = m_pAppDomainCB->FindFirst();
                        if (pADInfo == NULL)
                        {
                            STRESS_LOG0(LF_CORDB, LL_INFO10000, "No ADs, degenerating to early-attach case.\n");

                            // Mark as attached. Nothing's created yet, so we just treat this like a Create-case.
                            // Note that this should be a very very uncommon code-path. You need to attach very early,
                            // after the SSE is created, but before the 1st AD is created.
                            MarkDebuggerAttachedInternal();

                            m_pAppDomainCB->Unlock();


                            // We may have setup some threads before the appdomains.
                            // So even if we don't have any appdomains, we may have already created a managed thread
                            // (via SetupThread) and thus missed the window to create a thread-starter.
                            // Plug that hole here.
                            {
                                ThreadStore::LockThreadStore(GCHeap::SUSPEND_FOR_DEBUGGER);

                                Thread *pThread = NULL;
                                while ((pThread = ThreadStore::GetThreadList(pThread)) != NULL)
                                {
                                    Thread::ThreadState ts = pThread->GetSnapshotState();
                                    // If the thread is Unstarted, then don't bother doing the thread-create
                                    // b/c SetupThread will create it. If we're not unstarted, then we've
                                    // already missed the window in SetupThread
                                    if (!(ts & Thread::TS_Unstarted))
                                    {
                                        this->ThreadCreated(pThread);
                                    }
                                }
                                ThreadStore::UnlockThreadStore(FALSE, GCHeap::SUSPEND_FOR_DEBUGGER);
                            }

                            // Signal to any jit-attach thread that we're now attached.
                            VERIFY(SetEvent(GetAttachEvent()));
                            goto Label_DoneAttach;
                        }
                        else
                        {
                            STRESS_LOG0(LF_CORDB, LL_INFO10000, "Attaching w/ at least 1 AD, doing normal attach case.\n");
                            m_pAppDomainCB->Unlock();
                        }
                    }
                    else
                    {
                        STRESS_LOG0(LF_CORDB, LL_INFO10000, "Attaching - failed to get ADCB lock - possible deadlock if no ADs.\n");
                    }
                }

                TrapAllRuntimeThreads(&dbgLockHolder, NULL);

            Label_DoneAttach:
                ;
            }

            break;
        }
    case DB_IPCE_IS_TRANSITION_STUB:
        GetAndSendTransitionStubInfo((CORDB_ADDRESS_TYPE*)event->IsTransitionStub.address,
                                     iWhich);
        break;

    case DB_IPCE_MODIFY_LOGSWITCH:
        g_pEEInterface->DebuggerModifyingLogSwitch (
                            event->LogSwitchSettingMessage.iLevel,
                            event->LogSwitchSettingMessage.szSwitchName.GetString());

        break;

    case DB_IPCE_ENABLE_LOG_MESSAGES:
        {
            bool fOnOff = event->LogSwitchSettingMessage.iLevel ? true:false;
            EnableLogMessages (fOnOff);
        }
        break;

    case DB_IPCE_SET_IP:

            // This is a synchronous event (reply required)
            _ASSERTE( event->SetIP.firstExceptionHandler != NULL);

            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);

            // Don't have an explicit reply msg
            InitIPCEventWithThreadId(event,
                         DB_IPCE_SET_IP,
                         event->threadId,
                         event->appDomainToken);

            if (temporaryHelp)
            {
                event->hr = CORDBG_E_NOTREADY;
            }
            else if (!g_fProcessDetach)
            {
                //
                // Since this pointer is coming from the RS, it may be NULL or something
                // unexpected in an OOM situation.  Quickly just sanity check them.
                //
                DebuggerJitInfo *pDji = event->SetIP.nativeCodeJITInfoToken.UnWrap();
                Thread *pThread = event->SetIP.debuggerThreadToken.UnWrap();
                Module *pModule = event->SetIP.debuggerModule.UnWrap()->GetRuntimeModule();

                if ((pDji != NULL) && (pThread != NULL) && (pModule != NULL))
                {
                    CHECK_IF_CAN_TAKE_HELPER_LOCKS_IN_THIS_SCOPE(&(event->hr), GetCanary());
                    if (SUCCEEDED(event->hr))                    
                    {
                        event->hr = SetIP(event->SetIP.fCanSetIPOnly,
                                          pThread,
                                          pModule,
                                          event->SetIP.mdMethod,
                                          pDji,
                                          event->SetIP.offset,
                                          event->SetIP.fIsIL,
                                          event->SetIP.firstExceptionHandler);
                    }
                }
                else
                {
                    hr = E_INVALIDARG;
                }
            }
            else
            {
                event->hr = S_OK;
            }
            // Send the result
            m_pRCThread->SendIPCReply(iWhich);
        break;

    case DB_IPCE_ATTACH_TO_APP_DOMAIN:

        // Mark that we need to attach to a specific app domain (state
        // 10), but only if we're not already attaching to the process
        // as a whole (state 2).
        if (m_syncingForAttach != SYNC_STATE_2)
        {
            // We'd like to get rid of these intermediate states, but this is gets hit in the normal
            // launch case (b/c RS still does AttachToAppdomain)
            //_ASSERTE(!"Per-Appdomain debugging Not supported");
            m_syncingForAttach = SYNC_STATE_10;
            LOG((LF_CORDB, LL_INFO10000, "Attach state is now %s\n", g_ppszAttachStateToSZ[m_syncingForAttach]));
        }

        // Since we never do just per-appdomain debugging, we should already be stopped
        // when we get this event.
        _ASSERTE(IsStopped());
        _ASSERTE(m_trappingRuntimeThreads);

        // Simply trap all Runtime threads...
        if (!m_trappingRuntimeThreads)
        {
            // how can we are stopping and not having trappingRuntimeThreads set to true??
            _ASSERTE(!"We should not have reach here!");
        }
        event->hr = AttachDebuggerToAppDomain(event->AppDomainData.id);
        break;

    case DB_IPCE_DETACH_FROM_APP_DOMAIN:
        {
            AppDomain *ad;

            hr = DetachDebuggerFromAppDomain(event->AppDomainData.id, &ad);
            if (FAILED(hr) )
            {
                event->hr = hr;
                break;
            }

            if (ad != NULL)
            {
                LOG((LF_CORDB, LL_INFO10000, "Detaching from AppD:0x%x\n", ad));

                ClearAppDomainPatches(ad);
            }
        }
        break;

    case DB_IPCE_DETACH_FROM_PROCESS:
        LOG((LF_CORDB, LL_INFO10000, "Detaching from process!\n"));

        // See EnsureDebuggerAttached for why we reset this here.
        // We reset it here because detach is the longest possible time
        // after attach, and that makes the window really small.
        VERIFY(ResetEvent(GetAttachEvent()));

        // At this point, all patches should have been removed
        // by detaching from the appdomains.

        // Commented out for hotfix bug 94625.  Should be re-enabled at some point.
        //_ASSERTE(DebuggerController::GetNumberOfPatches() == 0);
        MarkDebuggerUnattachedInternal();


        // Need to close it before we recreate it.
        if (m_pRCThread->m_SetupSyncEvent == NULL)
        {
            hr = S_OK;

            // this will throw on major failures (oom)....
            m_pRCThread->CreateSetupSyncEvent();

            if (FAILED(hr))
            {
                event->hr = hr;
                break;
            }
        }

        VERIFY(SetEvent(m_pRCThread->m_SetupSyncEvent));

        m_pRCThread->RightSideDetach();


        // Clear JMC status
        {
            LOG((LF_CORDB, LL_EVERYTHING, "Setting all JMC methods to false:\n"));
            // On detach, set all DMI's JMC status to false.
            // We have to do this b/c we clear the DebuggerModules and allocated
            // new ones on re-attach; and the DMI & DM need to be in sync
            // (in this case, agreeing that JMC-status = false).
            // This also syncs the EE modules and disables all JMC probes.
            DebuggerMethodInfoTable * pTable = g_pDebugger->GetMethodInfoTable();

            if (pTable != NULL)
            {
                HASHFIND info;
                DebuggerDataLockHolder debuggerDataLockHolder(this);

                for (DebuggerMethodInfo *dmi = pTable->GetFirstMethodInfo(&info);
                    dmi != NULL;
                    dmi = pTable->GetNextMethodInfo(&info))
                {
                    dmi->SetJMCStatus(false);
                }
            }
            LOG((LF_CORDB, LL_EVERYTHING, "Done clearing JMC methods!\n"));
        }

        // Clean up the hash of DebuggerModules
        // This method is overridden to also free all DebuggerModule objects
        if (m_pModules != NULL)
        {

            // Removes all DebuggerModules
            DebuggerDataLockHolder ch(this);
            m_pModules->Clear();

        }

        // Reply to the detach message before we release any Runtime threads. This ensures that the debugger will get
        // the detach reply before the process exits if the main thread is near exiting.
        m_pRCThread->SendIPCReply(iWhich);

        // Let the process run free now... there is no debugger to bother it anymore.
        ret = ResumeThreads(NULL);

        //
        // Go ahead and release the TSL now that we're continuing. This ensures that we've held
        // the thread store lock the entire time the Runtime was just stopped.
        //
        ThreadStore::UnlockThreadStore(FALSE, GCHeap::SUSPEND_FOR_DEBUGGER);
        break;

#ifndef DACCESS_COMPILE

    case DB_IPCE_FUNC_EVAL:
        {
            // This is a synchronous event (reply required)
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            Thread *pThread;
            pThread = event->FuncEval.funcDebuggerThreadToken.UnWrap();

            InitIPCEvent(event, DB_IPCE_FUNC_EVAL_SETUP_RESULT, pThread, pThread->GetDomain());

            BYTE *argDataArea = NULL;
            DebuggerEval *debuggerEvalKey = NULL;

            event->hr = FuncEvalSetup(&(event->FuncEval), &argDataArea, &debuggerEvalKey);

            // Send the result of how the func eval setup went.
            event->FuncEvalSetupComplete.argDataArea = argDataArea;
            event->FuncEvalSetupComplete.debuggerEvalKey.Set(debuggerEvalKey);

            m_pRCThread->SendIPCReply(iWhich);
        }

        break;

#endif

    case DB_IPCE_SET_REFERENCE:
        {
            // This is a synchronous event (reply required)
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEventWithThreadId(event,
                         DB_IPCE_SET_REFERENCE_RESULT,
                         event->threadId,
                         event->appDomainToken);

            event->hr = SetReference(event->SetReference.objectRefAddress,
                                     event->SetReference.objectHandle,
                                     event->SetReference.newReference);

            // Send the result of how the set reference went.
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_SET_VALUE_CLASS:
        {
            // This is a synchronous event (reply required)
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);

            InitIPCEventWithThreadId(event, DB_IPCE_SET_VALUE_CLASS_RESULT, event->threadId, event->appDomainToken);

            event->hr = SetValueClass(event->SetValueClass.oldData,
                                      event->SetValueClass.newData,
                                      &event->SetValueClass.type);

            // Send the result of how the set reference went.
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;


    case DB_IPCE_GET_TASKID:
        {
            Thread *pThread = event->GetTaskId.debuggerThreadToken.UnWrap();

            // This is a synchronous event (reply required)
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEventWithThreadId(event,
                         DB_IPCE_GET_TASKID_RESULT,
                         event->threadId, // ==> don't change it.
                         event->appDomainToken);

            event->GetTaskIdResult.taskId = pThread->GetTaskId();
            event->hr = S_OK;
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_GET_CONNECTIONID:
        {
            Thread *pThread = event->GetConnectionId.debuggerThreadToken.UnWrap();

            // This is a synchronous event (reply required)
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEventWithThreadId(event,
                         DB_IPCE_GET_CONNECTIONID_RESULT,
                         event->threadId, // ==> don't change it.
                         event->appDomainToken);

            event->GetConnectionIdResult.connectionId = pThread->GetConnectionId();
            event->hr = S_OK;
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_GET_OSTHREADID:
        {
            Thread *pThread = event->GetOSThreadId.debuggerThreadToken.UnWrap();

            // This is a synchronous event (reply required)
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEventWithThreadId(event,
                         DB_IPCE_GET_OSTHREADID_RESULT,
                         event->threadId, // ==> don't change it.
                         event->appDomainToken);

            event->GetOSThreadIdResult.osThreadId = pThread->GetOSThreadIdForDebugger();
            event->hr = S_OK;
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_GET_THREADHANDLE:
        {
            Thread *pThread = event->GetThreadHandle.debuggerThreadToken.UnWrap();

            // This is a synchronous event (reply required)
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEventWithThreadId(event,
                         DB_IPCE_GET_THREADHANDLE_RESULT,
                         event->threadId, // ==> don't change it.
                         event->appDomainToken);

            event->GetThreadHandleResult.handle = pThread->GetThreadHandle();
            event->hr = S_OK;
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_GET_THREAD_FOR_TASKID:
        {
             TASKID taskid = event->GetThreadForTaskId.taskid;
             Thread *pThread = ThreadStore::GetThreadList(NULL);
             Thread *pThreadRet = NULL;
             while (pThread != NULL)
             {
                 if (pThread->GetTaskId() == taskid)
                 {
                     pThreadRet = pThread;
                     break;
                 }
                 pThread = ThreadStore::GetThreadList(pThread);
             }

             // This is a synchronous event (reply required)
             event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
             InitIPCEventWithThreadId(event,
                          DB_IPCE_GET_THREAD_FOR_TASKID_RESULT,
                          event->threadId, // ==> don't change it.
                          event->appDomainToken);

             event->GetThreadForTaskIdResult.debuggerThreadToken.Set(pThreadRet);
             event->hr = S_OK;
             m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_CREATE_HANDLE:
        {
             Object *pObject = (Object*)event->CreateHandle.objectToken;
             OBJECTREF objref = ObjectToOBJECTREF(pObject);
             AppDomain *pAppDomain = event->appDomainToken.UnWrap();
             BOOL   fStrong = event->CreateHandle.fStrong;
             OBJECTHANDLE oh;

             // This is a synchronous event (reply required)
             event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
             InitIPCEventWithThreadId(event,
                          DB_IPCE_CREATE_HANDLE_RESULT,
                          event->threadId, // ==> don't change it.
                          event->appDomainToken);

             {
                 // Handle creation may need to allocate memory.
                 // The API specifically limits the number of handls Cordbg can create,
                 // so we could preallocate and fail allocating anything beyond that.
                 CHECK_IF_CAN_TAKE_HELPER_LOCKS_IN_THIS_SCOPE(&(event->hr), GetCanary());
                 if (SUCCEEDED(event->hr))
                 {
                     if (fStrong == TRUE)
                     {
                         // create strong handle
                         oh = pAppDomain->CreateStrongHandle(objref);
                     }
                     else
                     {
                         // create the weak long handle
                         oh = pAppDomain->CreateLongWeakHandle(objref);
                     }
                     event->CreateHandleResult.objectHandle.Set(oh);
                 }
             }
             
             m_pRCThread->SendIPCReply(iWhich);
             break;
        }

    case DB_IPCE_DISPOSE_HANDLE:
        {
            // DISPOSE an object handle
            OBJECTHANDLE oh = event->DisposeHandle.objectHandle.UnWrap();
            if (event->DisposeHandle.fStrong == TRUE)
            {
                DestroyStrongHandle(oh);
            }
            else
            {
                DestroyLongWeakHandle(oh);
            }
            break;
        }

    case DB_IPCE_RESOLVE_ASSEMBLY:
        {
            DebuggerModule *pRefingDebuggerModule = event->ResolveAssembly.refingModuleToken.UnWrap();
            Module      *pRefingModule = pRefingDebuggerModule->GetRuntimeModule();
            Assembly    *pAssembly = NULL;
            mdAssemblyRef tk = event->ResolveAssembly.assemblyRefToken;

            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEventWithThreadId(event,
                         DB_IPCE_RESOLVE_ASSEMBLY_RESULT,
                         event->threadId, // ==> don't change it.
                         event->appDomainToken);

            pAssembly = pRefingModule->LookupAssemblyRef(tk);
            // domainAssembly = pAppDomain->FindCachedAssembly(&spec);

            // This is a synchronous event (reply required)
            event->ResolveAssemblyResult.debuggerAssemblyToken.Set(pAssembly);
            event->hr = S_OK;
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_RESOLVE_TYPEREF:
        {
            DebuggerModule *pRefingDebuggerModule = event->ResolveTypeRef.refingModuleToken.UnWrap();
            Module      *pRefingModule = pRefingDebuggerModule->GetRuntimeModule();

            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEventWithThreadId(event,
                         DB_IPCE_RESOLVE_TYPEREF_RESULT,
                         event->threadId, 
                         event->appDomainToken);

            // Resolve the type ref
            // g_pEEInterface->FindLoadedClass is almost what we want, but it isn't gauranteed to work if
            // the typeRef was originally loaded from a different assembly.  Also, we need to ensure that
            // we can resolve even unloaded types in fully loaded assemblies, so APIs such as 
            // LoadTypeDefOrRefThrowing aren't acceptable.
            Module* targetModule = NULL;
            mdTypeDef targetTypeDef = mdTokenNil;
            BOOL success = FALSE;
            {
                EX_TRY
                {
                    // The loader won't need to trigger a GC or throw because we've told it not to load anything
                    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();
                    success = ClassLoader::ResolveTokenToTypeDefThrowing( 
                        pRefingModule,
                        event->ResolveTypeRef.typeRefToken,
                        &targetModule,
                        &targetTypeDef,
                        Loader::SafeLookup   //don't load, no locks/allocations
                        );
                }
                EX_CATCH
                {
                    // We don't expect any failures here.
                    _ASSERTE(!"ResolveTokenToTypeDefThrowing failed even though we told it not to load anything");
                }
                EX_END_CATCH(RethrowTerminalExceptions);
            }

            if( success )
            {
                _ASSERTE( targetModule != NULL );
                _ASSERTE( TypeFromToken(targetTypeDef) == mdtTypeDef );

                DebuggerModule* targetDModule = LookupModule( targetModule, pRefingDebuggerModule->GetAppDomain() );
                _ASSERTE( targetDModule != NULL );

                event->ResolveTypeRefResult.debuggerModuleToken.Set( targetDModule );
                event->ResolveTypeRefResult.typeDefToken = targetTypeDef;
                event->hr = S_OK;
            }
            else
            {
                // failed - presumably because the target assembly isn't loaded                        
                event->hr = CORDBG_E_CLASS_NOT_LOADED;
            }

            // send the response
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

#ifndef DACCESS_COMPILE

    case DB_IPCE_FUNC_EVAL_ABORT:
        {
            LOG((LF_CORDB, LL_INFO1000, "D::HIPCE: Got FuncEvalAbort for pDE:%08x\n",
                event->FuncEvalAbort.debuggerEvalKey.UnWrap()));

            // This is a synchronous event (reply required)

            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEventWithThreadId(event,
                         DB_IPCE_FUNC_EVAL_ABORT_RESULT,
                         event->threadId,
                         event->appDomainToken);

            event->hr = FuncEvalAbort(event->FuncEvalAbort.debuggerEvalKey.UnWrap());

            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_FUNC_EVAL_RUDE_ABORT:
        {
            LOG((LF_CORDB, LL_INFO1000, "D::HIPCE: Got FuncEvalRudeAbort for pDE:%08x\n",
                event->FuncEvalRudeAbort.debuggerEvalKey.UnWrap()));

            // This is a synchronous event (reply required)

            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEventWithThreadId(event,
                                     DB_IPCE_FUNC_EVAL_RUDE_ABORT_RESULT,
                                     event->threadId,
                                     event->appDomainToken
                                    );

            event->hr = FuncEvalRudeAbort(event->FuncEvalRudeAbort.debuggerEvalKey.UnWrap());

            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_FUNC_EVAL_CLEANUP:

        // This is a synchronous event (reply required)

        event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
        InitIPCEventWithThreadId(event,
                     DB_IPCE_FUNC_EVAL_CLEANUP_RESULT,
                     event->threadId,
                                 event->appDomainToken
                                );

        event->hr = FuncEvalCleanup(event->FuncEvalCleanup.debuggerEvalKey.UnWrap());

        m_pRCThread->SendIPCReply(iWhich);

        break;

#endif

    case DB_IPCE_GET_THREAD_OBJECT:
        {
            // This is a synchronous event (reply required)
            Thread *pRuntimeThread =
                event->ObjectRef.debuggerObjectToken.UnWrap();
            _ASSERTE(pRuntimeThread != NULL);

            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(event,
                         DB_IPCE_THREAD_OBJECT_RESULT,
                         NULL,
                         pRuntimeThread->GetDomain());

            Thread::ThreadState ts = pRuntimeThread->GetSnapshotState();

            if ((ts & Thread::TS_Dead) ||
                (ts & Thread::TS_Unstarted) ||
                (ts & Thread::TS_Detached) ||
                g_fProcessDetach)
            {
                event->hr =  CORDBG_E_BAD_THREAD_STATE;
            }
            else
            {
                event->ObjectRef.managedObject.Set(
                    pRuntimeThread->GetExposedObjectHandleForDebugger());

                event->hr = S_OK;
            }
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_CHANGE_JIT_DEBUG_INFO:
        {
            Module *module = NULL;
            DWORD dwBits = 0;
            DebuggerModule *deModule = event->JitDebugInfo.debuggerModuleToken.UnWrap();

            if (IsAttachInProgress())
            {
                hr = CORDBG_E_CANNOT_BE_ON_ATTACH;
            }
            else
            {
                module = deModule->GetRuntimeModule();
                _ASSERTE(NULL != module);

                //
                // Initialize dwBits to all the values that we cannot set via this IPC event.
                // We ignore  DACF_TRACK_JIT_INFO because that's now always on.
                //
                dwBits = (module->GetDebuggerInfoBits() &
                          ~(DACF_ALLOW_JIT_OPTS | DACF_ENC_ENABLED));
                dwBits &= DACF_CONTROL_FLAGS_MASK;

                if (event->JitDebugInfo.fAllowJitOpts)
                {
                    dwBits |= DACF_ALLOW_JIT_OPTS;
                }

                // Settings from the debugger take precedence over all
                // other settings.
                dwBits |= DACF_USER_OVERRIDE;
            }

            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(event, DB_IPCE_CHANGE_JIT_INFO_RESULT, NULL, NULL);

            if (FAILED(hr))
            {
                event->hr = hr;
            }
            else
            {
                _ASSERTE(module != NULL);
                {
                    module->SetDebuggerInfoBits((DebuggerAssemblyControlFlags)dwBits);
                    event->hr = S_OK;

                    // Update our debugger module
                    if (!CORDisableJITOptimizations(dwBits))
                    {
                        deModule->GetPrimaryModule()->MarkAllowedOptimizedCode();
                    }
                    else
                    {
                        deModule->GetPrimaryModule()->UnmarkAllowedOptimizedCode();
                    }

                    LOG((LF_CORDB, LL_INFO100, "D::HIPCE, Changing Jit-Debug-Info: fOpt=%d, new bits=0x%08x, HasOptCode = %d\n",
                        event->JitDebugInfo.fAllowJitOpts,
                        dwBits,
                        deModule->HasAnyOptimizedCode()
                        ));
                }
            }

            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_SET_JIT_COMPILER_FLAGS:
        {
            Module *module = NULL;
            DWORD dwBits = 0;
            DebuggerModule *deModule = event->JitDebugInfo.debuggerModuleToken.UnWrap();

            if (IsAttachInProgress())
            {
                hr = CORDBG_E_CANNOT_BE_ON_ATTACH;
            }
            else
            {
                module = deModule->GetRuntimeModule();
                _ASSERTE(NULL != module);
                {

                    //
                    // Initialize dwBits to all the values that we cannot set via this IPC event.
                    //
                    dwBits = (module->GetDebuggerInfoBits() &
                              ~(DACF_ALLOW_JIT_OPTS | DACF_ENC_ENABLED));
                    dwBits &= DACF_CONTROL_FLAGS_MASK;

                    if (event->JitDebugInfo.fAllowJitOpts)
                    {
                        dwBits |= DACF_ALLOW_JIT_OPTS;
                    }
                    if (event->JitDebugInfo.fEnableEnC)
                    {
#ifdef _X86_
                        bool fIgnorePdbs = ((module->GetDebuggerInfoBits() & DACF_IGNORE_PDBS) != 0);

                        bool fAllowEnc =
                            RunningOnWinNT5() &&
                            module->IsEditAndContinueCapable() &&
                            !CORProfilerPresent() &&
                            fIgnorePdbs;
#else
                        // Enc not supported on any other platforms.
                        bool fAllowEnc = false;
#endif

                        if (!fAllowEnc )
                        {
                            hr = CORDBG_S_NOT_ALL_BITS_SET;
                        }
                        else
                        {
                            dwBits |= DACF_ENC_ENABLED;
                        }
                    }
                    // Settings from the debugger take precedence over all
                    // other settings.
                    dwBits |= DACF_USER_OVERRIDE;
                }
            }
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(event, DB_IPCE_SET_JIT_COMPILER_FLAGS_RESULT, NULL, NULL);

            event->hr = hr;
            if (SUCCEEDED(hr))
            {
                _ASSERTE(module != NULL);
                module->SetDebuggerInfoBits((DebuggerAssemblyControlFlags)dwBits);

                // Update our debugger module
                if (!CORDisableJITOptimizations(dwBits))
                {
                    deModule->GetPrimaryModule()->MarkAllowedOptimizedCode();
                }
                else
                {
                    deModule->GetPrimaryModule()->UnmarkAllowedOptimizedCode();
                }

                LOG((LF_CORDB, LL_INFO100, "D::HIPCE, Changed Jit-Debug-Info: fOpt=%d, fEnableEnC=%d, new bits=0x%08x, HasOptCode = %d\n",
                       (dwBits & DACF_ALLOW_JIT_OPTS) != 0,
                       (dwBits & DACF_ENC_ENABLED) != 0,
                        dwBits,
                        deModule->HasAnyOptimizedCode()
                        ));

            }

            // Treat this as a Get so that that RS can look at this and maintained a cached value on the module.
            event->JitDebugInfo.fEnableEnC = ((module == NULL) ? FALSE : (module->IsEditAndContinueEnabled()));

            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_GET_JIT_COMPILER_FLAGS:
        {
            Module *module = NULL;
            DWORD dwBits = 0;
            DebuggerModule *deModule = event->JitDebugInfo.debuggerModuleToken.UnWrap();
            
            
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(event, DB_IPCE_GET_JIT_COMPILER_FLAGS_RESULT, NULL, NULL);

            module = deModule->GetRuntimeModule();
            _ASSERTE(NULL != module);

            dwBits = module->GetDebuggerInfoBits();
            event->JitDebugInfo.fAllowJitOpts = !CORDisableJITOptimizations(dwBits);
            event->JitDebugInfo.fEnableEnC = module->IsEditAndContinueEnabled();
            hr = S_OK;
            
            event->hr = hr;            

            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_SET_NGEN_COMPILER_FLAGS:
        {
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(event, DB_IPCE_SET_NGEN_COMPILER_FLAGS_RESULT, NULL, NULL);

            if (FAILED(hr))
            {
                event->hr = hr;
            }
            else
            {
                // no-op if no prejit
                event->hr = CORDBG_E_NGEN_NOT_SUPPORTED;
                LOG((LF_CORDB, LL_EVERYTHING, "D::HIPCE, Changing NGEN-Debug-Info: fOpt=%d\n",
                    event->JitDebugInfo.fAllowJitOpts
                    ));
            }

            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_GET_NGEN_COMPILER_FLAGS:
        {
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(event, DB_IPCE_GET_NGEN_COMPILER_FLAGS_RESULT, NULL, NULL);
            event->hr = CORDBG_E_NGEN_NOT_SUPPORTED;
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_GET_COMPILER_FLAGS:
        {
            Module *module = NULL;
            DWORD dwBits = 0;
            DebuggerModule *deModule = event->JitDebugInfo.debuggerModuleToken.UnWrap();

            
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(event, DB_IPCE_GET_COMPILER_FLAGS_RESULT, NULL, NULL);

            module = deModule->GetRuntimeModule();
            _ASSERTE(NULL != module);

            {
                dwBits = module->GetDebuggerInfoBits();
                event->JitDebugInfo.fAllowJitOpts = !CORDisableJITOptimizations(dwBits);
                event->JitDebugInfo.fEnableEnC = ((dwBits & DACF_ENC_ENABLED) != 0);
                event->hr = S_OK;
            }

            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_CONTROL_C_EVENT_RESULT:
        {
            SetDebuggerHandlingCtrlC(event->hr == S_OK);
            VERIFY(SetEvent(GetCtrlCMutex()));
        }
        break;

    case DB_IPCE_GET_SYNC_BLOCK_FIELD:
        GetAndSendSyncBlockFieldInfo((Object *)event->GetSyncBlockField.pObject,
                                     &event->GetSyncBlockField.objectTypeData,
                                     event->GetSyncBlockField.offsetToVars,
                                     event->GetSyncBlockField.fldToken,
                                     m_pRCThread,
                                     iWhich);
       break;

    // Set the JMC status on invididual methods
    case DB_IPCE_SET_METHOD_JMC_STATUS:
        {
            // Get the info out of the event
            DebuggerModule * pModule = event->SetJMCFunctionStatus.debuggerModuleToken.UnWrap();
            Module * pRuntimeModule = pModule->GetRuntimeModule();

            bool fStatus = (event->SetJMCFunctionStatus.dwStatus != 0);

            mdMethodDef token = event->SetJMCFunctionStatus.funcMetadataToken;

            // Prepare reply
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(event, DB_IPCE_SET_METHOD_JMC_STATUS_RESULT, NULL, NULL);
            event->hr = S_OK;

            if (pModule->GetPrimaryModule()->HasAnyOptimizedCode() && fStatus)
            {
                // If there's optimized code, then we can't be set JMC status to true.
                // That's because JMC probes are not injected in optimized code, and we
                // need a JMC probe to have a JMC function.
                event->hr = CORDBG_E_CANT_SET_TO_JMC;
            }
            else
            {
                DebuggerDataLockHolder debuggerDataLockHolder(this);
                // This may be called on an unjitted method, so we may
                // have to create the MethodInfo.
                DebuggerMethodInfo * pInfo = GetOrCreateMethodInfo(pRuntimeModule, token);

                if (pInfo == NULL)
                {
                    event->hr = E_OUTOFMEMORY;
                }
                else
                {
                    // Update the storage on the LS
                    pInfo->SetJMCStatus(fStatus);
                }
            }

            // Send reply
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    // Get the JMC status on a given function
    case DB_IPCE_GET_METHOD_JMC_STATUS:
        {
            // Get the method
            DebuggerModule * pModule = event->SetJMCFunctionStatus.debuggerModuleToken.UnWrap();

            Module * pRuntimeModule = pModule->GetRuntimeModule();
            mdMethodDef token = event->SetJMCFunctionStatus.funcMetadataToken;

            // Init reply
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(event, DB_IPCE_GET_METHOD_JMC_STATUS_RESULT, NULL, NULL);

            //
            // This may be called on an unjitted method, so we may
            // have to create the MethodInfo.
            //
            DebuggerMethodInfo * pInfo = GetOrCreateMethodInfo(pRuntimeModule, token);

            if (pInfo == NULL)
            {
                event->hr = E_OUTOFMEMORY;
            }
            else
            {
                bool fStatus = pInfo->IsJMCFunction();
                event->SetJMCFunctionStatus.dwStatus = fStatus;
                event->hr = S_OK;
            }

            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_SET_MODULE_JMC_STATUS:
        {
            // Get data out of event
            DebuggerModule * pModule = event->SetJMCFunctionStatus.debuggerModuleToken.UnWrap();
            bool fStatus = (event->SetJMCFunctionStatus.dwStatus != 0);

            // Prepare reply
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEventWithThreadId(event, DB_IPCE_SET_MODULE_JMC_STATUS_RESULT, NULL, LSPTR_APPDOMAIN::NullPtr());
            event->hr = S_OK;

            DebuggerModule * pPrimary = pModule->GetPrimaryModule();
            if (pPrimary->HasAnyOptimizedCode() && fStatus)
            {
                // If there's optimized code, then we can't be set JMC status to true.
                // That's because JMC probes are not injected in optimized code, and we
                // need a JMC probe to have a JMC function.
                event->hr = CORDBG_E_CANT_SET_TO_JMC;
            }
            else
            {
                pPrimary->SetJMCStatus(fStatus, 0, NULL);
            }



            // Send reply
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;


    case DB_IPCE_INTERCEPT_EXCEPTION:
        GetAndSendInterceptCommand(event, iWhich);
        break;

    case DB_IPCE_GET_EXCEPTION:
        {
            Thread *pThread = event->GetException.threadToken.UnWrap();

            //
            // This is a synchronous event (reply required)
            //
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);

            InitIPCEventWithThreadId(event,
                                     DB_IPCE_GET_EXCEPTION_RESULT,
                                     event->threadId, // ==> don't change it.
                                     LSPTR_APPDOMAIN::MakePtr( pThread->GetDomain() ) );

            if (g_pEEInterface->IsThreadExceptionNull(pThread))
            {
                event->GetExceptionResult.exceptionHandle = LSPTR_OBJECTHANDLE::NullPtr();
            }
            else
            {
                event->GetExceptionResult.exceptionHandle.Set(g_pEEInterface->GetThreadException(pThread));
            }

            event->hr = S_OK;
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_GET_ASSEMBLY_TRUST:
        {
            // Get the asssembly and appDomain in question
            Assembly* pAssembly = event->GetAssemblyTrust.debuggerAssemblyToken.UnWrap();
            AppDomain* pAppDomain = event->GetAssemblyTrust.debuggerAppDomainToken.UnWrap();

            // Prepare and send the reply
            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            InitIPCEvent(event, DB_IPCE_GET_ASSEMBLY_TRUST_RESULT, NULL, NULL);

            // Determine whether the assembly is loaded with full trust
            AssemblySecurityDescriptor *secDisc = pAssembly->GetSecurityDescriptor( pAppDomain );
            event->GetAssemblyTrustResult.fIsFullyTrusted = Security::IsFullyTrusted(secDisc);
            event->hr = S_OK;
 
            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    case DB_IPCE_GET_CURRENT_APPDOMAIN:
        {
            Thread *pThread = event->GetCurrentAppDomain.threadToken.UnWrap();
            _ASSERTE( pThread != NULL );        // expect thread to be valid

            event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
            LSPTR_APPDOMAIN pDomain = LSPTR_APPDOMAIN::NullPtr();
            if( pThread != NULL )
            {
                pDomain = LSPTR_APPDOMAIN::MakePtr( pThread->GetDomain() );
            }

            InitIPCEventWithThreadId(event,
                                     DB_IPCE_GET_CURRENT_APPDOMAIN_RESULT,
                                     event->threadId, // don't change it
                                     pDomain );
            event->GetCurrentAppDomainResult.currentAppDomainToken = pDomain;
            if( pDomain == NULL )
            {
                // I'm not aware of any situation in which we won't know the domain of a thread
                _ASSERTE( pDomain != NULL );
                event->hr = E_FAIL;
            }
            else
            {
                event->hr = S_OK;
            }

            m_pRCThread->SendIPCReply(iWhich);
        }
        break;

    default:
        // We should never get an event that we don't know about.
        CONSISTENCY_CHECK_MSGF(false, ("Unknown Debug-Event on LS:id=0x%08x.", event->type));
        LOG((LF_CORDB, LL_INFO10000, "Unknown event type: 0x%08x\n",
             event->type));
    }

    STRESS_LOG0(LF_CORDB, LL_INFO10000, "D::HIPCE: finished handling event\n");

    // dbgLockHolder goes out of scope - implicit Release
    return ret;
}

/*
 * GetAndSendInterceptCommand
 *
 * This function processes an INTERCEPT_EXCEPTION IPC event, sending the appropriate response.
 *
 * Parameters:
 *   event - the event to process.
 *   iWhich - the ipc target.
 *
 * Returns:
 *   hr - HRESULT.
 *
 */
HRESULT Debugger::GetAndSendInterceptCommand(DebuggerIPCEvent *event, IpcTarget iWhich)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS_FROM_GETJITINFO;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    _ASSERTE((event->type & DB_IPCE_TYPE_MASK) == DB_IPCE_INTERCEPT_EXCEPTION);

    hr = CORDBG_E_NONINTERCEPTABLE_EXCEPTION;

    //
    // Prepare reply
    //
    event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
    InitIPCEventWithThreadId(event, DB_IPCE_INTERCEPT_EXCEPTION_RESULT, event->threadId, event->appDomainToken);
    event->hr = hr;

    //
    // Send reply
    //
    m_pRCThread->SendIPCReply(iWhich);

    return hr;
}

//
HRESULT Debugger::GetAndSendSyncBlockFieldInfo(Object *pObject,
                                               DebuggerIPCE_BasicTypeData *objType,
                                               SIZE_T offsetToVars,
                                               mdFieldDef fldToken,
                                               DebuggerRCThread* rcThread,
                                               IpcTarget iWhich)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO100000, "D::GASSBFI: dmtok:0x%x Obj:0x%x, objType"
        ":0x%x, offset:0x%x\n", objType->debuggerModuleToken.UnWrap(), pObject, objType->elementType,
        offsetToVars));

    DebuggerModule *dm = NULL;

    dm = objType->debuggerModuleToken.UnWrap();

    HRESULT hr = S_OK;

    // We'll wrap this in an SEH handler, even though we should
    // never actually get whacked by this - the CordbObject should
    // validate the pointer first.
    EX_TRY
    {
        // Use AVInRuntimeImplOkHolder.
        AVInRuntimeImplOkayHolder AVOkay(TRUE);

        FieldDesc *pFD = NULL;
        const BYTE *pORField = NULL;

        // Find the class given its module and token. The class must be loaded.
        DebuggerModule *pDebuggerModule = objType->debuggerModuleToken.UnWrap();

        TypeHandle th = g_pEEInterface->FindLoadedClass(pDebuggerModule->GetRuntimeModule(), objType->metadataToken);

        // Note that GASCI will scribble over both the data in the incoming
        // mesage, and the outgoing message, so don't bother to prep the reply
        // before calling this.
        hr = GetAndSendClassInfo(rcThread,
                                 th,
                                 th,
                                 false,
                                 dm->GetAppDomain(),
                                 fldToken,
                                 &pFD, //OUT
                                 iWhich);
        DebuggerIPCEvent *result = rcThread->GetIPCEventReceiveBuffer(
            iWhich);
        InitIPCEvent(result,
                     DB_IPCE_GET_SYNC_BLOCK_FIELD_RESULT);

        if (pFD == NULL)
        {
            result->hr = CORDBG_E_ENC_HANGING_FIELD;

            hr = rcThread->SendIPCReply(iWhich);
            goto LExit;
        }

        _ASSERTE(pFD->IsEnCNew()); // Shouldn't be here if it wasn't added to an
            //already loaded class.

        pORField = (BYTE *)pFD->GetAddress(pObject);

        result->GetSyncBlockFieldResult.fStatic = pFD->IsStatic();
        DebuggerIPCE_FieldData *currentFieldData;
        currentFieldData = &(result->GetSyncBlockFieldResult.fieldData);
        currentFieldData->Initialize();

        currentFieldData->fldIsStatic = (pFD->IsStatic() != 0);
        currentFieldData->fldDebuggerToken.Set(pFD);
        currentFieldData->fldIsTLS = (pFD->IsThreadStatic() == TRUE);
        currentFieldData->fldMetadataToken = pFD->GetMemberDef();
        currentFieldData->fldIsRVA = (pFD->IsRVA() == TRUE);
        currentFieldData->fldIsContextStatic = (pFD->IsContextStatic() == TRUE);
        currentFieldData->fldStorageAvailable = true;

        if (currentFieldData->fldIsStatic)
        {
            //EnC is only supported on regular static fields
            _ASSERTE( !currentFieldData->fldIsContextStatic );
            _ASSERTE( !currentFieldData->fldIsTLS );
            _ASSERTE( !currentFieldData->fldIsRVA );

            // pORField contains the absolute address
            currentFieldData->SetStaticAddress( (void*)pORField );
        }
        else
        {
            currentFieldData->SetInstanceOffset( pORField - ((BYTE *)pObject + offsetToVars) );
        }

        hr = rcThread->SendIPCReply(iWhich);

LExit: ;
    }
    EX_CATCH
    {
        _ASSERTE(!"Given a bad ref to GASSBFI for!");
        hr = CORDBG_E_BAD_REFERENCE_VALUE;
    }
    EX_END_CATCH(SwallowAllExceptions);


    return hr;
}


// Poll & wait for the real helper thread to come up.
// It's possible that the helper thread  is blocked by DllMain, and so we can't
// Wait infinite. If this poll does timeout, then it just means we're likely
// go do helper duty instead of have the real helper do it.
void Debugger::PollWaitingForHelper()
{

    LOG((LF_CORDB, LL_INFO10000, "PollWaitingForHelper() start\n"));

    DebuggerIPCControlBlock * pDCB = g_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC);

    PREFIX_ASSUME(pDCB != NULL);

    int nTotalMSToWait = 8 * 1000;

    // Spin waiting for either the real helper thread or a temp. to be ready.
    // This should never timeout unless the helper is blocked on the loader lock.
    while (!pDCB->m_helperThreadId && !pDCB->m_temporaryHelperThreadId)
    {
        STRESS_LOG1(LF_CORDB,LL_INFO1000, "PollWaitForHelper. %d\n", nTotalMSToWait);

        _ASSERTE(!ThreadHoldsLock());

        const DWORD dwTime = 50;
        ClrSleepEx(dwTime, FALSE);
        nTotalMSToWait -= dwTime;

        if (nTotalMSToWait <= 0)
        {
            LOG((LF_CORDB, LL_INFO10000, "PollWaitingForHelper() timeout\n"));
            return;
        }
    }

    LOG((LF_CORDB, LL_INFO10000, "PollWaitingForHelper() succeed\n"));
    return;
}

//
// GetAndSendFunctionData gets the necessary data for a function and
// sends it back to the right side.
//
HRESULT Debugger::GetAndSendFunctionData(DebuggerRCThread* rcThread,
                                         mdMethodDef funcMetadataToken,
                                         DebuggerModule* pDebuggerModule,
                                         SIZE_T nVersion,
                                         IpcTarget iWhich)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS_FROM_GETJITINFO;
        PRECONDITION(CheckPointer(pDebuggerModule));
        PRECONDITION(funcMetadataToken != NULL);
        PRECONDITION(pDebuggerModule->GetRuntimeModule() != NULL);
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::GASFD: getting function data for "
         "0x%08x 0x%08x.\n", funcMetadataToken, pDebuggerModule));

    BaseDomain *bd = pDebuggerModule->GetRuntimeModule()->GetDomain();
    // Setup the event that we'll be sending the results in.
    DebuggerIPCEvent* event = rcThread->GetIPCEventReceiveBuffer(iWhich);
    InitIPCEvent(event,
                 DB_IPCE_GET_FUNCTION_DATA_RESULT,
                 0,
                 (AppDomain *)bd);


    HRESULT hr = GetFuncData(funcMetadataToken, pDebuggerModule, nVersion, &(event->FunctionData.basicData));

    // To make sure we don't break V1 behaviour, e.g. GetNativeCode,
    // we return the most recent blob of native code for the version of the method that
    // we happen to have seen.  For non-generic methods
    // there will only be one, but for generic code there may be others we are
    // ignoring.
    if (SUCCEEDED(hr))
    {
        DebuggerJitInfo *dji = NULL;
        MethodDesc* pFD =
            g_pEEInterface->FindLoadedMethodRefOrDef(pDebuggerModule->GetRuntimeModule(), funcMetadataToken);

        EX_TRY
        {
            DebuggerMethodInfo *dmi = GetOrCreateMethodInfo(pDebuggerModule->GetRuntimeModule(), funcMetadataToken);
            if (dmi != NULL)
            {

                AppDomain * pAppDomain = pDebuggerModule->GetAppDomain();

                DebuggerMethodInfo::DJIIterator it;
                dmi->IterateAllDJIs(pAppDomain, &it);

                if (!it.IsAtEnd())
                {
                    dji = it.Current();
                    pFD = dji->m_fd;
                }
            }
        }
        EX_CATCH
        {
            // We never expect anything in this block to throw (except for oom, and we
            // don't handle that so we can still assert)
            _ASSERTE(!"Shouldn't be throwing here");

            // Just swallow it, we still have a MD. DJI will be NULL.
            // Note that the RS is still blocking waiting for the IPC event.
        }
        EX_END_CATCH(SwallowAllExceptions)

        hr = GetMethodDescData(pFD, dji, &(event->FunctionData.possibleNativeData));
    }
    event->hr = hr;

    LOG((LF_CORDB, LL_INFO10000, "D::GASFD: sending result\n"));

    // Send off the data to the right side.
    hr = rcThread->SendIPCReply(iWhich);

    return hr;
}


//
// GetFuncData gets part of the data for a function.
//
HRESULT Debugger::GetFuncData(mdMethodDef funcMetadataToken,
                              DebuggerModule* pDebuggerModule,
                             SIZE_T nVersion,
                             DebuggerIPCE_FuncData *data)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pDebuggerModule));
        PRECONDITION(funcMetadataToken != NULL);
        PRECONDITION(pDebuggerModule->GetRuntimeModule() != NULL);
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::GASFD: getting function data for "
         "0x%08x 0x%08x.\n", funcMetadataToken, pDebuggerModule));

    data->funcMetadataToken = funcMetadataToken;
    data->funcDebuggerModuleToken.Set(pDebuggerModule);
    data->funcRVA = 0;
    data->classMetadataToken = mdTypeDefNil;
    data->ilStartAddress = NULL;
    data->ilSize = 0;
    data->localVarSigToken = mdSignatureNil;
    data->currentEnCVersion = CorDB_DEFAULT_ENC_FUNCTION_VERSION;

    HRESULT hr = GetFunctionInfo(
          pDebuggerModule->GetRuntimeModule(),
         funcMetadataToken,
         (DWORD *)&data->funcRVA,
         (BYTE**) &data->ilStartAddress,
         (unsigned int *) &data->ilSize,
         (mdToken *)&data->localVarSigToken);

    if (FAILED(hr))
    {
        return hr;
    }

    DebuggerMethodInfo *dmi = GetOrCreateMethodInfo(pDebuggerModule->GetRuntimeModule(), funcMetadataToken);

    // NOTE: ji may be NULL - that's OK

    if (dmi)
    {
        data->currentEnCVersion = dmi->GetCurrentEnCVersion();
    }

    // Get the class this method is in.
    mdToken tkParent;
    {
        CHECK_IF_CAN_TAKE_HELPER_LOCKS_IN_THIS_SCOPE(&hr, GetCanary());
        if (SUCCEEDED(hr))
        {
            hr = g_pEEInterface->GetParentToken(pDebuggerModule->GetRuntimeModule(),
                                            funcMetadataToken,
                                            &tkParent);
        }
    }
    if (SUCCEEDED(hr))
    {
        _ASSERTE(TypeFromToken(tkParent) == mdtTypeDef);
        data->classMetadataToken = tkParent;
        LOG((LF_CORDB, LL_INFO10000, "D::GASFD: function is class. "
             "0x%08x\n",
             data->classMetadataToken));
    }

    return hr;
}


//
// GetMethodDescData gets the JIT-related data for a function.
//
HRESULT Debugger::GetMethodDescData(MethodDesc *pFD,
                                    DebuggerJitInfo *ji,
                                    DebuggerIPCE_JITFuncData *data)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::GMDD: getting MethodDescData for "
         "0x%08x.\n", pFD));

    data->nativeStartAddressPtr = NULL;
    data->nativeSize = 0;
    data->nativeStartAddressColdPtr = NULL;
    data->nativeColdSize = 0;

    data->nativeCodeMethodDescToken.Set(pFD);
    data->nativeCodeJITInfoToken.Set(NULL);

#ifdef DEBUG
    data->nativeOffset = 0xdeadbeef;
        // Since Populate doesn't create a CordbNativeFrame, we don't
        // need the nativeOffset field to contain anything valid...
#endif //DEBUG
    data->ilToNativeMapAddr = NULL;
    data->ilToNativeMapSize = 0;
    data->isInstantiatedGeneric = false;
    data->enCVersion = CorDB_DEFAULT_ENC_FUNCTION_VERSION;

    if (pFD != NULL)
    {
        data->isInstantiatedGeneric = pFD->HasClassOrMethodInstantiation();

        if (ji != NULL && ji->m_jitComplete)
        {
            LOG((LF_CORDB, LL_INFO10000, "EE:D::GMDD: JIT info given.\n"));

            // Send over the native info
            // Since we don't support debugging code-pitching, we can just send the address over directly.
            // Helper to copy Hot-Cold info from a code-region to a JITFuncData
            InitJITFuncDataFromCodeRegion(data, &ji->m_codeRegionInfo);

            _ASSERTE(data->nativeStartAddressPtr == CORDB_ADDRESS_TO_PTR(ji->m_addrOfCode));

            // We should use the DJI rather than GetFunctionSize because
            // LockAndSendEnCRemapEvent will stop us at a point
            // that's prior to the MethodDesc getting updated, so it will
            // look as though the method hasn't been JITted yet, even
            // though we may get the LockAndSendEnCRemapEvent as a result
            // of a JITComplete callback
            _ASSERTE(data->nativeSize == ji->m_sizeOfCode);

            data->nativeCodeJITInfoToken.Set(ji);

            // Pass back the pointers to the sequence point map so
            // that the RIght Side can copy it out if needed.

            data->ilToNativeMapAddr = ji->GetSequenceMap();
            data->ilToNativeMapSize = ji->GetSequenceMapCount();
            data->enCVersion = ji->m_encVersion;
        }

        // If we're not tracking then we still return information about the
        // native code.
        if (ji == NULL)
        {
            CodeRegionInfo info = CodeRegionInfo::GetCodeRegionInfo(NULL, pFD);
            InitJITFuncDataFromCodeRegion(data, &info);

            _ASSERTE(data->nativeStartAddressPtr == (void*)pFD->GetFunctionAddress());
            _ASSERTE(data->nativeSize == g_pEEInterface->GetFunctionSize(pFD));
        }
    }
    LOG((LF_CORDB, LL_INFO10000, "D::GMDDD: native address=0x%08x, md=0x%08x, dji=0x%08x\n",
         data->nativeStartAddressPtr, data->nativeCodeMethodDescToken.UnWrap(), data->nativeCodeJITInfoToken.UnWrap()));

    return S_OK;
}




void Debugger::TypeHandleToBasicTypeInfo(AppDomain *pAppDomain, TypeHandle th, DebuggerIPCE_BasicTypeData *res, IpcTarget iWhich)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::THTBTI: converting left-side type handle to basic right-side type info, ELEMENT_TYPE: %d.\n", th.GetSignatureCorElementType()));
    // GetSignatureCorElementType returns E_T_CLASS for E_T_STRING... :-(
    if (th.IsNull())
    {
        res->elementType = ELEMENT_TYPE_VOID;
    }
    else if (th.GetMethodTable() == g_pObjectClass)
    {
        res->elementType = ELEMENT_TYPE_OBJECT;
    }
    else if (th.GetMethodTable() == g_pStringClass)
    {
        res->elementType = ELEMENT_TYPE_STRING;
    }
    else
    {
        res->elementType = th.GetSignatureCorElementType();
    }

    switch (res->elementType)
    {
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:
    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_FNPTR:
    case ELEMENT_TYPE_BYREF:
        res->typeHandle = WrapTypeHandle(th);
        res->metadataToken = mdTokenNil;
        res->debuggerModuleToken.Set(NULL);
        break;

    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_VALUETYPE:
        {
            res->typeHandle = th.HasInstantiation() ? WrapTypeHandle(th) : LSPTR_TYPEHANDLE::NullPtr(); // only set if instantiated
            res->metadataToken = th.GetCl();
            DebuggerModule * pDModule = LookupModule(th.GetModule(), pAppDomain);
            res->debuggerModuleToken.Set(pDModule);
            break;
        }

    default:
        res->typeHandle = LSPTR_TYPEHANDLE::NullPtr();
        res->metadataToken = mdTokenNil;
        res->debuggerModuleToken.Set(NULL);
        break;
    }
    return;
}

void Debugger::TypeHandleToExpandedTypeInfo(AreValueTypesBoxed boxed,
                                            AppDomain *pAppDomain,
                                            TypeHandle th,
                                            DebuggerIPCE_ExpandedTypeData *res,
                                            IpcTarget iWhich)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (th.IsNull())
    {
        res->elementType = ELEMENT_TYPE_VOID;
    }
    else if (th.GetMethodTable() == g_pObjectClass)
    {
        res->elementType = ELEMENT_TYPE_OBJECT;
    }
    else if (th.GetMethodTable() == g_pStringClass)
    {
        res->elementType = ELEMENT_TYPE_STRING;
    }
    else
    {
    LOG((LF_CORDB, LL_INFO10000, "D::THTETI: converting left-side type handle to expanded right-side type info, ELEMENT_TYPE: %d.\n", th.GetSignatureCorElementType()));
    // GetSignatureCorElementType returns E_T_CLASS for E_T_STRING... :-(
        res->elementType = th.GetSignatureCorElementType();
    }

    switch (res->elementType)
    {
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:
        _ASSERTE(th.IsArray());
        res->ArrayTypeData.arrayRank = th.AsArray()->GetRank();
        TypeHandleToBasicTypeInfo(pAppDomain, th.AsArray()->GetArrayElementTypeHandle(), &(res->ArrayTypeData.arrayTypeArg), iWhich);
        break;

    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
        if (boxed == AllBoxed)
        {
            res->elementType = ELEMENT_TYPE_CLASS;
            goto treatAllValuesAsBoxed;
        }
        _ASSERTE(th.IsTypeDesc());
        TypeHandleToBasicTypeInfo(pAppDomain, th.AsTypeDesc()->GetTypeParam(), &(res->UnaryTypeData.unaryTypeArg), iWhich);
        break;

    case ELEMENT_TYPE_VALUETYPE:
        if (boxed == OnlyPrimitivesUnboxed || boxed == AllBoxed)
            res->elementType = ELEMENT_TYPE_CLASS;
        // drop through

    case ELEMENT_TYPE_CLASS:
        {
treatAllValuesAsBoxed:
            res->ClassTypeData.typeHandle = th.HasInstantiation() ? WrapTypeHandle(th) : LSPTR_TYPEHANDLE::NullPtr(); // only set if instantiated
            res->ClassTypeData.metadataToken = th.GetCl();
            DebuggerModule * pModule = LookupModule(th.GetModule(), pAppDomain);
            res->ClassTypeData.debuggerModuleToken.Set(pModule);
            _ASSERTE (res->ClassTypeData.debuggerModuleToken != NULL);
            break;
        }

    case ELEMENT_TYPE_VALUEARRAY:
        _ASSERTE(!"unimplemented!");
        break;

    case ELEMENT_TYPE_FNPTR:
        {
            if (boxed == AllBoxed)
            {
                res->elementType = ELEMENT_TYPE_CLASS;
                goto treatAllValuesAsBoxed;
            }
            res->NaryTypeData.typeHandle = WrapTypeHandle(th);
            break;
        }
    default:
        // The element type is sufficient, unless the type is effectively a "boxed"
        // primitive value type...
        if (boxed == AllBoxed)
        {
            res->elementType = ELEMENT_TYPE_CLASS;
            goto treatAllValuesAsBoxed;
        }
        break;
    }
    LOG((LF_CORDB, LL_INFO10000, "D::THTETI: converted left-side type handle to expanded right-side type info, res->ClassTypeData.typeHandle = 0x%08x.\n", LsPtrToCookie(res->ClassTypeData.typeHandle)));
    return;
}


HRESULT Debugger::BasicTypeInfoToTypeHandle(DebuggerIPCE_BasicTypeData *data, TypeHandle *pRes)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::BTITTH: expanding basic right-side type to left-side type, ELEMENT_TYPE: %d.\n", data->elementType));
    *pRes = TypeHandle();
    TypeHandle th;
    switch (data->elementType)
    {
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:
    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
        _ASSERTE(!data->typeHandle.IsNull());
        th = GetTypeHandle(data->typeHandle);
        break;

    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_VALUETYPE:
        {
            if (!data->typeHandle.IsNull())
            {
                th = GetTypeHandle(data->typeHandle);
            }
            else
            {
            DebuggerModule *pDebuggerModule = data->debuggerModuleToken.UnWrap();

                th = g_pEEInterface->FindLoadedClass(pDebuggerModule->GetRuntimeModule(), data->metadataToken);
            if (th.IsNull())
            {
                LOG((LF_CORDB, LL_INFO10000, "D::ETITTH: class isn't loaded.\n"));
                    return CORDBG_E_CLASS_NOT_LOADED;
            }

            _ASSERTE(th.GetNumGenericArgs() == 0);
            }
            break;
        }
    case ELEMENT_TYPE_VALUEARRAY:
        _ASSERTE(!"unimplemented!");
        break;
    case ELEMENT_TYPE_FNPTR:
        {
            _ASSERTE(!data->typeHandle.IsNull());
            th = GetTypeHandle(data->typeHandle);
            break;
        }

    default:
        th = g_pEEInterface->FindLoadedElementType(data->elementType);
        break;
    }
    if (th.IsNull())
        return CORDBG_E_CLASS_NOT_LOADED;
    *pRes = th;
    return S_OK;
}

HRESULT Debugger::ExpandedTypeInfoToTypeHandle(DebuggerIPCE_ExpandedTypeData *data,
                                               unsigned int genericArgsCount,
                                               DebuggerIPCE_BasicTypeData *genericArgs,
                                               TypeHandle *pRes)
{
    WRAPPER_CONTRACT;

    LOG((LF_CORDB, LL_INFO10000, "D::ETITTH: expanding right-side type to left-side type, ELEMENT_TYPE: %d.\n", data->elementType));
    HRESULT hr;

    *pRes = TypeHandle();
    switch (data->elementType)
    {
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:
        {
            TypeHandle typar;
            _ASSERTE(genericArgsCount == 1);
            IfFailRet(BasicTypeInfoToTypeHandle(&(genericArgs[0]), &typar));
            *pRes = g_pEEInterface->FindLoadedArrayType(data->elementType, typar, data->ArrayTypeData.arrayRank);
            break;
        }

    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
        {
            TypeHandle typar;
            _ASSERTE(genericArgsCount == 1);
            IfFailRet (BasicTypeInfoToTypeHandle(&(genericArgs[0]), &typar));
            *pRes = g_pEEInterface->FindLoadedPointerOrByrefType(data->elementType, typar);
            break;
        }

    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_VALUETYPE:
        {
            DebuggerModule *pDebuggerModule = data->ClassTypeData.debuggerModuleToken.UnWrap();

            TypeHandle tycon =
                g_pEEInterface->FindLoadedClass(pDebuggerModule->GetRuntimeModule(), data->ClassTypeData.metadataToken);

            // If we can't find the class, return the proper HR to the right side. Note: if the class is not a value class and
            // the class is also not restored, then we must pretend that the class is still not loaded. We are gonna let
            // unrestored value classes slide, though, and special case access to the class's parent below.
            if (tycon.IsNull())
            {
                LOG((LF_CORDB, LL_INFO10000, "D::ETITTH: class isn't loaded.\n"));
                return CORDBG_E_CLASS_NOT_LOADED;
            }

            if (genericArgsCount == 0)
            {
                *pRes = tycon;
                return S_OK;
            }

            if (genericArgsCount != tycon.GetNumGenericArgs())
            {
                LOG((LF_CORDB, LL_INFO10000, "D::ETITTH: wrong number of type parameters, %d given, %d expected\n", genericArgsCount, tycon.GetNumGenericArgs()));
                _ASSERTE(genericArgsCount == tycon.GetNumGenericArgs());
                return E_FAIL;
            }

            S_UINT32 allocSize = S_UINT32( genericArgsCount ) * S_UINT32( sizeof(TypeHandle) );
            if( allocSize.IsOverflow() )
            {
                IfFailRet( E_OUTOFMEMORY );
            }
            TypeHandle *inst = (TypeHandle *) _alloca( allocSize.Value() );
            for (unsigned int i = 0; i < genericArgsCount; i++)
            {
                IfFailRet (BasicTypeInfoToTypeHandle(&genericArgs[i], &inst[i]));
            }
            *pRes = g_pEEInterface->FindLoadedInstantiation(tycon.GetModule(), tycon.GetCl(), genericArgsCount, inst);

            break;
        }
    case ELEMENT_TYPE_VALUEARRAY:
        _ASSERTE(!"unimplemented!");
        return E_FAIL;
    case ELEMENT_TYPE_FNPTR:
        {
            S_UINT32 allocSize = S_UINT32( genericArgsCount ) * S_UINT32( sizeof(TypeHandle) );
            if( allocSize.IsOverflow() )
            {
                IfFailRet( E_OUTOFMEMORY );
            }
            TypeHandle *inst = (TypeHandle *) _alloca( allocSize.Value() );
            for (unsigned int i = 0; i < genericArgsCount; i++)
            {
                IfFailRet (BasicTypeInfoToTypeHandle(&genericArgs[i], &inst[i]));
            }
            *pRes = g_pEEInterface->FindLoadedFnptrType(inst, genericArgsCount);
            break;
        }
    default:
        *pRes = g_pEEInterface->FindLoadedElementType(data->elementType);
        break;
    }
    _ASSERTE(pRes != NULL);
    if (pRes->IsNull())
    {
        LOG((LF_CORDB, LL_INFO10000, "D::ETITTH: type isn't loaded.\n"));
        return CORDBG_E_CLASS_NOT_LOADED;
    }
    return S_OK;

}

// Iterate through the type argument data, creating type handles as we go.
void Debugger::TypeDataWalk::ReadTypeHandles(unsigned int nTypeArgs, TypeHandle *ppResults)
{
    WRAPPER_CONTRACT;

    for (unsigned int i = 0; i < nTypeArgs; i++)
        ppResults[i] = ReadTypeHandle();
    }

TypeHandle Debugger::TypeDataWalk::ReadInstantiation(Module *pModule, mdTypeDef tok, unsigned int nTypeArgs)
{
    WRAPPER_CONTRACT;

    TypeHandle *inst = (TypeHandle *) _alloca(sizeof(TypeHandle) * nTypeArgs);
    ReadTypeHandles(nTypeArgs, inst) ;
    TypeHandle th = g_pEEInterface->LoadInstantiation(pModule, tok, nTypeArgs, inst);
    if (th.IsNull())
      COMPlusThrow(kArgumentException, L"Argument_InvalidGenericArg");
    return th;
}

TypeHandle Debugger::TypeDataWalk::ReadTypeHandle()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    DebuggerIPCE_TypeArgData * data = ReadOne();
    if (!data)
      COMPlusThrow(kArgumentException, L"Argument_InvalidGenericArg");

    LOG((LF_CORDB, LL_INFO10000, "D::ETITTH: expanding right-side type to left-side type, ELEMENT_TYPE: %d.\n", data->data.elementType));

    TypeHandle th;
    CorElementType et = data->data.elementType;
    switch (et)
    {
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:
    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
        if(data->numTypeArgs == 1)
        {
            TypeHandle typar = ReadTypeHandle();
            switch (et)
            {
            case ELEMENT_TYPE_ARRAY:
            case ELEMENT_TYPE_SZARRAY:
                th = g_pEEInterface->LoadArrayType(data->data.elementType, typar, data->data.ArrayTypeData.arrayRank);
          break;
    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
                th = g_pEEInterface->LoadPointerOrByrefType(data->data.elementType, typar);
          break;
            default:
                _ASSERTE(0);
        }
        }
        break;

    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_VALUETYPE:
        {
            DebuggerModule *pDebuggerModule = data->data.ClassTypeData.debuggerModuleToken.UnWrap();
            th = ReadInstantiation(pDebuggerModule->GetRuntimeModule(), data->data.ClassTypeData.metadataToken, data->numTypeArgs);
            break;
        }
    case ELEMENT_TYPE_VALUEARRAY:
        _ASSERTE(!"unimplemented!");
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_Type");
        break;

    case ELEMENT_TYPE_FNPTR:
        {
            TypeHandle *inst = (TypeHandle *) _alloca(sizeof(TypeHandle) * data->numTypeArgs);
            ReadTypeHandles(data->numTypeArgs, inst) ;
            th = g_pEEInterface->LoadFnptrType(inst, data->numTypeArgs);
            break;
        }

    default:
        th = g_pEEInterface->LoadElementType(data->data.elementType);
        break;
    }
    if (th.IsNull())
      COMPlusThrow(kArgumentNullException, L"ArgumentNull_Type");
    return th;

}


void Debugger::TypeDataWalk::Skip()
{
    LEAF_CONTRACT;

    DebuggerIPCE_TypeArgData * data = ReadOne();
    if (!data)
        return;

    for (unsigned int i = 0; i < data->numTypeArgs; i++)
        Skip();
}

// Read a type handle when it is used in the position of a generic argument or
// argument of an array type.  Take into account generic code sharing if we
// have been requested to find the canonical representative amongst a set of shared-
// code generic types.  That is, if generics code sharing is enabled then return "Object"
// for all reference types, and canonicalize underneath value types, e.g. V<string> --> V<object>.
//
// Return TypeHandle() if any of the type handles are not loaded.
TypeHandle Debugger::TypeDataWalk::ReadLoadedTypeArg(BOOL fCanon)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#if !defined(FEATURE_SHARE_GENERIC_CODE)
    return ReadLoadedTypeHandle(FALSE);
#else

    if (!fCanon || !g_pConfig->ShareGenericCode())
        return ReadLoadedTypeHandle(FALSE);

    // This nasty bit of code works out what the "canonicalization" of a
    // parameter to a generic is once we take into account generics code sharing.
    //
    // This logic is somewhat a duplication of logic in vm\typehandle.cpp, though
    // that logic operates on a TypeHandle format, i.e. assumes we're finding the
    // canonical form of a type that has already been loaded.  Here we are finding
    // the canonical form of a type that may not have been loaded (but where we expect
    // its canonical form to have been loaded).
    //
    // Ideally this logic would not be duplicated in this way, but it is difficult
    // to arrange for that.
    DebuggerIPCE_TypeArgData * data = ReadOne();
    if (!data)
        return TypeHandle();

    CorElementType et = data->data.elementType;
    TypeHandle th;
    switch (et)
    {
    case ELEMENT_TYPE_PTR:
        {
            _ASSERTE(data->numTypeArgs == 1);
            TypeHandle typar = ReadLoadedTypeArg(fCanon);
            if (!typar.IsNull())
                th = g_pEEInterface->FindLoadedPointerOrByrefType(data->data.elementType, typar);
            break;
        }
    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_VALUETYPE:
        {
            DebuggerModule *pDebuggerModule = data->data.ClassTypeData.debuggerModuleToken.UnWrap();
            TypeHandle def = g_pEEInterface->FindLoadedClass(pDebuggerModule->GetRuntimeModule(),data->data.ClassTypeData.metadataToken);
            if ((!def.IsNull() && def.IsValueType()) || et == ELEMENT_TYPE_VALUETYPE)
                th = ReadLoadedInstantiation(fCanon,pDebuggerModule->GetRuntimeModule(), data->data.ClassTypeData.metadataToken, data->numTypeArgs);
            else
            {
                // skip the instantiation - no need to look at it since the type canonicalizes to "Object"
                for (unsigned int i = 0; i < data->numTypeArgs; i++)
                    Skip();
                th = TypeHandle(g_pHiddenMethodTableClass);
            }
            break;
        }
    case ELEMENT_TYPE_FNPTR:
        {
            TypeHandle *inst = (TypeHandle *) _alloca(sizeof(TypeHandle) * data->numTypeArgs);
            if (ReadLoadedTypeHandles(fCanon, data->numTypeArgs,inst))
                th = g_pEEInterface->FindLoadedFnptrType(inst, data->numTypeArgs);
            break;
        }
    default:
        {
            // If there are any type args (e.g. for arrays) they can be skipped.  The thing
            // is a reference type anyway.
            for (unsigned int i = 0; i < data->numTypeArgs; i++)
                Skip();
            if (CorTypeInfo::IsObjRef(et))
                th = TypeHandle(g_pHiddenMethodTableClass);
            else
                th = g_pEEInterface->FindLoadedElementType(data->data.elementType);
            break;
        }
    }

    // Nuked a debug check here as FindLoadedGenericCanon() is going away (due to recursion and __alloca,
    // it was never truly NOTHROW/NOGCTRIGGER.)

    return th;
#endif // FEATURE_SHARE_GENERIC_CODE
}

// Iterate through the type argument data, creating type handles as we go.
// Return FALSE if any of the type handles are not loaded.
BOOL Debugger::TypeDataWalk::ReadLoadedTypeHandles(BOOL fCanon, unsigned int nTypeArgs, TypeHandle *ppResults)
{
    WRAPPER_CONTRACT;

    BOOL allOK = true;
    for (unsigned int i = 0; i < nTypeArgs; i++)
    {
        ppResults[i] = ReadLoadedTypeArg(fCanon);
        allOK &= !ppResults[i].IsNull();
    }
    return allOK;
}

TypeHandle Debugger::TypeDataWalk::ReadLoadedInstantiation(BOOL fCanon, Module *pModule, mdTypeDef tok, unsigned int nTypeArgs)
{
    WRAPPER_CONTRACT;

    TypeHandle *inst = (TypeHandle *) _alloca(sizeof(TypeHandle) * nTypeArgs);
    if (!ReadLoadedTypeHandles(fCanon, nTypeArgs, inst))
        return TypeHandle();
    return g_pEEInterface->FindLoadedInstantiation(pModule, tok, nTypeArgs, inst);
}


TypeHandle Debugger::TypeDataWalk::ReadLoadedTypeHandle(BOOL fCanon)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    DebuggerIPCE_TypeArgData * data = ReadOne();
    if (!data)
      return TypeHandle();

    TypeHandle th;
    switch (data->data.elementType)
    {
        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_SZARRAY:
            {
                TypeHandle typar = ReadLoadedTypeArg(fCanon);
                if (!typar.IsNull())
                    th = g_pEEInterface->FindLoadedArrayType(data->data.elementType, typar, data->data.ArrayTypeData.arrayRank);
            }
            break;

        case ELEMENT_TYPE_PTR:
        case ELEMENT_TYPE_BYREF:
            {
                TypeHandle typar = ReadLoadedTypeArg(fCanon);
                if (!typar.IsNull())
                    th = g_pEEInterface->FindLoadedPointerOrByrefType(data->data.elementType, typar);
            }
            break;
        case ELEMENT_TYPE_CLASS:
        case ELEMENT_TYPE_VALUETYPE:
            {
                DebuggerModule *pDebuggerModule = data->data.ClassTypeData.debuggerModuleToken.UnWrap();
                th = ReadLoadedInstantiation(fCanon, pDebuggerModule->GetRuntimeModule(), data->data.ClassTypeData.metadataToken, data->numTypeArgs);
            }
            break;

        case ELEMENT_TYPE_FNPTR:
            {
                TypeHandle *inst = (TypeHandle *) _alloca(sizeof(TypeHandle) * data->numTypeArgs);
                if (ReadLoadedTypeHandles(fCanon, data->numTypeArgs, inst))
                    th = g_pEEInterface->FindLoadedFnptrType(inst, data->numTypeArgs);
        }
            break;

    default:
            th = g_pEEInterface->FindLoadedElementType(data->data.elementType);
        break;
    }
    return th;
}





//
// GetAndSendObjectInfo gets the necessary data for an object and
// sends it back to the right side.
//
HRESULT Debugger::GetAndSendObjectInfo(DebuggerRCThread* rcThread,
                                       AppDomain *pAppDomain,
                                       void* objectRefAddress,
                                       OBJECTHANDLE objectHandle,
                                       bool objectRefIsValue,
                                       CorElementType objectType,
                                       IpcTarget iWhich)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(objectType != ELEMENT_TYPE_GENERICINST);
        PRECONDITION(objectType != ELEMENT_TYPE_VAR);
        PRECONDITION(objectType != ELEMENT_TYPE_MVAR);
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::GASOI: getting info for "
         "0x%08x %d %d.\n", objectRefAddress, objectHandle,
         objectRefIsValue));

    TypeHandle objTypeHandle;
    Object *objPtr = NULL;
    void *objRef;

    _ASSERTE(pAppDomain != NULL);

    // Setup the event that we'll be sending the results in.
    DebuggerIPCEvent* event = rcThread->GetIPCEventReceiveBuffer(iWhich);
    InitIPCEvent(event,
                 DB_IPCE_GET_OBJECT_INFO_RESULT,
                 0,
                 pAppDomain);

    DebuggerIPCE_ObjectData *oi = &(event->GetObjectInfoResult);
    oi->objRef = NULL;
    oi->objRefBad = false;
    oi->objSize = 0;
    oi->objOffsetToVars = 0;
    oi->objTypeData.elementType = objectType;
    oi->nstructInfo.size = 0;
    oi->nstructInfo.ptr = NULL;

    bool badRef = false;
    bool plainSend = false;

    // We wrap this in SEH just in case the object reference is bad.
    // We can trap the access violation and return a reasonable result.
    EX_TRY
    {
        // Use AVInRuntimeImplOkHolder.
        AVInRuntimeImplOkayHolder AVOkay(TRUE);

        // We use this method for getting info about TypedByRef's,
        // too. But they're somewhat different than you're standard
        // object ref, so we special case here.
        if (objectType == ELEMENT_TYPE_TYPEDBYREF)
        {
            // The objectRefAddress really points to a TypedByRef struct.
            TypedByRef *ra = (TypedByRef*) objectRefAddress;

            TypeHandleToBasicTypeInfo(pAppDomain, ra->type, &(oi->typedByrefInfo.typedByrefType), iWhich);

            // The reference to the object is in the data field of the TypedByRef.
            oi->objRef = ra->data;

            LOG((LF_CORDB, LL_INFO10000, "D::GASOI: sending REFANY result: "
                 "ref=0x%08x, cls=0x%08x, mod=0x%08x\n",
                 oi->objRef,
                 oi->typedByrefInfo.typedByrefType.metadataToken,
                 oi->typedByrefInfo.typedByrefType.debuggerModuleToken.UnWrap()));

            // Send off the data to the right side.
            plainSend = true;
            goto LExit;
        }

        // Grab the pointer to the object.
        if (objectHandle != NULL)
        {
            // Case 1 - we're in an object handle.
            objPtr = (Object*) g_pEEInterface->GetObjectFromHandle(objectHandle);
            objRef = NULL;
        }
        else
        {
            if (objectRefIsValue)
            {
                // Case 2 - objectRefAddress is the object we're looking for. This is the
                // case when the RS is using a local copy
                objRef = objectRefAddress;
            }
            else
            {
                // Case 3 - objectRefAddress has 1 level of indirection. This is the case
                // when the RS is using a remote (LS) copy. They've sent it over to the LS for
                // us to deref.
                objRef = *((void**)objectRefAddress);
            }
            objPtr = (Object*) objRef;
        }

        // Pass back the object pointer.
        oi->objRef = objPtr;

        // Shortcut null references now...
        if (objPtr == NULL)
        {
            LOG((LF_CORDB, LL_INFO10000, "D::GASOI: ref is NULL.\n"));

            badRef = true;
            goto LExit;
        }

        if (FAILED(ValidateObject(objPtr)))
        {
            LOG((LF_CORDB, LL_INFO10000, "D::GASOI: address is not a valid object.\n"));

            badRef = true;
            goto LExit;
        }

        objTypeHandle = objPtr->GetTypeHandle();

        // Save basic object info.
        oi->objSize = objPtr->GetSize();
        oi->objOffsetToVars =
            (UINT_PTR)((Object*)objPtr)->GetData() - (UINT_PTR)objPtr;

        TypeHandleToExpandedTypeInfo(AllBoxed, pAppDomain, objTypeHandle, &(oi->objTypeData), iWhich);
        // If this is a string object, set the type to ELEMENT_TYPE_STRING.
        if (g_pEEInterface->IsStringObject((Object*)objPtr))
            oi->objTypeData.elementType = ELEMENT_TYPE_STRING;

        //Note that the next element type may be different from
        // (objTypeHandle.GetSignatureCorElementType()) when we have a boxed
        // value.
        switch (oi->objTypeData.elementType)
        {
        case ELEMENT_TYPE_STRING:
            {
                LOG((LF_CORDB, LL_INFO10000, "D::GASOI: its a string.\n"));

                StringObject *so = (StringObject*)objPtr;

                oi->stringInfo.length =
                    g_pEEInterface->StringObjectGetStringLength(so);
                oi->stringInfo.offsetToStringBase =
                    (UINT_PTR) g_pEEInterface->StringObjectGetBuffer(so) -
                    (UINT_PTR) objPtr;

            }

            break;

        case ELEMENT_TYPE_CLASS:
        case ELEMENT_TYPE_OBJECT:
            // the type carries all the necessary nifo. in these cases.
            break;

        //
        //
        case ELEMENT_TYPE_SZARRAY:
        case ELEMENT_TYPE_ARRAY:
            {
                if (!objTypeHandle.IsArray())
                {
                    LOG((LF_CORDB, LL_INFO10000,
                         "D::GASOI: object should be an array.\n"));

                    badRef = true;
                    goto LExit;
                }

                ArrayBase *arrPtr = (ArrayBase*)objPtr;

                oi->arrayInfo.rank = arrPtr->GetRank();  // this is also returned in the type information for the array - we return both for sanity checking...
                oi->arrayInfo.componentCount = arrPtr->GetNumComponents();
                oi->arrayInfo.offsetToArrayBase =
                    (UINT_PTR)arrPtr->GetDataPtr() - (UINT_PTR)arrPtr;

                if (arrPtr->IsMultiDimArray())
                {
                    oi->arrayInfo.offsetToUpperBounds =
                        (UINT_PTR)arrPtr->GetBoundsPtr() - (UINT_PTR)arrPtr;

                    oi->arrayInfo.offsetToLowerBounds =
                        (UINT_PTR)arrPtr->GetLowerBoundsPtr() - (UINT_PTR)arrPtr;
                }
                else
                {
                    oi->arrayInfo.offsetToUpperBounds = 0;
                    oi->arrayInfo.offsetToLowerBounds = 0;
                }

                oi->arrayInfo.elementSize =
                    arrPtr->GetMethodTable()->GetComponentSize();

                LOG((LF_CORDB, LL_INFO10000, "D::GASOI: array info: "
                    "baseOff=%d, lowerOff=%d, upperOff=%d, cnt=%d, rank=%d, rank (2) = %d,"
                     "eleSize=%d, eleType=0x%02x\n",
                     oi->arrayInfo.offsetToArrayBase,
                     oi->arrayInfo.offsetToLowerBounds,
                     oi->arrayInfo.offsetToUpperBounds,
                     oi->arrayInfo.componentCount,
                     oi->arrayInfo.rank,
                     oi->objTypeData.ArrayTypeData.arrayRank,
                     oi->arrayInfo.elementSize,
                     oi->objTypeData.ArrayTypeData.arrayTypeArg.elementType));
            }

            break;

        default:
            ASSERT(!"Invalid object type!");
        }

LExit: ;
    }
    EX_CATCH
    {
        LOG((LF_CORDB, LL_INFO10000,
             "D::GASOI: exception indicated ref is bad.\n"));

        badRef = true;
    }
    EX_END_CATCH(SwallowAllExceptions);

    if (plainSend) {
        return rcThread->SendIPCReply(iWhich);
    }

    oi->objRefBad = badRef;

    LOG((LF_CORDB, LL_INFO10000, "D::GASOI: sending result.\n"));

    // Send off the data to the right side.
    return rcThread->SendIPCReply(iWhich);
}

//
// GetAndSendClassInfo gets the necessary data for a Class or constructed type and
// sends it back to the right side.
//
// This method operates in one of two modes - the "send class or type info"
// mode, and "find me the field desc" mode, which is used by
// GetAndSendSyncBlockFieldInfo to get a FieldDesc for a specific
// field.  If fldToken is mdFieldDefNil, then we're in
// the first mode, if not, then we're in the FieldDesc mode.
//  FIELD DESC MODE: We DON'T send message in the FieldDesc mode.
//      We indicate success by setting *pFD to nonNULL, failure by
//      setting *pFD to NULL.
//
//
// Sometimes we need to get information about a generic instantiation, rather than
// a class.  In this case, GetAndSendClassInfo is passed the representative type
// for the set of constructed types that share layouts.
//
HRESULT Debugger::GetAndSendClassInfo(DebuggerRCThread* rcThread,
                                      TypeHandle thExact,
                                      TypeHandle thApprox,
                                      BOOL fInstantiatedType,
                                      AppDomain *pAppDomain,
                                      mdFieldDef fldToken,
                                      FieldDesc **pFD, //OUT
                                      IpcTarget iWhich)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    LOG((LF_CORDB, LL_EVERYTHING, "GetIPCEventSendBuffer called in GetAndSendClassInfo\n"));

    _ASSERTE( fldToken == mdFieldDefNil || pFD != NULL);

    BOOL fSendClassInfoMode = fldToken == mdFieldDefNil;

#ifdef _DEBUG
    if (!fSendClassInfoMode)
    {
        _ASSERTE(pFD != NULL);
        (*pFD) = NULL;
    }
#endif //_DEBUG

    // Setup the event that we will return the results in
    DebuggerIPCEvent* event= rcThread->GetIPCEventSendBuffer(iWhich);
    InitIPCEvent(event, DB_IPCE_GET_CLASS_INFO_RESULT, NULL, pAppDomain);

    // If we can't find the class, return the proper HR to the right side. Note: if the class is not a value class and
    // the class is also not restored, then we must pretend that the class is still not loaded. We are gonna let
    // unrestored value classes slide, though, and special case access to the class's parent below.
    if (thApprox.IsNull() || (!thApprox.IsValueType() && !thApprox.IsRestored()))
    {
        LOG((LF_CORDB, LL_INFO10000, "D::GASCI: class isn't loaded.\n"));

        event->hr = CORDBG_E_CLASS_NOT_LOADED;

        if (iWhich == IPC_TARGET_OUTOFPROC && fSendClassInfoMode)
            return rcThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
        else
            return S_OK;
    }
    // If the exact type handle is not restored ignore it.
    if (!thExact.IsNull() && !thExact.IsRestored())
    {
        thExact = TypeHandle();
    }


    // Count the instance and static fields for this class (not including parent).
    // This will not include any newly added EnC fields.
    unsigned int IFCount = thApprox.GetMethodTable()->GetNumIntroducedInstanceFields();
    unsigned int SFCount = thApprox.GetMethodTable()->GetNumStaticFields();


    unsigned int totalFields = IFCount + SFCount;
    unsigned int fieldCount = 0;

    event->GetClassInfoResult.isValueClass = (thApprox.IsValueType() != 0);
    event->GetClassInfoResult.genericArgsCount = thApprox.GetNumGenericArgs();

    // For Generic classes you must get the object size via the type handle, which
    // will get you to the right information for the particular instantiation
    // you're working with...
    event->GetClassInfoResult.objectSize =
        (event->GetClassInfoResult.genericArgsCount && !fInstantiatedType)
        ? 0
        : thApprox.GetClass()->GetNumInstanceFieldBytes();


    event->GetClassInfoResult.varCount = totalFields;
    event->GetClassInfoResult.fieldCount = 0;

    DebuggerIPCE_FieldData *currentFieldData = &(event->GetClassInfoResult.fieldData);
    unsigned int eventSize = (UINT_PTR)currentFieldData - (UINT_PTR)event;
    unsigned int eventMaxSize = CorDBIPC_BUFFER_SIZE;

     BYTE *pGCStaticsBase = NULL;
     BYTE *pNonGCStaticsBase = NULL;
     if( !thExact.IsNull() )
     {
        // If this is a non-generic type, or an instantiated type, then we'll be able to get the static var bases
        // If the typeHandle represents a generic type constructor (i.e. an uninstantiated generic class), then
        // the static bases will be null (since statics are per-instantiation).
        MethodTable* mt = thExact.GetMethodTable();
        Module* pModuleForStatics = mt->GetModuleForStatics();
        if (pModuleForStatics != NULL)
        {
            DomainLocalModule* pLocalModule = pModuleForStatics->GetDomainLocalModule(pAppDomain);
            if (pLocalModule != NULL)
            {
                pGCStaticsBase = (BYTE*)pLocalModule->GetGCStaticsBasePointer(mt);
                pNonGCStaticsBase = (BYTE*)pLocalModule->GetNonGCStaticsBasePointer(mt);
            }
        }
     }

    LOG((LF_CORDB, LL_INFO10000, "D::GASCI: total fields=%d.\n", totalFields));

    EncApproxFieldDescIterator fdIterator(thApprox.GetClass()->GetMethodTable(),ApproxFieldDescIterator::ALL_FIELDS);

    FieldDesc* fd;

    while ((fd = fdIterator.Next()) != NULL)
    {
        if (!fSendClassInfoMode)
        {
            // We're looking for a specific fieldDesc, see if we got it.
            if (fd->GetMemberDef() == fldToken)
            {
                (*pFD) = fd;
                return S_OK;
            }
            else
                continue;
        }

        // fill in the currentFieldData structure
        ComputeFieldData( fd, pGCStaticsBase, pNonGCStaticsBase, currentFieldData );

        // Bump our counts and pointers for the next event.
        event->GetClassInfoResult.fieldCount++;
        fieldCount++;
        currentFieldData++;
        eventSize += sizeof(DebuggerIPCE_FieldData);

        // If that was the last field that will fit, send the event now and prep the next one.
        if ((eventSize + sizeof(DebuggerIPCE_FieldData)) >= eventMaxSize)
        {
            LOG((LF_CORDB, LL_INFO10000, "D::GASCI: sending a result, fieldCount=%d, totalFields=%d\n",
                 event->GetClassInfoResult.fieldCount, totalFields));

            if (iWhich == IPC_TARGET_OUTOFPROC)
            {
                hr = rcThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
            }
            else
            {
                DebuggerIPCEvent *newEvent = m_pRCThread->GetIPCEventSendBufferContinuation(event);

                if (newEvent != NULL)
                {
                    InitIPCEvent(newEvent, DB_IPCE_GET_CLASS_INFO_RESULT, NULL, pAppDomain);
                    newEvent->GetClassInfoResult.isValueClass = event->GetClassInfoResult.isValueClass;
                    newEvent->GetClassInfoResult.objectSize = event->GetClassInfoResult.objectSize;
                    newEvent->GetClassInfoResult.varCount = event->GetClassInfoResult.varCount;

                    event = newEvent;
                }
                else
                    return E_OUTOFMEMORY;
            }

            event->GetClassInfoResult.fieldCount = 0;
            currentFieldData = &(event->GetClassInfoResult.fieldData);
            eventSize = (UINT_PTR)currentFieldData - (UINT_PTR)event;
        }
    }

    _ASSERTE(!fSendClassInfoMode ||
             fieldCount == totalFields);

    if (fSendClassInfoMode &&
         (event->GetClassInfoResult.fieldCount > 0 || totalFields == 0))
    {
        LOG((LF_CORDB, LL_INFO10000, "D::GASCI: sending final result, fieldCount=%d, totalFields=%d\n",
             event->GetClassInfoResult.fieldCount, totalFields));

        if (iWhich == IPC_TARGET_OUTOFPROC)
            hr = rcThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
        else
            hr = S_OK;
    }

    return hr;
}


// Compute the field info for fd and store in currentFieldData
void Debugger::ComputeFieldData( FieldDesc* fd,
                                 BYTE * pGCStaticsBase,
                                 BYTE * pNonGCStaticsBase,
                                 DebuggerIPCE_FieldData * currentFieldData )
{
    currentFieldData->fldIsStatic = (fd->IsStatic() == TRUE);
    currentFieldData->fldIsPrimitive = (fd->IsPrimitive() == TRUE);
    currentFieldData->Initialize();

    {
        // Otherwise, we'll compute the info & send it back.
        currentFieldData->fldMetadataToken = fd->GetMemberDef();
        currentFieldData->fldStorageAvailable = true;
        currentFieldData->fldDebuggerToken.Set(fd);
        currentFieldData->fldIsTLS = (fd->IsThreadStatic() == TRUE);
        currentFieldData->fldIsContextStatic = (fd->IsContextStatic() == TRUE);
        currentFieldData->fldIsRVA = (fd->IsRVA() == TRUE);

        // Compute the address of the field
        if (fd->IsStatic())
        {
            // statics are addressed using an absolute address.
            if( fd->IsRVA() )
            {
                // RVA statics are relative to a base module address
                DWORD offset = fd->GetOffset();
                void* addr = fd->GetModule()->GetRvaField(offset);
                currentFieldData->SetStaticAddress( addr );
            }
            else if( fd->IsThreadStatic() || fd->IsContextStatic() )
            {
                // this is a special type of static that must be queried using DB_IPCE_GET_SPECIAL_STATIC
            }
            else
            {
                // This is a normal static variable in the GC or Non-GC static base table
                BYTE* base = fd->IsPrimitive() ? pNonGCStaticsBase : pGCStaticsBase;
                if( base == NULL )
                {
                    // static var not available.  This may be an open generic class (not an instantiated type),
                    // or we might only have approximate type information because the type hasn't been
                    // initialized yet.
                    currentFieldData->SetStaticAddress( NULL );
                }
                else
                {
                    // calculate the absolute address using the base and the offset from the base
                    currentFieldData->SetStaticAddress( base + fd->GetOffset() );
                }
            }
         }
        else
        {
            // instance variables are addressed using an offset within the instance
            currentFieldData->SetInstanceOffset( fd->GetOffset() );
        }
    }

}

//
// GetAndSendTypeHandleParams gets the necessary data for a type handle, i.e. its
// type parameters, e.g. "String" and "List<int>" from the type handle
// for "Dict<String,List<int>>", and sends it back to the right side.
//
HRESULT Debugger::GetAndSendTypeHandleParams(DebuggerRCThread* rcThread,
                                 AppDomain *pAppDomain,
                                 TypeHandle pTypeHandle,
                                 IpcTarget iWhich)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::GASTHP: getting type parameters for 0x%08x 0x%0x8.\n",
         pAppDomain, pTypeHandle.AsPtr()));

    HRESULT hr = S_OK;

    // Setup the event that we will return the results in
    DebuggerIPCEvent* event= rcThread->GetIPCEventSendBuffer(iWhich);
    InitIPCEvent(event, DB_IPCE_GET_TYPE_HANDLE_PARAMS_RESULT, NULL, pAppDomain);

    // Find the class given its type handle.

    unsigned int nGenericArgs = pTypeHandle.GetNumGenericArgs();
    event->GetTypeHandleParamsResult.totalGenericArgsCount = nGenericArgs;
    event->GetTypeHandleParamsResult.genericArgsCount = 0;

    DebuggerIPCE_ExpandedTypeData *currGenericArgData = &(event->GetTypeHandleParamsResult.genericArgs);
    unsigned int eventSize = (UINT_PTR)currGenericArgData - (UINT_PTR)event;
    unsigned int eventMaxSize = CorDBIPC_BUFFER_SIZE;

    _ASSERTE(nGenericArgs != 0);

    for (unsigned int i = 0; i<nGenericArgs; i++)
    {
        TypeHandleToExpandedTypeInfo(NoValueTypeBoxing, pAppDomain, pTypeHandle.GetInstantiation()[i],
                                     currGenericArgData,iWhich);
        event->GetTypeHandleParamsResult.genericArgsCount++;
        currGenericArgData++;
        eventSize += sizeof(DebuggerIPCE_ExpandedTypeData);
        // If that was the last field that will fit, send the event now and prep the next one.
        if ((eventSize + sizeof(DebuggerIPCE_ExpandedTypeData)) >= eventMaxSize)
        {
            LOG((LF_CORDB, LL_INFO10000, "D::GASTHP: sending one event"));
            if (iWhich == IPC_TARGET_OUTOFPROC)
            {
                hr = rcThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
            }
            else
            {
                event = m_pRCThread->GetIPCEventSendBufferContinuation(event);

                if (event == NULL)
                    return E_OUTOFMEMORY;
                InitIPCEvent(event, DB_IPCE_GET_TYPE_HANDLE_PARAMS_RESULT, NULL, pAppDomain);

            }

            event->GetTypeHandleParamsResult.genericArgsCount = 0;
            currGenericArgData = &(event->GetTypeHandleParamsResult.genericArgs);
            eventSize = (UINT_PTR)currGenericArgData - (UINT_PTR)event;
        }
    }
    LOG((LF_CORDB, LL_INFO10000, "D::GASTHP: sending  final result"));

    if (event->GetTypeHandleParamsResult.genericArgsCount > 0)
    {
        if (iWhich == IPC_TARGET_OUTOFPROC)
            hr = rcThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
        else
            hr = S_OK;
    }

    return hr;
}


//
// GetAndSendExpandedTypeInfo returns the "expanded" information about a type
// given a type handle.  This is just enough information to decide
// whether it is an array type, pointer type etc., and to handle
// the case where these are recursively nested.
HRESULT Debugger::GetAndSendExpandedTypeInfo(DebuggerRCThread* rcThread,
                                     AppDomain *pAppDomain,
                                     TypeHandle pTypeHandle,
                                     IpcTarget iWhich)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::EAST: getting type parameters for 0x%08x 0x%0x8.\n",
         pAppDomain, pTypeHandle.AsPtr()));

    HRESULT hr = S_OK;

    DebuggerIPCEvent* event = rcThread->GetIPCEventReceiveBuffer(iWhich);
    InitIPCEvent(event, DB_IPCE_GET_EXPANDED_TYPE_INFO_RESULT, NULL, pAppDomain);

    // "AllValueTypesUnboxed" is because the this is only used to expand type parameters,
    // which we want to report in their natural state....
    TypeHandleToExpandedTypeInfo(NoValueTypeBoxing, pAppDomain, pTypeHandle,  &(event->ExpandTypeResult), iWhich);

    LOG((LF_CORDB, LL_INFO10000, "D::EAST: sending result"));

    event->hr = S_OK;

    if (iWhich == IPC_TARGET_OUTOFPROC)
        hr = rcThread->SendIPCReply(IPC_TARGET_OUTOFPROC);
    else
        hr = S_OK;

    return hr;
}

//
// GetAndSendTypeHandle finds the type handle for an instantiated
// type if it is available.
//
HRESULT Debugger::GetAndSendTypeHandle(DebuggerRCThread* rcThread,
                                       AppDomain *pAppDomain,
                                       DebuggerIPCE_ExpandedTypeData *typeData,
                                       unsigned int genericArgsCount,
                                       DebuggerIPCE_BasicTypeData *genericArgData,
                                       IpcTarget iWhich)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::GASTH: getting info.\n"));

    HRESULT hr = S_OK;

    // Setup the event that we will return the results in
    DebuggerIPCEvent* event = rcThread->GetIPCEventReceiveBuffer(iWhich);
    InitIPCEvent(event, DB_IPCE_GET_TYPE_HANDLE_RESULT, 0, pAppDomain);

    TypeHandle res;
    hr = ExpandedTypeInfoToTypeHandle(typeData, genericArgsCount, genericArgData, &res);
    if (SUCCEEDED(hr))
    {
        _ASSERTE(!res.IsNull());
        event->GetTypeHandleResult.typeHandleExact = WrapTypeHandle(res);
    }

    event->hr = hr;

    LOG((LF_CORDB, LL_INFO10000, "D::GASTH: sending result, hr = 0x%0x8, result = 0x%0x8\n", hr, LsPtrToCookie(event->GetTypeHandleResult.typeHandleExact)));

    // Free the buffer that is holding the genericArgData data. This is
    // a buffer that was created in response to a GET_BUFFER
    // message, so we release it with ReleaseRemoteBuffer.
    ReleaseRemoteBuffer((BYTE*)genericArgData, true);

    if (iWhich == IPC_TARGET_OUTOFPROC)
        hr = rcThread->SendIPCReply(IPC_TARGET_OUTOFPROC);
    else
        hr = S_OK;

    return hr;
}


//
// GetAndSendApproxTypeHandle finds the type handle for the layout of the instance fields of an instantiated
// type if it is available.
//
HRESULT Debugger::GetAndSendApproxTypeHandle(DebuggerRCThread* rcThread,
                                       AppDomain *pAppDomain,
                                       unsigned int nTypeData,
                                       DebuggerIPCE_TypeArgData *pTypeData,
                                       IpcTarget iWhich)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::GASATH: getting info.\n"));

    // Setup the event that we will return the results in
    DebuggerIPCEvent* event = rcThread->GetIPCEventReceiveBuffer(iWhich);
    InitIPCEvent(event, DB_IPCE_GET_APPROX_TYPE_HANDLE_RESULT, 0, pAppDomain);

    TypeDataWalk walk(pTypeData, nTypeData);
    TypeHandle res = walk.ReadLoadedTypeHandle(TRUE);
    if (!res.IsNull())
    {
        event->GetApproxTypeHandleResult.typeHandleApprox = WrapTypeHandle(res);
        event->hr = S_OK;
    }
    else
    {
        event->hr = CORDBG_E_CLASS_NOT_LOADED;
    }

    LOG((LF_CORDB, LL_INFO10000, "D::GASATH: sending result, result = 0x%0x8\n", LsPtrToCookie(event->GetApproxTypeHandleResult.typeHandleApprox)));

    // Free the buffer that is holding the data. This is
    // a buffer that was created in response to a GET_BUFFER
    // message, so we release it with ReleaseRemoteBuffer.
    ReleaseRemoteBuffer((BYTE*)pTypeData, true);

    HRESULT hr;
    if (iWhich == IPC_TARGET_OUTOFPROC)
        hr = rcThread->SendIPCReply(IPC_TARGET_OUTOFPROC);
    else
        hr = S_OK;

    return hr;
}


//
// GetAndSendMethodDescParams gets the type parameter data for a method descriptor
// for a generic method and sends it back.
//
HRESULT Debugger::GetAndSendMethodDescParams(DebuggerRCThread* rcThread,
                                             AppDomain *pAppDomain,
                                             MethodDesc *pFD,
                                             void *exactGenericArgsToken,
                                             IpcTarget iWhich)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::GASMDP: getting generic type parameters for md 0x%08x 0x%0x8.\n",
         pAppDomain, pFD));

    _ASSERTE(pFD != NULL );

    HRESULT hr = S_OK;

    PREFIX_ASSUME(iWhich == IPC_TARGET_OUTOFPROC);


    // Setup the event that we will return the results in
    DebuggerIPCEvent* event= rcThread->GetIPCEventSendBuffer(iWhich);

    InitIPCEvent(event, DB_IPCE_GET_METHOD_DESC_PARAMS_RESULT, NULL, pAppDomain);


    unsigned int nGenericClassArgs = pFD->GetNumGenericClassArgs();
    unsigned int nGenericMethodArgs = pFD->GetNumGenericMethodArgs();
    unsigned int nGenericArgs = nGenericClassArgs + nGenericMethodArgs;
    event->GetMethodDescParamsResult.totalGenericArgsCount = nGenericArgs;
    event->GetMethodDescParamsResult.totalGenericClassArgsCount = nGenericClassArgs;
    event->GetMethodDescParamsResult.genericArgsCount = 0;
    LOG((LF_CORDB, LL_INFO10000, "D::GASMDI: %d type parameters.\n", nGenericArgs));

    TypeHandle specificClass;
    MethodDesc* specificMethod;

    // Provide some protection against getting a bogus exactGenericArgsToken from the RS
    // Ideally this wouldn't be necessary - we should fix any place where this could happen,
    // and add debug sanity checks in those locations (eg. GetExactGenericArgsFromVarInfo).
    bool badGenArgTok = false;
    PAL_CPP_TRY
    {
        AVInRuntimeImplOkayHolder AVOkay(TRUE);

        Generics::GetExactInstantiationsFromCallInformation(pFD, exactGenericArgsToken, &specificClass, &specificMethod);
        if( !specificClass.GetMethodTable()->SanityCheck() || !specificMethod->GetMethodTable()->SanityCheck() )
        {
            badGenArgTok = true;
        }
    }
    PAL_CPP_CATCH_ALL
    {
        badGenArgTok = true;
    }
    PAL_CPP_ENDTRY

    if( badGenArgTok )
    {
        // token is not valid - fall back to representative instantiation
        _ASSERTE( !"bad exactGenericArgsToken in GetAndSendMethodDescParams - falling back to representative type" );
        STRESS_LOG0(LF_CORDB, LL_WARNING, "bad exactGenericArgsToken in GetAndSendMethodDescParams - falling back to representative type\n");
        specificClass = TypeHandle( pFD->GetMethodTable() );
        specificMethod = pFD;
    }


    TypeHandle *classInst = specificMethod->GetExactClassInstantiation(specificClass);     
    TypeHandle *methodInst = specificMethod->GetMethodInstantiation();

    // Prefix doesn't realize that pFD->GetClassification() == specificMethod->GetClasification() even when
    // it is simulating the trivial case in GetExactInstantiationsFromCallInformation of specificMethod==pFD.
    PREFIX_ASSUME( (classInst == NULL) == (nGenericClassArgs == 0) );
    PREFIX_ASSUME( (methodInst == NULL) == (nGenericMethodArgs == 0) );

    DebuggerIPCE_ExpandedTypeData *currGenericArgData = &(event->GetMethodDescParamsResult.genericArgs);
    unsigned int eventSize = (UINT_PTR)currGenericArgData - (UINT_PTR)event;
    unsigned int eventMaxSize = CorDBIPC_BUFFER_SIZE;
    ASSERT (nGenericArgs != 0);

    for (unsigned int i = 0; i<nGenericArgs; i++)
    {
        LOG((LF_CORDB, LL_INFO10000, "D::GASMDI: one type parameter.\n"));
        TypeHandle p = (i < nGenericClassArgs) ? classInst[i] : methodInst[i-nGenericClassArgs];
        TypeHandleToExpandedTypeInfo(NoValueTypeBoxing, pAppDomain, p,  currGenericArgData, iWhich);

        event->GetMethodDescParamsResult.genericArgsCount++;
        currGenericArgData++;
        eventSize += sizeof(DebuggerIPCE_ExpandedTypeData);
        // If that was the last field that will fit, send the event now and prep the next one.
        if ((eventSize + sizeof(DebuggerIPCE_ExpandedTypeData)) >= eventMaxSize)
        {
            hr = rcThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

            event->GetMethodDescParamsResult.genericArgsCount = 0;
            currGenericArgData = &(event->GetMethodDescParamsResult.genericArgs);
            eventSize = (UINT_PTR)currGenericArgData - (UINT_PTR)event;
        }
    }
    LOG((LF_CORDB, LL_INFO10000, "D::GASMDP: sending  final result"));

    if (event->GetMethodDescParamsResult.genericArgsCount > 0)
    {
        hr = rcThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
    }

    return hr;
}



//
// GetAndSendSpecialStaticInfo
//
HRESULT Debugger::GetAndSendSpecialStaticInfo(DebuggerRCThread* rcThread,
                                              FieldDesc *pField,
                                              Thread *pRuntimeThread,
                                              IpcTarget iWhich)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::GASSSI: getting info for "
         "0x%08x 0x%0x8.\n", pField, pRuntimeThread));

    HRESULT hr = S_OK;

    // Setup the event that we'll be sending the results in.
    DebuggerIPCEvent* event = rcThread->GetIPCEventReceiveBuffer(iWhich);
    PREFIX_ASSUME(event != NULL);
    InitIPCEvent(event,
                 DB_IPCE_GET_SPECIAL_STATIC_RESULT,
                 NULL, NULL);

    // Find out where the field is living...

    if (pField->IsThreadStatic())
    {
        event->GetSpecialStaticResult.fldAddress = (LPVOID)pRuntimeThread->GetStaticFieldAddrNoCreate(pField);
    }
    else if (pField->IsContextStatic())
    {
        event->GetSpecialStaticResult.fldAddress = pRuntimeThread->GetContext()->GetStaticFieldAddrNoCreate(pField);
    }
    else
    {
        // In case, we have more special cases added. You will never know!
        _ASSERTE(!"NYI");
    }

    // Send off the data to the right side.
    hr = rcThread->SendIPCReply(iWhich);

    return hr;
}


//
// GetAndSendJITInfo gets the necessary JIT data for a function and
// sends it back to the right side.
//
HRESULT Debugger::GetAndSendJITInfo(DebuggerRCThread* rcThread,
                                    DebuggerJitInfo *pJITInfo,
                                    AppDomain *pAppDomain,
                                    IpcTarget iWhich)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::GASJI: getting info for dji "
         "0x%08x\n", pJITInfo));

    unsigned int totalNativeInfos = 0;
    unsigned int argCount = 0;

    HRESULT hr = S_OK;

    if ((pJITInfo != NULL) && (pJITInfo->m_jitComplete) && !pJITInfo->m_codePitched)
    {
        argCount = GetArgCount(pJITInfo->m_fd);
        totalNativeInfos = pJITInfo->GetVarNativeInfoCount();
    }
    else
    {
        pJITInfo = NULL;
        LOG((LF_CORDB, LL_INFO10000, "D::GASJI: no JIT info found...\n"));
    }

    //
    // Prepare the result event.
    //
    DebuggerIPCEvent* event = rcThread->GetIPCEventSendBuffer(iWhich);
    InitIPCEvent(event,
                 DB_IPCE_GET_JIT_INFO_RESULT,
                 NULL,
                 pAppDomain);
    event->GetJITInfoResult.totalNativeInfos = totalNativeInfos;
    event->GetJITInfoResult.argumentCount = argCount;
    event->GetJITInfoResult.nativeInfoCount = 0;

    ICorJitInfo::NativeVarInfo *currentNativeInfo =
        &(event->GetJITInfoResult.nativeInfo);
    unsigned int eventSize = (UINT_PTR)currentNativeInfo - (UINT_PTR)event;
    unsigned int eventMaxSize = CorDBIPC_BUFFER_SIZE;

    unsigned int nativeInfoCount = 0;

    while (nativeInfoCount < totalNativeInfos)
    {
        *currentNativeInfo = pJITInfo->GetVarNativeInfo()[nativeInfoCount];

        //
        // Bump our counts and pointers for the next event.
        //
        event->GetJITInfoResult.nativeInfoCount++;
        nativeInfoCount++;
        currentNativeInfo++;
        eventSize += sizeof(*currentNativeInfo);

        //
        // If that was the last field that will fit, send the event now
        // and prep the next one.
        //
        if ((eventSize + sizeof(*currentNativeInfo)) >= eventMaxSize)
        {
            LOG((LF_CORDB, LL_INFO10000, "D::GASJI: sending a result\n"));

            if (iWhich == IPC_TARGET_OUTOFPROC)
            {
                hr = rcThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
            }

            event->GetJITInfoResult.nativeInfoCount = 0;
            currentNativeInfo = &(event->GetJITInfoResult.nativeInfo);
            eventSize = (UINT_PTR)currentNativeInfo - (UINT_PTR)event;
        }
    }

    if (((event->GetJITInfoResult.nativeInfoCount > 0) ||
         (totalNativeInfos == 0)) && (iWhich ==IPC_TARGET_OUTOFPROC))
    {
        LOG((LF_CORDB, LL_INFO10000, "D::GASJI: sending final result\n"));

        hr = rcThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
    }

    return hr;
}

//
// GetAndSendTransitionStubInfo figures out if an address is a stub
// address and sends the result back to the right side.
//
void Debugger::GetAndSendTransitionStubInfo(CORDB_ADDRESS_TYPE *stubAddress, IpcTarget iWhich)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::GASTSI: IsTransitionStub. Addr=0x%08x\n", stubAddress));

    bool result = false;

    result = g_pEEInterface->IsStub((const BYTE *)stubAddress);



    // This is a synchronous event (reply required)
    DebuggerIPCEvent *event = m_pRCThread->GetIPCEventReceiveBuffer(iWhich);
    InitIPCEvent(event, DB_IPCE_IS_TRANSITION_STUB_RESULT, NULL, NULL);
    event->IsTransitionStubResult.isStub = result;

    // Send the result
    m_pRCThread->SendIPCReply(iWhich);
}

/*
 * A generic request for a buffer
 *
 * This is a synchronous event (reply required).
 */
HRESULT Debugger::GetAndSendBuffer(DebuggerRCThread* rcThread, ULONG bufSize)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // This is a synchronous event (reply required)
    DebuggerIPCEvent* event = rcThread->GetIPCEventReceiveBuffer(IPC_TARGET_OUTOFPROC);
    PREFIX_ASSUME(event != NULL);
    InitIPCEvent(event, DB_IPCE_GET_BUFFER_RESULT, NULL, NULL);

    // Allocate the buffer
    event->GetBufferResult.pBuffer = new (interopsafe, nothrow) BYTE[bufSize];

    LOG((LF_CORDB, LL_EVERYTHING, "D::GASB: new'd 0x%x\n", event->GetBufferResult.pBuffer));

    // Check for out of memory error
    if (event->GetBufferResult.pBuffer == NULL)
        event->GetBufferResult.hr = E_OUTOFMEMORY;
    else
    {
        // The call to Append will call CUnorderedArray, which will call unsafe New.
        CHECK_IF_CAN_TAKE_HELPER_LOCKS_IN_THIS_SCOPE(&(event->GetBufferResult.hr), GetCanary());
        if (SUCCEEDED(event->GetBufferResult.hr))
        {
            BYTE **ppNextBlob = GetMemBlobs()->Append();
            if (ppNextBlob == NULL)
            {
                event->GetBufferResult.hr = E_OUTOFMEMORY;
            }
            else
            {
                (*ppNextBlob) = (BYTE *)event->GetBufferResult.pBuffer;
                event->GetBufferResult.hr = S_OK;
            }
        }            
    }

    // Send the result
    return rcThread->SendIPCReply(IPC_TARGET_OUTOFPROC);
}

/*
 * Used to release a previously-requested buffer
 *
 * This is a synchronous event (reply required).
 */
HRESULT Debugger::SendReleaseBuffer(DebuggerRCThread* rcThread, BYTE *pBuffer)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB,LL_INFO10000, "D::SRB for buffer 0x%x\n", pBuffer));

    // This is a synchronous event (reply required)
    DebuggerIPCEvent* event = rcThread->GetIPCEventReceiveBuffer(IPC_TARGET_OUTOFPROC);
    PREFIX_ASSUME(event != NULL);
    InitIPCEvent(event, DB_IPCE_RELEASE_BUFFER_RESULT, NULL, NULL);

    _ASSERTE(pBuffer != NULL);

    // Free the memory
    ReleaseRemoteBuffer(pBuffer, true);

    // Indicate success in reply
    event->ReleaseBufferResult.hr = S_OK;

    // Send the result
    return rcThread->SendIPCReply(IPC_TARGET_OUTOFPROC);
}


//
// Used to delete the buffer previously-requested  by the right side.
// We've factored the code since both the ~Debugger and SendReleaseBuffer
// methods do this.
//
HRESULT Debugger::ReleaseRemoteBuffer(BYTE *pBuffer, bool removeFromBlobList)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_EVERYTHING, "D::RRB: Releasing RS-alloc'd buffer 0x%x\n", pBuffer));

    // Remove the buffer from the blob list if necessary.
    if (removeFromBlobList)
    {
        USHORT cBlobs = GetMemBlobs()->Count();
        BYTE **rgpBlobs = GetMemBlobs()->Table();

        for (USHORT i = 0; i < cBlobs; i++)
        {
            if (rgpBlobs[i] == pBuffer)
            {
                GetMemBlobs()->DeleteByIndex(i);
                break;
            }
        }
    }

    // Delete the buffer.
    DeleteInteropSafe(pBuffer);

    return S_OK;
}

//
// UnrecoverableError causes the Left Side to enter a state where no more
// debugging can occur and we leave around enough information for the
// Right Side to tell what happened.
//
void Debugger::UnrecoverableError(HRESULT errorHR,
                                  unsigned int errorCode,
                                  const char *errorFile,
                                  unsigned int errorLine,
                                  bool exitThread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10,
         "Unrecoverable error: hr=0x%08x, code=%d, file=%s, line=%d\n",
         errorHR, errorCode, errorFile, errorLine));

    //
    // Setting this will ensure that not much else happens...
    //
    m_unrecoverableError = TRUE;

    //
    // Fill out the control block with the error.
    // in-proc will find out when the function fails
    //
    DebuggerIPCControlBlock *pDCB = m_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC);

    PREFIX_ASSUME(pDCB != NULL);

    pDCB->m_errorHR = errorHR;
    pDCB->m_errorCode = errorCode;

    //
    // If we're told to, exit the thread.
    //
    if (exitThread)
    {
        LOG((LF_CORDB, LL_INFO10,
             "Thread exiting due to unrecoverable error.\n"));
        ExitThread(errorHR);
    }
}

//
// Callback for IsThreadAtSafePlace's stack walk.
//
StackWalkAction Debugger::AtSafePlaceStackWalkCallback(CrawlFrame *pCF,
                                                       VOID* data)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;

        PRECONDITION(CheckPointer(pCF));
        PRECONDITION(CheckPointer(data));
    }
    CONTRACTL_END;

    bool *atSafePlace = (bool*)data;
    LOG((LF_CORDB, LL_INFO100000, "D:AtSafePlaceStackWalkCallback\n"));

    if (pCF->IsFrameless() && pCF->IsActiveFunc())
    {
        LOG((LF_CORDB, LL_INFO1000000, "D:AtSafePlaceStackWalkCallback, IsFrameLess() and IsActiveFunc()\n"));
        if (g_pEEInterface->CrawlFrameIsGcSafe(pCF))
        {
            LOG((LF_CORDB, LL_INFO1000000, "D:AtSafePlaceStackWalkCallback - TRUE: CrawlFrameIsGcSafe()\n"));
            *atSafePlace = true;
        }
    }
    return SWA_ABORT;
}

//
// Determine, via a quick one frame stack walk, if a given thread is
// in a gc safe place.
//
bool Debugger::IsThreadAtSafePlaceWorker(Thread *thread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;

        PRECONDITION(CheckPointer(thread));
    }
    CONTRACTL_END;

    bool atSafePlace = false;

    // Setup our register display.
    REGDISPLAY rd;
    CONTEXT *context = g_pEEInterface->GetThreadFilterContext(thread);

    _ASSERTE(!(g_pEEInterface->GetThreadFilterContext(thread) && ISREDIRECTEDTHREAD(thread)));
    if (context != NULL)
    {
        g_pEEInterface->InitRegDisplay(thread, &rd, context, TRUE);
    }
    else
    {
        CONTEXT ctx;
        ZeroMemory(&rd, sizeof(rd));
        ZeroMemory(&ctx, sizeof(ctx));
#if defined(_X86_)
        rd.pPC = (SLOT*)&(ctx.Eip);
        rd.PCTAddr = (TADDR)&(ctx.Eip);
#else
        FillRegDisplay(&rd, &ctx);
#endif

        if (ISREDIRECTEDTHREAD(thread))
        {
            thread->GetFrame()->UpdateRegDisplay(&rd);
        }
    }

    // Do the walk. If it fails, we don't care, because we default
    // atSafePlace to false.
    g_pEEInterface->StackWalkFramesEx(
                                 thread,
                                 &rd,
                                 Debugger::AtSafePlaceStackWalkCallback,
                                 (VOID*)(&atSafePlace),
                                 QUICKUNWIND | HANDLESKIPPEDFRAMES |
                                 DISABLE_MISSING_FRAME_DETECTION);

#ifdef LOGGING
    if (!atSafePlace)
        LOG((LF_CORDB | LF_GC, LL_INFO1000,
             "Thread 0x%x is not at a safe place.\n",
             GetThreadIdHelper(thread)));
#endif

    return atSafePlace;
}

bool Debugger::IsThreadAtSafePlace(Thread *thread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;

        PRECONDITION(CheckPointer(thread));
    }
    CONTRACTL_END;


    if (m_fShutdownMode)
    {
        return true;
    }

    //
    //
    if ( (thread->IsExceptionInProgress()) &&
         (g_pEEInterface->GetThreadException(thread) == CLRException::GetPreallocatedStackOverflowExceptionHandle()) )
    {
        return false;
    }
    // </HACKHACK>
    else
    {
        return IsThreadAtSafePlaceWorker(thread);
    }
}

//-----------------------------------------------------------------------------
// Get the complete user state flags.
// This will collect flags both from the EE and from the LS.
// This is the real implementation of the RS's ICorDebugThread::GetUserState().
//
// Parameters:
//    pThread - non-null thread to get state for.
//
// Returns: a CorDebugUserState flags enum describing state.
//-----------------------------------------------------------------------------
CorDebugUserState Debugger::GetFullUserState(Thread *pThread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pThread));
    }
    CONTRACTL_END;

    CorDebugUserState state = g_pEEInterface->GetPartialUserState(pThread);

    bool fSafe = IsThreadAtSafePlace(pThread);
    if (!fSafe)
    {
        state = (CorDebugUserState) (state | USER_UNSAFE_POINT);
    }

    return state;
}

/******************************************************************************
 *
 * Helper for debugger to get an unique thread id
 * If we are not in Fiber mode, we can safely use OSThreadId
 * Otherwise, we will use our own unique ID.
 *
 * We will return our unique ID when our host is hosting Thread.
 *
 *
 ******************************************************************************/
DWORD Debugger::GetThreadIdHelper(Thread *pThread)
{
    WRAPPER_CONTRACT;

    if (!CLRTaskHosted())
    {
        // use the plain old OS Thread ID
        return pThread->GetOSThreadId();
    }
    else
    {
        // use our unique thread ID
        return pThread->GetThreadId();
    }
}

//-----------------------------------------------------------------------------
// Called by EnC during remapping to get information about the local vars.
// EnC will then use this to set values in the new version to their corresponding
// values from the old version.
//
// Returns a pointer to the debugger's copies of the maps. Caller
// does not own the memory provided via vars outparameter.
//-----------------------------------------------------------------------------
void Debugger::GetVarInfo(MethodDesc *       fd,   // [IN] method of interest
                    void *DebuggerVersionToken,    // [IN] which edit version
                    SIZE_T *           cVars,      // [OUT] size of 'vars'
                    const NativeVarInfo **vars     // [OUT] map telling where local vars are stored
                    )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS_FROM_GETJITINFO;
    }
    CONTRACTL_END;

    DebuggerJitInfo * ji = (DebuggerJitInfo *)DebuggerVersionToken;

    // If we didn't supply a DJI, then we're asking for the most recent version.
    if (ji == NULL)
    {
        ji = GetLatestJitInfoFromMethodDesc(fd);
    }
    _ASSERTE(fd == ji->m_fd);

    PREFIX_ASSUME(ji != NULL);

    *vars = ji->GetVarNativeInfo();
    *cVars = ji->GetVarNativeInfoCount();
}

#include "openum.h"



/******************************************************************************
 *
 ******************************************************************************/
bool Debugger::GetILOffsetFromNative (MethodDesc *pFunc, const BYTE *pbAddr,
                                      DWORD nativeOffset, DWORD *ilOffset)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS_FROM_GETJITINFO;
    }
    CONTRACTL_END;

    if (!HasLazyData())
    {
        DebuggerLockHolder dbgLockHolder(this);
        // This is an entry path into the debugger, so make sure we're inited.
        LazyInit();
    }

    // Sometimes we'll get called w/ an instantiating stub MD.
    if (pFunc->IsWrapperStub())
    {
        pFunc = pFunc->GetWrappedMethodDesc();
    }

    DebuggerJitInfo *jitInfo =
            GetJitInfo(pFunc, (const BYTE *)pbAddr);

    if (jitInfo != NULL)
    {
        CorDebugMappingResult map;
        DWORD whichIDontCare;

        *ilOffset = jitInfo->MapNativeOffsetToIL(
                                        nativeOffset,
                                        &map,
                                        &whichIDontCare);

        return true;
    }

    return false;
}

/******************************************************************************
 *
 ******************************************************************************/
DWORD Debugger::GetHelperThreadID(void )
{
    LEAF_CONTRACT;

    return m_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC)
        ->m_temporaryHelperThreadId;
}


// HRESULT Debugger::InsertToMethodInfoList():  Make sure
//  that there's only one head of the the list of DebuggerMethodInfos
//  for the (implicitly) given MethodDef/Module pair.
HRESULT
Debugger::InsertToMethodInfoList( DebuggerMethodInfo *dmi )
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB,LL_INFO10000,"D:IAHOL DMI: dmi:0x%08x\n", dmi));

    HRESULT hr = S_OK;

    _ASSERTE(dmi != NULL);

    _ASSERTE(HasDebuggerDataLock());

//    CHECK_DJI_TABLE_DEBUGGER;

    hr = CheckInitMethodInfoTable();

    if (FAILED(hr)) {
        return (hr);
    }

    DebuggerMethodInfo *dmiPrev = m_pMethodInfos->GetMethodInfo(dmi->m_module, dmi->m_token);

    _ASSERTE((dmiPrev == NULL) || ((dmi->m_token == dmiPrev->m_token) && (dmi->m_module == dmiPrev->m_module)));

    LOG((LF_CORDB,LL_INFO10000,"D:IAHOL: current head of dmi list:0x%08x\n",dmiPrev));

    if (dmiPrev != NULL)
    {
        dmi->m_prevMethodInfo = dmiPrev;
        dmiPrev->m_nextMethodInfo = dmi;

        _ASSERTE(dmi->m_module != NULL);
        hr = m_pMethodInfos->OverwriteMethodInfo(dmi->m_module,
                                         dmi->m_token,
                                         dmi,
                                         FALSE);

        LOG((LF_CORDB,LL_INFO10000,"D:IAHOL: DMI version 0x%04x for token 0x%08x\n",
            dmi->GetCurrentEnCVersion(),dmi->m_token));
    }
    else
    {
        LOG((LF_CORDB, LL_EVERYTHING, "AddMethodInfo being called in D:IAHOL\n"));
        hr = m_pMethodInfos->AddMethodInfo(dmi->m_module,
                                         dmi->m_token,
                                         dmi);
    }
#ifdef _DEBUG
    dmiPrev = m_pMethodInfos->GetMethodInfo(dmi->m_module, dmi->m_token);
    LOG((LF_CORDB,LL_INFO10000,"D:IAHOL: new head of dmi list:0x%08x\n",
        dmiPrev));
#endif //_DEBUG

    // DebuggerDataLockHolder out of scope - release implied
    return hr;
}

//-----------------------------------------------------------------------------
// Helper to get an SString through the IPC buffer.
// We do this by putting the SString data into a LS_RS_buffer object,
// and then the RS reads it out as soon as it's queued.
// It's very very important that the SString's buffer is around while we send the event.
// So we pass the SString by reference in case there's an implicit conversion (because
// we don't want to do the conversion on a temporary object and then lose that object).
//-----------------------------------------------------------------------------
void SetLSBufferFromSString(Ls_Rs_StringBuffer * pBuffer, SString & str)
{
    // Copy string contents (+1 for null terminator) into a LS_RS_Buffer.
    // Then the RS can pull it out as a null-terminated string.
    pBuffer->SetLsData(
        (BYTE*) str.GetUnicode(),
        (str.GetCount() +1)* sizeof(WCHAR)
    );
}

//*************************************************************
// structure that we to marshal MDA Notification event data.
//*************************************************************
struct SendMDANotificationParams
{
    Thread * m_pThread; // may be NULL. Lets us send on behalf of other threads.

    // Pass SStrings by ptr in case to guarantee that they're shared (in case we internally modify their storage).
    SString * m_szName;
    SString * m_szDescription;
    SString * m_szXML;
    CorDebugMDAFlags m_flags;

    SendMDANotificationParams(
        Thread * pThread, // may be NULL. Lets us send on behalf of other threads.
        SString * szName,
        SString * szDescription,
        SString * szXML,
        CorDebugMDAFlags flags
    ) :
        m_pThread(pThread),
        m_szName(szName),
        m_szDescription(szDescription),
        m_szXML(szXML),
        m_flags(flags)
    {
        LEAF_CONTRACT;
    }

};

//-----------------------------------------------------------------------------
// Helper to send a MDA notification from the helper thread in the jit-attach case.
// This will send the event and finish off the jit-attach.
// Parameters:
//    params - data to initialize the IPC event.
//-----------------------------------------------------------------------------
void Debugger::SendMDANotificationOnHelperThreadProxy(SendMDANotificationParams * params)
{
    HRESULT hr = S_OK;

    // This function is only called on helper thread and it should be still holding the
    // debugger lock since attach is not yet fully finished.
    //
    _ASSERTE(g_pDebugger->ThreadHoldsLock());

    g_pDebugger->SendRawMDANotification(params);

    // Stop all Runtime threads
    g_pDebugger->TrapAllRuntimeThreads(NULL, NULL);

    // Since we're still syncing for attach, send sync complete now and
    // mark that the debugger has completed attaching.
    hr = g_pDebugger->FinishEnsureDebuggerAttached();

    return;
}

//-----------------------------------------------------------------------------
// Actually send the MDA event. (Could be on any thread)
// Parameters:
//    params - data to initialize the IPC event.
//-----------------------------------------------------------------------------
void Debugger::SendRawMDANotification(
    SendMDANotificationParams * params
)
{
    // Send the unload assembly event to the Right Side.
    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);

    Thread * pThread = params->m_pThread;
    AppDomain *pAppDomain = (pThread != NULL) ? pThread->GetDomain() : NULL;

    InitIPCEvent(ipce,
                 DB_IPCE_MDA_NOTIFICATION,
                 pThread,
                 pAppDomain);

    SetLSBufferFromSString(&ipce->MDANotification.szName, *(params->m_szName));
    SetLSBufferFromSString(&ipce->MDANotification.szDescription, *(params->m_szDescription));
    SetLSBufferFromSString(&ipce->MDANotification.szXml, *(params->m_szXML));
    ipce->MDANotification.dwOSThreadId = GetCurrentThreadId();
    ipce->MDANotification.flags = params->m_flags;

    m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
}

//-----------------------------------------------------------------------------
// Send an MDA notification. This ultimately translates to an ICorDebugMDA object on the Right-Side.
// Called by EE to send a MDA debug event. This will block on the debug event
// until the RS continues us.
// Debugger may or may not be attached. If bAttached, then this
// will trigger a jitattach as well.
// See MDA documentation for what szName, szDescription + szXML should look like.
// The debugger just passes them through.
//
// Parameters:
//   pThread - thread for debug event.  May be null.
//   szName - short name of MDA.
//   szDescription - full description of MDA.
//   szXML - xml string for MDA.
//-----------------------------------------------------------------------------
void Debugger::SendMDANotification(
    Thread * pThread, // may be NULL. Lets us send on behalf of other threads.
    SString * szName,
    SString * szDescription,
    SString * szXML,
    CorDebugMDAFlags flags,
    BOOL bAttach
)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    PREFIX_ASSUME(szName != NULL);
    PREFIX_ASSUME(szDescription != NULL);
    PREFIX_ASSUME(szXML != NULL);

    // Note: we normally don't send events like this when there is an unrecoverable error. However,
    // if a host attempts to setup fiber mode on a thread, then we'll set an unrecoverable error
    // and use an MDA to 1) tell the user and 2) get the Right Side to notice the unrecoverable error.
    // Therefore, we'll go ahead and send a MDA event if the unrecoverable error is
    // CORDBG_E_CANNOT_DEBUG_FIBER_PROCESS.
    DebuggerIPCControlBlock *pDCB = m_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC);


    // If the MDA is ocuring very early in startup before the DCB is setup, then bail.
    if (pDCB == NULL)
    {
        return;
    }

    if (CORDBUnrecoverableError(this) && (pDCB->m_errorHR != CORDBG_E_CANNOT_DEBUG_FIBER_PROCESS))
    {
        return;
    }

    // Validate flags. Make sure that folks don't start passing flags that we don't handle.
    // If pThread != current thread, caller should either pass in MDA_FLAG_SLIP or guarantee
    // that pThread is not slipping.
    _ASSERTE((flags & ~(MDA_FLAG_SLIP)) == 0);

    // Helper thread should not be triggering MDAs. The helper thread is executing code in a very constrained
    // and controlled region and shouldn't be able to do anything dangerous.
    // If we revise this in the future, we should probably just post the event to the RS w/ use the MDA_FLAG_SLIP flag,
    // and then not bother suspending the runtime. The RS will get it on its next event.
    // The jit-attach logic below assumes we're not on the helper. (If we are on the helper, then a debugger should already
    // be attached)
    if (ThisIsHelperThreadWorker())
    {
        CONSISTENCY_CHECK_MSGF(false, ("MDA '%s' fired on *helper* thread.\r\nDesc:%s",
            szName->GetUnicode(), szDescription->GetUnicode()
        ));

        return;
    }

    // Public entry point into the debugger. May cause a jit-attach, so we may need to be lazily-init.
    if (!HasLazyData())
    {
        DebuggerLockHolder dbgLockHolder(this);
        // This is an entry path into the debugger, so make sure we're inited.
        LazyInit();
    }


    // Cases:
    // 1) Debugger already attached, send event normally (ignore severity)
    // 2) No debugger attached, Non-severe probe - ignore.
    // 3) No debugger attached, Severe-probe - do a jit-attach.
    HRESULT hr = S_OK;
    bool bStillInAttachLogic = false;
    bool fTryJitAttach = bAttach == TRUE;

    // Check case #2 - no debugger, and no jit-attach. Early opt out.
    if (!IsDebuggerAttached() && !fTryJitAttach)
    {
        return;
    }


    bool disabled = false;

    if (pThread != NULL)
    {
        g_pEEInterface->IsPreemptiveGCDisabled();

        if (disabled)
        {
            g_pEEInterface->EnablePreemptiveGC();
        }
    }
    else
    {
        // If there's no thread object, then we're not blocking after the event,
        // and thus this probe may slip.
        flags = (CorDebugMDAFlags) (flags | MDA_FLAG_SLIP);
    }

    // For "Severe" probes, we'll do a jit attach dialog
    if (fTryJitAttach)
    {
        // May return:
        // - S_OK if we do a jit-attach,
        // - S_FALSE if a debugger is already attached.
        // - Error in other cases..
        
        StackSString sszResourcef;
        StackSString sszNotice;
        sszResourcef.LoadResource(MDARC_JIT_ATTACH, FALSE);
        ASSERT(!sszResourcef.IsEmpty());
        sszNotice.PPrintf(sszResourcef, szName->GetUnicode());

        hr = EnsureDebuggerAttached((pThread != NULL) ? pThread->GetDomain() : NULL,
                                    sszNotice.GetUnicode(), TRUE);
        if (hr == S_OK)
        {
            bStillInAttachLogic = true;
        }
    }

    if (SUCCEEDED(hr))
    {
        SendMDANotificationParams params(pThread, szName, szDescription, szXML, flags);


        if (bStillInAttachLogic)
        {
            // In this case, let the helper thread to send the proper event and finish
            // the attach logic for managed thread!

            m_pRCThread->DoFavor((FAVORCALLBACK) SendMDANotificationOnHelperThreadProxy, &params);
        }
        else
        {
            if (IsDebuggerAttached())
            {
                // Non-attach case. Send like normal event.
                // This includes if someone launch the debugger during the meantime.
                // just send the event
                DebuggerLockHolder dbgLockHolder(this, FALSE);
                LockForEventSending(&dbgLockHolder, hr == S_OK);

                // Send Log message event to the Right Side
                SendRawMDANotification(&params);

                // Stop all Runtime threads
                // Even if we don't have a managed thead object, this will catch us at the next good spot.
                TrapAllRuntimeThreads(&dbgLockHolder, NULL);

                // Let other Runtime threads handle their events.
                UnlockFromEventSending(&dbgLockHolder);
            }
        }
    }

    if (pThread != NULL)
    {
        // Set back to disabled GC
        g_pEEInterface->DisablePreemptiveGC();

        if (!disabled)
        {
            g_pEEInterface->EnablePreemptiveGC();
        }
    }
}


//*************************************************************
// structure that we use in do-favor for SendLogMessage on helper thread
//*************************************************************
struct SendLogMessageOnHelperThreadParams
{
    Debugger *m_pThis;
    Thread   *m_pThread;
    AppDomain *m_pAppDomain;
    int     m_iLevel;
    WCHAR   *m_pCategory;
    int     m_iCategoryLen;
    WCHAR   *m_pMessage;
    int     m_iMessageLen;
    HRESULT         m_retval;

    SendLogMessageOnHelperThreadParams() :
        m_pThis(NULL), m_pThread(NULL), m_pAppDomain(NULL), m_retval(S_OK) {LEAF_CONTRACT; }

};

//*************************************************************
//
// This function will be executed on native debugger helper thread. It unbundled parameter and
// call the true helper function.
// Proxy method to call the function on Debugger class.
//
//*************************************************************
void SendRawLogMessageOnHelperThreadProxy(SendLogMessageOnHelperThreadParams *p)
{
    WRAPPER_CONTRACT;
    _ASSERTE(p->m_pThis != NULL && p->m_pThread != NULL && p->m_pAppDomain != NULL);

    STRESS_LOG0(LF_CORDB, LL_INFO10, "SendRawUserBreakPointOnHelperThreadProxy\n");

    p->m_retval = p->m_pThis->SendRawLogMessageOnHelperThread(p->m_pThread,
                                                              p->m_pAppDomain,
                                                              p->m_iLevel,
                                                              p->m_pCategory,
                                                              p->m_iCategoryLen,
                                                              p->m_pMessage,
                                                              p->m_iMessageLen);
}

//*************************************************************
//
// This function will send LogMessage event and finish the attach logic
// on native debugger helper thread.
//
//*************************************************************
HRESULT Debugger::SendRawLogMessageOnHelperThread(
    Thread                                     *pThread,
    AppDomain                                  *pAppDomain,
    int                                         iLevel,
    __in_z __in_ecount(iCategoryLen + 1) WCHAR *pCategory,
    int                                         iCategoryLen,
    __in_ecount(iMessageLen + 1) WCHAR         *pMessage,
    int                                         iMessageLen)
{
    // This function is only called on helper thread and it should be still holding the
    // debugger lock since attach is not yet fully finished.
    //
    _ASSERTE(ThreadHoldsLock());

    // Send a user breakpoint event to the Right Side
    SendRawLogMessage(pThread, pAppDomain, iLevel, pCategory, iCategoryLen, pMessage, iMessageLen);

    // Stop all Runtime threads
    TrapAllRuntimeThreads(NULL, pAppDomain);

    // Since we're still syncing for attach, send sync complete now and
    // mark that the debugger has completed attaching.
    return FinishEnsureDebuggerAttached();
}


//*************************************************************
// This method sends a log message over to the right side for the debugger to log it.
//*************************************************************
void Debugger::SendLogMessage(int iLevel,
                              __in_z __in_ecount(iCategoryLen + 1) WCHAR *pCategory,
                              int iCategoryLen,
                              __in_ecount(iMessageLen + 1) WCHAR *pMessage,
                              int iMessageLen)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
    }
    CONTRACTL_END;

    bool disabled;

    LOG((LF_CORDB, LL_INFO10000, "D::SLM: Sending log message.\n"));

    // Send the message only if the debugger is attached to this appdomain.
    // Note the the debugger may detach at any time, so we'll have to check
    // this again after we get the lock.
    AppDomain *pAppDomain = g_pEEInterface->GetThread()->GetDomain();

    if (!pAppDomain->IsDebuggerAttached())
    {
        return;
    }

    disabled = g_pEEInterface->IsPreemptiveGCDisabled();

    if (disabled)
    {
        g_pEEInterface->EnablePreemptiveGC();
    }

    // Hold the debugger lock for the duration of this block
    {
        DebuggerLockHolder dbgLockHolder(this, FALSE);
        LockForEventSending(&dbgLockHolder, TRUE);

        // Send Log message event to the Right Side
        SendRawLogMessage(g_pEEInterface->GetThread(), pAppDomain, iLevel, pCategory, iCategoryLen, pMessage, iMessageLen);

        // Stop all Runtime threads
        TrapAllRuntimeThreads(&dbgLockHolder, pAppDomain);

        // Let other Runtime threads handle their events.
        UnlockFromEventSending(&dbgLockHolder);
    }

    // Set back to disabled GC
    g_pEEInterface->DisablePreemptiveGC();

    if (!disabled)
    {
        g_pEEInterface->EnablePreemptiveGC();
    }

}


//*************************************************************
//
// Helper function to just send LogMessage event. Can be called on either
// helper thread or managed thread.
//
//*************************************************************
void Debugger::SendRawLogMessage(
    Thread                                    *pThread,
    AppDomain                                 *pAppDomain,
    int                                        iLevel,
    __in_z __in_ecount(iCategoryLen+1) WCHAR  *pCategory,
    int                                        iCategoryLen,
    __in_ecount(iMessageLen+1) WCHAR          *pMessage,
    int                                        iMessageLen // length of pMessage in chars, not including null
)
{
    DebuggerIPCEvent* ipce;


    // @@@
    // We should have hold debugger lock
    // This can happen on either native helper thread or managed thread
    _ASSERTE(ThreadHoldsLock());

    // It's possible that the debugger dettached while we were waiting
    // for our lock. Check again and abort the event if it did.
    if (!pAppDomain->IsDebuggerAttached())
    {
        return;
    }

    ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);

    // Send a LogMessage event to the Right Side
    InitIPCEvent(ipce,
                 DB_IPCE_FIRST_LOG_MESSAGE,
                 pThread,
                 pAppDomain);

    ipce->FirstLogMessage.iLevel = iLevel;
    ipce->FirstLogMessage.szCategory.SetString(pCategory);
    ipce->FirstLogMessage.szContent.SetLsData((BYTE *)pMessage, (iMessageLen +1) * sizeof(WCHAR));

    m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

    // If it's a panic-level log, also explicitly send a user breakpoint event to the Right Side.
    if (iLevel == PanicLevel)
    {
        SendRawUserBreakpoint(pThread);
    }
}


// This function sends a message to the right side informing it about
// the creation/modification of a LogSwitch
void Debugger::SendLogSwitchSetting(int iLevel,
                                    int iReason,
                                    __in_z WCHAR *pLogSwitchName,
                                    __in_z WCHAR *pParentSwitchName)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000, "D::SLSS: Sending log switch message switch=%S parent=%S.\n",
        pLogSwitchName, pParentSwitchName));

    // Send the message only if the debugger is attached to this appdomain.
    AppDomain *pAppDomain = g_pEEInterface->GetThread()->GetDomain();

    if (!pAppDomain->IsDebuggerAttached())
        return;

    bool disabled = g_pEEInterface->IsPreemptiveGCDisabled();

    if (disabled)
        g_pEEInterface->EnablePreemptiveGC();

    // Prevent other Runtime threads from handling events.
    DebuggerLockHolder dbgLockHolder(this, FALSE);

    LockForEventSending(&dbgLockHolder);

    if (pAppDomain->IsDebuggerAttached())
    {
        DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce,
                     DB_IPCE_LOGSWITCH_SET_MESSAGE,
                     g_pEEInterface->GetThread(),
                     g_pEEInterface->GetThread()->GetDomain());

        ipce->LogSwitchSettingMessage.iLevel = iLevel;
        ipce->LogSwitchSettingMessage.iReason = iReason;


        ipce->LogSwitchSettingMessage.szSwitchName.SetString(pLogSwitchName);

        if (pParentSwitchName == NULL)
        {
            pParentSwitchName = L"";
        }

        ipce->LogSwitchSettingMessage.szParentSwitchName.SetString(pParentSwitchName);

        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

        // Stop all Runtime threads
        TrapAllRuntimeThreads(&dbgLockHolder, pAppDomain);
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::SLSS: Skipping SendIPCEvent because RS detached."));
    }

    UnlockFromEventSending(&dbgLockHolder);

    g_pEEInterface->DisablePreemptiveGC();

    if (!disabled)
        g_pEEInterface->EnablePreemptiveGC();
}

/******************************************************************************
 * Add the AppDomain to the list stored in the IPC block.
 ******************************************************************************/
HRESULT Debugger::AddAppDomainToIPC(AppDomain *pAppDomain)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    LPCWSTR szName = NULL;

    LOG((LF_CORDB, LL_INFO100, "D::AADTIPC: Executing AADTIPC for AppDomain 0x%08x (0x%x).\n",
        pAppDomain,
        pAppDomain->GetId().m_dwId));

    STRESS_LOG2(LF_CORDB, LL_INFO10000, "D::AADTIPC: AddAppDomainToIPC:%#08x, %#08x\n",
            pAppDomain, pAppDomain->GetId().m_dwId);



    _ASSERTE(m_pAppDomainCB->m_iTotalSlots > 0);
    _ASSERTE(m_pAppDomainCB->m_rgListOfAppDomains != NULL);

    {
        //
        // We need to synchronize this routine with the attach logic.  The "normal"
        // attach case uses the HelperThread and TrapAllRuntimeThreads to synchronize
        // the runtime before sending any of the events (including AppDomainCreates)
        // to the right-side.  Thus, we can synchronize with this case by forcing us
        // to go co-operative.  If we were already co-op, then the helper thread will
        // wait to start the attach until all co-op threads are paused.  If we were
        // pre-emptive, then going co-op will suspend us until the HelperThread finishes.
        //
        // The second case is under the IPC event for ATTACHING, which is where there are
        // zero app domains, so it is considered an 'early attach' case.  To synchronize
        // with this we have to grab and hold the AppDomainDB lock.
        //

        GCX_COOP();

        // Lock the list
        if (!m_pAppDomainCB->Lock())
            return E_FAIL;

        // Get a free entry from the list
        AppDomainInfo *pADInfo = m_pAppDomainCB->GetFreeEntry();

        // Function returns NULL if the list is full and a realloc failed.
        if (!pADInfo)
        {
            hr = E_OUTOFMEMORY;
            goto ErrExit;
        }

        // copy the ID
        pADInfo->m_id = pAppDomain->GetId().m_dwId;

        // Now set the AppDomainName.

        /*
         * FIXME :
         * nickbe 04/17/2003 05:33:55
         *
         * Make sure that returning NULL here does not result in a catastrophic
         * failure.
         *
         * GetFriendlyNameNoThrow may call SetFriendlyName, which may call
         * UpdateAppDomainEntryInIPC. There is no recursive death, however, because
         * the AppDomainInfo object does not contain a pointer to the app domain
         * yet.
         */
        szName = pAppDomain->GetFriendlyNameForDebugger();
        pADInfo->SetName(szName);

        // Save on to the appdomain pointer
        pADInfo->m_pAppDomain = pAppDomain;

        // bump the used slot count
        m_pAppDomainCB->m_iNumOfUsedSlots++;

ErrExit:
        // UnLock the list
        m_pAppDomainCB->Unlock();

        // Send event to debugger if one is attached.  Don't send the event if a debugger is already attached to
        // the domain, since the debugger could have attached to the process and domain in the time it takes
        // between creating the domain and when we notify the debugger.
        if (IsDebuggerAttached() && !pAppDomain->IsDebuggerAttached())
        {
            SendCreateAppDomainEvent(pAppDomain, FALSE);
        }

    }

    return hr;
}


/******************************************************************************
 * Remove the AppDomain from the list stored in the IPC block and send an ExitAppDomain 
 * event to the debugger if attached.
 ******************************************************************************/
HRESULT Debugger::RemoveAppDomainFromIPC (AppDomain *pAppDomain)
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    HRESULT hr = E_FAIL;

    LOG((LF_CORDB, LL_INFO100, "D::RADFIPC: Executing RADFIPC for AppDomain 0x%08x (0x%x).\n",
        pAppDomain,
        pAppDomain->GetId().m_dwId));

    // if none of the slots are occupied, then simply return.
    if (m_pAppDomainCB->m_iNumOfUsedSlots == 0)
        return hr;

    // Lock the list
    if (!m_pAppDomainCB->Lock())
        return (E_FAIL);


    // Look for the entry
    AppDomainInfo *pADInfo = m_pAppDomainCB->FindEntry(pAppDomain);

    // Shouldn't be trying to remove an appdomain that was never added
    if (!pADInfo)
    {
        // We'd like to assert this, but there is a small window where we may have
        // called AppDomain::Init (and so it's fair game to call Stop, and hence come here),
        // but not yet published the app domain.
        // _ASSERTE(!"D::RADFIPC: trying to remove an AppDomain that was never added");
        hr = (E_FAIL);
        goto ErrExit;
    }

    // Release the entry
    m_pAppDomainCB->FreeEntry(pADInfo);

ErrExit:
    // UnLock the list
    m_pAppDomainCB->Unlock();

    // send event to debugger if one is attached
    if (IsDebuggerAttached())
    {
        SendExitAppDomainEvent(pAppDomain);
    }

    return hr;
}

/******************************************************************************
 * Update the AppDomain in the list stored in the IPC block.
 ******************************************************************************/
HRESULT Debugger::UpdateAppDomainEntryInIPC(AppDomain *pAppDomain)
{
    CONTRACTL
    {
        NOTHROW;
        if (GetThread()) { GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    LPCWSTR szName = NULL;

    LOG((LF_CORDB, LL_INFO100,
         "D::UADEIIPC: Executing UpdateAppDomainEntryInIPC ad:0x%x.\n",
         pAppDomain));

    // if none of the slots are occupied, then simply return.
    if (m_pAppDomainCB->m_iNumOfUsedSlots == 0)
        return (E_FAIL);

    // Lock the list
    if (!m_pAppDomainCB->Lock())
        return (E_FAIL);

    // Look up the info entry
    AppDomainInfo *pADInfo = m_pAppDomainCB->FindEntry(pAppDomain);

    if (!pADInfo)
    {
        hr = E_FAIL;
        goto ErrExit;
    }

    // Update the name only if new name is non-null
    szName = pADInfo->m_pAppDomain->GetFriendlyNameForDebugger();
    pADInfo->SetName(szName);

    LOG((LF_CORDB, LL_INFO100,
         "D::UADEIIPC: New name:%ls (AD:0x%x)\n", pADInfo->m_szAppDomainName,
         pAppDomain));

ErrExit:
    // UnLock the list
    m_pAppDomainCB->Unlock();

    return hr;
}

/******************************************************************************
 * When attaching to a process, this is called to enumerate all of the
 * AppDomains currently in the process and communicate that information to the
 * debugger.
 ******************************************************************************/
HRESULT Debugger::IterateAppDomainsForAttach(
    AttachAppDomainEventsEnum EventsToSend,
    BOOL *fEventSent, BOOL fAttaching)
{
    CONTRACTL
    {
        THROWS;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
        SO_NOT_MAINLINE;

        PRECONDITION(ThisIsHelperThread());
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    _ASSERTE(fAttaching == TRUE);

#ifdef LOGGING
    static const char *(ev[]) = {"all", "app domain create", "don't send class events", "only send class events"};
#endif
    LOG((LF_CORDB, LL_INFO100, "EEDII::IADFA: Entered function IterateAppDomainsForAttach() isAttaching:%d Events:%s\n", fAttaching, ev[EventsToSend]));
    HRESULT hr = S_OK;

    int flags = 0;
    switch (EventsToSend)
    {
    case SEND_ALL_EVENTS:
        flags = ATTACH_ALL;
        break;
    case ONLY_SEND_APP_DOMAIN_CREATE_EVENTS:
        flags = 0;
        break;
    case DONT_SEND_CLASS_EVENTS:
        flags = ATTACH_ASSEMBLY_LOAD | ATTACH_MODULE_LOAD;
        break;
    case ONLY_SEND_CLASS_EVENTS:
        flags = ATTACH_CLASS_LOAD;
        break;
    default:
        _ASSERTE(!"unknown enum");
    }

    // Lock the list
    if (!m_pAppDomainCB->Lock())
        return (E_FAIL);

    // Iterate through the app domains
    AppDomainInfo *pADInfo = m_pAppDomainCB->FindFirst();

    while (pADInfo)
    {
        LOG((LF_CORDB, LL_INFO100, "EEDII::IADFA: Iterating over domain %#08x AD:%#08x %ls\n", pADInfo->m_pAppDomain->GetId().m_dwId, pADInfo->m_pAppDomain, pADInfo->m_szAppDomainName));

        // Send CreateAppDomain events for each app domain
        if (EventsToSend == ONLY_SEND_APP_DOMAIN_CREATE_EVENTS)
        {
            LOG((LF_CORDB, LL_INFO100, "EEDII::IADFA: Sending AppDomain Create Event for 0x%08x\n",pADInfo->m_pAppDomain->GetId().m_dwId));
            SendCreateAppDomainEvent(pADInfo->m_pAppDomain, fAttaching);

            *fEventSent = TRUE;
        }
        else
        {
            DWORD dwFlags = pADInfo->m_pAppDomain->GetDebuggerAttached();

            if ((dwFlags == AppDomain::DEBUGGER_ATTACHING) ||
                (dwFlags == AppDomain::DEBUGGER_ATTACHING_THREAD &&
                    EventsToSend == ONLY_SEND_CLASS_EVENTS))
            {
                LOG((LF_CORDB, LL_INFO100, "EEDII::IADFA: Mark as attaching thread for 0x%08x\n",pADInfo->m_pAppDomain->GetId().m_dwId));

                // Send Load events for the assemblies, modules, and/or classes
                // We have to remember if any event needs it's 'synch complete'
                // msg to be sent later.
                *fEventSent = pADInfo->m_pAppDomain->
                    NotifyDebuggerLoad(flags, fAttaching) || *fEventSent;

                pADInfo->m_pAppDomain->SetDebuggerAttached(
                    AppDomain::DEBUGGER_ATTACHING_THREAD);

                hr = S_OK;
            }
            else
            {
                LOG((LF_CORDB, LL_INFO100, "EEDII::IADFA: Doing nothing for 0x%08x\n",pADInfo->m_pAppDomain->GetId().m_dwId));
            }
        }

        // Get the next appdomain in the list
        pADInfo = m_pAppDomainCB->FindNext(pADInfo);
    }

    // Unlock the list
    m_pAppDomainCB->Unlock();

    LOG((LF_CORDB, LL_INFO100, "EEDII::IADFA: Exiting function IterateAppDomainsForAttach\n"));

    return hr;
}

/******************************************************************************
 * Attach the debugger to a specific appdomain given its id.
 ******************************************************************************/
HRESULT Debugger::AttachDebuggerToAppDomain(ULONG id)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000, "EEDII:ADTAD: Entered function AttachDebuggerToAppDomain 0x%08x()\n", id));

    HRESULT hr = S_OK;

    // Lock the list
    if (!m_pAppDomainCB->Lock())
        return (E_FAIL);

    // Iterate through the app domains
    AppDomainInfo *pADInfo = m_pAppDomainCB->FindFirst();

    hr = E_FAIL;

    while (pADInfo)
    {
        if (pADInfo->m_pAppDomain->GetId().m_dwId == id)
        {
            LOG((LF_CORDB, LL_INFO1000, "EEDII:ADTAD: Marked AppDomain 0x%08x as attaching\n", id));
            pADInfo->m_pAppDomain->SetDebuggerAttached(AppDomain::DEBUGGER_ATTACHING);

            hr = S_OK;
            break;
        }

        // Get the next appdomain in the list
        pADInfo = m_pAppDomainCB->FindNext(pADInfo);
    }

    // Unlock the list
    m_pAppDomainCB->Unlock();

    return hr;
}


// return true if we're attaching, but not yet fully attached. This will cover the window when the debugger
// is sending faked up appdomain, assembly, module, class and thread load events.
// We explicitily don't check other sync_states because those may be used in the non-attach case.
bool Debugger::IsAttachInProgress()
{
    return (m_syncingForAttach >= SYNC_STATE_1 && m_syncingForAttach <= SYNC_STATE_3);
}
/******************************************************************************
 * Mark any appdomains that we are in the process of attaching to as attached
 ******************************************************************************/
HRESULT Debugger::MarkAttachingAppDomainsAsAttachedToDebugger(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000, "EEDII:MAADAATD: Entered function MarkAttachingAppDomainsAsAttachedToDebugger\n"));

    HRESULT hr = S_OK;

    // Lock the list
    if (!m_pAppDomainCB->Lock())
        return (E_FAIL);

    // Iterate through the app domains
    AppDomainInfo *pADInfo = m_pAppDomainCB->FindFirst();

    hr = E_FAIL;

    while (pADInfo)
    {
        if (pADInfo->m_pAppDomain->GetDebuggerAttached() == AppDomain::DEBUGGER_ATTACHING_THREAD)
        {
            pADInfo->m_pAppDomain->SetDebuggerAttached(AppDomain::DEBUGGER_ATTACHED);

            LOG((LF_CORDB, LL_INFO10000, "EEDII:MAADAATD: AppDomain 0x%08x (0x%x) marked as attached\n",
                pADInfo->m_pAppDomain,
                pADInfo->m_pAppDomain->GetId().m_dwId));
        }

        // Get the next appdomain in the list
        pADInfo = m_pAppDomainCB->FindNext(pADInfo);
    }

    // Unlock the list
    m_pAppDomainCB->Unlock();

    return hr;
}


/******************************************************************************
 * Detach the debugger from a specific appdomain given its id.
 ******************************************************************************/
HRESULT Debugger::DetachDebuggerFromAppDomain(ULONG id, AppDomain **ppAppDomain)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    // Lock the list
    if (!m_pAppDomainCB->Lock())
        return (E_FAIL);

    // Iterate through the app domains
    AppDomainInfo *pADInfo = m_pAppDomainCB->FindFirst();

    while (pADInfo)
    {
        if (pADInfo->m_pAppDomain->GetId().m_dwId == id)
        {
            pADInfo->m_pAppDomain->SetDebuggerAttached(AppDomain::DEBUGGER_NOT_ATTACHED);
            (*ppAppDomain) = pADInfo->m_pAppDomain;
            break;
        }

        // Get the next appdomain in the list
        pADInfo = m_pAppDomainCB->FindNext(pADInfo);
    }

    // Unlock the list
    m_pAppDomainCB->Unlock();

    return hr;
}


/******************************************************************************
 *
 ******************************************************************************/
HRESULT Debugger::InitAppDomainIPC(void)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_INTOLERANT;

        PRECONDITION(CheckPointer(m_pAppDomainCB));
    }
    CONTRACTL_END;

    // Ensure that if we throw here, the Terminate will get called and cleanup all resources.
    // This will make Init an atomic operation - it either fully inits or fully fails.
    class EnsureCleanup
    {
        Debugger * m_pThis;

    public:
        EnsureCleanup(Debugger * pThis)
        {
            m_pThis = pThis;
        }

        void SupressCleanup()
        {
            m_pThis = NULL;
        }

        ~EnsureCleanup()
        {
            if (m_pThis != NULL)
            {
                m_pThis->TerminateAppDomainIPC();
            }
        }
    } hEnsureCleanup(this);

    DWORD dwStrLen = 0;
    WCHAR szExeName[MAX_PATH];
    int i;

    // all fields in the object can be zero initialized.
    // If we throw, before fully initializing this, then cleanup won't try to free
    // uninited values.
    ZeroMemory(m_pAppDomainCB, sizeof(*m_pAppDomainCB));

    Thread::BeginThreadAffinity();

    // Create a mutex to allow the Left and Right Sides to properly
    // synchronize. The Right Side will spin until m_hMutex is valid,
    // then it will acquire it before accessing the data.

    HandleHolder hMutex(WszCreateMutex(NULL, TRUE/*hold*/, NULL));
    if (hMutex == NULL)
    {
        ThrowLastError();
    }
    if (!m_pAppDomainCB->m_hMutex.SetLocal(hMutex))
    {
        ThrowLastError();
    }
    hMutex.SuppressRelease();

    m_pAppDomainCB->m_iSizeInBytes = INITIAL_APP_DOMAIN_INFO_LIST_SIZE *
                                                sizeof (AppDomainInfo);

    // Number of slots in AppDomainListElement array
    m_pAppDomainCB->m_rgListOfAppDomains = new AppDomainInfo[INITIAL_APP_DOMAIN_INFO_LIST_SIZE];
    _ASSERTE(m_pAppDomainCB->m_rgListOfAppDomains != NULL); // throws on oom


    m_pAppDomainCB->m_iTotalSlots = INITIAL_APP_DOMAIN_INFO_LIST_SIZE;

    // Initialize each AppDomainListElement
    for (i = 0; i < INITIAL_APP_DOMAIN_INFO_LIST_SIZE; i++)
    {
        m_pAppDomainCB->m_rgListOfAppDomains[i].FreeEntry();
    }

    // also initialize the process name
    dwStrLen = WszGetModuleFileName(NULL,
                                    szExeName,
                                    MAX_PATH);

    // If we couldn't get the name, then use a nice default.
    if (dwStrLen == 0)
    {
        wcscpy_s(szExeName, COUNTOF(szExeName), L"<NoProcessName>");
        dwStrLen = (DWORD)wcslen(szExeName);
    }

    // If we got the name, copy it into a buffer. dwStrLen is the
    // count of characters in the name, not including the null
    // terminator.
    m_pAppDomainCB->m_szProcessName = new WCHAR[dwStrLen + 1];
    _ASSERTE(m_pAppDomainCB->m_szProcessName != NULL); // throws on oom

    wcscpy_s(m_pAppDomainCB->m_szProcessName, dwStrLen + 1, szExeName);

    // Add 1 to the string length so the Right Side will copy out the
    // null terminator, too.
    m_pAppDomainCB->m_iProcessNameLengthInBytes = (dwStrLen + 1) * sizeof(WCHAR);

    if (m_pAppDomainCB->m_hMutex != NULL)
    {
        m_pAppDomainCB->Unlock();
    }

    hEnsureCleanup.SupressCleanup();
    return S_OK;
}

/******************************************************************************
 * Unitialize the AppDomain IPC block
 * Returns:
 * S_OK -if fully unitialized
 * E_FAIL - if we can't get ownership of the block, and thus no unitialization
 *          work is done.
 ******************************************************************************/
HRESULT Debugger::TerminateAppDomainIPC(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    if (m_pAppDomainCB == NULL)
        return S_OK;

    HRESULT hr = S_OK;

    // Lock the list
    // If there's no mutex, then we're in a partially created state.
    // This means InitAppDomainIPC failed halfway through. But we're still thread safe
    // since other threads can't access us if we don't have the mutex.
    if ((m_pAppDomainCB->m_hMutex != NULL) && !m_pAppDomainCB->Lock())
    {
        // The callers don't check our return value, we may want to know when we can't gracefully clean up
       LOG((LF_CORDB, LL_INFO10, "Debugger::TerminateAppDomainIPC: Failed to get AppDomain IPC lock, not cleaning up.\n"));

        // If the lock is valid, but we can't get it, then we can't really
        // uninitialize since someone else is using the block.
        return (E_FAIL);
    }

    // The shared IPC segment could still be around after the debugger
    // object has been destroyed during process shutdown. So, reset
    // the UsedSlots count to 0 so that any out of process clients
    // enumeratingthe app domains in this process see 0 AppDomains.
    m_pAppDomainCB->m_iNumOfUsedSlots = 0;
    m_pAppDomainCB->m_iTotalSlots = 0;

    // Now delete the memory alloacted for AppDomainInfo  array
    delete [] m_pAppDomainCB->m_rgListOfAppDomains;
    m_pAppDomainCB->m_rgListOfAppDomains = NULL;

    delete [] m_pAppDomainCB->m_szProcessName;
    m_pAppDomainCB->m_szProcessName = NULL;
    m_pAppDomainCB->m_iProcessNameLengthInBytes = 0;

    // Set the mutex handle to NULL.
    // If the Right Side acquires the mutex, it will verify
    // that the handle is still not NULL. If it is, then it knows it
    // really lost.
    RemoteHANDLE m = m_pAppDomainCB->m_hMutex;
    m_pAppDomainCB->m_hMutex.m_hLocal = NULL;

    // And bring us back to a fully unintialized state.
    ZeroMemory(m_pAppDomainCB, sizeof(*m_pAppDomainCB));

    // We're done. release and close the mutex.  Note that this must be done
    // after we clear it out above to ensure there is no race condition.
    if( m != NULL ) {
        VERIFY(ReleaseMutex(m));
        m.Close();
    }

    return hr;
}


#ifndef DACCESS_COMPILE

//
// FuncEvalSetup sets up a function evaluation for the given method on the given thread.
//
HRESULT Debugger::FuncEvalSetup(DebuggerIPCE_FuncEvalInfo *pEvalInfo,
                                BYTE **argDataArea,
                                DebuggerEval **debuggerEvalKey)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    Thread *pThread = pEvalInfo->funcDebuggerThreadToken.UnWrap();


    //
    // If TS_AbortRequested (which may have been set by a pending FuncEvalAbort),
    // we will not be able to do a new func-eval
    //
    if (pThread->m_State & Thread::TS_AbortRequested)
        return CORDBG_E_FUNC_EVAL_BAD_START_POINT;

    if (g_fProcessDetach)
        return CORDBG_E_FUNC_EVAL_BAD_START_POINT;


    bool fInException = pEvalInfo->evalDuringException;

    // The thread has to be at a GC safe place for now, just in case the func eval causes a collection. Processing an
    // exception also counts as a "safe place." Eventually, we'd like to have to avoid this check and eval anyway, but
    // that's a way's off...
    if (!fInException && !g_pDebugger->IsThreadAtSafePlace(pThread))
        return CORDBG_E_ILLEGAL_AT_GC_UNSAFE_POINT;

    // For now, we assume that the target thread must be stopped in managed code due to a single step or a
    // breakpoint. Being stopped while sending a first or second chance exception is also valid, and there may or may
    // not be a filter context when we do a func eval from such places. This will loosen over time, eventually allowing
    // threads that are stopped anywhere in managed code to perform func evals.
    CONTEXT *filterContext = GetManagedStoppedCtx(pThread);

    if (filterContext == NULL && !fInException)
    {
        return CORDBG_E_ILLEGAL_AT_GC_UNSAFE_POINT;
    }

    // Create a DebuggerEval to hold info about this eval while its in progress. Constructor copies the thread's
    // CONTEXT.
    DebuggerEval *pDE = new (interopsafe, nothrow) DebuggerEval(filterContext, pEvalInfo, fInException);

    if (pDE == NULL)
    {
        return E_OUTOFMEMORY;
    }
    else if (!pDE->Init())
    {
        // We fail to change the m_breakpointInstruction field to PAGE_EXECUTE_READWRITE permission.
        return E_FAIL;
    }

    SIZE_T argDataAreaSize = 0;

    argDataAreaSize += pEvalInfo->genericArgsNodeCount * sizeof(DebuggerIPCE_TypeArgData);

    if ((pEvalInfo->funcEvalType == DB_IPCE_FET_NORMAL) ||
        (pEvalInfo->funcEvalType == DB_IPCE_FET_NEW_OBJECT) ||
        (pEvalInfo->funcEvalType == DB_IPCE_FET_NEW_OBJECT_NC))
        argDataAreaSize += pEvalInfo->argCount * sizeof(DebuggerIPCE_FuncEvalArgData);
    else if (pEvalInfo->funcEvalType == DB_IPCE_FET_NEW_STRING)
        argDataAreaSize += pEvalInfo->stringSize;
    else if (pEvalInfo->funcEvalType == DB_IPCE_FET_NEW_ARRAY)
        argDataAreaSize += pEvalInfo->arrayRank * sizeof(SIZE_T);

    if (argDataAreaSize > 0)
    {
        pDE->m_argData = new (interopsafe, nothrow) BYTE[argDataAreaSize];

        if (pDE->m_argData == NULL)
        {
            DeleteInteropSafe(pDE);
            return E_OUTOFMEMORY;
        }

        // Pass back the address of the argument data area so the right side can write to it for us.
        *argDataArea = pDE->m_argData;
    }

    // Set the thread's IP (in the filter context) to our hijack function if we're stopped due to a breakpoint or single
    // step.
    if (!fInException)
    {
        _ASSERTE(filterContext != NULL);

        ::SetIP(filterContext, GetEEFuncEntryPoint(::FuncEvalHijack));

        // Don't be fooled into thinking you can push things onto the thread's stack now. If the thread is stopped at a
        // breakpoint or from a single step, then its really suspended in the SEH filter. ESP in the thread's CONTEXT,
        // therefore, points into the middle of the thread's current stack. So we pass things we need in the hijack in
        // the thread's registers.

        // Set the first argument to point to the DebuggerEval.
#if defined(_X86_)
        filterContext->Eax = (DWORD)pDE;
#elif defined(_PPC_)
        filterContext->Gpr11 = (DWORD)pDE;
#else
        PORTABILITY_ASSERT("Debugger::FuncEvalSetup is not implemented on this platform.");
#endif
    }
    else
    {
        HRESULT hr = CheckInitPendingFuncEvalTable();

        if (FAILED(hr))
        {
            DeleteInteropSafe(pDE);  // Note this runs the destructor for DebuggerEval, which releases its internal buffers
            return (hr);
        }
        // If we're in an exception, then add a pending eval for this thread. This will cause us to perform the func
        // eval when the user continues the process after the current exception event.
        GetPendingEvals()->AddPendingEval(pDE->m_thread, pDE);
    }

    //
    // To prevent GCs until the func-eval gets a chance to run, we increment the counter here.
    //
    g_pDebugger->IncThreadsAtUnsafePlaces();

    *debuggerEvalKey = pDE;

    LOG((LF_CORDB, LL_INFO100000, "D:FES for pDE:%08x evalType:%d on thread %#x, id=0x%x\n",
        pDE, pDE->m_evalType, pThread, GetThreadIdHelper(pThread)));

    return S_OK;
}

//
// FuncEvalSetupReAbort sets up a function evaluation specifically to rethrow a ThreadAbortException on the given
// thread.
//
HRESULT Debugger::FuncEvalSetupReAbort(Thread *pThread, Thread::ThreadAbortRequester requester)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000,
            "D::FESRA: performing reabort on thread %#x, id=0x%x\n",
            pThread, GetThreadIdHelper(pThread)));

    // The thread has to be at a GC safe place. It should be, since this is only done in response to a previous eval
    // completing with a ThreadAbortException.
    if (!g_pDebugger->IsThreadAtSafePlace(pThread))
        return CORDBG_E_ILLEGAL_AT_GC_UNSAFE_POINT;

    // Grab the filter context.
    CONTEXT *filterContext = GetManagedStoppedCtx(pThread);

    if (filterContext == NULL)
    {
        return CORDBG_E_ILLEGAL_AT_GC_UNSAFE_POINT;
    }

    // Create a DebuggerEval to hold info about this eval while its in progress. Constructor copies the thread's
    // CONTEXT.
    DebuggerEval *pDE = new (interopsafe, nothrow) DebuggerEval(filterContext, pThread, requester);

    if (pDE == NULL)
    {
        return E_OUTOFMEMORY;
    }
    else if (!pDE->Init())
    {
        // We fail to change the m_breakpointInstruction field to PAGE_EXECUTE_READWRITE permission.
        return E_FAIL;
    }

    // Set the thread's IP (in the filter context) to our hijack function.
    _ASSERTE(filterContext != NULL);

    ::SetIP(filterContext, GetEEFuncEntryPoint(::FuncEvalHijack));

#ifdef _X86_ // reliance on filterContext->Eip & Eax
    // Set EAX to point to the DebuggerEval.
    filterContext->Eax = (DWORD)pDE;

#elif defined(_PPC_)
    // Set r11 to point to the DebuggerEval.
    filterContext->Gpr11 = (DWORD)pDE;

#else
    PORTABILITY_ASSERT("FuncEvalSetupReAbort (Debugger.cpp) is not implemented on this platform.");
#endif

    // Now clear the bit requesting a re-abort
    pThread->ResetThreadStateNC(Thread::TSNC_DebuggerReAbort);

    g_pDebugger->IncThreadsAtUnsafePlaces();


    return S_OK;
}

//
// FuncEvalAbort: Does a gentle abort of a func-eval already in progress.
//    Because this type of abort waits for the thread to get to a good state,
//    it may never return, or may time out.
//

//
// Wait at most 0.5 seconds.
//
#define FUNC_EVAL_DEFAULT_TIMEOUT_VALUE 500

HRESULT
Debugger::FuncEvalAbort(
    DebuggerEval *debuggerEvalKey
    )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    DebuggerEval *pDE = (DebuggerEval*) debuggerEvalKey;
    HRESULT hr = S_OK;

    if (pDE->m_aborting == DebuggerEval::FE_ABORT_NONE)
    {
        // Remember that we're aborting this func eval.
        pDE->m_aborting = DebuggerEval::FE_ABORT_NORMAL;

        LOG((LF_CORDB, LL_INFO1000,
             "D::FEA: performing UserAbort on thread %#x, id=0x%x\n",
             pDE->m_thread, GetThreadIdHelper(pDE->m_thread)));

        if (!g_fProcessDetach && !pDE->m_completed)
        {
            //
            // Perform a stop on the thread that the eval is running on.
            // This will cause a ThreadAbortException to be thrown on the thread.
            //
            EX_TRY
            {
                hr = pDE->m_thread->UserAbort(Thread::TAR_FuncEval, EEPolicy::TA_Safe, (DWORD)FUNC_EVAL_DEFAULT_TIMEOUT_VALUE, Thread::UAC_Normal);
                if (hr == HRESULT_FROM_WIN32(ERROR_TIMEOUT))
                {
                    hr = S_OK;
                }
            }
            EX_CATCH
            {
                _ASSERTE(!"Unknown exception from UserAbort(), not expected");
                EX_RETHROW;
            }
            EX_END_CATCH(RethrowTerminalExceptions);

        }

        LOG((LF_CORDB, LL_INFO1000, "D::FEA: UserAbort complete.\n"));
    }

    return hr;
}

//
// FuncEvalRudeAbort: Does a rude abort of a func-eval in progress.  This
//     leaves the thread in an undetermined state.
//
HRESULT
Debugger::FuncEvalRudeAbort(
    DebuggerEval *debuggerEvalKey
    )
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    DebuggerEval *pDE = debuggerEvalKey;
    HRESULT hr = S_OK;

    if (!(pDE->m_aborting & DebuggerEval::FE_ABORT_RUDE))
    {
        //
        // Remember that we're aborting this func eval.
        //
        pDE->m_aborting = (DebuggerEval::FUNC_EVAL_ABORT_TYPE)(pDE->m_aborting | DebuggerEval::FE_ABORT_RUDE);

        LOG((LF_CORDB, LL_INFO1000,
             "D::FEA: performing RudeAbort on thread %#x, id=0x%x\n",
             pDE->m_thread, Debugger::GetThreadIdHelper(pDE->m_thread)));

        if (!g_fProcessDetach && !pDE->m_completed)
        {
            //
            // Perform a stop on the thread that the eval is running on.
            // This will cause a ThreadAbortException to be thrown on the thread.
            //
            EX_TRY
            {
                hr = pDE->m_thread->UserAbort(Thread::TAR_FuncEval, EEPolicy::TA_Rude, (DWORD)FUNC_EVAL_DEFAULT_TIMEOUT_VALUE, Thread::UAC_Normal);
                if (hr == HRESULT_FROM_WIN32(ERROR_TIMEOUT))
                {
                    hr = S_OK;
                }
            }
            EX_CATCH
            {
                    _ASSERTE(!"Unknown exception from UserAbort(), not expected");
                    EX_RETHROW;
            }
            EX_END_CATCH(RethrowTerminalExceptions);
        }

        LOG((LF_CORDB, LL_INFO1000, "D::FEA: RudeAbort complete.\n"));
    }

    return hr;
}

//
// FuncEvalCleanup cleans up after a function evaluation is released.
//
HRESULT Debugger::FuncEvalCleanup(DebuggerEval *debuggerEvalKey)
{
    LEAF_CONTRACT;

    DebuggerEval *pDE = debuggerEvalKey;

    _ASSERTE(pDE->m_completed);

    LOG((LF_CORDB, LL_INFO1000, "D::FEC: pDE:%08x 0x%08x, id=0x%x\n",
         pDE, pDE->m_thread, GetThreadIdHelper(pDE->m_thread)));

    DeleteInteropSafe(pDE);

    return S_OK;
}

#endif // ifndef DACCESS_COMPILE

//
// SetReference sets an object reference for the Right Side,
// respecting the write barrier for references that are in the heap.
//
HRESULT Debugger::SetReference(void *objectRefAddress,
                               LSPTR_OBJECTHANDLE objectHandle,
                               void *newReference)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HRESULT     hr = S_OK;

    hr = ValidateObject((Object *)newReference);
    if (FAILED(hr))
    {
        return hr;
    }


    // If the object ref isn't in a handle, then go ahead and use
    // SetObjectReference.
    if (objectHandle.IsNull())
    {
        OBJECTREF *dst = (OBJECTREF*)objectRefAddress;
        OBJECTREF  src = *((OBJECTREF*)&newReference);

        SetObjectReferenceUnchecked(dst, src);
    }
    else
    {

            // If the object reference to set is inside of a handle, then
            // fixup the handle.
            OBJECTHANDLE h = objectHandle.UnWrap();
            OBJECTREF  src = *((OBJECTREF*)&newReference);
            HndAssignHandle(h, src);
        }

    return S_OK;
}

//
// SetValueClass sets a value class for the Right Side, respecting the write barrier for references that are embedded
// within in the value class.
//
HRESULT Debugger::SetValueClass(void *oldData, void *newData, DebuggerIPCE_BasicTypeData * type)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    TypeHandle th;
    hr = BasicTypeInfoToTypeHandle(type, &th);

    if (FAILED(hr))
        return CORDBG_E_CLASS_NOT_LOADED;

    // Update the value class.
    CopyValueClassUnchecked(oldData, newData, th.GetMethodTable());

    // Free the buffer that is holding the new data. This is a buffer that was created in response to a GET_BUFFER
    // message, so we release it with ReleaseRemoteBuffer.
    ReleaseRemoteBuffer((BYTE*)newData, true);

    return hr;
}

/******************************************************************************
 *
 ******************************************************************************/
HRESULT Debugger::SetILInstrumentedCodeMap(MethodDesc *fd,
                                           BOOL fStartJit,
                                           ULONG32 cILMapEntries,
                                           COR_IL_MAP rgILMapEntries[])
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS_FROM_GETJITINFO;
    }
    CONTRACTL_END;

    if (!HasLazyData())
    {
        DebuggerLockHolder dbgLockHolder(this);
        // This is an entry path into the debugger, so make sure we're inited.
        LazyInit();
    }

    DebuggerMethodInfo * dmi = GetOrCreateMethodInfo(fd->GetModule(), fd->GetMemberDef());
    if (dmi == NULL)
    {
        return E_OUTOFMEMORY;
    }

    dmi->SetInstrumentedILMap(rgILMapEntries, cILMapEntries);

    return S_OK;
}

//
// EarlyHelperThreadDeath handles the case where the helper
// thread has been ripped out from underneath of us by
// ExitProcess or TerminateProcess. These calls are pure evil, wacking
// all threads except the caller in the process. This can happen, for
// instance, when an app calls ExitProcess. All threads are wacked,
// the main thread calls all DLL main's, and the EE starts shutting
// down in its DLL main with the helper thread nuked.
//
void Debugger::EarlyHelperThreadDeath(void)
{
    WRAPPER_CONTRACT;

    if (m_pRCThread)
        m_pRCThread->EarlyHelperThreadDeath();
}

//
// This tells the debugger that shutdown of the in-proc debugging services has begun. We need to know this during
// managed/unmanaged debugging so we can stop doing certian things to the process (like hijacking threads.)
//
void Debugger::ShutdownBegun(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_INTOLERANT;
    }
    CONTRACTL_END;


    // Shouldn't be Debugger-stopped if we're shutting down.
    // However, shutdown can occur in preemptive mode. Thus if the RS does an AsyncBreak late
    // enough, then the LS will appear to be stopped but may still shutdown.
    // Since the debuggee can exit asynchronously at any time (eg, suppose somebody forcefully
    // kills it with taskman), this doesn't introduce a new case.
    // That aside, it would be great to be able to assert this:
    //_ASSERTE(!IsStopped());

    if (m_pRCThread != NULL)
    {
        DebuggerIPCControlBlock *dcb = m_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC);

        if ((dcb != NULL) && (dcb->m_rightSideIsWin32Debugger))
            dcb->m_shutdownBegun = true;
    }
}

/*
 * LockDebuggerForShutdown
 *
 * This routine is used during shutdown to tell the in-process portion of the
 * debugger to synchronize with any threads that are currently using the
 * debugging facilities such that no more threads will run debugging services.
 *
 * This is accomplished by transitioning the debugger lock in to a state where
 * it will block all threads, except for the finalizer, shutdown, and helper thread.
 */
void Debugger::LockDebuggerForShutdown(void)
{
#ifndef DACCESS_COMPILE

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_INTOLERANT;
        MODE_ANY;
    }
    CONTRACTL_END;

    DebuggerLockHolder dbgLockHolder(this);

    // Shouldn't be Debugger-stopped if we're shutting down.
    // However, shutdown can occur in preemptive mode. Thus if the RS does an AsyncBreak late
    // enough, then the LS will appear to be stopped but may still shutdown.
    // Since the debuggee can exit asynchronously at any time (eg, suppose somebody forcefully
    // kills it with taskman), this doesn't introduce a new case.
    // That aside, it would be great to be able to assert this:
    //_ASSERTE(!IsStopped());

    // After setting this flag, nonspecial threads will not be able to
    // take the debugger lock.
    m_fShutdownMode = true;

    m_ignoreThreadDetach = TRUE;
#else
    DacNotImpl();
#endif
}


/*
 * DisableDebugger
 *
 * This routine is used by the EE to inform the debugger that it should block all
 * threads from executing as soon as it can.  Any thread entering the debugger can
 * block infinitely, as well.
 *
 * This is accomplished by transitioning the debugger lock into a mode where it will
 * block all threads infinitely rather than taking the lock.
 *
 */
void Debugger::DisableDebugger(void)
{
#ifndef DACCESS_COMPILE

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_INTOLERANT;
        PRECONDITION(ThisMaybeHelperThread());
    }
    CONTRACTL_END;

    m_fDisabled = true;

    CORDBDebuggerSetUnrecoverableError(this, CORDBG_E_DEBUGGING_DISABLED, false);

#else
    DacNotImpl();
#endif
}


/****************************************************************************
 * This will perform the duties of the helper thread if none already exists.
 * This is called in the case that the loader lock is held and so no new
 * threads can be spun up to be the helper thread, so the existing thread
 * must be the helper thread until a new one can spin up.
 * This is also called in the shutdown case (g_fProcessDetach==true) and our
 * helper may have already been blown away.
 ***************************************************************************/
void Debugger::DoHelperThreadDuty()
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        WRAPPER(GC_TRIGGERS);
    }
    CONTRACTL_END;

    // This should not be a real helper thread.
    _ASSERTE(!IsDbgHelperSpecialThread());
    _ASSERTE(ThreadHoldsLock());

    // We may be here in the shutdown case (only if the shutdown started after we got here).
    // We'll get killed randomly anyways, so not much we can do.

    // These assumptions are based off us being called from TART.
    _ASSERTE(ThreadStore::HoldingThreadStore() || g_fProcessDetach); // got this from TART
    _ASSERTE(m_trappingRuntimeThreads); // We're only called from TART.
    _ASSERTE(!m_stopped); // we haven't sent the sync-complete yet.

    // Can't have 2 threads doing helper duty.
    _ASSERTE(m_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC)->m_temporaryHelperThreadId == 0);

    LOG((LF_CORDB, LL_INFO1000,
         "D::SSCIPCE: helper thread is not ready, doing helper "
         "thread duty...\n"));

    // We're the temporary helper thread now.
    DWORD dwMyTID = GetCurrentThreadId();
    m_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC)->m_temporaryHelperThreadId = dwMyTID;

    // Make sure the helper thread has something to wait on while
    // we're trying to be the helper thread.
    VERIFY(ResetEvent(m_pRCThread->GetHelperThreadCanGoEvent()));

    // We have not sent the sync-complete flare yet.

    // Now that we've synchronized, we'll eventually send the sync-complete. But we're currently within the
    // scope of sombody already sending an event. So unlock from that event so that we can send the sync-complete.
    // Don't release the dubugger lock
    //
    UnlockFromEventSending(NULL);

    // We are the temporary helper thread. We will not deal with everything! But just pump for
    // continue.
    //
    m_pRCThread->TemporaryHelperThreadMainLoop();

    // We do not need to relock it since we never release it.
    LockForEventSending(NULL);
    _ASSERTE(ThreadHoldsLock());


    STRESS_LOG1(LF_CORDB, LL_INFO1000,
         "D::SSCIPCE: done doing helper thread duty. "
         "Current helper thread id=0x%x\n",
         m_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC)->m_helperThreadId);

    // We're not the temporary helper thread anymore.
    _ASSERTE(m_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC)->m_temporaryHelperThreadId == dwMyTID);
    m_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC)->m_temporaryHelperThreadId = 0;

    // Let the helper thread go if its waiting on us.
    VERIFY(SetEvent(m_pRCThread->GetHelperThreadCanGoEvent()));
}



// This function is called from the EE to notify the right side
// whenever the name of a thread or AppDomain changes
HRESULT Debugger::NameChangeEvent(AppDomain *pAppDomain, Thread *pThread)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    // Don't try to send one of these if the thread really isn't setup
    // yet. This can happen when initially setting up an app domain,
    // before the appdomain create event has been sent. Since the app
    // domain create event hasn't been sent yet in this case, its okay
    // to do this...
    if (g_pEEInterface->GetThread() == NULL)
        return S_OK;

    LOG((LF_CORDB, LL_INFO1000, "D::NCE: Sending NameChangeEvent 0x%x 0x%x\n",
        pAppDomain, pThread));

    bool disabled = g_pEEInterface->IsPreemptiveGCDisabled();

    if (disabled)
        g_pEEInterface->EnablePreemptiveGC();

    // Prevent other Runtime threads from handling events.
    DebuggerLockHolder dbgLockHolder(this, FALSE);

    LockForEventSending(&dbgLockHolder);

    if (CORDebuggerAttached())
    {

        DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce,
                     DB_IPCE_NAME_CHANGE,
                     g_pEEInterface->GetThread(),
                     g_pEEInterface->GetThread()->GetDomain());


        if (pAppDomain)
        {
            ipce->NameChange.eventType = APP_DOMAIN_NAME_CHANGE;
            ipce->NameChange.debuggerAppDomainToken.Set(pAppDomain);
        }
        else
        {
            ipce->NameChange.eventType = THREAD_NAME_CHANGE;
            _ASSERTE (pThread);
            ipce->NameChange.debuggerThreadIdToken = GetThreadIdHelper(pThread);
        }

        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

        // Stop all Runtime threads
        TrapAllRuntimeThreads(&dbgLockHolder, g_pEEInterface->GetThread()->GetDomain());
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::NCE: Skipping SendIPCEvent because RS detached."));
    }

    UnlockFromEventSending(&dbgLockHolder);

    g_pEEInterface->DisablePreemptiveGC();

    if (!disabled)
        g_pEEInterface->EnablePreemptiveGC();

    return S_OK;

}

/******************************************************************************
 *
 ******************************************************************************/
BOOL Debugger::SendCtrlCToDebugger(DWORD dwCtrlType)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000, "D::SCCTD: Sending CtrlC Event 0x%x\n",
        dwCtrlType));

    // Prevent other Runtime threads from handling events.
    DebuggerLockHolder dbgLockHolder(this, FALSE);

    LockForEventSending(&dbgLockHolder);

    if (CORDebuggerAttached())
    {
        DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce,
                     DB_IPCE_CONTROL_C_EVENT,
                     g_pEEInterface->GetThread(),
                     NULL);

        // The RS doesn't do anything with dwCtrlType
        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);

        // Stop all Runtime threads
        TrapAllRuntimeThreads(&dbgLockHolder, NULL);
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::SCCTD: Skipping SendIPCEvent because RS detached."));
    }

    UnlockFromEventSending(&dbgLockHolder);

    // now wait for notification from the right side about whether or not
    // the out-of-proc debugger is handling ControlC events.
    WaitForSingleObjectHelper(GetCtrlCMutex(), INFINITE);

    return GetDebuggerHandlingCtrlC();
}

/******************************************************************************
 *
 ******************************************************************************/
void Debugger::ClearAppDomainPatches(AppDomain *pAppDomain)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pAppDomain));
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "D::CADP\n"));

    DebuggerLockHolder lockHolder(this);

    DebuggerController::DeleteAllControllers(pAppDomain);
    // dbgLockHolder goes out of scope - implicit Release
}

// Allows the debugger to keep an up to date list of special threads
HRESULT Debugger::UpdateSpecialThreadList(DWORD cThreadArrayLength,
                                        DWORD *rgdwThreadIDArray)
{
    LEAF_CONTRACT;

    _ASSERTE(g_pRCThread != NULL);

    DebuggerIPCControlBlock *pIPC = g_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC);
    _ASSERTE(pIPC);

    if (!pIPC)
        return (E_FAIL);

    // Save the thread list information, and mark the dirty bit so
    // the right side knows.
    pIPC->m_specialThreadList = rgdwThreadIDArray;
    pIPC->m_specialThreadListLength = cThreadArrayLength;
    pIPC->m_specialThreadListDirty = true;

    return (S_OK);
}

// Updates the pointer for the debugger services
void Debugger::SetIDbgThreadControl(IDebuggerThreadControl *pIDbgThreadControl)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    if (m_pIDbgThreadControl)
        m_pIDbgThreadControl->Release();

    m_pIDbgThreadControl = pIDbgThreadControl;

    if (m_pIDbgThreadControl)
        m_pIDbgThreadControl->AddRef();
}

//
// If a thread is Win32 suspended right after hitting a breakpoint instruction, but before the OS has transitioned the
// thread over to the user-level exception dispatching logic, then we may see the IP pointing after the breakpoint
// instruction. There are times when the Runtime will use the IP to try to determine what code as run in the prolog or
// epilog, most notably when unwinding a frame. If the thread is suspended in such a case, then the unwind will believe
// that the instruction that the breakpoint replaced has really been executed, which is not true. This confuses the
// unwinding logic. This function is called from Thread::HandledJITCase() to help us recgonize when this may have
// happened and allow us to skip the unwind and abort the HandledJITCase.
//
// The criteria is this:
//
// 1) If a debugger is attached.
//
// 2) If the instruction before the IP is a breakpoint instruction.
//
// 3) If the IP is in the prolog or epilog of a managed function.
//
BOOL Debugger::IsThreadContextInvalid(Thread *pThread)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    BOOL invalid = FALSE;

    // Get the thread context.
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_CONTROL;
    BOOL success = pThread->GetThreadContext(&ctx);

    if (success)
    {
        // Check single-step flag
        if (IsSSFlagEnabled(&ctx))
        {
            // Can't hijack a thread whose SS-flag is set. This could lead to races
            // with the thread taking the SS-exception.
            // The debugger's controller filters will poll for GC to avoid starvation.
            STRESS_LOG0(LF_CORDB, LL_EVERYTHING, "HJC - Hardware trace flag applied\n");
            return TRUE;
        }
    }

    if (success)
    {
        LPVOID address = NULL;

#if defined(_X86_) || defined(_AMD64_)
        // Grab Eip - 1
        address = (((BYTE*)GetIP(&ctx)) - 1);
#elif defined(_PPC_)
        address = (((DWORD*)GetIP(&ctx)) - 1);
#else
        PORTABILITY_ASSERT("IsThreadContextInvalid (debugger.cpp) is not implemented on this platform.");
#endif

        EX_TRY
        {
            // Use AVInRuntimeImplOkHolder.
            AVInRuntimeImplOkayHolder AVOkay(TRUE);

            // Is it a breakpoint?
            if (AddressIsBreakpoint((CORDB_ADDRESS_TYPE*)address))
            {
                size_t prologSize; // Unused...
                if (g_pEEInterface->IsInPrologOrEpilog((BYTE*)GetIP(&ctx), &prologSize))
                {
                    LOG((LF_CORDB, LL_INFO1000, "D::ITCI: thread is after a BP and in prolog or epilog.\n"));
                    invalid = TRUE;
                }
            }
        }
        EX_CATCH
        {
            // If we fault trying to read the byte before EIP, then we know that its not a breakpoint.
            // Do nothing.  The default return value is FALSE.
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
    else
    {
        // If we can't get the context, then its definetly invalid... ;)
        LOG((LF_CORDB, LL_INFO1000, "D::ITCI: couldn't get thread's context!\n"));
        invalid = TRUE;
    }

    return invalid;
}


// notification when a SQL connection begins
void Debugger::CreateConnection(CONNID dwConnectionId, __in_z WCHAR *wzName, BOOL fAttaching)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    LOG((LF_CORDB,LL_INFO1000, "D::CreateConnection %d\n.", dwConnectionId));

    if (CORDBUnrecoverableError(this))
        return;

    bool disabled = false;
    DebuggerLockHolder dbgLockHolder(this, FALSE);

    if (!fAttaching)
    {
        disabled =  g_pEEInterface->IsPreemptiveGCDisabled();

        if (disabled)
            g_pEEInterface->EnablePreemptiveGC();

        // Prevent other Runtime threads from handling events.
        LockForEventSending(&dbgLockHolder);
    }

    if (CORDebuggerAttached() || fAttaching)
    {
        DebuggerIPCEvent* ipce;

        // Send a update module syns event to the Right Side.
        ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce, DB_IPCE_CREATE_CONNECTION,
                     g_pEEInterface->GetThread(),
                     NULL);
        ipce->CreateConnection.connectionId = dwConnectionId;
        _ASSERTE(wzName != NULL);
        ipce->CreateConnection.wzConnectionName.SetString(wzName);

        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::CreateConnection: Skipping SendIPCEvent because RS detached."));
    }

    if (!fAttaching)
    {
        // Stop all Runtime threads if we actually sent an event
        if (CORDebuggerAttached())
        {
            TrapAllRuntimeThreads(&dbgLockHolder, NULL);
        }

        // Let other Runtime threads handle their events.
        UnlockFromEventSending(&dbgLockHolder);

        g_pEEInterface->DisablePreemptiveGC();

        if (!disabled)
            g_pEEInterface->EnablePreemptiveGC();
    }
}

// notification when a SQL connection ends
void Debugger::DestroyConnection(CONNID dwConnectionId)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    LOG((LF_CORDB,LL_INFO1000, "D::DestroyConnection %d\n.", dwConnectionId));

    if (CORDBUnrecoverableError(this))
        return;

    // Note that the debugger lock is reentrant, so we may or may not hold it already.
    Debugger::DebuggerLockHolder dbgLockHolder(this, FALSE);
    SENDIPCEVENT_BEGIN(&dbgLockHolder);

    // Send a update module syns event to the Right Side.
    DebuggerIPCEvent* ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
    InitIPCEvent(ipce, DB_IPCE_DESTROY_CONNECTION,
                 g_pEEInterface->GetThread(),
                 NULL);
    ipce->ConnectionChange.connectionId = dwConnectionId;

    // IPC event is now initialized, so we can send it over.
    SendSimpleIPCEventAndBlock(&dbgLockHolder, IPC_TARGET_OUTOFPROC);

    // This will block on the continue
    SENDIPCEVENT_END;

}

// notification for SQL connection changes
void Debugger::ChangeConnection(CONNID dwConnectionId, BOOL fAttaching)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT;
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
    }
    CONTRACTL_END;

    LOG((LF_CORDB,LL_INFO1000, "D::ChangeConnection %d\n.", dwConnectionId));

    if (CORDBUnrecoverableError(this))
        return;

    bool disabled = false;
    DebuggerLockHolder dbgLockHolder(this, FALSE);

    if (!fAttaching)
    {
        disabled =  g_pEEInterface->IsPreemptiveGCDisabled();

        if (disabled)
            g_pEEInterface->EnablePreemptiveGC();

        // Prevent other Runtime threads from handling events.
        LockForEventSending(&dbgLockHolder);
    }

    if (CORDebuggerAttached() || fAttaching)
    {
        DebuggerIPCEvent* ipce;

        // Send a update module syns event to the Right Side.
        ipce = m_pRCThread->GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);
        InitIPCEvent(ipce, DB_IPCE_CHANGE_CONNECTION,
                     g_pEEInterface->GetThread(),
                     NULL);
        ipce->ConnectionChange.connectionId = dwConnectionId;
        m_pRCThread->SendIPCEvent(IPC_TARGET_OUTOFPROC);
    }
    else
    {
        LOG((LF_CORDB,LL_INFO1000, "D::ChangeConnection: Skipping SendIPCEvent because RS detached."));
    }
    if (!fAttaching)
    {
        // Stop all Runtime threads if we actually sent an event
        if (CORDebuggerAttached())
        {
            TrapAllRuntimeThreads(&dbgLockHolder, NULL);
        }

        // Let other Runtime threads handle their events.
        UnlockFromEventSending(&dbgLockHolder);

        g_pEEInterface->DisablePreemptiveGC();

        if (!disabled)
            g_pEEInterface->EnablePreemptiveGC();
    }
}


//
// Are we the helper thread?
// Some important things about running on the helper thread:
// - there's only 1, so guaranteed to be thread-safe.
// - we'll never run managed code.
// - therefore, Never GC.
// - It listens for events from the RS.
// - It's the only thread to send a sync complete.
//
bool ThisIsHelperThreadWorker(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // This can
    Thread * pThread;
    pThread = GetThread();

    // First check for a real helper thread. This will do a FLS access.
    bool fIsHelperThread = !!IsDbgHelperSpecialThread();
    if (fIsHelperThread)
    {
        // If we're on the real helper thread, we never run managed code
        // and so we'd better not have an EE thread object.
        _ASSERTE((pThread == NULL) || !"The helper thread should not being running managed code.\n"
            "Are you running managed code inside the dllmain? If so, your scenario is invalid and this"
            "assert is only the tip of the iceberg of evil things to come.\n");
        return true;
    }

    // Even if we're not on the real helper thread, we may still be on a thread
    // pretending to be the helper. (Helper Duty, etc).
    DWORD id = GetCurrentThreadId();

    // Check for temporary helper thread.
    if (ThisIsTempHelperThread(id))
    {
        return true;
    }

    return false;
}

//
// Make call to the static method.
// This is exposed to the contracts susbsystem so that the helper thread can call
// things on MODE_COOPERATIVE.
//
bool Debugger::ThisIsHelperThread(void)
{
    WRAPPER_CONTRACT;

    return ThisIsHelperThreadWorker();
}

// Check if we're the temporary helper thread. Have 2 forms of this, 1 that assumes the current
// thread (but has the overhead of an extra call to GetCurrentThreadId() if we laready know the tid.
bool ThisIsTempHelperThread()
{
    WRAPPER_CONTRACT;

    DWORD id = GetCurrentThreadId();
    return ThisIsTempHelperThread(id);
}

bool ThisIsTempHelperThread(DWORD tid)
{
    WRAPPER_CONTRACT;

    // If helper thread class isn't created, then there's no helper thread.
    // No one is doing helper thread duty either.
    // It's also possible we're in a shutdown case and have already deleted the
    // data for the helper thread.
    if (g_pRCThread != NULL)
    {
        // May be the temporary helper thread...
        DebuggerIPCControlBlock * pBlock = g_pRCThread->GetDCB(IPC_TARGET_OUTOFPROC);
        if (pBlock != NULL)
        {
            DWORD idTemp = pBlock->m_temporaryHelperThreadId;

            if (tid == idTemp)
            {
                return true;
            }
        }
    }
    return false;

}


// This function is called when host call ICLRSecurityAttributeManager::setDacl.
// It will redacl our SSE, RSEA, RSER events.
HRESULT Debugger::ReDaclEvents(PSECURITY_DESCRIPTOR securityDescriptor)
{
    WRAPPER_CONTRACT;

    return m_pRCThread->ReDaclEvents(securityDescriptor);
}

#endif // #ifndef DACCESS_COMPILE

/* ------------------------------------------------------------------------ *
 * DebuggerHeap impl
 * ------------------------------------------------------------------------ */

DebuggerHeap::DebuggerHeap()
{
}


DebuggerHeap::~DebuggerHeap()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    Destroy();
}

void DebuggerHeap::Destroy()
{
}

bool DebuggerHeap::IsInit()
{
    LEAF_CONTRACT;
    return true;
}

HRESULT DebuggerHeap::Init()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Have knob catch if we don't want to lazy init the debugger.
    _ASSERTE(!g_DbgShouldntUseDebugger);

    return S_OK;
}

// Only use canaries on x86 b/c they throw of alignment on Ia64.
#if defined(_DEBUG) && defined(_X86_)
#define USE_INTEROPSAFE_CANARY
#endif

#ifdef USE_INTEROPSAFE_CANARY
// Small header to to prefix interop-heap blocks.
// This lets us enforce that we don't delete interopheap data from a non-interop heap.
struct InteropHeapCanary
{
    ULONGLONG m_canary;

    // Raw address - this is what the heap alloc + free routines use.
    // User address - this is what the user sees after we adjust the raw address for the canary

    // Given a raw address to an allocated block, get the canary + mark it.
    static InteropHeapCanary * GetFromRawAddr(void * pStart)
    {
        _ASSERTE(pStart != NULL);
        InteropHeapCanary * p = (InteropHeapCanary*) pStart;
        p->Mark();
        return p;
    }

    // Get the raw address from this canary.
    void * GetRawAddr()
    {
        return (void*) this;
    }

    // Get a canary from a start address.
    static InteropHeapCanary * GetFromUserAddr(void * pStart)
    {
        _ASSERTE(pStart != NULL);
        InteropHeapCanary * p = ((InteropHeapCanary*) pStart)-1;
        p->Check();
        return p;
    }
    void * GetUserAddr()
    {
        this->Check();
        return (void*) (this + 1);
    }

protected:
    void Check()
    {
        CONSISTENCY_CHECK_MSGF((m_canary == kInteropHeapCookie),
            ("Using InteropSafe delete on non-interopsafe allocated memory.\n"));
    }
    void Mark()
    {
        m_canary = kInteropHeapCookie;
    }
    static const ULONGLONG kInteropHeapCookie = 0x12345678;
};
#endif // USE_INTEROPSAFE_CANARY

void *DebuggerHeap::Alloc(DWORD size)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("Debugger");

#ifdef USE_INTEROPSAFE_CANARY
    // Make sure we allocate enough space for the canary at the start.
    size += sizeof(InteropHeapCanary);
#endif

    void *ret;
    ret = ClrHeapAlloc(ClrGetProcessExecutableHeap(), NULL, size);

#ifdef USE_INTEROPSAFE_CANARY
    if (ret == NULL)
    {
        return NULL;
    }
    InteropHeapCanary * pCanary = InteropHeapCanary::GetFromRawAddr(ret);
    ret = pCanary->GetUserAddr();
#endif


    return ret;
}


// Realloc memory.
// If this fails, the original memory is still valid.
void *DebuggerHeap::Realloc(void *pMem, DWORD newSize, DWORD oldSize)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(pMem != NULL);
    _ASSERTE(newSize != 0);
    _ASSERTE(oldSize != 0);

    // impl Realloc on top of alloc & free.
    void *ret;

    ret = this->Alloc(newSize);
    if (ret == NULL)
    {
        // Not supposed to free original memory in failure condition.
        return NULL;
    }

    memcpy(ret, pMem, oldSize);
    this->Free(pMem);


    return ret;
}

void DebuggerHeap::Free(void *pMem)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifdef USE_INTEROPSAFE_CANARY
    // Check for canary

    if (pMem != NULL)
    {
        InteropHeapCanary * pCanary = InteropHeapCanary::GetFromUserAddr(pMem);
        pMem = pCanary->GetRawAddr();
    }
#endif

   ClrHeapFree(ClrGetProcessExecutableHeap(), NULL, pMem);
}

#ifndef DACCESS_COMPILE


// Undef this so we can call them from the EE versions.
#undef UtilMessageBoxVA

// Message box API for the left side of the debugger. This API handles calls from the
// debugger helper thread as well as from normal EE threads. It is the only one that
// should be used from inside the debugger left side.
int Debugger::MessageBox(
                  UINT uText,       // Resource Identifier for Text message
                  UINT uCaption,    // Resource Identifier for Caption
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  ...)              // Additional Arguments
{
    CONTRACTL
    {
        MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT;
        MODE_PREEMPTIVE;
        NOTHROW;

        PRECONDITION(ThisMaybeHelperThread());
    }
    CONTRACTL_END;

    va_list marker;
    va_start(marker, ShowFileNameInTitle);

    int result = UtilMessageBoxVA(NULL, uText, uCaption, uType, ShowFileNameInTitle, marker);
    va_end( marker );

    return result;
}

// Redefine this to an error just in case code is added after this point in the file.
#define UtilMessageBoxVA __error("Define and use EEMessageBoxVA from inside the EE")

#else // DACCESS_COMPILE
void
Debugger::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_VTHIS();

    if (m_pMethodInfos.IsValid())
    {
        m_pMethodInfos->EnumMemoryRegions(flags);
    }
}

#endif // #ifdef DACCESS_COMPILE

#endif //DEBUGGING_SUPPORTED

