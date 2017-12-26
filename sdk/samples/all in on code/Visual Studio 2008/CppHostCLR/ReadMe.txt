=============================================================================
            CONSOLE APPLICATION : CppHostCLR Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The Common Language Runtime (CLR) allows a level of integration between 
itself and a host. This C++ code sample demonstrates using the Hosting 
Interfaces of .NET Framework 1.0, 1.1 and 2.0 to host a specific version of 
CLR in the process, load a .NET assembly, and invoke the types in the 
assembly.


/////////////////////////////////////////////////////////////////////////////
Sample Relation:

CppHostCLR -> CSClassLibrary
CppHostCLR hosts CLR, instantiates a type in CSClassLibrary.dll and calls 
its methods.


/////////////////////////////////////////////////////////////////////////////
Implementation:

A. Demo .NET Framework 1.0 and 1.1 Hosting Interfaces in RuntimeHostV1.cpp
(http://msdn.microsoft.com/en-us/library/ms164318.aspx)

Step1. Include <mscoree.h> and link with mscoree.dll's import lib for 
CorBindToRuntimeEx and ICorRuntimeHost, and import mscorlib.tlb (Microsoft 
Common Language Runtime Class Library).

    // Include <mscoree.h> for CorBindToRuntimeEx and ICorRuntimeHost.
    #include <mscoree.h>
    // Link with mscoree.dll's import lib.
    #pragma comment(lib, "mscoree.lib") 

    // Import mscorlib.tlb (Microsoft Common Language Runtime Class Library)
    #import "mscorlib.tlb" raw_interfaces_only				\
	    high_property_prefixes("_get","_put","_putref")		\
	    rename("ReportEvent", "InteropServices_ReportEvent")
    using namespace mscorlib;

Step2. Load and start the .NET runtime.
Call CorBindToRuntimeEx to obtain an interface pointer to an ICorRuntimeHost 
that can be used to set additional options for configuring an instance of the 
CLR before it is started. Then call ICorRuntimeHost::Start to start the CLR.

    PCWSTR pszFlavor = L"wks";
    PCWSTR pszRuntimeModule = L"mscorwks";
    ICorRuntimeHost *pCorRuntimeHost = NULL;
    hr = CorBindToRuntimeEx(
        pszVersion,                     // Runtime version
        pszFlavor,                      // Flavor of the runtime to request
        0,                              // Runtime startup flags
        CLSID_CorRuntimeHost,           // CLSID of ICorRuntimeHost
        IID_PPV_ARGS(&pCorRuntimeHost)  // Return ICorRuntimeHost
        );
    if (FAILED(hr)) { /* Error handling */ }
    hr = pCorRuntimeHost->Start();
    if (FAILED(hr)) { /* Error handling */ }

Step3. Load the NET assembly CSClassLibrary.dll. Call the static method 
GetStringLength of the class CSSimpleObject. Instantiate the class 
CSSimpleObject and call its instance method ToString. Different from .NET 
Framework 2.0 Hosting Interfaces, .NET Framework 1.0 and 1.1 Hosting 
Interfaces does not limit the signature of the invoked method.

The process is similar to the following .NET reflection code. You can find 
the C++ code sample in RuntimeHostV1.cpp.

    Assembly assembly = AppDomain.CurrentDomain.Load("CSClassLibrary");
	object length = type.InvokeMember("GetStringLength", 
        BindingFlags.InvokeMethod | BindingFlags.Static | 
        BindingFlags.Public, null, null, new object[] { "HelloWorld" });
    object obj = assembly.CreateInstance("CSClassLibrary.CSSimpleObject");
    object str = type.InvokeMember("ToString", 
        BindingFlags.InvokeMethod | BindingFlags.Instance | 
        BindingFlags.Public, null, obj, new object[] { });

Step4. Stops the execution of code in the runtime for the current process. 
Please note that after a call to Stop, the CLR cannot be reinitialized into
the same process. This step is usually not necessary. You can leave the 
.NET runtime loaded in your process.

    pCorRuntimeHost->Stop();

B. Demo .NET Framework 2.0 Hosting Interfaces in RuntimeHostV2.cpp
(http://msdn.microsoft.com/en-us/library/ms164336.aspx)

Step1. Include <mscoree.h> and link with mscoree.dll's import lib for 
CorBindToRuntimeEx and ICLRRuntimeHost, and import mscorlib.tlb (Microsoft 
Common Language Runtime Class Library).

    // Include <mscoree.h> for CorBindToRuntimeEx and ICLRRuntimeHost.
    #include <mscoree.h>
    // Link with mscoree.dll's import lib.
    #pragma comment(lib, "mscoree.lib") 

    // Import mscorlib.tlb (Microsoft Common Language Runtime Class Library).
    #import "mscorlib.tlb" raw_interfaces_only				\
	    high_property_prefixes("_get","_put","_putref")		\
	    rename("ReportEvent", "InteropServices_ReportEvent")
    using namespace mscorlib;

Step2. Load and start the .NET runtime.
Call CorBindToRuntimeEx to obtain an interface pointer to an ICLRRuntimeHost 
Then call ICLRRuntimeHost::Start to start the CLR.

    PCWSTR pszFlavor = L"wks";
    PCWSTR pszRuntimeModule = L"mscorwks";
    ICLRRuntimeHost *pClrRuntimeHost = NULL;
    hr = CorBindToRuntimeEx(
        pszVersion,                     // Runtime version
        pszFlavor,                      // Flavor of the runtime to request
        0,                              // Runtime startup flags
        CLSID_CLRRuntimeHost,           // CLSID of ICorRuntimeHost
        IID_PPV_ARGS(&pClrRuntimeHost)  // Return ICLRRuntimeHost
        );
    if (FAILED(hr)) { /* Error handling */ }
    hr = pClrRuntimeHost->Start();
    if (FAILED(hr)) { /* Error handling */ }

Step3. Load the NET assembly CSClassLibrary.dll, and call the static method 
GetStringLength of the type CSSimpleObject in the assembly.

.NET Framework 2.0 Hosting Interfaces allows you to load a .NET assembly, and 
execute its code in the default application domain by calling 
ICLRRuntimeHost::ExecuteInDefaultAppDomain. However, there is a restriction 
to the invoked .NET method: The invoked method of ExecuteInDefaultAppDomain 
must have the following signature where pwzMethodName represents the name of 
the invoked method, and pwzArgument represents the string value passed as a 
parameter to that method.

    static int pwzMethodName (String pwzArgument)

If the HRESULT return value of ExecuteInDefaultAppDomain is set to S_OK, 
pReturnValue is set to the integer value returned by the invoked method. 
Otherwise, pReturnValue is not set.

    hr = pClrRuntimeHost->ExecuteInDefaultAppDomain(pszAssemblyPath, 
        pszClassName, pszStaticMethodName, pszStringArg, &dwLengthRet);

Step4. Stop the execution of code by the common language runtime. Please note 
that after a call to Stop, the CLR cannot be reinitialized into the same 
process. This step is usually not necessary. You can leave the .NET runtime 
loaded in your process.

    pClrRuntimeHost->Stop();


/////////////////////////////////////////////////////////////////////////////
References:

.NET Framework 1.0 and 1.1 Hosting Interfaces
http://msdn.microsoft.com/en-us/library/ms164318.aspx

.NET Framework 2.0 Hosting Interfaces
http://msdn.microsoft.com/en-us/library/ms164336.aspx

Hosting the Common Language Runtime
http://msdn.microsoft.com/en-us/library/9x0wh2z3.aspx

Calling A .NET Managed Method from Native Code
http://support.microsoft.com/kb/953836

CLR Hosting APIs
http://msdn.microsoft.com/en-us/magazine/cc163567.aspx


/////////////////////////////////////////////////////////////////////////////
