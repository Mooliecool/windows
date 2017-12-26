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
/*============================================================
**
** Header: COMSynchronizable.cpp
**
**                                             
**
** Purpose: Native methods on System.SynchronizableObject
**          and its subclasses.
**
** Date:  April 1, 1998
**
===========================================================*/

#include "common.h"

#include <object.h>
#include "threads.h"
#include "excep.h"
#include "vars.hpp"
#include "field.h"
#include "security.h"
#include "comsynchronizable.h"
#include "dbginterface.h"
#include "comdelegate.h"
#include "remoting.h"
#include "eeconfig.h"
#include "appdomainhelper.h"
#include "stackcompressor.h"
#include "objectclone.h"
#include "appdomain.hpp"
#include "appdomain.inl"
#include "crossdomaincalls.h"

MethodTable* ThreadNative::m_MT = NULL;

// The two threads need to communicate some information.  Any object references must
// be declared to GC.
struct SharedState
{
    OBJECTHANDLE    m_Threadable;
    OBJECTHANDLE    m_ThreadStartArg;
    Thread         *m_Internal;
    OBJECTHANDLE    m_Principal;

    SharedState(OBJECTREF threadable, OBJECTREF threadStartArg, Thread *internal, OBJECTREF principal)
    {
        CONTRACTL
        {
            GC_NOTRIGGER;
            THROWS;  // From CreateHandle()
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;
        AppDomainFromIDHolder ad(internal->GetKickOffDomainId(), TRUE);
        if (ad.IsUnloaded())
            COMPlusThrow(kAppDomainUnloadedException);
        m_Threadable = ad->CreateHandle(NULL);
        StoreObjectInHandle(m_Threadable, threadable);

        m_ThreadStartArg = ad->CreateHandle(NULL);
        StoreObjectInHandle(m_ThreadStartArg, threadStartArg);

        m_Internal = internal;

        m_Principal = ad->CreateHandle(NULL);
        StoreObjectInHandle(m_Principal, principal);
    }

    ~SharedState()
    {
        CONTRACTL
        {
            GC_NOTRIGGER;
            NOTHROW;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        // It's important to have no GC rendez-vous point between the checking and the clean-up below.
        // The three handles below could be in an appdomain which is just starting to be unloaded, or an appdomain
        // which has been unloaded already.  Thus, we need to check whether the appdomain is still valid before
        // we do the clean-up.  Since we suspend all runtime threads when we try to do the unload, there will be no
        // race condition between the checking and the clean-up as long as this thread cannot be suspended in between.
        AppDomainFromIDHolder ad(m_Internal->GetKickOffDomainId(), TRUE);
        if (!ad.IsUnloaded())
        {
            DestroyHandle(m_Threadable);
            DestroyHandle(m_ThreadStartArg);
            DestroyHandle(m_Principal);
        }
    }

    static SharedState  *MakeSharedState(OBJECTREF threadable, OBJECTREF threadStartArg, Thread *internal, OBJECTREF principal);
};


// For the following helpers, we make no attempt to synchronize.  The app developer
// is responsible for managing his own race conditions.
//
// Note: if the internal Thread is NULL, this implies that the exposed object has
//       finalized and then been resurrected.
static inline BOOL ThreadNotStarted(Thread *t)
{
    WRAPPER_CONTRACT;
    return (t && t->IsUnstarted() && !t->HasValidThreadHandle());
}

static inline BOOL ThreadIsRunning(Thread *t)
{
    WRAPPER_CONTRACT;
    return (t &&
            (t->m_State & (Thread::TS_ReportDead|Thread::TS_Dead)) == 0 &&
            (CLRTaskHosted()? t->GetHostTask()!=NULL:t->HasValidThreadHandle()));
}

static inline BOOL ThreadIsDead(Thread *t)
{
    WRAPPER_CONTRACT;
    return (t == 0 || t->IsDead());
}


// Map our exposed notion of thread priorities into the enumeration that NT uses.
static INT32 MapToNTPriority(INT32 ours)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

    INT32   NTPriority = 0;

    switch (ours)
    {
    case ThreadNative::PRIORITY_LOWEST:
        NTPriority = THREAD_PRIORITY_LOWEST;
        break;

    case ThreadNative::PRIORITY_BELOW_NORMAL:
        NTPriority = THREAD_PRIORITY_BELOW_NORMAL;
        break;

    case ThreadNative::PRIORITY_NORMAL:
        NTPriority = THREAD_PRIORITY_NORMAL;
        break;

    case ThreadNative::PRIORITY_ABOVE_NORMAL:
        NTPriority = THREAD_PRIORITY_ABOVE_NORMAL;
        break;

    case ThreadNative::PRIORITY_HIGHEST:
        NTPriority = THREAD_PRIORITY_HIGHEST;
        break;

    default:
        COMPlusThrow(kArgumentOutOfRangeException, L"Argument_InvalidFlag");
    }
    return NTPriority;
}


// Map to our exposed notion of thread priorities from the enumeration that NT uses.
INT32 MapFromNTPriority(INT32 NTPriority)
{
    LEAF_CONTRACT;

    INT32   ours = 0;

    if (NTPriority <= THREAD_PRIORITY_LOWEST)
    {
        // managed code does not support IDLE.  Map it to PRIORITY_LOWEST.
        ours = ThreadNative::PRIORITY_LOWEST;
    }
    else if (NTPriority >= THREAD_PRIORITY_HIGHEST)
    {
        ours = ThreadNative::PRIORITY_HIGHEST;
    }
    else if (NTPriority == THREAD_PRIORITY_BELOW_NORMAL)
    {
        ours = ThreadNative::PRIORITY_BELOW_NORMAL;
    }
    else if (NTPriority == THREAD_PRIORITY_NORMAL)
    {
        ours = ThreadNative::PRIORITY_NORMAL;
    }
    else if (NTPriority == THREAD_PRIORITY_ABOVE_NORMAL)
    {
        ours = ThreadNative::PRIORITY_ABOVE_NORMAL;
    }
    else
    {
        _ASSERTE (!"not supported priority");
        ours = ThreadNative::PRIORITY_NORMAL;
    }
    return ours;
}


void ThreadNative::KickOffThread_Worker(LPVOID ptr)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    KickOffThread_Args *args = (KickOffThread_Args *) ptr;
    _ASSERTE(ObjectFromHandle(args->share->m_Threadable) != NULL);
    args->retVal = 0;

    // we are saving the delagate and result primarily for debugging
    struct _gc
    {
        OBJECTREF orPrincipal;
        OBJECTREF orThreadStartArg;
        OBJECTREF orDelegate;
        OBJECTREF orResult;
        OBJECTREF orThread;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    EX_TRY {
        Thread *pThread;
        pThread = GetThread();
        _ASSERTE(pThread);
        GCPROTECT_BEGIN(gc);
        BEGIN_SO_INTOLERANT_CODE(pThread);

        gc.orPrincipal = ObjectFromHandle(args->share->m_Principal);
        // Push the initial security principal object (if any) onto the
        // managed thread.
        if (gc.orPrincipal != NULL)
        {
            gc.orThread = args->pThread->GetExposedObject();
            MethodDescCallSite setPrincipalInternal(METHOD__THREAD__SET_PRINCIPAL_INTERNAL, &gc.orThread);
            ARG_SLOT argsToSetPrincipal[2];
            argsToSetPrincipal[0] = ObjToArgSlot(gc.orThread);
            argsToSetPrincipal[1] = ObjToArgSlot(gc.orPrincipal);
            setPrincipalInternal.Call(argsToSetPrincipal);
        }

        gc.orDelegate = ObjectFromHandle(args->share->m_Threadable);
        gc.orThreadStartArg = ObjectFromHandle(args->share->m_ThreadStartArg);

        // We cannot call the Delegate Invoke method directly from ECall.  The
        //  stub has not been created for non multicast delegates.  Instead, we
        //  will invoke the Method on the OR stored in the delegate directly.
        // If there are changes to the signature of the ThreadStart delegate
        //  this code will need to change.  I've noted this in the Thread start
        //  class.

        delete args->share;
        args->share = 0;

        MethodDesc *pMeth = ((DelegateEEClass*)( gc.orDelegate->GetMethodTable()->GetClass() ))->m_pInvokeMethod;
        _ASSERTE(pMeth);
        MethodDescCallSite invokeMethod(pMeth, &gc.orDelegate);

        if (gc.orDelegate->GetMethodTable() == g_Mscorlib.FetchClass(CLASS__PARAMETERIZEDTHREADSTART))
        {
            //Parameterized ThreadStart
            ARG_SLOT arg[2];

            arg[0] = ObjToArgSlot(gc.orDelegate);
            arg[1]=ObjToArgSlot(gc.orThreadStartArg);
            invokeMethod.Call(arg);
        }
        else
        {
            //Simple ThreadStart
            ARG_SLOT arg[1];

            arg[0] = ObjToArgSlot(gc.orDelegate);
            invokeMethod.Call(arg);
        }
	    STRESS_LOG2(LF_SYNC, LL_INFO10, "Managed thread exiting normally for delegate %p Type %pT\n", OBJECTREFToObject(gc.orDelegate), (size_t) gc.orDelegate->GetMethodTable());

        END_SO_INTOLERANT_CODE;
        GCPROTECT_END();
    } EX_CATCH { EX_RETHROW; } EX_END_CATCH_UNREACHABLE;
}

// Helper to avoid two EX_TRY/EX_CATCH blocks in one function
static void PulseAllHelper(Thread* pThread)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        DISABLED(NOTHROW);
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    EX_TRY
    {
        // GetExposedObject() will either throw, or we have a valid object.  Note
        // that we re-acquire it each time, since it may move during calls.
        pThread->GetExposedObject()->EnterObjMonitor();
        pThread->GetExposedObject()->PulseAll();
        pThread->GetExposedObject()->LeaveObjMonitor();
    }
    EX_CATCH
    {
        // just keep going...
    }
    EX_END_CATCH(SwallowAllExceptions)
}

// When an exposed thread is started by Win32, this is where it starts.
ULONG __stdcall ThreadNative::KickOffThread(void* pass)
{

    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    ULONG retVal = 0;
    // Before we do anything else, get Setup so that we have a real thread.

    // Our thread isn't setup yet, so we can't use the standard probe
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return E_FAIL);

    KickOffThread_Args args;
    // don't have a separate var becuase this can be updated in the worker
    args.share   = (SharedState *) pass;
    args.pThread = args.share->m_Internal;

    Thread* pThread = args.pThread;

    _ASSERTE(pThread != NULL);

    BOOL ok = TRUE;

    {
        EX_TRY
        {
            CExecutionEngine::CheckThreadState(0);
        }
        EX_CATCH
        {
            if (!SwallowUnhandledExceptions())
            {
                EX_RETHROW;
            }
        }
        EX_END_CATCH(SwallowAllExceptions);
        if (CExecutionEngine::CheckThreadStateNoCreate(0) == NULL)
        {
            // We can not 
            pThread->DetachThread(FALSE);
            pThread->SetThreadState(Thread::TS_FailStarted);
            ok = FALSE;
        }
    }

    if (ok)
    {
        ok = pThread->HasStarted();
    }

    if (ok)
    {
        // Do not swallow the unhandled exception here
        //

        // We have a sticky problem here.
        //
        // Under some circumstances, the context of 'this' doesn't match the context
        // of the thread.  Today this can only happen if the thread is marked for an
        // STA.  If so, the delegate that is stored in the object may not be directly
        // suitable for invocation.  Instead, we need to call through a proxy so that
        // the correct context transitions occur.
        //
        // All the changes occur inside HasStarted(), which will switch this thread
        // over to a brand new STA as necessary.  We have to notice this happening, so
        // we can adjust the delegate we are going to invoke on.

        _ASSERTE(GetThread() == pThread);        // Now that it's started
        ManagedThreadBase::KickOff(pThread->GetKickOffDomainId(), KickOffThread_Worker, &args);

        // If TS_FailStarted is set then the args are deleted in ThreadNative::StartInner
        if ((args.share) && !pThread->HasThreadState(Thread::TS_FailStarted))
        {
            delete args.share;
        }

        PulseAllHelper(pThread);

        pThread->EnablePreemptiveGC();
        DestroyThread(pThread);
    }

#ifndef TOTALLY_DISBLE_STACK_GUARDS 
    // We may have destroyed Thread object.  It is not safe to use cached Thread object in 
    // DebugSOIntolerantTransitionHandler or SOIntolerantTransitionHandler
    __soIntolerantTransitionHandler.SetThread(NULL);
#endif

    END_SO_INTOLERANT_CODE;

    return retVal;
}


FCIMPL3(void, ThreadNative::Start, ThreadBaseObject* pThisUNSAFE, Object* pPrincipalUNSAFE, StackCrawlMark* pStackMark)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();

    StartInner(pThisUNSAFE, pPrincipalUNSAFE, pStackMark);

    HELPER_METHOD_FRAME_END_POLL();
}
FCIMPLEND

// Start up a thread, which by now should be in the ThreadStore's Unstarted list.
void ThreadNative::StartInner(ThreadBaseObject* pThisUNSAFE, Object* pPrincipalUNSAFE, StackCrawlMark* pStackMark)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    struct _gc
    {
        OBJECTREF       pPrincipal;
        THREADBASEREF   pThis;
    } gc;

    gc.pPrincipal  = (OBJECTREF) pPrincipalUNSAFE;
    gc.pThis       = (THREADBASEREF) pThisUNSAFE;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(COMPlusThrowSO());

    GCPROTECT_BEGIN(gc);

    if (gc.pThis == NULL)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

    Thread        *pNewThread = gc.pThis->GetInternal();

    _ASSERTE(GetThread() != NULL);          // Current thread wandered in!

    gc.pThis->EnterObjMonitor();

    EX_TRY
    {
        // Is the thread already started?  You can't restart a thread.
        if (!ThreadNotStarted(pNewThread))
        {
            COMPlusThrow(kThreadStateException, IDS_EE_THREADSTART_STATE);
        }

        OBJECTREF   threadable = gc.pThis->GetDelegate();
        OBJECTREF   threadStartArg = gc.pThis->GetThreadStartArg();
        gc.pThis->SetDelegate(NULL);
        gc.pThis->SetThreadStartArg(NULL);

        // This can never happen, because we construct it with a valid one and then
        // we never let you change it (because SetStart is private).
        _ASSERTE(threadable != NULL);

        // Allocate this away from our stack, so we can unwind without affecting
        // KickOffThread.  It is inside a GCFrame, so we can enable GC now.
        NewHolder<SharedState> share(SharedState::MakeSharedState(threadable, threadStartArg,pNewThread, gc.pPrincipal));
        if (share == NULL)
            COMPlusThrowOM();

        pNewThread->IncExternalCount();

        // As soon as we create the new thread, it is eligible for suspension, etc.
        // So it gets transitioned to cooperative mode before this call returns to
        // us.  It is our duty to start it running immediately, so that GC isn't blocked.

        BOOL success = pNewThread->CreateNewThread(
                                        pNewThread->RequestedThreadStackSize() /* 0 stackSize override*/,
                                        KickOffThread, share);

        if (!success)
        {
            pNewThread->DecExternalCount(FALSE);
            COMPlusThrowOM();
        }

        // After we have established the thread handle, we can check m_Priority.
        // This ordering is required to eliminate the race condition on setting the
        // priority of a thread just as it starts up.
        pNewThread->SetThreadPriority(MapToNTPriority(gc.pThis->m_Priority));

        FastInterlockOr((ULONG *) &pNewThread->m_State, Thread::TS_LegalToJoin);

        DWORD   ret;
        ret = pNewThread->StartThread();
        _ASSERTE(ret == 1);

        {
            GCX_PREEMP();

            // Synchronize with HasStarted.
            while (!pNewThread->HasThreadState(Thread::TS_FailStarted) &&
                   pNewThread->HasThreadState(Thread::TS_Unstarted))
            {
                __SwitchToThread(0);
            }
        }

        if (!pNewThread->HasThreadState(Thread::TS_FailStarted))
        {
            share.SuppressRelease();       // we have handed off ownership of the shared struct
        }
        else
        {
            share.Release();
            PulseAllHelper(pNewThread);
            pNewThread->HandleThreadStartupFailure();
        }
    }
    EX_CATCH
    {
        gc.pThis->LeaveObjMonitor();
        EX_RETHROW;
    }
    EX_END_CATCH_UNREACHABLE;
    
    gc.pThis->LeaveObjMonitor();

    GCPROTECT_END();

    END_SO_INTOLERANT_CODE;
}

FCIMPL1(void, ThreadNative::Abort, ThreadBaseObject* pThis)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThis == NULL)
        FCThrowVoid(kNullReferenceException);

    THREADBASEREF thisRef(pThis);
    // We need to keep the managed Thread object alive so that we can call UserAbort on
    // unmanaged thread object.
    HELPER_METHOD_FRAME_BEGIN_1(thisRef);

    Thread *thread = thisRef->GetInternal();
    if (thread == NULL)
        COMPlusThrow(kThreadStateException, IDS_EE_THREAD_CANNOT_GET);
#ifdef _DEBUG
    DWORD testAbort = g_pConfig->GetHostTestThreadAbort();
    if (testAbort != 0) {
        thread->UserAbort(Thread::TAR_Thread, testAbort == 1 ? EEPolicy::TA_Safe : EEPolicy::TA_Rude, INFINITE, Thread::UAC_Normal);
    }
    else
#endif
    thread->UserAbort(Thread::TAR_Thread, EEPolicy::TA_V1Compatible, INFINITE, Thread::UAC_Normal);

    if (thread->CatchAtSafePoint())
        CommonTripThread();
    HELPER_METHOD_FRAME_END_POLL();
}
FCIMPLEND

FCIMPL1(void, ThreadNative::ResetAbort, ThreadBaseObject* pThis)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE(pThis);
    VALIDATEOBJECTREF(pThis);

    Thread *thread = pThis->GetInternal();
    // We do not allow user to reset rude thread abort in MustRun code.
    if (thread && thread->IsRudeAbort())
    {
        return;
    }

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();

    if (thread == NULL)
        COMPlusThrow(kThreadStateException, IDS_EE_THREAD_CANNOT_GET);
    thread->UserResetAbort(Thread::TAR_Thread);
    HELPER_METHOD_FRAME_END_POLL();
}
FCIMPLEND

FCIMPL1(INT32, ThreadNative::GetHashCode, ThreadBaseObject* pThis)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThis == NULL)
        FCThrow(kNullReferenceException);

    THREADBASEREF thisRef(pThis);
    Thread *thread = thisRef->GetInternal();
    INT32 result = thread->GetThreadId();
    FC_GC_POLL_RET();
    return result;
}
FCIMPLEND

FCIMPL1(INT32, ThreadNative::GetManagedThreadId, ThreadBaseObject* pThis)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // This method has the same body as GetHashCode (which we don't want to call
    // in CER contexts because it's a virtual dispatch). Use the FCUnique macro
    // to ensure the compiler doesn't fold the implementations (the fcall
    // infrastructure requires this).
    FCUnique(0x77);

    if (pThis == NULL)
        FCThrow(kNullReferenceException);

    THREADBASEREF thisRef(pThis);
    Thread *thread = thisRef->GetInternal();
    INT32 result = thread->GetThreadId();
    FC_GC_POLL_RET();
    return result;
}
FCIMPLEND

// You can only suspend a running thread.
FCIMPL1(void, ThreadNative::Suspend, ThreadBaseObject* pThisUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    Thread  *thread = pThisUNSAFE->GetInternal();

    HELPER_METHOD_FRAME_BEGIN_0();

    
    if (!ThreadIsRunning(thread))
        COMPlusThrow(kThreadStateException, IDS_EE_THREAD_SUSPEND_NON_RUNNING);

    thread->UserSuspendThread();

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

// You can only resume a thread that is in the user-suspended state.  (This puts a large
// burden on the app developer, but we want him to be thinking carefully about race
// conditions.  Precise errors give him a hope of sorting out his logic).
FCIMPL1(void, ThreadNative::Resume, ThreadBaseObject* pThisUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    Thread  *thread = pThisUNSAFE->GetInternal();

    HELPER_METHOD_FRAME_BEGIN_0();

    // UserResumeThread() will return 0 if there isn't a user suspension for us to
    // clear.
    if (!ThreadIsRunning(thread))
        COMPlusThrow(kThreadStateException, IDS_EE_THREAD_RESUME_NON_RUNNING);

    if (thread->UserResumeThread() == 0)
        COMPlusThrow(kThreadStateException, IDS_EE_THREAD_RESUME_NON_USER_SUSPEND);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


// Note that you can manipulate the priority of a thread that hasn't started yet,
// or one that is running.  But you get an exception if you manipulate the priority
// of a thread that has died.
FCIMPL1(INT32, ThreadNative::GetPriority, ThreadBaseObject* pThisUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThisUNSAFE==NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    // validate the handle
    if (ThreadIsDead(pThisUNSAFE->GetInternal()))
        FCThrowEx(kThreadStateException, IDS_EE_THREAD_DEAD_PRIORITY, NULL, NULL, NULL);

    return pThisUNSAFE->m_Priority;
}
FCIMPLEND

FCIMPL2(void, ThreadNative::SetPriority, ThreadBaseObject* pThisUNSAFE, INT32 iPriority)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    int     priority;
    Thread *thread;

    THREADBASEREF  pThis = (THREADBASEREF) pThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_1(pThis);

    if (pThis==NULL)
    {
        COMPlusThrow(kNullReferenceException, L"NullReference_This");
    }

    // translate the priority (validating as well)
    priority = MapToNTPriority(iPriority);  // can throw; needs a frame

    // validate the thread
    thread = pThis->GetInternal();

    if (ThreadIsDead(thread))
    {
        COMPlusThrow(kThreadStateException, IDS_EE_THREAD_DEAD_PRIORITY, NULL, NULL, NULL);
    }

    INT32 oldPriority = pThis->m_Priority;

    // Eliminate the race condition by establishing m_Priority before we check for if
    // the thread is running.  See ThreadNative::Start() for the other half.
    pThis->m_Priority = iPriority;

    if (!thread->SetThreadPriority(priority))
    {
        pThis->m_Priority = oldPriority;
        COMPlusThrow(kThreadStateException, IDS_EE_THREAD_PRIORITY_FAIL, NULL, NULL, NULL);
    }

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

// This service can be called on unstarted and dead threads.  For unstarted ones, the
// next wait will be interrupted.  For dead ones, this service quietly does nothing.
FCIMPL1(void, ThreadNative::Interrupt, ThreadBaseObject* pThisUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThisUNSAFE==NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    Thread  *thread = pThisUNSAFE->GetInternal();

    if (thread == 0)
        FCThrowExVoid(kThreadStateException, IDS_EE_THREAD_CANNOT_GET, NULL, NULL, NULL);

    HELPER_METHOD_FRAME_BEGIN_0();

    thread->UserInterrupt(Thread::TI_Interrupt);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, ThreadNative::IsAlive, ThreadBaseObject* pThisUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThisUNSAFE==NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    THREADBASEREF thisRef(pThisUNSAFE);
    BOOL ret = false;

    HELPER_METHOD_FRAME_BEGIN_RET_1(thisRef);

    Thread  *thread = thisRef->GetInternal();

    if (thread == 0)
        COMPlusThrow(kThreadStateException, IDS_EE_THREAD_CANNOT_GET);

    ret = ThreadIsRunning(thread);

    HELPER_METHOD_FRAME_END();

    FC_GC_POLL_RET();

    FC_RETURN_BOOL(ret);
}
FCIMPLEND

FCIMPL1(void, ThreadNative::Join, ThreadBaseObject* pThisUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    THREADBASEREF pThis = (THREADBASEREF) pThisUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_1(pThis);

    DoJoin(pThis, INFINITE_TIMEOUT);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, ThreadNative::JoinTimeout, ThreadBaseObject* pThisUNSAFE, INT32 Timeout)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThisUNSAFE==NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    // validate the timeout
    if ((Timeout < 0) && (Timeout != INFINITE_TIMEOUT))
        FCThrowArgumentOutOfRange(L"millisecondsTimeout", L"ArgumentOutOfRange_NeedNonNegOrNegative1");

    BOOL            retVal = FALSE;
    THREADBASEREF   pThis   = (THREADBASEREF) pThisUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(pThis);

    retVal = DoJoin(pThis, Timeout);

    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(retVal);
}
FCIMPLEND

#undef Sleep
FCIMPL1(void, ThreadNative::Sleep, INT32 iTime)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // validate the sleep time
    if ((iTime < 0) && (iTime != INFINITE_TIMEOUT))
        FCThrowArgumentOutOfRangeVoid(L"millisecondsTimeout", L"ArgumentOutOfRange_NeedNonNegOrNegative1");

    HELPER_METHOD_FRAME_BEGIN_0();

    GetThread()->UserSleep(iTime);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND
#define Sleep(dwMilliseconds) Dont_Use_Sleep(dwMilliseconds)

FCIMPL0(Object*, ThreadNative::GetCurrentThread)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    Thread      *pCurThread = GetThread();
    OBJECTREF   refRetVal  = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    _ASSERTE(pCurThread->PreemptiveGCDisabled());

    refRetVal = pCurThread->GetExposedObject();

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

LPVOID F_CALL_CONV ThreadNative::FastGetCurrentThread()
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    Thread *pThread;
    OBJECTHANDLE ExposedObject;

    pThread = GetThread();
    if (!pThread) {
    return NULL;
    }
    ExposedObject = pThread->m_ExposedObject;
    if (ExposedObject) {
        return *(LPVOID *)ExposedObject;
    }
    return NULL;
}


FCIMPL3(void, ThreadNative::SetStart, ThreadBaseObject* pThisUNSAFE, Object* pDelegateUNSAFE, INT32 iRequestedStackSize)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThisUNSAFE==NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    THREADBASEREF   pThis       = (THREADBASEREF) pThisUNSAFE;
    OBJECTREF       pDelegate   = (OBJECTREF    ) pDelegateUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_2(pThis, pDelegate);

    _ASSERTE(pThis != NULL);
    _ASSERTE(pDelegate != NULL); // Thread's constructor validates this

    if (pThis->m_InternalThread == NULL)
    {
        // if we don't have an internal Thread object associated with this exposed object,
        // now is our first opportunity to create one.
        Thread      *unstarted = SetupUnstartedThread();

        PREFIX_ASSUME(unstarted != NULL);

        pThis->SetInternal(unstarted);
        unstarted->SetExposedObject(pThis);
        unstarted->RequestedThreadStackSize(iRequestedStackSize);
    }

    // save off the delegate
    pThis->SetDelegate(pDelegate);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


// Set whether or not this is a background thread.
FCIMPL2(void, ThreadNative::SetBackground, ThreadBaseObject* pThisUNSAFE, CLR_BOOL isBackground)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThisUNSAFE==NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    // validate the thread
    Thread  *thread = pThisUNSAFE->GetInternal();

    if (ThreadIsDead(thread))
        FCThrowExVoid(kThreadStateException, IDS_EE_THREAD_DEAD_STATE, NULL, NULL, NULL);

    HELPER_METHOD_FRAME_BEGIN_0();

    thread->SetBackground(isBackground);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

// Return whether or not this is a background thread.
FCIMPL1(FC_BOOL_RET, ThreadNative::IsBackground, ThreadBaseObject* pThisUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThisUNSAFE==NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    // validate the thread
    Thread  *thread = pThisUNSAFE->GetInternal();

    if (ThreadIsDead(thread))
        FCThrowEx(kThreadStateException, IDS_EE_THREAD_DEAD_STATE, NULL, NULL, NULL);

    FC_RETURN_BOOL(thread->IsBackground());
}
FCIMPLEND


// Deliver the state of the thread as a consistent set of bits.
// This copied in VM\EEDbgInterfaceImpl.h's
//     CorDebugUserState GetUserState( Thread *pThread )
// , so propogate changes to both functions
FCIMPL1(INT32, ThreadNative::GetThreadState, ThreadBaseObject* pThisUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    INT32               res = 0;
    Thread::ThreadState state;

    if (pThisUNSAFE==NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    // validate the thread.  Failure here implies that the thread was finalized
    // and then resurrected.
    Thread  *thread = pThisUNSAFE->GetInternal();

    if (!thread)
        FCThrowEx(kThreadStateException, IDS_EE_THREAD_CANNOT_GET, NULL, NULL, NULL);

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    // grab a snapshot
    state = thread->GetSnapshotState();

    if (state & Thread::TS_Background)
        res |= ThreadBackground;

    if (state & Thread::TS_Unstarted)
        res |= ThreadUnstarted;

    // Don't report a StopRequested if the thread has actually stopped.
    if (state & Thread::TS_Dead)
    {
        if (state & Thread::TS_AbortRequested)
            res |= ThreadAborted;
        else
            res |= ThreadStopped;
    }
    else
    {
        if (state & Thread::TS_AbortRequested)
            res |= ThreadAbortRequested;
    }

    if (state & Thread::TS_Interruptible)
        res |= ThreadWaitSleepJoin;

    // Don't report a SuspendRequested if the thread has actually Suspended.
    if ((state & Thread::TS_UserSuspendPending) &&
        (state & Thread::TS_SyncSuspended)
       )
    {
        res |= ThreadSuspended;
    }
    else
    if (state & Thread::TS_UserSuspendPending)
    {
        res |= ThreadSuspendRequested;
    }

    HELPER_METHOD_FRAME_END();

    FC_GC_POLL_RET();

    return res;
}
FCIMPLEND


// Wait for the thread to die
BOOL ThreadNative::DoJoin(THREADBASEREF DyingThread, INT32 timeout)
{
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    _ASSERTE(DyingThread != NULL);

    DWORD    dwTimeOut32 = 0;
    DWORD    rv = 0;
    Thread  *DyingInternal = NULL;

    if (timeout < 0 && timeout != INFINITE_TIMEOUT)
        COMPlusThrow(kArgumentOutOfRangeException, L"ArgumentOutOfRange_NeedNonNegOrNegative1");

    DyingInternal = DyingThread->GetInternal();

    // Validate the handle.  It's valid to Join a thread that's not running -- so
    // long as it was once started.
    if (DyingInternal == 0 ||
        !(DyingInternal->m_State & Thread::TS_LegalToJoin))
    {
        COMPlusThrow(kThreadStateException, IDS_EE_THREAD_NOTSTARTED);
    }

    // Don't grab the handle until we know it has started, to eliminate the race
    // condition.
    if (ThreadIsDead(DyingInternal) || !DyingInternal->HasValidThreadHandle())
        return TRUE;

    dwTimeOut32 = (timeout == INFINITE_TIMEOUT
                   ? INFINITE
                   : (DWORD) timeout);

    // There is a race here.  DyingThread is going to close its thread handle.
    // If we grab the handle and then DyingThread closes it, we will wait forever
    // in DoAppropriateWait.
    int RefCount = DyingInternal->IncExternalCount();
    if (RefCount == 1)
    {
        // !!! We resurrect the Thread Object.
        // !!! We will keep the Thread ref count to be 1 so that we will not try
        // !!! to destroy the Thread Object again.
        // !!! Do not call DecExternalCount here!
        _ASSERTE (!DyingInternal->HasValidThreadHandle());
        return TRUE;
    }
    BOOL validThreadHandle = DyingInternal->HasValidThreadHandle();
    
    if (!validThreadHandle)
    {
        DyingInternal->DecExternalCount(FALSE);
        return TRUE;
    }
    EX_TRY_FOR_FINALLY
    {
        Thread *pCurThread = GetThread();
        pCurThread->EnablePreemptiveGC();
        rv = DyingInternal->JoinEx(dwTimeOut32, (WaitMode)(WaitMode_Alertable/*alertable*/|WaitMode_InDeadlock));
        validThreadHandle = DyingInternal->HasValidThreadHandle();
        pCurThread->DisablePreemptiveGC();
    }
    EX_FINALLY
    {
        DyingInternal->DecExternalCount(FALSE);
    }
    EX_END_FINALLY

    switch(rv)
    {
        case WAIT_OBJECT_0:
            return TRUE;
            break;
                        
        case WAIT_TIMEOUT:
            return FALSE;
            break;

        case WAIT_FAILED:
            if(!validThreadHandle)
                return TRUE;
            return FALSE;
            break;
            
        default:
            _ASSERTE(!"This return code is not understood \n");
            return FALSE;
    }    
}


// There are two reasons for its existence.
// First: SharedState cannot be stack allocated because it will be passed
// between two threads.  One is free to return, before the other consumes it.
//
// Second, it's not possible to do a C++ 'new' in the same method as a COM catch/try.
// That's because they each use different try/fail (C++ vs. SEH).  So move it down
// here where hopefully it will not be inlined.
SharedState *SharedState::MakeSharedState(OBJECTREF threadable, OBJECTREF threadStartArg, Thread *internal, OBJECTREF principal)
{
    WRAPPER_CONTRACT;
    return new SharedState(threadable, threadStartArg, internal, principal);
}


// We don't get a constructor for ThreadBaseObject, so we rely on the fact that this
// method is only called once, out of SetStart.  Since SetStart is private/native
// and only called from the constructor, we'll only get called here once to set it
// up and once (with NULL) to tear it down.  The 'null' can only come from Finalize
// because the constructor throws if it doesn't get a valid delegate.
void ThreadBaseObject::SetDelegate(OBJECTREF delegate)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

#ifdef APPDOMAIN_STATE
    if (delegate != NULL)
    {
        AppDomain *pDomain = delegate->GetAppDomain();
        Thread *pThread = GetInternal();
        AppDomain *kickoffDomain = pThread->GetKickOffDomain();
        _ASSERTE_ALL_BUILDS(! pDomain || pDomain == kickoffDomain);
        _ASSERTE_ALL_BUILDS(kickoffDomain == GetThread()->GetDomain());
    }
#endif

    SetObjectReferenceUnchecked( (OBJECTREF *)&m_Delegate, delegate );

    // If the delegate is being set then initialize the other data members.
    if (m_Delegate != NULL)
    {
        // Initialize the thread priority to normal.
        m_Priority = ThreadNative::PRIORITY_NORMAL;
    }
}


// If the exposed object is created after-the-fact, for an existing thread, we call
// InitExisting on it.  This is the other "construction", as opposed to SetDelegate.
void ThreadBaseObject::InitExisting()
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    Thread *pThread = GetInternal();
    _ASSERTE (pThread);
    switch (pThread->GetThreadPriority())
    {
    case THREAD_PRIORITY_LOWEST:
    case THREAD_PRIORITY_IDLE:
        m_Priority = ThreadNative::PRIORITY_LOWEST;
        break;

    case THREAD_PRIORITY_BELOW_NORMAL:
        m_Priority = ThreadNative::PRIORITY_BELOW_NORMAL;
        break;

    case THREAD_PRIORITY_NORMAL:
        m_Priority = ThreadNative::PRIORITY_NORMAL;
        break;

    case THREAD_PRIORITY_ABOVE_NORMAL:
        m_Priority = ThreadNative::PRIORITY_ABOVE_NORMAL;
        break;

    case THREAD_PRIORITY_HIGHEST:
    case THREAD_PRIORITY_TIME_CRITICAL:
        m_Priority = ThreadNative::PRIORITY_HIGHEST;
        break;

    case THREAD_PRIORITY_ERROR_RETURN:
    default:
        _ASSERTE(FALSE);
        m_Priority = ThreadNative::PRIORITY_NORMAL;
        break;
    }

}


FCIMPL1(void, ThreadNative::Finalize, ThreadBaseObject* pThisUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // This function is intentionally blank.
    // See comment in MethodTable::CallFinalizer.

    _ASSERTE (!"Should not be called");

    FCUnique(0x21);
}
FCIMPLEND


FCIMPL0(Object*, ThreadNative::GetDomainLocalStore)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    Thread* thread = GetThread();

    if (thread && thread->m_pDLSHash && thread->GetDomain())
    {
        HashDatum Data;

        Thread *pCurThread = GetThread();
        BOOL toggleGC = pCurThread->PreemptiveGCDisabled();

        if (toggleGC)
            pCurThread->EnablePreemptiveGC();
        DLSLockHolder dlsHolder;
        if (toggleGC)
            pCurThread->DisablePreemptiveGC();

        if (thread->m_pDLSHash->GetValue(thread->GetDomain()->GetId().m_dwId, &Data))
        {
            LocalDataStore *pLDS = (LocalDataStore *) Data;
            refRetVal = (OBJECTREF) pLDS->GetRawExposedObject();
            _ASSERTE(refRetVal != NULL);
        }
    }

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL1(void, ThreadNative::SetDomainLocalStore, Object* pLocalDataStoreUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    LOCALDATASTOREREF refLocalDataStore = (LOCALDATASTOREREF)ObjectToOBJECTREF(pLocalDataStoreUNSAFE);
    HELPER_METHOD_FRAME_BEGIN_1(refLocalDataStore);

    Thread* thread = GetThread();

    if (thread && thread->GetDomain())
    {
        Thread *pCurThread = GetThread();
        BOOL toggleGC = pCurThread->PreemptiveGCDisabled();

        if (toggleGC)
            pCurThread->EnablePreemptiveGC();
        DLSLockHolder dlsHolder;
        if (toggleGC)
            pCurThread->DisablePreemptiveGC();

        if (!thread->m_pDLSHash)
        {
            thread->m_pDLSHash = new (nothrow) EEIntHashTable();
            if (!thread->m_pDLSHash)
            {
                COMPlusThrowOM();
            }
            LockOwner lock = {ThreadStore::s_pThreadStore->GetDLSHashCrst(),
                              IsOwnerOfCrst};

            BOOL fSuccess = thread->m_pDLSHash->Init(3,&lock);

            if (!fSuccess)
            {
                delete thread->m_pDLSHash;
                thread->m_pDLSHash = NULL;

                COMPlusThrowOM();
            }
        }

        LocalDataStore* pLDS = refLocalDataStore->GetLocalDataStore();
        if (!pLDS)
        {
            pLDS = new (nothrow) LocalDataStore();
            if (!pLDS)
            {
                COMPlusThrowOM();
            }

            ObjectInHandleHolder hndholder(pLDS->m_ExposedTypeObject);
            StoreFirstObjectInHandle(pLDS->m_ExposedTypeObject, refLocalDataStore);
            refLocalDataStore->SetLocalDataStore(pLDS);
            thread->m_pDLSHash->InsertValue(thread->GetDomain()->GetId().m_dwId, (HashDatum) pLDS);

            hndholder.SuppressRelease();
        }

    }

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, ThreadNative::SetThreadUILocale, INT32 lcid)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BOOL result = TRUE;
    IHostTaskManager *manager = CorHost2::GetHostTaskManager();
    if (manager) {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        result = (manager->SetUILocale(lcid) == S_OK);
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }
    FC_RETURN_BOOL(result);
}
FCIMPLEND

FCIMPL0(Object*, ThreadNative::GetDomain)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    APPDOMAINREF refRetVal = NULL;

    Thread* thread = GetThread();

    if ((thread) && (thread->GetDomain()))
    {
        HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);
        refRetVal = (APPDOMAINREF) thread->GetDomain()->GetExposedObject();
        HELPER_METHOD_FRAME_END();
    }

    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

LPVOID F_CALL_CONV ThreadNative::FastGetDomain()
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    Thread *pThread;
    AppDomain *pDomain;
    OBJECTHANDLE ExposedObject;

    pThread = GetThread();
    if (!pThread) {
    return NULL;
    }
    pDomain = pThread->GetDomain();
    if (!pDomain) {
    return NULL;
    }
    ExposedObject = pDomain->m_ExposedObject;
    if (ExposedObject) {
        return *(LPVOID *)ExposedObject;
    }
    return NULL;
}


// This is just a helper method that lets BCL get to the managed context
// from the contextID.
FCIMPL1(Object*, ThreadNative::GetContextFromContextID, LPVOID ContextID)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF   rv   = NULL;
    Context*    pCtx = (Context *) ContextID;
    // Get the managed context backing this unmanaged context
    rv = pCtx->GetExposedObjectRaw();

    // This assert maintains the following invariant:
    // Only default unmanaged contexts can have a null managed context
    // (All non-deafult contexts are created as managed contexts first, and then
    // hooked to the unmanaged context)
    _ASSERTE((rv != NULL) || (pCtx->GetDomain()->GetDefaultContext() == pCtx));

    return OBJECTREFToObject(rv);
}
FCIMPLEND


FCIMPL6(Object*, ThreadNative::InternalCrossContextCallback, ThreadBaseObject* refThis, ContextBaseObject* refContext, LPVOID contextID, INT32 appDomainId, Object* oDelegateUNSAFE, PtrArray* oArgsUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;  // We have a frame in place before we do any work
    }
    CONTRACTL_END;

    _ASSERTE(refThis != NULL);
    VALIDATEOBJECTREF(refThis);
    Thread *pThread = refThis->GetInternal();
    Context *pCtx = (Context *)contextID;


    _ASSERTE(pCtx && (refContext == NULL || pCtx->GetExposedObjectRaw() == NULL ||
             ObjectToOBJECTREF(refContext) == pCtx->GetExposedObjectRaw()));
    LOG((LF_APPDOMAIN, LL_INFO1000, "ThreadNative::InternalCrossContextCallback: %p, %p\n", refContext, pCtx));
    // install our frame. We have to put it here before we put the helper frame on

    // Set the VM conext

    struct _gc {
        OBJECTREF oRetVal;
        OBJECTREF oDelegate;
        OBJECTREF oArgs;
        // We need to report the managed context object because it may become unreachable in the caller, 
        // however we have to keep it alive, otherwise its finalizer could free the unmanaged internal context
        OBJECTREF oContext;
    } gc;

    gc.oRetVal   = NULL;
    gc.oDelegate = ObjectToOBJECTREF(oDelegateUNSAFE);
    gc.oArgs     = ObjectToOBJECTREF(oArgsUNSAFE);
    gc.oContext  = ObjectToOBJECTREF(refContext);

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    // If we have a non-zero appDomain index, this is a x-domain call
    // We must verify that the AppDomain is not unloaded
    PREPARE_NONVIRTUAL_CALLSITE(METHOD__THREAD__COMPLETE_CROSSCONTEXTCALLBACK);

    AppDomainFromIDHolder ad;
    if (appDomainId != 0)
    {
        //
        // NOTE: there is a potential race between the time we retrieve the app domain pointer,
        // and the time which this thread enters the domain.
        //
        // To solve the race, we rely on the fact that there is a thread sync
        // between releasing an app domain's handle, and destroying the app domain.  Thus
        // it is important that we not go into preemptive gc mode in that window.
        //
        {
            ad.Assign(ADID(appDomainId), TRUE); 
            
            if (ad.IsUnloaded() || !ad->CanThreadEnter(pThread))
                COMPlusThrow(kAppDomainUnloadedException, L"Remoting_AppDomainUnloaded");
        }
    }

    // Verify that the Context is valid.
    if ( !Context::ValidateContext(pCtx) )
        COMPlusThrow(kRemotingException, L"Remoting_InvalidContext");

    DEBUG_ASSURE_NO_RETURN_BEGIN

#ifdef _DEBUG
    MethodDesc* pTargetMD = COMDelegate::GetMethodDesc(gc.oDelegate);
    _ASSERTE(pTargetMD->IsStatic());
#endif

    FrameWithCookie<ContextTransitionFrame>  frame;

    Context*    pCurrContext    = pThread->GetContext();
    bool        fTransition     = (pCurrContext != pCtx);
    BOOL        fSameDomain     = (appDomainId==0) || (pCurrContext->GetDomain()->GetId() == (ADID)appDomainId);
    _ASSERTE( fTransition || fSameDomain);
    if (fTransition) 
        if (appDomainId!=0)
            ad->EnterContext(pThread,pCtx, &frame);
        else
            pThread->EnterContextRestricted(pCtx,&frame);
    ad.Release();


    LOG((LF_EH, LL_INFO100, "MSCORLIB_ENTER_CONTEXT( %s::%s ): %s\n",
        pTargetMD->m_pszDebugClassName,
        pTargetMD->m_pszDebugMethodName,
        fTransition ? "ENTERED" : "NOP"));

    Exception* pOriginalException=NULL;
        
    EX_TRY
    {
        DECLARE_ARGHOLDER_ARRAY(callArgs, 2);

#if CHECK_APP_DOMAIN_LEAKS
        // We're passing the delegate object to another appdomain
        // without marshaling, that is OK - it's a static function delegate
        // but we should mark it as agile then.
        gc.oDelegate->SetSyncBlockAppDomainAgile();
#endif
        callArgs[ARGNUM_0] = OBJECTREF_TO_ARGHOLDER(gc.oDelegate);
        callArgs[ARGNUM_1] = OBJECTREF_TO_ARGHOLDER(gc.oArgs);

        CALL_MANAGED_METHOD_RETREF(gc.oRetVal, OBJECTREF, Object*, callArgs);
    }
    EX_CATCH
    {
        LOG((LF_EH, LL_INFO100, "MSCORLIB_CONTEXT_TRANSITION(     %s::%s ): exception in flight\n", pTargetMD->m_pszDebugClassName, pTargetMD->m_pszDebugMethodName));

        if (!fTransition || fSameDomain)
        {
            if (fTransition)
            {
                GCX_FORBID();
                pThread->ReturnToContext(&frame);
            }
            EX_RETHROW;
        }
        pOriginalException=EXTRACT_EXCEPTION();
        goto lAfterCtxUnwind;
    }
    EX_END_CATCH_UNREACHABLE;
    if (0)
    {
lAfterCtxUnwind:
        LOG((LF_EH, LL_INFO100, "MSCORLIB_RaiseCrossContextException( %s::%s )\n", pTargetMD->m_pszDebugClassName, pTargetMD->m_pszDebugMethodName));
        pThread->RaiseCrossContextException(pOriginalException,&frame);
    }

    LOG((LF_EH, LL_INFO100, "MSCORLIB_LEAVE_CONTEXT_TRANSITION( %s::%s )\n", pTargetMD->m_pszDebugClassName, pTargetMD->m_pszDebugMethodName));

    if (fTransition)
    {
        GCX_FORBID();
        pThread->ReturnToContext(&frame);
    }

    DEBUG_ASSURE_NO_RETURN_END

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(gc.oRetVal);
}
FCIMPLEND


//
// nativeGetSafeCulture is used when the culture get requested from the thread object. 
// we have to check the culture in the FCALL because in FCALL the thread cannot be 
// interrupted and unload other app domian.
// the concern here is if the thread hold a subclassed culture object and somebody 
// requested it from other app domain then we shouldn't hold any reference to that 
// culture object any time because the app domain created this culture may get 
// unloaded and this culture will survive although the type metadata will be unloaded 
// and GC will crash first time accessing this object after the app domain unload.
//
FCIMPL4(FC_BOOL_RET, ThreadNative::nativeGetSafeCulture, 
                    ThreadBaseObject*   threadUNSAFE, 
                    int                 appDomainId, 
                    CLR_BOOL            isUI, 
                    OBJECTREF*          safeCulture)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    THREADBASEREF thread(threadUNSAFE);

    CULTUREINFOBASEREF pCulture = isUI ? thread->GetCurrentUICulture() : thread->GetCurrentUserCulture();
    if (pCulture != NULL) {
        if (pCulture->IsSafeCrossDomain() || pCulture->GetCreatedDomainID() == ADID(appDomainId)) {
            SetObjectReference(safeCulture, pCulture, pCulture->GetAppDomain());
        } else {
            FC_RETURN_BOOL(FALSE);
        }
    }
    FC_RETURN_BOOL(TRUE);
}
FCIMPLEND


FCIMPL2(void,
        ThreadNative::InformThreadNameChangeEx,
        ThreadBaseObject* threadUNSAFE,
        StringObject* nameUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    THREADBASEREF thread(threadUNSAFE);
    STRINGREF name = (STRINGREF) nameUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_2(name, thread);

    VALIDATEOBJECTREF(thread);
    VALIDATEOBJECTREF(name);

    Thread *pThread = thread->GetInternal();

#ifdef PROFILING_SUPPORTED
    if (CORProfilerTrackThreads())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->ThreadNameChanged((ThreadID)pThread, name->GetStringLength(), name->GetBuffer());
    }
#endif // PROFILING_SUPPORTED


#ifdef DEBUGGING_SUPPORTED
    if (CORDebuggerAttached())
    {
        _ASSERTE(NULL != g_pDebugInterface);
        g_pDebugInterface->NameChangeEvent(NULL, pThread);
    }
#endif // DEBUGGING_SUPPORTED

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL0(void, ThreadNative::BeginCriticalRegion)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    GetThread()->BeginCriticalRegion();
}
FCIMPLEND

FCIMPL0(void, ThreadNative::EndCriticalRegion)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    GetThread()->EndCriticalRegion();
}
FCIMPLEND

FCIMPL0(void, ThreadNative::BeginThreadAffinity)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    Thread::BeginThreadAffinity();
}
FCIMPLEND

FCIMPL0(void, ThreadNative::EndThreadAffinity)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    Thread::EndThreadAffinity();
}
FCIMPLEND


FCIMPL1(FC_BOOL_RET, ThreadNative::IsThreadpoolThread, ThreadBaseObject* thread)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (thread==NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Thread *pThread = thread->GetInternal();

    if (pThread == NULL)
        FCThrowEx(kThreadStateException, IDS_EE_THREAD_DEAD_STATE, NULL, NULL, NULL);

    BOOL ret = pThread->IsThreadPoolThread();

    FC_GC_POLL_RET();

    FC_RETURN_BOOL(ret);
}
FCIMPLEND


FCIMPL1(void, ThreadNative::SpinWait, int iterations)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    for(int i = 0; i < iterations; i++)
        YieldProcessor();
}
FCIMPLEND


FCIMPL2(void*, ThreadNative::SetAppDomainStack, ThreadBaseObject* pThis, SafeHandle* hcsUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    void* pRet = NULL;
    SAFEHANDLE hcsSAFE = (SAFEHANDLE) hcsUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(hcsSAFE);


    void* unmanagedCompressedStack = NULL;
    if (hcsSAFE != NULL)
    {
        unmanagedCompressedStack = (void *)hcsSAFE->GetHandle();
    }


    VALIDATEOBJECTREF(pThis);
    Thread *pThread = pThis->GetInternal();

    pRet = StackCompressor::SetAppDomainStack(pThread, unmanagedCompressedStack);
    HELPER_METHOD_FRAME_END_POLL();
    return pRet;
}
FCIMPLEND


FCIMPL2(void, ThreadNative::RestoreAppDomainStack, ThreadBaseObject* pThis, void* appDomainStack)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();

    VALIDATEOBJECTREF(pThis);
    Thread *pThread = pThis->GetInternal();

    StackCompressor::RestoreAppDomainStack(pThread, appDomainStack);
    HELPER_METHOD_FRAME_END_POLL();
}
FCIMPLEND


FCIMPL0(void, ThreadNative::FCMemoryBarrier)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    MemoryBarrier();
    FC_GC_POLL();
}
FCIMPLEND

FCIMPL1(void, ThreadNative::SetIsThreadStaticsArray, Object* pArrayUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        SO_TOLERANT;
    } CONTRACTL_END;

    // This code will not cause a GC, so don't protect pObject
#if CHECK_APP_DOMAIN_LEAKS
    OBJECTREF rArray = ObjectToOBJECTREF(pArrayUNSAFE);
    MethodTable * pMT = rArray->GetMethodTable ();
    _ASSERTE (pMT->IsArray () 
                && (pMT->GetArrayElementType() == ELEMENT_TYPE_CLASS || pMT->GetArrayElementType() == ELEMENT_TYPE_SZARRAY));
    HELPER_METHOD_FRAME_BEGIN_0();
    SyncBlock *psb = rArray->GetSyncBlock();
    
    if (!psb->IsThreadStaticsArray ())
        psb->SetIsThreadStaticsArray ();
    HELPER_METHOD_FRAME_END();
#endif
    //have to do something in non-debug build otherwise compiler won't generate the method
    //and we can't find the FCall
    FC_GC_POLL();
}
FCIMPLEND


FCIMPL2(void, ThreadNative::SetAbortReason, ThreadBaseObject* pThisUNSAFE, Object* pObject)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
    } CONTRACTL_END;

    if (pThisUNSAFE==NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    OBJECTREF refObject = static_cast<OBJECTREF>(pObject);

    Thread *pThread = pThisUNSAFE->GetInternal();

    // If the OBJECTHANDLE is not 0, already set so just return
    if (pThread->m_AbortReason != 0)
        return;

    // Set up a frame in case of GC or EH
    HELPER_METHOD_FRAME_BEGIN_1(refObject)

    // Get the AppDomain ID for the AppDomain on the currently running thread.
    //  NOTE: the currently running thread may be different from this thread object!
    AppDomain *pCurrentDomain = GetThread()->GetDomain();
    ADID adid = pCurrentDomain->GetId();

    // Create a OBJECTHANDLE for the object.
    OBJECTHANDLE oh = pCurrentDomain->CreateHandle(refObject);

    // Scope the lock to peeking at and updating the two fields on the Thread object.
    {   // Atomically check whether the OBJECTHANDLE has been set, and if not,
        //  store it and the ADID of the object.
        //  NOTE: get the lock on this thread object, not on the executing thread.
        Thread::AbortRequestLockHolder lock(pThread);
        if (pThread->m_AbortReason == 0)
        {
            pThread->m_AbortReason = oh;
            pThread->m_AbortReasonDomainID = adid;
            // Set the OBJECTHANDLE so we can know that we stored it on the Thread object.
            oh = 0;
        }
    }

    // If the OBJECTHANDLE created above was not stored onto the Thread object, then
    //  another thread beat this one to the update.  Destroy the OBJECTHANDLE that
    //  was not used, created above.
    if (oh != 0)
    {
        DestroyHandle(oh);
    }

    HELPER_METHOD_FRAME_END()

}
FCIMPLEND


FCIMPL1(Object*, ThreadNative::GetAbortReason, ThreadBaseObject *pThisUNSAFE)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThisUNSAFE==NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    OBJECTREF refRetVal = NULL;
    Thread   *pThread   = pThisUNSAFE->GetInternal();

    // Set up a frame in case of GC or EH
    HELPER_METHOD_FRAME_BEGIN_RET_1(refRetVal)

    // While the ExceptionInfo probably will be *set* from a different
    //  thread, it should only be *read* from the current thread.
    _ASSERTE(GetThread() == pThread);

    // Set cooperative mode, to avoid AD unload while we're working.
    GCX_COOP();

    OBJECTHANDLE oh=NULL;
    ADID adid;
    // Scope the lock to reading the two fields on the Thread object.
    {   // Atomically get the OBJECTHANDLE and ADID of the object
        //  NOTE: get the lock on this thread object, not on the executing thread.
        Thread::AbortRequestLockHolder lock(pThread);
        oh = pThread->m_AbortReason;
        adid = pThread->m_AbortReasonDomainID;
    }

    // If the OBJECTHANDLE is not 0...
    if (oh != 0)
    {

        AppDomain *pCurrentDomain = pThread->GetDomain();
        // See if the appdomain is equal to the appdomain of the currently running
        //  thread.  

        if (pCurrentDomain->GetId() == adid)
        {   // Same appdomain; just return object from the OBJECTHANDLE
            refRetVal = ObjectFromHandle(oh);
        }
        else
        {   // Otherwise, try to marshal the object from the other AppDomain
#if defined(_X86_) || defined(_WIN64)
            ENTER_DOMAIN_ID(adid);
            CrossAppDomainClonerCallback cadcc;
            ObjectClone Cloner(&cadcc, CrossAppDomain, FALSE);
            refRetVal = Cloner.Clone(ObjectFromHandle(oh), GetAppDomain(), pCurrentDomain, NULL);
            Cloner.RemoveGCFrames();
            END_DOMAIN_TRANSITION;
#else
            PORTABILITY_ASSERT("Cross Appdomain Marshaling needs to be implemented for GetExceptionStateInfo()");
#endif
        }
    }

    HELPER_METHOD_FRAME_END()

    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND


FCIMPL1(void, ThreadNative::ClearAbortReason, ThreadBaseObject* pThisUNSAFE)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (pThisUNSAFE==NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    Thread *pThread = pThisUNSAFE->GetInternal();

    // Clearing from managed code can only happen on the current thread.
    _ASSERTE(pThread == GetThread());

    HELPER_METHOD_FRAME_BEGIN_0();
    pThread->ClearAbortReason();
    HELPER_METHOD_FRAME_END();

}
FCIMPLEND


