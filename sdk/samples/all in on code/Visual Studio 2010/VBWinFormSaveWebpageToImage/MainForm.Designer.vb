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
        Me.webBrowserTargetPage = New System.Windows.Forms.WebBrowser()
        Me.groupBox1 = New System.Windows.Forms.GroupBox()
        Me.pctPreview = New System.Windows.Forms.PictureBox()
        Me.tbHeight = New System.Windows.Forms.TextBox()
        Me.tbWidth = New System.Windows.Forms.TextBox()
        Me.lbHeight = New System.Windows.Forms.Label()
        Me.lbWidth = New System.Windows.Forms.Label()
        Me.btnSavePage = New System.Windows.Forms.Button()
        Me.tbUrl = New System.Windows.Forms.TextBox()
        Me.btnOpen = New System.Windows.Forms.Button()
        Me.groupBox1.SuspendLayout()
        CType(Me.pctPreview, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'webBrowserTargetPage
        '
        Me.webBrowserTargetPage.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.webBrowserTargetPage.Location = New System.Drawing.Point(0, 0)
        Me.webBrowserTargetPage.MinimumSize = New System.Drawing.Size(20, 20)
        Me.webBrowserTargetPage.Name = "webBrowserTargetPage"
        Me.webBrowserTargetPage.Size = New System.Drawing.Size(755, 348)
        Me.webBrowserTargetPage.TabIndex = 1
        '
        'groupBox1
        '
        Me.groupBox1.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.groupBox1.Controls.Add(Me.pctPreview)
        Me.groupBox1.Location = New System.Drawing.Point(0, 420)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(755, 376)
        Me.groupBox1.TabIndex = 7
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Image Preview"
        '
        'pctPreview
        '
        Me.pctPreview.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pctPreview.Location = New System.Drawing.Point(3, 16)
        Me.pctPreview.Name = "pctPreview"
        Me.pctPreview.Size = New System.Drawing.Size(749, 357)
        Me.pctPreview.TabIndex = 0
        Me.pctPreview.TabStop = False
        '
        'tbHeight
        '
        Me.tbHeight.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tbHeight.Location = New System.Drawing.Point(549, 373)
        Me.tbHeight.Name = "tbHeight"
        Me.tbHeight.Size = New System.Drawing.Size(100, 20)
        Me.tbHeight.TabIndex = 12
        Me.tbHeight.Text = "900"
        '
        'tbWidth
        '
        Me.tbWidth.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tbWidth.Location = New System.Drawing.Point(393, 373)
        Me.tbWidth.Name = "tbWidth"
        Me.tbWidth.Size = New System.Drawing.Size(100, 20)
        Me.tbWidth.TabIndex = 11
        Me.tbWidth.Text = "1440"
        '
        'lbHeight
        '
        Me.lbHeight.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lbHeight.AutoSize = True
        Me.lbHeight.Location = New System.Drawing.Point(499, 376)
        Me.lbHeight.Name = "lbHeight"
        Me.lbHeight.Size = New System.Drawing.Size(38, 13)
        Me.lbHeight.TabIndex = 10
        Me.lbHeight.Text = "Height"
        '
        'lbWidth
        '
        Me.lbWidth.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lbWidth.AutoSize = True
        Me.lbWidth.Location = New System.Drawing.Point(342, 376)
        Me.lbWidth.Name = "lbWidth"
        Me.lbWidth.Size = New System.Drawing.Size(35, 13)
        Me.lbWidth.TabIndex = 9
        Me.lbWidth.Text = "Width"
        '
        'btnSavePage
        '
        Me.btnSavePage.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnSavePage.Location = New System.Drawing.Point(658, 371)
        Me.btnSavePage.Name = "btnSavePage"
        Me.btnSavePage.Size = New System.Drawing.Size(75, 23)
        Me.btnSavePage.TabIndex = 8
        Me.btnSavePage.Text = "Save Page"
        Me.btnSavePage.UseVisualStyleBackColor = True
        '
        'tbUrl
        '
        Me.tbUrl.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tbUrl.Location = New System.Drawing.Point(5, 371)
        Me.tbUrl.Name = "tbUrl"
        Me.tbUrl.Size = New System.Drawing.Size(201, 20)
        Me.tbUrl.TabIndex = 13
        Me.tbUrl.Text = "Http://"
        '
        'btnOpen
        '
        Me.btnOpen.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnOpen.Location = New System.Drawing.Point(212, 369)
        Me.btnOpen.Name = "btnOpen"
        Me.btnOpen.Size = New System.Drawing.Size(77, 23)
        Me.btnOpen.TabIndex = 14
        Me.btnOpen.Text = "Open URL"
        Me.btnOpen.UseVisualStyleBackColor = True
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(755, 782)
        Me.Controls.Add(Me.btnOpen)
        Me.Controls.Add(Me.tbUrl)
        Me.Controls.Add(Me.tbHeight)
        Me.Controls.Add(Me.tbWidth)
        Me.Controls.Add(Me.lbHeight)
        Me.Controls.Add(Me.lbWidth)
        Me.Controls.Add(Me.btnSavePage)
        Me.Controls.Add(Me.groupBox1)
        Me.Controls.Add(Me.webBrowserTargetPage)
        Me.Name = "MainForm"
        Me.Text = "VBWinFormSaveWebpageToImage"
        Me.groupBox1.ResumeLayout(False)
        CType(Me.pctPreview, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents webBrowserTargetPage As System.Windows.Forms.WebBrowser
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents pctPreview As System.Windows.Forms.PictureBox
    Private WithEvents tbHeight As System.Windows.Forms.TextBox
    Private WithEvents tbWidth As System.Windows.Forms.TextBox
    Private WithEvents lbHeight As System.Windows.Forms.Label
    Private WithEvents lbWidth As System.Windows.Forms.Label
    Private WithEvents btnSavePage As System.Windows.Forms.Button
    Friend WithEvents tbUrl As System.Windows.Forms.TextBox
    Private WithEvents btnOpen As System.Windows.Forms.Button

End Class
