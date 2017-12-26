// UniApi.cpp - Unicode API wrappers
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// November 8, 1998 [paulde]
//
//-----------------------------------------------------------------
#include "pch.h"
#include "uniapi.h"
#include "unistr.h"
#include "awconv.h"
#include "vsmem.h"
#include "dbgout.h"

#include <io.h>
#include <direct.h>
#include <wchar.h>
#include <richedit.h>
#include "strsafe.h"

//--------------------------------------------------------------
BOOL WINAPI W_IsUnicodeSystem()
{
    static BOOL g_fWAPI = -2;
    if (-2 == g_fWAPI)
    {
        OSVERSIONINFOA osvi;
        osvi.dwOSVersionInfoSize = sizeof(osvi);
        GetVersionExA(&osvi);
        g_fWAPI = (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId);
    }
    return g_fWAPI;
}

//--------------------------------------------------------------
// shorthand for common params reduces clutter & typing
#define MBToW(pstr, cchMB, pwstr, cchW) MultiByteToWideChar(CP_ACP, 0, pstr, cchMB, pwstr, cchW)
#define WToMB(pwstr, cchW, pstr, cchMB) WideCharToMultiByte(CP_ACP, 0, pwstr, cchW, pstr, cchMB, 0, 0)

#if !defined(_M_IA64) && !defined(_M_AMD64)
//--------------------------------------------------------------
// FindData_A2W converts a WIN32_FIND_DATAA to a WIN32_FIND_DATAW
//
void FindData_A2W(WIN32_FIND_DATAA * pfdA, WIN32_FIND_DATAW * pfd)
{
    memcpy(pfd, pfdA, sizeof(WIN32_FIND_DATAW) - (MAX_PATH+14));
    ASSERT(pfd->dwFileAttributes                == pfdA->dwFileAttributes);
    ASSERT(pfd->ftCreationTime.dwLowDateTime    == pfdA->ftCreationTime.dwLowDateTime );
    ASSERT(pfd->ftCreationTime.dwHighDateTime   == pfdA->ftCreationTime.dwHighDateTime);
    ASSERT(pfd->ftLastAccessTime.dwLowDateTime  == pfdA->ftLastAccessTime.dwLowDateTime );
    ASSERT(pfd->ftLastAccessTime.dwHighDateTime == pfdA->ftLastAccessTime.dwHighDateTime);
    ASSERT(pfd->ftLastWriteTime.dwLowDateTime   == pfdA->ftLastWriteTime.dwLowDateTime );
    ASSERT(pfd->ftLastWriteTime.dwHighDateTime  == pfdA->ftLastWriteTime.dwHighDateTime);
    ASSERT(pfd->nFileSizeHigh                   == pfdA->nFileSizeHigh);
    ASSERT(pfd->nFileSizeLow                    == pfdA->nFileSizeLow);
    ASSERT(pfd->dwReserved0                     == pfdA->dwReserved0);
    ASSERT(pfd->dwReserved1                     == pfdA->dwReserved1);

    if(!MBToW(pfdA->cFileName, -1, pfd->cFileName, MAX_PATH))
        pfd->cFileName[MAX_PATH-1] = NULL;
    if(!MBToW(pfdA->cAlternateFileName, -1, pfd->cAlternateFileName, 14))    
        pfd->cFileName[13] = NULL;
}

//--------------------------------------------------------------
HANDLE WINAPI W_FindFirstFile (PCWSTR pFileName, WIN32_FIND_DATAW * pfd)
{
    if (W_IsUnicodeSystem())
        return ::FindFirstFileW(pFileName, pfd);

    WIN32_FIND_DATAA    fdA;
    HANDLE              h;
    char                sz[MAX_PATH];

    if(!WToMB(pFileName, -1, sz, MAX_PATH))
        sz[MAX_PATH-1] = NULL;

    h = ::FindFirstFileA(sz, &fdA);
    if (INVALID_HANDLE_VALUE != h)
        FindData_A2W(&fdA, pfd);
    return h;
}

//--------------------------------------------------------------
BOOL WINAPI W_FindNextFile (HANDLE h, WIN32_FIND_DATAW * pfd)
{
    if (W_IsUnicodeSystem())
        return ::FindNextFileW(h, pfd);

    WIN32_FIND_DATAA fdA;
    BOOL f = ::FindNextFileA(h, &fdA);
    if (f)
        FindData_A2W(&fdA, pfd);
    return f;
}
#endif // !_WIN64

#if !defined(_WIN64)
//--------------------------------------------------------------
DWORD WINAPI W_GetFullPathName (__in_z PCWSTR pFileName, DWORD cch, __out_ecount(cch) PWSTR pBuffer, __deref_opt_out PWSTR * ppFilePart)
{

    if (NULL == pFileName || NULL == *pFileName)
        return 0;

    if (W_IsUnicodeSystem())
        return ::GetFullPathNameW (pFileName, cch, pBuffer, ppFilePart);

    char szFile[MAX_PATH], sz[MAX_PATH], *p;

    if(!WToMB(pFileName, -1, szFile, MAX_PATH))
        szFile[MAX_PATH-1] = NULL;

    DWORD cch2 = ::GetFullPathNameA(szFile, MAX_PATH, sz, &p);
    cch2 = MBToW(sz, -1, pBuffer, cch) - 1;
    if(cch2 == -1)
        sz[MAX_PATH-1] = NULL;

    if (cch && cch2 && pBuffer && ppFilePart)
    {
        if (p != NULL)
        {
            *p = 0;
            *ppFilePart = pBuffer + MBToW(sz, -1, 0, 0) - 1;
        }
        else
        {
            *ppFilePart = NULL;
        }
    }
    return cch2;

}

//--------------------------------------------------------------
DWORD WINAPI W_GetTempPath (DWORD cch, __out_ecount(cch) PWSTR pBuffer)
{
#ifdef FEATURE_CORESYSTEM
    return ::GetTempPathW (cch, pBuffer);
#else
    if (W_IsUnicodeSystem())
        return ::GetTempPathW (cch, pBuffer);

    char sz[MAX_PATH];
    DWORD cch2 = ::GetTempPathA (MAX_PATH, sz);
    cch2 = MBToW(sz, -1, pBuffer, cch);
    if(cch2 == 0)
        pBuffer[cch-1] = NULL;
    return cch2;
#endif
}

//--------------------------------------------------------------
UINT WINAPI W_GetTempFileName (__in_z PCWSTR pPathName, __in_z PCWSTR pPrefix, UINT uUnique, __out_ecount(MAX_PATH) PWSTR pTempFileName)
{
#ifdef FEATURE_CORESYSTEM
    return ::GetTempFileNameW (pPathName, pPrefix, uUnique, pTempFileName);
#else
    if (W_IsUnicodeSystem())
        return ::GetTempFileNameW (pPathName, pPrefix, uUnique, pTempFileName);

    char szPath[MAX_PATH];
    char szPrefix[MAX_PATH];
    char szTempFile[MAX_PATH];

    if(!WToMB(pPathName, -1, szPath, MAX_PATH))
        szPath[MAX_PATH-1] = NULL;

    if(!WToMB(pPrefix, -1, szPrefix, MAX_PATH))
        szPrefix[MAX_PATH-1] = NULL;
    
    UINT n = ::GetTempFileNameA (szPath, szPrefix, uUnique, szTempFile);
    if (n)
    {
        if(!MBToW(szTempFile, -1, pTempFileName, MAX_PATH))
            pTempFileName[MAX_PATH-1] = NULL;
    }
    return n;
#endif
}

//--------------------------------------------------------------
BOOL WINAPI W_DeleteFile (PCWSTR pPathName)
{
    if (W_IsUnicodeSystem())
        return ::DeleteFileW (pPathName);
    char sz[MAX_PATH];
    if(!WToMB(pPathName, -1, sz, MAX_PATH))
        sz[MAX_PATH-1] = NULL;

    return ::DeleteFileA(sz);
}

//--------------------------------------------------------------
BOOL WINAPI W_CopyFile (PCWSTR pExistingPathName, PCWSTR pNewPathName, BOOL bFailIfExists)
{
    if (W_IsUnicodeSystem())
        return ::CopyFileW(pExistingPathName, pNewPathName, bFailIfExists);
    char szExisting[MAX_PATH];
    char szNew[MAX_PATH];
    if(!WToMB(pExistingPathName, -1, szExisting, MAX_PATH))
        szExisting[MAX_PATH-1] = NULL;
    if(!WToMB(pNewPathName, -1, szNew, MAX_PATH))
        szNew[MAX_PATH-1] = NULL;

    return ::CopyFileA(szExisting, szNew, bFailIfExists);
}
#endif // !_WIN64

//--------------------------------------------------------------
int WINAPI W_Access (PCWSTR pPathName, int mode)
{
    // _access and _waccess return 0 for a success code and
    // -1 for a failure code.  However, _waccess_s and _access_s
    // return 0 for sucess and a non-zero value for failure.  
    // This function returns the old style success and failure codes
    // however, a proper check for failure is:
    // if(W_Access(pPathName, mode) != 0)
    //    // handle failure
    if (W_IsUnicodeSystem())
        return _waccess_s(pPathName, mode) == 0 ? 0 : -1;
    char sz[MAX_PATH];
    if(!WToMB(pPathName, -1, sz, MAX_PATH))
        sz[MAX_PATH-1] = NULL;
    return _access_s(sz, mode) == 0 ? 0 : -1;
}

//--------------------------------------------------------------
int WINAPI W_Rename (PCWSTR pOldPathName, PCWSTR pNewPathName)
{
    if (W_IsUnicodeSystem())
        return _wrename(pOldPathName, pNewPathName);
    char szOld[MAX_PATH];
    char szNew[MAX_PATH];
    if(!WToMB(pOldPathName, -1, szOld, MAX_PATH))
        szOld[MAX_PATH-1] = NULL;

    if(!WToMB(pNewPathName, -1, szNew, MAX_PATH))
        szNew[MAX_PATH-1] = NULL;

    return rename(szOld, szNew);
}

//--------------------------------------------------------------
int WINAPI W_Remove (PCWSTR pPathName)
{
    if (W_IsUnicodeSystem())
        return _wremove(pPathName);
    char sz[MAX_PATH];
    if(!WToMB(pPathName, -1, sz, MAX_PATH))
        sz[MAX_PATH-1] = NULL;
    return remove(sz);
}

//--------------------------------------------------------------
int WINAPI W_ChDir (PCWSTR pPathName)
{
    if (W_IsUnicodeSystem())
        return _wchdir(pPathName);
    char sz[MAX_PATH];
    if(!WToMB(pPathName, -1, sz, MAX_PATH))
        sz[MAX_PATH-1] = NULL;
    
    return _chdir(sz);
}

//--------------------------------------------------------------
int WINAPI W_MkDir (PCWSTR pPathName)
{
    if (W_IsUnicodeSystem())
        return _wmkdir(pPathName);
    char sz[MAX_PATH];
    if(!WToMB(pPathName, -1, sz, MAX_PATH))
        sz[MAX_PATH-1] = NULL;
    return _mkdir(sz);
}

//--------------------------------------------------------------
int WINAPI W_RmDir (PCWSTR pPathName)
{
    if (W_IsUnicodeSystem())
        return _wrmdir(pPathName);
    char sz[MAX_PATH];
    if(!WToMB(pPathName, -1, sz, MAX_PATH))
        sz[MAX_PATH-1] = NULL;
    return _rmdir(sz);
}

//--------------------------------------------------------------
BOOL WINAPI W_GetActualFileCase(__in_z PCWSTR pszName, __out_ecount(cchOut) PWSTR pszOut, DWORD cchOut)
{
    // Guaranteed to be large enough -- we never do anything to these strings
    // that would make them longer than the input string
    size_t cchName = StrLen(pszName);
    WCHAR *pszNormalized = VSALLOCSTR(cchName + 1);
    WCHAR *pszPartial = VSALLOCSTR(cchName + 1);
    if (pszNormalized == NULL || pszPartial == NULL)
    {
        goto Error;
    }

    PWSTR pszOutEnd = pszOut + cchOut;

    VERIFY(SUCCEEDED(StringCchCopyW(pszNormalized, cchName+1, pszName)));
    NormalizeFileSlashes(pszNormalized);

#ifdef _DEBUG
    // Assert our right to write to the entire buffer specified by pszOut & cchOut.
    // If the caller has made a mistake in these params, this should make it obvious.
    memset(pszOut, 0xAF, cchOut * sizeof(WCHAR));
#endif  // _DEBUG

    WCHAR *pchScan = pszNormalized;
    WCHAR *pchOut  = pszOut;
    WCHAR *pchSection = pszPartial;
    WCHAR *pchBuild = pchSection;

    // pchEnd points to the WCHAR beyond the end of the caller's buffer
    WCHAR *pchEnd = pszOut + cchOut;

    if (*pchScan == L'\\')
    {
        if (pchOut >= pchEnd)
        {
            goto Error;
        }
        *pchOut++ = *pchBuild++ = *pchScan++;
        if (*pchScan == L'\\')
        {
            // It's a UNC name.
            // 


            pchOut--;
            *pchBuild++ = *pchScan++;
            while (*pchScan && *pchScan != L'\\')
                *pchBuild++ = *pchScan++;
            *pchBuild++ = *pchScan++;
            while (*pchScan && *pchScan != L'\\')
                *pchBuild++ = *pchScan++;
            if (*pchScan == L'\\')
                *pchBuild++ = *pchScan++;
            *pchBuild = 0;
            if (pchOut + (pchBuild - pchSection) >= pchEnd)
            {
                goto Error;
            }
            VERIFY(SUCCEEDED(StringCchCopyExW(pchOut, (pszOutEnd - pchOut), pchSection, &pchOut, NULL, 0)));
            pchSection = pchBuild;
        }
    }
    else if (*(pchScan+1) == L':')
    {
        // It has a drive letter 
        *pchBuild++ = UpperCase(*pchScan++);
        *pchBuild++ = *pchScan++;
        if (*pchScan == L'\\')
            *pchBuild++ = *pchScan++;
        *pchBuild = 0;
        if (pchOut + (pchBuild - pchSection) >= pchEnd)
        {
            goto Error;
        }
        VERIFY(SUCCEEDED(StringCchCopyExW(pchOut, (pszOutEnd - pchOut), pchSection, &pchOut, NULL, 0)));
        pchSection = pchBuild;
    }

    // Handle the directories and/or filename
    for (;;)
    {
        // There is no path, or there's a trailing backslash
        if (*pchScan == L'\0')
        {
            *pchOut = L'\0';
            VSFree(pszNormalized);
            VSFree(pszPartial);
            return TRUE;
        }

        while (*pchScan && *pchScan != L'\\')
            *pchBuild++ = *pchScan++;
        *pchBuild = 0;

        WIN32_FIND_DATAW fd;
        HANDLE hFind = W_FindFirstFile(pszPartial, &fd);
        if (INVALID_HANDLE_VALUE == hFind)
        {
            goto Error;
        }
        ::FindClose(hFind);

        if (pchOut + StrLen(fd.cFileName) >= pchEnd)
        {
            goto Error;
        }
        VERIFY(SUCCEEDED(StringCchCopyExW(pchOut, (pszOutEnd - pchOut), fd.cFileName, &pchOut, NULL, 0)));
        if (0 == *pchScan)
        {
            VSFree(pszNormalized);
            VSFree(pszPartial);
            return TRUE;
        }

        pchScan++;
        if (pchOut >= pchEnd)
        {
            goto Error;
        }
        *pchBuild++ = *pchOut++ = L'\\';
        pchSection = pchBuild;
    }

Error:
    VSFree(pszNormalized);
    VSFree(pszPartial);

    return FALSE;
}

#if !defined(_M_IA64) && !defined(_M_AMD64)
//--------------------------------------------------------------
DWORD WINAPI W_GetFileAttributes (PCWSTR pFileName)
{
    if (W_IsUnicodeSystem())
        return ::GetFileAttributesW (pFileName);

    char sz[MAX_PATH];
    if(!WToMB(pFileName, -1, sz, MAX_PATH))
        sz[MAX_PATH-1] = NULL;
    return ::GetFileAttributesA (sz);
}

//--------------------------------------------------------------
BOOL WINAPI W_SetFileAttributes  (PCWSTR pFileName, DWORD dwAttr)
{
    if (W_IsUnicodeSystem())
        return ::SetFileAttributesW (pFileName, dwAttr);

    char sz[MAX_PATH];
    if(!WToMB(pFileName, -1, sz, MAX_PATH))
        sz[MAX_PATH-1] = NULL;
    return ::SetFileAttributesA (sz, dwAttr);
}

//--------------------------------------------------------------
HANDLE  WINAPI W_CreateFile (PCWSTR pFileName,  DWORD dwAccess, DWORD dwShare,
                             SECURITY_ATTRIBUTES * psa, DWORD dwCD, DWORD dwAttr, HANDLE hTemplate )
{
    ASSERT(0 != wcsncmp(pFileName, L"\\\\?\\", 4)); // not supported on Win95 - detect OS and use ::CreateFileW

    if (W_IsUnicodeSystem())
        return ::CreateFileW (pFileName, dwAccess, dwShare, psa, dwCD, dwAttr, hTemplate);

    char sz[MAX_PATH];
    if(!WToMB(pFileName, -1, sz, MAX_PATH))
        sz[MAX_PATH-1] = NULL;
    return ::CreateFileA (sz, dwAccess, dwShare, psa, dwCD, dwAttr, hTemplate);
}

//--------------------------------------------------------------
DWORD WINAPI W_GetCurrentDirectory (DWORD cch, __out_ecount(cch) PWSTR pBuffer)
{
    if (W_IsUnicodeSystem())
        return ::GetCurrentDirectoryW(cch, pBuffer);
    char sz[MAX_PATH];
    sz[0] = 0;
    int cch2 = ::GetCurrentDirectoryA(MAX_PATH, sz);
    if (cch && pBuffer)
    {
        DWORD dwRet = MBToW(sz, -1, pBuffer, cch) - 1;
        if(dwRet == -1)
            pBuffer[cch-1] = 0;
        return dwRet;
    }
    else
        return MBToW(sz, -1, 0, 0);
    
}

#ifndef FEATURE_CORESYSTEM
//--------------------------------------------------------------
int WINAPI W_GetWindowTextLength (HWND hwnd)
{
    if (W_IsUnicodeSystem())
        return ::GetWindowTextLengthW(hwnd);
    // API is documented to be inexact but large enough,
    // so this is sufficient.
    return ::GetWindowTextLengthA(hwnd); 
}

//--------------------------------------------------------------
ATOM WINAPI W_RegisterClass  (CONST WNDCLASSW * pWndClass)
{
    if (W_IsUnicodeSystem())
        return ::RegisterClassW(pWndClass);

    WNDCLASSA wc;
    ASSERT(sizeof(WNDCLASSA)==sizeof(WNDCLASSW));
    memcpy(&wc, pWndClass, sizeof(WNDCLASSA));
    wc.lpszMenuName  = ANSISTR(pWndClass->lpszMenuName);
    wc.lpszClassName = ANSISTR(pWndClass->lpszClassName);
    return ::RegisterClassA(&wc);
}

//--------------------------------------------------------------
HWND WINAPI W_CreateWindowEx (DWORD dwExStyle, PCWSTR pClassName, PCWSTR pWindowName,
                              DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                              HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam)
{
    if (W_IsUnicodeSystem())
        return ::CreateWindowExW(dwExStyle, pClassName, pWindowName, 
                                 dwStyle, X, Y, nWidth, nHeight,
                                 hWndParent, hMenu, hInstance, lpParam);
    // ANSISTR handles atoms and NULL transparently
    return ::CreateWindowExA(dwExStyle, ANSISTR(pClassName), ANSISTR(pWindowName), 
                             dwStyle, X, Y, nWidth, nHeight,
                             hWndParent, hMenu, hInstance, lpParam);
}

//--------------------------------------------------------------
HWND WINAPI W_CreateDialogParam (HINSTANCE hInstance, PCWSTR pTemplateName, HWND hWndParent, 
                                 DLGPROC pDialogFunc, LPARAM dwInitParam)
{
    if (W_IsUnicodeSystem())
        return ::CreateDialogParamW (hInstance, pTemplateName, hWndParent, pDialogFunc, dwInitParam);
    // ANSISTR handles atoms and NULL transparently
    return ::CreateDialogParamA (hInstance, ANSISTR(pTemplateName), hWndParent, pDialogFunc, dwInitParam);
}

//--------------------------------------------------------------
HWND WINAPI W_CreateDialogIndirectParam (HINSTANCE hInstance, const DLGTEMPLATE * pTemplate,
                                         HWND hWndParent, DLGPROC pDialogFunc, LPARAM dwInitParam)
{
    if (W_IsUnicodeSystem())
        return ::CreateDialogIndirectParamW(hInstance, pTemplate, hWndParent, pDialogFunc, dwInitParam);
    return ::CreateDialogIndirectParamA(hInstance, pTemplate, hWndParent, pDialogFunc, dwInitParam);
}

//--------------------------------------------------------------
int WINAPI W_GetWindowText (HWND hwnd, __out_ecount(cch) PWSTR psz, int cch)
{
    // We still use W_IsUnicodeSystem instead of IsWindowUnicode() because 
    // NT does the right thing for us even for Ansi windows.
    if (W_IsUnicodeSystem())
        return ::GetWindowTextW(hwnd, psz, cch);

    return 0;
}
#endif //!_WIN64
#endif
//--------------------------------------------------------------
// GetStringResourceImage
// Returns length-prefixed NOT null-terminated resource image for string
PCWSTR WINAPI W_GetStringResourceImage (HINSTANCE hinst, UINT id)
{
    // Declare a static const null string to ensure
    // the value is preserved after the function returns
    static const PCWSTR pszNullString = L"\0";
    
    PCWSTR pchToReturn = pszNullString;
    HRSRC  hres = ::FindResourceA(hinst, (PCSTR)(((UINT_PTR)id >> 4) + 1), MAKEINTRESOURCEA(6) /* RT_STRING */);
    if (hres)
    {
        HGLOBAL hg = ::LoadResource(hinst, hres);
        if (hg)
        {
            PCWSTR pch = (PCWSTR)::LockResource(hg);
            if (pch)
            {
                PCWSTR    pend  = pch + ::SizeofResource(hinst, hres)/sizeof(WCHAR);
                UINT_PTR  index = id & 0x000f;

                // Loop while the index is greater than zero, pch is 
                // less than its end point and the character pointed
                // at by pch is not negative
                while ((index > 0) && (pch < pend) && (*pch >= 0))
                {
                    pch += 1 + *pch;
                    --index;
                }

                // If pch is smaller than its start or greater to or
                // equal its end, something went wrong.  Return the null string
                if (pch >= pend)
                {
                    VSASSERT(pch >= pend, "pch should not be greater than equal to pend.  Is a resource corrupt?"); 
                }
                else
                {
                    // If what is pointed to by pch is an invalid character, something
                    // went wrong.  Return the null string
                    if (*pch < 0)
                    {
                        VSASSERT(*pch < 0, "*pch should not be less than zero.  Is a resource corrupt?"); 
                    } 
                    else if ((pch + *pch) >= pend)
                    {
                        VSASSERT(*pch < 0, "(pch + *pch) should not be greater than or equal to pend.  That is past the end of the resource.  Is a resource corrupt?"); 
                    }
                    else
                    {
                        // Everything succeeded.  Set the return value to the resource string
                        pchToReturn = pch;
                    }
                }
            }
            else
            {
                VSASSERT(false, "LockResource failed while trying to get a string resource image.");
            }
        }
        else
        {
            VSASSERT(false, "LoadResource failed while trying to get a string resource image.");
        }
    }
    else
    {
        VSASSERT(false, "FindResource failed while trying to get a string resource image.");
    }

    return pchToReturn;
}

//--------------------------------------------------------------
// W_LoadBSTR
BSTR WINAPI W_LoadBSTR (HINSTANCE hInst, UINT id)
{
    PCWSTR pres = W_GetStringResourceImage(hInst, id);
    return ::SysAllocStringLen(pres+1, *pres);
}

//--------------------------------------------------------------
// W_LoadString
int WINAPI W_LoadString (HINSTANCE hinst, UINT id, __out_ecount(cch) PWSTR pch, int cch)
{
    if (!pch || !cch) return 0;
    PCWSTR pres   = W_GetStringResourceImage(hinst, id);
    size_t cchStr = min(*pres, cch-1);
    
    memcpy(pch, pres + 1, sizeof(WCHAR)*cchStr);
    pch[cchStr] = 0;
    
    return (int)cchStr;
}

//--------------------------------------------------------------
PWSTR WINAPI W_LoadVSString (HINSTANCE hInst, UINT id)
{
    PCWSTR pch    = W_GetStringResourceImage(hInst, id);
    size_t cch    = *pch++;
    PWSTR  pszRet = VSALLOCSTR(cch + 1);
    if (pszRet)
    {
        memcpy(pszRet, pch, sizeof(WCHAR)*cch);
        pszRet[cch] = 0;
    }
    return pszRet;
}

//--------------------------------------------------------------
#ifndef FEATURE_CORESYSTEM
BOOL WINAPI W_GetWindowText(HWND hwnd, CComBSTR &bstrText)
{
  ULONG cch = GetWindowTextLengthW(hwnd);
  if (0 == cch)
  {
    bstrText.Empty();
    return TRUE;
  }

  // Fix to cover the fact that wmgettextlen doesn't include term
  ++cch;

  CComBSTR bstrResult(cch);
  if (!bstrResult)
    return FALSE;

  LRESULT lResult = GetWindowTextW(hwnd, bstrResult, cch);
  _ASSERTE(lResult <= ((LRESULT) cch));

  bstrText.Empty();
  bstrText.Attach(bstrResult.Detach());
  return TRUE;
}

//--------------------------------------------------------------

BOOL WINAPI W_GetWindowTextTrimSpaces(HWND hwnd, CComBSTR &bstrText)
{
  CComBSTR bstrWindowText;
  if(W_GetWindowText(hwnd, bstrWindowText))
  {
    TrimWhitespace(bstrWindowText);
    bstrText = (LPCOLESTR)bstrWindowText; //Allow the returned CComBSTR to stop at the first nul character so .Length() will work correctly
    return TRUE;
  }
  return FALSE;
}
#endif
