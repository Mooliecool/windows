// unistr.cpp
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// June 1, 1998 [paulde]
//
//-----------------------------------------------------------------

#include "pch.h"
#include "unistr.h"
#include "vsmem.h"
#include "dbgout.h"
#include "textmgr2.h"
#include "strsafe.h"

/*

    dwFlags         -   controls some details of the string copy:

        STRSAFE_FILL_BEHIND_NULL
                    if the function succeeds, the low byte of dwFlags will be
                    used to fill the uninitialize part of destination buffer
                    behind the null terminator

        STRSAFE_IGNORE_NULLS
                    treat NULL string pointers like empty strings (TEXT(""))

        STRSAFE_FILL_ON_FAILURE
                    if the function fails, the low byte of dwFlags will be
                    used to fill all of the destination buffer, and it will
                    be null terminated. This will overwrite any truncated 
                    string returned when the failure is
                    STRSAFE_E_INSUFFICIENT_BUFFER

        STRSAFE_NO_TRUNCATION /
        STRSAFE_NULL_ON_FAILURE
                    if the function fails, the destination buffer will be set
                    to the empty string. This will overwrite any truncated string
                    returned when the failure is STRSAFE_E_INSUFFICIENT_BUFFER.


*/
///////////////////////////////////////////////////////////////////////////

int __cdecl BufPrintArgs(DWORD dwFlags, _Out_z_cap_(cchBuf) LPWSTR pwszBuf, size_t cchBuf, _In_z_ LPCWSTR pwszFormat, va_list argList)
{
    int nRet = 0;

    if (cchBuf <= STRSAFE_MAX_CCH)
    {
        WCHAR* pwszLast = NULL;
        size_t cchLeft = 0;
        HRESULT hr = StringCchVPrintfExW(
                                    pwszBuf,
                                    cchBuf,
                                    &pwszLast,   // LPCWSTR* pointer the pointer to the last character copied
                                    &cchLeft,        // size_t*  size left in the buffer after the copy
                                    dwFlags,
                                    pwszFormat, 
                                    argList);
        
        if (SUCCEEDED(hr))
            nRet = (int)(cchBuf-cchLeft);
    }

    return nRet;
}

int __cdecl BufPrint( _Out_cap_(cchBuf) LPWSTR pwszBuf, size_t cchBuf, _In_z_ LPCWSTR pwszFormat, ...)
{
    int nRet = 0;
    va_list argList;

    va_start(argList, pwszFormat);
#ifdef _DEBUG
    nRet = BufPrintArgs(STRSAFE_FILL_BEHIND_NULL | 0xcc, pwszBuf, cchBuf, pwszFormat, argList);
#else
    nRet = BufPrintArgs(0, pwszBuf, cchBuf, pwszFormat, argList);
#endif
    va_end(argList);
    
    return nRet;
}


// non unicode versions


int __cdecl BufPrintArgsA(DWORD dwFlags, _Out_cap_(cchBuf) LPSTR pszBuf, size_t cchBuf, _In_z_ LPCSTR pszFormat, va_list argList)
{
    int nRet = 0;

    if (cchBuf <= STRSAFE_MAX_CCH)
    {
        CHAR* pszLast = NULL;
        size_t cchLeft = 0;
        HRESULT hr = StringCchVPrintfExA(
                                    pszBuf,
                                    cchBuf,
                                    &pszLast,    // LPCSTR* pointer the pointer to the last character copied
                                    &cchLeft,        // size_t*  size left in the buffer after the copy
                                    dwFlags,
                                    pszFormat, 
                                    argList);
        
        if (SUCCEEDED(hr))
            nRet = (int)(cchBuf-cchLeft);
    }

    return nRet;
}

int __cdecl BufPrintA( _Out_cap_(cchBuf) LPSTR pszBuf, size_t cchBuf, _In_z_ LPCSTR pszFormat, ...)
{
    int nRet = 0;
    va_list argList;

    va_start(argList, pszFormat);
#ifdef _DEBUG
    nRet = BufPrintArgsA(STRSAFE_FILL_BEHIND_NULL | 0xcc, pszBuf, cchBuf, pszFormat, argList);
#else
    nRet = BufPrintArgsA(0, pszBuf, cchBuf, pszFormat, argList);
#endif
    va_end(argList);
    
    return nRet;
}




//----------------------------------------------------------------
PWSTR WINAPI CopyCase ( _In_z_ PCWSTR pSrc, _Inout_z_ PWSTR pDst)
{
  if (pSrc == NULL || pDst == NULL)
  {
    ASSERT(FALSE);
    return NULL;
  }

  // $
  WCHAR chSrc, chDst;
  while ((chSrc = *pSrc) && (chDst = *pDst))
  {
    if (IsUppercaseLetter(chSrc))
      *pDst = UpperCase(chDst);
    else if (IsLowercaseLetter(chSrc))
      *pDst = LowerCase(chDst);
    ++ pSrc;
    ++ pDst;
  }
  return pDst;
}

//-----------------------------------------------------------------
#define JamoOther 0
#define JamoLead  1
#define JamoVowel 2
#define JamoTrail 3

inline int HangulJamoType (WCHAR ch)
{
    if (ch < UCH_HANGUL_JAMO_FIRST) return JamoOther;
    if (ch > UCH_HANGUL_JAMO_LAST ) return JamoOther;
    if (ch <= UCH_HANGUL_JAMO_LEAD_LAST ) return JamoLead;
    if (ch <= UCH_HANGUL_JAMO_VOWEL_LAST) return JamoVowel;
    return JamoTrail;
}

// Returns true if pchPoint is at a grapheme boundary
bool WINAPI IsGraphemeBreak ( _In_z_ PCWSTR pchStart, _In_count_(1) PCWSTR pchPoint)
{
    if (pchStart == NULL || pchPoint == NULL)
    {
        ASSERT(FALSE);
        return true;
    }

    if (pchPoint <= pchStart) return true;

    WCHAR cL, cR;

    cR = *pchPoint;
    if (!cR) return true;

    cL = *(pchPoint-1);
    if (!cL) return true;

    // break around line breaks
    if (IsLineBreak(cL) || IsLineBreak(cR)) return true; 

    // don't separate a combining char from it's base
    if (IsCombining(cR))
    {
        // A combining character after a quote or whitespace does not combine with the quote or whitespace
        if ( cL != L'\'' && cL != L'\"' && !IsWhitespace(cL)  &&  iswascii(cL) )
        {
            return false;
        }

    }

    // don't break surrogate pairs
    if (IsHighSurrogate(cL))
    {
        return !IsLowSurrogate(cR); 
    }

    int hjL = HangulJamoType(cL);
    int hjR = HangulJamoType(cR);
    if (hjL && hjR)
    {
        switch (hjL)
        {
        case JamoLead : return false;
        case JamoVowel: return JamoLead  == hjR;
        case JamoTrail: return JamoTrail != hjR;
        default:__assume(0);
        }
    }
    return true;
}

// Aware of combining chars, non-breaking chars, and surrogate pairs
// Returns NULL when *pch == 0
PCWSTR WINAPI NextChar ( _In_opt_z_ PCWSTR pch)
{
    if (!pch) return NULL;
    if (!*pch) return NULL;
    PCWSTR p = pch+1;
    while (!IsGraphemeBreak(pch, p))
        ++p;
    return p;
}

// Does NOT stop at nulls
PCWSTR WINAPI NextChar ( _In_opt_z_ PCWSTR pch, _In_opt_z_ PCWSTR pchEnd)
{
    if (!pch || !pchEnd) return NULL;
    if (pch >= pchEnd) return NULL;
    PCWSTR p = pch+1;
    while ((p < pchEnd) && !IsGraphemeBreak(pch, p))
        ++p;
    return p;
}

PCWSTR WINAPI PrevChar ( _In_ PCWSTR pchStart, _In_ PCWSTR pch)
{
    if (pch <= pchStart) return NULL;
    --pch;
    while (!IsGraphemeBreak(pchStart, pch))
        --pch;
    return pch;
}

void WINAPI PathSplit ( _In_z_ PCWSTR pszFN, _Out_opt_cap_(MAX_PATH) PWSTR pszV, _Out_opt_cap_(MAX_PATH) PWSTR pszD, _Out_opt_cap_(MAX_PATH) PWSTR pszN, _Out_opt_cap_(MAX_PATH) PWSTR pszE)
{
    SETRETVAL(pszV, 0);
    SETRETVAL(pszD, 0);
    SETRETVAL(pszN, 0);
    SETRETVAL(pszE, 0);
    if (!pszFN || !*pszFN) return;

    PWSTR pch, pch2;
    WCHAR szFile[MAX_PATH+1];

    // make a copy we can modify
    VERIFY(SUCCEEDED(StringCchCopyW(szFile, _countof(szFile), pszFN)));
    NormalizeFileSlashes(szFile);

    // extension
    // start after last slash, if any
    pch2 = FindLastChar(szFile, L'\\');
    if (!pch2)
        pch2 = szFile;
    pch = FindLastChar(pch2, L'.');
    if (pch)
    {
        if (pszE)
        {
            VERIFY(SUCCEEDED(StringCchCopyW(pszE, MAX_PATH, pch)));
        }
        *pch = 0;
    }

    // drive spec
    if (IsLocalAbsPath(szFile))
    {
        if (pszV)
        {
            pszV[0] = szFile[0];
            pszV[1] = L':';
            pszV[2] = 0;
        }
        pch = szFile + 2;
    }
    else
    // UNC
    if (IsUNC(szFile))
    {
        pch = FindChar(szFile + 2, L'\\');
        if ((NULL == pch) || 
            (NULL == (pch = FindChar(pch + 1, L'\\'))))
        {
            if (pszV)
            {
                VERIFY(SUCCEEDED(StringCchCopyW(pszV, MAX_PATH, szFile)));
            }
            return;
        }
        *pch = 0;
        if (pszV)
        {
            VERIFY(SUCCEEDED(StringCchCopyW(pszV, MAX_PATH, szFile)));
        }
        *pch = L'\\';
    }
    else
        pch = szFile;

    PWSTR pchStart = pch;
    pch = FindChar(pchStart, 0) - 1;
    if (pch < pchStart) return;

    // filename
    while (pch > pchStart && *pch != L'\\')
        pch--;
    if (pch >= pchStart)
    {
        if (*pch == L'\\')
            pch++;

        if (pszN)
        {
            VERIFY(SUCCEEDED(StringCchCopyW(pszN, MAX_PATH, pch)));
        }
        *pch = 0;
    }
    // dir
    if (pszD)
    {
        VERIFY(SUCCEEDED(StringCchCopyW(pszD, MAX_PATH, pchStart)));
    }

    return;
}

/*-----------------------------------------------------------------------------
Name: PathSplitInPlace

@func

Parses a path string in-place to discover its constituent components.

@owner [....]
-----------------------------------------------------------------------------*/
void WINAPI                 // @rdesc nothing
PathSplitInPlace
(
    _In_z_ PCWSTR pszPath,         // @parm [in] path string to parse in-place
    int iPathLength,        // @parm [in] length of the path
    int *piDrive,           // @parm [out] start of "drive" portion of path
    int *piDriveLength,     // @parm [out] length of "drive" portion of path
    int *piDirectory,       // @parm [out] start of "directory" portion of path
    int *piDirectoryLength, // @parm [out] length of "directory" portion of path
    int *piFilename,        // @parm [out] start of "filename" portion of path
    int *piFilenameLength,  // @parm [out] length of "filename" portion of path
    int *piExtension,       // @parm [out] start of "extension" portion of path
    int *piExtensionLength  // @parm [out] length of "extension" portion of path
)
{
    SETRETVAL(piDrive, -1);
    SETRETVAL(piDriveLength, 0);
    SETRETVAL(piDirectory, -1);
    SETRETVAL(piDirectoryLength, 0);
    SETRETVAL(piFilename, -1);
    SETRETVAL(piFilenameLength, 0);
    SETRETVAL(piExtension, -1);
    SETRETVAL(piExtensionLength, 0);

    if (!pszPath || !iPathLength || !*pszPath)
    {
        return;
    }

    PCWSTR pch;
    PCWSTR pszExtension = pszPath + iPathLength;

    if (piDirectory || piDirectoryLength || piFilename || piFilenameLength || piExtension || piExtensionLength)
    {
        // extension
        // start after last slash, if any
        PCWSTR pch2 = FindLastSlash(pszPath, iPathLength);
        if (!pch2)
            pch2 = pszPath;
        pch = FindLastChar(pch2, L'.');
        if (pch)
        {
            pszExtension = pch;
            SETRETVAL(piExtension, static_cast<int>(pch-pszPath));
            SETRETVAL(piExtensionLength, static_cast<int>((pszPath+iPathLength)-pch));
        }
    }

    if (piDrive || piDriveLength || piDirectory || piDirectoryLength || piFilename || piFilenameLength)
    {
        // drive spec
        if (IsLocalAbsPath(pszPath))
        {
            SETRETVAL(piDrive, 0);
            SETRETVAL(piDriveLength, 2);
            pch = pszPath + 2;
        }
        else
        // UNC
        if (IsUNC(pszPath) || (pszPath[0] == L'/' && pszPath[1] == L'/'))
        {
            pch = FindSlash(pszPath + 2);
            if ((NULL == pch) || 
                (NULL == (pch = FindSlash(pch + 1))))
            {
                SETRETVAL(piDrive, 0);
                SETRETVAL(piDriveLength, iPathLength);
                return;
            }
            SETRETVAL(piDrive, 0);
            SETRETVAL(piDriveLength, static_cast<int>(pch-pszPath));
        }
        else
            pch = pszPath;
    }

    if (piDirectory || piDirectoryLength || piFilename || piFilenameLength)
    {
        PCWSTR pchStart = pch;
        pch = pszExtension - 1;
        if (pch < pchStart) return;

        // filename
        PCWSTR pszFilename = pszExtension;
        while (pch > pchStart && *pch != L'\\' && *pch != L'/')
            pch--;
        if (pch >= pchStart)
        {
            if (*pch == L'\\' || *pch == L'/')
                pch++;
            pszFilename = pch;
            SETRETVAL(piFilename, static_cast<int>(pch-pszPath));
            SETRETVAL(piFilenameLength, static_cast<int>(pszExtension-pch));
        }
        // dir
        SETRETVAL(piDirectory, static_cast<int>(pchStart-pszPath));
        SETRETVAL(piDirectoryLength, static_cast<int>(pszFilename-pchStart));
    }
    return;
}

//-----------------------------------------------------------------
int WINAPI StrListSize ( _In_z_ PCWSTR psz)
{
    if (!psz) return 0;
    PCWSTR pch = psz;
    for (;;)
    {
        if (*pch)
            pch++;
        else
        {
            pch++;
            if (!*pch)
                break;
        }
    }
    return 1 + (LONG32)(pch - psz);
}

//-----------------------------------------------------------------
int WINAPI StrListCount ( _In_z_ PCWSTR psz)
{
    if (!psz) return 0;
    int cel = 0;
    for (;;)
    {
        if (*psz)
            psz++;
        else
        {
            cel++;
            psz++;
            if (!*psz)
                break;
        }
    }
    return cel;
}

//-----------------------------------------------------------------
int WINAPI StrListSizeA ( _In_z_ PCSTR psz)
{
    if (!psz) return 0;
    PCSTR pch = psz;
    for (;;)
    {
        if (*pch)
            pch++;
        else
        {
            pch++;
            if (!*pch)
                break;
        }
    }
    return 1 + (int)(pch - psz);
}

//-----------------------------------------------------------------
int WINAPI StrListCountA ( _In_z_ PCSTR psz)
{
    if (!psz) return 0;
    int cel = 0;
    for (;;)
    {
        if (*psz)
            psz++;
        else
        {
            cel++;
            psz++;
            if (!*psz)
                break;
        }
    }
    return cel;
}

//-----------------------------------------------------------------
// StrListCounts - Calculate memory size and number of strings in a StrList
//
// psz      String list
// piStr    Receives count of strings in the list
// Returns  Count of WCHARS to hold the list (including terminators)
//
int WINAPI StrListCounts ( _In_z_ PCWSTR psz, int * piStr)
{
    SETRETVAL(piStr, 0);
    if (!psz) return 0;

    PCWSTR  pch = psz;
    int     cel = 0;
    for (;;)
    {
        if (*pch)
            pch++;
        else
        {
            cel++;
            pch++;
            if (!*pch)
                break;
        }
    }
    SETRETVAL(piStr, cel);
    return 1 + (int)(pch - psz);
}

//-----------------------------------------------------------------
PCWSTR * WINAPI StrListCreateArray ( _In_z_ PCWSTR pList, int * pcel)
{
    SETRETVAL(pcel,0);
    if (!pList) return NULL;

    int cel = StrListCount(pList);
    SETRETVAL(pcel,cel);

    if (cel)
    {
        if (cel + 1 < cel)
            return NULL; // int overflow check

        PCWSTR * rgStr = VSALLOCTYPE(PCWSTR, cel + 1);
        if (rgStr)
        {
            int i = 0;
            for (i = 0; i < cel; i++)
            {
                rgStr[i] = pList;
                pList += StrLen(pList) + 1;
            }
            rgStr[i] = NULL;
        }
        return rgStr;
    }
    return NULL;
}

//-----------------------------------------------------------------
PCWSTR WINAPI StrListNext ( _In_z_ PCWSTR pList)
{
    return (pList && *pList) ? (1 + StrLen(pList) + pList) : NULL;
}

typedef int (WINAPI * FNCOMPARE) (PCWSTR, PCWSTR);

//-----------------------------------------------------------------
PCWSTR WINAPI StrListFind ( _In_z_ PCWSTR pList, _In_z_ PCWSTR pPattern, bool fCase/*= true*/)
{
    FNCOMPARE Compare = fCase ? CompareCase : CompareNoCase;
    while (pList)
    {
        if (0 == Compare(pList, pPattern))
            return pList;
        pList = StrListNext(pList);
    }
    return NULL;
}

//-----------------------------------------------------------------
PCWSTR WINAPI StrListFindSorted ( _In_z_ PCWSTR pList, _In_z_ PCWSTR pPattern, bool fCase/*= true*/)
{
    FNCOMPARE Compare = fCase ? CompareCase : CompareNoCase;
    while (pList)
    {
        switch (Compare(pPattern, pList))
        {
        case -1: break;
        case  0: return (PWSTR)pList;
        case  1: return NULL;
        default: __assume(0);
        }
        pList = StrListNext(pList);
    }
    return NULL;
}

//-----------------------------------------------------------------
int WINAPI StrSubstituteChar ( _Inout_z_ PWSTR psz, WCHAR chOld, WCHAR chNew)
{
    int count = 0;
    while (*psz)
    {
        if (chOld == *psz)
        {
            count++;
            *psz = chNew;
        }
        psz++;
    }
    return count;
}

//-----------------------------------------------------------------
int WINAPI StrSubstituteCharA ( _Inout_z_ PSTR psz, CHAR chOld, CHAR chNew)
{
    int count = 0;
    while (*psz)
    {
        if (IsDBCSLeadByte(*psz))
            psz += 2;
        else
        {
            if (chOld == *psz)
            {
                count++;
                *psz = chNew;
            }
            psz++;
        }
    }
    return count;
}

//-----------------------------------------------------------------
PCSTR WINAPI FindLastCharA ( _In_z_ PCSTR psz, CHAR ch)
{
    if (NULL == psz) return NULL;

    PCSTR pszRet = NULL;
    while (*psz)
    {
        if (*psz == ch)
            pszRet = psz;
        psz = CharNextA(psz);
    }
    return pszRet;
}

//-----------------------------------------------------------------
// Essentially _wcsnpbrk
PCWSTR WINAPI FindCharInSet ( _In_opt_count_(cchBuffer) PCWSTR pchBuffer, int cchBuffer, _In_opt_ PCWSTR set)
{
    if (!pchBuffer || !set)
        return NULL;
    WCHAR chBuf, chSet;
    if (-1 == cchBuffer)
    {
        while (chBuf = *pchBuffer)
        {
            PCWSTR pchSet = set;
            while (chSet = *pchSet++)
                if (chSet == chBuf)
                    return pchBuffer;
            pchBuffer++;
        }
    }
    else
    {
        while (cchBuffer--)
        {
            chBuf = *pchBuffer;
            PCWSTR pchSet = set;
            while (chSet = *pchSet++)
                if (chSet == chBuf)
                    return pchBuffer;
            pchBuffer++;
        }
    }
    return NULL;
}

#ifdef _DEBUG
void ValidateSet( _In_z_ PCWSTR set)
{
    // validate that the set is in ascending order and entries are unique
    PCWSTR pch1, pch2;
    pch1 = pch2 = set;
    if (*pch2++)
    {
        while (*pch2)
        {
            UASSERT(*pch1 < *pch2); // Malformed char set
            ++pch1;
            ++pch2;
        }
    }
}
#define VALIDATESET(set) ValidateSet(set)
#else
#define VALIDATESET(set)
#endif

PCWSTR WINAPI CharInOrderedSet (WCHAR ch, _In_opt_z_ PCWSTR set)
{
    if (!set || !*set) return NULL;
    VALIDATESET(set);

    WCHAR chSet;
    while ((chSet = *set) && (chSet < ch))
        ++set;
    return (chSet == ch) ? set : NULL;
}


PCWSTR WINAPI FindCharInOrderedSet ( _In_count_(cchBuffer) PCWSTR pchBuffer, int cchBuffer, _In_z_ PCWSTR set)
{
    if (!pchBuffer || !cchBuffer || !set || !*set) return NULL;
    VALIDATESET(set);

    WCHAR   chBuf, chSet;
    PCWSTR  pSet;
    if (-1 == cchBuffer)
    {
        while (chBuf = *pchBuffer)
        {
            pSet = set;
            while ((chSet = *pSet) && (chSet < chBuf))
                ++pSet;
            if (chSet == chBuf)
                return pchBuffer;
            pchBuffer++;
        }
    }
    else
    {
        PCWSTR pchEnd = pchBuffer + cchBuffer;
        while (pchBuffer < pchEnd)
        {
            chBuf = *pchBuffer;
            pSet  = set;
            while ((chSet = *pSet) && (chSet < chBuf))
                ++pSet;
            if (chSet == chBuf)
                return pchBuffer;
            pchBuffer++;
        }
    }
    return NULL;
}

/////////////////////////////////////////////////////////////////
// LineAndCharIndexOfPos
// calculate the line and character index of an offset into a text buffer
BOOL WINAPI LineAndCharIndexOfPos( _In_count_(cch) PCWSTR pchText, int cch, int cchPos, int * piLine, int * piIndex)
{
    BOOL fRet = FALSE;
    int iLine = 0;
    int iIndex = 0;
    
    if (pchText != NULL)
    {
        if (cchPos >= 0 && cchPos < cch)
        {
            PCWSTR pchScan = pchText;
            PCWSTR pchPos  = pchText + cchPos;
            while (pchScan < pchPos)
            {
                switch (*pchScan++)
                {
                case UCH_LF:
                    goto _CountLine;
                    break;

                case UCH_CR:
                    if ((pchScan < pchPos) && (UCH_LF == *pchScan))
                        pchScan++;
                    // fall through
                case UCH_LS:
                case UCH_PS:
                case UCH_NEL:
_CountLine:
                    iLine++;
                    iIndex = 0;
                    break;

                default:
                    iIndex++;
                }
            }
        }
    }
    if (piLine) *piLine = iLine;
    if (piIndex) *piIndex = iIndex;
    return fRet;
}


// Remove Bidi formatting characters from a string if the underlying system does not support them.
// They would be displayed as ? on Win9x systems that don't have appropriate codepage support.

void WINAPI StripUnsupportedBidiFormatChars( _Inout_z_ PWSTR sz)
{
    static bool fInited = false;
    static bool fUnsupportedBidiFormatChars;

    static WCHAR szBidiFormatChars[] = { UCH_ZWNJ, UCH_ZWJ, UCH_LTR, UCH_RTL, UCH_NULL };
    const int cchFormatChars = _countof(szBidiFormatChars) - 1;
    static bool rgfSupported[cchFormatChars];

#ifdef _DEBUG
    // Make sure characters are ordered, as required by CharInOrderedSet
    {
        for (int i = 1; i < cchFormatChars; i++)
        {
            ASSERT(szBidiFormatChars[i - 1] < szBidiFormatChars[i]);
        }
    }
#endif  // _DEBUG

    if (!fInited)
    {
        if (W_IsUnicodeSystem())
        {
            fUnsupportedBidiFormatChars = false;
        }
        else
        {
            char szConverted1[cchFormatChars];
            char chDefault1 = '\xAA';
            char szConverted2[cchFormatChars];
            char chDefault2 = '\x55';
            BOOL fDefaultUsed;

            int cchConverted = WideCharToMultiByte(CP_ACP, 0, szBidiFormatChars, cchFormatChars, 
                szConverted1, sizeof(szConverted1), &chDefault1, &fDefaultUsed);
            ASSERT(cchConverted == sizeof(szConverted1));

            if (!fDefaultUsed)
            {
                fUnsupportedBidiFormatChars = false;
            }
            else
            {
                cchConverted = WideCharToMultiByte(CP_ACP, 0, szBidiFormatChars, cchFormatChars, 
                    szConverted2, sizeof(szConverted2), &chDefault2, &fDefaultUsed);
                ASSERT(cchConverted == sizeof(szConverted2));

                ASSERT(fDefaultUsed);
                for (int i = 0; i < cchFormatChars; i++)
                {
                    rgfSupported[i] = (szConverted1[i] == chDefault1) && (szConverted2[i] == chDefault2);
                }

                fUnsupportedBidiFormatChars = true;
            }
        }

        fInited = true;
    }

    if (fUnsupportedBidiFormatChars)
    {
        PWSTR szDest = sz;
        while (*sz != UCH_NULL)
        {
            if (CharInOrderedSet(*sz, szBidiFormatChars) == NULL)
            {
                *szDest++ = *sz;
            }
            sz++;
        }
        *szDest = UCH_NULL;
    }
}

HRESULT StringCchIntExW  (_Out_cap_(cchDst) wchar_t*  dst, size_t cchDst,
                          int n, unsigned int radix,
                          _Out_opt_ _Deref_post_count_(*pcchDstEnd) wchar_t** ppDstEnd, _Out_opt_cap_(1) size_t* pcchDstEnd, unsigned long dwFlags)
{
    //Test for things that will upset _itow_s (we don't want an exception thrown for a bad radix)
    if ((radix >= 2) && (radix <= 36))
    {
        //Unfortunately, we can't, directly, use _itow_s (it asserts on a buffer overflow, we want safe termination without the assert).
        //So, write to a local buffer that will be large enough (base 2 + terminator)
        wchar_t  bfr[sizeof(n) * 8 + 1] = L"";
        _itow_s(n, bfr, _countof(bfr), radix);

        return StringCchCopyExW(dst, cchDst, bfr, ppDstEnd, pcchDstEnd, dwFlags);
    }
    else
    {
        //This is the failure case: we assert here, copy nothing over (but use StringCchCopyExW() so dwFlags gets interpreted correctly
        //and to force zero-termination). Return an error even if StringCchCopyExW() succeeds.
        ASSERT(radix >= 2);
        ASSERT(radix <= 36);

        StringCchCopyExW(dst, cchDst, L"", ppDstEnd, pcchDstEnd, dwFlags);

        return E_INVALIDARG;
    }
}

HRESULT StringCchIntExA  (_Out_cap_(cchDst) char*    dst, size_t cchDst,
                          int n, unsigned int radix,
                          _Out_opt_ _Deref_post_count_(*pcchDstEnd) char**    ppDstEnd, _Out_opt_cap_(1) size_t* pcchDstEnd, unsigned long dwFlags)
{
    //Test for things that will upset _itoa_s (we don't want an exception thrown for a bad radix)
    if ((radix >= 2) && (radix <= 36))
    {
        //Unfortunately, we can't, directly, use _itoa_s (it asserts on a buffer overflow, we want safe termination without the assert).
        //So, write to a local buffer that will be large enough (base 2 + terminator)
        char   bfr[sizeof(n) * 8 + 1] = "";
        _itoa_s(n, bfr, _countof(bfr), radix);

        return StringCchCopyExA(dst, cchDst, bfr, ppDstEnd, pcchDstEnd, dwFlags);
    }
    else
    {
        //This is the failure case: we assert here, copy nothing over (but use StringCchCopyExA() so dwFlags gets interpreted correctly
        //and to force zero-termination). Return an error even if StringCchCopyExA() succeeds.
        ASSERT(radix >= 2);
        ASSERT(radix <= 36);

        StringCchCopyExA(dst, cchDst, "", ppDstEnd, pcchDstEnd, dwFlags);

        return E_INVALIDARG;
    }
}
