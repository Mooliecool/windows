'******************************** Module Header ***************************************'
' Module Name:  MainForm.vb
' Project:      VBRunProcessAsUser
' Copyright (c) Microsoft Corporation.
' 
' The MainForm.vb file is backstage code which works for VBRunProcessAsUser main form.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************************'

Imports System.Security
Imports System.Runtime.InteropServices


Partial Public Class RunProcessAsUser
    Inherits Form


    Public Sub New()
        InitializeComponent()
    End Sub

    ''' <summary>
    ''' input execution command line using OpenFileDialog object
    ''' </summary>
    Private Sub btnCommand_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnCommand.Click
        Using ofdOpen As New OpenFileDialog()
            If ofdOpen.ShowDialog(Me) = DialogResult.OK Then
                tbCommand.Text = ofdOpen.FileName
            End If
        End Using
    End Sub

    ''' <summary>
    ''' Start Process using Process.Start Included in System.Diagnostics.
    ''' </summary>
    Private Sub btnRunCommand_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnRunCommand.Click
        Try
            If (Not String.IsNullOrEmpty(tbUserName.Text)) AndAlso _
                (Not String.IsNullOrEmpty(tbPassword.Text)) AndAlso _
                (Not String.IsNullOrEmpty(tbCommand.Text)) Then

                Dim password As SecureString = StringToSecureString(Me.tbPassword.Text.ToString())
                Dim proc As Process = Process.Start(tbCommand.Text.ToString(), _
                                                    tbUserName.Text.ToString(), _
                                                    password, _
                                                    tbDomain.Text.ToString())

                ProcessStarted(proc.Id)

                proc.EnableRaisingEvents = True
                AddHandler proc.Exited, AddressOf ProcessExited
            Else
                MessageBox.Show("Please fill in the user name, password and command")
                Return
            End If
        Catch w32e As System.ComponentModel.Win32Exception
            ProcessStartFailed(w32e.Message)
        End Try
    End Sub

    ''' <summary>
    ''' Load UserName variable for current user name and UserDomainName for current user domain name 
    ''' </summary>
    Private Sub RunProccessAsMutiUser_Load(ByVal sender As Object, ByVal e As EventArgs) Handles MyBase.Load
        tbPassword.PasswordChar = ChrW(&H25CF)
    End Sub

    ''' <summary>
    ''' Triggered when the target process is started.
    ''' </summary>
    Private Sub ProcessStarted(ByVal processId As Integer)
        MessageBox.Show("Process " & processId.ToString() & " started")
    End Sub

    ''' <summary>
    ''' Triggered when the target process is exited.
    ''' </summary>
    Private Sub ProcessExited(ByVal sender As Object, ByVal e As EventArgs)
        Dim proc As Process = TryCast(sender, Process)
        If proc IsNot Nothing Then
            MessageBox.Show("Process " & proc.Id.ToString() & " exited")
        End If
    End Sub

    ''' <summary>
    ''' Triggered when the target process failed to be started.
    ''' </summary>
    Private Sub ProcessStartFailed(ByVal [error] As String)
        MessageBox.Show([error], "Process failed to start", MessageBoxButtons.OK, MessageBoxIcon.Error)
    End Sub

    ''' <summary>
    ''' Demonstrate how to popup the credential prompt in order to input the user credential.
    ''' </summary>
    Private Sub btnCredentialUIPrompt_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnCredentialUIPrompt.Click
        Try
            Using dialog As New Kerr.PromptForCredential()
                dialog.Title = "Please Specify the user"
                dialog.DoNotPersist = True
                dialog.ShowSaveCheckBox = False
                dialog.TargetName = Environment.MachineName
                dialog.ExpectConfirmation = True

                If DialogResult.OK = dialog.ShowDialog(Me) Then
                    tbPassword.Text = SecureStringToString(dialog.Password)
                    Dim strSplit() As String = dialog.UserName.Split("\"c)
                    If (strSplit.Length = 2) Then
                        Me.tbUserName.Text = strSplit(1)
                        Me.tbDomain.Text = strSplit(0)
                    Else
                        Me.tbUserName.Text = dialog.UserName
                    End If
                End If
            End Using
        Catch ex As Exception
            MessageBox.Show(ex.ToString())
        End Try
    End Sub

    ''' <summary>
    ''' Helper function. It converts SecureString to String
    ''' </summary>
    Shared Function SecureStringToString(ByVal secureStr As SecureString) As String
        Dim bstr As IntPtr = Marshal.SecureStringToBSTR(secureStr)
        Try
            Return Marshal.PtrToStringBSTR(bstr)
        Finally
            Marshal.FreeBSTR(bstr)
        End Try
    End Function

    ''' <summary>
    ''' Helper function. It converts String to SecureString
    ''' </summary>
    Shared Function StringToSecureString(ByVal str As String) As SecureString
        Dim secureStr As New SecureString()
        Dim chars() As Char = str.ToCharArray()
        For i As Integer = 0 To chars.Length - 1
            secureStr.AppendChar(chars(i))
        Next i
        Return secureStr
    End Function
End Class

