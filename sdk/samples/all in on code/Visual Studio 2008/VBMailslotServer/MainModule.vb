'******************************* Module Header *********************************'
' Module Name:  MainModule.vb
' Project:      VBMailslotServer
' Copyright (c) Microsoft Corporation.
'
' Mailslot is a mechanism for one-way inter-process communication in the local 
' machine or across the computers in the intranet. Any clients can store messages 
' in a mailslot. The creator of the slot, i.e. the server, retrieves the messages 
' that are stored there:
' 
' Client (GENERIC_WRITE) ---> Server (GENERIC_READ)
' 
' This code sample demonstrates calling CreateMailslot to create a mailslot named 
' "\\.\mailslot\SampleMailslot". The security attributes of the slot are 
' customized to allow Authenticated Users read and write access to the slot, and 
' to allow the Administrators group full access to it. The sample first creates 
' such a mailslot, then it reads and displays new messages in the slot when user 
' presses ENTER in the console.
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

Imports System.Runtime.InteropServices
Imports System.Security
Imports System.Runtime.ConstrainedExecution
Imports Microsoft.Win32.SafeHandles
Imports System.Security.Permissions
Imports System.ComponentModel
Imports System.Text

#End Region


Module MainModule

    ' The full name of the mailslot is in the format of \\.\mailslot\[path]name
    ' The name field must be unique. The name may include multiple levels of 
    ' pseudo directories separated by backslashes. For example, both 
    ' \\.\mailslot\mailslot_name and \\.\mailslot\abc\def\ghi are valid.
    Friend Const MailslotName As String = "\\.\mailslot\SampleMailslot"


    Sub Main()
        Dim hMailslot As SafeMailslotHandle = Nothing

        Try
            ' Prepare the security attributes (the lpSecurityAttributes parameter 
            ' in CreateMailslot) for the mailslot. This is optional. If the 
            ' lpSecurityAttributes parameter of CreateMailslot is NULL, the 
            ' mailslot gets a default security descriptor and the handle cannot 
            ' be inherited. The ACLs in the default security descriptor of a 
            ' mailslot grant full control to the LocalSystem account, (elevated) 
            ' administrators, and the creator owner. They also give only read 
            ' access to members of the Everyone group and the anonymous account. 
            ' However, if you want to customize the security permission of the 
            ' mailslot, (e.g. to allow Authenticated Users to read from and 
            ' write to the mailslot), you need to create a SECURITY_ATTRIBUTES 
            ' structure.
            Dim sa As SECURITY_ATTRIBUTES = Nothing
            sa = CreateMailslotSecurity()

            ' Create the mailslot.
            hMailslot = NativeMethod.CreateMailslot(MailslotName, 0, _
                MAILSLOT_WAIT_FOREVER, sa)

            If (hMailslot.IsInvalid) Then
                Throw New Win32Exception
            End If

            Console.WriteLine("The mailslot ({0}) is created.", MailslotName)

            ' Check messages in the mailslot.
            Console.Write("Press ENTER to check new messages or press Q to quit ...")
            Dim cmd As String = Console.ReadLine()
            While (Not cmd.Equals("Q", StringComparison.OrdinalIgnoreCase))
                Console.WriteLine("Checking new messages...")
                ReadMailslot(hMailslot)

                Console.Write("Press ENTER to check new messages or press Q to quit ...")
                cmd = Console.ReadLine()
            End While

        Catch ex As Win32Exception
            Console.WriteLine("The server throws the error: {0}", ex.Message)
        Finally
            If (hMailslot IsNot Nothing) Then
                hMailslot.Close()
                hMailslot = Nothing
            End If
        End Try
    End Sub


    ''' <summary>
    ''' The CreateMailslotSecurity function creates and initializes a new 
    ''' SECURITY_ATTRIBUTES object to allow Authenticated Users read and write 
    ''' access to a mailslot, and to allow the Administrators group full access
    ''' to the mailslot.
    ''' </summary>
    ''' <returns>
    ''' A SECURITY_ATTRIBUTES object that allows Authenticated Users read and 
    ''' write access to a mailslot, and allows the Administrators group full 
    ''' access to the mailslot.
    ''' </returns>
    Function CreateMailslotSecurity() As SECURITY_ATTRIBUTES
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


    ''' <summary>
    ''' Read the messages from a mailslot by using the mailslot handle in a call 
    ''' to the ReadFile function. 
    ''' </summary>
    ''' <param name="hMailslot">The handle of the mailslot</param>
    ''' <returns>
    ''' If the function succeeds, the return value is true.
    ''' </returns>
    Function ReadMailslot(ByVal hMailslot As SafeMailslotHandle) As Boolean
        Dim cbMessageBytes As Integer = 0   ' Size of the message in bytes
        Dim cbBytesRead As Integer = 0      ' Number of bytes read from the mailslot
        Dim cMessages As Integer = 0        ' Number of messages in the slot
        Dim nMessageId As Integer = 0       ' Message ID

        Dim succeeded As Boolean = False

        ' Check for the number of messages in the mailslot.
        succeeded = NativeMethod.GetMailslotInfo(hMailslot, IntPtr.Zero, _
            cbMessageBytes, cMessages, IntPtr.Zero)
        If (Not succeeded) Then
            Console.WriteLine("GetMailslotInfo failed w/err 0x{0:X}", _
                Marshal.GetLastWin32Error())
            Return succeeded
        End If

        If (cbMessageBytes = MAILSLOT_NO_MESSAGE) Then
            ' There are no new messages in the mailslot at present
            Console.WriteLine("No new messages.")
            Return succeeded
        End If

        ' Retrieve the messages one by one from the mailslot.
        While (cMessages <> 0)
            nMessageId += 1

            ' Declare a byte array to fetch the data
            Dim bBuffer As Byte() = New Byte(cbMessageBytes - 1) {}
            succeeded = NativeMethod.ReadFile(hMailslot, bBuffer, cbMessageBytes, _
                cbBytesRead, IntPtr.Zero)
            If (Not succeeded) Then
                Console.WriteLine("ReadFile failed w/err 0x{0:X}", _
                    Marshal.GetLastWin32Error())
                Exit While
            End If

            ' Display the message. 
            Console.WriteLine("Message #{0}: {1}", nMessageId, _
                Encoding.Unicode.GetString(bBuffer))

            ' Get the current number of un-read messages in the slot. The number
            ' may not equal the initial message number because new messages may 
            ' arrive while we are reading the items in the slot.
            succeeded = NativeMethod.GetMailslotInfo(hMailslot, IntPtr.Zero, _
                cbMessageBytes, cMessages, IntPtr.Zero)
            If (Not succeeded) Then
                Console.WriteLine("GetMailslotInfo failed w/err 0x{0:X}", _
                    Marshal.GetLastWin32Error())
                Exit While
            End If
        End While

        Return succeeded
    End Function


#Region "Native API Signatures and Types"

    ''' <summary>
    ''' Mailslot waits forever for a message.
    ''' </summary>
    Friend Const MAILSLOT_WAIT_FOREVER As Integer = -1

    ''' <summary>
    ''' There is no next message.
    ''' </summary>
    Friend Const MAILSLOT_NO_MESSAGE As Integer = -1


    ''' <summary>
    ''' Represents a wrapper class for a mailslot handle. 
    ''' </summary>
    <SecurityCritical(SecurityCriticalScope.Everything), _
    HostProtection(SecurityAction.LinkDemand, MayLeakOnAbort:=True), _
    SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode:=True)> _
    Friend NotInheritable Class SafeMailslotHandle
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
        ''' Creates an instance of a mailslot and returns a handle for subsequent 
        ''' operations.
        ''' </summary>
        ''' <param name="mailslotName">Mailslot name</param>
        ''' <param name="nMaxMessageSize">
        ''' The maximum size of a single message
        ''' </param>
        ''' <param name="lReadTimeout">
        ''' The time a read operation can wait for a message.
        ''' </param>
        ''' <param name="securityAttributes">Security attributes</param>
        ''' <returns>
        ''' If the function succeeds, the return value is a handle to the server 
        ''' end of a mailslot instance.
        ''' </returns>
        <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function CreateMailslot( _
            ByVal mailslotName As String, _
            ByVal nMaxMessageSize As UInt32, _
            ByVal lReadTimeout As Integer, _
            ByVal securityAttributes As SECURITY_ATTRIBUTES) _
            As SafeMailslotHandle
        End Function


        ''' <summary>
        ''' Retrieves information about the specified mailslot.
        ''' </summary>
        ''' <param name="hMailslot">A handle to a mailslot</param>
        ''' <param name="lpMaxMessageSize">
        ''' The maximum message size, in bytes, allowed for this mailslot.
        ''' </param>
        ''' <param name="lpNextSize">
        ''' The size of the next message in bytes.
        ''' </param>
        ''' <param name="lpMessageCount">
        ''' The total number of messages waiting to be read.
        ''' </param>
        ''' <param name="lpReadTimeout">
        ''' The amount of time, in milliseconds, a read operation can wait for a 
        ''' message to be written to the mailslot before a time-out occurs. 
        ''' </param>
        ''' <returns></returns>
        <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function GetMailslotInfo( _
            ByVal hMailslot As SafeMailslotHandle, _
            ByVal lpMaxMessageSize As IntPtr, _
            <Out()> ByRef lpNextSize As Integer, _
            <Out()> ByRef lpMessageCount As Integer, _
            ByVal lpReadTimeout As IntPtr) _
            As <MarshalAs(UnmanagedType.Bool)> Boolean
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
        Public Shared Function ReadFile( _
            ByVal handle As SafeMailslotHandle, _
            ByVal bytes As Byte(), _
            ByVal numBytesToRead As Integer, _
            ByRef numBytesRead As Integer, _
            ByVal overlapped As IntPtr) _
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

End Module
