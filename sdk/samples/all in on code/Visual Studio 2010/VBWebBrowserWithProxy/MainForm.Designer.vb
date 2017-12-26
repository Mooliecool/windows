Imports System.Security.Permissions

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
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")>
    Private Sub InitializeComponent()
        Me.pnlMenu = New Panel()
        Me.cmbProxy = New ComboBox()
        Me.radUseProxy = New RadioButton()
        Me.tbUrl = New TextBox()
        Me.radNoProxy = New RadioButton()
        Me.btnNavigate = New Button()
        Me.pnlBrowser = New Panel()
        Me.pnlToolbar = New Panel()
        Me.prgBrowserProcess = New ProgressBar()
        Me.lbStatus = New Label()
        Me.pnlStatus = New Panel()
        Me.wbcSample = New VBWebBrowserWithProxy.WebBrowserWithProxy()
        Me.pnlMenu.SuspendLayout()
        Me.pnlBrowser.SuspendLayout()
        Me.pnlToolbar.SuspendLayout()
        Me.pnlStatus.SuspendLayout()
        Me.SuspendLayout()
        ' 
        ' pnlMenu
        ' 
        Me.pnlMenu.Controls.Add(Me.cmbProxy)
        Me.pnlMenu.Controls.Add(Me.radUseProxy)
        Me.pnlMenu.Controls.Add(Me.tbUrl)
        Me.pnlMenu.Controls.Add(Me.radNoProxy)
        Me.pnlMenu.Controls.Add(Me.btnNavigate)
        Me.pnlMenu.Dock = DockStyle.Top
        Me.pnlMenu.Location = New Point(0, 0)
        Me.pnlMenu.Name = "pnlMenu"
        Me.pnlMenu.Size = New Size(824, 54)
        Me.pnlMenu.TabIndex = 1
        ' 
        ' cmbProxy
        ' 
        Me.cmbProxy.DropDownStyle = ComboBoxStyle.DropDownList
        Me.cmbProxy.FormattingEnabled = True
        Me.cmbProxy.Items.AddRange(New Object() {"First Browser", "Second Browser"})
        Me.cmbProxy.Location = New Point(201, 23)
        Me.cmbProxy.Name = "cmbProxy"
        Me.cmbProxy.Size = New Size(121, 21)
        Me.cmbProxy.TabIndex = 10
        ' 
        ' radUseProxy
        ' 
        Me.radUseProxy.AutoSize = True
        Me.radUseProxy.Location = New Point(110, 25)
        Me.radUseProxy.Name = "radUseProxy"
        Me.radUseProxy.Size = New Size(85, 17)
        Me.radUseProxy.TabIndex = 6
        Me.radUseProxy.Text = "Proxy Server"
        Me.radUseProxy.UseVisualStyleBackColor = True
        ' 
        ' tbUrl
        ' 
        Me.tbUrl.Dock = DockStyle.Top
        Me.tbUrl.Location = New Point(0, 0)
        Me.tbUrl.Name = "tbUrl"
        Me.tbUrl.Size = New Size(824, 20)
        Me.tbUrl.TabIndex = 5
        Me.tbUrl.Text = "http://www.whatsmyip.us/"
        ' 
        ' radNoProxy
        ' 
        Me.radNoProxy.AutoSize = True
        Me.radNoProxy.Checked = True
        Me.radNoProxy.Location = New Point(12, 25)
        Me.radNoProxy.Name = "radNoProxy"
        Me.radNoProxy.Size = New Size(68, 17)
        Me.radNoProxy.TabIndex = 4
        Me.radNoProxy.TabStop = True
        Me.radNoProxy.Text = "No Proxy"
        Me.radNoProxy.UseVisualStyleBackColor = True
        ' 
        ' btnNavigate
        ' 
        Me.btnNavigate.Location = New Point(353, 22)
        Me.btnNavigate.Name = "btnNavigate"
        Me.btnNavigate.Size = New Size(79, 23)
        Me.btnNavigate.TabIndex = 1
        Me.btnNavigate.Text = "Go"
        Me.btnNavigate.UseVisualStyleBackColor = True
        '			Me.btnNavigate.Click += New System.EventHandler(Me.btnNavigate_Click)
        ' 
        ' pnlBrowser
        ' 
        Me.pnlBrowser.Controls.Add(Me.pnlToolbar)
        Me.pnlBrowser.Controls.Add(Me.wbcSample)
        Me.pnlBrowser.Dock = DockStyle.Fill
        Me.pnlBrowser.Location = New Point(0, 54)
        Me.pnlBrowser.Name = "pnlBrowser"
        Me.pnlBrowser.Size = New Size(824, 557)
        Me.pnlBrowser.TabIndex = 2
        ' 
        ' pnlToolbar
        ' 
        Me.pnlToolbar.Controls.Add(Me.pnlStatus)
        Me.pnlToolbar.Controls.Add(Me.prgBrowserProcess)
        Me.pnlToolbar.Dock = DockStyle.Bottom
        Me.pnlToolbar.Location = New Point(0, 530)
        Me.pnlToolbar.Name = "pnlToolbar"
        Me.pnlToolbar.Size = New Size(824, 27)
        Me.pnlToolbar.TabIndex = 1
        ' 
        ' prgBrowserProcess
        ' 
        Me.prgBrowserProcess.Dock = DockStyle.Fill
        Me.prgBrowserProcess.Location = New Point(0, 0)
        Me.prgBrowserProcess.Name = "prgBrowserProcess"
        Me.prgBrowserProcess.Size = New Size(824, 27)
        Me.prgBrowserProcess.TabIndex = 12
        ' 
        ' lbStatus
        ' 
        Me.lbStatus.AutoSize = True
        Me.lbStatus.Location = New Point(12, 10)
        Me.lbStatus.Name = "lbStatus"
        Me.lbStatus.Size = New Size(0, 13)
        Me.lbStatus.TabIndex = 13
        ' 
        ' pnlStatus
        ' 
        Me.pnlStatus.Controls.Add(Me.lbStatus)
        Me.pnlStatus.Dock = DockStyle.Left
        Me.pnlStatus.Location = New Point(0, 0)
        Me.pnlStatus.Name = "pnlStatus"
        Me.pnlStatus.Size = New Size(275, 27)
        Me.pnlStatus.TabIndex = 14
        ' 
        ' wbcSample
        ' 
        Me.wbcSample.Dock = DockStyle.Fill
        Me.wbcSample.Location = New Point(0, 0)
        Me.wbcSample.MinimumSize = New Size(20, 20)
        Me.wbcSample.Name = "wbcSample"
        Me.wbcSample.ScriptErrorsSuppressed = True
        Me.wbcSample.Size = New Size(824, 557)
        Me.wbcSample.TabIndex = 0
        '			Me.wbcSample.ProgressChanged += New System.Windows.Forms.WebBrowserProgressChangedEventHandler(Me.wbcSample_ProgressChanged)
        ' 
        ' MainForm
        ' 
        Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = AutoScaleMode.Font
        Me.ClientSize = New Size(824, 611)
        Me.Controls.Add(Me.pnlBrowser)
        Me.Controls.Add(Me.pnlMenu)
        Me.Name = "MainForm"
        Me.Text = "VBWebBrowserWithProxy"
        '			Me.Load += New System.EventHandler(Me.MainForm_Load)
        Me.pnlMenu.ResumeLayout(False)
        Me.pnlMenu.PerformLayout()
        Me.pnlBrowser.ResumeLayout(False)
        Me.pnlToolbar.ResumeLayout(False)
        Me.pnlStatus.ResumeLayout(False)
        Me.pnlStatus.PerformLayout()
        Me.ResumeLayout(False)

    End Sub


#End Region


    Private pnlMenu As Panel
    Private radUseProxy As RadioButton
    Private tbUrl As TextBox
    Private radNoProxy As RadioButton
    Private WithEvents btnNavigate As Button
    Private cmbProxy As ComboBox
    Private pnlBrowser As Panel
    Private WithEvents wbcSample As WebBrowserWithProxy
    Private pnlToolbar As Panel
    Private prgBrowserProcess As ProgressBar
    Private pnlStatus As Panel
    Private lbStatus As Label
End Class


