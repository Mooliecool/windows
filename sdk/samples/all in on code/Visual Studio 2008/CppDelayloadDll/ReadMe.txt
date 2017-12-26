=============================================================================
          CONSOLE APPLICATION : CppDelayloadDll Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The support of delayed loading of DLLs in Visual C++ linker relieves us of 
the need to use the API LoadLibrary and GetProcAddress to implement DLL 
delayed loading. DLL is implicitly linked but not actually loaded until the 
code attempts to reference a symbol contained within the DLL. An application 
can delay load a DLL using the /DELAYLOAD (Delay Load Import) linker option 
with a helper function (default implementation provided by Visual C++, see 
http://msdn.microsoft.com/en-us/library/09t6x5ds.aspx). The helper function 
will load the DLL at run time by calling LoadLibrary and GetProcAddress for 
us. 

We should consider delay loading a DLL if the program may not call a function
in the DLL or a function in the DLL may not get called until late in the 
program's execution. Delay loading a DLL saves the initialization time when 
the executable files starts up.

This code sample demonstrates delay loading CppDynamicLinkLibrary.dll, 
importing and using the data, functions and classes exported from the DLL, 
and explicitly unloading the module.

You can also learn how to hook notifications and failures in the delay-load 
process, and customize the default delay-load behavior. In this code sample, 
if the target DLL CppDynamicLinkLibrary.dll fails to be loaded, you will be 
asked to input the full path of the DLL, and the application will attempt to 
load the DLL again. 


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the DLL delayed loading 
sample.

Step1. After you successfully build the sample project CppDelayloadDll and 
its dependent project CppDynamicLinkLibrary in Visual Studio 2008, you will 
get an application CppDelayloadDll.exe and a DLL CppDynamicLinkLibrary.dll.

Step2. Run CppDelayloadDll.exe in a command prompt. The application should 
print the following messages in the console.

    Module "CppDynamicLinkLibrary" is not loaded
    Function: GetStringLength1("HelloWorld") => 10
    Function: GetStringLength2("HelloWorld") => 10
    Function: Max(2, 3) => 3
    Class: CSimpleObject::FloatProperty = 1.20
    Module "CppDynamicLinkLibrary" is loaded
    Unload the delay-loaded DLL
    Module "CppDynamicLinkLibrary" is not loaded

The messages indicate that, at first, CppDynamicLinkLibrary.dll was not 
loaded. The DLL is loaded after the application calls the functions or  
classes exported by the DLL. Last, it is allowed to unload the delay-loaded 
DLL. 

Step3. To demo the notification hooks and failure hooks, move the DLL file 
CppDynamicLinkLibrary.dll to a folder that is not in the default DLL search 
path (e.g. D:\Test\). Run CppDelayloadDll.exe again. The application displays 
that the DLL fails to be loaded. You are be asked to input the path of the 
DLL file.

	Module "CppDynamicLinkLibrary" is not loaded
	Failed to load the DLL CppDynamicLinkLibrary.dll w/err 0x0000007e.
	Please input the path of the DLL file:

By entering the new path of the DLL (e.g. D:\Test\CppDynamicLinkLibrary.dll), 
the application will attempt to load the DLL again. This time, the module 
is successfully loaded. The sample application proceeds to invoke the 
functions and classes exported from the DLL.

	Please input the path of the DLL file:
	D:\Test\CppDynamicLinkLibrary.dll
	Function: GetStringLength1("HelloWorld") => 10
	Function: GetStringLength2("HelloWorld") => 10
	Function: Max(2, 3) => 3
	Class: CSimpleObject::FloatProperty = 1.20
	Module "CppDynamicLinkLibrary" is loaded
	Unload the delay-loaded DLL
	Module "CppDynamicLinkLibrary" is not loaded


/////////////////////////////////////////////////////////////////////////////
Sample Relation:
(The relationship between the current sample and the rest samples in 
Microsoft All-In-One Code Framework http://1code.codeplex.com)

CppDelayloadDll -> CppDynamicLinkLibrary
CppDelayloadDll delay-loads CppDynamicLinkLibrary.dll and invokes the 
functions and classes exported by the DLL.


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step1. Link the LIB file of the DLL by entering the LIB file name in 
Project Properties / Linker / Input / Additional Dependencies. We can 
configure the search path of the LIB file in Project Properties / Linker / 
General / Additional Library Directories.

Step2. #include the header file that imports the symbols of the DLL.

	#include "CppDynamicLinkLibrary.h"

We can configure the search path of the header file in Project Properties / 
C/C++ / General / Additional Include Directories.

Step3. Specify the DLL for delay loading by entering the DLL file name, 
CppDynamicLinkLibrary.dll, in Project Properties / Linker / Input / Delay 
Loaded DLLs.

Step4. Specify to allow explicitly unloading of the delayed load DLLs by 
selecting "Support Unload (/DELAY:UNLOAD) in Project Properties / Linker / 
Advanced / Delay Loaded DLL.

Step5. Use the imported symbols. For example:

    PCWSTR pszString = L"HelloWorld";
    int nLength;
	nLength = GetStringLength1(pszString);

Note: Delay-load does not allow accessing/dllimport-ing data symbols.

Step6. Unload the delay-loaded DLL after the use.

	PCSTR pszDll = "CppDynamicLinkLibrary.dll";
	if (!__FUnloadDelayLoadedDLL2(pszDll))
	{
	    wprintf(L"The DLL failed to be unloaded\n");
	}

Step7. (Optional) Setup notification hooks and failure hooks if you want to 
customize the default behaviors in the delay-load helper function.

The notification hook is enabled by supplying a new definition of the pointer 
__pfnDliNotifyHook2 that is initialized to point to your own function that 
receives the notifications, or by setting the pointer __pfnDliNotifyHook2 to 
your hook function before any calls to the DLL that the program is delay 
loading.

	PfnDliHook __pfnDliNotifyHook2 = DelayLoadHook;

The failure hook is enabled in the same manner as the notification hook. 

	PfnDliHook __pfnDliFailureHook2 = DelayLoadHook;

The hook function is defined as 

	FARPROC WINAPI DelayLoadHook(unsigned dliNotify, PDelayLoadInfo pdli)
	{
	}

The delay-load helper function can call back to a notification hook in your 
program after each of the following actions: 

	Just before LoadLibrary is called in the helper function

	Just before GetProcAddress is called in the helper function

	If the call to LoadLibrary in the helper function failed

	If the call to GetProcAddress in the helper function failed

	After the helper function is done processing

In this code sample, we hook the dliFailGetProc notification. If the target 
DLL fails to be loaded, you will be asked to input the full path of the DLL, 
and the application will attempt to load the DLL again. 

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


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Linker Support for Delay-Loaded DLLs
http://msdn.microsoft.com/en-us/library/151kt790.aspx

MSDN: Unloading a Delay-Loaded DLL
http://msdn.microsoft.com/en-us/library/94zxdhbc.aspx

MSDN: Understanding the Helper Function
http://msdn.microsoft.com/en-us/library/09t6x5ds.aspx

MSDN: Notification Hooks
http://msdn.microsoft.com/en-us/library/z9h1h6ty.aspx

MSDN: Failure Hooks
http://msdn.microsoft.com/en-us/library/sfcfb0a3.aspx


/////////////////////////////////////////////////////////////////////////////
