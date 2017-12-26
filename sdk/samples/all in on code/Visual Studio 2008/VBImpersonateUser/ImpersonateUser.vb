'****************************** Module Header ********************************\
' Module Name:  ImpersonateUser.vb
' Project:      VBImpersonateUser
' Copyright (c) Microsoft Corporation.
' 
' The wrapper class of P/Invoke signatures for impersonating user
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************/

#Region "Imports directives"

Imports System
Imports System.Runtime.InteropServices
Imports System.Security.Principal
Imports System.Security.Permissions
Imports Microsoft.VisualBasic
Imports System.Security
Imports System.ComponentModel
Imports System.Runtime.ConstrainedExecution

#End Region


''' <summary>
''' The wrapper class for impersonating user
''' </summary>
Public Class ImpersonateUser

    ''' <summary>
    ''' A delegate that will be called under the impersonation context
    ''' </summary>
    ''' <typeparam name="TReturn"></typeparam>
    ''' <typeparam name="TParameter"></typeparam>
    ''' <param name="paramter"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Delegate Function ImpersonationWorkFunction(Of TReturn, TParameter) _
    (ByVal paramter As TParameter) As TReturn


    ''' <summary>
    ''' This method calles LogonUser API to impersonation the user and is 
    ''' a wrapper around the code exposed by the delegate which makes it 
    ''' run while impersonating 
    ''' </summary>
    ''' <typeparam name="TReturn">
    ''' Generic return type of the delegated function
    ''' </typeparam>
    ''' <typeparam name="TParameter">
    ''' Generic parameter of the delegated function
    ''' </typeparam>
    ''' <param name="userName">The user name</param>
    ''' <param name="domain">Domain</param>
    ''' <param name="password">Password</param>
    ''' <param name="parameter">Parameter of the delegated function</param>
    ''' <param name="impersonationWork">
    ''' Called method while impersonating
    ''' </param>
    ''' <param name="logonMethod">
    ''' The type of logon operation to perform
    ''' </param>
    ''' <param name="provider">The logon provider</param>
    ''' <returns>The return of the delegated function</returns>
    <SecurityPermission(SecurityAction.Demand, UnmanagedCode:=True)> _
    Public Shared Function Impersonate(Of TReturn, TParameter)( _
    ByVal userName As String, ByVal domain As String, _
    ByVal password As SecureString, ByVal parameter As TParameter, _
    ByVal impersonationWork As ImpersonationWorkFunction(Of TReturn, TParameter), _
    ByVal logonMethod As NativeMethod.LogonType, _
    ByVal provider As NativeMethod.LogonProvider) _
    As TReturn

        ' Check the parameters
        If String.IsNullOrEmpty(userName) Then
            Throw New ArgumentNullException("userName")
        End If
        If password Is Nothing Then
            Throw New ArgumentNullException("password")
        End If
        If impersonationWork = Nothing Then
            Throw New ArgumentNullException("impersonationWork")
        End If
        If logonMethod < NativeMethod.LogonType.LOGON32_LOGON_INTERACTIVE Or _
        NativeMethod.LogonType.LOGON32_LOGON_NEW_CREDENTIALS < logonMethod Then
            Throw New ArgumentOutOfRangeException("logonMethod")
        End If
        If provider < NativeMethod.LogonProvider.LOGON32_PROVIDER_DEFAULT Or _
        NativeMethod.LogonProvider.LOGON32_PROVIDER_WINNT50 < provider Then
            Throw New ArgumentOutOfRangeException("provider")
        End If

        Dim passwordPtr As IntPtr = IntPtr.Zero
        Dim token As SafeUserToken = Nothing
        Dim context As WindowsImpersonationContext = Nothing

        Try

            ' Convert the password to a string
            passwordPtr = Marshal.SecureStringToBSTR(password)
            Dim handle As IntPtr = IntPtr.Zero

            ' Attempts to log a user on to the local computer
            If Not NativeMethod.LogonUser(userName, domain, passwordPtr, _
                                          logonMethod, provider, handle) Then
                Throw New Win32Exception()
            Else
                token = New SafeUserToken(handle)
            End If

        Finally
            ' Erase the memory that the password was stored in
            If Not IntPtr.Zero.Equals(passwordPtr) Then
                Marshal.ZeroFreeBSTR(passwordPtr)
            End If
        End Try

        Try

            ' Impersonate
            Debug.Assert(token IsNot Nothing)
            context = WindowsIdentity.Impersonate(token.DangerousGetHandle())

            ' Call out to the work function
            Return impersonationWork(parameter)

        Finally
            UndoImpersonation(token, context)
        End Try

    End Function


    ''' <summary>
    ''' This method calles LogonUser API to impersonation the user and is 
    ''' a wrapper around the code exposed by the delegate which makes it 
    ''' run while impersonating 
    ''' </summary>
    ''' <typeparam name="TReturn">
    ''' Generic return type of the delegated function
    ''' </typeparam>
    ''' <typeparam name="TParameter">
    ''' Generic parameter of the delegated function
    ''' </typeparam>
    ''' <param name="userName">The user name</param>
    ''' <param name="domain">Domain</param>
    ''' <param name="password">Password</param>
    ''' <param name="parameter">Parameter of the delegated function</param>
    ''' <param name="impersonationWork">
    ''' Called method while impersonating
    ''' </param>
    ''' <returns>The return of the delegated function</returns>
    Public Shared Function Impersonate(Of TReturn, TParameter)( _
    ByVal userName As String, ByVal domain As String, _
    ByVal password As SecureString, ByVal parameter As TParameter, _
    ByVal impersonationWork As ImpersonationWorkFunction(Of TReturn, TParameter)) _
    As TReturn

        Return Impersonate(userName, domain, password, parameter, _
                           impersonationWork, _
                           NativeMethod.LogonType.LOGON32_LOGON_INTERACTIVE, _
                           NativeMethod.LogonProvider.LOGON32_PROVIDER_DEFAULT)

    End Function


    ''' <summary>
    ''' Undo impersonation for calling thread
    ''' </summary>
    ''' <param name="token"></param>
    ''' <param name="context"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Shared Function UndoImpersonation(ByRef token As SafeUserToken, _
                                              ByRef context As WindowsImpersonationContext) _
                                              As Boolean

        If context IsNot Nothing Then
            context.Undo()
            context = Nothing
        End If

        If token IsNot Nothing Then
            token.Dispose()
            token = Nothing
        End If

        Return True

    End Function


    ''' <summary>
    ''' Get user's password with SecureString
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Shared Function GetPassword() As SecureString

        Dim password As New SecureString()

        ' Get the first character of the password
        Dim nextKey As ConsoleKeyInfo = Console.ReadKey(True)
        While nextKey.Key <> ConsoleKey.Enter
            If nextKey.Key = ConsoleKey.Backspace Then
                If password.Length > 0 Then
                    password.RemoveAt(password.Length - 1)
                    ' erase the last * as well
                    Console.Write(nextKey.KeyChar)
                    Console.Write(" ")
                    Console.Write(nextKey.KeyChar)
                End If
            Else
                password.AppendChar(nextKey.KeyChar)
                Console.Write("*")
            End If
            nextKey = Console.ReadKey(True)
        End While
        Console.WriteLine()

        ' Lock the password down
        password.MakeReadOnly()
        Return password

    End Function


    ''' <summary>
    ''' A SafeHandle wrapper around the user token so that we gain all the 
    ''' benefits presented by the new SafeHandle model
    ''' </summary>
    ''' <remarks></remarks>
    Friend NotInheritable Class SafeUserToken
        Inherits SafeHandle

        <ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success), _
        SuppressUnmanagedCodeSecurity(), _
        DllImport("Kernel32", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Private Shared Function CloseHandle(ByVal hObject As IntPtr) As Boolean
        End Function


        '' Constructor
        Public Sub New(ByRef h As IntPtr)
            MyBase.New(h, True)
        End Sub


        '' Check to see if the handle is zero
        Public Overrides ReadOnly Property IsInvalid() As Boolean
            Get
                Return IntPtr.Zero.Equals(handle)
            End Get
        End Property


        '' Will call CloseHandle in order to close object handle
        Protected Overrides Function ReleaseHandle() As Boolean
            Return CloseHandle(handle)
        End Function

    End Class

End Class


Public Class NativeMethod

    ''' <summary>
    ''' Attempts to log a user on to the local computer.
    ''' </summary>
    ''' <param name="lpszUsername">The name of the user </param>
    ''' <param name="lpszDomain">The name of the domain</param>
    ''' <param name="lpszPassword">The user's password</param>
    ''' <param name="dwLogonType">The type of logon operation to perform</param>
    ''' <param name="dwLogonProvider">The logon provider</param>
    ''' <param name="phToken">
    ''' A handle to a token that represents the specified user
    ''' </param>
    ''' <returns></returns>
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function LogonUser(ByVal lpszUsername As String, _
                                     ByVal lpszDomain As String, _
                                     ByVal lpszPassword As IntPtr, _
                                     ByVal dwLogonType As LogonType, _
                                     ByVal dwLogonProvider As LogonProvider, _
                                     <Out()> ByRef phToken As IntPtr) _
                                     As Boolean
    End Function


    Public Enum LogonType As Integer

        ' This logon type is intended for users who will be interactively 
        ' using the computer
        LOGON32_LOGON_INTERACTIVE = 2

        ' This logon type is intended for high performance servers to 
        ' authenticate plaintext passwords
        LOGON32_LOGON_NETWORK = 3

        ' This logon type is intended for batch servers
        LOGON32_LOGON_BATCH = 4

        ' Indicates a service-type logon
        LOGON32_LOGON_SERVICE = 5

        ' This logon type is for GINA DLLs that log on users who will be 
        ' interactively using the computer 
        LOGON32_LOGON_UNLOCK = 7

        ' This logon type preserves the name and password in the 
        ' authentication package
        LOGON32_LOGON_NETWORK_CLEARTEXT = 8

        ' This logon type allows the caller to clone its current token 
        ' and specify new credentials for outbound connections. 
        LOGON32_LOGON_NEW_CREDENTIALS = 9

    End Enum


    Public Enum LogonProvider As Integer

        ' Use the standard logon provider for the system 
        LOGON32_PROVIDER_DEFAULT = 0

        ' Use the negotiate logon provider
        LOGON32_PROVIDER_WINNT50 = 1

        ' Use the NTLM logon provider
        LOGON32_PROVIDER_WINNT40 = 2

    End Enum

End Class