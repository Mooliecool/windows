//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

#include <stdio.h>
#include <mscoree.h>
#include <windows.h>
#include <Tlhelp32.h>

#define NumItems(x) sizeof(x)/sizeof(*x)

// This is the function pointer defintion for the shim API GetRequestedRuntimeInfoInfo.
// It has existed in mscoree.dll since v1.1, and in v2.0 it was modified to take "runtimeInfoFlags"
// which allow us to get even more information.
typedef HRESULT (STDAPICALLTYPE *PGetRRI)(LPCWSTR pExe, 
                                          LPCWSTR pwszVersion,
                                          LPCWSTR pConfigurationFile, 
                                          DWORD startupFlags,
                                          DWORD runtimeInfoFlags, 
                                          LPWSTR pDirectory, 
                                          DWORD dwDirectory, 
                                          DWORD *dwDirectoryLength, 
                                          LPWSTR pVersion, 
                                          DWORD cchBuffer, 
                                          DWORD* dwlength);

// This is the function pointer defintion for the shim API GetCorVersion.
// It has existed in mscoree.dll since v1.0, and will display the version of the runtime that is currently
// loaded into the process. If a CLR is not loaded into the process, it will load the latest version.
typedef HRESULT (STDAPICALLTYPE *PGetCV)(LPWSTR szBuffer, 
                                         DWORD cchBuffer,
                                         DWORD* dwLength);

//-------------------------------------------------------------
// PrintAllRuntimes
//
// This prints all of the runtimes installed on the machine
//-------------------------------------------------------------
int PrintAllRuntimes()
{
    BOOL V10installed = FALSE;     // Is v1.0 installed on the machine
    BOOL V11installed = FALSE;     // Is v1.1 installed on the machine
    WCHAR LatestRuntime[30] = {0}; // Latest runtime on the machine
    DWORD LatestRuntimeNumChars = 0;
    PGetRRI GetRequestedRuntimeInfoFunc = NULL;
    PGetCV GetCorVersionFunc = NULL;
    HMODULE MscoreeHandle = NULL;
    HRESULT hr = S_OK;

    // First, if mscoree.dll is not found on the machine, then there aren't any CLRs on the machine
    MscoreeHandle = LoadLibraryA("mscoree.dll");
    if (MscoreeHandle == NULL)
        goto DoneFindingRuntimes;


    // There were certain OS's that shipped with a "placeholder" mscoree.dll. The existance of this DLL
    // doesn't mean there are CLRs installed on the box.
    //
    // If this mscoree doesn't have an implementation for GetCORVersion, then we know it's one of these
    // placeholder dlls.
    GetCorVersionFunc = (PGetCV)GetProcAddress(MscoreeHandle, "GetCORVersion");
    
    if (GetCorVersionFunc == NULL)
        goto DoneFindingRuntimes;

    // Ok, so we now know that the CLR was, at one time, installed on this machine. Let's see what versions
    // of the runtime are on the box.

    // v1.0 and v1.1 had an annoying habit of popping up dialogs whenever you asked for runtimes that didn't exist.
    // We'll surpress those dialogs with this statement.
    SetErrorMode(SEM_FAILCRITICALERRORS);

    // v1.1 of mscoree shipped with the API GetRequestedRuntimeInfo(). This function will help us with identifying
    // runtimes.

    GetRequestedRuntimeInfoFunc = (PGetRRI)GetProcAddress(MscoreeHandle, "GetRequestedRuntimeInfo");

    if (GetRequestedRuntimeInfoFunc == NULL)
    {
        // Ok, that API didn't exist. We've got the v1.0 mscoree.dll on the box. We're guaranteed that there isn't
        // a later version of the CLR on the machine, but we're not 100% guaranteed that v1.0 of the CLR is on the
        // box.

        // Unfortuately, the only way to verify that v1.0 is on the box is to try and spin up v1.0 of the CLR and
        // see if it works.

        WCHAR Version[50];
        DWORD VersionNumChars = NumItems(Version);

        hr = GetCorVersionFunc(Version, VersionNumChars, &VersionNumChars);

        // If this failed, then either the v1.0 CLR didn't exist on the machine, or, if the buffer wasn't
        // big enough to copy the version information, then something is messed up on the machine (v1.0.3705 should
        // fit in a 50 character buffer)
        if (FAILED(hr))
            goto DoneFindingRuntimes;

        // If the returned string is not v1.0.3705, then this machine is messed up
        if (wcscmp(Version, L"v1.0.3705"))
        {
            printf("Installation error on this machine. v1.0 of mscoree.dll is running %S of the CLR.\n", Version);
            goto DoneFindingRuntimes;
        }

        // Ok, we've verified that v1.0 is installed.
        V10installed = TRUE;
        goto DoneFindingRuntimes;
    }

    // Ok, we know that, at a minimum, v1.1 of mscoree is installed on the machine. That makes this job much easier.

    // This function call will pop up a dialog if these runtimes don't exist on the machine, so make sure you call
    // SetErrorMode(SEM_FAILCRITICALERRORS); as we did up above
    
    WCHAR Version[50]; // The version of the runtime that satisfies the runtime request
    DWORD VersionNumChars = 0;
    WCHAR Directory[MAX_PATH]; // The top level directory where the runtime is located. Usually is %windir%\microsoft.net\framework
    DWORD DirectoryNumChars = 0;

    // Check to see if v1.0 is installed on the machine
    
    hr = GetRequestedRuntimeInfoFunc(NULL, // pExe
                   L"v1.0.3705", // pwszVersion
                   NULL, // ConfigurationFile
                   0, // startupFlags
                   0, // v1.1, this is reserved, in v2.0, runtimeInfoFlags 
                   Directory, // pDirectory
                   NumItems(Directory), // dwDirectory
                   &DirectoryNumChars, // dwDirectoryLength
                   Version, // pVersion
                   NumItems(Version), // cchBuffer
                   &VersionNumChars); // dwlength

    if (SUCCEEDED(hr))
        V10installed = TRUE;


    // Check to see if v1.1 is installed on the machine
    hr = GetRequestedRuntimeInfoFunc(NULL, // pExe
                   L"v1.1.4322", // pwszVersion
                   NULL, // ConfigurationFile
                   0, // startupFlags
                   0, // v1.1, this is reserved, in v2.0, runtimeInfoFlags 
                   Directory, // pDirectory
                   NumItems(Directory), // dwDirectory
                   &DirectoryNumChars, // dwDirectoryLength
                   Version, // pVersion
                   NumItems(Version), // cchBuffer
                   &VersionNumChars); // dwlength

    if (SUCCEEDED(hr))
        V11installed = TRUE;


    // The same thing can be done for v2.0 when the final version number of v2.0 is decided upon.


    // The v2.0 shim allows us to use flags for this function that makes it easier to use. The v1.1 mscoree.dll will
    // not allow us to call this function with 3 NULLs. However, the v2.0 mscoree.dll, along with the RUNTIME_INFO_UPGRADE_VERSION
    // flag, will return us the latest version of the CLR on the machine.

    hr = GetRequestedRuntimeInfoFunc(NULL, // pExe
                   NULL, // pwszVersion
                   NULL, // ConfigurationFile
                   0, // startupFlags
                   RUNTIME_INFO_UPGRADE_VERSION|RUNTIME_INFO_DONT_RETURN_DIRECTORY|RUNTIME_INFO_DONT_SHOW_ERROR_DIALOG, // runtimeInfoFlags,
                   NULL, // pDirectory
                   0, // dwDirectory
                   NULL, // dwDirectoryLength
                   LatestRuntime, // pVersion
                   NumItems(LatestRuntime), // cchBuffer
                   &LatestRuntimeNumChars); // dwlength

    // If this fails, then v2.0 of mscoree.dll was not installed on the machine.
      
DoneFindingRuntimes:

    printf("Versions installed on the machine:\n");

    if (V10installed)
        printf("v1.0.3705\n");

    if (V11installed)
        printf("v1.1.4322\n");

    if (*LatestRuntime)
        printf("%S\n", LatestRuntime);

    // If we didn't find any runtimes
    if (!V10installed && !V11installed && !*LatestRuntime)
        printf("<none>\n");

    return 0;
}// PrintAllRuntimes


//-------------------------------------------------------------
// main
//
// Entrypoint - Determines what the user wants to do and does it.
//-------------------------------------------------------------
int _cdecl main(int argc, char**argv)
{
    return PrintAllRuntimes();
}// main
