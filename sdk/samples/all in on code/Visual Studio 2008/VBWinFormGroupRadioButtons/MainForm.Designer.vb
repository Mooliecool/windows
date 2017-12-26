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
        Me.grp1 = New System.Windows.Forms.GroupBox
        Me.rad2 = New System.Windows.Forms.RadioButton
        Me.rad1 = New System.Windows.Forms.RadioButton
        Me.grp2 = New System.Windows.Forms.GroupBox
        Me.rad4 = New System.Windows.Forms.RadioButton
        Me.rad3 = New System.Windows.Forms.RadioButton
        Me.lb = New System.Windows.Forms.Label
        Me.grp1.SuspendLayout()
        Me.grp2.SuspendLayout()
        Me.SuspendLayout()
        '
        'grp1
        '
        Me.grp1.Controls.Add(Me.rad2)
        Me.grp1.Controls.Add(Me.rad1)
        Me.grp1.Location = New System.Drawing.Point(13, 13)
        Me.grp1.Name = "grp1"
        Me.grp1.Size = New System.Drawing.Size(267, 100)
        Me.grp1.TabIndex = 0
        Me.grp1.TabStop = False
        Me.grp1.Text = "grp1"
        '
        'rad2
        '
        Me.rad2.AutoSize = True
        Me.rad2.Location = New System.Drawing.Point(7, 77)
        Me.rad2.Name = "rad2"
        Me.rad2.Size = New System.Drawing.Size(46, 17)
        Me.rad2.TabIndex = 1
        Me.rad2.Text = "rad2"
        Me.rad2.UseVisualStyleBackColor = True
        '
        'rad1
        '
        Me.rad1.AutoSize = True
        Me.rad1.Checked = True
        Me.rad1.Location = New System.Drawing.Point(7, 20)
        Me.rad1.Name = "rad1"
        Me.rad1.Size = New System.Drawing.Size(46, 17)
        Me.rad1.TabIndex = 0
        Me.rad1.TabStop = True
        Me.rad1.Text = "rad1"
        Me.rad1.UseVisualStyleBackColor = True
        '
        'grp2
        '
        Me.grp2.Controls.Add(Me.rad4)
        Me.grp2.Controls.Add(Me.rad3)
        Me.grp2.Location = New System.Drawing.Point(13, 161)
        Me.grp2.Name = "grp2"
        Me.grp2.Size = New System.Drawing.Size(267, 100)
        Me.grp2.TabIndex = 1
        Me.grp2.TabStop = False
        Me.grp2.Text = "grp2"
        '
        'rad4
        '
        Me.rad4.AutoSize = True
        Me.rad4.Location = New System.Drawing.Point(7, 77)
        Me.rad4.Name = "rad4"
        Me.rad4.Size = New System.Drawing.Size(46, 17)
        Me.rad4.TabIndex = 1
        Me.rad4.TabStop = True
        Me.rad4.Text = "rad4"
        Me.rad4.UseVisualStyleBackColor = True
        '
        'rad3
        '
        Me.rad3.AutoSize = True
        Me.rad3.Location = New System.Drawing.Point(7, 20)
        Me.rad3.Name = "rad3"
        Me.rad3.Size = New System.Drawing.Size(46, 17)
        Me.rad3.TabIndex = 0
        Me.rad3.TabStop = True
        Me.rad3.Text = "rad3"
        Me.rad3.UseVisualStyleBackColor = True
        '
        'lb
        '
        Me.lb.AutoSize = True
        Me.lb.Location = New System.Drawing.Point(17, 132)
        Me.lb.Name = "lb"
        Me.lb.Size = New System.Drawing.Size(118, 13)
        Me.lb.TabIndex = 2
        Me.lb.Text = "rad1 has been selected"
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(292, 273)
        Me.Controls.Add(Me.lb)
        Me.Controls.Add(Me.grp2)
        Me.Controls.Add(Me.grp1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D
        Me.MaximizeBox = False
        Me.Name = "MainForm"
        Me.Text = "VBWinFormGroupRadioButtons"
        Me.grp1.ResumeLayout(False)
        Me.grp1.PerformLayout()
        Me.grp2.ResumeLayout(False)
        Me.grp2.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents grp1 As System.Windows.Forms.GroupBox
    Friend WithEvents rad2 As System.Windows.Forms.RadioButton
    Friend WithEvents rad1 As System.Windows.Forms.RadioButton
    Friend WithEvents grp2 As System.Windows.Forms.GroupBox
    Friend WithEvents rad4 As System.Windows.Forms.RadioButton
    Friend WithEvents rad3 As System.Windows.Forms.RadioButton
    Friend WithEvents lb As System.Windows.Forms.Label

End Class
