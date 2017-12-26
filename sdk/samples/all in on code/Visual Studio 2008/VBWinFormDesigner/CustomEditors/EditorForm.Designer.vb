Namespace CustomEditors
    <Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
    Partial Class EditorForm
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
            Me.button2 = New System.Windows.Forms.Button
            Me.button1 = New System.Windows.Forms.Button
            Me.groupBox1 = New System.Windows.Forms.GroupBox
            Me.monthCalendar1 = New System.Windows.Forms.MonthCalendar
            Me.label2 = New System.Windows.Forms.Label
            Me.textBox1 = New System.Windows.Forms.TextBox
            Me.label1 = New System.Windows.Forms.Label
            Me.groupBox1.SuspendLayout()
            Me.SuspendLayout()
            '
            'button2
            '
            Me.button2.Location = New System.Drawing.Point(260, 276)
            Me.button2.Name = "button2"
            Me.button2.Size = New System.Drawing.Size(75, 23)
            Me.button2.TabIndex = 5
            Me.button2.Text = "Cancel"
            Me.button2.UseVisualStyleBackColor = True
            '
            'button1
            '
            Me.button1.Location = New System.Drawing.Point(115, 276)
            Me.button1.Name = "button1"
            Me.button1.Size = New System.Drawing.Size(75, 23)
            Me.button1.TabIndex = 4
            Me.button1.Text = "OK"
            Me.button1.UseVisualStyleBackColor = True
            '
            'groupBox1
            '
            Me.groupBox1.Controls.Add(Me.button2)
            Me.groupBox1.Controls.Add(Me.button1)
            Me.groupBox1.Controls.Add(Me.monthCalendar1)
            Me.groupBox1.Controls.Add(Me.label2)
            Me.groupBox1.Controls.Add(Me.textBox1)
            Me.groupBox1.Controls.Add(Me.label1)
            Me.groupBox1.Font = New System.Drawing.Font("Verdana", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.groupBox1.Location = New System.Drawing.Point(24, 37)
            Me.groupBox1.Name = "groupBox1"
            Me.groupBox1.Size = New System.Drawing.Size(475, 305)
            Me.groupBox1.TabIndex = 2
            Me.groupBox1.TabStop = False
            Me.groupBox1.Text = "Custom Editor"
            '
            'monthCalendar1
            '
            Me.monthCalendar1.Location = New System.Drawing.Point(115, 81)
            Me.monthCalendar1.Name = "monthCalendar1"
            Me.monthCalendar1.TabIndex = 3
            '
            'label2
            '
            Me.label2.AutoSize = True
            Me.label2.Location = New System.Drawing.Point(34, 81)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(45, 16)
            Me.label2.TabIndex = 2
            Me.label2.Text = "Date:"
            '
            'textBox1
            '
            Me.textBox1.Location = New System.Drawing.Point(115, 33)
            Me.textBox1.Name = "textBox1"
            Me.textBox1.Size = New System.Drawing.Size(220, 23)
            Me.textBox1.TabIndex = 1
            '
            'label1
            '
            Me.label1.AutoSize = True
            Me.label1.Location = New System.Drawing.Point(34, 36)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(50, 16)
            Me.label1.TabIndex = 0
            Me.label1.Text = "Name:"
            '
            'EditorForm
            '
            Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.ClientSize = New System.Drawing.Size(520, 364)
            Me.Controls.Add(Me.groupBox1)
            Me.Name = "EditorForm"
            Me.Text = "EditorForm"
            Me.groupBox1.ResumeLayout(False)
            Me.groupBox1.PerformLayout()
            Me.ResumeLayout(False)

        End Sub
        Private WithEvents button2 As System.Windows.Forms.Button
        Private WithEvents button1 As System.Windows.Forms.Button
        Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
        Private WithEvents monthCalendar1 As System.Windows.Forms.MonthCalendar
        Private WithEvents label2 As System.Windows.Forms.Label
        Private WithEvents textBox1 As System.Windows.Forms.TextBox
        Private WithEvents label1 As System.Windows.Forms.Label
    End Class
End Namespace
