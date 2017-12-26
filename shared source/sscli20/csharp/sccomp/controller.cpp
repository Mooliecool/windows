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
// File: controller.cpp
//
// ===========================================================================

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// CController::CController

#if defined(_MSC_VER)
#pragma warning(disable:4355)  // allow "this" in member initializer list
#endif  // defined(_MSC_VER)
CController::CController () :
    m_pNameMgr(NULL),
    m_pPageHeap(NULL),
    m_StdHeap(this),
    m_pInputSets(NULL),
    m_ppNextInputSet(&m_pInputSets),
    m_pCompilerErrors(NULL),
    m_iErrorsReported(0),
    m_iWarnAsErrorsReported(0)
{
    m_fSuppressErrors = false;
}
#if defined(_MSC_VER)
#pragma warning(default:4355)
#endif  // defined(_MSC_VER)

////////////////////////////////////////////////////////////////////////////////
// CController::~CController

CController::~CController ()
{
    if (m_pPageHeap != NULL)
        m_pPageHeap->DecommitUnusedPages();
    m_StdHeap.FreeHeap (TRUE);

    // The name managed owns the page heap, so this releasing it must happen after any use of m_pPageHeap.
    m_pPageHeap = NULL;
    if (m_pNameMgr != NULL)
        m_pNameMgr->Release();

    ASSERT (m_pInputSets == NULL);      // Input sets have referenced pointer to us!!!
    ASSERT (m_pCompilerErrors == NULL); // Should have been already released!
}

////////////////////////////////////////////////////////////////////////////////
// CController::Initialize

HRESULT CController::Initialize (DWORD dwFlags, ICSCompilerHost *pHost, ICSNameTable *pNameTable)
{
    if (pHost == NULL || pNameTable == NULL)
        return E_POINTER;

    m_spHost = pHost;
    m_dwFlags = dwFlags;
    HRESULT hr = pNameTable->QueryInterface (IID_ICSPrivateNameTable, (void **)&m_pNameMgr);
    if (FAILED(hr))
        return hr;
        
    // Initialize page heap from the name table.
    m_pPageHeap = m_pNameMgr->GetPageHeap();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CController::CloneOptions

void CController::CloneOptions(COptionData & options)
{
    CTinyGate gate(&m_lockOptions);
    options = m_OptionData;
}

////////////////////////////////////////////////////////////////////////////////
// CController::CloneCoreOptions

void CController::CloneCoreOptions(CCoreOptionData & options)
{
    CTinyGate gate(&m_lockOptions);
    options = m_OptionData;
}

////////////////////////////////////////////////////////////////////////////////
// CController::CreateSourceModule

STDMETHODIMP CController::CreateSourceModule (ICSSourceText *pText, ICSSourceModule **ppModule)
{
    HRESULT     hr = E_FAIL;

    PAL_TRY
    {
        PAL_TRY
        {
            hr = CSourceModule::CreateInstance (this, pText, ppModule, 0);
        }
        EXCEPT_NON_COMPILER_EXCEPTION
        {
            CLEANUP_STACK // handle stack overflow
            GetHost()->OnCatastrophicError (TRUE, GetExceptionCode(), (LPVOID) CSourceModule::CreateInstance);
            hr = E_UNEXPECTED;
        }
        PAL_ENDTRY
    }
    EXCEPT_EXCEPTION
    {
        CLEANUP_STACK // handle stack overflow
        if (GetExceptionCode() == FATAL_EXCEPTION_CODE)
            hr = E_FAIL;
        else
            hr = E_UNEXPECTED;
    }
    PAL_ENDTRY

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CController::GetNameTable

STDMETHODIMP CController::GetNameTable (ICSNameTable **ppNameTable)
{
    if (m_pNameMgr == NULL)
    {
        VSFAIL ("How can a controller not have a name table?!?");
        return E_UNEXPECTED;
    }

    *ppNameTable = m_pNameMgr;
    m_pNameMgr->AddRef();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CController::Shutdown

STDMETHODIMP CController::Shutdown ()
{
    // Release all input sets.
    while (m_pInputSets != NULL)
    {
        CInputSet   *pTmp = m_pInputSets->m_pNext;
        m_pInputSets->Release();
        m_pInputSets = pTmp;
    }

    // Release the host
    m_spHost.Release();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CController::GetConfiguration

STDMETHODIMP CController::GetConfiguration (ICSCompilerConfig **ppEnum)
{
    HRESULT hr=E_OUTOFMEMORY;
    CComObject<CCompilerConfig> *pObj;

    if (SUCCEEDED (hr = CComObject<CCompilerConfig>::CreateInstance (&pObj)))
    {
        CTinyGate gate(&m_lockOptions);

        if (FAILED (hr = pObj->Initialize (this, &m_OptionData)) ||
            FAILED (hr = pObj->QueryInterface (IID_ICSCompilerConfig, (void **)ppEnum)))
        {
            delete pObj;
        }
    }
    
    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CController::AddInputSet

STDMETHODIMP CController::AddInputSet (ICSInputSet **ppInputSet)
{
    HRESULT                 hr = E_OUTOFMEMORY;
    CComObject<CInputSet>   *pObj;

    if (SUCCEEDED (hr = CComObject<CInputSet>::CreateInstance (&pObj)))
    {
        if (FAILED (hr = pObj->Initialize (this)) ||
            FAILED (hr = pObj->QueryInterface (IID_ICSInputSet, (void **)ppInputSet)))
        {
            delete pObj;
        }
    }

    if (SUCCEEDED (hr))
    {
        // Add this set to the list
        *m_ppNextInputSet = pObj;
        m_ppNextInputSet = &pObj->m_pNext;
        pObj->AddRef();
    }
    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CController::RemoveInputSet

STDMETHODIMP CController::RemoveInputSet (ICSInputSet *pInputSet)
{
    return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////////
// CController::Compile

STDMETHODIMP CController::Compile (ICSCompileProgress *pProgress)
{

    HRESULT     hr = RunCompiler(pProgress, NULL);
    Cleanup();
    return SUCCEEDED (hr) ? S_OK : S_FALSE;
}

HRESULT CController::Cleanup()
{
    // If we still have an error container, get rid of it
    if (m_pCompilerErrors != NULL)
    {
        // The container will have already been sent back to the host
        m_pCompilerErrors->Release();
        m_pCompilerErrors = NULL;
    }
    
    // Decommit any unused memory for efficiency.
    m_pPageHeap->DecommitUnusedPages();

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT CController::RunCompiler(ICSCompileProgress *pProgress, COMPILER **ppCompiler, ICompileCallback* pBindCallback)
{
    // Reset the error count here in case there's a problem
    // with one of the input-sets or imports
    m_iErrorsReported = 0;
    m_iWarnAsErrorsReported = 0;

    // Instantiate a compiler on the stack
    COMPILER    compiler (this, * m_pPageHeap, m_pNameMgr);   
    COMPILER    *pCompiler = &compiler;

    if (ppCompiler != NULL)
    {
        pCompiler = *ppCompiler = vs_new COMPILER(this, * m_pPageHeap, m_pNameMgr);
        if (pCompiler == NULL)
            return E_OUTOFMEMORY;
    }

    HRESULT     hr = S_OK;

    // Initialize it
    if (SUCCEEDED (hr = pCompiler->Init())) 
    {
        // For each input set, add the appropriate data to the compiler
        for (CInputSet *pSet = m_pInputSets; pSet != NULL; pSet = pSet->m_pNext)
            if (FAILED (hr = pCompiler->AddInputSet (pSet)))
                break;
    }

    // Tell it to compile
    if (SUCCEEDED(hr) && m_iErrorsReported == 0)
    {
        pCompiler->compileCallback.Init(pBindCallback);
        if (ppCompiler)
        {
            bool fNeedsCleanUp;
            hr = pCompiler->Compile(pProgress, &fNeedsCleanUp);
            if (!fNeedsCleanUp) {
                // Something very bad happened so the compiler has totally shut down.
                ASSERT(FAILED(hr));
                *ppCompiler = NULL;
                delete pCompiler;
            }
        }
        else
        {
            hr = pCompiler->Compile(pProgress, NULL);
        }
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CController::BuildForEnc
STDMETHODIMP CController::BuildForEnc(ICSCompileProgress *pProgress, ICSEncProjectServices *pEncService, IUnknown * punkPE)
{
    // Reset the error count here in case there's a problem
    // with one of the input-sets or imports
    m_iErrorsReported = 0;

    COMPILER compiler(this, * m_pPageHeap, m_pNameMgr);

    HRESULT     hr = S_OK;

    // Initialize it
    if (SUCCEEDED (hr = compiler.Init())) {
        // For each input set, add the appropriate data to the compiler
        for (CInputSet *pSet = m_pInputSets; pSet != NULL; pSet = pSet->m_pNext)
            if (FAILED(hr = compiler.AddInputSet(pSet)))
                break;
    }


    Cleanup();

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CController::GetOutputFileName

STDMETHODIMP CController::GetOutputFileName (PCWSTR *ppszFileName)
{
    // This doesn't handle multiple targets
    if (m_pInputSets == NULL || m_pInputSets->m_pNext != NULL)
        return E_FAIL;

    *ppszFileName = m_pInputSets->GetOutputName();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CController::CreateParser

STDMETHODIMP CController::CreateParser (ICSParser **ppParser)
{
    HRESULT                     hr = E_OUTOFMEMORY;
    CComObject<CTextParser>     *pObj;

    if (SUCCEEDED (hr = CComObject<CTextParser>::CreateInstance (&pObj)))
    {
        if (FAILED (hr = pObj->Initialize (this, m_OptionData.compatMode)) ||
            FAILED (hr = pObj->QueryInterface (IID_ICSParser, (void **)ppParser)))
        {
            delete pObj;
        }
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CController::NoMemory

__declspec(noreturn) void CController::NoMemory ()
{
    m_spHost->OnCatastrophicError(FALSE, 0, NULL);
    ALLOCHOST::ThrowOutOfMemoryException();
    UNREACHABLE();
}

////////////////////////////////////////////////////////////////////////////////
// CController::GetStandardHeap

MEMHEAP *CController::GetStandardHeap ()
{
    return &m_StdHeap;
}

////////////////////////////////////////////////////////////////////////////////
// CController::GetPageHeap

PAGEHEAP *CController::GetPageHeap ()
{
    return m_pPageHeap;
}


////////////////////////////////////////////////////////////////////////////////
// CController::HandleException

void CController::HandleException (DWORD dwException)
{
    // FATAL_EXCEPTION_CODE is thrown when the compiler hits a fatal error.  So,
    // an error message has already been produced -- we just do nothing.
    if (dwException == FATAL_EXCEPTION_CODE)
        return;


    // Attempt to notify the host about a catastrophic error.
    // Things might be is a very bad state, such that the following call can't
    // work. If another exception occurs while trying to inform the host,
    // just ignore it (better than crashing the process....)
    PAL_TRY {
        if (this != NULL && m_spHost != NULL) {
            m_spHost->OnCatastrophicError (TRUE, dwException, m_pExceptionAddr);
        }
    }
    PAL_EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
        // Do nothing!
    }
    PAL_ENDTRY
    
    m_iErrorsReported++; // We reported an error so stop the compilation
}

