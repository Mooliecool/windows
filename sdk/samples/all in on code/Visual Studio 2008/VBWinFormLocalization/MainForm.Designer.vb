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
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.label2 = New System.Windows.Forms.Label
        Me.button2 = New System.Windows.Forms.Button
        Me.button1 = New System.Windows.Forms.Button
        Me.label1 = New System.Windows.Forms.Label
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'groupBox1
        '
        Me.groupBox1.AccessibleDescription = Nothing
        Me.groupBox1.AccessibleName = Nothing
        resources.ApplyResources(Me.groupBox1, "groupBox1")
        Me.groupBox1.BackgroundImage = Nothing
        Me.groupBox1.Controls.Add(Me.label2)
        Me.groupBox1.Controls.Add(Me.button2)
        Me.groupBox1.Controls.Add(Me.button1)
        Me.groupBox1.Controls.Add(Me.label1)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.TabStop = False
        '
        'label2
        '
        Me.label2.AccessibleDescription = Nothing
        Me.label2.AccessibleName = Nothing
        resources.ApplyResources(Me.label2, "label2")
        Me.label2.Font = Nothing
        Me.label2.Name = "label2"
        '
        'button2
        '
        Me.button2.AccessibleDescription = Nothing
        Me.button2.AccessibleName = Nothing
        resources.ApplyResources(Me.button2, "button2")
        Me.button2.BackgroundImage = Nothing
        Me.button2.Font = Nothing
        Me.button2.Name = "button2"
        Me.button2.UseVisualStyleBackColor = True
        '
        'button1
        '
        Me.button1.AccessibleDescription = Nothing
        Me.button1.AccessibleName = Nothing
        resources.ApplyResources(Me.button1, "button1")
        Me.button1.BackgroundImage = Nothing
        Me.button1.Font = Nothing
        Me.button1.Name = "button1"
        Me.button1.UseVisualStyleBackColor = True
        '
        'label1
        '
        Me.label1.AccessibleDescription = Nothing
        Me.label1.AccessibleName = Nothing
        resources.ApplyResources(Me.label1, "label1")
        Me.label1.Name = "label1"
        '
        'MainForm
        '
        Me.AccessibleDescription = Nothing
        Me.AccessibleName = Nothing
        resources.ApplyResources(Me, "$this")
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BackgroundImage = Nothing
        Me.Controls.Add(Me.groupBox1)
        Me.Font = Nothing
        Me.Icon = Nothing
        Me.Name = "MainForm"
        Me.groupBox1.ResumeLayout(False)
        Me.groupBox1.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents label2 As System.Windows.Forms.Label
    Private WithEvents button2 As System.Windows.Forms.Button
    Private WithEvents button1 As System.Windows.Forms.Button
    Private WithEvents label1 As System.Windows.Forms.Label

End Class
