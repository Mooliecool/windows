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
        Me.openSubFormButton = New System.Windows.Forms.Button
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.setSubFormAppIDButton = New System.Windows.Forms.Button
        Me.resetSubFormAppIDButton = New System.Windows.Forms.Button
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'openSubFormButton
        '
        Me.openSubFormButton.Location = New System.Drawing.Point(60, 43)
        Me.openSubFormButton.Name = "openSubFormButton"
        Me.openSubFormButton.Size = New System.Drawing.Size(98, 23)
        Me.openSubFormButton.TabIndex = 1
        Me.openSubFormButton.Text = "Open Sub Form"
        Me.openSubFormButton.UseVisualStyleBackColor = True
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.setSubFormAppIDButton)
        Me.groupBox1.Controls.Add(Me.resetSubFormAppIDButton)
        Me.groupBox1.Location = New System.Drawing.Point(228, 21)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(200, 73)
        Me.groupBox1.TabIndex = 5
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Set/Reset SubForm AppID:"
        '
        'setSubFormAppIDButton
        '
        Me.setSubFormAppIDButton.Enabled = False
        Me.setSubFormAppIDButton.Location = New System.Drawing.Point(24, 31)
        Me.setSubFormAppIDButton.Name = "setSubFormAppIDButton"
        Me.setSubFormAppIDButton.Size = New System.Drawing.Size(63, 23)
        Me.setSubFormAppIDButton.TabIndex = 3
        Me.setSubFormAppIDButton.Text = "Set"
        Me.setSubFormAppIDButton.UseVisualStyleBackColor = True
        '
        'resetSubFormAppIDButton
        '
        Me.resetSubFormAppIDButton.Enabled = False
        Me.resetSubFormAppIDButton.Location = New System.Drawing.Point(109, 31)
        Me.resetSubFormAppIDButton.Name = "resetSubFormAppIDButton"
        Me.resetSubFormAppIDButton.Size = New System.Drawing.Size(63, 23)
        Me.resetSubFormAppIDButton.TabIndex = 1
        Me.resetSubFormAppIDButton.Text = "Reset"
        Me.resetSubFormAppIDButton.UseVisualStyleBackColor = True
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(460, 115)
        Me.Controls.Add(Me.groupBox1)
        Me.Controls.Add(Me.openSubFormButton)
        Me.Name = "MainForm"
        Me.Text = "MainForm"
        Me.groupBox1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents openSubFormButton As System.Windows.Forms.Button
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents setSubFormAppIDButton As System.Windows.Forms.Button
    Private WithEvents resetSubFormAppIDButton As System.Windows.Forms.Button

End Class
