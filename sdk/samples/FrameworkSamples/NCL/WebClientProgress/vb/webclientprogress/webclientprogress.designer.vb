Partial Public Class WebClientProgressForm
    Inherits System.Windows.Forms.Form

    <System.Diagnostics.DebuggerNonUserCode()> _
    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

    End Sub

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(WebClientProgressForm))
        Me.downloadUrlLabel = New System.Windows.Forms.Label
        Me.urlTextBox = New System.Windows.Forms.TextBox
        Me.downloadButton = New System.Windows.Forms.Button
        Me.progressBarPanel = New System.Windows.Forms.Panel
        Me.downloadProgressBar = New System.Windows.Forms.ProgressBar
        Me.webClient = New System.Net.WebClient
        Me.progressBarPanel.SuspendLayout()
        Me.SuspendLayout()
        '
        'downloadUrlLabel
        '
        Me.downloadUrlLabel.Location = New System.Drawing.Point(18, 24)
        Me.downloadUrlLabel.Name = "downloadUrlLabel"
        Me.downloadUrlLabel.Size = New System.Drawing.Size(86, 18)
        Me.downloadUrlLabel.TabIndex = 0
        Me.downloadUrlLabel.Text = "Download URL:"
        '
        'urlTextBox
        '
        Me.urlTextBox.Location = New System.Drawing.Point(104, 24)
        Me.urlTextBox.Name = "urlTextBox"
        Me.urlTextBox.Size = New System.Drawing.Size(247, 20)
        Me.urlTextBox.TabIndex = 1
        '
        'downloadButton
        '
        Me.downloadButton.Location = New System.Drawing.Point(367, 21)
        Me.downloadButton.Name = "downloadButton"
        Me.downloadButton.Size = New System.Drawing.Size(105, 25)
        Me.downloadButton.TabIndex = 2
        Me.downloadButton.Text = "Download"
        '
        'progressBarPanel
        '
        Me.progressBarPanel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.progressBarPanel.Controls.Add(Me.downloadProgressBar)
        Me.progressBarPanel.Location = New System.Drawing.Point(18, 52)
        Me.progressBarPanel.Name = "progressBarPanel"
        Me.progressBarPanel.Size = New System.Drawing.Size(456, 58)
        Me.progressBarPanel.TabIndex = 3
        '
        'downloadProgressBar
        '
        Me.downloadProgressBar.Location = New System.Drawing.Point(13, 21)
        Me.downloadProgressBar.Name = "downloadProgressBar"
        Me.downloadProgressBar.Size = New System.Drawing.Size(432, 18)
        Me.downloadProgressBar.TabIndex = 0
        '
        'webClient
        '
        Me.webClient.BaseAddress = ""
        Me.webClient.CachePolicy = Nothing
        Me.webClient.Credentials = Nothing
        Me.webClient.Encoding = CType(resources.GetObject("webClient.Encoding"), System.Text.Encoding)
        Me.webClient.Headers = CType(resources.GetObject("webClient.Headers"), System.Net.WebHeaderCollection)
        Me.webClient.QueryString = CType(resources.GetObject("webClient.QueryString"), System.Collections.Specialized.NameValueCollection)
        Me.webClient.UseDefaultCredentials = False
        '
        'webClientProgressForm
        '
        Me.AcceptButton = Me.downloadButton
        Me.AutoSize = True
        Me.ClientSize = New System.Drawing.Size(497, 143)
        Me.Controls.Add(Me.progressBarPanel)
        Me.Controls.Add(Me.downloadButton)
        Me.Controls.Add(Me.urlTextBox)
        Me.Controls.Add(Me.downloadUrlLabel)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.Name = "webClientProgressForm"
        Me.Padding = New System.Windows.Forms.Padding(9)
        Me.Text = "WebClient Download Sample"
        Me.progressBarPanel.ResumeLayout(False)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents downloadUrlLabel As System.Windows.Forms.Label
    Friend WithEvents urlTextBox As System.Windows.Forms.TextBox
    Friend WithEvents downloadButton As System.Windows.Forms.Button
    Friend WithEvents progressBarPanel As System.Windows.Forms.Panel
    Friend WithEvents downloadProgressBar As System.Windows.Forms.ProgressBar
    Friend WithEvents webClient As System.Net.WebClient

End Class
