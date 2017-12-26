<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class UC_EnableDesignTimeFuncForChildCtrl
    Inherits System.Windows.Forms.UserControl

    'UserControl overrides dispose to clean up the component list.
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
        Me._panel1 = New System.Windows.Forms.Panel
        Me.SuspendLayout()
        '
        '_panel1
        '
        Me._panel1.BackColor = System.Drawing.SystemColors.Control
        Me._panel1.Location = New System.Drawing.Point(17, 34)
        Me._panel1.Name = "panel1"
        Me._panel1.Size = New System.Drawing.Size(295, 155)
        Me._panel1.TabIndex = 1
        '
        'UC_EnableDesignTimeFuncForChildCtrl
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BackColor = System.Drawing.Color.White
        Me.Controls.Add(Me._panel1)
        Me.Name = "UC_EnableDesignTimeFuncForChildCtrl"
        Me.Size = New System.Drawing.Size(328, 223)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents _panel1 As System.Windows.Forms.Panel

End Class
