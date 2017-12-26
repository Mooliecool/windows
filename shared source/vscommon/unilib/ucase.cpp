// ucase.cpp -- Unicode case conversion
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// June 1, 1998 [paulde]
//
//-----------------------------------------------------------------
#include "pch.h"
#include "unicase.h"

#define szEmpty L""

//-----------------------------------------------------------------------------
// DEBUG verification routines

#ifdef _DEBUG

void VerifyLowerCase(__in_z PCWSTR sz)
{
    while (*sz)
    {
        UASSERT(LowerCase(*sz) == *sz);
        sz++;
    }
}

void VerifyLowerCaseN(__in_ecount(cch) PCWSTR sz, size_t cch)
{
    while (cch--)
    {
        UASSERT(LowerCase(*sz) == *sz);
        sz++;
    }
}

#define VERIFYLOWERCASE(sz) VerifyLowerCase(sz)
#define VERIFYLOWERCASEN(sz, cch) VerifyLowerCaseN(sz, cch)

#else
#define VERIFYLOWERCASE(sz)
#define VERIFYLOWERCASEN(sz, cch)
#endif


//-----------------------------------------------------------------------------
PWSTR WINAPI UpperCaseStr (_Inout_opt_z_ PWSTR pSrc)
{
    if (pSrc)
        while (*pSrc)
        {
            *pSrc = UpperCase(*pSrc);
            ++pSrc;
        }
    return pSrc;
}

//-----------------------------------------------------------------------------
PWSTR WINAPI LowerCaseStr (_Inout_opt_z_ PWSTR pSrc)
{
    if (pSrc)
        while (*pSrc)
        {
            *pSrc = LowerCase(*pSrc);
            ++pSrc;
        }
    return pSrc;
}

//-----------------------------------------------------------------------------
PWSTR WINAPI ToUpperCase (_In_opt_z_ _In_count_(cch) PCWSTR pSrc, _Out_z_cap_(cch) PWSTR pDst, size_t cch)
{
    if(!pDst)
    {
        UASSERT(false);
        return NULL;
    }
    if (!pSrc)
    {
        *pDst = 0;
        return pDst;
    }
    WCHAR ch;
    if (-1 == cch)
    {
        while (ch = *pSrc++)
            *pDst++ = UpperCase(ch);
        *pDst = 0;
        return pDst;
    }
    if (cch)
    {
        for (; cch && (ch = *pSrc++); cch--)
            *pDst++ = UpperCase(ch);
        if (cch) // if room, terminate
            *pDst = 0;
    }
    return pDst;
}

//-----------------------------------------------------------------------------
PWSTR WINAPI ToUpperCaseInPlace (_Inout_opt_z_ PWSTR pBuffer)
{
    if (!pBuffer)
        return NULL;

    WCHAR* pCh = pBuffer;
    while (*pCh != L'\0')
    {
        *pCh = UpperCase(*pCh);
        pCh++;
    }

    return pBuffer;
}

//-----------------------------------------------------------------------------
PWSTR WINAPI ToLowerCase (_In_opt_z_ _In_count_(cch) PCWSTR pSrc, _Out_z_cap_(cch) PWSTR pDst, size_t cch)
{
    if (!pSrc)
    {
        *pDst = 0;
        return pDst;
    }
    WCHAR ch;
    if (-1 == cch)
    {
        while (ch = *pSrc++)
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

//-----------------------------------------------------------------------------
PWSTR WINAPI ToLowerCaseInPlace (_Inout_opt_z_ PWSTR pBuffer)
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

//-----------------------------------------------------------------------------
// case-insensitive strstr
// Taken from CRT wcsstr.c, but converting chars to lower
PCWSTR WINAPI StrStrNoCase (_In_opt_z_ PCWSTR text, _In_opt_z_ PCWSTR pattern)
{
    if (!text || !pattern) return NULL;
    if (!*pattern) return text; // match strstr spec
    WCHAR ch1, ch2;
    PCWSTR s1;
    PCWSTR s2;
    PCWSTR pch = text;
    while (*pch)
    {
        s1 = pch;
        s2 = pattern;
        while ( (ch1 = *s1) && (!(ch1 - (ch2 = *s2)) || !(LowerCase(ch1)-LowerCase(ch2))) )
            s1++, s2++;
        if (!*s2)
            return pch;
        pch++;
    }
    return NULL;
}

//-----------------------------------------------------------------------------
PCWSTR WINAPI StrStrNoCaseN (_In_opt_count_(cch) PCWSTR text, size_t cch, _In_opt_z_ PCWSTR pattern)
{
    if (!text || !pattern || cch < 0) return NULL;
    if (!*pattern) return text; // match strstr spec

    for ( ; *text && cch; ++text, --cch)
    {
        for (
            PCWSTR s1 = text, s2 = pattern;
            (size_t)(s1 - text) < cch && (*s1 == *s2 || LowerCase(*s1) == LowerCase(*s2));
            ++s1, ++s2)
            {
            }

        if (!*s2)
            return text;
    }
    return NULL;
}

// pattern is prepped w/ lowercase
PCWSTR WINAPI StrStrNoCasePrepared (_In_opt_z_ PCWSTR text, _In_opt_z_ PCWSTR pattern)
{
    if (!text || !pattern) return NULL;
    if (!*pattern) return text; // match strstr spec
    VERIFYLOWERCASE(pattern);
    WCHAR ch1, ch2;
    PCWSTR s1;
    PCWSTR s2;
    PCWSTR pch = text;
    while (*pch)
    {
        s1 = pch;
        s2 = pattern;
        while ( (ch1 = *s1) && (!(ch1 - (ch2 = *s2)) || !(LowerCase(ch1)-ch2)) )
            s1++, s2++;
        if (!*s2)
            return pch;
        pch++;
    }
    return NULL;
}

int WINAPI CompareNoCase (_In_opt_z_ PCWSTR szA, _In_opt_z_ PCWSTR szB)
{
    if (!szA) szA = szEmpty;
    if (!szB) szB = szEmpty;
    if (szA == szB) return 0; // same string

    //Why are we not using wcsicmp?

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
    return SignOf(dx);
}

int WINAPI CompareNoCaseN (_In_opt_count_(cch) PCWSTR szA, _In_opt_count_(cch) PCWSTR szB, size_t cch)
{
    if (!szA) szA = szEmpty;
    if (!szB) szB = szEmpty;
    if (szA == szB) return 0; // same string

    //Why are we not using _wcsnicmp?

    WCHAR a, b;
    int dx = 0;
    while (cch--)
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
        if (0 == a)
            break;
    }
    return SignOf(dx);
}

int WINAPI CompareNoCasePrepared (_In_opt_z_ PCWSTR szA, _In_opt_z_ PCWSTR szB)
{
    if (!szA) szA = szEmpty;
    if (!szB) szB = szEmpty;
    VERIFYLOWERCASE(szA);
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
            b = LowerCase(b);
            dx = (a - b);
            if (dx)
              break;
        }
    } while (a);
    return SignOf(dx);
}

int WINAPI CompareNoCaseNPrepared (_In_opt_z_ _In_count_(cch) PCWSTR szA, _In_opt_z_ PCWSTR szB, size_t cch)
{
    if (!szA) szA = szEmpty;
    if (!szB) szB = szEmpty;
    VERIFYLOWERCASE(szA);
    if (szA == szB) return 0; // same string
    WCHAR a, b;
    int dx = 0;
    while (cch--)
    {
        a = *szA++;
        b = *szB++;
        dx = (a - b);
        if (dx && a)
        {
            b = LowerCase(b);
            dx = (a - b);
            if (dx)
              break;
        }
        if (0 == a)
            break;
    }
    return SignOf(dx);
}

int WINAPI CompareCase (_In_opt_z_ PCWSTR szA, _In_opt_z_ PCWSTR szB)
{
    if (!szA) szA = szEmpty;
    if (!szB) szB = szEmpty;
    if (szA == szB) return 0; // same string

    //Why not use wcscmp?

    int dx = 0;
    while( !(dx = (int)(*szA - *szB)) && *szB)
    {
        ++szA, ++szB;
    }
    return SignOf(dx);
}

//Helpers for quick sort
int __cdecl QCompareCase   (const void * pvA, const void * pvB)
{
    return CompareCase(*((PCWSTR*)pvA), *((PCWSTR*)pvB));
}

int __cdecl QCompareNoCase (const void * pvA, const void * pvB)
{
    return CompareNoCase(*((PCWSTR*)pvA), *((PCWSTR*)pvB));
}

int __cdecl BCompareCase   (const void * pvKey, const void * pvEl)
{
    return CompareCase((PCWSTR)pvKey, *(PCWSTR*)pvEl);
}

int __cdecl BCompareNoCase (const void * pvKey, const void * pvEl)
{
    return CompareNoCase((PCWSTR)pvKey, *(PCWSTR*)pvEl);
}

int __cdecl BCompareNoCasePrepared (const void * pvKey, const void * pvEl)
{
    return CompareNoCasePrepared((PCWSTR)pvKey, *(PCWSTR*)pvEl);
}
///////////////////////////////////////////////////////////////////
// From CRT wcscmp.c, delta = names, cdecl->WINAPI
int WINAPI CompareCaseN(_In_opt_count_(cch) PCWSTR szA, _In_opt_count_(cch) PCWSTR szB, size_t cch)
{
    if (!cch) return 0;
    if (!szA) szA = szEmpty;
    if (!szB) szB = szEmpty;
    if (szA == szB) return 0; // same string

    //Why not use wcsncmp?

    WCHAR ch;
    while (--cch && (ch = *szA) && (ch == *szB))
    {
        szA++;
        szB++;
    }
    return SignOf(*szA - *szB);
}

int WINAPI CompareString32 (_In_opt_z_ PCWSTR pszA, _In_opt_z_ PCWSTR pszB)
{
    int rv = CompareStringW(LOCALE_USER_DEFAULT, 0 /*flags*/, pszA ? pszA : L"", -1, pszB ? pszB : L"", -1);

    //return rv-2; is equivalent and faster than the test below
    if (rv == CSTR_LESS_THAN) 
        return -1;
    else if (rv == CSTR_EQUAL)
        return 0;
    else 
        return 1;
}

int WINAPI CompareString32NoCase (_In_opt_z_ PCWSTR pszA, _In_opt_z_ PCWSTR pszB)
{
    int rv = CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pszA ? pszA : L"", -1, pszB ? pszB : L"", -1);

    //return rv-2; is equivalent and faster than the test below
    if (rv == CSTR_LESS_THAN) 
        return -1;
    else if (rv == CSTR_EQUAL)
        return 0;
    else 
        return 1;
}

int WINAPI CompareFilenames (_In_opt_z_ PCWSTR szA, _In_opt_z_ PCWSTR szB)
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
            a = FileSystemLowerCase(a);
            b = FileSystemLowerCase(b);
            dx = (a - b);
            if (dx)
              break;
        }
    } while (a);
    return SignOf(dx);
}

int WINAPI CompareFilenamesN (_In_opt_count_(cch) PCWSTR szA, _In_opt_count_(cch) PCWSTR szB, size_t cch)
{
    if (!szA) szA = szEmpty;
    if (!szB) szB = szEmpty;
    if (szA == szB) return 0; // same string
    WCHAR a, b;
    int dx = 0;
    while (cch--)
    {
        a = *szA++;
        b = *szB++;
        dx = (a - b);
        if (dx && a)
        {
            a = FileSystemLowerCase(a);
            b = FileSystemLowerCase(b);
            dx = (a - b);
            if (dx)
              break;
        }
        if (0 == a)
            break;
    }
    return SignOf(dx);
}

int WINAPI CompareFilenamesForSort (_In_opt_z_ PCWSTR pszA, _In_opt_z_ PCWSTR pszB)
{
    // When comparing filenames for sorting (not just for equality) we want to use the system locale and
    // "string sort" so that our sorting is equivalent to the file system sorting, especially when using
    // locale-specific features like "sort by stroke"/"sort by pronunciation" (see VSWhidbey #429569).
    // In some cases, however, the sytem comparison can return equivalence when the filenames are not really
    // equal (see VSWhidbey #433428), so in that case we want to check character-by-character as well.

    int rv = CompareStringW(LOCALE_USER_DEFAULT, SORT_STRINGSORT, pszA ? pszA : L"", -1, pszB ? pszB : L"", -1);

    if (rv == CSTR_GREATER_THAN)
        return 1;
    else if (rv == CSTR_LESS_THAN)
        return -1;
    else
        return CompareFilenames(pszA, pszB);
}

PWSTR WINAPI FilenameToLowerCaseInPlace (_Inout_opt_z_ PWSTR pBuffer)
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

PWSTR WINAPI FilenameToUpperCaseInPlace (_Inout_opt_z_ PWSTR pBuffer)
{
    if (!pBuffer)
        return NULL;

    WCHAR* pCh = pBuffer;
    while (*pCh != L'\0')
    {
        *pCh = FileSystemUpperCase(*pCh);
        pCh++;
    }

    return pBuffer;
}
