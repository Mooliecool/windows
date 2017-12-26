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
// Disp.cpp
//
// Implementation for the meta data dispenser code.
//
//*****************************************************************************
#include "stdafx.h"
#include "disp.h"
#include "regmeta.h"
#include "mdutil.h"
#include <corerror.h>
#include <mdlog.h>
#include <mdcommon.h>


//*****************************************************************************
// Ctor.
//*****************************************************************************
Disp::Disp() : m_cRef(0), m_Namespace(0)
{
#if defined(LOGGING)
    // InitializeLogging() calls scattered around the code.
    InitializeLogging();
#endif

    m_OptionValue.m_DupCheck = MDDupDefault;
    m_OptionValue.m_RefToDefCheck = MDRefToDefDefault;
    m_OptionValue.m_NotifyRemap = MDNotifyDefault;
    m_OptionValue.m_UpdateMode = MDUpdateFull;
    m_OptionValue.m_ErrorIfEmitOutOfOrder = MDErrorOutOfOrderDefault;
    m_OptionValue.m_ThreadSafetyOptions = MDThreadSafetyDefault;
    m_OptionValue.m_GenerateTCEAdapters = FALSE;
    m_OptionValue.m_ImportOption = MDImportOptionDefault;
    m_OptionValue.m_LinkerOption = MDAssembly;
    m_OptionValue.m_RuntimeVersion = NULL;
    m_OptionValue.m_MetadataVersion = MDDefaultVersion;
    m_OptionValue.m_MergeOptions = MergeFlagsNone;

} // Disp::Disp()

Disp::~Disp()
{
    if (m_OptionValue.m_RuntimeVersion)
        delete [] m_OptionValue.m_RuntimeVersion;
    if (m_Namespace)
        delete [] m_Namespace;
} // Disp::~Disp()

//*****************************************************************************
// Create a brand new scope.  This is based on the CLSID that was used to get
// the dispenser.
//*****************************************************************************
HRESULT Disp::DefineScope(              // Return code.
    REFCLSID    rclsid,                 // [in] What version to create.
    DWORD       dwCreateFlags,          // [in] Flags on the create.
    REFIID      riid,                   // [in] The interface desired.
    IUnknown    **ppIUnk)               // [out] Return interface on success.
{
    HRESULT     hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    RegMeta     *pMeta = 0;
    OptionValue optionForNewScope = m_OptionValue;


    LOG((LF_METADATA, LL_INFO10, "Disp::DefineScope(0x%08x, 0x%08x, 0x%08x, 0x%08x)\n", rclsid, dwCreateFlags, riid, ppIUnk));

    if (dwCreateFlags)
        IfFailGo(E_INVALIDARG);

    // Figure out what version of the metadata to emit
    if (rclsid == CLSID_CLR_v1_MetaData)
    {
        optionForNewScope.m_MetadataVersion = MDVersion1;
    }
    else if (rclsid == CLSID_CLR_v2_MetaData)
    {
        optionForNewScope.m_MetadataVersion = MDVersion2;
    }
    else
    {
        // If it is a version we don't understand, then we cannot continue.
        IfFailGo(CLDB_E_FILE_OLDVER);
    }


    // Create a new coclass for this guy.
    pMeta = new (nothrow) RegMeta();
    IfNullGo( pMeta );

    IfFailGo(pMeta->SetOption(&optionForNewScope));

    // Create the MiniMd-style scope.
    IfFailGo(pMeta->CreateNewMD());

    // Get the requested interface.
    IfFailGo(pMeta->QueryInterface(riid, (void **) ppIUnk));
    
    // Add the new RegMeta to the cache.
    IfFailGo(pMeta->AddToCache());
    
    LOG((LOGMD, "{%08x} Created new emit scope\n", pMeta));

ErrExit:
    if (FAILED(hr))
    {
        if (pMeta) delete pMeta;
        *ppIUnk = 0;
    }
    END_ENTRYPOINT_NOTHROW;

    return (hr);
} // HRESULT Disp::DefineScope()


//*****************************************************************************
// Open an existing scope.
//*****************************************************************************
HRESULT Disp::OpenScope(                // Return code.
    LPCWSTR     szFileName,             // [in] The scope to open.
    DWORD       dwOpenFlags,            // [in] Open mode flags.
    REFIID      riid,                   // [in] The interface desired.
    IUnknown    **ppIUnk)               // [out] Return interface on success.
{
    HRESULT     hr;                     // A result.

    BEGIN_ENTRYPOINT_NOTHROW;

    RegMeta     *pMeta = 0;

    LOG((LF_METADATA, LL_INFO10, "Disp::OpenScope(%S, 0x%08x, 0x%08x, 0x%08x)\n", MDSTR(szFileName), dwOpenFlags, riid, ppIUnk));

    _ASSERTE(!IsOfReserved(dwOpenFlags));

    // Validate that there is some sort of file name.
    if (!szFileName || !szFileName[0] || !ppIUnk)
        IfFailGo(E_INVALIDARG);

    // If open-for-read, and there is already an open-for-read copy, return it.
    if (IsOfReadOnly(dwOpenFlags))
    {
        RegMeta::FindCachedReadOnlyEntry(szFileName, dwOpenFlags, &pMeta);
        if (pMeta != NULL)
        {
            // Return the requested interface.
            hr = pMeta->QueryInterface(riid, (void **) ppIUnk);
            if (FAILED(hr))
                pMeta = NULL; // Don't delete cached RegMeta!
            else
            {
                pMeta->Release(); // Give back refcount from QI
                LOG((LOGMD, "{%08x} Found in cache '%S'\n", pMeta, MDSTR(szFileName)));
            }

            goto ErrExit;
        }
    }
    // Create a new coclass for this guy.
    pMeta = new (nothrow) RegMeta();
    IfNullGo( pMeta );

    IfFailGo(pMeta->SetOption(&m_OptionValue));

    // Always initialize the RegMeta's stgdb. 
    if (memcmp(szFileName, L"file:", 10) == 0)
    {
        szFileName = &szFileName[5];
    }

    // Try to open the MiniMd-style scope.
    IfFailGo(pMeta->OpenExistingMD(szFileName, 0 /* pbData */,0 /* cbData */, dwOpenFlags));

    // Obtain the requested interface.
    IfFailGo(pMeta->QueryInterface(riid, (void **) ppIUnk) );

    // Add the new RegMeta to the cache.  If this is read-only, any future opens will
    //  find this entry.  If, due to another thread concurrently opening the same file,
    //  there is already another copy in the cache, well, then there will be two 
    //  read-only copies in the cache.  This is considered to be somewhat of a corner
    //  case, and the only harm is temporary memory usage.  All requests will be
    //  satisfied by one or the other (depending on search algorithm), and eventually, 
    //  the "other" copy will be released.
    IfFailGo(pMeta->AddToCache());
    
    LOG((LOGMD, "{%08x} Successfully opened '%S'\n", pMeta, MDSTR(szFileName)));

#if defined(_DEBUG)
    if (REGUTIL::GetConfigDWORD(L"MD_RegMetaDump", 0))
    {
        int DumpMD_impl(RegMeta *pMD);
        DumpMD_impl(pMeta);
    }
#endif // _DEBUG

ErrExit:
    if (FAILED(hr))
    {
        if (pMeta) delete pMeta;
        *ppIUnk = 0;
    }

    END_ENTRYPOINT_NOTHROW;
    
    return (hr);
} // HRESULT Disp::OpenScope()


//*****************************************************************************
// Open an existing scope.
//*****************************************************************************
HRESULT Disp::OpenScopeOnMemory(        // Return code.
    LPCVOID     pData,                  // [in] Location of scope data.
    ULONG       cbData,                 // [in] Size of the data pointed to by pData.
    DWORD       dwOpenFlags,            // [in] Open mode flags.
    REFIID      riid,                   // [in] The interface desired.
    IUnknown    **ppIUnk)               // [out] Return interface on success.
{
    HRESULT     hr;

    BEGIN_ENTRYPOINT_NOTHROW;

    RegMeta     *pMeta = 0;

    LOG((LF_METADATA, LL_INFO10, "Disp::OpenScopeOnMemory(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\n", pData, cbData, dwOpenFlags, riid, ppIUnk));
   
    _ASSERTE(!IsOfReserved(dwOpenFlags));

    // Create a new coclass for this guy.
    pMeta = new (nothrow) RegMeta();
    IfNullGo(pMeta);
    IfFailGo(pMeta->SetOption(&m_OptionValue));
    

    PREFIX_ASSUME(pMeta != NULL);
    // Always initialize the RegMeta's stgdb. 
    IfFailGo(pMeta->OpenExistingMD(0 /* szFileName */, const_cast<void*>(pData), cbData, dwOpenFlags));

    LOG((LOGMD, "{%08x} Opened new scope on memory, pData: %08x    cbData: %08x\n", pMeta, pData, cbData));

    // Return the requested interface.
    IfFailGo( pMeta->QueryInterface(riid, (void **) ppIUnk) );

    // Add the new RegMeta to the cache.
    IfFailGo(pMeta->AddToCache());
    
#if defined(_DEBUG)
    if (REGUTIL::GetConfigDWORD(L"MD_RegMetaDump", 0))
    {
        int DumpMD_impl(RegMeta *pMD);
        DumpMD_impl(pMeta);
    }
#endif // _DEBUG

ErrExit:
    if (FAILED(hr))
    {
        if (pMeta) delete pMeta;
        *ppIUnk = 0;
    }

    END_ENTRYPOINT_NOTHROW;
    
    return (hr);
} // HRESULT Disp::OpenScopeOnMemory()


//*****************************************************************************
// Get the directory where the CLR system resides.
//*****************************************************************************
HRESULT Disp::GetCORSystemDirectory(            // Return code.
     __out_ecount (cchBuffer) LPWSTR szBuffer,  // [out] Buffer for the directory name
     DWORD      cchBuffer,                      // [in] Size of the buffer
     DWORD      *pchBuffer)                     // [OUT] Number of characters returned
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

#ifndef DACCESS_COMPILE
    //
    hr = ::GetCORSystemDirectory(szBuffer, cchBuffer, pchBuffer);
#else
    DacNotImpl();
    hr = E_NOTIMPL;
#endif // #ifndef DACCESS_COMPILE
    END_ENTRYPOINT_NOTHROW;

    return hr;
} // HRESULT Disp::GetCORSystemDirectory()


HRESULT Disp::FindAssembly(             // S_OK or error
    LPCWSTR     szAppBase,              // [IN] optional - can be NULL
    LPCWSTR     szPrivateBin,           // [IN] optional - can be NULL
    LPCWSTR     szGlobalBin,            // [IN] optional - can be NULL
    LPCWSTR     szAssemblyName,         // [IN] required - this is the assembly you are requesting
    LPCWSTR     szName,                 // [OUT] buffer - to hold name 
    ULONG       cchName,                // [IN] the name buffer's size
    ULONG       *pcName)                // [OUT] the number of characters returend in the buffer
{
    BEGIN_ENTRYPOINT_NOTHROW;
    END_ENTRYPOINT_NOTHROW;

    return E_NOTIMPL;
} // HRESULT Disp::FindAssembly()

HRESULT Disp::FindAssemblyModule(           // S_OK or error
    LPCWSTR     szAppBase,                  // [IN] optional - can be NULL
    LPCWSTR     szPrivateBin,               // [IN] optional - can be NULL
    LPCWSTR     szGlobalBin,                // [IN] optional - can be NULL
    LPCWSTR     szAssemblyName,             // [IN] The assembly name or code base of the assembly
    LPCWSTR     szModuleName,               // [IN] required - the name of the module
    __out_ecount (cchName) LPWSTR  szName,  // [OUT] buffer - to hold name 
    ULONG       cchName,                    // [IN]  the name buffer's size
    ULONG       *pcName)                    // [OUT] the number of characters returend in the buffer
{
    BEGIN_ENTRYPOINT_NOTHROW;
    END_ENTRYPOINT_NOTHROW;

    return E_NOTIMPL;
} // HRESULT Disp::FindAssemblyModule()

//*****************************************************************************
// Open a scope on an ITypeInfo
//*****************************************************************************
HRESULT Disp::OpenScopeOnITypeInfo(     // Return code.
    ITypeInfo   *pITI,                  // [in] ITypeInfo to open.
    DWORD       dwOpenFlags,            // [in] Open mode flags.
    REFIID      riid,                   // [in] The interface desired.
    IUnknown    **ppIUnk)               // [out] Return interface on success.
{
    BEGIN_ENTRYPOINT_NOTHROW;
    END_ENTRYPOINT_NOTHROW;

    return E_NOTIMPL;
} // HRESULT Disp::OpenScopeOnITypeInfo()


//*****************************************************************************
// IUnknown
//*****************************************************************************

ULONG Disp::AddRef()
{
    return InterlockedIncrement(&m_cRef);
} // ULONG Disp::AddRef()

ULONG Disp::Release()
{
    ULONG   cRef = InterlockedDecrement(&m_cRef);
    if (!cRef)
        delete this;
    return (cRef);
} // ULONG Disp::Release()

HRESULT Disp::QueryInterface(REFIID riid, void **ppUnk)
{
    *ppUnk = 0;

    if (riid == IID_IUnknown)
        *ppUnk = (IUnknown *) (IMetaDataDispenser *) this;
    else if (riid == IID_IMetaDataDispenser)
        *ppUnk = (IMetaDataDispenser *) this;
    else if (riid == IID_IMetaDataDispenserEx)
        *ppUnk = (IMetaDataDispenserEx *) this;
    else
        return (E_NOINTERFACE);
    AddRef();
    return (S_OK);
} // HRESULT Disp::QueryInterface()


//*****************************************************************************
// Called by the class factory template to create a new instance of this object.
//*****************************************************************************
HRESULT Disp::CreateObject(REFIID riid, void **ppUnk)
{ 
    HRESULT     hr;
    Disp *pDisp = new (nothrow) Disp();

    if (pDisp == 0)
        return (E_OUTOFMEMORY);

    hr = pDisp->QueryInterface(riid, ppUnk);
    if (FAILED(hr))
        delete pDisp;
    return (hr);
} // HRESULT Disp::CreateObject()

//*****************************************************************************
// This routine provides the user a way to set certain properties on the
// Dispenser.
//*****************************************************************************
HRESULT Disp::SetOption(                // Return code.
    REFGUID     optionid,               // [in] GUID for the option to be set.
    const VARIANT *pvalue)              // [in] Value to which the option is to be set.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    LOG((LF_METADATA, LL_INFO10, "Disp::SetOption(0x%08x, 0x%08x)\n", optionid, pvalue));

    if (optionid == MetaDataCheckDuplicatesFor)
    {
        if (V_VT(pvalue) != VT_UI4)
        {
            _ASSERTE(!"Invalid Variant Type value!");
            IfFailGo(E_INVALIDARG);
        }
        m_OptionValue.m_DupCheck = (CorCheckDuplicatesFor) V_UI4(pvalue);
    }
    else if (optionid == MetaDataRefToDefCheck)
    {
        if (V_VT(pvalue) != VT_UI4)
        {
            _ASSERTE(!"Invalid Variant Type value!");
            IfFailGo(E_INVALIDARG);
        }
        m_OptionValue.m_RefToDefCheck = (CorRefToDefCheck) V_UI4(pvalue);
    }
    else if (optionid == MetaDataNotificationForTokenMovement)
    {
        if (V_VT(pvalue) != VT_UI4)
        {
            _ASSERTE(!"Invalid Variant Type value!");
            IfFailGo(E_INVALIDARG);
        }
        m_OptionValue.m_NotifyRemap = (CorNotificationForTokenMovement)V_UI4(pvalue);
    }
    else if (optionid == MetaDataSetENC)
    {
        if (V_VT(pvalue) != VT_UI4)
        {
            _ASSERTE(!"Invalid Variant Type value!");
            IfFailGo(E_INVALIDARG);
        }
        m_OptionValue.m_UpdateMode = V_UI4(pvalue);
    }
    else if (optionid == MetaDataErrorIfEmitOutOfOrder)
    {
        if (V_VT(pvalue) != VT_UI4)
        {
            _ASSERTE(!"Invalid Variant Type value!");
            IfFailGo(E_INVALIDARG);
        }
        m_OptionValue.m_ErrorIfEmitOutOfOrder = (CorErrorIfEmitOutOfOrder) V_UI4(pvalue);
    }
    else if (optionid == MetaDataImportOption)
    {
        if (V_VT(pvalue) != VT_UI4)
        {
            _ASSERTE(!"Invalid Variant Type value!");
            IfFailGo(E_INVALIDARG);
        }
        m_OptionValue.m_ImportOption = (CorImportOptions) V_UI4(pvalue);
    }
    else if (optionid == MetaDataThreadSafetyOptions)
    {
        if (V_VT(pvalue) != VT_UI4)
        {
            _ASSERTE(!"Invalid Variant Type value!");
            IfFailGo(E_INVALIDARG);
        }
        m_OptionValue.m_ThreadSafetyOptions = (CorThreadSafetyOptions) V_UI4(pvalue);
    }
    else if (optionid == MetaDataGenerateTCEAdapters)
    {
        if (V_VT(pvalue) != VT_BOOL)
        {
            _ASSERTE(!"Invalid Variant Type value!");
            IfFailGo(E_INVALIDARG);
        }
        m_OptionValue.m_GenerateTCEAdapters = V_BOOL(pvalue);
    }
    else if (optionid == MetaDataTypeLibImportNamespace)
    {
        if (V_VT(pvalue) != VT_BSTR && V_VT(pvalue) != VT_EMPTY && V_VT(pvalue) != VT_NULL)
        {
            _ASSERTE(!"Invalid Variant Type value for namespace.");
            IfFailGo(E_INVALIDARG);
        }
        if (m_Namespace)
            delete [] m_Namespace;
        if (V_VT(pvalue) == VT_EMPTY || V_VT(pvalue) == VT_NULL || ::SysStringLen(V_BSTR(pvalue)) == 0 || *V_BSTR(pvalue) == 0)
            m_Namespace = 0;
        else
        {
            m_Namespace = new (nothrow) WCHAR[1 + wcslen(V_BSTR(pvalue))];
            IfNullGo(m_Namespace);
            wcscpy_s(m_Namespace, 1 + wcslen(V_BSTR(pvalue)), V_BSTR(pvalue));
        }
    }
    else if (optionid == MetaDataLinkerOptions)
    {
        if (V_VT(pvalue) != VT_UI4)
        {
            _ASSERTE(!"Invalid Variant Type value!");
            IfFailGo(E_INVALIDARG);
        }
        m_OptionValue.m_LinkerOption = (CorLinkerOptions) V_UI4(pvalue);
    }
    else if (optionid == MetaDataRuntimeVersion)
    {
        if (V_VT(pvalue) != VT_BSTR && V_VT(pvalue) != VT_EMPTY && V_VT(pvalue) != VT_NULL)
        {
            _ASSERTE(!"Invalid Variant Type value for version.");
            IfFailGo(E_INVALIDARG);
        }
        if (m_OptionValue.m_RuntimeVersion)
            delete [] m_OptionValue.m_RuntimeVersion;

        if (V_VT(pvalue) == VT_EMPTY || V_VT(pvalue) == VT_NULL || ::SysStringLen(V_BSTR(pvalue)) == 0 || *V_BSTR(pvalue) == 0)
            m_OptionValue.m_RuntimeVersion = NULL;
        else {
            INT32 len = WszWideCharToMultiByte(CP_UTF8, 0, V_BSTR(pvalue), -1, NULL, 0, NULL, NULL);
            m_OptionValue.m_RuntimeVersion = new (nothrow) char[len];
            if (m_OptionValue.m_RuntimeVersion == NULL)
            IfFailGo(E_INVALIDARG);
            WszWideCharToMultiByte(CP_UTF8, 0, V_BSTR(pvalue), -1, m_OptionValue.m_RuntimeVersion, len, NULL, NULL);
        }
    }

    else if (optionid == MetaDataMergerOptions)
    {
        if (V_VT(pvalue) != VT_UI4)
        {
            _ASSERTE(!"Invalid Variant Type value!");
            IfFailGo(E_INVALIDARG);
        }
        m_OptionValue.m_MergeOptions = (MergeFlags) V_UI4(pvalue);
    }
    
    else
    {
        _ASSERTE(!"Invalid GUID");
        IfFailGo(E_INVALIDARG);
    }

ErrExit:

    END_ENTRYPOINT_NOTHROW;
    return hr;
} // HRESULT Disp::SetOption()

//*****************************************************************************
// This routine provides the user a way to set certain properties on the
// Dispenser.
//*****************************************************************************
HRESULT Disp::GetOption(                // Return code.
    REFGUID     optionid,               // [in] GUID for the option to be set.
    VARIANT *pvalue)                    // [out] Value to which the option is currently set.
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    LOG((LF_METADATA, LL_INFO10, "Disp::GetOption(0x%08x, 0x%08x)\n", optionid, pvalue));

    _ASSERTE(pvalue);
    if (optionid == MetaDataCheckDuplicatesFor)
    {
        V_VT(pvalue) = VT_UI4;
        V_UI4(pvalue) = m_OptionValue.m_DupCheck;
    }
    else if (optionid == MetaDataRefToDefCheck)
    {
        V_VT(pvalue) = VT_UI4;
        V_UI4(pvalue) = m_OptionValue.m_RefToDefCheck;
    }
    else if (optionid == MetaDataNotificationForTokenMovement)
    {
        V_VT(pvalue) = VT_UI4;
        V_UI4(pvalue) = m_OptionValue.m_NotifyRemap;
    }
    else if (optionid == MetaDataSetENC)
    {
        V_VT(pvalue) = VT_UI4;
        V_UI4(pvalue) = m_OptionValue.m_UpdateMode;
    }
    else if (optionid == MetaDataErrorIfEmitOutOfOrder)
    {
        V_VT(pvalue) = VT_UI4;
        V_UI4(pvalue) = m_OptionValue.m_ErrorIfEmitOutOfOrder;
    }
    else if (optionid == MetaDataGenerateTCEAdapters)
    {
        V_VT(pvalue) = VT_BOOL;
        V_BOOL(pvalue) = m_OptionValue.m_GenerateTCEAdapters;
    }
    else if (optionid == MetaDataLinkerOptions)
    {
        V_VT(pvalue) = VT_BOOL;
        V_UI4(pvalue) = m_OptionValue.m_LinkerOption;
    }
    else
    {
        _ASSERTE(!"Invalid GUID");
        IfFailGo(E_INVALIDARG);
    }
ErrExit:
    END_ENTRYPOINT_NOTHROW;

    return hr;
} // HRESULT Disp::GetOption()


//*****************************************************************************
// Process attach initialization.
//*****************************************************************************
static DWORD LoadedModulesReadWriteLock[sizeof(UTSemReadWrite)/sizeof(DWORD) + sizeof(DWORD)];
void InitMd()
{
    LOADEDMODULES::m_pSemReadWrite = new ((void*)LoadedModulesReadWriteLock) UTSemReadWrite;
} // void InitMd()

//*****************************************************************************
// Process attach cleanup.
//*****************************************************************************
void UninitMd()
{
    if (LOADEDMODULES::m_pSemReadWrite)
    {
        LOADEDMODULES::m_pSemReadWrite->~UTSemReadWrite();
        LOADEDMODULES::m_pSemReadWrite = 0;
    }
} // void UninitMd()


// EOF

