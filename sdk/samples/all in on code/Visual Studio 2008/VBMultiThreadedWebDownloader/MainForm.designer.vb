
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
        Me.prgDownload = New ProgressBar()
        Me.lbUrl = New Label()
        Me.tbURL = New TextBox()
        Me.btnDownload = New Button()
        Me.lbPath = New Label()
        Me.tbPath = New TextBox()
        Me.btnCancel = New Button()
        Me.lbStatus = New Label()
        Me.btnPause = New Button()
        Me.lbSummary = New Label()
        Me.btnCheck = New Button()
        Me.SuspendLayout()
        ' 
        ' prgDownload
        ' 
        Me.prgDownload.Dock = DockStyle.Bottom
        Me.prgDownload.Location = New Point(0, 142)
        Me.prgDownload.Name = "prgDownload"
        Me.prgDownload.Size = New Size(707, 23)
        Me.prgDownload.TabIndex = 0
        ' 
        ' lbUrl
        ' 
        Me.lbUrl.AutoSize = True
        Me.lbUrl.Location = New Point(12, 40)
        Me.lbUrl.Name = "lbUrl"
        Me.lbUrl.Size = New Size(29, 13)
        Me.lbUrl.TabIndex = 1
        Me.lbUrl.Text = "URL"
        ' 
        ' tbURL
        ' 
        Me.tbURL.Location = New Point(75, 36)
        Me.tbURL.Name = "tbURL"
        Me.tbURL.Size = New Size(540, 20)
        Me.tbURL.TabIndex = 2
        Me.tbURL.Text = "http://download.microsoft.com/download/9/5/A/95A9616B-7A37-4AF6-BC36-D6EA96C8DAAE" & "/dotNetFx40_Full_x86_x64.exe"
        ' 
        ' btnDownload
        ' 
        Me.btnDownload.Enabled = False
        Me.btnDownload.Location = New Point(457, 88)
        Me.btnDownload.Name = "btnDownload"
        Me.btnDownload.Size = New Size(75, 23)
        Me.btnDownload.TabIndex = 3
        Me.btnDownload.Text = "Download"
        Me.btnDownload.UseVisualStyleBackColor = True
        '			Me.btnDownload.Click += New System.EventHandler(Me.btnDownload_Click)
        ' 
        ' lbPath
        ' 
        Me.lbPath.AutoSize = True
        Me.lbPath.Location = New Point(12, 66)
        Me.lbPath.Name = "lbPath"
        Me.lbPath.Size = New Size(58, 13)
        Me.lbPath.TabIndex = 1
        Me.lbPath.Text = "Local Path"
        ' 
        ' tbPath
        ' 
        Me.tbPath.Enabled = False
        Me.tbPath.Location = New Point(76, 62)
        Me.tbPath.Name = "tbPath"
        Me.tbPath.Size = New Size(622, 20)
        Me.tbPath.TabIndex = 2
        Me.tbPath.Text = "D:\DotNetFx4.exe"
        ' 
        ' btnCancel
        ' 
        Me.btnCancel.Location = New Point(623, 88)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New Size(75, 23)
        Me.btnCancel.TabIndex = 3
        Me.btnCancel.Text = "Cancel"
        Me.btnCancel.UseVisualStyleBackColor = True
        '			Me.btnCancel.Click += New System.EventHandler(Me.btnCancel_Click)
        ' 
        ' lbStatus
        ' 
        Me.lbStatus.AutoSize = True
        Me.lbStatus.Location = New Point(12, 131)
        Me.lbStatus.Name = "lbStatus"
        Me.lbStatus.Size = New Size(0, 13)
        Me.lbStatus.TabIndex = 1
        ' 
        ' btnPause
        ' 
        Me.btnPause.Enabled = False
        Me.btnPause.Location = New Point(538, 88)
        Me.btnPause.Name = "btnPause"
        Me.btnPause.Size = New Size(75, 23)
        Me.btnPause.TabIndex = 4
        Me.btnPause.Text = "Pause"
        Me.btnPause.UseVisualStyleBackColor = True
        '			Me.btnPause.Click += New System.EventHandler(Me.btnPause_Click)
        ' 
        ' lbSummary
        ' 
        Me.lbSummary.AutoSize = True
        Me.lbSummary.Location = New Point(114, 131)
        Me.lbSummary.Name = "lbSummary"
        Me.lbSummary.Size = New Size(0, 13)
        Me.lbSummary.TabIndex = 1
        ' 
        ' btnCheck
        ' 
        Me.btnCheck.Location = New Point(623, 33)
        Me.btnCheck.Name = "btnCheck"
        Me.btnCheck.Size = New Size(75, 23)
        Me.btnCheck.TabIndex = 5
        Me.btnCheck.Text = "Check Url"
        Me.btnCheck.UseVisualStyleBackColor = True
        '			Me.btnCheck.Click += New System.EventHandler(Me.btnCheck_Click)
        ' 
        ' MainForm
        ' 
        Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = AutoScaleMode.Font
        Me.ClientSize = New Size(707, 165)
        Me.Controls.Add(Me.btnCheck)
        Me.Controls.Add(Me.btnPause)
        Me.Controls.Add(Me.btnCancel)
        Me.Controls.Add(Me.btnDownload)
        Me.Controls.Add(Me.tbPath)
        Me.Controls.Add(Me.tbURL)
        Me.Controls.Add(Me.lbSummary)
        Me.Controls.Add(Me.lbStatus)
        Me.Controls.Add(Me.lbPath)
        Me.Controls.Add(Me.lbUrl)
        Me.Controls.Add(Me.prgDownload)
        Me.Name = "MainForm"
        Me.Text = "VBMultiThreadedWebDownloader"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

#End Region

    Private prgDownload As ProgressBar
    Private lbUrl As Label
    Private tbURL As TextBox
    Private WithEvents btnDownload As Button
    Private lbPath As Label
    Private tbPath As TextBox
    Private WithEvents btnCancel As Button
    Private lbStatus As Label
    Private WithEvents btnPause As Button
    Private lbSummary As Label
    Private WithEvents btnCheck As Button
End Class


