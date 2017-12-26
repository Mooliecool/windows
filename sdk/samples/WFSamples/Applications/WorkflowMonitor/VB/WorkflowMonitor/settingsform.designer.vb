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

Namespace WorkflowMonitor
    <Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
    Partial Public Class SettingsForm
        Inherits System.Windows.Forms.Form

        'Form overrides dispose to clean up the component list.
        <System.Diagnostics.DebuggerNonUserCode()> _
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
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
            Me.AutoSelectLatest_Checkbox = New System.Windows.Forms.CheckBox
            Me.Unit_Label = New System.Windows.Forms.Label
            Me.Polling_TextBox = New System.Windows.Forms.TextBox
            Me.Polling_Label = New System.Windows.Forms.Label
            Me.Database_Textbox = New System.Windows.Forms.TextBox
            Me.Database_Label = New System.Windows.Forms.Label
            Me.Machine_TextBox = New System.Windows.Forms.TextBox
            Me.Machine_Label = New System.Windows.Forms.Label
            Me.Cancel_Button = New System.Windows.Forms.Button
            Me.OKButton = New System.Windows.Forms.Button
            Me.SuspendLayout()
            '
            'AutoSelectLatest_Checkbox
            '
            Me.AutoSelectLatest_Checkbox.AutoSize = True
            Me.AutoSelectLatest_Checkbox.Location = New System.Drawing.Point(7, 129)
            Me.AutoSelectLatest_Checkbox.Name = "AutoSelectLatest_Checkbox"
            Me.AutoSelectLatest_Checkbox.Size = New System.Drawing.Size(175, 17)
            Me.AutoSelectLatest_Checkbox.TabIndex = 3
            Me.AutoSelectLatest_Checkbox.Text = "Auto-Select Latest When Polling"
            '
            'Unit_Label
            '
            Me.Unit_Label.AutoSize = True
            Me.Unit_Label.Location = New System.Drawing.Point(163, 93)
            Me.Unit_Label.Name = "Unit_Label"
            Me.Unit_Label.Size = New System.Drawing.Size(16, 13)
            Me.Unit_Label.TabIndex = 23
            Me.Unit_Label.Text = "ms"
            '
            'Polling_TextBox
            '
            Me.Polling_TextBox.Location = New System.Drawing.Point(109, 87)
            Me.Polling_TextBox.Name = "Polling_TextBox"
            Me.Polling_TextBox.Size = New System.Drawing.Size(47, 20)
            Me.Polling_TextBox.TabIndex = 2
            Me.Polling_TextBox.Text = "500"
            '
            'Polling_Label
            '
            Me.Polling_Label.AutoSize = True
            Me.Polling_Label.BackColor = System.Drawing.SystemColors.Control
            Me.Polling_Label.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.Polling_Label.ForeColor = System.Drawing.Color.Black
            Me.Polling_Label.Location = New System.Drawing.Point(7, 87)
            Me.Polling_Label.Name = "Polling_Label"
            Me.Polling_Label.Size = New System.Drawing.Size(75, 13)
            Me.Polling_Label.TabIndex = 21
            Me.Polling_Label.Text = "Polling Interval:"
            '
            'Database_Textbox
            '
            Me.Database_Textbox.BackColor = System.Drawing.SystemColors.Window
            Me.Database_Textbox.Location = New System.Drawing.Point(109, 48)
            Me.Database_Textbox.Name = "Database_Textbox"
            Me.Database_Textbox.Size = New System.Drawing.Size(197, 20)
            Me.Database_Textbox.TabIndex = 1
            Me.Database_Textbox.Text = "WorkflowStore"
            '
            'Database_Label
            '
            Me.Database_Label.AutoSize = True
            Me.Database_Label.BackColor = System.Drawing.SystemColors.Control
            Me.Database_Label.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.Database_Label.ForeColor = System.Drawing.Color.Black
            Me.Database_Label.Location = New System.Drawing.Point(7, 48)
            Me.Database_Label.Name = "Database_Label"
            Me.Database_Label.Size = New System.Drawing.Size(97, 13)
            Me.Database_Label.TabIndex = 19
            Me.Database_Label.Text = "Tracking Database:"
            '
            'Machine_TextBox
            '
            Me.Machine_TextBox.Location = New System.Drawing.Point(109, 12)
            Me.Machine_TextBox.Name = "Machine_TextBox"
            Me.Machine_TextBox.Size = New System.Drawing.Size(197, 20)
            Me.Machine_TextBox.TabIndex = 0
            Me.Machine_TextBox.Text = "LocalHost"
            '
            'Machine_Label
            '
            Me.Machine_Label.AutoSize = True
            Me.Machine_Label.BackColor = System.Drawing.SystemColors.Control
            Me.Machine_Label.Location = New System.Drawing.Point(7, 12)
            Me.Machine_Label.Name = "Machine_Label"
            Me.Machine_Label.Size = New System.Drawing.Size(61, 13)
            Me.Machine_Label.TabIndex = 17
            Me.Machine_Label.Text = "SQL Server:"
            '
            'Cancel_Button
            '
            Me.Cancel_Button.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.Cancel_Button.Location = New System.Drawing.Point(231, 163)
            Me.Cancel_Button.Name = "Cancel_Button"
            Me.Cancel_Button.Size = New System.Drawing.Size(75, 23)
            Me.Cancel_Button.TabIndex = 5
            Me.Cancel_Button.Text = "Cancel"
            '
            'OKButton
            '
            Me.OKButton.Location = New System.Drawing.Point(138, 163)
            Me.OKButton.Margin = New System.Windows.Forms.Padding(1, 3, 3, 3)
            Me.OKButton.Name = "OKButton"
            Me.OKButton.Size = New System.Drawing.Size(75, 23)
            Me.OKButton.TabIndex = 4
            Me.OKButton.Text = "OK"
            '
            'SqlSettings
            '
            Me.AcceptButton = Me.OKButton
            Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.CancelButton = Me.Cancel_Button
            Me.ClientSize = New System.Drawing.Size(313, 199)
            Me.ControlBox = False
            Me.Controls.Add(Me.AutoSelectLatest_Checkbox)
            Me.Controls.Add(Me.Unit_Label)
            Me.Controls.Add(Me.Polling_TextBox)
            Me.Controls.Add(Me.Polling_Label)
            Me.Controls.Add(Me.Database_Textbox)
            Me.Controls.Add(Me.Database_Label)
            Me.Controls.Add(Me.Machine_TextBox)
            Me.Controls.Add(Me.Machine_Label)
            Me.Controls.Add(Me.Cancel_Button)
            Me.Controls.Add(Me.OKButton)
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
            Me.Name = "SqlSettings"
            Me.Text = "Settings"
            Me.ResumeLayout(False)
            Me.PerformLayout()

        End Sub
        Friend WithEvents AutoSelectLatest_Checkbox As System.Windows.Forms.CheckBox
        Friend WithEvents Unit_Label As System.Windows.Forms.Label
        Friend WithEvents Polling_TextBox As System.Windows.Forms.TextBox
        Friend WithEvents Polling_Label As System.Windows.Forms.Label
        Friend WithEvents Database_Textbox As System.Windows.Forms.TextBox
        Friend WithEvents Database_Label As System.Windows.Forms.Label
        Friend WithEvents Machine_TextBox As System.Windows.Forms.TextBox
        Friend WithEvents Machine_Label As System.Windows.Forms.Label
        Friend WithEvents Cancel_Button As System.Windows.Forms.Button
        Friend WithEvents OKButton As System.Windows.Forms.Button
    End Class
End Namespace

