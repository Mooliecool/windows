'****************************** Module Header ******************************\
' Module Name:  MainModule.vb
' Project:      VBPlatformDetector
' Copyright (c) Microsoft Corporation.
' 
' The CSPlatformDetector code sample demonstrates the following tasks related 
' to platform detection:
' 
' 1. Detect the name of the current operating system. 
'    (e.g. "Microsoft Windows 7 Enterprise")
' 2. Detect the version of the current operating system.
'    (e.g. "Microsoft Windows NT 6.1.7600.0")
' 3. Determine whether the current operating system is a 64-bit operating 
'    system. 
' 4. Determine whether the current process is a 64-bit process. 
' 5. Determine whether an arbitrary process running on the system is 64-bit. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System.Management
Imports System.Runtime.InteropServices


Module MainModule

    Sub Main(ByVal args As String())

        ' Print the name of the current operating system.
        Console.WriteLine("Current OS: " & GetOSName())

        ' Print the version string of the current operating system.
        Console.WriteLine("Version: " & Environment.OSVersion.VersionString)

        ' Determine the whether the current OS is a 64-bit operating system. 
        Console.WriteLine("Current OS is {0}64-bit", _
            If(Environment.Is64BitOperatingSystem, "", "not "))

        ' Determine whether the current process is a 64-bit process. 
        Console.WriteLine("Current process is {0}64-bit", _
            If(Environment.Is64BitProcess, "", "not "))

        ' Determine whether an arbitrary process running on the system is a 
        ' 64-bit process.
        If (args.Length > 0) Then
            ' If a process ID is specified in the command line, get the 
            ' process id, and open the process handle.
            Dim processId As Integer = 0
            If Integer.TryParse(args(0), processId) Then
                Dim hProcess As IntPtr = NativeMethods.OpenProcess( _
                    NativeMethods.PROCESS_QUERY_INFORMATION, False, processId)
                If (hProcess <> IntPtr.Zero) Then
                    Try
                        ' Detect whether the specified process is a 64-bit.
                        Dim is64bitProc As Boolean = Is64BitProcess(hProcess)
                        Console.WriteLine("Process {0} is {1}64-bit", _
                            processId.ToString, If(is64bitProc, "", "not "))
                    Finally
                        NativeMethods.CloseHandle(hProcess)
                    End Try
                Else
                    Dim errorCode As Integer = Marshal.GetLastWin32Error
                    Console.WriteLine("OpenProcess({0}) failed w/err 0x{1:X}", _
                        processId.ToString, errorCode.ToString)
                End If
            Else
                Console.WriteLine("Invalide process ID: {0}", processId.ToString)
            End If
        End If

    End Sub


    ''' <summary>
    ''' Gets the name of the currently running operating system. For example, 
    ''' "Microsoft Windows 7 Enterprise".
    ''' </summary>
    ''' <returns>The name of the currently running OS</returns>
    Function GetOSName() As String
        Dim searcher As New ManagementObjectSearcher("root\CIMV2", _
            "SELECT Caption FROM Win32_OperatingSystem")
        For Each queryObj As ManagementObject In searcher.Get()
            Return TryCast(queryObj.Item("Caption"), String)
        Next
        Return Nothing
    End Function


    ''' <summary>
    ''' Determines whether the specified process is a 64-bit process.
    ''' </summary>
    ''' <param name="hProcess">The process handle</param>
    ''' <returns>
    ''' true if the given process is 64-bit; otherwise, false.
    ''' </returns>
    Function Is64BitProcess(ByVal hProcess As IntPtr) As Boolean
        Dim flag As Boolean = False
        If Environment.Is64BitOperatingSystem Then
            ' On 64-bit OS, if a process is not running under Wow64 mode, 
            ' the process must be a 64-bit process.
            flag = Not (NativeMethods.IsWow64Process(hProcess, flag) AndAlso flag)
        End If
        Return flag
    End Function

End Module