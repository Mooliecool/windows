========================================================================
    CONSOLE APPLICATION : CppCheckOSBitness Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates how to determine whether the operating system of 
the current machine or any remote machine is a 64-bit operating system.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the sample.

Step1. After you successfully build the sample project in Visual Studio 2008, 
you will get an application: CppCheckOSBitness.exe. 

Step2. Run the application in a command prompt (cmd.exe) on a 32-bit 
operating system (e.g. Windows 7 x86). The application prints the following 
content in the command prompt:

  Current OS is not 64-bit
  Current OS is not 64-bit

It dictates that the current operating system is not a 64-bit operating 
system.


/////////////////////////////////////////////////////////////////////////////
Implementation:

The sample introduces two solutions of detecting programmatically whether you 
are running on 64-bit operating system.

Solution 1. Use the IsWow64Process function

    //
    //   FUNCTION: Is64BitOS()
    //
    //   PURPOSE: The function determines whether the current operating system is 
    //   a 64-bit operating system.
    //
    //   RETURN VALUE: The function returns TRUE if the operating system is 
    //   64-bit; otherwise, it returns FALSE.
    //
    BOOL Is64BitOS()

If the running process is a 64-bit process, the operating system must be a 
64-bit operating system.

#if defined(_WIN64)
    return TRUE;   // 64-bit programs run only on Win64

To detect programmatically whether your 32-bit program is running on 64-bit 
operating system, you can use the IsWow64Process function. 

#elif defined(_WIN32)
    // 32-bit programs run on both 32-bit and 64-bit Windows
    BOOL f64bitOS = FALSE;
    return (SafeIsWow64Process(GetCurrentProcess(), &f64bitOS) && f64bitOS);

SafeIsWow64Process is a wrapper of the IsWow64Process API. It determines 
whether the specified process is running under WOW64. IsWow64Process does not 
exist prior to Windows XP with SP2 and Window Server 2003  with SP1. For 
compatibility with operating systems that do not support IsWow64Process, call 
GetProcAddress to detect whether IsWow64Process is implemented in 
Kernel32.dll. If GetProcAddress succeeds, it is safe to call IsWow64Process 
dynamically. Otherwise, WOW64 is not present.

Solution 2. Query the Win32_Processor WMI class's AddressWidth property

    //
    //   FUNCTION: Is64BitOS(LPCWSTR, LPCWSTR, LPCWSTR)
    //
    //   PURPOSE: The function determines whether the operating system of the 
    //   current machine of any remote machine is a 64-bit operating system 
    //   through Windows Management Instrumentation (WMI).
    //
    //   PARAMETERS:
    //   * pszMachineName - the full computer name or IP address of the target 
    //     machine. "." or NULL means the local machine. 
    //   * pszUserName - the user name you need for a connection. A null value 
    //     indicates the current security context. If the user name is from a 
    //     domain other than the current domain, the string should contain the 
    //     domain name and user name, separated by a backslash: string 'username' 
    //     = "DomainName\\UserName". 
    //   * pszPassword - the password for the specified user.
    //
    //   RETURN VALUE: The function returns true if the operating system is 
    //   64-bit; otherwise, it returns false.
    //
    //   EXCEPTION: If this function fails, it throws a C++ exception which 
    //   contains the HRESULT of the failure. For example, 
    //   WBEM_E_LOCAL_CREDENTIALS (0x80041064) is thrown when user credentials 
    //   (pszUserName, pszPassword) are specified for local connections.
    //   COR_E_UNAUTHORIZEDACCESS (0x80070005) is thrown because of incorrect 
    //   user name or password. 
    //   RPC_S_SERVER_UNAVAILABLE (0x800706BA) is usually caused by the firewall 
    //   on the target machine that blocks the WMI connection or some network 
    //   problem.
    //
    //   EXAMPLE CALL:
    //     try
    //     {
    //         f64bitOS = Is64BitOS(L".", NULL, NULL);
    //         wprintf(L"Current OS %s 64-bit.\n", 
    //             f64bitOS ? L"is" : L"is not");
    //     }
    //     catch (HRESULT hr)
    //     {
    //         wprintf(L"Is64BitOS failed with HRESULT 0x%08lx\n", hr);
    //     }
    //
    BOOL Is64BitOS(LPCWSTR pszMachineName, LPCWSTR pszUserName, 
                                LPCWSTR pszPassword);

It queries Win32_Processor.AddressWidth which dicates the current operating 
mode of the processor (on a 32-bit OS, it would be 32; on a 64-bit OS, it 
would be 64). In contrast, Win32_Processor.DataWidth indicates the capability
of the processor. On a 64-bit processor, it is "64". The OSArchitecture 
property of the Win32_OperatingSystem WMI class can also tell the bitness of 
OS. On a 32-bit OS, it would be "32-bit". However, the property is only 
available on Windows Vista and newer operating systems.

Note: The first solution of using IsWow64Process is the preferred way to 
detect OS bitness of the current system because it is much easier and faster. 
The WMI solution is useful when you want to find this information on a remote 
system. The remote computer must be configured for remote connections of WMI:
http://msdn.microsoft.com/en-us/library/aa389290(VS.85).aspx


/////////////////////////////////////////////////////////////////////////////
References:

How to detect programmatically whether you are running on 64-bit Windows
http://blogs.msdn.com/oldnewthing/archive/2005/02/01/364563.aspx

MSDN: Win32_Processor Class
http://msdn.microsoft.com/en-us/library/aa394373(VS.85).aspx

MSDN: Win32_OperatingSystem Class
http://msdn.microsoft.com/en-us/library/aa394239(VS.85).aspx


/////////////////////////////////////////////////////////////////////////////