
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
        Me.btnGo = New System.Windows.Forms.Button()
        Me.pnlNavigate = New System.Windows.Forms.Panel()
        Me.lbUrl = New System.Windows.Forms.Label()
        Me.btnAutoComplete = New System.Windows.Forms.Button()
        Me.webBrowser = New VBWebBrowserAutomation.WebBrowserEx()
        Me.pnlHeader.SuspendLayout()
        Me.pnlNavigate.SuspendLayout()
        Me.SuspendLayout()
        '
        'pnlHeader
        '
        Me.pnlHeader.Controls.Add(Me.tbUrl)
        Me.pnlHeader.Controls.Add(Me.btnGo)
        Me.pnlHeader.Controls.Add(Me.pnlNavigate)
        Me.pnlHeader.Dock = System.Windows.Forms.DockStyle.Top
        Me.pnlHeader.Location = New System.Drawing.Point(0, 0)
        Me.pnlHeader.Name = "pnlHeader"
        Me.pnlHeader.Size = New System.Drawing.Size(1070, 32)
        Me.pnlHeader.TabIndex = 0
        '
        'tbUrl
        '
        Me.tbUrl.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tbUrl.Font = New System.Drawing.Font("Microsoft Sans Serif", 13.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(134, Byte))
        Me.tbUrl.Location = New System.Drawing.Point(152, 0)
        Me.tbUrl.Name = "tbUrl"
        Me.tbUrl.Size = New System.Drawing.Size(843, 27)
        Me.tbUrl.TabIndex = 3
        Me.tbUrl.Text = "https://www.codeplex.com/site/login?RedirectUrl=https%3a%2f%2fwww.codeplex.com%2f" & _
            "site%2fusers%2fupdate"
        '
        'btnGo
        '
        Me.btnGo.Dock = System.Windows.Forms.DockStyle.Right
        Me.btnGo.Location = New System.Drawing.Point(995, 0)
        Me.btnGo.Name = "btnGo"
        Me.btnGo.Size = New System.Drawing.Size(75, 32)
        Me.btnGo.TabIndex = 5
        Me.btnGo.Text = "Go"
        Me.btnGo.UseVisualStyleBackColor = True
        '
        'pnlNavigate
        '
        Me.pnlNavigate.Controls.Add(Me.lbUrl)
        Me.pnlNavigate.Controls.Add(Me.btnAutoComplete)
        Me.pnlNavigate.Dock = System.Windows.Forms.DockStyle.Left
        Me.pnlNavigate.Location = New System.Drawing.Point(0, 0)
        Me.pnlNavigate.Name = "pnlNavigate"
        Me.pnlNavigate.Size = New System.Drawing.Size(152, 32)
        Me.pnlNavigate.TabIndex = 1
        '
        'lbUrl
        '
        Me.lbUrl.AutoSize = True
        Me.lbUrl.Location = New System.Drawing.Point(128, 9)
        Me.lbUrl.Name = "lbUrl"
        Me.lbUrl.Size = New System.Drawing.Size(20, 13)
        Me.lbUrl.TabIndex = 1
        Me.lbUrl.Text = "Url"
        '
        'btnAutoComplete
        '
        Me.btnAutoComplete.Enabled = False
        Me.btnAutoComplete.Location = New System.Drawing.Point(12, 4)
        Me.btnAutoComplete.Name = "btnAutoComplete"
        Me.btnAutoComplete.Size = New System.Drawing.Size(110, 23)
        Me.btnAutoComplete.TabIndex = 0
        Me.btnAutoComplete.Text = "Auto Complete"
        Me.btnAutoComplete.UseVisualStyleBackColor = True
        '
        'webBrowser
        '
        Me.webBrowser.Dock = System.Windows.Forms.DockStyle.Fill
        Me.webBrowser.Location = New System.Drawing.Point(0, 32)
        Me.webBrowser.MinimumSize = New System.Drawing.Size(20, 20)
        Me.webBrowser.Name = "webBrowser"
        Me.webBrowser.Size = New System.Drawing.Size(1070, 483)
        Me.webBrowser.TabIndex = 1
        '
        'MainForm
        '
        Me.ClientSize = New System.Drawing.Size(1070, 515)
        Me.Controls.Add(Me.webBrowser)
        Me.Controls.Add(Me.pnlHeader)
        Me.Name = "MainForm"
        Me.Text = "WebBrowserAutomation"
        Me.pnlHeader.ResumeLayout(False)
        Me.pnlHeader.PerformLayout()
        Me.pnlNavigate.ResumeLayout(False)
        Me.pnlNavigate.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private pnlHeader As Panel
    Private WithEvents tbUrl As TextBox
    Private pnlNavigate As Panel
    Private lbUrl As Label
    Private webBrowser As WebBrowserEx
    Private WithEvents btnAutoComplete As Button
    Private WithEvents btnGo As System.Windows.Forms.Button
End Class

