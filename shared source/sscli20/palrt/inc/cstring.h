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

#ifndef __CSTRING_H__
#define __CSTRING_H__

#ifdef __cplusplus

#define AtlThrow(a) RaiseException(STATUS_NO_MEMORY,EXCEPTION_NONCONTINUABLE,0,NULL); 
#define ATLASSERT(a) _ASSERTE(a)

#include "ccombstr.h"

class CStringW : public CComBSTR
{
public:
    CStringW() {
    }
    
    CStringW(int nSize, LPCOLESTR sz) :
        CComBSTR (nSize, sz)
    {
    }

    
    CStringW(LPCOLESTR pSrc) :
        CComBSTR(pSrc)
    {
    }

    CStringW(const CStringW& src) :
        CComBSTR(src)
    {
    }

    CStringW (LPCSTR pSrc) :
        CComBSTR()
    {
        // Intentionaly create us as empty string, later
        //   we will overwrite ourselves with the
        //   converted string.
        int cchSize;
        cchSize = MultiByteToWideChar(CP_ACP, 0, pSrc, -1, NULL, 0);
        if (cchSize == 0)
        {
            AtlThrow(E_OUTOFMEMORY);
        }

        CComBSTR bstr(cchSize);
        // No convert the string
        // (Note that (BSTR)bstr will return a pointer to the
        // allocated WCHAR buffer - done by the CComBSTR constructor)
        if (MultiByteToWideChar(CP_ACP, 0, pSrc, -1, (WCHAR *)((BSTR)bstr), cchSize) == 0)
        {
            AtlThrow(E_OUTOFMEMORY);
        }

        // And now assign this new bstr to us
        //  The following is a trick how to avoid copying the string
        Attach(bstr.Detach());
    }

    ~CStringW() {
    }
};

#endif // __cplusplus

#endif // __CSTRING_H__
