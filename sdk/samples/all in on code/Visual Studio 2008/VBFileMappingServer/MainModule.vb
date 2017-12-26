'******************************* Module Header *********************************'
' Module Name:  MainModule.vb
' Project:      VBFileMappingServer
' Copyright (c) Microsoft Corporation.
'
' File mapping is a mechanism for one-way or duplex inter-process communication 
' among two or more processes in the local machine. To share a file or memory, 
' all of the processes must use the name or the handle of the same file mapping 
' object.
' 
' To share a file, the first process creates or opens a file by using the 
' CreateFile function. Next, it creates a file mapping object by using the 
' CreateFileMapping function, specifying the file handle and a name for the file
' mapping object. The names of event, semaphore, mutex, waitable timer, job, and 
' file mapping objects share the same name space. Therefore the CreateFileMapping
' and OpenFileMapping functions fail if they specify a name that is in use by an 
' object of another type.
' 
' To share memory that is not associated with a file, a process must use the 
' CreateFileMapping function and specify INVALID_HANDLE_VALUE as the hFile 
' parameter instead of an existing file handle. The corresponding file mapping 
' object accesses memory backed by the system paging file. You must specify a 
' size greater than zero when you use an hFile of INVALID_HANDLE_VALUE in a call 
' to CreateFileMapping.
' 
' Processes that share files or memory must create file views by using the 
' MapViewOfFile or MapViewOfFileEx function. They must coordinate their access 
' using semaphores, mutexes, events, or some other mutual exclusion technique.
' 
' The VB.NET code sample demonstrates creating a file mapping object named 
' "Local\SampleMap" and writing a string to the file mapping. Because the Base 
' Class Library of .NET Framework 2/3/3.5 does not have any public classes to 
' operate on file mapping objects, you have to P/Invoke the Windows APIs as shown
' in this code sample.
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
Imports System.Security
Imports System.Security.Permissions
Imports Microsoft.Win32.SafeHandles
Imports System.Runtime.ConstrainedExecution
Imports System.Runtime.InteropServices
Imports System.ComponentModel

#End Region


Module MainModule

    ' In terminal services: The name can have a "Global\" or "Local\" prefix to
    ' explicitly create the object in the global or session namespace. The 
    ' remainder of the name can contain any character except the backslash 
    ' character (\). For more information, see: 
    ' http://msdn.microsoft.com/en-us/library/aa366537.aspx
    Friend Const MapPrefix As String = "Local\"
    Friend Const MapName As String = "SampleMap"
    Friend Const FullMapName As String = MapPrefix & MapName

    ' Max size of the file mapping object.
    Friend Const MapSize As UInt32 = &H10000

    ' File offset where the view is to begin.
    Friend Const ViewOffset As UInt32 = 0

    ' The number of bytes of a file mapping to map to the view. All bytes of the
    ' view must be within the maximum size of the file mapping object (MAP_SIZE). 
    ' If VIEW_SIZE is 0, the mapping extends from the offset (VIEW_OFFSET) to 
    ' the end of the file mapping.
    Friend Const ViewSize As UInt32 = &H400

    ' Unicode string message to be written to the mapped view. Its size in byte 
    ' must be less than the view size (VIEW_SIZE). 
    Friend Const Message As String = "Message from the first process."


    Sub Main()
        Dim hMapFile As SafeFileMappingHandle = Nothing
        Dim pView As IntPtr = IntPtr.Zero

        Try
            ' Create the file mapping object.
            hMapFile = NativeMethod.CreateFileMapping( _
                INVALID_HANDLE_VALUE, _
                IntPtr.Zero, _
                FileProtection.PAGE_READWRITE, _
                0, _
                MapSize, _
                FullMapName)

            If (hMapFile.IsInvalid) Then
                Throw New Win32Exception
            End If

            Console.WriteLine("The file mapping ({0}) is created", FullMapName)

            ' Map a view of the file mapping into the address space of the 
            ' current process.
            pView = NativeMethod.MapViewOfFile( _
                hMapFile, _
                FileMapAccess.FILE_MAP_ALL_ACCESS, _
                0, _
                ViewOffset, _
                ViewSize)

            If (pView = IntPtr.Zero) Then
                Throw New Win32Exception
            End If

            Console.WriteLine("The file view is mapped")

            ' Prepare a message to be written to the view. Append '\0' to mark 
            ' the end of the string when it is marshaled to the native memory.
            Dim bMessage As Byte() = Encoding.Unicode.GetBytes( _
                "Message from the first process." & ControlChars.NullChar)

            ' Write the message to the view.
            Marshal.Copy(bMessage, 0, pView, bMessage.Length)

            Console.WriteLine("This message is written to the view:")
            Console.WriteLine("""{0}""", Message)

            ' Wait to clean up resources and stop the process.
            Console.Write("Press ENTER to clean up resources and quit")
            Console.ReadLine()

        Catch ex As Exception
            Console.WriteLine("The process throws the error: {0}", ex.Message)
        Finally
            If (Not hMapFile Is Nothing) Then
                If (pView <> IntPtr.Zero) Then
                    ' Unmap the file view.
                    NativeMethod.UnmapViewOfFile(pView)
                    pView = IntPtr.Zero
                End If
                ' Close the file mapping object.
                hMapFile.Close()
                hMapFile = Nothing
            End If
        End Try
    End Sub


#Region "Native API Signatures and Types"

    ''' <summary>
    ''' Memory Protection Constants
    ''' http://msdn.microsoft.com/en-us/library/aa366786.aspx
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum FileProtection As UInt32
        PAGE_NOACCESS = &H1
        PAGE_READONLY = &H2
        PAGE_READWRITE = &H4
        PAGE_WRITECOPY = &H8
        PAGE_EXECUTE = &H10
        PAGE_EXECUTE_READ = &H20
        PAGE_EXECUTE_READWRITE = &H40
        PAGE_EXECUTE_WRITECOPY = &H80
        PAGE_GUARD = &H100
        PAGE_NOCACHE = &H200
        PAGE_WRITECOMBINE = &H400
        SEC_FILE = &H800000
        SEC_IMAGE = &H1000000
        SEC_RESERVE = &H4000000
        SEC_COMMIT = &H8000000
        SEC_NOCACHE = &H10000000
    End Enum


    ''' <summary>
    ''' Access rights for file mapping objects
    ''' http://msdn.microsoft.com/en-us/library/aa366559.aspx
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum FileMapAccess
        FILE_MAP_COPY = 1
        FILE_MAP_WRITE = 2
        FILE_MAP_READ = 4
        FILE_MAP_ALL_ACCESS = &HF001F
    End Enum


    ''' <summary>
    ''' Represents a wrapper class for a file mapping handle. 
    ''' </summary>
    ''' <remarks></remarks>
    <SuppressUnmanagedCodeSecurity(), _
    HostProtection(SecurityAction.LinkDemand, MayLeakOnAbort:=True)> _
    Friend NotInheritable Class SafeFileMappingHandle
        Inherits SafeHandleZeroOrMinusOneIsInvalid

        <SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode:=True)> _
        Private Sub New()
            MyBase.New(True)
        End Sub

        <SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode:=True)> _
        Public Sub New(ByVal handle As IntPtr, ByVal ownsHandle As Boolean)
            MyBase.New(ownsHandle)
            MyBase.SetHandle(handle)
        End Sub

        <ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success), _
        DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Private Shared Function CloseHandle(ByVal handle As IntPtr) _
        As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function

        Protected Overrides Function ReleaseHandle() As Boolean
            Return SafeFileMappingHandle.CloseHandle(MyBase.handle)
        End Function

    End Class


    Friend ReadOnly INVALID_HANDLE_VALUE As New IntPtr(-1)


    ''' <summary>
    ''' The class exposes Windows APIs used in this code sample.
    ''' </summary>
    ''' <remarks></remarks>
    <SuppressUnmanagedCodeSecurity()> _
    Friend Class NativeMethod

        ''' <summary>
        ''' Creates or opens a named or unnamed file mapping object for a 
        ''' specified file.
        ''' </summary>
        ''' <param name="hFile">
        ''' A handle to the file from which to create a file mapping object. 
        ''' </param>
        ''' <param name="lpAttributes">
        ''' A pointer to a SECURITY_ATTRIBUTES structure that determines whether
        ''' a returned handle can be inherited by child processes.
        ''' </param>
        ''' <param name="flProtect">
        ''' Specifies the page protection of the file mapping object. All mapped 
        ''' views of the object must be compatible with this protection.
        ''' </param>
        ''' <param name="dwMaximumSizeHigh">
        ''' The high-order DWORD of the maximum size of the file mapping object.
        ''' </param>
        ''' <param name="dwMaximumSizeLow">
        ''' The low-order DWORD of the maximum size of the file mapping object.
        ''' </param>
        ''' <param name="lpName">
        ''' The name of the file mapping object.
        ''' </param>
        ''' <returns>
        ''' If the function succeeds, the return value is a handle to the newly 
        ''' created file mapping object.
        ''' </returns>
        <DllImport("Kernel32.dll", SetLastError:=True)> _
        Public Shared Function CreateFileMapping( _
            ByVal hFile As IntPtr, _
            ByVal lpAttributes As IntPtr, _
            ByVal flProtect As FileProtection, _
            ByVal dwMaximumSizeHigh As UInt32, _
            ByVal dwMaximumSizeLow As UInt32, _
            ByVal lpName As String) _
            As SafeFileMappingHandle
        End Function


        ''' <summary>
        ''' Maps a view of a file mapping into the address space of a calling 
        ''' process.
        ''' </summary>
        ''' <param name="hFileMappingObject">
        ''' A handle to a file mapping object. The CreateFileMapping and 
        ''' OpenFileMapping functions return this handle.
        ''' </param>
        ''' <param name="dwDesiredAccess">
        ''' The type of access to a file mapping object, which determines the 
        ''' protection of the pages.
        ''' </param>
        ''' <param name="dwFileOffsetHigh">
        ''' A high-order DWORD of the file offset where the view begins.
        ''' </param>
        ''' <param name="dwFileOffsetLow">
        ''' A low-order DWORD of the file offset where the view is to begin.
        ''' </param>
        ''' <param name="dwNumberOfBytesToMap">
        ''' The number of bytes of a file mapping to map to the view. All bytes 
        ''' must be within the maximum size specified by CreateFileMapping.
        ''' </param>
        ''' <returns>
        ''' If the function succeeds, the return value is the starting address of
        ''' the mapped view.
        ''' </returns>
        <DllImport("Kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function MapViewOfFile( _
            ByVal hFileMappingObject As SafeFileMappingHandle, _
            ByVal dwDesiredAccess As FileMapAccess, _
            ByVal dwFileOffsetHigh As UInt32, _
            ByVal dwFileOffsetLow As UInt32, _
            ByVal dwNumberOfBytesToMap As UInt32) _
            As IntPtr
        End Function


        ''' <summary>
        ''' Unmaps a mapped view of a file from the calling process's address 
        ''' space.
        ''' </summary>
        ''' <param name="lpBaseAddress">
        ''' A pointer to the base address of the mapped view of a file that is to 
        ''' be unmapped.
        ''' </param>
        ''' <returns></returns>
        <DllImport("Kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function UnmapViewOfFile( _
            ByVal lpBaseAddress As IntPtr) _
            As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function

    End Class

#End Region

End Module