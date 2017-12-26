/****************************** Module Header ******************************\
Module Name:  CppDelayloadDll.cpp
Project:      CppDelayloadDll
Copyright (c) Microsoft Corporation.

The support of delayed loading of DLLs in Visual C++ linker relieves us of 
the need to use the API LoadLibrary and GetProcAddress to implement DLL 
delayed loading. DLL is implicitly linked but not actually loaded until the 
code attempts to reference a symbol contained within the DLL. An  
application can delay load a DLL using the /DELAYLOAD (Delay Load Import) 
linker option with a helper function (default implementation provided by 
Visual C++, see http://msdn.microsoft.com/en-us/library/09t6x5ds.aspx). The 
helper function will load the DLL at run time by calling LoadLibrary and 
GetProcAddress for us. 

We should consider delay loading a DLL if the program may not call a 
function in the DLL or a function in the DLL may not get called until late 
in the program's execution. Delay loading a DLL saves the initialization 
time when the executable files starts up.

This code sample demonstrates delay loading CppDynamicLinkLibrary.dll, 
importing and using the data, functions and classes exported from the DLL, 
and unloading the module.

You can also learn how to hook notifications and failures in the delay-load 
process, and customize the default delay-load behavior. In this code sample, 
if the target DLL CppDynamicLinkLibrary.dll fails to be loaded, you will be 
asked to input the full path of the DLL, and the application will attempt to 
load the DLL again. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes and Imports
#include <stdio.h>
#include <windows.h>

// The header file of the DLL to be delay-loaded
#include "CppDynamicLinkLibrary.h"

// For error handling & advanced features (e.g. Unload the delayloaded DLL)
#include <delayimp.h>

// This line is not necessary because VS setting automatically imports
// delayimp.lib when we specify one or more DLLs for delay loading.
// #pragma comment(lib, "delayimp.lib")
#pragma endregion


#pragma region Delay-load Notification Hooks (Optional)
// Setup notification hooks and failure hooks if you want to customize the 
// default behaviors in the delay-load helper function.


FARPROC WINAPI DelayLoadHook(unsigned dliNotify, PDelayLoadInfo pdli)
{
    switch (dliNotify)
    {
    case dliStartProcessing:

        // If you want to return control to the delay-load helper, return 0. 
        // Otherwise, return a pointer to a FARPROC helper function that will 
        // be used instead, thereby bypassing the rest of the helper.
        break;

    case dliNotePreLoadLibrary:

        // If you want to return control to the delay-load helper, return 0.
        // Otherwise, return your own HMODULE to be used by the helper 
        // instead of having it call LoadLibrary itself.
        {
            // You can build the DLL path by yourself, and call LoadLibrary 
            // to load the DLL from the path. For simplicity, the sample uses 
            // the dll name to load the DLL, which is the default behavior of 
            // the helper function.
            HMODULE hLib = LoadLibraryA(pdli->szDll);
            return reinterpret_cast<FARPROC>(hLib);
        }

    case dliNotePreGetProcAddress:

        // If you want to return control to the delay-load helper, return 0. 
        // If you choose you may supply your own FARPROC function address and 
        // bypass the helper's call to GetProcAddress.
        break;

    case dliFailLoadLib : 

        // LoadLibrary failed.
        // If you don't want to handle this failure yourself, return 0. In 
        // this case the helper will raise an exception (ERROR_MOD_NOT_FOUND) 
        // and exit. If you want to handle the failure by loading an 
        // alternate DLL (for example), then return the HMODULE for the 
        // alternate DLL. The helper will continue execution with this 
        // alternate DLL and attempt to find the requested entrypoint via 
        // GetProcAddress.
        {
            printf("Failed to load the DLL %s w/err 0x%08lx.\n", pdli->szDll, 
                pdli->dwLastError);
            printf("Please input the path of the DLL file:\n");

            wchar_t szDll[MAX_PATH];
            fgetws(szDll, ARRAYSIZE(szDll), stdin);
            wchar_t *p = wcschr(szDll, L'\n');
            if (p != NULL)
            {
                *p = L'\0';  // Remove the trailing L'\n'
            }

            // Try to load the DLL again.
            HMODULE hLib = LoadLibrary(szDll);
            if (hLib == NULL)
            {
                wprintf(L"Still failed to load the DLL %s w/err 0x%08lx.\n", 
                    szDll, GetLastError());
            }
            return reinterpret_cast<FARPROC>(hLib);
        }

        break;

    case dliFailGetProc :

        // GetProcAddress failed.
        // If you don't want to handle this failure yourself, return 0. In 
        // this case the helper will raise an exception (ERROR_PROC_NOT_FOUND) 
        // and exit. If you choose you may handle the failure by returning an 
        // alternate FARPROC function address.
        printf("Failed to get the function %s.\n", pdli->dlp.szProcName);

        break;

    case dliNoteEndProcessing : 

        // This notification is called after all processing is done. There is 
        // no opportunity for modifying the helper's behavior at this point 
        // except by longjmp()/throw()/RaiseException. No return value is 
        // processed.

        break;
    }

    return NULL;
}


// At the global level, set the delay-load hooks.
PfnDliHook __pfnDliNotifyHook2 = DelayLoadHook;
PfnDliHook __pfnDliFailureHook2 = DelayLoadHook;

#pragma endregion


//
//   FUNCTION: IsModuleLoaded(PCWSTR)
//
//   PURPOSE: Check whether or not the specified module is loaded in the 
//   current process.
//
//   PARAMETERS:
//   * pszModuleName - the module name
//
//   RETURN VALUE: The function returns TRUE if the specified module is 
//   loaded in the current process. If the module is not loaded, the function 
//   returns FALSE.
//
BOOL IsModuleLoaded(PCWSTR pszModuleName) 
{
    // Get the module in the process according to the module name.
    HMODULE hMod = GetModuleHandle(pszModuleName);
    return (hMod != NULL);
}


//
//   FUNCTION: CompareInts(int, int)
//
//   PURPOSE: This is the callback function for the method Max exported from 
//   CppDynamicLinkLibrary.dll
//
//   PARAMETERS:
//   * a - the first integer
//   * b - the second integer
//
//   RETURN VALUE: The function returns a positive number if a > b, returns 0 
//   if a equals b, and returns a negative number if a < b.
//
int CALLBACK CompareInts(int a, int b)
{
    return (a - b);
}


int wmain(int argc, wchar_t *argv[])
{
    BOOL fLoaded = FALSE;

    // The name of the module to be delay-loaded.
    PCWSTR pszModuleName = L"CppDynamicLinkLibrary";

    // Check whether or not the module is loaded.
    fLoaded = IsModuleLoaded(pszModuleName);
    wprintf(L"Module \"%s\" is %sloaded\n", pszModuleName, fLoaded ? L"" : L"not ");

    //
    // Access the global data exported from a module.
    //

    // Delay-load does not allow you to access the global data exported from 
    // the DLL. The following line produce the fatal error LNK1194.
    //__declspec(dllimport) int g_nVal1;
    //wprintf(L"Global data: g_nVal1 = %d\n", g_nVal1);
    //wprintf(L"Global data: g_nVal2 = %d\n", g_nVal2);

    // 
    // Call the functions exported from a module.
    // 

    PCWSTR pszString = L"HelloWorld";
    int nLength;

    // Call int /*__cdecl*/ GetStringLength1(PCWSTR pszString);
    nLength = GetStringLength1(pszString);
    wprintf(L"Function: GetStringLength1(\"%s\") => %d\n", pszString, nLength);

    // Call int __stdcall GetStringLength2(PCWSTR pszString);
    nLength = GetStringLength2(pszString);
    wprintf(L"Function: GetStringLength2(\"%s\") => %d\n", pszString, nLength);

    // 
    // Call the callback functions exported from a module.
    // 

    int max = Max(2, 3, &CompareInts);
    wprintf(L"Function: Max(2, 3) => %d\n", max);

    // 
    // Use the class exported from a module.
    // 

    {
        CSimpleObject simpleObj;
        simpleObj.set_FloatProperty(1.2F);
        float fProp = simpleObj.get_FloatProperty();
        wprintf(L"Class: CSimpleObject::FloatProperty = %.2f\n", fProp);
    }

    // Check whether or not the module is loaded.
    fLoaded = IsModuleLoaded(pszModuleName);
    wprintf(L"Module \"%s\" is %sloaded\n", pszModuleName, fLoaded ? L"" : L"not ");

    //
    // Unload the delay-loaded DLL.
    //

    // NOTE: The argument to __FUnloadDelayLoadedDLL2 must exactly case-
    // sensitively match the DLL name specified by /DelayLoad. In Project 
    // Properies / Linker / Advanced / Delay Loaded DLL, you also need to 
    // select Support Unload (/DELAY:UNLOAD). If you choose "Don't Support 
    // Unload", calling __FUnloadDelayLoadedDLL2 does not unload the DLL.
    PCSTR pszDll = "CppDynamicLinkLibrary.dll";
    wprintf(L"Unload the delay-loaded DLL\n");
    if (!__FUnloadDelayLoadedDLL2(pszDll))
    {
        wprintf(L"The DLL failed to be unloaded\n");
    }

    // Check whether or not the module is loaded.
    fLoaded = IsModuleLoaded(pszModuleName);
    wprintf(L"Module \"%s\" is %sloaded\n", pszModuleName, fLoaded ? L"" : L"not ");

    return 0;
}