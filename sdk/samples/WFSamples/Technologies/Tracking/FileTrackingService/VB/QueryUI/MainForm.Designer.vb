'----------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
'  
'   Copyright (C) Microsoft Corporation.  All rights reserved.
'  
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
'  
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'----------------------------------------------------------------------

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class MainForm
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
        Me.Label1 = New System.Windows.Forms.Label
        Me.TextBoxInstanceId = New System.Windows.Forms.TextBox
        Me.ButtonGetWorkflowEvents = New System.Windows.Forms.Button
        Me.LabelWorkflowEventsList = New System.Windows.Forms.Label
        Me.ListViewWorkflowEvents = New System.Windows.Forms.ListView
        Me.ButtonExit = New System.Windows.Forms.Button
        Me.LabelReadingStatus = New System.Windows.Forms.Label
        Me.SuspendLayout()
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(0, 25)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(90, 13)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "Enter Instance ID"
        '
        'TextBoxInstanceId
        '
        Me.TextBoxInstanceId.Location = New System.Drawing.Point(96, 22)
        Me.TextBoxInstanceId.Name = "TextBoxInstanceId"
        Me.TextBoxInstanceId.Size = New System.Drawing.Size(203, 20)
        Me.TextBoxInstanceId.TabIndex = 0
        '
        'ButtonGetWorkflowEvents
        '
        Me.ButtonGetWorkflowEvents.Location = New System.Drawing.Point(96, 51)
        Me.ButtonGetWorkflowEvents.Name = "ButtonGetWorkflowEvents"
        Me.ButtonGetWorkflowEvents.Size = New System.Drawing.Size(203, 22)
        Me.ButtonGetWorkflowEvents.TabIndex = 1
        Me.ButtonGetWorkflowEvents.Text = "Get Tracked Workflow Events"
        Me.ButtonGetWorkflowEvents.UseVisualStyleBackColor = True
        '
        'LabelWorkflowEventsList
        '
        Me.LabelWorkflowEventsList.AutoSize = True
        Me.LabelWorkflowEventsList.Location = New System.Drawing.Point(107, 92)
        Me.LabelWorkflowEventsList.Name = "LabelWorkflowEventsList"
        Me.LabelWorkflowEventsList.Size = New System.Drawing.Size(88, 13)
        Me.LabelWorkflowEventsList.TabIndex = 2
        Me.LabelWorkflowEventsList.Text = "Workflow Events"
        '
        'ListViewWorkflowEvents
        '
        Me.ListViewWorkflowEvents.Location = New System.Drawing.Point(96, 109)
        Me.ListViewWorkflowEvents.Name = "ListViewWorkflowEvents"
        Me.ListViewWorkflowEvents.Size = New System.Drawing.Size(109, 128)
        Me.ListViewWorkflowEvents.TabIndex = 2
        Me.ListViewWorkflowEvents.UseCompatibleStateImageBehavior = False
        Me.ListViewWorkflowEvents.View = System.Windows.Forms.View.List
        '
        'ButtonExit
        '
        Me.ButtonExit.Location = New System.Drawing.Point(237, 206)
        Me.ButtonExit.Name = "ButtonExit"
        Me.ButtonExit.Size = New System.Drawing.Size(62, 30)
        Me.ButtonExit.TabIndex = 3
        Me.ButtonExit.Text = "Exit"
        Me.ButtonExit.UseVisualStyleBackColor = True
        '
        'LabelReadingStatus
        '
        Me.LabelReadingStatus.AutoSize = True
        Me.LabelReadingStatus.Location = New System.Drawing.Point(12, 108)
        Me.LabelReadingStatus.Name = "LabelReadingStatus"
        Me.LabelReadingStatus.Size = New System.Drawing.Size(0, 13)
        Me.LabelReadingStatus.TabIndex = 4
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(328, 266)
        Me.Controls.Add(Me.LabelReadingStatus)
        Me.Controls.Add(Me.ButtonExit)
        Me.Controls.Add(Me.ListViewWorkflowEvents)
        Me.Controls.Add(Me.LabelWorkflowEventsList)
        Me.Controls.Add(Me.ButtonGetWorkflowEvents)
        Me.Controls.Add(Me.TextBoxInstanceId)
        Me.Controls.Add(Me.Label1)
        Me.Name = "MainForm"
        Me.Text = "Simple File Tracking Service Query"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents Label1 As System.Windows.Forms.Label
    Private WithEvents TextBoxInstanceId As System.Windows.Forms.TextBox
    Private WithEvents ButtonGetWorkflowEvents As System.Windows.Forms.Button
    Private WithEvents LabelWorkflowEventsList As System.Windows.Forms.Label
    Private WithEvents ListViewWorkflowEvents As System.Windows.Forms.ListView
    Friend WithEvents ButtonExit As System.Windows.Forms.Button
    Friend WithEvents LabelReadingStatus As System.Windows.Forms.Label

End Class
