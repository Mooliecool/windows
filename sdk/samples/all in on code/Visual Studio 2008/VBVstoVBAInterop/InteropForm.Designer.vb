<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class InteropForm
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(InteropForm))
        Me.lblCode = New System.Windows.Forms.Label
        Me.btnShowVBE = New System.Windows.Forms.Button
        Me.btnInsertRun = New System.Windows.Forms.Button
        Me.txtVbaSub = New System.Windows.Forms.TextBox
        Me.btnRefresh = New System.Windows.Forms.Button
        Me.cboProjects = New System.Windows.Forms.ComboBox
        Me.label1 = New System.Windows.Forms.Label
        Me.chkEnableVbaAccess = New System.Windows.Forms.CheckBox
        Me.SuspendLayout()
        '
        'lblCode
        '
        Me.lblCode.AutoSize = True
        Me.lblCode.Location = New System.Drawing.Point(12, 81)
        Me.lblCode.Name = "lblCode"
        Me.lblCode.Size = New System.Drawing.Size(119, 13)
        Me.lblCode.TabIndex = 4
        Me.lblCode.Text = "VBA Code to be added:"
        '
        'btnShowVBE
        '
        Me.btnShowVBE.Location = New System.Drawing.Point(111, 279)
        Me.btnShowVBE.Name = "btnShowVBE"
        Me.btnShowVBE.Size = New System.Drawing.Size(112, 23)
        Me.btnShowVBE.TabIndex = 6
        Me.btnShowVBE.Text = "Show VBA IDE"
        Me.btnShowVBE.UseVisualStyleBackColor = True
        '
        'btnInsertRun
        '
        Me.btnInsertRun.Location = New System.Drawing.Point(229, 279)
        Me.btnInsertRun.Name = "btnInsertRun"
        Me.btnInsertRun.Size = New System.Drawing.Size(142, 23)
        Me.btnInsertRun.TabIndex = 7
        Me.btnInsertRun.Text = "Insert VBA Code && Run"
        Me.btnInsertRun.UseVisualStyleBackColor = True
        '
        'txtVbaSub
        '
        Me.txtVbaSub.Location = New System.Drawing.Point(15, 97)
        Me.txtVbaSub.Multiline = True
        Me.txtVbaSub.Name = "txtVbaSub"
        Me.txtVbaSub.ReadOnly = True
        Me.txtVbaSub.ScrollBars = System.Windows.Forms.ScrollBars.Both
        Me.txtVbaSub.Size = New System.Drawing.Size(356, 176)
        Me.txtVbaSub.TabIndex = 5
        Me.txtVbaSub.Text = resources.GetString("txtVbaSub.Text")
        '
        'btnRefresh
        '
        Me.btnRefresh.Location = New System.Drawing.Point(296, 46)
        Me.btnRefresh.Name = "btnRefresh"
        Me.btnRefresh.Size = New System.Drawing.Size(75, 23)
        Me.btnRefresh.TabIndex = 3
        Me.btnRefresh.Text = "Refresh"
        Me.btnRefresh.UseVisualStyleBackColor = True
        '
        'cboProjects
        '
        Me.cboProjects.DisplayMember = "Name"
        Me.cboProjects.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cboProjects.FormattingEnabled = True
        Me.cboProjects.Location = New System.Drawing.Point(112, 48)
        Me.cboProjects.Name = "cboProjects"
        Me.cboProjects.Size = New System.Drawing.Size(178, 21)
        Me.cboProjects.TabIndex = 2
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(12, 51)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(94, 13)
        Me.label1.TabIndex = 1
        Me.label1.Text = "Available Projects:"
        '
        'chkEnableVbaAccess
        '
        Me.chkEnableVbaAccess.AutoSize = True
        Me.chkEnableVbaAccess.Location = New System.Drawing.Point(12, 12)
        Me.chkEnableVbaAccess.Name = "chkEnableVbaAccess"
        Me.chkEnableVbaAccess.Size = New System.Drawing.Size(188, 17)
        Me.chkEnableVbaAccess.TabIndex = 0
        Me.chkEnableVbaAccess.Text = "Enable Access to VBA project OM"
        Me.chkEnableVbaAccess.UseVisualStyleBackColor = True
        '
        'InteropForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(383, 312)
        Me.Controls.Add(Me.lblCode)
        Me.Controls.Add(Me.btnShowVBE)
        Me.Controls.Add(Me.btnInsertRun)
        Me.Controls.Add(Me.txtVbaSub)
        Me.Controls.Add(Me.btnRefresh)
        Me.Controls.Add(Me.cboProjects)
        Me.Controls.Add(Me.label1)
        Me.Controls.Add(Me.chkEnableVbaAccess)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "InteropForm"
        Me.ShowInTaskbar = False
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "VBA Interop Demo"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents lblCode As System.Windows.Forms.Label
    Private WithEvents btnShowVBE As System.Windows.Forms.Button
    Private WithEvents btnInsertRun As System.Windows.Forms.Button
    Private WithEvents txtVbaSub As System.Windows.Forms.TextBox
    Private WithEvents btnRefresh As System.Windows.Forms.Button
    Private WithEvents cboProjects As System.Windows.Forms.ComboBox
    Private WithEvents label1 As System.Windows.Forms.Label
    Private WithEvents chkEnableVbaAccess As System.Windows.Forms.CheckBox
End Class
