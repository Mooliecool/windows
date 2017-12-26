'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'-----------------------------------------------------------------------
Imports System.Collections
Imports System.Collections.Generic

Namespace Microsoft.Samples.FileDemo
    Public NotInheritable Class EnvironVariables
        Inherits System.Windows.Forms.Form

        Private htUserChanges As Hashtable
        Private htMachineChanges As Hashtable

#Region " Windows Form Designer generated code "

        Public Sub New()
            MyBase.New()

            'This call is required by the Windows Form Designer.
            InitializeComponent()

            'Add any initialization after the InitializeComponent() call

        End Sub

        'Form overrides dispose to clean up the component list.
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub

        'Required by the Windows Form Designer
        Private components As System.ComponentModel.IContainer

        'NOTE: The following procedure is required by the Windows Form Designer
        'It can be modified using the Windows Form Designer.  
        'Do not modify it using the code editor.
        Friend WithEvents groupUser As System.Windows.Forms.GroupBox
        Friend WithEvents listUsers As System.Windows.Forms.ListView
        Friend WithEvents Variable As System.Windows.Forms.ColumnHeader
        Friend WithEvents Value As System.Windows.Forms.ColumnHeader
        Friend WithEvents buttonUserDelete As System.Windows.Forms.Button
        Friend WithEvents buttonUserEdit As System.Windows.Forms.Button
        Friend WithEvents buttonUserNew As System.Windows.Forms.Button
        Friend WithEvents buttonOK As System.Windows.Forms.Button
        Friend WithEvents listMachine As System.Windows.Forms.ListView
        Friend WithEvents ColumnHeader1 As System.Windows.Forms.ColumnHeader
        Friend WithEvents ColumnHeader2 As System.Windows.Forms.ColumnHeader
        Friend WithEvents buttonMachineNew As System.Windows.Forms.Button
        Friend WithEvents buttonMachineEdit As System.Windows.Forms.Button
        Friend WithEvents buttonMachineDelete As System.Windows.Forms.Button
        Friend WithEvents groupSystem As System.Windows.Forms.GroupBox
        Friend WithEvents buttonCancel As System.Windows.Forms.Button
        <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
            Me.buttonCancel = New System.Windows.Forms.Button
            Me.groupUser = New System.Windows.Forms.GroupBox
            Me.buttonUserDelete = New System.Windows.Forms.Button
            Me.buttonUserEdit = New System.Windows.Forms.Button
            Me.buttonUserNew = New System.Windows.Forms.Button
            Me.listUsers = New System.Windows.Forms.ListView
            Me.Variable = New System.Windows.Forms.ColumnHeader
            Me.Value = New System.Windows.Forms.ColumnHeader
            Me.buttonOK = New System.Windows.Forms.Button
            Me.listMachine = New System.Windows.Forms.ListView
            Me.ColumnHeader1 = New System.Windows.Forms.ColumnHeader
            Me.ColumnHeader2 = New System.Windows.Forms.ColumnHeader
            Me.buttonMachineNew = New System.Windows.Forms.Button
            Me.buttonMachineEdit = New System.Windows.Forms.Button
            Me.buttonMachineDelete = New System.Windows.Forms.Button
            Me.groupSystem = New System.Windows.Forms.GroupBox
            Me.groupUser.SuspendLayout()
            Me.groupSystem.SuspendLayout()
            Me.SuspendLayout()
            '
            'buttonCancel
            '
            Me.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.buttonCancel.Location = New System.Drawing.Point(577, 578)
            Me.buttonCancel.Name = "buttonCancel"
            Me.buttonCancel.Size = New System.Drawing.Size(88, 32)
            Me.buttonCancel.TabIndex = 4
            Me.buttonCancel.Text = "Cancel"
            '
            'groupUser
            '
            Me.groupUser.Controls.Add(Me.buttonUserDelete)
            Me.groupUser.Controls.Add(Me.buttonUserEdit)
            Me.groupUser.Controls.Add(Me.buttonUserNew)
            Me.groupUser.Controls.Add(Me.listUsers)
            Me.groupUser.Location = New System.Drawing.Point(9, 8)
            Me.groupUser.Name = "groupUser"
            Me.groupUser.Size = New System.Drawing.Size(656, 281)
            Me.groupUser.TabIndex = 6
            Me.groupUser.TabStop = False
            Me.groupUser.Text = "User Variables for "
            '
            'buttonUserDelete
            '
            Me.buttonUserDelete.Location = New System.Drawing.Point(564, 238)
            Me.buttonUserDelete.Name = "buttonUserDelete"
            Me.buttonUserDelete.Size = New System.Drawing.Size(80, 32)
            Me.buttonUserDelete.TabIndex = 6
            Me.buttonUserDelete.Text = "Delete"
            '
            'buttonUserEdit
            '
            Me.buttonUserEdit.Location = New System.Drawing.Point(476, 238)
            Me.buttonUserEdit.Name = "buttonUserEdit"
            Me.buttonUserEdit.Size = New System.Drawing.Size(80, 32)
            Me.buttonUserEdit.TabIndex = 5
            Me.buttonUserEdit.Text = "Edit"
            '
            'buttonUserNew
            '
            Me.buttonUserNew.Location = New System.Drawing.Point(388, 238)
            Me.buttonUserNew.Name = "buttonUserNew"
            Me.buttonUserNew.Size = New System.Drawing.Size(80, 32)
            Me.buttonUserNew.TabIndex = 4
            Me.buttonUserNew.Text = "New"
            '
            'listUsers
            '
            Me.listUsers.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.Variable, Me.Value})
            Me.listUsers.FullRowSelect = True
            Me.listUsers.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable
            Me.listUsers.HideSelection = False
            Me.listUsers.Location = New System.Drawing.Point(12, 26)
            Me.listUsers.MultiSelect = False
            Me.listUsers.Name = "listUsers"
            Me.listUsers.Size = New System.Drawing.Size(632, 203)
            Me.listUsers.TabIndex = 1
            Me.listUsers.View = System.Windows.Forms.View.Details
            '
            'buttonOK
            '
            Me.buttonOK.DialogResult = System.Windows.Forms.DialogResult.OK
            Me.buttonOK.Location = New System.Drawing.Point(477, 578)
            Me.buttonOK.Name = "buttonOK"
            Me.buttonOK.Size = New System.Drawing.Size(88, 32)
            Me.buttonOK.TabIndex = 8
            Me.buttonOK.Text = "OK"
            '
            'listMachine
            '
            Me.listMachine.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.ColumnHeader1, Me.ColumnHeader2})
            Me.listMachine.FullRowSelect = True
            Me.listMachine.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable
            Me.listMachine.HideSelection = False
            Me.listMachine.Location = New System.Drawing.Point(11, 26)
            Me.listMachine.MultiSelect = False
            Me.listMachine.Name = "listMachine"
            Me.listMachine.Size = New System.Drawing.Size(632, 203)
            Me.listMachine.TabIndex = 6
            Me.listMachine.View = System.Windows.Forms.View.Details
            '
            'buttonMachineNew
            '
            Me.buttonMachineNew.Location = New System.Drawing.Point(388, 238)
            Me.buttonMachineNew.Name = "buttonMachineNew"
            Me.buttonMachineNew.Size = New System.Drawing.Size(80, 32)
            Me.buttonMachineNew.TabIndex = 7
            Me.buttonMachineNew.Text = "New"
            '
            'buttonMachineEdit
            '
            Me.buttonMachineEdit.Location = New System.Drawing.Point(476, 238)
            Me.buttonMachineEdit.Name = "buttonMachineEdit"
            Me.buttonMachineEdit.Size = New System.Drawing.Size(80, 32)
            Me.buttonMachineEdit.TabIndex = 8
            Me.buttonMachineEdit.Text = "Edit"
            '
            'buttonMachineDelete
            '
            Me.buttonMachineDelete.Location = New System.Drawing.Point(564, 238)
            Me.buttonMachineDelete.Name = "buttonMachineDelete"
            Me.buttonMachineDelete.Size = New System.Drawing.Size(80, 32)
            Me.buttonMachineDelete.TabIndex = 9
            Me.buttonMachineDelete.Text = "Delete"
            '
            'groupSystem
            '
            Me.groupSystem.Controls.Add(Me.buttonMachineDelete)
            Me.groupSystem.Controls.Add(Me.buttonMachineEdit)
            Me.groupSystem.Controls.Add(Me.buttonMachineNew)
            Me.groupSystem.Controls.Add(Me.listMachine)
            Me.groupSystem.Location = New System.Drawing.Point(9, 291)
            Me.groupSystem.Name = "groupSystem"
            Me.groupSystem.Size = New System.Drawing.Size(656, 279)
            Me.groupSystem.TabIndex = 7
            Me.groupSystem.TabStop = False
            Me.groupSystem.Text = "System Variables"
            '
            'EnvironVariables
            '
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.ClientSize = New System.Drawing.Size(676, 619)
            Me.Controls.Add(Me.buttonOK)
            Me.Controls.Add(Me.groupSystem)
            Me.Controls.Add(Me.groupUser)
            Me.Controls.Add(Me.buttonCancel)
            Me.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "EnvironVariables"
            Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
            Me.Text = "Environment Variables"
            Me.groupUser.ResumeLayout(False)
            Me.groupSystem.ResumeLayout(False)
            Me.ResumeLayout(False)

        End Sub

#End Region

        Private Sub EnvironVariables_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

            Dim htUsers As IDictionary
            Dim htMachine As IDictionary

            EnvironmentChanged = False
            htUserChanges = New Hashtable()
            htMachineChanges = New Hashtable()
            SetupColumns()

            groupUser.Text = groupUser.Text & Environment.UserName

            htUsers = Environment.GetEnvironmentVariables(EnvironmentVariableTarget.User)
            htMachine = Environment.GetEnvironmentVariables(EnvironmentVariableTarget.Machine)

            For Each de As DictionaryEntry In htUsers
                Dim lvi As ListViewItem = New ListViewItem(de.Key.ToString())
                lvi.SubItems.Add(de.Value.ToString())
                listUsers.Items.Add(lvi)
            Next
            If listUsers.Items.Count > 0 Then
                listUsers.Items(0).Selected = True
            End If

            For Each de As DictionaryEntry In htMachine
                Dim lvi As ListViewItem = New ListViewItem(de.Key.ToString())
                lvi.SubItems.Add(de.Value.ToString())
                ListMachine.Items.Add(lvi)
            Next
            If ListMachine.Items.Count > 0 Then
                ListMachine.Items(0).Selected = True
            End If

        End Sub

        Private Sub buttonOK_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonOK.Click
            Try
                If EnvironmentChanged Then
                    Cursor = Cursors.WaitCursor
                    ' CAUTION urged when using these of course...
                    For Each de As DictionaryEntry In htUserChanges
                        Environment.SetEnvironmentVariable(CStr(de.Key), CStr(de.Value), EnvironmentVariableTarget.User)
                    Next

                    For Each de As DictionaryEntry In htMachineChanges

                        Environment.SetEnvironmentVariable(CStr(de.Key), CStr(de.Value), EnvironmentVariableTarget.Machine)
                    Next
                    Cursor = Cursors.Arrow
                End If
            Finally
                Me.Close()
            End Try
        End Sub

        Private Sub SetupColumns()

            listUsers.Columns(0).Text = "Variable"
            listUsers.Columns(1).Text = "Value"
            listUsers.Columns(0).Width = CInt(listUsers.Width / 3)
            listUsers.Columns(1).Width = CInt(listUsers.Width * 2 / 3)

            listMachine.Columns(0).Text = "Variable"
            listMachine.Columns(1).Text = "Value"
            listMachine.Columns(0).Width = CInt(listUsers.Width / 3)
            listMachine.Columns(1).Width = CInt(listUsers.Width * 2 / 3)

        End Sub

        Private Sub buttonCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonCancel.Click
            Me.Close()
        End Sub

        Private Sub buttonUserNew_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonUserNew.Click
            Dim ev As EditVariable = New EditVariable
            NewName = ""
            NewValue = ""
            ev.textName.Enabled = True
            ev.ShowDialog()

            If NewName <> "" Then
                For Each nextItem As ListViewItem In listUsers.Items
                    If NewName.ToLowerInvariant() = nextItem.Text.ToLowerInvariant() Then
                        Exit Sub
                    End If
                Next
                htUserChanges(NewName) = NewValue
                Dim lvi As ListViewItem = New ListViewItem(NewName)
                lvi.SubItems.Add(NewValue)
                listUsers.Items.Add(lvi).Selected = True
            End If
        End Sub

        Private Sub buttonUserEdit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonUserEdit.Click
            Dim ev As EditVariable = New EditVariable
            With listUsers.SelectedItems(0)
                ev.textName.Text = .Text
                ev.textValue.Text = .SubItems(1).Text

                NewName = .Text
                NewValue = ""

                ev.ShowDialog()

                ' just refresh the list
                If NewName <> .Text And NewName.Trim().Length > 0 Then
                    If .Text.ToLowerInvariant() <> NewValue.ToLowerInvariant() Then
                        htUserChanges(NewName) = NewValue
                        listUsers.SelectedItems(0).SubItems(1).Text = NewValue
                    End If
                End If
            End With
        End Sub

        Private Sub buttonMachineNew_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonMachineNew.Click
            Dim ev As EditVariable = New EditVariable
            NewName = ""
            NewValue = ""
            ev.textName.Enabled = True
            ev.ShowDialog()

            If NewName <> "" Then
                For Each nextItem As ListViewItem In listMachine.Items
                    If NewName.ToLowerInvariant() = nextItem.Text.ToLowerInvariant() Then
                        Exit Sub
                    End If
                Next
                htMachineChanges(NewName) = NewValue
                Dim lvi As ListViewItem = New ListViewItem(NewName)
                lvi.SubItems.Add(NewValue)
                listMachine.Items.Add(lvi).Selected = True
            End If
        End Sub

        Private Sub buttonMachineEdit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonMachineEdit.Click
            Dim ev As EditVariable = New EditVariable
            With listMachine.SelectedItems(0)
                ev.textName.Text = .Text
                ev.textValue.Text = .SubItems(1).Text

                NewName = .Text
                NewValue = ""

                ev.ShowDialog()

                ' just refresh the list
                If NewName <> .Text And NewName.Trim().Length > 0 Then
                    If .Text.ToLowerInvariant() <> NewValue.ToLowerInvariant() Then
                        htMachineChanges(NewName) = NewValue
                        listMachine.SelectedItems(0).SubItems(1).Text = NewValue
                    End If
                End If
            End With
        End Sub

        Private Sub buttonUserDelete_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonUserDelete.Click
            If listUsers.SelectedItems.Count > 0 Then
                If MsgBox("Are you sure you want to delete user variable " & listUsers.SelectedItems(0).Text & "?", MsgBoxStyle.Exclamation And MsgBoxStyle.YesNo, "Delete Environment Variable") = MsgBoxResult.Yes Then
                    EnvironmentChanged = True
                    htUserChanges(listUsers.SelectedItems(0).Text) = ""
                    listUsers.Items.Remove(listUsers.SelectedItems(0))
                    If listUsers.Items.Count > 0 Then
                        listUsers.Items(0).Selected = True
                    End If
                End If
            End If
        End Sub

        Private Sub buttonMachineDelete_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonMachineDelete.Click
            If listMachine.SelectedItems.Count > 0 Then
                If MsgBox("Are you sure you want to delete machine variable " & listMachine.SelectedItems(0).Text & "?", MsgBoxStyle.Exclamation And MsgBoxStyle.YesNo, "Delete Environment Variable") = MsgBoxResult.Yes Then
                    EnvironmentChanged = True
                    htMachineChanges(listMachine.SelectedItems(0).Text) = ""
                    listMachine.Items.Remove(listMachine.SelectedItems(0))
                    If listMachine.Items.Count > 0 Then
                        listMachine.Items(0).Selected = True
                    End If
                End If
            End If
        End Sub
    End Class
End Namespace