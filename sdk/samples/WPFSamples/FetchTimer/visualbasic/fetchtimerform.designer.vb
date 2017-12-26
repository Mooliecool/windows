 '************************************************************************************************
' *
' * File: FetchTimerForm.Designer.cs
' *
' * Description: Design-time code for the interface.
' * 
' * See FetchTimerForm.cs for a full description of the sample.
' *
' *     
' *  This file is part of the Microsoft WinfFX SDK Code Samples.
' * 
' *  Copyright (C) Microsoft Corporation.  All rights reserved.
' * 
' * This source code is intended only as a supplement to Microsoft
' * Development Tools and/or on-line documentation.  See these other
' * materials for detailed information regarding Microsoft code samples.
' * 
' * THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' * PARTICULAR PURPOSE.
' * 
' ************************************************************************************************

Partial Class FetchTimerForm
    Inherits Form
    ''' <summary>
    ''' Required designer variable.
    ''' </summary>
    Private components As System.ComponentModel.IContainer = Nothing


    ''' <summary>
    ''' Clean up any resources being used.
    ''' </summary>
    ''' <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso Not (components Is Nothing) Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)

    End Sub 'Dispose

#Region "Windows Form Designer generated code"


    ''' <summary>
    ''' Required method for Designer support - do not modify
    ''' the contents of this method with the code editor.
    ''' </summary>
    Private Sub InitializeComponent()
        Me.gbScope = New System.Windows.Forms.GroupBox
        Me.cbDescendants = New System.Windows.Forms.CheckBox
        Me.cbChildren = New System.Windows.Forms.CheckBox
        Me.comboElement = New System.Windows.Forms.CheckBox
        Me.tbOutput = New System.Windows.Forms.TextBox
        Me.lblOutput = New System.Windows.Forms.Label
        Me.btnProps = New System.Windows.Forms.Button
        Me.label1 = New System.Windows.Forms.Label
        Me.btnClear = New System.Windows.Forms.Button
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.rbNone = New System.Windows.Forms.RadioButton
        Me.rbFull = New System.Windows.Forms.RadioButton
        Me.gbScope.SuspendLayout()
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'gbScope
        '
        Me.gbScope.Controls.Add(Me.cbDescendants)
        Me.gbScope.Controls.Add(Me.cbChildren)
        Me.gbScope.Controls.Add(Me.comboElement)
        Me.gbScope.Location = New System.Drawing.Point(239, 76)
        Me.gbScope.Name = "gbScope"
        Me.gbScope.Size = New System.Drawing.Size(97, 93)
        Me.gbScope.TabIndex = 3
        Me.gbScope.TabStop = False
        Me.gbScope.Text = "TreeScope"
        '
        'cbDescendants
        '
        Me.cbDescendants.AutoSize = True
        Me.cbDescendants.Location = New System.Drawing.Point(6, 68)
        Me.cbDescendants.Name = "cbDescendants"
        Me.cbDescendants.Size = New System.Drawing.Size(89, 17)
        Me.cbDescendants.TabIndex = 2
        Me.cbDescendants.Text = "&Descendants"
        Me.cbDescendants.UseVisualStyleBackColor = True
        '
        'cbChildren
        '
        Me.cbChildren.AutoSize = True
        Me.cbChildren.Location = New System.Drawing.Point(6, 44)
        Me.cbChildren.Name = "cbChildren"
        Me.cbChildren.Size = New System.Drawing.Size(64, 17)
        Me.cbChildren.TabIndex = 1
        Me.cbChildren.Text = "&Children"
        Me.cbChildren.UseVisualStyleBackColor = True
        '
        'comboElement
        '
        Me.comboElement.AutoCheck = False
        Me.comboElement.AutoSize = True
        Me.comboElement.Checked = True
        Me.comboElement.CheckState = System.Windows.Forms.CheckState.Checked
        Me.comboElement.Enabled = False
        Me.comboElement.Location = New System.Drawing.Point(6, 20)
        Me.comboElement.Name = "comboElement"
        Me.comboElement.Size = New System.Drawing.Size(64, 17)
        Me.comboElement.TabIndex = 0
        Me.comboElement.TabStop = False
        Me.comboElement.Text = "Element"
        Me.comboElement.UseVisualStyleBackColor = True
        '
        'tbOutput
        '
        Me.tbOutput.BackColor = System.Drawing.SystemColors.Window
        Me.tbOutput.Location = New System.Drawing.Point(10, 180)
        Me.tbOutput.Multiline = True
        Me.tbOutput.Name = "tbOutput"
        Me.tbOutput.ReadOnly = True
        Me.tbOutput.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.tbOutput.Size = New System.Drawing.Size(385, 165)
        Me.tbOutput.TabIndex = 6
        Me.tbOutput.TabStop = False
        '
        'lblOutput
        '
        Me.lblOutput.AutoSize = True
        Me.lblOutput.Location = New System.Drawing.Point(20, 156)
        Me.lblOutput.Name = "lblOutput"
        Me.lblOutput.Size = New System.Drawing.Size(42, 13)
        Me.lblOutput.TabIndex = 5
        Me.lblOutput.Text = "Results"
        Me.lblOutput.Visible = False
        '
        'btnProps
        '
        Me.btnProps.Location = New System.Drawing.Point(156, 39)
        Me.btnProps.Name = "btnProps"
        Me.btnProps.Size = New System.Drawing.Size(100, 23)
        Me.btnProps.TabIndex = 1
        Me.btnProps.Text = "&Get Properties"
        Me.btnProps.UseVisualStyleBackColor = True
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(52, 7)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(319, 26)
        Me.label1.TabIndex = 0
        Me.label1.Text = "Activate this window, put the cursor over an element" & Global.Microsoft.VisualBasic.ChrW(10) & "anywhere on the screen, and " & _
            "invoke the button by pressing Alt+G."
        Me.label1.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'btnClear
        '
        Me.btnClear.Location = New System.Drawing.Point(68, 151)
        Me.btnClear.Name = "btnClear"
        Me.btnClear.Size = New System.Drawing.Size(60, 23)
        Me.btnClear.TabIndex = 7
        Me.btnClear.Text = "Clea&r"
        Me.btnClear.UseVisualStyleBackColor = True
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.rbNone)
        Me.groupBox1.Controls.Add(Me.rbFull)
        Me.groupBox1.Location = New System.Drawing.Point(36, 76)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(153, 69)
        Me.groupBox1.TabIndex = 8
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "AutomationElementMode"
        '
        'rbNone
        '
        Me.rbNone.AutoSize = True
        Me.rbNone.Location = New System.Drawing.Point(51, 44)
        Me.rbNone.Name = "rbNone"
        Me.rbNone.Size = New System.Drawing.Size(51, 17)
        Me.rbNone.TabIndex = 1
        Me.rbNone.Text = "&None"
        Me.rbNone.UseVisualStyleBackColor = True
        '
        'rbFull
        '
        Me.rbFull.AutoSize = True
        Me.rbFull.Checked = True
        Me.rbFull.Location = New System.Drawing.Point(51, 20)
        Me.rbFull.Name = "rbFull"
        Me.rbFull.Size = New System.Drawing.Size(41, 17)
        Me.rbFull.TabIndex = 0
        Me.rbFull.TabStop = True
        Me.rbFull.Text = "&Full"
        Me.rbFull.UseVisualStyleBackColor = True
        '
        'FetchTimerForm
        '
        Me.ClientSize = New System.Drawing.Size(405, 357)
        Me.Controls.Add(Me.groupBox1)
        Me.Controls.Add(Me.btnClear)
        Me.Controls.Add(Me.label1)
        Me.Controls.Add(Me.btnProps)
        Me.Controls.Add(Me.lblOutput)
        Me.Controls.Add(Me.tbOutput)
        Me.Controls.Add(Me.gbScope)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Name = "FetchTimerForm"
        Me.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide
        Me.Text = "UI Automation Caching Sample"
        Me.TopMost = True
        Me.gbScope.ResumeLayout(False)
        Me.gbScope.PerformLayout()
        Me.groupBox1.ResumeLayout(False)
        Me.groupBox1.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub 'InitializeComponent 

#End Region

    Private gbScope As System.Windows.Forms.GroupBox
    Private comboElement As System.Windows.Forms.CheckBox
    Private WithEvents cbDescendants As System.Windows.Forms.CheckBox
    Private cbChildren As System.Windows.Forms.CheckBox
    Private tbOutput As System.Windows.Forms.TextBox
    Private lblOutput As System.Windows.Forms.Label
    Private WithEvents btnProps As System.Windows.Forms.Button
    Private label1 As System.Windows.Forms.Label
    Private WithEvents btnClear As System.Windows.Forms.Button
    Private groupBox1 As System.Windows.Forms.GroupBox
    Private rbNone As System.Windows.Forms.RadioButton
    Private rbFull As System.Windows.Forms.RadioButton
End Class 'FetchTimerForm

