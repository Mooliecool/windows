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
// RegMeta.cpp
//
// Implementation for meta data public interface methods.
//
//*****************************************************************************
#include "stdafx.h"
#include "regmeta.h"
#include "metadata.h"
#include "corerror.h"
#include "mdutil.h"
#include "rwutil.h"
#include "mdlog.h"
#include "importhelper.h"
#include "filtermanager.h"
#include "mdperf.h"
#include "switches.h"
#include "posterror.h"
#include "stgio.h"
#include "sstring.h"

#include "corffi.h"

#include <metamodelrw.h>

#define DEFINE_CUSTOM_NODUPCHECK    1
#define DEFINE_CUSTOM_DUPCHECK      2
#define SET_CUSTOM                  3

#if defined(_DEBUG) && defined(_TRACE_REMAPS)
#define LOGGING
#endif
#include <log.h>

#ifdef _MSC_VER
#pragma warning(disable: 4102)
#endif


RegMeta::RegMeta() :
    m_pStgdb(0),
    m_pStgdbFreeList(NULL),
    m_pUnk(0),
    m_pFilterManager(0),
    m_pInternalImport(NULL),
    m_pSemReadWrite(NULL),
    m_fOwnSem(false),
    m_bRemap(false),
    m_bSaveOptimized(false),
    m_hasOptimizedRefToDef(false),
    m_pHandler(0),
    m_fIsTypeDefDirty(false),
    m_fIsMemberDefDirty(false),
    m_fStartedEE(false),
    m_pCorHost(NULL),
    m_pAppDomain(NULL),
    m_OpenFlags(0),
    m_cRef(0),
    m_bCached(false),
    m_trLanguageType(0),
    m_SetAPICaller(EXTERNAL_CALLER),
    m_ModuleType(ValidatorModuleTypeInvalid),
    m_pVEHandler(0),
    m_bKeepKnownCa(false),
    m_pCorProfileData(NULL)
{
    memset(&m_OptionValue, 0, sizeof(OptionValue));

#ifdef _DEBUG        
    if (REGUTIL::GetConfigDWORD(L"MD_RegMetaBreak", 0))
    {
        _ASSERTE(!"RegMeta()");
    }
    if (REGUTIL::GetConfigDWORD(L"MD_KeepKnownCA", 0))
        m_bKeepKnownCa = true;
#endif // _DEBUG

} // RegMeta::RegMeta()

RegMeta::~RegMeta()
{
    BEGIN_CLEANUP_ENTRYPOINT;
    
    _ASSERTE(!m_bCached);

    HRESULT hr = S_OK;

    LOCKWRITENORET();

    // This should have worked if we've cached the public interface in the past
    _ASSERTE(SUCCEEDED(hr) || m_pInternalImport == NULL || m_pInternalImport->GetCachedPublicInterface(false) == NULL);

    if (SUCCEEDED(hr))
    {
        

        if (m_pInternalImport)
        {
            // RegMeta is going away. Make sure we clear up the pointer from MDInternalRW to this RegMeta.
            m_pInternalImport->SetCachedPublicInterface(NULL);
            m_pInternalImport = NULL;
            m_fOwnSem = false;
        }

        UNLOCKWRITE();
    }

    if (m_pSemReadWrite && m_fOwnSem)
        delete m_pSemReadWrite;

    // If this RegMeta is a wrapper on an external StgDB, release it.
    if (IsOfExternalStgDB(m_OpenFlags))
    {
        _ASSERTE(m_pUnk != NULL);   // Owning IUnknown for external StgDB.
        if (m_pUnk)
            m_pUnk->Release();
        m_pUnk = 0;
    }
    else
    {   // Not a wrapper, so free our StgDB.
        _ASSERTE(m_pUnk == NULL); 
        // It's possible m_pStdbg is NULL in OOM scenarios
        if (m_pStgdb != NULL)
            delete m_pStgdb;
        m_pStgdb = 0;
    }

    // Delete the old copies of Stgdb list. This is the list track all of the 
    //  old snapshuts with ReOpenWithMemory call.
    CLiteWeightStgdbRW  *pCur; 
    while (m_pStgdbFreeList)
    {
        pCur = m_pStgdbFreeList;
        m_pStgdbFreeList = m_pStgdbFreeList->m_pNextStgdb;
        delete pCur;
    }

    if (m_pVEHandler)
        m_pVEHandler->Release();

    // If This RegMeta spun up the runtime (probably to process security 
    //  attributes), shut it down now.
    if (m_fStartedEE) 
    {
        m_pAppDomain->Release();
        m_pCorHost->Stop();
        m_pCorHost->Release();
    }

    if (m_pFilterManager)
        delete m_pFilterManager;


    if (m_OptionValue.m_RuntimeVersion != NULL)
        delete[] m_OptionValue.m_RuntimeVersion;

    END_CLEANUP_ENTRYPOINT;
    
} // RegMeta::~RegMeta()

HRESULT RegMeta::SetOption(OptionValue *pOptionValue)
{
    _ASSERTE(pOptionValue);
    char* pszRuntimeVersion = NULL;

    if (pOptionValue->m_RuntimeVersion != NULL)
    {
        SIZE_T dwBufferSize = strlen(pOptionValue->m_RuntimeVersion) + 1; // +1 for null
        pszRuntimeVersion = new (nothrow) char[dwBufferSize]; 
        if (pszRuntimeVersion == NULL)
        {
            return E_OUTOFMEMORY;
        }
        strcpy_s(pszRuntimeVersion, dwBufferSize, pOptionValue->m_RuntimeVersion);
    }

    memcpy(&m_OptionValue, pOptionValue, sizeof(OptionValue));
    m_OptionValue.m_RuntimeVersion = pszRuntimeVersion;

    return S_OK;
}// SetOption



//*****************************************************************************
// Call this after initialization is complete.
//*****************************************************************************
HRESULT RegMeta::AddToCache()
{
    HRESULT hr=S_OK;

    // The ref count must be > 0 before the module is published, else another
    //  thread could find, use, and release the module, causing it to be deleted
    //  before this thread gets a chance to addref.
    _ASSERTE(GetRefCount() > 0);
    // add this RegMeta to the loaded module list.
    m_bCached = true;
    IfFailGo(LOADEDMODULES::AddModuleToLoadedList(this));
ErrExit:
    if (FAILED(hr))
    {
        _ASSERTE(!LOADEDMODULES::IsEntryInList(this));
        m_bCached = false;
    }
    return hr;    
} // void RegMeta::AddToCache()

//*****************************************************************************
// Search the cached RegMetas for a given scope.
//*****************************************************************************
HRESULT RegMeta::FindCachedReadOnlyEntry(
    LPCWSTR     szName,                 // Name of the desired file.
    DWORD       dwOpenFlags,            // Flags the new file is opened with.
    RegMeta     **ppMeta)               // Put found RegMeta here.
{
    return LOADEDMODULES::FindCachedReadOnlyEntry(szName, dwOpenFlags, ppMeta);
} // HRESULT RegMeta::FindCachedReadOnlyEntry()

//*****************************************************************************
// Initialize with an existing stgdb.
//*****************************************************************************
HRESULT RegMeta::InitWithStgdb(
    IUnknown        *pUnk,              // The IUnknown that owns the life time for the existing stgdb
    CLiteWeightStgdbRW *pStgdb)         // existing light weight stgdb
{
    // RegMeta created this way will not create a read/write lock semaphore. 

    HRESULT     hr = S_OK;

    _ASSERTE(m_pStgdb == NULL);
    m_tdModule = COR_GLOBAL_PARENT_TOKEN;
    m_pStgdb = pStgdb;

    m_OpenFlags = ofExternalStgDB;

    // remember the owner of the light weight stgdb
    // AddRef it to ensure the lifetime
    //
    m_pUnk = pUnk;
    m_pUnk->AddRef();
    IfFailGo( m_pStgdb->m_MiniMd.GetOption(&m_OptionValue) );
ErrExit:
    return hr;
} // HRESULT RegMeta::InitWithStgdb()

//*****************************************************************************
// call stgdb InitNew
//*****************************************************************************
HRESULT RegMeta::CreateNewMD()
{
    HRESULT     hr = NOERROR;
    
    m_OpenFlags = ofWrite;

    // Allocate our m_pStgdb.
    _ASSERTE(m_pStgdb == NULL);
        IfNullGo( m_pStgdb = new (nothrow) CLiteWeightStgdbRW );
    
    // Initialize the new, empty database.

    // First tell the new database what sort of metadata to create
    m_pStgdb->m_MiniMd.m_OptionValue.m_MetadataVersion = m_OptionValue.m_MetadataVersion;
    m_pStgdb->InitNew();

    // Set up the Module record.
    ULONG       iRecord;
    ModuleRec   *pModule;
    GUID        mvid;
    IfNullGo(pModule=m_pStgdb->m_MiniMd.AddModuleRecord(&iRecord));
    IfFailGo(CoCreateGuid(&mvid));
    IfFailGo(m_pStgdb->m_MiniMd.PutGuid(TBL_Module, ModuleRec::COL_Mvid, pModule, mvid));

    // Add the dummy module typedef which we are using to parent global items.
    TypeDefRec  *pRecord;
    IfNullGo(pRecord=m_pStgdb->m_MiniMd.AddTypeDefRecord(&iRecord));
    m_tdModule = TokenFromRid(iRecord, mdtTypeDef);
    IfFailGo(m_pStgdb->m_MiniMd.PutStringW(TBL_TypeDef, TypeDefRec::COL_Name, pRecord, COR_WMODULE_CLASS));
    IfFailGo( m_pStgdb->m_MiniMd.SetOption(&m_OptionValue) );

    if (IsThreadSafetyOn())
    {
        m_pSemReadWrite = new (nothrow) UTSemReadWrite;
        IfNullGo( m_pSemReadWrite);
        m_fOwnSem = true;
    }    

    // initialize the embedded merger
    m_newMerger.Init(this);

ErrExit:

    return hr;
} // HRESULT RegMeta::CreateNewMD()

//*****************************************************************************
// call stgdb OpenForRead
//*****************************************************************************
HRESULT RegMeta::OpenExistingMD(
    LPCWSTR     szDatabase,             // Name of database.
    void        *pData,                 // Data to open on top of, 0 default.
    ULONG       cbData,                 // How big is the data.
    ULONG       dwOpenFlags)            // Flags for the open.
{
    HRESULT     hr = NOERROR;
    void        *pbData = pData;        // Pointer to original or copied data.
    
    
    
    m_OpenFlags = dwOpenFlags;          

    if (!IsOfReOpen(dwOpenFlags))
    {
        // Allocate our m_pStgdb, if we should.
        _ASSERTE(m_pStgdb == NULL);
        IfNullGo( m_pStgdb = new (nothrow) CLiteWeightStgdbRW );
    }
    
    IfFailGo( m_pStgdb->OpenForRead(
        szDatabase,
        pbData,
        cbData,
        m_OpenFlags) );

    if (m_pStgdb->m_MiniMd.m_Schema.m_major == METAMODEL_MAJOR_VER_V1_0 &&
        m_pStgdb->m_MiniMd.m_Schema.m_minor == METAMODEL_MINOR_VER_V1_0)
        m_OptionValue.m_MetadataVersion = MDVersion1;

    else
        m_OptionValue.m_MetadataVersion = MDVersion2;
        
    

    IfFailGo( m_pStgdb->m_MiniMd.SetOption(&m_OptionValue) );

    if (IsThreadSafetyOn())
    {
        m_pSemReadWrite = new (nothrow) UTSemReadWrite;
        IfNullGo( m_pSemReadWrite);
        m_fOwnSem = true;
    }

    if (!IsOfReOpen(dwOpenFlags))
{
        // initialize the embedded merger
        m_newMerger.Init(this);

        // There must always be a Global Module class and its the first entry in
        // the TypeDef table.
        m_tdModule = TokenFromRid(1, mdtTypeDef);
    }

ErrExit:

    return hr;
} // HRESULT    RegMeta::OpenExistingMD()

//*****************************************************************************
// Note that the returned IUnknown is not AddRef'ed. This function also does not
// trigger the creation of Internal interface.
//*****************************************************************************
IUnknown* RegMeta::GetCachedInternalInterface(BOOL fWithLock) 
{
    IUnknown        *pRet = NULL;
    HRESULT hr = S_OK;
    
    if (fWithLock)
    {
        LOCKREAD();
        
        pRet = m_pInternalImport;
    }
    else
    {
        pRet = m_pInternalImport;
    }
    if (pRet) pRet->AddRef();

ErrExit:

    return pRet;
} // IUnknown* RegMeta::GetCachedInternalInterface() 


//*****************************************************************************
// Set the cached Internal interface. This function will return an Error is the
// current cached internal interface is not empty and trying set a non-empty internal
// interface. One RegMeta will only associated
// with one Internal Object. Unless we have bugs somewhere else. It will QI on the 
// IUnknown for the IMDInternalImport. If this failed, error will be returned.
// Note: Caller should take a write lock
//*****************************************************************************
HRESULT RegMeta::SetCachedInternalInterface(IUnknown *pUnk)
{
    HRESULT     hr = NOERROR;
    IMDInternalImport *pInternal = NULL;

    if (pUnk)
    {
        if (m_pInternalImport)
        {
            _ASSERTE(!"Bad state!");
        }
        IfFailRet( pUnk->QueryInterface(IID_IMDInternalImport, (void **) &pInternal) );

        // Should be non-null
        _ASSERTE(pInternal);
        m_pInternalImport = pInternal;
    
        // We don't add ref the internal interface
        pInternal->Release();
    }
    else
    {
        // Internal interface is going away before the public interface. Take ownership on the 
        // reader writer lock.
        m_fOwnSem = true;
        m_pInternalImport = NULL;
    }
    return hr;
} // HRESULT RegMeta::SetCachedInternalInterface(IUnknown *pUnk)


//*****************************************************************************
// IMetaDataEmit2 methods
//*****************************************************************************

//*****************************************************************************
// Set module properties on a scope.
//*****************************************************************************
STDMETHODIMP RegMeta::SetModuleProps(   // S_OK or error.
    LPCWSTR     szName)                 // [IN] If not NULL, the name to set.
{
    HRESULT     hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    ModuleRec   *pModule;               // The module record to modify.

    LOG((LOGMD, "RegMeta::SetModuleProps(%S)\n", MDSTR(szName)));
    
    
    START_MD_PERF()
    LOCKWRITE();

    m_pStgdb->m_MiniMd.PreUpdate();

    pModule = m_pStgdb->m_MiniMd.getModule(1);
    if (szName)
    {
        WCHAR       rcFile[_MAX_PATH];
        WCHAR       rcExt[_MAX_PATH];       
        WCHAR       rcNewFileName[_MAX_PATH];       

        // If the total name is less than _MAX_PATH, the components are, too.
        if (wcslen(szName) >= _MAX_PATH)
            IfFailGo(E_INVALIDARG);

        SplitPath(szName, NULL, 0, NULL, 0, rcFile, COUNTOF(rcFile), rcExt, COUNTOF(rcExt));
        MakePath(rcNewFileName, NULL, NULL, rcFile, rcExt);
        IfFailGo(m_pStgdb->m_MiniMd.PutStringW(TBL_Module, ModuleRec::COL_Name, pModule, rcNewFileName));
    }

    IfFailGo(UpdateENCLog(TokenFromRid(1, mdtModule)));

ErrExit:
    
    STOP_MD_PERF(SetModuleProps);
    END_ENTRYPOINT_NOTHROW;

    return hr;
} // STDMETHODIMP RegMeta::SetModuleProps()

//*****************************************************************************
// Saves a scope to a file of a given name.
//*****************************************************************************
STDMETHODIMP RegMeta::Save(                     // S_OK or error.
    LPCWSTR     szFile,                 // [IN] The filename to save to.
    DWORD       dwSaveFlags)            // [IN] Flags for the save.
{
    HRESULT     hr=S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;


    
    
    LOG((LOGMD, "RegMeta::Save(%S, 0x%08x)\n", MDSTR(szFile), dwSaveFlags));
    START_MD_PERF()
    LOCKWRITE();

    // Check reserved param..
    if (dwSaveFlags != 0)
        IfFailGo (E_INVALIDARG);
    IfFailGo(PreSave());
    IfFailGo(m_pStgdb->Save(szFile, dwSaveFlags));

    // Reset m_bSaveOptimized, this is to handle the incremental and ENC
    // scenerios where one may do multiple saves.
    _ASSERTE(m_bSaveOptimized && !m_pStgdb->m_MiniMd.IsPreSaveDone());
    m_bSaveOptimized = false;

#if defined(_DEBUG)
    if (REGUTIL::GetConfigDWORD(L"MD_RegMetaDump", 0))
    {
        int DumpMD_impl(RegMeta *pMD);
        DumpMD_impl(this);
    }
#endif // _DEBUG

ErrExit:
    
    STOP_MD_PERF(Save);
    END_ENTRYPOINT_NOTHROW;

    return hr;
} // STDMETHODIMP RegMeta::Save()

//*****************************************************************************
// Saves a scope to a stream.
//*****************************************************************************
STDMETHODIMP RegMeta::SaveToStream(     // S_OK or error.
    IStream     *pIStream,              // [IN] A writable stream to save to.
    DWORD       dwSaveFlags)            // [IN] Flags for the save.
{
    HRESULT     hr=S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;
    
    LOCKWRITE();

    LOG((LOGMD, "RegMeta::SaveToStream(0x%08x, 0x%08x)\n", pIStream, dwSaveFlags));
    START_MD_PERF()

    //Save(L"Foo.clb", 0);
    m_pStgdb->m_MiniMd.PreUpdate();

    hr = _SaveToStream(pIStream, dwSaveFlags);

    STOP_MD_PERF(SaveToStream);
    
#if defined(_DEBUG)
    if (REGUTIL::GetConfigDWORD(L"MD_RegMetaDump", 0))
    {
        int DumpMD_impl(RegMeta *pMD);
        DumpMD_impl(this);
    }
#endif // _DEBUG

ErrExit:

    END_ENTRYPOINT_NOTHROW;

    return hr;
} // STDMETHODIMP RegMeta::SaveToStream()


//*****************************************************************************
// Saves a scope to a stream.
//*****************************************************************************
HRESULT RegMeta::_SaveToStream(         // S_OK or error.
    IStream     *pIStream,              // [IN] A writable stream to save to.
    DWORD       dwSaveFlags)            // [IN] Flags for the save.
{
    HRESULT     hr=S_OK;
  
    IfFailGo(PreSave());
    IfFailGo( m_pStgdb->SaveToStream(pIStream, m_pCorProfileData) );

    // Reset m_bSaveOptimized, this is to handle the incremental and ENC
    // scenerios where one may do multiple saves.
    _ASSERTE(m_bSaveOptimized && !m_pStgdb->m_MiniMd.IsPreSaveDone());
    m_bSaveOptimized = false;

ErrExit:

    return hr;
} // STDMETHODIMP RegMeta::_SaveToStream()

//*****************************************************************************
// As the Stgdb object to get the save size for the scope.
//*****************************************************************************
STDMETHODIMP RegMeta::GetSaveSize(      // S_OK or error.
    CorSaveSize fSave,                  // [IN] cssAccurate or cssQuick.
    DWORD       *pdwSaveSize)           // [OUT] Put the size here.
{
    HRESULT     hr=S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    FilterTable *ft = NULL;
    
    LOG((LOGMD, "RegMeta::GetSaveSize(0x%08x, 0x%08x)\n", fSave, pdwSaveSize));
    START_MD_PERF();
    LOCKWRITE();

    ft = m_pStgdb->m_MiniMd.GetFilterTable();
    IfNullGo(ft);

    if(m_pStgdb->m_MiniMd.m_USBlobs.IsEmpty())
    {
        if(!(IsENCDelta(m_pStgdb->m_MiniMd.m_OptionValue.m_UpdateMode)||m_pStgdb->m_MiniMd.IsMinimalDelta()))
        {
            BYTE pbData[] = {' ', 0, 0};
            ULONG ulOffset;
            m_pStgdb->m_MiniMd.PutUserString(pbData,3,&ulOffset);
            // Make sure this user string is marked
            if (ft->Count())
                IfFailGo( m_pFilterManager->MarkNewUserString(TokenFromRid(ulOffset, mdtString)));
        }
    }


    if ( ft->Count() )
    {
        int     iCount;

        // There is filter table. Linker is using /opt:ref.
        // Make sure that we are marking the AssemblyDef token!
        iCount = m_pStgdb->m_MiniMd.getCountAssemblys();
        _ASSERTE(iCount <= 1);

        if (iCount)
        {
            IfFailGo( m_pFilterManager->Mark(TokenFromRid(iCount, mdtAssembly) ));
        }
    }
    else if (m_newMerger.m_pImportDataList)
    {
        // always pipe through another pass of merge to drop unnecessary ref for linker.
        MarkAll();
    }

    IfFailGo(PreSave());
    
    hr = m_pStgdb->GetSaveSize(fSave, pdwSaveSize, m_pCorProfileData);
    

ErrExit:

    STOP_MD_PERF(GetSaveSize);

    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // STDMETHODIMP RegMeta::GetSaveSize()

//*****************************************************************************
// Merge the pImport scope to this scope
//*****************************************************************************
STDMETHODIMP RegMeta::Merge(            // S_OK or error.
    IMetaDataImport *pImport,           // [IN] The scope to be merged.
    IMapToken   *pHostMapToken,         // [IN] Host IMapToken interface to receive token remap notification
    IUnknown    *pHandler)              // [IN] An object to receive to receive error notification.
{
    HRESULT     hr = NOERROR;

    BEGIN_ENTRYPOINT_NOTHROW;

    IMetaDataImport2 *pI2=NULL;

    
    
    LOG((LOGMD, "RegMeta::Merge(0x%08x, 0x%08x)\n", pImport, pHandler));
    START_MD_PERF();
    LOCKWRITE();

    IfFailGo(pImport->QueryInterface(IID_IMetaDataImport2, (void**)&pI2));
    m_hasOptimizedRefToDef = false;

    // track this import
    IfFailGo(  m_newMerger.AddImport(pI2, pHostMapToken, pHandler) );

ErrExit:    
    if (pI2)
        pI2->Release();
    STOP_MD_PERF(Merge);
    END_ENTRYPOINT_NOTHROW;
    return (hr);
}


//*****************************************************************************
// real merge takes place here
//*****************************************************************************
STDMETHODIMP RegMeta::MergeEnd()        // S_OK or error.
{
    HRESULT     hr = NOERROR;

    BEGIN_ENTRYPOINT_NOTHROW;


    LOG((LOGMD, "RegMeta::MergeEnd()\n"));
    START_MD_PERF();
    LOCKWRITE();
    // Merge happens here!!

    IfFailGo(m_pStgdb->m_MiniMd.ExpandTables());

    IfFailGo(m_newMerger.Merge(m_OptionValue.m_MergeOptions, m_OptionValue.m_RefToDefCheck) );

ErrExit:    
    STOP_MD_PERF(MergeEnd);
    END_ENTRYPOINT_NOTHROW;

    return (hr);
}

//*****************************************************************************
// Helper function to startup the EE
//*****************************************************************************
HRESULT RegMeta::StartupEE()
{
    HRESULT         hr = S_OK;
    IUnknown        *pSetup = NULL;
    IManagedInstanceWrapper *pDomainSetup = NULL;
    bool             fDoneStart = false;

    PAL_TRY
    {
        DWORD   dwBuffer[1 + (MAX_PATH+1) * sizeof(WCHAR) / sizeof(DWORD) + 1];
        BSTR    bstrDir = NULL;

        // Create a hosting environment.
        IfFailGo(CoCreateInstance(CLSID_CorRuntimeHost,
                                    NULL,
                                    CLSCTX_INPROC_SERVER,
                                    IID_ICorRuntimeHost,
                                    (void**)&m_pCorHost));

        // Startup the runtime.
        IfFailGo(m_pCorHost->Start());
        fDoneStart = true;

        // Create an AppDomain Setup so we can set the AppBase.
        IfFailGo(m_pCorHost->CreateDomainSetup(&pSetup));

        // Get the current directory (place it in a BSTR).
        bstrDir = (BSTR)(dwBuffer + 1);
        if ((dwBuffer[0] = (WszGetCurrentDirectory(MAX_PATH + 1, bstrDir) * sizeof(WCHAR)))) {
            // QI for the IAppDomainSetup interface.
            IfFailGo(pSetup->QueryInterface(IID_IManagedInstanceWrapper,
                                            (void**)&pDomainSetup));

            // Set the AppBase.
            VARIANT var;
            V_VT(&var) = VT_BSTR;
            V_BSTR(&var) = bstrDir;
            IfFailGo(pDomainSetup->InvokeByName(L"ApplicationBase", CorFFISetProperty, 
                1, &var, NULL));
        }

        // Create a new AppDomain.
        IfFailGo(m_pCorHost->CreateDomainEx(L"Compilation Domain",
                                            pSetup,
                                            NULL,
                                            &m_pAppDomain));

        // That's it, we're all set up.
        _ASSERTE(m_pAppDomain != NULL);
        m_fStartedEE = true;

    ErrExit: ;
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        _ASSERTE(!"Unexpected exception setting up hosting environment for security attributes");
        hr = E_FAIL;
    }
    PAL_ENDTRY

    // Cleanup temporary resources.
    if (m_pAppDomain && FAILED(hr))
        m_pAppDomain->Release();
    if (pDomainSetup)
        pDomainSetup->Release();
    if (pSetup)
        pSetup->Release();
    if (fDoneStart && FAILED(hr))
        m_pCorHost->Stop();
    if (m_pCorHost && FAILED(hr))
        m_pCorHost->Release();

    return hr;
}

//*****************************************************************************
// Persist a set of security custom attributes into a set of permission set
// blobs on the same class or method.
//*****************************************************************************
HRESULT RegMeta::DefineSecurityAttributeSet(// Return code.
    mdToken     tkObj,                  // [IN] Class or method requiring security attributes.
    COR_SECATTR rSecAttrs[],            // [IN] Array of security attribute descriptions.
    ULONG       cSecAttrs,              // [IN] Count of elements in above array.
    ULONG       *pulErrorAttr)          // [OUT] On error, index of attribute causing problem.
{
    HRESULT         hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    NewArrayHolder <CORSEC_ATTRSET> rAttrSets;
    DWORD           i;
    mdPermission    ps;
    DWORD           dwAction;
    bool fProcessDeclarativeSecurityAtRuntime;

    LOG((LOGMD, "RegMeta::DefineSecurityAttributeSet(0x%08x, 0x%08x, 0x%08x, 0x%08x)\n",
         tkObj, rSecAttrs, cSecAttrs, pulErrorAttr));
    START_MD_PERF();
    LOCKWRITE();

    rAttrSets = new (nothrow) CORSEC_ATTRSET[dclMaximumValue + 1];
    if (rAttrSets == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto ErrExit;
    }

    memset(rAttrSets, 0, sizeof(CORSEC_ATTRSET) * (dclMaximumValue + 1));

    // Initialize error index to indicate a general error.
    if (pulErrorAttr)
        *pulErrorAttr = cSecAttrs;

    fProcessDeclarativeSecurityAtRuntime = true;

    // See if we should default to old v1.0/v1.1 serialization behavior
    if (WszGetEnvironmentVariable(SECURITY_OLD_ATTRIBUTES, NULL, 0) != 0 ||
        m_OptionValue.m_MetadataVersion < MDVersion2)
        fProcessDeclarativeSecurityAtRuntime = false;

    // Startup the EE just once, no matter how many times we're called (this is
    // better on performance and the EE falls over if we try a start-stop-start
    // cycle anyway).
    if (!m_fStartedEE && !fProcessDeclarativeSecurityAtRuntime)
    {
        IfFailGo(StartupEE());
    }

    // Group the security attributes by SecurityAction (thus creating an array of CORSEC_PERM's)
    IfFailGo(GroupSecurityAttributesByAction(/*OUT*/rAttrSets, rSecAttrs, cSecAttrs, tkObj, pulErrorAttr, &m_pStgdb->m_MiniMd, NULL));

    // Put appropriate data in the metadata
    for (i = 0; i <= dclMaximumValue; i++) 
    {
        NewArrayHolder <BYTE>    pbBlob(NULL);
        NewArrayHolder <BYTE>    pbNonCasBlob(NULL);
        DWORD              cbBlob = 0;
        DWORD              cbNonCasBlob = 0;

        rAttrSets[i].pImport = this;
        rAttrSets[i].pAppDomain = m_pAppDomain;
        if (rAttrSets[i].dwAttrCount == 0)
            continue;
        if (pulErrorAttr)
            *pulErrorAttr = i;

        if(fProcessDeclarativeSecurityAtRuntime)
        {
            // Put a serialized CORSEC_ATTRSET in the metadata
            IfFailGo(AttributeSetToBlob(&rAttrSets[i], NULL, &cbBlob, this, i)); // count size required for buffer
            pbBlob = new (nothrow) BYTE[cbBlob]; // allocate buffer
            if (pbBlob == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto ErrExit;
            }

            IfFailGo(AttributeSetToBlob(&rAttrSets[i], pbBlob, NULL, this, i)); // serialize into the buffer
            IfFailGo(_DefinePermissionSet(rAttrSets[i].tkObj, rAttrSets[i].dwAction, pbBlob, cbBlob, &ps)); // put it in metadata
        }
        else
        {
            // Now translate the sets of security attributes into a real permission
            // set and convert this to a serialized Xml blob. We may possibly end up
            // with two sets as the result of splitting CAS and non-CAS permissions
            // into separate sets.
            hr = TranslateSecurityAttributes(&rAttrSets[i], &pbBlob, &cbBlob, &pbNonCasBlob, &cbNonCasBlob, pulErrorAttr);
            IfFailGo(hr);

            // Persist the permission set blob into the metadata. For empty CAS
            // blobs this is only done if the corresponding non-CAS blob is empty
            if (cbBlob || !cbNonCasBlob)
                IfFailGo(_DefinePermissionSet(rAttrSets[i].tkObj, rAttrSets[i].dwAction, pbBlob, cbBlob, &ps));
            
            if (pbNonCasBlob)
            {
                // Map the SecurityAction to a special non-CAS action so this
                // blob will have its own entry in the metadata
                switch (rAttrSets[i].dwAction)
                {
                case dclDemand:
                    dwAction = dclNonCasDemand;
                    break;
                case dclLinktimeCheck:
                    dwAction = dclNonCasLinkDemand;
                    break;
                case dclInheritanceCheck:
                    dwAction = dclNonCasInheritance;
                    break;
                default:
                    PostError(CORSECATTR_E_BAD_NONCAS);
                    IfFailGo(CORSECATTR_E_BAD_NONCAS);
                }

                // Persist to metadata
                IfFailGo(_DefinePermissionSet(rAttrSets[i].tkObj,
                                              dwAction,
                                              pbNonCasBlob,
                                              cbNonCasBlob,
                                              &ps));
            }
        }
    }

ErrExit:
    STOP_MD_PERF(DefineSecurityAttributeSet);

    END_ENTRYPOINT_NOTHROW;
    
    return (hr);
}   // HRESULT RegMeta::DefineSecurityAttributeSet()

//*****************************************************************************
// Unmark everything in this module
//*****************************************************************************
HRESULT RegMeta::UnmarkAll()
{
    HRESULT         hr;

    BEGIN_ENTRYPOINT_NOTHROW;
    int             i;
    int             iCount;
    TypeDefRec      *pRec;
    ULONG           ulEncloser;
    NestedClassRec  *pNestedClass;
    CustomAttributeRec  *pCARec;
    mdToken         tkParent;
    int             iStart, iEnd;

    LOG((LOGMD, "RegMeta::UnmarkAll\n"));
    
    START_MD_PERF();
    LOCKWRITE();
    

    // calculate the TypeRef and TypeDef mapping here
    //
    IfFailGo( RefToDefOptimization() );

    // unmark everything in the MiniMd.
    IfFailGo( m_pStgdb->m_MiniMd.UnmarkAll() );

    // instantiate the filter manager
    m_pFilterManager = new (nothrow) FilterManager( &(m_pStgdb->m_MiniMd) );
    IfNullGo( m_pFilterManager );

    // Mark all public typedefs.
    iCount = m_pStgdb->m_MiniMd.getCountTypeDefs();

    // Mark all of the public TypeDef. We need to skip over the <Module> typedef
    for (i = 2; i <= iCount; i++)
    {
        pRec = m_pStgdb->m_MiniMd.getTypeDef(i);
        if (m_OptionValue.m_LinkerOption == MDNetModule)
        {
            // Client is asking us to keep private type as well. 
            IfFailGo( m_pFilterManager->Mark(TokenFromRid(i, mdtTypeDef)) );
        }
        else if (i != 1)
        {
            // when client is not set to MDNetModule, global functions/fields won't be keep by default
            //
            if (IsTdPublic(pRec->GetFlags()))
            {
                IfFailGo( m_pFilterManager->Mark(TokenFromRid(i, mdtTypeDef)) );
            }
            else if ( IsTdNestedPublic(pRec->GetFlags()) ||
                      IsTdNestedFamily(pRec->GetFlags()) ||
                      IsTdNestedFamORAssem(pRec->GetFlags()) )
            {
                // This nested class would potentially be visible outside, either
                // directly or through inheritence.  If the enclosing class is
                // marked, this nested class must be marked.
                //
                ulEncloser = m_pStgdb->m_MiniMd.FindNestedClassHelper(TokenFromRid(i, mdtTypeDef));
                _ASSERTE( !InvalidRid(ulEncloser) && 
                          "Bad metadata for nested type!" );
                pNestedClass = m_pStgdb->m_MiniMd.getNestedClass(ulEncloser);
                tkParent = m_pStgdb->m_MiniMd.getEnclosingClassOfNestedClass(pNestedClass);
                if ( m_pStgdb->m_MiniMd.GetFilterTable()->IsTypeDefMarked(tkParent))
                    IfFailGo( m_pFilterManager->Mark(TokenFromRid(i, mdtTypeDef)) );
            }
        }
    }

    if (m_OptionValue.m_LinkerOption == MDNetModule)
    {
        // Mark global function if NetModule. We will not keep _Delete method.
        pRec = m_pStgdb->m_MiniMd.getTypeDef(1);
        iStart = m_pStgdb->m_MiniMd.getMethodListOfTypeDef( pRec );
        iEnd = m_pStgdb->m_MiniMd.getEndMethodListOfTypeDef( pRec );
        for ( i = iStart; i < iEnd; i ++ )
        {
            RID         rid = m_pStgdb->m_MiniMd.GetMethodRid(i);
            MethodRec   *pMethodRec = m_pStgdb->m_MiniMd.getMethod(rid);

            // check the name
            if (IsMdRTSpecialName(pMethodRec->GetFlags()))
            {
                LPCUTF8     szName = m_pStgdb->m_MiniMd.getNameOfMethod(pMethodRec);

                // Only mark method if not a _Deleted method
                if (strcmp(szName, COR_DELETED_NAME_A) != 0)
                    IfFailGo( m_pFilterManager->Mark( TokenFromRid( rid, mdtMethodDef) ) );
            }
            else
            {
            if (!IsMiForwardRef(pMethodRec->GetImplFlags()) || 
                    IsMiRuntime(pMethodRec->GetImplFlags())    || 
                    IsMdPinvokeImpl(pMethodRec->GetFlags()) )

                IfFailGo( m_pFilterManager->Mark( TokenFromRid( rid, mdtMethodDef) ) );
            }
        }
    }

    // mark the module property
    IfFailGo( m_pFilterManager->Mark(TokenFromRid(1, mdtModule)) );

    // We will also keep all of the TypeRef that has any CustomAttribute hang off it.
    iCount = m_pStgdb->m_MiniMd.getCountCustomAttributes();

    // Mark all of the TypeRef used by CA's 
    for (i = 1; i <= iCount; i++)
    {
        pCARec = m_pStgdb->m_MiniMd.getCustomAttribute(i);
        tkParent = m_pStgdb->m_MiniMd.getParentOfCustomAttribute(pCARec);
        if (TypeFromToken(tkParent) == mdtTypeRef)
        {
            m_pFilterManager->Mark(tkParent);
        }
    }
ErrExit:
    
    STOP_MD_PERF(UnmarkAll);

    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // HRESULT RegMeta::UnmarkAll()



//*****************************************************************************
// Mark everything in this module
//*****************************************************************************
HRESULT RegMeta::MarkAll()
{
    HRESULT         hr = NOERROR;

    // mark everything in the MiniMd.
    IfFailGo( m_pStgdb->m_MiniMd.MarkAll() );

    // instantiate the filter manager if not instantiated
    if (m_pFilterManager == NULL)
    {        
        m_pFilterManager = new (nothrow) FilterManager( &(m_pStgdb->m_MiniMd) );
        IfNullGo( m_pFilterManager );
    }
ErrExit:
    
    return hr;
}   // HRESULT RegMeta::MarkAll

//*****************************************************************************
// Unmark all of the transient CAs
//*****************************************************************************
HRESULT RegMeta::UnmarkAllTransientCAs()
{
    HRESULT         hr = NOERROR;
    int             i;
    int             iCount;
    int             cTypeRefRecs;
    TypeDefRec      *pRec;
    CMiniMdRW       *pMiniMd = &(m_pStgdb->m_MiniMd);
    TypeRefRec      *pTypeRefRec;           // A TypeRef record.
    LPCUTF8         szNameTmp;              // A TypeRef's Name.
    LPCUTF8         szNamespaceTmp;         // A TypeRef's Name.
    LPCUTF8         szAsmRefName;           // assembly ref name
    mdToken         tkResTmp;               // TypeRef's resolution scope.
    mdTypeRef       trKnownDiscardable;
    mdMemberRef     mrType;                 // MemberRef token to the discardable TypeRef
    mdCustomAttribute   cv;
    mdTypeDef       td;
    bool            fFoundCompilerDefinedDiscardabeCA = false;
    TypeDefRec      *pTypeDefRec;
    AssemblyRefRec  *pAsmRefRec;
    RID             ridStart, ridEnd;
    CQuickBytes     qbNamespace;            // Namespace buffer.
    CQuickBytes     qbName;                 // Name buffer.
    ULONG           ulStringLen;            // Length of the TypeDef string.
    int             bSuccess;               // Return value for SplitPath().    

    

    if (m_pFilterManager == NULL)
        IfFailGo( MarkAll() );

    trKnownDiscardable = mdTypeRefNil;

    // Now find out all of the TypeDefs that are types for transient CAs
    // Mark all public typedefs.
    iCount = pMiniMd->getCountTypeDefs();

    // Find out the TypeRef referring to our library's System.CompilerServices.DiscardableAttribute
    cTypeRefRecs = pMiniMd->getCountTypeRefs();

    ulStringLen = (ULONG)strlen(COR_COMPILERSERVICE_DISCARDABLEATTRIBUTE_ASNI) + 1;
    IfFailGo(qbNamespace.ReSizeNoThrow(ulStringLen));
    IfFailGo(qbName.ReSizeNoThrow(ulStringLen));
    bSuccess = ns::SplitPath(COR_COMPILERSERVICE_DISCARDABLEATTRIBUTE_ASNI,
                             (LPUTF8)qbNamespace.Ptr(),
                             ulStringLen,
                             (LPUTF8)qbName.Ptr(),
                             ulStringLen);
    _ASSERTE(bSuccess);

    // Search for the TypeRef.
    for (i = 1; i <= cTypeRefRecs; i++)
    {
        pTypeRefRec = pMiniMd->getTypeRef(i);
        szNameTmp = pMiniMd->getNameOfTypeRef(pTypeRefRec);
        szNamespaceTmp = pMiniMd->getNamespaceOfTypeRef(pTypeRefRec);

        if (strcmp(szNameTmp, (LPUTF8)qbName.Ptr()) == 0 && strcmp(szNamespaceTmp, (LPUTF8)qbNamespace.Ptr()) == 0)
        {
            // found a match. Now check the resolution scope. Make sure it is a AssemblyRef to mscorlib.dll
            tkResTmp = pMiniMd->getResolutionScopeOfTypeRef(pTypeRefRec);
            if (TypeFromToken(tkResTmp) == mdtAssemblyRef)
            {
                pAsmRefRec = pMiniMd->getAssemblyRef(RidFromToken(tkResTmp));
                szAsmRefName = pMiniMd->getNameOfAssemblyRef(pAsmRefRec);
                if (SString::_stricmp(szAsmRefName, "mscorlib.dll") == 0)
                {
                    trKnownDiscardable = TokenFromRid(i, mdtTypeRef);
                    break;
                }
            }
        }
    }

    if (trKnownDiscardable != mdTypeRefNil)
    {
        hr = ImportHelper::FindMemberRef(pMiniMd, trKnownDiscardable, COR_CTOR_METHOD_NAME, NULL, 0, &mrType);

        // If we cannot find a MemberRef to the .ctor of the System.CompilerServices.DiscardableAttribute,
        // we won't have any TypeDef with the DiscardableAttribute CAs hang off it.
        //
        if (SUCCEEDED(hr))
        {
            // walk all of the user defined typedef
            for (i = 2; i <= iCount; i++)
            {
                pRec = pMiniMd->getTypeDef(i);
                if (IsTdNotPublic(pRec->GetFlags()))
                {
                    // check to see if there a CA associated with this TypeDef
                    IfFailGo( ImportHelper::FindCustomAttributeByToken(pMiniMd, TokenFromRid(i, mdtTypeDef), mrType, 0, 0, &cv) );
                    if (hr == S_OK)
                    {
                        // yes, this is a compiler defined discardable CA. Unmark the TypeDef

                        // check the shape of the TypeDef
                        // It should have no field, no event, and no property.
                        
                        // no field
                        pTypeDefRec = pMiniMd->getTypeDef( i );
                        td = TokenFromRid(i, mdtTypeDef);
                        ridStart = pMiniMd->getFieldListOfTypeDef( pTypeDefRec );
                        ridEnd = pMiniMd->getEndFieldListOfTypeDef( pTypeDefRec );
                        if ((ridEnd - ridStart) > 0)
                            continue;

                        // no property
                        ridStart = pMiniMd->FindPropertyMapFor( td );
                        if ( !InvalidRid(ridStart) )
                            continue;

                        // no event
                        ridStart = pMiniMd->FindEventMapFor( td );
                        if ( !InvalidRid(ridStart) )
                            continue;

                        IfFailGo( m_pFilterManager->UnmarkTypeDef( td ) );
                        fFoundCompilerDefinedDiscardabeCA = true;                        
                    }
                }
            }
        }
        if (hr == CLDB_E_RECORD_NOTFOUND)
            hr = NOERROR;
    }

ErrExit:
    
    return hr;
}   // HRESULT RegMeta::UnmarkAllTransientCAs


//*****************************************************************************
// determine if a token is valid or not
//*****************************************************************************
BOOL RegMeta::IsValidToken(             // true if tk is valid token
    mdToken     tk)                     // [IN] token to be checked
{
    BOOL fRet = FALSE;
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_VOIDRET;
    
    LOCKREADNORET();

    // If acquiring the lock failed...
    IfFailGo(hr);
    
    fRet = m_pStgdb->m_MiniMd._IsValidToken(tk);

ErrExit:
    END_ENTRYPOINT_VOIDRET;
    return fRet;
}

STDMETHODIMP RegMeta::MarkToken(        // Return code.
    mdToken     tk)                     // [IN] token to be Marked
{
    HRESULT     hr = NOERROR;

    BEGIN_ENTRYPOINT_NOTHROW;

    // LOG((LOGMD, "RegMeta::MarkToken(0x%08x)\n", tk));
    START_MD_PERF();
    LOCKWRITE();

    if (m_pStgdb->m_MiniMd.GetFilterTable() == NULL || m_pFilterManager == NULL)
    {
        // UnmarkAll has not been called. Everything is considered marked.
        // No need to do anything extra!
        IfFailGo( META_E_MUST_CALL_UNMARKALL );
    }

    switch ( TypeFromToken(tk) )
    {
    case mdtTypeDef: 
    case mdtMethodDef:
    case mdtFieldDef:
    case mdtMemberRef:
    case mdtTypeRef:
    case mdtTypeSpec:
    case mdtMethodSpec:
    case mdtSignature:
    case mdtString:
#if _DEBUG
        if (TypeFromToken(tk) == mdtTypeDef)
        {
            TypeDefRec   *pType;
            pType = m_pStgdb->m_MiniMd.getTypeDef(RidFromToken(tk));
            LOG((LOGMD, "MarkToken: Host is marking typetoken 0x%08x with name <%s>\n", tk, m_pStgdb->m_MiniMd.getNameOfTypeDef(pType)));
        }
        else
        if (TypeFromToken(tk) == mdtMethodDef)
        {
            MethodRec   *pMeth;
            pMeth = m_pStgdb->m_MiniMd.getMethod(RidFromToken(tk));
            LOG((LOGMD, "MarkToken: Host is marking methodtoken 0x%08x with name <%s>\n", tk, m_pStgdb->m_MiniMd.getNameOfMethod(pMeth)));
        }
        else
        if (TypeFromToken(tk) == mdtFieldDef)
        {
            FieldRec   *pField;
            pField = m_pStgdb->m_MiniMd.getField(RidFromToken(tk));
            LOG((LOGMD, "MarkToken: Host is marking field token 0x%08x with name <%s>\n", tk, m_pStgdb->m_MiniMd.getNameOfField(pField)));
        }
        else
        {
            LOG((LOGMD, "MarkToken: Host is marking token 0x%08x\n", tk));
        }
#endif // _DEBUG
        if (!IsValidToken(tk))
            IfFailGo( E_INVALIDARG );

        IfFailGo( m_pFilterManager->Mark(tk) );
        break;

    case mdtBaseType:
        // no need to mark base type
        goto ErrExit;

    default:
        _ASSERTE(!"Bad token type!");
        hr = E_INVALIDARG;
        break;
    }
ErrExit:
    
    STOP_MD_PERF(MarkToken);
    END_ENTRYPOINT_NOTHROW;
   
    return hr;
}

HRESULT RegMeta::IsTokenMarked(
    mdToken     tk,                 // [IN] Token to check if marked or not
    BOOL        *pIsMarked)         // [OUT] true if token is marked
{
    HRESULT     hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    FilterTable *pFilter = NULL;

    LOG((LOGMD, "RegMeta::IsTokenMarked(0x%08x)\n", tk));
    START_MD_PERF();
    LOCKREAD();

    pFilter = m_pStgdb->m_MiniMd.GetFilterTable();
    IfNullGo( pFilter );

    if (!IsValidToken(tk))
        IfFailGo( E_INVALIDARG );

    switch ( TypeFromToken(tk) )
    {
    case mdtTypeRef:
        *pIsMarked = pFilter->IsTypeRefMarked(tk);
        break;
    case mdtTypeDef: 
        *pIsMarked = pFilter->IsTypeDefMarked(tk);
        break;
    case mdtFieldDef:
        *pIsMarked = pFilter->IsFieldMarked(tk);
        break;
    case mdtMethodDef:
        *pIsMarked = pFilter->IsMethodMarked(tk);
        break;
    case mdtParamDef:
        *pIsMarked = pFilter->IsParamMarked(tk);
        break;
    case mdtMemberRef:
        *pIsMarked = pFilter->IsMemberRefMarked(tk);
        break;
    case mdtCustomAttribute:
        *pIsMarked = pFilter->IsCustomAttributeMarked(tk);
        break;
    case mdtPermission:
        *pIsMarked = pFilter->IsDeclSecurityMarked(tk);
        break;
    case mdtSignature:
        *pIsMarked = pFilter->IsSignatureMarked(tk);
        break;
    case mdtEvent:
        *pIsMarked = pFilter->IsEventMarked(tk);
        break;
    case mdtProperty:
        *pIsMarked = pFilter->IsPropertyMarked(tk);
        break;
    case mdtModuleRef:
        *pIsMarked = pFilter->IsModuleRefMarked(tk);
        break;
    case mdtTypeSpec:
        *pIsMarked = pFilter->IsTypeSpecMarked(tk);
        break;
    case mdtInterfaceImpl:
        *pIsMarked = pFilter->IsInterfaceImplMarked(tk);
        break;
    case mdtString:
    default:
        _ASSERTE(!"Bad token type!");
        hr = E_INVALIDARG;
        break;
    }
ErrExit:
    
    STOP_MD_PERF(IsTokenMarked);
    END_ENTRYPOINT_NOTHROW;
    
    return hr;
}   // IsTokenMarked

//*****************************************************************************
// Create and populate a new TypeDef record.
//*****************************************************************************
STDMETHODIMP RegMeta::DefineTypeDef(                // S_OK or error.
    LPCWSTR     szTypeDef,              // [IN] Name of TypeDef
    DWORD       dwTypeDefFlags,         // [IN] CustomAttribute flags
    mdToken     tkExtends,              // [IN] extends this TypeDef or typeref 
    mdToken     rtkImplements[],        // [IN] Implements interfaces
    mdTypeDef   *ptd)                   // [OUT] Put TypeDef token here
{
    HRESULT     hr = S_OK;              // A result.

    BEGIN_ENTRYPOINT_NOTHROW;

    LOG((LOGMD, "RegMeta::DefineTypeDef(%S, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\n", 
            MDSTR(szTypeDef), dwTypeDefFlags, tkExtends,
            rtkImplements, ptd));
    START_MD_PERF();
    LOCKWRITE();

    m_pStgdb->m_MiniMd.PreUpdate();

    _ASSERTE(!IsTdNested(dwTypeDefFlags));

    IfFailGo(_DefineTypeDef(szTypeDef, dwTypeDefFlags,
                tkExtends, rtkImplements, mdTokenNil, ptd));
ErrExit:    
    STOP_MD_PERF(DefineTypeDef);

    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // STDMETHODIMP RegMeta::DefineTypeDef()


//*****************************************************************************
//*****************************************************************************
STDMETHODIMP RegMeta::SetHandler(       // S_OK.
    IUnknown    *pUnk)                  // [IN] The new error handler.
{
    HRESULT     hr = S_OK;              // A result.

    BEGIN_ENTRYPOINT_NOTHROW;

    IMapToken *pIMap = NULL;

    LOG((LOGMD, "RegMeta::SetHandler(0x%08x)\n", pUnk));
    START_MD_PERF();
    LOCKWRITE();

    m_pHandler = pUnk;

    // Ignore the error return by SetHandler
    m_pStgdb->m_MiniMd.SetHandler(pUnk);

    // Figure out up front if remap is supported.
    if (pUnk)
        pUnk->QueryInterface(IID_IMapToken, (PVOID *) &pIMap);
    m_bRemap = (pIMap != 0); 
    if (pIMap)
        pIMap->Release();

ErrExit:
    
    STOP_MD_PERF(SetHandler);
    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // STDMETHODIMP RegMeta::SetHandler()

//*****************************************************************************
// As the Stgdb object to get the save size for the metadata delta.
//*****************************************************************************
STDMETHODIMP RegMeta::GetDeltaSaveSize(      // S_OK or error.
    CorSaveSize fSave,                  // [IN] cssAccurate or cssQuick.
    DWORD       *pdwSaveSize)           // [OUT] Put the size here.
{
    HRESULT hr = S_OK; 

    BEGIN_ENTRYPOINT_NOTHROW;

    // Make sure we're in EnC mode
    if (!IsENCOn())
    {
        _ASSERTE(!"Not in EnC mode!");
        IfFailGo(META_E_NOT_IN_ENC_MODE);
    }

    m_pStgdb->m_MiniMd.EnableDeltaMetadataGeneration();
    hr = GetSaveSize(fSave, pdwSaveSize);
    m_pStgdb->m_MiniMd.DisableDeltaMetadataGeneration();

ErrExit:
    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // STDMETHODIMP RegMeta::GetDeltaSaveSize()

//*****************************************************************************
// Saves a metadata delta to a file of a given name.
//*****************************************************************************
STDMETHODIMP RegMeta::SaveDelta(                     // S_OK or error.
    LPCWSTR     szFile,                 // [IN] The filename to save to.
    DWORD       dwSaveFlags)            // [IN] Flags for the save.
{
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    
    // Make sure we're in EnC mode
    if (!IsENCOn())
    {
        _ASSERTE(!"Not in EnC mode!");
        IfFailGo(META_E_NOT_IN_ENC_MODE);
    }

    

    m_pStgdb->m_MiniMd.EnableDeltaMetadataGeneration();
    hr = Save(szFile, dwSaveFlags);
    m_pStgdb->m_MiniMd.DisableDeltaMetadataGeneration();

ErrExit:

    END_ENTRYPOINT_NOTHROW;

    return hr;
} // STDMETHODIMP RegMeta::SaveDelta()

//*****************************************************************************
// Saves a metadata delta to a stream.
//*****************************************************************************
STDMETHODIMP RegMeta::SaveDeltaToStream(     // S_OK or error.
    IStream     *pIStream,              // [IN] A writable stream to save to.
    DWORD       dwSaveFlags)            // [IN] Flags for the save.
{
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    // Make sure we're in EnC mode
    if (!IsENCOn())
    {
        _ASSERTE(!"Not in EnC mode!");
        IfFailGo(META_E_NOT_IN_ENC_MODE);
    }

    

    m_pStgdb->m_MiniMd.EnableDeltaMetadataGeneration();
    hr = SaveToStream(pIStream, dwSaveFlags);
    m_pStgdb->m_MiniMd.DisableDeltaMetadataGeneration();

ErrExit:
    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // STDMETHODIMP RegMeta::SaveDeltaToStream()

//*****************************************************************************
// Saves a copy of the scope into the memory buffer provided.  The buffer size
// must be at least as large as the GetSaveSize value.
//*****************************************************************************
STDMETHODIMP RegMeta::SaveDeltaToMemory(           // S_OK or error.
    void        *pbData,                // [OUT] Location to write data.
    ULONG       cbData)                 // [IN] Max size of data buffer.
{
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    // Make sure we're in EnC mode
    if (!IsENCOn())
    {
        _ASSERTE(!"Not in EnC mode!");
        IfFailGo(META_E_NOT_IN_ENC_MODE);
    }


    m_pStgdb->m_MiniMd.EnableDeltaMetadataGeneration();
    hr = SaveToMemory(pbData, cbData);
    m_pStgdb->m_MiniMd.DisableDeltaMetadataGeneration();

ErrExit:

    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // STDMETHODIMP RegMeta::SaveDeltaToMemory()

//*****************************************************************************
// Resets the current edit and continue session
//*****************************************************************************
STDMETHODIMP RegMeta::ResetENCLog()           // S_OK or error.
{
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    // Make sure we're in EnC mode
    if (!IsENCOn())
    {
        _ASSERTE(!"Not in EnC mode!");
        IfFailGo(META_E_NOT_IN_ENC_MODE);
    }

    IfFailGo(m_pStgdb->m_MiniMd.ResetENCLog());
ErrExit:
    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // STDMETHODIMP RegMeta::ResetENCLog()


//*****************************************************************************
// Close an enumerator.
//*****************************************************************************
void __stdcall RegMeta::CloseEnum(
    HCORENUM        hEnum)          // The enumerator.
{
    BEGIN_CLEANUP_ENTRYPOINT;

    LOG((LOGMD, "RegMeta::CloseEnum(0x%08x)\n", hEnum));

    // No need to lock this function.
    HENUMInternal   *pmdEnum = reinterpret_cast<HENUMInternal *> (hEnum);

    if (pmdEnum == NULL)
        return;

    
    HENUMInternal::DestroyEnum(pmdEnum);
    END_CLEANUP_ENTRYPOINT;
} // void __stdcall RegMeta::CloseEnum()

//*****************************************************************************
// Query the count of items represented by an enumerator.
//*****************************************************************************
HRESULT CountEnum(
    HCORENUM        hEnum,              // The enumerator.
    ULONG           *pulCount)          // Put the count here.
{
    HENUMInternal   *pmdEnum = reinterpret_cast<HENUMInternal *> (hEnum);
    HRESULT         hr = S_OK;

    // No need to lock this function.

    LOG((LOGMD, "RegMeta::CountEnum(0x%08x, 0x%08x)\n", hEnum, pulCount));
    START_MD_PERF();

    _ASSERTE( pulCount );

    if (pmdEnum == NULL)
    {
        *pulCount = 0;
        goto ErrExit;
    }

    if (pmdEnum->m_tkKind == (TBL_MethodImpl << 24))
    {
        // Number of tokens must always be a multiple of 2.
        _ASSERTE(! (pmdEnum->m_ulCount % 2) );
        // There are two entries in the Enumerator for each MethodImpl.
        *pulCount = pmdEnum->m_ulCount / 2;
    }
    else
        *pulCount = pmdEnum->m_ulCount;
ErrExit:
    STOP_MD_PERF(CountEnum);
    return hr;
} // STDMETHODIMP RegMeta::CountEnum()

STDMETHODIMP RegMeta::CountEnum(
    HCORENUM        hEnum,              // The enumerator.
    ULONG           *pulCount)          // Put the count here.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = ::CountEnum(hEnum, pulCount);
    END_ENTRYPOINT_NOTHROW;
    return hr;
} // STDMETHODIMP RegMeta::CountEnum()

//*****************************************************************************
// Reset an enumerator to any position within the enumerator.
//*****************************************************************************
STDMETHODIMP RegMeta::ResetEnum(
    HCORENUM        hEnum,              // The enumerator.
    ULONG           ulPos)              // Seek position.
{
    HRESULT         hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    HENUMInternal   *pmdEnum = reinterpret_cast<HENUMInternal *> (hEnum);

    // No need to lock this function.

    LOG((LOGMD, "RegMeta::ResetEnum(0x%08x, 0x%08x)\n", hEnum, ulPos));
    START_MD_PERF();

    if (pmdEnum == NULL)
        goto ErrExit;

    pmdEnum->u.m_ulCur = pmdEnum->u.m_ulStart + ulPos;

ErrExit:
    STOP_MD_PERF(ResetEnum);
    END_ENTRYPOINT_NOTHROW;
    return hr;
} // STDMETHODIMP RegMeta::ResetEnum()

//*****************************************************************************
// Enumerate Sym.TypeDef.
//*****************************************************************************
STDMETHODIMP RegMeta::EnumTypeDefs(
    HCORENUM    *phEnum,                // Pointer to the enumerator.
    mdTypeDef   rTypeDefs[],            // Put TypeDefs here.
    ULONG       cMax,                   // Max TypeDefs to put.
    ULONG       *pcTypeDefs)            // Put # put here.
{
    HRESULT         hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    HENUMInternal   **ppmdEnum = reinterpret_cast<HENUMInternal **> (phEnum);
    HENUMInternal   *pEnum;

    LOG((LOGMD, "RegMeta::EnumTypeDefs(0x%08x, 0x%08x, 0x%08x, 0x%08x)\n", 
            phEnum, rTypeDefs, cMax, pcTypeDefs));
    START_MD_PERF();
    LOCKREAD();
    

    if ( *ppmdEnum == 0 )
    {
        // instantiating a new ENUM
        CMiniMdRW       *pMiniMd = &(m_pStgdb->m_MiniMd);

        if (pMiniMd->HasDelete() && 
            ((m_OptionValue.m_ImportOption & MDImportOptionAllTypeDefs) == 0))
        {
            IfFailGo( HENUMInternal::CreateDynamicArrayEnum( mdtTypeDef, &pEnum) );

            // add all Types to the dynamic array if name is not _Delete
            for (ULONG index = 2; index <= pMiniMd->getCountTypeDefs(); index ++ )
            {
                TypeDefRec       *pRec = pMiniMd->getTypeDef(index);
                if (IsDeletedName(pMiniMd->getNameOfTypeDef(pRec)) )
                {   
                    continue;
                }
                IfFailGo( HENUMInternal::AddElementToEnum(pEnum, TokenFromRid(index, mdtTypeDef) ) );
            }
        }
        else
        {
            // create the enumerator
            IfFailGo( HENUMInternal::CreateSimpleEnum(
                mdtTypeDef, 
                2, 
                pMiniMd->getCountTypeDefs() + 1, 
                &pEnum) );
        }
        
        // set the output parameter
        *ppmdEnum = pEnum;          
    }
    else
    {
        pEnum = *ppmdEnum;
    }

    // we can only fill the minimun of what caller asked for or what we have left
    hr = HENUMInternal::EnumWithCount(pEnum, cMax, rTypeDefs, pcTypeDefs);

ErrExit:
    HENUMInternal::DestroyEnumIfEmpty(ppmdEnum);
    
    STOP_MD_PERF(EnumTypeDefs);

    END_ENTRYPOINT_NOTHROW;

    return hr;
}   // RegMeta::EnumTypeDefs


//*****************************************************************************
// Enumerate Sym.InterfaceImpl where Coclass == td
//*****************************************************************************
STDMETHODIMP RegMeta::EnumInterfaceImpls(
    HCORENUM        *phEnum,            // Pointer to the enum.
    mdTypeDef       td,                 // TypeDef to scope the enumeration.
    mdInterfaceImpl rImpls[],           // Put InterfaceImpls here.
    ULONG           cMax,               // Max InterfaceImpls to put.
    ULONG           *pcImpls)           // Put # put here.
{
    HRESULT             hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    HENUMInternal       **ppmdEnum = reinterpret_cast<HENUMInternal **> (phEnum);
    ULONG               ridStart;
    ULONG               ridEnd;
    HENUMInternal       *pEnum;
    InterfaceImplRec    *pRec;
    ULONG               index;

    LOG((LOGMD, "RegMeta::EnumInterfaceImpls(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\n", 
            phEnum, td, rImpls, cMax, pcImpls));
    START_MD_PERF();
    LOCKREAD();
    
    _ASSERTE(TypeFromToken(td) == mdtTypeDef);


    if ( *ppmdEnum == 0 )
    {
        // instantiating a new ENUM
        CMiniMdRW       *pMiniMd = &(m_pStgdb->m_MiniMd);
        if ( pMiniMd->IsSorted( TBL_InterfaceImpl ) )
        {
            ridStart = pMiniMd->getInterfaceImplsForTypeDef(RidFromToken(td), &ridEnd);
            IfFailGo( HENUMInternal::CreateSimpleEnum( mdtInterfaceImpl, ridStart, ridEnd, &pEnum) );
        }
        else
        {
            // table is not sorted so we have to create dynmaic array 
            // create the dynamic enumerator
            //
            ridStart = 1;
            ridEnd = pMiniMd->getCountInterfaceImpls() + 1;

            IfFailGo( HENUMInternal::CreateDynamicArrayEnum( mdtInterfaceImpl, &pEnum) );             
            
            for (index = ridStart; index < ridEnd; index ++ )
            {
                pRec = pMiniMd->getInterfaceImpl(index);
                if ( td == pMiniMd->getClassOfInterfaceImpl(pRec) )
                {
                    IfFailGo( HENUMInternal::AddElementToEnum(pEnum, TokenFromRid(index, mdtInterfaceImpl) ) );
                }
            }
        }

        // set the output parameter
        *ppmdEnum = pEnum;          
    }
    else
    {
        pEnum = *ppmdEnum;
    }
    
    // fill the output token buffer
    hr = HENUMInternal::EnumWithCount(pEnum, cMax, rImpls, pcImpls);

ErrExit:
    HENUMInternal::DestroyEnumIfEmpty(ppmdEnum);
    
    STOP_MD_PERF(EnumInterfaceImpls);

    END_ENTRYPOINT_NOTHROW;

    return hr;
} // STDMETHODIMP RegMeta::EnumInterfaceImpls()

STDMETHODIMP RegMeta::EnumGenericParams(HCORENUM *phEnum, mdToken tkOwner,
        mdGenericParam rTokens[], ULONG cMaxTokens, ULONG *pcTokens)
{
    HRESULT             hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    HENUMInternal       **ppmdEnum = reinterpret_cast<HENUMInternal **> (phEnum);
    ULONG               ridStart;
    ULONG               ridEnd;
    HENUMInternal       *pEnum;
    GenericParamRec     *pRec;
    ULONG               index;
    CMiniMdRW           *pMiniMd = NULL;


    LOG((LOGMD, "RegMeta::EnumGenericParams(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\n", 
            phEnum, tkOwner, rTokens, cMaxTokens, pcTokens));
    START_MD_PERF();
    LOCKREAD();

    pMiniMd = &(m_pStgdb->m_MiniMd);


    // See if this version of the metadata can do Generics
    if (!pMiniMd->SupportsGenerics())
    {
        if (pcTokens)
            *pcTokens = 0;
        hr = S_FALSE;
        goto ErrExit;
    }

    
    _ASSERTE(TypeFromToken(tkOwner) == mdtTypeDef || TypeFromToken(tkOwner) == mdtMethodDef);


    if ( *ppmdEnum == 0 )
    {

        if ( pMiniMd->IsSorted( TBL_GenericParam ) )
        {
            if (TypeFromToken(tkOwner) == mdtTypeDef)
              ridStart = pMiniMd->getGenericParamsForTypeDef(RidFromToken(tkOwner), &ridEnd);
            else
              ridStart = pMiniMd->getGenericParamsForMethodDef(RidFromToken(tkOwner), &ridEnd);

            IfFailGo( HENUMInternal::CreateSimpleEnum(mdtGenericParam, ridStart, ridEnd, &pEnum) );
        }
        else
        {
            // table is not sorted so we have to create dynamic array 
            // create the dynamic enumerator
            //
            ridStart = 1;
            ridEnd = pMiniMd->getCountGenericParams() + 1;

            IfFailGo( HENUMInternal::CreateDynamicArrayEnum(mdtGenericParam, &pEnum) );             
            
            for (index = ridStart; index < ridEnd; index ++ )
            {
                pRec = pMiniMd->getGenericParam(index);
                if ( tkOwner == pMiniMd->getOwnerOfGenericParam(pRec) )
                {
                    IfFailGo( HENUMInternal::AddElementToEnum(pEnum, TokenFromRid(index, mdtGenericParam) ) );
                }
            }
        }

        // set the output parameter
        *ppmdEnum = pEnum;          
    }
    else
    {
        pEnum = *ppmdEnum;
    }
    
    // fill the output token buffer
    hr = HENUMInternal::EnumWithCount(pEnum, cMaxTokens, rTokens, pcTokens);

ErrExit:
    HENUMInternal::DestroyEnumIfEmpty(ppmdEnum);
    STOP_MD_PERF(EnumGenericPars);
    END_ENTRYPOINT_NOTHROW;

    return hr;
} // STDMETHODIMP RegMeta::EnumGenericParams()


STDMETHODIMP RegMeta::EnumMethodSpecs(
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
        mdToken      tkOwner,               // [IN] MethodDef or MemberRef whose MethodSpecs are requested
        mdMethodSpec rTokens[],             // [OUT] Put MethodSpecs here.   
        ULONG       cMaxTokens,             // [IN] Max tokens to put.  
        ULONG       *pcTokens)              // [OUT] Put actual count here.
{
    HRESULT             hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    HENUMInternal       **ppmdEnum = reinterpret_cast<HENUMInternal **> (phEnum);
    ULONG               ridStart;
    ULONG               ridEnd;
    HENUMInternal       *pEnum;
    MethodSpecRec       *pRec;
    ULONG               index;
    CMiniMdRW       *pMiniMd = NULL;

    LOG((LOGMD, "RegMeta::EnumMethodSpecs(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\n", 
            phEnum, tkOwner, rTokens, cMaxTokens, pcTokens));
    START_MD_PERF();
    LOCKREAD();

    pMiniMd = &(m_pStgdb->m_MiniMd);

    // See if this version of the metadata can do Generics
    if (!pMiniMd->SupportsGenerics())
    {
        if (pcTokens)
            *pcTokens = 0;
        hr = S_FALSE;
        goto ErrExit;
    }

    
    _ASSERTE(RidFromToken(tkOwner)==0 || TypeFromToken(tkOwner) == mdtMethodDef || TypeFromToken(tkOwner) == mdtMemberRef);


    if ( *ppmdEnum == 0 )
    {
        // instantiating a new ENUM

        if(RidFromToken(tkOwner)==0) // enumerate all MethodSpecs
        {
            ridStart = 1;
            ridEnd = pMiniMd->getCountMethodSpecs() + 1;
    
            IfFailGo( HENUMInternal::CreateSimpleEnum( mdtMethodSpec, ridStart, ridEnd, &pEnum) );
        }
        else
        {
            if ( pMiniMd->IsSorted( TBL_MethodSpec ) )
            {
                if (TypeFromToken(tkOwner) == mdtMemberRef)
                  ridStart = pMiniMd->getMethodSpecsForMemberRef(RidFromToken(tkOwner), &ridEnd);
                else
                  ridStart = pMiniMd->getMethodSpecsForMethodDef(RidFromToken(tkOwner), &ridEnd);
    
                IfFailGo( HENUMInternal::CreateSimpleEnum(mdtMethodSpec, ridStart, ridEnd, &pEnum) );
            }
            else
            {
                // table is not sorted so we have to create dynamic array 
                // create the dynamic enumerator
                //
                ridStart = 1;
                ridEnd = pMiniMd->getCountMethodSpecs() + 1;
    
                IfFailGo( HENUMInternal::CreateDynamicArrayEnum(mdtMethodSpec, &pEnum) );             
                
                for (index = ridStart; index < ridEnd; index ++ )
                {
                    pRec = pMiniMd->getMethodSpec(index);
                    if ( tkOwner == pMiniMd->getMethodOfMethodSpec(pRec) )
                    {
                        IfFailGo( HENUMInternal::AddElementToEnum(pEnum, TokenFromRid(index, mdtMethodSpec) ) );
                    }
                }
            }
        }
        // set the output parameter
        *ppmdEnum = pEnum;          
    }
    else
    {
        pEnum = *ppmdEnum;
    }
    
    // fill the output token buffer
    hr = HENUMInternal::EnumWithCount(pEnum, cMaxTokens, rTokens, pcTokens);

ErrExit:
    HENUMInternal::DestroyEnumIfEmpty(ppmdEnum);
    STOP_MD_PERF(EnumMethodSpecs);
    END_ENTRYPOINT_NOTHROW;

    return hr;
} // STDMETHODIMP RegMeta::EnumMethodSpecs()

STDMETHODIMP RegMeta::EnumGenericParamConstraints(
    HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
    mdGenericParam tkOwner,             // [IN] GenericParam whose constraints are requested
    mdGenericParamConstraint rTokens[],    // [OUT] Put GenericParamConstraints here.   
    ULONG       cMaxTokens,                   // [IN] Max GenericParamConstraints to put.  
    ULONG       *pcTokens)              // [OUT] Put # of tokens here.    
{
    HRESULT             hr = S_OK;
    
    BEGIN_ENTRYPOINT_NOTHROW;

    HENUMInternal       **ppmdEnum = reinterpret_cast<HENUMInternal **> (phEnum);
    ULONG               ridStart;
    ULONG               ridEnd;
    HENUMInternal       *pEnum;
    GenericParamConstraintRec     *pRec;
    ULONG               index;
    CMiniMdRW       *pMiniMd = NULL;

    LOG((LOGMD, "RegMeta::EnumGenericParamConstraints(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\n", 
            phEnum, tkOwner, rTokens, cMaxTokens, pcTokens));
    START_MD_PERF();
    LOCKREAD();

    pMiniMd = &(m_pStgdb->m_MiniMd);


    if(TypeFromToken(tkOwner) != mdtGenericParam)
        IfFailGo(META_E_BAD_INPUT_PARAMETER);

    // See if this version of the metadata can do Generics
    if (!pMiniMd->SupportsGenerics())
    {
        if (pcTokens)
            *pcTokens = 0;
        hr = S_FALSE;
        goto ErrExit;
    }

    if ( *ppmdEnum == 0 )
    {
        // instantiating a new ENUM

        if ( pMiniMd->IsSorted( TBL_GenericParamConstraint ) )
        {
            ridStart = pMiniMd->getGenericParamConstraintsForGenericParam(RidFromToken(tkOwner), &ridEnd);
            IfFailGo( HENUMInternal::CreateSimpleEnum(mdtGenericParamConstraint, ridStart, ridEnd, &pEnum) );
        }
        else
        {
            // table is not sorted so we have to create dynamic array 
            // create the dynamic enumerator
            //
            ridStart = 1;
            ridEnd = pMiniMd->getCountGenericParamConstraints() + 1;

            IfFailGo( HENUMInternal::CreateDynamicArrayEnum(mdtGenericParamConstraint, &pEnum));             

            for (index = ridStart; index < ridEnd; index ++ )
            {
                pRec = pMiniMd->getGenericParamConstraint(index);
                if ( tkOwner == pMiniMd->getOwnerOfGenericParamConstraint(pRec))
                {
                    IfFailGo( HENUMInternal::AddElementToEnum(pEnum, TokenFromRid(index, 
                                                                       mdtGenericParamConstraint)));
                }
            }
        }

        // set the output parameter
        *ppmdEnum = pEnum;          
    }
    else
    {
        pEnum = *ppmdEnum;
    }
    
    // fill the output token buffer
    hr = HENUMInternal::EnumWithCount(pEnum, cMaxTokens, rTokens, pcTokens);

ErrExit:
    HENUMInternal::DestroyEnumIfEmpty(ppmdEnum);
    STOP_MD_PERF(EnumGenericParamConstraints);
    END_ENTRYPOINT_NOTHROW;
    
    return hr;
}

//*****************************************************************************
// Enumerate Sym.TypeRef
//*****************************************************************************
STDMETHODIMP RegMeta::EnumTypeRefs(
    HCORENUM        *phEnum,            // Pointer to the enumerator.
    mdTypeRef       rTypeRefs[],        // Put TypeRefs here.
    ULONG           cMax,               // Max TypeRefs to put.
    ULONG           *pcTypeRefs)        // Put # put here.
{
    HRESULT         hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    HENUMInternal   **ppmdEnum = reinterpret_cast<HENUMInternal **> (phEnum);
    ULONG           cTotal;
    HENUMInternal   *pEnum = *ppmdEnum;

    

    LOG((LOGMD, "RegMeta::EnumTypeRefs(0x%08x, 0x%08x, 0x%08x, 0x%08x)\n", 
            phEnum, rTypeRefs, cMax, pcTypeRefs));
    START_MD_PERF();
    LOCKREAD();

    if ( pEnum == 0 )
    {
        // instantiating a new ENUM
        CMiniMdRW       *pMiniMd = &(m_pStgdb->m_MiniMd);
        cTotal = pMiniMd->getCountTypeRefs();

        IfFailGo( HENUMInternal::CreateSimpleEnum( mdtTypeRef, 1, cTotal + 1, &pEnum) );

        // set the output parameter
        *ppmdEnum = pEnum;          
    }
    
    // fill the output token buffer
    hr = HENUMInternal::EnumWithCount(pEnum, cMax, rTypeRefs, pcTypeRefs);
        
ErrExit:
    HENUMInternal::DestroyEnumIfEmpty(ppmdEnum);

    
    STOP_MD_PERF(EnumTypeRefs);
    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // STDMETHODIMP RegMeta::EnumTypeRefs()

//*****************************************************************************
// Given a namespace and a class name, return the typedef
//*****************************************************************************
STDMETHODIMP RegMeta::FindTypeDefByName(// S_OK or error.
    LPCWSTR     wzTypeDef,              // [IN] Name of the Type.
    mdToken     tkEnclosingClass,       // [IN] Enclosing class.
    mdTypeDef   *ptd)                   // [OUT] Put the TypeDef token here.
{
    HRESULT     hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW

    LOG((LOGMD, "{%08x} RegMeta::FindTypeDefByName(%S, 0x%08x, 0x%08x)\n", 
            this, MDSTR(wzTypeDef), tkEnclosingClass, ptd));
    START_MD_PERF();
    LOCKREAD();


    if (wzTypeDef == NULL)
        IfFailGo(E_INVALIDARG);
    PREFIX_ASSUME(wzTypeDef != NULL);
    LPSTR       szTypeDef;
    UTF8STR(wzTypeDef, szTypeDef);
    LPCSTR      szNamespace;
    LPCSTR      szName;

    

    
    _ASSERTE(ptd);
    _ASSERTE(TypeFromToken(tkEnclosingClass) == mdtTypeDef ||
             TypeFromToken(tkEnclosingClass) == mdtTypeRef ||
             IsNilToken(tkEnclosingClass));

    // initialize output parameter
    *ptd = mdTypeDefNil;

    ns::SplitInline(szTypeDef, szNamespace, szName);
    hr = ImportHelper::FindTypeDefByName(&(m_pStgdb->m_MiniMd),
                                        szNamespace,
                                        szName,
                                        tkEnclosingClass,
                                        ptd);
ErrExit:
    
    STOP_MD_PERF(FindTypeDefByName);
    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // STDMETHODIMP RegMeta::FindTypeDefByName()

//*****************************************************************************
// Get values from Sym.Module
//*****************************************************************************
STDMETHODIMP RegMeta::GetScopeProps(
    __out_ecount_opt (cchName) LPWSTR szName, // Put name here
    ULONG       cchName,                // Size in chars of name buffer
    ULONG       *pchName,               // Put actual length of name here
    GUID        *pmvid)                 // Put MVID here
{
    HRESULT     hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    CMiniMdRW   *pMiniMd = &(m_pStgdb->m_MiniMd);
    ModuleRec   *pModuleRec;
 

    LOG((LOGMD, "RegMeta::GetScopeProps(%S, 0x%08x, 0x%08x, 0x%08x)\n", 
            MDSTR(szName), cchName, pchName, pmvid));
    START_MD_PERF();
    LOCKREAD();

    // there is only one module record
    pModuleRec = pMiniMd->getModule(1);

    if (pmvid)
    {
        pMiniMd->getMvidOfModule(pModuleRec, pmvid);
    }
    if (szName || pchName)
        IfFailGo( pMiniMd->getNameOfModule(pModuleRec, szName, cchName, pchName) );
ErrExit:
    
    STOP_MD_PERF(GetScopeProps);
    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // STDMETHODIMP RegMeta::GetScopeProps()

//*****************************************************************************
// Get the token for a Scope's (primary) module record.
//*****************************************************************************
STDMETHODIMP RegMeta::GetModuleFromScope(// S_OK.
    mdModule    *pmd)                   // [OUT] Put mdModule token here.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    LOG((LOGMD, "RegMeta::GetModuleFromScope(0x%08x)\n", pmd));
    START_MD_PERF();

    _ASSERTE(pmd);

    // No need to lock this function.

    *pmd = TokenFromRid(1, mdtModule);

    STOP_MD_PERF(GetModuleFromScope);
    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // STDMETHODIMP RegMeta::GetModuleFromScope()

//*****************************************************************************
// Given a token, is it (or its parent) global?
//*****************************************************************************
HRESULT RegMeta::IsGlobal(              // S_OK ir error.
    mdToken     tk,                     // [IN] Type, Field, or Method token.
    int         *pbGlobal)              // [OUT] Put 1 if global, 0 otherwise.
{
    HRESULT     hr=S_OK;                // A result.

    BEGIN_ENTRYPOINT_NOTHROW;

    CMiniMdRW   *pMiniMd = &(m_pStgdb->m_MiniMd);
    mdToken     tkParent;               // Parent of field or method.
    
    LOG((LOGMD, "RegMeta::GetTokenForGlobalType(0x%08x, %08x)\n", tk, pbGlobal));
    //START_MD_PERF();

    // No need to lock this function.
    
    if (!IsValidToken(tk))
        IfFailGo(E_INVALIDARG);
    
    switch (TypeFromToken(tk))
    {
    case mdtTypeDef:
        *pbGlobal = IsGlobalMethodParentToken(tk);
        break;
        
    case mdtFieldDef:
        IfFailGo( pMiniMd->FindParentOfFieldHelper(tk, &tkParent) );
        *pbGlobal = IsGlobalMethodParentToken(tkParent);
        break;
        
    case mdtMethodDef:
        IfFailGo( pMiniMd->FindParentOfMethodHelper(tk, &tkParent) );
        *pbGlobal = IsGlobalMethodParentToken(tkParent);
        break;
        
    case mdtProperty:
        IfFailGo( pMiniMd->FindParentOfPropertyHelper(tk, &tkParent) );
        *pbGlobal = IsGlobalMethodParentToken(tkParent);
        break;
        
    case mdtEvent:
        IfFailGo( pMiniMd->FindParentOfEventHelper(tk, &tkParent) );
        *pbGlobal = IsGlobalMethodParentToken(tkParent);
        break;
        
    // Anything else is NOT global.
    default:
        *pbGlobal = FALSE;
    }

ErrExit:
    //STOP_MD_PERF(GetModuleFromScope);
    END_ENTRYPOINT_NOTHROW;

    return hr;
} // HRESULT RegMeta::IsGlobal()

//*****************************************************************************
// return flags for a given class
//*****************************************************************************
HRESULT RegMeta::GetTypeDefProps(  // S_OK or error.
    mdTypeDef   td,                     // [IN] TypeDef token for inquiry.
    __out_ecount_opt (cchTypeDef) LPWSTR szTypeDef, // [OUT] Put name here.
    ULONG       cchTypeDef,             // [IN] size of name buffer in wide chars.
    ULONG       *pchTypeDef,            // [OUT] put size of name (wide chars) here.
    DWORD       *pdwTypeDefFlags,       // [OUT] Put flags here.
    mdToken     *ptkExtends)            // [OUT] Put base class TypeDef/TypeRef here.
{
    HRESULT     hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    CMiniMdRW   *pMiniMd = &(m_pStgdb->m_MiniMd);
    TypeDefRec  *pTypeDefRec;
    int         bTruncation=0;          // Was there name truncation?

    LOG((LOGMD, "{%08x} RegMeta::GetTypeDefProps(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\n", 
            this, td, szTypeDef, cchTypeDef, pchTypeDef,
            pdwTypeDefFlags, ptkExtends));
    START_MD_PERF();
    LOCKREAD();

    if (TypeFromToken(td) != mdtTypeDef)
    {
        hr = S_FALSE;
        goto ErrExit;
    }
    _ASSERTE(TypeFromToken(td) == mdtTypeDef);

    pTypeDefRec = pMiniMd->getTypeDef(RidFromToken(td));

    if (szTypeDef || pchTypeDef)
    {
        LPCSTR  szNamespace;
        LPCSTR  szName;

        szNamespace = pMiniMd->getNamespaceOfTypeDef(pTypeDefRec);
        PREFIX_ASSUME(szNamespace != NULL);
        MAKE_WIDEPTR_FROMUTF8_NOTHROW(wzNamespace, szNamespace);
        IfNullGo(wzNamespace);

        szName = pMiniMd->getNameOfTypeDef(pTypeDefRec);
        PREFIX_ASSUME(szName != NULL);
        MAKE_WIDEPTR_FROMUTF8_NOTHROW(wzName, szName);
        IfNullGo(wzName);

        if (szTypeDef)
            bTruncation = ! (ns::MakePath(szTypeDef, cchTypeDef, wzNamespace, wzName));
        if (pchTypeDef)
        {
            if (bTruncation || !szTypeDef)
                *pchTypeDef = ns::GetFullLength(wzNamespace, wzName);
            else
                *pchTypeDef = (ULONG)(wcslen(szTypeDef) + 1);
        }
    }
    if (pdwTypeDefFlags)
    {
        // caller wants type flags
        *pdwTypeDefFlags = pMiniMd->getFlagsOfTypeDef(pTypeDefRec);
    }
    if (ptkExtends)
    {
        *ptkExtends = pMiniMd->getExtendsOfTypeDef(pTypeDefRec);

        // take care of the 0 case
        if (RidFromToken(*ptkExtends) == 0)
            *ptkExtends = mdTypeRefNil;
    }

    if (bTruncation && hr == S_OK)
        hr = CLDB_S_TRUNCATION;


ErrExit:
    ;
    END_ENTRYPOINT_NOTHROW;

    STOP_MD_PERF(GetTypeDefProps);
    return hr;
} // STDMETHODIMP RegMeta::GetTypeDefProps()


//*****************************************************************************
// Retrieve information about an implemented interface.
//*****************************************************************************
STDMETHODIMP RegMeta::GetInterfaceImplProps(        // S_OK or error.
    mdInterfaceImpl iiImpl,             // [IN] InterfaceImpl token.
    mdTypeDef   *pClass,                // [OUT] Put implementing class token here.
    mdToken     *ptkIface)              // [OUT] Put implemented interface token here.
{
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    CMiniMdRW       *pMiniMd = NULL;
    InterfaceImplRec *pIIRec = NULL;

    
    
    LOG((LOGMD, "RegMeta::GetInterfaceImplProps(0x%08x, 0x%08x, 0x%08x)\n", 
            iiImpl, pClass, ptkIface));
    START_MD_PERF();
    LOCKREAD();

    _ASSERTE(TypeFromToken(iiImpl) == mdtInterfaceImpl);

    pMiniMd = &(m_pStgdb->m_MiniMd);
    pIIRec = pMiniMd->getInterfaceImpl(RidFromToken(iiImpl));

    if (pClass)
    {
        *pClass = pMiniMd->getClassOfInterfaceImpl(pIIRec);     
    }
    if (ptkIface)
    {
        *ptkIface = pMiniMd->getInterfaceOfInterfaceImpl(pIIRec);       
    }

ErrExit:
    STOP_MD_PERF(GetInterfaceImplProps);
    END_ENTRYPOINT_NOTHROW;

    return hr;
} // STDMETHODIMP RegMeta::GetInterfaceImplProps()

//*****************************************************************************
// Retrieve information about a TypeRef.
//*****************************************************************************
STDMETHODIMP RegMeta::GetTypeRefProps(
    mdTypeRef   tr,                     // The class ref token.
    mdToken     *ptkResolutionScope,    // Resolution scope, ModuleRef or AssemblyRef.
    __out_ecount_opt (cchTypeRef) LPWSTR szTypeRef, // Put the name here.
    ULONG       cchTypeRef,             // Size of the name buffer, wide chars.
    ULONG       *pchTypeRef)            // Put actual size of name here.
{
    HRESULT     hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    LOG((LOGMD, "RegMeta::GetTypeRefProps(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\n",
        tr, ptkResolutionScope, szTypeRef, cchTypeRef, pchTypeRef));

    START_MD_PERF();
    LOCKREAD();
    //    _ASSERTE(TypeFromToken(tr) == mdtTypeRef);

    if (TypeFromToken(tr) != mdtTypeRef)
    {
      hr = S_FALSE;
      goto ErrExit;
    }

    CMiniMdRW   *pMiniMd;
    TypeRefRec  *pTypeRefRec;
    int          bTruncation;
    pMiniMd = &(m_pStgdb->m_MiniMd);
    pTypeRefRec = pMiniMd->getTypeRef(RidFromToken(tr));
    bTruncation=0;          // Was there name truncation?

    if (ptkResolutionScope)
        *ptkResolutionScope = pMiniMd->getResolutionScopeOfTypeRef(pTypeRefRec);

    if (szTypeRef || pchTypeRef)
    {
        LPCSTR  szNamespace;
        LPCSTR  szName;

        szNamespace = pMiniMd->getNamespaceOfTypeRef(pTypeRefRec);
        PREFIX_ASSUME(szNamespace != NULL);
        MAKE_WIDEPTR_FROMUTF8_NOTHROW(wzNamespace, szNamespace);
        IfNullGo(wzNamespace);

        szName = pMiniMd->getNameOfTypeRef(pTypeRefRec);
        PREFIX_ASSUME(szName != NULL);
        MAKE_WIDEPTR_FROMUTF8_NOTHROW(wzName, szName);
        IfNullGo(wzName);

        if (szTypeRef)
            bTruncation = ! (ns::MakePath(szTypeRef, cchTypeRef, wzNamespace, wzName));
        if (pchTypeRef)
        {
            if (bTruncation || !szTypeRef)
                *pchTypeRef = ns::GetFullLength(wzNamespace, wzName);
            else
                *pchTypeRef = (ULONG)(wcslen(szTypeRef) + 1);
        }
    }
    if (bTruncation && hr == S_OK)
        hr = CLDB_S_TRUNCATION;
ErrExit:
    STOP_MD_PERF(GetTypeRefProps);
    END_ENTRYPOINT_NOTHROW;
    return hr;
} // STDMETHODIMP RegMeta::GetTypeRefProps()

//*****************************************************************************
// Resolving a typeref
//*****************************************************************************
//#define NEW_RESOLVE_TYPEREF 1

STDMETHODIMP RegMeta::ResolveTypeRef(
    mdTypeRef   tr, 
    REFIID      riid, 
    IUnknown    **ppIScope, 
    mdTypeDef   *ptd)
{
    HRESULT     hr;

    BEGIN_ENTRYPOINT_NOTHROW;

    RegMeta     *pMeta = 0;
    TypeRefRec  *pTypeRefRec;
    WCHAR       wzNameSpace[_MAX_PATH];
    CMiniMdRW   *pMiniMd = NULL;


    

    LOG((LOGMD, "{%08x} RegMeta::ResolveTypeRef(0x%08x, 0x%08x, 0x%08x, 0x%08x)\n",
            this, tr, riid, ppIScope, ptd));

    START_MD_PERF();
    LOCKREAD();

    pMiniMd = &(m_pStgdb->m_MiniMd);

    _ASSERTE(ppIScope && ptd);

    // Init the output values.
    *ppIScope = 0;
    *ptd = 0;

    if (IsNilToken(tr))
    {
        if (ptd != NULL) 
        {
            *ptd = mdTypeDefNil;       
        }

        if (ppIScope != NULL)
        {
            *ppIScope = NULL;
        }
        
        STOP_MD_PERF(ResolveTypeRef);
        hr = E_INVALIDARG;
        goto ErrExit;
    }

    if (TypeFromToken(tr) == mdtTypeDef)
    {
        // Shortcut when we receive a TypeDef token
        *ptd = tr;
        STOP_MD_PERF(ResolveTypeRef);
        hr = this->QueryInterface(riid, (void **)ppIScope);
        goto ErrExit;
    }

    // Get the class ref row.
    _ASSERTE(TypeFromToken(tr) == mdtTypeRef);


    pTypeRefRec = pMiniMd->getTypeRef(RidFromToken(tr));
    IfFailGo( pMiniMd->getNamespaceOfTypeRef(pTypeRefRec, wzNameSpace, lengthof(wzNameSpace), 0) );

    //***********************
    // before we go off to CORPATH, check the loaded modules!
    //***********************
    if ( LOADEDMODULES::ResolveTypeRefWithLoadedModules(
                tr,
                pMiniMd,
                riid,
                ppIScope,
                ptd)  == NOERROR )
    {
        // Done!! We found one match among the loaded modules.
        goto ErrExit;
    }

        IfFailGo( META_E_CANNOTRESOLVETYPEREF );

ErrExit:

    if (FAILED(hr))
    {
        if (pMeta) delete pMeta;
    }

    
    STOP_MD_PERF(ResolveTypeRef);
    END_ENTRYPOINT_NOTHROW;
    
    return (hr);

} // STDMETHODIMP RegMeta::ResolveTypeRef()} // STDMETHODIMP RegMeta::ResolveTypeRef()


//*****************************************************************************
// Given a TypeRef name, return the typeref
//*****************************************************************************
STDMETHODIMP RegMeta::FindTypeRef(      // S_OK or error.
    mdToken     tkResolutionScope,      // [IN] Resolution Scope.
    LPCWSTR     wzTypeName,             // [IN] Name of the TypeRef.
    mdTypeRef   *ptk)                   // [OUT] Put the TypeRef token here.
{
    HRESULT     hr = S_OK;              // A result.

    BEGIN_ENTRYPOINT_NOTHROW;

    LPUTF8      szFullName;
    LPCUTF8     szNamespace;
    LPCUTF8     szName;
    CMiniMdRW   *pMiniMd = &(m_pStgdb->m_MiniMd);

    _ASSERTE(wzTypeName && ptk);

    

    LOG((LOGMD, "RegMeta::FindTypeRef(0x%8x, %ls, 0x%08x)\n", 
            tkResolutionScope, MDSTR(wzTypeName), ptk));
    START_MD_PERF();
    LOCKREAD();

    // Convert the  name to UTF8.
    PREFIX_ASSUME(wzTypeName != NULL); // caller might pass NULL, but they'll AV.
    UTF8STR(wzTypeName, szFullName);
    ns::SplitInline(szFullName, szNamespace, szName);

    // Look up the name.
    hr = ImportHelper::FindTypeRefByName(pMiniMd, tkResolutionScope,
                                         szNamespace,
                                         szName,
                                         ptk);
ErrExit:

    STOP_MD_PERF(FindTypeRef);
    END_ENTRYPOINT_NOTHROW;
    
    return hr;
} // STDMETHODIMP RegMeta::FindTypeRef()


//*****************************************************************************
// IUnknown
//*****************************************************************************

ULONG RegMeta::AddRef()
{
    return InterlockedIncrement(&m_cRef);
} // ULONG RegMeta::AddRef()

ULONG RegMeta::Release()
{
    BEGIN_CLEANUP_ENTRYPOINT;   

    _ASSERTE(!m_bCached || LOADEDMODULES::IsEntryInList(this));
    BOOL    bCached = m_bCached;
    ULONG   cRef = InterlockedDecrement(&m_cRef);
    // NOTE: 'this' may be unsafe after this point, if the module is cached, and
    //  another thread finds the module in the cache, releases it, and deletes it
    //  before we get around to deleting it. (That's why we must make a local copy
    //  of m_bCached.)
    // If no references left...
    if (cRef == 0)
    {   
        if (!bCached)
        {   // If the module is not (was not) cached, no other thread can have
            //  discovered the module, so this thread can now safely delete it.
            delete this;
        }
        else 
        if (LOADEDMODULES::RemoveModuleFromLoadedList(this))
        {   // If the module was cached, RemoveModuleFromLoadedList() will try to
            //  safely un-publish the module, and if it succeeds, no other thread
            //  has (or will) discover the module, so this thread can delete it.
            m_bCached = false;
            delete this;
        }
    }   
    END_CLEANUP_ENTRYPOINT
    
    return (cRef);
} // ULONG RegMeta::Release()

HRESULT RegMeta::QueryInterface(REFIID riid, void **ppUnk)
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;
    int         bRW = false;            // Is requested interface R/W?
    *ppUnk = 0;

    if (riid == IID_IUnknown)
        *ppUnk = (IUnknown *) (IMetaDataEmit2 *) this;

    else if (riid == IID_IMetaDataEmit)
        *ppUnk = (IMetaDataEmit2 *) this,                        bRW = true;
    else if (riid == IID_IMetaDataImport)
        *ppUnk = (IMetaDataImport2 *) this;

    else if (riid == IID_IMetaDataImport2)
        *ppUnk = (IMetaDataImport2 *) this;

    else if (riid == IID_IMetaDataEmit2)
        *ppUnk = (IMetaDataEmit2 *) this,                        bRW = true;

    else if (riid == IID_IMetaDataEmitHelper)
        *ppUnk = (IMetaDataEmitHelper *) this,                  bRW = true;
    else if (riid == IID_IMetaDataAssemblyEmit)
        *ppUnk = (IMetaDataAssemblyEmit *) this,                bRW = true;
    else if (riid == IID_IMetaDataAssemblyImport)
        *ppUnk = (IMetaDataAssemblyImport *) this;
    else if (riid == IID_IMetaDataValidate)
        *ppUnk = (IMetaDataValidate *) this;
    else if (riid == IID_IMetaDataFilter)
        *ppUnk = (IMetaDataFilter *) this;
    else if (riid == IID_IMetaDataHelper)
        *ppUnk = (IMetaDataHelper *) this;
    else if (riid == IID_IMetaDataTables)
        *ppUnk = static_cast<IMetaDataTables *>(this);
    else if (riid == IID_IMetaDataTables2)
        *ppUnk = static_cast<IMetaDataTables2 *>(this);
    else if (riid == IID_IMDInternalEmit)
        *ppUnk = static_cast<IMDInternalEmit *>(this);
    else
        IfFailGo(E_NOINTERFACE);


    if (bRW && IsOfReadOnly(m_OpenFlags))
    {
        // They are asking for a read/write interface and this scope was
        // opened as Read-Only

        *ppUnk = 0;
        IfFailGo(CLDB_E_INCOMPATIBLE);
    }

    if (bRW)
    {
        LOCKWRITENORET();

        if (SUCCEEDED(hr))
        {
            hr = m_pStgdb->m_MiniMd.ConvertToRW();
        }

        if (FAILED(hr))
        {
            *ppUnk = 0;
            goto ErrExit;
        }
    }

    AddRef();
ErrExit:
    
    END_ENTRYPOINT_NOTHROW;

    return hr;
}


//*****************************************************************************
// Called after a scope is opened to set up any add'l state.  Set the value
// for m_tdModule.
//*****************************************************************************
HRESULT RegMeta::PostOpen()    
{
    // There must always be a Global Module class and its the first entry in
    // the TypeDef table.
    m_tdModule = TokenFromRid(1, mdtTypeDef);
    
    // We don't care about failures yet.
    return (S_OK);
} // HRESULT RegMeta::PostOpen()

//*******************************************************************************
// Internal helper functions.
//*******************************************************************************

//*******************************************************************************
// Perform optimizations of the metadata prior to saving.
//*******************************************************************************
HRESULT RegMeta::PreSave()              // Return code.
{
    HRESULT     hr = S_OK;              // A result.
    CMiniMdRW   *pMiniMd;               // The MiniMd with the data.
    unsigned    bRemapOld = m_bRemap;
    MergeTokenManager *ptkMgr = NULL;

    

    // For convenience.
    pMiniMd = &(m_pStgdb->m_MiniMd);

    m_pStgdb->m_MiniMd.PreUpdate();

    // If the code has already been optimized there is nothing to do.
    if (m_bSaveOptimized)
        goto ErrExit;


    if (m_newMerger.m_pImportDataList)
    {
        // This is the linker scenario. We we have IMap for each scope. We will create an instance of our own mapper
        // who knows how to send notification back to host!

        // cache the host provided handler to the end our MergeTokenManager

        ptkMgr = new (nothrow) MergeTokenManager (m_newMerger.m_pImportDataList->m_pMDTokenMap, m_pHandler);
        IfNullGo( ptkMgr );
        hr = m_pStgdb->m_MiniMd.SetHandler( ptkMgr );
        _ASSERTE( SUCCEEDED(hr) );
    }



    IfFailGo( RefToDefOptimization() );

    IfFailGo( ProcessFilter() );

    if (m_newMerger.m_pImportDataList)
    {

        // Allocate a token mapper object that will be used for phase 1 if there is not Handler but 
        // linker has provided the IMapToken
        //
        m_bRemap = true;
    }

    // reget the minimd because it can be swapped in the call of ProcessFilter
    pMiniMd = &(m_pStgdb->m_MiniMd);


    // Don't repeat this process again.
    m_bSaveOptimized = true;

    // call get save size to trigger the PreSaveXXX on MetaModelRW class.
    IfFailGo( m_pStgdb->m_MiniMd.PreSave() );
    
ErrExit:
    if ( ptkMgr )
    {

        // recovery the initial state
        hr = m_pStgdb->m_MiniMd.SetHandler(NULL);
        ptkMgr->Release();
    }

    m_bRemap =  bRemapOld;
   
    return (hr);
} // HRESULT RegMeta::PreSave()



//*******************************************************************************
// Perform optimizations of ref to def
//*******************************************************************************
HRESULT RegMeta::RefToDefOptimization()
{
    mdToken     mfdef;                  // Method or Field Def.
    LPCSTR      szName;                 // MemberRef or TypeRef name.
    const COR_SIGNATURE *pvSig;         // Signature of the MemberRef.
    ULONG       cbSig;                  // Size of the signature blob.
    HRESULT     hr = S_OK;              // A result.
    ULONG       iMR;                    // For iterating MemberRefs.
    CMiniMdRW   *pMiniMd;               // The MiniMd with the data.
    ULONG       cMemberRefRecs;         // Count of MemberRefs.
    MemberRefRec *pMemberRefRec;        // A MemberRefRec.

    

    START_MD_PERF();

    // the Ref to Def map is still up-to-date
    if (IsMemberDefDirty() == false && IsTypeDefDirty() == false && m_hasOptimizedRefToDef == true)
        goto ErrExit;

    pMiniMd = &(m_pStgdb->m_MiniMd);

    // The basic algorithm here is:
    //
    //      calculate all of the TypeRef to TypeDef map and store it at TypeRefToTypeDefMap
    //      for each MemberRef mr
    //      {
    //          get the parent of mr
    //          if (parent of mr is a TypeRef and has been mapped to a TypeDef)
    //          {
    //              Remap MemberRef to MemberDef
    //          }
    //      }
    //
    // There are several places where errors are eaten, since this whole thing is
    // an optimization step and not doing it would still be valid.
    //

    // Ensure the size
    // initialize the token remap manager. This class will track all of the Refs to Defs map and also
    // token movements due to removing pointer tables or sorting.
    //
    if ( pMiniMd->GetTokenRemapManager() == NULL) 
    {

        IfFailGo( pMiniMd->InitTokenRemapManager() );
    }
    else
    {
        IfFailGo( pMiniMd->GetTokenRemapManager()->ClearAndEnsureCapacity(pMiniMd->getCountTypeRefs(), pMiniMd->getCountMemberRefs()));
    }

    // If this is the first time or more TypeDef has been introduced, recalculate the TypeRef to TypeDef map
    if (IsTypeDefDirty() || m_hasOptimizedRefToDef == false)
    {
        IfFailGo( pMiniMd->CalculateTypeRefToTypeDefMap() );
    }

    // If this is the first time or more memberdefs has been introduced, recalculate the TypeRef to TypeDef map
    if (IsMemberDefDirty() || m_hasOptimizedRefToDef == false)
    {
        mdToken     tkParent;
        cMemberRefRecs = pMiniMd->getCountMemberRefs();

        // Enum through all member ref's looking for ref's to internal things.
        for (iMR = 1; iMR<=cMemberRefRecs; iMR++)
        {   // Get a MemberRef.
            pMemberRefRec = pMiniMd->getMemberRef(iMR);

            // If not member of the TypeRef, skip it.
            tkParent = pMiniMd->getClassOfMemberRef(pMemberRefRec);

            if ( TypeFromToken(tkParent) == mdtMethodDef )
            {
                // always track the map even though it is already in the original scope
                *(pMiniMd->GetMemberRefToMemberDefMap()->Get(iMR)) =  tkParent;
                continue;
            }

            if ( TypeFromToken(tkParent) != mdtTypeRef && TypeFromToken(tkParent) != mdtTypeDef )
            {
                // this has been either optimized to mdtMethodDef, mdtFieldDef or referring to
                // ModuleRef
                continue;
            }

            // In the case of global function, we have tkParent as m_tdModule. 
            // We will always do the optmization.
            if (TypeFromToken(tkParent) == mdtTypeRef)
            {
                // The parent is a TypeRef. We need to check to see if this TypeRef is optimized to a TypeDef
                tkParent = *(pMiniMd->GetTypeRefToTypeDefMap()->Get(RidFromToken(tkParent)) );
                // tkParent = pMapTypeRefToTypeDef[RidFromToken(tkParent)];
                if ( RidFromToken(tkParent) == 0)
                {
                    continue;
                }
            }


            // Get the name and signature of this mr.
            szName = pMiniMd->getNameOfMemberRef(pMemberRefRec);
            pvSig = pMiniMd->getSignatureOfMemberRef(pMemberRefRec, &cbSig);
            
            // Look for a member with the same def.  Might not be found if it is
            // inherited from a base class.
            hr = ImportHelper::FindMember(pMiniMd, tkParent, szName, pvSig, cbSig, &mfdef);
            if (hr != S_OK)
            {
    #if _TRACE_REMAPS
            // Log the failure.
            LOG((LF_METADATA, LL_INFO10, "Member %S//%S.%S not found\n", szNamespace, szTDName, rcMRName));
    #endif
                continue;
            }

            // We will only record this if mfdef is a methoddef. We don't support
            // parent of MemberRef as fielddef. As if we can optimize MemberRef to FieldDef,
            // we can remove this row.
            //
            if ( (TypeFromToken(mfdef) == mdtMethodDef) &&
                  (m_bRemap || tkParent == m_tdModule ) )
            {
                // Always change the parent if it is the global function.
                // Or change the parent if we have a remap that we can send notification.
                //
                pMiniMd->PutToken(TBL_MemberRef, MemberRefRec::COL_Class, pMemberRefRec, mfdef);
            }
            
            // We will always track the changes. In MiniMd::PreSaveFull, we will use this map to send
            // notification to our host if there is any IMapToken provided.
            //
            *(pMiniMd->GetMemberRefToMemberDefMap()->Get(iMR)) =  mfdef;

        } // EnumMemberRefs
    }

    // Reset return code from likely search failures.
    hr = S_OK;

    SetMemberDefDirty(false);
    SetTypeDefDirty(false);
    m_hasOptimizedRefToDef = true;
ErrExit:
    STOP_MD_PERF(RefToDefOptimization);
   
    return hr;
}

//*****************************************************************************
// Process filter
//*****************************************************************************
HRESULT RegMeta::ProcessFilter()
{
    HRESULT         hr = NULL;
    CMiniMdRW       *pMiniMd;               // The MiniMd with the data.
    RegMeta         *pMetaNew = NULL;
    CMapToken       *pMergeMap = NULL;
    IMapToken       *pMapNew = NULL;
    MergeTokenManager *pCompositHandler = NULL;
    CLiteWeightStgdbRW  *pStgdbTmp;
    IMapToken       *pHostMapToken = NULL;

    

    START_MD_PERF();

    // For convenience.
    pMiniMd = &(m_pStgdb->m_MiniMd);
    IfNullGo( pMiniMd->GetFilterTable() );
    if ( pMiniMd->GetFilterTable()->Count() == 0 )
    {
        // there is no filter
        goto ErrExit;
    }

    // Yes, client has used filter to specify what are the metadata needed.
    // We will create another instance of RegMeta and make this module an imported module
    // to be merged into the new RegMeta. We will provide the handler to track all of the token
    // movements. We will replace the merged light weight stgdb to this RegMeta..
    // Then we will need to fix up the MergeTokenManager with this new movement.
    // The reason that we decide to choose this approach is because it will be more complicated
    // and very likely less efficient to fix up the signature blob pool and then compact all of the pools!
    //

    // Create a new RegMeta.
    pMetaNew = new (nothrow) RegMeta();
    IfNullGo( pMetaNew );
    pMetaNew->AddRef();
    IfFailGo(pMetaNew->SetOption(&m_OptionValue));


    // Remember the open type.
    IfFailGo(pMetaNew->CreateNewMD());
    IfFailGo(pMetaNew->AddToCache());

    // Ignore the error return by setting handler
    hr = pMetaNew->SetHandler(m_pHandler);

    // create the IMapToken to receive token remap information from merge
    pMergeMap = new (nothrow) CMapToken;
    IfNullGo( pMergeMap );

    // use merge to filter out the unneeded data. But we need to keep COMType and also need to drop off the 
    // CustomAttributes that associated with MemberRef with parent MethodDef
    //
    pMetaNew->m_hasOptimizedRefToDef = false;
    IfFailGo( pMetaNew->m_newMerger.AddImport(this, pMergeMap, NULL) );
    IfFailGo( pMetaNew->m_pStgdb->m_MiniMd.ExpandTables());
    IfFailGo( pMetaNew->m_newMerger.Merge((MergeFlags)(MergeManifest | DropMemberRefCAs | NoDupCheck), MDRefToDefDefault) );

    // Now we need to recalculate the token movement
    // 
    if (m_newMerger.m_pImportDataList)
    {

        // This is the case the filter is applied to merged emit scope. We need calculate how this implicit merge
        // affects the original merge remap. Basically we need to walk all the m_pTkMapList in the merger and replace
        // the to token to the most recent to token.
        // 
        MDTOKENMAP          *pMDTokenMapList;

        pMDTokenMapList = m_newMerger.m_pImportDataList->m_pMDTokenMap;

        MDTOKENMAP          *pMap;
        TOKENREC            *pTKRec;
        ULONG               i;
        mdToken             tkFinalTo;
        ModuleRec           *pMod;
        ModuleRec           *pModNew;
        LPCUTF8             pName;

        // update each import map from merge to have the m_tkTo points to the final mapped to token
        for (pMap = pMDTokenMapList; pMap; pMap = pMap->m_pNextMap)
        {
            // update each record
            for (i = 0; i < (ULONG) (pMap->Count()); i++)
            {
                TOKENREC    *pRecTo;
                pTKRec = pMap->Get(i);
                if ( pMergeMap->Find( pTKRec->m_tkTo, &pRecTo ) )
                {
                    // This record is kept by the filter and the tkTo is changed
                    pRecTo->m_isFoundInImport = true;
                    tkFinalTo = pRecTo->m_tkTo;
                    pTKRec->m_tkTo = tkFinalTo;
                    pTKRec->m_isDeleted = false;

                    // send the notification now. Because after merge, we may have everything in order and 
                    // won't send another set of notification.
                    //
                    LOG((LOGMD, "TokenRemap in RegMeta::ProcessFilter (IMapToken 0x%08x): from 0x%08x to 0x%08x\n", pMap->m_pMap, pTKRec->m_tkFrom, pTKRec->m_tkTo));

                    pMap->m_pMap->Map(pTKRec->m_tkFrom, pTKRec->m_tkTo);
                }
                else
                {
                    // This record is pruned by the filter upon save
                    pTKRec->m_isDeleted = true;
                }
            }
        }

        // now walk the pMergeMap and check to see if there is any entry that is not set to true for m_isFoundInImport.
        // These are the records that from calling DefineXXX methods directly on the Emitting scope!
        if (m_pHandler)
            m_pHandler->QueryInterface(IID_IMapToken, (void **)&pHostMapToken);
        if (pHostMapToken)
        {
            for (i = 0; i < (ULONG) (pMergeMap->m_pTKMap->Count()); i++)
            {
                pTKRec = pMergeMap->m_pTKMap->Get(i);
                if (pTKRec->m_isFoundInImport == false)
                {
                    LOG((LOGMD, "TokenRemap in RegMeta::ProcessFilter (default IMapToken 0x%08x): from 0x%08x to 0x%08x\n", pHostMapToken, pTKRec->m_tkFrom, pTKRec->m_tkTo));

                    // send the notification on the IMapToken from SetHandler of this RegMeta
                    pHostMapToken->Map(pTKRec->m_tkFrom, pTKRec->m_tkTo);
                }
            }
        }

        // Preserve module name across merge.
        pMod = m_pStgdb->m_MiniMd.getModule(1);
        pModNew = pMetaNew->m_pStgdb->m_MiniMd.getModule(1);
        pName = m_pStgdb->m_MiniMd.getNameOfModule(pMod);
        IfFailGo(pMetaNew->m_pStgdb->m_MiniMd.PutString(TBL_Module, ModuleRec::COL_Name, pModNew, pName));

        // now swap the stgdb but keep the merger...
        _ASSERTE( !IsOfExternalStgDB(m_OpenFlags) );
        
        pStgdbTmp = m_pStgdb;
        m_pStgdb = pMetaNew->m_pStgdb;
        pMetaNew->m_pStgdb = pStgdbTmp;
        
    }
    else
    {

        // swap the Stgdb
        pStgdbTmp = m_pStgdb;
        m_pStgdb = pMetaNew->m_pStgdb;
        pMetaNew->m_pStgdb = pStgdbTmp;

        // Client either open an existing scope and apply the filter mechanism, or client define the scope and then
        // apply the filter mechanism.

        // In this case, host better has supplied the handler!!
        _ASSERTE( m_bRemap && m_pHandler);
        IfFailGo( m_pHandler->QueryInterface(IID_IMapToken, (void **) &pMapNew) );

        
        {
            // Send the notification of token movement now because after merge we may not move tokens again
            // and thus no token notification will be send.
            MDTOKENMAP      *pMap = pMergeMap->m_pTKMap;
            TOKENREC        *pTKRec;
            ULONG           i;

            for (i=0; i < (ULONG) (pMap->Count()); i++)
            {
                pTKRec = pMap->Get(i);
                pMap->m_pMap->Map(pTKRec->m_tkFrom, pTKRec->m_tkTo);
            }

        }


        // What we need to do here is create a IMapToken that will replace the original handler. This new IMapToken 
        // upon called will first map the from token to the most original from token.
        //
        pCompositHandler = new (nothrow) MergeTokenManager(pMergeMap->m_pTKMap, NULL);
        IfNullGo( pCompositHandler );

        // now update the following field to hold on to the real IMapToken supplied by our client by SetHandler
        if (pMergeMap->m_pTKMap->m_pMap)
            pMergeMap->m_pTKMap->m_pMap->Release();
        _ASSERTE(pMapNew);
        pMergeMap->m_pTKMap->m_pMap = pMapNew;

        // ownership transferred
        pMergeMap = NULL;
        pMapNew = NULL;
    
        // now you want to replace all of the IMapToken set by calling SetHandler to this new MergeTokenManager
        IfFailGo( m_pStgdb->m_MiniMd.SetHandler(pCompositHandler) );

        m_pHandler = pCompositHandler;

        // ownership transferred
        pCompositHandler = NULL;
    }

    // Force a ref to def optimization because the remap information was stored in the thrown away CMiniMdRW
    m_hasOptimizedRefToDef = false;
    IfFailGo( RefToDefOptimization() );

ErrExit:
    if (pHostMapToken)
        pHostMapToken->Release();
    if (pMetaNew) 
        pMetaNew->Release();
    if (pMergeMap)
        pMergeMap->Release();
    if (pCompositHandler)
        pCompositHandler->Release();
    if (pMapNew)
        pMapNew->Release();
    STOP_MD_PERF(ProcessFilter);
    
    return hr;
} // HRESULT RegMeta::ProcessFilter()

//*****************************************************************************
// Define a TypeRef given the fully qualified name.
//*****************************************************************************
HRESULT RegMeta::_DefineTypeRef(
    mdToken     tkResolutionScope,          // [IN] ModuleRef or AssemblyRef.
    const void  *szName,                    // [IN] Name of the TypeRef.
    BOOL        isUnicode,                  // [IN] Specifies whether the URL is unicode.
    mdTypeRef   *ptk,                       // [OUT] Put mdTypeRef here.
    eCheckDups  eCheck)                     // [IN] Specifies whether to check for duplicates.
{
    HRESULT     hr = S_OK;
    LPUTF8      szUTF8FullQualName;
    CQuickBytes qbNamespace;
    CQuickBytes qbName;
    int         bSuccess;
    ULONG       ulStringLen;


    

    _ASSERTE(ptk && szName);
    _ASSERTE (TypeFromToken(tkResolutionScope) == mdtModule ||
              TypeFromToken(tkResolutionScope) == mdtModuleRef ||
              TypeFromToken(tkResolutionScope) == mdtAssemblyRef ||
              TypeFromToken(tkResolutionScope) == mdtTypeRef ||
              tkResolutionScope == mdTokenNil);

    if (isUnicode)
    {
        UTF8STR((LPCWSTR)szName, szUTF8FullQualName);
    }
    else
    {
        szUTF8FullQualName = (LPUTF8)szName;
    }
    PREFIX_ASSUME(szUTF8FullQualName != NULL);

    ulStringLen = (ULONG)(strlen(szUTF8FullQualName) + 1);
    IfFailGo(qbNamespace.ReSizeNoThrow(ulStringLen));
    IfFailGo(qbName.ReSizeNoThrow(ulStringLen));
    bSuccess = ns::SplitPath(szUTF8FullQualName,
                             (LPUTF8)qbNamespace.Ptr(),
                             ulStringLen,
                             (LPUTF8)qbName.Ptr(),
                             ulStringLen);
    _ASSERTE(bSuccess);

    // Search for existing TypeRef record.
    if (eCheck==eCheckYes || (eCheck==eCheckDefault && CheckDups(MDDupTypeRef)))
    {
        hr = ImportHelper::FindTypeRefByName(&(m_pStgdb->m_MiniMd), tkResolutionScope,
                                             (LPCUTF8)qbNamespace.Ptr(),
                                             (LPCUTF8)qbName.Ptr(), ptk);
        if (SUCCEEDED(hr))
        {
            if (IsENCOn())
            {
                hr = S_OK;
                goto NormalExit;
            }
            else
            {
                hr = META_S_DUPLICATE;
                goto NormalExit;
            }
        }
        else if (hr != CLDB_E_RECORD_NOTFOUND)
            IfFailGo(hr);
    }

    // Create TypeRef record.
    TypeRefRec      *pRecord;
    RID             iRecord;

    IfNullGo(pRecord = m_pStgdb->m_MiniMd.AddTypeRefRecord(&iRecord));

    // record the more defs are introduced.
    SetTypeDefDirty(true);

    // Give token back to caller.
    *ptk = TokenFromRid(iRecord, mdtTypeRef);

    // Set the fields of the TypeRef record.
    IfFailGo(m_pStgdb->m_MiniMd.PutString(TBL_TypeRef, TypeRefRec::COL_Namespace,
                        pRecord, (LPUTF8)qbNamespace.Ptr()));

    IfFailGo(m_pStgdb->m_MiniMd.PutString(TBL_TypeRef, TypeRefRec::COL_Name,
                        pRecord, (LPUTF8)qbName.Ptr()));

    if (!IsNilToken(tkResolutionScope))
        IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_TypeRef, TypeRefRec::COL_ResolutionScope,
                        pRecord, tkResolutionScope));
    IfFailGo(UpdateENCLog(*ptk));

    // Hash the name.
    IfFailGo(m_pStgdb->m_MiniMd.AddNamedItemToHash(TBL_TypeRef, *ptk, (LPUTF8)qbName.Ptr(), 0));

ErrExit:
    ;
NormalExit:
    
    return hr;
} // HRESULT RegMeta::_DefineTypeRef()

//*******************************************************************************
// Find a given param of a Method.
//*******************************************************************************
HRESULT RegMeta::_FindParamOfMethod(    // S_OK or error.
    mdMethodDef md,                     // [IN] The owning method of the param.
    ULONG       iSeq,                   // [IN] The sequence # of the param.
    mdParamDef  *pParamDef)             // [OUT] Put ParamDef token here.
{
    ParamRec    *pParamRec;
    RID         ridStart, ridEnd;
    RID         pmRid;

    _ASSERTE(TypeFromToken(md) == mdtMethodDef && pParamDef);

    // get the methoddef record
    MethodRec *pMethodRec = m_pStgdb->m_MiniMd.getMethod(RidFromToken(md));

    // figure out the start rid and end rid of the parameter list of this methoddef
    ridStart = m_pStgdb->m_MiniMd.getParamListOfMethod(pMethodRec);
    ridEnd = m_pStgdb->m_MiniMd.getEndParamListOfMethod(pMethodRec);

    // loop through each param
    for (; ridStart < ridEnd; ridStart++)
    {
        pmRid = m_pStgdb->m_MiniMd.GetParamRid(ridStart);
        pParamRec = m_pStgdb->m_MiniMd.getParam(pmRid);
        if (iSeq == m_pStgdb->m_MiniMd.getSequenceOfParam(pParamRec))
        {
            // parameter has the sequence number matches what we are looking for
            *pParamDef = TokenFromRid(pmRid, mdtParamDef);
            return S_OK;
        }
    }
    return CLDB_E_RECORD_NOTFOUND;
} // HRESULT RegMeta::_FindParamOfMethod()

//*******************************************************************************
// Define MethodSemantics
//*******************************************************************************
HRESULT RegMeta::_DefineMethodSemantics(    // S_OK or error.
    USHORT      usAttr,                     // [IN] CorMethodSemanticsAttr.
    mdMethodDef md,                         // [IN] Method.
    mdToken     tkAssoc,                    // [IN] Association.
    BOOL        bClear)                     // [IN] Specifies whether to delete the exisiting entries.
{
    HRESULT      hr = S_OK;
    MethodSemanticsRec *pRecord = 0;
    MethodSemanticsRec *pRecord1;           // Use this to recycle a MethodSemantics record.
    RID         iRecord;
    HENUMInternal hEnum;

    

    _ASSERTE(TypeFromToken(md) == mdtMethodDef || IsNilToken(md));
    _ASSERTE(RidFromToken(tkAssoc));
    memset(&hEnum, 0, sizeof(HENUMInternal));

    // Clear all matching records by setting association to a Nil token.
    if (bClear)
    {
        RID         i;

        IfFailGo( m_pStgdb->m_MiniMd.FindMethodSemanticsHelper(tkAssoc, &hEnum) );
        while (HENUMInternal::EnumNext(&hEnum, (mdToken *)&i))
        {
            pRecord1 = m_pStgdb->m_MiniMd.getMethodSemantics(i);
            if (usAttr == pRecord1->GetSemantic())
            {
                pRecord = pRecord1;
                iRecord = i;
                IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_MethodSemantics,
                    MethodSemanticsRec::COL_Association, pRecord, mdPropertyNil));
            }
        }
    }
    // If setting (not just clearing) the association, do that now.
    if (!IsNilToken(md))
    {
        // Create a new record required
        if (! pRecord)
            IfNullGo(pRecord=m_pStgdb->m_MiniMd.AddMethodSemanticsRecord(&iRecord));
    
        // Save the data.
        pRecord->SetSemantic(usAttr);
        IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_MethodSemantics,
                                             MethodSemanticsRec::COL_Method, pRecord, md));
        IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_MethodSemantics,
                                             MethodSemanticsRec::COL_Association, pRecord, tkAssoc));
    
        // regardless if we reuse the record or create the record, add the MethodSemantics to the hash
        IfFailGo( m_pStgdb->m_MiniMd.AddMethodSemanticsToHash(iRecord) );
    
        // Create log record for non-token table.
        IfFailGo(UpdateENCLog2(TBL_MethodSemantics, iRecord));
    }

ErrExit:
    HENUMInternal::ClearEnum(&hEnum);
    
    return hr;
} // HRESULT RegMeta::_DefineMethodSemantics()

//*******************************************************************************
// Given the signature, return the token for signature.
//*******************************************************************************
HRESULT RegMeta::_GetTokenFromSig(              // S_OK or error.
    PCCOR_SIGNATURE pvSig,              // [IN] Signature to define.
    ULONG       cbSig,                  // [IN] Size of signature data.
    mdSignature *pmsig)                 // [OUT] returned signature token.
{
    HRESULT     hr = S_OK;

    _ASSERTE(pmsig);

    if (CheckDups(MDDupSignature))
    {
        hr = ImportHelper::FindStandAloneSig(&(m_pStgdb->m_MiniMd), pvSig, cbSig, pmsig);
        if (SUCCEEDED(hr))
        {
            if (IsENCOn())
                return S_OK;
            else
                return META_S_DUPLICATE;
        }
        else if (hr != CLDB_E_RECORD_NOTFOUND)
            IfFailGo(hr);
    }

    // Create a new record.
    StandAloneSigRec *pSigRec;
    RID     iSigRec;

    IfNullGo(pSigRec = m_pStgdb->m_MiniMd.AddStandAloneSigRecord(&iSigRec));

    // Set output parameter.
    *pmsig = TokenFromRid(iSigRec, mdtSignature);

    // Save signature.
    IfFailGo(m_pStgdb->m_MiniMd.PutBlob(TBL_StandAloneSig, StandAloneSigRec::COL_Signature,
                                pSigRec, pvSig, cbSig));
    IfFailGo(UpdateENCLog(*pmsig));
ErrExit:
    return hr;
} // HRESULT RegMeta::_GetTokenFromSig()

//*******************************************************************************
// Turn the specified internal flags on.
//*******************************************************************************
HRESULT RegMeta::_TurnInternalFlagsOn(  // S_OK or error.
    mdToken     tkObj,                  // [IN] Target object whose internal flags are targetted.
    DWORD       flags)                  // [IN] Specifies flags to be turned on.
{
    MethodRec   *pMethodRec;
    FieldRec    *pFieldRec;
    TypeDefRec  *pTypeDefRec;

    switch (TypeFromToken(tkObj))
    {
    case mdtMethodDef:
        pMethodRec = m_pStgdb->m_MiniMd.getMethod(RidFromToken(tkObj));
        pMethodRec->AddFlags(flags);
        break;
    case mdtFieldDef:
        pFieldRec = m_pStgdb->m_MiniMd.getField(RidFromToken(tkObj));
        pFieldRec->AddFlags(flags);
        break;
    case mdtTypeDef:
        pTypeDefRec = m_pStgdb->m_MiniMd.getTypeDef(RidFromToken(tkObj));
        pTypeDefRec->AddFlags(flags);
        break;
    default:
        _ASSERTE(!"Not supported token type!");
        return E_INVALIDARG;
    }
    return S_OK;
} // HRESULT    RegMeta::_TurnInternalFlagsOn()


//*****************************************************************************
// Helper: Set the properties on the given TypeDef token.
//*****************************************************************************
HRESULT RegMeta::_SetTypeDefProps(      // S_OK or error.
    mdTypeDef   td,                     // [IN] The TypeDef.
    DWORD       dwTypeDefFlags,         // [IN] TypeDef flags.
    mdToken     tkExtends,              // [IN] Base TypeDef or TypeRef.
    mdToken     rtkImplements[])        // [IN] Implemented interfaces.
{
    HRESULT     hr = S_OK;              // A result.
    BOOL        bClear = IsENCOn() || IsCallerExternal();   // Specifies whether to clear the InterfaceImpl records.
    TypeDefRec  *pRecord;               // New TypeDef record.

    _ASSERTE(TypeFromToken(td) == mdtTypeDef);
    _ASSERTE(TypeFromToken(tkExtends) == mdtTypeDef || TypeFromToken(tkExtends) == mdtTypeRef || TypeFromToken(tkExtends) == mdtTypeSpec ||
                IsNilToken(tkExtends) || tkExtends == ULONG_MAX);

    // Get the record.
    pRecord=m_pStgdb->m_MiniMd.getTypeDef(RidFromToken(td));

    if (dwTypeDefFlags != ULONG_MAX)
    {
        // No one should try to set the reserved flags explicitly.
        _ASSERTE((dwTypeDefFlags & (tdReservedMask&~tdRTSpecialName)) == 0);
        // Clear the reserved flags from the flags passed in.
        dwTypeDefFlags &= (~tdReservedMask);
        // Preserve the reserved flags stored.
        dwTypeDefFlags |= (pRecord->GetFlags() & tdReservedMask);
        // Set the flags.
        pRecord->SetFlags(dwTypeDefFlags);
    }
    if (tkExtends != ULONG_MAX)
    {
        if (IsNilToken(tkExtends))
            tkExtends = mdTypeDefNil;
        IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_TypeDef, TypeDefRec::COL_Extends,
                                             pRecord, tkExtends));
    }

    // Implemented interfaces.
    if (rtkImplements)
        IfFailGo(_SetImplements(rtkImplements, td, bClear));

    IfFailGo(UpdateENCLog(td));
ErrExit:
    return hr;
} // HRESULT RegMeta::_SetTypeDefProps()


//******************************************************************************
// Creates and sets a row in the InterfaceImpl table.  Optionally clear
// pre-existing records for the owning class.
//******************************************************************************
HRESULT RegMeta::_SetImplements(        // S_OK or error.
    mdToken     rTk[],                  // Array of TypeRef or TypeDef or TypeSpec tokens for implemented interfaces.
    mdTypeDef   td,                     // Implementing TypeDef.
    BOOL        bClear)                 // Specifies whether to clear the existing records.
{
    HRESULT     hr = S_OK;
    ULONG       i = 0;
    ULONG       j;
    InterfaceImplRec *pInterfaceImpl;
    RID         iInterfaceImpl;
    RID         ridStart;
    RID         ridEnd;
    CQuickBytes cqbTk;
    const mdToken *pTk;

    

    _ASSERTE(TypeFromToken(td) == mdtTypeDef && rTk);
    _ASSERTE(!m_bSaveOptimized && "Cannot change records after PreSave() and before Save().");

    // Clear all exising InterfaceImpl records by setting the parent to Nil.
    if (bClear)
    {
        IfFailGo(m_pStgdb->m_MiniMd.GetInterfaceImplsForTypeDef(
                                        RidFromToken(td), &ridStart, &ridEnd));
        for (j = ridStart; j < ridEnd; j++)
        {
            pInterfaceImpl = m_pStgdb->m_MiniMd.getInterfaceImpl(
                                        m_pStgdb->m_MiniMd.GetInterfaceImplRid(j));
            _ASSERTE (td == m_pStgdb->m_MiniMd.getClassOfInterfaceImpl(pInterfaceImpl));
            IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_InterfaceImpl, InterfaceImplRec::COL_Class,
                                                 pInterfaceImpl, mdTypeDefNil));
        }
    }

    // Eliminate duplicates from the array passed in.
    if (CheckDups(MDDupInterfaceImpl))
    {
        IfFailGo(_InterfaceImplDupProc(rTk, td, &cqbTk));
        pTk = (mdToken *)cqbTk.Ptr();
    }
    else
        pTk = rTk;

    // Loop for each implemented interface.
    while (!IsNilToken(pTk[i]))
    {
        _ASSERTE(TypeFromToken(pTk[i]) == mdtTypeRef || TypeFromToken(pTk[i]) == mdtTypeDef
               || TypeFromToken(pTk[i]) == mdtTypeSpec);

        // Create the interface implementation record.
        IfNullGo(pInterfaceImpl = m_pStgdb->m_MiniMd.AddInterfaceImplRecord(&iInterfaceImpl));

        // Set data.
        IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_InterfaceImpl, InterfaceImplRec::COL_Class,
                                            pInterfaceImpl, td));
        IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_InterfaceImpl, InterfaceImplRec::COL_Interface,
                                            pInterfaceImpl, pTk[i]));

        i++;

        IfFailGo(UpdateENCLog(TokenFromRid(mdtInterfaceImpl, iInterfaceImpl)));
    }
ErrExit:
    
    return hr;
} // HRESULT RegMeta::_SetImplements()

//******************************************************************************
// This routine eliminates duplicates from the given list of InterfaceImpl tokens
// to be defined.  It checks for duplicates against the database only if the
// TypeDef for which these tokens are being defined is not a new one.
//******************************************************************************
HRESULT RegMeta::_InterfaceImplDupProc( // S_OK or error.
    mdToken     rTk[],                  // Array of TypeRef or TypeDef or TypeSpec tokens for implemented interfaces.
    mdTypeDef   td,                     // Implementing TypeDef.
    CQuickBytes *pcqbTk)                // Quick Byte object for placing the array of unique tokens.
{
    HRESULT     hr = S_OK;
    ULONG       i = 0;
    ULONG       iUniqCount = 0;
    BOOL        bDupFound;

    

    while (!IsNilToken(rTk[i]))
    {
        _ASSERTE(TypeFromToken(rTk[i]) == mdtTypeRef || TypeFromToken(rTk[i]) == mdtTypeDef
              || TypeFromToken(rTk[i]) == mdtTypeSpec);
        bDupFound = false;

        // Eliminate duplicates from the input list of tokens by looking within the list.
        for (ULONG j = 0; j < iUniqCount; j++)
        {
            if (rTk[i] == ((mdToken *)pcqbTk->Ptr())[j])
            {
                bDupFound = true;
                break;
            }
        }

        // If no duplicate is found record it in the list.
        if (!bDupFound)
        {
            IfFailGo(pcqbTk->ReSizeNoThrow((iUniqCount+1) * sizeof(mdToken)));
            ((mdToken *)pcqbTk->Ptr())[iUniqCount] = rTk[i];
            iUniqCount++;
        }
        i++;
    }

    // Create a Nil token to signify the end of list.
    IfFailGo(pcqbTk->ReSizeNoThrow((iUniqCount+1) * sizeof(mdToken)));
    ((mdToken *)pcqbTk->Ptr())[iUniqCount] = mdTokenNil;
ErrExit:
    
    return hr;
} // HRESULT RegMeta::_InterfaceImplDupProc()

//*******************************************************************************
// helper to define event
//*******************************************************************************
HRESULT RegMeta::_DefineEvent(          // Return hresult.
    mdTypeDef   td,                     // [IN] the class/interface on which the event is being defined 
    LPCWSTR     szEvent,                // [IN] Name of the event   
    DWORD       dwEventFlags,           // [IN] CorEventAttr    
    mdToken     tkEventType,            // [IN] a reference (mdTypeRef or mdTypeRef) to the Event class 
    mdEvent     *pmdEvent)              // [OUT] output event token 
{
    HRESULT     hr = S_OK;
    EventRec    *pEventRec = NULL;
    RID         iEventRec;
    EventMapRec *pEventMap;
    RID         iEventMap;
    mdEvent     mdEv;
    LPUTF8      szUTF8Event;
    UTF8STR(szEvent, szUTF8Event);
    PREFIX_ASSUME(szUTF8Event != NULL);

    

    _ASSERTE(TypeFromToken(td) == mdtTypeDef && td != mdTypeDefNil);
    _ASSERTE(IsNilToken(tkEventType) || TypeFromToken(tkEventType) == mdtTypeDef ||
                TypeFromToken(tkEventType) == mdtTypeRef || TypeFromToken(tkEventType) == mdtTypeSpec);
    _ASSERTE(szEvent && pmdEvent);

    if (CheckDups(MDDupEvent))
    {
        hr = ImportHelper::FindEvent(&(m_pStgdb->m_MiniMd), td, szUTF8Event, pmdEvent);
        if (SUCCEEDED(hr))
        {
            if (IsENCOn())
                pEventRec = m_pStgdb->m_MiniMd.getEvent(RidFromToken(*pmdEvent));
            else
            {
                hr = META_S_DUPLICATE;
                goto ErrExit;
            }
        }
        else if (hr != CLDB_E_RECORD_NOTFOUND)
            IfFailGo(hr);
    }

    if (! pEventRec)
    {
        // Create a new map if one doesn't exist already, else retrieve the existing one.
        // The event map must be created before the EventRecord, the new event map will
        // be pointing past the first event record.
        iEventMap = m_pStgdb->m_MiniMd.FindEventMapFor(RidFromToken(td));
        if (InvalidRid(iEventMap))
        {
            // Create new record.
            IfNullGo(pEventMap=m_pStgdb->m_MiniMd.AddEventMapRecord(&iEventMap));
            // Set parent.
            IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_EventMap, 
                                            EventMapRec::COL_Parent, pEventMap, td));
            IfFailGo(UpdateENCLog2(TBL_EventMap, iEventMap));
        }
        else
        {
            pEventMap = m_pStgdb->m_MiniMd.getEventMap(iEventMap);
        }

        // Create a new event record.
        IfNullGo(pEventRec = m_pStgdb->m_MiniMd.AddEventRecord(&iEventRec));

        // Set output parameter.
        *pmdEvent = TokenFromRid(iEventRec, mdtEvent);

        // Add Event to EventMap.
        IfFailGo(m_pStgdb->m_MiniMd.AddEventToEventMap(RidFromToken(iEventMap), iEventRec));
    
        IfFailGo(UpdateENCLog2(TBL_EventMap, iEventMap, CMiniMdRW::eDeltaEventCreate));     
    }

    mdEv = *pmdEvent;

    // Set data
    IfFailGo(m_pStgdb->m_MiniMd.PutString(TBL_Event, EventRec::COL_Name, pEventRec, szUTF8Event));
    IfFailGo(_SetEventProps1(*pmdEvent, dwEventFlags, tkEventType));

    // Add the <Event token, typedef token> to the lookup table
    if (m_pStgdb->m_MiniMd.HasIndirectTable(TBL_Event))
        IfFailGo( m_pStgdb->m_MiniMd.AddEventToLookUpTable(*pmdEvent, td) );

    IfFailGo(UpdateENCLog(*pmdEvent));

ErrExit:
    
    return hr;
} // HRESULT RegMeta::_DefineEvent()


//******************************************************************************
// Set the specified properties on the Event Token.
//******************************************************************************
HRESULT RegMeta::_SetEventProps1(                // Return hresult.
    mdEvent     ev,                     // [IN] Event token.
    DWORD       dwEventFlags,           // [IN] Event flags.
    mdToken     tkEventType)            // [IN] Event type class.
{
    EventRec    *pRecord;
    HRESULT     hr = S_OK;

    _ASSERTE(TypeFromToken(ev) == mdtEvent && RidFromToken(ev));

    pRecord = m_pStgdb->m_MiniMd.getEvent(RidFromToken(ev));
    if (dwEventFlags != ULONG_MAX)
    {
        // Don't let caller set reserved bits
        dwEventFlags &= ~evReservedMask;
        // Preserve reserved bits.
        dwEventFlags |= (pRecord->GetEventFlags() & evReservedMask);
        
        pRecord->SetEventFlags(static_cast<USHORT>(dwEventFlags));
    }
    if (!IsNilToken(tkEventType))
        IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_Event, EventRec::COL_EventType,
                                             pRecord, tkEventType));
ErrExit:
    return hr;
} // HRESULT RegMeta::_SetEventProps1()

//******************************************************************************
// Set the specified properties on the given Event token.
//******************************************************************************
HRESULT RegMeta::_SetEventProps2(                // Return hresult.
    mdEvent     ev,                     // [IN] Event token.
    mdMethodDef mdAddOn,                // [IN] Add method.
    mdMethodDef mdRemoveOn,             // [IN] Remove method.
    mdMethodDef mdFire,                 // [IN] Fire method.
    mdMethodDef rmdOtherMethods[],      // [IN] An array of other methods.
    BOOL        bClear)                 // [IN] Specifies whether to clear the existing MethodSemantics records.
{
    EventRec    *pRecord;
    HRESULT     hr = S_OK;

    

    _ASSERTE(TypeFromToken(ev) == mdtEvent && RidFromToken(ev));

    pRecord = m_pStgdb->m_MiniMd.getEvent(RidFromToken(ev));

    // Remember the AddOn method.
    if (!IsNilToken(mdAddOn))
    {
        _ASSERTE(TypeFromToken(mdAddOn) == mdtMethodDef);
        IfFailGo(_DefineMethodSemantics(msAddOn, mdAddOn, ev, bClear));
    }

    // Remember the RemoveOn method.
    if (!IsNilToken(mdRemoveOn))
    {
        _ASSERTE(TypeFromToken(mdRemoveOn) == mdtMethodDef);
        IfFailGo(_DefineMethodSemantics(msRemoveOn, mdRemoveOn, ev, bClear));
    }

    // Remember the fire method.
    if (!IsNilToken(mdFire))
    {
        _ASSERTE(TypeFromToken(mdFire) == mdtMethodDef);
        IfFailGo(_DefineMethodSemantics(msFire, mdFire, ev, bClear));
    }

    // Store all of the other methods.
    if (rmdOtherMethods)
    {
        int         i = 0;
        mdMethodDef mb;

        while (1)
        {
            mb = rmdOtherMethods[i++];
            if (IsNilToken(mb))
                break;
            _ASSERTE(TypeFromToken(mb) == mdtMethodDef);
            IfFailGo(_DefineMethodSemantics(msOther, mb, ev, bClear));

            // The first call would've cleared all the existing ones.
            bClear = false;
        }
    }
ErrExit:
    
    return hr;
} // HRESULT RegMeta::_SetEventProps2()

//******************************************************************************
// Set Permission on the given permission token.
//******************************************************************************
HRESULT RegMeta::_SetPermissionSetProps(         // Return hresult.
    mdPermission tkPerm,                // [IN] Permission token.
    DWORD       dwAction,               // [IN] CorDeclSecurity.
    void const  *pvPermission,          // [IN] Permission blob.
    ULONG       cbPermission)           // [IN] Count of bytes of pvPermission.
{
    DeclSecurityRec *pRecord;
    HRESULT     hr = S_OK;

    _ASSERTE(TypeFromToken(tkPerm) == mdtPermission && cbPermission != ULONG_MAX);
    _ASSERTE(dwAction && dwAction <= dclMaximumValue);

    pRecord = m_pStgdb->m_MiniMd.getDeclSecurity(RidFromToken(tkPerm));

    IfFailGo(m_pStgdb->m_MiniMd.PutBlob(TBL_DeclSecurity, DeclSecurityRec::COL_PermissionSet,
                                        pRecord, pvPermission, cbPermission));
ErrExit:
    return hr;
} // HRESULT RegMeta::_SetPermissionSetProps()

//******************************************************************************
// Define or set value on a constant record.
//******************************************************************************
HRESULT RegMeta::_DefineSetConstant(    // Return hresult.
    mdToken     tk,                     // [IN] Parent token.
    DWORD       dwCPlusTypeFlag,        // [IN] Flag for the value type, selected ELEMENT_TYPE_*
    void const  *pValue,                // [IN] Constant value.
    ULONG       cchString,              // [IN] Size of string in wide chars, or -1 for default.
    BOOL        bSearch)                // [IN] Specifies whether to search for an existing record.
{
    HRESULT     hr = S_OK;

    

    if ((dwCPlusTypeFlag != ELEMENT_TYPE_VOID && dwCPlusTypeFlag != ELEMENT_TYPE_END &&
         dwCPlusTypeFlag != ULONG_MAX) &&
        (pValue || (pValue == 0 && (dwCPlusTypeFlag == ELEMENT_TYPE_STRING ||
                                    dwCPlusTypeFlag == ELEMENT_TYPE_CLASS))))
    {
        ConstantRec *pConstRec = 0;
        RID         iConstRec;
        ULONG       cbBlob;
        ULONG       ulValue = 0;

        if (bSearch)
        {
            iConstRec = m_pStgdb->m_MiniMd.FindConstantHelper(tk);
            if (!InvalidRid(iConstRec))
                pConstRec = m_pStgdb->m_MiniMd.getConstant(iConstRec);
        }
        if (! pConstRec)
        {
            IfNullGo(pConstRec=m_pStgdb->m_MiniMd.AddConstantRecord(&iConstRec));
            IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_Constant, ConstantRec::COL_Parent,
                                                 pConstRec, tk));
            IfFailGo( m_pStgdb->m_MiniMd.AddConstantToHash(iConstRec) );
        }

        // Add values to the various columns of the constant value row.
        pConstRec->SetType(static_cast<BYTE>(dwCPlusTypeFlag));
        if (!pValue)
            pValue = &ulValue;
        cbBlob = _GetSizeOfConstantBlob(dwCPlusTypeFlag, (void *)pValue, cchString);
        if (cbBlob > 0)
        {
#if BIGENDIAN
            void *pValueTemp;
            pValueTemp = (void *)alloca(cbBlob);
            IfFailGo(m_pStgdb->m_MiniMd.SwapConstant(pValue, dwCPlusTypeFlag, pValueTemp, cbBlob));
            pValue = pValueTemp;
#endif
            IfFailGo(m_pStgdb->m_MiniMd.PutBlob(TBL_Constant, ConstantRec::COL_Value,
                                                pConstRec, pValue, cbBlob));
        }


        // Create log record for non-token record.
        IfFailGo(UpdateENCLog2(TBL_Constant, iConstRec));
    }
ErrExit:
    
    return hr;
} // HRESULT RegMeta::_DefineSetConstant()


//*****************************************************************************
// Helper: Set the properties on the given Method token.
//*****************************************************************************
HRESULT RegMeta::_SetMethodProps(       // S_OK or error.
    mdMethodDef md,                     // [IN] The MethodDef.
    DWORD       dwMethodFlags,          // [IN] Method attributes.
    ULONG       ulCodeRVA,              // [IN] Code RVA.
    DWORD       dwImplFlags)            // [IN] MethodImpl flags.
{
    MethodRec   *pRecord;
    HRESULT     hr = S_OK;

    _ASSERTE(TypeFromToken(md) == mdtMethodDef && RidFromToken(md));

    // Get the Method record.
    pRecord = m_pStgdb->m_MiniMd.getMethod(RidFromToken(md));

    // Set the data.
    if (dwMethodFlags != ULONG_MAX)
    {
        // Preserve the reserved flags stored already and always keep the mdRTSpecialName
        dwMethodFlags |= (pRecord->GetFlags() & mdReservedMask);
    
        // Set the flags.
        pRecord->SetFlags(static_cast<USHORT>(dwMethodFlags));
    }
    if (ulCodeRVA != ULONG_MAX)
        pRecord->SetRVA(ulCodeRVA);
    if (dwImplFlags != ULONG_MAX)
        pRecord->SetImplFlags(static_cast<USHORT>(dwImplFlags));

    IfFailGo(UpdateENCLog(md));
ErrExit:
    return hr;
} // HRESULT RegMeta::_SetMethodProps()


//*****************************************************************************
// Helper: Set the properties on the given Field token.
//*****************************************************************************
HRESULT RegMeta::_SetFieldProps(        // S_OK or error.
    mdFieldDef  fd,                     // [IN] The FieldDef.
    DWORD       dwFieldFlags,           // [IN] Field attributes.
    DWORD       dwCPlusTypeFlag,        // [IN] Flag for the value type, selected ELEMENT_TYPE_*
    void const  *pValue,                // [IN] Constant value.
    ULONG       cchValue)               // [IN] size of constant value (string, in wide chars).
{
    FieldRec    *pRecord;
    HRESULT     hr = S_OK;
    int         bHasDefault = false;    // If defining a constant, in this call.

    _ASSERTE (TypeFromToken(fd) == mdtFieldDef && RidFromToken(fd));

    // Get the Field record.
    pRecord = m_pStgdb->m_MiniMd.getField(RidFromToken(fd));

    // See if there is a Constant.
    if ((dwCPlusTypeFlag != ELEMENT_TYPE_VOID && dwCPlusTypeFlag != ELEMENT_TYPE_END &&
         dwCPlusTypeFlag != ULONG_MAX) &&
        (pValue || (pValue == 0 && (dwCPlusTypeFlag == ELEMENT_TYPE_STRING ||
                                    dwCPlusTypeFlag == ELEMENT_TYPE_CLASS))))
    {
        if (dwFieldFlags == ULONG_MAX)
            dwFieldFlags = pRecord->GetFlags();
        dwFieldFlags |= fdHasDefault;

        bHasDefault = true;
    }

    // Set the flags.
    if (dwFieldFlags != ULONG_MAX)
    {
        if ( IsFdHasFieldRVA(dwFieldFlags) && !IsFdHasFieldRVA(pRecord->GetFlags()) ) 
        {
            // This will trigger field RVA to be created if it is not yet created!
            _SetRVA(fd, 0, 0);
        }

        // Preserve the reserved flags stored.
        dwFieldFlags |= (pRecord->GetFlags() & fdReservedMask);
        // Set the flags.
        pRecord->SetFlags(static_cast<USHORT>(dwFieldFlags));
    }

    IfFailGo(UpdateENCLog(fd));
    
    // Set the Constant.
    if (bHasDefault)
    {
        BOOL bSearch = IsCallerExternal() || IsENCOn();
        IfFailGo(_DefineSetConstant(fd, dwCPlusTypeFlag, pValue, cchValue, bSearch));
    }

ErrExit:
    return hr;
} // HRESULT RegMeta::_SetFieldProps()

//*****************************************************************************
// Helper: Set the properties on the given Property token.
//*****************************************************************************
HRESULT RegMeta::_SetPropertyProps(      // S_OK or error.
    mdProperty  pr,                     // [IN] Property token.
    DWORD       dwPropFlags,            // [IN] CorPropertyAttr.
    DWORD       dwCPlusTypeFlag,        // [IN] Flag for value type, selected ELEMENT_TYPE_*
    void const  *pValue,                // [IN] Constant value.
    ULONG       cchValue,               // [IN] size of constant value (string, in wide chars).
    mdMethodDef mdSetter,               // [IN] Setter of the property.
    mdMethodDef mdGetter,               // [IN] Getter of the property.
    mdMethodDef rmdOtherMethods[])      // [IN] Array of other methods.
{
    PropertyRec *pRecord;
    BOOL        bClear = IsCallerExternal() || IsENCOn() || IsIncrementalOn();
    HRESULT     hr = S_OK;
    int         bHasDefault = false;    // If true, constant value this call.

    

    _ASSERTE(TypeFromToken(pr) == mdtProperty && RidFromToken(pr));

    pRecord = m_pStgdb->m_MiniMd.getProperty(RidFromToken(pr));

    if (dwPropFlags != ULONG_MAX)
    {
        // Clear the reserved flags from the flags passed in.
        dwPropFlags &= (~prReservedMask);
    }
    // See if there is a constant.
    if ((dwCPlusTypeFlag != ELEMENT_TYPE_VOID && dwCPlusTypeFlag != ELEMENT_TYPE_END &&
         dwCPlusTypeFlag != ULONG_MAX) &&
        (pValue || (pValue == 0 && (dwCPlusTypeFlag == ELEMENT_TYPE_STRING ||
                                    dwCPlusTypeFlag == ELEMENT_TYPE_CLASS))))
    {
        if (dwPropFlags == ULONG_MAX)
            dwPropFlags = pRecord->GetPropFlags();
        dwPropFlags |= prHasDefault;
        
        bHasDefault = true;
    }
    if (dwPropFlags != ULONG_MAX)
    {
        // Preserve the reserved flags.
        dwPropFlags |= (pRecord->GetPropFlags() & prReservedMask);
        // Set the flags.
        pRecord->SetPropFlags(static_cast<USHORT>(dwPropFlags));
    }

    // store the getter (or clear out old one).
    if (mdGetter != ULONG_MAX)
    {
        _ASSERTE(TypeFromToken(mdGetter) == mdtMethodDef || IsNilToken(mdGetter));
        IfFailGo(_DefineMethodSemantics(msGetter, mdGetter, pr, bClear));
    }

    // Store the setter (or clear out old one).
    if (mdSetter != ULONG_MAX)
    {
        _ASSERTE(TypeFromToken(mdSetter) == mdtMethodDef || IsNilToken(mdSetter));
        IfFailGo(_DefineMethodSemantics(msSetter, mdSetter, pr, bClear));
    }

    // Store all of the other methods.
    if (rmdOtherMethods)
    {
        int         i = 0;
        mdMethodDef mb;

        while (1)
        {
            mb = rmdOtherMethods[i++];
            if (IsNilToken(mb))
                break;
            _ASSERTE(TypeFromToken(mb) == mdtMethodDef);
            IfFailGo(_DefineMethodSemantics(msOther, mb, pr, bClear));

            // The first call to _DefineMethodSemantics would've cleared all the records
            // that match with msOther and pr.
            bClear = false;
        }
    }

    IfFailGo(UpdateENCLog(pr));
    
    // Set the constant.
    if (bHasDefault)
    {
        BOOL bSearch = IsCallerExternal() || IsENCOn() || IsIncrementalOn();
        IfFailGo(_DefineSetConstant(pr, dwCPlusTypeFlag, pValue, cchValue, bSearch));
    }

ErrExit:
    
    return hr;
} // HRESULT RegMeta::_SetPropertyProps()


//*****************************************************************************
// Helper: This routine sets properties on the given Param token.
//*****************************************************************************
HRESULT RegMeta::_SetParamProps(        // Return code.
    mdParamDef  pd,                     // [IN] Param token.   
    LPCWSTR     szName,                 // [IN] Param name.
    DWORD       dwParamFlags,           // [IN] Param flags.
    DWORD       dwCPlusTypeFlag,        // [IN] Flag for value type. selected ELEMENT_TYPE_*.
    void const  *pValue,                // [OUT] Constant value.
    ULONG       cchValue)               // [IN] size of constant value (string, in wide chars).
{
    HRESULT     hr = S_OK;
    ParamRec    *pRecord;
    int         bHasDefault = false;    // Is there a default for this call.

    _ASSERTE(TypeFromToken(pd) == mdtParamDef && RidFromToken(pd));

    pRecord = m_pStgdb->m_MiniMd.getParam(RidFromToken(pd));

    // Set the properties.
    if (szName)
        IfFailGo(m_pStgdb->m_MiniMd.PutStringW(TBL_Param, ParamRec::COL_Name, pRecord, szName));

    if (dwParamFlags != ULONG_MAX)
    {
        // No one should try to set the reserved flags explicitly.
        _ASSERTE((dwParamFlags & pdReservedMask) == 0);
        // Clear the reserved flags from the flags passed in.
        dwParamFlags &= (~pdReservedMask);
    }
    // See if there is a constant.
    if ((dwCPlusTypeFlag != ELEMENT_TYPE_VOID && dwCPlusTypeFlag != ELEMENT_TYPE_END &&
         dwCPlusTypeFlag != ULONG_MAX) &&
        (pValue || (pValue == 0 && (dwCPlusTypeFlag == ELEMENT_TYPE_STRING ||
                                    dwCPlusTypeFlag == ELEMENT_TYPE_CLASS))))
    {
        if (dwParamFlags == ULONG_MAX)
            dwParamFlags = pRecord->GetFlags();
        dwParamFlags |= pdHasDefault;

        bHasDefault = true;
    }
    // Set the flags.
    if (dwParamFlags != ULONG_MAX)
    {
        // Preserve the reserved flags stored.
        dwParamFlags |= (pRecord->GetFlags() & pdReservedMask);
        // Set the flags.
        pRecord->SetFlags(static_cast<USHORT>(dwParamFlags));
    }

    // ENC log for the param record.
    IfFailGo(UpdateENCLog(pd));
    
    // Defer setting the constant until after the ENC log for the param.  Due to the way that
    //  parameter records are re-ordered, ENC needs the param record log entry to be IMMEDIATELY
    //  after the param added function.

    // Set the constant.
    if (bHasDefault)
    {
        BOOL bSearch = IsCallerExternal() || IsENCOn();
        IfFailGo(_DefineSetConstant(pd, dwCPlusTypeFlag, pValue, cchValue, bSearch));
    }

ErrExit:
    return hr;
} // HRESULT RegMeta::_SetParamProps()

//*****************************************************************************
// Create and populate a new TypeDef record.
//*****************************************************************************
HRESULT RegMeta::_DefineTypeDef(        // S_OK or error.
    LPCWSTR     szTypeDef,              // [IN] Name of TypeDef
    DWORD       dwTypeDefFlags,         // [IN] CustomAttribute flags
    mdToken     tkExtends,              // [IN] extends this TypeDef or typeref 
    mdToken     rtkImplements[],        // [IN] Implements interfaces
    mdTypeDef   tdEncloser,             // [IN] TypeDef token of the Enclosing Type.
    mdTypeDef   *ptd)                   // [OUT] Put TypeDef token here
{
    HRESULT     hr = S_OK;              // A result.
    TypeDefRec  *pRecord = NULL;        // New TypeDef record.
    RID         iRecord;                // New TypeDef RID.
    CQuickBytes qbNamespace;            // Namespace buffer.
    CQuickBytes qbName;                 // Name buffer.
    LPUTF8      szTypeDefUTF8;          // Full name in UTF8.
    ULONG       ulStringLen;            // Length of the TypeDef string.
    int         bSuccess;               // Return value for SplitPath().

    

    _ASSERTE(IsTdAutoLayout(dwTypeDefFlags) || IsTdSequentialLayout(dwTypeDefFlags) || IsTdExplicitLayout(dwTypeDefFlags));

    _ASSERTE(ptd);
    _ASSERTE(TypeFromToken(tkExtends) == mdtTypeRef || TypeFromToken(tkExtends) == mdtTypeDef || TypeFromToken(tkExtends) == mdtTypeSpec
              || IsNilToken(tkExtends));
    _ASSERTE(szTypeDef && *szTypeDef);
    _ASSERTE(IsNilToken(tdEncloser) || IsTdNested(dwTypeDefFlags));

    UTF8STR(szTypeDef, szTypeDefUTF8);
    PREFIX_ASSUME(szTypeDefUTF8 != NULL);

    ulStringLen = (ULONG)(strlen(szTypeDefUTF8) + 1);
    IfFailGo(qbNamespace.ReSizeNoThrow(ulStringLen));
    IfFailGo(qbName.ReSizeNoThrow(ulStringLen));
    bSuccess = ns::SplitPath(szTypeDefUTF8,
                             (LPUTF8)qbNamespace.Ptr(),
                             ulStringLen,
                             (LPUTF8)qbName.Ptr(),
                             ulStringLen);
    _ASSERTE(bSuccess);

    if (CheckDups(MDDupTypeDef))
    {
        // Check for existence.  Do a query by namespace and name.
        hr = ImportHelper::FindTypeDefByName(&(m_pStgdb->m_MiniMd),
                                             (LPCUTF8)qbNamespace.Ptr(),
                                             (LPCUTF8)qbName.Ptr(),
                                             tdEncloser,
                                             ptd);
        if (SUCCEEDED(hr))
        {
            if (IsENCOn())
            {
                pRecord = m_pStgdb->m_MiniMd.getTypeDef(RidFromToken(*ptd));
            }
            else
            {
                hr = META_S_DUPLICATE;
                goto ErrExit;
            }
        }
        else if (hr != CLDB_E_RECORD_NOTFOUND)
            IfFailGo(hr);
    }

    if (!pRecord)
    {
        // Create the new record.
        IfNullGo(pRecord=m_pStgdb->m_MiniMd.AddTypeDefRecord(&iRecord));

        // Invalidate the ref to def optimization since more def is introduced
        SetTypeDefDirty(true);

        if (!IsNilToken(tdEncloser))
        {
            NestedClassRec  *pNestedClassRec;
            RID         iNestedClassRec;

            // Create a new NestedClass record.
            IfNullGo(pNestedClassRec = m_pStgdb->m_MiniMd.AddNestedClassRecord(&iNestedClassRec));
            // Set the NestedClass value.
            IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_NestedClass, NestedClassRec::COL_NestedClass,
                                                 pNestedClassRec, TokenFromRid(iRecord, mdtTypeDef)));
            // Set the NestedClass value.
            IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_NestedClass, NestedClassRec::COL_EnclosingClass,
                                                 pNestedClassRec, tdEncloser));

            IfFailGo( m_pStgdb->m_MiniMd.AddNestedClassToHash(iNestedClassRec) );

            // Create the log record for the non-token record.
            IfFailGo(UpdateENCLog2(TBL_NestedClass, iNestedClassRec));
        }

        // Give token back to caller.
        *ptd = TokenFromRid(iRecord, mdtTypeDef);
    }

    // Set the namespace and name.
    IfFailGo(m_pStgdb->m_MiniMd.PutString(TBL_TypeDef, TypeDefRec::COL_Name,
                                          pRecord, (LPCUTF8)qbName.Ptr()));
    IfFailGo(m_pStgdb->m_MiniMd.PutString(TBL_TypeDef, TypeDefRec::COL_Namespace,
                                          pRecord, (LPCUTF8)qbNamespace.Ptr()));

    SetCallerDefine();
    IfFailGo(_SetTypeDefProps(*ptd, dwTypeDefFlags, tkExtends, rtkImplements));
ErrExit:
    SetCallerExternal();

    return hr;
} // HRESULT RegMeta::_DefineTypeDef()


//******************************************************************************
//--- IMetaDataTables
//******************************************************************************
HRESULT RegMeta::GetStringHeapSize(    
    ULONG   *pcbStrings)                // [OUT] Size of the string heap.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    hr =  m_pStgdb->m_MiniMd.m_Strings.GetRawSize(pcbStrings);

    END_ENTRYPOINT_NOTHROW;
    return hr;
} // HRESULT RegMeta::GetStringHeapSize()

HRESULT RegMeta::GetBlobHeapSize(
    ULONG   *pcbBlobs)                  // [OUT] Size of the Blob heap.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    hr = m_pStgdb->m_MiniMd.m_Blobs.GetRawSize(pcbBlobs);
    END_ENTRYPOINT_NOTHROW;

    return hr;

} // HRESULT RegMeta::GetBlobHeapSize()

HRESULT RegMeta::GetGuidHeapSize(
    ULONG   *pcbGuids)                  // [OUT] Size of the Guid heap.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    hr = m_pStgdb->m_MiniMd.m_Guids.GetRawSize(pcbGuids);
    END_ENTRYPOINT_NOTHROW;
    return hr;

} // HRESULT RegMeta::GetGuidHeapSize()

HRESULT RegMeta::GetUserStringHeapSize(
    ULONG   *pcbStrings)                // [OUT] Size of the User String heap.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    hr = m_pStgdb->m_MiniMd.m_USBlobs.GetRawSize(pcbStrings);
    END_ENTRYPOINT_NOTHROW;
    return hr;
} // HRESULT RegMeta::GetUserStringHeapSize()

HRESULT RegMeta::GetNumTables(
    ULONG   *pcTables)                  // [OUT] Count of tables.
{
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    *pcTables = m_pStgdb->m_MiniMd.GetCountTables();
    END_ENTRYPOINT_NOTHROW;

    return S_OK;
} // HRESULT RegMeta::GetNumTables()

HRESULT RegMeta::GetTableIndex(   
    ULONG   token,                      // [IN] Token for which to get table index.
    ULONG   *pixTbl)                    // [OUT] Put table index here.
{
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    *pixTbl = CMiniMdRW::GetTableForToken(token);
    END_ENTRYPOINT_NOTHROW;

    return S_OK;
} // HRESULT RegMeta::GetTableIndex()

HRESULT RegMeta::GetTableInfo(
    ULONG   ixTbl,                      // [IN] Which table.
    ULONG   *pcbRow,                    // [OUT] Size of a row, bytes.
    ULONG   *pcRows,                    // [OUT] Number of rows.
    ULONG   *pcCols,                    // [OUT] Number of columns in each row.
    ULONG   *piKey,                     // [OUT] Key column, or -1 if none.
    const char **ppName)                // [OUT] Name of the table.
{
    HRESULT hr = S_OK;
    CMiniTableDef *pTbl = NULL;
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    if (ixTbl >= m_pStgdb->m_MiniMd.GetCountTables())
        IfFailGo(E_INVALIDARG);
    pTbl = &m_pStgdb->m_MiniMd.m_TableDefs[ixTbl];
    if (pcbRow)
        *pcbRow = pTbl->m_cbRec;
    if (pcRows)
        *pcRows = m_pStgdb->m_MiniMd.vGetCountRecs(ixTbl);
    if (pcCols)
        *pcCols = pTbl->m_cCols;
    if (piKey)
        *piKey = (pTbl->m_iKey == (BYTE) -1) ? -1 : pTbl->m_iKey;
    if (ppName)
        *ppName = g_Tables[ixTbl].m_pName;
ErrExit:
    END_ENTRYPOINT_NOTHROW;
    return hr;
} // HRESULT RegMeta::GetTableInfo()

HRESULT RegMeta::GetColumnInfo(   
    ULONG   ixTbl,                      // [IN] Which Table
    ULONG   ixCol,                      // [IN] Which Column in the table
    ULONG   *poCol,                     // [OUT] Offset of the column in the row.
    ULONG   *pcbCol,                    // [OUT] Size of a column, bytes.
    ULONG   *pType,                     // [OUT] Type of the column.
    const char **ppName)                // [OUT] Name of the Column.
{
    HRESULT hr = S_OK;
    CMiniTableDef *pTbl = NULL;
    CMiniColDef *pCol = NULL;

    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    if (ixTbl >= m_pStgdb->m_MiniMd.GetCountTables())
        IfFailGo(E_INVALIDARG);
    pTbl = &m_pStgdb->m_MiniMd.m_TableDefs[ixTbl];
    if (ixCol >= pTbl->m_cCols)
        IfFailGo(E_INVALIDARG);
    pCol = &pTbl->m_pColDefs[ixCol];
    if (poCol)
        *poCol = pCol->m_oColumn;
    if (pcbCol)
        *pcbCol = pCol->m_cbColumn;
    if (pType)
        *pType = pCol->m_Type;
    if (ppName)
        *ppName = g_Tables[ixTbl].m_pColNames[ixCol];

ErrExit:
    END_ENTRYPOINT_NOTHROW;
    return hr;
} //  HRESULT RegMeta::GetColumnInfo()

HRESULT RegMeta::GetCodedTokenInfo(   
    ULONG   ixCdTkn,                    // [IN] Which kind of coded token.
    ULONG   *pcTokens,                  // [OUT] Count of tokens.
    ULONG   **ppTokens,                 // [OUT] List of tokens.
    const char **ppName)                // [OUT] Name of the CodedToken.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    // Validate arguments.
    if (ixCdTkn >= CDTKN_COUNT)
        IfFailGo(E_INVALIDARG);

    if (pcTokens)
        *pcTokens = g_CodedTokens[ixCdTkn].m_cTokens;
    if (ppTokens)
        *ppTokens = (ULONG*)g_CodedTokens[ixCdTkn].m_pTokens;
    if (ppName)
        *ppName = g_CodedTokens[ixCdTkn].m_pName;

ErrExit:
    END_ENTRYPOINT_NOTHROW;

    return hr;
} // HRESULT RegMeta::GetCodedTokenInfo()

HRESULT RegMeta::GetRow(      
    ULONG   ixTbl,                      // [IN] Which table.
    ULONG   rid,                        // [IN] Which row.
    void    **ppRow)                    // [OUT] Put pointer to row here.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    // Validate arguments.
    if (ixTbl >= m_pStgdb->m_MiniMd.GetCountTables())
        IfFailGo(E_INVALIDARG);
    if (rid == 0 || rid > m_pStgdb->m_MiniMd.m_Schema.m_cRecs[ixTbl])
        IfFailGo(E_INVALIDARG);

    // Get the row.
    *ppRow = m_pStgdb->m_MiniMd.getRow(ixTbl, rid);

ErrExit:
    END_ENTRYPOINT_NOTHROW;
    return hr;
} // HRESULT RegMeta::GetRow()

HRESULT RegMeta::GetColumn(
    ULONG   ixTbl,                      // [IN] Which table.
    ULONG   ixCol,                      // [IN] Which column.
    ULONG   rid,                        // [IN] Which row.
    ULONG   *pVal)                      // [OUT] Put the column contents here.
{
    HRESULT hr = S_OK;
    CMiniColDef *pCol = NULL;
    CMiniTableDef *pTbl = NULL;

    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    void    *pRow = NULL;                      // Row with data.

    // Validate arguments.
    if (ixTbl >= m_pStgdb->m_MiniMd.GetCountTables())
        IfFailGo(E_INVALIDARG);
    pTbl = &m_pStgdb->m_MiniMd.m_TableDefs[ixTbl];
    if (ixCol >= pTbl->m_cCols)
        IfFailGo(E_INVALIDARG);
    if (rid == 0 || rid > m_pStgdb->m_MiniMd.m_Schema.m_cRecs[ixTbl])
        IfFailGo(E_INVALIDARG);

    // Get the row.
    pRow = m_pStgdb->m_MiniMd.getRow(ixTbl, rid);

    // Is column a token column?
    pCol = &pTbl->m_pColDefs[ixCol];
    if (pCol->m_Type <= iCodedTokenMax)
        *pVal = m_pStgdb->m_MiniMd.GetToken(ixTbl, ixCol, pRow);
    else
        *pVal = m_pStgdb->m_MiniMd.GetCol(ixTbl, ixCol, pRow);

ErrExit:
    END_ENTRYPOINT_NOTHROW;
    return hr;
} // HRESULT RegMeta::GetColumn()

HRESULT RegMeta::GetString(   
    ULONG   ixString,                   // [IN] Value from a string column.
    const char **ppString)              // [OUT] Put a pointer to the string here.
{
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    *ppString = m_pStgdb->m_MiniMd.getString(ixString);
    END_ENTRYPOINT_NOTHROW;

    return S_OK;
} // HRESULT RegMeta::GetString()

HRESULT RegMeta::GetBlob(     
    ULONG   ixBlob,                     // [IN] Value from a blob column.
    ULONG   *pcbData,                   // [OUT] Put size of the blob here.
    const void **ppData)                // [OUT] Put a pointer to the blob here.
{
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    *ppData = m_pStgdb->m_MiniMd.getBlob(ixBlob, pcbData);
    END_ENTRYPOINT_NOTHROW;

    return S_OK;
} // HRESULT RegMeta::GetBlob()

HRESULT RegMeta::GetGuid(     
    ULONG   ixGuid,                     // [IN] Value from a guid column.
    const GUID **ppGuid)                // [OUT] Put a pointer to the GUID here.
{
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    *ppGuid = m_pStgdb->m_MiniMd.m_Guids.GetGuid(ixGuid);
    END_ENTRYPOINT_NOTHROW;

    return S_OK;
} // HRESULT RegMeta::GetGuid()


HRESULT RegMeta::GetUserString(   
    ULONG   ixUserString,               // [IN] Value from a UserString column.
    ULONG   *pcbData,                   // [OUT] Put size of the UserString here.
    const void **ppData)                // [OUT] Put a pointer to the UserString here.
{
    BEGIN_ENTRYPOINT_NOTHROW;

    // These are for dumping metadata information. 
    // We probably don't need to do any lock here.

    *ppData = m_pStgdb->m_MiniMd.GetUserString(ixUserString, pcbData);
    END_ENTRYPOINT_NOTHROW;

    return S_OK;
} // HRESULT RegMeta::GetUserString()

HRESULT RegMeta::GetNextString(   
    ULONG   ixString,                   // [IN] Value from a string column.
    ULONG   *pNext)                     // [OUT] Put the index of the next string here.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = m_pStgdb->m_MiniMd.m_Strings.GetNextItem(ixString, pNext);

    END_ENTRYPOINT_NOTHROW;
    return hr;
} // STDMETHODIMP GetNextString()

HRESULT RegMeta::GetNextBlob(     
    ULONG   ixBlob,                     // [IN] Value from a blob column.
    ULONG   *pNext)                     // [OUT] Put the index of the netxt blob here.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = m_pStgdb->m_MiniMd.m_Blobs.GetNextItem(ixBlob, pNext);
    END_ENTRYPOINT_NOTHROW;
    return hr;
} // STDMETHODIMP GetNextBlob()

HRESULT RegMeta::GetNextGuid(     
    ULONG   ixGuid,                     // [IN] Value from a guid column.
    ULONG   *pNext)                     // [OUT] Put the index of the next guid here.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;
    hr = m_pStgdb->m_MiniMd.m_Guids.GetNextItem(ixGuid, pNext);
    END_ENTRYPOINT_NOTHROW;
    return hr;
} // STDMETHODIMP GetNextGuid()

HRESULT RegMeta::GetNextUserString(    
    ULONG   ixUserString,               // [IN] Value from a UserString column.
    ULONG   *pNext)                     // [OUT] Put the index of the next user string here.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;
    hr = m_pStgdb->m_MiniMd.m_USBlobs.GetNextItem(ixUserString, pNext);
    END_ENTRYPOINT_NOTHROW;
    return hr;
} // STDMETHODIMP GetNextUserString()

HRESULT RegMeta::GetMetaDataStorage(
    const void **ppvMd,                 // [OUT] put pointer to MD section here (aka, 'BSJB').
    ULONG   *pcbMd)                     // [OUT] put size of the stream here.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;
    hr = m_pStgdb->GetRawData(ppvMd, pcbMd);
    END_ENTRYPOINT_NOTHROW;
    return hr;
} // HRESULT RegMeta::GetMetaDataStorage()

HRESULT RegMeta::GetMetaDataStreamInfo( // Get info about the MD stream.
    ULONG   ix,                         // [IN] Stream ordinal desired.
    const char **ppchName,              // [OUT] put pointer to stream name here.
    const void **ppv,                   // [OUT] put pointer to MD stream here.
    ULONG   *pcb)                       // [OUT] put size of the stream here.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = m_pStgdb->GetRawStreamInfo(ix, ppchName, ppv, pcb);
    END_ENTRYPOINT_NOTHROW;
    return hr;
} // HRESULT RegMeta::GetMetaDataStreamInfo()


//------------------------------------------------------------------------------
// Metadata dump 
//
#if defined(_DEBUG)

#define STRING_BUFFER_LEN 1024
#define ENUM_BUFFER_SIZE 10

int DumpMD_Write(__in __in_z char *str)
{
    OutputDebugStringA(str);
    return 0; // strlen(str);
} // int DumpMD_Write()

int DumpMD_WriteLine(__in __in_z char *str)
{
    OutputDebugStringA(str);
    OutputDebugStringA("\n");
    return 0; // strlen(str);
} // int DumpMD_Write()

int DumpMD_VWriteMarker(__in __in_z char *str, va_list marker)
{
    CQuickBytes m_output;
    
    int     count = -1;
    int     i = 1;
    HRESULT hr;

    while (count < 0)
    {
        if (FAILED(hr = m_output.ReSizeNoThrow(STRING_BUFFER_LEN * i)))
            return 0;
        count = _vsnprintf((char *)m_output.Ptr(), STRING_BUFFER_LEN * i, str, marker);
        i *= 2;
    }
    OutputDebugStringA((LPCSTR)m_output.Ptr());
    return count;
} // int DumpMD_VWriteMarker()

int DumpMD_VWrite(__in __in_z char *str, ...)
{
    va_list marker;
    int     count;

    va_start(marker, str);
    count = DumpMD_VWriteMarker(str, marker);
    va_end(marker);
    return count;
} // int DumpMD_VWrite()

int DumpMD_VWriteLine(__in __in_z char *str, ...)
{
    va_list marker;
    int     count;

    va_start(marker, str);
    count = DumpMD_VWriteMarker(str, marker);
    DumpMD_Write("\n");
    va_end(marker);
    return count;
} // int DumpMD_VWriteLine()


const char *DumpMD_DumpRawNameOfType(RegMeta *pMD, ULONG iType)
{
    if (iType <= iRidMax)
    {
        const char *pNameTable;
        pMD->GetTableInfo(iType, 0,0,0,0, &pNameTable);
        return pNameTable;
    }
    else
    // Is the field a coded token?
    if (iType <= iCodedTokenMax)
    {
        int iCdTkn = iType - iCodedToken;
        const char *pNameCdTkn;
        pMD->GetCodedTokenInfo(iCdTkn, 0,0, &pNameCdTkn);
        return pNameCdTkn;
    }

    // Fixed type.
    switch (iType)
    {
    case iBYTE:
        return "BYTE";
    case iSHORT:
        return "short";
    case iUSHORT:
        return "USHORT";
    case iLONG:
        return "long";
    case iULONG:
        return "ULONG";
    case iSTRING:
        return "string";
    case iGUID:
        return "GUID";
    case iBLOB:
        return "blob";
    }
    // default:
    static char buf[30];
    sprintf_s(buf, NumItems(buf), "unknown type 0x%02x", iType);
    return buf;
} // const char *DumpMD_DumpRawNameOfType()

void DumpMD_DumpRawCol(RegMeta *pMD, ULONG ixTbl, ULONG ixCol, ULONG rid, bool bStats)
{
    ULONG       ulType;                 // Type of a column.
    ULONG       ulVal;                  // Value of a column.
    LPCUTF8     pString;                // Pointer to a string.
    const void  *pBlob;                 // Pointer to a blob.
    ULONG       cb;                     // Size of something.

    pMD->GetColumn(ixTbl, ixCol, rid, &ulVal);
    pMD->GetColumnInfo(ixTbl, ixCol, 0, 0, &ulType, 0);

    if (ulType <= iRidMax)
    {
        const char *pNameTable;
        pMD->GetTableInfo(ulType, 0,0,0,0, &pNameTable);
        DumpMD_VWrite("%s[%x]", pNameTable, ulVal);
    }
    else
    // Is the field a coded token?
    if (ulType <= iCodedTokenMax)
    {
        int iCdTkn = ulType - iCodedToken; 
        const char *pNameCdTkn;
        pMD->GetCodedTokenInfo(iCdTkn, 0,0, &pNameCdTkn);
        DumpMD_VWrite("%s[%08x]", pNameCdTkn, ulVal);
    }
    else
    {
        // Fixed type.
        switch (ulType)
        {
        case iBYTE:
            DumpMD_VWrite("%02x", ulVal);
            break;
        case iSHORT:
        case iUSHORT:
            DumpMD_VWrite("%04x", ulVal);
            break;
        case iLONG:
        case iULONG:
            DumpMD_VWrite("%08x", ulVal);
            break;
        case iSTRING:
            DumpMD_VWrite("string#%x", ulVal);
            if (bStats && ulVal)
            {
                pMD->GetString(ulVal, &pString);
                cb = (ULONG) strlen(pString) + 1;
                DumpMD_VWrite("(%d)", cb);
            }
            break;
        case iGUID:
            DumpMD_VWrite("guid#%x", ulVal);
            if (bStats && ulVal)
            {
                DumpMD_VWrite("(16)");
            }
            break;
        case iBLOB:
            DumpMD_VWrite("blob#%x", ulVal);
            if (bStats && ulVal)
            {
                pMD->GetBlob(ulVal, &cb, &pBlob);
                cb += 1;
                if (cb > 128)
                    cb += 1;
                if (cb > 16535)
                    cb += 1;
                DumpMD_VWrite("(%d)", cb);
            }
            break;
        default:
            DumpMD_VWrite("unknown type 0x%04x", ulVal);
            break;
        }
    }
} // void DumpMD_DumpRawCol()

ULONG DumpMD_DumpRawColStats(RegMeta *pMD, ULONG ixTbl, ULONG ixCol, ULONG cRows)
{
    ULONG rslt = 0;
    ULONG       ulType;                 // Type of a column.
    ULONG       ulVal;                  // Value of a column.
    LPCUTF8     pString;                // Pointer to a string.
    const void  *pBlob;                 // Pointer to a blob.
    ULONG       cb;                     // Size of something.

    pMD->GetColumnInfo(ixTbl, ixCol, 0, 0, &ulType, 0);

    if (IsHeapType(ulType))
    {
        for (ULONG rid=1; rid<=cRows; ++rid)
        {
            pMD->GetColumn(ixTbl, ixCol, rid, &ulVal);
            // Fixed type.
            switch (ulType)
            {
            case iSTRING:
                if (ulVal)
                {
                    pMD->GetString(ulVal, &pString);
                    cb = (ULONG) strlen(pString);
                    rslt += cb + 1;
                }
                break;
            case iGUID:
                if (ulVal)
                    rslt += 16;
                break;
            case iBLOB:
                if (ulVal)
                {
                    pMD->GetBlob(ulVal, &cb, &pBlob);
                    rslt += cb + 1;
                    if (cb > 128)
                        rslt += 1;
                    if (cb > 16535)
                        rslt += 1;
                }
                break;
            default:
                break;
            }
        }
    }
    return rslt;
} // ULONG DumpMD_DumpRawColStats()

int DumpMD_DumpHex(
    const char  *szPrefix,              // String prefix for first line.
    const void  *pvData,                // The data to print.
    ULONG       cbData,                 // Bytes of data to print.
    int         bText=1,                // If true, also dump text.
    ULONG       nLine=16)               // Bytes per line to print.
{
    const BYTE  *pbData = static_cast<const BYTE*>(pvData);
    ULONG       i;                      // Loop control.
    ULONG       nPrint;                 // Number to print in an iteration.
    ULONG       nSpace;                 // Spacing calculations.
    ULONG       nPrefix;                // Size of the prefix.
    ULONG       nLines=0;               // Number of lines printed.
    const char  *pPrefix;               // For counting spaces in the prefix.

    // Round down to 8 characters.
    nLine = nLine & ~0x7;

    for (nPrefix=0, pPrefix=szPrefix; *pPrefix; ++pPrefix)
    {
        if (*pPrefix == '\t')
            nPrefix = (nPrefix + 8) & ~7;
        else
            ++nPrefix;
    }
    //nPrefix = strlen(szPrefix);
    do 
    {   // Write the line prefix.
        if (szPrefix)
            DumpMD_VWrite("%s:", szPrefix);
        else
            DumpMD_VWrite("%*s:", nPrefix, "");
        szPrefix = 0;
        ++nLines;

        // Calculate spacing.
        nPrint = min(cbData, nLine);
        nSpace = nLine - nPrint;

            // dump in hex.
        for(i=0; i<nPrint; i++)
            {
            if ((i&7) == 0)
                    DumpMD_Write(" ");
            DumpMD_VWrite("%02x ", pbData[i]);
            }
        if (bText)
        {
            // Space out to the text spot.
            if (nSpace)
                DumpMD_VWrite("%*s", nSpace*3+nSpace/8, "");
            // Dump in text.
            DumpMD_Write(">");
            for(i=0; i<nPrint; i++)
                DumpMD_VWrite("%c", (isprint(pbData[i])) ? pbData[i] : ' ');
            // Space out the text, and finish the line.
            DumpMD_VWrite("%*s<", nSpace, "");
        }
        DumpMD_VWriteLine("");

        // Next data to print.
        cbData -= nPrint;
        pbData += nPrint;
        }
    while (cbData > 0);

    return nLines;
} // int DumpMD_DumpHex()

void DumpMD_DisplayUserStrings(
    RegMeta     *pMD)                   // The scope to dump.
{
    HCORENUM    stringEnum = NULL;      // string enumerator.
    mdString    Strings[ENUM_BUFFER_SIZE]; // String tokens from enumerator.
    CQuickArray<WCHAR> rUserString;     // Buffer to receive string.
    WCHAR       *szUserString;          // Working pointer into buffer.
    ULONG       chUserString;           // Size of user string.
    CQuickArray<char> rcBuf;            // Buffer to hold the BLOB version of the string.
    char        *szBuf;                 // Working pointer into buffer.
    ULONG       chBuf;                  // Saved size of the user string.
    ULONG       count;                  // Items returned from enumerator.
    ULONG       totalCount = 1;         // Running count of strings.
    bool        bUnprint = false;       // Is an unprintable character found?
    HRESULT     hr;                     // A result.
    while (SUCCEEDED(hr = pMD->EnumUserStrings( &stringEnum,
                             Strings, NumItems(Strings), &count)) &&
            count > 0)
    {
        if (totalCount == 1)
        {   // If only one, it is the NULL string, so don't print it.
            DumpMD_WriteLine("User Strings");
            DumpMD_WriteLine("-------------------------------------------------------");
        }
        for (ULONG i = 0; i < count; i++, totalCount++)
        {
            do { // Try to get the string into the existing buffer.
                hr = pMD->GetUserString( Strings[i], rUserString.Ptr(),(ULONG32)rUserString.MaxSize(), &chUserString);
                if (hr == CLDB_S_TRUNCATION)
                {   // Buffer wasn't big enough, try to enlarge it.
                    if (FAILED(rUserString.ReSizeNoThrow(chUserString)))
                        DumpMD_VWriteLine("malloc failed: %#8x.", E_OUTOFMEMORY);
                    continue;
                }
            } while (0);
            if (FAILED(hr)) DumpMD_VWriteLine("GetUserString failed: %#8x.", hr);

            szUserString = rUserString.Ptr();
            chBuf = chUserString;

            DumpMD_VWrite("%08x : (%2d) L\"", Strings[i], chUserString);
            while (chUserString)
            {   
                switch (*szUserString)
                {
                case 0:
                    DumpMD_Write("\\0"); break;
                case L'\r':
                    DumpMD_Write("\\r"); break;
                case L'\n':
                    DumpMD_Write("\\n"); break;
                case L'\t':
                    DumpMD_Write("\\t"); break;
                default:
                    if (iswprint(*szUserString))
                        DumpMD_VWrite("%lc", *szUserString);
                    else 
                    {
                        bUnprint = true;
                        DumpMD_Write(".");
                    }
                    break;
                }
                ++szUserString;
                --chUserString;
            }
            DumpMD_WriteLine("\"");

            // Print the user string as a blob if an unprintable character is found.
            if (bUnprint)
            {
                bUnprint = false;
                szUserString = rUserString.Ptr();
                rcBuf.ReSizeNoThrow(81); //(chBuf * 5 + 1);
                szBuf = rcBuf.Ptr();
                ULONG j,k;
                DumpMD_WriteLine("\t\tUser string has unprintables, hex format below:");
                for (j = 0,k=0; j < chBuf; j++)
                {
                    // See rcBuf.ResSizeNoThrow(81) above
                    sprintf_s (&szBuf[k*5],81-(k*5), "%04x ", szUserString[j]);
                    k++;
                    if((k==16)||(j == (chBuf-1)))
                    {
                        szBuf[k*5] = '\0';
                        DumpMD_VWriteLine("\t\t%s", szBuf);
                        k=0;
                    }
                }
            }
        }
    }
    if (stringEnum)
        pMD->CloseEnum(stringEnum);
}   // void MDInfo::DisplayUserStrings()

void DumpMD_DumpRawHeaps(
    RegMeta     *pMD)                   // The scope to dump.
{
    HRESULT     hr;                     // A result.
    ULONG       ulSize;                 // Bytes in a heap.
    const BYTE  *pData;                 // Pointer to a blob.
    ULONG       cbData;                 // Size of a blob.
    ULONG       oData;                  // Offset of current blob.
    char        rcPrefix[30];           // To format line prefix.

    pMD->GetBlobHeapSize(&ulSize);
    DumpMD_VWriteLine("");
    DumpMD_VWriteLine("Blob Heap:  %d(%#x) bytes", ulSize,ulSize);
    oData = 0;
    do 
    {
        pMD->GetBlob(oData, &cbData, (const void**)&pData);
        sprintf_s(rcPrefix, NumItems(rcPrefix), "%5x,%-2x", oData, cbData);
        DumpMD_DumpHex(rcPrefix, pData, cbData);
        hr = pMD->GetNextBlob(oData, &oData);
    }
    while (hr == S_OK);

    pMD->GetStringHeapSize(&ulSize);
    DumpMD_VWriteLine("");
    DumpMD_VWriteLine("String Heap:  %d(%#x) bytes", ulSize,ulSize);
    oData = 0;
    const char *pString;
    do 
    {
        pMD->GetString(oData, &pString);
        sprintf_s(rcPrefix, NumItems(rcPrefix), "%08x", oData);
        DumpMD_DumpHex(rcPrefix, pString, (ULONG)strlen(pString)+1);
        if (*pString != 0)
            DumpMD_VWrite("%08x: %s\n", oData, pString);
        hr = pMD->GetNextString(oData, &oData);
    }
    while (hr == S_OK);
    DumpMD_VWriteLine("");
    
    DumpMD_DisplayUserStrings(pMD);

} // void DumpMD_DumpRawHeaps()


void DumpMD_DumpRaw(RegMeta *pMD, int iDump, bool bStats)
{
    ULONG       cTables;                // Tables in the database.
    ULONG       cCols;                  // Columns in a table.
    ULONG       cRows;                  // Rows in a table.
    ULONG       cbRow;                  // Bytes in a row of a table.
    ULONG       iKey;                   // Key column of a table.
    const char  *pNameTable;            // Name of a table.
    ULONG       oCol;                   // Offset of a column.
    ULONG       cbCol;                  // Size of a column.
    ULONG       ulType;                 // Type of a column.
    const char  *pNameColumn;           // Name of a column.
    ULONG       ulSize;

    pMD->GetNumTables(&cTables);

    pMD->GetStringHeapSize(&ulSize);
    DumpMD_VWrite("Strings: %d(%#x)", ulSize, ulSize);
    pMD->GetBlobHeapSize(&ulSize);
    DumpMD_VWrite(", Blobs: %d(%#x)", ulSize, ulSize);
    pMD->GetGuidHeapSize(&ulSize);
    DumpMD_VWrite(", Guids: %d(%#x)", ulSize, ulSize);
    pMD->GetUserStringHeapSize(&ulSize);
    DumpMD_VWriteLine(", User strings: %d(%#x)", ulSize, ulSize);

    for (ULONG ixTbl = 0; ixTbl < cTables; ++ixTbl)
    {
        pMD->GetTableInfo(ixTbl, &cbRow, &cRows, &cCols, &iKey, &pNameTable);

        if (cRows == 0 && iDump < 3)
            continue;

        if (iDump >= 2)
            DumpMD_VWriteLine("=================================================");
        DumpMD_VWriteLine("%2d: %-20s cRecs:%5d(%#x), cbRec:%3d(%#x), cbTable:%6d(%#x)",
            ixTbl, pNameTable, cRows, cRows, cbRow, cbRow, cbRow * cRows, cbRow * cRows);

        if (iDump < 2)
            continue;

        // Dump column definitions for the table.
        ULONG ixCol;
        for (ixCol=0; ixCol<cCols; ++ixCol)
        {
            pMD->GetColumnInfo(ixTbl, ixCol, &oCol, &cbCol, &ulType, &pNameColumn);

            DumpMD_VWrite("  col %2x:%c %-12s oCol:%2x, cbCol:%x, %-7s",
                ixCol, ((ixCol==iKey)?'*':' '), pNameColumn, oCol, cbCol, DumpMD_DumpRawNameOfType(pMD, ulType));

            if (bStats)
            {
                ulSize = DumpMD_DumpRawColStats(pMD, ixTbl, ixCol, cRows);
                if (ulSize)
                    DumpMD_VWrite("(%d)", ulSize);
            }
            DumpMD_VWriteLine("");
        }

        if (iDump < 3) 
            continue;

        // Dump the rows.
        for (ULONG rid = 1; rid <= cRows; ++rid)
        {
            if (rid == 1)
                DumpMD_VWriteLine("-------------------------------------------------");
            DumpMD_VWrite(" %3x == ", rid);
            for (ixCol=0; ixCol < cCols; ++ixCol)
            {
                if (ixCol) DumpMD_VWrite(", ");
                DumpMD_VWrite("%d:", ixCol);
                DumpMD_DumpRawCol(pMD, ixTbl, ixCol, rid, bStats);
            }
            DumpMD_VWriteLine("");
        }
    }

    DumpMD_DumpRawHeaps(pMD);

} // void DumpMD_DumpRaw()


int DumpMD_impl(RegMeta *pMD)
{
   DumpMD_DumpRaw(pMD, 3, false);
   return 0;
}

int DumpMD(UINT_PTR iMD)
{
    RegMeta *pMD = reinterpret_cast<RegMeta*>(iMD);
    return DumpMD_impl(pMD);
}
#endif

//******************************************************************************
// --- IMetaDataTables
//******************************************************************************

//*****************************************************************************
// Using the existing RegMeta and reopen with another chuck of memory. Make sure that all stgdb
// is still kept alive.
//*****************************************************************************
HRESULT RegMeta::ReOpenWithMemory(     
    LPCVOID     pData,                  // [in] Location of scope data.
    ULONG       cbData,                 // [in] Size of the data pointed to by pData.
    DWORD       dwReOpenFlags)           // [in] ReOpen flags
{
    HRESULT         hr = NOERROR;

    // Only allow the ofCopyMemory and ofTakeOwnership flags 
    if (dwReOpenFlags != 0 && ((dwReOpenFlags & (~(ofCopyMemory|ofTakeOwnership))) > 0))
        return E_INVALIDARG;

    LOCKWRITE();

    // put the current m_pStgdb to the free list
    m_pStgdb->m_pNextStgdb = m_pStgdbFreeList;
    m_pStgdbFreeList = m_pStgdb;
    m_pStgdb = new (nothrow) CLiteWeightStgdbRW;
    IfNullGo( m_pStgdb );
    IfFailGo( OpenExistingMD(0 /* szFileName */, const_cast<void*>(pData), cbData, ofReOpen|dwReOpenFlags /* flags */) );

    // we are done!

ErrExit:
    if (FAILED(hr))
    {
        // recover to the old state
        if (m_pStgdb)
            delete m_pStgdb;
        m_pStgdb = m_pStgdbFreeList;
        m_pStgdbFreeList = m_pStgdbFreeList->m_pNextStgdb;
    }

    return hr;
} // HRESULT RegMeta::ReOpenWithMemory()


//*****************************************************************************
// This function returns the requested public interface based on the given
// internal import interface.
//*****************************************************************************
STDAPI MDReOpenMetaDataWithMemoryEx(
    void        *pImport,               // [IN] Given scope. public interfaces
    LPCVOID     pData,                  // [in] Location of scope data.
    ULONG       cbData,                 // [in] Size of the data pointed to by pData.
    DWORD       dwReOpenFlags)          // [in] Flags for ReOpen
{
    HRESULT             hr = S_OK;
    IUnknown            *pUnk = (IUnknown *) pImport;
    IMetaDataImport2    *pMDImport = NULL;
    RegMeta             *pRegMeta = NULL;
   
    _ASSERTE(pImport);

    IfFailGo( pUnk->QueryInterface(IID_IMetaDataImport2, (void **) &pMDImport) );
    pRegMeta = (RegMeta*) pMDImport;

    IfFailGo( pRegMeta->ReOpenWithMemory(pData, cbData, dwReOpenFlags) );

ErrExit:
    if (pMDImport)
        pMDImport->Release();
   
    return hr;
} // STDAPI MDReOpenMetaDataWithMemoryEx()

STDAPI MDReOpenMetaDataWithMemory(
    void        *pImport,               // [IN] Given scope. public interfaces
    LPCVOID     pData,                  // [in] Location of scope data.
    ULONG       cbData)                 // [in] Size of the data pointed to by pData.
{
    return MDReOpenMetaDataWithMemoryEx(pImport, pData, cbData, 0);
}
 
