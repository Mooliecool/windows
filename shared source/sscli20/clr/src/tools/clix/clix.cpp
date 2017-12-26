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
// File: clix.cpp
// 
// Managed application launcher
// ===========================================================================

#include "windows.h"
#include "stdlib.h"

#include "clix.h"
#include "cor.h"

#include <palstartup.h>

static const WCHAR c_wzSatellite[] = L"clix.satellite";

HSATELLITE LoadSatellite(void)
{
    WCHAR Path[_MAX_PATH];

    if (!PAL_GetPALDirectoryW(Path, _MAX_PATH)) {
        return NULL;
    }
    if (wcslen(Path) + sizeof(c_wzSatellite)/sizeof(WCHAR) >= _MAX_PATH) {
        return NULL;
    }
    wcscat(Path, c_wzSatellite);
    return PAL_LoadSatelliteResourceW(Path);
}

void DisplayMessageFromSystem(DWORD LastErr)
{
    DWORD dw;
    WCHAR ErrorBuffer[256];

    dw = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                LastErr,
                0,
                ErrorBuffer,
                sizeof(ErrorBuffer)/sizeof(WCHAR),
                NULL);
    if (dw == 0) {
        fwprintf(stderr, L"ERROR:  LastError=%d\n", LastErr);
        return;
    }

    fwprintf(stderr, L"%s", ErrorBuffer);
}

void DisplayMessage(UINT MsgID)
{
    HSATELLITE hSat = NULL;
    UINT u;
    WCHAR SatelliteBuffer[256];

    hSat = LoadSatellite();
    if (!hSat) {
        goto LError;
    }

    u = PAL_LoadSatelliteStringW(hSat, 
                 MsgID, 
                 SatelliteBuffer, 
                 sizeof(SatelliteBuffer)/sizeof(WCHAR));
    if (u == sizeof(SatelliteBuffer)/sizeof(WCHAR) || u == 0) {
        goto LError;
    }

    fwprintf(stderr, SatelliteBuffer);
    goto LExit;
    
LError:
    fwprintf(stderr, L"ERROR:  MessageID=%d\n", MsgID);

LExit:
    if (hSat) {
        PAL_FreeSatelliteResource(hSat);
    }
}

DWORD Launch(WCHAR* pFileName, WCHAR* pCmdLine)
{
    WCHAR exeFileName[MAX_PATH + 1];
    DWORD dwAttrs;
    DWORD dwError;
    DWORD nExitCode;

    dwAttrs = ::GetFileAttributesW(pFileName);

    if (dwAttrs == INVALID_FILE_ATTRIBUTES) {
        dwError = ::GetLastError();
    }
    else if ((dwAttrs & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        dwError = ERROR_FILE_NOT_FOUND;
    }
    else {
        dwError = ERROR_SUCCESS;
    }
 
    if (dwError == ERROR_FILE_NOT_FOUND) {
        // If the file doesn't exist, append a '.exe' extension and
        // try again.
 
        const WCHAR *exeExtension = L".exe";
        if (wcslen(pFileName) + wcslen(exeExtension) <
                sizeof(exeFileName) / sizeof(WCHAR))
        {
            wcscpy(exeFileName, pFileName);
            wcscat(exeFileName, exeExtension);
            dwAttrs = ::GetFileAttributesW(exeFileName);

            if (dwAttrs == INVALID_FILE_ATTRIBUTES) {
                dwError = ::GetLastError();
            }
            else if ((dwAttrs & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                dwError = ERROR_FILE_NOT_FOUND;
            }
            else {
                pFileName = exeFileName;
                dwError = ERROR_SUCCESS;
            }
        }
    }
 
    if (dwError != ERROR_SUCCESS) {
        // We can't find the file, or there's some other problem. Exit with an error.
        fwprintf(stderr, L"%s: ", pFileName);
        DisplayMessageFromSystem(dwError);
        return 1;   // error
    }

    nExitCode = _CorExeMain2(NULL, 0, pFileName, NULL, pCmdLine);

    // _CorExeMain2 never returns with success
    _ASSERTE(nExitCode != 0);

    DisplayMessageFromSystem(::GetLastError());

    return nExitCode;
}

int __cdecl main(int argc, char **argv)
{
    DWORD nExitCode = 1; // error
    WCHAR* pwzCmdLine;

    if ( !PAL_RegisterLibrary(L"rotor_palrt")
            || !PAL_RegisterLibrary(L"sscoree") ) {
        DisplayMessageFromSystem(::GetLastError());
        return 1;
    }

#if FV_DEBUG_CORDBDB
    // This gives us the opportunity to attach a debugger, modify
    // DebuggerAttached and continue
    static bool DebuggerAttached = false;    
    while (DebuggerAttached == false)
    {
        Sleep(1);
    }
#endif

    pwzCmdLine = ::GetCommandLineW();

    // Allocate for the worst case storage requirement.
    WCHAR *pAlloc = (WCHAR*)malloc((wcslen(pwzCmdLine) + 1) * sizeof(WCHAR));
    if (!pAlloc) {
        DisplayMessageFromSystem(ERROR_OUTOFMEMORY);
        return 1;
    }

    WCHAR* psrc = pwzCmdLine;
    WCHAR* pdst = pAlloc;
    BOOL inquote = FALSE;

    WCHAR* pModuleName;
    WCHAR* pActualCmdLine;

    // First, parse the program name. Anything up to the first whitespace outside 
    // a quoted substring is accepted (algorithm from clr/src/vm/util.cpp)

    for (;;)
    {
        switch (*psrc)
        {
        case L'\"':
            inquote = !inquote;
            psrc++;
            continue;

        case '\0':
            break;

        case L' ':
        case L'\t':
            if (!inquote)
                break;
            // intentionally fall through
        default:
            psrc++;
            continue;
        }

        break;
    }

    // Skip the whitespace
    while (*psrc == ' ' || *psrc == '\t')
    {
        psrc++;
    }

    if (*psrc == '\0')
    {
        DisplayMessage(MSG_Usage);
        goto LExit;
    }

    pActualCmdLine = psrc;

    // Parse the first arg - the name of the module to run
    pModuleName = pdst;

    for (;;)
    {
        switch (*psrc)
        {
        case L'\"':
            inquote = !inquote;
            psrc++;
            continue;

        case '\0':
            break;

        case L' ':
        case L'\t':
            if (!inquote)
                break;
            // intentionally fall through
        default:
            *pdst++ = *psrc++;
            continue;
        }

        break;
    }

    // zero terminate
    *pdst = 0;

    nExitCode = Launch(pModuleName, pActualCmdLine);

LExit:
    free(pAlloc);

    return nExitCode;
}
