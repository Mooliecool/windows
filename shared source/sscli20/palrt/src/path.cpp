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
// File: path.cpp
//
// Path APIs ported from shlwapi (especially for Fusion)
// ===========================================================================

#include "shlwapip.h"

#define CH_SLASH TEXT('/')
#define CH_WHACK TEXT('\\')

//
// Inline function to check for a double-backslash at the
// beginning of a string
//

static __inline BOOL DBL_BSLASH(LPCWSTR psz)
{
    return (psz[0] == TEXT('\\') && psz[1] == TEXT('\\'));
}

//
// Inline function to check for a path separator character.
//

static __inline BOOL IsPathSeparator(WCHAR ch)
{
    return (ch == CH_SLASH || ch == CH_WHACK);
}

STDAPI_(BOOL) PathAppendW(LPWSTR pszPath, LPCWSTR pszMore)
{
    RIPMSG(pszPath && IS_VALID_STRING_PTR(pszPath, -1) && IS_VALID_WRITE_BUFFER(pszPath, TCHAR, MAX_PATH), "PathAppend: caller passed bad pszPath");
    RIPMSG(pszMore && IS_VALID_STRING_PTR(pszMore, -1), "PathAppend: caller passed bad pszMore");

    if (pszPath && pszMore)
    {
        // Skip any initial terminators on input, unless it is a UNC path in wich case we will 
        // treat it as a full path
        if (!PathIsUNCW(pszMore))
        {
            while (*pszMore == CH_WHACK)
            {
                pszMore++;
            }
        }

        return PathCombineW(pszPath, pszPath, pszMore) ? TRUE : FALSE;
    }
    
    return FALSE;
}

// returns a pointer to the extension of a file.
//
// in:
//      qualified or unqualfied file name
//
// returns:
//      pointer to the extension of this file.  if there is no extension
//      as in "foo" we return a pointer to the NULL at the end
//      of the file
//
//      foo.txt     ==> ".txt"
//      foo         ==> ""
//      foo.        ==> "."
//
STDAPI_(LPWSTR) PathFindExtensionW(LPCWSTR pszPath)
{
    LPCWSTR pszDot = NULL;

    RIPMSG(pszPath && IS_VALID_STRING_PTR(pszPath, -1), "PathFindExtension: caller passed bad pszPath");

    if (pszPath)
    {
        for (; *pszPath; pszPath++)
        {
            switch (*pszPath)
            {
                case TEXT('.'):
                    pszDot = pszPath;   // remember the last dot
                    break;

                case CH_WHACK:
                case CH_SLASH:
                case TEXT(' '):         // extensions can't have spaces
                    pszDot = NULL;      // forget last dot, it was in a directory
                    break;
            }
        }
    }

    // if we found the extension, return ptr to the dot, else
    // ptr to end of the string (NULL extension) (cast->non const)
    return pszDot ? (LPWSTR)pszDot : (LPWSTR)pszPath;
}

// check if a path is a root
//
// returns:
//  TRUE 
//      "\" "X:\" "\\" "\\foo" "\\foo\bar"
//
//  FALSE for others including "\\foo\bar\" (!)
//
STDAPI_(BOOL) PathIsRootW(LPCWSTR pPath)
{
    RIPMSG(pPath && IS_VALID_STRING_PTR(pPath, -1), "PathIsRoot: caller passed bad pPath");
    
    if (!pPath || !*pPath)
    {
        return FALSE;
    }
    
    if (!lstrcmpi(pPath + 1, TEXT(":\\")))
    {
        return TRUE;    // "X:\" case
    }
    
    if (IsPathSeparator(*pPath) && (*(pPath + 1) == 0))
    {
        return TRUE;    // "/" or "\" case
    }
    
    if (DBL_BSLASH(pPath))      // smells like UNC name
    {
        LPCWSTR p;
        int cBackslashes = 0;
        
        for (p = pPath + 2; *p; p++)
        {
            if (*p == TEXT('\\')) 
            {
                //
                //  return FALSE for "\\server\share\dir"
                //  so just check if there is more than one slash
                //
                //  "\\server\" without a share name causes
                //  problems for WNet APIs.  we should return
                //  FALSE for this as well
                //
                if ((++cBackslashes > 1) || !*(p+1))
                    return FALSE;   
            }
        }
        // end of string with only 1 more backslash
        // must be a bare UNC, which looks like a root dir
        return TRUE;
    }
    return FALSE;
}

/*
// rips the last part of the path off including the backslash
//      C:\foo      -> C:\
//      C:\foo\bar  -> C:\foo
//      C:\foo\     -> C:\foo
//      \\x\y\x     -> \\x\y
//      \\x\y       -> \\x
//      \\x         -> \\ (Just the double slash!)
//      \foo        -> \  (Just the slash!)
//
// in/out:
//      pFile   fully qualified path name
// returns:
//      TRUE    we stripped something
//      FALSE   didn't strip anything (root directory case)
//
*/
STDAPI_(BOOL) PathRemoveFileSpecW(LPWSTR pFile)
{
    RIPMSG(pFile && IS_VALID_STRING_PTR(pFile, -1), "PathRemoveFileSpec: caller passed bad pFile");

    if (pFile)
    {
        LPWSTR pT;
        LPWSTR pT2 = pFile;

        for (pT = pT2; *pT2; pT2++)
        {
            if (IsPathSeparator(*pT2))
            {
                pT = pT2;             // last "\" found, (we will strip here)
            }
            else if (*pT2 == TEXT(':'))     // skip ":\" so we don't
            {
                if (IsPathSeparator(pT2[1]))    // strip the "\" from "C:\"
                {
                    pT2++;
                }
                pT = pT2 + 1;
            }
        }

        if (*pT == 0)
        {
            // didn't strip anything
            return FALSE;
        }
        else if (((pT == pFile) && IsPathSeparator(*pT)) ||                     //  is it the "\foo" case?
                 ((pT == pFile+1) && (*pT == CH_WHACK && *pFile == CH_WHACK)))  //  or the "\\bar" case?
        {
            // Is it just a '\'?
            if (*(pT+1) != TEXT('\0'))
            {
                // Nope.
                *(pT+1) = TEXT('\0');
                return TRUE;        // stripped something
            }
            else
            {
                // Yep.
                return FALSE;
            }
        }
        else
        {
            *pT = 0;
            return TRUE;    // stripped something
        }
    }
    return  FALSE;
}

//
// Return a pointer to the end of the next path component in the string.
// ie return a pointer to the next backslash or terminating NULL.
//
LPCWSTR GetPCEnd(LPCWSTR lpszStart)
{
    LPCWSTR lpszEnd;
    LPCWSTR lpszSlash;

    lpszEnd = StrChr(lpszStart, CH_WHACK);
    lpszSlash = StrChr(lpszStart, CH_SLASH);
    if (lpszSlash > lpszEnd)
    {
        lpszEnd = lpszSlash;
    }
    if (!lpszEnd)
    {
        lpszEnd = lpszStart + lstrlen(lpszStart);
    }

    return lpszEnd;
}

//
// Given a pointer to the end of a path component, return a pointer to
// its begining.
// ie return a pointer to the previous backslash (or start of the string).
//
LPCWSTR PCStart(LPCWSTR lpszStart, LPCWSTR lpszEnd)
{
    LPCWSTR lpszBegin = StrRChr(lpszStart, lpszEnd, CH_WHACK);
    LPCWSTR lpszSlash = StrRChr(lpszStart, lpszEnd, CH_SLASH);
    if (lpszSlash > lpszBegin)
    {
        lpszBegin = lpszSlash;
    }
    if (!lpszBegin)
    {
        lpszBegin = lpszStart;
    }
    return lpszBegin;
}

//
// Fix up a few special cases so that things roughly make sense.
//
void NearRootFixups(LPWSTR lpszPath, BOOL fUNC)
{
    // Check for empty path.
    if (lpszPath[0] == TEXT('\0'))
    {
        // Fix up.
#ifndef PLATFORM_UNIX        
        lpszPath[0] = CH_WHACK;
#else
        lpszPath[0] = CH_SLASH;
#endif
        lpszPath[1] = TEXT('\0');
    }
    // Check for missing slash.
    if (lpszPath[1] == TEXT(':') && lpszPath[2] == TEXT('\0'))
    {
        // Fix up.
        lpszPath[2] = TEXT('\\');
        lpszPath[3] = TEXT('\0');
    }
    // Check for UNC root.
    if (fUNC && lpszPath[0] == TEXT('\\') && lpszPath[1] == TEXT('\0'))
    {
        // Fix up.
        //lpszPath[0] = TEXT('\\'); // already checked in if guard
        lpszPath[1] = TEXT('\\');
        lpszPath[2] = TEXT('\0');
    }
}

/*----------------------------------------------------------
Purpose: Canonicalize a path.

Returns:
Cond:    --
*/
STDAPI_(BOOL) PathCanonicalizeW(LPWSTR lpszDst, LPCWSTR lpszSrc)
{
    LPCWSTR lpchSrc;
    LPCWSTR lpchPCEnd;      // Pointer to end of path component.
    LPWSTR lpchDst;
    BOOL fUNC;
    int cchPC;

    RIPMSG(lpszDst && IS_VALID_WRITE_BUFFER(lpszDst, TCHAR, MAX_PATH), "PathCanonicalize: caller passed bad lpszDst");
    RIPMSG(lpszSrc && IS_VALID_STRING_PTR(lpszSrc, -1), "PathCanonicalize: caller passed bad lpszSrc");
    RIPMSG(lpszDst != lpszSrc, "PathCanonicalize: caller passed the same buffer for lpszDst and lpszSrc");

    if (!lpszDst || !lpszSrc)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    *lpszDst = TEXT('\0');
    
    fUNC = PathIsUNCW(lpszSrc);    // Check for UNCness.

    // Init.
    lpchSrc = lpszSrc;
    lpchDst = lpszDst;

    while (*lpchSrc)
    {
        lpchPCEnd = GetPCEnd(lpchSrc);
        cchPC = (int) (lpchPCEnd - lpchSrc)+1;

        if (cchPC == 1 && IsPathSeparator(*lpchSrc))   // Check for slashes.
        {
            // Just copy them.
#ifndef PLATFORM_UNIX            
            *lpchDst = CH_WHACK;
#else
            *lpchDst = CH_SLASH;
#endif
            lpchDst++;
            lpchSrc++;
        }
        else if (cchPC == 2 && *lpchSrc == TEXT('.'))  // Check for dots.
        {
            // Skip it...
            // Are we at the end?
            if (*(lpchSrc+1) == TEXT('\0'))
            {
                lpchSrc++;

                // remove the last slash we copied (if we've copied one), but don't make a mal-formed root
                if ((lpchDst > lpszDst) && !PathIsRootW(lpszDst))
                    lpchDst--;
            }
            else
            {
                lpchSrc += 2;
            }
        }
        else if (cchPC == 3 && *lpchSrc == TEXT('.') && *(lpchSrc + 1) == TEXT('.')) // Check for dot dot.
        {
            // make sure we aren't already at the root
            if (!PathIsRootW(lpszDst))
            {
                // Go up... Remove the previous path component.
                lpchDst = (LPWSTR)PCStart(lpszDst, lpchDst - 1);
            }
            else
            {
                // When we can't back up, skip the trailing backslash
                // so we don't copy one again. (C:\..\FOO would otherwise
                // turn into C:\\FOO).
                if (IsPathSeparator(*(lpchSrc + 2)))
                {
                    lpchSrc++;
                }
            }

            // skip ".."
            lpchSrc += 2;       
        }
        else                                                                        // Everything else
        {
            // Just copy it.
            lstrcpyn(lpchDst, lpchSrc, cchPC);
            lpchDst += cchPC - 1;
            lpchSrc += cchPC - 1;
        }

        // Keep everything nice and tidy.
        *lpchDst = TEXT('\0');
    }

    // Check for weirdo root directory stuff.
    NearRootFixups(lpszDst, fUNC);

    return TRUE;
}

// Modifies:
//      pszRoot
//
// Returns:
//      TRUE if a drive root was found
//      FALSE otherwise
//
STDAPI_(BOOL) PathStripToRootW(LPWSTR pszRoot)
{
    RIPMSG(pszRoot && IS_VALID_STRING_PTR(pszRoot, -1), "PathStripToRoot: caller passed bad pszRoot");

    if (pszRoot)
    {
        while (!PathIsRootW(pszRoot))
        {
            if (!PathRemoveFileSpecW(pszRoot))
            {
                // If we didn't strip anything off,
                // must be current drive
                return FALSE;
            }
        }
        return TRUE;
    }
    return FALSE;
}

// Removes a trailing backslash from a path
//
// in:
//  lpszPath    (A:\, C:\foo\, etc)
//
// out:
//  lpszPath    (A:\, C:\foo, etc)
//
// returns:
//  pointer to NULL that replaced the backslash
//  or the pointer to the last character if it isn't a backslash.
//
STDAPI_(LPWSTR) PathRemoveBackslashW(LPWSTR lpszPath)
{
    RIPMSG(lpszPath && IS_VALID_STRING_PTR(lpszPath, -1), "PathRemoveBackslash: caller passed bad lpszPath");

    if (lpszPath)
    {
        int len = lstrlen(lpszPath)-1;

        if (!PathIsRootW(lpszPath) && IsPathSeparator(lpszPath[len]))
            lpszPath[len] = TEXT('\0');

        return lpszPath + len;
    }
    return NULL;
}

// Removes a trailing extension from a path
//
// in:
//  pszPath    (A:\, C:\foo.txt, etc.)
//
// out:
//  pszPath    (A:\, C:\foo, etc.)
//
// returns:
//  nothing
//
STDAPI_(void) PathRemoveExtensionW(LPWSTR pszPath)
{
    RIPMSG(pszPath && IS_VALID_STRING_PTR(pszPath, -1), "PathRemoveExtension: caller passed bad pszPath");

    if (pszPath)
    {
        int len = lstrlen(pszPath) - 1;
        for(int i = len; i >= 0; i--)
        {
            if (IsPathSeparator(pszPath[i]))
            {
                break;
            }
            if (pszPath[i] == '.')
            {
                pszPath[i] = L'\0';
                break;
            }
        }
    }
}


/*----------------------------------------------------------
Purpose: Concatenate lpszDir and lpszFile into a properly formed
         path and canonicalize any relative path pieces.

         lpszDest and lpszFile can be the same buffer
         lpszDest and lpszDir can be the same buffer

Returns: pointer to lpszDest
*/
STDAPI_(LPWSTR) PathCombineW(LPWSTR lpszDest, LPCWSTR lpszDir, LPCWSTR lpszFile)
{
#ifdef DEBUG
    RIPMSG(lpszDest && IS_VALID_WRITE_BUFFER(lpszDest, TCHAR, MAX_PATH), "PathCombine: caller passed bad lpszDest");
    RIPMSG(!lpszDir || IS_VALID_STRING_PTR(lpszDir, -1), "PathCombine: caller passed bad lpszDir");
    RIPMSG(!lpszFile || IS_VALID_STRING_PTR(lpszFile, -1), "PathCombine: caller passed bad lpszFile");
    RIPMSG(lpszDir || lpszFile, "PathCombine: caller neglected to pass lpszDir or lpszFile");
#endif // DEBUG


    if (lpszDest)
    {
        TCHAR szTemp[MAX_PATH];
        LPWSTR pszT;

        *szTemp = TEXT('\0');

        if (lpszDir && *lpszDir)
        {
            if (!lpszFile || *lpszFile==TEXT('\0'))
            {
                lstrcpyn(szTemp, lpszDir, ARRAYSIZE(szTemp));       // lpszFile is empty
            }
            else if (PathIsRelativeW(lpszFile))
            {
                lstrcpyn(szTemp, lpszDir, ARRAYSIZE(szTemp));
                pszT = PathAddBackslashW(szTemp);
                if (pszT)
                {
                    int iRemaining = (int)(ARRAYSIZE(szTemp) - (pszT - szTemp));

                    if (lstrlen(lpszFile) < iRemaining)
                    {
                        lstrcpyn(pszT, lpszFile, iRemaining);
                    }
                    else
                    {
                        *szTemp = TEXT('\0');
                    }
                }
                else
                {
                    *szTemp = TEXT('\0');
                }
            }
            else if (IsPathSeparator(*lpszFile) && !PathIsUNCW(lpszFile))
            {
                lstrcpyn(szTemp, lpszDir, ARRAYSIZE(szTemp));
                // FEATURE: Note that we do not check that an actual root is returned;
                // it is assumed that we are given valid parameters
                PathStripToRootW(szTemp);

                pszT = PathAddBackslashW(szTemp);
                if (pszT)
                {
                    // Skip the backslash when copying
                    // Note: We don't support strings longer than 4GB, but that's
                    // okay because we already barf at MAX_PATH
                    lstrcpyn(pszT, lpszFile+1, (int)(ARRAYSIZE(szTemp) - (pszT - szTemp)));
                }
                else
                {
                    *szTemp = TEXT('\0');
                }
            }
            else
            {
                lstrcpyn(szTemp, lpszFile, ARRAYSIZE(szTemp));     // already fully qualified file part
            }
        }
        else if (lpszFile && *lpszFile)
        {
            lstrcpyn(szTemp, lpszFile, ARRAYSIZE(szTemp));     // no dir just use file.
        }

        //
        // if szTemp has something in it we succeeded.  Also if szTemp is empty and
        // the input strings are empty we succeed and PathCanonicalize() will
        // return "\"
        // 
        if (*szTemp || ((lpszDir || lpszFile) && !((lpszDir && *lpszDir) || (lpszFile && *lpszFile))))
        {
            PathCanonicalizeW(lpszDest, szTemp); // this deals with .. and . stuff
                                                // returns "\" on empty szTemp
        }
        else
        {
            *lpszDest = TEXT('\0');   // set output buffer to empty string.
            lpszDest  = NULL;         // return failure.
        }
    }

    return lpszDest;
}

// add a backslash to a qualified path
//
// in:
//  lpszPath    path (A:, C:\foo, etc)
//
// out:
//  lpszPath    A:\, C:\foo\    ;
//
// returns:
//  pointer to the NULL that terminates the path
//
STDAPI_(LPWSTR) PathAddBackslashW(LPWSTR lpszPath)
{
    LPWSTR lpszRet = NULL;

    RIPMSG(lpszPath && IS_VALID_STRING_PTR(lpszPath, -1), "PathAddBackslash: caller passed bad lpszPath");

    if (lpszPath)
    {
        int    ichPath = lstrlen(lpszPath);
        LPWSTR lpszEnd = lpszPath + ichPath;

        if (ichPath)
        {

            // Get the end of the source directory
            switch(*(lpszEnd-1))
            {
                case CH_SLASH:
                case CH_WHACK:
                    break;

                default:
                    // try to keep us from tromping over MAX_PATH in size.
                    // if we find these cases, return NULL.  Note: We need to
                    // check those places that call us to handle their GP fault
                    // if they try to use the NULL!
                    if (ichPath >= (MAX_PATH - 2)) // -2 because ichPath doesn't include NULL, and we're adding a CH_WHACK.
                    {
                        return(NULL);
                    }

                    *lpszEnd++ = CH_WHACK;
                    *lpszEnd = TEXT('\0');
            }
        }

        lpszRet = lpszEnd;
    }

    return lpszRet;
}


// Returns a pointer to the last component of a path string.
//
// in:
//      path name, either fully qualified or not
//
// returns:
//      pointer into the path where the path is.  if none is found
//      returns a poiter to the start of the path
//
//  c:\foo\bar  -> bar
//  c:\foo      -> foo
//  c:\foo\     -> c:\foo\                                                 
//  c:\         -> c:\                                                     
//  c:          -> c:
//  foo         -> foo
//
STDAPI_(LPWSTR) PathFindFileNameW(LPCWSTR pPath)
{
    LPCWSTR pT = pPath;
    
    RIPMSG(pPath && IS_VALID_STRING_PTR(pPath, -1), "PathFindFileName: caller passed bad pPath");

    if (pPath)
    {
        for ( ; *pPath; pPath++)
        {
            if ((pPath[0] == TEXT('\\') || pPath[0] == TEXT(':') || pPath[0] == TEXT('/'))
                && pPath[1] &&  pPath[1] != TEXT('\\')  &&   pPath[1] != TEXT('/'))
                pT = pPath + 1;
        }
    }

    return (LPWSTR)pT;   // const -> non const
}


//---------------------------------------------------------------------------
// Returns TRUE if the given string is a UNC path.
//
// TRUE
//      "\\foo\bar"
//      "\\foo"         <- careful
//      "\\"
// FALSE
//      "\foo"
//      "foo"
//      "c:\foo"
//
//
STDAPI_(BOOL) PathIsUNCW(LPCWSTR pszPath)
{
    RIPMSG(pszPath && IS_VALID_STRING_PTR(pszPath, -1), "PathIsUNC: caller passed bad pszPath");

    if (pszPath)
    {
        return DBL_BSLASH(pszPath);
    }
    return FALSE;
}



//---------------------------------------------------------------------------
// Returns 0 through 25 (corresponding to 'A' through 'Z') if the path has
// a drive letter, otherwise returns -1.
//
//
STDAPI_(int) PathGetDriveNumberW(LPCWSTR lpsz)
{
    RIPMSG(lpsz && IS_VALID_STRING_PTR(lpsz, -1), "PathGetDriveNumber: caller passed bad lpsz");

    if (lpsz)
    {
        if (lpsz[0] != TEXT('\0') && lpsz[1] == TEXT(':'))
        {
            if (lpsz[0] >= TEXT('a') && lpsz[0] <= TEXT('z'))
            {
                return (lpsz[0] - TEXT('a'));
            }
            else if (lpsz[0] >= TEXT('A') && lpsz[0] <= TEXT('Z'))
            {
                return (lpsz[0] - TEXT('A'));
            }
        }
    }

    return -1;
}


//---------------------------------------------------------------------------
// Return TRUE if the path isn't absoulte.
//
// TRUE
//      "foo.exe"
//      ".\foo.exe"
//      "..\boo\foo.exe"
//
// FALSE
//      "\foo"
//      "c:bar"     <- be careful
//      "c:\bar"
//      "\\foo\bar"
//
STDAPI_(BOOL) PathIsRelativeW(LPCWSTR lpszPath)
{
    RIPMSG(lpszPath && IS_VALID_STRING_PTR(lpszPath, -1), "PathIsRelative: caller passed bad lpszPath");

    if (!lpszPath || *lpszPath == 0)
    {
        // The NULL path is assumed relative
        return TRUE;
    }

    if (IsPathSeparator(lpszPath[0]))
    {
        // Does it begin with a slash ?
        return FALSE;
    }
    else if (lpszPath[1] == TEXT(':'))
    {
        // Does it begin with a drive and a colon ?
        return FALSE;
    }
    else
    {
        // Probably relative.
        return TRUE;
    }
}

// find the next slash or null terminator
LPWSTR StrSlash(LPCWSTR psz)
{
    for (; *psz && !IsPathSeparator(*psz); psz++);

    // Cast to a non-const string to mimic the behavior
    // of wcschr/StrChr and strchr.
    return (LPWSTR) psz;
}

#define StrIntlEqNI(s1, s2, len) _StrCmpLocaleW(NORM_IGNORECASE, s1, len, s2, len)

int _StrCmpLocaleW(DWORD dwFlags, LPCWSTR psz1, int cch1, LPCWSTR psz2, int cch2)
{
    int i = CompareStringW(GetThreadLocale(), dwFlags, psz1, cch1, psz2, cch2);
    if (!i)
    {
        i = CompareStringW(LOCALE_USER_DEFAULT, dwFlags, psz1, cch1, psz2, cch2);
    }
    return i - CSTR_EQUAL;
}

//
// in:
//      pszFile1 -- fully qualified path name to file #1.
//      pszFile2 -- fully qualified path name to file #2.
//
// out:
//      pszPath  -- pointer to a string buffer (may be NULL)
//
// returns:
//      length of output buffer not including the NULL
//
// examples:
//      c:\win\desktop\foo.txt
//      c:\win\tray\bar.txt
//      -> c:\win
//
//      c:\                                ;
//      c:\                                ;
//      -> c:\  NOTE, includes slash
//
// Returns:
//      Length of the common prefix string usually does NOT include
//      trailing slash, BUT for roots it does.
//
STDAPI_(int) PathCommonPrefixW(LPCWSTR pszFile1, LPCWSTR pszFile2, LPWSTR  pszPath)
{
    RIPMSG(pszFile1 && IS_VALID_STRING_PTR(pszFile1, -1), "PathCommonPrefix: caller passed bad pszFile1");
    RIPMSG(pszFile2 && IS_VALID_STRING_PTR(pszFile2, -1), "PathCommonPrefix: caller passed bad pszFile2");
    RIPMSG(!pszPath || IS_VALID_WRITE_BUFFER(pszPath, TCHAR, MAX_PATH), "PathCommonPrefix: caller passed bad pszPath");

    if (pszFile1 && pszFile2)
    {
        LPCWSTR psz1, psz2, pszNext1, pszNext2, pszCommon;
        int cch;

        pszCommon = NULL;
        if (pszPath)
            *pszPath = TEXT('\0');

        psz1 = pszFile1;
        psz2 = pszFile2;

        // special cases for UNC, don't allow "\\" to be a common prefix

        if (DBL_BSLASH(pszFile1))
        {
            if (!DBL_BSLASH(pszFile2))
                return 0;

            psz1 = pszFile1 + 2;
        }
        if (DBL_BSLASH(pszFile2))
        {
            if (!DBL_BSLASH(pszFile1))
                return 0;

            psz2 = pszFile2 + 2;
        }

        while (1)
        {
            pszNext1 = StrSlash(psz1);
            pszNext2 = StrSlash(psz2);

            cch = (int) (pszNext1 - psz1);

            if (cch != (pszNext2 - psz2))
                break;      // lengths of segments not equal

            if (StrIntlEqNI(psz1, psz2, cch))
                pszCommon = pszNext1;
            else
                break;

            ASSERT(*pszNext1 == TEXT('\0') || IsPathSeparator(*pszNext1));
            ASSERT(*pszNext2 == TEXT('\0') || IsPathSeparator(*pszNext2));

            if (*pszNext1 == TEXT('\0'))
                break;

            psz1 = pszNext1 + 1;

            if (*pszNext2 == TEXT('\0'))
                break;

            psz2 = pszNext2 + 1;
        }

        if (pszCommon)
        {
            cch = (int) (pszCommon - pszFile1);

            // special case the root to include the slash
            if (cch == 2)
            {
                ASSERT(pszFile1[1] == TEXT(':'));
                cch++;
            }
        }
        else
            cch = 0;

        if (pszPath)
        {
            CopyMemory(pszPath, pszFile1, cch * SIZEOF(TCHAR));
            pszPath[cch] = TEXT('\0');
        }

        return cch;
    }

    return 0;
}

//---------------------------------------------------------------------------
// Given a pointer to a point in a path - return a ptr the start of the
// next path component. Path components are delimted by slashes or the
// null at the end.
// There's special handling for UNC names.
// This returns NULL if you pass in a pointer to a NULL ie if you're about
// to go off the end of the  path.
//
STDAPI_(LPWSTR) PathFindNextComponentW(LPCWSTR pszPath)
{
    RIPMSG(pszPath && IS_VALID_STRING_PTR(pszPath, -1), "PathFindNextComponent: caller passed bad pszPath");

    if (pszPath)
    {
        LPWSTR pszLastSlash;

        // Are we at the end of a path.
        if (!*pszPath)
        {
            // Yep, quit.
            return NULL;
        }
        // Find the next slash.
        pszLastSlash = StrSlash(pszPath);

        // Is there a slash?
        if (!pszLastSlash)
        {
            // No - Return a ptr to the NULL.
            return (LPWSTR)pszPath + lstrlen(pszPath);
        }
        else
        {
            // Is it a UNC style name?
            if (*(pszLastSlash + 1) == TEXT('\\'))
            {
                // Yep, skip over the second slash.
                return pszLastSlash + 2;
            }
            else
            {
                // Nope. just skip over one slash.
                return pszLastSlash + 1;
            }
        }
    }

    return NULL;
}


static const WCHAR c_szDot[] = TEXT(".");
static const WCHAR c_szDotDot[] = TEXT("..");

#ifdef PLATFORM_UNIX
static const WCHAR c_szDotDotSlash[] = TEXT("../");
#else
static const WCHAR c_szDotDotSlash[] = TEXT("..\\");
#endif

// in:
//      pszFrom         base path, including filespec!
//      pszTo           path to be relative to pszFrom
// out:
//      relative path to construct pszTo from the base path of pszFrom
//
//      c:\a\b\FileA
//      c:\a\x\y\FileB
//      -> ..\x\y\FileB
//
STDAPI_(BOOL) PathRelativePathToW(LPWSTR pszPath, LPCWSTR pszFrom, DWORD dwAttrFrom, LPCWSTR pszTo, DWORD dwAttrTo)
{
#ifdef DEBUG
    WCHAR szFromCopy[MAX_PATH];
    WCHAR szToCopy[MAX_PATH];

    RIPMSG(pszPath && IS_VALID_WRITE_BUFFER(pszPath, TCHAR, MAX_PATH), "PathRelativePathTo: caller passed bad pszPath");
    RIPMSG(pszFrom && IS_VALID_STRING_PTR(pszFrom, -1), "PathRelativePathTo: caller passed bad pszFrom");
    RIPMSG(pszTo && IS_VALID_STRING_PTR(pszTo, -1), "PathRelativePathTo: caller passed bad pszTo");

    // we make copies of the pszFrom and pszTo buffers in case one of the strings they are passing is a pointer
    // inside pszPath buffer. If this were the case, it would be trampled when we call DEBUGWhackPathBuffer().
    if (pszFrom)
    {
        lstrcpyn(szFromCopy, pszFrom, ARRAYSIZE(szFromCopy));
        pszFrom = szFromCopy;
    }
    
    if (pszTo)
    {
        lstrcpyn(szToCopy, pszTo, ARRAYSIZE(szToCopy));
        pszTo = szToCopy;
    }
#endif // DEBUG


    if (pszPath && pszFrom && pszTo)
    {
        WCHAR szFrom[MAX_PATH], szTo[MAX_PATH];
        LPWSTR psz;
        UINT cchCommon;

        *pszPath = 0;       // assume none

        lstrcpyn(szFrom, pszFrom, ARRAYSIZE(szFrom));
        lstrcpyn(szTo, pszTo, ARRAYSIZE(szTo));

        if (!(dwAttrFrom & FILE_ATTRIBUTE_DIRECTORY))
            PathRemoveFileSpecW(szFrom);

        if (!(dwAttrTo & FILE_ATTRIBUTE_DIRECTORY))
            PathRemoveFileSpecW(szTo);

        cchCommon = PathCommonPrefixW(szFrom, szTo, NULL);
        if (cchCommon == 0)
            return FALSE;

        psz = szFrom + cchCommon;

        if (*psz)
        {
            // build ..\.. part of the path
            if (IsPathSeparator(*psz))
                psz++;              // skip slash
            while (*psz)
            {
                psz = PathFindNextComponentW(psz);
                // WARNING: in a degenerate case where each path component
                // is 1 character (less than "..\") we can overflow pszPath
                lstrcat(pszPath, *psz ? c_szDotDotSlash : c_szDotDot);
            }
        }
        else
        {
            lstrcpy(pszPath, c_szDot);
        }
        if (pszTo[cchCommon])
        {
            // deal with root case
            if (!IsPathSeparator(pszTo[cchCommon]))
                cchCommon--;

            if ((lstrlen(pszPath) + lstrlen(pszTo + cchCommon)) >= MAX_PATH)
            {
                *pszPath = 0;
                return FALSE;
            }

            ASSERT(IsPathSeparator(pszTo[cchCommon]));
            lstrcat(pszPath, pszTo + cchCommon);
        }

        ASSERT(PathIsRelative(pszPath));
        ASSERT(lstrlen(pszPath) < MAX_PATH);

        return TRUE;
    }

    return FALSE;
}

STDAPI_(void) PathStripPathW (LPWSTR pszPath)
{
    RIPMSG(pszPath && IS_VALID_STRING_PTR(pszPath, -1), "PathStripPath: caller passed bad pszPath");

    if (pszPath)
    {
        LPWSTR pszName = PathFindFileNameW(pszPath);

        if (pszName != pszPath)
        {
            lstrcpy(pszPath, pszName);
        }
    }
}

STDAPI_(BOOL) PathRenameExtensionW(LPWSTR pszPath, LPCWSTR pszExt)
{
    RIPMSG(pszPath && IS_VALID_STRING_PTR(pszPath, -1) && IS_VALID_WRITE_BUFFER(pszPath, TCHAR, MAX_PATH), "PathRenameExtension: caller passed bad pszPath");
    RIPMSG(pszExt && IS_VALID_STRING_PTR(pszExt, -1), "PathRenameExtension: caller passed bad pszExt");

    if (pszPath && pszExt)
    {
        LPWSTR pExt = PathFindExtensionW(pszPath);  // Rets ptr to end of str if none
        if (pExt - pszPath + lstrlen(pszExt) > MAX_PATH - 1)
        {
            return FALSE;
        }
        lstrcpy(pExt, pszExt);
        return TRUE;
    }
    return FALSE;
}
