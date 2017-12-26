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
// Generic Stack Probe Code
// Used to setup stack guards and probes outside the VM tree
//-----------------------------------------------------------------------------
#ifndef __GENERICSTACKPROBE_h__
#define __GENERICSTACKPROBE_h__

#include "staticcontract.h"

#define TOTALLY_DISBLE_STACK_GUARDS

// The types of stack validation we support in holders.
enum HolderStackValidation
{
    HSV_NoValidation,
    HSV_ValidateMinimumStackReq,
    HSV_ValidateNormalStackReq,        
};

// Used to track transitions into the profiler
#define PROFILER_CALL \
        REMOVE_STACK_GUARD

#ifdef TOTALLY_DISBLE_STACK_GUARDS 

#define VALIDATE_BACKOUT_STACK_CONSUMPTION
#define VALIDATE_BACKOUT_STACK_CONSUMPTION_FOR
#define UNSAFE_BEGIN_VALIDATE_BACKOUT_STACK_CONSUMPTION_NO_DISABLE
#define UNSAFE_BEGIN_VALIDATE_BACKOUT_STACK_CONSUMPTION_NO_DISABLE_FOR(numPages)
#define UNSAFE_END_VALIDATE_BACKOUT_STACK_CONSUMPTION_NO_DISABLE
#define VALIDATE_HOLDER_STACK_CONSUMPTION_FOR_TYPE(validationType)
#define BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(ActionOnSO)
#define BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD_FORCE_SO()
#define END_SO_INTOLERANT_CODE
#define RESTORE_SO_TOLERANCE_STATE
#define DISABLE_BACKOUT_STACK_VALIDATION
#define BACKOUT_STACK_VALIDATION_VIOLATION
#define BEGIN_SO_INTOLERANT_CODE_NOPROBE                                                  
#define END_SO_INTOLERANT_CODE_NOPROBE                                                  
#define REMOVE_STACK_GUARD



#else

// For AMD64, the stack size is 4K, same as X86, but the pointer size is 64, so the
// stack tends to grow a lot faster than X86.  IA64 has 8K page, so probe sizes for
// X86 are closer to correct.  
#define ADJUST_PROBE(n)  (n)

// In debug builds, we redefine DEFAULT_ENTRY_PROBE_AMOUNT to a global static
// so that we can tune the entry point probe size at runtime.
#define DEFAULT_ENTRY_PROBE_SIZE 12
#define DEFAULT_ENTRY_PROBE_AMOUNT DEFAULT_ENTRY_PROBE_SIZE

#define BACKOUT_CODE_STACK_LIMIT 4.0
#define HOLDER_CODE_NORMAL_STACK_LIMIT BACKOUT_CODE_STACK_LIMIT
#define HOLDER_CODE_MINIMUM_STACK_LIMIT 0.25

void DontCallDirectlyForceStackOverflow();
void SOBackoutViolation(const char *szFunction, const char *szFile, int lineNum); 
typedef void *EEThreadHandle;
class SOIntolerantTransitionHandler;
extern void (*g_fpCheckForSOInSOIntolerantCode)(EEThreadHandle);
extern void (*g_fpChangeThreadToSOTolerant)(EEThreadHandle, BOOL);
extern BOOL (*g_fpThreadIsSOTolerant)(EEThreadHandle*);
extern BOOL (*g_fpDoProbe)(SOIntolerantTransitionHandler *pTransitionHandler, unsigned int n);
extern void (*g_fpHandleSoftStackOverflow)(BOOL fSkipDebugger);
// Once we enter SO-intolerant code, we can never take a hard SO as we will be 
// in an unknown state. SOIntolerantTransitionHandler is used to detect a hard SO in SO-intolerant
// code and to raise a Fatal Error if one occurs.
class SOIntolerantTransitionHandler
{
  public:

  private:
    BOOL m_fSOTolerantAtEntry;
    BOOL m_exceptionOccured;
    EEThreadHandle m_pThread;
    
  public: 
    SOIntolerantTransitionHandler(EEThreadHandle pThread) 
    : m_pThread(pThread)
    {
        m_exceptionOccured = TRUE;

        if (g_fpThreadIsSOTolerant)
        {
            m_fSOTolerantAtEntry = g_fpThreadIsSOTolerant(&m_pThread);
            g_fpChangeThreadToSOTolerant(m_pThread, FALSE);
        }
        else
        {
            m_fSOTolerantAtEntry = TRUE;
        }
    }

    ~SOIntolerantTransitionHandler()
    {
        // if we take a stack overflow exception in SO intolerant code, then we must
        // rip the process.  We check this by determining if the SP is beyond the calculated
        // limit.   Checking for the guard page being present is too much overhead during
        // exception handling (if you can believe that) and impacts perf.
        if (g_fpCheckForSOInSOIntolerantCode)
        {
            if (m_exceptionOccured)
            {
                g_fpCheckForSOInSOIntolerantCode(m_pThread);
            }
            g_fpChangeThreadToSOTolerant(m_pThread, m_fSOTolerantAtEntry);
        }
    }

    BOOL RetailStackProbeNoThrowNoThread(unsigned int n)
    {
        if (! g_fpDoProbe)
        {
            return TRUE;
        }
        return g_fpDoProbe(this, n);
    }
    
    void SetNoException()
    {
        m_exceptionOccured = FALSE;
    }

    void SetThread(EEThreadHandle thread)
    {
        m_pThread = thread;
    }

    BOOL DidExceptionOccur()
    {
        return m_exceptionOccured;
    }
};


#ifdef STACK_GUARDS_DEBUG

#define STACK_COOKIE_VALUE 0x01234567
#define DISABLED_STACK_COOKIE_VALUE 0xDCDCDCDC

// This allows us to adjust the probe amount at run-time in checked builds
#undef DEFAULT_ENTRY_PROBE_AMOUNT
#define DEFAULT_ENTRY_PROBE_AMOUNT g_EntryPointProbeAmount

class BaseStackGuardGeneric;
class BaseStackGuard;

extern void (*g_fpRestoreCurrentStackGuard)(BOOL fDisabled);
extern BOOL (*g_fp_BaseStackGuard_RequiresNStackPages)(BaseStackGuardGeneric *pGuard, unsigned int n, BOOL fThrowOnSO);
extern void (*g_fp_BaseStackGuard_CheckStack)(BaseStackGuardGeneric *pGuard);
extern BOOL (*g_fpCheckNStackPagesAvailable)(unsigned int n);
extern BOOL  g_ProtectStackPagesInDebugger;
void RestoreSOToleranceState();
void EnsureSOTolerant();

extern BOOL g_EnableBackoutStackValidation;
extern DWORD g_EntryPointProbeAmount;

//-----------------------------------------------------------------------------
// Check if a cookie is still at the given marker
//-----------------------------------------------------------------------------
inline  BOOL IsMarkerOverrun(UINT_PTR *pMarker)
{
    return (*pMarker != STACK_COOKIE_VALUE);
}

// This class is used to place a marker upstack and verify that it was not overrun.  It is
// different from the full blow stack probes in that it does not chain with other probes or
// test for stack overflow.  Its sole purpose is to verify stack consumption.
// It is effectively an implicit probe though, because we are guaranteeing that we have
// enought stack to run and will not take an SO.  So we enter SO-intolerant code when
// we install one of these.

class StackMarkerStack;
struct ClrDebugState;

class BaseStackMarker
{
    friend StackMarkerStack;

    ClrDebugState  *m_pDebugState;  
    BOOL            m_prevWasSOTolerant;   // Were we SO-tolerant when we came in? 
    BOOL            m_fMarkerSet;          // Has the marker been set?
    BOOL            m_fTemporarilyDisabled;// Has the marker been temporarely disabled?
    BOOL            m_fAddedToStack;       // Has this BaseStackMarker been added to the stack of markers for the thread.
    float           m_numPages;
    UINT_PTR       *m_pMarker;    // Pointer to where to put our marker cookie on the stack.
    BaseStackMarker*m_pPrevious;
    BOOL            m_fProtectedStackPage;
    BOOL            m_fAllowDisabling;

    BaseStackMarker() {};   // no default construction allowed

    // These should only be called by the ClrDebugState.
    void RareDisableMarker();
    void RareReEnableMarker();        

  public:
    BaseStackMarker(float numPages, BOOL fAllowDisabling); 

    // we have this so that the check of the global can be inlined
    // and we don't make the call to CheckMarker unless we need to.
    void CheckForBackoutViolation();

    void SetMarker(float numPages);
    void CheckMarker();
    
    void ProtectMarkerPageInDebugger();
    void UndoPageProtectionInDebugger();
    
};

class AutoCleanupStackMarker : public BaseStackMarker
{
  public:
    AutoCleanupStackMarker(float numPages) : 
        BaseStackMarker(numPages, TRUE) {}
    ~AutoCleanupStackMarker()
    {
        CheckForBackoutViolation();
    }
};

class StackMarkerStack
{
public:
    // Since this is used from the ClrDebugState which can't have a default constructor,
    // we need to provide an Init method to intialize the instance instead of having a constructor.
    void Init() 
    {
        m_pTopStackMarker = NULL;
        m_fDisabled = FALSE;
    }
            
    void PushStackMarker(BaseStackMarker *pStackMarker);
    BaseStackMarker *PopStackMarker();
    
    BOOL IsEmpty()
    {
        return (m_pTopStackMarker == NULL);
    } 
    BOOL IsDisabled()
    {
        return m_fDisabled;
    }

    void RareDisableStackMarkers();
    void RareReEnableStackMarkers();

private:
    BaseStackMarker     *m_pTopStackMarker;     // The top of the stack of stack markers for the current thread.
    BOOL                m_fDisabled;
};

#define VALIDATE_BACKOUT_STACK_CONSUMPTION \
    AutoCleanupStackMarker __stackMarker(BACKOUT_CODE_STACK_LIMIT);

#define VALIDATE_BACKOUT_STACK_CONSUMPTION_FOR(numPages) \
    AutoCleanupStackMarker __stackMarker(numPages);

#define UNSAFE_BEGIN_VALIDATE_BACKOUT_STACK_CONSUMPTION_NO_DISABLE \
    BaseStackMarker __stackMarkerNoDisable(BACKOUT_CODE_STACK_LIMIT, FALSE);

#define UNSAFE_BEGIN_VALIDATE_BACKOUT_STACK_CONSUMPTION_NO_DISABLE_FOR(numPages) \
    BaseStackMarker __stackMarkerNoDisable(numPages, FALSE);

#define UNSAFE_END_VALIDATE_BACKOUT_STACK_CONSUMPTION_NO_DISABLE \
        __stackMarkerNoDisable.CheckForBackoutViolation(); 

#define VALIDATE_HOLDER_STACK_CONSUMPTION_FOR_TYPE(validationType) \
    _ASSERTE(validationType != HSV_NoValidation);                  \
    AutoCleanupStackMarker __stackMarker(                          \
        validationType == HSV_ValidateNormalStackReq ? HOLDER_CODE_NORMAL_STACK_LIMIT : HOLDER_CODE_MINIMUM_STACK_LIMIT);

class AutoCleanupDisableBackoutStackValidation
{
  public:
    AutoCleanupDisableBackoutStackValidation();
    ~AutoCleanupDisableBackoutStackValidation();
    
private:
    BOOL m_fAlreadyDisabled;

};

// This macros disables the backout stack validation in the current scope. It should 
// only be used in very rare situations. If you think you might have such a situation, 
// please talk to the stack overflow devs before using it.
#define DISABLE_BACKOUT_STACK_VALIDATION \
    AutoCleanupDisableBackoutStackValidation __disableBacoutStackValidation;

struct ClrDebugState;

// In debug mode, we want to do a little more work on this transition to note the transition in the thread.
class DebugSOIntolerantTransitionHandler : public SOIntolerantTransitionHandler
{
    BOOL m_prevSOTolerantState;
    ClrDebugState* m_clrDebugState;

  public: 
    DebugSOIntolerantTransitionHandler(EEThreadHandle pThread); 
    ~DebugSOIntolerantTransitionHandler();
};

// This is the base class structure for our probe infrastructure.  We declare it here
// so that we can properly declare instances outside of the VM tree.  But we only do the
// probes when we have a managed thread.
class BaseStackGuardGeneric
{
public:
    enum
    {
        cPartialInit,       // Not yet intialized
        cInit,              // Initialized and installed
        cUnwound,           // Unwound on a normal path (used for debugging)
        cEHUnwound          // Unwound on an exception path (used for debugging)
    } m_eInitialized;
        
    // *** Following fields must not move.  The fault injection framework depends on them.
    BaseStackGuard *m_pPrevGuard; // Previous guard for this thread.
    UINT_PTR       *m_pMarker;    // Pointer to where to put our marker cookie on the stack.
    unsigned int    m_numPages;        // space needed, specified in number of pages
    BOOL            m_isBoundaryGuard;  // used to mark when we've left the EE
    BOOL            m_fDisabled;       // Used to enable/disable stack guard


    // *** End of fault injection-dependent fields

    // The following fields are really here to provide us with some nice debugging information.
    const char     *m_szFunction;
    const char     *m_szFile;
    unsigned int    m_lineNum;
    const char     *m_szNextFunction;       // Name of the probe that came after us.
    const char     *m_szNextFile;
    unsigned int    m_nextLineNum;
    DWORD           m_UniqueId;
    unsigned int    m_depth;                // How deep is this guard in the list of guards for this thread?
    BOOL            m_fProtectedStackPage;  // TRUE if we protected a stack page with PAGE_NOACCESS.
    BOOL            m_fEHInProgress;        // Is an EH in progress?  This is cleared on a catch.
    BOOL            m_exceptionOccured;     // Did an exception occur through this probe?

protected:
    BaseStackGuardGeneric()
    {
    }

public:
    BaseStackGuardGeneric(const char *szFunction, const char *szFile, unsigned int lineNum) :
        m_pPrevGuard(NULL), m_pMarker(NULL), 
        m_szFunction(szFunction), m_szFile(szFile), m_lineNum(lineNum),
        m_szNextFunction(NULL), m_szNextFile(NULL), m_nextLineNum(0),
        m_fProtectedStackPage(FALSE), m_UniqueId(-1), m_numPages(0), 
        m_eInitialized(cPartialInit), m_fDisabled(FALSE),
        m_isBoundaryGuard(FALSE),
        m_fEHInProgress(FALSE),     
        m_exceptionOccured(FALSE)
    { 
        STATIC_CONTRACT_LEAF;
    }

    BOOL RequiresNStackPages(unsigned int n, BOOL fThrowOnSO = TRUE)
    {
        if (g_fp_BaseStackGuard_RequiresNStackPages == NULL)
        {
            return TRUE;
        }
        return g_fp_BaseStackGuard_RequiresNStackPages(this, n, fThrowOnSO);
    }

    void CheckStack()
    {
        if (m_eInitialized == cInit)
        {
            g_fp_BaseStackGuard_CheckStack(this);
        }
    }

    void SetNoException()
    {
        m_exceptionOccured = FALSE;
    }

    BOOL DidExceptionOccur()
    {
        return m_exceptionOccured;
    }

    BOOL Enabled()
    {
        return !m_fDisabled;
    }

    void DisableGuard()
    {
        // As long as we don't have threads mucking with other thread's stack
        // guards, we don't need to synchronize this.
        m_fDisabled = TRUE;
    }

    void EnableGuard()
    {
        // As long as we don't have threads mucking with other thread's stack
        // guards, we don't need to synchronize this.
        m_fDisabled = FALSE;
    }

    
};

class StackGuardDisabler
{
    BOOL m_fDisabledGuard;

public:
    StackGuardDisabler();
    ~StackGuardDisabler();
    void NeverRestoreGuard();

};



// Derived version, add a dtor that automatically calls Check_Stack, move convenient, but can't use with SEH.
class AutoCleanupStackGuardGeneric : public BaseStackGuardGeneric
{
protected:
    AutoCleanupStackGuardGeneric()
    {
    }
    
public:
    AutoCleanupStackGuardGeneric(const char *szFunction, const char *szFile, unsigned int lineNum) :
        BaseStackGuardGeneric(szFunction, szFile, lineNum)
    { 
        STATIC_CONTRACT_LEAF;
    }

    ~AutoCleanupStackGuardGeneric()
    { 
        STATIC_CONTRACT_WRAPPER;
        CheckStack(); 
    }
};


// Used to remove stack guard... (kind of like a poor man's BEGIN_SO_TOLERANT
#define REMOVE_STACK_GUARD \
        StackGuardDisabler __guardDisable;

// Used to transition into intolerant code when handling a SO
#define BEGIN_SO_INTOLERANT_CODE_NOPROBE                                                  \
    {                                                                                     \
        DebugSOIntolerantTransitionHandler __soIntolerantTransitionHandler(NULL);         \
        /* work around unreachable code warning */                                        \
        if (true)                                                                         \
        {                                                                                 \
            DEBUG_ASSURE_NO_RETURN_BEGIN_SO_INTOLERANT;                                   \

#define END_SO_INTOLERANT_CODE_NOPROBE                              \
            ;                                                       \
            DEBUG_ASSURE_NO_RETURN_END_SO_INTOLERANT;               \
        }                                                           \
        __soIntolerantTransitionHandler.SetNoException();           \
    }                                                               \
            


#define BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(ActionOnSO)                        \
    {                                                                                       \
        AutoCleanupStackGuardGeneric stack_guard_XXX(__FUNCTION__, __FILE__, __LINE__);         \
        if (! stack_guard_XXX.RequiresNStackPages(ADJUST_PROBE(g_EntryPointProbeAmount), FALSE)) \
        {                                                                                   \
            ActionOnSO;                                                                     \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            DebugSOIntolerantTransitionHandler __soIntolerantTransitionHandler(NULL);       \
            ANNOTATION_SO_PROBE_BEGIN(DEFAULT_ENTRY_PROBE_AMOUNT);                          \
            if (true)                                                                       \
            {                                                                               \
                DEBUG_ASSURE_NO_RETURN_BEGIN_SO_INTOLERANT;                                 \


#define END_SO_INTOLERANT_CODE                                                              \
                ;                                                                           \
                DEBUG_ASSURE_NO_RETURN_END_SO_INTOLERANT;                                   \
            }                                                                               \
            ANNOTATION_SO_PROBE_END;                                                        \
            __soIntolerantTransitionHandler.SetNoException();                               \
            stack_guard_XXX.SetNoException();                                               \
        }                                                                                   \
    }                                                                                       \


#define BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD_FORCE_SO()                           \
    EnsureSOTolerant();                                                                     \
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(DontCallDirectlyForceStackOverflow());   \
    

// Restores the SO-tolerance state and the marker for the current guard if any
#define RESTORE_SO_TOLERANCE_STATE \
    RestoreSOToleranceState();

#elif defined(STACK_GUARDS_RELEASE)

#define VALIDATE_BACKOUT_STACK_CONSUMPTION
#define VALIDATE_BACKOUT_STACK_CONSUMPTION_FOR
#define UNSAFE_BEGIN_VALIDATE_BACKOUT_STACK_CONSUMPTION_NO_DISABLE
#define UNSAFE_BEGIN_VALIDATE_BACKOUT_STACK_CONSUMPTION_NO_DISABLE_FOR(numPages)
#define UNSAFE_END_VALIDATE_BACKOUT_STACK_CONSUMPTION_NO_DISABLE
#define VALIDATE_HOLDER_STACK_CONSUMPTION_FOR_TYPE(validationType)
#define RESTORE_SO_TOLERANCE_STATE
#define DISABLE_BACKOUT_STACK_VALIDATION
#define BACKOUT_STACK_VALIDATION_VIOLATION
#define BEGIN_SO_INTOLERANT_CODE_NOPROBE                                                  
#define END_SO_INTOLERANT_CODE_NOPROBE                                                  
#define REMOVE_STACK_GUARD

#define BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(ActionOnSO)                        \
    {                                                                                       \
	    SOIntolerantTransitionHandler __soIntolerantTransitionHandler(NULL);                \
	    if (! __soIntolerantTransitionHandler.RetailStackProbeNoThrowNoThread(ADJUST_PROBE(DEFAULT_ENTRY_PROBE_AMOUNT)))        \
    {                                                                                       \
	        __soIntolerantTransitionHandler.SetNoException();                               \
        ActionOnSO;                                                                         \
    }                                                                                       \
        else if (true)                                                                      \
        {                                                                                   \
            DEBUG_ASSURE_NO_RETURN_BEGIN_SO_INTOLERANT;                                     \

#define BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD_FORCE_SO()                           \
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(DontCallDirectlyForceStackOverflow());   \

#define END_SO_INTOLERANT_CODE                                                              \
            DEBUG_ASSURE_NO_RETURN_END_SO_INTOLERANT;                                       \
	        __soIntolerantTransitionHandler.SetNoException();                               \
		} 																					\
    } \

#endif // STACK_GUARDS_RELEASE

#endif //TOTALLY_DISBLE_STACK_GUARDS 

#endif  // __GENERICSTACKPROBE_h__
