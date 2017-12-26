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
//-----------------------------------------------------------------------------
// Stack Probe Header
// Used to setup stack guards
//-----------------------------------------------------------------------------
#ifndef __STACKPROBE_h__
#define __STACKPROBE_h__

//-----------------------------------------------------------------------------
// Stack Guards.
//
// The idea is to force stack overflows to occur at convenient spots.
// * Fire at RequiresNPagesStack (beggining of func) if this functions locals
// cause overflow. Note that in a debug mode, initing the locals to garbage
// will cause the overflow before this macro is executed.
//
// * Fire at CheckStack (end of func) if either our nested function calls
// cause or use of _alloca cause the stack overflow. Note that this macro
// is debug only, so release builds won't catch on this
//
// Some comments:
// - Stack grows *down*,
// - Ideally, all funcs would have EBP frame and we'd use EBP instead of ESP,
//    however, we use the 'this' ptr to get the stack ptr, since the guard
//    is declared on the stack.
//
// Comments about inlining assembly w/ Macros:
// - Must use cstyle comments /* ... */
// - No semi colons, need __asm keyword at the start of each line
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// *How* to use stack guards.
//
// See <http://teams/sites/clrdev/Documents/SO Guide for CLR Developers.doc>
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Stack guards have 3 compiler states:
//#define TOTALLY_DISBLE_STACK_GUARDS
// (DEBUG) All stack guard code is completely removed by the preprocessor.
// This should only be used to disable guards to control debugging situations
//
//#define STACK_GUARDS_DEBUG
// (DEBUG) Full stack guard debugging including cookies, tracking ips, and
// chaining. More heavy weight, recommended for a debug build only
//
//#define STACK_GUARDS_RELEASE
// (RELEASE) Light stack guard code. For golden builds. Forces Stack Overflow
// to happen at "convenient" times. No debugging help.
//-----------------------------------------------------------------------------

#include "genericstackprobe.h"
#include "utilcode.h"

#ifndef TOTALLY_DISBLE_STACK_GUARDS

#define DEFAULT_INTERIOR_PROBE_AMOUNT 4

#define MINIMUM_STACK_REQUIREMENT (0.25)

class RestoreSOState
{
    Thread *m_pThread;
    BOOL m_fSOIntolerantAtEntry;

public:
    RestoreSOState (Thread *pThread);
    ~RestoreSOState();
};

BOOL IsBackoutCalledForEH(BYTE *origSP, BYTE *backoutSP);

//=============================================================================
// Common code
//=============================================================================
// Release version of the probe function
BOOL RetailStackProbeNoThrow(unsigned int n, Thread *pThread);
void RetailStackProbe(unsigned int n, Thread *pThread);
void ReportStackOverflow(Thread *pThread);

void RetailStackProbe(unsigned int n);

BOOL ShouldProbeOnThisThread();

int SOTolerantBoundaryFilter(EXCEPTION_POINTERS *pExceptionInfo, BOOL fReturnToSOIntolerant);

#endif

//=============================================================================
// DEBUG
//=============================================================================
#if defined(STACK_GUARDS_DEBUG)

#include "common.h"

class BaseStackGuard;

//-----------------------------------------------------------------------------
// Need to chain together stack guard address for nested functions
// Use a TLS slot to store the head of the chain
//-----------------------------------------------------------------------------
extern DWORD g_CurrentStackGuardTlsIdx;

//-----------------------------------------------------------------------------
// Class
//-----------------------------------------------------------------------------

// Base version - has no ctor/dtor, so we can use it with SEH
//
// *** Don't declare any members here.  Put them in BaseStackGuardGeneric.
// We downcast directly from the base to the derived, using the knowledge
// that the base class and the derived class are identical for members.
//
class BaseStackGuard : public BaseStackGuardGeneric
{
protected:
    BaseStackGuard()
    {
        _ASSERTE(!"No default construction allowed");
    }

public:
    BaseStackGuard(const char *szFunction, const char *szFile, unsigned int lineNum) :
        BaseStackGuardGeneric(szFunction, szFile, lineNum)
    {
        STATIC_CONTRACT_LEAF;
    }

    UINT_PTR *Marker() { return m_pMarker; }


    unsigned int Depth() { return m_depth; }

    const char *FunctionName() { return m_szFunction; }

    BOOL IsProbeGuard()
    {
        return (m_isBoundaryGuard == FALSE);
    }

    BOOL IsBoundaryGuard()
    {
        return (m_isBoundaryGuard == TRUE);
    }

    inline BOOL ShouldCheckPreviousCookieIntegrity();
    inline BOOL ShouldCheckThisCookieIntegrity();

    BOOL RequiresNStackPages(unsigned int n, BOOL fThrowOnSO = TRUE);
    BOOL DoProbe(unsigned int n, BOOL fThrowOnSO);
    void CheckStack();

    static void RestoreCurrentGuard(BOOL fWasDisabled = FALSE);
    void PopGuardForEH();

    // Different error messages for the different times we detemine there's a problem.
    void HandleBlowThisStackGuard(__in_z char *stackID);
    void HandleBlowPreviousStackGuard(int shortFall, __in_z char *stackID);
    void HandleBlowCurrentStackGuard(int shortFall, __in_z char *stackID);
    static void HandleBlowCurrentStackGuard(void *pGuard, int shortFall, __in_z char *stackID);

    void CheckMarkerIntegrity();
    void RestorePreviousGuard();
    void ProtectMarkerPageInDebugger();
    void UndoPageProtectionInDebugger();
    static void ProtectMarkerPageInDebugger(void *pGuard);
    static void UndoPageProtectionInDebugger(void *pGuard);

    inline HRESULT PrepGuard()
    {
        // See if it has already been prepped...
        if (ClrFlsGetValue(g_CurrentStackGuardTlsIdx) != NULL)
            return S_OK;

        // Let's see if we'll be able to put in a guard page
        ClrFlsSetValue(g_CurrentStackGuardTlsIdx, 
(void*)-1);

        if (ClrFlsGetValue(g_CurrentStackGuardTlsIdx) != (void*)-1)
            return E_OUTOFMEMORY;

        return S_OK;

    }

    inline static BaseStackGuard* GetCurrentGuard()
    {
        if (g_CurrentStackGuardTlsIdx != -1)
            return (BaseStackGuard*) ClrFlsGetValue(g_CurrentStackGuardTlsIdx);
        else
            return NULL;
    }

    inline static BOOL IsGuard(BaseStackGuard *probe)
    {
        return (probe != NULL);
    }
    static void SetCurrentGuard(BaseStackGuard* pGuard);
    static void ResetCurrentGuard(BaseStackGuard* pGuard);

    inline static BOOL IsProbeGuard(BaseStackGuard *probe)
    {
        return (IsGuard(probe) != NULL && probe->IsProbeGuard());
    }

    inline static BOOL IsBoundaryGuard(BaseStackGuard *probe)
    {
        return (IsGuard(probe) != NULL && probe->IsBoundaryGuard());
    }

    static void InitProbeReportingToFaultInjectionFramework();
    BOOL ReportProbeToFaultInjectionFramework();

    static void Terminate();


    static HMODULE  m_hProbeCallBack;
    typedef BOOL (*ProbeCallbackType)(unsigned, const char *);
    static ProbeCallbackType m_pfnProbeCallback;

};


// Derived version, add a dtor that automatically calls Check_Stack, move convenient, but can't use with SEH.
class AutoCleanupStackGuard : public BaseStackGuard
{
protected:
    AutoCleanupStackGuard()
    {
        _ASSERTE(!"No default construction allowed");
    }

public:
    AutoCleanupStackGuard(const char *szFunction, const char *szFile, unsigned int lineNum) :
        BaseStackGuard(szFunction, szFile, lineNum)
    {
        LEAF_CONTRACT;
    }

    ~AutoCleanupStackGuard()
    {
        WRAPPER_CONTRACT;
        CheckStack();
    }
};

class DebugSOIntolerantTransitionHandlerBeginOnly
{
    BOOL m_prevSOTolerantState;
    ClrDebugState* m_clrDebugState;
    char *m_ctorSP;

  public:
    DebugSOIntolerantTransitionHandlerBeginOnly(EEThreadHandle thread);
    ~DebugSOIntolerantTransitionHandlerBeginOnly();
};



extern DWORD g_InteriorProbeAmount;

//=============================================================================
// Macros for transition into SO_INTOLERANT code
//=============================================================================

FORCEINLINE DWORD DefaultEntryProbeAmount() { return g_EntryPointProbeAmount; }

#define BEGIN_SO_INTOLERANT_CODE(pThread)                                                   \
    BEGIN_SO_INTOLERANT_CODE_FOR(pThread, g_EntryPointProbeAmount)                          \

#define BEGIN_SO_INTOLERANT_CODE_FOR(pThread, n)                                            \
    {                                                                                       \
    /*_ASSERTE(pThread); */                                                                 \
    AutoCleanupStackGuard stack_guard_XXX(__FUNCTION__, __FILE__, __LINE__);                \
    stack_guard_XXX.RequiresNStackPages(ADJUST_PROBE(n), TRUE);                             \
    /* work around unreachable code warning */                                              \
        if (true)                                                                           \
        {                                                                                   \
        DebugSOIntolerantTransitionHandler __soIntolerantTransitionHandler(pThread);        \
        ANNOTATION_SO_PROBE_BEGIN(DEFAULT_ENTRY_PROBE_AMOUNT);                              \
        /* work around unreachable code warning */                                          \
            if (true)                                                                       \
            {                                                                               \
            DEBUG_ASSURE_NO_RETURN_BEGIN_SO_INTOLERANT;

#define BEGIN_SO_INTOLERANT_CODE_NOTHROW(pThread, ActionOnSO)                               \
    {                                                                                       \
    _ASSERTE(pThread);                                                                      \
    AutoCleanupStackGuard stack_guard_XXX(__FUNCTION__, __FILE__, __LINE__);                \
    if (! stack_guard_XXX.RequiresNStackPages(ADJUST_PROBE(g_EntryPointProbeAmount), FALSE)) \
    {                                                                                       \
            stack_guard_XXX.SetNoException();                                               \
        ActionOnSO;                                                                         \
    }                                                                                       \
        /* work around unreachable code warning */                                          \
        else                                                                                \
        {                                                                                   \
        DebugSOIntolerantTransitionHandler __soIntolerantTransitionHandler(pThread);        \
        ANNOTATION_SO_PROBE_BEGIN(DEFAULT_ENTRY_PROBE_AMOUNT);                              \
        /* work around unreachable code warning */                                          \
            if (true)                                                                       \
            {                                                                               \
                DEBUG_ASSURE_NO_RETURN_BEGIN_SO_INTOLERANT;


// This cannot be used in SO-intolerant code, because the SO it forces will rip the process
#define BEGIN_SO_INTOLERANT_CODE_NOTHROW_FORCE_SO(pThread)                                  \
    _ASSERTE(pThread->IsSOTolerant());                                                    \
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(pThread, DontCallDirectlyForceStackOverflow())


// This is defined just for using in the InternalSetupForComCall macro which
// doesn't have a corresponding end macro because no exception will pass through it
// It should not be used in any situation where an exception could pass through
// the transition.
#define SO_INTOLERANT_CODE_RETVAL(pThread, hr)                                              \
    AutoCleanupStackGuard stack_guard_XXX(__FUNCTION__, __FILE__, __LINE__);                \
    if (! stack_guard_XXX.RequiresNStackPages(ADJUST_PROBE(g_EntryPointProbeAmount), FALSE)) \
    {                                                                                       \
        return hr;                                                                          \
    }                                                                                       \
    stack_guard_XXX.SetNoException();                                                       \
    DebugSOIntolerantTransitionHandlerBeginOnly __soIntolerantTransitionHandler(pThread);   \
    ANNOTATION_SO_PROBE_BEGIN(DEFAULT_ENTRY_PROBE_AMOUNT);



#define MINIMAL_STACK_PROBE_CHECK_THREAD(pThread)                                           \
    Thread *__pThread = pThread;                                                            \
    if (__pThread && ! __pThread->IsStackSpaceAvailable(MINIMUM_STACK_REQUIREMENT))         \
    {                                                                                       \
        ReportStackOverflow(pThread);                                                       \
    }                                                                                       \
    CONTRACT_VIOLATION(SOToleranceViolation);

// We don't use the DebugSOIntolerantTransitionHandler here because we don't need to transition into
// SO-intolerant code.   We're already there.  We also don't need to annotate as having probed,
// because this only matters for entry point functions.
// We have a way to separate the declaration from the actual probing for cases where need
// to do a test, such as IsGCThread(), to decide if should probe.
#define DECLARE_INTERIOR_STACK_PROBE                                            \
    {                                                                           \
        AutoCleanupStackGuard stack_guard_XXX(__FUNCTION__, __FILE__, __LINE__);\
        DEBUG_ASSURE_NO_RETURN_BEGIN


// A function containing an interior probe is implicilty SO-Intolerant because we
// assume that it is not behind a probe.  So confirm that we are in the correct state.
#define DO_INTERIOR_STACK_PROBE_FOR(pThread, n)                                 \
    _ASSERTE(pThread != NULL);                                                  \
    stack_guard_XXX.RequiresNStackPages(ADJUST_PROBE(n), TRUE);                 \
    EnsureSOIntolerantOK(__FUNCTION__, __FILE__, __LINE__);


#define INTERIOR_STACK_PROBE_FOR(pThread, n)                                    \
    DECLARE_INTERIOR_STACK_PROBE;                                               \
    DO_INTERIOR_STACK_PROBE_FOR(pThread, n);

#define INTERIOR_STACK_PROBE(pThread)                                           \
    INTERIOR_STACK_PROBE_FOR(pThread, g_InteriorProbeAmount);                   \

#define INTERIOR_STACK_PROBE_CHECK_THREAD                                       \
    DECLARE_INTERIOR_STACK_PROBE;                                               \
    if (ShouldProbeOnThisThread())                                              \
    {                                                                           \
        DO_INTERIOR_STACK_PROBE_FOR(GetThread(), g_InteriorProbeAmount);        \
    }                                                                           
    
#define INTERIOR_STACK_PROBE_NOTHROW_CHECK_THREAD(actionOnSO)                   \
    DECLARE_INTERIOR_STACK_PROBE;                                               \
    if (ShouldProbeOnThisThread())                                              \
    {                                                                           \
        DO_INTERIOR_STACK_PROBE_NOTHROW(GetThread(), actionOnSO);               \
    }                                                                           \

// A function containing an interior probe is implicilty SO-Intolerant because we
// assume that it is not behind a probe.  So confirm that we are in the correct state.
#define DO_INTERIOR_STACK_PROBE_FOR_NOTHROW(pThread, n, actionOnSO)             \
        _ASSERTE(pThread != NULL);                                              \
        if (! stack_guard_XXX.RequiresNStackPages(ADJUST_PROBE(n), FALSE))      \
        {                                                                       \
            stack_guard_XXX.SetNoException();                                   \
            actionOnSO;                                                         \
        }                                                                       \
        EnsureSOIntolerantOK(__FUNCTION__, __FILE__, __LINE__);

#define DO_INTERIOR_STACK_PROBE_NOTHROW(pThread, actionOnSO)                    \
    DO_INTERIOR_STACK_PROBE_FOR_NOTHROW(pThread, g_InteriorProbeAmount, actionOnSO);

#define END_INTERIOR_STACK_PROBE                                                \
        DEBUG_ASSURE_NO_RETURN_END                                              \
        stack_guard_XXX.SetNoException();                                       \
    }

#define RETURN_FROM_INTERIOR_PROBE(x)                                           \
        DEBUG_OK_TO_RETURN_BEGIN                                                \
        stack_guard_XXX.SetNoException();                                       \
        RETURN(x);                                                              \
        DEBUG_OK_TO_RETURN_END


// This is used for EH code where we are about to throw.
// To avoid taking an SO during EH processing, want to include it in our probe limits
// So we will just do a big probe and then throw.
#define STACK_PROBE_FOR_THROW(pThread)                                                  \
    AutoCleanupStackGuard stack_guard_XXX(__FUNCTION__, __FILE__, __LINE__);            \
    if (pThread != NULL)                                                                \
    {                                                                                   \
        DO_INTERIOR_STACK_PROBE_FOR(pThread, ADJUST_PROBE(DEFAULT_ENTRY_PROBE_AMOUNT)); \
    }

// This is used for throws where we cannot use a dtor-based probe.
#define PUSH_STACK_PROBE_FOR_THROW(pThread)                                     \
    BaseStackGuard stack_guard_XXX(__FUNCTION__, __FILE__, __LINE__);           \
    stack_guard_XXX.RequiresNStackPages(ADJUST_PROBE(g_EntryPointProbeAmount), TRUE);

#define SAVE_ADDRESS_OF_STACK_PROBE_FOR_THROW(pGuard)                           \
    pGuard = &stack_guard_XXX;

// We never return from RaiseException, so shouldn't have to call SetNoException.
// However, in the debugger we can, and if we don't call SetNoException we get
// a short-circuit return assert.
#define POP_STACK_PROBE_FOR_THROW(pGuard) \
    pGuard->SetNoException ();            \
    pGuard->CheckStack();



class BoundaryStackGuard : public BaseStackGuard
{
protected:
    BoundaryStackGuard()
    {
        LEAF_CONTRACT;

        _ASSERTE(!"No default construction allowed");
    }

public:
    BoundaryStackGuard(const char *szFunction, const char *szFile, unsigned int lineNum)
        : BaseStackGuard(szFunction, szFile, lineNum)
    {
        LEAF_CONTRACT;

        m_isBoundaryGuard = TRUE;
    }

    void Push();
    void Pop();

};

// Derived version, add a dtor that automatically calls Pop, more convenient, but can't use with SEH.
class AutoCleanupBoundaryStackGuard : public BoundaryStackGuard
{
protected:
    AutoCleanupBoundaryStackGuard()
    {
        _ASSERTE(!"No default construction allowed");
    }

public:
    AutoCleanupBoundaryStackGuard(const char *szFunction, const char *szFile, unsigned int lineNum) :
        BoundaryStackGuard(szFunction, szFile, lineNum)
    {
        LEAF_CONTRACT;
    }

    ~AutoCleanupBoundaryStackGuard()
    {
        WRAPPER_CONTRACT;
        Pop();
    }
};


class DebugSOTolerantTransitionHandler
{
    BOOL m_prevSOTolerantState;
    ClrDebugState* m_clrDebugState;

  public:
    void EnterSOTolerantCode(Thread *pThread);
    void ReturnFromSOTolerantCode();
};

class AutoCleanupDebugSOTolerantTransitionHandler : DebugSOTolerantTransitionHandler
{
    BOOL m_prevSOTolerantState;
    ClrDebugState* m_clrDebugState;

  public:
    AutoCleanupDebugSOTolerantTransitionHandler(Thread *pThread)
    {
        EnterSOTolerantCode(pThread);
    }
    ~AutoCleanupDebugSOTolerantTransitionHandler()
    {
        ReturnFromSOTolerantCode();
    }
};


// When we enter SO-tolerant code, we
// 1) probe to make sure that we will have enough stack to run our backout code.  We don't
//    need to check that the cookie was overrun because we only care that we had enough stack.
//    But we do anyway, to pop off the guard.s
//    The backout code infrastcture ensures that we stay below the BACKOUT_CODE_STACK_LIMIT.
// 2) Install a boundary guard, which will preserve our cookie and prevent spurious checks if
//    we call back into the EE.
// 3) Formally transition into SO-tolerant code so that we can make sure we are probing if we call
//    back into the EE.
//

#define BEGIN_SO_TOLERANT_CODE(pThread)                                                     \
    { /* add an outer scope so that we'll restore our state as soon as we return */         \
        BOOL __fReturnToSOIntolerant = FALSE;                                               \
        Thread *__pThread = pThread;                                                        \
        BOOL __fInCooperative = FALSE;                                                      \
        Frame* __pSafeForSOFrame = NULL;                                                    \
        if (__pThread)                                                                      \
        {                                                                                   \
            __fReturnToSOIntolerant = !__pThread->IsSOTolerant();                           \
            __pThread->ResetThreadStateNC(Thread::TSNC_SOIntolerant);                       \
            __fInCooperative = __pThread->PreemptiveGCDisabled();                           \
            __pSafeForSOFrame = __pThread->GetFrame();                                      \
        }                                                                                   \
        BoundaryStackGuard boundary_guard_XXX(__FUNCTION__, __FILE__, __LINE__);            \
        boundary_guard_XXX.Push();                                                          \
        DebugSOTolerantTransitionHandler __soTolerantTransitionHandler;                     \
        __soTolerantTransitionHandler.EnterSOTolerantCode(__pThread);                       \
        BOOL __fRecoverStack = FALSE;                                                       \
        BOOL __fIsStackOverflow = FALSE;                                                    \
        BOOL __fIsSoftStackOverflow = FALSE;                                                \
        __try                                                                               \
        {                                                                                   \
            __try                                                                           \
            {                                                                               \
                DEBUG_ASSURE_NO_RETURN_BEGIN;                                               

// We need to catch any hard SO that comes through in order to get our stack back and make sure that we can run our backout code.
// Also can't allow a hard SO to propogate into SO-intolerant code, as we can't tell where it came from and would have to rip the process.
// So install a filter and catch hard SO and rethrow a C++ SO.  Note that we don't check the host policy here it only applies to exceptions
// that will leak back into managed code.
#define END_SO_TOLERANT_CODE                                                                \
                DEBUG_ASSURE_NO_RETURN_END;                                                 \
                boundary_guard_XXX.SetNoException();                                        \
            }                                                                               \
            __except(SOTolerantBoundaryFilter(GetExceptionInformation(), __fReturnToSOIntolerant))                 \
            {                                                                               \
                /* do nothing here.  Get our stack back post-catch and then throw a new exception */ \
                __fRecoverStack = TRUE;                                                     \
                __soTolerantTransitionHandler.ReturnFromSOTolerantCode();                   \
                if (::GetExceptionCode() == STATUS_STACK_OVERFLOW ||                        \
                    ::GetExceptionCode() == EXCEPTION_SOFTSO)                               \
                {                                                                           \
                    if (::GetExceptionCode() == EXCEPTION_SOFTSO)                           \
                    {                                                                       \
                        __fIsSoftStackOverflow = TRUE;                                      \
                    }                                                                       \
                    __fIsStackOverflow = TRUE;                                              \
                    /* If there is a SO_INTOLERANT region above this */                     \
                    /* we should have processed it already in SOIntolerantTransitionHandler */ \
                    EEPolicy::HandleStackOverflow(__fIsSoftStackOverflow?SOD_SOTolerantTransitorGotSoftSO:SOD_SOTolerantTransitor);                 \
                    __pThread->SetSOExceptionInfo(NULL, NULL);                              \
                    /* If this assertion fires, then it means that we have not unwound the frame chain */ \
                    _ASSERTE(__pSafeForSOFrame == __pThread->GetFrame());                   \
                    __pThread->ClearExceptionStateAfterSO(__pSafeForSOFrame);               \
                }                                                                           \
            }                                                                               \
            if (__fReturnToSOIntolerant)                                                    \
            {                                                                               \
                __pThread->SetThreadStateNC(Thread::TSNC_SOIntolerant);                     \
            }                                                                               \
            if (__fRecoverStack)                                                            \
            {                                                                               \
                if (!__fIsSoftStackOverflow)                                                \
                {                                                                           \
                    __pThread->RestoreGuardPage();                                          \
                }                                                                           \
                if (__fIsStackOverflow)                                                     \
                {                                                                           \
                    if (!__pThread->PreemptiveGCDisabled())                                 \
                    {                                                                       \
                        __pThread->DisablePreemptiveGC();                                   \
                    }                                                                       \
                    /* We have enough stack now.  Start unload */                           \
                    EEPolicy::PerformADUnloadAction(eRudeUnloadAppDomain, TRUE, TRUE);      \
                }                                                                           \
                if (__pThread->PreemptiveGCDisabled() != __fInCooperative)                  \
                {                                                                           \
                    if (__fInCooperative)                                                   \
                    {                                                                       \
                        __pThread->DisablePreemptiveGC();                                   \
                    }                                                                       \
                    else                                                                    \
                    {                                                                       \
                        __pThread->EnablePreemptiveGC();                                    \
                    }                                                                       \
                }                                                                           \
                COMPlusThrowSO();                                                           \
            }                                                                               \
        }                                                                                   \
        __finally                                                                           \
        {                                                                                   \
            __soTolerantTransitionHandler.ReturnFromSOTolerantCode();                       \
            boundary_guard_XXX.Pop();                                                       \
        }                                                                                   \
    }                                                                                       

// This is used for Com interop where we have a catch-all handler to handle any exceptions that
// come through.  We only need to probe, install the boundary guard, and transition into SO-tolerant code.
#define BEGIN_SO_TOLERANT_CODE_HAS_CATCH(pThread)                                           \
    {                                                                                       \
        LOG((LF_EH, LL_INFO10000, "BEGIN_SO_TOLERANT_CODE: line %d in %s(%s) \n", __LINE__, __FUNCTION__, __FILE__)); \
        RestoreSOState __restoreSOState(pThread);                                           \
        if (pThread)                                                                        \
        {                                                                                   \
            pThread->ResetThreadStateNC(Thread::TSNC_SOIntolerant);                         \
        }                                                                                   \
        AutoCleanupBoundaryStackGuard boundary_guard_XXX(__FUNCTION__, __FILE__, __LINE__); \
        boundary_guard_XXX.Push();                                                          \
        AutoCleanupDebugSOTolerantTransitionHandler __soTolerantTransitionHandler(pThread); \
        DEBUG_ASSURE_NO_RETURN_BEGIN;                                                       \

#define END_SO_TOLERANT_CODE_HAS_CATCH                                                      \
        DEBUG_ASSURE_NO_RETURN_END;                                                         \
        boundary_guard_XXX.SetNoException();                                                \
    }

extern unsigned __int64 getTimeStamp();

// This is used for calling into host
// We only need to install the boundary guard, and transition into SO-tolerant code.
#define BEGIN_SO_TOLERANT_CODE_CALLING_HOST(pThread)                                        \
    {                                                                                       \
        LOG((LF_EH, LL_INFO10000, "BEGIN_SO_TOLERANT_CODE_CALLING_HOST: line %d in %s(%s) \n", __LINE__, __FUNCTION__, __FILE__)); \
        ULONGLONG __entryTime = 0;                                                          \
        __int64 __entryTimeStamp = 0;                                                       \
        if (CLRTaskHosted())                                                                \
        {                                                                                   \
            __entryTimeStamp = getTimeStamp();                                              \
            __entryTime = CLRGetTickCount64();                                              \
        }                                                                                   \
        _ASSERTE(CanThisThreadCallIntoHost());                                              \
        _ASSERTE(pThread == NULL || !pThread->IsInForbidSuspendRegion());                   \
        RestoreSOState __restoreSOState(pThread);                                           \
        if (pThread)                                                                        \
        {                                                                                   \
            pThread->ResetThreadStateNC(Thread::TSNC_SOIntolerant);                         \
        }                                                                                   \
        AutoCleanupBoundaryStackGuard boundary_guard_XXX(__FUNCTION__, __FILE__, __LINE__); \
        boundary_guard_XXX.Push();                                                          \
        AutoCleanupDebugSOTolerantTransitionHandler __soTolerantTransitionHandler(pThread); \
        DEBUG_ASSURE_NO_RETURN_BEGIN;                                                       \

#define END_SO_TOLERANT_CODE_CALLING_HOST                                                   \
        if (CLRTaskHosted())                                                                \
        {                                                                                   \
            ULONGLONG __endTime = CLRGetTickCount64();                                      \
            ULONGLONG __elapse = __endTime - __entryTime;                                       \
            if (__elapse > 20000 && __entryTimeStamp)                                       \
            {                                                                               \
                STRESS_LOG4(LF_EH, LL_INFO10, "CALLING HOST takes %d ms: line %d in %s(%s)\n", (int)__elapse, __LINE__, __FUNCTION__, __FILE__); \
            }                                                                               \
        }                                                                                   \
        DEBUG_ASSURE_NO_RETURN_END;                                                         \
        boundary_guard_XXX.SetNoException();                                                \
    }

//-----------------------------------------------------------------------------
// Startup & Shutdown stack guard subsystem
//-----------------------------------------------------------------------------
void InitStackProbes();
void TerminateStackProbes();
#elif defined(TOTALLY_DISBLE_STACK_GUARDS)

//=============================================================================
// Totally Disabled
//=============================================================================
inline void InitStackProbes()
{
    LEAF_CONTRACT;
}

inline void TerminateStackProbes()
{
    LEAF_CONTRACT;
}

#define BEGIN_SO_INTOLERANT_CODE(pThread)
#define BEGIN_SO_INTOLERANT_CODE_FOR(pThread, n)
#define BEGIN_SO_INTOLERANT_CODE_NOTHROW(pThread, ActionOnSO)
#define BEGIN_SO_INTOLERANT_CODE_NOTHROW_FORCE_SO(pThread)
#define SO_INTOLERANT_CODE_RETVAL(pThread, hr)
#define MINIMAL_STACK_PROBE_CHECK_THREAD(pThread)

#define DECLARE_INTERIOR_STACK_PROBE
#define DO_INTERIOR_STACK_PROBE_FOR(pThread, n)
#define END_INTERIOR_STACK_PROBE
#define RETURN_FROM_INTERIOR_PROBE(x) RETURN(x)
#define INTERIOR_STACK_PROBE(pThread)
#define INTERIOR_STACK_PROBE_FOR(pThread, n)
#define INTERIOR_STACK_PROBE_CHECK_THREAD
#define INTERIOR_STACK_PROBE_NOTHROW_CHECK_THREAD(actionOnSO)
#define DO_INTERIOR_STACK_PROBE_FOR_NOTHROW(pThread, n, actionOnSO)
#define DO_INTERIOR_STACK_PROBE_NOTHROW(pThread, actionOnSO)

#define STACK_PROBE_FOR_THROW(pThread)
#define PUSH_STACK_PROBE_FOR_THROW(pThread)
#define SAVE_ADDRESS_OF_STACK_PROBE_FOR_THROW(pGuard)
#define POP_STACK_PROBE_FOR_THROW(pGuard)

#define BEGIN_SO_TOLERANT_CODE(pThread)
#define END_SO_TOLERANT_CODE
#define RETURN_FROM_SO_TOLERANT_CODE_HAS_CATCH
#define BEGIN_SO_TOLERANT_CODE_HAS_CATCH(pThread)
#define END_SO_TOLERANT_CODE_HAS_CATCH
#define BEGIN_SO_TOLERANT_CODE_CALLING_HOST(pThread) \
    _ASSERTE(CanThisThreadCallIntoHost());
#define END_SO_TOLERANT_CODE_CALLING_HOST


FORCEINLINE BOOL ShouldProbeOnThisThread()
{
    return FALSE;
}

#elif defined(STACK_GUARDS_RELEASE)
//=============================================================================
// Release - really streamlined,
//=============================================================================

void InitStackProbesRetail();
inline void InitStackProbes()
{
    InitStackProbesRetail();
}

inline void TerminateStackProbes()
{
    LEAF_CONTRACT;
}


//=============================================================================
// Macros for transition into SO_INTOLERANT code
//=============================================================================

FORCEINLINE DWORD DefaultEntryProbeAmount() { return DEFAULT_ENTRY_PROBE_AMOUNT; }

#define BEGIN_SO_INTOLERANT_CODE(pThread)                                           \
    BEGIN_SO_INTOLERANT_CODE_FOR(pThread, DEFAULT_ENTRY_PROBE_AMOUNT)               \

#define BEGIN_SO_INTOLERANT_CODE_FOR(pThread, n)                                    \
    RetailStackProbe(ADJUST_PROBE(n), pThread);                                     \
    /* match with the else used in other macros */                                  \
    if (true) {                                                                     \
        SOIntolerantTransitionHandler __soIntolerantTransitionHandler(pThread);     \
        /* work around unreachable code warning */                                  \
        if (true) {                                                                 \
            DEBUG_ASSURE_NO_RETURN_BEGIN_SO_INTOLERANT;                             \

#define BEGIN_SO_INTOLERANT_CODE_NOTHROW(pThread, ActionOnSO) \
    if (! RetailStackProbeNoThrow(ADJUST_PROBE(DEFAULT_ENTRY_PROBE_AMOUNT), pThread)) \
    { \
        ActionOnSO; \
    } else { \
        SOIntolerantTransitionHandler __soIntolerantTransitionHandler(pThread); \
        /* work around unreachable code warning */                                          \
        if (true) {                                                             \
            DEBUG_ASSURE_NO_RETURN_BEGIN_SO_INTOLERANT;                                 \

// This cannot be used in SO-intolerant code, because the SO it forces will rip the process
#define BEGIN_SO_INTOLERANT_CODE_NOTHROW_FORCE_SO(pThread)                                      \
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(pThread, DontCallDirectlyForceStackOverflow())


// This is defined just for using in the InternalSetupForComCall macro which
// doesn't have a corresponding end macro because no exception will pass through it
// It should not be used in any situation where an exception could pass through
// the transition.
#define SO_INTOLERANT_CODE_RETVAL(pThread, hr) \
    if (! RetailStackProbeNoThrow(ADJUST_PROBE(DEFAULT_ENTRY_PROBE_AMOUNT), pThread)) \
    { \
        return hr; \
    } \

#define MINIMAL_STACK_PROBE_CHECK_THREAD(pThread)                                               \
        Thread *__pThread = pThread;                                                            \
        if (__pThread && ! __pThread->IsStackSpaceAvailable(MINIMUM_STACK_REQUIREMENT))         \
        {                                                                                       \
            ReportStackOverflow(pThread);                                                       \
        }                                                                                       \

#define DECLARE_INTERIOR_STACK_PROBE

#define DO_INTERIOR_STACK_PROBE_FOR(pThread, n) \
    RetailStackProbe(ADJUST_PROBE(n), pThread); \

#define INTERIOR_STACK_PROBE_CHECK_THREAD                                       \
    DECLARE_INTERIOR_STACK_PROBE;                                               \
    if (ShouldProbeOnThisThread())                                              \
    {                                                                           \
        DO_INTERIOR_STACK_PROBE_FOR(GetThread(), DEFAULT_ENTRY_PROBE_AMOUNT);   \
    }                                                                           
    
#define INTERIOR_STACK_PROBE_NOTHROW_CHECK_THREAD(actionOnSO)                   \
    DECLARE_INTERIOR_STACK_PROBE;                                               \
    if (ShouldProbeOnThisThread())                                              \
    {                                                                           \
        DO_INTERIOR_STACK_PROBE_NOTHROW(GetThread(), actionOnSO);               \
    }                                                                           \

#define INTERIOR_STACK_PROBE_FOR(pThread, n) \
    DO_INTERIOR_STACK_PROBE_FOR(pThread, n); \

#define DO_INTERIOR_STACK_PROBE_FOR_NOTHROW(pThread, n, ActionOnSO)             \
    if (! RetailStackProbeNoThrow(ADJUST_PROBE(n), pThread))                    \
    {                                                                           \
        ActionOnSO;                                                             \
    }                                                                           \


#define DO_INTERIOR_STACK_PROBE_NOTHROW(pThread, ActionOnSO)                    \
    DO_INTERIOR_STACK_PROBE_FOR_NOTHROW(pThread, DEFAULT_INTERIOR_PROBE_AMOUNT, ActionOnSO)


#define INTERIOR_STACK_PROBE(pThread) INTERIOR_STACK_PROBE_FOR(pThread, DEFAULT_INTERIOR_PROBE_AMOUNT);

#define END_INTERIOR_STACK_PROBE

#define RETURN_FROM_INTERIOR_PROBE(x) RETURN(x)

// This is used for EH code where we are about to throw
// To avoid taking an SO during EH processing, want to include it in our probe limits
// So we will just do a big probe and then throw.
#define STACK_PROBE_FOR_THROW(pThread)                                      \
    if (pThread != NULL)                                                    \
    {                                                                       \
        RetailStackProbe(ADJUST_PROBE(DEFAULT_ENTRY_PROBE_AMOUNT), pThread); \
    }                                                                       \

#define PUSH_STACK_PROBE_FOR_THROW(pThread)                                     \
    RetailStackProbe(ADJUST_PROBE(DEFAULT_ENTRY_PROBE_AMOUNT), pThread);

#define SAVE_ADDRESS_OF_STACK_PROBE_FOR_THROW(pGuard)

#define POP_STACK_PROBE_FOR_THROW(pGuard)


//=============================================================================
// Macros for transition into SO_TOLERANT code
//=============================================================================

#define BEGIN_SO_TOLERANT_CODE(pThread)                                                     \
{                                                                                           \
    Thread *__pThread = pThread;                                                            \
    BOOL __fRecoverStack = FALSE;                                                           \
    BOOL __fIsStackOverflow = FALSE;                                                        \
    BOOL __fIsSoftStackOverflow = FALSE;                                                    \
    BOOL __fReturnToSOIntolerant = FALSE;                                                   \
    BOOL __fInCooperative = FALSE;                                                          \
    Frame* __pSafeForSOFrame = NULL;                                                        \
    if (__pThread)                                                                          \
    {                                                                                       \
        __fReturnToSOIntolerant = !__pThread->IsSOTolerant();                               \
        __pThread->ResetThreadStateNC(Thread::TSNC_SOIntolerant);                           \
        __fInCooperative = __pThread->PreemptiveGCDisabled();                               \
        __pSafeForSOFrame = __pThread->GetFrame();                                          \
    }                                                                                       \
    __try                                                                                   \
    {                                                                                       \

// We need to catch any hard SO that comes through in order to get our stack back and make sure that we can run our backout code.
// Also can't allow a hard SO to propogate into SO-intolerant code, as we can't tell where it came from and would have to rip the process.
// So install a filter and catch hard SO and rethrow a C++ SO.
#define END_SO_TOLERANT_CODE                                                                 \
    }                                                                                        \
    __except(SOTolerantBoundaryFilter(GetExceptionInformation(), __fReturnToSOIntolerant))   \
    {                                                                                        \
        /* do nothing here.  Get our stack back post-catch and then throw a new exception */ \
        __fRecoverStack = TRUE;                                                              \
        if (::GetExceptionCode() == STATUS_STACK_OVERFLOW ||                                 \
            ::GetExceptionCode() == EXCEPTION_SOFTSO)                                        \
        {                                                                                    \
            if (::GetExceptionCode() == EXCEPTION_SOFTSO)                                    \
            {                                                                                \
                __fIsSoftStackOverflow = TRUE;                                               \
            }                                                                                \
            __fIsStackOverflow = TRUE;                                                       \
            /* If there is a SO_INTOLERANT region above this */                              \
            /* we should have processed it already in SOIntolerantTransitionHandler */       \
            EEPolicy::HandleStackOverflow(__fIsSoftStackOverflow?SOD_SOTolerantTransitorGotSoftSO:SOD_SOTolerantTransitor);                 \
            __pThread->SetSOExceptionInfo(NULL, NULL);                                       \
            /* If this assertion fires, then it means that we have not unwound the frame chain */ \
            _ASSERTE(__pSafeForSOFrame == __pThread->GetFrame());                            \
            __pThread->ClearExceptionStateAfterSO(__pSafeForSOFrame);                        \
        }                                                                                    \
    }                                                                                        \
    if (__fReturnToSOIntolerant)                                                             \
    {                                                                                        \
        __pThread->SetThreadStateNC(Thread::TSNC_SOIntolerant);                              \
    }                                                                                        \
    if (__fRecoverStack)                                                                     \
    {                                                                                        \
        if (!__fIsSoftStackOverflow)                                                         \
        {                                                                                    \
            __pThread->RestoreGuardPage();                                                   \
        }                                                                                    \
        if (__fIsStackOverflow)                                                              \
        {                                                                                    \
            if (!__pThread->PreemptiveGCDisabled())                                          \
            {                                                                                \
                __pThread->DisablePreemptiveGC();                                            \
            }                                                                                \
            /* We have enough stack now.  Start unload */                                    \
            EEPolicy::PerformADUnloadAction(eRudeUnloadAppDomain, TRUE, TRUE);               \
        }                                                                                    \
        if (__pThread->PreemptiveGCDisabled() != __fInCooperative)                           \
        {                                                                                    \
            if (__fInCooperative)                                                            \
            {                                                                                \
                __pThread->DisablePreemptiveGC();                                            \
            }                                                                                \
            else                                                                             \
            {                                                                                \
                __pThread->EnablePreemptiveGC();                                             \
            }                                                                                \
        }                                                                                    \
        COMPlusThrowSO();                                                                    \
    }                                                                                        \
}

#define BEGIN_SO_TOLERANT_CODE_HAS_CATCH(pThread)                                            \
    {                                                                                        \
    Thread *__pThread = pThread;                                                             \
    RestoreSOState __restoreSOState(__pThread);                                              \
    if (__pThread)                                                                           \
    {                                                                                        \
        __pThread->ResetThreadStateNC(Thread::TSNC_SOIntolerant);                            \
    }                                                                                        \

#define END_SO_TOLERANT_CODE_HAS_CATCH                                                       \
    }

#define BEGIN_SO_TOLERANT_CODE_CALLING_HOST(pThread)                                         \
    {                                                                                        \
    Thread *__pThread = pThread;                                                             \
    RestoreSOState __restoreSOState(__pThread);                                              \
    if (__pThread)                                                                           \
    {                                                                                        \
        __pThread->ResetThreadStateNC(Thread::TSNC_SOIntolerant);                            \
    }                                                                                        \

#define END_SO_TOLERANT_CODE_CALLING_HOST                                                    \
    }


#else

// Should have explicitly specified which version we're using
#error No Stack Guard setting provided. Must specify one of \
    TOTALLY_DISBLE_STACK_GUARDS, STACK_GUARDS_DEBUG or STACK_GUARDS_RELEASE

#endif


#endif  // __STACKPROBE_h__
