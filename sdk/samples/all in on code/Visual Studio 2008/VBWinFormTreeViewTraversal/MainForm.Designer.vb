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
        Dim TreeNode40 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Windows 2000")
        Dim TreeNode41 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Windows XP")
        Dim TreeNode42 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Windows 2003")
        Dim TreeNode43 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Windows Vista")
        Dim TreeNode44 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Windows 2008")
        Dim TreeNode45 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Windows 7")
        Dim TreeNode46 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Windows", New System.Windows.Forms.TreeNode() {TreeNode40, TreeNode41, TreeNode42, TreeNode43, TreeNode44, TreeNode45})
        Dim TreeNode47 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Office 2000")
        Dim TreeNode48 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Office XP")
        Dim TreeNode49 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Office 2003")
        Dim TreeNode50 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Office 2007")
        Dim TreeNode51 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Office", New System.Windows.Forms.TreeNode() {TreeNode47, TreeNode48, TreeNode49, TreeNode50})
        Dim TreeNode52 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("Product", New System.Windows.Forms.TreeNode() {TreeNode46, TreeNode51})
        Me.ckMatchWholeWord = New System.Windows.Forms.CheckBox
        Me.btnFindNext = New System.Windows.Forms.Button
        Me.textBox1 = New System.Windows.Forms.TextBox
        Me.btnClear = New System.Windows.Forms.Button
        Me.label1 = New System.Windows.Forms.Label
        Me.btnTravel = New System.Windows.Forms.Button
        Me.listBox1 = New System.Windows.Forms.ListBox
        Me.treeView1 = New System.Windows.Forms.TreeView
        Me.SuspendLayout()
        '
        'ckMatchWholeWord
        '
        Me.ckMatchWholeWord.AutoSize = True
        Me.ckMatchWholeWord.Location = New System.Drawing.Point(242, 405)
        Me.ckMatchWholeWord.Name = "ckMatchWholeWord"
        Me.ckMatchWholeWord.Size = New System.Drawing.Size(113, 17)
        Me.ckMatchWholeWord.TabIndex = 15
        Me.ckMatchWholeWord.Text = "MatchWholeWord"
        Me.ckMatchWholeWord.UseVisualStyleBackColor = True
        '
        'btnFindNext
        '
        Me.btnFindNext.Location = New System.Drawing.Point(358, 401)
        Me.btnFindNext.Name = "btnFindNext"
        Me.btnFindNext.Size = New System.Drawing.Size(75, 23)
        Me.btnFindNext.TabIndex = 14
        Me.btnFindNext.Text = "Find Next"
        Me.btnFindNext.UseVisualStyleBackColor = True
        '
        'textBox1
        '
        Me.textBox1.Location = New System.Drawing.Point(242, 364)
        Me.textBox1.Name = "textBox1"
        Me.textBox1.Size = New System.Drawing.Size(191, 20)
        Me.textBox1.TabIndex = 13
        '
        'btnClear
        '
        Me.btnClear.Location = New System.Drawing.Point(277, 334)
        Me.btnClear.Name = "btnClear"
        Me.btnClear.Size = New System.Drawing.Size(75, 23)
        Me.btnClear.TabIndex = 12
        Me.btnClear.Text = "Clear"
        Me.btnClear.UseVisualStyleBackColor = True
        '
        'label1
        '
        Me.label1.Font = New System.Drawing.Font("Microsoft Sans Serif", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(134, Byte))
        Me.label1.Location = New System.Drawing.Point(13, 12)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(420, 80)
        Me.label1.TabIndex = 11
        Me.label1.Text = resources.GetString("label1.Text")
        '
        'btnTravel
        '
        Me.btnTravel.Location = New System.Drawing.Point(358, 334)
        Me.btnTravel.Name = "btnTravel"
        Me.btnTravel.Size = New System.Drawing.Size(75, 23)
        Me.btnTravel.TabIndex = 10
        Me.btnTravel.Text = "Travel"
        Me.btnTravel.UseVisualStyleBackColor = True
        '
        'listBox1
        '
        Me.listBox1.FormattingEnabled = True
        Me.listBox1.Location = New System.Drawing.Point(242, 103)
        Me.listBox1.Name = "listBox1"
        Me.listBox1.Size = New System.Drawing.Size(191, 225)
        Me.listBox1.TabIndex = 9
        '
        'treeView1
        '
        Me.treeView1.Location = New System.Drawing.Point(13, 103)
        Me.treeView1.Name = "treeView1"
        TreeNode40.Name = "Node2"
        TreeNode40.Text = "Windows 2000"
        TreeNode41.Name = "Node4"
        TreeNode41.Text = "Windows XP"
        TreeNode42.Name = "Node5"
        TreeNode42.Text = "Windows 2003"
        TreeNode43.Name = "Node6"
        TreeNode43.Text = "Windows Vista"
        TreeNode44.Name = "Node7"
        TreeNode44.Text = "Windows 2008"
        TreeNode45.Name = "Node8"
        TreeNode45.Text = "Windows 7"
        TreeNode46.Name = "Node1"
        TreeNode46.Text = "Windows"
        TreeNode47.Name = "Node10"
        TreeNode47.Text = "Office 2000"
        TreeNode48.Name = "Node11"
        TreeNode48.Text = "Office XP"
        TreeNode49.Name = "Node12"
        TreeNode49.Text = "Office 2003"
        TreeNode50.Name = "Node13"
        TreeNode50.Text = "Office 2007"
        TreeNode51.Name = "Node9"
        TreeNode51.Text = "Office"
        TreeNode52.Name = "Node0"
        TreeNode52.Text = "Product"
        Me.treeView1.Nodes.AddRange(New System.Windows.Forms.TreeNode() {TreeNode52})
        Me.treeView1.Size = New System.Drawing.Size(223, 321)
        Me.treeView1.TabIndex = 8
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(446, 437)
        Me.Controls.Add(Me.ckMatchWholeWord)
        Me.Controls.Add(Me.btnFindNext)
        Me.Controls.Add(Me.textBox1)
        Me.Controls.Add(Me.btnClear)
        Me.Controls.Add(Me.label1)
        Me.Controls.Add(Me.btnTravel)
        Me.Controls.Add(Me.listBox1)
        Me.Controls.Add(Me.treeView1)
        Me.Name = "MainForm"
        Me.Text = "Form1"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents ckMatchWholeWord As System.Windows.Forms.CheckBox
    Private WithEvents btnFindNext As System.Windows.Forms.Button
    Private WithEvents textBox1 As System.Windows.Forms.TextBox
    Private WithEvents btnClear As System.Windows.Forms.Button
    Private WithEvents label1 As System.Windows.Forms.Label
    Private WithEvents btnTravel As System.Windows.Forms.Button
    Private WithEvents listBox1 As System.Windows.Forms.ListBox
    Private WithEvents treeView1 As System.Windows.Forms.TreeView

End Class
