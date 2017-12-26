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
// SafeWrap.cpp
//
// This file contains wrapper functions for Win32 API's that take SStrings 
// and use CLR-safe holders.
//
// See guidelines in SafeWrap.h for writing these APIs.
//*****************************************************************************

#include "stdafx.h"                     // Precompiled header key.
#include "safewrap.h"
#include "winwrap.h"                    // Header for macros and functions.
#include "utilcode.h"
#include "holder.h"
#include "ndpversion.h"
#include "sstring.h"
#include "ex.h"

//-----------------------------------------------------------------------------
// Get the current directory. 
// On success, returns true and sets 'Value' to unicode version of cur dir.
// Throws on all failures. This should mainly be oom.
//-----------------------------------------------------------------------------
void ClrGetCurrentDirectory(SString & value)
{
    CONTRACTL
    {
        THROWS; 
        GC_NOTRIGGER; 
    }
    CONTRACTL_END;

    // Get size needed
    DWORD lenWithNull = WszGetCurrentDirectory(0, NULL);

    // Now read it for content.
    WCHAR * pCharBuf = value.OpenUnicodeBuffer(lenWithNull);
    DWORD lenWithoutNull = WszGetCurrentDirectory(lenWithNull, pCharBuf);

    // An actual API failure in GetCurrentDirectory failure should be very rare, so we'll throw on those.
    if (lenWithoutNull == 0)
    {   
        value.CloseBuffer(0);    
        ThrowLastError();
    }
    if (lenWithoutNull != (lenWithNull - 1))
    {
        value.CloseBuffer(lenWithoutNull);
    
        // must have changed underneath us.
        ThrowHR(E_FAIL);
    }
    
    value.CloseBuffer(lenWithoutNull);
}

// Nothrowing wrapper.
bool ClrGetCurrentDirectoryNoThrow(SString & value)
{
    CONTRACTL
    {
        NOTHROW; 
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    bool fOk = true;
    EX_TRY
    {
        ClrGetCurrentDirectory(value);
    }
    EX_CATCH
    {
        fOk = false;
    }
    EX_END_CATCH(SwallowAllExceptions)
    return fOk;
}
//-----------------------------------------------------------------------------
// Reads an environment variable into the given SString.
// Returns true on success, false on failure (includes if the var does not exist).
// May throw on oom.
//-----------------------------------------------------------------------------
bool ClrGetEnvironmentVariable(LPCSTR szEnvVarName, SString & value)
{
    CONTRACTL
    {
        THROWS; 
        GC_NOTRIGGER;
        
        PRECONDITION(szEnvVarName != NULL);
    }
    CONTRACTL_END;

    // First read it to get the needed length.
    DWORD lenWithNull = GetEnvironmentVariableA(szEnvVarName, NULL, 0);
    if (lenWithNull == 0)
    {
        return false;
    }

    // Now read it for content.
    char * pCharBuf = value.OpenANSIBuffer(lenWithNull);                   
    DWORD lenWithoutNull = GetEnvironmentVariableA(szEnvVarName, pCharBuf, lenWithNull);
    value.CloseBuffer(lenWithoutNull);

    if (lenWithoutNull != (lenWithNull - 1))
    {
        // Env var must have changed underneath us.
        return false;
    }
    return true;
}

// Nothrowing wrapper.
bool ClrGetEnvironmentVariableNoThrow(LPCSTR szEnvVarName, SString & value)
{
    CONTRACTL
    {
        NOTHROW; 
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    

    bool fOk = false;
    EX_TRY
    {
        fOk = ClrGetEnvironmentVariable(szEnvVarName, value);
    }
    EX_CATCH
    {
        fOk = false;
    }
    EX_END_CATCH(SwallowAllExceptions)
    return fOk;
}

void ClrGetModuleFileName(HMODULE hModule, SString & value)
{
    CONTRACTL
    {
        THROWS; 
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    {
        WCHAR * pCharBuf = value.OpenUnicodeBuffer(_MAX_PATH);                   
        DWORD numChars = GetModuleFileNameW(hModule, pCharBuf, _MAX_PATH);
        value.CloseBuffer(numChars);
    }
}

bool ClrGetModuleFileNameNoThrow(HMODULE hModule, SString & value)
{
    CONTRACTL
    {
        NOTHROW; 
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    bool fOk = true;
    EX_TRY
    {
        ClrGetModuleFileName(hModule, value);
    }
    EX_CATCH
    {
        fOk = false;
    }
    EX_END_CATCH(SwallowAllExceptions)
    return fOk;
}

ClrDirectoryEnumerator::ClrDirectoryEnumerator(LPCWSTR pBaseDirectory, LPCWSTR pMask /*= L"*"*/)
{
    CONTRACTL
    {
        THROWS; 
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    StackSString strMask;
    strMask.Set(pBaseDirectory, SL(L"\\"), pMask);
    dirHandle = WszFindFirstFile(strMask, &data);

    if (dirHandle == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() != ERROR_NO_MORE_FILES)
            ThrowLastError();
    }

    fFindNext = FALSE;
}

bool ClrDirectoryEnumerator::Next()
{
    CONTRACTL
    {
        THROWS; 
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (dirHandle == INVALID_HANDLE_VALUE)
        return FALSE;

    for (;;)
    {
        if (fFindNext)
        {
            if (!WszFindNextFile(dirHandle, &data))
            {
                if (GetLastError() != ERROR_NO_MORE_FILES)
                    ThrowLastError();

                return FALSE;
            }
        }
        else
        {
            fFindNext  = TRUE;
        }

        // Skip junk
        if (wcscmp(data.cFileName, L".") != 0 && wcscmp(data.cFileName, L"..") != 0)
            return TRUE;
    }        
}

