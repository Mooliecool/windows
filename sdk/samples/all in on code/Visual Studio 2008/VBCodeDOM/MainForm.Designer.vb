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
        Me.tableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel
        Me.gbNamespaces = New System.Windows.Forms.GroupBox
        Me.btnAddNamespace = New System.Windows.Forms.Button
        Me.btnRemoveNamespace = New System.Windows.Forms.Button
        Me.txtNamespace = New System.Windows.Forms.TextBox
        Me.lstNamespaces = New System.Windows.Forms.ListBox
        Me.gbAssemblyRef = New System.Windows.Forms.GroupBox
        Me.btnAddAsmRef = New System.Windows.Forms.Button
        Me.btnRemoveAsmRef = New System.Windows.Forms.Button
        Me.txtAssemblyRef = New System.Windows.Forms.TextBox
        Me.lstAssemblyRef = New System.Windows.Forms.ListBox
        Me.txtSource = New System.Windows.Forms.TextBox
        Me.chkSpDomain = New System.Windows.Forms.CheckBox
        Me.cboLang = New System.Windows.Forms.ComboBox
        Me.lblLang = New System.Windows.Forms.Label
        Me.btnRun = New System.Windows.Forms.Button
        Me.tableLayoutPanel1.SuspendLayout()
        Me.gbNamespaces.SuspendLayout()
        Me.gbAssemblyRef.SuspendLayout()
        Me.SuspendLayout()
        '
        'tableLayoutPanel1
        '
        Me.tableLayoutPanel1.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tableLayoutPanel1.AutoSize = True
        Me.tableLayoutPanel1.ColumnCount = 2
        Me.tableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.tableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.tableLayoutPanel1.Controls.Add(Me.gbNamespaces, 0, 0)
        Me.tableLayoutPanel1.Controls.Add(Me.gbAssemblyRef, 1, 0)
        Me.tableLayoutPanel1.Location = New System.Drawing.Point(12, 256)
        Me.tableLayoutPanel1.Name = "tableLayoutPanel1"
        Me.tableLayoutPanel1.RowCount = 1
        Me.tableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle)
        Me.tableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20.0!))
        Me.tableLayoutPanel1.Size = New System.Drawing.Size(600, 145)
        Me.tableLayoutPanel1.TabIndex = 7
        '
        'gbNamespaces
        '
        Me.gbNamespaces.Controls.Add(Me.btnAddNamespace)
        Me.gbNamespaces.Controls.Add(Me.btnRemoveNamespace)
        Me.gbNamespaces.Controls.Add(Me.txtNamespace)
        Me.gbNamespaces.Controls.Add(Me.lstNamespaces)
        Me.gbNamespaces.Dock = System.Windows.Forms.DockStyle.Fill
        Me.gbNamespaces.Location = New System.Drawing.Point(3, 3)
        Me.gbNamespaces.Name = "gbNamespaces"
        Me.gbNamespaces.Size = New System.Drawing.Size(294, 139)
        Me.gbNamespaces.TabIndex = 0
        Me.gbNamespaces.TabStop = False
        Me.gbNamespaces.Text = "Import Namespaces"
        '
        'btnAddNamespace
        '
        Me.btnAddNamespace.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnAddNamespace.Enabled = False
        Me.btnAddNamespace.Location = New System.Drawing.Point(262, 105)
        Me.btnAddNamespace.Name = "btnAddNamespace"
        Me.btnAddNamespace.Size = New System.Drawing.Size(23, 23)
        Me.btnAddNamespace.TabIndex = 2
        Me.btnAddNamespace.Text = "+"
        Me.btnAddNamespace.UseVisualStyleBackColor = True
        '
        'btnRemoveNamespace
        '
        Me.btnRemoveNamespace.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnRemoveNamespace.Enabled = False
        Me.btnRemoveNamespace.Location = New System.Drawing.Point(262, 78)
        Me.btnRemoveNamespace.Name = "btnRemoveNamespace"
        Me.btnRemoveNamespace.Size = New System.Drawing.Size(23, 23)
        Me.btnRemoveNamespace.TabIndex = 3
        Me.btnRemoveNamespace.Text = "-"
        Me.btnRemoveNamespace.UseVisualStyleBackColor = True
        '
        'txtNamespace
        '
        Me.txtNamespace.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.txtNamespace.Location = New System.Drawing.Point(6, 107)
        Me.txtNamespace.Name = "txtNamespace"
        Me.txtNamespace.Size = New System.Drawing.Size(250, 20)
        Me.txtNamespace.TabIndex = 1
        '
        'lstNamespaces
        '
        Me.lstNamespaces.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lstNamespaces.FormattingEnabled = True
        Me.lstNamespaces.Items.AddRange(New Object() {"System", "System.IO", "System.Data", "System.Linq", "System.Xml", "System.Text", "System.Collections.Generic", "System.Drawing", "System.ComponentModel", "System.Windows.Forms", "Microsoft.Win32", "Microsoft.VisualBasic", "Microsoft.JScript"})
        Me.lstNamespaces.Location = New System.Drawing.Point(6, 19)
        Me.lstNamespaces.Name = "lstNamespaces"
        Me.lstNamespaces.Size = New System.Drawing.Size(250, 82)
        Me.lstNamespaces.TabIndex = 0
        '
        'gbAssemblyRef
        '
        Me.gbAssemblyRef.Controls.Add(Me.btnAddAsmRef)
        Me.gbAssemblyRef.Controls.Add(Me.btnRemoveAsmRef)
        Me.gbAssemblyRef.Controls.Add(Me.txtAssemblyRef)
        Me.gbAssemblyRef.Controls.Add(Me.lstAssemblyRef)
        Me.gbAssemblyRef.Dock = System.Windows.Forms.DockStyle.Fill
        Me.gbAssemblyRef.Location = New System.Drawing.Point(303, 3)
        Me.gbAssemblyRef.Name = "gbAssemblyRef"
        Me.gbAssemblyRef.Size = New System.Drawing.Size(294, 139)
        Me.gbAssemblyRef.TabIndex = 1
        Me.gbAssemblyRef.TabStop = False
        Me.gbAssemblyRef.Text = "Assembly References"
        '
        'btnAddAsmRef
        '
        Me.btnAddAsmRef.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnAddAsmRef.Enabled = False
        Me.btnAddAsmRef.Location = New System.Drawing.Point(262, 105)
        Me.btnAddAsmRef.Name = "btnAddAsmRef"
        Me.btnAddAsmRef.Size = New System.Drawing.Size(23, 23)
        Me.btnAddAsmRef.TabIndex = 2
        Me.btnAddAsmRef.Text = "+"
        Me.btnAddAsmRef.UseVisualStyleBackColor = True
        '
        'btnRemoveAsmRef
        '
        Me.btnRemoveAsmRef.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnRemoveAsmRef.Enabled = False
        Me.btnRemoveAsmRef.Location = New System.Drawing.Point(262, 78)
        Me.btnRemoveAsmRef.Name = "btnRemoveAsmRef"
        Me.btnRemoveAsmRef.Size = New System.Drawing.Size(23, 23)
        Me.btnRemoveAsmRef.TabIndex = 3
        Me.btnRemoveAsmRef.Text = "-"
        Me.btnRemoveAsmRef.UseVisualStyleBackColor = True
        '
        'txtAssemblyRef
        '
        Me.txtAssemblyRef.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.txtAssemblyRef.Location = New System.Drawing.Point(6, 108)
        Me.txtAssemblyRef.Name = "txtAssemblyRef"
        Me.txtAssemblyRef.Size = New System.Drawing.Size(250, 20)
        Me.txtAssemblyRef.TabIndex = 1
        '
        'lstAssemblyRef
        '
        Me.lstAssemblyRef.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lstAssemblyRef.FormattingEnabled = True
        Me.lstAssemblyRef.Items.AddRange(New Object() {"mscorlib.dll", "System.dll", "System.Core.dll", "System.Drawing.dll", "System.Data.dll", "System.Xml.dll", "System.Xml.Linq.dll", "System.Windows.Forms.dll", "Microsoft.VisualBasic.dll", "Microsoft.JScript.dll"})
        Me.lstAssemblyRef.Location = New System.Drawing.Point(6, 19)
        Me.lstAssemblyRef.Name = "lstAssemblyRef"
        Me.lstAssemblyRef.Size = New System.Drawing.Size(250, 82)
        Me.lstAssemblyRef.TabIndex = 0
        '
        'txtSource
        '
        Me.txtSource.AcceptsReturn = True
        Me.txtSource.AcceptsTab = True
        Me.txtSource.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.txtSource.Font = New System.Drawing.Font("Consolas", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtSource.Location = New System.Drawing.Point(12, 12)
        Me.txtSource.Multiline = True
        Me.txtSource.Name = "txtSource"
        Me.txtSource.ScrollBars = System.Windows.Forms.ScrollBars.Both
        Me.txtSource.Size = New System.Drawing.Size(600, 238)
        Me.txtSource.TabIndex = 6
        Me.txtSource.Text = resources.GetString("txtSource.Text")
        Me.txtSource.WordWrap = False
        '
        'chkSpDomain
        '
        Me.chkSpDomain.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.chkSpDomain.AutoSize = True
        Me.chkSpDomain.Checked = True
        Me.chkSpDomain.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkSpDomain.Location = New System.Drawing.Point(264, 411)
        Me.chkSpDomain.Name = "chkSpDomain"
        Me.chkSpDomain.Size = New System.Drawing.Size(187, 17)
        Me.chkSpDomain.TabIndex = 10
        Me.chkSpDomain.Text = "Run script in separate App&Domain"
        Me.chkSpDomain.UseVisualStyleBackColor = True
        '
        'cboLang
        '
        Me.cboLang.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.cboLang.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cboLang.FormattingEnabled = True
        Me.cboLang.Items.AddRange(New Object() {"C#", "Visual Basic", "JScript"})
        Me.cboLang.Location = New System.Drawing.Point(76, 409)
        Me.cboLang.Name = "cboLang"
        Me.cboLang.Size = New System.Drawing.Size(151, 21)
        Me.cboLang.TabIndex = 9
        '
        'lblLang
        '
        Me.lblLang.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.lblLang.AutoSize = True
        Me.lblLang.Location = New System.Drawing.Point(12, 412)
        Me.lblLang.Name = "lblLang"
        Me.lblLang.Size = New System.Drawing.Size(58, 13)
        Me.lblLang.TabIndex = 8
        Me.lblLang.Text = "&Language:"
        '
        'btnRun
        '
        Me.btnRun.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnRun.Location = New System.Drawing.Point(537, 407)
        Me.btnRun.Name = "btnRun"
        Me.btnRun.Size = New System.Drawing.Size(75, 23)
        Me.btnRun.TabIndex = 11
        Me.btnRun.Text = "&Run"
        Me.btnRun.UseVisualStyleBackColor = True
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(624, 442)
        Me.Controls.Add(Me.tableLayoutPanel1)
        Me.Controls.Add(Me.txtSource)
        Me.Controls.Add(Me.chkSpDomain)
        Me.Controls.Add(Me.cboLang)
        Me.Controls.Add(Me.lblLang)
        Me.Controls.Add(Me.btnRun)
        Me.MinimumSize = New System.Drawing.Size(640, 480)
        Me.Name = "MainForm"
        Me.Text = "VBCodeDOM"
        Me.tableLayoutPanel1.ResumeLayout(False)
        Me.gbNamespaces.ResumeLayout(False)
        Me.gbNamespaces.PerformLayout()
        Me.gbAssemblyRef.ResumeLayout(False)
        Me.gbAssemblyRef.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents tableLayoutPanel1 As System.Windows.Forms.TableLayoutPanel
    Private WithEvents gbNamespaces As System.Windows.Forms.GroupBox
    Private WithEvents btnAddNamespace As System.Windows.Forms.Button
    Private WithEvents btnRemoveNamespace As System.Windows.Forms.Button
    Private WithEvents txtNamespace As System.Windows.Forms.TextBox
    Private WithEvents lstNamespaces As System.Windows.Forms.ListBox
    Private WithEvents gbAssemblyRef As System.Windows.Forms.GroupBox
    Private WithEvents btnAddAsmRef As System.Windows.Forms.Button
    Private WithEvents btnRemoveAsmRef As System.Windows.Forms.Button
    Private WithEvents txtAssemblyRef As System.Windows.Forms.TextBox
    Private WithEvents lstAssemblyRef As System.Windows.Forms.ListBox
    Private WithEvents txtSource As System.Windows.Forms.TextBox
    Private WithEvents chkSpDomain As System.Windows.Forms.CheckBox
    Private WithEvents cboLang As System.Windows.Forms.ComboBox
    Private WithEvents lblLang As System.Windows.Forms.Label
    Private WithEvents btnRun As System.Windows.Forms.Button

End Class
