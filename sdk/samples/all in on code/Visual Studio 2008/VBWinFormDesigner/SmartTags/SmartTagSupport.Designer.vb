<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class SmartTagSupport
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
        Me.label1 = New System.Windows.Forms.Label
        Me.UC_SmartTagSupport1 = New VBWinFormDesigner.SmartTags.UC_SmartTagSupport
        Me.SuspendLayout()
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Font = New System.Drawing.Font("Verdana", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.label1.Location = New System.Drawing.Point(34, 20)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(488, 42)
        Me.label1.TabIndex = 2
        Me.label1.Text = "This sample demonstrates how to add smart tag to a control at design time." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "Sel" & _
            "ect the following control and click the smart tag at the right-top corner."
        '
        'UC_SmartTagSupport1
        '
        Me.UC_SmartTagSupport1.BackColor = System.Drawing.Color.FromArgb(CType(CType(255, Byte), Integer), CType(CType(255, Byte), Integer), CType(CType(192, Byte), Integer))
        Me.UC_SmartTagSupport1.ForeColor = System.Drawing.SystemColors.ActiveCaption
        Me.UC_SmartTagSupport1.Location = New System.Drawing.Point(37, 106)
        Me.UC_SmartTagSupport1.Name = "UC_SmartTagSupport1"
        Me.UC_SmartTagSupport1.Size = New System.Drawing.Size(174, 123)
        Me.UC_SmartTagSupport1.TabIndex = 3
        '
        'SmartTagSupport
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(574, 346)
        Me.Controls.Add(Me.UC_SmartTagSupport1)
        Me.Controls.Add(Me.label1)
        Me.Name = "SmartTagSupport"
        Me.Text = "SmartTagSupport"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents label1 As System.Windows.Forms.Label
    Friend WithEvents UC_SmartTagSupport1 As VBWinFormDesigner.SmartTags.UC_SmartTagSupport
End Class
