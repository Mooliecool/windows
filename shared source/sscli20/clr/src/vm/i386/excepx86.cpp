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
/*  EXCEP.CPP:
 *
 */
#include "common.h"

#include "frames.h"
#include "excep.h"
#include "object.h"
#include "comstring.h"
#include "field.h"
#include "dbginterface.h"
#include "cgensys.h"
#include "gcscan.h"
#include "comutilnative.h"
#include "comsystem.h"
#include "sigformat.h"
#include "siginfo.hpp"
#include "gc.h"
#include "eedbginterfaceimpl.h" //so we can clearexception in COMPlusThrow
#include "perfcounters.h"
#include "eventtrace.h"
#include "eetoprofinterfacewrapper.inl"
#include "eedbginterfaceimpl.inl"
#include "dllimportcallback.h"
#include "threads.h"
#include "appdomainhelper.h"
#include "eeconfig.h"
#include "vars.hpp"
#include "generics.h"

#include "threads.inl"

#define FORMAT_MESSAGE_BUFFER_LENGTH 1024

BOOL ComPlusFrameSEH(EXCEPTION_REGISTRATION_RECORD*);
PEXCEPTION_REGISTRATION_RECORD GetPrevSEHRecord(EXCEPTION_REGISTRATION_RECORD*);

extern "C" {
// in asmhelpers.asm:
#if defined(USE_FEF) // external declaration -- remove from compile to catch uses of it.
VOID __stdcall NakedThrowHelper(VOID);
#endif // USE_FEF
VOID __stdcall ResumeAtJitEHHelper(EHContext *pContext);
int __stdcall CallJitEHFilterHelper(size_t *pShadowSP, EHContext *pContext);
VOID __stdcall CallJitEHFinallyHelper(size_t *pShadowSP, EHContext *pContext);
}

static inline BOOL
CPFH_ShouldUnwindStack(DWORD exceptionCode) {

    LEAF_CONTRACT;

    return TRUE;

}

static inline BOOL IsComPlusNestedExceptionRecord(EXCEPTION_REGISTRATION_RECORD* pEHR)
{
    LEAF_CONTRACT;
    if (pEHR->Handler == (PEXCEPTION_ROUTINE)COMPlusNestedExceptionHandler)
        return TRUE;
    return FALSE;
}

EXCEPTION_REGISTRATION_RECORD *TryFindNestedEstablisherFrame(EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame)
{
    LEAF_CONTRACT;
    while (pEstablisherFrame->Handler != (PEXCEPTION_ROUTINE)COMPlusNestedExceptionHandler) {
        pEstablisherFrame = pEstablisherFrame->Next;
        if (pEstablisherFrame == EXCEPTION_CHAIN_END) return 0;
    }
    return pEstablisherFrame;
}

#ifdef _DEBUG
// stores last handler we went to in case we didn't get an endcatch and stack is
// corrupted we can figure out who did it.
static MethodDesc *gLastResumedExceptionFunc = NULL;
static DWORD gLastResumedExceptionHandler = 0;
#endif


BOOL NExportSEH(EXCEPTION_REGISTRATION_RECORD* pEHR)
{
    LEAF_CONTRACT;

    if ((LPVOID)pEHR->Handler == (LPVOID)UMThunkPrestubHandler)
    {
        return TRUE;
    }
    return FALSE;
}

BOOL FastNExportSEH(EXCEPTION_REGISTRATION_RECORD* pEHR)
{
    LEAF_CONTRACT;

    if ((LPVOID)pEHR->Handler == (LPVOID)FastNExportExceptHandler)
        return TRUE;
    return FALSE;
}


UnmanagedToManagedCallFrame* GetCurrFrame(ComToManagedExRecord *);

//
// Returns true if the given SEH handler is one of our SEH handlers that is responsible for managing exceptions in
// regions of managed code.
//
inline BOOL IsUnmanagedToManagedSEHHandler(EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame)
{
    WRAPPER_CONTRACT;

    //
    // ComPlusFrameSEH() is for COMPlusFrameHandler & COMPlusNestedExceptionHandler.
    // FastNExportSEH() is for FastNExportExceptHandler.
    // NExportSEH() is for UMThunkPrestubHandler.
    //
    return (ComPlusFrameSEH(pEstablisherFrame) || FastNExportSEH(pEstablisherFrame) || NExportSEH(pEstablisherFrame));
}

Frame *GetCurrFrame(EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame)
{
    Frame *pFrame;
    WRAPPER_CONTRACT;
    _ASSERTE(IsUnmanagedToManagedSEHHandler(pEstablisherFrame));
    if (ComPlusFrameSEH(pEstablisherFrame) || FastNExportSEH(pEstablisherFrame))
        pFrame = ((FrameHandlerExRecord *)pEstablisherFrame)->GetCurrFrame();
    else
        pFrame = GetCurrFrame((ComToManagedExRecord*)pEstablisherFrame);
    _ASSERTE(GetThread() == NULL || GetThread()->GetFrame() <= pFrame);
    return pFrame;
}

EXCEPTION_REGISTRATION_RECORD* GetNextCOMPlusSEHRecord(EXCEPTION_REGISTRATION_RECORD* pRec) {
    WRAPPER_CONTRACT;
    if (pRec == EXCEPTION_CHAIN_END)
        return EXCEPTION_CHAIN_END;

    do {
        _ASSERTE(pRec != 0);
        pRec = pRec->Next;
    } while (pRec != EXCEPTION_CHAIN_END && !IsUnmanagedToManagedSEHHandler(pRec));

    _ASSERTE(pRec == EXCEPTION_CHAIN_END || IsUnmanagedToManagedSEHHandler(pRec));
    return pRec;
}


/*
 * GetClrSEHRecordServicingStackPointer
 *
 * This function searchs all the Frame SEH records, and finds the one that is
 * currently signed up to do all exception handling for the given stack pointer
 * on the given thread.
 *
 * Parameters:
 *   pThread - The thread to search on.
 *   pStackPointer - The stack location that we are finding the Frame SEH Record for.
 *
 * Returns
 *   A pointer to the SEH record, or EXCEPTION_CHAIN_END if none was found.
 *
 */

PEXCEPTION_REGISTRATION_RECORD
GetClrSEHRecordServicingStackPointer(Thread *pThread,
                                     void *pStackPointer)
{
    ThreadExceptionState* pExState = pThread->GetExceptionState();

    //
    // We can only do this if there is a context in the pExInfo. There are cases (most notably the
    // EEPolicy::HandleFatalError case) where we don't have that.  In these cases we will return
    // no enclosing handler since we cannot accurately determine the FS:0 entry which services
    // this stack address.
    //
    // The side effect of this is that for these cases, the debugger cannot intercept
    // the exception
    //
    CONTEXT* pContextRecord = pExState->GetContextRecord();
    if (pContextRecord == NULL)
    {
        return EXCEPTION_CHAIN_END;
    }

    void *exceptionSP = GetSP(pContextRecord);


    //
    // Now set the establishing frame.  What this means in English is that we need to find
    // the fs:0 entry that handles exceptions for the place on the stack given in stackPointer.
    //
    PEXCEPTION_REGISTRATION_RECORD pSEHRecord = GetFirstCOMPlusSEHRecord(pThread);

    while (pSEHRecord != EXCEPTION_CHAIN_END)
    {

        //
        // Skip any SEHRecord which is not a CLR record or was pushed after the exception
        // on this thread occurred.
        //
        if (ComPlusFrameSEH(pSEHRecord) && (exceptionSP <= (void *)pSEHRecord))
        {
            Frame *pFrame = GetCurrFrame(pSEHRecord);
            //
            // Arcane knowledge here.  All Frame records are stored on the stack by the runtime
            // in ever decreasing address space.  So, we merely have to search back until
            // we find the first frame record with a higher stack value to find the
            // establishing frame for the given stack address.
            //
            if (((void *)pFrame) >= pStackPointer)
            {
                break;
            }

        }

        pSEHRecord = GetNextCOMPlusSEHRecord(pSEHRecord);
    }

    return pSEHRecord;
}

#ifdef _DEBUG
// We've deteremined during a stack walk that managed code is transitioning to unamanaged (EE) code. Check that the
// state of the EH chain is correct.
//
// For x86, check that we do INSTALL_COMPLUS_EXCEPTION_HANDLER before calling managed code.  This check should be
// done for all managed code sites, not just transistions. But this will catch most problem cases.
void VerifyValidTransitionFromManagedCode(Thread *pThread, CrawlFrame *pCF)
{
    WRAPPER_CONTRACT;

    _ASSERTE(ExecutionManager::FindJitMan((PBYTE)GetControlPC(pCF->GetRegisterSet())));

    // Cannot get to the TEB of other threads. So ignore them.
    if (pThread != GetThread())
    {
        return;
    }

    // Find the EH record guarding the current region of managed code, based on the CrawlFrame passed in.
    PEXCEPTION_REGISTRATION_RECORD pEHR = GetCurrentSEHRecord();

    while ((pEHR != EXCEPTION_CHAIN_END) && ((PVOID)pEHR < GetRegdisplaySP(pCF->GetRegisterSet())))
    {
        pEHR = pEHR->Next;
    }

    // Great, we've got the EH record that's next up the stack from the current SP (which is in managed code). That
    // had better be a record for one of our handlers responsible for handling exceptions in managed code. If its
    // not, then someone made it into managed code without setting up one of our EH handlers, and that's really
    // bad.
    CONSISTENCY_CHECK_MSGF(IsUnmanagedToManagedSEHHandler(pEHR),
                           ("Invalid transition into managed code!\n\n"
                            "We're walking this thread's stack and we've reached a managed frame at Esp=0x%p. "
                            "(The method is %s::%s) "
                            "The very next FS:0 record (0x%p) up from this point on the stack should be one of "
                            "our 'unmanaged to managed SEH handlers', but its not... its something else, and "
                            "that's very bad. It indicates that someone managed to call into managed code without "
                            "setting up the proper exception handling.\n\n"
                            "Get a good unmanaged stack trace for this thread. All FS:0 records are on the stack, "
                            "so you can see who installed the last handler. Somewhere between that function and "
                            "where the thread is now is where the bad transition occured.\n\n"
                            "A little extra info: FS:0 = 0x%p, pEHR->Handler = 0x%p\n",
                            GetRegdisplaySP(pCF->GetRegisterSet()),
                            pCF->GetFunction()->m_pszDebugClassName,
                            pCF->GetFunction()->m_pszDebugMethodName,
                            pEHR,
                            GetCurrentSEHRecord(),
                            pEHR->Handler));
}

#endif

//================================================================================

// There are some things that should never be true when handling an
// exception.  This function checks for them.  Will assert or trap
// if it finds an error.
static inline void
CPFH_VerifyThreadIsInValidState(Thread* pThread, DWORD exceptionCode, EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame) {
    WRAPPER_CONTRACT;

    if (   exceptionCode == STATUS_BREAKPOINT
        || exceptionCode == STATUS_SINGLE_STEP) {
        return;
    }

#ifdef _DEBUG
    // check for overwriting of stack
    CheckStackBarrier(pEstablisherFrame);
    // trigger check for bad fs:0 chain
    GetCurrentSEHRecord();
#endif

    if (!g_fEEShutDown) {
        // An exception on the GC thread, or while holding the thread store lock, will likely lock out the entire process.
        if (::IsGCThread() || ThreadStore::HoldingThreadStore())
        {
            _ASSERTE(!"Exception during garbage collection or while holding thread store");
            if (REGUTIL::GetConfigDWORD(L"EHGolden", 0))
            {
                DebugBreak();
            }

            EEPOLICY_HANDLE_FATAL_ERROR(COR_E_EXECUTIONENGINE);
        }
    }
}




// We want to leave true null reference exceptions alone.  But if we are
// trashing memory, we don't want the application to swallow it.  The 0x100
// below will give us false positives for debugging, if the app is accessing
// a field more than 256 bytes down an object, where the reference is null.
//
// Removed use of the IgnoreUnmanagedExceptions reg key...simply return false now.
//
static inline BOOL
CPFH_ShouldIgnoreException(EXCEPTION_RECORD *pExceptionRecord) {
    LEAF_CONTRACT;
     return FALSE;
}

static inline void
CPFH_UpdatePerformanceCounters() {
    WRAPPER_CONTRACT;
    COUNTER_ONLY(GetPrivatePerfCounters().m_Excep.cThrown++);
    if ( ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) ) {
        g_pEtwTracer->EtwTraceEvent(&ExceptionGuid,
                                    ETW_TYPE_EXCEPTION_THROWN,
                                    NULL, 0);
    }

}


//******************************************************************************
EXCEPTION_DISPOSITION COMPlusAfterUnwind(
        EXCEPTION_RECORD *pExceptionRecord,
        EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
        ThrowCallbackType& tct)
{
    WRAPPER_CONTRACT;

    // Note: we've completed the unwind pass up to the establisher frame, and we're headed off to finish our
    // cleanup and end up back in jitted code. Any more FS0 handlers pushed from this point on out will _not_ be
    // unwound. We go ahead and assert right here that indeed there are no handlers below the establisher frame
    // before we go any further.
    _ASSERTE(pEstablisherFrame == GetCurrentSEHRecord());

    DWORD exceptionCode = pExceptionRecord->ExceptionCode;
    Thread* pThread = GetThread();
    ExInfo* pExInfo = &(pThread->GetExceptionState()->m_currentExInfo);

    _ASSERTE(tct.pCurrentExceptionRecord == pEstablisherFrame);

    NestedHandlerExRecord nestedHandlerExRecord;
    nestedHandlerExRecord.Init(0, (PEXCEPTION_ROUTINE)COMPlusNestedExceptionHandler, GetCurrFrame(pEstablisherFrame));

    // ... and now, put the nested record back on.
    INSTALL_EXCEPTION_HANDLING_RECORD(&(nestedHandlerExRecord.m_ExReg));

    if (!pThread->PreemptiveGCDisabled())
        pThread->DisablePreemptiveGC();

    tct.bIsUnwind = TRUE;
    tct.pProfilerNotify = NULL;

    // save catch handler of  catch so can unwind our nested handler info to the right spot if necessary
    pExInfo->m_pCatchHandler = pEstablisherFrame;

    LOG((LF_EH, LL_INFO100, "COMPlusFrameHandler: unwinding\n"));

    tct.bUnwindStack = CPFH_ShouldUnwindStack(exceptionCode);

    LOG((LF_EH, LL_INFO1000, "COMPlusAfterUnwind: going to: pFunc:%#X, pStack:%#X\n",
        tct.pFunc, tct.pStack));

    pThread->ResetThreadStateNC(Thread::TSNC_SOIntolerant);

    UnwindFrames(pThread, &tct);
    _ASSERTE(!"Should not get here");
    return ExceptionContinueSearch;
} // EXCEPTION_DISPOSITION COMPlusAfterUnwind()

#ifdef DEBUGGING_SUPPORTED

EXCEPTION_DISPOSITION ClrDebuggerDoUnwindAndIntercept(EXCEPTION_REGISTRATION_RECORD *pCurrentEstablisherFrame,
                                                      EXCEPTION_RECORD *pExceptionRecord)
{
    WRAPPER_CONTRACT;

    if (!CheckThreadExceptionStateForInterception())
    {
        return ExceptionContinueSearch;
    }

    Thread*               pThread  = GetThread();
    ThreadExceptionState* pExState = pThread->GetExceptionState();

    EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame;
    ThrowCallbackType tct;
    tct.Init();


    pExState->GetDebuggerState()->GetDebuggerInterceptInfo(&pEstablisherFrame,
                                      &(tct.pFunc),
                                      &(tct.dHandler),
                                      &(tct.pStack),
                                      NULL,
                                      &(tct.pBottomFrame)
                                     );

    //
    // If the handler that we've selected as the handler for the target frame of the unwind is in fact above the
    // handler that we're currently executing in, then use the current handler instead. Why? Our handlers for
    // nested exceptions actually process managed frames that live above them, up to the COMPlusFrameHanlder that
    // pushed the nested handler. If the user selectes a frame above the nested handler, then we will have selected
    // the COMPlusFrameHandler above the current nested handler. But we don't want to ask RtlUnwind to unwind past
    // the nested handler that we're currently executing in.
    //
    if (pEstablisherFrame > pCurrentEstablisherFrame)
    {
        // This should only happen if we're in a COMPlusNestedExceptionHandler.
        _ASSERTE(IsComPlusNestedExceptionRecord(pCurrentEstablisherFrame));

        pEstablisherFrame = pCurrentEstablisherFrame;
    }

#ifdef _DEBUG
    tct.pCurrentExceptionRecord = pEstablisherFrame;
#endif

    LOG((LF_EH|LF_CORDB, LL_INFO100, "ClrDebuggerDoUnwindAndIntercept: Intercepting at %s\n", tct.pFunc->m_pszDebugMethodName));
    LOG((LF_EH|LF_CORDB, LL_INFO100, "\t\t: pFunc is 0x%X\n", tct.pFunc));
    LOG((LF_EH|LF_CORDB, LL_INFO100, "\t\t: pStack is 0x%X\n", tct.pStack));

    pEstablisherFrame->dwFlags |= PAL_EXCEPTION_FLAGS_UnwindCallback;
    return ExceptionStackUnwind;
} // EXCEPTION_DISPOSITION ClrDebuggerDoUnwindAndIntercept()

#endif // DEBUGGING_SUPPORTED

//******************************************************************************
// The essence of the first pass handler (after we've decided to actually do
//  the first pass handling).
//******************************************************************************
inline EXCEPTION_DISPOSITION __cdecl
CPFH_RealFirstPassHandler(                  // ExceptionContinueSearch, etc.
    EXCEPTION_RECORD *pExceptionRecord,     // The exception record, with exception type.
    EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,   // Exception frame on whose behalf this is called.
    CONTEXT     *pContext,                  // Context from the exception.
    void        *pDispatcherContext,
    BOOL        bAsynchronousThreadStop,
    BOOL        fPGCDisabledOnEntry)
{
    // We don't want to use a runtime contract here since this codepath is used during
    // the processing of a hard SO. Contracts use a significant amount of stack
    // which we can't afford for those cases.
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_SO_TOLERANT;

#ifdef _DEBUG
    static int breakOnFirstPass = -1;

    if (breakOnFirstPass == -1)
        breakOnFirstPass = REGUTIL::GetConfigDWORD(L"BreakOnFirstPass", 0);

    if (breakOnFirstPass != 0)
    {
        _ASSERTE(!"First pass exception handler");
    }
#endif

    EXCEPTION_DISPOSITION retval;
    DWORD exceptionCode = pExceptionRecord->ExceptionCode;
    Thread *pThread = GetThread();

    static int breakOnAV = -1;
    if (breakOnAV == -1)
        breakOnAV = REGUTIL::GetConfigDWORD(L"BreakOnAV", 0);

    if (breakOnAV != 0 && exceptionCode == STATUS_ACCESS_VIOLATION)
    {
#ifdef _DEBUG
        _ASSERTE(!"AV occured");
#else
        if (REGUTIL::GetConfigDWORD(L"EHGolden", 0))
            DebugBreak();
#endif
    }

    static int breakOnSO = -1;

    if (breakOnSO == -1)
        breakOnSO = REGUTIL::GetConfigDWORD(L"BreakOnSO", 0);

    if (breakOnSO != 0 && exceptionCode == STATUS_STACK_OVERFLOW)
    {
#ifdef _DEBUG
        DebugBreak();   // ASSERTing will blow the guard region
#else
        if (REGUTIL::GetConfigDWORD(L"EHGolden", 0))
            DebugBreak();
#endif
    }
    // We always want to be in co-operative mode when we run this function and whenever we return
    // from it, want to go to pre-emptive mode because are returning to OS.
    _ASSERTE(pThread->PreemptiveGCDisabled());

    BOOL bPopNestedHandlerExRecord = FALSE;
    LFH found = LFH_NOT_FOUND;          // Result of calling LookForHandler.
    BOOL bRethrownException = FALSE;
    BOOL bNestedException = FALSE;

#if defined(USE_FEF)
    BOOL bPopFaultingExceptionFrame = FALSE;
    FrameWithCookie<FaultingExceptionFrame> faultingExceptionFrame;
#endif // USE_FEF
    ExInfo* pExInfo = &(pThread->GetExceptionState()->m_currentExInfo);

    ThrowCallbackType &tct = ((FrameHandlerExRecord *)pEstablisherFrame)->m_tct;
    tct.Init();


    tct.pTopFrame = GetCurrFrame(pEstablisherFrame); // highest frame to search to

    if (bAsynchronousThreadStop)
    {
        tct.bLastChance = FALSE;
    }

#ifdef _DEBUG
    tct.pCurrentExceptionRecord = pEstablisherFrame;
    tct.pPrevExceptionRecord    = GetPrevSEHRecord(pEstablisherFrame);
#endif // _DEBUG

    ICodeManager *pMgr = ExecutionManager::FindCodeMan((SLOT)GetIP(pContext));


    // this establishes a marker so can determine if are processing a nested exception
    // don't want to use the current frame to limit search as it could have been unwound by
    // the time get to nested handler (ie if find an exception, unwind to the call point and
    // then resume in the catch and then get another exception) so make the nested handler
    // have the same boundary as this one. If nested handler can't find a handler, we won't
    // end up searching this frame list twice because the nested handler will set the search
    // boundary in the thread and so if get back to this handler it will have a range that starts
    // and ends at the same place.

    NestedHandlerExRecord nestedHandlerExRecord;
    nestedHandlerExRecord.Init(0, (PEXCEPTION_ROUTINE)COMPlusNestedExceptionHandler, GetCurrFrame(pEstablisherFrame));

    INSTALL_EXCEPTION_HANDLING_RECORD(&(nestedHandlerExRecord.m_ExReg));
    bPopNestedHandlerExRecord = TRUE;

#if defined(USE_FEF)
    // Note: don't attempt to push a FEF for an exception in managed code if we weren't in cooperative mode when
    // the exception was received. If preemptive GC was enabled when we received the exception, then it means the
    // exception was rethrown from unmangaed code (including EE impl), and we shouldn't push a FEF.
    if (pMgr &&
        fPGCDisabledOnEntry &&
        (pThread->m_pFrame == FRAME_TOP ||
         pThread->m_pFrame->GetVTablePtr() != FaultingExceptionFrame::GetMethodFrameVPtr() ||
         (size_t)pThread->m_pFrame > (size_t)pEstablisherFrame))
    {
        // setup interrupted frame so that GC during calls to init won't collect the frames
        // only need it for non COM+ exceptions in managed code when haven't already
        // got one on the stack (will have one already if we have called rtlunwind because
        // the instantiation that called unwind would have installed one)
        faultingExceptionFrame.InitAndLink(pContext);
        bPopFaultingExceptionFrame = TRUE;
    }
#endif // USE_FEF

    OBJECTREF e;
    e = pThread->LastThrownObject();


    STRESS_LOG7(LF_EH, LL_INFO10, "CPFH_RealFirstPassHandler: code:%X, LastThrownObject:%p, MT:%pT"
        ", IP:%p, SP:%p, pContext:%p, pEstablisherFrame:%p\n",
        exceptionCode, OBJECTREFToObject(e), (e!=0)?e->GetMethodTable():0,
        GetIP(pContext), GetSP(pContext), pContext, pEstablisherFrame);

#ifdef LOGGING
    // If it is a complus exception, and there is a thrown object, get its name, for better logging.
    if (exceptionCode == EXCEPTION_COMPLUS)
    {
        const char * eClsName = "!EXCEPTION_COMPLUS";
        if (e != 0)
        {
            eClsName = e->GetTrueMethodTable()->GetDebugClassName();
    }
        LOG((LF_EH, LL_INFO100, "CPFH_RealFirstPassHandler: exception: 0x%08X, class: '%s', IP: 0x%p\n",
         exceptionCode, eClsName, GetIP(pContext)));
    }
#endif

    EXCEPTION_POINTERS exceptionPointers = {pExceptionRecord, pContext};

    LOG((LF_EH, LL_INFO100, "CPFH_RealFirstPassHandler: setting boundaries: BottomMostHandler:0x%p, SearchBoundary:0x%p, TopFrame:0x%p\n",
         pExInfo->m_pBottomMostHandler, pExInfo->m_pSearchBoundary, tct.pTopFrame));

    // Here we are trying to decide if we are coming in as:
    // 1) first handler in a brand new exception
    // 2) a subsequent handler in an exception
    // 3) a nested exception
    // m_pBottomMostHandler is the registration structure (establisher frame) for the most recent (ie lowest in
    // memory) non-nested handler that was installed  and pEstablisher frame is what the current handler
    // was registered with.
    // The OS calls each registered handler in the chain, passing its establisher frame to it.
    if (pExInfo->m_pBottomMostHandler != NULL && pEstablisherFrame > pExInfo->m_pBottomMostHandler)
    {
        LOG((LF_EH, LL_INFO1000, "CPFH_RealFirstPassHandler: detected subsequent handler.  BottomMost:0x%p SearchBoundary:0x%p\n",
             pExInfo->m_pBottomMostHandler, pExInfo->m_pSearchBoundary));
        
        // If the establisher frame of this handler is greater than the bottommost then it must have been
        // installed earlier and therefore we are case 2
        if (pThread->GetThrowable() == NULL)
        {
            // Bottommost didn't setup a throwable, so not exception not for us
            retval = ExceptionContinueSearch;
            goto exit;
        }

        // setup search start point
        tct.pBottomFrame = pExInfo->m_pSearchBoundary;

        if (tct.pTopFrame == tct.pBottomFrame)
        {
            // this will happen if our nested handler already searched for us so we don't want
            // to search again
            retval = ExceptionContinueSearch;
            goto exit;
        }
    }
    else
    {   // we are either case 1 or case 3
#if defined(_DEBUG_IMPL)
        if (pThread->GetFrame() != FRAME_TOP)
            pThread->GetFrame()->LogFrameChain(LF_EH, LL_INFO1000);
#endif // _DEBUG_IMPL

        // If the exception was rethrown, we'll create a new ExInfo, which will represent the rethrown exception.
        //  The original exception is not the rethrown one.
        if (pExInfo->m_ExceptionFlags.IsRethrown() && pThread->LastThrownObject() != NULL)
        {
            pExInfo->m_ExceptionFlags.ResetIsRethrown();
            bRethrownException = TRUE;

#if defined(USE_FEF)
            if (bPopFaultingExceptionFrame)
            {
                // if we added a FEF, it will refer to the frame at the point of the original exception which is
                // already unwound so don't want it.
                // If we rethrew the exception we have already added a helper frame for the rethrow, so don't
                // need this one. If we didn't rethrow it, (ie rethrow from native) then there the topmost frame will
                // be a transition to native frame in which case we don't need it either
                faultingExceptionFrame.Pop();
                bPopFaultingExceptionFrame = FALSE;
            }
#endif
        }

        // if the establisher frame is less than the bottommost handler, then this is nested because the
        // establisher frame was installed after the bottommost
        if (pEstablisherFrame < pExInfo->m_pBottomMostHandler
            /* || IsComPlusNestedExceptionRecord(pEstablisherFrame) */ )
        {
            bNestedException = TRUE;

            // case 3: this is a nested exception. Need to save and restore the thread info
            LOG((LF_EH, LL_INFO1000, "CPFH_RealFirstPassHandler: detected nested exception 0x%p < 0x%p\n",
                 pEstablisherFrame, pExInfo->m_pBottomMostHandler));

            EXCEPTION_REGISTRATION_RECORD* pNestedER = TryFindNestedEstablisherFrame(pEstablisherFrame);
            ExInfo *pNestedExInfo;

            if (!pNestedER || pNestedER >= pExInfo->m_pBottomMostHandler )
            {
                // RARE CASE.  We've re-entered the EE from an unmanaged filter.
                void *limit = (void *) GetPrevSEHRecord(pExInfo->m_pBottomMostHandler);

                pNestedExInfo = new (nothrow) ExInfo();     // Very rare failure here; need robust allocator.
                if (pNestedExInfo == NULL)
                {   // if we can't allocate memory, we can't correctly continue.
                    #if defined(_DEBUG)
                    if (REGUTIL::GetConfigDWORD(L"NestedEhOom", 0))
                        _ASSERTE(!"OOM in callback from unmanaged filter.");
                    #endif // _DEBUG
                    
                    EEPOLICY_HANDLE_FATAL_ERROR(COR_E_OUTOFMEMORY);
                }

                
                pNestedExInfo->m_StackAddress = limit;      // Note: this is also the flag that tells us this
                                                            // ExInfo was stack allocated.
            }
            else
            {
                pNestedExInfo = &((NestedHandlerExRecord*)pNestedER)->m_handlerInfo;
            }

            LOG((LF_EH, LL_INFO100, "CPFH_RealFirstPassHandler: PushExInfo() current: 0x%p previous: 0x%p\n", 
                 pExInfo->m_StackAddress, pNestedExInfo->m_StackAddress));

            _ASSERTE(pNestedExInfo);
            pNestedExInfo->m_hThrowable = NULL; // pNestedExInfo may be stack allocated, and as such full of
                                                // garbage. m_hThrowable must be sane, so set it to NULL. (We could
                                                // zero the entire record, but this is cheaper.)

            pNestedExInfo->CopyAndClearSource(pExInfo);

            pExInfo->m_pPrevNestedInfo = pNestedExInfo;     // Save at head of nested info chain

        }

        // If we are handling a fault from managed code, we need to set the Thread->ExInfo->pContext to
        //  the current fault context, which is used in the stack walk to get back into the managed
        //  stack with the correct registers.  (Previously, this was done by linking in a FaultingExceptionFrame
        //  record.)
        // We are about to create the managed exception object, which may trigger a GC, so set this up now.

        memcpy(&pExInfo->m_ExceptionRecord, pExceptionRecord, sizeof(EXCEPTION_RECORD));
        pExInfo->m_pContext = NULL;
        // It should now be safe for a GC to happen.

        // case 1 & 3: this is the first time through of a new, nested, or rethrown exception, so see if we can
        // find a handler.  Only setup throwable if are bottommost handler
        if ((exceptionCode == EXCEPTION_COMPLUS) && (!bAsynchronousThreadStop))
        {

            // Update the throwable from the last thrown object. Note: this may cause OOM, in which case we replace
            // both throwables with the preallocated OOM exception.
            pThread->SafeSetThrowables(pThread->LastThrownObject());

            // now we've got a COM+ exception, fall through to so see if we handle it

            pExInfo->m_pBottomMostHandler = pEstablisherFrame;
        }
        else if (bRethrownException)
        {
            // If it was rethrown and not COM+, will still be the last one thrown. Either we threw it last and
            // stashed it here or someone else caught it and rethrew it, in which case it will still have been
            // originally stashed here.

            // Update the throwable from the last thrown object. Note: this may cause OOM, in which case we replace
            // both throwables with the preallocated OOM exception.
            pThread->SafeSetThrowables(pThread->LastThrownObject());
            pExInfo->m_pBottomMostHandler = pEstablisherFrame;

        }
        else
        {
            if (pMgr == NULL)
            {
                tct.bDontCatch = false;
            }

            if (exceptionCode == STATUS_BREAKPOINT)
            {
                // don't catch int 3
                retval = ExceptionContinueSearch;
                goto exit;
            }

            // We need to set m_pBottomMostHandler here, Thread::IsExceptionInProgress returns 1.
            // This is a necessary part of suppressing thread abort exceptions in the constructor
            // of any exception object we might create.
            pExInfo->m_pBottomMostHandler = pEstablisherFrame;

            // Create the managed exception object.
            OBJECTREF throwable = CreateCOMPlusExceptionObject(pThread, pExceptionRecord, bAsynchronousThreadStop);

            // Set the throwables on the thread to the newly created object. If this fails, it will return a
            // preallocated exception object instead. This also updates the last thrown exception, for rethrows.
            throwable = pThread->SafeSetThrowables(throwable);

            // Set the exception code and pointers. We set these after setting the throwables on the thread,
            // because if the proper exception is replaced by an OOM exception, we still want the exception code
            // and pointers set in the OOM exception.
            EXCEPTIONREF exceptionRef = (EXCEPTIONREF)throwable;
            exceptionRef->SetXCode(pExceptionRecord->ExceptionCode);
            exceptionRef->SetXPtrs(&exceptionPointers);
        }

        LOG((LF_EH, LL_INFO100, "CPFH_RealFirstPassHandler: m_pBottomMostHandler is now 0x%p\n",
             pExInfo->m_pBottomMostHandler));

        tct.pBottomFrame = NULL;

        EEToProfilerExceptionInterfaceWrapper::ExceptionThrown(pThread);
        
        CPFH_UpdatePerformanceCounters();
    } // End of case-1-or-3

    {
        // Allocate storage for the stack trace.
        OBJECTREF throwable = pThread->GetThrowable();

        // If we're out of memory, then we figure there's probably not memory to maintain a stack trace, so we skip it.
        // If we've got a stack overflow, then we figure the stack will be so huge as to make tracking the stack trace
        // impracticle, so we skip it.
        if ((throwable == CLRException::GetPreallocatedOutOfMemoryException()) ||
            (throwable == CLRException::GetPreallocatedStackOverflowException()))
        {
            tct.bAllowAllocMem = FALSE;
        }
        else
        {
            pExInfo->m_StackTraceInfo.AllocateStackTrace();
        }
    }

    // Set up information for GetExceptionPointers()/GetExceptionCode() callback.
    pExInfo->m_ExceptionCode = exceptionCode;

#ifdef DEBUGGING_SUPPORTED
    //
    // At this point the exception is still fresh to us, so assert that
    // there should be nothing from the debugger on it.
    //
    _ASSERTE(!pExInfo->m_ExceptionFlags.DebuggerInterceptInfo());
#endif

    if (pThread->IsRudeAbort())
    {
        OBJECTREF rudeAbortThrowable = CLRException::GetPreallocatedRudeThreadAbortException();

        if (pThread->GetThrowable() != rudeAbortThrowable)
        {
            // Neither of these sets will throw because the throwable that we're setting is a preallocated
            // exception. This also updates the last thrown exception, for rethrows.
            pThread->SafeSetThrowables(rudeAbortThrowable);
        }

        if (!pThread->IsRudeAbortInitiated())
        {
            pThread->PreWorkForThreadAbort();
        }
    }



    LOG((LF_EH, LL_INFO100, "CPFH_RealFirstPassHandler: looking for handler bottom %x, top %x\n",
         tct.pBottomFrame, tct.pTopFrame));
    tct.bReplaceStack = pExInfo->m_pBottomMostHandler == pEstablisherFrame && !bRethrownException;
    tct.bSkipLastElement = bRethrownException && bNestedException;
    found = LookForHandler(&exceptionPointers,
                                pThread,
                                &tct);

    // We have searched this far.
    pExInfo->m_pSearchBoundary = tct.pTopFrame;
    LOG((LF_EH, LL_INFO1000, "CPFH_RealFirstPassHandler: set pSearchBoundary to 0x%p\n", pExInfo->m_pSearchBoundary));

    if ((found == LFH_NOT_FOUND)
#ifdef DEBUGGING_SUPPORTED
        && !pExInfo->m_ExceptionFlags.DebuggerInterceptInfo()
#endif
        )
    {
        LOG((LF_EH, LL_INFO100, "CPFH_RealFirstPassHandler: NOT_FOUND\n"));

        if (tct.pTopFrame == FRAME_TOP)
        {
            LOG((LF_EH, LL_INFO100, "CPFH_RealFirstPassHandler: NOT_FOUND at FRAME_TOP\n"));

            // Possibly capture Watson buckets based on configuration.  Allows !WatsonBuckets in
            //  the debugger on second chance exceptions.
            GetThread()->GetExceptionState()->MaybeEagerlyCaptureUnhandledInfoForWatson();
        }

        retval = ExceptionContinueSearch;
        goto exit;
    }

    // so we are going to handle the exception

    // Remove the nested exception record -- before calling RtlUnwind.
    // The second-pass callback for a NestedExceptionRecord assumes that if it's
    // being unwound, it should pop one exception from the pExInfo chain.  This is
    // true for any older NestedRecords that might be unwound -- but not for the
    // new one we're about to add.  To avoid this, we remove the new record
    // before calling Unwind.
    //
    _ASSERTE(bPopNestedHandlerExRecord);

    UNINSTALL_EXCEPTION_HANDLING_RECORD(&(nestedHandlerExRecord.m_ExReg));

    pThread->EnablePreemptiveGC();

#ifdef DEBUGGING_SUPPORTED
    //
    // Check if the debugger wants to intercept this frame at a different point than where we are.
    //
    if (pExInfo->m_ExceptionFlags.DebuggerInterceptInfo())
    {
        ClrDebuggerDoUnwindAndIntercept(pEstablisherFrame, pExceptionRecord);

        //
        // If this returns, then the debugger couldn't do it's stuff and we default to the found handler.
        //
        if (found == LFH_NOT_FOUND)
        {
            retval = ExceptionContinueSearch;
            goto exit;
        }
    }
#endif

    LOG((LF_EH, LL_INFO100, "CPFH_RealFirstPassHandler: handler found: %s\n", tct.pFunc->m_pszDebugMethodName));

    pEstablisherFrame->dwFlags |= PAL_EXCEPTION_FLAGS_UnwindCallback;
    return ExceptionStackUnwind;

exit:
    EEToProfilerExceptionInterfaceWrapper::ExceptionOSHandlerLeave(pThread, &(tct.pProfilerNotify));

    // If we got as far as saving pExInfo, save the context pointer so it's available for the unwind.

#if defined(USE_FEF)
    if (bPopFaultingExceptionFrame)
    {
        faultingExceptionFrame.Pop();
    }
#endif // USE_FEF

    if (bPopNestedHandlerExRecord)
    {
        UNINSTALL_EXCEPTION_HANDLING_RECORD(&(nestedHandlerExRecord.m_ExReg));
    }
    return retval;
} // CPFH_RealFirstPassHandler()


//******************************************************************************
//
void InitializeExceptionHandling()
{
    WRAPPER_CONTRACT;
    InitSavedExceptionInfo();
    CLRAddVectoredHandlers();
}

#if defined(PLATFORM_UNIX) && defined(_X86_)
static
int CPFH_AdjustArithmeticException(EXCEPTION_RECORD *pExceptionRecord, CONTEXT * pContext)
{
    LEAF_CONTRACT;
    /* We check for  0x80000000 / -1 and set exception code to STATUS_INTEGER_OVERFLOW*/
    /* NT and Memphis are reporting STATUS_INTEGER_OVERFLOW, whereas */
    /* Win95, OSR1, OSR2, FreeBSD are reporting STATUS_DIVIDED_BY_ZERO */

    _ASSERTE(pExceptionRecord->ExceptionCode  == STATUS_INTEGER_DIVIDE_BY_ZERO);

    BYTE *        code     = (BYTE*)(size_t)pContext->Eip;
    int           divisor  = 0;

    // Check that eip points at a "idiv ..." instruction
    if (*code++ != 0xF7)
        return false;

    switch (*code++)
    {
        /* idiv [EBP+d8]   F7 7D 'd8' */
    case 0x7D :
        divisor = *((int*)(size_t)(*((INT8 *)code) + pContext->Ebp));
        break;

        /* idiv [EBP+d32] F7 BD 'd32' */
    case 0xBD:
        divisor = *((int*)(size_t)(*((INT32*)code) + pContext->Ebp));
        break;

        /* idiv [ESP]     F7 3C 24 */
    case 0x3C:
        if (*code++ != 0x24)
            break;

        divisor = *((int*)(size_t) pContext->Esp);
        break;

        /* idiv [ESP+d8]  F7 7C 24 'd8' */
    case 0x7C:
        if (*code++ != 0x24)
            break;
        divisor = *((int*)(size_t)(*((INT8 *)code) + pContext->Esp));
        break;

        /* idiv [ESP+d32]  F7 BC 24 'd32' */
    case 0xBC:
        if (*code++ != 0x24)
            break;
        divisor = *((int*)(size_t)(*((INT32*)code) + pContext->Esp));
        break;

        /* idiv reg        F7 F8..FF */
    case 0xF8:
        divisor = (unsigned) pContext->Eax;
        break;

    case 0xF9:
        divisor = (unsigned) pContext->Ecx;
        break;

    case 0xFA:
        divisor = (unsigned) pContext->Edx;
        break;

    case 0xFB:
        divisor = (unsigned) pContext->Ebx;
        break;

#ifdef _DEBUG
    case 0xFC: //div esp will not be issued
        assert(!"'div esp' is a silly instruction");
#endif // _DEBUG

    case 0xFD:
        divisor = (unsigned) pContext->Ebp;
        break;

    case 0xFE:
        divisor = (unsigned) pContext->Esi;
        break;

    case 0xFF:
        divisor = (unsigned) pContext->Edi;
        break;

    default:
        break;
    }

    if (divisor != -1)
        return false;

    /* It's the special case, fix the exception code and return true */
    pExceptionRecord->ExceptionCode = STATUS_INTEGER_OVERFLOW;

    return true;
}
#endif // PLATFORM_UNIX && _X86_


//******************************************************************************
static inline EXCEPTION_DISPOSITION __cdecl
CPFH_FirstPassHandler(EXCEPTION_RECORD *pExceptionRecord,
                      EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
                      CONTEXT *pContext,
                      DISPATCHER_CONTEXT *pDispatcherContext)
{
    WRAPPER_CONTRACT;
    EXCEPTION_DISPOSITION retval;

    _ASSERTE (!(pExceptionRecord->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND)));

    DWORD exceptionCode = pExceptionRecord->ExceptionCode;

    Thread *pThread = GetThread();

    STRESS_LOG4(LF_EH, LL_INFO100,
                "CPFH_FirstPassHandler: pEstablisherFrame = %x EH code = %x  EIP = %x with ESP = %x\n",
        pEstablisherFrame, exceptionCode, GetIP(pContext), GetSP(pContext));

    EXCEPTION_POINTERS ptrs = { pExceptionRecord, pContext };

    // Call to the vectored handler to give other parts of the Runtime a chance to jump in and take over an
    // exception before we do too much with it. The most important point in the vectored handler is not to toggle
    // the GC mode.
    DWORD filter = CLRVectoredExceptionHandler(&ptrs);

    if (filter == (DWORD) EXCEPTION_CONTINUE_EXECUTION)
    {
        return ExceptionContinueExecution;
    }
    else if (filter == EXCEPTION_CONTINUE_SEARCH)
    {
        return ExceptionContinueSearch;
    }

#if defined(STRESS_HEAP)
#endif // STRESS_HEAP

    // We always want to be in co-operative mode when we run this function and whenever we return
    // from it, want to go to pre-emptive mode because are returning to OS.
    BOOL disabled = pThread->PreemptiveGCDisabled();

    if (!disabled)
    {
        pThread->DisablePreemptiveGC();
    }

    BOOL bAsynchronousThreadStop = IsThreadHijackedForThreadStop(pThread, pExceptionRecord);

    if (bAsynchronousThreadStop)
    {
        // If we ever get here in preemptive mode, we're in trouble.  We've
        // changed the thread's IP to point at a little function that throws ... if
        // the thread were to be in preemptive mode and a GC occured, the stack
        // crawl would have been all messed up (becuase we have no frame that points
        // us back to the right place in managed code).
        _ASSERTE(disabled);

        AdjustContextForThreadStop(pThread, pContext);
        LOG((LF_EH, LL_INFO100, "CPFH_FirstPassHandler is Asynchronous Thread Stop or Abort\n"));
    }
#if defined(PLATFORM_UNIX) && defined(_X86_)
    else if (exceptionCode == STATUS_INTEGER_DIVIDE_BY_ZERO )
    {
        // FreeBSD like Win95 doesn't report 0x800000/-1 as an overflow but instead as division by zero here we
        // catch this case and change the exception code.
        if ( CPFH_AdjustArithmeticException(pExceptionRecord, pContext) )
        {
            exceptionCode = pExceptionRecord->ExceptionCode; // = STATUS_INTEGER_OVERFLOW;
        }
    }
#endif // PLATFORM_UNIX && _X86_

    pThread->ResetThrowControlForThread();

    CPFH_VerifyThreadIsInValidState(pThread, exceptionCode, pEstablisherFrame);

    // If we were in cooperative mode when we came in here, then its okay to see if we should do HandleManagedFault
    // and push a FaultingExceptionFrame. If we weren't in coop mode coming in here, then it means that there's no
    // way the exception could really be from managed code. I might look like it was from managed code, but in
    // reality its a rethrow from unmanaged code, either unmanaged user code, or unmanaged EE implementation.
    if (disabled && ShouldHandleManagedFault(pExceptionRecord, pContext, pEstablisherFrame, pThread))
    {
#if defined(USE_FEF)
        HandleManagedFault(pExceptionRecord, pContext, pEstablisherFrame, pThread);
        retval = ExceptionContinueExecution;
        goto exit;
#else

#endif // USE_FEF
    }

    // OK. We're finally ready to start the real work. Nobody else grabbed the exception in front of us. Now we can
    // get started.
    retval = CPFH_RealFirstPassHandler(pExceptionRecord,
                                       pEstablisherFrame,
                                       pContext,
                                       pDispatcherContext,
                                       bAsynchronousThreadStop,
                                       disabled);

    if (retval == ExceptionStackUnwind)
    {
        // Exit immediately - PreemptiveGC flag has been already set.
        return retval;
    }


#if defined(USE_FEF) // This label is only used in the HandleManagedFault() case above.
exit:
#endif
    if (retval != ExceptionContinueExecution || !disabled)
    {
        pThread->EnablePreemptiveGC();
    }

    STRESS_LOG1(LF_EH, LL_INFO100, "CPFH_FirstPassHandler: exiting with retval %d\n", retval);
    return retval;
} // CPFH_FirstPassHandler()

//******************************************************************************
inline void
CPFH_UnwindFrames1(Thread* pThread, EXCEPTION_REGISTRATION_RECORD* pEstablisherFrame, DWORD exceptionCode)
{
    WRAPPER_CONTRACT;

    ExInfo* pExInfo = &(pThread->GetExceptionState()->m_currentExInfo);

    // Ready to unwind the stack...
    ThrowCallbackType tct;
    tct.Init();
    tct.bIsUnwind = TRUE;
    tct.pTopFrame = GetCurrFrame(pEstablisherFrame); // highest frame to search to
    tct.pBottomFrame = NULL;


    #ifdef _DEBUG
    tct.pCurrentExceptionRecord = pEstablisherFrame;
    tct.pPrevExceptionRecord = GetPrevSEHRecord(pEstablisherFrame);
    #endif

    #ifdef DEBUGGING_SUPPORTED
        EXCEPTION_REGISTRATION_RECORD *pInterceptEstablisherFrame = NULL;
    
        if (pExInfo->m_ExceptionFlags.DebuggerInterceptInfo())
        {
            pExInfo->m_DebuggerExState.GetDebuggerInterceptInfo(&pInterceptEstablisherFrame,
                                              NULL,     // MethodDesc **ppFunc,                              
                                              NULL,     // int *pdHandler,                                   
                                              NULL,     // BYTE **ppStack                                    
                                              NULL,     // ULONG_PTR *pNativeOffset,                        
                                              NULL      // Frame **ppFrame)                                 
                                             );
            LOG((LF_EH, LL_INFO1000, "CPFH_UnwindFrames1: frames are Est 0x%X, Intercept 0x%X\n",
                 pEstablisherFrame, pInterceptEstablisherFrame));
    
            //
            // When we set up for the interception we store off the CPFH or CPNEH that we
            // *know* will handle unwinding the destination of the intercept.
            //
            // However, a CPNEH with the same limiting Capital-F-rame could do the work
            // and unwind us, so...
            //
            // If this is the exact frame handler we are supposed to search for, or
            // if this frame handler services the same Capital-F-rame as the frame handler
            // we are looking for (i.e. this frame handler may do the work that we would
            // expect our frame handler to do),
            // then
            //   we need to pass the interception destination during this unwind.
            //
            _ASSERTE(ComPlusFrameSEH(pEstablisherFrame));
    
            if ((pEstablisherFrame == pInterceptEstablisherFrame) ||
                (GetCurrFrame(pEstablisherFrame) == GetCurrFrame(pInterceptEstablisherFrame)))
            {
                pExInfo->m_DebuggerExState.GetDebuggerInterceptInfo(NULL,
                                              &(tct.pFunc),
                                              &(tct.dHandler),
                                              &(tct.pStack),
                                              NULL,
                                              &(tct.pBottomFrame)
                                             );
    
                LOG((LF_EH, LL_INFO1000, "CPFH_UnwindFrames1: going to: pFunc:%#X, pStack:%#X\n",
                    tct.pFunc, tct.pStack));
    
            }
    
        }
    #endif
    
    UnwindFrames(pThread, &tct);

    LOG((LF_EH, LL_INFO1000, "CPFH_UnwindFrames1: after unwind ec:%#x, tct.pTopFrame:0x%p, pSearchBndry:0x%p\n"
                             "                    pEstFrame:0x%p, IsC+NestExRec:%d, !Nest||Active:%d\n", 
         exceptionCode, tct.pTopFrame, pExInfo->m_pSearchBoundary, pEstablisherFrame,  
         IsComPlusNestedExceptionRecord(pEstablisherFrame), 
         (!IsComPlusNestedExceptionRecord(pEstablisherFrame) || reinterpret_cast<NestedHandlerExRecord*>(pEstablisherFrame)->m_ActiveForUnwind)));

    if (tct.pTopFrame >= pExInfo->m_pSearchBoundary &&
         (!IsComPlusNestedExceptionRecord(pEstablisherFrame) ||
          reinterpret_cast<NestedHandlerExRecord*>(pEstablisherFrame)->m_ActiveForUnwind) )
    {
        // If this is the search boundary, and we're not a nested handler, then
        // this is the last time we'll see this exception.  Time to unwind our
        // exinfo.
        STRESS_LOG0(LF_EH, LL_INFO100, "CPFH_UnwindFrames1: Exception unwind -- unmanaged catcher detected\n");
        pExInfo->UnwindExInfo((VOID*)pEstablisherFrame);
    }

    // Notify the profiler that we are leaving this SEH entry
    EEToProfilerExceptionInterfaceWrapper::ExceptionOSHandlerLeave(pThread, &(tct.pProfilerNotify));
} // CPFH_UnwindFrames1()

//******************************************************************************
inline EXCEPTION_DISPOSITION __cdecl
CPFH_UnwindHandler(EXCEPTION_RECORD *pExceptionRecord,
                   EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
                   CONTEXT *pContext,
                   void *pDispatcherContext)
{
    WRAPPER_CONTRACT;
    _ASSERTE (pExceptionRecord->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND));

        if (pEstablisherFrame->dwFlags & PAL_EXCEPTION_FLAGS_UnwindCallback)
        {
            pEstablisherFrame->dwFlags &= ~PAL_EXCEPTION_FLAGS_UnwindCallback;
            ((FrameHandlerExRecord *)pEstablisherFrame)->m_tct.pBottomFrame = NULL;
            return COMPlusAfterUnwind(pExceptionRecord, pEstablisherFrame,
                ((FrameHandlerExRecord *)pEstablisherFrame)->m_tct);
        }

    #ifdef _DEBUG
    // Note: you might be inclined to write "static int breakOnSecondPass = REGUTIL::GetConfigDWORD(...);", but
    // you can't do that here. That causes C++ EH to be generated under the covers for this function, and this
    // function isn't allowed to have any C++ EH in it because its never going to return.
    static int breakOnSecondPass; // = 0
    static BOOL breakOnSecondPassSetup; // = FALSE
    if (!breakOnSecondPassSetup)
    {
        breakOnSecondPass = REGUTIL::GetConfigDWORD(L"BreakOnSecondPass", 0);
        breakOnSecondPassSetup = TRUE;
    }
    if (breakOnSecondPass != 0)
    {
        _ASSERTE(!"Unwind handler");
    }
    #endif

    DWORD exceptionCode = pExceptionRecord->ExceptionCode;
    Thread *pThread = GetThread();

    ExInfo* pExInfo = &(pThread->GetExceptionState()->m_currentExInfo);

    STRESS_LOG4(LF_EH, LL_INFO100, "In CPFH_UnwindHandler EHCode = %x EIP = %x with ESP = %x, pEstablisherFrame = 0x%p\n", exceptionCode,
        pContext ? GetIP(pContext) : 0, pContext ? GetSP(pContext) : 0, pEstablisherFrame);

    // We always want to be in co-operative mode when we run this function.  Whenever we return
    // from it, want to go to pre-emptive mode because are returning to OS.
    BOOL disabled = pThread->PreemptiveGCDisabled();
    if (!disabled)
        pThread->DisablePreemptiveGC();

    CPFH_VerifyThreadIsInValidState(pThread, exceptionCode, pEstablisherFrame);

    BOOL fIsSoTolerant = !pThread->HasThreadStateNC(Thread::TSNC_SOIntolerant);
    pThread->ResetThreadStateNC(Thread::TSNC_SOIntolerant);

    // this establishes a marker so can determine if are processing a nested exception
    // don't want to use the current frame to limit search as it could have been unwound by
    // the time get to nested handler (ie if find an exception, unwind to the call point and
    // then resume in the catch and then get another exception) so make the nested handler
    // have the same boundary as this one. If nested handler can't find a handler, we won't
    // end up searching this frame list twice because the nested handler will set the search
    // boundary in the thread and so if get back to this handler it will have a range that starts
    // and ends at the same place.
    NestedHandlerExRecord nestedHandlerExRecord;
    nestedHandlerExRecord.Init(0, (PEXCEPTION_ROUTINE)COMPlusNestedExceptionHandler, GetCurrFrame(pEstablisherFrame));
    nestedHandlerExRecord.m_ActiveForUnwind = TRUE;
    INSTALL_EXCEPTION_HANDLING_RECORD(&(nestedHandlerExRecord.m_ExReg));

    // Unwind the stack.  The establisher frame sets the boundary.
    CPFH_UnwindFrames1(pThread, pEstablisherFrame, exceptionCode);

    // We're unwinding -- the bottom most handler is potentially off top-of-stack now.  If
    // it is, change it to the next COM+ frame.  (This one is not good, as it's about to
    // disappear.)
    if (pExInfo->m_pBottomMostHandler &&
        pExInfo->m_pBottomMostHandler <= pEstablisherFrame) 
    {
        EXCEPTION_REGISTRATION_RECORD *pNextBottomMost = GetNextCOMPlusSEHRecord(pEstablisherFrame);

        // If there is no previous COM+ SEH handler, GetNextCOMPlusSEHRecord() will return -1.  Much later, we will dereference that and AV.
        _ASSERTE (pNextBottomMost != EXCEPTION_CHAIN_END);
        
        STRESS_LOG2(LF_EH, LL_INFO100, "CPFH_UnwindHandler: setting m_pBottomMostHandler from 0x%p to 0x%p\n", 
            pExInfo->m_pBottomMostHandler, pNextBottomMost);
        
        pExInfo->m_pBottomMostHandler = pNextBottomMost;
    }

    pThread->EnablePreemptiveGC();
    UNINSTALL_EXCEPTION_HANDLING_RECORD(&(nestedHandlerExRecord.m_ExReg));

    if (!fIsSoTolerant)
    {
        pThread->SetThreadStateNC(Thread::TSNC_SOIntolerant);
    }
    else
    {
        pThread->ResetThreadStateNC(Thread::TSNC_SOIntolerant);
    }

    if (pThread->IsAbortRequested() &&
        GetNextCOMPlusSEHRecord(pEstablisherFrame) == EXCEPTION_CHAIN_END) 
    {
        // Topmost handler and abort requested.
        pThread->EEResetAbort(Thread::TAR_Thread);
        LOG((LF_EH, LL_INFO100, "CPFH_UnwindHandler: topmost handler resets abort.\n"));
    }

    STRESS_LOG0(LF_EH, LL_INFO100, "CPFH_UnwindHandler: Leaving with ExceptionContinueSearch\n");
    return ExceptionContinueSearch;
} // CPFH_UnwindHandler()

//******************************************************************************
// This is the first handler that is called iin the context of managed code
// It is the first level of defense and tries to find a handler in the user
// code to handle the exception
//-------------------------------------------------------------------------
// EXCEPTION_DISPOSITION __cdecl COMPlusFrameHandler(
//     EXCEPTION_RECORD *pExceptionRecord,
//     _EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
//     CONTEXT *pContext,
//     DISPATCHER_CONTEXT *pDispatcherContext)
EXCEPTION_HANDLER_IMPL(COMPlusFrameHandler)
{
    WRAPPER_CONTRACT;
    _ASSERTE(!DebugIsEECxxException(pExceptionRecord) && "EE C++ Exception leaked into managed code!");

    STRESS_LOG4(LF_EH, LL_INFO100, "In COMPlusFrameHander EH code = %x  EIP = %x with ESP = %x, pEstablisherFrame = 0x%p\n",
        pExceptionRecord->ExceptionCode, GetIP(pContext), GetSP(pContext), pEstablisherFrame);

    _ASSERTE((pContext == NULL) || ((pContext->ContextFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL));
    
    if (g_fNoExceptions)
        return ExceptionContinueSearch; // No EH during EE shutdown.

    EXCEPTION_DISPOSITION retVal = ExceptionContinueSearch;

    Thread *pThread = GetThread();
    if ((pExceptionRecord->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND)) == 0)
    {
        if (STATUS_STACK_OVERFLOW == pExceptionRecord->ExceptionCode ||
            EXCEPTION_SOFTSO == pExceptionRecord->ExceptionCode)
        {
            pThread->SetSOExceptionInfo(pExceptionRecord, pContext);
            EEPolicy::HandleStackOverflow(SOD_ManagedFrameHandler);
            if (!pThread->PreemptiveGCDisabled())
            {
                pThread->DisablePreemptiveGC();
            }
            if (pThread->GetFrame() < GetCurrFrame(pEstablisherFrame))
            {
                // We are very short of stack.  We avoid calling UnwindFrame which may
                // run unknown code here.
                pThread->SetFrame(GetCurrFrame(pEstablisherFrame));
            }
            return ExceptionContinueSearch;
        }
        else
        {
            pThread->SetSOExceptionInfo(NULL, NULL);
#ifndef TOTALLY_DISBLE_STACK_GUARDS
            if (GetEEPolicy()->GetActionOnFailure(FAIL_StackOverflow) == eRudeUnloadAppDomain)
            {
                pThread->ResetThreadStateNC(Thread::TSNC_SOIntolerant);
                RetailStackProbe(ADJUST_PROBE(BACKOUT_CODE_STACK_LIMIT), pThread);
            }
#endif
        }
    }
    else
    {
        if (pThread->HasThreadStateNC(Thread::TSNC_SOWorkNeeded))
        {
            return ExceptionContinueSearch;
        }
    }

    BEGIN_CONTRACT_VIOLATION(SOToleranceViolation);

    if (pExceptionRecord->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND))
    {
        retVal =  CPFH_UnwindHandler(pExceptionRecord,
                                     pEstablisherFrame,
                                     pContext,
                                     pDispatcherContext);
    }
    else
    {

        /* Make no assumptions about the current machine state.
                                                                                                */
        ResetCurrentContext();

        // clear the second pass flags to handle nested exceptions
        pEstablisherFrame->dwFlags &= ~PAL_EXCEPTION_FLAGS_All;

        retVal = CPFH_FirstPassHandler(pExceptionRecord,
                                       pEstablisherFrame,
                                       pContext,
                                       pDispatcherContext);

    }

    END_CONTRACT_VIOLATION;

    return retVal;
} // COMPlusFrameHandler()


//-------------------------------------------------------------------------
// This is called by the EE to restore the stack pointer if necessary.
//-------------------------------------------------------------------------

LPVOID __stdcall COMPlusEndCatch( Thread *pThread, CONTEXT *pCtx, void *pSEH)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    LOG((LF_EH, LL_INFO1000, "COMPlusPEndCatch:called with "
        "pThread:0x%x\n",pThread));

    if (NULL == pThread )
    {
        _ASSERTE( pCtx == NULL );
        pThread = GetThread();
    }
    else
    {
        _ASSERTE( pCtx != NULL);
        _ASSERTE( pSEH != NULL);
    }

    // indicate that we are out of the managed clause as early as possible
    ExInfo* pExInfo = &(pThread->GetExceptionState()->m_currentExInfo);
    pExInfo->m_EHClauseInfo.SetManagedCodeEntered(FALSE);

    void* esp = NULL;

    BEGIN_CONTRACT_VIOLATION(SOToleranceViolation);

    // Notify the profiler that the catcher has finished running
    EEToProfilerExceptionInterfaceWrapper::ExceptionCatcherLeave(pThread);

    // no need to set pExInfo->m_ClauseType = (DWORD)COR_PRF_CLAUSE_NONE now that the
    // notification is done because because the ExInfo record is about to be popped off anyway

    LOG((LF_EH, LL_INFO1000, "COMPlusPEndCatch:pThread:0x%x\n",pThread));
    
#ifdef _DEBUG
    gLastResumedExceptionFunc = NULL;
    gLastResumedExceptionHandler = 0;
#endif
    // Set the thrown object to NULL as no longer needed. This also sets the last thrown object to NULL.
    pThread->SafeSetThrowables(NULL);

    // reset the stashed exception info

    if  (pExInfo->m_pShadowSP)
    {
        *pExInfo->m_pShadowSP = 0;  // Reset the shadow SP
    }

    // pExInfo->m_dEsp was set in ResumeAtJITEH(). It is the Esp of the
    // handler nesting level which catches the exception.
    esp = (void*)(size_t)pExInfo->m_dEsp;

    pExInfo->UnwindExInfo(esp);
    
    // This will set the last thrown to be either null if we have handled all the exceptions in the nested chain or
    // to whatever the current exception is.
    //
    // In a case when we're nested inside another catch block, the domain in which we're executing may not be the
    // same as the one the domain of the throwable that was just made the current throwable above. Therefore, we
    // make a special effort to preserve the domain of the throwable as we update the the last thrown object.
    pThread->SafeUpdateLastThrownObject();

    // We are going to resume at a handler nesting level whose esp is dEsp. Pop off any SEH records below it. This
    // would be the COMPlusNestedExceptionHandler we had inserted.
    if (pCtx == NULL)
    {
        PopNestedExceptionRecords(esp);
    }
    else
    {
        _ASSERTE( pSEH != NULL);

        PopNestedExceptionRecords(esp, pCtx, pSEH);
    }

    LOG((LF_EH, LL_INFO1000, "COMPlusPEndCatch: esp=%p\n", esp));
   
    END_CONTRACT_VIOLATION;

    return esp;
}

#if defined(_MSC_VER)
#pragma warning (disable : 4035)
#endif
PEXCEPTION_REGISTRATION_RECORD GetCurrentSEHRecord()
{
    WRAPPER_CONTRACT;
    return PAL_GetBottommostRegistration();
}
#if defined(_MSC_VER)
#pragma warning (default : 4035)
#endif

PEXCEPTION_REGISTRATION_RECORD GetFirstCOMPlusSEHRecord(Thread *pThread) {
    WRAPPER_CONTRACT;
    EXCEPTION_REGISTRATION_RECORD *pEHR = *(pThread->GetExceptionListPtr());
    if (pEHR == EXCEPTION_CHAIN_END || IsUnmanagedToManagedSEHHandler(pEHR)) {
        return pEHR;
    } else {
        return GetNextCOMPlusSEHRecord(pEHR);
    }
}


PEXCEPTION_REGISTRATION_RECORD GetPrevSEHRecord(EXCEPTION_REGISTRATION_RECORD *next)
{
    WRAPPER_CONTRACT;
    _ASSERTE(IsUnmanagedToManagedSEHHandler(next));

    EXCEPTION_REGISTRATION_RECORD *pEHR = GetCurrentSEHRecord();
    _ASSERTE(pEHR != 0 && pEHR != EXCEPTION_CHAIN_END);

    EXCEPTION_REGISTRATION_RECORD *pBest = 0;
    while (pEHR != next) {
        if (IsUnmanagedToManagedSEHHandler(pEHR))
            pBest = pEHR;
        pEHR = pEHR->Next;
        _ASSERTE(pEHR != 0 && pEHR != EXCEPTION_CHAIN_END);
    }

    return pBest;
}

VOID SetCurrentSEHRecord(EXCEPTION_REGISTRATION_RECORD *pSEH)
{
    WRAPPER_CONTRACT;
    PAL_SetBottommostRegistration(pSEH);
}

//
// Pop SEH records below the given target ESP. This is only used to pop nested exception records.
//
VOID PopNestedExceptionRecords(LPVOID pTargetSP)
{
    // No CONTRACT here, because we can't run the risk of it pushing any SEH into the current method.
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    PEXCEPTION_REGISTRATION_RECORD pEHR = GetCurrentSEHRecord();

    while ((LPVOID)pEHR < pTargetSP)
    {

        pEHR = pEHR->Next;
    }

    SetCurrentSEHRecord(pEHR);
}

//
// This is implemented differently from the PopNestedExceptionRecords above because it's called in the context of
// the DebuggerRCThread to operate on the stack of another thread.
//
VOID PopNestedExceptionRecords(LPVOID pTargetSP, CONTEXT *pCtx, void *pSEH)
{
    // No CONTRACT here, because we can't run the risk of it pushing any SEH into the current method.
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

#ifdef _DEBUG
    LOG((LF_CORDB,LL_INFO1000, "\nPrintSEHRecords:\n"));

    EXCEPTION_REGISTRATION_RECORD *pEHR = (EXCEPTION_REGISTRATION_RECORD *)(size_t)*(DWORD *)pSEH;

    // check that all the eh frames are all greater than the current stack value. If not, the
    // stack has been updated somehow w/o unwinding the SEH chain.
    while (pEHR != NULL && pEHR != EXCEPTION_CHAIN_END)
    {
        LOG((LF_EH, LL_INFO1000000, "\t%08x: next:%08x handler:%x\n", pEHR, pEHR->Next, pEHR->Handler));
        pEHR = pEHR->Next;
    }
#endif

    DWORD dwCur = *(DWORD*)pSEH; // 'EAX' in the original routine
    DWORD dwPrev = (DWORD)(size_t)pSEH;

    while (dwCur < (DWORD)(size_t)pTargetSP)
    {
        // Watch for the OS handler
        // for nested exceptions, or any C++ handlers for destructors in our call
        // stack, or anything else.
        if (dwCur < (DWORD)(size_t)GetSP(pCtx))
            dwPrev = dwCur;

        dwCur = *(DWORD *)(size_t)dwCur;

        LOG((LF_CORDB,LL_INFO10000, "dwCur: 0x%x dwPrev:0x%x pTargetSP:0x%x\n",
            dwCur, dwPrev, pTargetSP));
    }

    *(DWORD *)(size_t)dwPrev = dwCur;

#ifdef _DEBUG
    pEHR = (EXCEPTION_REGISTRATION_RECORD *)(size_t)*(DWORD *)pSEH;
    // check that all the eh frames are all greater than the current stack value. If not, the
    // stack has been updated somehow w/o unwinding the SEH chain.

    LOG((LF_CORDB,LL_INFO1000, "\nPopSEHRecords:\n"));
    while (pEHR != NULL && pEHR != (void *)-1)
    {
        LOG((LF_EH, LL_INFO1000000, "\t%08x: next:%08x handler:%x\n", pEHR, pEHR->Next, pEHR->Handler));
        pEHR = pEHR->Next;
    }
#endif
}

//
// Remove a SEH record from somewhere on the FS:0 chain. It may be the head, or it may be somewhere in the
// chain. Only use this when you really, really know that you want to possibly unlink an entry from the middle of
// the chain.
//
VOID RemoveSEHRecordOutOfOrder(EXCEPTION_REGISTRATION_RECORD *pEHR)
{
    // Can't have a pointer to FS:[0], so go ahead and make a temporary head pointer here.
    EXCEPTION_REGISTRATION_RECORD *pHead = GetCurrentSEHRecord();
    EXCEPTION_REGISTRATION_RECORD **pTmp = &pHead;

    // Walk the list and find the entry we want to remove.
    while ((*pTmp != EXCEPTION_CHAIN_END) && (*pTmp != pEHR))
    {
        pTmp = &((*pTmp)->Next);
    }

    // If we found the entry we want to remove, then remove it.
    if (*pTmp != EXCEPTION_CHAIN_END)
    {
        *pTmp = (*pTmp)->Next;
        pEHR->Next = NULL;  // Null the next pointer of the dead entry to help find bugs more quickly.

        // Special case for the head. Since we can't have a pointer to FS:[0], we have to have this special case.
        if (pTmp == &pHead)
        {
            SetCurrentSEHRecord(pHead);
        }
    }
}

//==========================================================================
// COMPlusThrowCallback
//
//==========================================================================

/*
 *
 * COMPlusThrowCallbackHelper
 *
 * This function is a simple helper function for COMPlusThrowCallback.  It is needed
 * because of the EX_TRY macro.  This macro does an alloca(), which allocates space
 * off the stack, not free'ing it.  Thus, doing a EX_TRY in a loop can easily result
 * in a stack overflow error.  By factoring out the EX_TRY into a separate function,
 * we recover that stack space.
 *
 * Parameters:
 *   pJitManager - The JIT manager that will filter the EH.
 *   pCf - The frame to crawl.
 *   EHClausePtr
 *   nestingLevel
 *   pThread - Used to determine if the thread is throwable or not.
 *
 * Return:
 *   Exception status.
 *
 */
int COMPlusThrowCallbackHelper(IJitManager *pJitManager,
                               CrawlFrame *pCf,
                               ThrowCallbackType* pData,
                               EE_ILEXCEPTION_CLAUSE  *EHClausePtr,
                               DWORD nestingLevel,
                               OBJECTREF throwable
                              )
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    int iFilt = 0;

    EX_TRY
    {
        
        // We want to call filters even if the thread is aborting, so suppress abort
        // checks while the filter runs.
        ThreadPreventAbortHolder preventAbort(TRUE);

        iFilt = pJitManager->CallJitEHFilter(pCf,
                                             EHClausePtr,
                                             nestingLevel,
                                             throwable);
    }
    EX_CATCH
    {
        //
        // Swallow exception.  Treat as exception continue search.
        //
        iFilt = EXCEPTION_CONTINUE_SEARCH;

    }
    EX_END_CATCH(SwallowAllExceptions)

    return iFilt;
}

//******************************************************************************
// The stack walk callback for exception handling on x86.
// Returns one of:
//    SWA_CONTINUE    = 0,    // continue walking
//    SWA_ABORT       = 1,    // stop walking, early out in "failure case"
//    SWA_FAILED      = 2     // couldn't walk stack
StackWalkAction COMPlusThrowCallback(       // SWA value
    CrawlFrame  *pCf,                       // Data from StackWalkFramesEx
    ThrowCallbackType *pData)               // Context data passed through from CPFH
{
    // We don't want to use a runtime contract here since this codepath is used during
    // the processing of a hard SO. Contracts use a significant amount of stack
    // which we can't afford for those cases.
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    Frame *pFrame = pCf->GetFrame();
    MethodDesc *pFunc = pCf->GetFunction();

    #if defined(_DEBUG)
    #define METHODNAME(pFunc) (pFunc?pFunc->m_pszDebugMethodName:"<n/a>")
    #else
    #define METHODNAME(pFunc) "<n/a>"
    #endif
    STRESS_LOG4(LF_EH, LL_INFO100, "COMPlusThrowCallback: STACKCRAWL method:%pM ('%s'), Frame:%p, FrameVtable = %pV\n",
        pFunc, METHODNAME(pFunc), pFrame, pCf->IsFrameless()?0:(*(void**)pFrame));
    #undef METHODNAME

    Thread *pThread = GetThread();

    if (pFrame && pData->pTopFrame == pFrame)
        /* Don't look past limiting frame if there is one */
        return SWA_ABORT;

    if (!pFunc)
        return SWA_CONTINUE;

    if (pThread->IsRudeAbortInitiated() && !pThread->IsWithinCer(pCf))
    {
        return SWA_CONTINUE;
    }

    ExInfo* pExInfo = &(pThread->GetExceptionState()->m_currentExInfo);

    _ASSERTE(!pData->bIsUnwind);
#ifdef _DEBUG
    if (!pExInfo->m_pPrevNestedInfo) {
        if (pData->pCurrentExceptionRecord) {
            if (pFrame) _ASSERTE(pData->pCurrentExceptionRecord > pFrame);
            if (pCf->IsFrameless()) _ASSERTE((PVOID)pData->pCurrentExceptionRecord >= GetRegdisplaySP(pCf->GetRegisterSet()));
        }
        if (pData->pPrevExceptionRecord) {
            // FCALLS have an extra SEH record in debug because of the desctructor
            // associated with ForbidGC checking.  This is benign, so just ignore it.
            if (pFrame) _ASSERTE(pData->pPrevExceptionRecord < pFrame || pFrame->GetVTablePtr() == HelperMethodFrame::GetMethodFrameVPtr());
            if (pCf->IsFrameless()) _ASSERTE((PVOID)pData->pPrevExceptionRecord <= GetRegdisplaySP(pCf->GetRegisterSet()));
        }
    }
#endif

    UINT_PTR currentIP = 0;
    UINT_PTR currentSP = 0;

    if (pCf->IsFrameless())
    {
        currentIP = (UINT_PTR)*(pCf->GetRegisterSet()->pPC);
        currentSP = (UINT_PTR)GetRegdisplaySP(pCf->GetRegisterSet());
    }
    else if (InlinedCallFrame::FrameHasActiveCall(pFrame))
    {
        // don't have the IP, SP for native code
        currentIP = 0;
        currentSP = 0;
    }
    else
    {
        currentIP = (UINT_PTR)(pCf->GetFrame()->GetIP());
        currentSP = 0; //Don't have an SP to get.
    }

    // Append the current frame to the stack trace and save the save trace to the managed Exception object.
    pExInfo->m_StackTraceInfo.AppendElement(pData->bAllowAllocMem, currentIP, currentSP, pFunc, pCf);
    pExInfo->m_StackTraceInfo.SaveStackTrace(pData->bAllowAllocMem,
                                             pThread->GetThrowableAsHandle(),
                                             pData->bReplaceStack,
                                             pData->bSkipLastElement);

    // Reset the flags.  These flags are set only once before each stack walk done by LookForHandler(), and
    // they apply only to the first frame we append to the stack trace.  Subsequent frames are always appended.
    if (pData->bReplaceStack)
    {
        pData->bReplaceStack = FALSE;
    }
    if (pData->bSkipLastElement)
    {
        pData->bSkipLastElement = FALSE;
    }
    // now we've got the stack trace, if we aren't allowed to catch this and we're first pass, return
    if (pData->bDontCatch)
        return SWA_CONTINUE;

    if (!pCf->IsFrameless())
    {
        extern bool g_EnableSIS;
        if (g_EnableSIS)
        {
            // For debugger, we may want to notify 1st chance exceptions if they're coming out of a stub.
            // We recognize stubs as Frames with a M2U transition type. The debugger's stackwalker also
            // recognizes these frames and publishes ICorDebugInternalFrames in the stackwalk. It's
            // important to use pFrame as the stack address so that the Exception callback matches up
            // w/ the ICorDebugInternlFrame stack range.
            if (CORDebuggerAttached())
            {
                Frame * pFrameStub = pCf->GetFrame();
                Frame::ETransitionType t = pFrameStub->GetTransitionType();
                if (t == Frame::TT_M2U)
                {
                    // Use address of the frame as the stack address.
                    currentSP = (SIZE_T) ((void*) pFrameStub);
                    currentIP = 0; // no IP.
                    EEToDebuggerExceptionInterfaceWrapper::FirstChanceManagedException(pThread, (SIZE_T)currentIP, (SIZE_T)currentSP);
                }
            }
        }
        return SWA_CONTINUE;
    }

    // Let the profiler know that we are searching for a handler within this function instance
    EEToProfilerExceptionInterfaceWrapper::ExceptionSearchFunctionEnter(pThread, pFunc);

#ifdef DEBUGGING_SUPPORTED
    //
    // Go ahead and notify any debugger of this exception.
    //
    EEToDebuggerExceptionInterfaceWrapper::FirstChanceManagedException(pThread, (SIZE_T)currentIP, (SIZE_T)currentSP);

    if (CORDebuggerAttached() && pExInfo->m_ExceptionFlags.DebuggerInterceptInfo())
    {
        return SWA_ABORT;
    }
#endif // DEBUGGING_SUPPORTED

    IJitManager* pJitManager = pCf->GetJitManager();
    _ASSERTE(pJitManager);

    EH_CLAUSE_ENUMERATOR pEnumState;
    unsigned EHCount = pJitManager->InitializeEHEnumeration(pCf->GetMethodToken(), &pEnumState);
    if (EHCount == 0)
    {
        // Inform the profiler that we're leaving, and what pass we're on
        EEToProfilerExceptionInterfaceWrapper::ExceptionSearchFunctionLeave(pThread);
        return SWA_CONTINUE;
    }

    TypeHandle thrownType = TypeHandle();
    // if we are being called on an unwind for an exception that we did not try to catch, eg.
    // an internal EE exception, then pThread->GetThrowable will be null
    {
        OBJECTREF  throwable = pThread->GetThrowable();
        if (throwable != NULL)
        {
            throwable = PossiblyUnwrapThrowable(throwable, pCf->GetAssembly());
            thrownType = TypeHandle(throwable->GetTrueMethodTable());
        }
    }

    PREGDISPLAY regs = pCf->GetRegisterSet();
    BYTE *pStack = (BYTE *) GetRegdisplaySP(regs);
#ifdef DEBUGGING_SUPPORTED
    BYTE *pHandlerEBP   = (BYTE *) GetRegdisplayFP(regs);
#endif

    DWORD offs = (DWORD)pCf->GetRelOffset();  //= (BYTE*) (*regs->pPC) - (BYTE*) pCf->GetStartAddress();
    STRESS_LOG1(LF_EH, LL_INFO10000, "COMPlusThrowCallback: offset is %d\n", offs);

    EE_ILEXCEPTION_CLAUSE EHClause, *EHClausePtr;
    unsigned start_adjust, end_adjust;

    start_adjust = !(pCf->HasFaulted() || pCf->IsIPadjusted());
    end_adjust = pCf->IsActiveFunc();

    for(ULONG i=0; i < EHCount; i++)
    {
         EHClausePtr = pJitManager->GetNextEHClause(pCf->GetMethodToken(),&pEnumState, &EHClause);
         _ASSERTE(IsValidClause(EHClausePtr));

        STRESS_LOG4(LF_EH, LL_INFO100, "COMPlusThrowCallback: considering '%s' clause [%d,%d], ofs:%d\n",
                (IsFault(EHClausePtr) ? "fault" : (
                 IsFinally(EHClausePtr) ? "finally" : (
                 IsFilterHandler(EHClausePtr) ? "filter" : (
                 IsTypedHandler(EHClausePtr) ? "typed" : "unknown")))),
                EHClausePtr->TryStartPC,
                EHClausePtr->TryEndPC,
                offs
                );

        // Checking the exception range is a bit tricky because
        // on CPU faults (null pointer access, div 0, ..., the IP points
        // to the faulting instruction, but on calls, the IP points
        // to the next instruction.
        // This means that we should not include the start point on calls
        // as this would be a call just preceding the try block.
        // Also, we should include the end point on calls, but not faults.

        // If we're in the FILTER part of a filter clause, then we
        // want to stop crawling.  It's going to be caught in a
        // EX_CATCH just above us.  If not, the exception
        if (   IsFilterHandler(EHClausePtr)
            && (   offs > EHClausePtr->FilterOffset
                || offs == EHClausePtr->FilterOffset && !start_adjust)
            && (   offs < EHClausePtr->HandlerStartPC
                || offs == EHClausePtr->HandlerStartPC && !end_adjust)) {

            STRESS_LOG4(LF_EH, LL_INFO100, "COMPlusThrowCallback: Fault inside filter [%d,%d] startAdj %d endAdj %d\n",
                        EHClausePtr->FilterOffset, EHClausePtr->HandlerStartPC, start_adjust, end_adjust);
            
            EEToProfilerExceptionInterfaceWrapper::ExceptionSearchFunctionLeave(pThread);
            return SWA_ABORT;
        }

        if ( (offs < EHClausePtr->TryStartPC) ||
             (offs > EHClausePtr->TryEndPC) ||
             (offs == EHClausePtr->TryStartPC && start_adjust) ||
             (offs == EHClausePtr->TryEndPC && end_adjust))
            continue;

        BOOL typeMatch = FALSE;
        //BOOL isFaultOrFinally = IsFaultOrFinally(EHClausePtr);
        BOOL isTypedHandler = IsTypedHandler(EHClausePtr);
        //BOOL hasCachedTypeHandle = HasCachedTypeHandle(EHClausePtr);

        if (isTypedHandler && !thrownType.IsNull())
        {
            if (EHClausePtr->TypeHandle == (void*)(size_t)mdTypeRefNil)
            {
                // this is a catch(...)
                typeMatch = TRUE;
            }
            else
            {
                TypeHandle exnType;

                if (!HasCachedTypeHandle(EHClausePtr))
                {
                    exnType = pJitManager->ResolveEHClause(pCf->GetMethodToken(),&pEnumState,EHClausePtr,pCf);
                }
                else
                {
                    exnType = TypeHandle::FromPtr(EHClausePtr->TypeHandle);

                    // We never cache a NULL type handle, so we'd better not have a NULL handler here.
                    _ASSERTE(!exnType.IsNull());
                }

                // if doesn't have cached class then class wasn't loaded so couldn't have been thrown
                typeMatch = !exnType.IsNull() && ExceptionIsOfRightType(exnType, thrownType);
            }
        }


        // Determine the nesting level of EHClause. Just walk the table
        // again, and find out how many handlers enclose it
        DWORD nestingLevel = 0;

        if (IsFaultOrFinally(EHClausePtr))
            continue;
        if (isTypedHandler)
        {
            LOG((LF_EH, LL_INFO100, "COMPlusThrowCallback: %s match for typed handler.\n", typeMatch?"Found":"Did not find"));
            if (!typeMatch)
            {
                continue;
            }
        }
        else
        {

            // Must be an exception filter (__except() part of __try{}__except(){}).
            _ASSERTE(EHClausePtr->HandlerEndPC != (DWORD) -1);
            nestingLevel = ComputeEnclosingHandlerNestingLevel_DEPRECATED(pJitManager,
                                                                          pCf->GetMethodToken(),
                                                                          EHClausePtr->HandlerStartPC);

            // The call above doesn't determine the nesting correctly for the following cases
            // PC offset
            // 000001: Start A / Start B
            // 000002: End B
            // 000003: End A
            //
            // 000001: Start A
            // 000002: Start B
            // 000003: End A / End B
            // Checking for nesting at of the end of the handler fixes that. However the invariant is
            // either starting or ending address must be different. So the following is illegal:
            // 000001: Start A / Start B
            // 000003: End A / End B

            DWORD nestingLevelEnd =  ComputeEnclosingHandlerNestingLevel_DEPRECATED( pJitManager,
                                                                                     pCf->GetMethodToken(),
                                                                                     EHClausePtr->HandlerEndPC);

           nestingLevel = nestingLevel > nestingLevelEnd ? nestingLevel : nestingLevelEnd;
            // We just need *any* address within the method. This will let the debugger
            // resolve the EnC version of the method.
            TADDR pMethodAddr = taGetControlPC(regs);            
            EEToDebuggerExceptionInterfaceWrapper::ExceptionFilter(pFunc, pMethodAddr, EHClausePtr->FilterOffset, pHandlerEBP);

            UINT_PTR uStartAddress = (UINT_PTR)pJitManager-> JitTokenToStartAddress(pCf->GetMethodToken());

            // save clause information in the exinfo
            pExInfo->m_EHClauseInfo.SetInfo(COR_PRF_CLAUSE_FILTER, 
                                            uStartAddress + EHClausePtr->FilterOffset, 
                                            StackFrame((UINT_PTR)pHandlerEBP));
           
            // Let the profiler know we are entering a filter
            EEToProfilerExceptionInterfaceWrapper::ExceptionSearchFilterEnter(pThread, pFunc);

            COUNTER_ONLY(GetPrivatePerfCounters().m_Excep.cFiltersExecuted++);

            STRESS_LOG3(LF_EH, LL_INFO10, "COMPlusThrowCallback: calling filter code, EHClausePtr:%08x, Start:%08x, End:%08x\n",
                EHClausePtr, EHClausePtr->HandlerStartPC, EHClausePtr->HandlerEndPC);

            OBJECTREF throwable = PossiblyUnwrapThrowable(pThread->GetThrowable(), pCf->GetAssembly());

            pExInfo->m_EHClauseInfo.SetManagedCodeEntered(TRUE);

            int iFilt = COMPlusThrowCallbackHelper(pJitManager,
                                                   pCf,
                                                   pData,
                                                   EHClausePtr,
                                                   nestingLevel,
                                                   throwable);

            pExInfo->m_EHClauseInfo.SetManagedCodeEntered(FALSE);

            // Let the profiler know we are leaving a filter
            EEToProfilerExceptionInterfaceWrapper::ExceptionSearchFilterLeave(pThread);          
            pExInfo->m_EHClauseInfo.ResetInfo();

            if (pThread->IsRudeAbortInitiated() && !pThread->IsWithinCer(pCf))
            {
                EEToProfilerExceptionInterfaceWrapper::ExceptionSearchFunctionLeave(pThread);
                return SWA_CONTINUE;
            }

            // If this filter didn't want the exception, keep looking.
            if (EXCEPTION_EXECUTE_HANDLER != iFilt)
                continue;
        }


        // Record this location, to stop the unwind phase, later.
        pData->pFunc = pFunc;
        pData->dHandler = i;
        pData->pStack = pStack;

        // Notify the profiler that a catcher has been found
        EEToProfilerExceptionInterfaceWrapper::ExceptionSearchCatcherFound(pThread, pFunc);
        EEToProfilerExceptionInterfaceWrapper::ExceptionSearchFunctionLeave(pThread);

#ifdef DEBUGGING_SUPPORTED
        //
        // Notify debugger that a catcher has been found.
        //
        if (CORDebuggerAttached() && !pExInfo->m_ExceptionFlags.DebuggerInterceptInfo())
        {
            _ASSERTE(pData);
            // We just need *any* address within the method. This will let the debugger
            // resolve the EnC version of the method.
            TADDR pMethodAddr = taGetControlPC(regs);            

            EEToDebuggerExceptionInterfaceWrapper::FirstChanceManagedExceptionCatcherFound(pThread,
                                                                                           pData->pFunc, pMethodAddr,
                                                                                           (SIZE_T)pData->pStack,
                                                                                           EHClausePtr);
        }
#endif // DEBUGGING_SUPPORTED

        return SWA_ABORT;
    }
    EEToProfilerExceptionInterfaceWrapper::ExceptionSearchFunctionLeave(pThread);
    return SWA_CONTINUE;
} // StackWalkAction COMPlusThrowCallback()


//==========================================================================
// COMPlusUnwindCallback
//==========================================================================

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable : 4740) // There is inline asm code in this function, which disables
                                 // global optimizations.
#pragma warning (disable : 4731)
#endif
StackWalkAction COMPlusUnwindCallback (CrawlFrame *pCf, ThrowCallbackType *pData)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    _ASSERTE(pData->bIsUnwind);

    Frame *pFrame = pCf->GetFrame();
    MethodDesc *pFunc = pCf->GetFunction();

    #if defined(_DEBUG)
    #define METHODNAME(pFunc) (pFunc?pFunc->m_pszDebugMethodName:"<n/a>")
    #else
    #define METHODNAME(pFunc) "<n/a>"
    #endif
    STRESS_LOG4(LF_EH, LL_INFO100, "COMPlusUnwindCallback: STACKCRAWL method:%pM ('%s'), Frame:%p, FrameVtable = %pV\n",
        pFunc, METHODNAME(pFunc), pFrame, pCf->IsFrameless()?0:(*(void**)pFrame));
    #undef METHODNAME

    if (pFrame && pData->pTopFrame == pFrame)
        /* Don't look past limiting frame if there is one */
        return SWA_ABORT;

    if (!pFunc)
        return SWA_CONTINUE;

    if (!pCf->IsFrameless())
        return SWA_CONTINUE;

    Thread *pThread = GetThread();

    // If the thread is being RudeAbort, we will not run any finally
    if (pThread->IsRudeAbortInitiated() && !pThread->IsWithinCer(pCf))
    {
        return SWA_CONTINUE;
    }

    IJitManager* pJitManager = pCf->GetJitManager();
    _ASSERTE(pJitManager);

    ExInfo *pExInfo = &(pThread->GetExceptionState()->m_currentExInfo);

    PREGDISPLAY regs = pCf->GetRegisterSet();
    BYTE *pStack = (BYTE *) GetRegdisplaySP(regs);

    TypeHandle thrownType = TypeHandle();

#ifdef DEBUGGING_SUPPORTED
    LOG((LF_EH, LL_INFO1000, "COMPlusUnwindCallback: Intercept %d, pData->pFunc 0x%X, pFunc 0x%X, pData->pStack 0x%X, pStack 0x%X\n",
         pExInfo->m_ExceptionFlags.DebuggerInterceptInfo(),
         pData->pFunc,
         pFunc,
         pData->pStack,
         pStack));

    //
    // If the debugger wants to intercept this exception here, go do that.
    //
    if (pExInfo->m_ExceptionFlags.DebuggerInterceptInfo() && (pData->pFunc == pFunc) && (pData->pStack == pStack))
    {
        goto LDoDebuggerIntercept;
    }
#endif

    // Notify the profiler of the function we're dealing with in the unwind phase
    EEToProfilerExceptionInterfaceWrapper::ExceptionUnwindFunctionEnter(pThread, pFunc);

    EH_CLAUSE_ENUMERATOR pEnumState;
    unsigned EHCount;

    EHCount = pJitManager->InitializeEHEnumeration(pCf->GetMethodToken(), &pEnumState);

    if (EHCount == 0)
    {
        // Inform the profiler that we're leaving, and what pass we're on
        EEToProfilerExceptionInterfaceWrapper::ExceptionUnwindFunctionLeave(pThread);

        return SWA_CONTINUE;
    }

    // if we are being called on an unwind for an exception that we did not try to catch, eg.
    // an internal EE exception, then pThread->GetThrowable will be null
    {
        OBJECTREF  throwable = pThread->GetThrowable();
        if (throwable != NULL)
        {
            throwable = PossiblyUnwrapThrowable(throwable, pCf->GetAssembly());
            thrownType = TypeHandle(throwable->GetTrueMethodTable());
        }
    }
#ifdef DEBUGGING_SUPPORTED
    BYTE *pHandlerEBP;
    pHandlerEBP = (BYTE *) GetRegdisplayFP(regs);
#endif

    DWORD offs;
    offs = (DWORD)pCf->GetRelOffset();  //= (BYTE*) (*regs->pPC) - (BYTE*) pCf->GetStartAddress();

    LOG((LF_EH, LL_INFO100, "COMPlusUnwindCallback: current EIP offset in method 0x%x, \n", offs));

    EE_ILEXCEPTION_CLAUSE EHClause, *EHClausePtr;
    unsigned start_adjust, end_adjust;

    start_adjust = !(pCf->HasFaulted() || pCf->IsIPadjusted());
    end_adjust = pCf->IsActiveFunc();

    for(ULONG i=0; i < EHCount; i++)
    {
         EHClausePtr = pJitManager->GetNextEHClause(pCf->GetMethodToken(),&pEnumState, &EHClause);
         _ASSERTE(IsValidClause(EHClausePtr));

        STRESS_LOG4(LF_EH, LL_INFO100, "COMPlusUnwindCallback: considering '%s' clause [%d,%d], offs:%d\n",
                (IsFault(EHClausePtr) ? "fault" : (
                 IsFinally(EHClausePtr) ? "finally" : (
                 IsFilterHandler(EHClausePtr) ? "filter" : (
                 IsTypedHandler(EHClausePtr) ? "typed" : "unknown")))),
                EHClausePtr->TryStartPC,
                EHClausePtr->TryEndPC,
                offs
                );

        // Checking the exception range is a bit tricky because
        // on CPU faults (null pointer access, div 0, ..., the IP points
        // to the faulting instruction, but on calls, the IP points
        // to the next instruction.
        // This means that we should not include the start point on calls
        // as this would be a call just preceding the try block.
        // Also, we should include the end point on calls, but not faults.

        if (   IsFilterHandler(EHClausePtr)
            && (   offs > EHClausePtr->FilterOffset
                || offs == EHClausePtr->FilterOffset && !start_adjust)
            && (   offs < EHClausePtr->HandlerStartPC
                || offs == EHClausePtr->HandlerStartPC && !end_adjust)
            ) {
            STRESS_LOG4(LF_EH, LL_INFO100, "COMPlusUnwindCallback: Fault inside filter [%d,%d] startAdj %d endAdj %d\n",
                        EHClausePtr->FilterOffset, EHClausePtr->HandlerStartPC, start_adjust, end_adjust);

            // Make the filter as done. See comment in CallJitEHFilter
            // on why we have to do it here.
            Frame* pFilterFrame = pThread->GetFrame();
            _ASSERTE(pFilterFrame->GetVTablePtr() == ExceptionFilterFrame::GetFrameVtable());
            ((ExceptionFilterFrame*)pFilterFrame)->SetFilterDone();

            // Inform the profiler that we're leaving, and what pass we're on
            EEToProfilerExceptionInterfaceWrapper::ExceptionUnwindFunctionLeave(pThread);

            return SWA_ABORT;
        }

        if ( (offs <  EHClausePtr->TryStartPC) ||
             (offs > EHClausePtr->TryEndPC) ||
             (offs == EHClausePtr->TryStartPC && start_adjust) ||
             (offs == EHClausePtr->TryEndPC && end_adjust))
            continue;


        // Determine the nesting level of EHClause. Just walk the table
        // again, and find out how many handlers enclose it

        _ASSERTE(EHClausePtr->HandlerEndPC != (DWORD) -1);
        DWORD nestingLevel = ComputeEnclosingHandlerNestingLevel_DEPRECATED(pJitManager,
                                                                            pCf->GetMethodToken(),
                                                                            EHClausePtr->HandlerStartPC);

        // The call above doesn't determine the nesting correctly for the following cases
        // PC offset
        // 000001: Start A / Start B
        // 000002: End B
        // 000003: End A
        //
        // 000001: Start A
        // 000002: Start B
        // 000003: End A / End B
        // Checking for nesting at of the end of the handler fixes that. However the invariant is
        // either starting or ending address must be different. So the following is illegal:
        // 000001: Start A / Start B
        // 000003: End A / End B

        DWORD nestingLevelEnd =  ComputeEnclosingHandlerNestingLevel_DEPRECATED(pJitManager,
                                                                                pCf->GetMethodToken(),
                                                                                EHClausePtr->HandlerEndPC);

        nestingLevel = nestingLevel > nestingLevelEnd ? nestingLevel : nestingLevelEnd;


        // We just need *any* address within the method. This will let the debugger
        // resolve the EnC version of the method.
        TADDR pMethodAddr = taGetControlPC(regs);

        UINT_PTR uStartAddress = (UINT_PTR)pJitManager-> JitTokenToStartAddress(pCf->GetMethodToken());

        if (IsFaultOrFinally(EHClausePtr))
        {

            COUNTER_ONLY(GetPrivatePerfCounters().m_Excep.cFinallysExecuted++);

            EEToDebuggerExceptionInterfaceWrapper::ExceptionHandle(pFunc, pMethodAddr, EHClausePtr->HandlerStartPC, pHandlerEBP);

            pExInfo->m_EHClauseInfo.SetInfo(COR_PRF_CLAUSE_FINALLY,
                                            uStartAddress + EHClausePtr->HandlerStartPC,
                                            StackFrame((UINT_PTR)pHandlerEBP));
            
            // Notify the profiler that we are about to execute the finally code
            EEToProfilerExceptionInterfaceWrapper::ExceptionUnwindFinallyEnter(pThread, pFunc);

            LOG((LF_EH, LL_INFO100, "COMPlusUnwindCallback: finally clause [%d,%d] - call\n", EHClausePtr->TryStartPC, EHClausePtr->TryEndPC));

            pExInfo->m_EHClauseInfo.SetManagedCodeEntered(TRUE);
            pJitManager->CallJitEHFinally(pCf, EHClausePtr, nestingLevel);
            pExInfo->m_EHClauseInfo.SetManagedCodeEntered(FALSE);

            LOG((LF_EH, LL_INFO100, "COMPlusUnwindCallback: finally - returned\n"));

            // Notify the profiler that we are done with the finally code
            EEToProfilerExceptionInterfaceWrapper::ExceptionUnwindFinallyLeave(pThread);

            pExInfo->m_EHClauseInfo.ResetInfo();

            continue;
        }

        // Current is not a finally, check if it's the catching handler (or filter).
        if (pData->pFunc != pFunc || (ULONG)(pData->dHandler) != i || pData->pStack != pStack)
        {
            continue;
        }

#ifdef _DEBUG
        gLastResumedExceptionFunc = pCf->GetFunction();
        gLastResumedExceptionHandler = i;
#endif

        // save clause information in the exinfo
        pExInfo->m_EHClauseInfo.SetInfo(COR_PRF_CLAUSE_CATCH,
                                        uStartAddress  + EHClausePtr->HandlerStartPC,
                                        StackFrame((UINT_PTR)pHandlerEBP));

        // Notify the profiler that we are about to resume at the catcher.         
        EEToProfilerExceptionInterfaceWrapper::ExceptionCatcherEnter(pThread, pFunc);

        EEToDebuggerExceptionInterfaceWrapper::ExceptionHandle(pFunc, pMethodAddr, EHClausePtr->HandlerStartPC, pHandlerEBP);

        STRESS_LOG4(LF_EH, LL_INFO100, "COMPlusUnwindCallback: offset 0x%x matches clause [0x%x, 0x%x) matches in method %pM\n",
                    offs, EHClausePtr->TryStartPC, EHClausePtr->TryEndPC, pFunc);

        // ResumeAtJitEH will set pExInfo->m_EHClauseInfo.m_fManagedCodeEntered = TRUE; at the appropriate time
        pJitManager->ResumeAtJitEH(pCf,
            EHClausePtr, nestingLevel,
            pThread,
            pData->bUnwindStack);
        _ASSERTE(!"ResumeAtJitHander returned!");
        
        // we do not set pExInfo->m_EHClauseInfo.m_fManagedCodeEntered = FALSE here, 
        // that happens when the catch clause calls back to COMPlusEndCatch 
        
    }

    STRESS_LOG1(LF_EH, LL_INFO100, "COMPlusUnwindCallback: no handler found in method %pM\n", pFunc);
    EEToProfilerExceptionInterfaceWrapper::ExceptionUnwindFunctionLeave(pThread);

    return SWA_CONTINUE;


#ifdef DEBUGGING_SUPPORTED
LDoDebuggerIntercept:

    STRESS_LOG1(LF_EH|LF_CORDB, LL_INFO100, "COMPlusUnwindCallback: Intercepting in method %pM\n", pFunc);

    // ROTORTODO: DebuggerIntercept
    _ASSERTE(false);
    return SWA_CONTINUE;

#endif // DEBUGGING_SUPPORTED
} // StackWalkAction COMPlusUnwindCallback ()
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

void EHContext::Setup(LPVOID resumePC, PREGDISPLAY regs)
{
    LEAF_CONTRACT;

#if defined(_X86_)
    // EAX ECX EDX are scratch
    this->Esp  = regs->Esp;
    this->Ebx = *regs->pEbx;
    this->Esi = *regs->pEsi;
    this->Edi = *regs->pEdi;
    this->Ebp = *regs->pEbp;

    this->Eip = (ULONG)(size_t)resumePC;
#elif defined(_PPC_)
    int i;

    memset(&R[0], 0, 12 * sizeof(R[0]));
    this->R[12] = (ULONG)(size_t)resumePC;
    for (i = 0; i < NUM_CALLEESAVED_REGISTERS; i++) {
        this->R[i+13] = *regs->pR[i];
    }

    for (i = 0; i < NUM_FLOAT_CALLEESAVED_REGISTERS; i++) {
        this->F[i] = *regs->pF[i];
    }
    this->CR = regs->CR;
#else
    PORTABILITY_ASSERT("EHContext::Setup");
#endif
}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable : 4740) // There is inline asm code in this function, which disables
                                 // global optimizations.
#pragma warning (disable : 4731)
#endif
void ResumeAtJitEH(CrawlFrame* pCf,
                   BYTE* startPC,
                   EE_ILEXCEPTION_CLAUSE *EHClausePtr,
                   DWORD nestingLevel,
                   Thread *pThread,
                   BOOL unwindStack)
{
    // No dynamic contract here because this function doesn't return and destructors wouldn't be executed
    WRAPPER_CONTRACT;

    EHContext context;

    context.Setup(startPC + EHClausePtr->HandlerStartPC, pCf->GetRegisterSet());

    size_t * pShadowSP = NULL; // Write Esp to *pShadowSP before jumping to handler
    size_t * pHandlerEnd = NULL;

    OBJECTREF throwable = PossiblyUnwrapThrowable(pThread->GetThrowable(), pCf->GetAssembly());

    pCf->GetCodeManager()->FixContext(ICodeManager::CATCH_CONTEXT,
                                      &context,
                                      pCf->GetInfoBlock(),
                                      startPC,
                                      nestingLevel,
                                      throwable,
                                      pCf->GetCodeManState(),
                                      &pShadowSP,
                                      &pHandlerEnd);

    if (pHandlerEnd)
    {
        *pHandlerEnd = EHClausePtr->HandlerEndPC;
    }

    // save esp so that endcatch can restore it (it always restores, so want correct value)
    ExInfo* pExInfo = &(pThread->GetExceptionState()->m_currentExInfo);
    pExInfo->m_dEsp = context.GetSP();
    LOG((LF_EH, LL_INFO1000, "ResumeAtJitEH: current m_dEsp set to %p\n", context.GetSP()));

    NestedHandlerExRecord *pNestedHandlerExRecord;

    PVOID dEsp = GetCurrentSP();

    if (!unwindStack)
    {

        // so down below won't really update esp
        context.SetSP(dEsp);
        pExInfo->m_pShadowSP = pShadowSP; // so that endcatch can zero it back

        if  (pShadowSP)
        {
            *pShadowSP = (size_t)dEsp;
        }
    }
    else
    {
        // so shadow SP has the real SP as we are going to unwind the stack
        dEsp = context.GetSP();

        // BEGIN: pExInfo->UnwindExInfo(dEsp);
        ExInfo *pPrevNestedInfo = pExInfo->m_pPrevNestedInfo;

        while (pPrevNestedInfo && pPrevNestedInfo->m_StackAddress < dEsp)
        {
            LOG((LF_EH, LL_INFO1000, "ResumeAtJitEH: popping nested ExInfo at 0x%p\n", pPrevNestedInfo->m_StackAddress));

            pPrevNestedInfo->DestroyExceptionHandle();
            pPrevNestedInfo->m_StackTraceInfo.FreeStackTrace();

#ifdef DEBUGGING_SUPPORTED
            if (g_pDebugInterface != NULL)
            {
                g_pDebugInterface->DeleteInterceptContext(pPrevNestedInfo->m_DebuggerExState.GetDebuggerInterceptContext());
            }
#endif // DEBUGGING_SUPPORTED

            pPrevNestedInfo = pPrevNestedInfo->m_pPrevNestedInfo;
        }

        pExInfo->m_pPrevNestedInfo = pPrevNestedInfo;

        _ASSERTE(pExInfo->m_pPrevNestedInfo == 0 || pExInfo->m_pPrevNestedInfo->m_StackAddress >= dEsp);

        // Before we unwind the SEH records, get the Frame from the top-most nested exception record.
        Frame* pNestedFrame = GetCurrFrame(FindNestedEstablisherFrame(GetCurrentSEHRecord()));

        PopNestedExceptionRecords((LPVOID)(size_t)dEsp);

        EXCEPTION_REGISTRATION_RECORD* pNewBottomMostHandler = GetCurrentSEHRecord();

        pExInfo->m_pShadowSP = pShadowSP;


#if defined(PRESERVE_WATSON_ACROSS_CONTEXTS) // Watson isn't implemented on Rotor or DAC
        // Since we're here, this exception is not going unhandled.
        pThread->GetExceptionState()->ClearUnhandledInfoForWatson();
#endif // rotor or dac

        // We're going to put one nested record back on the stack before we resume.  This is
        // where it goes.
        pNestedHandlerExRecord = (NestedHandlerExRecord*)((BYTE*)dEsp - ALIGN_UP(sizeof(NestedHandlerExRecord), STACK_ALIGN_SIZE));

        // The point of no return.  The next statement starts scribbling on the stack.  It's
        // deep enough that we won't hit our own locals.  (That's important, 'cuz we're still
        // using them.)
        //
        _ASSERTE(dEsp > &pCf);
        pNestedHandlerExRecord->m_handlerInfo.m_hThrowable=NULL; // This is random memory.  Handle
                                                                 // must be initialized to null before
                                                                 // calling Init(), as Init() will try
                                                                 // to free any old handle.
        pNestedHandlerExRecord->Init(0, (PEXCEPTION_ROUTINE)COMPlusNestedExceptionHandler, pNestedFrame);

        INSTALL_EXCEPTION_HANDLING_RECORD(&(pNestedHandlerExRecord->m_ExReg));

        context.SetSP(pNestedHandlerExRecord);

        // We might have moved the bottommost handler.  The nested record itself is never
        // the bottom most handler -- it's pushed afte the fact.  So we have to make the
        // bottom-most handler the one BEFORE the nested record.
        if (pExInfo->m_pBottomMostHandler < pNewBottomMostHandler)
        {
          pExInfo->m_pBottomMostHandler = pNewBottomMostHandler;
        }

        if  (pShadowSP)
        {
            *pShadowSP = (size_t)context.GetSP();
        }
    }

    STRESS_LOG3(LF_EH, LL_INFO100, "ResumeAtJitEH: resuming at EIP = %p  ESP = %p EBP = %p\n",
                context.Eip, context.GetSP(), context.GetFP());

#ifdef STACK_GUARDS_DEBUG
    // We are transitioning back to managed code, so ensure that we are in 
    // SO-tolerant mode before we do so. 
    RestoreSOToleranceState();
#endif

    pThread->ResetThreadStateNC(Thread::TSNC_SOIntolerant);


#if defined(FEATURE_PAL) && !defined(PLATFORM_UNIX)
    // Must use the value of ESP from before we put on the NestedExceptionRecord
    // Otherwise we might miss some records!
    PAL_DoLastMomentUnwind((PVOID)(SIZE_T)dEsp);
#endif

    // we want this to happen as late as possible but certainly after the notification
    // that the handle for the current ExInfo has been freed has been delivered
    pExInfo->m_EHClauseInfo.SetManagedCodeEntered(TRUE);

    ResumeAtJitEHHelper(&context);
    UNREACHABLE_MSG("Should never return from ResumeAtJitEHHelper!");

    // we do not set pExInfo->m_EHClauseInfo.m_fManagedCodeEntered = FALSE here, 
    // that happens when the catch clause calls back to COMPlusEndCatch 
    // we don't return to this point so it would be moot (see unreachable_msg above)
    
}
#if defined(_MSC_VER)
#pragma warning(pop)
#endif    

// Must be in a separate function because INSTALL_COMPLUS_EXCEPTION_HANDLER has a filter
int CallJitEHFilterWorker(size_t *pShadowSP, EHContext *pContext)
{
    WRAPPER_CONTRACT;

    int retVal = EXCEPTION_CONTINUE_SEARCH;

    BEGIN_CALL_TO_MANAGED();

    retVal = CallJitEHFilterHelper(pShadowSP, pContext);

    END_CALL_TO_MANAGED();

    return retVal;
}

int CallJitEHFilter(CrawlFrame* pCf, BYTE* startPC, EE_ILEXCEPTION_CLAUSE *EHClausePtr, DWORD nestingLevel, OBJECTREF thrownObj)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    int retVal = EXCEPTION_CONTINUE_SEARCH;
    size_t * pShadowSP = NULL;
    EHContext context;

    context.Setup(startPC + EHClausePtr->FilterOffset, pCf->GetRegisterSet());

    size_t * pEndFilter = NULL; // Write
    pCf->GetCodeManager()->FixContext(ICodeManager::FILTER_CONTEXT, &context, pCf->GetInfoBlock(),
                                      startPC, nestingLevel, thrownObj, pCf->GetCodeManState(),
                                      &pShadowSP, &pEndFilter);

    // End of the filter is the same as start of handler
    if (pEndFilter)
    {
        *pEndFilter = EHClausePtr->HandlerStartPC;
    }

    // ExceptionFilterFrame serves two purposes:
    //
    // 1. It serves as a frame that stops the managed search for handler 
    // if we fault in the filter. ThrowCallbackType.pTopFrame is going point 
    // to this frame during search for exception handler inside filter.
    // The search for handler needs a frame to stop. If we had no frame here,
    // the exceptions in filters would not be swallowed correctly since we would
    // walk past the EX_TRY/EX_CATCH block in COMPlusThrowCallbackHelper.
    //
    // 2. It allows setting of SHADOW_SP_FILTER_DONE flag in UnwindFrames() 
    // if we fault in the filter. We have to set this flag together with unwinding
    // of the filter frame. Using a regular C++ holder to clear this flag here would cause 
    // GC holes. The stack would be in inconsistent state when we trigger gc just before
    // returning from UnwindFrames.

    FrameWithCookie<ExceptionFilterFrame> exceptionFilterFrame(pShadowSP);
    retVal = CallJitEHFilterWorker(pShadowSP, &context);
    exceptionFilterFrame.Pop();

    return retVal;
}

void CallJitEHFinally(CrawlFrame* pCf, BYTE* startPC, EE_ILEXCEPTION_CLAUSE *EHClausePtr, DWORD nestingLevel)
{
    WRAPPER_CONTRACT;

    EHContext context;
    context.Setup(startPC + EHClausePtr->HandlerStartPC, pCf->GetRegisterSet());

    size_t * pShadowSP = NULL; // Write Esp to *pShadowSP before jumping to handler

    size_t * pFinallyEnd = NULL;
    pCf->GetCodeManager()->FixContext(
        ICodeManager::FINALLY_CONTEXT, &context, pCf->GetInfoBlock(),
        startPC, nestingLevel, ObjectToOBJECTREF((Object *) NULL), pCf->GetCodeManState(),
        &pShadowSP, &pFinallyEnd);

    if (pFinallyEnd)
    {
        *pFinallyEnd = EHClausePtr->HandlerEndPC;
    }

    CallJitEHFinallyHelper(pShadowSP, &context);

    // This does not need to be guarded by a holder because the frame is dead if an exception gets thrown.  Filters are different
    //  since they are run in the first pass, so we must update the shadowSP reset in CallJitEHFilter.
    if (pShadowSP) {
        *pShadowSP = 0;  // reset the shadowSP to 0
    }
}
#if defined(_MSC_VER)
#pragma warning (default : 4731)
#endif

//=====================================================================
// *********************************************************************
//========================================================================
//  PLEASE READ, if you are using the following SEH setup functions in your stub
//  EmitSEHProlog :: is used for setting up SEH handler prolog
//  EmitSEHEpilog :: is used for setting up SEH handler epilog
//
//  The following exception record is pushed into the stack, the layout
//  is similar to NT's ExceptionRegistrationRecord,
//  from the pointer to the exception record, we can detect the beginning
//  of the frame which is at a well-known offset from the exception record
//
//  NT exception registration record looks as follows
//  typedef struct _EXCEPTION_REGISTRATION_RECORD {
//      struct _EXCEPTION_REGISTRATION_RECORD *Next;
//        PEXCEPTION_ROUTINE Handler;
//  } EXCEPTION_REGISTRATION_RECORD;
//
//  typedef EXCEPTION_REGISTRATION_RECORD *PEXCEPTION_REGISTRATION_RECORD;
//
//   But our exception records have extra information towards the end
//  struct CUSTOM_EXCEPTION_REGISTRATION_RECORD
//  {
//      PEXCEPTION_REGISTRATION_RECORD  m_pNext;
//      LPVOID                  m_pvFrameHandler;
//      .... frame specific data, the handler should know the offset to the frame
//  };
//
//========================================================================

//-------------------------------------------------------------------------
// Exception handler for COM to managed frame
//  and the layout of the exception registration record structure in the stack
//  the layout is similar to the NT's EXCEPTIONREGISTRATION record
//  followed by the UnmanagedToManagedCallFrame specific info

struct ComToManagedExRecord
{
    EXCEPTION_REGISTRATION_RECORD   m_ExReg;

    // scratch area - corresponds to FrameHandlerExRecord::m_tct
    ThrowCallbackType               m_tct;

    UnmanagedToManagedCallFrame*        GetCurrFrame()
    {
        LEAF_CONTRACT;
        return (UnmanagedToManagedCallFrame*)
            ((BYTE*)this + sizeof(ComToManagedExRecord) + UMThkCallFrame::GetNegSpaceSize());

    }
};

UnmanagedToManagedCallFrame* GetCurrFrame(ComToManagedExRecord *pExRecord)
{
    WRAPPER_CONTRACT;
    return pExRecord->GetCurrFrame();
}

BOOL ComPlusFrameSEH(EXCEPTION_REGISTRATION_RECORD* pEHR)
{
    LEAF_CONTRACT;

    return ((LPVOID)pEHR->Handler == (LPVOID)COMPlusFrameHandler || (LPVOID)pEHR->Handler == (LPVOID)COMPlusNestedExceptionHandler);
}


//
//-------------------------------------------------------------------------
// This is installed when we call COMPlusFrameHandler to provide a bound to
// determine when are within a nested exception
//-------------------------------------------------------------------------
EXCEPTION_HANDLER_IMPL(COMPlusNestedExceptionHandler)
{
    WRAPPER_CONTRACT;

    if (pExceptionRecord->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND)
        && !(pEstablisherFrame->dwFlags & PAL_EXCEPTION_FLAGS_All)
        ) {

        LOG((LF_EH, LL_INFO100, "    COMPlusNestedHandler(unwind) with %x at %x\n", pExceptionRecord->ExceptionCode,
            pContext ? GetIP(pContext) : 0));

        // We're unwinding past a nested exception record, which means that we've thrown
        // a new exception out of a region in which we're handling a previous one.  The
        // previous exception is overridden -- and needs to be unwound.

        // The preceding is ALMOST true.  There is one more case, where we use setjmp/longjmp
        // from withing a nested handler.  We won't have a nested exception in that case -- just
        // the unwind.

        Thread* pThread = GetThread();
        _ASSERTE(pThread);
        ExInfo* pExInfo = &(pThread->GetExceptionState()->m_currentExInfo);
        ExInfo* pPrevNestedInfo = pExInfo->m_pPrevNestedInfo;

        if (pPrevNestedInfo == &((NestedHandlerExRecord*)pEstablisherFrame)->m_handlerInfo)
        {
            _ASSERTE(pPrevNestedInfo);

            LOG((LF_EH, LL_INFO100, "COMPlusNestedExceptionHandler: PopExInfo(): popping nested ExInfo at 0x%p\n", pPrevNestedInfo));

            pPrevNestedInfo->DestroyExceptionHandle();
            pPrevNestedInfo->m_StackTraceInfo.FreeStackTrace();

#ifdef DEBUGGING_SUPPORTED
            if (g_pDebugInterface != NULL)
            {
                g_pDebugInterface->DeleteInterceptContext(pPrevNestedInfo->m_DebuggerExState.GetDebuggerInterceptContext());
            }
#endif // DEBUGGING_SUPPORTED

            pExInfo->m_pPrevNestedInfo = pPrevNestedInfo->m_pPrevNestedInfo;

        } else {
            // The whacky setjmp/longjmp case.  Nothing to do.
        }

    } else {
        LOG((LF_EH, LL_INFO100, "    InCOMPlusNestedHandler with %x at %x\n", pExceptionRecord->ExceptionCode,
            pContext ? GetIP(pContext) : 0));
    }


    // There is a nasty "gotcha" in the way exception unwinding, finally's, and nested exceptions
    // interact.  Here's the scenario ... it involves two exceptions, one normal one, and one
    // raised in a finally.
    //
    // The first exception occurs, and is caught by some handler way up the stack.  That handler
    // calls RtlUnwind -- and handlers that didn't catch this first exception are called again, with
    // the UNWIND flag set.  If, one of the handlers throws an exception during
    // unwind (like, a throw from a finally) -- then that same handler is not called during
    // the unwind pass of the second exception.  [ASIDE: It is called on first-pass.]
    //
    // What that means is -- the COMPlusExceptionHandler, can't count on unwinding itself correctly
    // if an exception is thrown from a finally.  Instead, it relies on the NestedExceptionHandler
    // that it pushes for this.
    //

    EXCEPTION_DISPOSITION retval = EXCEPTION_HANDLER_FWD(COMPlusFrameHandler);
    LOG((LF_EH, LL_INFO100, "Leaving COMPlusNestedExceptionHandler with %d\n", retval));
    return retval;
}

EXCEPTION_REGISTRATION_RECORD *FindNestedEstablisherFrame(EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame)
{
    LEAF_CONTRACT;

    while (pEstablisherFrame->Handler != (PEXCEPTION_ROUTINE)COMPlusNestedExceptionHandler) {
        pEstablisherFrame = pEstablisherFrame->Next;
        _ASSERTE(pEstablisherFrame != EXCEPTION_CHAIN_END);   // should always find one
    }
    return pEstablisherFrame;
}

EXCEPTION_HANDLER_IMPL(FastNExportExceptHandler)
{
    LEAF_CONTRACT;

    // We have our own handler here to work around a debug build check where excep.cpp
    // asserts that a buffer of sentinel values below the SEH frame hasn't been overwritten
    // Having our own handler may also come in handy for future flexibility.
    return EXCEPTION_HANDLER_FWD(COMPlusFrameHandler);
}


// Just like a regular NExport handler -- except it pops an extra frame on unwind.  A handler
// like this is needed by the COMMethodStubProlog code.  It first pushes a frame -- and then
// pushes a handler.  When we unwind, we need to pop the extra frame to avoid corrupting the
// frame chain in the event of an unmanaged catcher.
//
EXCEPTION_HANDLER_IMPL(UMThunkPrestubHandler)
{
    STATIC_CONTRACT_THROWS; // COMPlusFrameHandler throws
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;

    EXCEPTION_DISPOSITION retval = ExceptionContinueSearch;
    
    BEGIN_CONTRACT_VIOLATION(SOToleranceViolation);
    
    // We must forward to the COMPlusFrameHandler. This will unwind the Frame Chain up to here, and also leave the
    // preemptive GC mode set correctly.
    retval = EXCEPTION_HANDLER_FWD(COMPlusFrameHandler);

    if (IS_UNWINDING(pExceptionRecord->ExceptionFlags))
    {
        // Pops an extra frame on unwind.

        GCX_COOP();     // Must be cooperative to modify frame chain.

        Thread *pThread = GetThread();
        _ASSERTE(pThread);
        Frame *pFrame = pThread->GetFrame();
        pFrame->ExceptionUnwind();
        pFrame->Pop(pThread);
    }

    END_CONTRACT_VIOLATION;
    
    return retval;
}


bool IsInstrModifyFault(PEXCEPTION_POINTERS pExceptionInfo)
{
    LEAF_CONTRACT;

    // No longer implemented on x86.
    return false;
}

#define DBG_PRINTEXCEPTION_C             ((DWORD)0x40010006L)

EXCEPTION_HANDLER_IMPL(CLRVectoredExceptionHandlerShimX86)
{
    WRAPPER_CONTRACT;

    EXCEPTION_POINTERS      ExceptionInfo = { pExceptionRecord, pContext };
    EXCEPTION_DISPOSITION   result = ExceptionContinueSearch;

    if (pExceptionRecord->ExceptionCode == DBG_PRINTEXCEPTION_C)
    {
        return ExceptionContinueSearch;
    }

    //
    // ok to use LOG() for the rest of the function...
    //

    LONG vectoredResult;
    vectoredResult = CLRVectoredExceptionHandler(&ExceptionInfo);

    switch (vectoredResult)
    {
        case EXCEPTION_CONTINUE_SEARCH: // fall thru
        case EXCEPTION_EXECUTE_HANDLER:     result = ExceptionContinueSearch; break;
        case EXCEPTION_CONTINUE_EXECUTION:  result = ExceptionContinueExecution; break;
        default: UNREACHABLE_MSG("unexpected result returned from CLRVectoredExceptionHandler");
    }

    return result;
}

LONG CLRNoCatchHandler(EXCEPTION_POINTERS* pExceptionInfo, PVOID pv)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_ENTRY_POINT;

    LONG result = EXCEPTION_CONTINUE_SEARCH;

    // This function can be called during the handling of a SO
    //BEGIN_ENTRYPOINT_VOIDRET;

    result = CLRVectoredExceptionHandler(pExceptionInfo);

    if (EXCEPTION_EXECUTE_HANDLER == result)
    {
        result = EXCEPTION_CONTINUE_SEARCH;
    }

    // Possibly capture Watson buckets based on configuration.  Allows !WatsonBuckets in
    //  the debugger on second chance exceptions.
    GetThread()->GetExceptionState()->MaybeEagerlyCaptureUnhandledInfoForWatson();

    //END_ENTRYPOINT_VOIDRET;

    return result;
}

