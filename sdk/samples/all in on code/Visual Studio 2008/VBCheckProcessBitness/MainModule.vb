'****************************** Module Header ******************************\
' Module Name:  MainModule.vb
' Project:      VBCheckProcessBitness
' Copyright (c) Microsoft Corporation.
' 
' The code sample demonstrates how to determine whether the given process is 
' a 64-bit process or not.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System.Runtime.InteropServices


Module MainModule

    Sub Main(ByVal args As String())
        If args.Length > 0 Then
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
        Else
            ' If no process id was specified, use the id of the current 
            ' process, and determine whether the current process is a 64-bit 
            ' process.
            Console.WriteLine("Current process is {0}64-bit", _
                If(Is64BitProcess, "", "not "))
        End If
    End Sub


    ''' <summary>
    ''' Determines whether the current process is a 64-bit process.
    ''' </summary>
    ''' <returns>
    ''' true if the process is 64-bit; otherwise, false.
    ''' </returns>
    Function Is64BitProcess() As Boolean
        Return (IntPtr.Size = 8)
    End Function


    ''' <summary>
    ''' Determines whether the specified process is a 64-bit process.
    ''' </summary>
    ''' <param name="hProcess">The process handle</param>
    ''' <returns>
    ''' true if the process is 64-bit; otherwise, false.
    ''' </returns>
    Function Is64BitProcess(ByVal hProcess As IntPtr) As Boolean
        Dim flag As Boolean = False

        If Is64BitOS() Then
            ' On 64-bit OS, if a process is not running under Wow64 mode, 
            ' the process must be a 64-bit process.
            flag = Not (NativeMethods.IsWow64Process(hProcess, flag) AndAlso flag)
        End If

        Return flag
    End Function


    ''' <summary>
    ''' Determines whether the current operating system is a 64-bit operating 
    ''' system.
    ''' </summary>
    ''' <returns>
    ''' true if the operating system is 64-bit; otherwise, false.
    ''' </returns>
    Function Is64BitOS() As Boolean
        If IntPtr.Size = 8 Then ' 64-bit programs run only on Win64
            Return True
        Else ' 32-bit programs run on both 32-bit and 64-bit Windows
            ' Detect whether the current process is a 32-bit process running 
            ' on a 64-bit system.
            Dim flag As Boolean
            Return ((DoesWin32MethodExist("kernel32.dll", "IsWow64Process") _
                AndAlso NativeMethods.IsWow64Process(NativeMethods.GetCurrentProcess, flag)) _
                AndAlso flag)
        End If
    End Function


    ''' <summary>
    ''' Determines whether a method exists in the export table of a certain 
    ''' module.
    ''' </summary>
    ''' <param name="moduleName">The name of the module</param>
    ''' <param name="methodName">The name of the method</param>
    ''' <returns>
    ''' The function returns true if the method specified by methodName 
    ''' exists in the export table of the module specified by moduleName.
    ''' </returns>
    Function DoesWin32MethodExist(ByVal moduleName As String, ByVal methodName As String) As Boolean
        Dim moduleHandle As IntPtr = NativeMethods.GetModuleHandle(moduleName)
        If moduleHandle = IntPtr.Zero Then
            Return False
        End If
        Return (NativeMethods.GetProcAddress(moduleHandle, methodName) <> IntPtr.Zero)
    End Function

End Module