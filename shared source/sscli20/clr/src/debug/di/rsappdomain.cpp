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
// File: RsAppDomain.cpp
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


const WCHAR * CordbAppDomain::s_szBadAppDomainName = L"<UnknownAppDomain>";

/* ------------------------------------------------------------------------- *
 * AppDomain class methods
 * ------------------------------------------------------------------------- */
CordbAppDomain::CordbAppDomain(CordbProcess* pProcess,
                               LSPTR_APPDOMAIN pAppDomainToken,
                               ULONG id
                               )


    : CordbBase(pProcess, LsPtrToCookie(pAppDomainToken), enumCordbAppDomain),
    m_fAttached(FALSE),
    m_fHasAtLeastOneThreadInsideIt(false),
    m_AppDomainId(id),
    m_assemblies(9),
    m_modules(17),
    m_breakpoints(17),
    m_sharedtypes(3),
    m_szAppDomainName(NULL),
    m_nameIsValid(false),
    m_synchronizedAD(false),
    m_pLSAppDomain(pAppDomainToken)
{
    // Use DAC to get the friendly name.
    RefreshName();

    LOG((LF_CORDB,LL_INFO10000, "CAD::CAD: this:0x%x (void*)this:0x%x"
        "<%S>\n", this, (void *)this, GetFriendlyName()));

}

/*
    A list of which resources owened by this object are accounted for.

    RESOLVED:
        // AddRef() in CordbHashTable::GetBase for a special InProc case
        // AddRef() on the DB_IPCE_CREATE_APP_DOMAIN event from the LS
        // Release()ed in Neuter
        CordbProcess        *m_pProcess;

        WCHAR               *m_szAppDomainName; // Deleted in ~CordbAppDomain

        // Cleaned up in Neuter
        CordbHashTable      m_assemblies;
        CordbHashTable      m_sharedtypes;
        CordbHashTable      m_modules;
        CordbHashTable      m_breakpoints; // Disconnect()ed in ~CordbAppDomain

    private:
*/

CordbAppDomain::~CordbAppDomain()
{

    // We expect to be Neutered before being released. Neutering will release our process ref
    _ASSERTE(IsNeutered());

    if (m_szAppDomainName != s_szBadAppDomainName)
    {
        delete [] m_szAppDomainName;
    }

    //
    // Disconnect any active breakpoints
    //
    CordbBreakpoint* entry;
    HASHFIND find;

    for (entry =  m_breakpoints.FindFirst(&find);
         entry != NULL;
         entry =  m_breakpoints.FindNext(&find))
    {
        entry->Disconnect();
    }

}


// Neutered by process. Once we're neutered, we lose our backpointer to the CordbProcess object, and
// thus can't do things like call GetProcess() or Continue().
void CordbAppDomain::Neuter(NeuterTicket ticket)
{
    // This check prevents us from calling this twice and underflowing the inter ref count!
    if (IsNeutered())
    {
        return;
    }


    //
    // Sweep neuter lists.
    //
    m_TypeNeuterList.NeuterAndClear(ticket);
    m_SweepableNeuterList.NeuterAndClear(ticket);


    NeuterAndClearHashtable(&m_assemblies, ticket);
    NeuterAndClearHashtable(&m_modules, ticket);
    NeuterAndClearHashtable(&m_sharedtypes, ticket);
    NeuterAndClearHashtable(&m_breakpoints, ticket);

    CordbBase::Neuter(ticket);
}


HRESULT CordbAppDomain::QueryInterface(REFIID id, void **ppInterface)
{
    if (id == IID_ICorDebugAppDomain)
    {
        *ppInterface = (ICorDebugAppDomain*)this;
    }
    else if (id == IID_ICorDebugAppDomain2)
    {
        *ppInterface = (ICorDebugAppDomain2*)this;
    }
    else if (id == IID_ICorDebugController)
        *ppInterface = (ICorDebugController*)(ICorDebugAppDomain*)this;
    else if (id == IID_IUnknown)
        *ppInterface = (IUnknown*)(ICorDebugAppDomain*)this;
    else
    {
        *ppInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

// Get the friendly name.
const WCHAR * CordbAppDomain::GetFriendlyName()
{
    // It's the caller's responsibility to call RefreshName()
    // to set m_szAppDomainName before calling this.
    _ASSERTE(m_szAppDomainName != NULL);

    return m_szAppDomainName;
}

// Use DAC to get the AppDomain's friendly name.
// Note this value is unbounded.
void CordbAppDomain::RefreshName()
{
    if (m_nameIsValid)
    {
        // If we already have a valid name, we're done.
        _ASSERTE(m_szAppDomainName != NULL);
        return;
    }

    if (m_szAppDomainName != s_szBadAppDomainName)
    {
        delete [] m_szAppDomainName;
    }
    m_szAppDomainName = NULL;

    // Use DAC to get the name.

    // ROTORTODO: Need an plan to get appdomain name using alternative mechanism
    goto Label_Done;

Label_Done:
    // If at any point we fail, then use a standard "unknown" name
    if (!m_nameIsValid)
    {
        // Delete any memory we may have allocated, and set us to a predefined literal.
        _ASSERTE(m_szAppDomainName != s_szBadAppDomainName);
        delete [] m_szAppDomainName;
        m_szAppDomainName = (WCHAR*) s_szBadAppDomainName;
    }
    else
    {
        _ASSERTE(m_szAppDomainName != s_szBadAppDomainName);
        _ASSERTE(m_szAppDomainName != NULL);
    }
}


HRESULT CordbAppDomain::Stop(DWORD dwTimeout)
{
    FAIL_IF_NEUTERED(this);
    PUBLIC_API_ENTRY(this);
    return (m_pProcess->StopInternal(dwTimeout, this->GetADToken()));
}

HRESULT CordbAppDomain::Continue(BOOL fIsOutOfBand)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    return m_pProcess->ContinueInternal(fIsOutOfBand);
}

HRESULT CordbAppDomain::IsRunning(BOOL *pbRunning)
{
    PUBLIC_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pbRunning, BOOL *);
    FAIL_IF_NEUTERED(this);

    *pbRunning = !m_pProcess->GetSynchronized();

    return S_OK;
}

HRESULT CordbAppDomain::HasQueuedCallbacks(ICorDebugThread *pThread, BOOL *pbQueued)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pThread,ICorDebugThread *);
    VALIDATE_POINTER_TO_OBJECT(pbQueued,BOOL *);

    return m_pProcess->HasQueuedCallbacks (pThread, pbQueued);
}

HRESULT CordbAppDomain::EnumerateThreads(ICorDebugThreadEnum **ppThreads)
{
    //
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppThreads,ICorDebugThreadEnum **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;
    CordbEnumFilter *pThreadEnum;

    CORDBRequireProcessSynchronized(this->m_pProcess, this);

    CordbHashTableEnum *e = new (nothrow) CordbHashTableEnum(
            this, NULL,  // ownership
            &m_pProcess->m_userThreads,
            IID_ICorDebugThreadEnum);

    if (e == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    pThreadEnum = new (nothrow) CordbEnumFilter(GetProcess(), GetProcess()->GetContinueNeuterList());
    if (pThreadEnum == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    hr = pThreadEnum->Init (e, this);

    if (SUCCEEDED (hr))
    {
        *ppThreads = static_cast<ICorDebugThreadEnum *> (pThreadEnum);
        pThreadEnum->ExternalAddRef();
    }
    else
        delete pThreadEnum;

Error:
    if (e != NULL)
    {
        NeuterTicket ticket(GetProcess());
        e->Neuter(ticket);
        delete e;
    }
    return hr;
}


HRESULT CordbAppDomain::SetAllThreadsDebugState(CorDebugThreadState state,
                                   ICorDebugThread *pExceptThisThread)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    return m_pProcess->SetAllThreadsDebugState(state, pExceptThisThread);
}

HRESULT CordbAppDomain::DetachWorker()
{
    LOG((LF_CORDB, LL_INFO1000, "CAD::DetachWorker - beginning\n"));
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;

    if (m_fAttached)
    {
        _ASSERTE(!FAILED(m_pProcess->IsReadyForDetach()));

        // Remember that we're no longer attached to this AD.
        m_fAttached = FALSE;

        // Tell the Left Side that we're no longer attached to this AD.
        DebuggerIPCEvent *event = (DebuggerIPCEvent*) _alloca(CorDBIPC_BUFFER_SIZE);

        m_pProcess->InitIPCEvent(event,
                                 DB_IPCE_DETACH_FROM_APP_DOMAIN,
                                 false,
                                 this->GetADToken());

        event->AppDomainData.id = m_AppDomainId;

        hr = m_pProcess->m_cordb->SendIPCEvent(this->m_pProcess,
                                               event,
                                               CorDBIPC_BUFFER_SIZE);
        hr = WORST_HR(hr, event->hr);

        LOG((LF_CORDB, LL_INFO1000, "[%x] CAD::DetachWorker: pProcess=%x sent.\n",
             GetCurrentThreadId(), this));
    }

    LOG((LF_CORDB, LL_INFO10000, "CP::DetachWorker - returning w/ hr=0x%x\n", hr));
    return hr;
}

HRESULT CordbAppDomain::Detach()
{
    PUBLIC_REENTRANT_API_ENTRY(this); // may be called from IMDA::Detach
    FAIL_IF_NEUTERED(this);

    return E_NOTIMPL;
}

HRESULT CordbAppDomain::Terminate(unsigned int exitCode)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    return E_NOTIMPL;
}

void CordbAppDomain::AddToTypeList(CordbBase *pObject)
{
    INTERNAL_API_ENTRY(this);
    _ASSERTE(pObject != NULL);
    this->m_TypeNeuterList.Add(pObject);
}


HRESULT CordbAppDomain::CanCommitChanges(
    ULONG cSnapshots,
    ICorDebugEditAndContinueSnapshot *pSnapshots[],
    ICorDebugErrorInfoEnum **pError)
{
    return E_NOTIMPL;
}

HRESULT CordbAppDomain::CommitChanges(
    ULONG cSnapshots,
    ICorDebugEditAndContinueSnapshot *pSnapshots[],
    ICorDebugErrorInfoEnum **pError)
{
    return E_NOTIMPL;
}


/*
 * GetProcess returns the process containing the app domain
 */
HRESULT CordbAppDomain::GetProcess(ICorDebugProcess **ppProcess)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    VALIDATE_POINTER_TO_OBJECT(ppProcess,ICorDebugProcess **);

    _ASSERTE (m_pProcess != NULL);

    *ppProcess = static_cast<ICorDebugProcess *> (m_pProcess);
    m_pProcess->ExternalAddRef();

    return S_OK;
}

/*
 * EnumerateAssemblies enumerates all assemblies in the app domain
 */
HRESULT CordbAppDomain::EnumerateAssemblies(ICorDebugAssemblyEnum **ppAssemblies)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    VALIDATE_POINTER_TO_OBJECT(ppAssemblies,ICorDebugAssemblyEnum **);

    CordbHashTableEnum *e = new (nothrow) CordbHashTableEnum(
        this, GetProcess()->GetContinueNeuterList(), // ownership
        &m_assemblies,
        IID_ICorDebugAssemblyEnum);

    if (e == NULL)
        return E_OUTOFMEMORY;

    *ppAssemblies = static_cast<ICorDebugAssemblyEnum*> (e);
    e->ExternalAddRef();

    return S_OK;
}


HRESULT CordbAppDomain::GetModuleFromMetaDataInterface(
                                                  IUnknown *pIMetaData,
                                                  ICorDebugModule **ppModule)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pIMetaData, IUnknown *);
    VALIDATE_POINTER_TO_OBJECT(ppModule, ICorDebugModule **);

    IMetaDataImport *imi = NULL;

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;

    // Grab the interface we need...
    hr = pIMetaData->QueryInterface(IID_IMetaDataImport,
                                            (void**)&imi);

    if (FAILED(hr))
    {
        hr =  E_INVALIDARG;
        goto exit;
    }

    // Get the mvid of the given module.
    GUID matchMVID;
    hr = imi->GetScopeProps(NULL, 0, 0, &matchMVID);

    if (FAILED(hr))
        goto exit;

    CordbModule* moduleentry;
    HASHFIND findmodule;

    for (moduleentry =  m_modules.FindFirst(&findmodule);
         moduleentry != NULL;
         moduleentry =  m_modules.FindNext(&findmodule))
    {
        CordbModule* m = moduleentry;

        if (m->m_pIMImport != NULL)
        {
        // Get the mvid of this module
        GUID MVID;
        hr = m->m_pIMImport->GetScopeProps(NULL, 0, 0, &MVID);

        if (FAILED(hr))
            goto exit;

        if (MVID == matchMVID)
        {
            *ppModule = static_cast<ICorDebugModule*> (m);
            m->ExternalAddRef();

            goto exit;
            }
        }
    }

    hr = E_INVALIDARG;

exit:
    if (imi != NULL)
        imi->Release();
    return hr;
}

CordbModule *CordbAppDomain::GetAnyModule(void)
{
    // Get the first module in the assembly
    HASHFIND find;
    CordbModule *module = m_modules.FindFirst(&find);

    return module;
}


HRESULT CordbAppDomain::EnumerateBreakpoints(ICorDebugBreakpointEnum **ppBreakpoints)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    VALIDATE_POINTER_TO_OBJECT(ppBreakpoints, ICorDebugBreakpointEnum **);

    CORDBRequireProcessSynchronized(this->m_pProcess, this);

    CordbHashTableEnum *e = new (nothrow) CordbHashTableEnum(
        this, GetProcess()->GetContinueNeuterList(), // ownership
        &m_breakpoints,
        IID_ICorDebugBreakpointEnum);

    if (e == NULL)
        return E_OUTOFMEMORY;

    *ppBreakpoints = static_cast<ICorDebugBreakpointEnum*> (e);
    e->ExternalAddRef();

    return S_OK;
}

HRESULT CordbAppDomain::EnumerateSteppers(ICorDebugStepperEnum **ppSteppers)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    VALIDATE_POINTER_TO_OBJECT(ppSteppers,ICorDebugStepperEnum **);

    //
    // !!! m_steppers may be modified while user is enumerating,
    // if steppers complete (if process is running)
    //

    CordbHashTableEnum *e = new (nothrow) CordbHashTableEnum(
        GetProcess(), GetProcess()->GetContinueNeuterList(),  // ownership
        &(m_pProcess->m_steppers),
        IID_ICorDebugStepperEnum);

    if (e == NULL)
        return E_OUTOFMEMORY;

    *ppSteppers = static_cast<ICorDebugStepperEnum*> (e);
    e->ExternalAddRef();

    return S_OK;
}


/*
 * IsAttached returns whether or not the debugger is attached to the
 * app domain.  The controller methods on the app domain cannot be used
 * until the debugger attaches to the app domain.
 */
HRESULT CordbAppDomain::IsAttached(BOOL *pbAttached)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pbAttached, BOOL *);

    *pbAttached = m_fAttached;

    return S_OK;
}

HRESULT inline CordbAppDomain::Attach (void)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(m_pProcess);

    //
    // This method can only be called from inside the ADCreate notification callback.
    //
    if (m_pProcess->GetDispatchedEvent() != DB_IPCE_CREATE_APP_DOMAIN)
    {
        return E_FAIL;
    }

    m_pProcess->Attach(m_AppDomainId);
    m_fAttached = TRUE;

    return S_OK;
}

/*
 * GetName returns the name of the app domain.
 */
HRESULT CordbAppDomain::GetName(ULONG32 cchName,
                                ULONG32 *pcchName,
                                WCHAR szName[])
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY_OR_NULL(szName, WCHAR, cchName, true, true);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pcchName, ULONG32 *);

    // Some reasonable defaults
    if (szName)
        *szName = 0;

    if (pcchName)
        *pcchName = 0;


    // Lazily refresh.
    RefreshName();

    const WCHAR * pName = GetFriendlyName();
    _ASSERTE(pName != NULL); // should be at least set to a dummy literal string.

    return CopyOutString(pName, cchName, pcchName, szName);
}

/*
 * GetObject returns the runtime app domain object.
 * Note:   This method is not yet implemented.
 */
HRESULT CordbAppDomain::GetObject(ICorDebugValue **ppObject)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppObject,ICorDebugObjectValue **);

    return E_NOTIMPL;
}

/*
 * Get the ID of the app domain.
 */
HRESULT CordbAppDomain::GetID (ULONG32 *pId)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    OK_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pId, ULONG32 *);

    *pId = m_AppDomainId;

    return S_OK;
}

//
// LookupModule finds an existing CordbModule given the address of the
// corresponding DebuggerModule object on the RC-side.
//
CordbModule* CordbAppDomain::LookupModule(LSPTR_DMODULE debuggerModuleToken)
{
    INTERNAL_API_ENTRY(this);
    CordbModule *pModule;

    // check to see if the module is present in this app domain
    pModule = m_modules.GetBase(LsPtrToCookie(debuggerModuleToken));

    _ASSERTE( pModule == m_pProcess->LookupModule(debuggerModuleToken ) );

    return pModule;
}

HRESULT CordbAppDomain::GetArrayOrPointerType(CorElementType elementType,
                                              ULONG32 nRank,
                                              ICorDebugType *pTypeArg,
                                              ICorDebugType **ppRes)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppRes, ICorDebugType **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    CordbType *pRes = NULL;

    if (!(elementType == ELEMENT_TYPE_PTR && nRank == 0) &&
        !(elementType == ELEMENT_TYPE_BYREF && nRank == 0) &&
        !(elementType == ELEMENT_TYPE_SZARRAY && nRank == 1) &&
        !(elementType == ELEMENT_TYPE_ARRAY))
        return E_INVALIDARG;

    HRESULT hr = CordbType::MkType(this, elementType, (ULONG) nRank, (CordbType *) pTypeArg, &pRes);

    if (FAILED(hr))
        return hr;
    _ASSERTE(pRes != NULL);

    pRes->ExternalAddRef();
    *ppRes = static_cast<ICorDebugType *> (pRes);
    return hr;

}


HRESULT CordbAppDomain::GetFunctionPointerType(ULONG32 nTypeArgs,
                                               ICorDebugType *ppTypeArgs[],
                                               ICorDebugType **ppRes)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppRes, ICorDebugType **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // Prefast overflow check:
    S_UINT32 allocSize = S_UINT32( nTypeArgs ) * S_UINT32( sizeof(CordbType* ) );
    if( allocSize.IsOverflow() )
    {
        return E_INVALIDARG;
    }

    CordbType **ppInst = (CordbType **) _alloca( allocSize.Value() );
    for (unsigned int i = 0; i<nTypeArgs;i++)
        ppInst[i] = (CordbType *) ppTypeArgs[i];

    Instantiation inst(nTypeArgs, ppInst);

    CordbType *pRes = NULL;

    HRESULT hr = CordbType::MkType(this, ELEMENT_TYPE_FNPTR, inst, &pRes);

    if (FAILED(hr))
        return hr;
    _ASSERTE(pRes != NULL);

    pRes->ExternalAddRef();
    *ppRes = static_cast<ICorDebugType *> (pRes);

    return hr;

}

