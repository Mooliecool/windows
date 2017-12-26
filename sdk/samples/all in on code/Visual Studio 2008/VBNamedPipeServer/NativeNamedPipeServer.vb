'******************************** Module Header ********************************'
' Module Name:  NativeNamedPipeServer.vb
' Project:      VBNamedPipeServer
' Copyright (c) Microsoft Corporation.
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

#Region "Imports directives"

Imports System.Text
Imports System.Runtime.InteropServices
Imports System.Runtime.ConstrainedExecution
Imports System.ComponentModel
Imports System.Security
Imports System.Security.Permissions
Imports Microsoft.Win32.SafeHandles

#End Region


Class NativeNamedPipeServer

    ''' <summary>
    ''' P/Invoke the native APIs related to named pipe operations to create the 
    ''' named pipe.
    ''' </summary>
    Public Shared Sub Run()
        Dim hNamedPipe As SafePipeHandle = Nothing

        Try
            ' Prepare the security attributes (the securityAttributes parameter
            ' in CreateNamedPipe) for the pipe. This is optional. If 
            ' securityAttributes of CreateNamedPipe is null, the named pipe gets 
            ' a default security descriptor and the handle cannot be inherited. 
            ' The ACLs in the default security descriptor of a pipe grant full 
            ' control to the LocalSystem account, (elevated) administrators, and 
            ' the creator owner. They also give only read access to members of 
            ' the Everyone group and the anonymous account. However, if you want 
            ' to customize the security permission of the pipe, (e.g. to allow 
            ' Authenticated Users to read from and write to the pipe), you need 
            ' to create a SECURITY_ATTRIBUTES object.
            Dim sa As SECURITY_ATTRIBUTES = Nothing
            sa = CreateNativePipeSecurity()

            ' Create the named pipe.
            hNamedPipe = NativeMethod.CreateNamedPipe( _
                FullPipeName, _
                PipeOpenMode.PIPE_ACCESS_DUPLEX, _
                PipeMode.PIPE_TYPE_MESSAGE _
                Or PipeMode.PIPE_READMODE_MESSAGE _
                Or PipeMode.PIPE_WAIT, _
                PIPE_UNLIMITED_INSTANCES, _
                BufferSize, _
                BufferSize, _
                NMPWAIT_USE_DEFAULT_WAIT, _
                sa)

            If (hNamedPipe.IsInvalid) Then
                Throw New Win32Exception
            End If

            Console.WriteLine("The named pipe ({0}) is created.", FullPipeName)

            ' Wait for the client to connect.
            Console.WriteLine("Waiting for the client's connection...")
            If (Not NativeMethod.ConnectNamedPipe(hNamedPipe, IntPtr.Zero)) Then
                If (Marshal.GetLastWin32Error() <> ERROR_PIPE_CONNECTED) Then
                    Throw New Win32Exception
                End If
            End If
            Console.WriteLine("Client is connected.")

            ' 
            ' Receive a request from client.
            ' 

            Dim message As String
            Dim finishRead As Boolean = False
            Do
                Dim bRequest(BufferSize - 1) As Byte
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

            message = ResponseMessage
            Dim bResponse As Byte() = Encoding.Unicode.GetBytes(message)
            Dim cbResponse As Integer = bResponse.Length, cbWritten As Integer

            If (Not NativeMethod.WriteFile(hNamedPipe, bResponse, cbResponse, _
                cbWritten, IntPtr.Zero)) Then
                Throw New Win32Exception
            End If

            Console.WriteLine("Send {0} bytes to client: ""{1}""", _
                cbWritten, message.TrimEnd(ControlChars.NullChar))

            ' Flush the pipe to allow the client to read the pipe's contents 
            ' before disconnecting. Then disconnect the client's connection.
            NativeMethod.FlushFileBuffers(hNamedPipe)
            NativeMethod.DisconnectNamedPipe(hNamedPipe)

        Catch ex As Exception
            Console.WriteLine("The server throws the error: {0}", ex.Message)
        Finally
            If (hNamedPipe IsNot Nothing) Then
                hNamedPipe.Close()
                hNamedPipe = Nothing
            End If
        End Try
    End Sub


    ''' <summary>
    ''' The CreateNativePipeSecurity function creates and initializes a new 
    ''' SECURITY_ATTRIBUTES object to allow Authenticated Users read and 
    ''' write access to a pipe, and to allow the Administrators group full 
    ''' access to the pipe.
    ''' </summary>
    ''' <returns>
    ''' A SECURITY_ATTRIBUTES object that allows Authenticated Users read and 
    ''' write access to a pipe, and allows the Administrators group full 
    ''' access to the pipe.
    ''' </returns>
    Private Shared Function CreateNativePipeSecurity() As SECURITY_ATTRIBUTES
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
        Return sa
    End Function


#Region "Native API Signatures and Types"

    ''' <summary>
    ''' Desired Access of File/Device
    ''' </summary>
    <Flags()> _
    Friend Enum FileDesiredAccess As UInteger
        GENERIC_READ = &H80000000UI
        GENERIC_WRITE = &H40000000
        GENERIC_EXECUTE = &H20000000
        GENERIC_ALL = &H10000000
    End Enum

    ''' <summary>
    ''' File share mode
    ''' </summary>
    <Flags()> _
    Friend Enum FileShareMode As UInteger
        Zero = &H0  ' No sharing.
        FILE_SHARE_DELETE = &H4
        FILE_SHARE_READ = &H1
        FILE_SHARE_WRITE = &H2
    End Enum

    ''' <summary>
    ''' File Creation Disposition
    ''' </summary>
    Friend Enum FileCreationDisposition As UInteger
        CREATE_NEW = 1
        CREATE_ALWAYS = 2
        OPEN_EXISTING = 3
        OPEN_ALWAYS = 4
        TRUNCATE_EXISTING = 5
    End Enum

    ''' <summary>
    ''' Named Pipe Open Modes
    ''' http://msdn.microsoft.com/en-us/library/aa365596.aspx
    ''' </summary>
    <Flags()> _
    Friend Enum PipeOpenMode As UInteger
        PIPE_ACCESS_INBOUND = &H1   ' Inbound pipe access.
        PIPE_ACCESS_OUTBOUND = &H2  ' Outbound pipe access.
        PIPE_ACCESS_DUPLEX = &H3    ' Duplex pipe access.
    End Enum

    ''' <summary>
    ''' Named Pipe Type, Read, and Wait Modes
    ''' http://msdn.microsoft.com/en-us/library/aa365605.aspx
    ''' </summary>
    Friend Enum PipeMode As UInteger
        ' Type Mode
        PIPE_TYPE_BYTE = &H0        ' Byte pipe type.
        PIPE_TYPE_MESSAGE = &H4     ' Message pipe type.

        ' Read Mode
        PIPE_READMODE_BYTE = &H0    ' Read mode of type Byte.
        PIPE_READMODE_MESSAGE = &H2 ' Read mode of type Message.

        ' Wait Mode
        PIPE_WAIT = &H0             ' Pipe blocking mode.
        PIPE_NOWAIT = &H1           ' Pipe non-blocking mode.
    End Enum

    ''' <summary>
    ''' Unlimited server pipe instances.
    ''' </summary>
    Friend Const PIPE_UNLIMITED_INSTANCES As Integer = 255

    ''' <summary>
    ''' The operation completed successfully.
    ''' </summary>
    Friend Const ERROR_SUCCESS As Integer = 0

    ''' <summary>
    ''' The system cannot find the file specified.
    ''' </summary>
    Friend Const ERROR_CANNOT_CONNECT_TO_PIPE As Integer = 2

    ''' <summary>
    ''' All pipe instances are busy.
    ''' </summary>
    Friend Const ERROR_PIPE_BUSY As Integer = 231

    ''' <summary>
    ''' The pipe is being closed.
    ''' </summary>
    Friend Const ERROR_NO_DATA As Integer = 232

    ''' <summary>
    ''' No process is on the other end of the pipe.
    ''' </summary>
    Friend Const ERROR_PIPE_NOT_CONNECTED As Integer = 233

    ''' <summary>
    ''' More data is available.
    ''' </summary>
    Public Const ERROR_MORE_DATA As Integer = 234

    ''' <summary>
    ''' There is a process on other end of the pipe.
    ''' </summary>
    Friend Const ERROR_PIPE_CONNECTED As Integer = 535

    ''' <summary>
    ''' Waiting for a process to open the other end of the pipe.
    ''' </summary>
    Friend Const ERROR_PIPE_LISTENING As Integer = 536


    ''' <summary>
    ''' Waits indefinitely when connecting to a pipe.
    ''' </summary>
    Friend Const NMPWAIT_WAIT_FOREVER As UInteger = &HFFFFFFFFUI

    ''' <summary>
    ''' Does not wait for the named pipe.
    ''' </summary>
    Friend Const NMPWAIT_NOWAIT As UInteger = &H1

    ''' <summary>
    ''' Uses the default time-out specified in a call to the
    ''' CreateNamedPipe method.
    ''' </summary>
    Friend Const NMPWAIT_USE_DEFAULT_WAIT As UInteger = &H0


    ''' <summary>
    ''' Represents a wrapper class for a pipe handle.
    ''' </summary>
    <SecurityCritical(SecurityCriticalScope.Everything), _
    HostProtection(SecurityAction.LinkDemand, MayLeakOnAbort:=True), _
    SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode:=True)> _
    Friend NotInheritable Class SafePipeHandle
        Inherits SafeHandleZeroOrMinusOneIsInvalid

        Private Sub New()
            MyBase.New(True)
        End Sub

        Public Sub New(ByVal preexistingHandle As IntPtr, _
            ByVal ownsHandle As Boolean)
            MyBase.New(ownsHandle)
            MyBase.SetHandle(preexistingHandle)
        End Sub

        <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Private Shared Function CloseHandle(ByVal handle As IntPtr) _
            As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function

        Protected Overloads Overrides Function ReleaseHandle() As Boolean
            Return (CloseHandle(MyBase.handle))
        End Function
    End Class


    ''' <summary>
    ''' The SECURITY_ATTRIBUTES structure contains the security descriptor for an
    ''' object and specifies whether the handle retrieved by specifying this 
    ''' structure is inheritable. This structure provides security settings for
    ''' objects created by various functions, such as CreateFile, CreateNamedPipe, 
    ''' CreateProcess, RegCreateKeyEx, or RegSaveKeyEx.
    ''' </summary>
    <StructLayout(LayoutKind.Sequential)> _
    Friend Class SECURITY_ATTRIBUTES
        Public nLength As Integer
        Public lpSecurityDescriptor As SafeLocalMemHandle
        Public bInheritHandle As Boolean
    End Class


    ''' <summary>
    ''' Represents a wrapper class for a local memory pointer.
    ''' </summary>
    <SuppressUnmanagedCodeSecurity(), _
    HostProtection(SecurityAction.LinkDemand, MayLeakOnAbort:=True)> _
    Friend NotInheritable Class SafeLocalMemHandle
        Inherits SafeHandleZeroOrMinusOneIsInvalid

        Public Sub New()
            MyBase.New(True)
        End Sub

        Public Sub New(ByVal preexistingHandle As IntPtr, _
            ByVal ownsHandle As Boolean)
            MyBase.New(ownsHandle)
            MyBase.SetHandle(preexistingHandle)
        End Sub

        <ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success), _
            DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Private Shared Function LocalFree(ByVal hMem As IntPtr) As IntPtr
        End Function

        Protected Overloads Overrides Function ReleaseHandle() As Boolean
            Return (LocalFree(MyBase.handle) = IntPtr.Zero)
        End Function
    End Class


    ''' <summary>
    ''' The class exposes Windows APIs to be used in this code sample.
    ''' </summary>
    <SuppressUnmanagedCodeSecurity()> _
    Friend Class NativeMethod

        ''' <summary>
        ''' Creates an instance of a named pipe and returns a handle for
        ''' subsequent pipe operations.
        ''' </summary>
        ''' <param name="pipeName">Pipe name</param>
        ''' <param name="openMode">Pipe open mode</param>
        ''' <param name="pipeMode">Pipe-specific modes</param>
        ''' <param name="maxInstances">Maximum number of instances</param>
        ''' <param name="outBufferSize">Output buffer size</param>
        ''' <param name="inBufferSize">Input buffer size</param>
        ''' <param name="defaultTimeout">Time-out interval</param>
        ''' <param name="securityAttributes">Security attributes</param>
        ''' <returns>
        ''' If the function succeeds, the return value is a handle to the server
        ''' end of a named pipe instance.
        ''' </returns>
        <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function CreateNamedPipe(ByVal pipeName As String, _
            ByVal openMode As PipeOpenMode, _
            ByVal pipeMode As PipeMode, _
            ByVal maxInstances As Integer, _
            ByVal outBufferSize As Integer, _
            ByVal inBufferSize As Integer, _
            ByVal defaultTimeout As UInteger, _
            ByVal securityAttributes As SECURITY_ATTRIBUTES) As SafePipeHandle
        End Function

        ''' <summary>
        ''' Enables a named pipe server process to wait for a client process to
        ''' connect to an instance of a named pipe.
        ''' </summary>
        ''' <param name="hNamedPipe">
        ''' Handle to the server end of a named pipe instance.
        ''' </param>
        ''' <param name="overlapped">Pointer to an Overlapped object.</param>
        ''' <returns>
        ''' If the function succeeds, the return value is true.
        ''' </returns>
        <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function ConnectNamedPipe(ByVal hNamedPipe As SafePipeHandle, _
            ByVal overlapped As IntPtr) As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function


        ''' <summary>
        ''' Waits until either a time-out interval elapses or an instance of the
        ''' specified named pipe is available for connection (that is, the pipe's
        ''' server process has a pending ConnectNamedPipe operation on the pipe).
        ''' </summary>
        ''' <param name="pipeName">The name of the named pipe.</param>
        ''' <param name="timeout">
        ''' The number of milliseconds that the function will wait for an
        ''' instance of the named pipe to be available.
        ''' </param>
        ''' <returns>
        ''' If an instance of the pipe is available before the time-out interval
        ''' elapses, the return value is true.
        ''' </returns>
        <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function WaitNamedPipe(ByVal pipeName As String, _
            ByVal timeout As UInteger) As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function


        ''' <summary>
        ''' Sets the read mode and the blocking mode of the specified named pipe.
        ''' </summary>
        ''' <remarks>
        ''' If the specified handle is to the client end of a named pipe and if
        ''' the named pipe server process is on a remote computer, the function
        ''' can also be used to control local buffering.
        ''' </remarks>
        ''' <param name="hNamedPipe">Handle to the named pipe instance.</param>
        ''' <param name="mode">
        ''' Pointer to a variable that supplies the new mode.
        ''' </param>
        ''' <param name="maxCollectionCount">
        ''' Reference to a variable that specifies the maximum number of bytes
        ''' collected on the client computer before transmission to the server.
        ''' </param>
        ''' <param name="collectDataTimeout">
        ''' Reference to a variable that specifies the maximum time, in
        ''' milliseconds, that can pass before a remote named pipe transfers
        ''' information over the network.
        ''' </param>
        ''' <returns>If the function succeeds, the return value is true.</returns>
        <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function SetNamedPipeHandleState( _
            ByVal hNamedPipe As SafePipeHandle, _
            ByRef mode As PipeMode, _
            ByVal maxCollectionCount As IntPtr, _
            ByVal collectDataTimeout As IntPtr) _
            As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function


        ''' <summary>
        ''' Creates or opens a file, directory, physical disk, volume, console
        ''' buffer, tape drive, communications resource, mailslot, or named pipe.
        ''' </summary>
        ''' <param name="fileName">
        ''' The name of the file or device to be created or opened.
        ''' </param>
        ''' <param name="desiredAccess">
        ''' The requested access to the file or device, which can be summarized
        ''' as read, write, both or neither (zero).
        ''' </param>
        ''' <param name="shareMode">
        ''' The requested sharing mode of the file or device, which can be read,
        ''' write, both, delete, all of these, or none (refer to the following
        ''' table).
        ''' </param>
        ''' <param name="securityAttributes">
        ''' A SECURITY_ATTRIBUTES object that contains two separate but related
        ''' data members: an optional security descriptor, and a Boolean value
        ''' that determines whether the returned handle can be inherited by
        ''' child processes.
        ''' </param>
        ''' <param name="creationDisposition">
        ''' An action to take on a file or device that exists or does not exist.
        ''' </param>
        ''' <param name="flagsAndAttributes">
        ''' The file or device attributes and flags.
        ''' </param>
        ''' <param name="hTemplateFile">Handle to a template file.</param>
        ''' <returns>
        ''' If the function succeeds, the return value is an open handle to the
        ''' specified file, device, named pipe, or mail slot.
        ''' If the function fails, the return value is INVALID_HANDLE_VALUE.
        ''' </returns>
        <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function CreateFile(ByVal fileName As String, _
            ByVal desiredAccess As FileDesiredAccess, _
            ByVal shareMode As FileShareMode, _
            ByVal securityAttributes As SECURITY_ATTRIBUTES, _
            ByVal creationDisposition As FileCreationDisposition, _
            ByVal flagsAndAttributes As Integer, _
            ByVal hTemplateFile As IntPtr) As SafePipeHandle
        End Function


        ''' <summary>
        ''' Reads data from the specified file or input/output (I/O) device.
        ''' </summary>
        ''' <param name="handle">
        ''' A handle to the device (for example, a file, file stream, physical
        ''' disk, volume, console buffer, tape drive, socket, communications
        ''' resource, mailslot, or pipe).
        ''' </param>
        ''' <param name="bytes">
        ''' A buffer that receives the data read from a file or device.
        ''' </param>
        ''' <param name="numBytesToRead">
        ''' The maximum number of bytes to be read.
        ''' </param>
        ''' <param name="numBytesRead">
        ''' The number of bytes read when using a synchronous IO.
        ''' </param>
        ''' <param name="overlapped">
        ''' A pointer to an OVERLAPPED structure if the file was opened with
        ''' FILE_FLAG_OVERLAPPED.
        ''' </param>
        ''' <returns>
        ''' If the function succeeds, the return value is true. If the function
        ''' fails, or is completing asynchronously, the return value is false.
        ''' </returns>
        <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function ReadFile(ByVal handle As SafePipeHandle, _
            ByVal bytes As Byte(), _
            ByVal numBytesToRead As Integer, _
            ByRef numBytesRead As Integer, _
            ByVal overlapped As IntPtr) _
            As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function


        ''' <summary>
        ''' Writes data to the specified file or input/output (I/O) device.
        ''' </summary>
        ''' <param name="handle">
        ''' A handle to the file or I/O device (for example, a file, file stream,
        ''' physical disk, volume, console buffer, tape drive, socket,
        ''' communications resource, mailslot, or pipe).
        ''' </param>
        ''' <param name="bytes">
        ''' A buffer containing the data to be written to the file or device.
        ''' </param>
        ''' <param name="numBytesToWrite">
        ''' The number of bytes to be written to the file or device.
        ''' </param>
        ''' <param name="numBytesWritten">
        ''' The number of bytes written when using a synchronous IO.
        ''' </param>
        ''' <param name="overlapped">
        ''' A pointer to an OVERLAPPED structure is required if the file was
        ''' opened with FILE_FLAG_OVERLAPPED.
        ''' </param>
        ''' <returns>
        ''' If the function succeeds, the return value is true. If the function
        ''' fails, or is completing asynchronously, the return value is false.
        ''' </returns>
        <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function WriteFile(ByVal handle As SafePipeHandle, _
            ByVal bytes As Byte(), _
            ByVal numBytesToWrite As Integer, _
            ByRef numBytesWritten As Integer, _
            ByVal overlapped As IntPtr) _
            As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function


        ''' <summary>
        ''' Flushes the buffers of the specified file and causes all buffered
        ''' data to be written to the file.
        ''' </summary>
        ''' <param name="handle">A handle to the open file. </param>
        ''' <returns>
        ''' If the function succeeds, the return value is true.
        ''' </returns>
        <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function FlushFileBuffers(ByVal handle As SafePipeHandle) _
            As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function


        ''' <summary>
        ''' Disconnects the server end of a named pipe instance from a client
        ''' process.
        ''' </summary>
        ''' <param name="hNamedPipe">Handle to a named pipe instance.</param>
        ''' <returns>
        ''' If the function succeeds, the return value is true.
        ''' </returns>
        <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function DisconnectNamedPipe(ByVal hNamedPipe As SafePipeHandle) _
            As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function


        ''' <summary>
        ''' The ConvertStringSecurityDescriptorToSecurityDescriptor function
        ''' converts a string-format security descriptor into a valid,
        ''' functional security descriptor.
        ''' </summary>
        ''' <param name="sddlSecurityDescriptor">
        ''' A string containing the string-format security descriptor (SDDL)
        ''' to convert.
        ''' </param>
        ''' <param name="sddlRevision">
        ''' The revision level of the sddlSecurityDescriptor string.
        ''' Currently this value must be 1.
        ''' </param>
        ''' <param name="pSecurityDescriptor">
        ''' A pointer to a variable that receives a pointer to the converted
        ''' security descriptor.
        ''' </param>
        ''' <param name="securityDescriptorSize">
        ''' A pointer to a variable that receives the size, in bytes, of the
        ''' converted security descriptor. This parameter can be IntPtr.Zero.
        ''' </param>
        ''' <returns>
        ''' If the function succeeds, the return value is true.
        ''' </returns>
        <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function ConvertStringSecurityDescriptorToSecurityDescriptor( _
            ByVal sddlSecurityDescriptor As String, _
            ByVal sddlRevision As Integer, _
            ByRef pSecurityDescriptor As SafeLocalMemHandle, _
            ByVal securityDescriptorSize As IntPtr) _
            As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function

    End Class

#End Region

End Class
