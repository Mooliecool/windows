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
// ===========================================================================
// File: inputset.cpp
//
// ===========================================================================

#include "stdafx.h"

#if _MSC_VER >= 1301
#pragma warning(push)
#pragma warning(disable:4345) // behavior change: a POD object constructed with the initializer of the form () will be default-initialized
#endif

////////////////////////////////////////////////////////////////////////////////
// CInputSet::CInputSet

CInputSet::CInputSet () :
    m_pController(NULL),
    m_tableSources(NULL),
    m_pSrcHead(NULL),
    m_tableResources(NULL),
    m_pResHead(NULL),
    m_fDLL(FALSE),
    m_fNoOutput(FALSE),
    m_fWinApp(FALSE),
    m_fAssemble(TRUE),
    m_ImageBase(0),
    m_dwFileAlign(0),
    m_pNext(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::~CInputSet

CInputSet::~CInputSet ()
{
    if (m_pController != NULL)
        m_pController->Release();
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::Initialize

HRESULT CInputSet::Initialize (CController *pController)
{
    m_pController = pController;
    pController->AddRef();      // Keep a ref on the controller...

    m_tableSources.SetNameTable (pController->GetNameMgr());
    m_tableResources.SetNameTable (pController->GetNameMgr());
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::GetCompiler

STDMETHODIMP CInputSet::GetCompiler (ICSCompiler **ppCompiler)
{
    ASSERT (m_pController != NULL);

    *ppCompiler = m_pController;
    m_pController->AddRef();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::AddSourceFile

STDMETHODIMP CInputSet::AddSourceFile (PCWSTR pszFileName, FILETIME* pFT)
{
    HRESULT hr;
    DWORD_PTR dwCookie;

    if (wcslen(pszFileName) >= MAX_PATH)
        return HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);

    // See if this source file is already there
    if (FAILED (hr = m_tableSources.FindOrAddNoCase (pszFileName, true/*fIsFileName*/, &dwCookie)))
        return hr;

    if (hr == S_OK)
    {
        // Was already there.  Update the file time if we were given one.
        if (pFT != NULL) 
        {
            CSourceFileInfo *pSrc = m_tableSources.GetData (dwCookie);
            pSrc->m_ft = *pFT;
        }

        return S_FALSE;
    }

    // Create a new source info object to store in the table...
    CSourceFileInfo *pSrcInfo = new CSourceFileInfo();
    if (pSrcInfo == NULL)
        return E_OUTOFMEMORY;
    if (FAILED (hr = m_tableSources.AddName (pszFileName, &pSrcInfo->m_pName)))
        return hr;
    if (pFT != NULL)
        pSrcInfo->m_ft = *pFT;

    m_tableSources.SetData (dwCookie, pSrcInfo);

    // Add it into the linked list to preserve ordering
    pSrcInfo->m_pNext = m_pSrcHead;
    m_pSrcHead = pSrcInfo;

    // If we are generating an output (as far as we know) and don't have a name
    // for it established yet, tell the compiler to pick one
    if (!m_fNoOutput && (m_sbstrOutputName == NULL))
    {
        m_sbstrOutputName.Attach (SysAllocString (L"?"));
        if (m_sbstrOutputName == NULL)
            return E_OUTOFMEMORY;
    }

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::RemoveSourceFile

STDMETHODIMP CInputSet::RemoveSourceFile (PCWSTR pszFileName)
{
    DWORD_PTR dwCookie = 0;
    // See if this source file is already there
    if (FAILED (m_tableSources.FindNoCase (pszFileName, true/*fIsFileName*/, &dwCookie)))
        return S_FALSE;

    ASSERT(m_pSrcHead != NULL);
    CSourceFileInfo *pSrc = m_tableSources.GetData (dwCookie);
    
#ifdef _DEBUG
    bool bFound = false;
#endif // _DEBUG
    for (CSourceFileInfo **ppCurrent = &m_pSrcHead;
         *ppCurrent != NULL; ppCurrent = &(*ppCurrent)->m_pNext) {
        if (*ppCurrent == pSrc) {
            // Remove it from the list
            *ppCurrent = pSrc->m_pNext;
            pSrc->m_pNext = NULL;
#ifdef _DEBUG
            bFound = true;
#endif // _DEBUG
            break;
        }
    }
    ASSERT(bFound == true);

    m_tableSources.Remove (dwCookie);
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

BOOL CInputSet::CopySources(CSourceFileInfo **ppSrcFiles, size_t iCount)
{
    CSourceFileInfo* pCurrent = m_pSrcHead;
    size_t i = m_tableSources.Count();

    if (i != iCount)
        return FALSE;
    
    // Put the sources in the array backwards because
    // The list has them linked backwards
    while (i > 0 && pCurrent != NULL)
        pCurrent = (ppSrcFiles[--i] = pCurrent)->m_pNext;
    ASSERT(pCurrent == NULL && i == 0);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::RemoveAllSourceFiles

STDMETHODIMP CInputSet::RemoveAllSourceFiles ()
{
    m_tableSources.RemoveAll ();
    m_pSrcHead = NULL;
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::AddResourceFile

STDMETHODIMP CInputSet::AddResourceFile (PCWSTR pszFileName, PCWSTR pszIdent, BOOL bEmbed, BOOL bVis)
{
    HRESULT hr;

    if (wcslen(pszFileName) >= MAX_PATH)
        return HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);

    // Create a new source info object to store in the table...
    CResourceFileInfo *pResInfo = new CResourceFileInfo();
    if (pResInfo == NULL)
        return E_OUTOFMEMORY;
    pResInfo->m_fEmbed = bEmbed;
    pResInfo->m_fVisible = bVis;

    IfFailGo(m_tableResources.AddName (pszFileName, &pResInfo->m_pFileName));
    IfFailGo(m_tableResources.AddName (pszIdent, &pResInfo->m_pIdent));
    IfFailGo(m_tableResources.Add ( pszIdent, pResInfo ));

    // S_FALSE means the identity already exists
    if (hr == S_FALSE)
        goto ErrExit;

    // Add it into the linked list to preserve ordering
    pResInfo->m_pNext = m_pResHead;
    m_pResHead = pResInfo;

    return hr;

ErrExit:
    if (pResInfo)
        delete pResInfo;

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::RemoveResourceFile

STDMETHODIMP CInputSet::RemoveResourceFile (PCWSTR pszFileName, PCWSTR pszIdent, BOOL bEmbed, BOOL bVis)
{
    DWORD_PTR dwCookie = 0;
    // See if this source file is already there
    if (FAILED (m_tableResources.Find(pszIdent, &dwCookie)))
        return S_FALSE;

    ASSERT(m_pResHead != NULL);
    CResourceFileInfo *pSrc = m_tableResources.GetData (dwCookie);
    
#ifdef _DEBUG
    bool bFound = false;
#endif // _DEBUG
    for (CResourceFileInfo **ppCurrent = &m_pResHead;
         *ppCurrent != NULL; ppCurrent = &(*ppCurrent)->m_pNext) {
        if (*ppCurrent == pSrc) {
            // Remove it from the list
            *ppCurrent = pSrc->m_pNext;
            pSrc->m_pNext = NULL;
#ifdef _DEBUG
            bFound = true;
#endif // _DEBUG
            break;
        }
    }
    ASSERT(bFound == true);

    m_tableResources.Remove (dwCookie);
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::CopyResources

BOOL CInputSet::CopyResources(CResourceFileInfo **ppResFiles, size_t iCount)
{
    CResourceFileInfo* pCurrent = m_pResHead;
    size_t i = m_tableResources.Count();

    if (i != iCount)
        return FALSE;
    
    // Put the sources in the array backwards because
    // The list has them linked backwards
    while (i > 0 && pCurrent != NULL)
        pCurrent = (ppResFiles[--i] = pCurrent)->m_pNext;
    ASSERT(pCurrent == NULL && i == 0);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CInputSet::GetWin32Resource(PCWSTR* ppzsFileName) 
{
    if (m_sbstrResourceFile == NULL) 
        return E_FAIL;

    *ppzsFileName = m_pController->GetNameMgr()->AddString(m_sbstrResourceFile)->text;
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CInputSet::SetWin32Resource (PCWSTR pszFileName)
{
    m_sbstrResourceFile.Empty();
    if (pszFileName == NULL)
        return S_OK;

    if (m_sbstrIconFile != NULL)
        return E_INVALIDARG;

    if (wcslen(pszFileName) >= MAX_PATH)
        return HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
    
    m_sbstrResourceFile.Attach (SysAllocString (pszFileName));
    return m_sbstrResourceFile == NULL ? E_OUTOFMEMORY : S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::SetOutputFileName

STDMETHODIMP CInputSet::SetOutputFileName (PCWSTR pszFileName)
{
    if (pszFileName != NULL && wcslen(pszFileName) >= MAX_PATH)
        return HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
    
    m_sbstrOutputName.Empty();
    if (pszFileName == NULL || pszFileName[0] == 0)
    {
        m_fNoOutput = TRUE;
        return S_OK;
    }

    m_fNoOutput = FALSE;
    m_sbstrOutputName.Attach (SysAllocString (pszFileName));
    return m_sbstrOutputName == NULL ? E_OUTOFMEMORY : S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::SetOutputFileType

STDMETHODIMP CInputSet::SetOutputFileType (DWORD dwFileType)
{
    switch(dwFileType) {
    case OUTPUT_CONSOLE:
        m_fAssemble = TRUE;
        m_fDLL = FALSE;
        m_fWinApp = FALSE;
        break;
    case OUTPUT_WINDOWS:
        m_fAssemble = TRUE;
        m_fDLL = FALSE;
        m_fWinApp = TRUE;
        break;
    case OUTPUT_LIBRARY:
        m_fAssemble = TRUE;
        m_fDLL = TRUE;
        m_fWinApp = FALSE;
        break;
    case OUTPUT_MODULE:
        m_fAssemble = FALSE;
        m_fDLL = TRUE;
        m_fWinApp = FALSE;
        break;
    default:
        return E_INVALIDARG;
    }
    if (m_fDLL == TRUE && m_sbstrMainClass != NULL)
        return S_FALSE;
    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
// CInputSet::SetImageBase2

STDMETHODIMP CInputSet::SetImageBase2 (ULONGLONG ImageBase)
{
    m_ImageBase = ImageBase;
    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
// CInputSet::SetFileAlignment

STDMETHODIMP CInputSet::SetFileAlignment (DWORD dwAlign)
{
    DWORD temp = (dwAlign >> 9);
    if (dwAlign < 0x0000200 || dwAlign > 0x0002000)
        return E_INVALIDARG;
    while ((temp & 0x0001) == 0)
        temp >>= 1;
    if (temp != 1)
        return E_INVALIDARG;
    m_dwFileAlign = dwAlign;
    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
// CInputSet::SetMainClass

STDMETHODIMP CInputSet::SetMainClass (PCWSTR pszFQClassName)
{
    m_sbstrMainClass.Empty();
    if (pszFQClassName == NULL)
        return S_OK;

    if (m_fDLL)
        return E_INVALIDARG;

    m_sbstrMainClass.Attach (SysAllocString (pszFQClassName));
    return m_sbstrMainClass == NULL ? E_OUTOFMEMORY : S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::SetWin32Icon

STDMETHODIMP CInputSet::SetWin32Icon (PCWSTR pszIconFileName)
{
    m_sbstrIconFile.Empty();
    if (pszIconFileName == NULL)
        return S_OK;

    if (m_sbstrResourceFile != NULL)
        return E_INVALIDARG;

    if (wcslen(pszIconFileName) >= MAX_PATH)
        return HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
    
    m_sbstrIconFile.Attach (SysAllocString (pszIconFileName));
    return m_sbstrIconFile == NULL ? E_OUTOFMEMORY : S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CInputSet::SetPDBFileName

STDMETHODIMP CInputSet::SetPDBFileName (PCWSTR pszFileName)
{
    m_sbstrPDBFile.Empty();
    if (pszFileName == NULL)
        return S_OK;

    if (wcslen(pszFileName) >= MAX_PATH)
        return HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
    
    m_sbstrPDBFile.Attach (SysAllocString (pszFileName));
    return m_sbstrPDBFile == NULL ? E_OUTOFMEMORY : S_OK;
}
