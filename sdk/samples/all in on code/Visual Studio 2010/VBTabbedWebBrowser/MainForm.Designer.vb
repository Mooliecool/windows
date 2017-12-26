
Partial Public Class MainForm
    ''' <summary>
    ''' Required designer variable.
    ''' </summary>
    Private components As System.ComponentModel.IContainer = Nothing

    ''' <summary>
    ''' Clean up any resources being used.
    ''' </summary>
    ''' <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso (components IsNot Nothing) Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

#Region "Windows Form Designer generated code"

    ''' <summary>
    ''' Required method for Designer support - do not modify
    ''' the contents of this method with the code editor.
    ''' </summary>
    Private Sub InitializeComponent()
        Me.pnlHeader = New System.Windows.Forms.Panel()
        Me.tbUrl = New System.Windows.Forms.TextBox()
        Me.pnlNavigate = New System.Windows.Forms.Panel()
        Me.lbUrl = New System.Windows.Forms.Label()
        Me.btnRefresh = New System.Windows.Forms.Button()
        Me.btnForward = New System.Windows.Forms.Button()
        Me.btnBack = New System.Windows.Forms.Button()
        Me.pnlTabCommand = New System.Windows.Forms.Panel()
        Me.chkEnableTab = New System.Windows.Forms.CheckBox()
        Me.btnCloseTab = New System.Windows.Forms.Button()
        Me.btnNewTab = New System.Windows.Forms.Button()
        Me.webBrowserContainer = New VBTabbedWebBrowser.TabbedWebBrowserContainer()
        Me.lbGo = New System.Windows.Forms.Label()
        Me.pnlHeader.SuspendLayout()
        Me.pnlNavigate.SuspendLayout()
        Me.pnlTabCommand.SuspendLayout()
        Me.SuspendLayout()
        '
        'pnlHeader
        '
        Me.pnlHeader.Controls.Add(Me.tbUrl)
        Me.pnlHeader.Controls.Add(Me.pnlNavigate)
        Me.pnlHeader.Controls.Add(Me.pnlTabCommand)
        Me.pnlHeader.Dock = System.Windows.Forms.DockStyle.Top
        Me.pnlHeader.Location = New System.Drawing.Point(0, 0)
        Me.pnlHeader.Name = "pnlHeader"
        Me.pnlHeader.Size = New System.Drawing.Size(928, 30)
        Me.pnlHeader.TabIndex = 0
        '
        'tbUrl
        '
        Me.tbUrl.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tbUrl.Font = New System.Drawing.Font("Microsoft Sans Serif", 13.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(134, Byte))
        Me.tbUrl.Location = New System.Drawing.Point(206, 0)
        Me.tbUrl.Name = "tbUrl"
        Me.tbUrl.Size = New System.Drawing.Size(313, 27)
        Me.tbUrl.TabIndex = 2
        Me.tbUrl.Text = "http://1code.codeplex.com/"
        '
        'pnlNavigate
        '
        Me.pnlNavigate.Controls.Add(Me.lbUrl)
        Me.pnlNavigate.Controls.Add(Me.btnRefresh)
        Me.pnlNavigate.Controls.Add(Me.btnForward)
        Me.pnlNavigate.Controls.Add(Me.btnBack)
        Me.pnlNavigate.Dock = System.Windows.Forms.DockStyle.Left
        Me.pnlNavigate.Location = New System.Drawing.Point(0, 0)
        Me.pnlNavigate.Name = "pnlNavigate"
        Me.pnlNavigate.Size = New System.Drawing.Size(206, 30)
        Me.pnlNavigate.TabIndex = 0
        '
        'lbUrl
        '
        Me.lbUrl.AutoSize = True
        Me.lbUrl.Location = New System.Drawing.Point(183, 9)
        Me.lbUrl.Name = "lbUrl"
        Me.lbUrl.Size = New System.Drawing.Size(20, 13)
        Me.lbUrl.TabIndex = 1
        Me.lbUrl.Text = "Url"
        '
        'btnRefresh
        '
        Me.btnRefresh.Location = New System.Drawing.Point(122, 3)
        Me.btnRefresh.Name = "btnRefresh"
        Me.btnRefresh.Size = New System.Drawing.Size(55, 23)
        Me.btnRefresh.TabIndex = 0
        Me.btnRefresh.Text = "Refresh"
        Me.btnRefresh.UseVisualStyleBackColor = True
        '
        'btnForward
        '
        Me.btnForward.Location = New System.Drawing.Point(62, 4)
        Me.btnForward.Name = "btnForward"
        Me.btnForward.Size = New System.Drawing.Size(54, 23)
        Me.btnForward.TabIndex = 0
        Me.btnForward.Text = "Forward"
        Me.btnForward.UseVisualStyleBackColor = True
        '
        'btnBack
        '
        Me.btnBack.Location = New System.Drawing.Point(3, 4)
        Me.btnBack.Name = "btnBack"
        Me.btnBack.Size = New System.Drawing.Size(53, 23)
        Me.btnBack.TabIndex = 0
        Me.btnBack.Text = "Back"
        Me.btnBack.UseVisualStyleBackColor = True
        '
        'pnlTabCommand
        '
        Me.pnlTabCommand.Controls.Add(Me.lbGo)
        Me.pnlTabCommand.Controls.Add(Me.chkEnableTab)
        Me.pnlTabCommand.Controls.Add(Me.btnCloseTab)
        Me.pnlTabCommand.Controls.Add(Me.btnNewTab)
        Me.pnlTabCommand.Dock = System.Windows.Forms.DockStyle.Right
        Me.pnlTabCommand.Location = New System.Drawing.Point(519, 0)
        Me.pnlTabCommand.Name = "pnlTabCommand"
        Me.pnlTabCommand.Size = New System.Drawing.Size(409, 30)
        Me.pnlTabCommand.TabIndex = 3
        '
        'chkEnableTab
        '
        Me.chkEnableTab.AutoSize = True
        Me.chkEnableTab.Location = New System.Drawing.Point(160, 8)
        Me.chkEnableTab.Name = "chkEnableTab"
        Me.chkEnableTab.Size = New System.Drawing.Size(81, 17)
        Me.chkEnableTab.TabIndex = 1
        Me.chkEnableTab.Text = "Enable Tab"
        Me.chkEnableTab.UseVisualStyleBackColor = True
        '
        'btnCloseTab
        '
        Me.btnCloseTab.Location = New System.Drawing.Point(328, 4)
        Me.btnCloseTab.Name = "btnCloseTab"
        Me.btnCloseTab.Size = New System.Drawing.Size(75, 23)
        Me.btnCloseTab.TabIndex = 0
        Me.btnCloseTab.Text = "Close Tab"
        Me.btnCloseTab.UseVisualStyleBackColor = True
        '
        'btnNewTab
        '
        Me.btnNewTab.Location = New System.Drawing.Point(247, 4)
        Me.btnNewTab.Name = "btnNewTab"
        Me.btnNewTab.Size = New System.Drawing.Size(75, 23)
        Me.btnNewTab.TabIndex = 0
        Me.btnNewTab.Text = "New Tab"
        Me.btnNewTab.UseVisualStyleBackColor = True
        '
        'webBrowserContainer
        '
        Me.webBrowserContainer.Dock = System.Windows.Forms.DockStyle.Fill
        Me.webBrowserContainer.Location = New System.Drawing.Point(0, 30)
        Me.webBrowserContainer.Name = "webBrowserContainer"
        Me.webBrowserContainer.Size = New System.Drawing.Size(928, 539)
        Me.webBrowserContainer.TabIndex = 1
        '
        'lbGo
        '
        Me.lbGo.AutoSize = True
        Me.lbGo.Location = New System.Drawing.Point(7, 8)
        Me.lbGo.Name = "lbGo"
        Me.lbGo.Size = New System.Drawing.Size(137, 13)
        Me.lbGo.TabIndex = 2
        Me.lbGo.Text = "Press Enter to visit the URL"
        '
        'MainForm
        '
        Me.ClientSize = New System.Drawing.Size(928, 569)
        Me.Controls.Add(Me.webBrowserContainer)
        Me.Controls.Add(Me.pnlHeader)
        Me.Name = "MainForm"
        Me.Text = "TabbedWebBrowser"
        Me.pnlHeader.ResumeLayout(False)
        Me.pnlHeader.PerformLayout()
        Me.pnlNavigate.ResumeLayout(False)
        Me.pnlNavigate.PerformLayout()
        Me.pnlTabCommand.ResumeLayout(False)
        Me.pnlTabCommand.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private pnlHeader As Panel
    Private WithEvents tbUrl As TextBox
    Private pnlNavigate As Panel
    Private lbUrl As Label
    Private WithEvents btnRefresh As Button
    Private WithEvents btnForward As Button
    Private WithEvents btnBack As Button
    Private webBrowserContainer As TabbedWebBrowserContainer
    Private pnlTabCommand As Panel
    Private WithEvents btnCloseTab As Button
    Private WithEvents btnNewTab As Button
    Private chkEnableTab As CheckBox
    Friend WithEvents lbGo As System.Windows.Forms.Label
End Class

