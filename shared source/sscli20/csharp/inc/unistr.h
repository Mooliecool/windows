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

// unistr.h

#ifdef _MSC_VER
#pragma once
#endif

#ifndef __UNISTR_H__
#define __UNISTR_H__

#ifdef _MSC_VER
#if !defined(_M_CEE)
#pragma intrinsic(strlen)
#endif
#endif

// Replacement for _snwprintf, etc., that performs correct zero termination
int __cdecl BufPrint(__out_ecount(cchBuf) LPWSTR pwszBuf, size_t cchBuf, __in_z LPCWSTR pwszFormat, ...);
int __cdecl BufPrintArgs(DWORD dwFlags, __out_ecount(cchBuf) LPWSTR pwszBuf, size_t cchBuf, __in_z LPCWSTR pwszFormat, va_list argList);

// Replacement for _snwprintf, etc., that performs correct zero termination
int __cdecl BufPrintA( __out_ecount(cchBuf) LPSTR pszBuf, size_t cchBuf, __in_z LPCSTR pszFormat, ...);
int __cdecl BufPrintArgsA(DWORD dwFlags, __out_ecount(cchBuf) LPSTR pszBuf, size_t cchBuf, __in_z LPCSTR pszFormat, va_list argList);



int     WINAPI   StrLen          ( __in_opt __in_z PCWSTR psz);
int     WINAPI   StrLenA         ( __in_opt __in_z PCSTR  psz);


//Note that all of the functions below are somewhat unsafe

// Copy strings for catenation.  Returns the next position for appending.

//These methods are all unsafe and should be deprecated.
PWSTR   WINAPI   CopyCat         ( __out_z PWSTR dst, __in_z PCWSTR src);
PSTR    WINAPI   CopyCatA        ( __out_z PSTR  dst, __in_z PCSTR  src);

// The 'N' versions guarantee zero termination, but can write one more than cchz.
PWSTR   WINAPI   CopyNCat        ( __out_ecount(cchz+1) PWSTR dst, PCWSTR src, int cchz);  
PSTR    WINAPI   CopyNCatA       ( __out_ecount(cchz+1) PSTR  dst, PCSTR  src, int cchz);

PWSTR   WINAPI   CopyCatInt      ( __out_z PWSTR dst, int n, int radix);
PSTR    WINAPI   CopyCatIntA     ( __out_z PSTR  dst, int n, int radix);

//Instead of using the functions above, you should use the various equivalent methods from strsafe.h
//So, instead of:
//      WCHAR*  p = sz;
//
//      p = CopyCat   (p, szSrc1);
//      p = CopyCat   (p, szSrc2);
//      p = CopyNCat  (p, szSrc3, iLenSrc3);
//      p = CopyCatInt(p, n, base);
//      CopyCat(p, szSrc4);
//
//You should do the following:
//      WCHAR*  p = sz;
//      size_t  cchP = _countof(sz);
//      HRESULT hr;
//
//      hr = StringCchCopyExW (p, cchP, szSrc1, &p, &cchP, 0);
//      hr = StringCchCopyExW (p, cchP, szSrc2, &p, &cchP, 0);
//      hr = StringCchCopyNExW(p, cchP, szSrc3, iLenSrc3, &p, &cchP, 0);
//      hr = StringCchIntExW  (p, cchP, n, base, &p, &cchP, 0);
//      hr = StringCchCopyW   (p, cchP, szSrc4);

//As above, but copy the integer represented in the given base rather than the source string
//Note that these functions -- though they follow the StringCchCopyEx semantics -- are implemented in unistr.cpp
HRESULT StringCchIntExW  (__out_ecount(cchDst) wchar_t*  dst, size_t cchDst,
                          int n, unsigned int radix,
                          __deref_opt_out_ecount(*pcchDstEnd) wchar_t** ppDstEnd, __out_ecount_opt(1) size_t* pcchDstEnd, unsigned long dwFlags);
HRESULT StringCchIntExA  (__out_ecount(cchDst) char*    dst, size_t cchDst,
                          int n, unsigned int radix,
                          __deref_opt_out_ecount(*pcchDstEnd) char**    ppDstEnd, __out_ecount_opt(1) size_t* pcchDstEnd, unsigned long dwFlags);

//----------------------------------------------------------------
// NextChar, PrevChar - Walk text by logical characters
// These routines honor 'grapheme' boundaries. They are aware of things like 
// combining characters (e.g. diacritics), surrogate pairs, Hangul syllables 
// formed by Hangul combining Jamo.
//
// They return NULL after reaching the bound of the string or buffer.
//
PCWSTR  WINAPI   NextChar ( __in_opt __in_z PCWSTR pchPoint);                // NUL terminated
PCWSTR  WINAPI   NextChar ( __in_opt __in_z PCWSTR pchPoint, __in_opt PCWSTR pchEnd); // length-bounded
PCWSTR  WINAPI   PrevChar ( __in PCWSTR pchStart, __in PCWSTR pchPoint);

// VS7:32377 non-const variants, like CRT
inline PWSTR WINAPI NextChar ( __in_opt __in_z PWSTR pchPoint                ) {return const_cast<PWSTR>(NextChar (const_cast<PCWSTR>(pchPoint)));}
inline PWSTR WINAPI NextChar ( __in_opt __in_z PWSTR pchPoint, PWSTR pchEnd  ) {return const_cast<PWSTR>(NextChar (const_cast<PCWSTR>(pchPoint), const_cast<PCWSTR>(pchEnd)));}
inline PWSTR WINAPI PrevChar ( __in PWSTR pchStart, __in PWSTR pchPoint) {return const_cast<PWSTR>(PrevChar (const_cast<PCWSTR>(pchStart), const_cast<PCWSTR>(pchPoint)));}

// Returns true if pchPoint is at a grapheme boundary
bool    WINAPI   IsGraphemeBreak ( __in_z PCWSTR pchStart, __in_ecount(1) PCWSTR pchPoint);

//----------------------------------------------------------------
// See docs below
//#define wcsnpbrk FindCharInSet
PCWSTR  WINAPI   FindCharInSet         (PCWSTR pchBuffer, int cchBuffer, PCWSTR set);
PCWSTR  WINAPI   FindCharInOrderedSet  (PCWSTR pchBuffer, int cchBuffer, PCWSTR set);
PCWSTR  WINAPI   FindChar              (PCWSTR psz, WCHAR ch);
PCWSTR  WINAPI   FindCharN             (PCWSTR pchBuffer, int cch, WCHAR ch);
PCWSTR  WINAPI   FindLastChar          (PCWSTR psz, WCHAR ch);
PCSTR   WINAPI   FindLastCharA         (PCSTR  psz, CHAR  ch);
PCWSTR  WINAPI   CharInOrderedSet      (WCHAR ch, PCWSTR set);

// VS7:32377  non-const variants, like CRT
inline PWSTR WINAPI FindCharInSet         (PWSTR pchBuffer, int cchBuffer, PCWSTR set) {return const_cast<PWSTR>(FindCharInSet (const_cast<PCWSTR>(pchBuffer), cchBuffer, set));}
inline PWSTR WINAPI FindCharInOrderedSet  (PWSTR pchBuffer, int cchBuffer, PCWSTR set) {return const_cast<PWSTR>(FindCharInOrderedSet (const_cast<PCWSTR>(pchBuffer), cchBuffer, set));}
inline PWSTR WINAPI FindChar              (PWSTR psz, WCHAR ch) {return const_cast<PWSTR>(FindChar (const_cast<PCWSTR>(psz), ch));}
inline PWSTR WINAPI FindCharN             (PWSTR pchBuffer, int cch, WCHAR ch) {return const_cast<PWSTR>(FindCharN (const_cast<PCWSTR>(pchBuffer), cch, ch));}
inline PWSTR WINAPI FindLastChar          (PWSTR psz, WCHAR ch) {return const_cast<PWSTR>(FindLastChar (const_cast<PCWSTR>(psz), ch));}
inline PSTR  WINAPI FindLastCharA         (PSTR  psz, CHAR  ch) {return const_cast<PSTR>(FindLastCharA (const_cast<PCSTR>(psz), ch));}

//
//
//
void WINAPI PathSplit ( __in_z PCWSTR pszFN, __out_ecount_opt(MAX_PATH) PWSTR pszV, __out_ecount_opt(MAX_PATH) PWSTR pszD, __out_ecount_opt(MAX_PATH) PWSTR pszN, __out_ecount_opt(MAX_PATH) PWSTR pszE);

void WINAPI PathSplitInPlace
(
	__in_z PCWSTR pszPath,
	int iPathLength,
	int *piDrive,
	int *piDriveLength,
	int *piDirectory,
	int *piDirectoryLength,
	int *piFilename,
	int *piFilenameLength,
	int *piExtension,
	int *piExtensionLength
);

//----------------------------------------------------------------
// Calculate the line and character index of an offset into a text buffer
BOOL    WINAPI   LineAndCharIndexOfPos (PCWSTR pchText, int cch, int cchPos, int * piLine, int * piIndex);

//----------------------------------------------------------------
// StrList* operate on empty string-terminated lists of NUL-terminated strings (e.g. filter strings).
//
int     WINAPI   StrListSize   ( __in_z PCWSTR psz); // count of chars to hold the list (includes terminator)
int     WINAPI   StrListSizeA  ( __in_z PCSTR  psz); // count of bytes to hold the list (includes terminator)
int     WINAPI   StrListCount  ( __in_z PCWSTR psz); // count of strings in the list (not including terminator)
int     WINAPI   StrListCountA ( __in_z PCSTR  psz);
int     WINAPI   StrListCounts ( __in_z PCWSTR psz, int * pcStr = NULL); // return count of chars to hold list, *pcStr=count of strings

PCWSTR * WINAPI  StrListCreateArray ( __in_z PCWSTR pList, int * pcel);
PCWSTR  WINAPI   StrListNext        ( __in_z PCWSTR pList);
PCWSTR  WINAPI   StrListFind        ( __in_z PCWSTR pList, __in_z PCWSTR pPattern, bool fCase = true);
PCWSTR  WINAPI   StrListFindSorted  ( __in_z PCWSTR pList, __in_z PCWSTR pPattern, bool fCase = true);

// VS7:32377  non-const variants
inline PWSTR * WINAPI StrListCreateArray ( __in_z PWSTR pList, int * pcel) {return const_cast<PWSTR*>(StrListCreateArray (const_cast<PCWSTR>(pList), pcel));}
inline PWSTR   WINAPI StrListNext        ( __in_z PWSTR pList) {return const_cast<PWSTR>(StrListNext(const_cast<PCWSTR>(pList)));}
inline PWSTR   WINAPI StrListFind        ( __in_z PWSTR pList, __in_z PCWSTR pPattern, bool fCase = true){return const_cast<PWSTR>(StrListFind        (const_cast<PCWSTR>(pList), pPattern, fCase));}
inline PWSTR   WINAPI StrListFindSorted  ( __in_z PWSTR pList, __in_z PCWSTR pPattern, bool fCase = true){return const_cast<PWSTR>(StrListFindSorted  (const_cast<PCWSTR>(pList), pPattern, fCase));}

int     WINAPI   StrSubstituteChar  ( __inout_z PWSTR psz, WCHAR chOld, WCHAR chNew); 
int     WINAPI   StrSubstituteCharA ( __inout_z PSTR  psz, CHAR  chOld, CHAR  chNew); // skips double byte chars

//----------------------------------------------------------------
// !! WARNING!! If you call FindURL or IsProtocol, you must call FreeCachedURLResources()
#define NO_HITTEST -1
BOOL    WINAPI   FindURL (
    __in_z PCWSTR sz,                // IN buffer
    int iLen,                 // IN length of buffer
    int iAt,                  // IN index of point to intersect, or NO_HITTEST
    INT_PTR * piStart,        // IN/OUT starting index to begin scan (IN), start of URL (OUT)
    INT_PTR * piEndProtocol,  // OUT index of end of protocol
    INT_PTR * piEnd           // OUT index of end of URL
    );
BOOL    WINAPI   IsProtocol ( __in_z PCWSTR sz);
void    WINAPI   FreeCachedURLResources (void);

//----------------------------------------------------------------
void    WINAPI   SwapSegments    ( __inout_z PWSTR x, __inout_z PWSTR y, __inout_z PWSTR z);
void    WINAPI   PivotSegments   ( __inout_z PWSTR pA, __inout_z PWSTR pB, __inout_z PWSTR pC, __inout_z PWSTR pD);

#define TRUNC_BEGIN    0 // remove text at the very beginning
#define TRUNC_LEFT     1 // remove text towards the beginning
#define TRUNC_CENTER   2 // remove text from the center
#define TRUNC_RIGHT    3 // remove text towards the end
#define TRUNC_END      4 // remove text from the very end

void    WINAPI   FitText( __in PCWSTR pszText, size_t cchText, __out_ecount(cchDst) PWSTR pszDst, size_t cchDst, __in_z PCWSTR pszFill, DWORD flags);


// NormalizeFileSlashes
// Convert forward slashes to backslashes.
// Reduce multiple slashes to a single slash (leading double slash allowed).
//
// To normalize to forward slashes use StrSubstituteChar(psz, '\\', '/'); after NormalizeFileSlashes
//
// Returns a pointer to the 0 terminator of the transformed string
//
PWSTR   WINAPI   NormalizeFileSlashes  ( __inout_z PWSTR szFile);
PSTR    WINAPI   NormalizeFileSlashesA ( __inout_z PSTR  szFile);

void    WINAPI   TrimWhitespace  ( __inout_z PWSTR psz); // removes leading and trailing whitespace from psz

// Remove blanks adjacent to line ends within a buffer.
#define TLB_START      1
#define TLB_END        2
int     WINAPI   TrimLineBlanks  ( __inout PWSTR pchBuf, int cch, DWORD dwFlags);

enum StripBlanksFlags{
//  --------------------    ------     -----------------------   -------------------------
//  Flag:                   Value:     On input:                 Return value:
//  --------------------    ------     -----------------------   -------------------------
    STB_NONE              = 0x0000, // Remove all blanks.        No blanks found.
    STB_SINGLE            = 0x0001, // Reduce to single blanks.  Only single blanks found.
    STB_MULTIPLE          = 0x0002, // (N/A)                     Multiple blanks found.
    STB_STRIP_LINEBREAKS  = 0x0010  // Include line breaks.      (N/A)
};

StripBlanksFlags  WINAPI  StripBlanks ( __inout_ecount(*plen+1) WCHAR * pchBuf, INT_PTR * plen, int flags = STB_NONE);

// Determine whether a file path is local, UNC, or absolute 
//
// These examine the string only, and only the first part at that.  They don't
// attempt to decide whether the path is a valid filename or not.

BOOL WINAPI IsLocalAbsPath( __in_z PCWSTR sz);   // Path starts with <letter>:
BOOL WINAPI IsUNC( __in_z PCWSTR sz);            // Path starts with two backslashes
BOOL WINAPI IsAbsPath( __in_z PCWSTR sz);        // Either of the above

// Remove Bidi formatting characters from a string if the underlying system does not support them.
// They would be displayed as ? on Win9x systems that don't have appropriate codepage support.

void WINAPI StripUnsupportedBidiFormatChars( __inout_z PWSTR sz);

//-----------------------------------------------------------------
/*
  BOOL FindURL (
    PCWSTR sz,                // IN buffer
    int iLen,                 // IN length of buffer
    int iAt,                  // IN index of point to intersect, or NO_HITTEST
    INT_PTR * piStart,        // IN/OUT starting index to begin scan (IN), start of URL (OUT)
    INT_PTR * piEndProtocol,  // OUT index of end of protocol
    INT_PTR * piEnd           // OUT index of end of URL
    );

  Find an URL in text, starting at *piStart index into wsz.
  iAt is NO_HITTEST to find the first URL in the text.
  To find an URL that intersects a point in the text, iAt is index from wsz of the point.
*/

//-----------------------------------------------------------------
//
// int StrLen (PCWSTR psz);
// Returns: count of chars in string 
//

//-----------------------------------------------------------------
//
// PSTR FindLastCharA (PCSTR psz, CHAR ch);
//
// MBCS-aware version of strrchr.
// Returns: pointer to right-most instance of ch in psz.
//

//-----------------------------------------------------------------
//
// PWSTR FindCharInSet ( __in_ecount(cchBuffer) PCWSTR pchBuffer, int cchBuffer, __in_z PCWSTR set);
//
// Length-limited wide-char version of strpbrk.
// Returns: pointer to first char from set in buffer.
//

//-----------------------------------------------------------------
//
// PWSTR CopyCat (PWSTR dst, PCWSTR src)
//
// Wide char string copy for concatenation. Copy Src to Dst.
// Return: position of NUL in dst for further catenation.
//

//-----------------------------------------------------------------
//
// PWSTR CopyNCat (PWSTR dst, PCWSTR src, int cchz);
//
// NUL-limited char copy up to n chars for catentation
//
// Return: Position after char n in destination or position of 
//         copied NUL for further catenation.
//

//-----------------------------------------------------------------
//
// int StrListSize   (PCWSTR psz); 
//
// Return: count of chars to hold the list (includes terminator)

//-----------------------------------------------------------------
//
// int StrListSizeA  (PCSTR  psz); 
//
// Return: count of bytes to hold the list (includes terminator)

//-----------------------------------------------------------------
//
// int StrListCount  (PCWSTR psz); 
//
// Return: count of strings in the list (not including terminator)

//-----------------------------------------------------------------
//
// int StrListCountA (PCSTR  psz);
//
// Return: count of strings in the list (not including terminator)

//-----------------------------------------------------------------
//
// int StrListCounts (PCWSTR psz, int * pcStr = NULL); 
//
// pcStr    Receives count of strings in the list (not including terminator)
//
// Return: return count of chars to hold list
//

//-----------------------------------------------------------------
//
// PCWSTR * StrListCreateArray (PCWSTR pList, int * pcel);
//
// pcel     Receives a count of elements in the returned array,
//          not including the terminating NULL entry.
//
// Return: NULL-terminated array of PWSTRs pointing to the strings 
//         in pList, or NULL if out of memory.
//
// You must free the returned array using VSFree
//

//-----------------------------------------------------------------
//
// PCWSTR StrListNext (PCWSTR pList);
//
// Return: Pointer to the next non-empty string in the list, or 
//         NULL if no more strings
//

//-----------------------------------------------------------------
//
// HRESULT StrListSort (PWSTR pList, bool fCase = true);
//
// Sorts the string list
//
// fCase    true for case-sensitive, false for case-insensitive
//
// Return: success
//

//-----------------------------------------------------------------
//
// HRESULT StrListSortPairs (PWSTR pList, bool fCase = true);
//
// Sorts the paired-string (key/value) list. There must be an even 
// number of strings in the list. Keys and values must not contain L'\1'.
//
// fCase    true for case-sensitive, false for case-insensitive
//
// Return: success
//

//-----------------------------------------------------------------
//
// PCWSTR StrListFind (PCWSTR pList, PCWSTR pPattern, bool fCase = true);
//
// pList     Sorted list of strings to search
// pPattern  String to find in list
// fCase     true to match exact case, false to ignore case 
//
// Return: Matching string in list or NULL if not found
//

//-----------------------------------------------------------------
//
// PCWSTR StrListFindSorted (PCWSTR pList, PCWSTR pPattern, bool fCase = true);
//
// pList     Sorted list of strings to search
// pPattern  String to find in list
// fCase     true to match exact case, false to ignore case 
//           The list must be sorted in ascending order with the same fCase
//
// Return: Matching string in list or NULL if not found
//

//-----------------------------------------------------------------
//
// int StrSubstituteChar  (PWSTR psz, WCHAR chOld, WCHAR chNew); 
// int StrSubstituteCharA (PSTR  psz, CHAR  chOld, CHAR  chNew); // skips double byte chars
//
// Return: Count of chars replaced
//

//-----------------------------------------------------------------
//
// void TrimWhitespace (PWSTR psz);
//
// Remove whitespace from the start and end of the string
//

//-----------------------------------------------------------------
//
// int TrimLineBlanks (PWSTR pchBuf, int cch, DWORD dwFlags);
//
// Remove blanks adjacent to line ends within a buffer.
//
// cch == -1 for zero-terminated string.
// Set TLB_START in dwFlags to also remove blanks at the start of the buffer.
// Set TLB_END in dwFlags to also remove blanks at the end of the buffer.
// 
// Returns trimmed length (if cch == -1, length includes terminator)
// 

//-----------------------------------------------------------------
//
// void WINAPI SwapSegments(PWSTR x, PWSTR y, PWSTR z);
//
// Swap two segments of a string.
//
//  IN: xxxxxYYYz
// OUT: YYYxxxxxz
//
// Z can point to the zero terminator or past the end of the buffer.
// The swap is performed in-place.
//

//-----------------------------------------------------------------
// void PivotSegments (PWSTR pA, PWSTR pB, PWSTR pC, PWSTR pD);
//
// Pivot two segments of a string around a middle segment.
//
//  IN: aaaaaaaBBcccccD
// OUT: cccccBBaaaaaaaD
//
// D can point to the zero terminator or past the end of the buffer.
// The pivot is performed in-place.
//

//=================================================================
//===== Implementation ============================================
//=================================================================

inline int WINAPI StrLen ( __in_opt __in_z PCWSTR psz)
{
    if (!psz) return 0;

    return (int)wcslen(psz);
}

inline int WINAPI StrLenA ( __in_opt __in_z PCSTR psz) 
{ 
    if (!psz) return 0;

    return (int) strlen(psz);
}

inline PWSTR WINAPI CopyCat ( __out_z PWSTR dst, __in_z PCWSTR src)
{
    while ((*dst++ = *src++))
        ;
    return --dst; 
}

// Ansi version
inline PSTR WINAPI CopyCatA ( __out_z PSTR dst, __in_z PCSTR src)
{
    while ((*dst++ = *src++))
        ;
    return --dst; 
}

// Guarantees zero termination. Can write one more than cchz
inline PWSTR WINAPI CopyNCat ( __out_ecount(cchz+1) PWSTR dst, __in_z PCWSTR src, int cchz)
{
    WCHAR ch = 0xFFFF; // UCH_NONCHAR
    while (cchz-- && (ch = *dst++ = *src++) != 0)
        ;
    if (ch)
    {
        *dst = 0;
        return dst;
    }
    else
        return --dst;
}


// Ansi version
inline PSTR WINAPI CopyNCatA ( __out_ecount(cchz+1) PSTR dst, __in_z PCSTR src, int cchz)
{
    CHAR ch = 1;
    while (cchz-- && (ch = *dst++ = *src++))
        ;
    if (ch)
    {
        *dst = 0;
        return dst;
    }
    else
        return --dst;
}

inline PWSTR WINAPI CopyCatInt ( __out_z PWSTR dst, int n, int radix)
{
#ifdef _MSC_VER
/* TEMP (alecont): Temporarly ignore Secure CRT deprecation */
#pragma warning(push)
#pragma warning(disable:4996)
#endif
	_itow(n, dst, radix);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
	while (*dst)
		dst++;
	return dst;
}


//Do not deprecate these functions yet: there are still a small number of people using them
//whose code is, by inspection, safe. Deprecate or remove post RTM, however, to avoid problems
//in the future.

inline PCWSTR WINAPI FindChar ( __in_z PCWSTR psz, WCHAR ch)
{
    if (!psz) 
        return NULL;
    WCHAR T;
    do
	{
        T = *psz;
        if (T == ch)
            return psz;
        psz++;
    } while (T);
    return NULL;
}

inline PCWSTR WINAPI FindSlash ( __in_z PCWSTR psz)
{
    if (!psz) 
        return NULL;
    WCHAR T;
    do
	{
        T = *psz;
        if ((T == L'\\') || (T == L'/'))
            return psz;
        psz++;
    } while (T);
    return NULL;
}

inline PCWSTR WINAPI FindCharN ( __in_ecount(cch) PCWSTR pchBuffer, int cch, WCHAR ch)
{
    if (!pchBuffer) 
        return NULL;
    while (cch--)
    {
        if (ch == *pchBuffer)
            return pchBuffer;
        ++pchBuffer;
    }
    return NULL;
}

inline PCWSTR WINAPI FindLastChar ( __in_z PCWSTR  psz, WCHAR ch)
{
    if (!psz) 
        return NULL;
    PCWSTR pch = NULL;
    WCHAR T;
    for (; (T = *psz); psz++)
    {
        if (T == ch)
            pch = psz;
    }
    return pch;
}

inline PCWSTR WINAPI FindLastSlash ( __in_z PCWSTR  psz, int iLength)
{
    if (!psz || !iLength) 
        return NULL;

    for (PCWSTR pch = psz + (iLength-1); pch >= psz; pch--)
    {
        if ((*pch == L'\\') || (*pch == L'/'))
            return pch;
    }
    return NULL;
}

inline BOOL WINAPI IsLocalAbsPath ( __in_z PCWSTR sz)
{
    return ((sz[0] >= L'A' && sz[0] <= L'Z') || (sz[0] >= L'a' && sz[0] <= L'z'))
        && sz[1] == L':';
}

inline BOOL WINAPI IsUNC ( __in_z PCWSTR sz)
{
    return sz[0] == L'\\' && sz[1] == L'\\';
}

inline BOOL WINAPI IsAbsPath (PCWSTR sz)
{
    return IsLocalAbsPath(sz) || IsUNC(sz);
}

#endif // __UNISTR_H__

