'********************************** Module Header **********************************\
' Module Name:  HelperMethods.cs
' Project:      CSWin7TaskbarJumpList
' Copyright (c) Microsoft Corporation.
' 
' The file contains the helper methods tohandle Admin session check, restart 
' application to elevate the user session, register/unregister application ID and 
' file handle, validate file name, and create files under system temp folder.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***********************************************************************************/

#Region "Imports directive"
Imports System.Security.Principal
Imports System.IO
Imports Microsoft.Win32
#End Region


Public Class HelperMethod

    ' Shared registry key (HKCU or HKCR)
    Shared classRoot As RegistryKey


    '  Helper method to check whether the current application is runas Admin
    Public Shared Function IsAdmin() As Boolean
        Dim id As WindowsIdentity = WindowsIdentity.GetCurrent()
        Dim principal As WindowsPrincipal = New WindowsPrincipal(id)
        Return principal.IsInRole(WindowsBuiltInRole.Administrator)
    End Function


    ' Helper method to restart the current application to evelate it to Admin session
    Public Shared Sub RestartApplicationAsAdmin()
        Dim proc As ProcessStartInfo = New ProcessStartInfo()
        proc.UseShellExecute = True
        ' Get the current application directory
        proc.WorkingDirectory = Environment.CurrentDirectory
        ' Get the current application executable file path
        proc.FileName = Application.ExecutablePath
        proc.Verb = "runas"  ' Elevate the privilege
        Process.Start(proc)  ' Restart the current process
    End Sub


    ' Helper method to check whether the current application has registered some file handle
    Public Shared Function IsApplicationRegistered(ByVal appid As String) _
    As Boolean
        Try
            ' Open the current application's AppID key under HKCR
            Dim progIdKey As RegistryKey = Registry.ClassesRoot.OpenSubKey(appid)

            If Not progIdKey Is Nothing Then
                progIdKey.Close()
                Return True
            End If
        Catch ex As Exception
            MessageBox.Show(ex.Message, ex.Source)
        End Try
        Return False
    End Function


    ' Call InternalRegisterFileAssociations method to register the file handle
    Public Shared Sub RegisterFileAssociations( _
    ByVal progId As String, ByVal registerInHKCU As Boolean, ByVal appId As String, _
    ByVal openWith As String, ByVal ParamArray extensions() As String)
        InternalRegisterFileAssociations(False, progId, registerInHKCU, appId, openWith, extensions)
    End Sub


    ' Call InternalRegisterFileAssociations method to unregister the file handle
    Public Shared Sub UnregisterFileAssociations( _
    ByVal progId As String, ByVal registerInHKCU As Boolean, ByVal appId As String, _
    ByVal openWith As String, ByVal ParamArray extensions() As String)
        InternalRegisterFileAssociations(True, progId, registerInHKCU, appId, openWith, extensions)
    End Sub


    ' Private helper method to register/unregister application file handle
    Private Shared Sub InternalRegisterFileAssociations( _
    ByVal unregister As Boolean, ByVal progId As String, ByVal registerInHKCU As Boolean, _
    ByVal appId As String, ByVal openWith As String, ByVal associationsToRegister() As String)
        Try
            ' Check whether to register the file handle under HKCU or HKCR
            If registerInHKCU Then
                classRoot = Registry.CurrentUser.OpenSubKey("Software\Classes")
            Else
                classRoot = Registry.ClassesRoot
            End If

            ' First of all, unregister the file handle
            For Each assoc In associationsToRegister
                UnregisterFileAssociation(progId, assoc)
            Next
            UnregisterProgId(progId)

            ' Register the application ID and the file handle for each file extension
            If Not unregister Then
                RegisterProgId(progId, appId, openWith)
                For Each assoc In associationsToRegister
                    RegisterFileAssociation(progId, assoc)
                Next
            End If
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub


    ' Register the application ID
    Private Shared Sub RegisterProgId(ByVal progId As String, ByVal appId As String, ByVal openWith As String)
        Dim progIdKey = classRoot.CreateSubKey(progId)
        progIdKey.SetValue("FriendlyTypeName", "@shell32.dll,-8975")
        progIdKey.SetValue("DefaultIcon", "@shell32.dll,-47")
        progIdKey.SetValue("CurVer", progId)
        progIdKey.SetValue("AppUserModelID", appId)
        Dim shell = progIdKey.CreateSubKey("shell")
        shell.SetValue(String.Empty, "Open")
        shell = shell.CreateSubKey("Open")
        shell = shell.CreateSubKey("Command")
        shell.SetValue(String.Empty, openWith)
        shell.Close()
        progIdKey.Close()
    End Sub


    ' Unregister the application ID
    Private Shared Sub UnregisterProgId(ByVal progId As String)
        classRoot.DeleteSubKeyTree(progId)
    End Sub


    ' Register the file handle
    Private Shared Sub RegisterFileAssociation(ByVal progId As String, ByVal extension As String)
        Dim openWithKey = classRoot.CreateSubKey(Path.Combine(extension, "OpenWithProgIds"))
        openWithKey.SetValue(progId, String.Empty)
        openWithKey.Close()
    End Sub


    ' Unregister the file handle
    Private Shared Sub UnregisterFileAssociation(ByVal progId As String, ByVal extension As String)
        Dim openWithKey = classRoot.CreateSubKey(Path.Combine(extension, "OpenWithProgIds"))
        openWithKey.DeleteValue(progId)
        openWithKey.Close()
    End Sub


    ' A helper method to check whether a file name is valid
    Public Shared Function CheckFileName(ByVal fileName As String) As Boolean
        If Not fileName.IndexOfAny(Path.GetInvalidFileNameChars()) = -1 Then
            MessageBox.Show("Please use only characters that are allowed in file names.")
            Return False
        End If
        Return True
    End Function


    ' A helper method to create a file in the temp folder
    Public Shared Function GetTempFileName(ByVal fileName As String)
        Dim path As String = System.IO.Path.Combine(System.IO.Path.GetTempPath(), fileName + ".txt")
        ' Ensure the file exists
        File.Create(path).Close()
        Return path
    End Function

End Class