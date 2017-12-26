'/************************************* Module Header **************************************\
'* Module Name:  NativeMethods.vb
'* Project:      VBListFilesInDirectory
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBListFilesInDirectory project demonstrates how to implement an IEnumerable(Of String)
'* that utilizes the Win32 File Management functions to enable application to get files and
'* sub-directories in a specified directory one item a time.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 30/8/2009 1:00 PM Jie Wang Created
'\******************************************************************************************/


#Region "Using directives"
Imports System
Imports System.Runtime.InteropServices
Imports System.Runtime.ConstrainedExecution
Imports System.Security.Permissions
Imports Microsoft.Win32.SafeHandles
#End Region


<Serializable(), StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Auto), _
BestFitMapping(False)> _
Friend Class WIN32_FIND_DATA
    Friend dwFileAttributes As Integer
    Friend ftCreationTime_dwLowDateTime As Integer
    Friend ftCreationTime_dwHighDateTime As Integer
    Friend ftLastAccessTime_dwLowDateTime As Integer
    Friend ftLastAccessTime_dwHighDateTime As Integer
    Friend ftLastWriteTime_dwLowDateTime As Integer
    Friend ftLastWriteTime_dwHighDateTime As Integer
    Friend nFileSizeHigh As Integer
    Friend nFileSizeLow As Integer
    Friend dwReserved0 As Integer
    Friend dwReserved1 As Integer
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=260)> _
    Friend cFileName As String
    <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=14)> _
    Friend cAlternateFileName As String
End Class


''' <summary>
''' Win32 Native P/Invoke
''' </summary>
Friend Module NativeMethods
    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Friend Function FindFirstFile( _
        ByVal fileName As String, _
        <[In](), Out()> ByVal data As WIN32_FIND_DATA) As SafeFindHandle
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Friend Function FindNextFile( _
        ByVal hndFindFile As SafeFindHandle, _
        <[In](), Out(), MarshalAs(UnmanagedType.LPStruct)> ByVal _
         lpFindFileData As WIN32_FIND_DATA) As Boolean
    End Function

    <ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success), DllImport("kernel32.dll")> _
    Friend Function FindClose(ByVal handle As IntPtr) As Boolean
    End Function

    Friend Const ERROR_SUCCESS As Integer = 0
    Friend Const ERROR_NO_MORE_FILES As Integer = 18
    Friend Const ERROR_FILE_NOT_FOUND As Integer = 2
    Friend Const FILE_ATTRIBUTE_DIRECTORY As Integer = &H10
End Module


''' <summary>
''' Safe handle for using with the Find File APIs.
''' </summary>
Friend NotInheritable Class SafeFindHandle
    Inherits SafeHandleZeroOrMinusOneIsInvalid

    <SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode:=True)> _
    Friend Sub New()
        MyBase.New(True)
    End Sub

    Protected Overrides Function ReleaseHandle() As Boolean
        ' Close the search handle.
        Return NativeMethods.FindClose(MyBase.handle)
    End Function
End Class