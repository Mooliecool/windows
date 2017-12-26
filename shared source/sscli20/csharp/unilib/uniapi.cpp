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

#include "pch.h"
#include "uniapi.h"

// Note this doesn't set the errno if there's an error but no one seemed to be using it.
int WINAPI W_Access(PCWSTR pPathName, int mode)
{
    // Only support checking for read access
    _ASSERTE(mode == 0x4);
    HANDLE hFile;
    hFile = CreateFileW(pPathName,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        // Not Errno not set
        CloseHandle (hFile);
        return 0;
    }
    return -1;
}

BOOL WINAPI W_IsUnicodeSystem()
{
    return TRUE;
}

int WINAPI W_LoadString (HINSTANCE hinst, UINT id, PWSTR pch, int cch)
{
    return PAL_LoadSatelliteStringW ((HSATELLITE)hinst, id, pch, cch);
}
