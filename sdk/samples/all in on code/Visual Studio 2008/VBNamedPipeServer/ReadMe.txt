========================================================================
    CONSOLE APPLICATION : VBNamedPipeServer Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

Named pipe is a mechanism for one-way or duplex inter-process communication 
between the pipe server and one or more pipe clients in the local machine or 
across the computers in the intranet:

Client (GENERIC_WRITE) ---> Server (GENERIC_READ)

PIPE_ACCESS_OUTBOUND:
Client (GENERIC_READ) <--- Server (GENERIC_WRITE)

PIPE_ACCESS_DUPLEX:
Client (GENERIC_READ or GENERIC_WRITE, or both) <--> 
Server (GENERIC_READ and GENERIC_WRITE)

This code sample demonstrate two methods to create named pipe in VB.NET.

1. Use the System.IO.Pipes namespace

The System.IO.Pipes namespace contains types that provide a means for 
interprocess communication through anonymous and/or named pipes. 
http://msdn.microsoft.com/en-us/library/system.io.pipes.aspx
These classes make the programming of named pipe in .NET much easier and 
safer than P/Invoking native APIs directly. However, the System.IO.Pipes 
namespace is not available before .NET Framework 3.5. So, if you are 
programming against .NET Framework 2.0, you have to P/Invoke native APIs 
to use named pipe.

The sample code in SystemIONamedPipeServer.Run() uses the 
Systen.IO.Pipes.NamedPipeServerStream class to create a pipe that is named 
"\\.\pipe\SamplePipe" to perform message-based communication. The pipe 
supports duplex connections, so both client and server can read 
from and write to the pipe. The security attributes of the pipe are 
customized to allow Authenticated Users read and write access to a pipe, 
and to allow the Administrators group full access to the pipe. When the 
pipe is connected by a client, the server attempts to read the client's 
message from the pipe, and write a response.

2. P/Invoke the native APIs related to named pipe operations.

The .NET interop services make it possible to call native APIs related to 
named pipe operations from .NET. The sample code in 
NativeNamedPipeServer.Run() demonstrates calling CreateNamedPipe to create 
a pipe named "\\.\pipe\SamplePipe", which supports duplex 
connections so that both client and server can read from and write to the 
pipe. The security attributes of the pipe are customized to allow 
Authenticated Users read and write access to a pipe, and to allow the 
Administrators group full access to the pipe. When the pipe is connected by 
a client, the server attempts to read the client's message from the pipe by 
calling ReadFile, and write a response by calling WriteFile. Please note 
that if you are programming against .NET Framework 3.5 or any newer 
releases of .NET Framework, it is safer and easier to use the types in the 
System.IO.Pipes namespace to operate named pipes.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the named pipe sample.

Step1. After you successfully build the VBNamedPipeClient and 
VBNamedPipeServer sample projects in Visual Studio 2008, you will get the 
applications: VBNamedPipeClient.exe and VBNamedPipeServer.exe. 

Step2. Run VBNamedPipeServer.exe in a command prompt to start up the server 
end of the named pipe. If the command is "VBNamedPipeServer.exe", the pipe 
server is created by the types in the System.IO.Pipes namespace. If the 
command is "VBNamedPipeServer.exe -native", the pipe server is created by 
P/Invoking the native APIs related to named pipe operations. In both cases, 
the application outputs the following information in the command prompt if 
the pipe is created successfully.

 Server:
  The named pipe (\\.\pipe\SamplePipe) is created.
  Waiting for the client's connection...

Step3. Run VBNamedPipeClient.exe in another command prompt to start up the 
client end of the named pipe. If the command is "VBNamedPipeClient.exe", the 
client connects to the pipe by using the types in the System.IO.Pipes 
namespace. If the command is "VBNamedPipeClient.exe -native", the client 
connects to the pipe by P/Invoking the native APIs related to named pipe 
operations. In both cases, the application should output the message below 
in the command prompt when the client successfully connects to the named pipe.

 Client:
  The named pipe (\\.\pipe\SamplePipe) is connected.

In the meantime, the server application outputs this message to indicate that 
the pipe is connected by a client.

 Server:
  Client is connected.

Step4. The client attempts to write a message to the named pipe. You will see 
the message below in the commond prompt running the client application.

 Client:
  Send 56 bytes to server: "Default request from client"

When the server application reads the message from the client, it prints:

 Server:
  Receive 56 bytes from client: "Default request from client"

Next, the server writes a response to the pipe.

 Server:
  Send 58 bytes to client: "Default response from server"

And the client receives the response:

 Client:
  Receive 58 bytes from server: "Default response from server"

The connection is disconnected and the pipe is closed after that.


/////////////////////////////////////////////////////////////////////////////
Sample Relation:
(The relationship between the current sample and the rest samples in 
Microsoft All-In-One Code Framework http://1code.codeplex.com)

CSNamedPipeClient/VBNamedPipeClient/CppNamedPipeClient -> VBNamedPipeServer
VBNamedPipeServer is the server end of the named pipe. CSNamedPipeClient, 
VBNamedPipeClient and CppNamedPipeClient can be the client ends that connect
to the named pipe.

VBNamedPipeServer - CSNamedPipeServer - CppNamedPipeServer
VBNamedPipeServer, CSNamedPipeServer and CppNamedPipeServer are the same 
named pipe server ends written in three different programming languages.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Named pipe server by using the System.IO.Pipes namespace. 
(SystemIONamedPipeServer.Run())

1. Create a named pipe server (System.IO.Pipes.NamedPipeServerStream) object 
and specify pipe name, pipe direction, options, transmission mode, security 
attributes, etc.

    ' Create the named pipe.
    pipeServer = New NamedPipeServerStream( _
        PIPE_NAME, _
        PipeDirection.InOut, _
        NamedPipeServerStream.MaxAllowedServerInstances, _
        PipeTransmissionMode.Message, _
        PipeOptions.None, _
        BUFFER_SIZE, _
        BUFFER_SIZE, _
        pipeSecurity, _
        HandleInheritability.None)

In this code sample, the pipe server support message-based duplex 
communications. The security attributes of the pipe are customized to allow 
Authenticated Users read and write access to a pipe, and to allow the 
Administrators group full access to the pipe.

    Dim pipeSecurity As New PipeSecurity()

    ' Allow Everyone read and write access to the pipe.
    pipeSecurity.SetAccessRule(New PipeAccessRule("Authenticated Users", _
        PipeAccessRights.ReadWrite, AccessControlType.Allow))

    ' Allow the Administrators group full access to the pipe.
    pipeSecurity.SetAccessRule(New PipeAccessRule("Administrators", _
        PipeAccessRights.FullControl, AccessControlType.Allow))

2. Wait for the client to connect by calling 
NamedPipeServerStream.WaitForConnection().

    pipeServer.WaitForConnection()

3. Read the client's request from the pipe and write a response by calling 
NamedPipeServerStream.Read and NamedPipeServerStream.Write. The named pipe 
was created to support message-based communication. This allows a reading 
process to read varying-length messages precisely as sent by the writing 
process. In this mode you should not use StreamWriter to write the pipe, or 
use StreamReader to read the pipe. You can read more about the difference 
from the article: http://go.microsoft.com/?linkid=9721786.

    ' 
    ' Receive a request from client.
    ' 

    Dim message As String
    Do
        Dim bRequest(BUFFER_SIZE - 1) As Byte
        Dim cbRequest As Integer = bRequest.Length, cbRead As Integer

        cbRead = pipeServer.Read(bRequest, 0, cbRequest)

        ' Unicode-encode the received byte array and trim all the '\0' 
        ' characters at the end.
        message = Encoding.Unicode.GetString(bRequest).TrimEnd( _
            ControlChars.NullChar)
        Console.WriteLine("Receive {0} bytes from client: ""{1}""", _
            cbRead, message)
    Loop While Not pipeServer.IsMessageComplete

    ' 
    ' Send a response from server to client.
    ' 

    message = RESPONSE_MESSAGE
    Dim bResponse As Byte() = Encoding.Unicode.GetBytes(message)
    Dim cbResponse As Integer = bResponse.Length

    pipeServer.Write(bResponse, 0, cbResponse)

    Console.WriteLine("Send {0} bytes to client: ""{1}""", _
        cbResponse, message.TrimEnd(ControlChars.NullChar))

4. Flush the pipe to allow the client to read the pipe's contents before 
disconnecting. Then disconnect the client's connection.

    pipeServer.WaitForPipeDrain()
    pipeServer.Disconnect()

5. Close the server end of the pipe by calling NamedPipeServerStream.Close().

    pipeServer.Close()

-------------------------

B. Named pipe server by P/Invoke the native APIs related to named pipe 
operations. (NativeNamedPipeServer.Run())

1. Create a named pipe server by P/Invoking CreateNamedPipe and specifying 
the pipe name, pipe direction, options, transmission mode, security 
attributes, etc.

    ' Create the named pipe.
    hNamedPipe = NativeMethod.CreateNamedPipe( _
        FULL_PIPE_NAME, _
        PipeOpenMode.PIPE_ACCESS_DUPLEX, _
        PipeMode.PIPE_TYPE_MESSAGE _
        Or PipeMode.PIPE_READMODE_MESSAGE _
        Or PipeMode.PIPE_WAIT, _
        PIPE_UNLIMITED_INSTANCES, _
        BUFFER_SIZE, _
        BUFFER_SIZE, _
        NMPWAIT_USE_DEFAULT_WAIT, _
        sa)

In this code sample, the pipe server support message-based duplex 
communications. The security attributes of the pipe are customized to allow 
Authenticated Users read and write access to a pipe, and to allow the 
Administrators group full access to the pipe.

    ' Define the SDDL for the security descriptor that allows read/write to 
    ' authenticated users and allows full control to administrators.
    Dim sddl As String = "D:(A;OICI;GRGW;;;AU)(A;OICI;GA;;;BA)"

    Dim pSecurityDescriptor As New SafeLocalMemHandle
    If (Not NativeMethod.ConvertStringSecurityDescriptorToSecurityDescriptor( _
        sddl, 1, pSecurityDescriptor, IntPtr.Zero)) Then
        Throw New Win32Exception
    End If

    Dim sa As New SECURITY_ATTRIBUTES()
    sa.nLength = Marshal.SizeOf(sa)
    sa.lpSecurityDescriptor = pSecurityDescriptor
    sa.bInheritHandle = False

2. Wait for the client to connect by calling ConnectNamedPipe.

    If (Not NativeMethod.ConnectNamedPipe(hNamedPipe, IntPtr.Zero)) Then
        If (Marshal.GetLastWin32Error() <> ERROR_PIPE_CONNECTED) Then
            Throw New Win32Exception
        End If
    End If

3. Read the client's request from the pipe and write a response by calling 
ReadFile and WriteFile.

    ' 
    ' Receive a request from client.
    ' 

    Dim message As String
    Dim finishRead As Boolean = False
    Do
        Dim bRequest(BUFFER_SIZE - 1) As Byte
        Dim cbRequest As Integer = bRequest.Length, cbRead As Integer

        finishRead = NativeMethod.ReadFile(hNamedPipe, bRequest, _
            cbRequest, cbRead, IntPtr.Zero)

        If (Not finishRead _
            AndAlso Marshal.GetLastWin32Error() <> ERROR_MORE_DATA) Then
            Throw New Win32Exception
        End If

        ' Unicode-encode the received byte array and trim all the '\0' 
        ' characters at the end.
        message = Encoding.Unicode.GetString(bRequest).TrimEnd( _
            ControlChars.NullChar)
        Console.WriteLine("Receive {0} bytes from client: ""{1}""", _
            cbRead, message)
    Loop While Not finishRead   ' Repeat loop if ERROR_MORE_DATA

    '
    ' Send a response from server to client.
    ' 

    message = RESPONSE_MESSAGE
    Dim bResponse As Byte() = Encoding.Unicode.GetBytes(message)
    Dim cbResponse As Integer = bResponse.Length, cbWritten As Integer

    If (Not NativeMethod.WriteFile(hNamedPipe, bResponse, cbResponse, _
        cbWritten, IntPtr.Zero)) Then
        Throw New Win32Exception
    End If

    Console.WriteLine("Send {0} bytes to client: ""{1}""", _
        cbWritten, message.TrimEnd(ControlChars.NullChar))

4. Flush the pipe to allow the client to read the pipe's contents before 
disconnecting. Then disconnect the client's connection.

    NativeMethod.FlushFileBuffers(hNamedPipe)
    NativeMethod.DisconnectNamedPipe(hNamedPipe)

5. Close the server end of the pipe.

    hNamedPipe.Close()


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: System.IO.Pipes Namespace
http://msdn.microsoft.com/en-us/library/system.io.pipes.aspx

MSDN: NamedPipeServerStream
http://msdn.microsoft.com/en-us/library/system.io.pipes.namedpipeserverstream.aspx

How to: Use Named Pipes to Communicate Between Processes over a Network
http://msdn.microsoft.com/en-us/library/bb546085.aspx

Introducing Pipes [Justin Van Patten]
http://blogs.msdn.com/bclteam/archive/2006/12/07/introducing-pipes-justin-van-patten.aspx


/////////////////////////////////////////////////////////////////////////////