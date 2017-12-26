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
Partial Public Class DesignerShell
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(DesignerShell))
        Me.descriptionTextBox = New System.Windows.Forms.TextBox
        Me.nameTextBox = New System.Windows.Forms.TextBox
        Me.textBox4 = New System.Windows.Forms.TextBox
        Me.label1 = New System.Windows.Forms.Label
        Me.updateButton = New System.Windows.Forms.Button
        Me.groupBox2 = New System.Windows.Forms.GroupBox
        Me.zoomNumericUpDown = New System.Windows.Forms.NumericUpDown
        Me.label2 = New System.Windows.Forms.Label
        Me.addButton = New System.Windows.Forms.Button
        Me.panel1 = New System.Windows.Forms.Panel
        Me.groupBox3 = New System.Windows.Forms.GroupBox
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.label6 = New System.Windows.Forms.Label
        Me.Panel2 = New System.Windows.Forms.Panel
        Me.groupBox2.SuspendLayout()
        CType(Me.zoomNumericUpDown, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.panel1.SuspendLayout()
        Me.groupBox3.SuspendLayout()
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'descriptionTextBox
        '
        Me.descriptionTextBox.Location = New System.Drawing.Point(6, 71)
        Me.descriptionTextBox.Multiline = True
        Me.descriptionTextBox.Name = "descriptionTextBox"
        Me.descriptionTextBox.Size = New System.Drawing.Size(198, 20)
        Me.descriptionTextBox.TabIndex = 3
        '
        'nameTextBox
        '
        Me.nameTextBox.Location = New System.Drawing.Point(6, 32)
        Me.nameTextBox.Multiline = True
        Me.nameTextBox.Name = "nameTextBox"
        Me.nameTextBox.Size = New System.Drawing.Size(198, 20)
        Me.nameTextBox.TabIndex = 1
        '
        'textBox4
        '
        Me.textBox4.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.textBox4.Dock = System.Windows.Forms.DockStyle.Fill
        Me.textBox4.Location = New System.Drawing.Point(3, 16)
        Me.textBox4.Multiline = True
        Me.textBox4.Name = "textBox4"
        Me.textBox4.ReadOnly = True
        Me.textBox4.Size = New System.Drawing.Size(204, 163)
        Me.textBox4.TabIndex = 0
        Me.textBox4.TabStop = False
        Me.textBox4.Text = resources.GetString("textBox4.Text")
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(3, 55)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(63, 13)
        Me.label1.TabIndex = 2
        Me.label1.Text = "&Description:"
        '
        'updateButton
        '
        Me.updateButton.Location = New System.Drawing.Point(129, 97)
        Me.updateButton.Name = "updateButton"
        Me.updateButton.Size = New System.Drawing.Size(75, 23)
        Me.updateButton.TabIndex = 5
        Me.updateButton.Text = "&Update"
        '
        'groupBox2
        '
        Me.groupBox2.Controls.Add(Me.zoomNumericUpDown)
        Me.groupBox2.Controls.Add(Me.label2)
        Me.groupBox2.Location = New System.Drawing.Point(4, 137)
        Me.groupBox2.Name = "groupBox2"
        Me.groupBox2.Size = New System.Drawing.Size(210, 50)
        Me.groupBox2.TabIndex = 1
        Me.groupBox2.TabStop = False
        Me.groupBox2.Text = "Zoom"
        '
        'zoomNumericUpDown
        '
        Me.zoomNumericUpDown.Location = New System.Drawing.Point(56, 18)
        Me.zoomNumericUpDown.Maximum = New Decimal(New Integer() {400, 0, 0, 0})
        Me.zoomNumericUpDown.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.zoomNumericUpDown.Name = "zoomNumericUpDown"
        Me.zoomNumericUpDown.Size = New System.Drawing.Size(47, 20)
        Me.zoomNumericUpDown.TabIndex = 1
        Me.zoomNumericUpDown.Value = New Decimal(New Integer() {100, 0, 0, 0})
        '
        'label2
        '
        Me.label2.AutoSize = True
        Me.label2.Location = New System.Drawing.Point(3, 20)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(47, 13)
        Me.label2.TabIndex = 0
        Me.label2.Text = "&Percent:"
        '
        'addButton
        '
        Me.addButton.Location = New System.Drawing.Point(50, 97)
        Me.addButton.Name = "addButton"
        Me.addButton.Size = New System.Drawing.Size(75, 23)
        Me.addButton.TabIndex = 4
        Me.addButton.Text = "&Add"
        Me.addButton.UseVisualStyleBackColor = True
        '
        'panel1
        '
        Me.panel1.Controls.Add(Me.groupBox3)
        Me.panel1.Controls.Add(Me.groupBox2)
        Me.panel1.Controls.Add(Me.groupBox1)
        Me.panel1.Dock = System.Windows.Forms.DockStyle.Left
        Me.panel1.Location = New System.Drawing.Point(0, 0)
        Me.panel1.Name = "panel1"
        Me.panel1.Size = New System.Drawing.Size(220, 462)
        Me.panel1.TabIndex = 5
        '
        'groupBox3
        '
        Me.groupBox3.Controls.Add(Me.textBox4)
        Me.groupBox3.Location = New System.Drawing.Point(4, 193)
        Me.groupBox3.Name = "groupBox3"
        Me.groupBox3.Size = New System.Drawing.Size(210, 182)
        Me.groupBox3.TabIndex = 2
        Me.groupBox3.TabStop = False
        Me.groupBox3.Text = "Instructions"
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.addButton)
        Me.groupBox1.Controls.Add(Me.updateButton)
        Me.groupBox1.Controls.Add(Me.descriptionTextBox)
        Me.groupBox1.Controls.Add(Me.nameTextBox)
        Me.groupBox1.Controls.Add(Me.label1)
        Me.groupBox1.Controls.Add(Me.label6)
        Me.groupBox1.Location = New System.Drawing.Point(4, 3)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(210, 128)
        Me.groupBox1.TabIndex = 0
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "CodeActivity"
        '
        'label6
        '
        Me.label6.AutoSize = True
        Me.label6.Location = New System.Drawing.Point(6, 16)
        Me.label6.Name = "label6"
        Me.label6.Size = New System.Drawing.Size(38, 13)
        Me.label6.TabIndex = 0
        Me.label6.Text = "&Name:"
        '
        'Panel2
        '
        Me.Panel2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel2.Location = New System.Drawing.Point(220, 0)
        Me.Panel2.Name = "Panel2"
        Me.Panel2.Size = New System.Drawing.Size(460, 462)
        Me.Panel2.TabIndex = 0
        '
        'DesignerShell
        '
        Me.AcceptButton = Me.updateButton
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(680, 462)
        Me.Controls.Add(Me.Panel2)
        Me.Controls.Add(Me.panel1)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MinimumSize = New System.Drawing.Size(578, 410)
        Me.Name = "DesignerShell"
        Me.Text = "Designer ReHosting Sample"
        Me.groupBox2.ResumeLayout(False)
        Me.groupBox2.PerformLayout()
        CType(Me.zoomNumericUpDown, System.ComponentModel.ISupportInitialize).EndInit()
        Me.panel1.ResumeLayout(False)
        Me.groupBox3.ResumeLayout(False)
        Me.groupBox3.PerformLayout()
        Me.groupBox1.ResumeLayout(False)
        Me.groupBox1.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents descriptionTextBox As System.Windows.Forms.TextBox
    Friend WithEvents nameTextBox As System.Windows.Forms.TextBox
    Friend WithEvents textBox4 As System.Windows.Forms.TextBox
    Friend WithEvents label1 As System.Windows.Forms.Label
    Friend WithEvents updateButton As System.Windows.Forms.Button
    Friend WithEvents groupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents zoomNumericUpDown As System.Windows.Forms.NumericUpDown
    Friend WithEvents label2 As System.Windows.Forms.Label
    Private WithEvents addButton As System.Windows.Forms.Button
    Friend WithEvents panel1 As System.Windows.Forms.Panel
    Friend WithEvents groupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents label6 As System.Windows.Forms.Label
    Friend WithEvents Panel2 As System.Windows.Forms.Panel

End Class
