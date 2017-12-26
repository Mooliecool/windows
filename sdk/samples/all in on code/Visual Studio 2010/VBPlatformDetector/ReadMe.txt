=============================================================================
       CONSOLE APPLICATION : VBPlatformDetector Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The VBPlatformDetector code sample demonstrates the following tasks related 
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

Step1. After you successfully build the sample project in Visual Studio 2010 
targeting the "Any CPU" platform, you will get an application: 
VBPlatformDetector.exe. 

Step2. Run the application in a command prompt (cmd.exe) on a 64-bit 
operating system (e.g. Windows 7 x64 Ultimate). The application prints the 
following information in the command prompt:

  Current OS: Microsoft Windows 7 Ultimate
  Version: Microsoft Windows NT 6.1.7600.0
  Current OS is 64-bit
  Current process is 64-bit

It dictates that the current operating system is Microsoft Windows 7 Ultimate. 
Its version is 6.1.7600.0. The OS is a workstation instead of a server or 
domain controller. The system is 64-bit. The current process is a 64-bit 
process. 

Step3. In Task Manager, find a 32-bit process running on the system, and get 
its process ID (e.g. 6100). Run VBPlatformDetector.exe with the process ID 
as the first argument. For example, 

    VBPlatformDetector.exe 6100

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
(http://msdn.microsoft.com/en-us/library/aa394239.aspx). You can find the VC# 
code that queries the value of Win32_OperatingSystem.Caption in the GetOSName 
method.

Alternatively, you can build the string of the operating system name by using 
the GetVersionEx, GetSystemMetrics, GetProductInfo, and GetNativeSystemInfo 
functions. The MSDN article "Getting the System Version" gives an C++ example:
http://msdn.microsoft.com/en-us/library/ms724429.aspx. However, the solution 
is not flexible for new releases of operating systems. 

--------------------

B. Get the version of the current operating system.
   (e.g. "Microsoft Windows NT 6.1.7600.0")

The System.Environment.OSVersion property returns an OperatingSystem object 
that contains the current platform identifier and version numbers.
http://msdn.microsoft.com/en-us/library/system.environment.osversion.aspx
http://msdn.microsoft.com/en-us/library/system.operatingsystem.aspx
You can use these numbers to quickly determine what the operating system is, 
whether a certain Service Pack is installed, etc. 

In the code sample, Environment.OSVersion.VersionString gets the concatenated 
string representation of the platform identifier, version, and service pack 
that are currently installed on the operating system. For example, 
"Microsoft Windows NT 6.1.7600.0".

--------------------

C. Determine the whether the current OS is a 64-bit operating system.  

The Environment.Is64BitOperatingSystem property new in .NET Framework 4 
determines whether the current operating system is a 64-bit operating system.
http://msdn.microsoft.com/en-us/library/system.environment.is64bitoperatingsystem.aspx

The implementation of Environment.Is64BitOperatingSystem is based on this 
logic:

  If the running process is a 64-bit process, the operating system must be a 
  64-bit operating system. 

  If the running process is a 32-bit process, the process may be running in a 
  32-bit operating system, or under WOW64 of a 64-bit operating system. To 
  detect whether the 32-bit program is running in a 64-bit operating system, 
  you can use the IsWow64Process function. 

    Dim flag As Boolean
    Return ((Win32Native.DoesWin32MethodExist("kernel32.dll", "IsWow64Process") _
        AndAlso Win32Native.IsWow64Process(Win32Native.GetCurrentProcess, flag)) _
        AndAlso flag)

--------------------

D. Determine whether the current process or an arbitrary process running on 
the system is a 64-bit process. 

If you are determining whether the currently running process is a 64-bit 
process, you can use the Environment.Is64BitProcess property new in .NET 
Framework 4. 
http://msdn.microsoft.com/en-us/library/system.environment.is64bitprocess.aspx

If you are detecting whether an arbitrary application running on the system 
is a 64-bit process, you need to determine the OS bitness and if it is 64-bit, 
call IsWow64Process() with the target process handle.

    Function Is64BitProcess(ByVal hProcess As IntPtr) As Boolean
        Dim flag As Boolean = False
        If Environment.Is64BitOperatingSystem Then
            ' On 64-bit OS, if a process is not running under Wow64 mode, 
            ' the process must be a 64-bit process.
            flag = Not (NativeMethods.IsWow64Process(hProcess, flag) AndAlso flag)
        End If
        Return flag
    End Function


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Environment.Is64BitOperatingSystem Property 
http://msdn.microsoft.com/en-us/library/system.environment.is64bitoperatingsystem.aspx

MSDN: Environment.Is64BitProcess Property 
http://msdn.microsoft.com/en-us/library/system.environment.is64bitprocess.aspx

MSDN: Getting the System Version
http://msdn.microsoft.com/en-us/library/ms724429.aspx

How to detect programmatically whether you are running on 64-bit Windows
http://blogs.msdn.com/oldnewthing/archive/2005/02/01/364563.aspx


/////////////////////////////////////////////////////////////////////////////