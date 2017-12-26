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
#ifndef __ExState_h__
#define __ExState_h__ 

class ExceptionFlags;
class DebuggerExState;
class EHClauseInfo;


#include "exinfo.h"


//
// This class serves as a forwarding and abstraction layer for the EH subsystem.
// Since we have two different implementations, this class is needed to unify 
// the EE's view of EH.  Ideally, this is just a step along the way to a unified
// EH subsystem.
//
class ThreadExceptionState
{
    friend class ClrDataExceptionState;
    friend class CheckAsmOffsets;
    friend class StackFrameIterator;

#ifdef DACCESS_COMPILE
    friend class ClrDataAccess;
#endif // DACCESS_COMPILE

    // ProfToEEInterfaceImpl::GetNotifiedExceptionClauseInfo needs access so that it can fetch the
    // ExceptionTracker or the ExInfo as appropriate for the platform
    friend class ProfToEEInterfaceImpl;

    friend class ExInfo;

public:
    
    void FreeAllStackTraces();
    void ClearThrowablesForUnload(HandleTableBucket* pHndTblBucket);

#ifdef _DEBUG
    typedef enum 
    {
        STEC_All,
        STEC_CurrentTrackerEqualNullOkHackForFatalStackOverflow
    } SetThrowableErrorChecking;
#endif

    void                SetThrowable(OBJECTREF throwable DEBUG_ARG(SetThrowableErrorChecking stecFlags = STEC_All));
    OBJECTREF           GetThrowable();
    OBJECTHANDLE        GetThrowableAsHandle();
    DWORD               GetExceptionCode();
    EXCEPTION_POINTERS* GetExceptionPointers();
    EXCEPTION_RECORD*   GetExceptionRecord();
    CONTEXT*            GetContextRecord();
    BOOL                IsExceptionInProgress();
    void                GetLeafFrameInfo(StackTraceElement* pStackTrace);

    ExceptionFlags*     GetFlags();

    ThreadExceptionState();
    ~ThreadExceptionState();



#ifdef DEBUGGING_SUPPORTED    
    DebuggerExState*    GetDebuggerState();
    BOOL                IsDebuggerInterceptable();
#endif // DEBUGGING_SUPPORTED

    EHClauseInfo*       GetCurrentEHClauseInfo();

#ifdef DACCESS_COMPILE
    void EnumChainMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif // DACCESS_COMPILE
    
    
    void MaybeEagerlyCaptureUnhandledInfoForWatson(); // always defined -- empty when not implemented

#if defined(PRESERVE_WATSON_ACROSS_CONTEXTS) // Watson isn't implemented on Rotor or DAC
    void CaptureUnhandledInfoForWatson();
    void ClearUnhandledInfoForWatson();
    void SaveIpForUnhandledInfo(UINT_PTR ip);
    UINT_PTR RetrieveIpForUnhandledInfo();
    
#if defined(_DEBUG)
    void AssertStackTraceInfo(StackTraceInfo *pSTI);
#endif // _debug

#endif // rotor or dac

    // After unwinding from an SO, there may be stale exception state.
    void ClearExceptionStateAfterSO(void* pStackFrameSP);

    enum ThreadExceptionFlag
    {
        TEF_None                          = 0x00000000,

        // Right now this flag is only used on WIN64.  We set this flag near the end of the second pass when we pop 
        // the ExceptionTracker for the current exception but before we actually resume execution.  It is unsafe
        // to start a funclet-skipping stackwalk in this time window.
        TEF_InconsistentExceptionState    = 0x00000001,
    };

    void SetThreadExceptionFlag(ThreadExceptionFlag flag);
    void ResetThreadExceptionFlag(ThreadExceptionFlag flag);
    BOOL HasThreadExceptionFlag(ThreadExceptionFlag flag);

private:
    Thread* GetMyThread();

    ExInfo                  m_currentExInfo;


   ThreadExceptionFlag      m_flag; 


    
    //
    // @NICE: Ideally, these friends shouldn't all be enumerated like this.  If they were all part of the same
    // class, that would be nice.  I'm trying to avoid adding x86-specific accessors to this class as well as
    // trying to limit the visibility of the ExInfo struct since Win64 doesn't use ExInfo.
    //
    friend EXCEPTION_DISPOSITION COMPlusAfterUnwind(
            EXCEPTION_RECORD *pExceptionRecord,
            EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
            ThrowCallbackType& tct);
    friend EXCEPTION_DISPOSITION COMPlusAfterUnwind(
            EXCEPTION_RECORD *pExceptionRecord,
            EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
            ThrowCallbackType& tct,
            Frame *pStartFrame);

    friend EXCEPTION_DISPOSITION __cdecl
    CPFH_RealFirstPassHandler(EXCEPTION_RECORD *pExceptionRecord,
                              EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
                              CONTEXT *pContext,
                              void *pDispatcherContext,
                              BOOL bAsynchronousThreadStop,
                              BOOL fPGCDisabledOnEntry);

    friend EXCEPTION_DISPOSITION __cdecl
    CPFH_UnwindHandler(EXCEPTION_RECORD *pExceptionRecord,
                       EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
                       CONTEXT *pContext,
                       void *pDispatcherContext);
    
    friend void CPFH_UnwindFrames1(Thread* pThread,
                                   EXCEPTION_REGISTRATION_RECORD* pEstablisherFrame,
                                   DWORD exceptionCode);

    friend LPVOID __stdcall COMPlusEndCatch( Thread *pThread, CONTEXT *pCtx, void *pSEH);

    friend StackWalkAction COMPlusThrowCallback(CrawlFrame *pCf, ThrowCallbackType *pData);

    friend StackWalkAction COMPlusUnwindCallback(CrawlFrame *pCf, ThrowCallbackType *pData);

    friend void ResumeAtJitEH(CrawlFrame* pCf, BYTE* startPC, EE_ILEXCEPTION_CLAUSE *EHClausePtr, 
                                   DWORD nestingLevel, Thread *pThread, BOOL unwindStack);

    friend _EXCEPTION_HANDLER_DECL(COMPlusNestedExceptionHandler);

    friend void COMPlusCooperativeTransitionHandler(Frame* pFrame);

    friend bool ShouldHandleManagedFault(
                        EXCEPTION_RECORD*               pExceptionRecord,
                        CONTEXT*                        pContext,
                        EXCEPTION_REGISTRATION_RECORD*  pEstablisherFrame,
                        Thread*                         pThread);

    friend LONG WINAPI CLRVectoredExceptionHandlerShim(PEXCEPTION_POINTERS pExceptionInfo);

    friend class Thread;
    // It it the following method that needs to be a friend.  But the prototype pulls in a lot more stuff,
    //  so just make the Thread class a friend.
    // friend StackWalkAction Thread::StackWalkFramesEx(PREGDISPLAY pRD, PSTACKWALKFRAMESCALLBACK pCallback,
    //                 VOID *pData, unsigned flags, Frame *pStartFrame);


};


// <WARNING>
// This holder is not thread safe.
// </WARNING>
class ThreadExceptionFlagHolder
{
public:
    ThreadExceptionFlagHolder(ThreadExceptionState::ThreadExceptionFlag flag);
    ~ThreadExceptionFlagHolder();

private:
    ThreadExceptionState*                       m_pExState;
    ThreadExceptionState::ThreadExceptionFlag   m_flag;
};


#endif // __ExState_h__
