=============================================================================
          APPLICATION : VBCheckOSBitness Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary: 

The code sample demonstrates how to determine whether the operating system of 
the current machine or any remote machine is a 64-bit operating system.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the sample.

Step1. After you successfully build the sample project in Visual Studio 2008, 
you will get an application: VBCheckOSBitness.exe. 

Step2. Run the application in a command prompt (cmd.exe) on a 32-bit 
operating system (e.g. Windows 7 x86). The application prints the following 
content in the command prompt:

  Current OS is not 64-bit
  Current OS is not 64-bit

It dictates that the current operating system is not a 64-bit operating 
system.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

The sample introduces two solutions of detecting programmatically whether you 
are running on 64-bit operating system.

Solution 1. Use the IsWow64Process function

    ''' <summary>
    ''' The function determines whether the current operating system is a 
    ''' 64-bit operating system.
    ''' </summary>
    ''' <returns>
    ''' The function returns true if the operating system is 64-bit; 
    ''' otherwise, it returns false.
    ''' </returns>
    Public Function Is64BitOS() As Boolean

To detect programmatically whether your 32-bit program is running on 64-bit 
operating system, you can use the IsWow64Process function. 

    Dim flag As Boolean
    Return ((DoesWin32MethodExist("kernel32.dll", "IsWow64Process") _
            AndAlso IsWow64Process(GetCurrentProcess, flag)) AndAlso flag)

If the running process is a 64-bit process, the operating system must be a 
64-bit operating system.

    If (IntPtr.Size = 8) Then  ' 64-bit programs run only on Win64
        Return True
    End If

Solution 2. Query the Win32_Processor WMI class's AddressWidth property

    ''' <summary>
    ''' The function determines whether the operating system of the current 
    ''' machine of any remote machine is a 64-bit operating system through 
    ''' Windows Management Instrumentation (WMI).
    ''' </summary>
    ''' <param name="machineName">
    ''' The full computer name or IP address of the target machine. "." or 
    ''' null means the local machine. 
    ''' </param>
    ''' <param name="userName">
    ''' The user name you need for a connection. A Nothing value indicates 
    ''' the current security context. If the user name is from a domain other 
    ''' than the current domain, the string should contain the domain name 
    ''' and user name, separated by a backslash: string 'username' = 
    ''' "DomainName\\UserName". 
    ''' </param>
    ''' <param name="password">The password for the specified user.</param>
    ''' <returns>
    ''' The function returns true if the operating system is 64-bit; 
    ''' otherwise, it returns false.
    ''' </returns>
    ''' <exception cref="System.Management.ManagementException">
    ''' The ManagementException exception is generally thrown with the error
    ''' message "User credentials cannot be used for local connections". To 
    ''' solve it, do not specify userName and password when machineName 
    ''' refers to the local computer.
    ''' </exception>
    ''' <exception cref="System.UnauthorizedAccessException">
    ''' This exception is usually caused by incorrect user name or password.
    ''' </exception>
    ''' <exception cref="System.Runtime.InteropServices.COMException">
    ''' A common error accompanied with the COMException is "The RPC server 
    ''' is unavailable. (Exception from HRESULT: 0x800706BA)". This is 
    ''' usually caused by the firewall on the target machine that blocks the 
    ''' WMI connection or some network problem.
    ''' </exception>
    Public Function Is64BitOS(ByVal machineName As String, _
                              ByVal userName As String, _
                              ByVal password As String) _
                              As Boolean

It queries Win32_Processor.AddressWidth which dicates the current operating 
mode of the processor (on a 32-bit OS, it would be "32"; on a 64-bit OS, it 
would be "64"). In contrast, Win32_Processor.DataWidth indicates the 
capability of the processor. On a 64-bit processor, it is "64". The 
OSArchitecture property of the Win32_OperatingSystem WMI class can also tell 
the bitness of OS. On a 32-bit OS, it would be "32-bit". However, the 
property is only available on Windows Vista and newer operating systems. 

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