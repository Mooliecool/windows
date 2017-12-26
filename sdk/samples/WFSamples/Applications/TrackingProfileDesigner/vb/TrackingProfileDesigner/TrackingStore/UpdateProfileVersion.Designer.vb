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
Partial Class UpdateProfileVersion
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
        Me.infoLabel = New System.Windows.Forms.Label
        Me.existingProfileLabel = New System.Windows.Forms.Label
        Me.Label1 = New System.Windows.Forms.Label
        Me.profileVersion = New System.Windows.Forms.TextBox
        Me.save = New System.Windows.Forms.Button
        Me.cancel = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'infoLabel
        '
        Me.infoLabel.Location = New System.Drawing.Point(13, 13)
        Me.infoLabel.Name = "infoLabel"
        Me.infoLabel.Size = New System.Drawing.Size(157, 89)
        Me.infoLabel.TabIndex = 0
        Me.infoLabel.Text = "A tracking profile with a later version already exists for the workflow type sele" & _
            "cted.  Please enter a later version for this profile, or cancel, check the track" & _
            "ing service , and try again."
        '
        'existingProfileLabel
        '
        Me.existingProfileLabel.AutoSize = True
        Me.existingProfileLabel.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.existingProfileLabel.Location = New System.Drawing.Point(14, 98)
        Me.existingProfileLabel.Name = "existingProfileLabel"
        Me.existingProfileLabel.Size = New System.Drawing.Size(142, 13)
        Me.existingProfileLabel.TabIndex = 1
        Me.existingProfileLabel.Text = "Current Profile Version: "
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(12, 145)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(77, 13)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "Profile Version:"
        '
        'profileVersion
        '
        Me.profileVersion.Location = New System.Drawing.Point(95, 142)
        Me.profileVersion.Name = "profileVersion"
        Me.profileVersion.Size = New System.Drawing.Size(75, 20)
        Me.profileVersion.TabIndex = 3
        '
        'save
        '
        Me.save.Location = New System.Drawing.Point(17, 173)
        Me.save.Name = "save"
        Me.save.Size = New System.Drawing.Size(75, 23)
        Me.save.TabIndex = 4
        Me.save.Text = "&OK"
        Me.save.UseVisualStyleBackColor = True
        '
        'cancel
        '
        Me.cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.cancel.Location = New System.Drawing.Point(98, 173)
        Me.cancel.Name = "cancel"
        Me.cancel.Size = New System.Drawing.Size(75, 23)
        Me.cancel.TabIndex = 5
        Me.cancel.Text = "&Cancel"
        Me.cancel.UseVisualStyleBackColor = True
        '
        'UpdateProfileVersion
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(194, 211)
        Me.Controls.Add(Me.cancel)
        Me.Controls.Add(Me.save)
        Me.Controls.Add(Me.profileVersion)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.existingProfileLabel)
        Me.Controls.Add(Me.infoLabel)
        Me.Name = "UpdateProfileVersion"
        Me.Text = "Update Version"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents infoLabel As System.Windows.Forms.Label
    Friend WithEvents existingProfileLabel As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents profileVersion As System.Windows.Forms.TextBox
    Friend WithEvents save As System.Windows.Forms.Button
    Friend WithEvents cancel As System.Windows.Forms.Button
End Class
