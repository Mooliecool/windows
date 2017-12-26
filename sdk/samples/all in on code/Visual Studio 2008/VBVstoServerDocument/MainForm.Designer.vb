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
        Me.components = New System.ComponentModel.Container
        Me.txtAssembly = New System.Windows.Forms.TextBox
        Me.lblAssembly = New System.Windows.Forms.Label
        Me.grpCustomization = New System.Windows.Forms.GroupBox
        Me.txtManifest = New System.Windows.Forms.TextBox
        Me.lblManifest = New System.Windows.Forms.Label
        Me.btnRemove = New System.Windows.Forms.Button
        Me.btnAdd = New System.Windows.Forms.Button
        Me.lblFileName = New System.Windows.Forms.Label
        Me.toolTip = New System.Windows.Forms.ToolTip(Me.components)
        Me.btnSelectFile = New System.Windows.Forms.Button
        Me.ofd = New System.Windows.Forms.OpenFileDialog
        Me.lstDocInfo = New System.Windows.Forms.ListBox
        Me.txtFileName = New System.Windows.Forms.TextBox
        Me.grpCustomization.SuspendLayout()
        Me.SuspendLayout()
        '
        'txtAssembly
        '
        Me.txtAssembly.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.txtAssembly.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest
        Me.txtAssembly.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.AllUrl
        Me.txtAssembly.Enabled = False
        Me.txtAssembly.Location = New System.Drawing.Point(121, 45)
        Me.txtAssembly.Name = "txtAssembly"
        Me.txtAssembly.Size = New System.Drawing.Size(307, 20)
        Me.txtAssembly.TabIndex = 3
        Me.toolTip.SetToolTip(Me.txtAssembly, "VSTO SE customization assembly URI")
        '
        'lblAssembly
        '
        Me.lblAssembly.AutoSize = True
        Me.lblAssembly.Enabled = False
        Me.lblAssembly.Location = New System.Drawing.Point(6, 48)
        Me.lblAssembly.Name = "lblAssembly"
        Me.lblAssembly.Size = New System.Drawing.Size(79, 13)
        Me.lblAssembly.TabIndex = 2
        Me.lblAssembly.Text = "Assembly Path:"
        '
        'grpCustomization
        '
        Me.grpCustomization.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.grpCustomization.Controls.Add(Me.txtAssembly)
        Me.grpCustomization.Controls.Add(Me.lblAssembly)
        Me.grpCustomization.Controls.Add(Me.txtManifest)
        Me.grpCustomization.Controls.Add(Me.lblManifest)
        Me.grpCustomization.Controls.Add(Me.btnRemove)
        Me.grpCustomization.Controls.Add(Me.btnAdd)
        Me.grpCustomization.Location = New System.Drawing.Point(12, 192)
        Me.grpCustomization.Name = "grpCustomization"
        Me.grpCustomization.Size = New System.Drawing.Size(434, 134)
        Me.grpCustomization.TabIndex = 4
        Me.grpCustomization.TabStop = False
        Me.grpCustomization.Text = "Document Customization"
        '
        'txtManifest
        '
        Me.txtManifest.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.txtManifest.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest
        Me.txtManifest.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.AllUrl
        Me.txtManifest.Enabled = False
        Me.txtManifest.Location = New System.Drawing.Point(121, 19)
        Me.txtManifest.Name = "txtManifest"
        Me.txtManifest.Size = New System.Drawing.Size(307, 20)
        Me.txtManifest.TabIndex = 1
        Me.toolTip.SetToolTip(Me.txtManifest, "Deployment manifest URI")
        '
        'lblManifest
        '
        Me.lblManifest.AutoSize = True
        Me.lblManifest.Enabled = False
        Me.lblManifest.Location = New System.Drawing.Point(6, 22)
        Me.lblManifest.Name = "lblManifest"
        Me.lblManifest.Size = New System.Drawing.Size(109, 13)
        Me.lblManifest.TabIndex = 0
        Me.lblManifest.Text = "Deployment Manifest:"
        '
        'btnRemove
        '
        Me.btnRemove.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnRemove.Enabled = False
        Me.btnRemove.Location = New System.Drawing.Point(298, 105)
        Me.btnRemove.Name = "btnRemove"
        Me.btnRemove.Size = New System.Drawing.Size(130, 23)
        Me.btnRemove.TabIndex = 5
        Me.btnRemove.Text = "Remove Customization"
        Me.btnRemove.UseVisualStyleBackColor = True
        '
        'btnAdd
        '
        Me.btnAdd.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnAdd.Enabled = False
        Me.btnAdd.Location = New System.Drawing.Point(298, 76)
        Me.btnAdd.Name = "btnAdd"
        Me.btnAdd.Size = New System.Drawing.Size(130, 23)
        Me.btnAdd.TabIndex = 4
        Me.btnAdd.Text = "Add Customization..."
        Me.btnAdd.UseVisualStyleBackColor = True
        '
        'lblFileName
        '
        Me.lblFileName.AutoSize = True
        Me.lblFileName.Location = New System.Drawing.Point(12, 15)
        Me.lblFileName.Name = "lblFileName"
        Me.lblFileName.Size = New System.Drawing.Size(120, 13)
        Me.lblFileName.TabIndex = 0
        Me.lblFileName.Text = "Document / Workbook:"
        '
        'btnSelectFile
        '
        Me.btnSelectFile.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnSelectFile.Location = New System.Drawing.Point(371, 10)
        Me.btnSelectFile.Name = "btnSelectFile"
        Me.btnSelectFile.Size = New System.Drawing.Size(75, 23)
        Me.btnSelectFile.TabIndex = 2
        Me.btnSelectFile.Text = "Select..."
        Me.btnSelectFile.UseVisualStyleBackColor = True
        '
        'ofd
        '
        Me.ofd.Filter = "Word Douments|*.doc;*.dot;*.docx;*.dotx;*.docm;*.dotm|Excel Workbooks|*.xls;*.xlt" & _
            ";*.xlsx;*.xltx;*.xlsm;*.xltm"
        '
        'lstDocInfo
        '
        Me.lstDocInfo.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lstDocInfo.FormattingEnabled = True
        Me.lstDocInfo.HorizontalScrollbar = True
        Me.lstDocInfo.Location = New System.Drawing.Point(15, 51)
        Me.lstDocInfo.Name = "lstDocInfo"
        Me.lstDocInfo.Size = New System.Drawing.Size(431, 121)
        Me.lstDocInfo.TabIndex = 3
        '
        'txtFileName
        '
        Me.txtFileName.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.txtFileName.Location = New System.Drawing.Point(138, 12)
        Me.txtFileName.Name = "txtFileName"
        Me.txtFileName.ReadOnly = True
        Me.txtFileName.Size = New System.Drawing.Size(227, 20)
        Me.txtFileName.TabIndex = 1
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(458, 342)
        Me.Controls.Add(Me.grpCustomization)
        Me.Controls.Add(Me.lblFileName)
        Me.Controls.Add(Me.btnSelectFile)
        Me.Controls.Add(Me.lstDocInfo)
        Me.Controls.Add(Me.txtFileName)
        Me.MinimumSize = New System.Drawing.Size(449, 380)
        Me.Name = "MainForm"
        Me.Text = "ServerDocument Demo"
        Me.grpCustomization.ResumeLayout(False)
        Me.grpCustomization.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents txtAssembly As System.Windows.Forms.TextBox
    Private WithEvents toolTip As System.Windows.Forms.ToolTip
    Private WithEvents lblAssembly As System.Windows.Forms.Label
    Private WithEvents grpCustomization As System.Windows.Forms.GroupBox
    Private WithEvents txtManifest As System.Windows.Forms.TextBox
    Private WithEvents lblManifest As System.Windows.Forms.Label
    Private WithEvents btnRemove As System.Windows.Forms.Button
    Private WithEvents btnAdd As System.Windows.Forms.Button
    Private WithEvents lblFileName As System.Windows.Forms.Label
    Private WithEvents btnSelectFile As System.Windows.Forms.Button
    Private WithEvents ofd As System.Windows.Forms.OpenFileDialog
    Private WithEvents lstDocInfo As System.Windows.Forms.ListBox
    Private WithEvents txtFileName As System.Windows.Forms.TextBox

End Class
