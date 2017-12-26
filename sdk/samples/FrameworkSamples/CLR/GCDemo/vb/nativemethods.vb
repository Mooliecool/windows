'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'-----------------------------------------------------------------------
Imports System
Imports System.Runtime.InteropServices

'Namespace Microsoft.Samples


<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Unicode)> _
Structure MEMORYSTATUSEX
    Dim dwLength As Integer
    Dim dwMemoryLoad As Integer
    Dim ullTotalPhys As Long
    Dim ullAvailPhys As Long
    Dim ullTotalPageFile As Long
    Dim ullAvailPageFile As Long
    Dim ullTotalVirtual As Long
    Dim ullAvailVirtual As Long
    Dim ullAvailExtendedVirtual As Long

    Sub Init()
        dwLength = Marshal.SizeOf(GetType(MEMORYSTATUSEX))
    End Sub
End Structure

Friend NotInheritable Class NativeMethods
    Private Sub New()
    End Sub

    <DllImport("kernel32.dll", CharSet:=CharSet.Unicode)> _
    Shared Function GlobalMemoryStatusEx(ByRef memoryStatusEx As MEMORYSTATUSEX) As Integer
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Unicode)> _
    Shared Function VirtualAlloc(ByVal ptr As IntPtr, ByVal size As Integer, ByVal type As Integer, ByVal protect As Integer) As IntPtr
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Unicode)> _
    Shared Function VirtualFree(ByVal ptr As IntPtr, ByVal size As Integer, ByVal type As Integer) As Boolean
    End Function
End Class

'End Namespace
