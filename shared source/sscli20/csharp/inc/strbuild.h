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
// File: strbuild.h
//
// ===========================================================================

#ifndef _STRBUILD_H_
#define _STRBUILD_H_

#include "name.h"

interface ICSNameTable;

////////////////////////////////////////////////////////////////////////////////
// CStringBuilder
//
// This is a string buffer optimized for appending text onto the end in several
// small increments, potentially resulting in large strings.  The string buffer
// starts out as a static buffer, and grows as necessary to accomodate larger
// strings.  Note that the append function(s) don't return a result code; instead
// any failure is remembered, and returned when the string is eventually turned
// into a NAME or a BSTR.  Users can also check for successful operation by
// calling GetResultCode(), in the event that the resulting string is not 
// required in the form of either a BSTR or NAME.

class CStringBuilder
{

private:
    // Use CopyFrom instead;
    CStringBuilder(const CStringBuilder&);
    CStringBuilder & operator=(const CStringBuilder&);

protected:
    // NOTE:  Keep this on a power-of-2 boundary!
    enum { ALLOC_SIZE = 256, ALLOC_MASK = ~(ALLOC_SIZE - 1) };

    WCHAR       m_szBuf[ALLOC_SIZE];    // Initial buffer, used for nearly all strings
    PWSTR       m_pszText;              // Buffer -- either points to m_szBuf, or an allocated block
    PWSTR       m_pszCur;               // Pointer to next append location
    size_t      m_iLen;                 // Length so far 
    size_t      m_iBufferSize;          // Size of buffer allocated so far (is ALLOC_SIZE when using the static buffer)
    size_t      m_iBlockSize;           // Variable allocation size
    HRESULT     m_hr;                   // Result code

    static      long        m_iTabSize;             // Size of tabs
    static      BOOL        m_fKeepTabs;            // TRUE if tabs should be used to create indent strings

    size_t      RoundToFullBlock (size_t iLen) { return RoundUp(iLen + 1, m_iBlockSize); }

public:
    CStringBuilder (PCWSTR pszText = NULL) : 
        m_pszText(m_szBuf), 
        m_pszCur(m_szBuf), 
        m_iLen(0), 
        m_iBufferSize(ALLOC_SIZE),
        m_iBlockSize(ALLOC_SIZE), 
        m_hr(S_OK) 
    { 
        m_szBuf[0] = 0; 
        if (pszText != NULL) 
            Append (pszText); 
    }

    ~CStringBuilder () { if (m_pszText != m_szBuf) VSFree (m_pszText); }

    void CopyFrom(const CStringBuilder &other)
    {
        this->Rewind(0);
        this->Append(other.operator PWSTR());
    }

    // Create a BSTR out of the text in the buffer.
    HRESULT     CreateBSTR (BSTR *pbstrOut);

    // Create a NAME out of the text in the buffer
    HRESULT     CreateName (ICSNameTable *pNameTable, NAME **ppName);

    // Get the result code of all previous operations
    HRESULT     GetResultCode () { return m_hr; }

    // Convert the string to lower case
    HRESULT     ToLower ();

    // Append text
    // WARNING: iLen is the size that we'll actually copy, not a max (wcsncpy vs. memcpy)
    void        Append (PCWSTR pszText, size_t iLen = (size_t)-1);

    // Append a single character 
    void        Append (WCHAR c)
    {
        if (RoundToFullBlock (m_iLen + 1) != m_iBufferSize)
        {
            // Must grow -- let the standard Append function deal with it.
            Append (&c, 1);
            return;
        }
        *m_pszCur++ = c;
        *m_pszCur = 0;
        m_iLen++;
    }

    void AppendLong(long val)
    {
        WCHAR buffer[32];
        _i64tow_s(val, buffer, ArrayLength(buffer), 10);
        Append(buffer);
    }

    // Append text checking for and escaping keyword (for code gen)
    void        AppendPossibleKeyword (ICSNameTable *pNameTable, PCWSTR pszText, size_t iLen = (size_t)-1);
    void        AppendPossibleKeyword (ICSNameTable *pNameTable, NAME *pText);

    // Get current length
    size_t      GetLength () { return m_iLen; }
    bool        IsEmpty() { return m_iLen == 0; }

    // Rewind
    void        Rewind (size_t iLen) 
    { 
        ASSERT (iLen <= m_iLen); 
        if (SUCCEEDED (m_hr)) 
        { 
            m_iLen = iLen; 
            m_pszCur = m_pszText + iLen;
            m_pszCur[0] = 0;
        }
    }

    bool Contains(WCHAR c);

    // For ease of use...
    CStringBuilder  & operator += (PCWSTR pszText) { Append (pszText); return *this; }
    CStringBuilder  & operator = (PCWSTR pszText) { Rewind (0); Append (pszText); return *this; }
    operator PWSTR () const { return m_pszText; }
    
    // For formatting/indentation assistance
    static  void    SetTabSettings (long iTabSize, BOOL fKeepTabs) { m_iTabSize = iTabSize; m_fKeepTabs = fKeepTabs; }

    void    AppendIndentString (long iIndentSize);
};

class CXMLStringBuilder : 
    public CStringBuilder
{
public:
    void AppendEncoded (PCWSTR pszText, size_t iLen = (size_t)-1);
};

#endif  // _STRBUILD_H_
