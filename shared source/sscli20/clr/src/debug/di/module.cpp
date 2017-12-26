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
// File: module.cpp
//
//*****************************************************************************
#include "stdafx.h"

// We have an assert in ceemain.cpp that validates this assumption
#define FIELD_OFFSET_NEW_ENC_DB          0x07FFFFFB

#include "winbase.h"
#include "corpriv.h"


STDAPI ReOpenMetaDataWithMemoryEx(
    void        *pUnk,
    LPCVOID     pData,
    ULONG       cbData,
    DWORD       dwReOpenFlags)

    // In PAL, this is statically linked in.
    ;


CordbModule::CordbModule(CordbProcess *process, CordbAssembly *pAssembly,
                         LSPTR_DMODULE debuggerModuleToken, REMOTE_PTR PEBaseAddress,
                         ULONG nPESize, BOOL fDynamic, BOOL fInMemory,
                         const WCHAR *szName,
                         const WCHAR * szNgenName,
                         CordbAppDomain *pAppDomain)

    : CordbBase(process, LsPtrToCookie(debuggerModuleToken), enumCordbModule), 
    m_pAssembly(pAssembly),
    m_pAppDomain(pAppDomain),
    m_classes(11),
    m_functions(101),
    m_debuggerModuleToken(debuggerModuleToken),
    m_EnCCount(0),
    m_PEBaseAddress(PEBaseAddress),
    m_nPESize(nPESize),
    m_fDynamic(fDynamic),
    m_fInMemory(fInMemory),
    m_pClass(NULL)
{
    _ASSERTE(m_debuggerModuleToken != NULL);

    // Make a copy of the name.
    // In worse case, we get a zero-length string, never NULL.
    // However, m_szModuleName is NULL if there's no name.
    _ASSERTE(szName != NULL);

    // If we're not ngenned, then this is 0-length, but never null.
    _ASSERTE(szNgenName != NULL);

    // Make copies of strings. In oom scenario, this will just leave the values as null.
    m_szModuleName.AssignCopy(szName);
    m_szNgenName.AssignCopy(szNgenName);



    {
        DWORD dwErr;
        dwErr = process->GetID(&m_dwProcessId);
        _ASSERTE(!FAILED(dwErr));
    }
}

/*
    A list of which resources owned by this object are accounted for.

UNKNOWN:
        void*            m_pMetadataStartToBe;
        void*            m_pMetadataStart;
HANDLED:
        CordbProcess*    m_process; // Assigned w/o AddRef()
        CordbAssembly*   m_pAssembly; // Assigned w/o AddRef()
        CordbAppDomain*  m_pAppDomain; // Assigned w/o AddRef()
        CordbHashTable   m_classes; // Neutered
        CordbHashTable   m_functions; // Neutered
        IMetaDataImport *m_pIMImport; // Released in ~CordbModule
        WCHAR*           m_szModuleName; // Deleted in ~CordbModule
        CordbClass*      m_pClass; // Released in ~CordbModule
*/

CordbModule::~CordbModule()
{
    // We should have been explicitly neutered before our internal ref went to 0.
    _ASSERTE(IsNeutered());

    _ASSERTE(m_pIMImport == NULL);
}

// Neutered by CordbAppDomain
void CordbModule::Neuter(NeuterTicket ticket)
{
    RSSmartPtr<CordbModule> pRef(this);
    {
        // m_pAppDomain, m_pAssembly assigned w/o AddRef()
        NeuterAndClearHashtable(&m_classes, ticket);
        NeuterAndClearHashtable(&m_functions, ticket);

        m_pClass.Clear();

        // This is very important because it also releases the metadata's potential file locks.
        m_pIMImport.Clear();

        CordbBase::Neuter(ticket);
    }
    // implicit release on pRef
}

//
// Initialize the metadata for this module.  Called only once when the module is created.
//
HRESULT CordbModule::Init(REMOTE_PTR pMetadataStart, ULONG nMetadataSize)
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //

    if (nMetadataSize == 0)
    {
        return S_OK;
    }
    _ASSERTE(pMetadataStart != NULL);

    // The debuggee already has a copy of the metadata in its process.
    // If we OpenScope on file as read-only, the OS file-system will share our metadata with the
    // copy in the debuggee. This can be a major perf win. FX metadata can be over 8 MB+.
    // OpenScopeOnMemory can't be shared b/c we allocate a buffer.
    bool fOpenFromFile = true;

    // Dynamic, In-memory, modules must be OpenScopeOnMemory.
    // For Enc, we'll can use OpenScope(onFile) and it will get converted to Memory when we get an emitter.
    // We're called from before the ModuleLoad callback, so EnC status hasn't been set yet, so
    // EnC will be false.
    if (m_fDynamic || m_fInMemory)
    {
        fOpenFromFile = false;
    }

#ifdef _DEBUG
    // Reg key override to force us to use Open-by-memory. This can let us run perf tests to
    // compare the Open-by-mem vs. Open-by-file.
    static DWORD hack = 99;
    if (hack == 99)
        hack = REGUTIL::GetConfigDWORD(L"DbgNoOpenMDByFile", 0);

    if (hack)
    {
        fOpenFromFile = false;
    }
#endif

    HRESULT hr;

    // Get from File. A module can't change from having it's MD being "Open by Memory" to
    // "Open by file". The properties that determine that decision are set at module creation time
    // and can't be changed.
    if (fOpenFromFile)
    {
        ReleaseHolder<IMetaDataDispenserEx> pDisp;
        hr = GetProcess()->m_cordb->m_pMetaDispenser->QueryInterface(
                                                    IID_IMetaDataDispenserEx,
                                                    (void**)&pDisp);
        if(FAILED(hr))
        {
            return hr;
        }

        m_pIMImport.Clear();

        // Get filename. There are 2 filenames to choose from:
        // - ngen (if applicable).
        // - non-ngen (aka "normal).
        // By loading metadata out of the same OS file as loaded into the debuggee space, the OS can share those pages.
        // So, if we have a ngen image, then use that b/c the module's non-ngen path may not even be loaded.
        // Else use the normal non-ngen path.
        const WCHAR * szFileName = m_szNgenName;
        if (szFileName == NULL)
        {
            szFileName = m_szModuleName;
        }

        // We're opening it as "read". If we QI for an IEmit interface (which we need for EnC),
        // then the metadata engine will convert it to a "write" underneath us.
        // We want "read" so that we can let the OS share the pages.
        const DWORD dwOpenFlags = 0;
        hr = pDisp->OpenScope(szFileName, dwOpenFlags, IID_IMetaDataImport, (IUnknown**)&m_pIMImport);

        _ASSERTE(SUCCEEDED(hr) == (m_pIMImport != NULL));

        if (SUCCEEDED(hr))
        {
            return hr;
        }

        // If we can't OpenScopeOnFile, then fall through and try to open it on memory.
        // This should never happen in normal scenarios.  It could happen if someone has renamed
        // the assembly after it was opened by the debugee process, but this should be rare enough
        // that we don't mind taking the perf. hit and loading from memory.
         LOG((LF_CORDB,LL_WARNING, "CM::RI: Couldn't open metadata in file \"%s\" (hr=%x), falling back to in-memory\n", szFileName, hr));
    }

    //
    // We're opening the metadata from memory
    //

    // copy it over from the remote process

    CoTaskMemHolder<VOID> pMetadataCopy;
    hr = CopyRemoteMetadata( pMetadataStart, nMetadataSize, &pMetadataCopy );
    if (FAILED(hr))
    {
        return hr;
    }
    
     //
    // Setup our metadata import object, m_pIMImport
    //

    ReleaseHolder<IMetaDataDispenserEx> pDisp;
    hr = GetProcess()->m_cordb->m_pMetaDispenser->QueryInterface(
                                                IID_IMetaDataDispenserEx,
                                                (void**)&pDisp);
    if( FAILED(hr) )
    {
        return hr;
    }

    // Save the old mode for restoration
    VARIANT valueOld;
    hr = pDisp->GetOption(MetaDataSetUpdate, &valueOld);
    _ASSERTE( !FAILED(hr) );

    // Set R/W mode so that we can update the metadata when
    // we do EnC operations.
    VARIANT valueRW;
    V_VT(&valueRW) = VT_UI4;
    V_I4(&valueRW) = MDUpdateFull;
    hr = pDisp->SetOption(MetaDataSetUpdate, &valueRW);
    _ASSERTE( !FAILED(hr) );

    m_pIMImport.Clear();
    hr = pDisp->OpenScopeOnMemory(pMetadataCopy,
                                  nMetadataSize,
                                  ofTakeOwnership,
                                  IID_IMetaDataImport,
                                  reinterpret_cast<IUnknown**>( &m_pIMImport ));
    if (FAILED(hr))
    {
        return hr;
    }

    // Metadata has taken ownership -don't free the memory
    pMetadataCopy.SuppressRelease();
    
    // Restore the old setting
    hr = pDisp->SetOption(MetaDataSetUpdate, &valueOld);
    _ASSERTE( !FAILED(hr) );

    return hr;
}

//
// Update Metedata by copying it from the remote process and updating
// our IMetadataImport ojbect.
// Useful for additional class-loads into a dynamic module.
//
HRESULT CordbModule::UpdateMetadataFromRemote(
    REMOTE_PTR pRemoteMetadataPtr, DWORD dwMetadataSize )
{
    if (pRemoteMetadataPtr == NULL || dwMetadataSize == 0)
        return E_INVALIDARG;

    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //

    HRESULT hr;

    // First copy it from the remote process
    CoTaskMemHolder<VOID> pLocalMetadataPtr;
     hr = CopyRemoteMetadata( pRemoteMetadataPtr, dwMetadataSize, &pLocalMetadataPtr );

     if( FAILED(hr) )
     {
         return hr;
     }

    ReleaseHolder<IMetaDataDispenserEx> pDisp;
    hr = GetProcess()->m_cordb->m_pMetaDispenser->QueryInterface(
                                                IID_IMetaDataDispenserEx,
                                                (void**)&pDisp);
    if( FAILED(hr) )
    {
        return hr;
    }

    LOG((LF_CORDB,LL_INFO100000, "CM::RI: converting to new metadata\n"));

     // now verify that the metadata is valid by opening a temporary scope on the memory
     {
        ReleaseHolder<IMetaDataImport> pIMImport;
        hr = pDisp->OpenScopeOnMemory(pLocalMetadataPtr,
                                      dwMetadataSize,
                                      0,
                                      IID_IMetaDataImport,
                                      (IUnknown**)&pIMImport);
        if (FAILED(hr))
        {
            return hr;
        }
     }

    // Now tell our current IMetadataImport object to re-initialize by swapping in the new memory block.
    // This allows us to keep manipulating metadata objects on other threads without crashing.
    hr = ReOpenMetaDataWithMemoryEx(m_pIMImport, pLocalMetadataPtr, dwMetadataSize, ofTakeOwnership );
    if( FAILED(hr) )
    {
        return hr;
    }

    // Success.  Metadata now owns the metadata memory
    pLocalMetadataPtr.SuppressRelease();
    
    return hr;
}


//
//  Copy metadata memory from the remote process into a newly allocated local buffer.
//
HRESULT CordbModule::CopyRemoteMetadata(
    REMOTE_PTR pRemoteMetadataPtr, DWORD dwMetadataSize, PVOID* ppLocalMetadataPtr)
{
    _ASSERTE( ppLocalMetadataPtr != NULL );
    _ASSERTE( dwMetadataSize > 0 );
    _ASSERTE( pRemoteMetadataPtr != NULL );
    *ppLocalMetadataPtr = NULL;

    // Allocate space for the local copy of the metadata
    // No need to zero out the memory since we'll fill it all here.
   CoTaskMemHolder<VOID> pLocalMetadataPtr( CoTaskMemAlloc( dwMetadataSize ) );
    if (pLocalMetadataPtr == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // Copy the metadata from the left side
    HANDLE hProcess = GetProcess()->m_handle;
    BOOL succ;
    succ = ReadProcessMemoryI(hProcess,
                              pRemoteMetadataPtr,
                              pLocalMetadataPtr,
                              dwMetadataSize,
                              NULL);

    if (!succ)
    {
        return HRESULT_FROM_GetLastError();
    }

    *ppLocalMetadataPtr = pLocalMetadataPtr.Extract();
    
    return S_OK;
}

HRESULT CordbModule::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugModule)
    {
        *pInterface = static_cast<ICorDebugModule*>(this);
    }
    else if (id == IID_ICorDebugModule2)
    {
        *pInterface = static_cast<ICorDebugModule2*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugModule*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbModule::GetProcess(ICorDebugProcess **ppProcess)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppProcess, ICorDebugProcess **);

    *ppProcess = static_cast<ICorDebugProcess*> (GetProcess());
    GetProcess()->ExternalAddRef();

    return S_OK;
}

HRESULT CordbModule::GetBaseAddress(CORDB_ADDRESS *pAddress)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pAddress, CORDB_ADDRESS *);

    *pAddress = PTR_TO_CORDB_ADDRESS(m_PEBaseAddress);
    return S_OK;
}

HRESULT CordbModule::GetAssembly(ICorDebugAssembly **ppAssembly)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppAssembly, ICorDebugAssembly **);

    *ppAssembly = static_cast<ICorDebugAssembly *> (m_pAssembly);
    if (m_pAssembly != NULL)
    {
        m_pAssembly->ExternalAddRef();
    }

    return S_OK;
}

// Gets the module file name.
// Note that this isn't necessarily the same as the module name in the metadata.
// cchName is the count of characters in the szName buffer on input.
// *pcchName is an optional out parameter, which gets set to the fully requested size
// (not just how many characters are written).
// Returns S_OK on success.
// Returns S_FALSE if we fabricate the name.
HRESULT CordbModule::GetName(ULONG32 cchName, ULONG32 *pcchName, WCHAR szName[])
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY_OR_NULL(szName, WCHAR, cchName, true, true);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pcchName, ULONG32);

    const WCHAR *szTempName = m_szModuleName;

    // In case we didn't get the name (most likely out of memory on ctor or an in-memory module).
    if (!szTempName)
        szTempName = L"<unknown>";

    HRESULT hr = CopyOutString(szTempName, cchName, pcchName, szName);

    // Special case to return S_FALSE if we fabricated the name.
    if ((hr == S_OK) && (m_szModuleName == NULL))
    {
        return S_FALSE;
    }
    return hr;
}

HRESULT CordbModule::EnableJITDebugging(BOOL bTrackJITInfo, BOOL bAllowJitOpts)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    CordbProcess *pProcess = GetProcess();

    static ConfigDWORD newJitCompilerFlags;
    DWORD newFlags = newJitCompilerFlags.val(L"DbgNewJitCompilerFlags", 1);
    // in M3.3 can only call this API from context of LoadModule callback
    if (newFlags)
    {
        if (pProcess->GetDispatchedEvent() != DB_IPCE_LOAD_MODULE)
        {
            return CORDBG_E_MUST_BE_IN_LOAD_MODULE;
        }
    }

    ATT_REQUIRE_STOPPED_MAY_FAIL(pProcess);

    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event,
                           DB_IPCE_CHANGE_JIT_DEBUG_INFO,
                           true,
                           GetAppDomain()->GetADToken());

    event.JitDebugInfo.debuggerModuleToken = m_debuggerModuleToken;
    event.JitDebugInfo.fAllowJitOpts = bAllowJitOpts;
    // bTrackJITInfo is the default and cannot be turned off.

    // Note: two-way event here...
    HRESULT hr = pProcess->m_cordb->SendIPCEvent(pProcess,
                                                 &event,
                                                 sizeof(DebuggerIPCEvent));

    if (FAILED(hr))
        goto LExit;

    _ASSERTE(event.type == DB_IPCE_CHANGE_JIT_INFO_RESULT);
    hr = event.hr;

LExit:
    // simulate the less strict v1 error handling
    if (hr == CORDBG_E_CANNOT_BE_ON_ATTACH && pProcess->m_cordb->GetDebuggerVersion() <= CorDebugVersion_1_0)
        hr = S_OK;

    return hr;
}

HRESULT CordbModule::EnableClassLoadCallbacks(BOOL bClassLoadCallbacks)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_ALLOW_LIVE_DO_STOPGO(GetProcess());

    // You must receive ClassLoad callbacks for dynamic modules so that we can keep the metadata up-to-date on the Right
    // Side. Therefore, we refuse to turn them off for all dynamic modules (they were forced on when the module was
    // loaded on the Left Side.)
    if (m_fDynamic && !bClassLoadCallbacks)
        return E_INVALIDARG;

    // Send a Set Class Load Flag event to the left side. There is no need to wait for a response, and this can be
    // called whether or not the process is synchronized.
    CordbProcess *pProcess = GetProcess();

    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event,
                           DB_IPCE_SET_CLASS_LOAD_FLAG,
                           false,
                           (GetAppDomain()->GetADToken()));
    event.SetClassLoad.debuggerModuleToken = m_debuggerModuleToken;
    event.SetClassLoad.flag = (bClassLoadCallbacks == TRUE);

    HRESULT hr = pProcess->m_cordb->SendIPCEvent(pProcess, &event,
                                                 sizeof(DebuggerIPCEvent));
    hr = WORST_HR(hr, event.hr);
    return hr;
}

// Get the CordbFunction matches this token / module pair.
// This will be a random function for generics.
// Each time a function is Enc-ed, it gets its own CordbFunction object.
// This will return the latest EnC version of the function for this Module,Token pair.
HRESULT CordbModule::GetFunctionFromToken(mdMethodDef token,
                                          ICorDebugFunction **ppFunction)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    ATT_ALLOW_LIVE_DO_STOPGO(GetProcess());

    if (token == mdMethodDefNil)
        return E_INVALIDARG;

    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppFunction, ICorDebugFunction **);

    HRESULT hr = S_OK;


    // If we already have a CordbFunction for this token, then we'll
    // take since we know it has to be valid.
    CordbFunction *f = m_functions.GetBase(token);

    if (f == NULL)
    {
        // If this function was ever EnC-ed, then we expect that the LS sent us a notification
        // telling us about it, which would have also populated the cache.
        // Thus if the lookup fails and we're here, we know that we weren't EnC.
        // Thus we can safely assume the function has version=1.

        // Validate the token.
        if (!m_pIMImport->IsValidToken(token))
        {
            hr = E_INVALIDARG;
            goto LExit;
        }

        f = new (nothrow) CordbFunction(this, token, 0, CorDB_DEFAULT_ENC_FUNCTION_VERSION);

        if (f == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto LExit;
        }

        hr = m_functions.AddBase(f);

        if (FAILED(hr))
        {
            delete f;
            goto LExit;
        }
    }

    *ppFunction = static_cast<ICorDebugFunction*> (f);
    f->ExternalAddRef();

LExit:
    return hr;
}

HRESULT CordbModule::GetFunctionFromRVA(CORDB_ADDRESS rva,
                                        ICorDebugFunction **ppFunction)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppFunction, ICorDebugFunction **);

    return E_NOTIMPL;
}

HRESULT CordbModule::LookupClassByToken(mdTypeDef token,
                                        CordbClass **ppClass)
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //
    FAIL_IF_NEUTERED(this);

    *ppClass = NULL;

    if ((token == mdTypeDefNil) || (TypeFromToken(token) != mdtTypeDef))
        return E_INVALIDARG;

    CordbClass *c = m_classes.GetBase(token);

    if (c == NULL)
    {
        // Validate the token.
        if (!m_pIMImport->IsValidToken(token))
            return E_INVALIDARG;

        c = new (nothrow) CordbClass(this, token);

        if (c == NULL)
            return E_OUTOFMEMORY;

        HRESULT res = m_classes.AddBase(c);

        if (FAILED(res))
        {
            delete c;
            return (res);
        }
    }

    *ppClass = c;

    return S_OK;
}

HRESULT CordbModule::GetClassFromToken(mdTypeDef token,
                                       ICorDebugClass **ppClass)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_ALLOW_LIVE_DO_STOPGO(this->GetProcess());
    VALIDATE_POINTER_TO_OBJECT(ppClass, ICorDebugClass **);

    CordbClass *c;
    *ppClass = NULL;

    // Validate the token.
    if (!m_pIMImport->IsValidToken(token))
        return E_INVALIDARG;

    HRESULT hr = LookupClassByToken(token, &c);

    if (SUCCEEDED(hr))
    {
        *ppClass = static_cast<ICorDebugClass*> (c);
        c->ExternalAddRef();
    }

    return hr;
}

HRESULT CordbModule::CreateBreakpoint(ICorDebugModuleBreakpoint **ppBreakpoint)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppBreakpoint, ICorDebugModuleBreakpoint **);

    return E_NOTIMPL;
}

//
// Return the token for the Module table entry for this object.  The token
// may then be passed to the meta data import api's.
//
HRESULT CordbModule::GetToken(mdModule *pToken)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pToken, mdModule *);
    HRESULT hr = S_OK;

    _ASSERTE(m_pIMImport);
    hr = (m_pIMImport->GetModuleFromScope(pToken));

    return hr;
}


//
// Return a meta data interface pointer that can be used to examine the
// meta data for this module.
HRESULT CordbModule::GetMetaDataInterface(REFIID riid, IUnknown **ppObj)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppObj, IUnknown **);

    HRESULT hr = S_OK;

    // QI the importer that we already have and return the result.
    hr = m_pIMImport->QueryInterface(riid, (void**)ppObj);

    return hr;
}

//
// LookupFunction finds an existing CordbFunction in the given module.
// If the function doesn't exist, it returns NULL.
//
CordbFunction* CordbModule::LookupFunction(mdMethodDef funcMetadataToken)
{
    INTERNAL_API_ENTRY(this);
    return m_functions.GetBase(funcMetadataToken);
}

//
// LookupOrCreateFunction finds an existing version of CordbFunction in the given module.
// If the function doesn't exist, it creates it.
//
HRESULT CordbModule::LookupOrCreateFunction(mdMethodDef funcMetadataToken, SIZE_T funcRVA, SIZE_T enCVersion, CordbFunction **ppRes)
{
    INTERNAL_API_ENTRY(this);

    if (! ppRes)
    {
        return E_INVALIDARG;
    }
    *ppRes = NULL;

    HRESULT hr = E_FAIL;

    CordbFunction *f = m_functions.GetBase(funcMetadataToken);

    // special case non-existance as need to add to the hash table too
    if (f == NULL)
    {
        hr = CreateFunction(funcMetadataToken, funcRVA, enCVersion, &f);
        if (SUCCEEDED(hr))
        {
            *ppRes = f;
        }
        return hr;
    }

    if (f->m_dwEnCVersionNumber == enCVersion)
    {
        // if have a new RVA to set then use it. Will only have one for non-EnC case.
        if ((f->m_dwEnCVersionNumber == CorDB_DEFAULT_ENC_FUNCTION_VERSION) && (funcRVA != 0))
        {
            f->m_functionRVA = funcRVA;
        }
        *ppRes = f;
        return S_OK;
    }

    // linked list sorted with most recent version at front. Version numbers correspond
    // to actual edit count against the module, so version numbers not necessarily contiguous.
    // Any valid EnC version must already exist as we would have created it on the ApplyChanges
    for (CordbFunction *pf=f; pf != NULL; pf = pf->m_pPrevVersion)
    {
        if (pf->m_dwEnCVersionNumber == enCVersion)
        {
            *ppRes = pf;
            return S_OK;
        }
    }

    _ASSERTE("Couldn't find EnC version of function\n");
    return E_FAIL;
}

HRESULT CordbModule::IsDynamic(BOOL *pDynamic)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pDynamic, BOOL *);

    (*pDynamic) = m_fDynamic;

    return S_OK;
}

BOOL CordbModule::IsDynamic()
{
    return m_fDynamic;
}


HRESULT CordbModule::IsInMemory(BOOL *pInMemory)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pInMemory, BOOL *);

    (*pInMemory) = m_fInMemory;

    return S_OK;
}

HRESULT CordbModule::GetGlobalVariableValue(mdFieldDef fieldDef,
                                            ICorDebugValue **ppValue)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());

    HRESULT hr = S_OK;

    if (m_pClass == NULL)
    {
        CordbClass * pGlobalClass = NULL;
        hr = LookupClassByToken(COR_GLOBAL_PARENT_TOKEN,
                                &pGlobalClass);
        if (FAILED(hr))
            goto LExit;

        m_pClass.Assign(pGlobalClass);

        _ASSERTE( m_pClass != NULL);
    }

    hr = m_pClass->GetStaticFieldValue(fieldDef, NULL, ppValue);

LExit:
    return hr;
}



//
// CreateFunction creates a new function from the given information and
// adds it to the module.
//
HRESULT CordbModule::CreateFunction(mdMethodDef funcMetadataToken,
                                    SIZE_T funcRVA,
                                    SIZE_T enCVersion,
                                    CordbFunction** ppFunction)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    // RVA should be zero if function has been EnC
    if (enCVersion > 0)
    {
        funcRVA = 0;
    }

    // Create a new function object.
    CordbFunction* pFunction = new (nothrow) CordbFunction(this,funcMetadataToken, funcRVA, enCVersion);

    if (pFunction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // Add the function to the Module's hash of all functions.
    HRESULT hr = m_functions.AddBase(pFunction);

    if (SUCCEEDED(hr))
    {
        *ppFunction = pFunction;
    }
    else
    {
        delete pFunction;
    }

    return hr;
}



HRESULT CordbModule::LookupOrCreateClass(mdTypeDef classMetadataToken,CordbClass** ppClass)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    HRESULT hr = S_OK;
    *ppClass = LookupClass(classMetadataToken);
    if (*ppClass == NULL)
    {
        hr = CreateClass(classMetadataToken,ppClass);
        if (!SUCCEEDED(hr))
            return hr;
    }
    return hr;
}

//
// LookupClass finds an existing CordbClass in the given module.
// If the class doesn't exist, it returns NULL.
//
CordbClass* CordbModule::LookupClass(mdTypeDef classMetadataToken)
{
    INTERNAL_API_ENTRY(this);
    return m_classes.GetBase(classMetadataToken);
}

//
// CreateClass creates a new class from the given information and
// adds it to the module.
//
HRESULT CordbModule::CreateClass(mdTypeDef classMetadataToken,
                                 CordbClass** ppClass)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    CordbClass* pClass =
        new (nothrow) CordbClass(this, classMetadataToken);

    if (pClass == NULL)
        return E_OUTOFMEMORY;

    HRESULT hr = m_classes.AddBase(pClass);

    if (SUCCEEDED(hr))
        *ppClass = pClass;
    else
        delete pClass;

    if (classMetadataToken == COR_GLOBAL_PARENT_TOKEN)
    {
        _ASSERTE( m_pClass == NULL ); //redundant create
        m_pClass.Assign(pClass);
    }

    return hr;
}

HRESULT CordbModule::ResolveTypeRef(mdTypeRef token,
                                    CordbClass **ppClass)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    _ASSERTE(GetProcess()->GetSynchronized());


    if ((token == mdTypeRefNil) || (TypeFromToken(token) != mdtTypeRef))
    {
        return E_INVALIDARG;
    }

    HRESULT         hr = S_OK;
    *ppClass = NULL;
     
    DebuggerIPCEvent event;
    GetProcess()->InitIPCEvent(&event,
                           DB_IPCE_RESOLVE_TYPEREF,
                           true,
                           (GetAppDomain()->GetADToken()));
    event.ResolveTypeRef.refingModuleToken = m_debuggerModuleToken;
    event.ResolveTypeRef.typeRefToken = token;
    
    hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event, sizeof(DebuggerIPCEvent));

    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    _ASSERTE(event.type == DB_IPCE_RESOLVE_TYPEREF_RESULT);

    if (!SUCCEEDED(event.hr))
    {
        return event.hr;
    }

    CordbModule* mod = m_pAppDomain->LookupModule( event.ResolveTypeRefResult.debuggerModuleToken );

    if( mod == NULL )
    {
        // If we cant find the module, perhaps we haven't gotten the load event for it for some reason
        _ASSERTE(!"TypeRef resolved to a DebuggerModule we don't recognize" );
        return CORDBG_E_CLASS_NOT_LOADED;
    }
    
    return mod->LookupClassByToken( event.ResolveTypeRefResult.typeDefToken, ppClass );
    
}

HRESULT CordbModule::ResolveTypeRefOrDef(mdToken token,
                                    CordbClass **ppClass)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //

    if ((token == mdTypeRefNil) ||
        (TypeFromToken(token) != mdtTypeRef && TypeFromToken(token) != mdtTypeDef))
        return E_INVALIDARG;

    if (TypeFromToken(token)==mdtTypeRef)
    {
        return ( ResolveTypeRef(token, ppClass) );
    }
    else
    {
        return ( LookupClassByToken(token, ppClass) );
    }

}

//
// GetSize returns the size of the module.
//
HRESULT CordbModule::GetSize(ULONG32 *pcBytes)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pcBytes, ULONG32 *);

    *pcBytes = m_nPESize;

    return S_OK;
}

CordbAssembly *CordbModule::GetCordbAssembly(void)
{
    INTERNAL_API_ENTRY(this);
    return m_pAssembly;
}


HRESULT CordbModule::GetEditAndContinueSnapshot(
    ICorDebugEditAndContinueSnapshot **ppEditAndContinueSnapshot)
{
    return E_NOTIMPL;
}



HRESULT CordbModule::ApplyChanges(ULONG cbMetadata,
    BYTE pbMetadata[], ULONG cbIL, BYTE pbIL[])
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());


    return E_NOTIMPL;
}





HRESULT CordbModule::ApplyChangesInternal(ULONG cbMetadata, BYTE pbMetadata[], ULONG cbIL, BYTE pbIL[])
{
    LOG((LF_ENC,LL_INFO100, "CordbProcess::ApplyChangesInternal\n"));

    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //

    return E_NOTIMPL;

}

// Set the JMC status for the entire module.
// All methods specified in others[] will have jmc status !fIsUserCode
// All other methods will have jmc status fIsUserCode.
HRESULT CordbModule::SetJMCStatus(
        BOOL fIsUserCode,
        ULONG32 cOthers,
        mdToken others[])
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    if (cOthers != 0)
    {
        _ASSERTE(!"not yet impl for cOthers != 0");
        return E_NOTIMPL;
    }

    // Send event to the LS.
    CordbProcess* pProcess = this->GetProcess();
    _ASSERTE(pProcess != NULL);


    // Tell the LS that this module is/is not user code
    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event, DB_IPCE_SET_MODULE_JMC_STATUS, true, this->GetAppDomain()->GetADToken());
    event.SetJMCFunctionStatus.debuggerModuleToken = m_debuggerModuleToken;
    event.SetJMCFunctionStatus.dwStatus = fIsUserCode;

    _ASSERTE(m_debuggerModuleToken != NULL);

    // Note: two-way event here...
    HRESULT hr = pProcess->m_cordb->SendIPCEvent(pProcess, &event, sizeof(DebuggerIPCEvent));

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
    {
        LOG((LF_CORDB, LL_INFO10, "CordbModule::SetJMCStatus failed  0x%08x...\n", hr));

        return hr;
    }

    _ASSERTE(event.type == DB_IPCE_SET_MODULE_JMC_STATUS_RESULT);

    LOG((LF_CORDB, LL_INFO10, "returning from CordbModule::SetJMCStatus 0x%08x...\n", hr));

    return event.hr;
}



// Resolve an assembly given an AssemblyRef token. Note that
// this will not trigger the loading of assembly. If assembly is not yet loaded,
// this will return an CORDBG_E_CANNOT_RESOLVE_ASSEMBLY error
//
HRESULT CordbModule::ResolveAssembly(mdToken tkAssemblyRef,
                                    ICorDebugAssembly **ppAssembly)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());

    if( ppAssembly )
    {
        *ppAssembly = NULL;
    }
    
    CordbAssembly *pCordbAsm = NULL;
    HRESULT hr = ResolveAssemblyInternal( tkAssemblyRef, &pCordbAsm );

    if( ppAssembly && SUCCEEDED(hr) )
    {
        _ASSERTE(pCordbAsm != NULL);
        *ppAssembly = static_cast<ICorDebugAssembly*> (pCordbAsm);
         pCordbAsm->ExternalAddRef();
    }

    return hr;
}

HRESULT CordbModule::ResolveAssemblyInternal( mdToken tkAssemblyRef,
                                CordbAssembly **ppAssembly)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //
    
    if (TypeFromToken(tkAssemblyRef) != mdtAssemblyRef || tkAssemblyRef == mdAssemblyRefNil)
    {
        // Not a valid token
        return E_INVALIDARG;
    }

    if (ppAssembly)
    {
        *ppAssembly = NULL;
    }

    HRESULT         hr = S_OK;
    CordbAssembly   *a = NULL;
    HASHFIND        find;
    LSPTR_ASSEMBLY  assemblyToken;
    DebuggerIPCEvent *event = (DebuggerIPCEvent*) _alloca(CorDBIPC_BUFFER_SIZE);

    // two-way events
    GetProcess()->InitIPCEvent(event, DB_IPCE_RESOLVE_ASSEMBLY, true, m_pAppDomain->GetADToken());
    event->ResolveAssembly.refingModuleToken.Set(reinterpret_cast<void *>(LsPtrToCookie(m_debuggerModuleToken)));
    event->ResolveAssembly.assemblyRefToken = tkAssemblyRef;

    hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), event, sizeof(DebuggerIPCEvent));
    hr = WORST_HR(hr, event->hr);

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        goto exit;

    _ASSERTE(event->type == DB_IPCE_RESOLVE_ASSEMBLY_RESULT);

    // now find the ICorDebugAssembly corresponding to it
    assemblyToken = event->ResolveAssemblyResult.debuggerAssemblyToken;

    if (assemblyToken != NULL)
    {
        for (a  = m_pAppDomain->m_assemblies.FindFirst(&find);
            a != NULL;
            a  = m_pAppDomain->m_assemblies.FindNext(&find))
        {
            // The fiber cookie is really a ptr to the EE's Thread object,
            // which is exactly what out m_debuggerThreadToken is.
            if (a->m_id == LsPtrToCookie(assemblyToken))
                break;
        }
    }

    if (a == NULL)
    {
        _ASSERTE(*ppAssembly == NULL);
        return CORDBG_E_CANNOT_RESOLVE_ASSEMBLY;
    }
    else
    {
        if (ppAssembly != NULL)
        {
            *ppAssembly = a;
        }
    }
exit:
    return hr;
}



HRESULT CordbModule::SetJITCompilerFlags(DWORD dwFlags)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    CordbProcess *pProcess = GetProcess();

    ATT_REQUIRE_STOPPED_MAY_FAIL(pProcess);

    // can't have a subset of these, eg 0x101, so make sure we have an exact match
    if ((dwFlags != CORDEBUG_JIT_DEFAULT) &&
        (dwFlags != CORDEBUG_JIT_DISABLE_OPTIMIZATION) &&
        (dwFlags != CORDEBUG_JIT_ENABLE_ENC))
    {
        return E_INVALIDARG;
    }


    if (pProcess->GetDispatchedEvent() != DB_IPCE_LOAD_MODULE)
    {
        return CORDBG_E_MUST_BE_IN_LOAD_MODULE;
    }

    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event,
                           DB_IPCE_SET_JIT_COMPILER_FLAGS,
                           true,
                           GetAppDomain()->GetADToken());

    event.JitDebugInfo.debuggerModuleToken = m_debuggerModuleToken;
    event.JitDebugInfo.fAllowJitOpts =
        ((dwFlags & CORDEBUG_JIT_DISABLE_OPTIMIZATION) != CORDEBUG_JIT_DISABLE_OPTIMIZATION);

    event.JitDebugInfo.fEnableEnC = ((dwFlags & CORDEBUG_JIT_ENABLE_ENC) == CORDEBUG_JIT_ENABLE_ENC);

    // Note: two-way event here...
    HRESULT hr = pProcess->m_cordb->SendIPCEvent(pProcess,
                                                 &event,
                                                 sizeof(DebuggerIPCEvent));

    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    _ASSERTE(event.type == DB_IPCE_SET_JIT_COMPILER_FLAGS_RESULT);

    return event.hr;
}

HRESULT CordbModule::GetJITCompilerFlags(DWORD *pdwFlags )
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pdwFlags, DWORD*);
    *pdwFlags = 0;

    CordbProcess *pProcess = GetProcess();


    ATT_REQUIRE_STOPPED_MAY_FAIL(pProcess);

    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event,
                           DB_IPCE_GET_JIT_COMPILER_FLAGS,
                           true,
                           GetAppDomain()->GetADToken());

    event.JitDebugInfo.debuggerModuleToken = m_debuggerModuleToken;
    // Note: two-way event here...
    HRESULT hr = pProcess->m_cordb->SendIPCEvent(pProcess,
                                                 &event,
                                                 sizeof(DebuggerIPCEvent));

    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    _ASSERTE(event.type == DB_IPCE_GET_JIT_COMPILER_FLAGS_RESULT);

    DWORD dwFlags = CORDEBUG_JIT_DEFAULT;
    if (event.JitDebugInfo.fEnableEnC)
    {
        dwFlags = CORDEBUG_JIT_ENABLE_ENC;
    }
    else if (! event.JitDebugInfo.fAllowJitOpts)
    {
        dwFlags = CORDEBUG_JIT_DISABLE_OPTIMIZATION;
    }

    *pdwFlags = dwFlags;
    return event.hr;
}

/* ------------------------------------------------------------------------- *
 * Class class
 * ------------------------------------------------------------------------- */

CordbClass::CordbClass(CordbModule *m, mdTypeDef classMetadataToken)
  : CordbBase(m->GetProcess(), classMetadataToken, enumCordbClass),
    m_needsInit(TRUE),
    m_loadEventSent(FALSE),
    m_hasBeenUnloaded(false),
    m_module(m),
    m_token(classMetadataToken),
    m_typarCount(0),
    m_isValueClass(false),
    m_isValueClassKnown(false),
    m_continueCounterLastSync(0),
    m_varCount(0),
    m_fields(NULL)
{
}


/*
    A list of which resources owned by this object are accounted for.

    UNKNOWN:
        CordbSyncBlockFieldTable m_syncBlockFieldsStatic;
    HANDLED:
        CordbModule*            m_module; // Assigned w/o AddRef()
        DebuggerIPCE_FieldData *m_fields; // Deleted in ~CordbClass
*/

CordbClass::~CordbClass()
{
    // We should have been explicitly neutered before our internal ref went to 0.
    _ASSERTE(IsNeutered());

    if(m_fields)
        delete [] m_fields;
}

// Neutered by CordbModule
void CordbClass::Neuter(NeuterTicket ticket)
{
    RSSmartPtr<CordbClass> pRef(this);
    {
        // Reduce the reference count on the type object for this class
        m_type.Clear();
        CordbBase::Neuter(ticket);
    }
    // Implicit release on pRef
}





HRESULT CordbClass::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugClass)
    {
        *pInterface = static_cast<ICorDebugClass*>(this);
    }
    else if (id == IID_ICorDebugClass2)
    {
        *pInterface = static_cast<ICorDebugClass2*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugClass*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}



HRESULT CordbClass::GetStaticFieldValue(mdFieldDef fieldDef,
                                        ICorDebugFrame *pFrame,
                                        ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT          hr = S_OK;
    *ppValue = NULL;
    BOOL             fSyncBlockField = FALSE;

    // Validate the token.
    if (!GetModule()->m_pIMImport->IsValidToken(fieldDef))
    {
        hr = E_INVALIDARG;
        goto LExit;
    }

    // Make sure we have enough info about the class.
    hr = Init(FALSE);

    if (!SUCCEEDED(hr))
        goto LExit;

    // Uninstantiated generics (eg, Foo<T>) don't have static data. Must use instantiaged (eg Foo<int>)
    // But all CordbClass instances are uninstantiated. So this should fail for all generic types.
    // Normally, debuggers should be using ICorDebugType instead.
    // Though in the forward compat case, they'll hit this.
    if (m_typarCount != 0)
    {
        return CORDBG_E_STATIC_VAR_NOT_AVAILABLE;
    }


    // Lookup the field given its metadata token.
    DebuggerIPCE_FieldData *pFieldData;

    hr = GetFieldInfo(fieldDef, &pFieldData);

    if (hr == CORDBG_E_ENC_HANGING_FIELD)
    {
        // Static fields added with EnC hang off the sync block
        hr = GetSyncBlockField(fieldDef,
                               &pFieldData,
                               NULL);

        if (SUCCEEDED(hr))
            fSyncBlockField = TRUE;
    }

    if (!SUCCEEDED(hr))
        goto LExit;

    hr = CordbClass::GetStaticFieldValue2(GetModule(),
                                          pFieldData,
                                          fSyncBlockField,
                                          Instantiation (),
                                          pFrame,
                                          ppValue);

LExit:
    hr = CordbClass::PostProcessUnavailableHRESULT(hr, GetModule()->m_pIMImport, fieldDef);

    return hr;

}


// Common helper for accessing statics from both CordbClass and CordbType.
/* static */
HRESULT CordbClass::GetStaticFieldValue2(CordbModule *pModule,
                                         DebuggerIPCE_FieldData *pFieldData,
                                         BOOL fSyncBlockField,
                                         const Instantiation &inst,
                                         ICorDebugFrame *pFrame,
                                         ICorDebugValue **ppValue)
{
    FAIL_IF_NEUTERED(pModule);
    INTERNAL_SYNC_API_ENTRY(pModule->GetProcess()); //
    _ASSERTE(pModule->GetProcess()->GetSynchronized());
    HRESULT          hr = S_OK;

    if (!pFieldData->fldIsStatic)
    {
        return CORDBG_E_FIELD_NOT_STATIC;
    }

    REMOTE_PTR pRmtStaticValue = NULL;

    if (!pFieldData->fldIsTLS && !pFieldData->fldIsContextStatic)
    {
        // Statics never move, so we always address them using their absolute address.
        pRmtStaticValue = pFieldData->GetStaticAddress();
    }
    else
    {
        // We've got a thread or context local static

        if( fSyncBlockField )
        {
            // fSyncBlockField is set for fields added with EnC which hang off the SyncBlock
            // thread-local and context-local statics cannot be added with EnC, so we shouldn't be here.
            _ASSERTE(!pFieldData->fldIsTLS );
            _ASSERTE(!pFieldData->fldIsContextStatic );
        }
        else
        {
            // What thread are we working on here.
            if (pFrame == NULL)
            {
                return E_INVALIDARG;
            }

            ICorDebugChain *pChain = NULL;

            IfFailRet (pFrame->GetChain(&pChain));

            CordbChain *c = (CordbChain*)pChain;
            CordbThread *t = c->m_thread;

            // Send an event to the Left Side to find out the address of this field for the given thread.
            DebuggerIPCEvent event;
            pModule->GetProcess()->InitIPCEvent(&event, DB_IPCE_GET_SPECIAL_STATIC, true, pModule->GetAppDomain()->GetADToken());
            event.GetSpecialStatic.fldDebuggerToken = pFieldData->fldDebuggerToken;
            event.GetSpecialStatic.debuggerThreadToken = t->m_debuggerThreadToken;

            // Note: two-way event here...
            hr = pModule->GetProcess()->m_cordb->SendIPCEvent(pModule->GetProcess(), &event, sizeof(DebuggerIPCEvent));
            hr = WORST_HR(hr, event.hr);
            IfFailRet(hr);

            _ASSERTE(event.type == DB_IPCE_GET_SPECIAL_STATIC_RESULT);

            pRmtStaticValue = (BYTE*)event.GetSpecialStaticResult.fldAddress;
        }
    }

    if (pRmtStaticValue == NULL)
    {
        // type probably wasn't loaded yet.
        // The debugger may chose to func-eval the creation of an instance of this type and try again.
        return CORDBG_E_STATIC_VAR_NOT_AVAILABLE;
    }

    SigParser sigParser;
    IfFailRet (pFieldData->GetFieldSignature(pModule, &sigParser));

    CordbType *type;
    IfFailRet (CordbType::SigToType(pModule, &sigParser, inst, &type));

    bool isValueClass;
    IfFailRet (type->IsValueType(&isValueClass));


    // Static value classes are stored as handles so that GC can deal with them properly.  Thus, we need to follow the
    // handle like an objectref.  Do this by forcing CreateValueByType to think this is an objectref. Note: we don't do
    // this for value classes that have an RVA, since they're layed out at the RVA with no handle.
    bool boxed = (isValueClass &&
                  !pFieldData->fldIsRVA &&
                  !pFieldData->fldIsPrimitive &&
                  !pFieldData->fldIsTLS &&
                  !pFieldData->fldIsContextStatic);

    ICorDebugValue *pValue;
    hr = CordbValue::CreateValueByType(pModule->GetAppDomain(),
                                       type,
                                       boxed,
                                       pRmtStaticValue, NULL,
                                       NULL,
                                       NULL,
                                       &pValue);

    if (SUCCEEDED(hr))
    {
        *ppValue = pValue;
    }

    return hr;
}


HRESULT CordbClass::GetParameterizedType(CorElementType et, ULONG32 nTypeArgs, ICorDebugType *ppTypeArgs[], ICorDebugType **pType)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pType, ICorDebugType **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

  // Note: Do not call Init() to find out if its a VC or not.
  // Rather expect the client to tell us. This means the debug client
  // can describe type instantiations not yet seen in the EE.

    if (et != ELEMENT_TYPE_CLASS && et != ELEMENT_TYPE_VALUETYPE)
    {
      return E_INVALIDARG;
    }

    // Prefast overflow check:
    S_UINT32 allocSize = S_UINT32( nTypeArgs ) * S_UINT32( sizeof(CordbType*) );
    if( allocSize.IsOverflow() )
    {
        return E_INVALIDARG;
    }

    CordbAppDomain * classAd = GetAppDomain();

    // Note: casting from (ICorDebugType **) to (CordbType **) is not valid.
    // Offsets may differ.  Copy and validate the type array.
    CordbType **args = (CordbType **) _alloca( allocSize.Value() );
    for (unsigned int i = 0; i<nTypeArgs;i++)
    {
        args[i] = static_cast<CordbType*>( ppTypeArgs[i] );
        CordbAppDomain * ad = args[i]->GetAppDomain();
        if( ad != NULL && ad != classAd )
        {
            return CORDBG_E_APPDOMAIN_MISMATCH;
        }
    }

    CordbType *res;
    HRESULT hr = CordbType::MkType( classAd, et, this, Instantiation(nTypeArgs,args), &res);
    if (FAILED(hr))
        return hr;
    *pType = static_cast<ICorDebugType*>( res );
    _ASSERTE(*pType);
    if (*pType)
        (*pType)->AddRef();
    return S_OK;
}

// Returns true if the field is a static literal.
// In this case, the debugger should get the value from the metadata.
bool IsFieldStaticLiteral(IMetaDataImport *pImport, mdFieldDef fieldDef)
{
    DWORD dwFieldAttr;
    HRESULT hr2 = pImport->GetFieldProps(
        fieldDef,
        NULL,
        NULL,
        0,
        NULL,
        &dwFieldAttr,
        NULL,
        0,
        NULL,
        NULL,
        0);

    if (SUCCEEDED(hr2) && IsFdLiteral(dwFieldAttr))
    {
        return true;
    }

    return false;
}


HRESULT CordbClass::PostProcessUnavailableHRESULT(HRESULT hr,
                                       IMetaDataImport *pImport,
                                       mdFieldDef fieldDef)
{
    if (hr == CORDBG_E_FIELD_NOT_AVAILABLE)
    {
        if (IsFieldStaticLiteral(pImport, fieldDef))
        {
            return CORDBG_E_VARIABLE_IS_ACTUALLY_LITERAL;
        }
    }

    return hr;
}

HRESULT CordbClass::GetModule(ICorDebugModule **ppModule)
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppModule, ICorDebugModule **);

    *ppModule = static_cast<ICorDebugModule*> (m_module);
    m_module->ExternalAddRef();

    return S_OK;
}

HRESULT CordbClass::GetToken(mdTypeDef *pTypeDef)
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pTypeDef, mdTypeDef *);

    _ASSERTE(TypeFromToken(m_token) == mdtTypeDef);

    *pTypeDef = m_token;

    return S_OK;
}

// Set the JMC status on all of our member functions.
// The current implementation just uses the metadata to enumerate all
// methods and then calls SetJMCStatus on each method.
// This isn't great perf, but this should never be needed in a
// perf-critical situation.
HRESULT CordbClass::SetJMCStatus(BOOL fIsUserCode)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // Get the member functions via a meta data interface
    CordbModule * pModule = GetModule();

    // Ensure that our process is in a sane state.
    CordbProcess * pProcess;
    pProcess = pModule->GetProcess();
    _ASSERTE(pProcess != NULL);


    IMetaDataImport * pMI = pModule->m_pIMImport;

    HRESULT hr;

    HCORENUM phEnum = 0;
    mdMethodDef rTokens[100];
    ULONG i;
    ULONG count;

    do
    {
        hr = pMI->EnumMethods(&phEnum, m_token, rTokens, NumItems(rTokens), &count);

        if (FAILED(hr))
        {
            _ASSERTE(phEnum == NULL);
            return hr;
        }

        for (i = 0; i < count; i++)
        {
            // Need the ICorDebugFunction to query for JMC status.
            ICorDebugFunction * pFunc = NULL;
            pModule->GetFunctionFromToken(rTokens[i], &pFunc);
            _ASSERTE(pFunc != NULL);

            ICorDebugFunction2 * pFunc2 = NULL;
            hr = pFunc->QueryInterface(IID_ICorDebugFunction2, (void**) &pFunc2);
            pFunc->Release();
            if (pFunc2 == NULL)
            {
                goto Exit;
            }

            hr = pFunc2->SetJMCStatus(fIsUserCode);
            pFunc2->Release();
            if (FAILED(hr))
            {
                goto Exit;
            }
        }
    }
    while (count > 0);

    _ASSERTE(SUCCEEDED(hr));

Exit:
    pMI->CloseEnum(phEnum);

    return hr;

}


// We have to go the the EE to find out if a class is a value
// class or not.  This is because there is no flag for this, but rather
// it depends on whether the class subclasses System.ValueType (apart
// from System.Enum...).  Replicating all that resoultion logic
// does not seem like a good plan.
//
// We also accept other "evidence" that the class is or isn't a VC, in
// particular:
//   - It is definitely a VC if it has been used after a
//     E_T_VALUETYPE in a signature.
//   - It is definitely not a VC if it has been used after a
//     E_T_CLASS in a signature.
//   - It is definitely a VC if it has been used in combination with
//     E_T_VALUETYPE in one of COM API operations that take both
//     a ICorDebugClass and a CorElementType (e.g. GetParameterizedType)
//
// !!!Note the following!!!!
//   - A class may still be a VC even if it has been
//     used in combination with E_T_CLASS in one of COM API operations that take both
//     a ICorDebugClass and a CorElementType (e.g. GetParameterizedType).
//     We allow the user of the API to specify E_T_CLASS when the VC status
//     is not know or is not important.
HRESULT CordbClass::IsValueClass(bool *pIsValueClass)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    HRESULT hr = S_OK;
    if (!m_isValueClassKnown)
    {
        ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

        *pIsValueClass = false;
        hr = Init(FALSE);
    }
    *pIsValueClass = m_isValueClass;

    return hr;
}

HRESULT CordbClass::GetThisType(const Instantiation &inst, CordbType **pRes)
{
    FAIL_IF_NEUTERED(this);
  // Note: We have to call Init() here to find out if it really a VC or not.
    bool isVC;
    HRESULT hr = IsValueClass(&isVC);
    if (FAILED(hr))
      return hr;

    if (isVC)
    {
        CordbType *ty;
        hr = CordbType::MkType(GetAppDomain(),ELEMENT_TYPE_VALUETYPE,this, inst, &ty);  // OK: this E_T_VALUETYPE will be normalized by MkType
        if (!SUCCEEDED(hr))
            return hr;
        hr = CordbType::MkType(GetAppDomain(),ELEMENT_TYPE_BYREF,0, ty, pRes);
        if (!SUCCEEDED(hr))
            return hr;
    }
    else {
        hr = CordbType::MkType(GetAppDomain(),ELEMENT_TYPE_CLASS,this,inst, pRes);
        if (!SUCCEEDED(hr))
            return hr;
    }

    return hr;
}

HRESULT CordbClass::Init(BOOL fForceInit)
{
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //

    // If we've done a continue since we last time we got hanging static fields,
    // we should clear our our cache, since everything may have moved.
    if (m_continueCounterLastSync < GetProcess()->m_continueCounter)
    {
        m_syncBlockFieldsStatic.Clear();
        m_continueCounterLastSync = GetProcess()->m_continueCounter;
    }

    // We don't have to reinit if the EnC version is up-to-date &
    // we haven't been told to do the init regardless.
    if (!m_needsInit && !fForceInit)
        return S_OK;

    bool fFirstEvent = true;
    bool fGotUnallocatedStatic = false;
    unsigned int fieldIndex = 0;
    unsigned int totalFieldCount = 0;

    INDEBUG(int dbgTotalFieldCount = 0);
    DebuggerIPCEvent *retEvent = NULL;

    _ASSERTE(GetProcess()->GetSynchronized());

    HRESULT hr = S_OK;

    // We need to send to the left side to get real information about
    // the class, including its instance and static variables.
    CordbProcess *pProcess = GetProcess();

    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event,
                           DB_IPCE_GET_CLASS_INFO,
                           false,
                           m_module->GetAppDomain()->GetADToken());
    event.GetClassInfo.metadataToken = m_token;
    event.GetClassInfo.debuggerModuleToken = m_module->m_debuggerModuleToken;
    event.GetClassInfo.typeHandleExact = LSPTR_TYPEHANDLE::NullPtr();
    event.GetClassInfo.typeHandleApprox = LSPTR_TYPEHANDLE::NullPtr();

    hr = pProcess->m_cordb->SendIPCEvent(pProcess, &event,
                                         sizeof(DebuggerIPCEvent));
    hr = WORST_HR(hr, event.hr);

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        goto exit;

    // Wait for events to return from the RC. We expect at least one
    // class info result event.
    retEvent = (DebuggerIPCEvent *) _alloca(CorDBIPC_BUFFER_SIZE);

    do
    {
        hr = pProcess->m_cordb->WaitForIPCEventFromProcess(pProcess,
                                                    m_module->GetAppDomain(),
                                                    retEvent);

        if (!SUCCEEDED(hr))
            goto exit;

        _ASSERTE(retEvent->type == DB_IPCE_GET_CLASS_INFO_RESULT);

        // If this is the first event back from the RC, then create the
        // array to hold the field.
        if (fFirstEvent)
        {
            fFirstEvent = false;

            _ASSERTE(!m_isValueClassKnown || m_isValueClass == retEvent->GetClassInfoResult.isValueClass);

            m_isValueClass = retEvent->GetClassInfoResult.isValueClass;
            m_isValueClassKnown = true;
            m_typarCount = retEvent->GetClassInfoResult.genericArgsCount;
            // If type is a generic  type then use the size in the instantiated type
            m_objectSize = (m_typarCount != 0) ? 0xbadbad : retEvent->GetClassInfoResult.objectSize;
            m_varCount = retEvent->GetClassInfoResult.varCount;

            totalFieldCount = m_varCount;

            // Since we don't keep pointers to the m_fields elements,
            // just toss it & get a new one.
            if (m_fields != NULL)
            {
                delete [] m_fields;
                m_fields = NULL;
            }

            if (totalFieldCount > 0)
            {
                m_fields = new (nothrow) DebuggerIPCE_FieldData[totalFieldCount];

                if (m_fields == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto exit;
                }
            }
        }

        DebuggerIPCE_FieldData *currentFieldData =
            &(retEvent->GetClassInfoResult.fieldData);

        INDEBUG( dbgTotalFieldCount += retEvent->GetClassInfoResult.fieldCount);
        _ASSERTE((int) dbgTotalFieldCount <= (int) totalFieldCount);

        for (unsigned int i = 0; i < retEvent->GetClassInfoResult.fieldCount;
             i++)
        {
            PREFIX_ASSUME(totalFieldCount > 0);
            PREFIX_ASSUME(m_fields != NULL);
            m_fields[fieldIndex] = *currentFieldData;

            if( currentFieldData->fldIsStatic &&
                !currentFieldData->fldIsContextStatic &&
                !currentFieldData->fldIsTLS &&
                currentFieldData->GetStaticAddress() == NULL )
            {
                // The address for a regular static field isn't available yet
                // How can this happen?  Statics appear to get allocated during domain load.
                // There may be some lazieness or a race-condition involved.
                fGotUnallocatedStatic = true;
            }

            currentFieldData++;
            fieldIndex++;
        }

    } // end while
    while( fieldIndex < totalFieldCount );
    _ASSERTE((int) dbgTotalFieldCount == (int) totalFieldCount);


exit:
    if (SUCCEEDED(hr) && !fGotUnallocatedStatic)
    {
        // We sucesffully initialized, no need to ever do this again
        m_needsInit = FALSE;
    }

    return hr;
}

/*
 * DebuggerIPCE_FieldData::GetFieldSignature
 *
 * Get the field's full metadata signature. This may be cached, but for dynamic modules we'll always read it from
 * the metadata.
 *
 * Parameters:
 *    pModule - pointer to the module that contains the field
 *
 *    pSigParser - OUT: the full signature for the field.
 *
 * Returns:
 *    HRESULT for success or failure.
 *
 */
HRESULT DebuggerIPCE_FieldData::GetFieldSignature(CordbModule *pModule, 
                                                  SigParser *pSigParser)
{
    INTERNAL_SYNC_API_ENTRY(pModule->GetProcess());
    HRESULT hr = S_OK;

    PCCOR_SIGNATURE fieldSignature = NULL;
    ULONG size = ((ULONG) -1);

    _ASSERTE(pSigParser != NULL);

    // If the module is dynamic, there had better not be a cached field signature.
    _ASSERTE(!pModule->IsDynamic() || (fldSignatureCache == NULL));

    // If the field signature cache is null, or if this is a dynamic module, then go read the signature from the
    // matadata. We always read from the metadata for dynamic modules because our metadata blob is constantly
    // getting deleted and re-allocated. If we kept a pointer to the signature, we'd end up pointing to bad data.
    if (fldSignatureCache == NULL)
    {
        // Go to the metadata for all fields: previously the left-side tranferred over
        // single-byte signatures as part of the field info.  Since the left-side
        // goes to the metadata anyway, and we already fetch plenty of other metadata,
        // I don't believe that fetching it here instead of transferring it over
        // is going to slow things down at all, and
        // in any case will not be where the primary optimizations lie...

        IfFailRet(pModule->m_pIMImport->GetFieldProps(fldMetadataToken, NULL, NULL, 0, NULL, NULL,
                                                      &fieldSignature,
                                                      &size,
                                                      NULL, NULL, NULL));

        // Point past the calling convention
        CorCallingConvention conv;

        // Move pointer, 
        BYTE * pOldPtr = (BYTE*) fieldSignature;
        conv = (CorCallingConvention) CorSigUncompressData(fieldSignature);
        _ASSERTE(conv == IMAGE_CEE_CS_CALLCONV_FIELD);
        size -= (ULONG) (((BYTE*) fieldSignature) - pOldPtr); // since we updated filedSignature, adjust size

        // Although the pointer will keep updating, the size should be the same. So we assert that.
        _ASSERTE((fldSignatureCacheSize == 0) || (fldSignatureCacheSize == size));
        
        // Cache the value for non-dynamic modules, so this is faster later.
        // Since we're caching in a DebuggerIPCE_FieldData, we can't store the actual SigParser object.
        if (!pModule->IsDynamic())
        {
            fldSignatureCache = fieldSignature;
            fldSignatureCacheSize = size;
        }      
    }
    else
    {
        // We have a cached value, so return it. Note: we should never have a cached value for a field in a dynamic
        // module.
        CONSISTENCY_CHECK_MSGF((!pModule->IsDynamic()),
                               ("We should never cache a field signature in a dynamic module! Module=%p This=%p",
                                pModule, this));

        fieldSignature  = fldSignatureCache;
        size            = fldSignatureCacheSize;
    }

    _ASSERTE(fieldSignature != NULL);
    _ASSERTE(size != ((ULONG) -1));
    *pSigParser = SigParser(fieldSignature, size);
    return hr;
}

// object is NULL if this is being called from GetStaticFieldValue
HRESULT CordbClass::GetSyncBlockField(mdFieldDef fldToken,
                                      DebuggerIPCE_FieldData **ppFieldData,
                                      CordbObjectValue *object)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess());

    HRESULT hr = S_OK;
    _ASSERTE(object == NULL || !object->IsNeutered() );

    if (m_typarCount > 0)
    {
        _ASSERTE(!"sync block field not yet implemented on constructed types!");
        return E_FAIL;
    }

    BOOL fStatic = (object == NULL);

    // Static stuff should _NOT_ be cleared, since they stick around.  Thus
    // the separate tables.

    // We must get new copies each time we call continue b/c we get the
    // actual Object ptr from the left side, which can move during a GC.

    DebuggerIPCE_FieldData *pInfo = NULL;

    if (!fStatic)
    {
        pInfo = object->m_syncBlockFieldsInstance.GetFieldInfo(fldToken);

        // We've found a previously located entry
        if (pInfo != NULL)
        {
            (*ppFieldData) = pInfo;
            return S_OK;
        }
    }
    else
    {
        pInfo = m_syncBlockFieldsStatic.GetFieldInfo(fldToken);

        // We've found a previously located entry
        if (pInfo != NULL)
        {
            (*ppFieldData) = pInfo;
            return S_OK;
        }
    }

    _ASSERTE(pInfo == NULL);

    // We're not going to be able to get the instance-specific field
    // if we can't get the instance.
    if (!fStatic && object->m_info.objRefBad)
    {
        return CORDBG_E_ENC_HANGING_FIELD;
    }

    // Go get this particular field.
    DebuggerIPCEvent event;
    CordbProcess *process = GetModule()->GetProcess();
    _ASSERTE(process != NULL);

    process->InitIPCEvent(&event,
                          DB_IPCE_GET_SYNC_BLOCK_FIELD,
                          true, // two-way event
                          m_module->GetAppDomain()->GetADToken());

    LSPTR_DMODULE pDModule;
    pDModule.Set((void*) GetModule()->m_id);
    event.GetSyncBlockField.objectTypeData.debuggerModuleToken = pDModule;


    hr = GetToken(&(event.GetSyncBlockField.objectTypeData.metadataToken));
    _ASSERTE(!FAILED(hr));
    event.GetSyncBlockField.fldToken = fldToken;

    if (fStatic)
    {
        event.GetSyncBlockField.pObject = NULL;
        event.GetSyncBlockField.objectTypeData.elementType = ELEMENT_TYPE_MAX;
        event.GetSyncBlockField.offsetToVars = NULL;
    }
    else
    {
        _ASSERTE(object != NULL);

        event.GetSyncBlockField.pObject = (void *)object->m_id;
        event.GetSyncBlockField.objectTypeData.elementType = object->m_info.objTypeData.elementType;
        event.GetSyncBlockField.offsetToVars = object->m_info.objOffsetToVars;
    }

    // Note: two-way event here...
    hr = process->m_cordb->SendIPCEvent(process,
                                        &event,
                                        sizeof(DebuggerIPCEvent));

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    _ASSERTE(event.type == DB_IPCE_GET_SYNC_BLOCK_FIELD_RESULT);

    if (!SUCCEEDED(event.hr))
    {
        return event.hr;
    }

    _ASSERTE(fStatic == event.GetSyncBlockFieldResult.fStatic);

    // Save the results for later.
    pInfo = &(event.GetSyncBlockFieldResult.fieldData);

    _ASSERTE(!pInfo->fldIsStatic == !fStatic);

    if (!fStatic)
    {
        object->m_syncBlockFieldsInstance.AddFieldInfo(pInfo);
    }
    else
    {
        m_syncBlockFieldsStatic.AddFieldInfo(pInfo);
    }

    // Pass the data out.
    (*ppFieldData) = pInfo;

    return S_OK;
}


HRESULT CordbClass::GetFieldInfo(mdFieldDef fldToken, DebuggerIPCE_FieldData **ppFieldData)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //
    HRESULT hr = S_OK;

    *ppFieldData = NULL;

    hr = Init(FALSE);

    if (!SUCCEEDED(hr))
        return hr;

    return SearchFieldInfo(GetModule(), m_varCount, m_fields, m_token, fldToken, ppFieldData);
}


/* static */ HRESULT CordbClass::SearchFieldInfo(CordbModule *module, unsigned int cData, DebuggerIPCE_FieldData *data, mdTypeDef classToken, mdFieldDef fldToken, DebuggerIPCE_FieldData **ppFieldData)
{
    unsigned int i;

    HRESULT hr = S_OK;
    for (i = 0; i < cData; i++)
    {
        if (data[i].fldMetadataToken == fldToken)
        {
            // If the storage for this field isn't yet available (i.e. it is newly added with EnC)
            if (!data[i].fldStorageAvailable)
            {
                // If we're a static literal, then return special HR to let
                // debugger know that it should look it up via the metadata.
                // Check fldIsStatic first b/c that's fast.
                if (data[i].fldIsStatic)
                {
                    if (IsFieldStaticLiteral(module->m_pIMImport, fldToken))
                    {
                        return CORDBG_E_VARIABLE_IS_ACTUALLY_LITERAL;
                    }
                }

                return CORDBG_E_ENC_HANGING_FIELD; // caller should get instance-specific info.
            }

            *ppFieldData = &(data[i]);
            return S_OK;
        }
    }

    // Hmmm... we didn't find the field on this class. See if the field really belongs to this class or not.
    mdTypeDef classTok;

    hr = module->m_pIMImport->GetFieldProps(fldToken, &classTok, NULL, 0, NULL, NULL, NULL, 0, NULL, NULL, NULL);

    if (FAILED(hr))
        return hr;

    if (classTok == (mdTypeDef) classToken)
    {
        // Well, the field belongs in this class. The assumption is that the Runtime optimized the field away.
        return CORDBG_E_FIELD_NOT_AVAILABLE;
    }

    // Well, the field doesn't even belong to this class...
    return E_INVALIDARG;
}

/* ------------------------------------------------------------------------- *
 * Function class
 * ------------------------------------------------------------------------- */

CordbFunction::CordbFunction(CordbModule *m,
                             mdMethodDef funcMetadataToken,
                             SIZE_T funcRVA,
                             SIZE_T enCVersion)
  : CordbBase(m->GetProcess(), funcMetadataToken, enumCordbFunction), m_module(m), m_class(NULL),
    m_ILCode(NULL),
    m_jitinfos(1),
    m_token(funcMetadataToken),
    m_functionRVA(funcRVA),
    m_dwEnCVersionNumber(enCVersion),
    m_pPrevVersion(NULL),
    m_isNativeImpl(FALSE),
    m_fCachedMethodValuesValid(FALSE),
    m_argCountCached(0),
    m_isStaticCached(FALSE),
    m_fCachedLocalValuesValid(FALSE),
    m_localVarSigToken(mdSignatureNil),
    m_localVarCachedCount(0)
{
    m_methodSigParserCached = SigParser(NULL, 0);
    m_localSigParserCached = SigParser(NULL, 0);

    _ASSERTE(enCVersion >= CorDB_DEFAULT_ENC_FUNCTION_VERSION);
}



/*
    A list of which resources owned by this object are accounted for.

    UNKNOWN:
        ICorJitInfo::NativeVarInfo *m_nativeInfo;

    HANDLED:
        CordbModule             *m_module; // Assigned w/o AddRef()
        CordbClass              *m_class; // Assigned w/o AddRef()
*/

CordbFunction::~CordbFunction()
{
    // We should have been explicitly neutered before our internal ref went to 0.
    _ASSERTE(IsNeutered());

    // Since we've been neutered, we shouldn't have any References to release and
    // our hash of JitInfos should be empty.
    _ASSERTE(m_ILCode == NULL);
    _ASSERTE(m_pPrevVersion == NULL);
}

// Neutered by CordbModule
void CordbFunction::Neuter(NeuterTicket ticket)
{
    RSSmartPtr<CordbFunction> pRef(this);
    {
        // Neuter any/all IL & Native CordbCode objects
        if (m_ILCode != NULL)
        {
            m_ILCode->Neuter(ticket);
            m_ILCode.Assign(NULL); // this will internal release.
        }

        // Neuter & Release the Prev-Function list.
        if (m_pPrevVersion != NULL)
        {
            m_pPrevVersion->Neuter(ticket);
            m_pPrevVersion.Assign(NULL); // this will internal release.
        }

        NeuterAndClearHashtable(&m_jitinfos, ticket);

        CordbBase::Neuter(ticket);
    }
    // Implicit release
}


//-----------------------------------------------------------------------------
// Used to create a CordbFunction object from a given data.
// Useful for lazily created CordbFunctions (such as in stack traces).
//-----------------------------------------------------------------------------
HRESULT CordbFunction::LookupOrCreateFromFuncData(CordbProcess *pProcess, CordbAppDomain *pAppDomain,
                                                  DebuggerIPCE_FuncData *data, SIZE_T enCVersion, CordbFunction **ppRes)
{
    CordbModule* pFunctionModule = pAppDomain->LookupModule(data->funcDebuggerModuleToken);
    PREFIX_ASSUME(pFunctionModule != NULL);

    // Does this function already exist?
    CordbFunction *pFunction = NULL;

    HRESULT hr = S_OK;

    hr = pFunctionModule->LookupOrCreateFunction(data->funcMetadataToken, data->funcRVA, enCVersion, &pFunction);

    if (FAILED(hr))
    {
            return hr;
    }

    PREFIX_ASSUME(pFunction != NULL);
    pFunction->SetLocalVarToken(data->localVarSigToken);

    // Does this function have a class?
    if ((pFunction->m_class == NULL) && (data->classMetadataToken != mdTypeDefNil))
    {
        // No. Go ahead and create the class.
        CordbModule* pClassModule = pAppDomain->LookupModule(data->funcDebuggerModuleToken);
        PREFIX_ASSUME(pClassModule != NULL);

        // Does the class already exist?
        CordbClass *pClass;
        hr = pClassModule->LookupOrCreateClass(data->classMetadataToken,
            &pClass);
        _ASSERTE(SUCCEEDED(hr) || !"FAILURE");

        if (!SUCCEEDED(hr))
            return hr;

        _ASSERTE(pClass != NULL);
        pFunction->m_class = pClass;
    }
    *ppRes = pFunction;
    return hr;

}

HRESULT CordbFunction::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugFunction)
    {
        *pInterface = static_cast<ICorDebugFunction*>(this);
    }
    else if (id == IID_ICorDebugFunction2)
    {
        *pInterface = static_cast<ICorDebugFunction2*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugFunction*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}


HRESULT CordbFunction::GetModule(ICorDebugModule **ppModule)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppModule, ICorDebugModule **);

    HRESULT hr = S_OK;

    // Module is set on creation, so just return it.
    *ppModule = static_cast<ICorDebugModule*> (m_module);
    m_module->ExternalAddRef();

    return hr;
}

//-----------------------------------------------------------------------------
// Public function to get ICorDebugClass that this function is in.
//-----------------------------------------------------------------------------
HRESULT CordbFunction::GetClass(ICorDebugClass **ppClass)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppClass, ICorDebugClass **);
    ATT_ALLOW_LIVE_DO_STOPGO(GetProcess());
    *ppClass = NULL;

    HRESULT hr = S_OK;

    if (m_class == NULL)
    {
        // We're not looking for any particular version, just
        // the class info.  This seems like the best version to request
        hr = Populate();

        if (FAILED(hr))
            goto LExit;
    }

    *ppClass = (ICorDebugClass*) m_class;

LExit:
    if (FAILED(hr))
        return hr;

    if (*ppClass)
    {
        m_class->ExternalAddRef();
        return S_OK;
    }
    else
        return S_FALSE;
}

//-----------------------------------------------------------------------------
// Public function to get the metadata token for this function.
//-----------------------------------------------------------------------------
HRESULT CordbFunction::GetToken(mdMethodDef *pMemberDef)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pMemberDef, mdMethodDef *);


    // Token is set on creation, so no updating needed.

    CONSISTENCY_CHECK_MSGF((TypeFromToken(m_token) == mdtMethodDef),
        ("CordbFunction token (%08x) is not a mdtMethodDef. This=%p", m_token, this));

    *pMemberDef = m_token;
    return S_OK;
}

//-----------------------------------------------------------------------------
// Public function to get an ICorDebugCode object for this function.
// If we EnC, we get a new ICorDebugFunction, so the IL code & function
// should be 1:1.
//-----------------------------------------------------------------------------
HRESULT CordbFunction::GetILCode(ICorDebugCode **ppCode)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppCode, ICorDebugCode **);
    ATT_ALLOW_LIVE_DO_STOPGO(GetProcess());

    *ppCode = NULL;
    HRESULT hr = S_OK;

    // Get the code object.
    CordbCode *pCode = NULL;
    hr = GetCode(&pCode);
    _ASSERTE((pCode == NULL) == FAILED(hr));

    if (FAILED(hr))
        return hr;

    *ppCode = (ICorDebugCode*)pCode;

    return hr;
}

// Note that this gets a pretty much random version of the native code when the
// function is a generic method that gets JITted more than once, e.g. for generics.
// Use EnuemrateNativeCode instead in this case.
HRESULT CordbFunction::GetNativeCode(ICorDebugCode **ppCode)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppCode, ICorDebugCode **);
    ATT_ALLOW_LIVE_DO_STOPGO(GetProcess());

    HRESULT hr = S_OK;

    // Make sure native code is updated before we go searching it.
    hr = Populate();
    if (FAILED(hr))
        return hr;

    CordbCode *pCode = NULL;
    HASHFIND srch;
    for (CordbJITInfo *ji = m_jitinfos.FindFirst(&srch);
          ji != NULL;
          (ji = m_jitinfos.FindNext(&srch)))
    {
        if (ji->m_nativecode->m_nVersion == m_dwEnCVersionNumber)
        {
            pCode = ji->m_nativecode;
            break;
        }
    }

    if (pCode == NULL)
    {
        hr = CORDBG_E_CODE_NOT_AVAILABLE;
    }
    else
    {
        pCode->ExternalAddRef();
        *ppCode = pCode;
        hr = S_OK;
    }

    return hr;
}

//-----------------------------------------------------------------------------
// If the IL code object exists, then return it,
// else create it w/ the info from the params.
//
// nVer - ignored, should be removed
// ppCode - out param to take code.
// rest of the parameters are used to build an IL code.
//
// Returns S_OK if *ppCode is valid, else some failure.
//-----------------------------------------------------------------------------
HRESULT CordbFunction::GetOrCreateILCode(
    CordbCode **ppCode,
    REMOTE_PTR startAddress,
    SIZE_T size,
    SIZE_T nVersion
)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppCode, ICorDebugCode **);

    _ASSERTE(*ppCode == NULL && "Common source of errors is getting addref'd copy here and never Release()ing it");
    *ppCode = NULL;

    // If we don't have it, create it.
    if (m_ILCode == NULL)
    {
        m_ILCode.Assign(CordbCode::BuildILCode(
            this,
            startAddress,
            size,
            nVersion
            ));

        if (m_ILCode == NULL)
        {
            return E_OUTOFMEMORY;
        }
    }

    _ASSERTE(m_ILCode != NULL);

    // If everything succeeded, we set the IL code object (it's an outparam here).
    *ppCode = m_ILCode;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Get the IL code.
//
// ppCode - out parameter, the IL code object for this function.
//-----------------------------------------------------------------------------
HRESULT CordbFunction::GetCode(CordbCode **ppCode)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //
    VALIDATE_POINTER_TO_OBJECT(ppCode, ICorDebugCode **);

    _ASSERTE(*ppCode == NULL && "Common source of errors is getting addref'd copy here and never Release()ing it");
    *ppCode = NULL;

    // Its okay to do this if the process is not sync'd.
    CORDBRequireProcessStateOK(GetProcess());

    HRESULT hr = S_OK;
    CordbCode *pCode = NULL;

    hr = Populate();
    if (FAILED(hr))
    {
        return hr;
    }

    // It's possible that m_ILCode will still be NULL.
    pCode = m_ILCode;

    if (pCode != NULL)
    {
        pCode->ExternalAddRef();
        *ppCode = pCode;

        return hr;
    }
    else
    {
        return CORDBG_E_CODE_NOT_AVAILABLE;
    }
}

HRESULT CordbFunction::CreateBreakpoint(ICorDebugFunctionBreakpoint **ppBreakpoint)
{
    HRESULT hr = S_OK;

    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppBreakpoint, ICorDebugFunctionBreakpoint **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    ICorDebugCode *pCode = NULL;

    // Use the IL code so that we stop after the prolog
    hr = GetILCode(&pCode);

    if (FAILED(hr))
        goto LError;

    hr = pCode->CreateBreakpoint(0, ppBreakpoint);

LError:
    if (pCode != NULL)
        pCode->Release();

    return hr;
}


//-----------------------------------------------------------------------------
// Public function to get signature.
//-----------------------------------------------------------------------------
HRESULT CordbFunction::GetLocalVarSigToken(mdSignature *pmdSig)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pmdSig, mdSignature *);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = Populate();
    if (FAILED(hr))
        return hr;

    *pmdSig = m_localVarSigToken;

    return S_OK;
}


HRESULT CordbFunction::GetCurrentVersionNumber(ULONG32 *pnCurrentVersion)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pnCurrentVersion, ULONG32 *);

    HRESULT hr = S_OK;

    // the most current version will always be the one found.
    CordbFunction* curFunc = m_module->LookupFunction(m_token);

    // will always find at least ourself
    PREFIX_ASSUME(curFunc != NULL);

    *pnCurrentVersion = (ULONG32)(curFunc->m_dwEnCVersionNumber);


    return hr;
}

HRESULT CordbFunction::GetVersionNumber(ULONG32 *pnVersion)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pnVersion, ULONG32 *);

    if (! this->GetProcess()->SupportsVersion(ver_ICorDebugFunction2))
    {
        return E_NOTIMPL;
    }

    *pnVersion = (ULONG32)m_dwEnCVersionNumber;


    return S_OK;
}


//-----------------------------------------------------------------------------
// Lazily Update the information in the CordbFunction object
// If the function doesn't have IL, we fail.
// Else we get DebuggerIPCE_FuncData from the LS.
// And we create CordbClass / CordbCodes (IL) objects if not already created.
//
//-----------------------------------------------------------------------------
HRESULT CordbFunction::Populate()
{
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    HRESULT hr = S_OK;
    CordbProcess* pProcess = m_module->GetProcess();

    _ASSERTE(m_token != mdMethodDefNil);

    // Bail now if we've already discovered that this function is implemented natively as part of the Runtime.
    if (m_isNativeImpl)
        return CORDBG_E_FUNCTION_NOT_IL;

    // Caller should have guaranteed we're stopped.
    _ASSERTE(pProcess->IsStopped());

    // Figure out if this function is implemented as a native part of the Runtime. If it is, then this ICorDebugFunction
    // is just a container for certian Right Side bits of info, i.e., module, class, token, etc.
    DWORD attrs;
    DWORD implAttrs;
    ULONG ulRVA;
    BOOL    isDynamic;

    IfFailRet( GetModule()->m_pIMImport->GetMethodProps(m_token, NULL, NULL, 0, NULL,
                                     &attrs, NULL, NULL, &ulRVA, &implAttrs) );
    isDynamic = GetModule()->IsDynamic();

    BOOL isEnC = (GetModule()->m_EnCCount != 0);
    if (IsMiNative(implAttrs) || (isDynamic == FALSE && isEnC == FALSE && ulRVA == 0))
    {
        m_isNativeImpl = true;
        return CORDBG_E_FUNCTION_NOT_IL;
    }

    // If we've already populated everything, then there's nothing else to do here,
    // so just return now and don't bother sending the IPC Event.
    if ((m_ILCode != NULL) && (m_jitinfos.GetCount() > 0) && (m_class != NULL))
    {
        return S_OK;
    }


    // Make sure the Left Side is running free before trying to send an event to it.
    DebuggerIPCEvent event;
    {
        // Send the get function data event to the RC.
        pProcess->InitIPCEvent(&event, DB_IPCE_GET_FUNCTION_DATA, true, m_module->GetAppDomain()->GetADToken());
        event.GetFunctionData.funcMetadataToken = m_token;
        event.GetFunctionData.funcDebuggerModuleToken = m_module->m_debuggerModuleToken;
        event.GetFunctionData.nVersion = m_dwEnCVersionNumber;

        _ASSERTE(m_module->m_debuggerModuleToken != NULL);

        // Note: two-way event here...
        hr = pProcess->m_cordb->SendIPCEvent(pProcess, &event, sizeof(DebuggerIPCEvent));
        hr = WORST_HR(hr, event.hr);
    }

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        return hr;
    _ASSERTE(event.type == DB_IPCE_GET_FUNCTION_DATA_RESULT);

    // Fill in the proper function data.
    m_functionRVA = event.FunctionData.basicData.funcRVA;

    // Should we make or fill in some class data for this function?
    if ((m_class == NULL) && (event.FunctionData.basicData.classMetadataToken != mdTypeDefNil))
    {
        CordbAssembly *pAssembly = m_module->GetCordbAssembly();
        PREFIX_ASSUME(pAssembly != NULL);

        CordbModule* pClassModule = pAssembly->GetAppDomain()->LookupModule(event.FunctionData.basicData.funcDebuggerModuleToken);
        PREFIX_ASSUME(pClassModule != NULL);

        CordbClass *pClass;
        hr = pClassModule->LookupOrCreateClass(event.FunctionData.basicData.classMetadataToken, &pClass);

        if (!SUCCEEDED(hr))
            goto exit;

        _ASSERTE(pClass != NULL);
        m_class = pClass;
    }

    // Do we need to make any code objects for this function?
    LOG((LF_CORDB,LL_INFO10000,"R:CF::Pop: looking for IL code, version 0x%x\n", event.FunctionData.basicData.currentEnCVersion));

    if ((m_ILCode == NULL) && event.FunctionData.basicData.ilStartAddress != 0)
    {
        LOG((LF_CORDB,LL_INFO10000,"R:CF::Pop: not found, creating...\n"));

        // If everything succeeded, we set the IL code object (it's an outparam here).
        _ASSERTE(m_ILCode == NULL);
        m_ILCode.Assign(CordbCode::BuildILCode(
            this,
            event.FunctionData.basicData.ilStartAddress,
            event.FunctionData.basicData.ilSize,
            event.FunctionData.basicData.currentEnCVersion
            ));

        if (m_ILCode == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto exit;
        }
    }

    // Ok to set this multiple times.
    SetLocalVarToken(event.FunctionData.basicData.localVarSigToken);

    // To make sure we don't break V1 behaviour, e.g. GetNativeCode,
    // we allow for CordbFunction's to be associated
    // with at least one CordbJITInfo.  We could implement an iterator to go and fetch all the
    // native code blobs and then just choose one, but we may as well keep it simple for the
    // moment and just return one specified here.

    if (event.FunctionData.possibleNativeData.nativeCodeMethodDescToken != NULL &&
        event.FunctionData.possibleNativeData.nativeStartAddressPtr != 0)
    {
        CordbJITInfo *pInfo = NULL;
        hr = CordbJITInfo::LookupOrCreateFromJITData(this, &event.FunctionData.basicData, &event.FunctionData.possibleNativeData, &pInfo);
        if (!SUCCEEDED(hr))
        {
            goto exit;
    }
    }
    else
    {
        LOG((LF_CORDB,LL_INFO10000,"R:CF:LOCFJD: native code won't be available for ver:0x%x, md:0x%x, dji:0x%x, map:0x%x, nativeStart=0x%08x,nativeSize=0x%08x\n",
            event.FunctionData.possibleNativeData.enCVersion,
            LsPtrToCookie(event.FunctionData.possibleNativeData.nativeCodeMethodDescToken),
            LsPtrToCookie(event.FunctionData.possibleNativeData.nativeCodeJITInfoToken),
            event.FunctionData.possibleNativeData.ilToNativeMapAddr,
            event.FunctionData.possibleNativeData.nativeStartAddressPtr,
            event.FunctionData.possibleNativeData.nativeSize));
    }



    _ASSERTE(SUCCEEDED(hr)); // shouldn't have made it this far if we failed.

exit:

    return hr;
}

// Set the JMC status of this function.
// return S_OK on success.
HRESULT CordbFunction::SetJMCStatus(BOOL fIsUserCode)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    HRESULT hr = S_OK;

    LOG((LF_CORDB,LL_INFO10000,"CordbFunction::SetJMCStatus to %d, (token=0x%08x, module=%p)\n",
        fIsUserCode, m_token, m_module));

    // Make sure the Left-Side is in a good state.
    FAIL_IF_NEUTERED(this);
    CordbProcess* pProcess = m_module->GetProcess();
    ATT_REQUIRE_STOPPED_MAY_FAIL(pProcess);



    // Send an event to the LS to keep it updated.

    // Validation - we can't have any active JMC steppers. That may lead to very
    // goofy & undefined behavior.
#ifdef _DEBUG

    // We'll use the ICorDebug API (instead of the classes directly) here because
    // the API's enumeration is much cleaner.
    ICorDebugStepperEnum * pEnum = NULL;
    CordbAppDomain * pAppDomain = m_module->GetAppDomain();
    hr = pAppDomain->EnumerateSteppers(&pEnum);
    if (SUCCEEDED(hr))
    {
        ICorDebugStepper * pStepper = NULL;
        ULONG count;
        _ASSERTE(pEnum != NULL);

        while(true)
        {
            hr = pEnum->Next(1, &pStepper, &count);
            if (count == 0)
                break;

            _ASSERTE(pStepper != NULL);
            // ICorDebugSteppers don't expose their stop mask, so we must
            // grab it from the class.
            CordbStepper * p = static_cast<CordbStepper*> (pStepper);
            if (p->m_fIsJMCStepper)
            {
                _ASSERTE(!"API misuse - can't SetJMCStatus with outstanding JMC steppers");
            }
            pStepper->Release();
        }
        pEnum->Release();
    }

#endif


    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event, DB_IPCE_SET_METHOD_JMC_STATUS, true, m_module->GetAppDomain()->GetADToken());
    event.SetJMCFunctionStatus.debuggerModuleToken = m_module->m_debuggerModuleToken;
    event.SetJMCFunctionStatus.funcMetadataToken   = m_token;
    event.SetJMCFunctionStatus.dwStatus            = fIsUserCode;

    _ASSERTE(m_module->m_debuggerModuleToken != NULL);

    // Note: two-way event here...
    hr = pProcess->m_cordb->SendIPCEvent(pProcess, &event, sizeof(DebuggerIPCEvent));

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        return hr;

    _ASSERTE(event.type == DB_IPCE_SET_METHOD_JMC_STATUS_RESULT);

    return event.hr;
}

// Get the JMC status of this function
// returns S_OK if *pfIsUserCode is set.
// *pfIsUserCode = true iff this function is user code, else false.
HRESULT CordbFunction::GetJMCStatus(BOOL * pfIsUserCode)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    VALIDATE_POINTER_TO_OBJECT(pfIsUserCode, BOOL*);


    _ASSERTE(pfIsUserCode != NULL);
    if (pfIsUserCode == NULL)
        return E_INVALIDARG;


    // Make sure the process is in a sane state.
    CordbProcess* pProcess = m_module->GetProcess();
    _ASSERTE(pProcess != NULL);

    // Ask the left-side if a method is user code or not.
    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event, DB_IPCE_GET_METHOD_JMC_STATUS, true, m_module->GetAppDomain()->GetADToken());
    event.SetJMCFunctionStatus.debuggerModuleToken = m_module->m_debuggerModuleToken;
    event.SetJMCFunctionStatus.funcMetadataToken   = m_token;

    _ASSERTE(m_module->m_debuggerModuleToken != NULL);

    // Note: two-way event here...
    HRESULT hr = pProcess->m_cordb->SendIPCEvent(pProcess, &event, sizeof(DebuggerIPCEvent));

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        return hr;

    _ASSERTE(event.type == DB_IPCE_GET_METHOD_JMC_STATUS_RESULT);

    // update our internal copy of the status.
    BOOL fIsUserCode = event.SetJMCFunctionStatus.dwStatus;

    *pfIsUserCode = fIsUserCode;

    return event.hr;
}

//-----------------------------------------------------------------------------
// Called to create a CordbJitInfo (if it's not already created).
// Used by CordbFunction::Populate() & Stack Trace code.
//
// pFunction - the function calling us.
// currentFuncData - IL info about the function
// currentJITFundData - Jitted info about the function
// ppRes - out parameter, for created CordbJitInfo
//-----------------------------------------------------------------------------
HRESULT CordbJITInfo::LookupOrCreateFromJITData(CordbFunction *pFunction, DebuggerIPCE_FuncData *currentFuncData, DebuggerIPCE_JITFuncData* currentJITFuncData, CordbJITInfo **ppRes)
{
    INTERNAL_API_ENTRY(this);

    _ASSERTE(ppRes != NULL);
    HRESULT hr = S_OK;

    _ASSERTE(currentJITFuncData->nativeStartAddressPtr != NULL);

    // We use the JITInfoToken as the key if it is available.  This covers the EnC case where
    // we have different JITInfoTokens for the same MethodDesc.  There is no EnC on methods
    // that do not have JITInfoTokens, so in that case we use the MethodDescToken as the key.
    // Note that these tokens are guaranteed to be distinct.
    *ppRes = pFunction->m_jitinfos.GetBase((UINT_PTR) (currentJITFuncData->nativeStartAddressPtr));

    if (*ppRes != NULL)
    {
        return S_OK;
    }

        LOG((LF_CORDB,LL_INFO10000,"R:CT::RSCreating code w/ ver:0x%x, md:0x%x, dji:0x%x, map:0x%x, nativeStart=0x%08x,nativeSize=0x%08x\n",
        currentJITFuncData->enCVersion,
            LsPtrToCookie(currentJITFuncData->nativeCodeMethodDescToken),
            LsPtrToCookie(currentJITFuncData->nativeCodeJITInfoToken),
            currentJITFuncData->ilToNativeMapAddr,
            currentJITFuncData->nativeStartAddressPtr,
            currentJITFuncData->nativeSize));

        CordbCode *pCode = CordbCode::BuildNativeCode(pFunction,  currentJITFuncData);

        if (pCode == NULL)
        {
            hr = E_OUTOFMEMORY;
            return hr;
        }

        *ppRes = new (nothrow) CordbJITInfo(pFunction, pCode, currentJITFuncData->isInstantiatedGeneric);

        if (*ppRes == NULL)
        {
            delete pCode;
            hr = E_OUTOFMEMORY;
            return hr;
        }
        hr = pFunction->m_jitinfos.AddBase(*ppRes);
        if (FAILED(hr))
          {
            delete pCode;
            delete *ppRes;
            return hr;
          }

    _ASSERTE(*ppRes != NULL);

    return hr;
}

// This function is located at this point in the file only to
// help SourceDepot match this up with the original source file
// we've derived it from...
//
// LoadNativeInfo loads from the left side any native variable info
// from the JIT.
//
HRESULT CordbJITInfo::LoadNativeInfo(void)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //

    HRESULT hr = S_OK;

    // Caller was supposed to sync us.
    _ASSERTE(GetProcess()->GetSynchronized());


    // Then, if we've either never done this before (no info), or we have, but the version number has increased, we
    // should try and get a newer version of our JIT info.
    if(m_nativeInfoValid)
        return S_OK;

    // You can't do this if the function is implemented as part of the Runtime.
    if (m_function->m_isNativeImpl)
        return CORDBG_E_FUNCTION_NOT_IL;

    DebuggerIPCEvent *retEvent = NULL;
    bool wait = true;

    // We might be here b/c we've done some EnCs, but we also may have pitched some code, so don't overwrite this until
    // we're sure we've got a good replacement.
    unsigned int argumentCount = 0;
    unsigned int nativeInfoCount = 0;
    unsigned int nativeInfoCountTotal = 0;
    ICorJitInfo::NativeVarInfo *nativeInfo = NULL;

    // We need to send to the left side to get real information
    // about the class, including its instance and static variables.
    CordbProcess *pProcess = GetProcess();

    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event, DB_IPCE_GET_JIT_INFO, false, GetAppDomain()->GetADToken());
    event.GetJITInfo.nativeCodeMethodDescToken = m_nativecode->m_nativeCodeMethodDescToken;
    event.GetJITInfo.nativeCodeJITInfoToken = m_nativecode->m_nativeCodeJITInfoToken;

    hr = pProcess->m_cordb->SendIPCEvent(pProcess, &event, sizeof(DebuggerIPCEvent));
    hr = WORST_HR(hr, event.hr);

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        goto exit;

    // Wait for events to return from the RC. We expect at least one jit info result event.
    retEvent = (DebuggerIPCEvent *) _alloca(CorDBIPC_BUFFER_SIZE);

    while (wait)
    {
        unsigned int currentInfoCount = 0;

        hr = pProcess->m_cordb->WaitForIPCEventFromProcess(pProcess, GetAppDomain(), retEvent);

        if (!SUCCEEDED(hr))
            goto exit;

        _ASSERTE(retEvent->type == DB_IPCE_GET_JIT_INFO_RESULT);

        // If this is the first event back from the RC, then create the array to hold the data.
        if ((retEvent->GetJITInfoResult.totalNativeInfos > 0) && (nativeInfo == NULL))
        {
            argumentCount = retEvent->GetJITInfoResult.argumentCount;
            nativeInfoCountTotal = retEvent->GetJITInfoResult.totalNativeInfos;

            nativeInfo = new (nothrow) ICorJitInfo::NativeVarInfo[nativeInfoCountTotal];

            if (nativeInfo == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto exit;
            }
        }

        ICorJitInfo::NativeVarInfo *currentNativeInfo = &(retEvent->GetJITInfoResult.nativeInfo);

        while (currentInfoCount++ < retEvent->GetJITInfoResult.nativeInfoCount)
        {
            nativeInfo[nativeInfoCount] = *currentNativeInfo;

            currentNativeInfo++;
            nativeInfoCount++;
        }

        if (nativeInfoCount >= nativeInfoCountTotal)
            wait = false;
    }

    if (m_nativeInfo != NULL)
    {
        delete [] m_nativeInfo;
        m_nativeInfo = NULL;
    }

    m_nativeInfo = nativeInfo;
    m_argumentCount = argumentCount;
    m_nativeInfoCount = nativeInfoCount;
    m_nativeInfoValid = true;

exit:
    return hr;
}

/*
 * CordbFunction::GetSig
 *
 * Get the method's full metadata signature. This may be cached, but for dynamic modules we'll always read it from
 * the metadata. This function also returns the argument count and whether or not the method is static.
 *
 * Parameters:
 *    pMethodSigParser - OUT: the signature parser class to use for all signature parsing.
 *    pFunctionArgCount - OUT: the number of arguments the method takes.
 *    pFunctionIsStatic - OUT: TRUE if the method is static, FALSE if it is not..
 *
 * Returns:
 *    HRESULT for success or failure.
 *
 */
HRESULT CordbFunction::GetSig(SigParser *pMethodSigParser,
                              ULONG *pFunctionArgCount,
                              BOOL *pFunctionIsStatic)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    HRESULT hr = S_OK;

    // If the module is dynamic, there had better not be a cached locals signature.
    _ASSERTE(!GetModule()->IsDynamic() || !m_fCachedMethodValuesValid);

    // If the method signature cache is null, then go read the signature from the
    // matadata. For dynamic methods we never cache the parser because the method
    // may change and the cached value will not match.
    if (!m_fCachedMethodValuesValid)
    {
        PCCOR_SIGNATURE functionSignature;
        ULONG size;
        DWORD methodAttr = 0;
        ULONG argCount;

        IfFailRet(GetModule()->m_pIMImport->GetMethodProps(m_token, NULL, NULL, 0, NULL,
                                                           &methodAttr, &functionSignature, &size, NULL, NULL));

        SigParser sigParser = SigParser(functionSignature, size);

        IfFailRet(sigParser.SkipMethodHeaderSignature(&argCount));

        // If this function is not static, then we've got one extra arg.
        BOOL isStatic = (methodAttr & mdStatic) != 0;

        if (!isStatic)
        {
            argCount++;
        }

        // Cache the value for non-dynamic modules, so this is faster later.
        if (!GetModule()->IsDynamic())
        {
            m_methodSigParserCached = sigParser;
            m_argCountCached = argCount;
            m_isStaticCached = isStatic;
            m_fCachedMethodValuesValid = TRUE;
        }
        else 
        {
            if (pMethodSigParser != NULL)
            {
                *pMethodSigParser = sigParser;
            }
            
            if (pFunctionArgCount != NULL)
            {
                *pFunctionArgCount = argCount;
            }

            if (pFunctionIsStatic != NULL)
            {
                *pFunctionIsStatic = isStatic;
            }
        }
    }

    if (m_fCachedMethodValuesValid)
    {
        //
        // Retrieve values from cache
        //

        if (pMethodSigParser != NULL)
        {
            //
            // Give them a new instance of the cached value
            //
            *pMethodSigParser = m_methodSigParserCached;
        }

        if (pFunctionArgCount != NULL)
        {
            *pFunctionArgCount = m_argCountCached;
        }

        if (pFunctionIsStatic != NULL)
        {
            *pFunctionIsStatic = m_isStaticCached;
        }

    }

    //
    // We should never have a cached value for in a dynamic module.
    // 
    CONSISTENCY_CHECK_MSGF(((GetModule()->IsDynamic() && !m_fCachedMethodValuesValid) ||
                            (!GetModule()->IsDynamic() && m_fCachedMethodValuesValid)),
                           ("No dynamic modules should be cached! Module=%p This=%p", GetModule(), this));
    
    return hr;
}


//
// Given an IL argument number, return its type.
//
HRESULT CordbFunction::GetArgumentType(DWORD dwIndex,
                                       const Instantiation &inst,
                                       CordbType **res)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    HRESULT hr = S_OK;

    // Get the method's signature.
    SigParser sigParser;
    ULONG methodArgCount;
    BOOL methodIsStatic;

    IfFailRet(GetSig(&sigParser, &methodArgCount, &methodIsStatic));

    // Check the index
    if (dwIndex >= methodArgCount)
    {
        return E_INVALIDARG;
    }

    if (!methodIsStatic)
    {
        if (dwIndex == 0)
        {
            // Return the signature for the 'this' pointer for the
            // class this method is in.
            return m_class->GetThisType(inst, res);
        }
        else
        {
            dwIndex--;
        }
    }

    // Run the signature and find the required argument.
    for (unsigned int i = 0; i < dwIndex; i++)
    {
        IfFailRet(sigParser.SkipExactlyOne());
    }

    hr = CordbType::SigToType(m_module, &sigParser, inst, res);

    return hr;
}

//
// Set the info needed to build a local var signature for this function.
//
void CordbFunction::SetLocalVarToken(mdSignature localVarSigToken)
{
    _ASSERTE(TypeFromToken(localVarSigToken) == mdtSignature);

    m_localVarSigToken = localVarSigToken;
}


#include "corpriv.h"

/*
 * CordbFunction::GetLocalVarSig
 *
 * Get the method's local variable metadata signature. This may be cached, but for dynamic modules we'll always
 * read it from the metadata. This function also returns the count of local variables in the method.
 *
 * Parameters:
 *    pLocalSigParser - OUT: the local variable signature for the method.
 *    pLocalCount - OUT: the number of locals the method has.
 *
 * Returns:
 *    HRESULT for success or failure.
 *
 */
HRESULT CordbFunction::GetLocalVarSig(SigParser *pLocalSigParser,
                                      ULONG *pLocalVarCount)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess());
    FAIL_IF_NEUTERED(this);
    HRESULT hr = S_OK;

    // If the module is dynamic, there had better not be a cached locals signature.
    _ASSERTE(!GetModule()->IsDynamic() || !m_fCachedLocalValuesValid);

    // If the locals signature cache is null, or if this is a dynamic module, then go read the signature from the
    // matadata. We always read from the metadata for dynamic modules because our metadata blob is constantly
    // getting deleted and re-allocated. If we kept a pointer to the signature, we'd end up pointing to bad data.
    if (!m_fCachedLocalValuesValid)
    {
        // Make sure the function is populated. We do this to ensure that m_localVarSigToken is set.
        IfFailRet(Populate());

        // A function will not have a local var sig if it has no locals!
        if (m_localVarSigToken != mdSignatureNil)
        {
            PCCOR_SIGNATURE localSignature;
            ULONG size;
            ULONG localCount;
            
            IfFailRet(GetModule()->m_pIMImport->GetSigFromToken(m_localVarSigToken,
                                                                &localSignature,
                                                                &size));

            SigParser sigParser = SigParser(localSignature, size);

            ULONG data;

            IfFailRet(sigParser.GetCallingConvInfo(&data));

            _ASSERTE(data == IMAGE_CEE_CS_CALLCONV_LOCAL_SIG);

            // Snagg the count of locals in the sig.
            IfFailRet(sigParser.GetData(&localCount));

            // Cache the value for non-dynamic modules, so this is faster later.
            if (!GetModule()->IsDynamic())
            {
                m_localSigParserCached = sigParser;
                m_localVarCachedCount = localCount;
                m_fCachedLocalValuesValid = TRUE;
            }
            else
            {
                if (pLocalSigParser != NULL)
                {
                    *pLocalSigParser = sigParser;
                }

                if (pLocalVarCount != NULL)
                {
                    *pLocalVarCount = localCount;
                }
            }
        }
        else
        {
            //
            // Signature is Nil, so fill in everything with NULLs and zeros
            //

            if (pLocalSigParser != NULL)
            {
                *pLocalSigParser = SigParser(NULL, 0);
            }

            if (pLocalVarCount != NULL)
            {
                *pLocalVarCount = 0;
            }
        }
    }

    if (m_fCachedLocalValuesValid)
    {
        //
        // Get values from the cache
        //

        if (pLocalSigParser != NULL)
        {
            //
            // Give them a new instance of the cached value
            //
            *pLocalSigParser = m_localSigParserCached;
        }

        if (pLocalVarCount != NULL)
        {
            *pLocalVarCount = m_localVarCachedCount;
        }
    }

    //
    // We should never have a cached value for in a dynamic module or something without a signature.
    //
    CONSISTENCY_CHECK_MSGF((((GetModule()->IsDynamic() || (m_localVarSigToken == mdSignatureNil)) && !m_fCachedLocalValuesValid) ||
                            (!GetModule()->IsDynamic() && (m_localVarSigToken != mdSignatureNil) && m_fCachedLocalValuesValid)),
                           ("Caching is inconsistent! Module=%p This=%p", GetModule(), this));

    return hr;
}

//
// Given an IL variable number, return its type.
//
HRESULT CordbFunction::GetLocalVariableType(DWORD dwIndex,
                                            const Instantiation &inst,
                                            CordbType **res)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_ALLOW_LIVE_DO_STOPGO(GetProcess());

    HRESULT hr = S_OK;

    // Get the local variable signature.
    SigParser sigParser;
    ULONG localsCount;

    IfFailRet(GetLocalVarSig(&sigParser, &localsCount));

    // Check the index.
    if (dwIndex >= localsCount)
    {
        return E_INVALIDARG;
    }

    // Run the signature and find the required argument.
    for (unsigned int i = 0; i < dwIndex; i++)
    {
        IfFailRet(sigParser.SkipExactlyOne());
    }

    hr = CordbType::SigToType(m_module, &sigParser, inst, res);

    return hr;
}


/* ------------------------------------------------------------------------- *
 * CordbJITInfo class.
 * ------------------------------------------------------------------------- */

CordbJITInfo::CordbJITInfo(CordbFunction *f, CordbCode *code, BOOL isInstantiatedGeneric)
  : CordbBase(f->GetProcess(), (UINT_PTR) (code->GetAddress()), enumCordbJITInfo),
    m_function(f),
    m_nativecode(code), // implicit internal addref
    m_isInstantiatedGeneric(isInstantiatedGeneric),
    m_nativeInfoCount(0),
    m_nativeInfo(NULL),
    m_nativeInfoValid(0),
    m_argumentCount(0)
{

}


/*
    A list of which resources owened by this object are accounted for.

    UNKNOWN:

    HANDLED:
       m_nativeInfo  Local array - freed in ~CordbJITInfo
       m_nativecode   Owned by this object.  Neutered in Neuter()
*/

CordbJITInfo::~CordbJITInfo()
{
    _ASSERTE(IsNeutered());

    if (m_nativeInfo != NULL)
        delete [] m_nativeInfo;
}

// Neutered by CordbFunction
void CordbJITInfo::Neuter(NeuterTicket ticket)
{
    RSSmartPtr<CordbJITInfo> pRef(this);
    {
        CordbBase::Neuter(ticket);
        m_nativecode->Neuter(ticket);
        m_nativecode.Assign(NULL);
    }
    // Implicit release on pRef
}


HRESULT CordbJITInfo::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown*>(this);
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

//
// Given an IL local variable number and a native IP offset, return the
// location of the variable in jitted code.
//
HRESULT CordbJITInfo::ILVariableToNative(DWORD dwIndex,
                                          SIZE_T ip,
                                          ICorJitInfo::NativeVarInfo **ppNativeInfo)
{
    _ASSERTE(m_nativeInfoValid);

    return FindNativeInfoInILVariableArray(dwIndex,
                                           ip,
                                           ppNativeInfo,
                                           m_nativeInfoCount,
                                           m_nativeInfo);
}


/* ------------------------------------------------------------------------- *
 * Code class
 * ------------------------------------------------------------------------- */

// To make IL code:
CordbCode::CordbCode(CordbFunction *m, REMOTE_PTR startAddress,
                     SIZE_T size, SIZE_T nVersion)
  : CordbBase(m->GetProcess(), 0, enumCordbCode),
    m_isIL(true),
    m_function(m),
    m_nVersion(nVersion),
    m_address(startAddress),
    m_size(size),
    m_rgbCode(NULL),
    m_continueCounterLastSync(0),
    m_ilToNativeMapAddr(NULL),
    m_ilToNativeMapSize(0)
{
    m_nativeCodeJITInfoToken.Set(NULL),
    m_nativeCodeMethodDescToken.Set(NULL);
    _ASSERTE(nVersion >= CorDB_DEFAULT_ENC_FUNCTION_VERSION);

    // IL doesn't have a cold region.
    m_addressCold = NULL;
    m_sizeCold = 0;
}

// To make native code:
CordbCode::CordbCode(CordbFunction *m, DebuggerIPCE_JITFuncData * pJitData)
  : CordbBase(m->GetProcess(), 0, enumCordbCode),
    m_isIL(false),
    m_function(m),
    m_nVersion(pJitData->enCVersion),
    m_nativeCodeJITInfoToken(pJitData->nativeCodeJITInfoToken),
    m_nativeCodeMethodDescToken(pJitData->nativeCodeMethodDescToken),
    m_address(pJitData->nativeStartAddressPtr), m_size(pJitData->nativeSize),
    m_addressCold(pJitData->nativeStartAddressColdPtr), m_sizeCold(pJitData->nativeColdSize),
    m_rgbCode(NULL),
    m_continueCounterLastSync(0),
    m_ilToNativeMapAddr(pJitData->ilToNativeMapAddr),
    m_ilToNativeMapSize(pJitData->ilToNativeMapSize)
{
    _ASSERTE(m_nVersion >= CorDB_DEFAULT_ENC_FUNCTION_VERSION);
}

CordbCode::~CordbCode()
{
    _ASSERTE(IsNeutered());

    if (m_rgbCode != NULL)
        delete [] m_rgbCode;
}


// Neutered by CordbFunction
void CordbCode::Neuter(NeuterTicket ticket)
{
    RSSmartPtr<CordbCode> pRef(this);
    {
        CordbBase::Neuter(ticket);
    }
    // implicit release on pRef
}

HRESULT CordbCode::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugCode)
    {
        *pInterface = static_cast<ICorDebugCode*>(this);
    }
    else if (id == IID_ICorDebugCode2)
    {
        *pInterface = static_cast<ICorDebugCode2*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugCode*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbCode::IsIL(BOOL *pbIL)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pbIL, BOOL *);

    *pbIL = IsIL();

    return S_OK;
}


HRESULT CordbCode::GetFunction(ICorDebugFunction **ppFunction)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppFunction, ICorDebugFunction **);

    *ppFunction = static_cast<ICorDebugFunction*> (m_function);
    m_function->ExternalAddRef();

    return S_OK;
}

HRESULT CordbCode::GetAddress(CORDB_ADDRESS *pStart)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pStart, CORDB_ADDRESS *);


    _ASSERTE( this != NULL );
    _ASSERTE( this->m_function != NULL );
    _ASSERTE( this->m_function->m_module != NULL );
    _ASSERTE( this->m_function->m_module->GetProcess() == GetProcess() );

    // Since we don't do code-pitching, the address points directly to the code.
    *pStart = PTR_TO_CORDB_ADDRESS(m_address);

    if (!m_isIL && (*pStart == NULL))
    {
        return CORDBG_E_CODE_NOT_AVAILABLE;
    }
    return S_OK;
}

HRESULT CordbCode::GetSize(ULONG32 *pcBytes)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pcBytes, ULONG32 *);

    *pcBytes = (ULONG32)m_size;
    return S_OK;
}

HRESULT CordbCode::CreateBreakpoint(ULONG32 offset,
                                    ICorDebugFunctionBreakpoint **ppBreakpoint)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppBreakpoint, ICorDebugFunctionBreakpoint **);
    LOG((LF_CORDB, LL_INFO10000, "CCode::CreateBreakpoint, offset=%d, size=%d, IsIl=%d, this=0x%p\n",
        offset, m_size, m_isIL, this));




    // Make sure the offset is within range of the method.
    // If we're native code, then both offset & m_size are bytes of native code,
    // else they're both bytes of IL.
    if (offset >= m_size)
    {
        return CORDBG_E_UNABLE_TO_SET_BREAKPOINT;
    }

    CordbFunctionBreakpoint *bp = new (nothrow) CordbFunctionBreakpoint(this, offset);

    if (bp == NULL)
        return E_OUTOFMEMORY;

    HRESULT hr = bp->Activate(TRUE);
    if (SUCCEEDED(hr))
    {
        *ppBreakpoint = static_cast<ICorDebugFunctionBreakpoint*> (bp);
        bp->ExternalAddRef();
        return S_OK;
    }
    else
    {
        delete bp;
        return hr;
    }
}

HRESULT CordbCode::GetCode(ULONG32 startOffset,
                           ULONG32 endOffset,
                           ULONG32 cBufferAlloc,
                           BYTE buffer[],
                           ULONG32 *pcBufferSize)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(buffer, BYTE, cBufferAlloc, true, true);
    VALIDATE_POINTER_TO_OBJECT(pcBufferSize, ULONG32 *);

    LOG((LF_CORDB,LL_EVERYTHING, "CC::GC: for token:0x%x\n", m_function->m_token));

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;
    *pcBufferSize = 0;

    //
    // Check ranges.
    //

    if (cBufferAlloc < endOffset - startOffset)
        endOffset = startOffset + cBufferAlloc;

    if (endOffset > m_size)
        endOffset = (ULONG32)m_size;

    if (startOffset > m_size)
        startOffset = (ULONG32)m_size;

    // Check the continue counter since WriteMemory bumps it up.
    if ((m_rgbCode == NULL) ||
        (m_continueCounterLastSync < GetProcess()->m_continueCounter))
    {


        // We have an address & size, so whether it's IL or native, we'll just call ReadMemory.
        // This will conveniently strip out any patches too.
        REMOTE_PTR pHotStart = m_address;
        REMOTE_PTR pColdStart = m_addressCold;
        ULONG32 cbHotSize = (ULONG32) m_size - (ULONG32) m_sizeCold;
        ULONG32 cbColdSize = (ULONG32) m_sizeCold;

        delete [] m_rgbCode;
        m_rgbCode = new (nothrow) BYTE[m_size];
        if (m_rgbCode == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto LExit;
        }

        SIZE_T cbRead;
        hr = GetProcess()->ReadMemory(PTR_TO_CORDB_ADDRESS(pHotStart), cbHotSize, m_rgbCode, &cbRead);
        if (FAILED(hr))
        {
            goto LExit;
        }
        SIMPLIFYING_ASSUMPTION(cbRead == cbHotSize);

        if (HasColdRegion())
        {
            hr = GetProcess()->ReadMemory(PTR_TO_CORDB_ADDRESS(pColdStart), cbColdSize, (BYTE*) m_rgbCode + cbHotSize, &cbRead);
            if (FAILED(hr))
            {
                goto LExit;
            }
            SIMPLIFYING_ASSUMPTION(cbRead == cbColdSize);
            }


        m_continueCounterLastSync = GetProcess()->m_continueCounter;
    }


    // if we just got the code, we'll have to copy it over
    if (*pcBufferSize == 0 && m_rgbCode != NULL)
    {
        memcpy(buffer,
               m_rgbCode+startOffset,
               endOffset - startOffset);
        *pcBufferSize = endOffset - startOffset;
    }

LExit:
    return hr;
}

#include "dbgipcevents.h"
HRESULT CordbCode::GetVersionNumber( ULONG32 *nVersion)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(nVersion, ULONG32 *);

    LOG((LF_CORDB,LL_INFO10000,"R:CC:GVN:Returning 0x%x "
        "as version\n",m_nVersion));

    *nVersion = (ULONG32)m_nVersion;


    return S_OK;
}

HRESULT CordbCode::GetILToNativeMapping(ULONG32 cMap,
                                        ULONG32 *pcMap,
                                        COR_DEBUG_IL_TO_NATIVE_MAP map[])
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pcMap, ULONG32 *);
    VALIDATE_POINTER_TO_OBJECT_ARRAY_OR_NULL(map, COR_DEBUG_IL_TO_NATIVE_MAP *,cMap,true,true);

    // Gotta have a map address to return a map.
    if (m_ilToNativeMapAddr == NULL)
        return CORDBG_E_NON_NATIVE_FRAME;

    HRESULT hr = S_OK;
    DebuggerILToNativeMap *mapInt = NULL;

    mapInt = new (nothrow) DebuggerILToNativeMap[cMap];

    if (mapInt == NULL)
        return E_OUTOFMEMORY;

    // If they gave us space to copy into...
    if (map != NULL)
    {
        // Only copy as much as either they gave us or we have to copy.
        SIZE_T cnt = min(cMap, m_ilToNativeMapSize);

        if (cnt > 0)
        {
            // Read the map right out of the Left Side.
            BOOL succ = ReadProcessMemory(GetProcess()->m_handle,
                                          m_ilToNativeMapAddr,
                                          mapInt,
                                          cnt *
                                          sizeof(DebuggerILToNativeMap),
                                          NULL);

            if (!succ)
                hr = HRESULT_FROM_GetLastError();
        }

        // Remember that we need to translate between our internal DebuggerILToNativeMap and the external
        // COR_DEBUG_IL_TO_NATIVE_MAP!
        if (SUCCEEDED(hr))
            ExportILToNativeMap(cMap, map, mapInt, m_size);
    }

    if (pcMap)
        *pcMap = (ULONG32)m_ilToNativeMapSize;

    if (mapInt != NULL)
        delete [] mapInt;

    return hr;
}

HRESULT CordbCode::GetEnCRemapSequencePoints(ULONG32 cMap, ULONG32 *pcMap, ULONG32 offsets[])
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pcMap, ULONG32*);
    VALIDATE_POINTER_TO_OBJECT_ARRAY_OR_NULL(offsets, ULONG32*, cMap, true, true);

    //
    // Old EnC interface - deprecated
    //
    return E_NOTIMPL;
}

// Get the hot & cold regions of code.
HRESULT CordbCode::GetCodeChunks(
    ULONG32 cbufSize,
    ULONG32 * pcnumChunks,
    CodeChunkInfo chunks[]
)
{
    PUBLIC_API_ENTRY(this);

    if (pcnumChunks == NULL)
    {
        return E_INVALIDARG;
    }
    if ((chunks == NULL) != (cbufSize == 0))
    {
        return E_INVALIDARG;
    }

    // We know that there's only at most 2 possible chunks right now.
    ULONG32 cActualChunks = HasColdRegion() ? 2 : 1;

    // If no buf size, then we're querying the total number of chunks.
    if (cbufSize == 0)
    {
        *pcnumChunks = cActualChunks;
        return S_OK;
    }

    // Else give them as many as they asked for.
    if (cbufSize >= 1)
    {
        // Fill in the hot region
        chunks[0].startAddr = PTR_TO_CORDB_ADDRESS(m_address);
        chunks[0].length = (ULONG32) (m_size - m_sizeCold);
        *pcnumChunks = 1;
    }

    if ((cbufSize >= 2) && (cActualChunks >= 2))
    {
        // Fill in the cold region.
        chunks[1].startAddr = PTR_TO_CORDB_ADDRESS(m_addressCold);
        chunks[1].length = (ULONG32) m_sizeCold;
        *pcnumChunks = 2;
    }

    return S_OK;
}


// Public entry point to get code flags
HRESULT CordbCode::GetCompilerFlags(DWORD *pdwFlags)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pdwFlags, DWORD*);
    *pdwFlags = 0;
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    return GetCompilerFlagsInternal(pdwFlags);

}

// Internal entry point to get code flags.
HRESULT CordbCode::GetCompilerFlagsInternal(DWORD *pdwFlags)
{
    CordbProcess *pProcess = GetProcess();

    INTERNAL_SYNC_API_ENTRY(pProcess);
    _ASSERTE(pdwFlags != NULL);

    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event,
                           DB_IPCE_GET_COMPILER_FLAGS,
                           true,
                           GetAppDomain()->GetADToken());

    event.JitDebugInfo.debuggerModuleToken = m_function->m_module->m_debuggerModuleToken;
    // Note: two-way event here...
    HRESULT hr = pProcess->m_cordb->SendIPCEvent(pProcess,
                                         &event,
                                         sizeof(DebuggerIPCEvent));
    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    _ASSERTE(event.type == DB_IPCE_GET_COMPILER_FLAGS_RESULT);

    DWORD dwFlags = CORDEBUG_JIT_DEFAULT;
    if (event.JitDebugInfo.fEnableEnC)
    {
        dwFlags = CORDEBUG_JIT_ENABLE_ENC;
    }
    else if (! event.JitDebugInfo.fAllowJitOpts)
    {
        dwFlags = CORDEBUG_JIT_DISABLE_OPTIMIZATION;
    }

    *pdwFlags = dwFlags;
    return event.hr;
}


