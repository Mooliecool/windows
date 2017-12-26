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
// ---------------------------------------------------------------------------
// EEPolicy.h
// ---------------------------------------------------------------------------

#ifndef EEPOLICY_H_
#define EEPOLICY_H_

#include "vars.hpp"
#include "corhost.h"

extern "C" UINT_PTR __stdcall GetCurrentIP();

#if USE_INDIRECT_GET_THREAD_APPDOMAIN
extern "C" Thread* (__stdcall *GetThread)(void);
#else
extern "C" Thread* GetThread();
#endif

enum StackOverflowDetector
{
    SOD_VectoredExceptionHandler,
    SOD_ManagedFrameHandler,
    SOD_SOTolerantTransitor,
    SOD_SOIntolerantTransitor,
    SOD_SOTolerantTransitorGotSoftSO,
};

// EEPolicy maintains actions for resource failure and timeout
class EEPolicy
{
public:
    enum ThreadAbortTypes
    {
        TA_None,  // No Abort
        // Abort at a safe spot: not having any lock, not inside finally, not inside catch
        TA_Safe,
        // Do we need this one?
        TA_V1Compatible,
        // Do not run user finally, no attention to lock count
        TA_Rude
    };

    enum AppDomainUnloadTypes
    {
        ADU_Safe,
        ADU_Rude
    };

    EEPolicy ();

    HRESULT SetTimeout(EClrOperation operation, DWORD timeout);

    DWORD GetTimeout(EClrOperation operation)
    {
        LEAF_CONTRACT;
        _ASSERTE (operation < MaxClrOperation);
        return m_Timeout[operation];
    }

    HRESULT SetActionOnTimeout(EClrOperation operation, EPolicyAction action);
    EPolicyAction GetActionOnTimeout(EClrOperation operation, Thread *pThread)
    {
        WRAPPER_CONTRACT;
        _ASSERTE (operation < MaxClrOperation);
        EPolicyAction action = m_ActionOnTimeout[operation];
    
        return GetFinalAction(action, pThread);
    }

    void NotifyHostOnTimeout(EClrOperation operation, EPolicyAction action);

    HRESULT SetTimeoutAndAction(EClrOperation operation, DWORD timeout, EPolicyAction action);
    
    HRESULT SetDefaultAction(EClrOperation operation, EPolicyAction action);
    EPolicyAction GetDefaultAction(EClrOperation operation, Thread *pThread)
    {
        WRAPPER_CONTRACT;
        _ASSERTE (operation < MaxClrOperation);
        return GetFinalAction (m_DefaultAction[operation], pThread);
    }

    void NotifyHostOnDefaultAction(EClrOperation operation, EPolicyAction action);

    HRESULT SetActionOnFailure(EClrFailure failure, EPolicyAction action);

    EPolicyAction GetActionOnFailure(EClrFailure failure);

    // get and set unhandled exception policy
    void SetUnhandledExceptionPolicy(EClrUnhandledException policy)
    {
        LEAF_CONTRACT;
        m_unhandledExceptionPolicy = policy;
    }
    EClrUnhandledException GetUnhandledExceptionPolicy()
    {
        LEAF_CONTRACT;
        return m_unhandledExceptionPolicy;
    }

    static EPolicyAction DetermineResourceConstraintAction(Thread *pThread);

    static void PerformResourceConstraintAction(Thread *pThread, EPolicyAction action, UINT exitCode, BOOL haveStack);

    static void PerformADUnloadAction(EPolicyAction action, BOOL haveStack, BOOL forStackOverflow = FALSE);

    static void HandleOutOfMemory();

    static void HandleStackOverflow(StackOverflowDetector detector);

    static void HandleSoftStackOverflow(BOOL fSkipDebugger = FALSE);
    
    static void HandleExitProcess();

    static void DECLSPEC_NORETURN HandleFatalError(UINT exitCode, UINT_PTR address, LPCWSTR pMessage=NULL, PEXCEPTION_POINTERS pExceptionInfo= NULL);

    static void DECLSPEC_NORETURN HandleFatalStackOverflow(EXCEPTION_POINTERS *pException, BOOL fSkipDebugger = FALSE);

    static void HandleExitProcessFromEscalation(EPolicyAction action, UINT exitCode);

private:
    DWORD m_Timeout[MaxClrOperation];
    EPolicyAction m_ActionOnTimeout[MaxClrOperation];
    EPolicyAction m_DefaultAction[MaxClrOperation];
    EPolicyAction m_ActionOnFailure[MaxClrFailure];
    EClrUnhandledException m_unhandledExceptionPolicy;
    

    BOOL IsValidActionForOperation(EClrOperation operation, EPolicyAction action);
    BOOL IsValidActionForTimeout(EClrOperation operation, EPolicyAction action);
    BOOL IsValidActionForFailure(EClrFailure failure, EPolicyAction action);
    EPolicyAction GetFinalAction(EPolicyAction action, Thread *pThread);

    static void LogFatalError(UINT exitCode, UINT_PTR address, LPCWSTR pMessage=NULL, PEXCEPTION_POINTERS pExceptionInfo=NULL);
};

void InitEEPolicy();

extern BYTE g_EEPolicyInstance[];

inline EEPolicy* GetEEPolicy()
{
    return (EEPolicy*)&g_EEPolicyInstance;
}

extern void FinalizerThreadAbortOnTimeout();
extern ULONGLONG GetObjFinalizeStartTime();

//
// Use EEPOLICY_HANDLE_FATAL_ERROR when you have a situtation where the Runtime's internal state would be
// inconsistent if execution were allowed to continue. This will apply the proper host's policy for fatal
// errors. Note: this call will never return.
//
// NOTE: make sure to use the macro instead of claling EEPolicy::HandleFatalError directly. The macro grabs the IP
// of where you are calling this from, so we can log it to help when debugging these failures later.
//


#define EEPOLICY_HANDLE_FATAL_ERROR(_exitcode) EEPolicy::HandleFatalError(_exitcode, GetCurrentIP());

#define EEPOLICY_HANDLE_FATAL_ERROR_USING_EXCEPTION_INFO(_exitcode, _pExceptionInfo) EEPolicy::HandleFatalError(_exitcode, GetCurrentIP(), NULL, _pExceptionInfo);

#endif  // EEPOLICY_H_
