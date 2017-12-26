
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
        Me.pnlMenu = New System.Windows.Forms.Panel()
        Me.chkSuppressAllDialog = New System.Windows.Forms.CheckBox()
        Me.chkSuppressNavigationError = New System.Windows.Forms.CheckBox()
        Me.tbUrl = New System.Windows.Forms.TextBox()
        Me.chkSuppressJITDebugger = New System.Windows.Forms.CheckBox()
        Me.chkSuppressHtmlElementError = New System.Windows.Forms.CheckBox()
        Me.btnNavigate = New System.Windows.Forms.Button()
        Me.lbUrl = New System.Windows.Forms.Label()
        Me.pnlBrowser = New System.Windows.Forms.Panel()
        Me.wbcSample = New VBWebBrowserSuppressError.WebBrowserEx()
        Me.pnlMenu.SuspendLayout()
        Me.pnlBrowser.SuspendLayout()
        Me.SuspendLayout()
        '
        'pnlMenu
        '
        Me.pnlMenu.Controls.Add(Me.chkSuppressAllDialog)
        Me.pnlMenu.Controls.Add(Me.chkSuppressNavigationError)
        Me.pnlMenu.Controls.Add(Me.tbUrl)
        Me.pnlMenu.Controls.Add(Me.chkSuppressJITDebugger)
        Me.pnlMenu.Controls.Add(Me.chkSuppressHtmlElementError)
        Me.pnlMenu.Controls.Add(Me.btnNavigate)
        Me.pnlMenu.Controls.Add(Me.lbUrl)
        Me.pnlMenu.Dock = System.Windows.Forms.DockStyle.Top
        Me.pnlMenu.Location = New System.Drawing.Point(0, 0)
        Me.pnlMenu.Name = "pnlMenu"
        Me.pnlMenu.Size = New System.Drawing.Size(824, 73)
        Me.pnlMenu.TabIndex = 1
        '
        'chkSuppressAllDialog
        '
        Me.chkSuppressAllDialog.AutoSize = True
        Me.chkSuppressAllDialog.Location = New System.Drawing.Point(185, 44)
        Me.chkSuppressAllDialog.Name = "chkSuppressAllDialog"
        Me.chkSuppressAllDialog.Size = New System.Drawing.Size(119, 17)
        Me.chkSuppressAllDialog.TabIndex = 7
        Me.chkSuppressAllDialog.Text = "Suppress all dialogs"
        Me.chkSuppressAllDialog.UseVisualStyleBackColor = True
        '
        'chkSuppressNavigationError
        '
        Me.chkSuppressNavigationError.AutoSize = True
        Me.chkSuppressNavigationError.Location = New System.Drawing.Point(314, 44)
        Me.chkSuppressNavigationError.Name = "chkSuppressNavigationError"
        Me.chkSuppressNavigationError.Size = New System.Drawing.Size(149, 17)
        Me.chkSuppressNavigationError.TabIndex = 6
        Me.chkSuppressNavigationError.Text = "Suppress Navigation Error"
        Me.chkSuppressNavigationError.UseVisualStyleBackColor = True
        '
        'tbUrl
        '
        Me.tbUrl.Dock = System.Windows.Forms.DockStyle.Top
        Me.tbUrl.Location = New System.Drawing.Point(0, 13)
        Me.tbUrl.Name = "tbUrl"
        Me.tbUrl.Size = New System.Drawing.Size(824, 20)
        Me.tbUrl.TabIndex = 5
        '
        'chkSuppressJITDebugger
        '
        Me.chkSuppressJITDebugger.AutoSize = True
        Me.chkSuppressJITDebugger.Location = New System.Drawing.Point(469, 44)
        Me.chkSuppressJITDebugger.Name = "chkSuppressJITDebugger"
        Me.chkSuppressJITDebugger.Size = New System.Drawing.Size(138, 17)
        Me.chkSuppressJITDebugger.TabIndex = 2
        Me.chkSuppressJITDebugger.Text = "Suppress JIT Debugger"
        Me.chkSuppressJITDebugger.UseVisualStyleBackColor = True
        '
        'chkSuppressHtmlElementError
        '
        Me.chkSuppressHtmlElementError.AutoSize = True
        Me.chkSuppressHtmlElementError.Location = New System.Drawing.Point(12, 44)
        Me.chkSuppressHtmlElementError.Name = "chkSuppressHtmlElementError"
        Me.chkSuppressHtmlElementError.Size = New System.Drawing.Size(165, 17)
        Me.chkSuppressHtmlElementError.TabIndex = 2
        Me.chkSuppressHtmlElementError.Text = "Suppress Html Element Errors"
        Me.chkSuppressHtmlElementError.UseVisualStyleBackColor = True
        '
        'btnNavigate
        '
        Me.btnNavigate.Location = New System.Drawing.Point(733, 43)
        Me.btnNavigate.Name = "btnNavigate"
        Me.btnNavigate.Size = New System.Drawing.Size(79, 23)
        Me.btnNavigate.TabIndex = 1
        Me.btnNavigate.Text = "Go"
        Me.btnNavigate.UseVisualStyleBackColor = True
        '
        'lbUrl
        '
        Me.lbUrl.AutoSize = True
        Me.lbUrl.Dock = System.Windows.Forms.DockStyle.Top
        Me.lbUrl.Location = New System.Drawing.Point(0, 0)
        Me.lbUrl.Name = "lbUrl"
        Me.lbUrl.Size = New System.Drawing.Size(225, 13)
        Me.lbUrl.TabIndex = 8
        Me.lbUrl.Text = "URL (leave blank to load the internal test html)"
        '
        'pnlBrowser
        '
        Me.pnlBrowser.Controls.Add(Me.wbcSample)
        Me.pnlBrowser.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnlBrowser.Location = New System.Drawing.Point(0, 73)
        Me.pnlBrowser.Name = "pnlBrowser"
        Me.pnlBrowser.Size = New System.Drawing.Size(824, 538)
        Me.pnlBrowser.TabIndex = 2
        '
        'wbcSample
        '
        Me.wbcSample.Dock = System.Windows.Forms.DockStyle.Fill
        Me.wbcSample.HtmlElementErrorsSuppressed = False
        Me.wbcSample.Location = New System.Drawing.Point(0, 0)
        Me.wbcSample.MinimumSize = New System.Drawing.Size(20, 20)
        Me.wbcSample.Name = "wbcSample"
        Me.wbcSample.Size = New System.Drawing.Size(824, 538)
        Me.wbcSample.TabIndex = 0
        '
        'MainForm
        '
        Me.AcceptButton = Me.btnNavigate
        Me.ClientSize = New System.Drawing.Size(824, 611)
        Me.Controls.Add(Me.pnlBrowser)
        Me.Controls.Add(Me.pnlMenu)
        Me.Name = "MainForm"
        Me.Text = "VBWebBrowserSuppressError"
        Me.pnlMenu.ResumeLayout(False)
        Me.pnlMenu.PerformLayout()
        Me.pnlBrowser.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub


#End Region


    Private pnlMenu As Panel
    Private tbUrl As TextBox
    Private WithEvents chkSuppressHtmlElementError As CheckBox
    Private WithEvents btnNavigate As Button
    Private chkSuppressJITDebugger As CheckBox
    Private pnlBrowser As Panel
    Private wbcSample As WebBrowserEx
    Private chkSuppressNavigationError As CheckBox
    Private WithEvents chkSuppressAllDialog As CheckBox
    Friend WithEvents lbUrl As System.Windows.Forms.Label
End Class


