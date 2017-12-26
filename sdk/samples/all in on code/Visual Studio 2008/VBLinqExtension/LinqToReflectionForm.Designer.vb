<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class LinqToReflectionForm
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
        Me.btnGroup = New System.Windows.Forms.Button
        Me.btnBrowse = New System.Windows.Forms.Button
        Me.txtBoxPath = New System.Windows.Forms.TextBox
        Me.groupBox2 = New System.Windows.Forms.GroupBox
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.treeView1 = New System.Windows.Forms.TreeView
        Me.openFileDialog1 = New System.Windows.Forms.OpenFileDialog
        Me.groupBox2.SuspendLayout()
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnGroup
        '
        Me.btnGroup.Location = New System.Drawing.Point(6, 118)
        Me.btnGroup.Name = "btnGroup"
        Me.btnGroup.Size = New System.Drawing.Size(136, 30)
        Me.btnGroup.TabIndex = 0
        Me.btnGroup.Text = "Group By Return Type"
        Me.btnGroup.UseVisualStyleBackColor = True
        '
        'btnBrowse
        '
        Me.btnBrowse.Location = New System.Drawing.Point(384, 24)
        Me.btnBrowse.Name = "btnBrowse"
        Me.btnBrowse.Size = New System.Drawing.Size(75, 23)
        Me.btnBrowse.TabIndex = 1
        Me.btnBrowse.Text = "Browse..."
        Me.btnBrowse.UseVisualStyleBackColor = True
        '
        'txtBoxPath
        '
        Me.txtBoxPath.Location = New System.Drawing.Point(22, 26)
        Me.txtBoxPath.Name = "txtBoxPath"
        Me.txtBoxPath.Size = New System.Drawing.Size(338, 21)
        Me.txtBoxPath.TabIndex = 0
        '
        'groupBox2
        '
        Me.groupBox2.Controls.Add(Me.btnBrowse)
        Me.groupBox2.Controls.Add(Me.txtBoxPath)
        Me.groupBox2.Location = New System.Drawing.Point(12, 13)
        Me.groupBox2.Name = "groupBox2"
        Me.groupBox2.Size = New System.Drawing.Size(470, 67)
        Me.groupBox2.TabIndex = 5
        Me.groupBox2.TabStop = False
        Me.groupBox2.Text = "Open Assembly"
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.btnGroup)
        Me.groupBox1.Controls.Add(Me.treeView1)
        Me.groupBox1.Location = New System.Drawing.Point(12, 104)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(470, 351)
        Me.groupBox1.TabIndex = 4
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Assembly Information"
        '
        'treeView1
        '
        Me.treeView1.Location = New System.Drawing.Point(163, 20)
        Me.treeView1.Name = "treeView1"
        Me.treeView1.Size = New System.Drawing.Size(288, 308)
        Me.treeView1.TabIndex = 1
        '
        'openFileDialog1
        '
        Me.openFileDialog1.FileName = "openFileDialog1"
        Me.openFileDialog1.Filter = "Assembly files|*.dll;*.exe"
        '
        'LinqToReflectionForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(494, 476)
        Me.Controls.Add(Me.groupBox2)
        Me.Controls.Add(Me.groupBox1)
        Me.Name = "LinqToReflectionForm"
        Me.Text = "LINQ To Reflection"
        Me.groupBox2.ResumeLayout(False)
        Me.groupBox2.PerformLayout()
        Me.groupBox1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents btnGroup As System.Windows.Forms.Button
    Private WithEvents btnBrowse As System.Windows.Forms.Button
    Private WithEvents txtBoxPath As System.Windows.Forms.TextBox
    Private WithEvents groupBox2 As System.Windows.Forms.GroupBox
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents treeView1 As System.Windows.Forms.TreeView
    Private WithEvents openFileDialog1 As System.Windows.Forms.OpenFileDialog
End Class
