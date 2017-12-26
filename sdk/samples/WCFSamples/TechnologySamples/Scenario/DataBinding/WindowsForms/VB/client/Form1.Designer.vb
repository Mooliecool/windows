' Copyright (c) Microsoft Corporation. All rights reserved.

Namespace Microsoft.ServiceModel.Samples

    <Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
    Partial Class Form1
        Inherits System.Windows.Forms.Form

        'Form overrides dispose to clean up the component list.
        <System.Diagnostics.DebuggerNonUserCode()> _
        Protected Overrides Sub Dispose(ByVal disposing As Boolean)
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
            MyBase.Dispose(disposing)
        End Sub

        'Required by the Windows Form Designer
        Private components As System.ComponentModel.IContainer

        'NOTE: The following procedure is required by the Windows Form Designer
        'It can be modified using the Windows Form Designer.  
        'Do not modify it using the code editor.
        <System.Diagnostics.DebuggerStepThrough()> _
        Private Sub InitializeComponent()
            Me.dataGridView1 = New System.Windows.Forms.DataGridView
            Me.button1 = New System.Windows.Forms.Button
            CType(Me.dataGridView1, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.SuspendLayout()
            '
            'dataGridView1
            '
            Me.dataGridView1.Location = New System.Drawing.Point(25, 30)
            Me.dataGridView1.Name = "dataGridView1"
            Me.dataGridView1.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.None
            Me.dataGridView1.Size = New System.Drawing.Size(339, 194)
            Me.dataGridView1.TabIndex = 2
            Me.dataGridView1.Text = "dataGridView1"
            '
            'button1
            '
            Me.button1.Location = New System.Drawing.Point(114, 240)
            Me.button1.Name = "button1"
            Me.button1.Size = New System.Drawing.Size(152, 23)
            Me.button1.TabIndex = 3
            Me.button1.Text = "Retrieve Weather Data"
            '
            'Form1
            '
            Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.ClientSize = New System.Drawing.Size(392, 277)
            Me.Controls.Add(Me.button1)
            Me.Controls.Add(Me.dataGridView1)
            Me.Name = "Form1"
            Me.Text = "Form1"
            CType(Me.dataGridView1, System.ComponentModel.ISupportInitialize).EndInit()
            Me.ResumeLayout(False)

        End Sub
        Private WithEvents dataGridView1 As System.Windows.Forms.DataGridView
        Private WithEvents button1 As System.Windows.Forms.Button
    End Class

End Namespace