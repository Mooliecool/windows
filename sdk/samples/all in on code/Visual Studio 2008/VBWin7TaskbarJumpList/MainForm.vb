'*************************************** Module Header ***************************************\
' Module Name:  MainForm.cs
' Project:      VBWin7TaskbarJumpList
' Copyright (c) Microsoft Corporation.
' 
' The Jump List feature is designed to provide you with quick access to the documents and tasks 
' associated with your applications. You can think of Jump Lists like little application-
' specific Start menus. Jump Lists can be found on the application icons that appear on the 
' Taskbar when an application is running or on the Start menu in the recently opened programs
' section. Jump Lists can also be found on the icons of applications that have been 
' specifically pinned to the Taskbar or the Start menu.
' 
' VBWin7TaskbarJumpList example demostrates how to set register Jump List file handle, add 
' items into Recent/Frequent known categories, add/remove user tasks, and add items/links into 
' custom categories in Windows 7 Taskbar Jump List using Taskbar related APIs in Windows API 
' Code Pack.   
' 
' This MainForm can register the .txt file as the application Jump List file handle, add .txt 
' file into Recent/Frequent known categories, add/remove notepad.exe, mspaint.exe, calc.exe as 
' user tasks, and add items/links into custom categories in Windows 7 Taskbar Jump List.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*********************************************************************************************/

#Region "Imports directive"
Imports System.Reflection
Imports System.IO
Imports Microsoft.WindowsAPICodePack.Taskbar
Imports Microsoft.WindowsAPICodePack.Shell
#End Region


Public Class MainForm

    ' The Application ID for the current application. The AppID is necessary since we need to 
    ' register the file extension based on the specific AppID
    Const AppID As String = "All-In-One Code Framework.Win7Taskbar.VBWin7TaskbarJumpList"

    ' Jump List custom category instance
    Private _currentCategory As JumpListCustomCategory

    Private _jumpList As JumpList  ' Jump List instance

    ' Private readonly property of the Jump List instance
    Public ReadOnly Property JumpList() As JumpList
        Get
            ' Create a new Jump List instance if it is null
            If _jumpList Is Nothing Then
                _jumpList = JumpList.CreateJumpList()  ' Create Jump List instance

                ' Set the known category based on the radio button checked value
                _jumpList.KnownCategoryToDisplay = _
                If(radRecent.Checked, JumpListKnownCategoryType.Recent, _
                   JumpListKnownCategoryType.Frequent)

                _jumpList.Refresh()  ' Refresh the Jump List
            End If
            Return _jumpList
        End Get
    End Property


    ' Check the Windows version, if it is Windows 7 or Windows Server 2008 R2, set the AppID 
    ' and update the UI, otherwise exit the process
    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' Check whether the current system is Windows 7 or Windows Server 2008 R2
        If TaskbarManager.IsPlatformSupported Then
            ' Set the AppID (For detail, please see VBWin7TaskbarAppID example)
            TaskbarManager.Instance.ApplicationId = AppID

            ' Update the UI
            chkNotepad.Checked = True
            chkCalc.Checked = True
            chkPaint.Checked = True
            btnClearTask.Enabled = False
            btnAddItem.Enabled = False
            addLinkButton.Enabled = False
            tbItem.Enabled = False
            tbLink.Enabled = False
        Else
            MessageBox.Show("Jump List is not supported in your operation system!" & vbNewLine & _
                            "Please launch the application in Windows 7 systems.")
            Application.Exit()  ' Exit the current process
        End If
    End Sub


    ' Register the Jump List file handle for the application
    Private Sub btnRegisterFileType_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRegisterFileType.Click
        ' Check whether the application ID has been registered
        If HelperMethod.IsApplicationRegistered(TaskbarManager.Instance.ApplicationId) Then
            MessageBox.Show(".txt file type has been registered!")
            Return
        End If

        ' Check whether the application is runas Admin, since we need the Admin privilege 
        ' to modify the HKCR registry values
        If Not HelperMethod.IsAdmin() Then
            ' Ask the user whether to elevate the application to Admin session
            Dim result = MessageBox.Show("This operation needs Admin privilege!" & vbNewLine & _
                                         "Restart and run the application as Admin?", _
                                         "Warning!", MessageBoxButtons.YesNo)

            If result = Windows.Forms.DialogResult.Yes Then
                Try
                    ' Call helper method to restart the application and run the application as Admin
                    HelperMethod.RestartApplicationAsAdmin()
                Catch
                    Return
                End Try

                ' Kill the current application instance
                Application.Exit()
            Else
                Return
            End If
        End If

        ' If the application is runas Admin
        Try
            ' Call helper method to register the .txt file handle
            HelperMethod.RegisterFileAssociations(TaskbarManager.Instance.ApplicationId, False, _
            TaskbarManager.Instance.ApplicationId, Assembly.GetExecutingAssembly().Location, ".txt")
            MessageBox.Show(".txt file type is registered successfully!")
        Catch ex As Exception
            MessageBox.Show("Error registering file type association:" & vbNewLine & ex.Message)
        End Try
    End Sub


    ' Unregister the Jump List file handle
    Private Sub btnUnregisterFileType_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUnregisterFileType.Click
        ' Check whether the application ID has been registered
        If Not HelperMethod.IsApplicationRegistered(TaskbarManager.Instance.ApplicationId) Then
            MessageBox.Show(".txt file type has not been registered!")
            Return
        End If

        ' Check whether the application is runas Admin, since we need the Admin privilege 
        ' to modify the HKCR registry values.
        If Not HelperMethod.IsAdmin() Then
            ' Ask the user whether to elevate the application to Admin session
            Dim result = MessageBox.Show("This operation needs Admin privilege!" & vbNewLine & _
                                         "Restart and run the application as Admin?", _
                                         "Warning!", MessageBoxButtons.YesNo)

            If result = Windows.Forms.DialogResult.OK Then
                Try
                    ' Call helper method to restart the application and run the application as Admin
                    HelperMethod.RestartApplicationAsAdmin()
                Catch
                    Return
                End Try

                ' Kill the current application instance
                Application.Exit()
            Else
                Return
            End If
        End If

        ' If the application is runas Admin
        Try
            ' Call helper method to unregister the .txt file handle
            HelperMethod.UnregisterFileAssociations( _
            TaskbarManager.Instance.ApplicationId, False, _
            TaskbarManager.Instance.ApplicationId, _
            Assembly.GetExecutingAssembly().Location, ".txt")

            MessageBox.Show(".txt file type is unregistered successfully!")
        Catch ex As Exception
            MessageBox.Show("Error unregistering file type association:" & vbNewLine & ex.Message)
        End Try
    End Sub


    ' Update the known category (Recent/Frequent) based on the Radio button status
    Private Sub radFrequentRecent_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles radFrequent.CheckedChanged, radRecent.CheckedChanged
        If radFrequent.Checked Then
            JumpList.KnownCategoryToDisplay = JumpListKnownCategoryType.Frequent
        Else
            JumpList.KnownCategoryToDisplay = JumpListKnownCategoryType.Recent
        End If
        JumpList.Refresh()
    End Sub


    ' Open an OpenFileDialog to make the .txt file show in the known category (Recent/Frequent) 
    Private Sub btnChooseFile_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnChooseFile.Click
        If recentFileOpenFileDialog.ShowDialog() = Windows.Forms.DialogResult.OK Then
            radRecent.Checked = True
        End If
    End Sub


    ' Add user tasks based on the three CheckBox (notepad, paint, calculator)
    Private Sub btnAddTask_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAddTask.Click
        Try
            ' Retrieve the system folder
            Dim systemFolder = Environment.GetFolderPath(Environment.SpecialFolder.System)

            If (chkNotepad.Checked) Then
                ' Add the notepad.exe user task and set the icon
                Dim notepadTask As IJumpListTask = New JumpListLink( _
                Path.Combine(systemFolder, "notepad.exe"), "Open Notepad") With _
                {.IconReference = New IconReference(Path.Combine(systemFolder, "notepad.exe"), 0)}

                JumpList.AddUserTasks(notepadTask) ' Add the notepad user task into the Jump List
            End If

            If chkCalc.Checked Then
                ' Add the calc.exe user task and set the icon
                Dim calcTask As IJumpListTask = New JumpListLink( _
                Path.Combine(systemFolder, "calc.exe"), "Open Calculator") With _
                {.IconReference = New IconReference(Path.Combine(systemFolder, "calc.exe"), 0)}

                JumpList.AddUserTasks(calcTask) ' Add the notepad user task into the Jump List
            End If

            If chkPaint.Checked Then
                ' Add the mspaint.exe user task and a Jump List separator 
                Dim paintTask As IJumpListTask = New JumpListLink( _
                Path.Combine(systemFolder, "mspaint.exe"), "Open Paint") With _
                {.IconReference = New IconReference(Path.Combine(systemFolder, "mspaint.exe"), 0)}

                If chkNotepad.Checked Or chkCalc.Checked Then
                    ' Add a Jump List separator and the paint user task
                    JumpList.AddUserTasks(New JumpListSeparator(), paintTask)
                Else
                    JumpList.AddUserTasks(paintTask) ' Only add the paint user task
                End If
            End If

            ' Refresh the Jump List instance and update the UI
            JumpList.Refresh()
            btnClearTask.Enabled = True
            btnAddTask.Enabled = False
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub


    ' Clear all the user tasks
    Private Sub btnClearTask_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClearTask.Click
        Try
            JumpList.ClearAllUserTasks()  ' Clear all the user tasks

            ' Refresh the Jump List instance and update the UI
            JumpList.Refresh()
            btnAddTask.Enabled = True
            btnClearTask.Enabled = False
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub


    ' Create a custom category
    Private Sub btnCreateCategory_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCreateCategory.Click
        Try
            ' Create a new custom category based on the category name in the categoryTextBox
            _currentCategory = New JumpListCustomCategory(tbCategory.Text)

            ' Add the custom category instance into the Jump List
            JumpList.AddCustomCategories(_currentCategory)

            ' Refresh the Jump List instance and update the UI
            JumpList.Refresh()
            btnAddItem.Enabled = True
            addLinkButton.Enabled = True
            tbItem.Enabled = True
            tbLink.Enabled = True
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub


    ' Add a custom shell item to the custom category
    Private Sub btnAddItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAddItem.Click
        Try
            ' Check the if the file name is valid
            If Not HelperMethod.CheckFileName(tbItem.Text) Then Return

            ' Create a .txt file in the temp foler and create a shell item for this file
            Dim jli As JumpListItem = New JumpListItem(HelperMethod.GetTempFileName(tbItem.Text))

            _currentCategory.AddJumpListItems(jli)  ' Add the shell item to the custom category

            JumpList.Refresh()  ' Refresh the Jump List
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub


    ' Add a custom shell item to the custom category
    Private Sub btnAddLink_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles addLinkButton.Click
        Try
            ' Check the if the file name is valid
            If Not HelperMethod.CheckFileName(tbLink.Text) Then Return

            ' Create a .txt file in the temp foler and create a shell link for this file
            Dim jli As JumpListLink = New JumpListLink(HelperMethod.GetTempFileName(tbLink.Text), tbLink.Text)

            _currentCategory.AddJumpListItems(jli)  ' Add the shell link to the custom category

            JumpList.Refresh()  ' Refresh the Jump List
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub

End Class