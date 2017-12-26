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
        Me.statusLabel = New System.Windows.Forms.Label
        Me.showIconCheckBox = New System.Windows.Forms.CheckBox
        Me.statusComboBox = New System.Windows.Forms.ComboBox
        Me.SuspendLayout()
        '
        'statusLabel
        '
        Me.statusLabel.AutoSize = True
        Me.statusLabel.Location = New System.Drawing.Point(46, 69)
        Me.statusLabel.Name = "statusLabel"
        Me.statusLabel.Size = New System.Drawing.Size(59, 13)
        Me.statusLabel.TabIndex = 5
        Me.statusLabel.Text = "Set Status:"
        '
        'showIconCheckBox
        '
        Me.showIconCheckBox.AutoSize = True
        Me.showIconCheckBox.Location = New System.Drawing.Point(85, 26)
        Me.showIconCheckBox.Name = "showIconCheckBox"
        Me.showIconCheckBox.Size = New System.Drawing.Size(121, 17)
        Me.showIconCheckBox.TabIndex = 4
        Me.showIconCheckBox.Text = "Show Overlay Icons"
        Me.showIconCheckBox.UseVisualStyleBackColor = True
        '
        'statusComboBox
        '
        Me.statusComboBox.FormattingEnabled = True
        Me.statusComboBox.Items.AddRange(New Object() {"Available", "Away", "Appear Offline"})
        Me.statusComboBox.Location = New System.Drawing.Point(120, 66)
        Me.statusComboBox.Name = "statusComboBox"
        Me.statusComboBox.Size = New System.Drawing.Size(121, 21)
        Me.statusComboBox.TabIndex = 3
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(286, 112)
        Me.Controls.Add(Me.statusLabel)
        Me.Controls.Add(Me.showIconCheckBox)
        Me.Controls.Add(Me.statusComboBox)
        Me.Name = "MainForm"
        Me.Text = "Win7 Taskbar OverlayIcons"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents statusLabel As System.Windows.Forms.Label
    Private WithEvents showIconCheckBox As System.Windows.Forms.CheckBox
    Private WithEvents statusComboBox As System.Windows.Forms.ComboBox

End Class
