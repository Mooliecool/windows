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
        Me.groupBox2 = New System.Windows.Forms.GroupBox
        Me.txtBoxIntro = New System.Windows.Forms.TextBox
        Me.lblIntroduction = New System.Windows.Forms.Label
        Me.btnLinqToCSV = New System.Windows.Forms.Button
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.btnLinqToFileSystem = New System.Windows.Forms.Button
        Me.btnLinqToString = New System.Windows.Forms.Button
        Me.btnLinqToReflection = New System.Windows.Forms.Button
        Me.groupBox2.SuspendLayout()
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'groupBox2
        '
        Me.groupBox2.Controls.Add(Me.txtBoxIntro)
        Me.groupBox2.Controls.Add(Me.lblIntroduction)
        Me.groupBox2.Location = New System.Drawing.Point(204, 20)
        Me.groupBox2.Name = "groupBox2"
        Me.groupBox2.Size = New System.Drawing.Size(312, 338)
        Me.groupBox2.TabIndex = 5
        Me.groupBox2.TabStop = False
        Me.groupBox2.Text = "Introduction"
        '
        'txtBoxIntro
        '
        Me.txtBoxIntro.Enabled = False
        Me.txtBoxIntro.Location = New System.Drawing.Point(17, 61)
        Me.txtBoxIntro.Multiline = True
        Me.txtBoxIntro.Name = "txtBoxIntro"
        Me.txtBoxIntro.Size = New System.Drawing.Size(281, 262)
        Me.txtBoxIntro.TabIndex = 1
        '
        'lblIntroduction
        '
        Me.lblIntroduction.AutoSize = True
        Me.lblIntroduction.Location = New System.Drawing.Point(29, 35)
        Me.lblIntroduction.Name = "lblIntroduction"
        Me.lblIntroduction.Size = New System.Drawing.Size(0, 12)
        Me.lblIntroduction.TabIndex = 0
        '
        'btnLinqToCSV
        '
        Me.btnLinqToCSV.Location = New System.Drawing.Point(41, 46)
        Me.btnLinqToCSV.Name = "btnLinqToCSV"
        Me.btnLinqToCSV.Size = New System.Drawing.Size(130, 30)
        Me.btnLinqToCSV.TabIndex = 0
        Me.btnLinqToCSV.Text = "LINQ To CSV"
        Me.btnLinqToCSV.UseVisualStyleBackColor = True
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.groupBox2)
        Me.groupBox1.Controls.Add(Me.btnLinqToCSV)
        Me.groupBox1.Controls.Add(Me.btnLinqToFileSystem)
        Me.groupBox1.Controls.Add(Me.btnLinqToString)
        Me.groupBox1.Controls.Add(Me.btnLinqToReflection)
        Me.groupBox1.Location = New System.Drawing.Point(12, 12)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(535, 377)
        Me.groupBox1.TabIndex = 6
        Me.groupBox1.TabStop = False
        '
        'btnLinqToFileSystem
        '
        Me.btnLinqToFileSystem.Location = New System.Drawing.Point(41, 120)
        Me.btnLinqToFileSystem.Name = "btnLinqToFileSystem"
        Me.btnLinqToFileSystem.Size = New System.Drawing.Size(130, 30)
        Me.btnLinqToFileSystem.TabIndex = 1
        Me.btnLinqToFileSystem.Text = "LINQ To File System"
        Me.btnLinqToFileSystem.UseVisualStyleBackColor = True
        '
        'btnLinqToString
        '
        Me.btnLinqToString.Location = New System.Drawing.Point(41, 268)
        Me.btnLinqToString.Name = "btnLinqToString"
        Me.btnLinqToString.Size = New System.Drawing.Size(130, 30)
        Me.btnLinqToString.TabIndex = 3
        Me.btnLinqToString.Text = "LINQ To String"
        Me.btnLinqToString.UseVisualStyleBackColor = True
        '
        'btnLinqToReflection
        '
        Me.btnLinqToReflection.Location = New System.Drawing.Point(41, 194)
        Me.btnLinqToReflection.Name = "btnLinqToReflection"
        Me.btnLinqToReflection.Size = New System.Drawing.Size(130, 30)
        Me.btnLinqToReflection.TabIndex = 2
        Me.btnLinqToReflection.Text = "LINQ To Reflection"
        Me.btnLinqToReflection.UseVisualStyleBackColor = True
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(559, 401)
        Me.Controls.Add(Me.groupBox1)
        Me.Name = "MainForm"
        Me.Text = "VBLinqExtension"
        Me.groupBox2.ResumeLayout(False)
        Me.groupBox2.PerformLayout()
        Me.groupBox1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents groupBox2 As System.Windows.Forms.GroupBox
    Private WithEvents txtBoxIntro As System.Windows.Forms.TextBox
    Private WithEvents lblIntroduction As System.Windows.Forms.Label
    Private WithEvents btnLinqToCSV As System.Windows.Forms.Button
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents btnLinqToFileSystem As System.Windows.Forms.Button
    Private WithEvents btnLinqToString As System.Windows.Forms.Button
    Private WithEvents btnLinqToReflection As System.Windows.Forms.Button
End Class
