<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class LinqToFileSystemForm
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
        Me.txtBoxPath = New System.Windows.Forms.TextBox
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.btnBrowse = New System.Windows.Forms.Button
        Me.btnMax = New System.Windows.Forms.Button
        Me.treeView1 = New System.Windows.Forms.TreeView
        Me.groupBox2 = New System.Windows.Forms.GroupBox
        Me.groupBox1.SuspendLayout()
        Me.groupBox2.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnGroup
        '
        Me.btnGroup.Location = New System.Drawing.Point(18, 100)
        Me.btnGroup.Name = "btnGroup"
        Me.btnGroup.Size = New System.Drawing.Size(170, 33)
        Me.btnGroup.TabIndex = 0
        Me.btnGroup.Text = "Group by Extension name"
        Me.btnGroup.UseVisualStyleBackColor = True
        '
        'txtBoxPath
        '
        Me.txtBoxPath.Location = New System.Drawing.Point(18, 22)
        Me.txtBoxPath.Name = "txtBoxPath"
        Me.txtBoxPath.Size = New System.Drawing.Size(338, 20)
        Me.txtBoxPath.TabIndex = 4
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.txtBoxPath)
        Me.groupBox1.Controls.Add(Me.btnBrowse)
        Me.groupBox1.Location = New System.Drawing.Point(30, 13)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(463, 64)
        Me.groupBox1.TabIndex = 6
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Folder Information"
        '
        'btnBrowse
        '
        Me.btnBrowse.Location = New System.Drawing.Point(374, 20)
        Me.btnBrowse.Name = "btnBrowse"
        Me.btnBrowse.Size = New System.Drawing.Size(75, 25)
        Me.btnBrowse.TabIndex = 3
        Me.btnBrowse.Text = "Browse..."
        Me.btnBrowse.UseVisualStyleBackColor = True
        '
        'btnMax
        '
        Me.btnMax.Location = New System.Drawing.Point(18, 184)
        Me.btnMax.Name = "btnMax"
        Me.btnMax.Size = New System.Drawing.Size(170, 33)
        Me.btnMax.TabIndex = 2
        Me.btnMax.Text = "Show Largest File"
        Me.btnMax.UseVisualStyleBackColor = True
        '
        'treeView1
        '
        Me.treeView1.Location = New System.Drawing.Point(208, 22)
        Me.treeView1.Name = "treeView1"
        Me.treeView1.Size = New System.Drawing.Size(241, 297)
        Me.treeView1.TabIndex = 1
        '
        'groupBox2
        '
        Me.groupBox2.Controls.Add(Me.btnGroup)
        Me.groupBox2.Controls.Add(Me.btnMax)
        Me.groupBox2.Controls.Add(Me.treeView1)
        Me.groupBox2.Location = New System.Drawing.Point(30, 94)
        Me.groupBox2.Name = "groupBox2"
        Me.groupBox2.Size = New System.Drawing.Size(463, 342)
        Me.groupBox2.TabIndex = 7
        Me.groupBox2.TabStop = False
        Me.groupBox2.Text = "File System Information"
        '
        'LinqToFileSystemForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(528, 479)
        Me.Controls.Add(Me.groupBox1)
        Me.Controls.Add(Me.groupBox2)
        Me.Name = "LinqToFileSystemForm"
        Me.Text = "LINQ To File System"
        Me.groupBox1.ResumeLayout(False)
        Me.groupBox1.PerformLayout()
        Me.groupBox2.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents btnGroup As System.Windows.Forms.Button
    Private WithEvents txtBoxPath As System.Windows.Forms.TextBox
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents btnBrowse As System.Windows.Forms.Button
    Private WithEvents btnMax As System.Windows.Forms.Button
    Private WithEvents treeView1 As System.Windows.Forms.TreeView
    Private WithEvents groupBox2 As System.Windows.Forms.GroupBox
End Class
