
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
        Me.panel1 = New System.Windows.Forms.Panel()
        Me.tbURL = New System.Windows.Forms.TextBox()
        Me.btnGo = New System.Windows.Forms.Button()
        Me.stsLoad = New System.Windows.Forms.StatusStrip()
        Me.lbStatus = New System.Windows.Forms.ToolStripStatusLabel()
        Me.webEx = New VBWebBrowserLoadComplete.WebBrowserEx()
        Me.lstActivities = New System.Windows.Forms.ListBox()
        Me.panel1.SuspendLayout()
        Me.stsLoad.SuspendLayout()
        Me.SuspendLayout()
        '
        'panel1
        '
        Me.panel1.Controls.Add(Me.tbURL)
        Me.panel1.Controls.Add(Me.btnGo)
        Me.panel1.Dock = System.Windows.Forms.DockStyle.Top
        Me.panel1.Location = New System.Drawing.Point(0, 0)
        Me.panel1.Name = "panel1"
        Me.panel1.Size = New System.Drawing.Size(710, 21)
        Me.panel1.TabIndex = 0
        '
        'tbURL
        '
        Me.tbURL.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tbURL.Location = New System.Drawing.Point(0, 0)
        Me.tbURL.Name = "tbURL"
        Me.tbURL.Size = New System.Drawing.Size(635, 20)
        Me.tbURL.TabIndex = 2
        Me.tbURL.Text = "http://msdn.microsoft.com"
        '
        'btnGo
        '
        Me.btnGo.Dock = System.Windows.Forms.DockStyle.Right
        Me.btnGo.Location = New System.Drawing.Point(635, 0)
        Me.btnGo.Name = "btnGo"
        Me.btnGo.Size = New System.Drawing.Size(75, 21)
        Me.btnGo.TabIndex = 1
        Me.btnGo.Text = "GO"
        Me.btnGo.UseVisualStyleBackColor = True
        '
        'stsLoad
        '
        Me.stsLoad.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.lbStatus})
        Me.stsLoad.Location = New System.Drawing.Point(0, 432)
        Me.stsLoad.Name = "stsLoad"
        Me.stsLoad.Size = New System.Drawing.Size(710, 22)
        Me.stsLoad.TabIndex = 1
        '
        'lbStatus
        '
        Me.lbStatus.Name = "lbStatus"
        Me.lbStatus.Size = New System.Drawing.Size(0, 17)
        '
        'webEx
        '
        Me.webEx.Dock = System.Windows.Forms.DockStyle.Fill
        Me.webEx.Location = New System.Drawing.Point(0, 21)
        Me.webEx.MinimumSize = New System.Drawing.Size(20, 20)
        Me.webEx.Name = "webEx"
        Me.webEx.Size = New System.Drawing.Size(710, 277)
        Me.webEx.TabIndex = 2
        '
        'lstActivities
        '
        Me.lstActivities.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.lstActivities.FormattingEnabled = True
        Me.lstActivities.Location = New System.Drawing.Point(0, 298)
        Me.lstActivities.Name = "lstActivities"
        Me.lstActivities.Size = New System.Drawing.Size(710, 134)
        Me.lstActivities.TabIndex = 4
        '
        'MainForm
        '
        Me.ClientSize = New System.Drawing.Size(710, 454)
        Me.Controls.Add(Me.webEx)
        Me.Controls.Add(Me.lstActivities)
        Me.Controls.Add(Me.stsLoad)
        Me.Controls.Add(Me.panel1)
        Me.Name = "MainForm"
        Me.Text = "VBWebBrowserLoadComplete"
        Me.panel1.ResumeLayout(False)
        Me.panel1.PerformLayout()
        Me.stsLoad.ResumeLayout(False)
        Me.stsLoad.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

#End Region

    Private panel1 As Panel
    Private tbURL As TextBox
    Private WithEvents btnGo As Button
    Private stsLoad As StatusStrip
    Private lbStatus As ToolStripStatusLabel
    Private webEx As WebBrowserEx
    Private WithEvents lstActivities As System.Windows.Forms.ListBox
End Class


