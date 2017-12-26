Namespace SmartTags
    <Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
    Partial Class UC_SmartTagSupport
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
            Me.label1 = New System.Windows.Forms.Label
            Me.SuspendLayout()
            '
            'label1
            '
            Me.label1.AutoSize = True
            Me.label1.Location = New System.Drawing.Point(70, 55)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(35, 13)
            Me.label1.TabIndex = 1
            Me.label1.Text = "label1"
            '
            'UC_SmartTagSupport
            '
            Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.BackColor = System.Drawing.Color.FromArgb(CType(CType(255, Byte), Integer), CType(CType(255, Byte), Integer), CType(CType(192, Byte), Integer))
            Me.Controls.Add(Me.label1)
            Me.ForeColor = System.Drawing.SystemColors.ActiveCaption
            Me.Name = "UC_SmartTagSupport"
            Me.Size = New System.Drawing.Size(174, 123)
            Me.ResumeLayout(False)
            Me.PerformLayout()

        End Sub
        Private WithEvents label1 As System.Windows.Forms.Label

    End Class
End Namespace