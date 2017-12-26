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
Imports System.IO
Imports System.Threading
Imports System.Globalization
Imports System.Security.AccessControl
Imports System.Security.Principal
Imports System.Collections.Generic

'Namespace Microsoft.Samples.FileDemo
'<DefaultInstanceProperty("GetInstance")> Friend Class FileDirectoryProperties
Namespace Microsoft.Samples.FileDemo
    Friend Class FileDirectoryProperties
        Inherits System.Windows.Forms.Form

        Private attributes As FileAttributes
        Private oldUserGroupIndex As Integer
        Private changeAllowed As Boolean
        Private makingCheckChange As Boolean
        Friend Changes As ChangeType
        Friend ItemToManipulate As String
        Friend ChangedName As String
        Friend ItemType As FileItemType
        Friend WithEvents labelTypeValue As System.Windows.Forms.Label
        Friend WithEvents labelType As System.Windows.Forms.Label
        Friend WithEvents groupLine2 As System.Windows.Forms.GroupBox
        Friend WithEvents textName As System.Windows.Forms.TextBox
        Friend WithEvents groupLine0 As System.Windows.Forms.GroupBox
        Friend WithEvents labelDeny As System.Windows.Forms.Label
        Friend WithEvents labelAllow As System.Windows.Forms.Label
        Friend DefaultView As DefaultPropertiesView
        Friend WithEvents labelPermissions As System.Windows.Forms.Label
        Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
        Friend WithEvents groupLine1 As System.Windows.Forms.GroupBox
        Friend WithEvents checkAllowFull As System.Windows.Forms.CheckBox
        Friend WithEvents checkDenyFull As System.Windows.Forms.CheckBox
        Friend WithEvents checkAllowModify As System.Windows.Forms.CheckBox
        Friend WithEvents checkAllowReadAndExecute As System.Windows.Forms.CheckBox
        Friend WithEvents checkAllowRead As System.Windows.Forms.CheckBox
        Friend WithEvents checkDenyWrite As System.Windows.Forms.CheckBox
        Friend WithEvents checkAllowWrite As System.Windows.Forms.CheckBox
        Friend WithEvents checkDenyRead As System.Windows.Forms.CheckBox
        Friend WithEvents checkDenyReadAndExecute As System.Windows.Forms.CheckBox
        Friend WithEvents checkDenyModify As System.Windows.Forms.CheckBox
        Friend WithEvents labelWrite As System.Windows.Forms.Label
        Friend WithEvents labelRead As System.Windows.Forms.Label
        Friend WithEvents labelReadAndExecute As System.Windows.Forms.Label
        Friend WithEvents labelModify As System.Windows.Forms.Label
        Friend WithEvents labelFullControl As System.Windows.Forms.Label
        Friend WithEvents panelPermissions As System.Windows.Forms.Panel
        Private rightsList As Dictionary(Of String, List(Of FileSystemAccessRule))
        Private originalList As Dictionary(Of String, List(Of FileSystemAccessRule))

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
        Friend WithEvents tabMain As System.Windows.Forms.TabControl
        Friend WithEvents tabPageGeneral As System.Windows.Forms.TabPage
        Friend WithEvents tabPageSecurity As System.Windows.Forms.TabPage
        Friend WithEvents buttonOK As System.Windows.Forms.Button
        Friend WithEvents buttonCancel As System.Windows.Forms.Button
        Friend WithEvents buttonApply As System.Windows.Forms.Button
        Friend WithEvents buttonGroupsUsersRemove As System.Windows.Forms.Button
        Friend WithEvents buttonGroupsUsersAdd As System.Windows.Forms.Button
        Friend WithEvents labelGroupsUsers As System.Windows.Forms.Label
        Friend WithEvents listGroupsUsers As System.Windows.Forms.ListBox
        Friend WithEvents labelAccessed As System.Windows.Forms.Label
        Friend WithEvents labelModified As System.Windows.Forms.Label
        Friend WithEvents labelCreated As System.Windows.Forms.Label
        Friend WithEvents labelSizeValue As System.Windows.Forms.Label
        Friend WithEvents labelSize As System.Windows.Forms.Label
        Friend WithEvents labelLocationValue As System.Windows.Forms.Label
        Friend WithEvents labelLocation As System.Windows.Forms.Label
        Friend WithEvents buttonAttributesAdvanced As System.Windows.Forms.Button
        Friend WithEvents checkHidden As System.Windows.Forms.CheckBox
        Friend WithEvents checkReadOnly As System.Windows.Forms.CheckBox
        Friend WithEvents labelAttributes As System.Windows.Forms.Label
        Friend WithEvents labelAccessedValue As System.Windows.Forms.Label
        Friend WithEvents labelModifiedValue As System.Windows.Forms.Label
        Friend WithEvents labelCreatedValue As System.Windows.Forms.Label

        'Required by the Windows Form Designer
        Private components As System.ComponentModel.IContainer

        'NOTE: The following procedure is required by the Windows Form Designer
        'It can be modified using the Windows Form Designer.  
        'Do not modify it using the code editor.
        <System.Diagnostics.DebuggerNonUserCode()> Private Sub InitializeComponent()
            Me.tabMain = New System.Windows.Forms.TabControl
            Me.tabPageGeneral = New System.Windows.Forms.TabPage
            Me.GroupBox1 = New System.Windows.Forms.GroupBox
            Me.groupLine1 = New System.Windows.Forms.GroupBox
            Me.groupLine0 = New System.Windows.Forms.GroupBox
            Me.textName = New System.Windows.Forms.TextBox
            Me.labelTypeValue = New System.Windows.Forms.Label
            Me.labelType = New System.Windows.Forms.Label
            Me.buttonAttributesAdvanced = New System.Windows.Forms.Button
            Me.checkHidden = New System.Windows.Forms.CheckBox
            Me.checkReadOnly = New System.Windows.Forms.CheckBox
            Me.labelAttributes = New System.Windows.Forms.Label
            Me.labelAccessedValue = New System.Windows.Forms.Label
            Me.labelAccessed = New System.Windows.Forms.Label
            Me.labelModifiedValue = New System.Windows.Forms.Label
            Me.labelModified = New System.Windows.Forms.Label
            Me.labelCreatedValue = New System.Windows.Forms.Label
            Me.labelCreated = New System.Windows.Forms.Label
            Me.labelSizeValue = New System.Windows.Forms.Label
            Me.labelSize = New System.Windows.Forms.Label
            Me.labelLocationValue = New System.Windows.Forms.Label
            Me.labelLocation = New System.Windows.Forms.Label
            Me.groupLine2 = New System.Windows.Forms.GroupBox
            Me.tabPageSecurity = New System.Windows.Forms.TabPage
            Me.panelPermissions = New System.Windows.Forms.Panel
            Me.labelWrite = New System.Windows.Forms.Label
            Me.labelRead = New System.Windows.Forms.Label
            Me.labelReadAndExecute = New System.Windows.Forms.Label
            Me.labelModify = New System.Windows.Forms.Label
            Me.labelFullControl = New System.Windows.Forms.Label
            Me.checkDenyWrite = New System.Windows.Forms.CheckBox
            Me.checkAllowWrite = New System.Windows.Forms.CheckBox
            Me.checkDenyRead = New System.Windows.Forms.CheckBox
            Me.checkAllowRead = New System.Windows.Forms.CheckBox
            Me.checkDenyReadAndExecute = New System.Windows.Forms.CheckBox
            Me.checkAllowReadAndExecute = New System.Windows.Forms.CheckBox
            Me.checkDenyModify = New System.Windows.Forms.CheckBox
            Me.checkAllowModify = New System.Windows.Forms.CheckBox
            Me.checkDenyFull = New System.Windows.Forms.CheckBox
            Me.checkAllowFull = New System.Windows.Forms.CheckBox
            Me.labelPermissions = New System.Windows.Forms.Label
            Me.labelDeny = New System.Windows.Forms.Label
            Me.labelAllow = New System.Windows.Forms.Label
            Me.buttonGroupsUsersRemove = New System.Windows.Forms.Button
            Me.buttonGroupsUsersAdd = New System.Windows.Forms.Button
            Me.listGroupsUsers = New System.Windows.Forms.ListBox
            Me.labelGroupsUsers = New System.Windows.Forms.Label
            Me.buttonOK = New System.Windows.Forms.Button
            Me.buttonCancel = New System.Windows.Forms.Button
            Me.buttonApply = New System.Windows.Forms.Button
            Me.tabMain.SuspendLayout()
            Me.tabPageGeneral.SuspendLayout()
            Me.tabPageSecurity.SuspendLayout()
            Me.panelPermissions.SuspendLayout()
            Me.SuspendLayout()
            '
            'tabMain
            '
            Me.tabMain.Controls.Add(Me.tabPageGeneral)
            Me.tabMain.Controls.Add(Me.tabPageSecurity)
            Me.tabMain.Location = New System.Drawing.Point(0, 0)
            Me.tabMain.Name = "tabMain"
            Me.tabMain.SelectedIndex = 0
            Me.tabMain.ShowToolTips = True
            Me.tabMain.Size = New System.Drawing.Size(337, 382)
            Me.tabMain.TabIndex = 0
            '
            'tabPageGeneral
            '
            Me.tabPageGeneral.Controls.Add(Me.GroupBox1)
            Me.tabPageGeneral.Controls.Add(Me.groupLine1)
            Me.tabPageGeneral.Controls.Add(Me.groupLine0)
            Me.tabPageGeneral.Controls.Add(Me.textName)
            Me.tabPageGeneral.Controls.Add(Me.labelTypeValue)
            Me.tabPageGeneral.Controls.Add(Me.labelType)
            Me.tabPageGeneral.Controls.Add(Me.buttonAttributesAdvanced)
            Me.tabPageGeneral.Controls.Add(Me.checkHidden)
            Me.tabPageGeneral.Controls.Add(Me.checkReadOnly)
            Me.tabPageGeneral.Controls.Add(Me.labelAttributes)
            Me.tabPageGeneral.Controls.Add(Me.labelAccessedValue)
            Me.tabPageGeneral.Controls.Add(Me.labelAccessed)
            Me.tabPageGeneral.Controls.Add(Me.labelModifiedValue)
            Me.tabPageGeneral.Controls.Add(Me.labelModified)
            Me.tabPageGeneral.Controls.Add(Me.labelCreatedValue)
            Me.tabPageGeneral.Controls.Add(Me.labelCreated)
            Me.tabPageGeneral.Controls.Add(Me.labelSizeValue)
            Me.tabPageGeneral.Controls.Add(Me.labelSize)
            Me.tabPageGeneral.Controls.Add(Me.labelLocationValue)
            Me.tabPageGeneral.Controls.Add(Me.labelLocation)
            Me.tabPageGeneral.Controls.Add(Me.groupLine2)
            Me.tabPageGeneral.Location = New System.Drawing.Point(4, 22)
            Me.tabPageGeneral.Name = "tabPageGeneral"
            Me.tabPageGeneral.Padding = New System.Windows.Forms.Padding(3)
            Me.tabPageGeneral.Size = New System.Drawing.Size(329, 356)
            Me.tabPageGeneral.TabIndex = 0
            Me.tabPageGeneral.Text = "General"
            '
            'GroupBox1
            '
            Me.GroupBox1.Location = New System.Drawing.Point(13, 238)
            Me.GroupBox1.Name = "GroupBox1"
            Me.GroupBox1.Size = New System.Drawing.Size(311, 8)
            Me.GroupBox1.TabIndex = 22
            Me.GroupBox1.TabStop = False
            '
            'groupLine1
            '
            Me.groupLine1.Location = New System.Drawing.Point(10, 77)
            Me.groupLine1.Name = "groupLine1"
            Me.groupLine1.Size = New System.Drawing.Size(311, 8)
            Me.groupLine1.TabIndex = 21
            Me.groupLine1.TabStop = False
            '
            'groupLine0
            '
            Me.groupLine0.Location = New System.Drawing.Point(10, 41)
            Me.groupLine0.Name = "groupLine0"
            Me.groupLine0.Size = New System.Drawing.Size(311, 8)
            Me.groupLine0.TabIndex = 20
            Me.groupLine0.TabStop = False
            '
            'textName
            '
            Me.textName.Location = New System.Drawing.Point(88, 15)
            Me.textName.Name = "textName"
            Me.textName.Size = New System.Drawing.Size(228, 20)
            Me.textName.TabIndex = 19
            '
            'labelTypeValue
            '
            Me.labelTypeValue.Location = New System.Drawing.Point(88, 56)
            Me.labelTypeValue.Name = "labelTypeValue"
            Me.labelTypeValue.Size = New System.Drawing.Size(178, 19)
            Me.labelTypeValue.TabIndex = 15
            '
            'labelType
            '
            Me.labelType.Location = New System.Drawing.Point(14, 58)
            Me.labelType.Name = "labelType"
            Me.labelType.Size = New System.Drawing.Size(77, 19)
            Me.labelType.TabIndex = 14
            Me.labelType.Text = "Type"
            '
            'buttonAttributesAdvanced
            '
            Me.buttonAttributesAdvanced.Enabled = False
            Me.buttonAttributesAdvanced.Location = New System.Drawing.Point(246, 254)
            Me.buttonAttributesAdvanced.Name = "buttonAttributesAdvanced"
            Me.buttonAttributesAdvanced.Size = New System.Drawing.Size(75, 30)
            Me.buttonAttributesAdvanced.TabIndex = 13
            Me.buttonAttributesAdvanced.Text = "Advanced..."
            '
            'checkHidden
            '
            Me.checkHidden.Location = New System.Drawing.Point(176, 256)
            Me.checkHidden.Name = "checkHidden"
            Me.checkHidden.Size = New System.Drawing.Size(74, 24)
            Me.checkHidden.TabIndex = 12
            Me.checkHidden.Text = "Hidden"
            '
            'checkReadOnly
            '
            Me.checkReadOnly.Location = New System.Drawing.Point(96, 255)
            Me.checkReadOnly.Name = "checkReadOnly"
            Me.checkReadOnly.Size = New System.Drawing.Size(74, 24)
            Me.checkReadOnly.TabIndex = 11
            Me.checkReadOnly.Text = "ReadOnly"
            '
            'labelAttributes
            '
            Me.labelAttributes.Location = New System.Drawing.Point(11, 259)
            Me.labelAttributes.Name = "labelAttributes"
            Me.labelAttributes.Size = New System.Drawing.Size(77, 19)
            Me.labelAttributes.TabIndex = 10
            Me.labelAttributes.Text = "Attributes"
            '
            'labelAccessedValue
            '
            Me.labelAccessedValue.Location = New System.Drawing.Point(89, 212)
            Me.labelAccessedValue.Name = "labelAccessedValue"
            Me.labelAccessedValue.Size = New System.Drawing.Size(178, 19)
            Me.labelAccessedValue.TabIndex = 9
            '
            'labelAccessed
            '
            Me.labelAccessed.Location = New System.Drawing.Point(16, 216)
            Me.labelAccessed.Name = "labelAccessed"
            Me.labelAccessed.Size = New System.Drawing.Size(72, 19)
            Me.labelAccessed.TabIndex = 8
            Me.labelAccessed.Text = "Accessed"
            '
            'labelModifiedValue
            '
            Me.labelModifiedValue.Location = New System.Drawing.Point(90, 183)
            Me.labelModifiedValue.Name = "labelModifiedValue"
            Me.labelModifiedValue.Size = New System.Drawing.Size(178, 19)
            Me.labelModifiedValue.TabIndex = 7
            '
            'labelModified
            '
            Me.labelModified.Location = New System.Drawing.Point(16, 185)
            Me.labelModified.Name = "labelModified"
            Me.labelModified.Size = New System.Drawing.Size(72, 19)
            Me.labelModified.TabIndex = 6
            Me.labelModified.Text = "Modified"
            '
            'labelCreatedValue
            '
            Me.labelCreatedValue.Location = New System.Drawing.Point(88, 157)
            Me.labelCreatedValue.Name = "labelCreatedValue"
            Me.labelCreatedValue.Size = New System.Drawing.Size(178, 19)
            Me.labelCreatedValue.TabIndex = 5
            '
            'labelCreated
            '
            Me.labelCreated.Location = New System.Drawing.Point(15, 157)
            Me.labelCreated.Name = "labelCreated"
            Me.labelCreated.Size = New System.Drawing.Size(73, 19)
            Me.labelCreated.TabIndex = 4
            Me.labelCreated.Text = "Created"
            '
            'labelSizeValue
            '
            Me.labelSizeValue.Location = New System.Drawing.Point(88, 116)
            Me.labelSizeValue.Name = "labelSizeValue"
            Me.labelSizeValue.Size = New System.Drawing.Size(178, 19)
            Me.labelSizeValue.TabIndex = 3
            '
            'labelSize
            '
            Me.labelSize.Location = New System.Drawing.Point(16, 118)
            Me.labelSize.Name = "labelSize"
            Me.labelSize.Size = New System.Drawing.Size(77, 19)
            Me.labelSize.TabIndex = 2
            Me.labelSize.Text = "Size"
            '
            'labelLocationValue
            '
            Me.labelLocationValue.Location = New System.Drawing.Point(88, 92)
            Me.labelLocationValue.Name = "labelLocationValue"
            Me.labelLocationValue.Size = New System.Drawing.Size(178, 19)
            Me.labelLocationValue.TabIndex = 1
            '
            'labelLocation
            '
            Me.labelLocation.Location = New System.Drawing.Point(15, 92)
            Me.labelLocation.Name = "labelLocation"
            Me.labelLocation.Size = New System.Drawing.Size(77, 19)
            Me.labelLocation.TabIndex = 0
            Me.labelLocation.Text = "Location"
            '
            'groupLine2
            '
            Me.groupLine2.Location = New System.Drawing.Point(13, 138)
            Me.groupLine2.Name = "groupLine2"
            Me.groupLine2.Size = New System.Drawing.Size(311, 8)
            Me.groupLine2.TabIndex = 18
            Me.groupLine2.TabStop = False
            '
            'tabPageSecurity
            '
            Me.tabPageSecurity.Controls.Add(Me.panelPermissions)
            Me.tabPageSecurity.Controls.Add(Me.buttonGroupsUsersRemove)
            Me.tabPageSecurity.Controls.Add(Me.buttonGroupsUsersAdd)
            Me.tabPageSecurity.Controls.Add(Me.listGroupsUsers)
            Me.tabPageSecurity.Controls.Add(Me.labelGroupsUsers)
            Me.tabPageSecurity.Location = New System.Drawing.Point(4, 22)
            Me.tabPageSecurity.Name = "tabPageSecurity"
            Me.tabPageSecurity.Padding = New System.Windows.Forms.Padding(3)
            Me.tabPageSecurity.Size = New System.Drawing.Size(329, 356)
            Me.tabPageSecurity.TabIndex = 1
            Me.tabPageSecurity.Text = "Security"
            '
            'panelPermissions
            '
            Me.panelPermissions.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
            Me.panelPermissions.Controls.Add(Me.labelWrite)
            Me.panelPermissions.Controls.Add(Me.labelRead)
            Me.panelPermissions.Controls.Add(Me.labelReadAndExecute)
            Me.panelPermissions.Controls.Add(Me.labelModify)
            Me.panelPermissions.Controls.Add(Me.labelFullControl)
            Me.panelPermissions.Controls.Add(Me.checkDenyWrite)
            Me.panelPermissions.Controls.Add(Me.checkAllowWrite)
            Me.panelPermissions.Controls.Add(Me.checkDenyRead)
            Me.panelPermissions.Controls.Add(Me.checkAllowRead)
            Me.panelPermissions.Controls.Add(Me.checkDenyReadAndExecute)
            Me.panelPermissions.Controls.Add(Me.checkAllowReadAndExecute)
            Me.panelPermissions.Controls.Add(Me.checkDenyModify)
            Me.panelPermissions.Controls.Add(Me.checkAllowModify)
            Me.panelPermissions.Controls.Add(Me.checkDenyFull)
            Me.panelPermissions.Controls.Add(Me.checkAllowFull)
            Me.panelPermissions.Controls.Add(Me.labelPermissions)
            Me.panelPermissions.Controls.Add(Me.labelDeny)
            Me.panelPermissions.Controls.Add(Me.labelAllow)
            Me.panelPermissions.Location = New System.Drawing.Point(12, 205)
            Me.panelPermissions.Name = "panelPermissions"
            Me.panelPermissions.Size = New System.Drawing.Size(299, 142)
            Me.panelPermissions.TabIndex = 27
            '
            'labelWrite
            '
            Me.labelWrite.Location = New System.Drawing.Point(48, 117)
            Me.labelWrite.Name = "labelWrite"
            Me.labelWrite.Size = New System.Drawing.Size(100, 16)
            Me.labelWrite.TabIndex = 25
            Me.labelWrite.Text = "Write"
            '
            'labelRead
            '
            Me.labelRead.Location = New System.Drawing.Point(49, 94)
            Me.labelRead.Name = "labelRead"
            Me.labelRead.Size = New System.Drawing.Size(100, 16)
            Me.labelRead.TabIndex = 24
            Me.labelRead.Text = "Read"
            '
            'labelReadAndExecute
            '
            Me.labelReadAndExecute.Location = New System.Drawing.Point(48, 71)
            Me.labelReadAndExecute.Name = "labelReadAndExecute"
            Me.labelReadAndExecute.Size = New System.Drawing.Size(100, 16)
            Me.labelReadAndExecute.TabIndex = 23
            Me.labelReadAndExecute.Text = "Read && Execute"
            '
            'labelModify
            '
            Me.labelModify.Location = New System.Drawing.Point(48, 48)
            Me.labelModify.Name = "labelModify"
            Me.labelModify.Size = New System.Drawing.Size(100, 16)
            Me.labelModify.TabIndex = 22
            Me.labelModify.Text = "Modify"
            '
            'labelFullControl
            '
            Me.labelFullControl.Location = New System.Drawing.Point(49, 25)
            Me.labelFullControl.Name = "labelFullControl"
            Me.labelFullControl.Size = New System.Drawing.Size(99, 16)
            Me.labelFullControl.TabIndex = 21
            Me.labelFullControl.Text = "Full Control"
            '
            'checkDenyWrite
            '
            Me.checkDenyWrite.Location = New System.Drawing.Point(235, 117)
            Me.checkDenyWrite.Name = "checkDenyWrite"
            Me.checkDenyWrite.Size = New System.Drawing.Size(16, 16)
            Me.checkDenyWrite.TabIndex = 18
            '
            'checkAllowWrite
            '
            Me.checkAllowWrite.Location = New System.Drawing.Point(179, 117)
            Me.checkAllowWrite.Name = "checkAllowWrite"
            Me.checkAllowWrite.Size = New System.Drawing.Size(16, 16)
            Me.checkAllowWrite.TabIndex = 17
            '
            'checkDenyRead
            '
            Me.checkDenyRead.Location = New System.Drawing.Point(235, 94)
            Me.checkDenyRead.Name = "checkDenyRead"
            Me.checkDenyRead.Size = New System.Drawing.Size(16, 16)
            Me.checkDenyRead.TabIndex = 16
            '
            'checkAllowRead
            '
            Me.checkAllowRead.Location = New System.Drawing.Point(179, 94)
            Me.checkAllowRead.Name = "checkAllowRead"
            Me.checkAllowRead.Size = New System.Drawing.Size(16, 16)
            Me.checkAllowRead.TabIndex = 15
            '
            'checkDenyReadAndExecute
            '
            Me.checkDenyReadAndExecute.Location = New System.Drawing.Point(235, 71)
            Me.checkDenyReadAndExecute.Name = "checkDenyReadAndExecute"
            Me.checkDenyReadAndExecute.Size = New System.Drawing.Size(16, 16)
            Me.checkDenyReadAndExecute.TabIndex = 14
            '
            'checkAllowReadAndExecute
            '
            Me.checkAllowReadAndExecute.Location = New System.Drawing.Point(179, 71)
            Me.checkAllowReadAndExecute.Name = "checkAllowReadAndExecute"
            Me.checkAllowReadAndExecute.Size = New System.Drawing.Size(16, 16)
            Me.checkAllowReadAndExecute.TabIndex = 13
            '
            'checkDenyModify
            '
            Me.checkDenyModify.Location = New System.Drawing.Point(235, 48)
            Me.checkDenyModify.Name = "checkDenyModify"
            Me.checkDenyModify.Size = New System.Drawing.Size(16, 16)
            Me.checkDenyModify.TabIndex = 12
            '
            'checkAllowModify
            '
            Me.checkAllowModify.Location = New System.Drawing.Point(179, 48)
            Me.checkAllowModify.Name = "checkAllowModify"
            Me.checkAllowModify.Size = New System.Drawing.Size(16, 16)
            Me.checkAllowModify.TabIndex = 11
            '
            'checkDenyFull
            '
            Me.checkDenyFull.Location = New System.Drawing.Point(235, 25)
            Me.checkDenyFull.Name = "checkDenyFull"
            Me.checkDenyFull.Size = New System.Drawing.Size(16, 16)
            Me.checkDenyFull.TabIndex = 10
            '
            'checkAllowFull
            '
            Me.checkAllowFull.Location = New System.Drawing.Point(179, 25)
            Me.checkAllowFull.Name = "checkAllowFull"
            Me.checkAllowFull.Size = New System.Drawing.Size(16, 16)
            Me.checkAllowFull.TabIndex = 9
            '
            'labelPermissions
            '
            Me.labelPermissions.Location = New System.Drawing.Point(36, 3)
            Me.labelPermissions.Name = "labelPermissions"
            Me.labelPermissions.Size = New System.Drawing.Size(99, 16)
            Me.labelPermissions.TabIndex = 8
            Me.labelPermissions.Text = "Permissions for "
            '
            'labelDeny
            '
            Me.labelDeny.Location = New System.Drawing.Point(228, 3)
            Me.labelDeny.Name = "labelDeny"
            Me.labelDeny.Size = New System.Drawing.Size(34, 16)
            Me.labelDeny.TabIndex = 6
            Me.labelDeny.Text = "Deny"
            '
            'labelAllow
            '
            Me.labelAllow.Location = New System.Drawing.Point(171, 3)
            Me.labelAllow.Name = "labelAllow"
            Me.labelAllow.Size = New System.Drawing.Size(34, 16)
            Me.labelAllow.TabIndex = 5
            Me.labelAllow.Text = "Allow"
            '
            'buttonGroupsUsersRemove
            '
            Me.buttonGroupsUsersRemove.Location = New System.Drawing.Point(236, 162)
            Me.buttonGroupsUsersRemove.Name = "buttonGroupsUsersRemove"
            Me.buttonGroupsUsersRemove.Size = New System.Drawing.Size(75, 30)
            Me.buttonGroupsUsersRemove.TabIndex = 3
            Me.buttonGroupsUsersRemove.Text = "Remove"
            '
            'buttonGroupsUsersAdd
            '
            Me.buttonGroupsUsersAdd.Location = New System.Drawing.Point(154, 162)
            Me.buttonGroupsUsersAdd.Name = "buttonGroupsUsersAdd"
            Me.buttonGroupsUsersAdd.Size = New System.Drawing.Size(75, 30)
            Me.buttonGroupsUsersAdd.TabIndex = 2
            Me.buttonGroupsUsersAdd.Text = "Add"
            '
            'listGroupsUsers
            '
            Me.listGroupsUsers.FormattingEnabled = True
            Me.listGroupsUsers.Location = New System.Drawing.Point(11, 23)
            Me.listGroupsUsers.Name = "listGroupsUsers"
            Me.listGroupsUsers.Size = New System.Drawing.Size(300, 134)
            Me.listGroupsUsers.TabIndex = 1
            '
            'labelGroupsUsers
            '
            Me.labelGroupsUsers.Location = New System.Drawing.Point(10, 9)
            Me.labelGroupsUsers.Name = "labelGroupsUsers"
            Me.labelGroupsUsers.Size = New System.Drawing.Size(204, 16)
            Me.labelGroupsUsers.TabIndex = 0
            Me.labelGroupsUsers.Text = "Group or user names:"
            '
            'buttonOK
            '
            Me.buttonOK.Location = New System.Drawing.Point(98, 389)
            Me.buttonOK.Name = "buttonOK"
            Me.buttonOK.Size = New System.Drawing.Size(75, 30)
            Me.buttonOK.TabIndex = 1
            Me.buttonOK.Text = "OK"
            '
            'buttonCancel
            '
            Me.buttonCancel.Location = New System.Drawing.Point(262, 389)
            Me.buttonCancel.Name = "buttonCancel"
            Me.buttonCancel.Size = New System.Drawing.Size(75, 30)
            Me.buttonCancel.TabIndex = 2
            Me.buttonCancel.Text = "Cancel"
            '
            'buttonApply
            '
            Me.buttonApply.Enabled = False
            Me.buttonApply.Location = New System.Drawing.Point(180, 389)
            Me.buttonApply.Name = "buttonApply"
            Me.buttonApply.Size = New System.Drawing.Size(75, 30)
            Me.buttonApply.TabIndex = 3
            Me.buttonApply.Text = "Apply"
            '
            'FileDirectoryProperties
            '
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.ClientSize = New System.Drawing.Size(346, 426)
            Me.Controls.Add(Me.buttonApply)
            Me.Controls.Add(Me.buttonCancel)
            Me.Controls.Add(Me.buttonOK)
            Me.Controls.Add(Me.tabMain)
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "FileDirectoryProperties"
            Me.ShowInTaskbar = False
            Me.tabMain.ResumeLayout(False)
            Me.tabPageGeneral.ResumeLayout(False)
            Me.tabPageGeneral.PerformLayout()
            Me.tabPageSecurity.ResumeLayout(False)
            Me.panelPermissions.ResumeLayout(False)
            Me.ResumeLayout(False)

        End Sub

        Friend Shared ReadOnly Property GetInstance() As FileDirectoryProperties
            Get
                If m_DefaultInstance Is Nothing OrElse m_DefaultInstance.IsDisposed() Then
                    SyncLock m_SyncObject
                        If m_DefaultInstance Is Nothing OrElse m_DefaultInstance.IsDisposed() Then
                            m_DefaultInstance = New FileDirectoryProperties
                        End If
                    End SyncLock
                End If
                Return m_DefaultInstance
            End Get
        End Property

        Private Shared m_DefaultInstance As FileDirectoryProperties
        Private Shared m_SyncObject As New Object
#End Region

        Private Sub buttonOK_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonOK.Click
            If Changes <> ChangeType.None Then
                If ApplyChanges() Then
                    Me.Hide()
                End If
            End If
        End Sub

        Private Sub buttonCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonCancel.Click
            Me.Hide()
        End Sub

        Private Sub formSecurity_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

            If ItemType = FileItemType.File Then
                GetFileDetails()
            ElseIf ItemType = FileItemType.Directory Then
                GetDirectoryDetails()
                labelCreated.Visible = False
                labelCreatedValue.Visible = False
                labelAccessed.Top = labelSize.Top
                labelAccessedValue.Top = labelSize.Top
                labelSizeValue.Visible = False
                labelSize.Visible = False

                listGroupsUsers.Items.Add("Directory View/Edit of ACLs not supported... yet")
            Else
                Me.Hide()
            End If

            If ItemType = FileItemType.File Then
                GetFileSecurityDetails()
            Else
                'todo: implement the same kind of security information details for directories
                ' this function simply turns the screen off right now...
                GetDirectorySecurityDetails()
            End If

            If DefaultView = DefaultPropertiesView.General Then
                tabMain.SelectTab(0)
            ElseIf DefaultView = DefaultPropertiesView.Security Then
                tabMain.SelectTab(1)
            End If

            changeAllowed = True

            If listGroupsUsers.Items.Count > 0 Then
                listGroupsUsers.SelectedIndex = 0
            End If

            Me.Text = ItemToManipulate

        End Sub

        Private Sub GetFileSecurityDetails()
            Dim fs As FileSecurity = File.GetAccessControl(ItemToManipulate, _
                        AccessControlSections.Group Or AccessControlSections.Access Or _
                        AccessControlSections.Owner)

            Dim ac As AuthorizationRuleCollection = fs.GetAccessRules(True, True, GetType(NTAccount))

            rightsList = New Dictionary(Of String, List(Of FileSystemAccessRule))
            originalList = New Dictionary(Of String, List(Of FileSystemAccessRule))
            For Each fsar As FileSystemAccessRule In ac
                If rightsList.ContainsKey(fsar.IdentityReference.Value) Then
                    rightsList(fsar.IdentityReference.Value).Add(fsar)
                    originalList(fsar.IdentityReference.Value).Add(fsar)
                Else
                    Dim l As New List(Of FileSystemAccessRule)
                    l.Add(fsar)
                    rightsList.Add(fsar.IdentityReference.Value, l)
                    originalList.Add(fsar.IdentityReference.Value, l)
                    listGroupsUsers.Items.Add(fsar.IdentityReference.Value)
                End If
            Next

        End Sub

        Private Sub GetDirectorySecurityDetails()
            ' not implemented: simply turn off the functionality
            listGroupsUsers.Enabled = False
            buttonGroupsUsersAdd.Enabled = False
            buttonGroupsUsersRemove.Enabled = False

            checkAllowFull.Enabled = False
            checkAllowModify.Enabled = False
            checkAllowRead.Enabled = False
            checkAllowReadAndExecute.Enabled = False
            checkAllowWrite.Enabled = False

            checkDenyFull.Enabled = False
            checkDenyModify.Enabled = False
            checkDenyRead.Enabled = False
            checkDenyReadAndExecute.Enabled = False
            checkDenyWrite.Enabled = False
        End Sub

        Private Sub GetFileDetails()
            Dim fi As New FileInfo(ItemToManipulate)
            labelLocationValue.Text = fi.DirectoryName
            labelAccessedValue.Text = fi.LastAccessTime.ToString()
            labelCreatedValue.Text = fi.CreationTime.ToString()
            labelModifiedValue.Text = fi.LastWriteTime.ToString()
            Dim i As Long = fi.Length

            If fi.Length > 1024 Then
                labelSizeValue.Text = String.Format(CultureInfo.CurrentCulture, "{0:N2} KB ({1:N0} byes)", i / 1024, i)
            Else
                labelSizeValue.Text = String.Format(CultureInfo.CurrentCulture, "{0} bytes ({0:N0} byes)", i, i)
            End If
            checkReadOnly.Checked = CBool(fi.Attributes And FileAttributes.ReadOnly)
            checkHidden.Checked = CBool(fi.Attributes And FileAttributes.Hidden)
            textName.Text = fi.Name
            labelTypeValue.Text = "File"
            attributes = fi.Attributes
        End Sub

        Private Sub GetDirectoryDetails()
            Dim di As New DirectoryInfo(ItemToManipulate)
            labelLocationValue.Text = di.FullName
            labelModifiedValue.Text = di.CreationTime.ToString()
            labelModified.Text = "Created"
            labelAccessed.Text = "Contains"
            labelAccessedValue.Text = String.Format(CultureInfo.CurrentCulture, "{0} files, {1} folders", di.GetFiles().Length, di.GetDirectories().Length)
            labelTypeValue.Text = "File Folder"
            textName.Text = di.Name
            checkReadOnly.Checked = CBool(di.Attributes And FileAttributes.ReadOnly)
            checkHidden.Checked = CBool(di.Attributes And FileAttributes.Hidden)
            attributes = di.Attributes
        End Sub

        Private Sub checkReadOnly_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles checkReadOnly.CheckedChanged
            If checkReadOnly.Checked Then
                attributes = attributes Or FileAttributes.ReadOnly
            Else
                ' basically, resets the attributes value based on this value being unchecked ...
                attributes = attributes And (CType(63, FileAttributes) Xor FileAttributes.ReadOnly)
            End If
            SetChange(ChangeType.Attributes)
        End Sub

        Private Sub SetChange(ByVal changeType As ChangeType)
            If changeAllowed Then
                buttonApply.Enabled = True
                If Changes <> FileDemo.ChangeType.None Then
                    Changes = Changes Or changeType
                Else
                    Changes = changeType
                End If
            End If
        End Sub

        Private Sub checkHidden_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles checkHidden.CheckedChanged
            If checkHidden.Checked Then
                attributes = attributes Or FileAttributes.Hidden
            Else
                ' basically, resets the attributes value based on this value being unchecked ...
                attributes = attributes And (CType(63, FileAttributes) Xor FileAttributes.Hidden)
            End If
            SetChange(ChangeType.Attributes)
        End Sub

        Private Sub textName_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles textName.TextChanged
            SetChange(ChangeType.Name)
        End Sub

        Private Sub buttonApply_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonApply.Click
            If ApplyChanges() Then
                buttonApply.Enabled = False
            End If
        End Sub

        Private Function ApplyChanges() As Boolean

            If textName.Text.Trim().Length = 0 Then
                MsgBox("You cannot change the name of the file/directory to a zero-length name")
                textName.Focus()
                Return False
            End If

            If (Changes And ChangeType.Name) = ChangeType.Name Then
                If ItemType = FileItemType.File Then
                    ChangedName = New FileInfo(ItemToManipulate).DirectoryName & "\" & textName.Text
                    If ChangedName.ToLowerInvariant() <> ItemToManipulate.ToLowerInvariant() Then
                        File.Move(ItemToManipulate, ChangedName)
                    End If
                ElseIf ItemType = FileItemType.Directory Then
                    Dim di As New DirectoryInfo(ItemToManipulate)
                    ChangedName = di.FullName.Substring(0, di.FullName.Length - di.Name.Length) & textName.Text
                    If (ChangedName.ToLowerInvariant()) <> ItemToManipulate.ToLowerInvariant() Then
                        Directory.Move(ItemToManipulate, ChangedName)
                    End If
                End If
            Else
                If ItemType = FileItemType.File Then
                    ChangedName = New FileInfo(ItemToManipulate).FullName
                ElseIf ItemType = FileItemType.Directory Then
                    ChangedName = New DirectoryInfo(ItemToManipulate).FullName
                End If
            End If

            If (Changes And ChangeType.Attributes) = ChangeType.Attributes Then
                Dim fsi As FileSystemInfo
                If ItemType = FileItemType.File Then
                    fsi = New FileInfo(ChangedName)
                ElseIf ItemType = FileItemType.Directory Then
                    fsi = New DirectoryInfo(ChangedName)
                Else
                    Throw New ArgumentException("Unrecognized Item Type: " & ItemType)
                End If

                Try
                    fsi.Attributes = attributes
                Catch e As ArgumentException
                    MsgBox("You can't change the attributes on this file, you don't have appropraite permissions!")
                    Return False
                End Try
            End If

            If (Changes And ChangeType.Security) = ChangeType.Security Then
                SaveSecurity()
            End If

            Return True
        End Function

        Private Sub SaveSecurity()
            ' because of the way I track this, I have to do a Remove of all the non-inherited
            ' existing acls on the file, and then do an add, for the current state
            ' If I don't do this remove, then removing someone's existing rights is not
            ' picked up by the changes I put onto the file.
            ' You could choose to keep track of the changes on the file in a different manner
            Dim fs As New FileSecurity
            For Each kvp As KeyValuePair(Of String, List(Of FileSystemAccessRule)) In originalList
                For Each fsar As FileSystemAccessRule In kvp.Value
                    If Not fsar.IsInherited Then
                        fs.RemoveAccessRule(fsar)
                    End If
                Next
            Next
            File.SetAccessControl(ItemToManipulate, fs)

            fs = New FileSecurity
            For Each kvp As KeyValuePair(Of String, List(Of FileSystemAccessRule)) In rightsList
                For Each fsar As FileSystemAccessRule In kvp.Value
                    If Not fsar.IsInherited Then
                        fs.AddAccessRule(fsar)
                    End If
                Next
            Next
            File.SetAccessControl(ItemToManipulate, fs)

        End Sub

        Private Sub listGroupsUsers_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles listGroupsUsers.SelectedIndexChanged

            If listGroupsUsers.SelectedIndex >= 0 And ItemType <> FileItemType.Directory Then
                If changeAllowed Then
                    makingCheckChange = True
                    UnCheckAll()
                    For Each kvp As KeyValuePair(Of String, List(Of FileSystemAccessRule)) In rightsList
                        If kvp.Key = listGroupsUsers.SelectedItem.ToString() Then
                            For Each fsar As FileSystemAccessRule In kvp.Value
                                If fsar.AccessControlType = AccessControlType.Allow Then
                                    If (fsar.IsInherited) Then
                                        If (fsar.FileSystemRights And FileRightShortCuts.FullControl) = FileRightShortCuts.FullControl Then
                                            checkAllowFull.Enabled = False
                                        End If
                                        If (fsar.FileSystemRights And FileRightShortCuts.Modify) = FileRightShortCuts.Modify Then
                                            checkAllowModify.Enabled = False
                                        End If
                                        If (fsar.FileSystemRights And FileRightShortCuts.Read) = FileRightShortCuts.Read Then
                                            checkAllowRead.Enabled = False
                                        End If
                                        If (fsar.FileSystemRights And FileRightShortCuts.ReadAndExecute) = FileRightShortCuts.ReadAndExecute Then
                                            checkAllowReadAndExecute.Enabled = False
                                        End If
                                        If (fsar.FileSystemRights And FileRightShortCuts.Write) = FileRightShortCuts.Write Then
                                            checkAllowWrite.Enabled = False
                                        End If
                                    End If

                                    checkAllowFull.Checked = (fsar.FileSystemRights And FileRightShortCuts.FullControl) = FileRightShortCuts.FullControl
                                    checkAllowModify.Checked = (fsar.FileSystemRights And FileRightShortCuts.Modify) = FileRightShortCuts.Modify
                                    checkAllowRead.Checked = (fsar.FileSystemRights And FileRightShortCuts.Read) = FileRightShortCuts.Read
                                    checkAllowReadAndExecute.Checked = (fsar.FileSystemRights And FileRightShortCuts.ReadAndExecute) = FileRightShortCuts.ReadAndExecute
                                    checkAllowWrite.Checked = (fsar.FileSystemRights And FileRightShortCuts.Write) = FileRightShortCuts.Write
                                Else
                                    If (fsar.IsInherited) Then
                                        If (fsar.FileSystemRights And FileRightShortCuts.FullControl) = FileRightShortCuts.FullControl Then
                                            checkDenyFull.Enabled = False
                                        End If
                                        If (fsar.FileSystemRights And FileRightShortCuts.Modify) = FileRightShortCuts.Modify Then
                                            checkDenyModify.Enabled = False
                                        End If
                                        If (fsar.FileSystemRights And FileRightShortCuts.Read) = FileRightShortCuts.Read Then
                                            checkDenyRead.Enabled = False
                                        End If
                                        If (fsar.FileSystemRights And FileRightShortCuts.ReadAndExecute) = FileRightShortCuts.ReadAndExecute Then
                                            checkDenyReadAndExecute.Enabled = False
                                        End If
                                        If (fsar.FileSystemRights And FileRightShortCuts.Write) = FileRightShortCuts.Write Then
                                            checkDenyWrite.Enabled = False
                                        End If
                                    End If
                                    checkDenyFull.Checked = (fsar.FileSystemRights And FileRightShortCuts.FullControl) = FileRightShortCuts.FullControl
                                    checkDenyModify.Checked = (fsar.FileSystemRights And FileRightShortCuts.Modify) = FileRightShortCuts.Modify
                                    checkDenyRead.Checked = (fsar.FileSystemRights And FileRightShortCuts.Read) = FileRightShortCuts.Read

                                    checkDenyReadAndExecute.Checked = (fsar.FileSystemRights And FileRightShortCuts.ReadAndExecute) = FileRightShortCuts.ReadAndExecute
                                    checkDenyWrite.Checked = (fsar.FileSystemRights And FileRightShortCuts.Write) = FileRightShortCuts.Write
                                End If
                            Next
                            Exit For
                        End If
                    Next
                    makingCheckChange = False
                End If
                oldUserGroupIndex = listGroupsUsers.SelectedIndex
            End If


        End Sub

        Private Sub UnCheckAll()
            checkDenyFull.Checked = False
            checkDenyModify.Checked = False
            checkDenyReadAndExecute.Checked = False
            checkDenyRead.Checked = False
            checkDenyWrite.Checked = False

            checkAllowFull.Checked = False
            checkAllowModify.Checked = False
            checkAllowReadAndExecute.Checked = False
            checkAllowRead.Checked = False
            checkAllowWrite.Checked = False

            checkDenyFull.Enabled = True
            checkDenyModify.Enabled = True
            checkDenyReadAndExecute.Enabled = True
            checkDenyRead.Enabled = True
            checkDenyWrite.Enabled = True

            checkAllowFull.Enabled = True
            checkAllowModify.Enabled = True
            checkAllowReadAndExecute.Enabled = True
            checkAllowRead.Enabled = True
            checkAllowWrite.Enabled = True
        End Sub

        Private Sub ChangeCheckStatus(ByVal check As Boolean, ByVal ParamArray items() As CheckBox)
            For Each c As CheckBox In items
                If (c.Enabled) Then
                    c.Checked = check
                    Changes = ChangeType.Security
                End If
            Next
        End Sub

        Private Sub UpdateRightsList(ByVal index As Integer)
            If changeAllowed Then
                For Each kvp As KeyValuePair(Of String, List(Of FileSystemAccessRule)) In rightsList
                    If kvp.Key = listGroupsUsers.Items(index).ToString() Then
                        Dim allowRule As FileSystemAccessRule = Nothing
                        Dim denyRule As FileSystemAccessRule = Nothing
                        For Each fsar As FileSystemAccessRule In kvp.Value
                            If fsar.AccessControlType = AccessControlType.Allow Then
                                allowRule = GetAllowRights(fsar)
                            Else
                                denyRule = GetDenyRights(fsar)
                            End If
                        Next

                        Dim l As New List(Of FileSystemAccessRule)
                        If Not allowRule Is Nothing Then
                            l.Add(allowRule)
                        Else
                            allowRule = GetAllowRights(Nothing, kvp.Key)
                            If Not allowRule Is Nothing Then
                                l.Add(allowRule)
                            End If
                        End If
                        If Not denyRule Is Nothing Then
                            l.Add(denyRule)
                        Else
                            denyRule = GetDenyRights(Nothing, kvp.Key)
                            If Not denyRule Is Nothing Then
                                l.Add(denyRule)
                            End If
                        End If

                        ' this code is essential. Basically, you can't edit any inherited rules, IN
                        ' a given item (a file in this case). Because of this,
                        ' we need to ensure they are preserved, or else it won't be clear
                        ' if the user revisits this particular entry, whether the item was inherited
                        For Each fsarToBeMoved As FileSystemAccessRule In kvp.Value
                            If fsarToBeMoved.IsInherited Then
                                l.Add(fsarToBeMoved)
                            End If
                        Next

                        rightsList.Remove(kvp.Key)
                        rightsList.Add(kvp.Key, l)

                        Exit For
                    End If
                Next
            End If
        End Sub

        Private Function GetAllowRights(ByVal fsar As FileSystemAccessRule, Optional ByVal id As String = Nothing) As FileSystemAccessRule
            Dim fsr As New FileSystemRights
            If checkAllowFull.Checked And checkAllowFull.Enabled Then
                fsr = fsr Or CType(FileRightShortCuts.FullControl, FileSystemRights)
            End If
            If checkAllowModify.Checked And checkAllowModify.Enabled Then
                fsr = fsr Or CType(FileRightShortCuts.Modify, FileSystemRights)
            End If
            If checkAllowReadAndExecute.Checked And checkAllowReadAndExecute.Enabled Then
                fsr = fsr Or CType(FileRightShortCuts.ReadAndExecute, FileSystemRights)
            End If
            If checkAllowRead.Checked And checkAllowRead.Enabled Then
                fsr = fsr Or CType(FileRightShortCuts.Read, FileSystemRights)
            End If
            If checkAllowWrite.Checked And checkAllowWrite.Enabled Then
                fsr = fsr Or CType(FileRightShortCuts.Write, FileSystemRights)
            End If

            If fsr <> 0 Then
                If id Is Nothing Then
                    Return New FileSystemAccessRule(fsar.IdentityReference, fsr, fsar.InheritanceFlags, fsar.PropagationFlags, fsar.AccessControlType)
                Else
                    Return New FileSystemAccessRule(id, fsr, InheritanceFlags.None, PropagationFlags.None, AccessControlType.Allow)
                End If
            Else
                Return Nothing
            End If
        End Function

        Private Function GetDenyRights(ByVal fsar As FileSystemAccessRule, Optional ByVal id As String = Nothing) As FileSystemAccessRule
            Dim fsr As New FileSystemRights
            If checkDenyFull.Checked And checkDenyFull.Enabled Then
                fsr = fsr Or CType(FileRightShortCuts.FullControl, FileSystemRights)
            End If
            If checkDenyModify.Checked And checkDenyModify.Enabled Then
                fsr = fsr Or CType(FileRightShortCuts.Modify, FileSystemRights)
            End If
            If checkDenyReadAndExecute.Checked And checkDenyReadAndExecute.Enabled Then
                fsr = fsr Or CType(FileRightShortCuts.ReadAndExecute, FileSystemRights)
            End If
            If checkDenyRead.Checked And checkDenyRead.Enabled Then
                fsr = fsr Or CType(FileRightShortCuts.Read, FileSystemRights)
            End If
            If checkDenyWrite.Checked And checkDenyWrite.Enabled Then
                fsr = fsr Or CType(FileRightShortCuts.Write, FileSystemRights)
            End If

            If fsr <> 0 Then
                If id Is Nothing Then
                    Return New FileSystemAccessRule(fsar.IdentityReference, fsr, fsar.InheritanceFlags, fsar.PropagationFlags, fsar.AccessControlType)
                Else
                    Return New FileSystemAccessRule(id, fsr, InheritanceFlags.None, PropagationFlags.None, AccessControlType.Deny)
                End If
                'End If
            Else
                Return Nothing
            End If
        End Function

        Private Sub checkAllowFull_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles checkAllowFull.CheckedChanged
            If Not makingCheckChange Then
                SetChange(ChangeType.Security)
                makingCheckChange = True
                If checkAllowFull.Checked Then
                    ChangeCheckStatus(True, checkAllowModify, checkAllowRead, checkAllowReadAndExecute, checkAllowWrite)
                    ChangeCheckStatus(False, checkDenyFull, checkDenyModify, checkDenyRead, checkDenyReadAndExecute, checkDenyWrite)
                End If
                UpdateRightsList(oldUserGroupIndex)
                Thread.Sleep(0)
                makingCheckChange = False
            End If
        End Sub

        Private Sub checkDenyFull_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles checkDenyFull.CheckedChanged
            If Not makingCheckChange Then
                SetChange(ChangeType.Security)
                makingCheckChange = True
                If checkDenyFull.Checked Then
                    ChangeCheckStatus(False, checkAllowFull, checkAllowModify, checkAllowRead, checkAllowReadAndExecute, checkAllowWrite)
                    ChangeCheckStatus(True, checkDenyModify, checkDenyRead, checkDenyReadAndExecute, checkDenyWrite)
                End If
                UpdateRightsList(oldUserGroupIndex)
                Thread.Sleep(0)
                makingCheckChange = False
            End If
        End Sub

        Private Sub checkAllowModify_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles checkAllowModify.CheckedChanged
            If Not makingCheckChange Then
                SetChange(ChangeType.Security)
                makingCheckChange = True
                If checkAllowModify.Checked Then
                    ChangeCheckStatus(True, checkAllowRead, checkAllowReadAndExecute, checkAllowWrite)
                    ChangeCheckStatus(False, checkDenyFull, checkDenyModify, checkDenyRead, checkDenyReadAndExecute, checkDenyWrite)
                Else
                    ChangeCheckStatus(False, checkAllowFull)
                End If
                UpdateRightsList(oldUserGroupIndex)
                Thread.Sleep(0)
                makingCheckChange = False
            End If
        End Sub

        Private Sub checkDenyModify_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles checkDenyModify.CheckedChanged
            If Not makingCheckChange Then
                SetChange(ChangeType.Security)
                makingCheckChange = True
                If checkDenyModify.Checked Then
                    ChangeCheckStatus(True, checkDenyRead, checkDenyReadAndExecute, checkDenyWrite)
                    ChangeCheckStatus(False, checkAllowFull, checkAllowModify, checkAllowRead, checkAllowReadAndExecute, checkAllowWrite)
                Else
                    ChangeCheckStatus(False, checkDenyFull)
                End If
                UpdateRightsList(oldUserGroupIndex)
                Thread.Sleep(0)
                makingCheckChange = False
            End If
        End Sub

        Private Sub checkAllowReadAndExecute_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles checkAllowReadAndExecute.CheckedChanged
            If Not makingCheckChange Then
                SetChange(ChangeType.Security)
                makingCheckChange = True
                If checkAllowReadAndExecute.Checked Then
                    ChangeCheckStatus(False, checkDenyFull, checkDenyModify, checkDenyRead, checkDenyReadAndExecute)
                    ChangeCheckStatus(True, checkAllowRead)
                Else
                    ChangeCheckStatus(False, checkAllowFull, checkAllowModify)
                End If
                UpdateRightsList(oldUserGroupIndex)
                Thread.Sleep(0)
                makingCheckChange = False
            End If
        End Sub

        Private Sub checkDenyReadAndExecute_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles checkDenyReadAndExecute.CheckedChanged
            If Not makingCheckChange Then
                SetChange(ChangeType.Security)
                makingCheckChange = True
                If checkDenyReadAndExecute.Checked Then
                    ChangeCheckStatus(False, checkAllowFull, checkAllowModify, checkAllowRead, checkAllowReadAndExecute)
                    ChangeCheckStatus(True, checkDenyRead)
                Else
                    ChangeCheckStatus(False, checkDenyFull, checkDenyModify)
                End If
                UpdateRightsList(oldUserGroupIndex)
                Thread.Sleep(0)
                makingCheckChange = False
            End If
        End Sub

        Private Sub checkAllowRead_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles checkAllowRead.CheckedChanged
            If Not makingCheckChange Then
                SetChange(ChangeType.Security)
                makingCheckChange = True
                If checkAllowRead.Checked Then
                    ChangeCheckStatus(False, checkDenyFull, checkDenyModify, checkDenyRead, checkDenyReadAndExecute)
                Else
                    ChangeCheckStatus(False, checkAllowFull, checkAllowModify, checkAllowReadAndExecute)
                End If
                UpdateRightsList(oldUserGroupIndex)
                Thread.Sleep(0)
                makingCheckChange = False
            End If
        End Sub

        Private Sub checkDenyRead_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles checkDenyRead.CheckedChanged
            If Not makingCheckChange Then
                SetChange(ChangeType.Security)
                makingCheckChange = True
                If checkDenyRead.Checked Then
                    ChangeCheckStatus(False, checkAllowFull, checkAllowModify, checkAllowRead, checkAllowReadAndExecute)
                Else
                    ChangeCheckStatus(False, checkDenyFull, checkDenyModify, checkDenyReadAndExecute)
                End If
                UpdateRightsList(oldUserGroupIndex)
                Thread.Sleep(0)
                makingCheckChange = False
            End If
        End Sub

        Private Sub checkAllowWrite_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles checkAllowWrite.CheckedChanged
            If Not makingCheckChange Then
                SetChange(ChangeType.Security)
                makingCheckChange = True
                If checkAllowWrite.Checked Then
                    ChangeCheckStatus(False, checkDenyFull, checkDenyModify, checkDenyWrite)
                Else
                    ChangeCheckStatus(False, checkAllowFull, checkAllowModify)
                End If
                UpdateRightsList(oldUserGroupIndex)
                Thread.Sleep(0)
                makingCheckChange = False
            End If
        End Sub

        Private Sub checkDenyWrite_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles checkDenyWrite.CheckedChanged
            If Not makingCheckChange Then
                SetChange(ChangeType.Security)
                makingCheckChange = True
                If checkDenyWrite.Checked Then
                    ChangeCheckStatus(False, checkAllowFull, checkAllowModify, checkAllowWrite)
                Else
                    ChangeCheckStatus(False, checkDenyFull, checkDenyModify)
                End If

                UpdateRightsList(oldUserGroupIndex)
                Thread.Sleep(0)
                makingCheckChange = False
            End If
        End Sub

        Private Sub buttonGroupsUsersRemove_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonGroupsUsersRemove.Click
            ' Note: you cannot remove any inherited acls. If all there are is inherited acl's, 
            ' then show the user a msgbox indicating this
            Dim itemsRemoved As Boolean = False
            For Each kvp As KeyValuePair(Of String, List(Of FileSystemAccessRule)) In rightsList
                If kvp.Key = listGroupsUsers.Items(listGroupsUsers.SelectedIndex).ToString() Then
                    Dim inheritedsExisted As Boolean = False
                    For i As Integer = kvp.Value.Count - 1 To 0 Step -1

                        If kvp.Value(i).IsInherited Then
                            inheritedsExisted = True
                        Else
                            kvp.Value.RemoveAt(i)
                            Changes = Changes Or ChangeType.Security
                            itemsRemoved = True
                        End If
                    Next

                    If itemsRemoved Then
                        If Not inheritedsExisted Then
                            rightsList.Remove(kvp.Key)
                            listGroupsUsers.Items.RemoveAt(listGroupsUsers.SelectedIndex)
                            If listGroupsUsers.Items.Count > 0 Then
                                makingCheckChange = True
                                listGroupsUsers.SelectedIndex = 0

                                makingCheckChange = False
                            End If
                        Else
                            ' simply reset the selectedindex, to ensure that the display
                            ' shows the changes...
                            Dim temp As Integer = listGroupsUsers.SelectedIndex
                            listGroupsUsers.SelectedIndex = -1
                            listGroupsUsers.SelectedIndex = temp
                        End If
                    Else
                        If inheritedsExisted Then
                            MsgBox("You cannot remove an item which wholly inherits its access control")
                        End If

                    End If

                    Exit For
                End If
            Next
        End Sub

        Private Sub buttonGroupsUsersAdd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonGroupsUsersAdd.Click
            Dim entry As String = InputBox("Please enter the loginname, or domain\loginname of the person to add rights for", "Add user to file")
            If entry.Trim().Length > 0 Then
                entry = entry.ToUpperInvariant()
                Dim l As New List(Of FileSystemAccessRule)
                l.Add(New FileSystemAccessRule(entry, CType(FileRightShortCuts.ReadAndExecute, FileSystemRights), AccessControlType.Allow))
                rightsList.Add(entry, l)
                listGroupsUsers.SelectedIndex = listGroupsUsers.Items.Add(entry)
                Changes = ChangeType.Security
                buttonApply.Enabled = True
            End If
        End Sub
    End Class

    <Flags()> _
    Enum ChangeType
        None = 1
        Security = 2
        Name = 4
        Attributes = 8
    End Enum
End Namespace
