<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class TestForm
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
        Me.btnGetPageTitle = New System.Windows.Forms.Button()
        Me.SuspendLayout()
        '
        'btnGetPageTitle
        '
        Me.btnGetPageTitle.Location = New System.Drawing.Point(66, 96)
        Me.btnGetPageTitle.Name = "btnGetPageTitle"
        Me.btnGetPageTitle.Size = New System.Drawing.Size(142, 23)
        Me.btnGetPageTitle.TabIndex = 0
        Me.btnGetPageTitle.Text = "GetPageTitle"
        Me.btnGetPageTitle.UseVisualStyleBackColor = True
        '
        'TestForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(284, 262)
        Me.Controls.Add(Me.btnGetPageTitle)
        Me.Name = "TestForm"
        Me.Text = "TestForm"
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents btnGetPageTitle As System.Windows.Forms.Button
End Class
