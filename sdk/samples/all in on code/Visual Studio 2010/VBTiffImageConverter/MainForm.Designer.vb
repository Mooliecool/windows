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
        Me.groupBox1 = New System.Windows.Forms.GroupBox()
        Me.textBox1 = New System.Windows.Forms.TextBox()
        Me.chkIsMultipage = New System.Windows.Forms.CheckBox()
        Me.btnConvertToTiff = New System.Windows.Forms.Button()
        Me.groupBox2 = New System.Windows.Forms.GroupBox()
        Me.textBox2 = New System.Windows.Forms.TextBox()
        Me.btnConvertToJpeg = New System.Windows.Forms.Button()
        Me.dlgOpenFileDialog = New System.Windows.Forms.OpenFileDialog()
        Me.groupBox1.SuspendLayout()
        Me.groupBox2.SuspendLayout()
        Me.SuspendLayout()
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.textBox1)
        Me.groupBox1.Controls.Add(Me.chkIsMultipage)
        Me.groupBox1.Controls.Add(Me.btnConvertToTiff)
        Me.groupBox1.Location = New System.Drawing.Point(8, 7)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(349, 137)
        Me.groupBox1.TabIndex = 5
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Jpeg -> Tiff"
        '
        'textBox1
        '
        Me.textBox1.BackColor = System.Drawing.SystemColors.Control
        Me.textBox1.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.textBox1.Location = New System.Drawing.Point(22, 20)
        Me.textBox1.Multiline = True
        Me.textBox1.Name = "textBox1"
        Me.textBox1.ReadOnly = True
        Me.textBox1.Size = New System.Drawing.Size(303, 37)
        Me.textBox1.TabIndex = 3
        Me.textBox1.Text = "Click on ""Convert To Tiff"" button to browse the jpeg images & also converts them " & _
    "into tiff file & saves @ the same location."
        '
        'chkIsMultipage
        '
        Me.chkIsMultipage.AutoSize = True
        Me.chkIsMultipage.Checked = True
        Me.chkIsMultipage.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkIsMultipage.Location = New System.Drawing.Point(22, 64)
        Me.chkIsMultipage.Name = "chkIsMultipage"
        Me.chkIsMultipage.Size = New System.Drawing.Size(216, 17)
        Me.chkIsMultipage.TabIndex = 2
        Me.chkIsMultipage.Text = "Check to create multipage tiff (single) file"
        Me.chkIsMultipage.UseVisualStyleBackColor = True
        '
        'btnConvertToTiff
        '
        Me.btnConvertToTiff.Location = New System.Drawing.Point(70, 100)
        Me.btnConvertToTiff.Name = "btnConvertToTiff"
        Me.btnConvertToTiff.Size = New System.Drawing.Size(179, 23)
        Me.btnConvertToTiff.TabIndex = 0
        Me.btnConvertToTiff.Text = "Convert To Tiff"
        Me.btnConvertToTiff.UseVisualStyleBackColor = True
        '
        'groupBox2
        '
        Me.groupBox2.Controls.Add(Me.textBox2)
        Me.groupBox2.Controls.Add(Me.btnConvertToJpeg)
        Me.groupBox2.Location = New System.Drawing.Point(8, 150)
        Me.groupBox2.Name = "groupBox2"
        Me.groupBox2.Size = New System.Drawing.Size(349, 153)
        Me.groupBox2.TabIndex = 6
        Me.groupBox2.TabStop = False
        Me.groupBox2.Text = "Tiff -> Jpeg"
        '
        'textBox2
        '
        Me.textBox2.BackColor = System.Drawing.SystemColors.Control
        Me.textBox2.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.textBox2.Location = New System.Drawing.Point(23, 32)
        Me.textBox2.Multiline = True
        Me.textBox2.Name = "textBox2"
        Me.textBox2.ReadOnly = True
        Me.textBox2.Size = New System.Drawing.Size(303, 37)
        Me.textBox2.TabIndex = 5
        Me.textBox2.Text = "Click on ""Convert To Jpeg"" button to browse the tiff images & also converts them " & _
    "into jpeg file & saves @ the same location."
        '
        'btnConvertToJpeg
        '
        Me.btnConvertToJpeg.Location = New System.Drawing.Point(70, 99)
        Me.btnConvertToJpeg.Name = "btnConvertToJpeg"
        Me.btnConvertToJpeg.Size = New System.Drawing.Size(179, 23)
        Me.btnConvertToJpeg.TabIndex = 1
        Me.btnConvertToJpeg.Text = "Convert To Jpeg"
        Me.btnConvertToJpeg.UseVisualStyleBackColor = True
        '
        'dlgOpenFileDialog
        '
        Me.dlgOpenFileDialog.Filter = "Image files (.jpg, .jpeg, .tif)|*.jpg;*.jpeg;*.tif;*.tiff"
        Me.dlgOpenFileDialog.Multiselect = True
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(364, 311)
        Me.Controls.Add(Me.groupBox1)
        Me.Controls.Add(Me.groupBox2)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.Name = "MainForm"
        Me.Text = "VBTiffImageConverter"
        Me.groupBox1.ResumeLayout(False)
        Me.groupBox1.PerformLayout()
        Me.groupBox2.ResumeLayout(False)
        Me.groupBox2.PerformLayout()
        Me.ResumeLayout(False)

    End Sub


    Friend WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents textBox1 As System.Windows.Forms.TextBox
    Friend WithEvents chkIsMultipage As System.Windows.Forms.CheckBox
    Friend WithEvents btnConvertToTiff As System.Windows.Forms.Button
    Friend WithEvents groupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents textBox2 As System.Windows.Forms.TextBox
    Friend WithEvents btnConvertToJpeg As System.Windows.Forms.Button
    Friend WithEvents dlgOpenFileDialog As System.Windows.Forms.OpenFileDialog

End Class