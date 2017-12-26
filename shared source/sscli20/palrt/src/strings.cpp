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
// File: strings.cpp
//
// String APIs ported from shlwapi (especially for Fusion)
// ===========================================================================

#include "shlwapip.h"

// NOTE: diff from wcscat - StrNCatW's ccMax includes \0 terminator
STDAPI_(LPWSTR) StrNCatW(LPWSTR lpFront, LPCWSTR lpBack, int cchMax)
{
    LPWSTR lpStart = lpFront;

    RIPMSG(lpFront && IS_VALID_STRING_PTRW(lpFront, -1), "StrNCatW: Caller passed invalid lpFront");
    RIPMSG(lpBack && IS_VALID_STRING_PTRW(lpFront, cchMax), "StrNCatW: Caller passed invalid lpBack");
    RIPMSG(cchMax >= 0, "StrNCatW: Caller passed invalid cchMax");
    if (lpFront && lpBack)
    {
        while (*lpFront++)
                    ;
        lpFront--;

        StrCpyNW(lpFront, lpBack, cchMax);
    }
    return(lpStart);
}

#define IS_DIGITW(ch)    InRange(ch, L'0', L'9')

__inline BOOL ChrCmpW_inline(WCHAR w1, WCHAR wMatch)
{
    return(!(w1 == wMatch));
}

BOOL ChrCmpIW(WCHAR w1, WCHAR wMatch)
{
    WCHAR sz1[2], sz2[2];

    sz1[0] = w1;
    sz1[1] = '\0';
    sz2[0] = wMatch;
    sz2[1] = '\0';

    return StrCmpIW(sz1, sz2);
}

STDAPI_(int) StrToIntW(LPCWSTR lpSrc)
{
    RIPMSG(lpSrc && IS_VALID_STRING_PTRW(lpSrc, -1), "StrToIntW: Caller passed bad lpSrc");
    if (lpSrc)
    {
        int n = 0;
        BOOL bNeg = FALSE;

        if (*lpSrc == L'-')
        {
            bNeg = TRUE;
            lpSrc++;
        }

        while (IS_DIGITW(*lpSrc))
        {
            n *= 10;
            n += *lpSrc - L'0';
            lpSrc++;
        }
        return bNeg ? -n : n;
    }
    return 0;
}

STDAPI_(LPWSTR) StrChrIW(LPCWSTR lpStart, WCHAR wMatch)
{
    RIPMSG(lpStart && IS_VALID_STRING_PTRW(lpStart, -1), "StrChrIW: caller passed bad lpStart");
    if (lpStart)
    {
        for ( ; *lpStart; lpStart++)
        {
            if (!ChrCmpIW(*lpStart, wMatch))
                return((LPWSTR)lpStart);
        }
    }
    return (NULL);
}

STDAPI_(LPWSTR) StrStrIW(LPCWSTR lpFirst, LPCWSTR lpSrch)
{
    RIPMSG(lpFirst && IS_VALID_STRING_PTRW(lpFirst, -1), "StrStrIW: Caller passed bad lpFirst");
    RIPMSG(lpSrch && IS_VALID_STRING_PTRW(lpSrch, -1), "StrStrIW: Caller passed bad lpSrch");
    if (lpFirst && lpSrch)
    {
        UINT uLen = (UINT)lstrlenW(lpSrch);
        WCHAR wMatch = *lpSrch;

        for ( ; (lpFirst = StrChrIW(lpFirst, wMatch)) != 0 && StrCmpNIW(lpFirst, lpSrch, uLen);
             lpFirst++)
            continue; /* continue until we hit the end of the string or get a match */

        return (LPWSTR)lpFirst;
    }
    return NULL;
}

STDAPI_(LPWSTR) StrRChrW(LPCWSTR lpStart, LPCWSTR lpEnd, WCHAR wMatch)
{
    LPCWSTR lpFound = NULL;

    RIPMSG(lpStart && IS_VALID_STRING_PTRW(lpStart, -1), "StrRChrW: caller passed bad lpStart");
    RIPMSG(!lpEnd || lpEnd <= lpStart + lstrlenW(lpStart), "StrRChrW: caller passed bad lpEnd");
    // don't need to check for NULL lpStart

    if (!lpEnd)
        lpEnd = lpStart + lstrlenW(lpStart);

    for ( ; lpStart < lpEnd; lpStart++)
    {
        if (!ChrCmpW_inline(*lpStart, wMatch))
            lpFound = lpStart;
    }
    return ((LPWSTR)lpFound);
}

STDAPI_(LPWSTR) StrCatBuffW(LPWSTR pszDest, LPCWSTR pszSrc, int cchDestBuffSize)
{
    RIPMSG(pszDest && IS_VALID_STRING_PTRW(pszDest, -1), "StrCatBuffW: Caller passed invalid pszDest");
    RIPMSG(pszSrc && IS_VALID_STRING_PTRW(pszSrc, -1), "StrCatBuffW: Caller passed invalid pszSrc");
    RIPMSG(cchDestBuffSize >= 0, "StrCatBuffW: Caller passed invalid cchDestBuffSize");
    RIPMSG(!(pszDest && IS_VALID_STRING_PTRW(pszDest, -1)) || cchDestBuffSize<0 || lstrlenW(pszDest)<cchDestBuffSize, "StrCatBuffW: Caller passed odd pszDest - string larger than cchDestBuffSize!");

    if (pszDest && pszSrc)
    {
        LPWSTR psz = pszDest;

        // we walk forward till we find the end of pszDest, subtracting
        // from cchDestBuffSize as we go.
        while (*psz)
        {
            psz++;
            cchDestBuffSize--;
        }

        if (cchDestBuffSize > 0)
        {
            // call the shlwapi function here because win95 does not have lstrcpynW
            StrCpyNW(psz, pszSrc, cchDestBuffSize);
        }
    }
    return pszDest;
}
