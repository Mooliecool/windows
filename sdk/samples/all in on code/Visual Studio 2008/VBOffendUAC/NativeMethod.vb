'****************************** Module Header ******************************'
' Module Name:	NativeMethod.vb
' Project:		VBOffendUAC
' Copyright (c) Microsoft Corporation.
' 
' The P/Invoke signatures of some native APIs.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 8/27/2009 10:12 PM Jialiang Ge Created
'***************************************************************************'

#Region "Imports directives"

Imports System.Runtime.InteropServices
Imports System.Text

#End Region


<StructLayout(LayoutKind.Sequential)> _
Friend Structure LUID
    Public LowPart As UInt32
    Public HighPart As UInt32
End Structure


<StructLayout(LayoutKind.Sequential)> _
Friend Structure LUID_AND_ATTRIBUTES
    Public Luid As LUID
    Public Attributes As UInt32
End Structure


<StructLayout(LayoutKind.Sequential)> _
Friend Structure TOKEN_PRIVILEGES
    Public PrivilegeCount As UInt32
    <MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
    Public Privileges() As LUID_AND_ATTRIBUTES
End Structure


''' <summary>
''' Native methods
''' </summary>
''' <remarks></remarks>
Friend Class NativeMethod

    ''' <summary>
    ''' Tests whether the current user is a member of the Administrator's 
    ''' group.
    ''' </summary>
    ''' <returns>
    ''' Returns TRUE if the user is a member of the Administrator's group; 
    ''' otherwise, FALSE.
    ''' </returns>
    <DllImport("shell32.dll", CharSet:=CharSet.Auto)> _
    Friend Shared Function IsUserAnAdmin() As Boolean
    End Function


    Friend Const ERROR_NOT_ALL_ASSIGNED As Integer = 1300

    Friend Const TOKEN_QUERY As Integer = &H8
    Friend Const TOKEN_ADJUST_PRIVILEGES As Integer = &H20
    Friend Const SE_PRIVILEGE_ENABLED As Integer = &H2


    ''' <summary>
    ''' The LookupPrivilegeValue function retrieves the locally unique 
    ''' identifier (LUID) used on a specified system to locally represent 
    ''' the specified privilege name.
    ''' </summary>
    ''' <param name="lpSystemName">
    ''' A pointer to a null-terminated string that specifies the name of 
    ''' the system on which the privilege name is retrieved. 
    ''' </param>
    ''' <param name="lpName">
    ''' A pointer to a null-terminated string that specifies the name of 
    ''' the privilege, as defined in the Winnt.h header file. 
    ''' </param>
    ''' <param name="lpLuid">
    ''' A pointer to a variable that receives the LUID by which the privilege 
    ''' is known on the system specified by the lpSystemName parameter.
    ''' </param>
    <DllImport("advapi32.dll", SetLastError:=True)> _
    Friend Shared Function LookupPrivilegeValue( _
    ByVal lpSystemName As String, ByVal lpName As String, _
    ByRef lpLuid As LUID) As Boolean
    End Function


    ''' <summary>
    ''' The AdjustTokenPrivileges function enables or disables privileges in 
    ''' the specified access token. Enabling or disabling privileges in an 
    ''' access token requires TOKEN_ADJUST_PRIVILEGES access.
    ''' </summary>
    ''' <param name="TokenHandle">
    ''' A handle to the access token that contains the privileges to be 
    ''' modified.
    ''' </param>
    ''' <param name="DisableAllPrivileges">
    ''' Specifies whether the function disables all of the token's privileges.
    ''' </param>
    ''' <param name="NewState">
    ''' A pointer to a TOKEN_PRIVILEGES structure that specifies an array of 
    ''' privileges and their attributes.
    ''' </param>
    ''' <param name="BufferLength">
    ''' Specifies the size, in bytes, of the buffer pointed to by the 
    ''' PreviousState parameter.
    ''' </param>
    ''' <param name="PreviousState">
    ''' A pointer to a buffer that the function fills with a TOKEN_PRIVILEGES 
    ''' structure that contains the previous state of any privileges that the 
    ''' function modifies. 
    ''' </param>
    ''' <param name="ReturnLength">
    ''' A pointer to a variable that receives the required size, in bytes, of 
    ''' the buffer pointed to by the PreviousState parameter. 
    ''' </param>
    <DllImport("advapi32.dll", SetLastError:=True)> _
    Friend Shared Function AdjustTokenPrivileges( _
    ByVal TokenHandle As IntPtr, ByVal DisableAllPrivileges As Boolean, _
    ByRef NewState As TOKEN_PRIVILEGES, ByVal BufferLength As Integer, _
    ByRef PreviousState As TOKEN_PRIVILEGES, ByRef ReturnLength As IntPtr) _
    As Boolean
    End Function


    ''' <summary>
    ''' The OpenProcessToken function opens the access token associated with 
    ''' a process.
    ''' </summary>
    ''' <param name="ProcessHandle">
    ''' A handle to the process whose access token is opened.
    ''' </param>
    ''' <param name="DesiredAccess">
    ''' Specifies an access mask that specifies the requested types of access 
    ''' to the access token.
    ''' </param>
    ''' <param name="TokenHandle">
    ''' A pointer to a handle that identifies the newly opened access token 
    ''' when the function returns.
    ''' </param>
    <DllImport("advapi32.dll", SetLastError:=True)> _
    Friend Shared Function OpenProcessToken( _
    ByVal ProcessHandle As IntPtr, ByVal DesiredAccess As Integer, _
    ByRef TokenHandle As IntPtr) As Boolean
    End Function


    ''' <summary>
    ''' Closes an open object handle.
    ''' </summary>
    ''' <param name="hHandle">A valid handle to an open object.</param>
    <DllImport("kernel32.dll", SetLastError:=True)> _
    Friend Shared Function CloseHandle(ByVal hHandle As IntPtr) As Boolean
    End Function


    ''' <summary>
    ''' Copies a string into the specified section of an initialization file.
    ''' </summary>
    ''' <param name="lpAppName">
    ''' The name of the section to which the string will be copied.
    ''' </param>
    ''' <param name="lpKeyName">
    ''' The name of the key to be associated with a string.
    ''' </param>
    ''' <param name="lpString">
    ''' A null-terminated string to be written to the file.
    ''' </param>
    ''' <param name="lpFileName">The name of the initialization file.</param>
    <DllImport("kernel32.dll", SetLastError:=True)> _
    Friend Shared Function WritePrivateProfileString( _
    ByVal lpAppName As String, ByVal lpKeyName As String, _
    ByVal lpString As String, ByVal lpFileName As String) As Boolean
    End Function


    ''' <summary>
    ''' Retrieves a string from the specified section in an initialization 
    ''' file.
    ''' </summary>
    ''' <param name="lpAppName">
    ''' The name of the section containing the key name.
    ''' </param>
    ''' <param name="lpKeyName">
    ''' The name of the key whose associated string is to be retrieved.
    ''' </param>
    ''' <param name="lpDefault">A default string.</param>
    ''' <param name="lpReturnedString">
    ''' A pointer to the buffer that receives the retrieved string.
    ''' </param>
    ''' <param name="nSize">
    ''' The size of the buffer pointed to by lpReturnedString
    ''' </param>
    ''' <param name="lpFileName">The name of the initialization file.</param>
    <DllImport("kernel32.dll", SetLastError:=True)> _
    Friend Shared Function GetPrivateProfileString( _
    ByVal lpAppName As String, ByVal lpKeyName As String, _
    ByVal lpDefault As String, ByVal lpReturnedString As StringBuilder, _
    ByVal nSize As Integer, ByVal lpFileName As String) As Integer
    End Function


    ''' <summary>
    ''' Copies a string into the specified section of the Win.ini file. If 
    ''' Win.ini uses Unicode characters, the function writes Unicode 
    ''' characters to the file. Otherwise, the function writes ANSI 
    ''' characters.
    ''' </summary>
    ''' <param name="lpAppName">
    ''' The section to which the string is to be copied.
    ''' </param>
    ''' <param name="lpKeyName">
    ''' The key to be associated with the string.
    ''' </param>
    ''' <param name="lpString">
    ''' A null-terminated string to be written to the file.
    ''' </param>
    <DllImport("kernel32.dll", SetLastError:=True)> _
    Friend Shared Function WriteProfileString( _
    ByVal lpAppName As String, ByVal lpKeyName As String, _
    ByVal lpString As String) As Boolean
    End Function


    ''' <summary>
    ''' Retrieves the string associated with a key in the specified section 
    ''' of the Win.ini file.
    ''' </summary>
    ''' <param name="lpAppName">
    ''' The name of the section containing the key.
    ''' </param>
    ''' <param name="lpKeyName">
    ''' The name of the key whose associated string is to be retrieved.
    ''' </param>
    ''' <param name="lpDefault">A default string.</param>
    ''' <param name="lpReturnedString">
    ''' A pointer to a buffer that receives the character string.
    ''' </param>
    ''' <param name="nSize">
    ''' The size of the buffer pointed to by lpReturnedString.
    ''' </param>
    <DllImport("kernel32.dll", SetLastError:=True)> _
    Friend Shared Function GetProfileString( _
    ByVal lpAppName As String, ByVal lpKeyName As String, _
    ByVal lpDefault As String, ByVal lpReturnedString As StringBuilder, _
    ByVal nSize As Integer) As Integer
    End Function

End Class
