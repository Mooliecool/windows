'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
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
' HandleRef.vb

Imports System
Imports System.IO
Imports System.Text
Imports System.Runtime.InteropServices

Namespace Microsoft.Samples
    ' declared as structure
    <StructLayout(LayoutKind.Sequential)> _
    Public Structure Overlapped
        Private intrnal As IntPtr
        Private internalHigh As IntPtr
        Public offset As Integer
        Public offsetHigh As Integer
        Private hEvent As IntPtr
    End Structure 'Overlapped

    ' declared as class
    <StructLayout(LayoutKind.Sequential)> _
    Public Class Overlapped2
        'Private intrnal As IntPtr
        'Private internalHigh As IntPtr
        Public offset As Integer
        Public offsetHigh As Integer
        'Private hEvent As IntPtr
    End Class 'Overlapped2


    Public Class LibWrap

        ' to prevent FileStream to be GC-ed before call ends, 
        ' its handle should be wrapped in HandleRef

        ' since Overlapped is struct, Nothing can't be passed instead, 
        ' we must declare overload method if we will use this	

        ' BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
        '				LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);    

        Overloads Declare Ansi Function ReadFile Lib "Kernel32.dll" ( _
         ByVal hndRef As SafeHandle, _
         ByVal buffer As StringBuilder, _
         ByVal numberOfBytesToRead As Integer, _
         ByRef numberOfBytesRead As Integer, _
         ByRef flag As Overlapped) _
        As Boolean

        Overloads Declare Ansi Function ReadFile Lib "Kernel32.dll" ( _
         ByVal hndRef As SafeHandle, _
         ByVal buffer As StringBuilder, _
         ByVal numberOfBytesToRead As Integer, _
         ByRef numberOfBytesRead As Integer, _
         ByVal flag As Integer) _
        As Boolean  ' int instead of structure reference

        ' since Overlapped2 is class, we can pass Nothing as parameter  
        ' no overload is needed		

        Declare Ansi Function ReadFile2 Lib "Kernel32.dll" Alias "ReadFile" ( _
         ByVal hndRef As SafeHandle, _
         ByVal buffer As StringBuilder, _
         ByVal numberOfBytesToRead As Integer, _
         ByRef numberOfBytesRead As Integer, _
         <[In](), Out()> ByVal flag As Overlapped2) _
        As Boolean

    End Class 'LibWrap

    Public Class App
        Public Shared Sub Main()

            Dim fs As New FileStream("HandleRef.txt", FileMode.Open)
            Dim sh As SafeHandle = fs.SafeFileHandle
            Dim buffer As New StringBuilder(5)
            Dim read As Integer = 0

            ' platform invoke will hold reference to HandleRef until call ends

            LibWrap.ReadFile(sh, buffer, 5, read, 0)
            Console.WriteLine("Read with struct parameter: {0}", buffer)

            LibWrap.ReadFile2(sh, buffer, 5, read, Nothing)
            Console.WriteLine("Read with class parameter: {0}", buffer)

        End Sub 'Main
    End Class 'App
End Namespace
