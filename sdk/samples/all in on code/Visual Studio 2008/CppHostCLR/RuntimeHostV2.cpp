/****************************** Module Header ******************************\
Module Name:  RuntimeHostV2.cpp
Project:      CppHostCLR
Copyright (c) Microsoft Corporation.

The code in this file demonstrates using .NET Framework 2.0 Hosting 
Interfaces (http://msdn.microsoft.com/en-us/library/ms164336.aspx) to host 
.NET runtime 2.0 , load a .NET assebmly, and call the method of a type in 
the assembly.

References:
 http://msdn.microsoft.com/en-us/magazine/cc163567.aspx
 http://blogs.msdn.com/calvin_hsia/archive/2006/08/07/691467.aspx

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes and Imports
#include "stdafx.h"

// Include <mscoree.h> for CorBindToRuntimeEx and ICLRRuntimeHost.
#include <mscoree.h>
// Link with mscoree.dll's import lib.
#pragma comment(lib, "mscoree.lib")

// Import mscorlib.tlb (Microsoft Common Language Runtime Class Library).
#import "mscorlib.tlb" raw_interfaces_only				\
	high_property_prefixes("_get","_put","_putref")		\
	rename("ReportEvent", "InteropServices_ReportEvent")
using namespace mscorlib;

#pragma endregion


HRESULT RuntimeHostV2Demo()
{
    HRESULT hr;

    // Specify the CLR version to be loaded. It must start with the character 
    // "v" followed by the first three parts of the version number (e.g. 
    // "v2.0.50727"). pszVersion = NULL loads the highest runtime on the 
    // machine - up to .NET runtime v2.0. Because .NET Framework 2.0 Hosting 
    // Interfaces does not support hosting .NET 1.x runtimes, you must not 
    // specify pszVersion as "v1.0.3705" or "v1.1.4322" here.
    PCWSTR pszVersion = L"v2.0.50727";

    // Specify whether to load the server or the workstation build of the 
    // CLR. Valid values are "svr" and "wks". pszFlavor = NULL loads the 
    // workstation build.
    PCWSTR pszFlavor = L"wks";

    // For .NET runtime 2.0, the runtime module is always "mscorwks".
    PCWSTR pszRuntimeModule = L"mscorwks";
    BOOL fLoaded = FALSE;

    ICLRRuntimeHost *pClrRuntimeHost = NULL;

    // The .NET assembly to load.
    PCWSTR pszAssemblyPath = L"CSClassLibrary.dll";

    // The .NET class to instantiate.
    PCWSTR pszClassName = L"CSClassLibrary.CSSimpleObject";

    // The static method in the .NET class to invoke.
    PCWSTR pszStaticMethodName = L"GetStringLength";
    PCWSTR pszStringArg = L"HelloWorld";
    DWORD dwLengthRet;

    // Check whether or not the .NET runtime is loaded.
	fLoaded = IsModuleLoaded(pszRuntimeModule);
    wprintf(L"Module \"%s\" is %sloaded\n", pszRuntimeModule, 
        fLoaded ? L"" : L"not ");

    //
    // Load and start the .NET runtime.
    //

    wprintf(L"Load and start %s .NET runtime\n", 
        pszVersion == NULL ? L"the latest" : pszVersion);

    hr = CorBindToRuntimeEx(
        pszVersion,                     // Runtime version
        pszFlavor,                      // Flavor of the runtime to request
        0,                              // Runtime startup flags
        CLSID_CLRRuntimeHost,           // CLSID of ICorRuntimeHost
        IID_PPV_ARGS(&pClrRuntimeHost)  // Return ICLRRuntimeHost
        );
    if (FAILED(hr))
    {
        wprintf(L"CorBindToRuntimeEx failed w/hr 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Start the CLR. 
    hr = pClrRuntimeHost->Start();
    if (FAILED(hr))
    {
        wprintf(L"CLR failed to start w/hr 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Check whether or not the .NET runtime is loaded.
	fLoaded = IsModuleLoaded(pszRuntimeModule);
    wprintf(L"Module \"%s\" is %sloaded\n", pszRuntimeModule, 
        fLoaded ? L"" : L"not ");

    //
    // Load the NET assembly CSClassLibrary.dll, and call the static method 
    // GetStringLength of the type CSSimpleObject in the assembly.
    //

    // The invoked method of ExecuteInDefaultAppDomain must have the 
    // following signature: static int pwzMethodName (String pwzArgument)
    // where pwzMethodName represents the name of the invoked method, and 
    // pwzArgument represents the string value passed as a parameter to that 
    // method. If the HRESULT return value of ExecuteInDefaultAppDomain is 
    // set to S_OK, pReturnValue is set to the integer value returned by the 
    // invoked method. Otherwise, pReturnValue is not set.
    hr = pClrRuntimeHost->ExecuteInDefaultAppDomain(pszAssemblyPath, 
        pszClassName, pszStaticMethodName, pszStringArg, &dwLengthRet);
    if (FAILED(hr))
    {
        wprintf(L"Failed to call GetStringLength w/hr 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Print the call result of the static method.
    wprintf(L"Call %s.%s(\"%s\") => %d\n", pszClassName, pszStaticMethodName, 
        pszStringArg, dwLengthRet);

Cleanup:

    if (pClrRuntimeHost)
    {
        // Please note that after a call to Stop, the CLR cannot be 
        // reinitialized into the same process. This step is usually not 
        // necessary. You can leave the .NET runtime loaded in your process.
        //wprintf(L"Stop the .NET runtime\n");
        //pClrRuntimeHost->Stop();

        pClrRuntimeHost->Release();
        pClrRuntimeHost = NULL;

        // Check whether or not the .NET runtime is loaded. You will see that 
        // stopping the .NET runtime does not unload MSCORWKS.dll.
        fLoaded = IsModuleLoaded(pszRuntimeModule);
        wprintf(L"Module \"%s\" is %sloaded\n", pszRuntimeModule, 
            fLoaded ? L"" : L"not ");
    }

    return hr;
}