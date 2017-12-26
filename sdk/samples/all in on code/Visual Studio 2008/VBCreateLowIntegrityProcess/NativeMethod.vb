'***************************** Module Header *******************************\
' Module Name:  NativeMethod.vb
' Project:      VBCreateLowIntegrityProcess
' Copyright (c) Microsoft Corporation.
' 
' The P/Invoke signature some native Windows APIs used by the code sample.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports directives"

Imports System.Runtime.InteropServices
Imports Microsoft.Win32.SafeHandles

#End Region


''' <summary>
''' The TOKEN_INFORMATION_CLASS enumeration type contains values that specify 
''' the type of information being assigned to or retrieved from an access 
''' token.
''' </summary>
Friend Enum TOKEN_INFORMATION_CLASS
    TokenUser = 1
    TokenGroups
    TokenPrivileges
    TokenOwner
    TokenPrimaryGroup
    TokenDefaultDacl
    TokenSource
    TokenType
    TokenImpersonationLevel
    TokenStatistics
    TokenRestrictedSids
    TokenSessionId
    TokenGroupsAndPrivileges
    TokenSessionReference
    TokenSandBoxInert
    TokenAuditPolicy
    TokenOrigin
    TokenElevationType
    TokenLinkedToken
    TokenElevation
    TokenHasRestrictions
    TokenAccessInformation
    TokenVirtualizationAllowed
    TokenVirtualizationEnabled
    TokenIntegrityLevel
    TokenUIAccess
    TokenMandatoryPolicy
    TokenLogonSid
    MaxTokenInfoClass
End Enum


''' <summary>
''' The SECURITY_IMPERSONATION_LEVEL enumeration type contains values 
''' that specify security impersonation levels. Security impersonation 
''' levels govern the degree to which a server process can act on behalf 
''' of a client process.
''' </summary>
Friend Enum SECURITY_IMPERSONATION_LEVEL
    SecurityAnonymous = 0
    SecurityIdentification
    SecurityImpersonation
    SecurityDelegation
End Enum


''' <summary>
''' The TOKEN_TYPE enumeration type contains values that differentiate 
''' between a primary token and an impersonation token. 
''' </summary>
''' <remarks></remarks>
Friend Enum TOKEN_TYPE
    TokenPrimary = 1
    TokenImpersonation
End Enum


''' <summary>
''' The structure represents a security identifier (SID) and its attributes.
''' SIDs are used to uniquely identify users or groups.
''' </summary>
<StructLayout(LayoutKind.Sequential)> _
Friend Structure SID_AND_ATTRIBUTES
    Public Sid As IntPtr
    Public Attributes As UInteger
End Structure


''' <summary>
''' The SID_IDENTIFIER_AUTHORITY structure represents the top-level authority
''' of a security identifier (SID).
''' </summary>
''' <remarks></remarks>
<StructLayout(LayoutKind.Sequential)> _
Friend Structure SID_IDENTIFIER_AUTHORITY
    <MarshalAs(UnmanagedType.ByValArray, SizeConst:=6, _
               ArraySubType:=UnmanagedType.I1)> _
    Public Value As Byte()

    Public Sub New(ByVal value As Byte())
        Me.Value = value
    End Sub
End Structure


''' <summary>
''' The structure specifies the mandatory integrity level for a token.
''' </summary>
<StructLayout(LayoutKind.Sequential)> _
Friend Structure TOKEN_MANDATORY_LABEL
    Public Label As SID_AND_ATTRIBUTES
End Structure


''' <summary>
''' Specifies the window station, desktop, standard handles, and 
''' appearance of the main window for a process at creation time.
''' </summary>
''' <remarks></remarks>
<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Unicode)> _
Friend Structure STARTUPINFO
    Public cb As Integer
    Public lpReserved As String
    Public lpDesktop As String
    Public lpTitle As String
    Public dwX As Integer
    Public dwY As Integer
    Public dwXSize As Integer
    Public dwYSize As Integer
    Public dwXCountChars As Integer
    Public dwYCountChars As Integer
    Public dwFillAttribute As Integer
    Public dwFlags As Integer
    Public wShowWindow As Short
    Public cbReserved2 As Short
    Public lpReserved2 As IntPtr
    Public hStdInput As IntPtr
    Public hStdOutput As IntPtr
    Public hStdError As IntPtr
End Structure


''' <summary>
''' Contains information about a newly created process and its primary 
''' thread.
''' </summary>
''' <remarks></remarks>
<StructLayout(LayoutKind.Sequential)> _
Friend Structure PROCESS_INFORMATION
    Public hProcess As IntPtr
    Public hThread As IntPtr
    Public dwProcessId As Integer
    Public dwThreadId As Integer
End Structure


''' <summary>
''' Represents a wrapper class for a token handle.
''' </summary>
Friend Class SafeTokenHandle
    Inherits SafeHandleZeroOrMinusOneIsInvalid

    Private Sub New()
        MyBase.New(True)
    End Sub

    Friend Sub New(ByVal handle As IntPtr)
        MyBase.New(True)
        MyBase.SetHandle(handle)
    End Sub

    Protected Overrides Function ReleaseHandle() As Boolean
        Return NativeMethod.CloseHandle(MyBase.handle)
    End Function

End Class


Friend Class NativeMethod

    ' Token Specific Access Rights

    Public Const STANDARD_RIGHTS_REQUIRED As UInt32 = &HF0000
    Public Const STANDARD_RIGHTS_READ As UInt32 = &H20000
    Public Const TOKEN_ASSIGN_PRIMARY As UInt32 = 1
    Public Const TOKEN_DUPLICATE As UInt32 = 2
    Public Const TOKEN_IMPERSONATE As UInt32 = 4
    Public Const TOKEN_QUERY As UInt32 = 8
    Public Const TOKEN_QUERY_SOURCE As UInt32 = &H10
    Public Const TOKEN_ADJUST_PRIVILEGES As UInt32 = &H20
    Public Const TOKEN_ADJUST_GROUPS As UInt32 = &H40
    Public Const TOKEN_ADJUST_DEFAULT As UInt32 = &H80
    Public Const TOKEN_ADJUST_SESSIONID As UInt32 = &H100
    Public Const TOKEN_READ As UInt32 = (STANDARD_RIGHTS_READ Or TOKEN_QUERY)
    Public Const TOKEN_ALL_ACCESS As UInt32 = ( _
    STANDARD_RIGHTS_REQUIRED Or TOKEN_ASSIGN_PRIMARY Or TOKEN_DUPLICATE Or _
    TOKEN_IMPERSONATE Or TOKEN_QUERY Or TOKEN_QUERY_SOURCE Or _
    TOKEN_ADJUST_PRIVILEGES Or TOKEN_ADJUST_GROUPS Or TOKEN_ADJUST_DEFAULT Or _
    TOKEN_ADJUST_SESSIONID)


    Public Const ERROR_INSUFFICIENT_BUFFER As Int32 = 122


    ' Integrity Levels

    Public Shared SECURITY_MANDATORY_LABEL_AUTHORITY As New  _
        SID_IDENTIFIER_AUTHORITY(New Byte() {0, 0, 0, 0, 0, 16})
    Public Const SECURITY_MANDATORY_UNTRUSTED_RID As Integer = 0
    Public Const SECURITY_MANDATORY_LOW_RID As Integer = &H1000
    Public Const SECURITY_MANDATORY_MEDIUM_RID As Integer = &H2000
    Public Const SECURITY_MANDATORY_HIGH_RID As Integer = &H3000
    Public Const SECURITY_MANDATORY_SYSTEM_RID As Integer = &H4000


    ' Group related SID Attributes

    Public Const SE_GROUP_MANDATORY As UInt32 = 1
    Public Const SE_GROUP_ENABLED_BY_DEFAULT As UInt32 = 2
    Public Const SE_GROUP_ENABLED As UInt32 = 4
    Public Const SE_GROUP_OWNER As UInt32 = 8
    Public Const SE_GROUP_USE_FOR_DENY_ONLY As UInt32 = &H10
    Public Const SE_GROUP_INTEGRITY As UInt32 = &H20
    Public Const SE_GROUP_INTEGRITY_ENABLED As UInt32 = &H40
    Public Const SE_GROUP_LOGON_ID As UInt32 = &HC0000000UI
    Public Const SE_GROUP_RESOURCE As UInt32 = &H20000000
    Public Const SE_GROUP_VALID_ATTRIBUTES As UInt32 = (SE_GROUP_MANDATORY Or _
        SE_GROUP_ENABLED_BY_DEFAULT Or SE_GROUP_ENABLED Or SE_GROUP_OWNER Or _
        SE_GROUP_USE_FOR_DENY_ONLY Or SE_GROUP_LOGON_ID Or SE_GROUP_RESOURCE Or _
        SE_GROUP_INTEGRITY Or SE_GROUP_INTEGRITY_ENABLED)


    ''' <summary>
    ''' The function opens the access token associated with a process.
    ''' </summary>
    ''' <param name="hProcess">
    ''' A handle to the process whose access token is opened.
    ''' </param>
    ''' <param name="desiredAccess">
    ''' Specifies an access mask that specifies the requested types of access 
    ''' to the access token. 
    ''' </param>
    ''' <param name="hToken">
    ''' Outputs a handle that identifies the newly opened access token 
    ''' when the function returns.
    ''' </param>
    ''' <returns></returns>
    <DllImport("advapi32", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function OpenProcessToken( _
        ByVal hProcess As IntPtr, _
        ByVal desiredAccess As UInt32, _
        <Out()> ByRef hToken As SafeTokenHandle) _
        As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function


    ''' <summary>
    ''' The DuplicateTokenEx function creates a new access token that 
    ''' duplicates an existing token. This function can create either a 
    ''' primary token or an impersonation token.
    ''' </summary>
    ''' <param name="hExistingToken">
    ''' A handle to an access token opened with TOKEN_DUPLICATE access.
    ''' </param>
    ''' <param name="desiredAccess">
    ''' Specifies the requested access rights for the new token.
    ''' </param>
    ''' <param name="pTokenAttributes">
    ''' A pointer to a SECURITY_ATTRIBUTES structure that specifies a 
    ''' security descriptor for the new token and determines whether 
    ''' child processes can inherit the token. If lpTokenAttributes is 
    ''' NULL, the token gets a default security descriptor and the handle 
    ''' cannot be inherited. 
    ''' </param>
    ''' <param name="ImpersonationLevel">
    ''' Specifies the impersonation level of the new token.
    ''' </param>
    ''' <param name="TokenType">
    ''' TokenPrimary - The new token is a primary token that you can use 
    ''' in the CreateProcessAsUser function.
    ''' TokenImpersonation - The new token is an impersonation token.
    ''' </param>
    ''' <param name="hNewToken">
    ''' Receives the new token.
    ''' </param>
    ''' <returns></returns>
    <DllImport("advapi32", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function DuplicateTokenEx( _
        ByVal hExistingToken As SafeTokenHandle, _
        ByVal desiredAccess As UInt32, _
        ByVal pTokenAttributes As IntPtr, _
        ByVal ImpersonationLevel As SECURITY_IMPERSONATION_LEVEL, _
        ByVal TokenType As TOKEN_TYPE, _
        <Out()> ByRef hNewToken As SafeTokenHandle) _
        As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function


    ''' <summary>
    ''' The function retrieves a specified type of information about an 
    ''' access token. The calling process must have appropriate access rights
    ''' to obtain the information.
    ''' </summary>
    ''' <param name="hToken">
    ''' A handle to an access token from which information is retrieved.
    ''' </param>
    ''' <param name="tokenInfoClass">
    ''' Specifies a value from the TOKEN_INFORMATION_CLASS enumerated type to 
    ''' identify the type of information the function retrieves.
    ''' </param>
    ''' <param name="pTokenInfo">
    ''' A pointer to a buffer the function fills with the requested 
    ''' information.
    ''' </param>
    ''' <param name="tokenInfoLength">
    ''' Specifies the size, in bytes, of the buffer pointed to by the 
    ''' TokenInformation parameter. 
    ''' </param>
    ''' <param name="returnLength">
    ''' A pointer to a variable that receives the number of bytes needed for 
    ''' the buffer pointed to by the TokenInformation parameter. 
    ''' </param>
    ''' <returns></returns>
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function GetTokenInformation( _
        ByVal hToken As SafeTokenHandle, _
        ByVal tokenInfoClass As TOKEN_INFORMATION_CLASS, _
        ByVal pTokenInfo As IntPtr, _
        ByVal tokenInfoLength As Integer, _
        <Out()> ByRef returnLength As Integer) _
        As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function


    ''' <summary>
    ''' The function sets various types of information for a specified 
    ''' access token. The information that this function sets replaces 
    ''' existing information. The calling process must have appropriate 
    ''' access rights to set the information.
    ''' </summary>
    ''' <param name="hToken">
    ''' A handle to the access token for which information is to be set.
    ''' </param>
    ''' <param name="tokenInfoClass">
    ''' A value from the TOKEN_INFORMATION_CLASS enumerated type that 
    ''' identifies the type of information the function sets. 
    ''' </param>
    ''' <param name="pTokenInfo">
    ''' A pointer to a buffer that contains the information set in the 
    ''' access token. 
    ''' </param>
    ''' <param name="tokenInfoLength">
    ''' Specifies the length, in bytes, of the buffer pointed to by 
    ''' TokenInformation.
    ''' </param>
    ''' <returns></returns>
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function SetTokenInformation( _
        ByVal hToken As SafeTokenHandle, _
        ByVal tokenInfoClass As TOKEN_INFORMATION_CLASS, _
        ByVal pTokenInfo As IntPtr, _
        ByVal tokenInfoLength As Integer) _
        As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function


    ''' <summary>
    ''' The function returns a pointer to a specified subauthority in a 
    ''' security identifier (SID). The subauthority value is a relative 
    ''' identifier (RID).
    ''' </summary>
    ''' <param name="pSid">
    ''' A pointer to the SID structure from which a pointer to a subauthority
    ''' is to be returned.
    ''' </param>
    ''' <param name="nSubAuthority">
    ''' Specifies an index value identifying the subauthority array element 
    ''' whose address the function will return.
    ''' </param>
    ''' <returns>
    ''' If the function succeeds, the return value is a pointer to the 
    ''' specified SID subauthority. To get extended error information, call
    ''' GetLastError. If the function fails, the return value is undefined.
    ''' The function fails if the specified SID structure is not valid or if 
    ''' the index value specified by the nSubAuthority parameter is out of
    ''' bounds. 
    ''' </returns>
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function GetSidSubAuthority( _
        ByVal pSid As IntPtr, _
        ByVal nSubAuthority As UInt32) _
        As IntPtr
    End Function


    ''' <summary>
    ''' The AllocateAndInitializeSid function allocates and initializes a 
    ''' security identifier (SID) with up to eight subauthorities.
    ''' </summary>
    ''' <param name="pIdentifierAuthority">
    ''' A reference of a SID_IDENTIFIER_AUTHORITY structure. This structure
    ''' provides the top-level identifier authority value to set in the SID.
    ''' </param>
    ''' <param name="nSubAuthorityCount">
    ''' Specifies the number of subauthorities to place in the SID. 
    ''' </param>
    ''' <param name="dwSubAuthority0">
    ''' Subauthority value to place in the SID.
    ''' </param>
    ''' <param name="dwSubAuthority1">
    ''' Subauthority value to place in the SID.
    ''' </param>
    ''' <param name="dwSubAuthority2">
    ''' Subauthority value to place in the SID.
    ''' </param>
    ''' <param name="dwSubAuthority3">
    ''' Subauthority value to place in the SID.
    ''' </param>
    ''' <param name="dwSubAuthority4">
    ''' Subauthority value to place in the SID.
    ''' </param>
    ''' <param name="dwSubAuthority5">
    ''' Subauthority value to place in the SID.
    ''' </param>
    ''' <param name="dwSubAuthority6">
    ''' Subauthority value to place in the SID.
    ''' </param>
    ''' <param name="dwSubAuthority7">
    ''' Subauthority value to place in the SID.
    ''' </param>
    ''' <param name="pSid">
    ''' Outputs the allocated and initialized SID structure.
    ''' </param>
    ''' <returns>
    ''' If the function succeeds, the return value is true. If the function
    ''' fails, the return value is false. To get extended error information, 
    ''' call GetLastError.
    ''' </returns>
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function AllocateAndInitializeSid( _
        ByRef pIdentifierAuthority As SID_IDENTIFIER_AUTHORITY, _
        ByVal nSubAuthorityCount As Byte, _
        ByVal dwSubAuthority0 As Integer, _
        ByVal dwSubAuthority1 As Integer, _
        ByVal dwSubAuthority2 As Integer, _
        ByVal dwSubAuthority3 As Integer, _
        ByVal dwSubAuthority4 As Integer, _
        ByVal dwSubAuthority5 As Integer, _
        ByVal dwSubAuthority6 As Integer, _
        ByVal dwSubAuthority7 As Integer, _
        <Out()> ByRef pSid As IntPtr) _
        As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function


    ''' <summary>
    ''' The FreeSid function frees a security identifier (SID) previously 
    ''' allocated by using the AllocateAndInitializeSid function.
    ''' </summary>
    ''' <param name="pSid">
    ''' A pointer to the SID structure to free.
    ''' </param>
    ''' <returns>
    ''' If the function succeeds, the function returns NULL. If the function 
    ''' fails, it returns a pointer to the SID structure represented by the 
    ''' pSid parameter.
    ''' </returns>
    <DllImport("advapi32.dll")> _
    Public Shared Function FreeSid(ByVal pSid As IntPtr) As IntPtr
    End Function


    ''' <summary>
    ''' The function returns the length, in bytes, of a valid security 
    ''' identifier (SID).
    ''' </summary>
    ''' <param name="pSID">
    ''' A pointer to the SID structure whose length is returned. 
    ''' </param>
    ''' <returns>
    ''' If the SID structure is valid, the return value is the length, in 
    ''' bytes, of the SID structure.
    ''' </returns>
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function GetLengthSid(ByVal pSID As IntPtr) As Integer
    End Function


    ''' <summary>
    ''' Creates a new process and its primary thread. The new process runs in 
    ''' the security context of the user represented by the specified token. 
    ''' </summary>
    ''' <param name="hToken">
    ''' A handle to the primary token that represents a user. 
    ''' </param>
    ''' <param name="applicationName">
    ''' The name of the module to be executed.
    ''' </param>
    ''' <param name="commandLine">
    ''' The command line to be executed. The maximum length of this string is
    ''' 32K characters. 
    ''' </param>
    ''' <param name="pProcessAttributes">
    ''' A pointer to a SECURITY_ATTRIBUTES structure that specifies a 
    ''' security descriptor for the new process object and determines whether
    ''' child processes can inherit the returned handle to the process.
    ''' </param>
    ''' <param name="pThreadAttributes">
    ''' A pointer to a SECURITY_ATTRIBUTES structure that specifies a 
    ''' security descriptor for the new thread object and determines whether
    ''' child processes can inherit the returned handle to the thread.
    ''' </param>
    ''' <param name="bInheritHandles">
    ''' If this parameter is true, each inheritable handle in the calling 
    ''' process is inherited by the new process. If the parameter is false, 
    ''' the handles are not inherited. 
    ''' </param>
    ''' <param name="dwCreationFlags">
    ''' The flags that control the priority class and the creation of the 
    ''' process.
    ''' </param>
    ''' <param name="pEnvironment">
    ''' A pointer to an environment block for the new process. 
    ''' </param>
    ''' <param name="currentDirectory">
    ''' The full path to the current directory for the process. 
    ''' </param>
    ''' <param name="startupInfo">
    ''' References a STARTUPINFO structure.
    ''' </param>
    ''' <param name="processInformation">
    ''' Outputs a PROCESS_INFORMATION structure that receives identification
    ''' information about the new process. 
    ''' </param>
    ''' <returns></returns>
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function CreateProcessAsUser( _
        ByVal hToken As SafeTokenHandle, _
        ByVal applicationName As String, _
        ByVal commandLine As String, _
        ByVal pProcessAttributes As IntPtr, _
        ByVal pThreadAttributes As IntPtr, _
        ByVal bInheritHandles As Boolean, _
        ByVal dwCreationFlags As UInt32, _
        ByVal pEnvironment As IntPtr, _
        ByVal currentDirectory As String, _
        ByRef startupInfo As STARTUPINFO, _
        <Out()> ByRef processInformation As PROCESS_INFORMATION) _
        As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function


    ''' <summary>
    ''' Closes an open object handle.
    ''' </summary>
    ''' <param name="handle">A valid handle to an open object.</param>
    ''' <returns></returns>
    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function CloseHandle( _
        ByVal handle As IntPtr) _
        As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

End Class


''' <summary>
''' Well-known folder paths
''' </summary>
''' <remarks></remarks>
Friend Class KnownFolder

    Private Shared ReadOnly LocalAppDataGuid As Guid = New Guid( _
        "F1B32785-6FBA-4FCF-9D55-7B8E7F157091")
    Public Shared ReadOnly Property LocalAppData() As String
        Get
            Return SHGetKnownFolderPath(KnownFolder.LocalAppDataGuid)
        End Get
    End Property

    Private Shared ReadOnly LocalAppDataLowGuid As Guid = New Guid( _
        "A520A1A4-1780-4FF6-BD18-167343C5AF16")
    Public Shared ReadOnly Property LocalAppDataLow() As String
        Get
            Return SHGetKnownFolderPath(KnownFolder.LocalAppDataLowGuid)
        End Get
    End Property


    ''' <summary>
    ''' Retrieves the full path of a known folder identified by the 
    ''' folder's KNOWNFOLDERID.
    ''' </summary>
    ''' <param name="rfid">
    ''' A reference to the KNOWNFOLDERID that identifies the folder.
    ''' </param>
    ''' <returns></returns>
    Public Shared Function SHGetKnownFolderPath(ByVal rfid As Guid) As String
        Dim pPath As IntPtr = IntPtr.Zero
        Dim path As String = Nothing
        Try
            Dim hr As Integer = SHGetKnownFolderPath(rfid, 0, IntPtr.Zero, pPath)
            If (hr <> 0) Then
                Throw Marshal.GetExceptionForHR(hr)
            End If
            path = Marshal.PtrToStringUni(pPath)
        Finally
            If (pPath <> IntPtr.Zero) Then
                Marshal.FreeCoTaskMem(pPath)
                pPath = IntPtr.Zero
            End If
        End Try
        Return path
    End Function


    ''' <summary>
    ''' Retrieves the full path of a known folder identified by the folder's 
    ''' KNOWNFOLDERID.
    ''' </summary>
    ''' <param name="rfid">
    ''' A reference to the KNOWNFOLDERID that identifies the folder.
    ''' </param>
    ''' <param name="dwFlags">
    ''' Flags that specify special retrieval options.
    ''' </param>
    ''' <param name="hToken">
    ''' An access token that represents a particular user. If this parameter 
    ''' is NULL, which is the most common usage, the function requests the 
    ''' known folder for the current user.
    ''' </param>
    ''' <param name="pszPath">
    ''' When this method returns, contains the address of a pointer to a 
    ''' null-terminated Unicode string that specifies the path of the known 
    ''' folder. The calling process is responsible for freeing this resource 
    ''' once it is no longer needed by calling CoTaskMemFree.
    ''' </param>
    ''' <returns>HRESULT</returns>
    <DllImport("shell32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Private Shared Function SHGetKnownFolderPath( _
        <MarshalAs(UnmanagedType.LPStruct)> ByVal rfid As Guid, _
        ByVal dwFlags As UInt32, _
        ByVal hToken As IntPtr, _
        <Out()> ByRef pszPath As IntPtr) _
        As Integer
    End Function

End Class