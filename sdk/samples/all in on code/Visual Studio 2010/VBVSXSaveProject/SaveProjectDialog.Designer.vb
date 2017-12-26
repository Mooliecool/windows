<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class SaveProjectDialog
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
        Dim DataGridViewCellStyle1 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle2 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle3 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Me.dgvFiles = New System.Windows.Forms.DataGridView()
        Me.colCopy = New System.Windows.Forms.DataGridViewCheckBoxColumn()
        Me.colUnderProjectFolder = New System.Windows.Forms.DataGridViewCheckBoxColumn()
        Me.FileName = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.FullName = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.SaveFileDialog1 = New System.Windows.Forms.SaveFileDialog()
        Me.pnlOperation = New System.Windows.Forms.Panel()
        Me.chkOpenProject = New System.Windows.Forms.CheckBox()
        Me.btnCancel = New System.Windows.Forms.Button()
        Me.btnSaveAs = New System.Windows.Forms.Button()
        CType(Me.dgvFiles, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.pnlOperation.SuspendLayout()
        Me.SuspendLayout()
        '
        'dgvFiles
        '
        DataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control
        DataGridViewCellStyle1.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(134, Byte))
        DataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText
        DataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight
        DataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText
        DataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.[True]
        Me.dgvFiles.ColumnHeadersDefaultCellStyle = DataGridViewCellStyle1
        Me.dgvFiles.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvFiles.Columns.AddRange(New System.Windows.Forms.DataGridViewColumn() {Me.colCopy, Me.colUnderProjectFolder, Me.FileName, Me.FullName})
        DataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window
        DataGridViewCellStyle2.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(134, Byte))
        DataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText
        DataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight
        DataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText
        DataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.[False]
        Me.dgvFiles.DefaultCellStyle = DataGridViewCellStyle2
        Me.dgvFiles.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvFiles.Location = New System.Drawing.Point(0, 0)
        Me.dgvFiles.Name = "dgvFiles"
        DataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.Control
        DataGridViewCellStyle3.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(134, Byte))
        DataGridViewCellStyle3.ForeColor = System.Drawing.SystemColors.WindowText
        DataGridViewCellStyle3.SelectionBackColor = System.Drawing.SystemColors.Highlight
        DataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.HighlightText
        DataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.[True]
        Me.dgvFiles.RowHeadersDefaultCellStyle = DataGridViewCellStyle3
        Me.dgvFiles.RowHeadersVisible = False
        Me.dgvFiles.RowTemplate.Height = 24
        Me.dgvFiles.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect
        Me.dgvFiles.Size = New System.Drawing.Size(716, 191)
        Me.dgvFiles.TabIndex = 0
        '
        'colCopy
        '
        Me.colCopy.DataPropertyName = "NeedCopy"
        Me.colCopy.FillWeight = 50.0!
        Me.colCopy.HeaderText = "Copy"
        Me.colCopy.Name = "colCopy"
        Me.colCopy.Width = 50
        '
        'colUnderProjectFolder
        '
        Me.colUnderProjectFolder.DataPropertyName = "IsUnderProjectFolder"
        Me.colUnderProjectFolder.HeaderText = "UnderProjectFolder"
        Me.colUnderProjectFolder.Name = "colUnderProjectFolder"
        Me.colUnderProjectFolder.ReadOnly = True
        '
        'FileName
        '
        Me.FileName.DataPropertyName = "FileName"
        Me.FileName.FillWeight = 200.0!
        Me.FileName.HeaderText = "FileName"
        Me.FileName.Name = "FileName"
        Me.FileName.Width = 200
        '
        'FullName
        '
        Me.FullName.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill
        Me.FullName.DataPropertyName = "FullName"
        Me.FullName.FillWeight = 200.0!
        Me.FullName.HeaderText = "FullName"
        Me.FullName.Name = "FullName"
        '
        'pnlOperation
        '
        Me.pnlOperation.Controls.Add(Me.chkOpenProject)
        Me.pnlOperation.Controls.Add(Me.btnCancel)
        Me.pnlOperation.Controls.Add(Me.btnSaveAs)
        Me.pnlOperation.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.pnlOperation.Location = New System.Drawing.Point(0, 191)
        Me.pnlOperation.Margin = New System.Windows.Forms.Padding(2)
        Me.pnlOperation.Name = "pnlOperation"
        Me.pnlOperation.Size = New System.Drawing.Size(716, 51)
        Me.pnlOperation.TabIndex = 3
        '
        'chkOpenProject
        '
        Me.chkOpenProject.AutoSize = True
        Me.chkOpenProject.Location = New System.Drawing.Point(367, 19)
        Me.chkOpenProject.Margin = New System.Windows.Forms.Padding(2)
        Me.chkOpenProject.Name = "chkOpenProject"
        Me.chkOpenProject.Size = New System.Drawing.Size(113, 17)
        Me.chkOpenProject.TabIndex = 3
        Me.chkOpenProject.Text = "Open New Project"
        Me.chkOpenProject.UseVisualStyleBackColor = True
        '
        'btnCancel
        '
        Me.btnCancel.Location = New System.Drawing.Point(629, 16)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New System.Drawing.Size(75, 23)
        Me.btnCancel.TabIndex = 2
        Me.btnCancel.Text = "Cancel"
        Me.btnCancel.UseVisualStyleBackColor = True
        '
        'btnSaveAs
        '
        Me.btnSaveAs.Location = New System.Drawing.Point(520, 15)
        Me.btnSaveAs.Name = "btnSaveAs"
        Me.btnSaveAs.Size = New System.Drawing.Size(85, 23)
        Me.btnSaveAs.TabIndex = 1
        Me.btnSaveAs.Text = "Save As"
        Me.btnSaveAs.UseVisualStyleBackColor = True
        '
        'SaveProjectDialog
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(716, 242)
        Me.Controls.Add(Me.dgvFiles)
        Me.Controls.Add(Me.pnlOperation)
        Me.Name = "SaveProjectDialog"
        Me.Text = "SaveProjectDialog"
        CType(Me.dgvFiles, System.ComponentModel.ISupportInitialize).EndInit()
        Me.pnlOperation.ResumeLayout(False)
        Me.pnlOperation.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dgvFiles As System.Windows.Forms.DataGridView
    Friend WithEvents SaveFileDialog1 As System.Windows.Forms.SaveFileDialog
    Friend WithEvents pnlOperation As System.Windows.Forms.Panel
    Friend WithEvents chkOpenProject As System.Windows.Forms.CheckBox
    Friend WithEvents btnCancel As System.Windows.Forms.Button
    Friend WithEvents btnSaveAs As System.Windows.Forms.Button
    Friend WithEvents colCopy As System.Windows.Forms.DataGridViewCheckBoxColumn
    Friend WithEvents colUnderProjectFolder As System.Windows.Forms.DataGridViewCheckBoxColumn
    Friend WithEvents FileName As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents FullName As System.Windows.Forms.DataGridViewTextBoxColumn

End Class
