========================================================================
    CONSOLE APPLICATION : VBNamedPipeClient Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

Named pipe is a mechanism for one-way or duplex inter-process communication 
between the pipe server and one or more pipe clients in the local machine or 
across the computers in the intranet:

PIPE_ACCESS_INBOUND:
Client (GENERIC_WRITE) ---> Server (GENERIC_READ)

PIPE_ACCESS_OUTBOUND:
Client (GENERIC_READ) <--- Server (GENERIC_WRITE)

PIPE_ACCESS_DUPLEX:
Client (GENERIC_READ or GENERIC_WRITE, or both) <--> 
Server (GENERIC_READ and GENERIC_WRITE)

This code sample demonstrate two methods to use named pipe in Visual C#.

1. Use the System.IO.Pipes namespace

The System.IO.Pipes namespace contains types that provide a means for 
interprocess communication through anonymous and/or named pipes. 
http://msdn.microsoft.com/en-us/library/system.io.pipes.aspx
These classes make the programming of named pipe in .NET much easier and 
safer than P/Invoking native APIs directly. However, the System.IO.Pipes 
namespace is not available before .NET Framework 3.5. So, if you are 
programming against .NET Framework 2.0, you have to P/Invoke native APIs 
to use named pipe.

The sample code in SystemIONamedPipeClient.Run() uses the 
Systen.IO.Pipes.NamedPipeClientStream class to connect to the named pipe 
"\\.\pipe\SamplePipe" to perform message-based duplex communication. 
The client then writes a message to the pipe and receives the response from the 
pipe server.

2. P/Invoke the native APIs related to named pipe operations.

The .NET interop services make it possible to call native APIs related to 
named pipe operations from .NET. The sample code in 
NativeNamedPipeClient.Run() demonstrates calling CreateFile to connect to 
the named pipe "\\.\pipe\SamplePipe" with the GENERIC_READ and 
GENERIC_WRITE accesses, and calling WriteFile and ReadFile to write a 
message to the pipe and receive the response from the pipe server. Please 
note that if you are programming against .NET Framework 3.5 or any newer 
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

VBNamedPipeClient -> CSNamedPipeServer/VBNamedPipeServer/CppNamedPipeServer
VBNamedPipeServer is the client end of the named pipe. CSNamedPipeServer, 
VBNamedPipeServer and CppNamedPipeServer can be the server ends that create 
the named pipe.

VBNamedPipeClient - CSNamedPipeClient - CppNamedPipeClient
VBNamedPipeClient, CSNamedPipeClient and CppNamedPipeServer are the same 
named pipe client ends written in three different programming languages.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Named pipe client by using the System.IO.Pipes namespace. 
(SystemIONamedPipeClient.Run())

1. Create a NamedPipeClientStream object and specify the pipe server, name, 
pipe direction, options, etc. 

    pipeClient = New NamedPipeClientStream(Program.SERVER_NAME, _
        Program.PIPE_NAME, PipeDirection.InOut, PipeOptions.None)

2. Connect to the named pipe by calling NamedPipeClientStream.Connect().

    pipeClient.Connect(5000)

3. Set the read mode and the blocking mode of the named pipe. In this sample, 
we set data to be read from the pipe as a stream of messages. This allows a 
reading process to read varying-length messages precisely as sent by the 
writing process. In this mode, you should not use StreamWriter to write the 
pipe, or use StreamReader to read the pipe. You can read more about the 
difference from http://go.microsoft.com/?linkid=9721786.

    pipeClient.ReadMode = PipeTransmissionMode.Message

4. Send a message to the pipe server and receive its response through 
NamedPipeClientStream.Read and NamedPipeClientStream.Write. Because 
pipeClient.ReadMode = PipeTransmissionMode.Message, you should not use 
StreamWriter to write the pipe, or use StreamReader to read the pipe.

    '
    ' Send a request from client to server.
    '

    Dim message As String = REQUEST_MESSAGE
    Dim bRequest As Byte() = Encoding.Unicode.GetBytes(message)
    Dim cbRequest As Integer = bRequest.Length

    pipeClient.Write(bRequest, 0, cbRequest)

    Console.WriteLine("Send {0} bytes to server: ""{1}""", _
        cbRequest, message.TrimEnd(ControlChars.NullChar))

    '
    ' Receive a response from server.
    '

    Do
        Dim bResponse(BUFFER_SIZE - 1) As Byte
        Dim cbResponse As Integer = bResponse.Length, cbRead As Integer

        cbRead = pipeClient.Read(bResponse, 0, cbResponse)

        ' Unicode-encode the received byte array and trim all the '\0' 
        ' characters at the end.
        message = Encoding.Unicode.GetString(bResponse).TrimEnd( _
            ControlChars.NullChar)
        Console.WriteLine("Receive {0} bytes from server: ""{1}""", _
            cbRead, message)
    Loop While Not pipeClient.IsMessageComplete

5. Close the client end of the pipe by calling NamedPipeClientStream.Close().

    pipeClient.Close()

-------------------------

B. Named pipe client by P/Invoke the native APIs related to named pipe 
operations. (NativeNamedPipeClient.Run())

1. Try to connect to a named pipe by P/Invoking CreateFile and specifying the 
target pipe server, name, desired access, etc. 

    hNamedPipe = NativeMethod.CreateFile(FULL_PIPE_NAME, _
        FileDesiredAccess.GENERIC_READ Or _
        FileDesiredAccess.GENERIC_WRITE, _
        FileShareMode.Zero, Nothing, _
        FileCreationDisposition.OPEN_EXISTING, _
        0, IntPtr.Zero)

If all pipe instances are busy, wait for 5 seconds and connect again.

    If (Not NativeMethod.WaitNamedPipe(Program.FULL_PIPE_NAME, 5000)) Then
        Throw New Win32Exception
    End If

2. Set the read mode and the blocking mode of the named pipe. In this sample, 
we set data to be read from the pipe as a stream of messages.

    Dim mode As PipeMode = PipeMode.PIPE_READMODE_MESSAGE
    If (Not NativeMethod.SetNamedPipeHandleState(hNamedPipe, mode, _
        IntPtr.Zero, IntPtr.Zero)) Then
        Throw New Win32Exception
    End If

3. Send a message to the pipe server and receive its response by calling 
the WriteFile and ReadFile functions.

    '
    ' Send a request from client to server.
    '

    Dim message As String = REQUEST_MESSAGE
    Dim bRequest As Byte() = Encoding.Unicode.GetBytes(message)
    Dim cbRequest As Integer = bRequest.Length, cbWritten As Integer

    If (Not NativeMethod.WriteFile(hNamedPipe, bRequest, cbRequest, _
        cbWritten, IntPtr.Zero)) Then
        Throw New Win32Exception
    End If

    Console.WriteLine("Send {0} bytes to server: ""{1}""", _
        cbWritten, message.TrimEnd(ControlChars.NullChar))

    '
    ' Receive a response from server.
    '

    Dim finishRead As Boolean = False
    Do
        Dim bResponse(BUFFER_SIZE - 1) As Byte
        Dim cbResponse As Integer = bResponse.Length, cbRead As Integer

        finishRead = NativeMethod.ReadFile(hNamedPipe, bResponse, _
            cbResponse, cbRead, IntPtr.Zero)

        If (Not finishRead _
            AndAlso Marshal.GetLastWin32Error() <> ERROR_MORE_DATA) Then
            Throw New Win32Exception
        End If

        ' Unicode-encode the received byte array and trim all the '\0' 
        ' characters at the end.
        message = Encoding.Unicode.GetString(bResponse).TrimEnd( _
            ControlChars.NullChar)
        Console.WriteLine("Receive {0} bytes from server: ""{1}""", _
            cbRead, message)

    Loop While Not finishRead   ' Repeat loop if ERROR_MORE_DATA

4. Close the pipe.

    hNamedPipe.Close()


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: System.IO.Pipes Namespace
http://msdn.microsoft.com/en-us/library/system.io.pipes.aspx

MSDN: NamedPipeClientStream
http://msdn.microsoft.com/en-us/library/system.io.pipes.namedpipeclientstream.aspx

How to: Use Named Pipes to Communicate Between Processes over a Network
http://msdn.microsoft.com/en-us/library/bb546085.aspx

Introducing Pipes [Justin Van Patten]
http://blogs.msdn.com/bclteam/archive/2006/12/07/introducing-pipes-justin-van-patten.aspx


/////////////////////////////////////////////////////////////////////////////