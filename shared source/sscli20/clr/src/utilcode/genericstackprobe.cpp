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
//  genericstackprobe.cpp
//
//  This contains code for generic SO stack probes outside the VM, where we don't have a thread object
//
//*****************************************************************************

#include "stdafx.h"                     // Precompiled header key.
#include "utilcode.h"
#include "genericstackprobe.h"
#include "log.h"


#ifndef TOTALLY_DISBLE_STACK_GUARDS
void (*g_fpCheckForSOInSOIntolerantCode)(EEThreadHandle);
void (*g_fpChangeThreadToSOTolerant)(EEThreadHandle, BOOL);
BOOL (*g_fpThreadIsSOTolerant)(EEThreadHandle *);
BOOL (*g_fpDoProbe)(SOIntolerantTransitionHandler *pTransitionHandler, unsigned int n);
void (*g_fpHandleSoftStackOverflow)(BOOL fSkipDebugger);
// This function is used for NO_THROW probes that have no error return path.  In this
// case, we'll just force a stack overflow exception.  Do not call it directly - use
// one of the FORCE_SO macros.
void DontCallDirectlyForceStackOverflow()
{

    UINT_PTR *sp = NULL;
    // we don't have access to GetCurrentSP from here, so just get an approximation
    sp = (UINT_PTR *)&sp;
    while (TRUE)
    {
        sp -= (OS_PAGE_SIZE / sizeof(UINT_PTR));
        *sp = NULL;
    }

}

#endif

#ifdef STACK_GUARDS_DEBUG

// If this is TRUE, we'll make the stack page that we put our stack marker in PAGE_NOACCESS so that you get an AV
// as soon as you blow a stack guard.
BOOL  g_ProtectStackPagesInDebugger = FALSE;

// This is the smallest size backout probe for which we will try to do a virtual protect for debugging. 
// If this number is too small, the 1 page ganularity of VirtualProtect becomes a problem. This number 
// should be less than or equal to the default backout probe size.
#define MINIMUM_PAGES_FOR_DEBUGGER_PROTECTION 4.0

void (*g_fpRestoreCurrentStackGuard)(BOOL fDisabled) = 0;
BOOL g_EnableBackoutStackValidation = FALSE;
BOOL (*g_fpShouldValidateSOToleranceOnThisThread)() = 0;
BOOL (*g_fp_BaseStackGuard_RequiresNStackPages)(BaseStackGuardGeneric *pGuard, unsigned int n, BOOL fThrowOnSO) = NULL;
void (*g_fp_BaseStackGuard_CheckStack)(BaseStackGuardGeneric *pGuard) = NULL;
BOOL (*g_fpCheckNStackPagesAvailable)(unsigned int n) = NULL;

// Always initialize g_EntryPointProbeAmount to a valid value as there could be a race where a 
// function probes with g_EntryPointProbeAmount's value before it is initialized in InitStackProbes.
DWORD g_EntryPointProbeAmount = DEFAULT_ENTRY_PROBE_SIZE;

//
//
// 

void RestoreSOToleranceState()
{
    if (!g_fpRestoreCurrentStackGuard)
    {
        // If g_fpUnwindGuardChainTo has not been set, then we haven't called InitStackProbes
        // and we aren't probing, so bail.
        return;
    }

    // Reset the SO-tolerance state and restore the current guard
    g_fpRestoreCurrentStackGuard(FALSE);
}

//
// EnsureSOTolerant ASSERTS if we are not in an SO-tolerant mode
//
void EnsureSOTolerant()
{
    ClrDebugState *pClrDebugState = GetClrDebugState();
    _ASSERTE(! pClrDebugState || pClrDebugState->IsSOTolerant());
}

DebugSOIntolerantTransitionHandler::DebugSOIntolerantTransitionHandler(EEThreadHandle thread) 
    : SOIntolerantTransitionHandler(thread)
{
    m_clrDebugState = GetClrDebugState();
    if (m_clrDebugState)
    {
        m_prevSOTolerantState = m_clrDebugState->BeginSOIntolerant();
    }
}

DebugSOIntolerantTransitionHandler::~DebugSOIntolerantTransitionHandler()
{
    if (m_clrDebugState)
    {
        m_clrDebugState->SetSOTolerance(m_prevSOTolerantState);
    }
}

// This is effectively an implicit probe, because we are guaranteeing that we have
// enought stack to run and will not take an SO.  So we enter SO-intolerant code when
// we install one of these.
BaseStackMarker::BaseStackMarker(float numPages, BOOL fAllowDisabling) 
        : m_prevWasSOTolerant(FALSE), m_pDebugState(CheckClrDebugState()), m_fMarkerSet(FALSE) 
        , m_fTemporarilyDisabled(FALSE), m_fAddedToStack(FALSE), m_pPrevious(NULL)
        , m_numPages(0.0), m_pMarker(NULL)
        , m_fProtectedStackPage(FALSE), m_fAllowDisabling(fAllowDisabling)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;

    // If backout stack validation isn't enabled then we are done.
    if (!g_EnableBackoutStackValidation)
    {
        return;
    }
    
    // If we can't talk to other markers then the markers could get in each others way
    if (!m_pDebugState)
    {
        return;
    }    

    // Allow only the lowest marker to be active at any one time. Yes, this means that
    // the stack will only ever have one element in it. However having multiple markers
    // is problematic for debugging and conflicts with the VirtualProtect option. It
    // adds little value, in that small backout checks stop happening in exception
    // codepaths, but these get plenty of coverage in success cases and the lowest
    // placed marked is the one that could actually indicate a stack overflow.
    if (!m_pDebugState->m_StackMarkerStack.IsEmpty())
    {
        return;
    }        

    // Switch the SO tolerance mode
    m_prevWasSOTolerant = m_pDebugState->SetSOTolerance(FALSE);
    
    // If we have less then numPages left before the end of the stack then there is
    // no point in adding a marker since we will take an SO anyway if we use too much
    // stack. Putting the marker is actually very bad since it artificially forces an
    // SO in cases where it wouldn't normally occur if we use less than num pages of stack.
    if (g_fpCheckNStackPagesAvailable && 
        !g_fpCheckNStackPagesAvailable(numPages < 1 ? 1 : (unsigned int)numPages))
    {
        return;
    }       

    if (m_fAllowDisabling) 
    {
        // Push ourselves on to the stack of stack markers on the CLR debug state.
        m_pDebugState->m_StackMarkerStack.PushStackMarker(this);
        m_fAddedToStack = TRUE;
    }

    // Set the actual stack guard marker if we have enough stack to do so.
    SetMarker(numPages);
    
    if (m_fMarkerSet && m_fAllowDisabling)
    {
        ProtectMarkerPageInDebugger();    
    }
}

// we have this so that the check of the global can be inlined
// and we don't make the call to CheckMarker unless we need to.
void BaseStackMarker::CheckForBackoutViolation()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;

    // The marker should always be re-enabled at this point.
    CONSISTENCY_CHECK_MSG(!m_fTemporarilyDisabled, "The stack guard was disabled but not properly re-enabled. This is a bug somewhere in the code called after this marker has been set up.");

    if (!m_pDebugState || m_fTemporarilyDisabled)
    {
        return;
    }    

    // Reset the SO tolerance of the thread.
    m_pDebugState->SetSOTolerance(m_prevWasSOTolerant);
    
    if (m_fAddedToStack)
    {
        // Pop ourselves off of the stack of stack markers on the CLR debug state.
        CONSISTENCY_CHECK(m_pDebugState != NULL);       
        BaseStackMarker *pPopResult = m_pDebugState->m_StackMarkerStack.PopStackMarker();
        
        CONSISTENCY_CHECK_MSG(pPopResult == this, "The marker we pop off the stack should always be the current marker.");
        CONSISTENCY_CHECK_MSG(m_pPrevious == NULL, "PopStackMarker should reset the current marker's m_pPrevious field to NULL.");
    }
    // Not cancellable markers should only be checked when no cancellable markers are present.    
    if (!m_fAllowDisabling && !(m_pDebugState->m_StackMarkerStack.IsEmpty())) 
    {
        return;
    }

    if (m_fProtectedStackPage) 
    {
        UndoPageProtectionInDebugger();
    }
        
    if (m_fMarkerSet)
    {
        // Check to see if we overwrote the stack guard marker.        
        CheckMarker();
    }
}

void BaseStackMarker::SetMarker(float numPages)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;
    
    m_numPages = numPages;

    // Use the address of the argument to get the current stack pointer. Note that this
    // won't be the exact SP; however it will be close enough.
    LPVOID pStack = &numPages;

    UINT_PTR *pMarker = (UINT_PTR*)pStack  - (int)(OS_PAGE_SIZE / sizeof(UINT_PTR) * m_numPages);
    
    // We might not have committed our stack yet, so allocate the number of pages
    // we need so that they will be commited and we won't AV when we try to set the mark.
    _alloca( (int)(OS_PAGE_SIZE * m_numPages) );
    m_pMarker = pMarker;
    *m_pMarker = STACK_COOKIE_VALUE;

    m_fMarkerSet = TRUE;

}

void BaseStackMarker::RareDisableMarker()
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;
        
    if (m_fProtectedStackPage) 
    {
        UndoPageProtectionInDebugger();
    }

    m_fTemporarilyDisabled = TRUE;
    
    if (m_fMarkerSet) 
    {
        *m_pMarker = DISABLED_STACK_COOKIE_VALUE;
    }
}

void BaseStackMarker::RareReEnableMarker()
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;
    
    m_fTemporarilyDisabled = FALSE;

    if (m_fMarkerSet) {    
        *m_pMarker = STACK_COOKIE_VALUE;
    }

    if (m_fProtectedStackPage) 
    {
        ProtectMarkerPageInDebugger();
    }
}

//-----------------------------------------------------------------------------
// Protect the page where we put the marker if a debugger is attached. That way, you get an AV right away
// when you blow the guard when running under a debugger.
//-----------------------------------------------------------------------------
void BaseStackMarker::ProtectMarkerPageInDebugger()
{
    WRAPPER_CONTRACT;
    DEBUG_ONLY_FUNCTION;

    if (!g_ProtectStackPagesInDebugger)
    {
        return;
    }
    
    if (m_numPages < MINIMUM_PAGES_FOR_DEBUGGER_PROTECTION) 
    {
        return;
    }

    DWORD flOldProtect;

    LOG((LF_EH, LL_INFO100000, "BSM::PMP: m_pMarker 0x%p, value 0x%p\n", m_pMarker, *m_pMarker));

    // We cannot call into host for VirtualProtect. EEVirtualProtect will try to restore previous
    // guard, but the location has been marked with PAGE_NOACCESS.
#undef VirtualProtect
    BOOL fSuccess = ::VirtualProtect(m_pMarker, 1, PAGE_NOACCESS, &flOldProtect);
    _ASSERTE(fSuccess);


#define VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect) \
        Dont_Use_VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect)

    m_fProtectedStackPage = fSuccess;
}

//-----------------------------------------------------------------------------
// Remove page protection installed for this probe
//-----------------------------------------------------------------------------
void BaseStackMarker::UndoPageProtectionInDebugger()
{
    WRAPPER_CONTRACT;
    DEBUG_ONLY_FUNCTION;

    _ASSERTE(m_fProtectedStackPage);
    _ASSERTE(!m_fTemporarilyDisabled);

    DWORD flOldProtect;
    // EEVirtualProtect installs a BoundaryStackGuard.  To avoid recursion, we call
    // into OS for VirtualProtect instead.
#undef VirtualProtect
    BOOL fSuccess = ::VirtualProtect(m_pMarker, 1, PAGE_READWRITE, &flOldProtect);
    _ASSERTE(fSuccess);

    LOG((LF_EH, LL_INFO100000, "BSM::UMP m_pMarker 0x%p\n", m_pMarker));
    
#define VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect) \
        Dont_Use_VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect)
}

void BaseStackMarker::CheckMarker()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;
    
    if ( IsMarkerOverrun(m_pMarker)
    )
    {
        SOBackoutViolation(__FUNCTION__, __FILE__, __LINE__);
    }
}

AutoCleanupDisableBackoutStackValidation::AutoCleanupDisableBackoutStackValidation()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;

    m_fAlreadyDisabled = GetClrDebugState()->m_StackMarkerStack.IsDisabled();    
    if (!m_fAlreadyDisabled) 
    {
        GetClrDebugState()->m_StackMarkerStack.RareDisableStackMarkers();    
    }
}

AutoCleanupDisableBackoutStackValidation::~AutoCleanupDisableBackoutStackValidation()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;

    if (!m_fAlreadyDisabled) 
    {
        GetClrDebugState()->m_StackMarkerStack.RareReEnableStackMarkers();
    }
}

inline void StackMarkerStack::PushStackMarker(BaseStackMarker *pStackMarker)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;

    pStackMarker->m_pPrevious = m_pTopStackMarker;       
    m_pTopStackMarker = pStackMarker;
}

BaseStackMarker *StackMarkerStack::PopStackMarker()
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;

    BaseStackMarker *pOldTop = m_pTopStackMarker;
    m_pTopStackMarker = pOldTop->m_pPrevious;
    pOldTop->m_pPrevious = NULL;
    return pOldTop;
}

void StackMarkerStack::RareDisableStackMarkers()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;

    // Walk up the stack of markers and disable them all.
    BaseStackMarker *pCurrentStackMarker = m_pTopStackMarker;
    while (pCurrentStackMarker)
    {
        pCurrentStackMarker->RareDisableMarker();
        pCurrentStackMarker = pCurrentStackMarker->m_pPrevious;
    }
    m_fDisabled = TRUE;
}

void StackMarkerStack::RareReEnableStackMarkers()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;

    // Walk up the stack of markers and re-enable them all.
    BaseStackMarker *pCurrentStackMarker = m_pTopStackMarker;
    while (pCurrentStackMarker)
    {
        pCurrentStackMarker->RareReEnableMarker();
        pCurrentStackMarker = pCurrentStackMarker->m_pPrevious;
    }
    m_fDisabled = FALSE;
}


#endif
