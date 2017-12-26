'****************************** Module Header ******************************\
' Module Name:  NativeMethods.vb
' Project:      VBCheckProcessBitness
' Copyright (c) Microsoft Corporation.
' 
' Declares the P/Invoke signatures of Win32 APIs.
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


Friend Class NativeMethods

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function GetCurrentProcess() As IntPtr
    End Function

    Public Const PROCESS_QUERY_INFORMATION As Int32 = &H400

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function OpenProcess(ByVal dwDesiredAccess As Int32, _
        <MarshalAs(UnmanagedType.Bool)> ByVal bInheritHandle As Boolean, _
        ByVal dwProcessId As Int32) As IntPtr
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function CloseHandle(ByVal hObject As IntPtr) _
        As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function GetModuleHandle(ByVal moduleName As String) As IntPtr
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function GetProcAddress(ByVal hModule As IntPtr, _
        <MarshalAs(UnmanagedType.LPStr)> ByVal procName As String) As IntPtr
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function IsWow64Process(ByVal hProcess As IntPtr, _
        <Out()> ByRef wow64Process As Boolean) _
        As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

End Class
