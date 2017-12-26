<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class LinqToCSVForm
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
        Me.btnLoad = New System.Windows.Forms.Button
        Me.dgvLinqToCSV = New System.Windows.Forms.DataGridView
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        CType(Me.dgvLinqToCSV, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnLoad
        '
        Me.btnLoad.Location = New System.Drawing.Point(62, 32)
        Me.btnLoad.Name = "btnLoad"
        Me.btnLoad.Size = New System.Drawing.Size(75, 21)
        Me.btnLoad.TabIndex = 0
        Me.btnLoad.Text = "Load"
        Me.btnLoad.UseVisualStyleBackColor = True
        '
        'dgvLinqToCSV
        '
        Me.dgvLinqToCSV.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.DisplayedCells
        Me.dgvLinqToCSV.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvLinqToCSV.Location = New System.Drawing.Point(21, 72)
        Me.dgvLinqToCSV.Name = "dgvLinqToCSV"
        Me.dgvLinqToCSV.RowTemplate.Height = 23
        Me.dgvLinqToCSV.Size = New System.Drawing.Size(527, 216)
        Me.dgvLinqToCSV.TabIndex = 1
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.btnLoad)
        Me.groupBox1.Controls.Add(Me.dgvLinqToCSV)
        Me.groupBox1.Location = New System.Drawing.Point(12, 12)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(570, 312)
        Me.groupBox1.TabIndex = 3
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Load data from CSV file"
        '
        'LinqToCSVForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(592, 336)
        Me.Controls.Add(Me.groupBox1)
        Me.Name = "LinqToCSVForm"
        Me.Text = "LINQ To CSV"
        CType(Me.dgvLinqToCSV, System.ComponentModel.ISupportInitialize).EndInit()
        Me.groupBox1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents btnLoad As System.Windows.Forms.Button
    Private WithEvents dgvLinqToCSV As System.Windows.Forms.DataGridView
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
End Class
