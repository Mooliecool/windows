/****************************** Module Header ******************************\
Module Name:  CppLoadLibrary.cpp
Project:      CppLoadLibrary
Copyright (c) Microsoft Corporation.

This is an example of dynamically loading a DLL using the APIs LoadLibrary, 
GetProcAddress and FreeLibrary. In contrast with implicit linking (static 
loading), dynamic loading does not require the LIB file, and the  
application loads the module just before we call a function in the DLL. 
The API functions LoadLibrary and GetProcAddress are used to load the DLL 
and then retrieve the address of a function in the export table. Because 
we explicitly invoke these APIs, this kind of loading is also referred to 
as explicit linking. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes
#include <stdio.h>
#include <windows.h>
#pragma endregion


// Function pointer types for functions exported from the DLL module

typedef int     (_cdecl* LPFNGETSTRINGLENGTH1)      (PCWSTR);

// CALLBACK, aka __stdcall, can only be used for stdcall methods. If it is
// used for __cdecl methods, this error will be thrown in runtime: The value 
// of ESP was not properly saved across a function call. This is usually a 
// result of calling a function declared with one calling convention with a
// function pointer declared with a different calling convention.
typedef int     (CALLBACK* LPFNGETSTRINGLENGTH2)    (PCWSTR);

// Type-definition of the 'PFN_COMPARE' callback function, and the Max 
// function that requires the callback as one of the arguments.
typedef int     (CALLBACK* PFN_COMPARE)             (int, int);
typedef int     (CALLBACK* LPFNMAX)                 (int, int, PFN_COMPARE);


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
    HINSTANCE hModule = NULL;

	// The name of the module to be dynamically-loaded.
	PCWSTR pszModuleName = L"CppDynamicLinkLibrary";

	// Check whether or not the module is loaded.
	fLoaded = IsModuleLoaded(pszModuleName);
    wprintf(L"Module \"%s\" is %sloaded\n", pszModuleName, fLoaded ? L"" : L"not ");

	// Dynamically load the library.
    wprintf(L"Load the library\n");
	hModule = LoadLibrary(pszModuleName);
	if (hModule == NULL)
	{
        wprintf(L"LoadLibrary failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Check whether or not the module is loaded.
	fLoaded = IsModuleLoaded(pszModuleName);
    wprintf(L"Module \"%s\" is %sloaded\n", pszModuleName, fLoaded ? L"" : L"not ");

    // 
    // Access the global data exported from a module.
    // 

    // Dynamically-loaded DLL does not allow you to access the global data 
    // exported from the DLL.

    // 
    // Call the functions exported from a module.
    // 

    PCWSTR pszString = L"HelloWorld";
    int nLength;

    // Call int /*__cdecl*/ GetStringLength1(PWSTR pszString);
    LPFNGETSTRINGLENGTH1 lpfnGetStringLength1 = (LPFNGETSTRINGLENGTH1) 
        GetProcAddress(hModule, "GetStringLength1");
    if (lpfnGetStringLength1 == NULL)
    {
        wprintf(L"GetStringLength1 cannot be found (Error: 0x%08lx)\n", 
            GetLastError());
        goto Cleanup;
    }
	nLength = lpfnGetStringLength1(pszString);
    wprintf(L"Function: GetStringLength1(\"%s\") => %d\n", pszString, nLength);

    // Call int __stdcall GetStringLength2(PWSTR pszString);
    LPFNGETSTRINGLENGTH2 lpfnGetStringLength2 = (LPFNGETSTRINGLENGTH2) 
        GetProcAddress(hModule, "_GetStringLength2@4");
    if (lpfnGetStringLength2 == NULL)
    {
        wprintf(L"GetStringLength2 cannot be found (Error: 0x%08lx)\n", 
            GetLastError());
        goto Cleanup;
    }
    nLength = lpfnGetStringLength2(pszString);
    wprintf(L"Function: GetStringLength2(\"%s\") => %d\n", pszString, nLength);

    //
    // Call the callback functions exported from a module.
    //

    // Call int __stdcall Max(int a, int b, PFN_COMPARE cmpFunc);
    LPFNMAX lpfnMax = (LPFNMAX)GetProcAddress(hModule, "Max");
    if (lpfnMax == NULL)
    {
        wprintf(L"Max cannot be found (Error: 0x%08lx)\n", GetLastError());
        goto Cleanup;
    }
    int max = lpfnMax(2, 3, &CompareInts);
    wprintf(L"Function: Max(2, 3) => %d\n", max);

    //
    // Use the class exported from a module.
    //

    // Dynamically-loaded DLL does not allow you to use the class exported 
    // from the DLL.

Cleanup:

    if (hModule)
    {
        // Attempt to free and unload the library.
        wprintf(L"Unload the dynamically-loaded DLL\n");
        if (!FreeLibrary(hModule))
        {
            wprintf(L"FreeLibrary failed w/err 0x%08lx\n", GetLastError());
        }
    }
    
    // Check whether or not the module is loaded.
	fLoaded = IsModuleLoaded(pszModuleName);
    wprintf(L"Module \"%s\" is %sloaded\n", pszModuleName, fLoaded ? L"" : L"not ");

	return 0;
}