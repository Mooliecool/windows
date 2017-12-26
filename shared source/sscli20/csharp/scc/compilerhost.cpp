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

#include "stdafx.h"
#include "compilerhost.h"
#include "posdata.h"
#include "unilib.h"
#include <strsafe.h>

#include "scc.h"

#include "atl.h"

CompilerHost::CompilerHost(ConsoleOutput *output) : 
pCompiler(NULL), 
m_pOutput(output)
{
    m_pOutput = output;
    m_uiCodePage = 0;
}

CompilerHost::~CompilerHost()
{
}

////////////////////////////////////////////////////////////////////////////////
// CompilerHost::ReportError

void CompilerHost::ReportError (ICSError *pError, ConsoleOutput *pOutput)
{
    long        iErrorId;
    ERRORKIND   iKind;
    PCWSTR      pszText;

    if (FAILED (pError->GetErrorInfo (&iErrorId, &iKind, &pszText)))
    {
        pOutput->ShowErrorId (FTL_NoMemory, ERROR_FATAL);
        return;
    }

    long        iLocations = 0;

    // Get the location(s) if any.
    if (SUCCEEDED (pError->GetLocationCount (&iLocations)) && iLocations > 0)
    {
        PCWSTR      pszFileName = NULL;
        POSDATA     posStart, posEnd;

        // For each location, display the error.  For locations past the first one,
        // display the "related symbol to previous error" as the text
        for (long i=0; i<iLocations; i++)
        {
            if (SUCCEEDED (pError->GetLocationAt (i, &pszFileName, &posStart, &posEnd)))
            {
                int iLine = posStart.IsUninitialized() ? -1 : (int)posStart.iLine + 1;
                int iLineEnd = posEnd.IsUninitialized() ? -1 : (int)posEnd.iLine + 1;

                if (i==0)
                    pOutput->ShowError (iErrorId, iKind, pszFileName, iLine, posStart.iChar + 1, iLineEnd, posEnd.iChar + 1, pszText);
                else
                    pOutput->ShowRelatedLocation (iKind, pszFileName, iLine, posStart.iChar + 1, iLineEnd, posEnd.iChar + 1);
            }
        }
    }
    else
    {
        pOutput->ShowError (iErrorId, iKind, NULL, -1, 0, -1, 0, pszText);
    }
}

////////////////////////////////////////////////////////////////////////////////
// CompilerHost::ReportErrors
//
// This is called from the compiler when errors are available for "display".

STDMETHODIMP CompilerHost::ReportErrors (ICSErrorContainer *pErrors)
{
    long    iErrors;

    if (FAILED (pErrors->GetErrorCount (NULL, NULL, NULL, &iErrors)))
    {
        m_pOutput->ShowErrorId (FTL_NoMemory, ERROR_FATAL);
        return S_OK;
    }

    for (long i=0; i<iErrors; i++)
    {
        CComPtr<ICSError>   spError;

        if (FAILED (pErrors->GetErrorAt (i, &spError)))
        {
            m_pOutput->ShowErrorId (FTL_NoMemory, ERROR_FATAL);
            return S_OK;
        }

        ReportError (spError, m_pOutput);
    }

    return NOERROR;
}

////////////////////////////////////////////////////////////////////////////////
// CompilerHost::OnCatastrophicError

STDMETHODIMP_(void) CompilerHost::OnCatastrophicError (BOOL fException, DWORD dwException, void *pAddress)
{
    m_pOutput->ShowErrorIdString (FTL_InternalError, ERROR_FATAL, fException ? dwException : E_FAIL);
}



/*
 * Given a file name (fully qualified by the compiler), load the source text into a
 * buffer wrapped by an ICSSourceText implementation, and turn around and ask the
 * compiler to create an ICSSourceModule object for it.
 */
STDMETHODIMP CompilerHost::GetSourceModule (PCWSTR pszFileName, BOOL fNeedChecksum, ICSSourceModule **ppModule)
{
    *ppModule = NULL;

    CSourceText     *pSourceText = new CSourceText();

    if (pSourceText == NULL)
        return E_OUTOFMEMORY;

    HRESULT     hr;

    if (FAILED (hr = pSourceText->Initialize (pszFileName, fNeedChecksum, m_uiCodePage)) ||
        FAILED (hr = pCompiler->CreateSourceModule (pSourceText, ppModule)))
    {
        delete pSourceText;
    }

    return hr;
}

void CompilerHost::NotifyBinaryFile(LPCWSTR pszFileName)
{
    m_pOutput->OutputBinaryFileToRepro(pszFileName, NULL);
}
void CompilerHost::NotifyMetadataFile(LPCWSTR pszFileName, LPCWSTR pszCorSystemDirectory)
{
    //This notification is only for the purpose of building a bugreport file.
    //Do not report files in the framework directory. They are assumed to be well understood.
    if (_wcsnicmp(pszFileName, pszCorSystemDirectory, wcslen(pszCorSystemDirectory)))
    {
        NotifyBinaryFile(pszFileName);
    }
}

HRESULT CompilerHost::GetBugReportFileName(__out_ecount(cchLength) PWSTR pszFileName, DWORD cchLength)
{
    // If the compiler is askign for this it means we've crashed, which means
    // force close the bugreport so it can be sent
    m_pOutput->CloseBugReport();
    return m_pOutput->GetBugReportFileName(WCBuffer(pszFileName, cchLength));
}

