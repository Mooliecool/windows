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
// File: RsAssembly.cpp
//
//*****************************************************************************
#include "stdafx.h"
#include "primitives.h"
#include "safewrap.h"

#include "check.h"


#ifndef SM_REMOTESESSION
#define SM_REMOTESESSION 0x1000
#endif

#include "corpriv.h"
#include "../../dlls/mscorrc/resource.h"
#include <limits.h>

#include "dacprivate.h"


/* ------------------------------------------------------------------------- *
 * Assembly class
 * ------------------------------------------------------------------------- */
CordbAssembly::CordbAssembly(CordbAppDomain* pAppDomain,
                    LSPTR_ASSEMBLY debuggerAssemblyToken,
                    const WCHAR *szName,
                    BOOL fIsSystemAssembly)

    : CordbBase(pAppDomain->GetProcess(), LsPtrToCookie(debuggerAssemblyToken), enumCordbAssembly),
      m_debuggerAssemblyToken(debuggerAssemblyToken),
      m_pAppDomain(pAppDomain),
      m_fIsSystemAssembly(fIsSystemAssembly),
      m_fIsTrustLevelKnown(false),
      m_fIsFullTrust(false)
{
    // Make a copy of the name.
    if (szName == NULL)
    {
        szName = L"<Unknown>";
    }

    m_szAssemblyName.AssignCopy(szName);
}

/*
    A list of which resources owned by this object are accounted for.

    public:
        CordbAppDomain      *m_pAppDomain; // Assigned w/o addRef(), Deleted in ~CordbAssembly
*/

CordbAssembly::~CordbAssembly()
{
}

HRESULT CordbAssembly::QueryInterface(REFIID id, void **ppInterface)
{
    if (id == IID_ICorDebugAssembly)
        *ppInterface = static_cast<ICorDebugAssembly*>(this);
    else if (id == IID_ICorDebugAssembly2)
        *ppInterface = static_cast<ICorDebugAssembly2*>(this);
    else if (id == IID_IUnknown)
        *ppInterface = static_cast<IUnknown*>( static_cast<ICorDebugAssembly*>(this) );
    else
    {
        *ppInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

// Neutered by AppDomain
void CordbAssembly::Neuter(NeuterTicket ticket)
{
    RSSmartPtr<CordbAssembly> pRef(this);
    {
        CordbBase::Neuter(ticket);
    }
}

/*
 * GetProcess returns the process containing the assembly
 */
HRESULT CordbAssembly::GetProcess(ICorDebugProcess **ppProcess)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppProcess, ICorDebugProcess **);

    return (m_pAppDomain->GetProcess (ppProcess));
}

/*
 * GetAppDomain returns the app domain containing the assembly.
 * Returns null if this is the system assembly
 */
HRESULT CordbAssembly::GetAppDomain(ICorDebugAppDomain **ppAppDomain)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppAppDomain, ICorDebugAppDomain **);

    if (m_fIsSystemAssembly == TRUE)
    {
        *ppAppDomain = NULL;
    }
    else
    {
        _ASSERTE (m_pAppDomain != NULL);

        *ppAppDomain = static_cast<ICorDebugAppDomain *> (m_pAppDomain);
        m_pAppDomain->ExternalAddRef();
    }
    return S_OK;
}

/*
 * EnumerateModules enumerates all modules in the assembly
 */
HRESULT CordbAssembly::EnumerateModules(ICorDebugModuleEnum **ppModules)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    VALIDATE_POINTER_TO_OBJECT(ppModules, ICorDebugModuleEnum **);

    HRESULT hr = S_OK;
    CordbEnumFilter *pModEnum;


    CordbHashTableEnum *e = new (nothrow) CordbHashTableEnum(
        this, NULL,  // ownership
        &m_pAppDomain->m_modules,
        IID_ICorDebugModuleEnum);

    if (e == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    pModEnum = new (nothrow) CordbEnumFilter(GetProcess(), GetProcess()->GetContinueNeuterList());
    if (pModEnum == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    hr = pModEnum->Init (e, this);

    if (SUCCEEDED (hr))
    {
        *ppModules = static_cast<ICorDebugModuleEnum *> (pModEnum);
        pModEnum->ExternalAddRef();
    }
    else
    {
        delete pModEnum;
    }

Error:
    if (e != NULL)
    {
        NeuterTicket ticket(GetProcess());
        e->Neuter(ticket);
        delete e;
    }

    return hr;
}


/*
 * GetCodeBase returns the code base used to load the assembly
 */
HRESULT CordbAssembly::GetCodeBase(ULONG32 cchName,
                    ULONG32 *pcchName,
                    WCHAR szName[])
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(szName, WCHAR, cchName, true, true);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pcchName, ULONG32 *);

    return E_NOTIMPL;
}

/*
 * GetName returns the name of the assembly
 */
HRESULT CordbAssembly::GetName(ULONG32 cchName,
                               ULONG32 *pcchName,
                               WCHAR szName[])
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY_OR_NULL(szName, WCHAR, cchName, true, true);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pcchName, ULONG32 *);

    const WCHAR *szTempName = m_szAssemblyName;

    // In case we didn't get the name (most likely out of memory on ctor).
    if (!szTempName)
    {
        szTempName = L"<unknown>";
    }        

    return CopyOutString(szTempName, cchName, pcchName, szName);
}

HRESULT CordbAssembly::IsFullyTrusted( BOOL *pbFullyTrusted )
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    VALIDATE_POINTER_TO_OBJECT(pbFullyTrusted, BOOL*);

    // Check for cached result 
    if( m_fIsTrustLevelKnown )
    {
        *pbFullyTrusted = m_fIsFullTrust;
        return S_OK;
    }

    *pbFullyTrusted = FALSE;    // just in-case return code isn't checked

    CordbProcess *pProcess = m_pAppDomain->GetProcess();
    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event, DB_IPCE_GET_ASSEMBLY_TRUST, true, m_pAppDomain->GetADToken());
    event.GetAssemblyTrust.debuggerAssemblyToken = m_debuggerAssemblyToken;
    event.GetAssemblyTrust.debuggerAppDomainToken = m_pAppDomain->GetADToken();

    // Note: two-way event here...
    HRESULT hr = pProcess->m_cordb->SendIPCEvent(pProcess, &event, sizeof(DebuggerIPCEvent));
    if (FAILED(hr))
    {
        return hr;      // couldn't send the event
    }

    _ASSERTE(event.type == DB_IPCE_GET_ASSEMBLY_TRUST_RESULT);
    if( FAILED(event.hr) )
    {
        return event.hr;   // LS failed to get trust information for us
    }

    // Cache the result to prevent unnecessary IPC events
    // Once the trust level of an assembly is known, it cannot change.
    m_fIsFullTrust = event.GetAssemblyTrustResult.fIsFullyTrusted;
    m_fIsTrustLevelKnown = TRUE;
    
    *pbFullyTrusted = m_fIsFullTrust;
    return S_OK;        
}

