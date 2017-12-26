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
// File: strbuild.cpp
//
// ===========================================================================

#include "pch.h"
#include "strbuild.h"
#include "csiface.h"

long    CStringBuilder::m_iTabSize = 4;
BOOL    CStringBuilder::m_fKeepTabs = FALSE;

////////////////////////////////////////////////////////////////////////////////
// CStringBuilder::CreateBSTR

HRESULT CStringBuilder::CreateBSTR (BSTR *pbstrOut)
{
    // Report any accumulation failure
    if (FAILED (m_hr)) {
        *pbstrOut = NULL;
        return m_hr;
    }

    // Just allocate a BSTR out of our current contents.
    return AllocateBSTR (m_pszText, pbstrOut);
}

////////////////////////////////////////////////////////////////////////////////
// CStringBuilder::CreateName

HRESULT CStringBuilder::CreateName (ICSNameTable *pNameTable, NAME **ppName)
{
    // Report any accumulation failure
    if (FAILED (m_hr)) {
        *ppName = NULL;
        return m_hr;
    }

    // Create a name using the given name table
    return pNameTable->Add (m_pszText, ppName);
}


////////////////////////////////////////////////////////////////////////////////
// CStringBuilder::Append

void CStringBuilder::Append (PCWSTR pszText, size_t iLen)
{
    // Do nothing if we've already failed
    if (FAILED (m_hr))
        return;

    // If -1, use nul to determine length
    if (iLen == (size_t)-1)
    {
        if (pszText == NULL)
            iLen = 0;
        else
            iLen = wcslen (pszText);
    }

    // Empty string, don't do anything.
    if (iLen == 0)
        return;

    // This ensure we will never overflow when calculating "iReqSize".
    // Suppose m_iLen = 100 and iLen == 2^32-2, we would get iReqSize = 256
    // and copy 2^32-2 bytes!
    if (iLen >= INT_MAX || m_iLen >= INT_MAX || (iLen + m_iLen) >= INT_MAX)
    {
        ASSERT(!"Bad string length");
        m_hr = E_INVALIDARG;
        return;
    }

    // Determine new size and reallocate if necessary
    size_t      iReqSize = RoundToFullBlock (m_iLen + iLen);

    if (m_pszText == m_szBuf)
    {
        // this line could have been written "if (iReqSize > ALLOC_SIZE)"
        // remember that m_szBuf is a pre-allocated buffer, and we're trying
        // to figure out if we need to move to the heap.
        if (iReqSize > (sizeof (m_szBuf) / sizeof (m_szBuf[0])))
        {
            // The size needed is bigger than our static buffer.  Time to allocate
            // the dynamic buffer for the first time
            m_pszText = (PWSTR)VSAlloc (iReqSize * sizeof (WCHAR));
            if (m_pszText == NULL)
            {
                m_hr = E_OUTOFMEMORY;
                m_pszText = m_szBuf;
                return;
            }

            // Copy the current text (m_iLen doesn't include the terminating 0, so +1 to copy it)
            memcpy (m_pszText, m_szBuf, (m_iLen + 1) * sizeof (WCHAR));

            // Keep our current pointer current...
            m_pszCur = m_pszText + m_iLen;
            m_iBufferSize = iReqSize;
            ASSERT (m_pszCur[0] == 0);
        }
        else
        {
            // Do nothing -- the text will fit in our static buffer.
        }
    }
    else if (iReqSize != m_iBufferSize)
    {
        // See if we need to increase our allocation size
        if (iReqSize >= 0x10000 && m_iBlockSize < 0x10000)
        {
            m_iBlockSize = 0x10000;
            iReqSize = RoundToFullBlock (m_iLen + iLen);
        }
        else if (iReqSize >= 0x1000 && m_iBlockSize < 0x1000)
        {
            m_iBlockSize = 0x1000;
            iReqSize = RoundToFullBlock (m_iLen + iLen);
        }

        // Need to grow
        PWSTR   pszNewText = (PWSTR)VSRealloc (m_pszText, iReqSize * sizeof (WCHAR));
        if (pszNewText == NULL)
        {
            m_hr = E_OUTOFMEMORY;
            return;
        }
        m_pszText = pszNewText;

        // Keep our current pointer current...
        m_pszCur = m_pszText + m_iLen;
        m_iBufferSize = iReqSize;
        ASSERT (m_pszCur[0] == 0);
    }

    ASSERT (iReqSize > m_iLen + iLen);

    // Copy incoming text to end of buffer. Note that StringCchCopyN *will* NUL
    // terminate "m_pszCur".
    m_hr = StringCchCopyNW (m_pszCur, iLen + 1, pszText, iLen);
    VSASSERT(SUCCEEDED(m_hr), "Well, it looks like we messed up a call to StringCchCopyN");

    m_iLen += iLen;
    m_pszCur += iLen;

    VSASSERT(m_pszCur[0] == 0, "String not null terminated at expected spot. It looks like we messed up a call to StringCchCopyN");
}

////////////////////////////////////////////////////////////////////////////////
// CStringBuilder::AppendPossibleKeyword

void CStringBuilder::AppendPossibleKeyword (ICSNameTable *pNameTable, NAME *pName)
{
    HRESULT     hr = E_FAIL;
    long        iTok;

    // Check to see if the text is a keyword
    if (FAILED (hr = pNameTable->IsKeyword (pName, CompatibilityNone, &iTok)))
    {
        m_hr = hr;
        return;
    }

    if (hr == S_OK)
        Append (L"@");
    Append (pName->text);
}

////////////////////////////////////////////////////////////////////////////////
// CStringBuilder::AppendPossibleKeyword

void CStringBuilder::AppendPossibleKeyword (ICSNameTable *pNameTable, PCWSTR pszText, size_t iLen)
{
    HRESULT     hr = E_FAIL;
    NAME        *pName;

    if (iLen == (size_t)-1)
        iLen = wcslen (pszText);

    if (FAILED (hr = pNameTable->AddLen (pszText, (long)iLen, &pName)))
    {
        m_hr = hr;
        return;
    }

    AppendPossibleKeyword (pNameTable, pName);
}

////////////////////////////////////////////////////////////////////////////////
// CStringBuilder::ToLower

HRESULT CStringBuilder::ToLower ()
{
    // Call the unilib functions to convert this inplace.
    if (NULL == ToLowerCaseInPlace(m_pszText))
        return E_FAIL;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CStringBuilder::AppendIndentString

void CStringBuilder::AppendIndentString (long iIndent)
{
    // Check for the no-brainer case...
    if (iIndent == 0)
        return;

    long    iTabs = 0, iSpaces = iIndent, i;

    if (m_fKeepTabs)
    {
        iTabs = iIndent / m_iTabSize;
        iSpaces -= iTabs * m_iTabSize;
    }

    for (i=0; i<iTabs; i++)
        Append ('\t');

    for (i=0; i<iSpaces; i++)
        Append (' ');
}

////////////////////////////////////////////////////////////////////////////////

bool CStringBuilder::Contains(WCHAR c)
{
    return wcschr(this->operator PWSTR(), c) != NULL;
}

static const WCHAR  gpszEncodedChars[] = L"<>&";
static const PCWSTR grgszEncodings[] = {L"&lt;", L"&gt;", L"&amp;", L"&zero;"};

////////////////////////////////////////////////////////////////////////////////

void CXMLStringBuilder::AppendEncoded (PCWSTR pszText, size_t iLen)
{
    if (iLen == (size_t)-1)
        iLen = wcslen (pszText);

    for (size_t i=0; i<iLen; i++)
    {
        size_t  iBase = i;

        size_t j;
        for (j=i; j<iLen; j++)
        {
            PCWSTR  pszChar = pszText[j] != 0 ? wcschr (gpszEncodedChars, pszText[j]) : (gpszEncodedChars + wcslen(gpszEncodedChars));

            if (pszChar != NULL)
            {
                // Append the characters scanned so far
                if (j > iBase)
                    Append (pszText + iBase, j - iBase);

                // Append the encoding for this char
                Append (grgszEncodings[pszChar - gpszEncodedChars]);

                // Exit the inner loop
                iBase = j;
                break;
            }
        }

        if (j > iBase)
            Append (pszText + iBase, j - iBase);

        i = j;
    }
}
