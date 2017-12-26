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
//*****************************************************************************
// WinWrap.cpp
//
// This file contains wrapper functions for Win32 API's that take strings.
// Support on each platform works as follows:
//      OS          Behavior
//      ---------   -------------------------------------------------------
//      NT          Fully supports both W and A funtions.
//      Win 9x      Supports on A functions, stubs out the W functions but
//                      then fails silently on you with no warning.
//      CE          Only has the W entry points.
//
// COM+ internally uses UNICODE as the internal state and string format.  This
// file will undef the mapping macros so that one cannot mistakingly call a
// method that isn't going to work.  Instead, you have to call the correct
// wrapper API.
//
//*****************************************************************************

#include "stdafx.h"                     // Precompiled header key.
#include "winwrap.h"                    // Header for macros and functions.
#include "utilcode.h"
#include "holder.h"
#include "ndpversion.h"
#include "pedecoder.h"


#define WIN9X_CONTRACT_VIOLATION(violationmask)




// ====== READ BEFORE ADDING CONTRACTS ==================================================
// The functions in this file propagate SetLastError codes to their callers.
// Contracts are not guaranteed to preserve these codes (and no, we're not taking
// the overhead hit to make them do so. Don't bother asking.)
//
// Most of the wrappers have a contract of the form:
//
//     NOTHROW;
//     INJECT_FAULT(xxx);
//
// For such functions, use the special purpose construct:
//
//     WINWRAPPER_CONTRACT(xxx);
//
// For everything else, use STATIC_CONTRACT.
//     
#undef CONTRACT
#define CONTRACT $$$$$$$$READ_COMMENT_IN_WINFIX_CPP$$$$$$$$$$

#undef CONTRACTL
#define CONTRACTL $$$$$$$$READ_COMMENT_IN_WINFIX_CPP$$$$$$$$$$


#define WINWRAPPER_CONTRACT(stmt)
#define STATIC_WINWRAPPER_CONTRACT(stmt) 


ULONG DBCS_MAXWID=0;

void ForceUnicodeWrappers()
{
    LEAF_CONTRACT;
    return;
}


int UseUnicodeAPI()
{
    LEAF_CONTRACT;
    return TRUE;
}

BOOL OnUnicodeSystem()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

    if (DBCS_MAXWID == 0)
    {
        CPINFO  cpInfo;

        if (GetCPInfo(CP_ACP, &cpInfo))
            DBCS_MAXWID = cpInfo.MaxCharSize;
        else
            DBCS_MAXWID = 2;
    }

    return TRUE;
}



/////////////////////////////////////////////////////////////////////////
//
// WARNING: below is a very large #ifdef that groups together all the
//          wrappers that are X86-only.  They all mirror some function
//          that is known to be available on the non-X86 win32 platforms
//          in only the Unicode variants.
//
/////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////
//
// END OF X86-ONLY wrappers
//
//////////////////////////////////////////////



//-----------------------------------------------------------------------------
// WszConvertToUnicode
//
// @func Convert a string from Ansi to Unicode
//
// @devnote cbIn can be -1 for Null Terminated string
//
// @rdesc HResult indicating status of Conversion
//      @flag S_OK | Converted to Ansi
//      @flag S_FALSE | Truncation occurred
//      @flag E_OUTOFMEMORY | Allocation problem.
//      @flag ERROR_NO_UNICODE_TRANSLATION | Invalid bytes in this code page.
//-----------------------------------------------------------------------------------
HRESULT WszConvertToUnicode
    (
    LPCSTR          szIn,       //@parm IN | Ansi String
    LONG            cbIn,       //@parm IN | Length of Ansi String in bytest
    __deref_inout_opt LPWSTR*         lpwszOut,   //@parm INOUT | Unicode Buffer
    __inout_opt ULONG*          lpcchOut,   //@parm INOUT | Length of Unicode String in characters -- including '\0'
    BOOL            fAlloc      //@parm IN | Alloc memory or not
    )
{
    WINWRAPPER_CONTRACT(return E_OUTOFMEMORY;);

    ULONG       cchOut;
    ULONG       cbOutJunk = 0;
//  ULONG       cchIn = szIn ? strlen(szIn) + 1 : 0;

//  _ASSERTE(lpwszOut);

    if (!(lpcchOut))
        lpcchOut = &cbOutJunk;

    if ((szIn == NULL) || (cbIn == 0))
    {
        *lpwszOut = NULL;
        if( lpcchOut )
            *lpcchOut = 0;
        return S_OK;
    }

    // Allocate memory if requested.   Note that we allocate as
    // much space as in the unicode buffer, since all of the input
    // characters could be double byte...
    if (fAlloc)
    {
        // Determine the number of characters needed
        cchOut = (MultiByteToWideChar(CP_ACP,
                                MB_ERR_INVALID_CHARS,
                                szIn,
                                cbIn,
                                NULL,
                                0));

        if (cchOut == 0)
            return HRESULT_FROM_WIN32(GetLastError()); // NOTREACHED

        // _ASSERTE( cchOut != 0 );
        *lpwszOut = (LPWSTR) new (nothrow) WCHAR[cchOut];
        *lpcchOut = cchOut;     // Includes '\0'.

        if (!(*lpwszOut))
        {
//          TRACE("WszConvertToUnicode failed to allocate memory");
            return E_OUTOFMEMORY;
        }
    }

    if( !(*lpwszOut) )
        return S_OK;
//  _ASSERTE(*lpwszOut);

    cchOut = (MultiByteToWideChar(CP_ACP,
                                  MB_ERR_INVALID_CHARS,
                                  szIn,
                                  cbIn,
                                  *lpwszOut,
                                  *lpcchOut));

    if (cchOut)
    {
        *lpcchOut = cchOut;
        return S_OK;
    }


//  _ASSERTE(*lpwszOut);
    if( fAlloc )
    {
        delete[] *lpwszOut;
        *lpwszOut = NULL;
    }
/*
    switch (GetLastError())
    {
        case    ERROR_NO_UNICODE_TRANSLATION:
        {
            OutputDebugString(TEXT("ODBC: no unicode translation for installer string"));
            return E_FAIL;
        }

        default:


        {
            _ASSERTE("Unexpected unicode error code from GetLastError" == NULL);
            return E_FAIL
        }
    }
*/
    return HRESULT_FROM_WIN32(GetLastError()); // NOTREACHED
}


//-----------------------------------------------------------------------------
// WszConvertToAnsi
//
// @func Convert a string from Unicode to Ansi
//
// @rdesc HResult indicating status of Conversion
//      @flag S_OK | Converted to Ansi
//      @flag S_FALSE | Truncation occurred
//      @flag E_OUTOFMEMORY | Allocation problem.
//-----------------------------------------------------------------------------------
HRESULT WszConvertToAnsi
    (
    LPCWSTR         szIn,       //@parm IN | Unicode string
    __deref_inout_opt LPSTR*          lpszOut,    //@parm INOUT | Pointer for buffer for ansi string
    ULONG           cbOutMax,   //@parm IN | Max string length in bytes
    __inout_opt ULONG*          lpcbOut,    //@parm INOUT | Count of bytes for return buffer
    BOOL            fAlloc,     //@parm IN | Alloc memory or not
    BOOL            bBestFitMapping      //@parm IN | Alloc memory or not
    )
{
    STATIC_WINWRAPPER_CONTRACT(return E_OUTOFMEMORY;);

    //SCAN_IGNORE_FAULT;  // we don't call 'new' when fAlloc is true

    ULONG           cchInActual;
    ULONG           cbOutJunk;
    ULONG           cchIn = szIn ? lstrlenW (szIn) + 1 : 0;

    _ASSERTE(lpszOut != NULL);

    if (!(lpcbOut))
        lpcbOut = &cbOutJunk;

    if ((szIn == NULL) || (cchIn == 0))
    {
        *lpszOut = NULL;
        *lpcbOut = 0;
        return S_OK;
    }

    // Allocate memory if requested.   Note that we allocate as
    // much space as in the unicode buffer, since all of the input
    // characters could be double byte...
    cchInActual = cchIn;
    if (fAlloc)
    {
        cbOutMax = (WideCharToMultiByte(CP_ACP,
                                        0,
                                        szIn,
                                        cchInActual,
                                        NULL,
                                        0,
                                        NULL,
                                        FALSE));

        *lpszOut = (LPSTR) new (nothrow) CHAR[cbOutMax];

        if (!(*lpszOut))
        {
//          TRACE("WszConvertToAnsi failed to allocate memory");
            SetLastError(ERROR_OUTOFMEMORY);
            return E_OUTOFMEMORY;
        }

    }

    if (!(*lpszOut))
        return S_OK;

    BOOL usedDefaultChar = FALSE;
    *lpcbOut = (WszWideCharToMultiByte(CP_ACP,
                                       bBestFitMapping ? 0 : WC_NO_BEST_FIT_CHARS,
                                       szIn,
                                       cchInActual,
                                       *lpszOut,
                                       cbOutMax,
                                       NULL,
                                       &usedDefaultChar));

    // If we failed, make sure we clean up.
    if ((*lpcbOut == 0 && cchInActual > 0) || usedDefaultChar)
    {
        if (fAlloc) {
            delete[] *lpszOut;
            *lpszOut = NULL;
        }

        // Don't allow default character replacement (nor best fit character
        // mapping, which we've told WC2MB to treat by using the default
        // character).  This prevents problems with characters like '\'.
        // Note U+2216 (Set Minus) looks like a '\' and may get mapped to
        // a normal backslash (U+005C) implicitly here otherwise, causing
        // a potential security bug.
        if (usedDefaultChar) {
            SetLastError(ERROR_NO_UNICODE_TRANSLATION);
            return HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION);
        }

        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Overflow on unicode conversion
    if (*lpcbOut > cbOutMax)
    {
        // If we had data truncation before, we have to guess
        // how big the string could be.   Guess large.
        if (cchIn > cbOutMax)
            *lpcbOut = cchIn * DBCS_MAXWID;

        return S_FALSE;
    }

    // handle external (driver-done) truncation
    if (cchIn > cbOutMax)
        *lpcbOut = cchIn * DBCS_MAXWID;
//  _ASSERTE(*lpcbOut);

    return S_OK;
}


// Running with an interactive workstation.
BOOL RunningInteractive()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

    static int fInteractive = -1;
    if (fInteractive != -1)
        return fInteractive != 0;

    if (!RunningOnWin95())
    {
    }
    if (fInteractive != 0)
        fInteractive = 1;

    return fInteractive != 0;
}




BOOL
WszCreateProcess(
    LPCWSTR lpApplicationName,
    LPCWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
    )
{
    WINWRAPPER_CONTRACT(SetLastError(ERROR_OUTOFMEMORY); return 0;);

    if (UseUnicodeAPI())
    {
        BOOL fResult;
        DWORD err;
        {
            size_t commandLineLength = wcslen(lpCommandLine) + 1;
            NewArrayHolder<WCHAR> nonConstCommandLine(new (nothrow) WCHAR[commandLineLength]);
            if (nonConstCommandLine.IsNull())
            {
                SetLastError(ERROR_OUTOFMEMORY);
                return 0;
            }
            
            memcpy(nonConstCommandLine, lpCommandLine, commandLineLength * sizeof(WCHAR));
            
            fResult =CreateProcessW(lpApplicationName,
                                  nonConstCommandLine,
                                  lpProcessAttributes,
                                  lpThreadAttributes,
                                  bInheritHandles,
                                  dwCreationFlags,
                                  lpEnvironment,
                                  lpCurrentDirectory,
                                  lpStartupInfo,
                                  lpProcessInformation);
            err = GetLastError();                                  
        }
        SetLastError(err);
        return fResult;                              
    }


    _ASSERTE(!"This should never be executed if FEATURE_PAL is defined!");
    return 0;

}


DWORD
WszGetWorkingSet()
{
    WINWRAPPER_CONTRACT(SetLastError(ERROR_OUTOFMEMORY); return 0;);

    DWORD dwMemUsage = 0;


    return dwMemUsage;
}

