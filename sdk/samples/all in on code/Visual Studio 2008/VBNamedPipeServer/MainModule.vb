'******************************** Module Header ********************************'
' Module Name:  MainModule.vb
' Project:      VBNamedPipeServer
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
' This code sample demonstrate two methods to create named pipe in VB.NET.
' 
' 1. Use the System.IO.Pipes namespace
' 
' The System.IO.Pipes namespace contains types that provide a means for 
' interprocess communication through anonymous and/or named pipes. 
' http://msdn.microsoft.com/en-us/library/system.io.pipes.aspx
' These classes make the programming of named pipe in .NET much easier and safer
' than P/Invoking native APIs directly. However, the System.IO.Pipes namespace is 
' not available before .NET Framework 3.5. So, if you are programming against 
' .NET Framework 2.0, you have to P/Invoke native APIs to use named pipe.
' 
' The sample code in SystemIONamedPipeServer.Run() uses the 
' Systen.IO.Pipes.NamedPipeServerStream class to create a pipe that is named 
' "\\.\pipe\SamplePipe" to perform message-based communication. The pipe supports
' duplex connections, so both client and server can read from and write 
' to the pipe. The security attributes of the pipe are customized to allow 
' Authenticated Users read and write access to a pipe, and to allow the 
' Administrators group full access to the pipe. When the pipe is connected by a 
' client, the server attempts to read the client's message from the pipe, and 
' write a response.
' 
' 2. P/Invoke the native APIs related to named pipe operations.
' 
' The .NET interop services make it possible to call native APIs related to named
' pipe operations from .NET. The sample code in NativeNamedPipeServer.Run() 
' demonstrates calling CreateNamedPipe to create a pipe named 
' "\\.\pipe\SamplePipe", which supports duplex connections so that both 
' client and server can read from and write to the pipe. The security attributes 
' of the pipe are customized to allow Authenticated Users read and write access 
' to a pipe, and to allow the Administrators group full access to the pipe. When 
' the pipe is connected by a client, the server attempts to read the client's 
' message from the pipe by calling ReadFile, and write a response by calling 
' WriteFile. Please note that if you are programming against .NET Framework 3.5 
' or any newer releases of .NET Framework, it is safer and easier to use the 
' types in the System.IO.Pipes namespace to operate named pipes.
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
    Friend Const ResponseMessage As String = "Default response from server" & _
    ControlChars.NullChar


    Sub Main(ByVal args As String())

        If args.Length > 0 AndAlso args(0) = "-native" Then
            ' If the command line is "VBNamedPipeServer.exe -native", P/Invoke 
            ' the native pipe APIs to create the named pipe.
            NativeNamedPipeServer.Run()
        Else
            ' Use the types in the System.IO.Pipes namespace to create the named
            ' pipe. This solution is recommended when you program against .NET 
            ' Framework 3.5 or any newer releases of .NET Framework.
            SystemIONamedPipeServer.Run()
        End If

    End Sub

End Module