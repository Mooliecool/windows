//------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//     Defines a metered string convenience class
//
// History:
//      2005/06/19 - [....]
//          Created
//      2007/09/20-[....]
//          Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once

#ifndef STRING_HXX_INCLUDED
#define STRING_HXX_INCLUDED

#include <WinInet.h> //INTERNET_MAX_URL_LENGTH

struct IStream;

class CString
{
    // Copy not implemented. We don't have a good way to respond to allocation errors.
    CString(const CString &src);
    void operator=(const CString &src);
public:
    CString(size_t maxLength = INTERNET_MAX_URL_LENGTH + 1);
    ~CString();

    LPCWSTR GetValue() const { return m_pwzValue; }
    HRESULT SetValue(__in_opt LPCWSTR pszValue);
    static CString* CreateOnHeap(LPCWSTR pszValue, size_t maxLength = INTERNET_MAX_URL_LENGTH + 1);

    size_t Length() const { return m_curLength; }

    HRESULT WriteToStream(IStream* pOutputStream) const;
    HRESULT ReadFromStream(IStream* pInputStream);

private:
    void Free();

    LPWSTR m_pwzValue;
    size_t m_maxLength, m_curLength;
};

// This defines a plain string property. 
#define STRING_PROP(name) \
    HRESULT Set##name(__in_opt LPCWSTR pszValue) { return m_str##name.SetValue(pszValue); } \
    LPCWSTR Get##name() const { return m_str##name.GetValue(); } \
    CString* name() { return &m_str##name; }

#endif
