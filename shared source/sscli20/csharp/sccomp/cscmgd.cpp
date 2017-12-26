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
// File: cscmgd.cpp
//
// Routines for in memory compilation.
// ===========================================================================

#include "stdafx.h"

/*
NB: the method below (InMemoryCompile) is used by vsproject
    so please coordinate any changes to the signature with whoever owns that file
*/

// can't use SEH in functions that require object unwinding
static void InMemoryCompileWorker(CInMemoryCompilerHost& host, ICSCompiler* compiler)
{
    PAL_TRY {
        compiler->Compile( NULL);
    } PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER ) {
        host.OnCatastrophicError(FALSE, 0, NULL);
    }
    PAL_ENDTRY
}

HRESULT STDMETHODCALLTYPE InMemoryCompile_Impl(LPCWSTR * pszText, const COMPILEPARAMS * params)
{
    HRESULT hr;

    int nText              = params->nText;
    LPCWSTR * pszNames     = params->pszNames;
    LPCWSTR pszTarget      = params->pszTarget;
    DWORD optCount         = params->optCount;
    LPCWSTR * pszOptions   = params->pszOptions;
    VARIANT * pValues      = params->pValues;
    ErrorReporter reporter = params->reporter;

    // Don't need to Initialize COM (done by the runtime)

    if (reporter == NULL)
        return E_INVALIDARG;

    // Initialize UNILIB
    W_IsUnicodeSystem();

    CComPtr<ICSCompilerFactory>  pFactory;
    CComPtr<ICSCompiler>         compiler;
    CComPtr<ICSCompilerConfig>   pConfig;
    CComPtr<ICSInputSet>         pInputSet;
    WCHAR                        szTargetPath[MAX_PATH];
    WCBuffer                     szTargetPathBuffer(szTargetPath);
    CInMemoryCompilerHost        host(reporter);

    if (S_OK != (hr = host.Init(nText, pszText, pszNames)))
        return hr;

    // Instanciate the compiler factory and create a compiler.  Let it
    // create its own name table.
    if (FAILED (hr = CreateCompilerFactory (&pFactory)) ||
        FAILED (hr = pFactory->CreateCompiler (0, &host, NULL, &compiler)) ||
        FAILED (hr = compiler->GetConfiguration (&pConfig)))
    {
        host.OnCatastrophicError(FALSE, 0, NULL);
        return hr;
    } else {
        host.SetCompiler (compiler);
        if (FAILED(hr = compiler->AddInputSet( &pInputSet)))
            goto FAIL;

        if (FAILED(hr = host.AddInputs( pInputSet)))
            goto FAIL;

        if (0 == GetCanonFilePath( pszTarget, szTargetPathBuffer, true)) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto FAIL;
        }
        if (FAILED(hr = pInputSet->SetOutputFileName( szTargetPath)))
            goto FAIL;
        if (S_OK == (hr = host.SetConfig( pConfig, pInputSet, optCount, pszOptions, pValues))) {
            InMemoryCompileWorker(host, compiler);
        }
FAIL:
        compiler->Shutdown();
    }

    return hr;
}

CInMemoryCompilerHost::CInMemoryCompilerHost (ErrorReporter reporter) : pCompiler(NULL), pSource(NULL), pNames(NULL), m_count(0), m_reporter(reporter)
{
}

HRESULT CInMemoryCompilerHost::Init (int count, LPCWSTR text [], LPCWSTR names [])
{
    HRESULT hr = S_OK;
    ASSERT(count > 0);

    // Initialize the messages DLL
    if (!hModuleMessages)
        hModuleMessages = GetMessageDll();

    if (!hModuleMessages ||
        0 == W_LoadString (hModuleMessages, IDS_RELATEDWARNING, m_szRelatedWarning, lengthof (m_szRelatedWarning)) ||
        0 == W_LoadString (hModuleMessages, IDS_RELATEDERROR, m_szRelatedError, lengthof (m_szRelatedError))) {
        VSFAIL("Failed to load strings");
        hr = m_reporter( NULL, 0, 0, ErrorNumberFromID(FTL_NoMessagesDLL), L"Unable to find messages file '" MESSAGE_DLLW L"'", ERROR_FATAL);
        return SUCCEEDED(hr) ? S_FALSE : hr;
    }

    pSource = vs_new ICSSourceText*[count];
    if (pSource != NULL)
        memset(pSource, 0, sizeof(ICSSourceText*) * count);

    pNames = new LPWSTR[count];
    if (pNames != NULL)
        memset(pNames, 0, sizeof(LPWSTR) * count);

    if (pNames == NULL || pSource == NULL)
        return E_OUTOFMEMORY;

    m_count = count;
    m_lastFound = 0;
    for (int i = 0; i < count; i++) {
        WCHAR fullname[_MAX_PATH];
        WCBuffer fullnameBuffer(fullname);
        int len = GetCanonFilePath( names[i], fullnameBuffer, false);
        if (len == 0) {
            if (*fullname == L'\0') {
                // This could easily fail because of an overflow, but that's OK
                // we only want what will fit in the output buffer so we can print
                // a good error message
                StringCchCopyW(fullname, lengthof(fullname) - 4, names[i]);
                StringCchCatW(fullname, lengthof(fullname), L"...");
            }
            Error(FTL_InputFileNameTooLong, fullname);
            hr = S_FALSE;
        } else {
            HRESULT hrInner;
            pNames[i] = new WCHAR[len + 1];
            if (pNames[i] == NULL)
                return E_OUTOFMEMORY;
            if(FAILED(hrInner = StringCchCopyW(pNames[i], len + 1, fullname))) {
                return hrInner;
            }
        }
        pSource[i] = new CInMemorySourceText(pNames[i],text[i]);
        if (pSource[i] == NULL)
            return E_OUTOFMEMORY;
        pSource[i]->AddRef();
    }

    return hr;
}

CInMemoryCompilerHost::~CInMemoryCompilerHost ()
{
    if (pSource != NULL) {
        for (int i = 0; i < m_count; i++) {
            if (pSource[i] != NULL) {
                pSource[i]->Release();
                pSource[i] = NULL;
            }
        }
        delete pSource;
        pSource = NULL;
    }
    if (pNames != NULL) {
        for (int i = 0; i < m_count; i++) {
            if (pNames[i] != NULL)  {
                delete pNames[i];
                pNames[i] = NULL;
            }
        }
        delete pNames;
        pNames = NULL;
    }
}


HRESULT CInMemoryCompilerHost::AddInputs(ICSInputSet *pInputSet)
{
    FILETIME                     ft;
    GetSystemTimeAsFileTime( &ft);

    for (int i = 0; i < m_count; i++) {
        HRESULT hr = pInputSet->AddSourceFile( pNames[i], &ft);
        if (FAILED(hr)) return hr;
    }
    return S_OK;
}

STDMETHODIMP CInMemoryCompilerHost::GetSourceModule (LPCWSTR pszFileName, BOOL fNeedChecksum, ICSSourceModule **ppModule)
{
    int probe = m_lastFound;
    for (int i = 0; i < m_count; ++i) {
        if (wcscmp(pszFileName, pNames[probe]) == 0) {
            m_lastFound = probe;
    	    return pCompiler->CreateSourceModule (pSource[probe], ppModule);
    	}
    	else {
    	    ++probe;
    	    if (probe >= m_count)
    	        probe -= m_count;
    	}
    }
    
    ASSERT(0); // didn't find the name; shouldn't happen. 
    return E_FAIL;       
}

STDMETHODIMP_(void) CInMemoryCompilerHost::OnCatastrophicError (BOOL fException, DWORD dwException, void *pAddress)
{
    WCHAR szFatal[1024];
    if (W_LoadString (hModuleMessages, IDS_NoMemory, szFatal, lengthof(szFatal)))
        m_reporter( NULL, 0, 0, ErrorNumberFromID(FTL_NoMemory), szFatal, ERROR_FATAL);
}

STDMETHODIMP_(void) CInMemoryCompilerHost::Error (HRESULT hr)
{
    const wchar_t * psz = (const wchar_t *)(INT_PTR)hr;
    ErrorArgs(FTL_InternalError, &psz);
}

STDMETHODIMP_(void) CInMemoryCompilerHost::ErrorArgs(int id, VarArgList args)
{
    HRESULT hr;
    LPCWSTR text;
    CComObject<CError> *err;
    long num = 0;
    ERRORKIND kind = ERROR_FATAL;

    if (SUCCEEDED(hr = CComObject<CError>::CreateInstance (&err)) &&
        SUCCEEDED(hr = err->Initialize(hModuleMessages, id, args)) &&
        SUCCEEDED(hr = err->GetErrorInfo( &num, &kind, &text)))
        m_reporter( NULL, 0, 0, num, text, kind);
    delete err;

    if (FAILED(hr))
        OnCatastrophicError( FALSE, 0, NULL);
}

STDMETHODIMP CInMemoryCompilerHost::ReportError (ICSError *err) {
    // report them to the managed host
    HRESULT hr = S_OK;
    POSDATA posStart, posEnd;
    ERRORKIND level;
    long errorNumber, LocCount;
    LPCWSTR errorMessage, sourceFile;

    if (FAILED(hr = err->GetErrorInfo (&errorNumber, &level, &errorMessage)))
        return hr;
    if (FAILED(hr = err->GetLocationCount (&LocCount)))
        return hr;
    if (LocCount > 0) {
        if (FAILED(hr = err->GetLocationAt (0, &sourceFile, &posStart, &posEnd)))
            return hr;
        if (!posStart.IsUninitialized()) {
            hr = m_reporter( sourceFile, posStart.iLine + 1, posStart.iChar + 1, errorNumber, errorMessage, level);
        } else {
            hr = m_reporter( sourceFile, 0, 0, errorNumber, errorMessage, level);
        }
    } else {
        return m_reporter( NULL, 0, 0, errorNumber, errorMessage, level);
    }
    if (LocCount == 1)
        return hr;

    if (level == ERROR_ERROR) {
        errorMessage = m_szRelatedError;
    } else {
        ASSERT (level == ERROR_WARNING);
        errorMessage = m_szRelatedWarning;
    }
    for (int j = 1; SUCCEEDED(hr) && j < LocCount; j++) {
        if (FAILED(hr = err->GetLocationAt (j, &sourceFile, &posStart, &posEnd)))
            return hr;
        if (!posStart.IsUninitialized()) {
            hr = m_reporter( sourceFile, posStart.iLine + 1, posStart.iChar + 1, errorNumber, errorMessage, level);
        } else {
            hr = m_reporter( sourceFile, 0, 0, errorNumber, errorMessage, level);
        }
    }
    return hr;
}

STDMETHODIMP_(HRESULT) CInMemoryCompilerHost::ReportErrors (ICSErrorContainer *errors) {
    // report them to the managed host
    HRESULT hr = S_OK;
    long cErrors = 0, i = 0;
    if (FAILED(hr = errors->GetErrorCount (NULL, NULL, NULL, &cErrors)))
        return hr;

    for (i = 0; i < cErrors; i++) {
        CComPtr<ICSError> err;

        if (FAILED(hr = errors->GetErrorAt (i, &err)) ||
            FAILED(hr = ReportError(err)))
            break;
    }
    return hr;
}

HRESULT ResHelper( VARIANT * pValue, ICSInputSet * pInputSet, bool bEmbed)
{
    HRESULT hr = S_OK;
    WCHAR szName[MAX_PATH], szIdent[MAX_PATH];
    WCBuffer szNameBuffer(szName);
    WCHAR * pszNext = NULL;

    if (V_VT (pValue) != VT_BSTR || V_BSTR (pValue) == NULL || V_BSTR (pValue)[0] == L'\0')
        return E_INVALIDARG;

    pszNext = wcschr( V_BSTR (pValue), L',');
    if (pszNext != NULL) {
        if (FAILED(hr = StringCchCopyNW(szName, lengthof(szName), V_BSTR (pValue), pszNext - V_BSTR (pValue))))
            return hr;
        if (0 == GetCanonFilePath(szName, szNameBuffer, false))
            return E_INVALIDARG;
        pszNext++;
    } else {
        if (0 == GetCanonFilePath(V_BSTR (pValue), szNameBuffer, false))
            return E_INVALIDARG;
    }

    if (pszNext == NULL || *pszNext == L'\0') {
        // Identifier defaults to filename minus path
        LPCWSTR pTemp = NULL;
        if ((pTemp = wcsrchr(szName, L'\\')) == NULL)
            hr = StringCchCopyW(szIdent, lengthof(szIdent), szName);
        else
            hr = StringCchCopyW(szIdent, lengthof(szIdent), pTemp + 1);
    } else {
        hr = StringCchCopyW(szIdent, lengthof(szIdent), pszNext);
    }
    if (FAILED(hr))
        return hr;

    if (FAILED (hr = pInputSet->AddResourceFile (szName, szIdent, bEmbed, true)))
        return hr;
    
    if (hr == S_FALSE) {
        // ShowErrorIdString( ERR_ResourceNotUnique, ERROR_ERROR, szIdent);
        return E_INVALIDARG;
    }

    return S_OK;
}

HRESULT CInMemoryCompilerHost::SetConfig(ICSCompilerConfig * pConfig, ICSInputSet * pInputSet, DWORD optCount, LPCWSTR pszOptions[], VARIANT pValues[])
{
    DWORD iOpt;
    LPCWSTR pszArg = NULL;
    long    iOptionId, iOptionCount;
    DWORD   dwFlags;
    HRESULT hr = S_OK;


    if (FAILED(hr = pConfig->GetOptionCount( &iOptionCount)))
        return hr;

    for (iOpt = 0; iOpt < optCount; iOpt ++) {
        pszArg = pszOptions[iOpt];

        if (_wcsicmp(pszArg, L"m") == 0) {
            if (V_VT(&pValues[iOpt]) != VT_BSTR || V_BSTR(&pValues[iOpt]) == NULL || V_BSTR(&pValues[iOpt])[0] == '\0')
                return E_INVALIDARG;

            if (FAILED(hr = pInputSet->SetMainClass( V_BSTR(&pValues[iOpt]))))
                return hr;
            continue;
        } else if (_wcsicmp(pszArg, L"target") == 0) {
            if (V_VT(&pValues[iOpt]) != VT_BSTR || V_BSTR(&pValues[iOpt]) == NULL || V_BSTR(&pValues[iOpt])[0] == '\0')
                return E_INVALIDARG;

            WCHAR * target = V_BSTR(&pValues[iOpt]);

            if (_wcsicmp(target, L"module") == 0) {
                if (FAILED(hr = pInputSet->SetOutputFileType(OUTPUT_MODULE)))
                    return hr;
            } else if (_wcsicmp(target, L"library") == 0) {
                if (FAILED(hr = pInputSet->SetOutputFileType(OUTPUT_LIBRARY)))
                    return hr;
            } else if (_wcsicmp(target, L"exe") == 0) {
                if (FAILED(hr = pInputSet->SetOutputFileType(OUTPUT_CONSOLE)))
                    return hr;
            } else if (_wcsicmp(target, L"winexe") == 0) {
                if (FAILED(hr = pInputSet->SetOutputFileType(OUTPUT_WINDOWS)))
                    return hr;
            } else 
                return E_INVALIDARG;

            continue;
        }
        else if (_wcsicmp(pszArg, L"baseaddress") == 0) {
            if (V_VT(&pValues[iOpt]) == VT_UI4 || V_VT(&pValues[iOpt]) == VT_I4)
                pInputSet->SetImageBase( (((DWORD)V_I4(&pValues[iOpt])) + 0x00008000) & 0xFFFF0000); // Round the low order word to align it
            else if (V_VT(&pValues[iOpt]) == VT_BSTR)
                pInputSet->SetImageBase ((wcstoul( V_BSTR(&pValues[iOpt]), NULL, 0) + 0x00008000) & 0xFFFF0000); // Round the low order word to align it
            else
                return E_INVALIDARG;
            
            continue;
        } 
        else if (_wcsicmp(pszArg, L"filealign") == 0) {
            DWORD val = 0;
            if (V_VT(&pValues[iOpt]) == VT_UI4 || V_VT(&pValues[iOpt]) == VT_I4)
                val = V_UI4(&pValues[iOpt]);
            else if (V_VT(&pValues[iOpt]) == VT_BSTR)
                val = wcstoul( V_BSTR(&pValues[iOpt]), NULL, 0);
            else
                return E_INVALIDARG;

            if (FAILED(hr = pInputSet->SetFileAlignment(val)))
                // This does the range checking
                return hr;
            
            continue;
        } else if (_wcsicmp (pszArg, L"res") == 0) {
            // Embedded resource
            if (FAILED(hr = ResHelper( pValues + iOpt, pInputSet, true)))
                return hr;
            continue;
        } else if (_wcsicmp (pszArg, L"linkres") == 0) {
            // Link in a resource
            if (FAILED(hr = ResHelper( pValues + iOpt, pInputSet, false)))
                return hr;
            continue;
        }


        // It isn't one we recognize, so it must be an option exposed by the compiler.
        // See if we can find it.
        long i;
        for (i=0; i<iOptionCount; i++)
        {
            PCWSTR  pszShortSwitch;
            PCWSTR pszLongSwitch;

            // Get the info for this switch
            if (FAILED (hr = pConfig->GetOptionInfoAtEx (i, &iOptionId, &pszShortSwitch, &pszLongSwitch, NULL, NULL, &dwFlags))) {
                return hr;
            }

            // See if it matches the arg we have
            if (_wcsicmp (pszArg, pszLongSwitch) == 0 || (pszShortSwitch && (_wcsicmp( pszArg, pszShortSwitch) == 0)))
            {
                // We have a match, make sure the value is correct
                if ((dwFlags & COF_BOOLEAN && V_VT(&pValues[iOpt]) != VT_BOOL) ||
                    (((dwFlags & COF_BOOLEAN) == 0) && (V_VT(&pValues[iOpt]) != VT_BSTR || V_BSTR(&pValues[iOpt]) == NULL || V_BSTR(&pValues[iOpt])[0] == '\0')))
                    return E_INVALIDARG;

                // Got the option -- call in to set it
                if (FAILED (hr = pConfig->SetOption (iOptionId, pValues[iOpt]))) {
                    return hr;
                }
                break;
            }
        }

        if (i == iOptionCount)
        {
            // Didn't find it...
            return E_INVALIDARG;
        }
    }

    CComPtr<ICSError>   spError;

    // Okay, we've successfully finished parsing options.  Now we must validate the settings.
    if (FAILED (hr = pConfig->CommitChanges (&spError)))
        return hr;

    if (hr == S_FALSE)
    {
        ASSERT (spError != NULL);
        // Something with the compiler settings is no good.

        ERRORKIND errorKind;
        if (SUCCEEDED (hr = spError->GetErrorInfo(NULL, &errorKind, NULL)) &&
            SUCCEEDED(hr = ReportError (spError)))
        {
            if (ERROR_WARNING == errorKind)
                hr = S_OK;  //Proceed even though there is a warning.
            else
                hr = S_FALSE;
        }
    }

    return hr;
}
