<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class EnableDesignTimeFuncForChildCtrl
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(EnableDesignTimeFuncForChildCtrl))
        Me.label1 = New System.Windows.Forms.Label
        Me.UC_EnableDesignTimeFuncForChildCtrl1 = New VBWinFormDesigner.UC_EnableDesignTimeFuncForChildCtrl
        Me.label2 = New System.Windows.Forms.Label
        Me.SuspendLayout()
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Font = New System.Drawing.Font("Verdana", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.label1.Location = New System.Drawing.Point(12, 9)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(632, 252)
        Me.label1.TabIndex = 3
        Me.label1.Text = resources.GetString("label1.Text")
        '
        'UC_EnableDesignTimeFuncForChildCtrl1
        '
        Me.UC_EnableDesignTimeFuncForChildCtrl1.BackColor = System.Drawing.Color.White
        Me.UC_EnableDesignTimeFuncForChildCtrl1.Location = New System.Drawing.Point(15, 276)
        Me.UC_EnableDesignTimeFuncForChildCtrl1.Name = "UC_EnableDesignTimeFuncForChildCtrl1"
        '
        '
        '
        Me.UC_EnableDesignTimeFuncForChildCtrl1.Panel1.BackColor = System.Drawing.SystemColors.Control
        Me.UC_EnableDesignTimeFuncForChildCtrl1.Panel1.Location = New System.Drawing.Point(17, 34)
        Me.UC_EnableDesignTimeFuncForChildCtrl1.Panel1.Name = "panel1"
        Me.UC_EnableDesignTimeFuncForChildCtrl1.Panel1.Size = New System.Drawing.Size(295, 155)
        Me.UC_EnableDesignTimeFuncForChildCtrl1.Panel1.TabIndex = 1
        Me.UC_EnableDesignTimeFuncForChildCtrl1.Size = New System.Drawing.Size(328, 223)
        Me.UC_EnableDesignTimeFuncForChildCtrl1.TabIndex = 4
        '
        'label2
        '
        Me.label2.AutoSize = True
        Me.label2.Font = New System.Drawing.Font("Verdana", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.label2.Location = New System.Drawing.Point(48, 347)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(244, 16)
        Me.label2.TabIndex = 5
        Me.label2.Text = "Edit me! Drag some controls on me!"
        '
        'EnableDesignTimeFuncForChildCtrl
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(679, 564)
        Me.Controls.Add(Me.label2)
        Me.Controls.Add(Me.UC_EnableDesignTimeFuncForChildCtrl1)
        Me.Controls.Add(Me.label1)
        Me.Name = "EnableDesignTimeFuncForChildCtrl"
        Me.Text = "EnableDesignTimeFuncForChildCtrl"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents label1 As System.Windows.Forms.Label
    Friend WithEvents UC_EnableDesignTimeFuncForChildCtrl1 As VBWinFormDesigner.UC_EnableDesignTimeFuncForChildCtrl
    Private WithEvents label2 As System.Windows.Forms.Label
End Class
