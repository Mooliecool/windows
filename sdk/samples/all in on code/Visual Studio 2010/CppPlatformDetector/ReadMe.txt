=============================================================================
        CONSOLE APPLICATION : CppPlatformDetector Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The CppPlatformDetector code sample demonstrates the following tasks related 
to platform detection:

1. Detect the name of the current operating system. 
   (e.g. "Microsoft Windows 7 Enterprise")
2. Detect the version of the current operating system.
   (e.g. "Microsoft Windows NT 6.1.7600.0")
3. Determine whether the current operating system is a 64-bit operating 
   system. 
4. Determine whether the current process is a 64-bit process. 
5. Determine whether an arbitrary process running on the system is 64-bit. 


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the sample.

Step1. In Visual Studio 2010, configure the sample project to target the x64
platform. After you successfully build the sample project, you will get an 
application: CppPlatformDetector.exe. 

Step2. Run the application in a command prompt (cmd.exe) on a 64-bit 
operating system (e.g. Windows 7 x64 Ultimate). The application prints the 
following information in the command prompt:

  Current OS: Microsoft Windows 7 Ultimate
  Version: Microsoft Windows NT 6.1.7600.0 Workstation
  Current OS is 64-bit
  Current process is 64-bit

It dictates that the current operating system is Microsoft Windows 7 Ultimate. 
Its version is 6.1.7600.0. The OS is a workstation instead of a server or 
domain controller. The system is 64-bit. The current process is a 64-bit 
process. 

Step3. In Task Manager, find a 32-bit process running on the system, and get 
its process ID (e.g. 6100). Run CppPlatformDetector.exe with the process ID 
as the first argument. For example, 

    CppPlatformDetector.exe 6100

The application will output:

  ...
  Process 6100 is not 64-bit

It indicates that the specified process is not a 64-bit process.


/////////////////////////////////////////////////////////////////////////////
Implementation:

A. Get the name of the current operating system. 
   (e.g. "Microsoft Windows 7 Enterprise")

The name of the operating system (e.g. "Microsoft Windows 7 Ultimate") can be 
retrieved from the Caption property of the Win32_OperatingSystem WMI class 
(http://msdn.microsoft.com/en-us/library/aa394239.aspx). You can find the C++ 
code that queries the value of Win32_OperatingSystem.Caption in the GetOSName 
function of PlatformDetector.h/cpp.

Alternatively, you can build the string of the operating system name by using 
the GetVersionEx, GetSystemMetrics, GetProductInfo, and GetNativeSystemInfo 
functions. The MSDN article "Getting the System Version" gives an example:
http://msdn.microsoft.com/en-us/library/ms724429.aspx. However, the solution 
is not flexible for new releases of operating systems. 

--------------------

B. Get the version of the current operating system.
   (e.g. "Microsoft Windows NT 6.1.7600.0")

The OSVERSIONINFOEX structure outputted by GetVersionEx tells the major, 
minor version numbers (dwMajorVersion, dwMinorVersion), the build number 
(dwBuildNumber), and the major, minor version number of the latest Service 
Pack (wServicePackMajor, wServicePackMinor). You can use these numbers to 
quickly determine what the operating system is, whether a certain Service Pack 
is installed, etc. OSVERSIONINFOEX.wProductType can tell whether the 
operating system is a workstation or a server or a domain controller. 

The GetOSVersionString function in PlatformDetector.h/cpp retrieves the 
concatenated string representation of the platform identifier, version, and 
service pack that are currently installed on the operating system. For 
example, "Microsoft Windows NT 6.1.7600.0 Workstation".

--------------------

C. Determine the whether the current OS is a 64-bit operating system.  

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

--------------------

D. Determine whether the current process or an arbitrary process running on 
the system is a 64-bit process. 

If you are determining whether the currently running process is a 64-bit 
process, you can use the VC++ preprocessor symbols which get set during the 
build to simply return true/false.

    BOOL Is64BitProcess(void)
    {
    #if defined(_WIN64)
        return TRUE;   // 64-bit program
    #else
        return FALSE;
    #endif
    }

If you are detecting whether an arbitrary application running on the system 
is a 64-bit process, you need to determine the OS bitness and if it is 64-bit, 
call IsWow64Process() with the target process handle.

    BOOL Is64BitProcess(HANDLE hProcess)
    {
	    BOOL f64bitProc = FALSE;

        if (Is64BitOS())
	    {
		    // On 64bit OS, if a process is not running under Wow64 mode, the 
            // process must be a 64bit process.
            f64bitProc = !(SafeIsWow64Process(hProcess, &f64bitProc) && f64bitProc);
	    }

	    return f64bitProc;
    }


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Getting the System Version
http://msdn.microsoft.com/en-us/library/ms724429.aspx

How to detect programmatically whether you are running on 64-bit Windows
http://blogs.msdn.com/oldnewthing/archive/2005/02/01/364563.aspx


/////////////////////////////////////////////////////////////////////////////