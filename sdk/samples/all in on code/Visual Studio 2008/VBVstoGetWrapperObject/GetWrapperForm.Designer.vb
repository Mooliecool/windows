<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class GetWrapperForm
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
        Me.btnAddListObject = New System.Windows.Forms.Button
        Me.btnRefreshWs = New System.Windows.Forms.Button
        Me.cboWorksheets = New System.Windows.Forms.ComboBox
        Me.lblWorksheets = New System.Windows.Forms.Label
        Me.btnRefreshWb = New System.Windows.Forms.Button
        Me.cboWorkbooks = New System.Windows.Forms.ComboBox
        Me.lblWorkbooks = New System.Windows.Forms.Label
        Me.SuspendLayout()
        '
        'btnAddListObject
        '
        Me.btnAddListObject.Enabled = False
        Me.btnAddListObject.Location = New System.Drawing.Point(199, 104)
        Me.btnAddListObject.Name = "btnAddListObject"
        Me.btnAddListObject.Size = New System.Drawing.Size(152, 23)
        Me.btnAddListObject.TabIndex = 13
        Me.btnAddListObject.Text = "Add VSTO ListObject"
        Me.btnAddListObject.UseVisualStyleBackColor = True
        '
        'btnRefreshWs
        '
        Me.btnRefreshWs.Location = New System.Drawing.Point(272, 57)
        Me.btnRefreshWs.Name = "btnRefreshWs"
        Me.btnRefreshWs.Size = New System.Drawing.Size(75, 23)
        Me.btnRefreshWs.TabIndex = 5
        Me.btnRefreshWs.Text = "Refresh"
        Me.btnRefreshWs.UseVisualStyleBackColor = True
        '
        'cboWorksheets
        '
        Me.cboWorksheets.DisplayMember = "Name"
        Me.cboWorksheets.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cboWorksheets.FormattingEnabled = True
        Me.cboWorksheets.Location = New System.Drawing.Point(85, 59)
        Me.cboWorksheets.Name = "cboWorksheets"
        Me.cboWorksheets.Size = New System.Drawing.Size(181, 21)
        Me.cboWorksheets.TabIndex = 4
        '
        'lblWorksheets
        '
        Me.lblWorksheets.AutoSize = True
        Me.lblWorksheets.Location = New System.Drawing.Point(16, 62)
        Me.lblWorksheets.Name = "lblWorksheets"
        Me.lblWorksheets.Size = New System.Drawing.Size(67, 13)
        Me.lblWorksheets.TabIndex = 10
        Me.lblWorksheets.Text = "Worksheets:"
        '
        'btnRefreshWb
        '
        Me.btnRefreshWb.Location = New System.Drawing.Point(272, 10)
        Me.btnRefreshWb.Name = "btnRefreshWb"
        Me.btnRefreshWb.Size = New System.Drawing.Size(75, 23)
        Me.btnRefreshWb.TabIndex = 2
        Me.btnRefreshWb.Text = "Refresh"
        Me.btnRefreshWb.UseVisualStyleBackColor = True
        '
        'cboWorkbooks
        '
        Me.cboWorkbooks.DisplayMember = "Name"
        Me.cboWorkbooks.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cboWorkbooks.FormattingEnabled = True
        Me.cboWorkbooks.Location = New System.Drawing.Point(85, 12)
        Me.cboWorkbooks.Name = "cboWorkbooks"
        Me.cboWorkbooks.Size = New System.Drawing.Size(181, 21)
        Me.cboWorkbooks.TabIndex = 1
        '
        'lblWorkbooks
        '
        Me.lblWorkbooks.AutoSize = True
        Me.lblWorkbooks.Location = New System.Drawing.Point(16, 15)
        Me.lblWorkbooks.Name = "lblWorkbooks"
        Me.lblWorkbooks.Size = New System.Drawing.Size(65, 13)
        Me.lblWorkbooks.TabIndex = 7
        Me.lblWorkbooks.Text = "Workbooks:"
        '
        'GetWrapperForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(373, 144)
        Me.Controls.Add(Me.btnAddListObject)
        Me.Controls.Add(Me.btnRefreshWs)
        Me.Controls.Add(Me.cboWorksheets)
        Me.Controls.Add(Me.lblWorksheets)
        Me.Controls.Add(Me.btnRefreshWb)
        Me.Controls.Add(Me.cboWorkbooks)
        Me.Controls.Add(Me.lblWorkbooks)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "GetWrapperForm"
        Me.ShowIcon = False
        Me.ShowInTaskbar = False
        Me.Text = "GetWrapperForm"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents btnAddListObject As System.Windows.Forms.Button
    Private WithEvents btnRefreshWs As System.Windows.Forms.Button
    Private WithEvents cboWorksheets As System.Windows.Forms.ComboBox
    Private WithEvents lblWorksheets As System.Windows.Forms.Label
    Private WithEvents btnRefreshWb As System.Windows.Forms.Button
    Private WithEvents cboWorkbooks As System.Windows.Forms.ComboBox
    Private WithEvents lblWorkbooks As System.Windows.Forms.Label
End Class
