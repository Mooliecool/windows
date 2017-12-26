<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class VBActiveXCtrl
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
        Me.label2 = New System.Windows.Forms.Label
        Me.lbFloatProperty = New System.Windows.Forms.Label
        Me.tbMessage = New System.Windows.Forms.TextBox
        Me.bnMessage = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(8, 9)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(73, 13)
        Me.label1.TabIndex = 1
        Me.label1.Text = "VBActiveXCtrl"
        '
        'label2
        '
        Me.label2.AutoSize = True
        Me.label2.Location = New System.Drawing.Point(8, 35)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(72, 13)
        Me.label2.TabIndex = 2
        Me.label2.Text = "FloatProperty:"
        '
        'lbFloatProperty
        '
        Me.lbFloatProperty.AutoSize = True
        Me.lbFloatProperty.Location = New System.Drawing.Point(86, 35)
        Me.lbFloatProperty.Name = "lbFloatProperty"
        Me.lbFloatProperty.Size = New System.Drawing.Size(13, 13)
        Me.lbFloatProperty.TabIndex = 3
        Me.lbFloatProperty.Text = "0"
        '
        'tbMessage
        '
        Me.tbMessage.Location = New System.Drawing.Point(11, 61)
        Me.tbMessage.Name = "tbMessage"
        Me.tbMessage.Size = New System.Drawing.Size(153, 20)
        Me.tbMessage.TabIndex = 4
        '
        'bnMessage
        '
        Me.bnMessage.Location = New System.Drawing.Point(170, 59)
        Me.bnMessage.Name = "bnMessage"
        Me.bnMessage.Size = New System.Drawing.Size(75, 23)
        Me.bnMessage.TabIndex = 5
        Me.bnMessage.Text = "MSGBOX"
        Me.bnMessage.UseVisualStyleBackColor = True
        '
        'VBActiveXCtrl
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.bnMessage)
        Me.Controls.Add(Me.tbMessage)
        Me.Controls.Add(Me.lbFloatProperty)
        Me.Controls.Add(Me.label2)
        Me.Controls.Add(Me.label1)
        Me.Name = "VBActiveXCtrl"
        Me.Size = New System.Drawing.Size(256, 95)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents label1 As System.Windows.Forms.Label
    Private WithEvents label2 As System.Windows.Forms.Label
    Private WithEvents lbFloatProperty As System.Windows.Forms.Label
    Private WithEvents tbMessage As System.Windows.Forms.TextBox
    Private WithEvents bnMessage As System.Windows.Forms.Button

End Class
