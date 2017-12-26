'******************************* Module Header *********************************'
' Module Name:  MainModule.vb
' Project:      VBNamedPipeClient
' Copyright (c) Microsoft Corporation.
'
' Named pipe is a mechanism for one-way or duplex inter-process communication 
' between the pipe server and one or more pipe clients in the local machine or 
' across the computers in the intranet:
'
' PIPE_ACCESS_INBOUND:
' Client (GENERIC_WRITE) ---> Server (GENERIC_READ)
'
' PIPE_ACCESS_OUTBOUND:
' Client (GENERIC_READ) <--- Server (GENERIC_WRITE)
'
' PIPE_ACCESS_DUPLEX:
' Client (GENERIC_READ or GENERIC_WRITE, or both) <-->
' Server (GENERIC_READ and GENERIC_WRITE)
'
' This code sample demonstrate two methods to use named pipe in VB.NET.
'
' 1. Use the System.IO.Pipes namespace
'
' The System.IO.Pipes namespace contains types that provide a means for
' interprocess communication through anonymous and/or named pipes.
' http://msdn.microsoft.com/en-us/library/system.io.pipes.aspx
' These classes make the programming of named pipe in .NET much easier and
' safer than P/Invoking native APIs directly. However, the System.IO.Pipes
' namespace is not available before .NET Framework 3.5. So, if you are
' programming against .NET Framework 2.0, you have to P/Invoke native APIs
' to use named pipe.
'
' The sample code in SystemIONamedPipeClient.Run() uses the
' Systen.IO.Pipes.NamedPipeClientStream class to connect to the named pipe
' "\\.\pipe\SamplePipe" to perform message-based duplex communication.
' The client then writes a message to the pipe and receives the response from
' the pipe server.
'
' 2. P/Invoke the native APIs related to named pipe operations.
'
' The .NET interop services make it possible to call native APIs related to
' named pipe operations from .NET. The sample code in
' NativeNamedPipeClient.Run() demonstrates calling CreateFile to connect to
' the named pipe "\\.\pipe\SamplePipe" with the GENERIC_READ and
' GENERIC_WRITE accesses, and calling WriteFile and ReadFile to write a
' message to the pipe and receive the response from the pipe server. Please
' note that if you are programming against .NET Framework 3.5 or any newer
' releases of .NET Framework, it is safer and easier to use the types in the
' System.IO.Pipes namespace to operate named pipes.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*******************************************************************************'


Module MainModule

    ' The full name of the pipe in the format of \\servername\pipe\pipename.
    Friend Const ServerName As String = "."
    Friend Const PipeName As String = "SamplePipe"
    Friend Const FullPipeName As String = "\\" & ServerName & "\pipe\" & PipeName

    Friend Const BufferSize As Integer = 1024

    ' Request message from client to server. '\0' is appended in the end because 
    ' the client may be a native C++ application that expects NULL termiated 
    ' string.
    Friend Const RequestMessage As String = "Default request from client" & _
    ControlChars.NullChar


    Sub Main(ByVal args As String())

        If args.Length > 0 AndAlso args(0) = "-native" Then
            ' If the command line is "VBNamedPipeClient.exe -native", P/Invoke 
            ' the native APIs related to named pipe operations to connect to the 
            ' named pipe.
            NativeNamedPipeClient.Run()
        Else
            ' Use the types in the System.IO.Pipes namespace to connect to the 
            ' named pipe. This solution is recommended when you program against 
            ' .NET Framework 3.5 or any newer releases of .NET Framework. 
            SystemIONamedPipeClient.Run()
        End If

    End Sub

End Module