/****************************** Module Header ******************************\
Module Name:  CppCallNETAssemblyWrapper.cpp
Project:      CppCallNETAssemblyWrapper
Copyright (c) Microsoft Corporation.

The code sample demonstrates calling the .NET classes defined in a managed 
assembly from a native C++ application through C++/CLI wrapper classes.

CppCallNETAssemblyWrapper (this native C++ application)
-->
CppCLINETAssemblyWrapper (the C++/CLI wrapper)
-->
CSClassLibrary (a .NET assembly)

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
#include "CppCLINETAssemblyWrapper.h"
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


int wmain(int argc, wchar_t *argv[])
{
    BOOL fLoaded = FALSE;

    // The name of the .NET assembly to be 
    PCWSTR pszModuleName = L"CSClassLibrary";

    // Check whether or not the module is loaded.
    fLoaded = IsModuleLoaded(pszModuleName);
    wprintf(L"Module \"%s\" is %sloaded\n", pszModuleName, fLoaded ? L"" : L"not ");

    //
    // Invoke the public class defined in the .NET assembly.
    //

    // Use the .NET class CSSimpleObject defined in the C# class library 
    // through the C++/CLI wrapper class CSSimpleObjectWrapper.
    CSSimpleObjectWrapper obj;

    obj.set_FloatProperty(1.2F);
    float fProp = obj.get_FloatProperty();
    wprintf(L"Class: CSSimpleObject::FloatProperty = %.2f\n", fProp);

    wchar_t szStr[100];
    HRESULT hr = obj.ToString(szStr, ARRAYSIZE(szStr));
    if (SUCCEEDED(hr))
    {
        wprintf(L"Class: CSSimpleObject::ToString => \"%s\"\n", szStr);
    }
    else
    {
        wprintf(L"CSSimpleObject::ToString failed w/hr 0x%08lx\n", hr);
    }

    // You cannot unload the .NET assembly. 
    // Check whether or not the module is loaded.
    fLoaded = IsModuleLoaded(pszModuleName);
    wprintf(L"Module \"%s\" is %sloaded\n", pszModuleName, fLoaded ? L"" : L"not ");

    return 0;
}