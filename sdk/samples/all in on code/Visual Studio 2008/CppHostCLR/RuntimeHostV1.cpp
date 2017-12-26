/****************************** Module Header ******************************\
Module Name:  RuntimeHostV1.cpp
Project:      CppHostCLR
Copyright (c) Microsoft Corporation.

The code in this file demonstrates using .NET Framework 1.0 and 1.1 Hosting 
Interfaces (http://msdn.microsoft.com/en-us/library/ms164318.aspx) to host 
.NET runtime 2.0, load a .NET assebmly, and invoke a type in the assembly.

References:
 http://support.microsoft.com/kb/953836 
 http://msdn.microsoft.com/en-us/magazine/cc301479.aspx

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


#pragma region Includes and Imports
#include "stdafx.h"

// Include <mscoree.h> for CorBindToRuntimeEx and ICorRuntimeHost.
#include <mscoree.h>
// Link with mscoree.dll's import lib.
#pragma comment(lib, "mscoree.lib")

// Import mscorlib.tlb (Microsoft Common Language Runtime Class Library).
#import "mscorlib.tlb" raw_interfaces_only				\
	high_property_prefixes("_get","_put","_putref")		\
	rename("ReportEvent", "InteropServices_ReportEvent")
using namespace mscorlib;

#pragma endregion


HRESULT RuntimeHostV1Demo()
{
    HRESULT hr;

    // Specify the CLR version to be loaded. It must start with the character 
    // "v" followed by the first three parts of the version number (e.g. 
    // "v1.0.1529"). pszVersion = NULL loads the highest runtime on the 
    // machine - up to .NET runtime v2.0.
    PCWSTR pszVersion = L"v2.0.50727";

    // Specify whether to load the server or the workstation build of the 
    // CLR. Valid values are "svr" and "wks". pszFlavor = NULL loads the 
    // workstation build.
    PCWSTR pszFlavor = L"wks";

    // pszRuntimeModule specifies the .NET runtime module. If you are hosting 
    // .NET 1.0 or 1.1 runtime, the runtime module is "mscorwks" for work-
    // station build and "mscorsvr" for server build. If you are hosting .NET 
    // 2.0, the runtime module is always "mscorwks".
    PCWSTR pszRuntimeModule = L"mscorwks";
    BOOL fLoaded = FALSE;

    ICorRuntimeHost *pCorRuntimeHost = NULL;

    IUnknownPtr spAppDomainThunk = NULL;
    _AppDomainPtr spDefaultAppDomain = NULL;

    // The .NET assembly to load.
    bstr_t bstrAssemblyName(L"CSClassLibrary");
    _AssemblyPtr spAssembly = NULL;

    // The .NET class to instantiate.
    bstr_t bstrClassName(L"CSClassLibrary.CSSimpleObject");
    _TypePtr spType = NULL;
    variant_t vtObject;
    variant_t vtEmpty;

    // The static method in the .NET class to invoke.
    bstr_t bstrStaticMethodName(L"GetStringLength");
    SAFEARRAY *psaStaticMethodArgs = NULL;
    variant_t vtStringArg(L"HelloWorld");
    variant_t vtLengthRet;

    // The instance method in the .NET class to invoke.
    bstr_t bstrMethodName(L"ToString");
    SAFEARRAY *psaMethodArgs = NULL;
    variant_t vtStringRet;

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
        CLSID_CorRuntimeHost,           // CLSID of ICorRuntimeHost
        IID_PPV_ARGS(&pCorRuntimeHost)  // Return ICorRuntimeHost
        );
    if (FAILED(hr))
    {
        wprintf(L"CorBindToRuntimeEx failed w/hr 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Start the CLR.
    hr = pCorRuntimeHost->Start();
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
    // Load the NET assembly CSClassLibrary.dll. Call the static method 
    // GetStringLength of the class CSSimpleObject. Instantiate the class 
    // CSSimpleObject and call its instance method ToString.
    //

    // The following C++ code does the same thing as this C# code:
    // 
    //   Assembly assembly = AppDomain.CurrentDomain.Load("CSClassLibrary");
	//   object length = type.InvokeMember("GetStringLength", 
    //       BindingFlags.InvokeMethod | BindingFlags.Static | 
    //       BindingFlags.Public, null, null, new object[] { "HelloWorld" });
    //   object obj = assembly.CreateInstance("CSClassLibrary.CSSimpleObject");
    //   object str = type.InvokeMember("ToString", 
    //       BindingFlags.InvokeMethod | BindingFlags.Instance | 
    //       BindingFlags.Public, null, obj, new object[] { });

    // Get a pointer to the default AppDomain in the CLR.
    hr = pCorRuntimeHost->GetDefaultDomain(&spAppDomainThunk);
    if (FAILED(hr))
    {
        wprintf(L"ICorRuntimeHost::GetDefaultDomain failed w/hr 0x%08lx\n", hr);
        goto Cleanup;
    }

    hr = spAppDomainThunk->QueryInterface(IID_PPV_ARGS(&spDefaultAppDomain));
    if (FAILED(hr))
    {
        wprintf(L"Failed to get default AppDomain w/hr 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Load the .NET assembly.
    wprintf(L"\nLoad the assembly %s\n", (PCWSTR)bstrAssemblyName);
    hr = spDefaultAppDomain->Load_2(bstrAssemblyName, &spAssembly);
    if (FAILED(hr))
    {
        wprintf(L"Failed to load the assembly w/hr 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Get the Type of CSSimpleObject.
    hr = spAssembly->GetType_2(bstrClassName, &spType);
    if (FAILED(hr))
    {
        wprintf(L"Failed to get the Type interface w/hr 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Call the static method of the class:
    //   public static int GetStringLength(string str);

    // Create a safe array to contain the arguments of the method. The safe 
    // array must be created with vt = VT_VARIANT because .NET reflection 
    // expects an array of Object - VT_VARIANT. There is only one argument, 
    // so cElements = 1.
    psaStaticMethodArgs = SafeArrayCreateVector(VT_VARIANT, 0, 1);
    LONG index = 0;
    hr = SafeArrayPutElement(psaStaticMethodArgs, &index, &vtStringArg);
    if (FAILED(hr))
    {
        wprintf(L"SafeArrayPutElement failed w/hr 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Invoke the "GetStringLength" method from the Type interface.
    hr = spType->InvokeMember_3(bstrStaticMethodName, (BindingFlags)
        (BindingFlags_InvokeMethod | BindingFlags_Static | BindingFlags_Public), 
        NULL, vtEmpty, psaStaticMethodArgs, &vtLengthRet);
    if (FAILED(hr))
    {
        wprintf(L"Failed to invoke GetStringLength w/hr 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Print the call result of the static method.
    wprintf(L"Call %s.%s(\"%s\") => %d\n", (PCWSTR)bstrClassName, 
        (PCWSTR)bstrStaticMethodName, (PCWSTR)vtStringArg.bstrVal, 
        vtLengthRet.lVal);

    // Instantiate the class.
    hr = spAssembly->CreateInstance(bstrClassName, &vtObject);
    if (FAILED(hr))
    {
        wprintf(L"Assembly::CreateInstance failed w/hr 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Call the instance method of the class.
    //   public string ToString();

    // Create a safe array to contain the arguments of the method.
    psaMethodArgs = SafeArrayCreateVector(VT_VARIANT, 0, 0);

    // Invoke the "ToString" method from the Type interface.
    hr = spType->InvokeMember_3(bstrMethodName, (BindingFlags)
        (BindingFlags_InvokeMethod | BindingFlags_Instance | BindingFlags_Public),
        NULL, vtObject, psaMethodArgs, &vtStringRet);
    if (FAILED(hr))
    {
        wprintf(L"Failed to invoke ToString w/hr 0x%08lx\n", hr);
        goto Cleanup;
    }
    
    // Print the call result of the method.
    wprintf(L"Call %s.%s() => %s\n", (PCWSTR)bstrClassName, 
        (PCWSTR)bstrMethodName, (PCWSTR)vtStringRet.bstrVal);

Cleanup:

    if (pCorRuntimeHost)
    {
        // Please note that after a call to Stop, the CLR cannot be 
        // reinitialized into the same process. This step is usually not 
        // necessary. You can leave the .NET runtime loaded in your process.
        //wprintf(L"Stop the .NET runtime\n");
        //pCorRuntimeHost->Stop();

        pCorRuntimeHost->Release();
        pCorRuntimeHost = NULL;

        // Check whether or not the .NET runtime is loaded. You will see that 
        // stopping the .NET runtime does not unload MSCORWKS.dll.
        fLoaded = IsModuleLoaded(pszRuntimeModule);
        wprintf(L"Module \"%s\" is %sloaded\n", pszRuntimeModule, 
            fLoaded ? L"" : L"not ");
    }

    // Release other allocated resources and memory.
    if (psaStaticMethodArgs)
    {
        SafeArrayDestroy(psaStaticMethodArgs);
        psaStaticMethodArgs = NULL;
    }
    if (psaMethodArgs)
    {
        SafeArrayDestroy(psaMethodArgs);
        psaMethodArgs = NULL;
    }

    return hr;
}