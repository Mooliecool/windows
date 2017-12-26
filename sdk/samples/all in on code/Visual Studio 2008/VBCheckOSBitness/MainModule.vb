'***************************** Module Header *******************************\
' Module Name:  MainModule.vb
' Project:      VBCheckOSBitness
' Copyright (c) Microsoft Corporation.
' 
' The code sample demonstrates how to determine whether the operating system 
' of the current machine or any remote machine is a 64-bit operating system.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports directives"

Imports System.Runtime.InteropServices
Imports System.Management

#End Region


Module MainModule

#Region "Is64BitOS (IsWow64Process)"

    ''' <summary>
    ''' The function determines whether the current operating system is a 
    ''' 64-bit operating system.
    ''' </summary>
    ''' <returns>
    ''' The function returns true if the operating system is 64-bit; 
    ''' otherwise, it returns false.
    ''' </returns>
    Public Function Is64BitOS() As Boolean
        If (IntPtr.Size = 8) Then  ' 64-bit programs run only on Win64
            Return True
        Else  ' 32-bit programs run on both 32-bit and 64-bit Windows
            ' Detect whether the current process is a 32-bit process running 
            ' on a 64-bit system.
            Dim flag As Boolean
            Return ((DoesWin32MethodExist("kernel32.dll", "IsWow64Process") _
                AndAlso IsWow64Process(GetCurrentProcess, flag)) AndAlso flag)
        End If
    End Function

    ''' <summary>
    ''' The function determins whether a method exists in the export table of 
    ''' a certain module.
    ''' </summary>
    ''' <param name="moduleName">The name of the module</param>
    ''' <param name="methodName">The name of the method</param>
    ''' <returns>
    ''' The function returns true if the method specified by methodName 
    ''' exists in the export table of the module specified by moduleName.
    ''' </returns>
    Function DoesWin32MethodExist(ByVal moduleName As String, ByVal methodName As String) As Boolean
        Dim moduleHandle As IntPtr = GetModuleHandle(moduleName)
        If (moduleHandle = IntPtr.Zero) Then
            Return False
        End If
        Return (GetProcAddress(moduleHandle, methodName) <> IntPtr.Zero)
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Function GetCurrentProcess() As IntPtr
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Function GetModuleHandle(ByVal moduleName As String) As IntPtr
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Function GetProcAddress(ByVal hModule As IntPtr, _
                            <MarshalAs(UnmanagedType.LPStr)> _
                            ByVal procName As String) _
                            As IntPtr
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Function IsWow64Process(ByVal hProcess As IntPtr, _
                            <Out()> ByRef wow64Process As Boolean) _
                            As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

#End Region

#Region "Is64BitOS (WMI)"

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

        Dim options As ConnectionOptions = Nothing

        If Not String.IsNullOrEmpty(userName) Then
            ' Build a ConnectionOptions object for the remote connection if 
            ' you plan to connect to the remote with a different user name 
            ' and password than the one you are currently using.
            options = New ConnectionOptions
            options.Username = userName
            options.Password = password
        End If
        ' Else the connection will use the currently logged-on user

        ' Make a connection to the target computer.
        If (String.IsNullOrEmpty(machineName)) Then
            machineName = "."
        End If
        Dim path As String = "\\" & machineName & "\root\cimv2"
        Dim scope As New ManagementScope(path, options)
        scope.Connect()

        ' Query Win32_Processor.AddressWidth which dicates the current 
        ' operating mode of the processor (on a 32-bit OS, it would be "32"; 
        ' on a 64-bit OS, it would be "64").
        ' Note: Win32_Processor.DataWidth indicates the capability of the 
        ' processor. On a 64-bit processor, it is "64".
        ' Note: Win32_OperatingSystem.OSArchitecture tells the bitness of OS 
        ' too. On a 32-bit OS, it would be "32-bit". However, it is only 
        ' available on Windows Vista and newer OS.
        Dim query As New ObjectQuery("SELECT AddressWidth FROM Win32_Processor")

        ' Perform the query and get the result.
        Dim searcher As New ManagementObjectSearcher(scope, query)
        Dim queryCollection As ManagementObjectCollection = searcher.Get
        For Each queryObj As ManagementObject In queryCollection
            If (queryObj("AddressWidth").ToString = "64") Then
                Return True
            End If
        Next
        Return False

    End Function

#End Region


    Sub Main()

        ' Solution 1. Is64BitOS (IsWow64Process)
        ' Determine whether the current operating system is a 64 bit 
        ' operating system.
        Dim f64bitOS As Boolean = Is64BitOS()
        Console.WriteLine("Current OS is {0}64-bit", IIf(f64bitOS, "", "not "))

        ' Solution 2. Is64BitOS (WMI)
        ' Determine whether the current operating system is a 64 bit 
        ' operating system through WMI. Note: The first solution of using 
        ' IsWow64Process is the preferred way to detect OS bitness of the 
        ' current system because it is much easier and faster. The WMI 
        ' solution is useful when you want to find this information on a 
        ' remote system. 
        Try
            ' If you want to get the OS bitness information of a remote 
            ' system, configure the system for remote connections of WMI 
            ' (http://msdn.microsoft.com/en-us/library/aa389290.aspx), and 
            ' replace the parameters (".", Nothing, Nothing) with the remote 
            ' computer name and credentials for the connection.
            f64bitOS = Is64BitOS(".", Nothing, Nothing)
            Console.WriteLine("Current OS is {0}64-bit", IIf(f64bitOS, "", "not "))
        Catch ex As Exception
            Console.WriteLine("Is64BitOS throws the exception: {0}", ex.Message)
        End Try

    End Sub

End Module