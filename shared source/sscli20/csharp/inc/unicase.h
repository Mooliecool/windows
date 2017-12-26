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

// unicase.h - Unicode letter case

#ifdef _MSC_VER
#pragma once
#endif

#ifndef __UNICASE_H__
#define __UNICASE_H__

WCHAR   WINAPI  UpperCase (WCHAR c);
WCHAR   WINAPI  LowerCase (WCHAR c);
PWSTR   WINAPI  UpperCaseStr   (__inout_opt __inout_z PWSTR pSrc); // Returns ptr to 0 terminator
PWSTR   WINAPI  LowerCaseStr   (__inout_opt __inout_z PWSTR pSrc); // Returns ptr to 0 terminator

WCHAR   WINAPI  FileSystemUpperCase (WCHAR c);
WCHAR   WINAPI  FileSystemLowerCase (WCHAR c);

//
// To{Upper|Lower}Case - Convert buffer to Upper/Lower case.
// pSrc -- Source string
// pDst -- Destination buffer
// cch  -- For zero-terminated mode, pass -1. pDst assumed to be large enough.
//         Otherwise pass maximum number of characters to convert into pDst
//         including the zero terminator. Conversion always stops at a 0 char
//         in pSrc.
// Returns: pointer to END of pDst
//
PWSTR   WINAPI  ToUpperCase    (__in_ecount_opt(cch) PCWSTR pSrc, __out_ecount(cch) PWSTR pDst, size_t cch);
PWSTR   WINAPI  ToLowerCase    (__in_ecount_opt(cch) PCWSTR pSrc, __out_ecount(cch) PWSTR pDst, size_t cch);

//
// To{Upper|Lower}Case - Convert buffer to Upper/Lower case in place.
// pBuffer -- String to {upper|lower} case.
// Returns: pointer to BEGINNING of pBuffer.
//
PWSTR   WINAPI  ToUpperCaseInPlace    (__inout_opt __inout_z PWSTR pBuffer); 
PWSTR   WINAPI  ToLowerCaseInPlace    (__inout_opt __inout_z PWSTR pBuffer);

//
// Compare[No]Case[N][Prepared]
//
// The szA and/or szB params may be NULL.
//
// *Prepared variations assume that szA is prepared according to 
// LowerCaseStr or ToLowerCase.
//
// Returns:
//      -1  A < B
//       0  A == B
//       1  A > B
//
int     WINAPI  CompareCase            (__in_opt __in_z PCWSTR szA, __in_opt __in_z PCWSTR szB);
int     WINAPI  CompareNoCase          (__in_opt __in_z PCWSTR szA, __in_opt __in_z PCWSTR szB);
int     WINAPI  CompareCaseN           (__in_ecount_opt(cch) PCWSTR szA, __in_ecount_opt(cch) PCWSTR szB, size_t cch);
int     WINAPI  CompareNoCaseN         (__in_ecount_opt(cch) PCWSTR szA, __in_ecount_opt(cch) PCWSTR szB, size_t cch);
int     WINAPI  CompareNoCasePrepared  (__in_opt __in_z PCWSTR szA, __in_opt __in_z PCWSTR szB);
int     WINAPI  CompareNoCaseNPrepared (__in_ecount_opt(cch) PCWSTR szA, __in_ecount_opt(cch) PCWSTR szB, size_t cch);

//
// CompareString32[NoCase]
//
// String comparision using Win32 CompareString().  Operates on
// the user default locale.  This is just a convenience wrapper
// to make CompareString() appear like wcscmp().  
// szA and szB cannot be NULL.  
//
// Returns:   
//      -1 if A < B
//       0 if A == B
//       1 if A > B
//
int     WINAPI  CompareString32          (__in_opt __in_z PCWSTR szA, __in_opt __in_z PCWSTR szB);
int     WINAPI  CompareString32NoCase    (__in_opt __in_z PCWSTR szA, __in_opt __in_z PCWSTR szB);

//
// CompareFilenames
//
// String comparision for two strings which are filenames or
// extensions.  This is a wrapper around CompareNoCase[N].  
// szA and/or szB can be NULL. 
//
// Returns:   
//      -1 if A < B
//       0 if A == B
//       1 if A > B
//
int     WINAPI  CompareFilenames         (__in_opt __in_z PCWSTR szA, __in_opt __in_z PCWSTR szB);
int     WINAPI  CompareFilenamesN        (__in_ecount_opt(cch) PCWSTR szA, __in_ecount_opt(cch) PCWSTR szB, size_t cch);
int     WINAPI  CompareFilenamesForSort  (__in_opt __in_z PCWSTR szA, __in_opt __in_z PCWSTR szB);

//
// FilenameTo{Upper|Lower}CaseInPlace - Convert filename to Upper/Lower case in place.
// pBuffer -- Filename to {upper|lower} case.
// Returns: pointer to BEGINNING of pBuffer.
//
PWSTR   WINAPI  FilenameToUpperCaseInPlace    (__inout_opt __inout_z PWSTR pBuffer); 
PWSTR   WINAPI  FilenameToLowerCaseInPlace    (__inout_opt __inout_z PWSTR pBuffer);

//
// StrStrNoCase[prepared] - Find <pattern> within <text>
//
// StrStrNoCasePrepared assumes that <pattern> is prepared 
// according to LowerCaseStr or ToLowerCase.
//
// For case-sensitive, you can use the CRT's wcsstr from wchar.h.
//
PCWSTR  WINAPI  StrStrNoCase           (__in_opt __in_z PCWSTR text, __in_opt __in_z PCWSTR pattern);
PCWSTR  WINAPI  StrStrNoCasePrepared   (__in_opt __in_z PCWSTR text, __in_opt __in_z PCWSTR pattern);

// VS7:32377  non-const variants
inline PWSTR WINAPI  StrStrNoCase           (__in_opt __in_z PWSTR text, __in_opt __in_z PCWSTR pattern){return const_cast<PWSTR>(StrStrNoCase (const_cast<PCWSTR>(text), pattern));}
inline PWSTR WINAPI  StrStrNoCasePrepared   (__in_opt __in_z PWSTR text, __in_opt __in_z PCWSTR pattern){return const_cast<PWSTR>(StrStrNoCasePrepared (const_cast<PCWSTR>(text), pattern));}

WCHAR   WINAPI  TitleCaseForUpperCase (WCHAR ch);
BOOL    WINAPI  ExcludeTitleCase      (WCHAR ch);

//----------------------------------------------------------------
// CopyCase  - Copy the letter case of one string to another.
// Changes the case of text in pDst to match the case of the text
// in pSrc. Can be used to implement "Keep Case"-type feature in 
// Find/Replace.
PWSTR WINAPI CopyCase (PCWSTR pSrc, PWSTR pDst);

//
// QCompare[No]Case - qsort WCHAR ptr array functions
//
// qsort(arr, cel, sizeof(PCWSTR), QCompareCase);
//
int __cdecl QCompareCase           (const void * pvA, const void * pvB);
int __cdecl QCompareNoCase         (const void * pvA, const void * pvB);

//
// BCompare[No]Case[Prepared] - bsearch WCHAR ptr array
//
// PCWSTR pszKey = L"Find this";
// PCWSTR psz = bsearch(pszKey, arr, cel, sizeof(PCWSTR), BCompareCase);
//
// BCompareNoCasePrepared assumes pvKey is prepared according to 
// LowerCaseStr or ToLowerCase.
//
int __cdecl BCompareCase           (const void * pvKey, const void * pvEl);
int __cdecl BCompareNoCase         (const void * pvKey, const void * pvEl);
int __cdecl BCompareNoCasePrepared (const void * pvKey, const void * pvEl);

#include "unipriv.h"

// PHarring: Performance optimization for LowerCase most common case
WCHAR WINAPI LowerCaseComplete(WCHAR ch);

// Deal with the most frequent cases first for performance
// To get the best performance out of this, compile with /Ot (optimize for speed)
inline WCHAR WINAPI LowerCase(WCHAR ch)
{
    if (ch <  L'A')
    {
        return ch;
    }
    
    // For upper-case A-Z, convert to 'a'-'z' (constant offset)
    if( ch <= L'Z' )
    {
        return ch + (L'a' - L'A');
    }

    // For chars less than 0x00c0 (Latin Capital Letter A With Grave)
    // leave them as-is (already lower-case)
    if( ch < 0x00c0 )
    {
        return ch;
    }

    // For everything else, do the full test
    return LowerCaseComplete(ch);
}

/////////////////////////////////////////////////////////////////
// Title case
//

//---------------------------------------------------------------
// TitleCaseForUpperCase
//
// When you have determined that a character is initial and have 
// the Uppercase form, this function substitutes another character 
// that is better for Titlecase than the Uppercase char, otherwise
// it passes the character through.
//
inline WCHAR WINAPI TitleCaseForUpperCase(WCHAR ch)
{
    UASSERT(ch == UpperCase(ch)); // Call this function only with uppercase form!
    
    // probably not
    if (ch < 0x01c4 || ch > 0x01f1)
        return ch;

    // 01C4;LATIN CAPITAL LETTER DZ WITH CARON -> 01C5;LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON
    // 01C7;LATIN CAPITAL LETTER LJ -> 01C8;LATIN CAPITAL LETTER L WITH SMALL LETTER J
    // 01CA;LATIN CAPITAL LETTER NJ -> 01CB;LATIN CAPITAL LETTER N WITH SMALL LETTER J
    // 01F1;LATIN CAPITAL LETTER DZ -> 01F2;LATIN CAPITAL LETTER D WITH SMALL LETTER Z
    //
    // Unicode nicely places all these adjacent with the Titlecase code point 
    // between the Upper and Lower code points.
    if ((0x01C4 == ch) || (0x01C7 == ch) || (0x01CA == ch) || (0x01F1 == ch))
        ch++; 

    return ch;
}

//---------------------------------------------------------------
// ExcludeTitleCase
//
// For a character with a mapping to Uppercase, excludes that 
// character from mapping to Titlecase.
//
// These are all Greek technical symbols that have Uppercase 
// equivalents, but not for Titlecase use.
//
// Pass this function the original, unmapped char -- not the Upppercase char
//
inline BOOL WINAPI ExcludeTitleCase(WCHAR ch)
{
    // probably not
    if ((ch < 0x3D0) || (ch > 0x03F1))
        return FALSE;

    // In the absence of frequency data to determine an optimal 
    // test ordering, use an ordering with strictly humorous value.
    return ((ch == 0x03D1) || // GREEK THETA SYMBOL   // this one I've actually used back in college...
            (ch == 0x03D5) || // GREEK PHI SYMBOL     // to the
            (ch == 0x03D0) || // GREEK BETA SYMBOL    //    head of
            (ch == 0x03F0) || // GREEK KAPPA SYMBOL   //       the class
            (ch == 0x03F1) || // GREEK RHO SYMBOL     // rho your boat downstream
            (ch == 0x03D6)    // GREEK PI SYMBOL      // dessert comes last (I'd have it first, but it's an odd flavor of pi)
        );
}

#endif // __UNICASE_H__
