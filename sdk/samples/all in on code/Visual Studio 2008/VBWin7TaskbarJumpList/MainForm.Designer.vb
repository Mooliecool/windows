<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class MainForm
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.groupBox4 = New System.Windows.Forms.GroupBox
        Me.addLinkButton = New System.Windows.Forms.Button
        Me.tbLink = New System.Windows.Forms.TextBox
        Me.btnAddItem = New System.Windows.Forms.Button
        Me.tbItem = New System.Windows.Forms.TextBox
        Me.tbCategory = New System.Windows.Forms.TextBox
        Me.btnCreateCategory = New System.Windows.Forms.Button
        Me.groupBox3 = New System.Windows.Forms.GroupBox
        Me.chkPaint = New System.Windows.Forms.CheckBox
        Me.chkCalc = New System.Windows.Forms.CheckBox
        Me.chkNotepad = New System.Windows.Forms.CheckBox
        Me.btnClearTask = New System.Windows.Forms.Button
        Me.btnAddTask = New System.Windows.Forms.Button
        Me.groupBox2 = New System.Windows.Forms.GroupBox
        Me.btnChooseFile = New System.Windows.Forms.Button
        Me.radRecent = New System.Windows.Forms.RadioButton
        Me.radFrequent = New System.Windows.Forms.RadioButton
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.btnUnregisterFileType = New System.Windows.Forms.Button
        Me.btnRegisterFileType = New System.Windows.Forms.Button
        Me.recentFileOpenFileDialog = New System.Windows.Forms.OpenFileDialog
        Me.groupBox4.SuspendLayout()
        Me.groupBox3.SuspendLayout()
        Me.groupBox2.SuspendLayout()
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'groupBox4
        '
        Me.groupBox4.Controls.Add(Me.addLinkButton)
        Me.groupBox4.Controls.Add(Me.tbLink)
        Me.groupBox4.Controls.Add(Me.btnAddItem)
        Me.groupBox4.Controls.Add(Me.tbItem)
        Me.groupBox4.Controls.Add(Me.tbCategory)
        Me.groupBox4.Controls.Add(Me.btnCreateCategory)
        Me.groupBox4.Location = New System.Drawing.Point(28, 293)
        Me.groupBox4.Name = "groupBox4"
        Me.groupBox4.Size = New System.Drawing.Size(334, 89)
        Me.groupBox4.TabIndex = 8
        Me.groupBox4.TabStop = False
        Me.groupBox4.Text = "Categories"
        '
        'addLinkButton
        '
        Me.addLinkButton.Location = New System.Drawing.Point(248, 46)
        Me.addLinkButton.Name = "addLinkButton"
        Me.addLinkButton.Size = New System.Drawing.Size(65, 23)
        Me.addLinkButton.TabIndex = 9
        Me.addLinkButton.Text = "Add Link"
        Me.addLinkButton.UseVisualStyleBackColor = True
        '
        'tbLink
        '
        Me.tbLink.Location = New System.Drawing.Point(172, 48)
        Me.tbLink.Name = "tbLink"
        Me.tbLink.Size = New System.Drawing.Size(70, 20)
        Me.tbLink.TabIndex = 8
        Me.tbLink.Text = "Link1"
        '
        'btnAddItem
        '
        Me.btnAddItem.Location = New System.Drawing.Point(98, 47)
        Me.btnAddItem.Name = "btnAddItem"
        Me.btnAddItem.Size = New System.Drawing.Size(65, 23)
        Me.btnAddItem.TabIndex = 7
        Me.btnAddItem.Text = "Add Item"
        Me.btnAddItem.UseVisualStyleBackColor = True
        '
        'tbItem
        '
        Me.tbItem.Location = New System.Drawing.Point(20, 48)
        Me.tbItem.Name = "tbItem"
        Me.tbItem.Size = New System.Drawing.Size(77, 20)
        Me.tbItem.TabIndex = 6
        Me.tbItem.Text = "Item1"
        '
        'tbCategory
        '
        Me.tbCategory.Location = New System.Drawing.Point(20, 22)
        Me.tbCategory.Name = "tbCategory"
        Me.tbCategory.Size = New System.Drawing.Size(77, 20)
        Me.tbCategory.TabIndex = 5
        Me.tbCategory.Text = "MyCategory"
        '
        'btnCreateCategory
        '
        Me.btnCreateCategory.Location = New System.Drawing.Point(98, 20)
        Me.btnCreateCategory.Name = "btnCreateCategory"
        Me.btnCreateCategory.Size = New System.Drawing.Size(65, 23)
        Me.btnCreateCategory.TabIndex = 0
        Me.btnCreateCategory.Text = "Create"
        Me.btnCreateCategory.UseVisualStyleBackColor = True
        '
        'groupBox3
        '
        Me.groupBox3.Controls.Add(Me.chkPaint)
        Me.groupBox3.Controls.Add(Me.chkCalc)
        Me.groupBox3.Controls.Add(Me.chkNotepad)
        Me.groupBox3.Controls.Add(Me.btnClearTask)
        Me.groupBox3.Controls.Add(Me.btnAddTask)
        Me.groupBox3.Location = New System.Drawing.Point(28, 198)
        Me.groupBox3.Name = "groupBox3"
        Me.groupBox3.Size = New System.Drawing.Size(334, 89)
        Me.groupBox3.TabIndex = 7
        Me.groupBox3.TabStop = False
        Me.groupBox3.Text = "Tasks"
        '
        'chkPaint
        '
        Me.chkPaint.AutoSize = True
        Me.chkPaint.Location = New System.Drawing.Point(20, 58)
        Me.chkPaint.Name = "chkPaint"
        Me.chkPaint.Size = New System.Drawing.Size(49, 17)
        Me.chkPaint.TabIndex = 4
        Me.chkPaint.Text = "paint"
        Me.chkPaint.UseVisualStyleBackColor = True
        '
        'chkCalc
        '
        Me.chkCalc.AutoSize = True
        Me.chkCalc.Location = New System.Drawing.Point(20, 39)
        Me.chkCalc.Name = "chkCalc"
        Me.chkCalc.Size = New System.Drawing.Size(72, 17)
        Me.chkCalc.TabIndex = 3
        Me.chkCalc.Text = "calculator"
        Me.chkCalc.UseVisualStyleBackColor = True
        '
        'chkNotepad
        '
        Me.chkNotepad.AutoSize = True
        Me.chkNotepad.Location = New System.Drawing.Point(20, 20)
        Me.chkNotepad.Name = "chkNotepad"
        Me.chkNotepad.Size = New System.Drawing.Size(65, 17)
        Me.chkNotepad.TabIndex = 2
        Me.chkNotepad.Text = "notepad"
        Me.chkNotepad.UseVisualStyleBackColor = True
        '
        'btnClearTask
        '
        Me.btnClearTask.Location = New System.Drawing.Point(229, 34)
        Me.btnClearTask.Name = "btnClearTask"
        Me.btnClearTask.Size = New System.Drawing.Size(84, 23)
        Me.btnClearTask.TabIndex = 1
        Me.btnClearTask.Text = "Clear Tasks"
        Me.btnClearTask.UseVisualStyleBackColor = True
        '
        'btnAddTask
        '
        Me.btnAddTask.Location = New System.Drawing.Point(141, 34)
        Me.btnAddTask.Name = "btnAddTask"
        Me.btnAddTask.Size = New System.Drawing.Size(82, 23)
        Me.btnAddTask.TabIndex = 0
        Me.btnAddTask.Text = "Add Tasks"
        Me.btnAddTask.UseVisualStyleBackColor = True
        '
        'groupBox2
        '
        Me.groupBox2.Controls.Add(Me.btnChooseFile)
        Me.groupBox2.Controls.Add(Me.radRecent)
        Me.groupBox2.Controls.Add(Me.radFrequent)
        Me.groupBox2.Location = New System.Drawing.Point(28, 114)
        Me.groupBox2.Name = "groupBox2"
        Me.groupBox2.Size = New System.Drawing.Size(334, 78)
        Me.groupBox2.TabIndex = 6
        Me.groupBox2.TabStop = False
        Me.groupBox2.Text = "Frequent/Recent Category"
        '
        'btnChooseFile
        '
        Me.btnChooseFile.Location = New System.Drawing.Point(229, 19)
        Me.btnChooseFile.Name = "btnChooseFile"
        Me.btnChooseFile.Size = New System.Drawing.Size(84, 23)
        Me.btnChooseFile.TabIndex = 4
        Me.btnChooseFile.Text = "Choose File..."
        Me.btnChooseFile.UseVisualStyleBackColor = True
        '
        'radRecent
        '
        Me.radRecent.AutoSize = True
        Me.radRecent.Location = New System.Drawing.Point(20, 42)
        Me.radRecent.Name = "radRecent"
        Me.radRecent.Size = New System.Drawing.Size(60, 17)
        Me.radRecent.TabIndex = 3
        Me.radRecent.TabStop = True
        Me.radRecent.Text = "Recent"
        Me.radRecent.UseVisualStyleBackColor = True
        '
        'radFrequent
        '
        Me.radFrequent.AutoSize = True
        Me.radFrequent.Location = New System.Drawing.Point(20, 19)
        Me.radFrequent.Name = "radFrequent"
        Me.radFrequent.Size = New System.Drawing.Size(67, 17)
        Me.radFrequent.TabIndex = 0
        Me.radFrequent.TabStop = True
        Me.radFrequent.Text = "Frequent"
        Me.radFrequent.UseVisualStyleBackColor = True
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.btnUnregisterFileType)
        Me.groupBox1.Controls.Add(Me.btnRegisterFileType)
        Me.groupBox1.Location = New System.Drawing.Point(28, 26)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(334, 82)
        Me.groupBox1.TabIndex = 5
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "File Type Registration"
        '
        'btnUnregisterFileType
        '
        Me.btnUnregisterFileType.Location = New System.Drawing.Point(198, 19)
        Me.btnUnregisterFileType.Name = "btnUnregisterFileType"
        Me.btnUnregisterFileType.Size = New System.Drawing.Size(115, 41)
        Me.btnUnregisterFileType.TabIndex = 1
        Me.btnUnregisterFileType.Text = "Unregister File Type"
        Me.btnUnregisterFileType.UseVisualStyleBackColor = True
        '
        'btnRegisterFileType
        '
        Me.btnRegisterFileType.Location = New System.Drawing.Point(20, 19)
        Me.btnRegisterFileType.Name = "btnRegisterFileType"
        Me.btnRegisterFileType.Size = New System.Drawing.Size(115, 41)
        Me.btnRegisterFileType.TabIndex = 0
        Me.btnRegisterFileType.Text = "Register File Type"
        Me.btnRegisterFileType.UseVisualStyleBackColor = True
        '
        'recentFileOpenFileDialog
        '
        Me.recentFileOpenFileDialog.Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*"""
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(390, 416)
        Me.Controls.Add(Me.groupBox4)
        Me.Controls.Add(Me.groupBox3)
        Me.Controls.Add(Me.groupBox2)
        Me.Controls.Add(Me.groupBox1)
        Me.Name = "MainForm"
        Me.Text = "Win7 Taskbar JumpList"
        Me.groupBox4.ResumeLayout(False)
        Me.groupBox4.PerformLayout()
        Me.groupBox3.ResumeLayout(False)
        Me.groupBox3.PerformLayout()
        Me.groupBox2.ResumeLayout(False)
        Me.groupBox2.PerformLayout()
        Me.groupBox1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents groupBox4 As System.Windows.Forms.GroupBox
    Private WithEvents addLinkButton As System.Windows.Forms.Button
    Private WithEvents tbLink As System.Windows.Forms.TextBox
    Private WithEvents btnAddItem As System.Windows.Forms.Button
    Private WithEvents tbItem As System.Windows.Forms.TextBox
    Private WithEvents tbCategory As System.Windows.Forms.TextBox
    Private WithEvents btnCreateCategory As System.Windows.Forms.Button
    Private WithEvents groupBox3 As System.Windows.Forms.GroupBox
    Private WithEvents chkPaint As System.Windows.Forms.CheckBox
    Private WithEvents chkCalc As System.Windows.Forms.CheckBox
    Private WithEvents chkNotepad As System.Windows.Forms.CheckBox
    Private WithEvents btnClearTask As System.Windows.Forms.Button
    Private WithEvents btnAddTask As System.Windows.Forms.Button
    Private WithEvents groupBox2 As System.Windows.Forms.GroupBox
    Private WithEvents btnChooseFile As System.Windows.Forms.Button
    Private WithEvents radRecent As System.Windows.Forms.RadioButton
    Private WithEvents radFrequent As System.Windows.Forms.RadioButton
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents btnUnregisterFileType As System.Windows.Forms.Button
    Private WithEvents btnRegisterFileType As System.Windows.Forms.Button
    Private WithEvents recentFileOpenFileDialog As System.Windows.Forms.OpenFileDialog

End Class
