<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class PaneControl
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
        Me.btnCCXMLMapping = New System.Windows.Forms.Button
        Me.btnInsertBuiltIn = New System.Windows.Forms.Button
        Me.btnInsertCC = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'btnCCXMLMapping
        '
        Me.btnCCXMLMapping.Location = New System.Drawing.Point(0, 59)
        Me.btnCCXMLMapping.Name = "btnCCXMLMapping"
        Me.btnCCXMLMapping.Size = New System.Drawing.Size(147, 40)
        Me.btnCCXMLMapping.TabIndex = 5
        Me.btnCCXMLMapping.Text = "Content Control XMLMapping "
        Me.btnCCXMLMapping.UseVisualStyleBackColor = True
        '
        'btnInsertBuiltIn
        '
        Me.btnInsertBuiltIn.Location = New System.Drawing.Point(0, 30)
        Me.btnInsertBuiltIn.Name = "btnInsertBuiltIn"
        Me.btnInsertBuiltIn.Size = New System.Drawing.Size(147, 23)
        Me.btnInsertBuiltIn.TabIndex = 4
        Me.btnInsertBuiltIn.Text = "Insert Building Blocks"
        Me.btnInsertBuiltIn.UseVisualStyleBackColor = True
        '
        'btnInsertCC
        '
        Me.btnInsertCC.Location = New System.Drawing.Point(0, 1)
        Me.btnInsertCC.Name = "btnInsertCC"
        Me.btnInsertCC.Size = New System.Drawing.Size(147, 23)
        Me.btnInsertCC.TabIndex = 3
        Me.btnInsertCC.Text = "Insert ContentControls"
        Me.btnInsertCC.UseVisualStyleBackColor = True
        '
        'PaneControl
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.btnCCXMLMapping)
        Me.Controls.Add(Me.btnInsertBuiltIn)
        Me.Controls.Add(Me.btnInsertCC)
        Me.Name = "PaneControl"
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents btnCCXMLMapping As System.Windows.Forms.Button
    Private WithEvents btnInsertBuiltIn As System.Windows.Forms.Button
    Private WithEvents btnInsertCC As System.Windows.Forms.Button

End Class
