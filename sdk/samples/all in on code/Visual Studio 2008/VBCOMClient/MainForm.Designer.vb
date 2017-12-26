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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(MainForm))
        Me.AxMFCActiveX1 = New AxMFCActiveXLib.AxMFCActiveX
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.BnGetFloatProperty = New System.Windows.Forms.Button
        Me.BnSetFloatProperty = New System.Windows.Forms.Button
        Me.TbFloatProperty = New System.Windows.Forms.TextBox
        Me.Label1 = New System.Windows.Forms.Label
        CType(Me.AxMFCActiveX1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'AxMFCActiveX1
        '
        Me.AxMFCActiveX1.Enabled = True
        Me.AxMFCActiveX1.Location = New System.Drawing.Point(0, 12)
        Me.AxMFCActiveX1.Name = "AxMFCActiveX1"
        Me.AxMFCActiveX1.OcxState = CType(resources.GetObject("AxMFCActiveX1.OcxState"), System.Windows.Forms.AxHost.State)
        Me.AxMFCActiveX1.Size = New System.Drawing.Size(272, 95)
        Me.AxMFCActiveX1.TabIndex = 0
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.BnGetFloatProperty)
        Me.GroupBox1.Controls.Add(Me.BnSetFloatProperty)
        Me.GroupBox1.Controls.Add(Me.TbFloatProperty)
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Location = New System.Drawing.Point(12, 113)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(250, 64)
        Me.GroupBox1.TabIndex = 1
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Operation"
        '
        'BnGetFloatProperty
        '
        Me.BnGetFloatProperty.Location = New System.Drawing.Point(211, 28)
        Me.BnGetFloatProperty.Name = "BnGetFloatProperty"
        Me.BnGetFloatProperty.Size = New System.Drawing.Size(33, 23)
        Me.BnGetFloatProperty.TabIndex = 3
        Me.BnGetFloatProperty.Text = "Get"
        Me.BnGetFloatProperty.UseVisualStyleBackColor = True
        '
        'BnSetFloatProperty
        '
        Me.BnSetFloatProperty.Location = New System.Drawing.Point(179, 28)
        Me.BnSetFloatProperty.Name = "BnSetFloatProperty"
        Me.BnSetFloatProperty.Size = New System.Drawing.Size(32, 23)
        Me.BnSetFloatProperty.TabIndex = 2
        Me.BnSetFloatProperty.Text = "Set"
        Me.BnSetFloatProperty.UseVisualStyleBackColor = True
        '
        'TbFloatProperty
        '
        Me.TbFloatProperty.Location = New System.Drawing.Point(85, 30)
        Me.TbFloatProperty.MaxLength = 10
        Me.TbFloatProperty.Name = "TbFloatProperty"
        Me.TbFloatProperty.Size = New System.Drawing.Size(91, 20)
        Me.TbFloatProperty.TabIndex = 1
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(7, 33)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(72, 13)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "FloatProperty:"
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(275, 197)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.AxMFCActiveX1)
        Me.Name = "MainForm"
        Me.Text = "VBCOMClient"
        CType(Me.AxMFCActiveX1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents AxMFCActiveX1 As AxMFCActiveXLib.AxMFCActiveX
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents TbFloatProperty As System.Windows.Forms.TextBox
    Friend WithEvents BnSetFloatProperty As System.Windows.Forms.Button
    Friend WithEvents BnGetFloatProperty As System.Windows.Forms.Button
End Class
