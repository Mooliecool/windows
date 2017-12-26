'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
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
'---------------------------------------------------------------------

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Mainform
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
        Me.components = New System.ComponentModel.Container
        Me.GroupBoxCreateNewRequest = New System.Windows.Forms.GroupBox
        Me.ButtonCreateNewRequest = New System.Windows.Forms.Button
        Me.TextBoxApprover = New System.Windows.Forms.TextBox
        Me.Label1 = New System.Windows.Forms.Label
        Me.GroupBoxExistingRequests = New System.Windows.Forms.GroupBox
        Me.ListViewExistingRequests = New System.Windows.Forms.ListView
        Me.ColumnHeaderApprover = New System.Windows.Forms.ColumnHeader
        Me.ColumnHeaderRequestId = New System.Windows.Forms.ColumnHeader
        Me.ContextMenuListViewExistingWorkflows = New System.Windows.Forms.ContextMenuStrip(Me.components)
        Me.ApproveDocumentToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.GroupBoxCreateNewRequest.SuspendLayout()
        Me.GroupBoxExistingRequests.SuspendLayout()
        Me.ContextMenuListViewExistingWorkflows.SuspendLayout()
        Me.SuspendLayout()
        '
        'GroupBoxCreateNewRequest
        '
        Me.GroupBoxCreateNewRequest.Controls.Add(Me.ButtonCreateNewRequest)
        Me.GroupBoxCreateNewRequest.Controls.Add(Me.TextBoxApprover)
        Me.GroupBoxCreateNewRequest.Controls.Add(Me.Label1)
        Me.GroupBoxCreateNewRequest.Location = New System.Drawing.Point(13, 13)
        Me.GroupBoxCreateNewRequest.Name = "GroupBoxCreateNewRequest"
        Me.GroupBoxCreateNewRequest.Size = New System.Drawing.Size(380, 71)
        Me.GroupBoxCreateNewRequest.TabIndex = 0
        Me.GroupBoxCreateNewRequest.TabStop = False
        Me.GroupBoxCreateNewRequest.Text = "Create New Request"
        '
        'ButtonCreateNewRequest
        '
        Me.ButtonCreateNewRequest.Location = New System.Drawing.Point(256, 35)
        Me.ButtonCreateNewRequest.Name = "ButtonCreateNewRequest"
        Me.ButtonCreateNewRequest.Size = New System.Drawing.Size(118, 23)
        Me.ButtonCreateNewRequest.TabIndex = 2
        Me.ButtonCreateNewRequest.Text = "Create"
        Me.ButtonCreateNewRequest.UseVisualStyleBackColor = True
        '
        'TextBoxApprover
        '
        Me.TextBoxApprover.Location = New System.Drawing.Point(10, 37)
        Me.TextBoxApprover.Name = "TextBoxApprover"
        Me.TextBoxApprover.Size = New System.Drawing.Size(240, 20)
        Me.TextBoxApprover.TabIndex = 1
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(7, 20)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(50, 13)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "Approver"
        '
        'GroupBoxExistingRequests
        '
        Me.GroupBoxExistingRequests.Controls.Add(Me.ListViewExistingRequests)
        Me.GroupBoxExistingRequests.Location = New System.Drawing.Point(13, 91)
        Me.GroupBoxExistingRequests.Name = "GroupBoxExistingRequests"
        Me.GroupBoxExistingRequests.Size = New System.Drawing.Size(380, 250)
        Me.GroupBoxExistingRequests.TabIndex = 1
        Me.GroupBoxExistingRequests.TabStop = False
        Me.GroupBoxExistingRequests.Text = "Existing Requests"
        '
        'ListViewExistingRequests
        '
        Me.ListViewExistingRequests.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.ColumnHeaderApprover, Me.ColumnHeaderRequestId})
        Me.ListViewExistingRequests.ContextMenuStrip = Me.ContextMenuListViewExistingWorkflows
        Me.ListViewExistingRequests.FullRowSelect = True
        Me.ListViewExistingRequests.Location = New System.Drawing.Point(7, 20)
        Me.ListViewExistingRequests.Name = "ListViewExistingRequests"
        Me.ListViewExistingRequests.Size = New System.Drawing.Size(367, 224)
        Me.ListViewExistingRequests.TabIndex = 0
        Me.ListViewExistingRequests.UseCompatibleStateImageBehavior = False
        Me.ListViewExistingRequests.View = System.Windows.Forms.View.Details
        '
        'ColumnHeaderApprover
        '
        Me.ColumnHeaderApprover.Text = "Approver"
        Me.ColumnHeaderApprover.Width = 111
        '
        'ColumnHeaderRequestId
        '
        Me.ColumnHeaderRequestId.Text = "Request ID"
        Me.ColumnHeaderRequestId.Width = 243
        '
        'ContextMenuListViewExistingWorkflows
        '
        Me.ContextMenuListViewExistingWorkflows.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ApproveDocumentToolStripMenuItem})
        Me.ContextMenuListViewExistingWorkflows.Name = "ContextMenuListViewExistingWorkflows"
        Me.ContextMenuListViewExistingWorkflows.Size = New System.Drawing.Size(167, 26)
        '
        'ApproveDocumentToolStripMenuItem
        '
        Me.ApproveDocumentToolStripMenuItem.Name = "ApproveDocumentToolStripMenuItem"
        Me.ApproveDocumentToolStripMenuItem.Size = New System.Drawing.Size(166, 22)
        Me.ApproveDocumentToolStripMenuItem.Text = "Approve Document"
        '
        'Mainform
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(405, 353)
        Me.Controls.Add(Me.GroupBoxExistingRequests)
        Me.Controls.Add(Me.GroupBoxCreateNewRequest)
        Me.Name = "Mainform"
        Me.Text = "Document Approval"
        Me.GroupBoxCreateNewRequest.ResumeLayout(False)
        Me.GroupBoxCreateNewRequest.PerformLayout()
        Me.GroupBoxExistingRequests.ResumeLayout(False)
        Me.ContextMenuListViewExistingWorkflows.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents GroupBoxCreateNewRequest As System.Windows.Forms.GroupBox
    Friend WithEvents ButtonCreateNewRequest As System.Windows.Forms.Button
    Private WithEvents TextBoxApprover As System.Windows.Forms.TextBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents GroupBoxExistingRequests As System.Windows.Forms.GroupBox
    Friend WithEvents ListViewExistingRequests As System.Windows.Forms.ListView
    Friend WithEvents ColumnHeaderApprover As System.Windows.Forms.ColumnHeader
    Friend WithEvents ColumnHeaderRequestId As System.Windows.Forms.ColumnHeader
    Friend WithEvents ContextMenuListViewExistingWorkflows As System.Windows.Forms.ContextMenuStrip
    Friend WithEvents ApproveDocumentToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem

End Class
