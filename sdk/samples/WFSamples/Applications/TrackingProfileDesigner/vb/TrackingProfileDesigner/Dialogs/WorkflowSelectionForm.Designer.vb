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
Partial Class WorkflowSelectionForm
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
        Me.workflowListView = New System.Windows.Forms.ListView
        Me.descriptionHeader = New System.Windows.Forms.Label
        Me.columnHeader1 = New System.Windows.Forms.ColumnHeader
        Me.workflowDescription = New System.Windows.Forms.Label
        Me.workflowViewPanel = New System.Windows.Forms.Panel
        Me.ok = New System.Windows.Forms.Button
        Me.cancel = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'workflowListView
        '
        Me.workflowListView.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.columnHeader1})
        Me.workflowListView.Location = New System.Drawing.Point(13, 13)
        Me.workflowListView.MultiSelect = False
        Me.workflowListView.Name = "workflowListView"
        Me.workflowListView.Size = New System.Drawing.Size(121, 107)
        Me.workflowListView.TabIndex = 3
        Me.workflowListView.UseCompatibleStateImageBehavior = False
        Me.workflowListView.View = System.Windows.Forms.View.Details
        '
        'descriptionHeader
        '
        Me.descriptionHeader.AutoSize = True
        Me.descriptionHeader.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.descriptionHeader.Location = New System.Drawing.Point(13, 127)
        Me.descriptionHeader.Name = "descriptionHeader"
        Me.descriptionHeader.Size = New System.Drawing.Size(75, 13)
        Me.descriptionHeader.TabIndex = 4
        Me.descriptionHeader.Text = "Description:"
        '
        'columnHeader1
        '
        Me.columnHeader1.Text = "Workflow"
        Me.columnHeader1.Width = 116
        '
        'workflowDescription
        '
        Me.workflowDescription.Location = New System.Drawing.Point(16, 144)
        Me.workflowDescription.Name = "workflowDescription"
        Me.workflowDescription.Size = New System.Drawing.Size(111, 106)
        Me.workflowDescription.TabIndex = 5
        Me.workflowDescription.Text = "This is the workflow description."
        Me.workflowDescription.Visible = False
        '
        'workflowViewPanel
        '
        Me.workflowViewPanel.Location = New System.Drawing.Point(142, 12)
        Me.workflowViewPanel.Name = "workflowViewPanel"
        Me.workflowViewPanel.Size = New System.Drawing.Size(200, 238)
        Me.workflowViewPanel.TabIndex = 6
        '
        'okButton
        '
        Me.ok.Location = New System.Drawing.Point(186, 256)
        Me.ok.Name = "okButton"
        Me.ok.Size = New System.Drawing.Size(75, 23)
        Me.ok.TabIndex = 1
        Me.ok.Text = "&OK"
        Me.ok.UseVisualStyleBackColor = True
        '
        'cancelButton
        '
        Me.cancel.Location = New System.Drawing.Point(267, 256)
        Me.cancel.Name = "cancelButton"
        Me.cancel.Size = New System.Drawing.Size(75, 23)
        Me.cancel.TabIndex = 2
        Me.cancel.Text = "&Cancel"
        Me.cancel.UseVisualStyleBackColor = True
        '
        'WorkflowSelectionForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(354, 287)
        Me.Controls.Add(Me.cancel)
        Me.Controls.Add(Me.ok)
        Me.Controls.Add(Me.workflowViewPanel)
        Me.Controls.Add(Me.workflowDescription)
        Me.Controls.Add(Me.descriptionHeader)
        Me.Controls.Add(Me.workflowListView)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Name = "WorkflowSelectionForm"
        Me.Text = "Choose Workflow..."
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents workflowListView As System.Windows.Forms.ListView
    Private WithEvents columnHeader1 As System.Windows.Forms.ColumnHeader
    Private WithEvents descriptionHeader As System.Windows.Forms.Label
    Private WithEvents workflowDescription As System.Windows.Forms.Label
    Private WithEvents workflowViewPanel As System.Windows.Forms.Panel
    Private WithEvents ok As System.Windows.Forms.Button
    Private WithEvents cancel As System.Windows.Forms.Button
End Class
