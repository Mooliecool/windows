
'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
' 
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'---------------------------------------------------------------------
<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class LoadFromStore
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.label1 = New System.Windows.Forms.Label
        Me.workflowList = New System.Windows.Forms.ListBox
        Me.label2 = New System.Windows.Forms.Label
        Me.profileVersionList = New System.Windows.Forms.ListBox
        Me.ok = New System.Windows.Forms.Button
        Me.cancel = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.label1.Location = New System.Drawing.Point(13, 13)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(132, 13)
        Me.label1.TabIndex = 4
        Me.label1.Text = "1. Choose a Workflow"
        '
        'workflowList
        '
        Me.workflowList.FormattingEnabled = True
        Me.workflowList.Location = New System.Drawing.Point(16, 30)
        Me.workflowList.Name = "workflowList"
        Me.workflowList.Size = New System.Drawing.Size(469, 199)
        Me.workflowList.TabIndex = 0
        '
        'label2
        '
        Me.label2.AutoSize = True
        Me.label2.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.label2.Location = New System.Drawing.Point(16, 236)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(119, 13)
        Me.label2.TabIndex = 5
        Me.label2.Text = "2.  Choose a Profile"
        '
        'profileVersionList
        '
        Me.profileVersionList.FormattingEnabled = True
        Me.profileVersionList.Location = New System.Drawing.Point(19, 253)
        Me.profileVersionList.Name = "profileVersionList"
        Me.profileVersionList.Size = New System.Drawing.Size(467, 69)
        Me.profileVersionList.TabIndex = 6
        '
        'ok
        '
        Me.ok.Location = New System.Drawing.Point(324, 326)
        Me.ok.Name = "ok"
        Me.ok.Size = New System.Drawing.Size(75, 23)
        Me.ok.TabIndex = 5
        Me.ok.Text = "&OK"
        Me.ok.UseVisualStyleBackColor = True
        '
        'cancel
        '
        Me.cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.cancel.Location = New System.Drawing.Point(406, 326)
        Me.cancel.Name = "cancel"
        Me.cancel.Size = New System.Drawing.Size(75, 23)
        Me.cancel.TabIndex = 6
        Me.cancel.Text = "&Cancel"
        Me.cancel.UseVisualStyleBackColor = True
        '
        'LoadFromStore
        '
        Me.AcceptButton = Me.ok
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(497, 363)
        Me.Controls.Add(Me.cancel)
        Me.Controls.Add(Me.ok)
        Me.Controls.Add(Me.profileVersionList)
        Me.Controls.Add(Me.label2)
        Me.Controls.Add(Me.workflowList)
        Me.Controls.Add(Me.label1)
        Me.Name = "LoadFromStore"
        Me.Text = "Load Workflow And Profile From Store"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents label1 As System.Windows.Forms.Label
    Private WithEvents workflowList As System.Windows.Forms.ListBox
    Private WithEvents label2 As System.Windows.Forms.Label
    Friend WithEvents profileVersionList As System.Windows.Forms.ListBox
    Private WithEvents ok As System.Windows.Forms.Button
    Private WithEvents cancel As System.Windows.Forms.Button
End Class
