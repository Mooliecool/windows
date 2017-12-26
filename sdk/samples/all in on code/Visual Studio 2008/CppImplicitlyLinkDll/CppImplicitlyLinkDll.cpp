/****************************** Module Header ******************************\
* Module Name:  CppImplicitlyLinkDll.cpp
* Project:      CppImplicitlyLinkDll
* Copyright (c) Microsoft Corporation.
* 
* Normally, when we link to a DLL via a LIB file, the DLL is loaded when the  
* application starts up. This kind of loading is kwown as implicit linking,  
* because the system takes care of loading the DLL for us - all we need to  
* do is link with the LIB file.
* 
* After the configuration of linking, we can import symbols of a DLL into  
* the application using the keyword __declspec(dllimport) no matter whether  
* the symbols were exported with __declspec(dllexport) or with a .def file.
* 
* This sample demonstrates implicitly linking CppDynamicLinkLibrary.dll and 
* importing and using its symbols.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes and Imports
#include <stdio.h>
#include <windows.h>

// The header file of the DLL to be loaded.
#include "CppDynamicLinkLibrary.h"
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

    // The name of the module that is implicitly-linked.
	PCWSTR pszModuleName = L"CppDynamicLinkLibrary";

	// Check whether or not the module is loaded.
	fLoaded = IsModuleLoaded(pszModuleName);
    wprintf(L"Module \"%s\" is %sloaded\n", pszModuleName, fLoaded ? L"" : L"not ");

    //
	// Access the global data exported from the module.
    //

    __declspec(dllimport) int g_nVal1;
    wprintf(L"Global data: g_nVal1 = %d\n", g_nVal1);
    wprintf(L"Global data: g_nVal2 = %d\n", g_nVal2);

    // 
    // Call the functions exported from the module.
    // 

    PCWSTR pszString = L"HelloWorld";
    int nLength;

	// Call int /*__cdecl*/ GetStringLength1(PWSTR pszString);
    nLength = GetStringLength1(pszString);
    wprintf(L"Function: GetStringLength1(\"%s\") => %d\n", pszString, nLength);

    // Call int __stdcall GetStringLength2(PWSTR pszString);
    nLength = GetStringLength2(pszString);
    wprintf(L"Function: GetStringLength2(\"%s\") => %d\n", pszString, nLength);

    // 
    // Call the callback functions exported from the module.
    // 

    int max = Max(2, 3, &CompareInts);
    wprintf(L"Function: Max(2, 3) => %d\n", max);

    //
    // Use the class exported from the module.
    //

    CSimpleObject simpleObj;
    simpleObj.set_FloatProperty(1.2F);
    float fProp = simpleObj.get_FloatProperty();
    wprintf(L"Class: CSimpleObject::FloatProperty = %.2f\n", fProp);

	// You cannot unload the DLL by calling GetModuleHandle and FreeLibrary.

	// Check whether or not the module is loaded.
	fLoaded = IsModuleLoaded(pszModuleName);
    wprintf(L"Module \"%s\" is %sloaded\n", pszModuleName, fLoaded ? L"" : L"not ");

	return 0;
}