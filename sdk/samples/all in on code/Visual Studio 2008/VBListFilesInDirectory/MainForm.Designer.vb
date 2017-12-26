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
        Me.chkFiles = New System.Windows.Forms.CheckBox
        Me.chkDir = New System.Windows.Forms.CheckBox
        Me.label1 = New System.Windows.Forms.Label
        Me.txtPattern = New System.Windows.Forms.TextBox
        Me.lblCount = New System.Windows.Forms.Label
        Me.lstFiles = New System.Windows.Forms.ListBox
        Me.btnList = New System.Windows.Forms.Button
        Me.txtDir = New System.Windows.Forms.TextBox
        Me.SuspendLayout()
        '
        'chkFiles
        '
        Me.chkFiles.AutoSize = True
        Me.chkFiles.Checked = True
        Me.chkFiles.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkFiles.Location = New System.Drawing.Point(86, 38)
        Me.chkFiles.Name = "chkFiles"
        Me.chkFiles.Size = New System.Drawing.Size(47, 17)
        Me.chkFiles.TabIndex = 15
        Me.chkFiles.Text = "Files"
        Me.chkFiles.UseVisualStyleBackColor = True
        '
        'chkDir
        '
        Me.chkDir.AutoSize = True
        Me.chkDir.Checked = True
        Me.chkDir.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkDir.Location = New System.Drawing.Point(12, 38)
        Me.chkDir.Name = "chkDir"
        Me.chkDir.Size = New System.Drawing.Size(68, 17)
        Me.chkDir.TabIndex = 14
        Me.chkDir.Text = "Directory"
        Me.chkDir.UseVisualStyleBackColor = True
        '
        'label1
        '
        Me.label1.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(307, 15)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(12, 13)
        Me.label1.TabIndex = 9
        Me.label1.Text = "\"
        '
        'txtPattern
        '
        Me.txtPattern.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.txtPattern.Location = New System.Drawing.Point(325, 12)
        Me.txtPattern.Name = "txtPattern"
        Me.txtPattern.Size = New System.Drawing.Size(96, 20)
        Me.txtPattern.TabIndex = 10
        Me.txtPattern.Text = "*.*"
        '
        'lblCount
        '
        Me.lblCount.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.lblCount.AutoSize = True
        Me.lblCount.Location = New System.Drawing.Point(12, 364)
        Me.lblCount.Name = "lblCount"
        Me.lblCount.Size = New System.Drawing.Size(58, 13)
        Me.lblCount.TabIndex = 13
        Me.lblCount.Text = "Item Count"
        '
        'lstFiles
        '
        Me.lstFiles.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lstFiles.FormattingEnabled = True
        Me.lstFiles.Location = New System.Drawing.Point(12, 64)
        Me.lstFiles.Name = "lstFiles"
        Me.lstFiles.Size = New System.Drawing.Size(490, 290)
        Me.lstFiles.TabIndex = 12
        '
        'btnList
        '
        Me.btnList.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnList.Enabled = False
        Me.btnList.Location = New System.Drawing.Point(427, 10)
        Me.btnList.Name = "btnList"
        Me.btnList.Size = New System.Drawing.Size(75, 23)
        Me.btnList.TabIndex = 11
        Me.btnList.Text = "List"
        Me.btnList.UseVisualStyleBackColor = True
        '
        'txtDir
        '
        Me.txtDir.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.txtDir.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest
        Me.txtDir.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.FileSystemDirectories
        Me.txtDir.Location = New System.Drawing.Point(12, 12)
        Me.txtDir.Name = "txtDir"
        Me.txtDir.Size = New System.Drawing.Size(289, 20)
        Me.txtDir.TabIndex = 8
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(514, 386)
        Me.Controls.Add(Me.chkFiles)
        Me.Controls.Add(Me.chkDir)
        Me.Controls.Add(Me.label1)
        Me.Controls.Add(Me.txtPattern)
        Me.Controls.Add(Me.lblCount)
        Me.Controls.Add(Me.lstFiles)
        Me.Controls.Add(Me.btnList)
        Me.Controls.Add(Me.txtDir)
        Me.MinimumSize = New System.Drawing.Size(500, 400)
        Me.Name = "MainForm"
        Me.Text = "File Enum Test"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents chkFiles As System.Windows.Forms.CheckBox
    Private WithEvents chkDir As System.Windows.Forms.CheckBox
    Private WithEvents label1 As System.Windows.Forms.Label
    Private WithEvents txtPattern As System.Windows.Forms.TextBox
    Private WithEvents lblCount As System.Windows.Forms.Label
    Private WithEvents lstFiles As System.Windows.Forms.ListBox
    Private WithEvents btnList As System.Windows.Forms.Button
    Private WithEvents txtDir As System.Windows.Forms.TextBox

End Class
