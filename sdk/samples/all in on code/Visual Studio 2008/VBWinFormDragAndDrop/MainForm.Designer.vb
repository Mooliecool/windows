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
        Me.treeView1 = New System.Windows.Forms.TreeView
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.listBox1 = New System.Windows.Forms.ListBox
        Me.label1 = New System.Windows.Forms.Label
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'treeView1
        '
        Me.treeView1.Location = New System.Drawing.Point(20, 112)
        Me.treeView1.Name = "treeView1"
        Me.treeView1.Size = New System.Drawing.Size(303, 287)
        Me.treeView1.TabIndex = 2
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.treeView1)
        Me.groupBox1.Controls.Add(Me.listBox1)
        Me.groupBox1.Controls.Add(Me.label1)
        Me.groupBox1.Font = New System.Drawing.Font("Verdana", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.groupBox1.Location = New System.Drawing.Point(25, 24)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(700, 429)
        Me.groupBox1.TabIndex = 9
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Drag and Drop"
        '
        'listBox1
        '
        Me.listBox1.FormattingEnabled = True
        Me.listBox1.ItemHeight = 16
        Me.listBox1.Location = New System.Drawing.Point(359, 107)
        Me.listBox1.Name = "listBox1"
        Me.listBox1.Size = New System.Drawing.Size(320, 292)
        Me.listBox1.TabIndex = 1
        '
        'label1
        '
        Me.label1.Font = New System.Drawing.Font("Verdana", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.label1.Location = New System.Drawing.Point(17, 19)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(676, 85)
        Me.label1.TabIndex = 0
        Me.label1.Text = "" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "This example demonstrates how to perform drag-and-drop operaions in Windows For" & _
            "ms application." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "To see the result, drag an item from the ListBox and drop it " & _
            "to the TreeView."
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(751, 476)
        Me.Controls.Add(Me.groupBox1)
        Me.Name = "MainForm"
        Me.Text = "Form1"
        Me.groupBox1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents treeView1 As System.Windows.Forms.TreeView
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents listBox1 As System.Windows.Forms.ListBox
    Private WithEvents label1 As System.Windows.Forms.Label

End Class
