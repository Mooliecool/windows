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
Partial Class WizardForm
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
        Me.compileWorkflowButton = New System.Windows.Forms.Button
        Me.startWorkflowButton = New System.Windows.Forms.Button
        Me.ifParameterLinkLabel = New System.Windows.Forms.LinkLabel
        Me.actionsCheckedListBox = New System.Windows.Forms.CheckedListBox
        Me.tabPage5 = New System.Windows.Forms.TabPage
        Me.generateWorkflowButton = New System.Windows.Forms.Button
        Me.nextStepButton = New System.Windows.Forms.Button
        Me.splitContainer1 = New System.Windows.Forms.SplitContainer
        Me.tabControl1 = New System.Windows.Forms.TabControl
        Me.tabPage1 = New System.Windows.Forms.TabPage
        Me.outboxRadioButton = New System.Windows.Forms.RadioButton
        Me.sentRadioButton = New System.Windows.Forms.RadioButton
        Me.inboxRadioButton = New System.Windows.Forms.RadioButton
        Me.tabPage2 = New System.Windows.Forms.TabPage
        Me.ccRadioButton = New System.Windows.Forms.RadioButton
        Me.toRadioButton = New System.Windows.Forms.RadioButton
        Me.bccRadioButton = New System.Windows.Forms.RadioButton
        Me.fromRadioButton = New System.Windows.Forms.RadioButton
        Me.subjectRadioButton = New System.Windows.Forms.RadioButton
        Me.tabPage3 = New System.Windows.Forms.TabPage
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.actionsSelectionLabel = New System.Windows.Forms.Label
        Me.ifParameterField = New System.Windows.Forms.TextBox
        Me.conditionSelectionLabel = New System.Windows.Forms.Label
        Me.folderSelectionLabel = New System.Windows.Forms.Label
        Me.tabPage5.SuspendLayout()
        Me.splitContainer1.Panel1.SuspendLayout()
        Me.splitContainer1.SuspendLayout()
        Me.tabControl1.SuspendLayout()
        Me.tabPage1.SuspendLayout()
        Me.tabPage2.SuspendLayout()
        Me.tabPage3.SuspendLayout()
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'compileWorkflowButton
        '
        Me.compileWorkflowButton.Enabled = False
        Me.compileWorkflowButton.Location = New System.Drawing.Point(75, 49)
        Me.compileWorkflowButton.Name = "compileWorkflowButton"
        Me.compileWorkflowButton.Size = New System.Drawing.Size(160, 23)
        Me.compileWorkflowButton.TabIndex = 1
        Me.compileWorkflowButton.Text = "Compile Workflow Definition"
        '
        'startWorkflowButton
        '
        Me.startWorkflowButton.Enabled = False
        Me.startWorkflowButton.Location = New System.Drawing.Point(75, 91)
        Me.startWorkflowButton.Name = "startWorkflowButton"
        Me.startWorkflowButton.Size = New System.Drawing.Size(160, 23)
        Me.startWorkflowButton.TabIndex = 2
        Me.startWorkflowButton.Text = "Start Workflow Definition"
        '
        'ifParameterLinkLabel
        '
        Me.ifParameterLinkLabel.AutoSize = True
        Me.ifParameterLinkLabel.Location = New System.Drawing.Point(90, 44)
        Me.ifParameterLinkLabel.Name = "ifParameterLinkLabel"
        Me.ifParameterLinkLabel.Size = New System.Drawing.Size(49, 13)
        Me.ifParameterLinkLabel.TabIndex = 2
        Me.ifParameterLinkLabel.TabStop = True
        Me.ifParameterLinkLabel.Text = "linkLabel"
        Me.ifParameterLinkLabel.UseMnemonic = False
        '
        'actionsCheckedListBox
        '
        Me.actionsCheckedListBox.BackColor = System.Drawing.SystemColors.Control
        Me.actionsCheckedListBox.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.actionsCheckedListBox.CheckOnClick = True
        Me.actionsCheckedListBox.FormattingEnabled = True
        Me.actionsCheckedListBox.Items.AddRange(New Object() {"Send Auto-Reply Email", "Create Outlook Note", "Create Outlook Task"})
        Me.actionsCheckedListBox.Location = New System.Drawing.Point(4, 7)
        Me.actionsCheckedListBox.Name = "actionsCheckedListBox"
        Me.actionsCheckedListBox.Size = New System.Drawing.Size(300, 45)
        Me.actionsCheckedListBox.TabIndex = 0
        '
        'tabPage5
        '
        Me.tabPage5.Controls.Add(Me.generateWorkflowButton)
        Me.tabPage5.Controls.Add(Me.compileWorkflowButton)
        Me.tabPage5.Controls.Add(Me.startWorkflowButton)
        Me.tabPage5.Location = New System.Drawing.Point(4, 22)
        Me.tabPage5.Name = "tabPage5"
        Me.tabPage5.Padding = New System.Windows.Forms.Padding(3)
        Me.tabPage5.Size = New System.Drawing.Size(310, 120)
        Me.tabPage5.TabIndex = 4
        Me.tabPage5.Text = "Execute"
        '
        'generateWorkflowButton
        '
        Me.generateWorkflowButton.Location = New System.Drawing.Point(75, 6)
        Me.generateWorkflowButton.Name = "generateWorkflowButton"
        Me.generateWorkflowButton.Size = New System.Drawing.Size(160, 23)
        Me.generateWorkflowButton.TabIndex = 0
        Me.generateWorkflowButton.Text = "Generate Workflow Definition"
        '
        'nextStepButton
        '
        Me.nextStepButton.Anchor = System.Windows.Forms.AnchorStyles.Top
        Me.nextStepButton.Location = New System.Drawing.Point(127, 317)
        Me.nextStepButton.Name = "nextStepButton"
        Me.nextStepButton.Size = New System.Drawing.Size(75, 23)
        Me.nextStepButton.TabIndex = 2
        Me.nextStepButton.Text = "Next"
        '
        'splitContainer1
        '
        Me.splitContainer1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1
        Me.splitContainer1.Location = New System.Drawing.Point(0, 0)
        Me.splitContainer1.Name = "splitContainer1"
        '
        'splitContainer1.Panel1
        '
        Me.splitContainer1.Panel1.Controls.Add(Me.nextStepButton)
        Me.splitContainer1.Panel1.Controls.Add(Me.tabControl1)
        Me.splitContainer1.Panel1.Controls.Add(Me.groupBox1)
        Me.splitContainer1.Size = New System.Drawing.Size(693, 461)
        Me.splitContainer1.SplitterDistance = 331
        Me.splitContainer1.TabIndex = 3
        Me.splitContainer1.Text = "splitContainer1"
        '
        'tabControl1
        '
        Me.tabControl1.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tabControl1.Controls.Add(Me.tabPage1)
        Me.tabControl1.Controls.Add(Me.tabPage2)
        Me.tabControl1.Controls.Add(Me.tabPage3)
        Me.tabControl1.Controls.Add(Me.tabPage5)
        Me.tabControl1.Location = New System.Drawing.Point(6, 29)
        Me.tabControl1.Name = "tabControl1"
        Me.tabControl1.SelectedIndex = 0
        Me.tabControl1.Size = New System.Drawing.Size(318, 146)
        Me.tabControl1.TabIndex = 0
        '
        'tabPage1
        '
        Me.tabPage1.BackColor = System.Drawing.SystemColors.Control
        Me.tabPage1.Controls.Add(Me.outboxRadioButton)
        Me.tabPage1.Controls.Add(Me.sentRadioButton)
        Me.tabPage1.Controls.Add(Me.inboxRadioButton)
        Me.tabPage1.Location = New System.Drawing.Point(4, 22)
        Me.tabPage1.Name = "tabPage1"
        Me.tabPage1.Padding = New System.Windows.Forms.Padding(3)
        Me.tabPage1.Size = New System.Drawing.Size(310, 120)
        Me.tabPage1.TabIndex = 0
        Me.tabPage1.Text = "Folder"
        '
        'outboxRadioButton
        '
        Me.outboxRadioButton.AutoSize = True
        Me.outboxRadioButton.Location = New System.Drawing.Point(6, 52)
        Me.outboxRadioButton.Name = "outboxRadioButton"
        Me.outboxRadioButton.Size = New System.Drawing.Size(71, 17)
        Me.outboxRadioButton.TabIndex = 2
        Me.outboxRadioButton.TabStop = True
        Me.outboxRadioButton.Text = "In Outbox"
        Me.outboxRadioButton.UseVisualStyleBackColor = True
        '
        'sentRadioButton
        '
        Me.sentRadioButton.AutoSize = True
        Me.sentRadioButton.Location = New System.Drawing.Point(6, 29)
        Me.sentRadioButton.Name = "sentRadioButton"
        Me.sentRadioButton.Size = New System.Drawing.Size(87, 17)
        Me.sentRadioButton.TabIndex = 1
        Me.sentRadioButton.TabStop = True
        Me.sentRadioButton.Text = "In Sent Items"
        Me.sentRadioButton.UseVisualStyleBackColor = True
        '
        'inboxRadioButton
        '
        Me.inboxRadioButton.AutoSize = True
        Me.inboxRadioButton.Location = New System.Drawing.Point(6, 6)
        Me.inboxRadioButton.Name = "inboxRadioButton"
        Me.inboxRadioButton.Size = New System.Drawing.Size(63, 17)
        Me.inboxRadioButton.TabIndex = 0
        Me.inboxRadioButton.TabStop = True
        Me.inboxRadioButton.Text = "In Inbox"
        Me.inboxRadioButton.UseVisualStyleBackColor = True
        '
        'tabPage2
        '
        Me.tabPage2.BackColor = System.Drawing.SystemColors.Control
        Me.tabPage2.Controls.Add(Me.ccRadioButton)
        Me.tabPage2.Controls.Add(Me.toRadioButton)
        Me.tabPage2.Controls.Add(Me.bccRadioButton)
        Me.tabPage2.Controls.Add(Me.fromRadioButton)
        Me.tabPage2.Controls.Add(Me.subjectRadioButton)
        Me.tabPage2.Location = New System.Drawing.Point(4, 22)
        Me.tabPage2.Name = "tabPage2"
        Me.tabPage2.Padding = New System.Windows.Forms.Padding(3)
        Me.tabPage2.Size = New System.Drawing.Size(310, 120)
        Me.tabPage2.TabIndex = 1
        Me.tabPage2.Text = "Condition"
        '
        'ccRadioButton
        '
        Me.ccRadioButton.AutoSize = True
        Me.ccRadioButton.Location = New System.Drawing.Point(6, 75)
        Me.ccRadioButton.Name = "ccRadioButton"
        Me.ccRadioButton.Size = New System.Drawing.Size(82, 17)
        Me.ccRadioButton.TabIndex = 3
        Me.ccRadioButton.Text = "If CC equals"
        Me.ccRadioButton.UseVisualStyleBackColor = True
        '
        'toRadioButton
        '
        Me.toRadioButton.AutoSize = True
        Me.toRadioButton.Location = New System.Drawing.Point(6, 52)
        Me.toRadioButton.Name = "toRadioButton"
        Me.toRadioButton.Size = New System.Drawing.Size(81, 17)
        Me.toRadioButton.TabIndex = 2
        Me.toRadioButton.Text = "If To equals"
        Me.toRadioButton.UseVisualStyleBackColor = True
        '
        'bccRadioButton
        '
        Me.bccRadioButton.AutoSize = True
        Me.bccRadioButton.Location = New System.Drawing.Point(6, 98)
        Me.bccRadioButton.Name = "bccRadioButton"
        Me.bccRadioButton.Size = New System.Drawing.Size(89, 17)
        Me.bccRadioButton.TabIndex = 4
        Me.bccRadioButton.Text = "If BCC equals"
        Me.bccRadioButton.UseVisualStyleBackColor = True
        '
        'fromRadioButton
        '
        Me.fromRadioButton.AutoSize = True
        Me.fromRadioButton.Location = New System.Drawing.Point(6, 29)
        Me.fromRadioButton.Name = "fromRadioButton"
        Me.fromRadioButton.Size = New System.Drawing.Size(91, 17)
        Me.fromRadioButton.TabIndex = 1
        Me.fromRadioButton.Text = "If From equals"
        Me.fromRadioButton.UseVisualStyleBackColor = True
        '
        'subjectRadioButton
        '
        Me.subjectRadioButton.AutoSize = True
        Me.subjectRadioButton.Location = New System.Drawing.Point(6, 6)
        Me.subjectRadioButton.Name = "subjectRadioButton"
        Me.subjectRadioButton.Size = New System.Drawing.Size(104, 17)
        Me.subjectRadioButton.TabIndex = 0
        Me.subjectRadioButton.Text = "If Subject equals"
        Me.subjectRadioButton.UseVisualStyleBackColor = True
        '
        'tabPage3
        '
        Me.tabPage3.BackColor = System.Drawing.SystemColors.Control
        Me.tabPage3.Controls.Add(Me.actionsCheckedListBox)
        Me.tabPage3.Location = New System.Drawing.Point(4, 22)
        Me.tabPage3.Name = "tabPage3"
        Me.tabPage3.Padding = New System.Windows.Forms.Padding(3)
        Me.tabPage3.Size = New System.Drawing.Size(310, 120)
        Me.tabPage3.TabIndex = 2
        Me.tabPage3.Text = "Actions"
        '
        'groupBox1
        '
        Me.groupBox1.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.groupBox1.BackColor = System.Drawing.Color.Transparent
        Me.groupBox1.Controls.Add(Me.actionsSelectionLabel)
        Me.groupBox1.Controls.Add(Me.ifParameterLinkLabel)
        Me.groupBox1.Controls.Add(Me.ifParameterField)
        Me.groupBox1.Controls.Add(Me.conditionSelectionLabel)
        Me.groupBox1.Controls.Add(Me.folderSelectionLabel)
        Me.groupBox1.FlatStyle = System.Windows.Forms.FlatStyle.Popup
        Me.groupBox1.ForeColor = System.Drawing.Color.Black
        Me.groupBox1.Location = New System.Drawing.Point(6, 179)
        Me.groupBox1.Margin = New System.Windows.Forms.Padding(1)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Padding = New System.Windows.Forms.Padding(1)
        Me.groupBox1.Size = New System.Drawing.Size(318, 124)
        Me.groupBox1.TabIndex = 1
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Workflow Description"
        '
        'actionsSelectionLabel
        '
        Me.actionsSelectionLabel.ForeColor = System.Drawing.Color.DarkGreen
        Me.actionsSelectionLabel.Location = New System.Drawing.Point(56, 66)
        Me.actionsSelectionLabel.Name = "actionsSelectionLabel"
        Me.actionsSelectionLabel.Size = New System.Drawing.Size(256, 52)
        Me.actionsSelectionLabel.TabIndex = 3
        Me.actionsSelectionLabel.Text = "L3"
        '
        'ifParameterField
        '
        Me.ifParameterField.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.ifParameterField.Location = New System.Drawing.Point(119, 42)
        Me.ifParameterField.Name = "ifParameterField"
        Me.ifParameterField.Size = New System.Drawing.Size(175, 20)
        Me.ifParameterField.TabIndex = 3
        Me.ifParameterField.Visible = False
        '
        'conditionSelectionLabel
        '
        Me.conditionSelectionLabel.AutoSize = True
        Me.conditionSelectionLabel.ForeColor = System.Drawing.Color.Maroon
        Me.conditionSelectionLabel.Location = New System.Drawing.Point(34, 44)
        Me.conditionSelectionLabel.Name = "conditionSelectionLabel"
        Me.conditionSelectionLabel.Size = New System.Drawing.Size(19, 13)
        Me.conditionSelectionLabel.TabIndex = 1
        Me.conditionSelectionLabel.Text = "L2"
        '
        'folderSelectionLabel
        '
        Me.folderSelectionLabel.AutoSize = True
        Me.folderSelectionLabel.ForeColor = System.Drawing.SystemColors.Desktop
        Me.folderSelectionLabel.Location = New System.Drawing.Point(10, 20)
        Me.folderSelectionLabel.Name = "folderSelectionLabel"
        Me.folderSelectionLabel.Size = New System.Drawing.Size(19, 13)
        Me.folderSelectionLabel.TabIndex = 0
        Me.folderSelectionLabel.Text = "L1"
        '
        'WizardForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(693, 461)
        Me.Controls.Add(Me.splitContainer1)
        Me.Name = "WizardForm"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "Workflow Wizard Application"
        Me.tabPage5.ResumeLayout(False)
        Me.splitContainer1.Panel1.ResumeLayout(False)
        Me.splitContainer1.ResumeLayout(False)
        Me.tabControl1.ResumeLayout(False)
        Me.tabPage1.ResumeLayout(False)
        Me.tabPage1.PerformLayout()
        Me.tabPage2.ResumeLayout(False)
        Me.tabPage2.PerformLayout()
        Me.tabPage3.ResumeLayout(False)
        Me.groupBox1.ResumeLayout(False)
        Me.groupBox1.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents compileWorkflowButton As System.Windows.Forms.Button
    Private WithEvents startWorkflowButton As System.Windows.Forms.Button
    Private WithEvents ifParameterLinkLabel As System.Windows.Forms.LinkLabel
    Private WithEvents actionsCheckedListBox As System.Windows.Forms.CheckedListBox
    Private WithEvents tabPage5 As System.Windows.Forms.TabPage
    Private WithEvents generateWorkflowButton As System.Windows.Forms.Button
    Private WithEvents nextStepButton As System.Windows.Forms.Button
    Private WithEvents splitContainer1 As System.Windows.Forms.SplitContainer
    Private WithEvents tabControl1 As System.Windows.Forms.TabControl
    Private WithEvents tabPage1 As System.Windows.Forms.TabPage
    Private WithEvents outboxRadioButton As System.Windows.Forms.RadioButton
    Private WithEvents sentRadioButton As System.Windows.Forms.RadioButton
    Private WithEvents inboxRadioButton As System.Windows.Forms.RadioButton
    Private WithEvents tabPage2 As System.Windows.Forms.TabPage
    Private WithEvents ccRadioButton As System.Windows.Forms.RadioButton
    Private WithEvents toRadioButton As System.Windows.Forms.RadioButton
    Private WithEvents bccRadioButton As System.Windows.Forms.RadioButton
    Private WithEvents fromRadioButton As System.Windows.Forms.RadioButton
    Private WithEvents subjectRadioButton As System.Windows.Forms.RadioButton
    Private WithEvents tabPage3 As System.Windows.Forms.TabPage
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents ifParameterField As System.Windows.Forms.TextBox
    Private WithEvents conditionSelectionLabel As System.Windows.Forms.Label
    Private WithEvents folderSelectionLabel As System.Windows.Forms.Label
    Friend WithEvents actionsSelectionLabel As System.Windows.Forms.Label

End Class
