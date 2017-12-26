'******************************************* Module Header *******************************************'
' Module Name:	MainForm.vb
' Project:		VBOffendUAC
' Copyright (c) Microsoft Corporation.
' 
' VBOffendUAC demonstrates various tasks requiring administrative privilege. It writes to file system 
' and registry, writes to .ini files in protected locations, restarts services, explicitly checks for 
' admin rights, etc.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 8/27/2009 10:40 PM Jialiang Ge Created
'*****************************************************************************************************'

#Region "Imports directives"

Imports System.IO
Imports Microsoft.Win32
Imports System.Runtime.InteropServices
Imports System.Text
Imports System.ServiceProcess

#End Region


Public Class MainForm

    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles MyBase.Load
        Me.ddlRootRegKey.SelectedIndex = 0

        ' Check for administrative privilege
        If Not NativeMethod.IsUserAnAdmin Then
            MessageBox.Show("You must be an administrator to run this application", _
                            "Permission denied", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End If
    End Sub

    Private Sub btnBrowseFolder_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnBrowseFolder.Click
        If Me.FolderBrowserDialog1.ShowDialog = Windows.Forms.DialogResult.OK Then
            Me.tbDirectoryPath.Text = Me.FolderBrowserDialog1.SelectedPath
        End If
    End Sub

    Private Sub btnSetCurrentDirectory_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnSetCurrentDirectory.Click
    End Sub

    Private Sub btnCreateDirectory_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnCreateDirectory.Click
        Directory.CreateDirectory(Me.tbDirectoryPath.Text)
    End Sub

    Private Sub btnDeleteDirectory_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnDeleteDirectory.Click
        Directory.Delete(Me.tbDirectoryPath.Text)
    End Sub

    Private Sub btnBrowseFile_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnBrowseFile.Click
        If Me.OpenFileDialog1.ShowDialog = Windows.Forms.DialogResult.OK Then
            Me.tbFilePath.Text = Me.OpenFileDialog1.FileName
        End If
    End Sub

    Private Sub btnWriteFile_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnWriteFile.Click
        File.AppendAllText(Me.tbFilePath.Text, Me.tbWriteFile.Text)
    End Sub

    Private Sub btnReadFile_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnReadFile.Click
        Me.tbReadFile.Text = File.ReadAllText(Me.tbFilePath.Text)
    End Sub

    Private Sub btnDeleteFile_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnDeleteFile.Click
        File.Delete(Me.tbFilePath.Text)
    End Sub

    Private Function SelectedRootRegKey() As RegistryKey
        If Me.ddlRootRegKey.Text = "HKLM" Then
            Return Registry.LocalMachine
        ElseIf Me.ddlRootRegKey.Text = "HKCR" Then
            Return Registry.ClassesRoot
        ElseIf Me.ddlRootRegKey.Text = "HKCU" Then
            Return Registry.CurrentUser
        Else
            Return Nothing
        End If
    End Function

    Private Sub btnCreateRegKey_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnCreateRegKey.Click
        SelectedRootRegKey.CreateSubKey(Me.ddlChildRegKey.Text)
    End Sub

    Private Sub btnOpenRegKey_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnOpenRegKey.Click
        SelectedRootRegKey.OpenSubKey(Me.ddlChildRegKey.Text)
    End Sub

    Private Sub btnDeleteRegKey_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnDeleteRegKey.Click
        SelectedRootRegKey.DeleteSubKeyTree(Me.ddlChildRegKey.Text)
    End Sub


    Private Sub bnWriteIni_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles bnWriteIni.Click
        If NativeMethod.WritePrivateProfileString(Me.Text, "Last played at", DateTime.Now.ToString, _
                                                  tbIniFileName.Text) Then
            MessageBox.Show("Wrote to " & tbIniFileName.Text)
        Else
            MessageBox.Show("Unable to write to " & tbIniFileName.Text & "; error # " & Err.LastDllError)
        End If
    End Sub

    Private Sub bnReadIni_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles bnReadIni.Click
        Dim data As New StringBuilder(260)
        If NativeMethod.GetPrivateProfileString(Me.Text, "Last played at", "(Value not found)", _
                                                data, 260, tbIniFileName.Text) <> 0 Then
            MessageBox.Show("Read from " & tbIniFileName.Text & ":" & vbCrLf & data.ToString)
        End If
    End Sub

    Private Sub bnWriteWinIni_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles bnWriteWinIni.Click
        If NativeMethod.WriteProfileString(Me.Text, "Last played at", DateTime.Now.ToString) Then
            MessageBox.Show("Wrote to win.ini")
        Else
            MessageBox.Show("Unable to write to win.ini; error # " & Err.LastDllError)
        End If
    End Sub

    Private Sub bnReadWinIni_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles bnReadWinIni.Click
        Dim data As New StringBuilder(260)
        If NativeMethod.GetProfileString(Me.Text, "Last played at", "(Value not found)", _
                                         data, 260) <> 0 Then
            MessageBox.Show("Read from win.ini:" & vbCrLf & data.ToString)
        End If
    End Sub

    Private Sub btnIsAdmin_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnIsAdmin.Click
        If NativeMethod.IsUserAnAdmin Then
            MessageBox.Show("User is an admin")
        Else
            MessageBox.Show("User is NOT an admin")
        End If
    End Sub

    ''' <summary>
    ''' The SetPrivilege function will accept a privilege, and a flag to either enable/disable 
    ''' that privilege. The function will attempt to perform the desired action upon the token 
    ''' returning TRUE if it succeeded, or FALSE if it failed.
    ''' </summary>
    ''' <param name="Privilege"></param>
    ''' <param name="bSetFlag"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function SetPrivilege(ByVal Privilege As String, ByVal bSetFlag As Boolean) As Boolean
        ' Handle to your process token.
        Dim hToken As IntPtr
        If Not NativeMethod.OpenProcessToken(Process.GetCurrentProcess().Handle, _
                                        NativeMethod.TOKEN_ADJUST_PRIVILEGES Or _
                                        NativeMethod.TOKEN_QUERY, hToken) Then
            Return False
        End If

        Try
            ' Grab the LUID for the request privilege.
            Dim Luid As New LUID
            If Not NativeMethod.LookupPrivilegeValue("", Privilege, Luid) Then
                Return False
            End If

            ' Set up a LUID_AND_ATTRIBUTES structure containing the requested privilege
            Dim luaAttr As New LUID_AND_ATTRIBUTES
            luaAttr.Luid = Luid
            If bSetFlag Then
                luaAttr.Attributes = NativeMethod.SE_PRIVILEGE_ENABLED
            Else
                luaAttr.Attributes = 0
            End If

            'Set up a TOKEN_PRIVILEGES structure containing the requested privilege.
            Dim newState As New TOKEN_PRIVILEGES
            newState.PrivilegeCount = 1
            newState.Privileges = New LUID_AND_ATTRIBUTES() {luaAttr}

            'Set up a TOKEN_PRIVILEGES structure for the returned (modified) privileges.
            Dim prevState As TOKEN_PRIVILEGES = New TOKEN_PRIVILEGES
            ReDim prevState.Privileges(CInt(newState.PrivilegeCount))

            'Apply the TOKEN_PRIVILEGES structure to the current process's token.
            Dim returnLength As IntPtr
            Dim result As Boolean = NativeMethod.AdjustTokenPrivileges( _
            hToken, False, newState, Marshal.SizeOf(prevState), prevState, returnLength)
            Return result And Marshal.GetLastWin32Error() = 0

        Finally
            NativeMethod.CloseHandle(hToken)
        End Try
    End Function

    Private Sub btnSetPrivilege_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnSetPrivilege.Click
        Dim message As New StringBuilder
        If Not SetPrivilege(ddlPrivilege.Text, True) Then
            message.AppendLine("Unable to set the privilege.")
            If Marshal.GetLastWin32Error() = NativeMethod.ERROR_NOT_ALL_ASSIGNED Then
                message.AppendLine("The process does not have the specified privilege in its token")
            End If
        Else
            message.AppendLine("The privilege is set successfully")
        End If
        MessageBox.Show(message.ToString)
    End Sub

    Private Sub btnRemovePrivilege_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnRemovePrivilege.Click
        Dim message As New StringBuilder
        If Not SetPrivilege(ddlPrivilege.Text, False) Then
            message.AppendLine("Unable to remove the privilege.")
            If Marshal.GetLastWin32Error() = NativeMethod.ERROR_NOT_ALL_ASSIGNED Then
                message.AppendLine("The process does not have the specified privilege in its token")
            End If
        Else
            message.AppendLine("The privilege is removed successfully")
        End If
        MessageBox.Show(message.ToString)
    End Sub

    Private Sub bnRestartWinService_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles bnRestartWinService.Click
        Using sc As New ServiceController("Lanmanserver")
            sc.Stop()
            sc.Start()
        End Using
        MessageBox.Show("Lanmanserver service is restarted")
    End Sub

End Class
