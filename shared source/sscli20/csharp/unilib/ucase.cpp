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

#include "pch.h"
#include "unicase.h"

#define szEmpty L""

int WINAPI CompareNoCase (PCWSTR szA, PCWSTR szB)
{
    if (!szA) szA = szEmpty;
    if (!szB) szB = szEmpty;
    if (szA == szB) return 0; // same string
    WCHAR a, b;
    int dx = 0;
    do
    {
        a = *szA++;
        b = *szB++;
        dx = (a - b);
        if (dx && a)
        {
            a = LowerCase(a);
            b = LowerCase(b);
            dx = (a - b);
            if (dx)
              break;
        }
    } while (a);
    return ((dx < 0) ? -1 : (dx > 0));
}

PWSTR WINAPI ToLowerCase (PCWSTR pSrc, PWSTR pDst, size_t cch)
{
    if (!pSrc)
    {
        *pDst = 0;
        return pDst;
    }
    WCHAR ch;
    if ((size_t)-1 == cch)
    {
        while ((ch = *pSrc++))
            *pDst++ = LowerCase(ch);
        *pDst = 0;
        return pDst;
    }
    if (cch)
    {
        for (; cch && (ch = *pSrc++); cch--)
            *pDst++ = LowerCase(ch);
        if (cch) // if room, terminate
            *pDst = 0;
    }
    return pDst;
}

PWSTR WINAPI ToLowerCaseInPlace (PWSTR pBuffer)
{
    if (!pBuffer)
        return NULL;

    WCHAR* pCh = pBuffer;
    while (*pCh != L'\0')
    {
        *pCh = LowerCase(*pCh);
        pCh++;
    }

    return pBuffer;
}

PWSTR WINAPI FilenameToLowerCaseInPlace (PWSTR pBuffer)
{
    if (!pBuffer)
        return NULL;

    WCHAR* pCh = pBuffer;
    while (*pCh != L'\0')
    {
        *pCh = FileSystemLowerCase(*pCh);
        pCh++;
    }

    return pBuffer;
}
