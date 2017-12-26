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
        Me.groupBox2 = New System.Windows.Forms.GroupBox
        Me.groupBox7 = New System.Windows.Forms.GroupBox
        Me.dgvDetail = New System.Windows.Forms.DataGridView
        Me.dgvMaster = New System.Windows.Forms.DataGridView
        Me.groupBox6 = New System.Windows.Forms.GroupBox
        Me.dataGridView2 = New System.Windows.Forms.DataGridView
        Me.groupBox5 = New System.Windows.Forms.GroupBox
        Me.dataGridView1 = New System.Windows.Forms.DataGridView
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.groupBox4 = New System.Windows.Forms.GroupBox
        Me.btnNext = New System.Windows.Forms.Button
        Me.btnPrev = New System.Windows.Forms.Button
        Me.label3 = New System.Windows.Forms.Label
        Me.label2 = New System.Windows.Forms.Label
        Me.textBox4 = New System.Windows.Forms.TextBox
        Me.textBox3 = New System.Windows.Forms.TextBox
        Me.groupBox3 = New System.Windows.Forms.GroupBox
        Me.btnSet = New System.Windows.Forms.Button
        Me.label1 = New System.Windows.Forms.Label
        Me.textBox2 = New System.Windows.Forms.TextBox
        Me.textBox1 = New System.Windows.Forms.TextBox
        Me.checkBox1 = New System.Windows.Forms.CheckBox
        Me.flowLayoutPanel1 = New System.Windows.Forms.FlowLayoutPanel
        Me.groupBox2.SuspendLayout()
        Me.groupBox7.SuspendLayout()
        CType(Me.dgvDetail, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvMaster, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.groupBox6.SuspendLayout()
        CType(Me.dataGridView2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.groupBox5.SuspendLayout()
        CType(Me.dataGridView1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.groupBox1.SuspendLayout()
        Me.groupBox4.SuspendLayout()
        Me.groupBox3.SuspendLayout()
        Me.SuspendLayout()
        '
        'groupBox2
        '
        Me.groupBox2.Controls.Add(Me.groupBox7)
        Me.groupBox2.Controls.Add(Me.groupBox6)
        Me.groupBox2.Controls.Add(Me.groupBox5)
        Me.groupBox2.Location = New System.Drawing.Point(30, 198)
        Me.groupBox2.Name = "groupBox2"
        Me.groupBox2.Size = New System.Drawing.Size(717, 528)
        Me.groupBox2.TabIndex = 3
        Me.groupBox2.TabStop = False
        Me.groupBox2.Text = "Complex Binding"
        '
        'groupBox7
        '
        Me.groupBox7.Controls.Add(Me.dgvDetail)
        Me.groupBox7.Controls.Add(Me.dgvMaster)
        Me.groupBox7.Location = New System.Drawing.Point(20, 341)
        Me.groupBox7.Name = "groupBox7"
        Me.groupBox7.Size = New System.Drawing.Size(683, 181)
        Me.groupBox7.TabIndex = 2
        Me.groupBox7.TabStop = False
        Me.groupBox7.Text = "Master/Detail Binding"
        '
        'dgvDetail
        '
        Me.dgvDetail.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvDetail.Location = New System.Drawing.Point(343, 21)
        Me.dgvDetail.Name = "dgvDetail"
        Me.dgvDetail.Size = New System.Drawing.Size(317, 153)
        Me.dgvDetail.TabIndex = 1
        '
        'dgvMaster
        '
        Me.dgvMaster.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvMaster.Location = New System.Drawing.Point(20, 19)
        Me.dgvMaster.Name = "dgvMaster"
        Me.dgvMaster.Size = New System.Drawing.Size(307, 156)
        Me.dgvMaster.TabIndex = 0
        '
        'groupBox6
        '
        Me.groupBox6.Controls.Add(Me.dataGridView2)
        Me.groupBox6.Location = New System.Drawing.Point(21, 174)
        Me.groupBox6.Name = "groupBox6"
        Me.groupBox6.Size = New System.Drawing.Size(681, 158)
        Me.groupBox6.TabIndex = 1
        Me.groupBox6.TabStop = False
        Me.groupBox6.Text = "Display Data From Business Objects"
        '
        'dataGridView2
        '
        Me.dataGridView2.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dataGridView2.Location = New System.Drawing.Point(20, 19)
        Me.dataGridView2.Name = "dataGridView2"
        Me.dataGridView2.Size = New System.Drawing.Size(639, 120)
        Me.dataGridView2.TabIndex = 0
        '
        'groupBox5
        '
        Me.groupBox5.Controls.Add(Me.dataGridView1)
        Me.groupBox5.Location = New System.Drawing.Point(20, 19)
        Me.groupBox5.Name = "groupBox5"
        Me.groupBox5.Size = New System.Drawing.Size(682, 149)
        Me.groupBox5.TabIndex = 0
        Me.groupBox5.TabStop = False
        Me.groupBox5.Text = "Display Data From Database"
        '
        'dataGridView1
        '
        Me.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dataGridView1.Location = New System.Drawing.Point(20, 19)
        Me.dataGridView1.Name = "dataGridView1"
        Me.dataGridView1.Size = New System.Drawing.Size(640, 112)
        Me.dataGridView1.TabIndex = 0
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.groupBox4)
        Me.groupBox1.Controls.Add(Me.groupBox3)
        Me.groupBox1.Controls.Add(Me.flowLayoutPanel1)
        Me.groupBox1.Location = New System.Drawing.Point(30, 18)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(717, 174)
        Me.groupBox1.TabIndex = 2
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Simple (Property) Binding"
        '
        'groupBox4
        '
        Me.groupBox4.Controls.Add(Me.btnNext)
        Me.groupBox4.Controls.Add(Me.btnPrev)
        Me.groupBox4.Controls.Add(Me.label3)
        Me.groupBox4.Controls.Add(Me.label2)
        Me.groupBox4.Controls.Add(Me.textBox4)
        Me.groupBox4.Controls.Add(Me.textBox3)
        Me.groupBox4.Location = New System.Drawing.Point(21, 87)
        Me.groupBox4.Name = "groupBox4"
        Me.groupBox4.Size = New System.Drawing.Size(682, 81)
        Me.groupBox4.TabIndex = 17
        Me.groupBox4.TabStop = False
        Me.groupBox4.Text = "Binding To DataTable Column"
        '
        'btnNext
        '
        Me.btnNext.Location = New System.Drawing.Point(19, 46)
        Me.btnNext.Name = "btnNext"
        Me.btnNext.Size = New System.Drawing.Size(149, 23)
        Me.btnNext.TabIndex = 5
        Me.btnNext.Text = "Next Record   >> "
        Me.btnNext.UseVisualStyleBackColor = True
        '
        'btnPrev
        '
        Me.btnPrev.Location = New System.Drawing.Point(19, 19)
        Me.btnPrev.Name = "btnPrev"
        Me.btnPrev.Size = New System.Drawing.Size(149, 23)
        Me.btnPrev.TabIndex = 4
        Me.btnPrev.Text = "<<   Previous Record"
        Me.btnPrev.UseVisualStyleBackColor = True
        '
        'label3
        '
        Me.label3.AutoSize = True
        Me.label3.Location = New System.Drawing.Point(261, 53)
        Me.label3.Name = "label3"
        Me.label3.Size = New System.Drawing.Size(38, 13)
        Me.label3.TabIndex = 3
        Me.label3.Text = "Name:"
        '
        'label2
        '
        Me.label2.AutoSize = True
        Me.label2.Location = New System.Drawing.Point(261, 25)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(21, 13)
        Me.label2.TabIndex = 2
        Me.label2.Text = "ID:"
        '
        'textBox4
        '
        Me.textBox4.Location = New System.Drawing.Point(306, 48)
        Me.textBox4.Name = "textBox4"
        Me.textBox4.Size = New System.Drawing.Size(100, 20)
        Me.textBox4.TabIndex = 1
        '
        'textBox3
        '
        Me.textBox3.Location = New System.Drawing.Point(306, 22)
        Me.textBox3.Name = "textBox3"
        Me.textBox3.Size = New System.Drawing.Size(100, 20)
        Me.textBox3.TabIndex = 0
        '
        'groupBox3
        '
        Me.groupBox3.Controls.Add(Me.btnSet)
        Me.groupBox3.Controls.Add(Me.label1)
        Me.groupBox3.Controls.Add(Me.textBox2)
        Me.groupBox3.Controls.Add(Me.textBox1)
        Me.groupBox3.Controls.Add(Me.checkBox1)
        Me.groupBox3.Location = New System.Drawing.Point(21, 22)
        Me.groupBox3.Name = "groupBox3"
        Me.groupBox3.Size = New System.Drawing.Size(683, 50)
        Me.groupBox3.TabIndex = 16
        Me.groupBox3.TabStop = False
        Me.groupBox3.Text = "Binding To Class Property"
        '
        'btnSet
        '
        Me.btnSet.Location = New System.Drawing.Point(550, 15)
        Me.btnSet.Name = "btnSet"
        Me.btnSet.Size = New System.Drawing.Size(50, 23)
        Me.btnSet.TabIndex = 18
        Me.btnSet.Text = "Set"
        Me.btnSet.UseVisualStyleBackColor = True
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(387, 21)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(56, 13)
        Me.label1.TabIndex = 17
        Me.label1.Text = "Form Size:"
        '
        'textBox2
        '
        Me.textBox2.Location = New System.Drawing.Point(444, 18)
        Me.textBox2.Name = "textBox2"
        Me.textBox2.Size = New System.Drawing.Size(100, 20)
        Me.textBox2.TabIndex = 16
        '
        'textBox1
        '
        Me.textBox1.Location = New System.Drawing.Point(19, 19)
        Me.textBox1.Name = "textBox1"
        Me.textBox1.ReadOnly = True
        Me.textBox1.Size = New System.Drawing.Size(62, 20)
        Me.textBox1.TabIndex = 15
        '
        'checkBox1
        '
        Me.checkBox1.AutoSize = True
        Me.checkBox1.Location = New System.Drawing.Point(99, 21)
        Me.checkBox1.Name = "checkBox1"
        Me.checkBox1.Size = New System.Drawing.Size(200, 17)
        Me.checkBox1.TabIndex = 9
        Me.checkBox1.Text = "Binding to property of another control"
        Me.checkBox1.UseVisualStyleBackColor = True
        '
        'flowLayoutPanel1
        '
        Me.flowLayoutPanel1.AutoSize = True
        Me.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Top
        Me.flowLayoutPanel1.Location = New System.Drawing.Point(3, 16)
        Me.flowLayoutPanel1.Name = "flowLayoutPanel1"
        Me.flowLayoutPanel1.Size = New System.Drawing.Size(711, 0)
        Me.flowLayoutPanel1.TabIndex = 14
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(776, 744)
        Me.Controls.Add(Me.groupBox2)
        Me.Controls.Add(Me.groupBox1)
        Me.Name = "MainForm"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "VBWinFormDataBinding"
        Me.groupBox2.ResumeLayout(False)
        Me.groupBox7.ResumeLayout(False)
        CType(Me.dgvDetail, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvMaster, System.ComponentModel.ISupportInitialize).EndInit()
        Me.groupBox6.ResumeLayout(False)
        CType(Me.dataGridView2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.groupBox5.ResumeLayout(False)
        CType(Me.dataGridView1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.groupBox1.ResumeLayout(False)
        Me.groupBox1.PerformLayout()
        Me.groupBox4.ResumeLayout(False)
        Me.groupBox4.PerformLayout()
        Me.groupBox3.ResumeLayout(False)
        Me.groupBox3.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents groupBox2 As System.Windows.Forms.GroupBox
    Private WithEvents groupBox7 As System.Windows.Forms.GroupBox
    Private WithEvents dgvDetail As System.Windows.Forms.DataGridView
    Private WithEvents dgvMaster As System.Windows.Forms.DataGridView
    Private WithEvents groupBox6 As System.Windows.Forms.GroupBox
    Private WithEvents dataGridView2 As System.Windows.Forms.DataGridView
    Private WithEvents groupBox5 As System.Windows.Forms.GroupBox
    Private WithEvents dataGridView1 As System.Windows.Forms.DataGridView
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents groupBox4 As System.Windows.Forms.GroupBox
    Private WithEvents btnNext As System.Windows.Forms.Button
    Private WithEvents btnPrev As System.Windows.Forms.Button
    Private WithEvents label3 As System.Windows.Forms.Label
    Private WithEvents label2 As System.Windows.Forms.Label
    Private WithEvents textBox4 As System.Windows.Forms.TextBox
    Private WithEvents textBox3 As System.Windows.Forms.TextBox
    Private WithEvents groupBox3 As System.Windows.Forms.GroupBox
    Private WithEvents btnSet As System.Windows.Forms.Button
    Private WithEvents label1 As System.Windows.Forms.Label
    Private WithEvents textBox2 As System.Windows.Forms.TextBox
    Private WithEvents textBox1 As System.Windows.Forms.TextBox
    Private WithEvents checkBox1 As System.Windows.Forms.CheckBox
    Private WithEvents flowLayoutPanel1 As System.Windows.Forms.FlowLayoutPanel

End Class
