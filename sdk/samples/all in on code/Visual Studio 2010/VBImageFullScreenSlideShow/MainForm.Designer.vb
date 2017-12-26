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
        Me.components = New System.ComponentModel.Container()
        Me.pnlSlideShow = New System.Windows.Forms.Panel()
        Me.pictureBox = New System.Windows.Forms.PictureBox()
        Me.imageFolderBrowserDlg = New System.Windows.Forms.FolderBrowserDialog()
        Me.timer = New System.Windows.Forms.Timer(Me.components)
        Me.gbButtons = New System.Windows.Forms.GroupBox()
        Me.btnFullScreen = New System.Windows.Forms.Button()
        Me.btnSetting = New System.Windows.Forms.Button()
        Me.btnImageSlideShow = New System.Windows.Forms.Button()
        Me.btnNext = New System.Windows.Forms.Button()
        Me.btnOpenFolder = New System.Windows.Forms.Button()
        Me.btnPrevious = New System.Windows.Forms.Button()
        Me.pnlSlideShow.SuspendLayout()
        CType(Me.pictureBox, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.gbButtons.SuspendLayout()
        Me.SuspendLayout()
        '
        'pnlSlideShow
        '
        Me.pnlSlideShow.AutoScroll = True
        Me.pnlSlideShow.BackColor = System.Drawing.Color.Black
        Me.pnlSlideShow.Controls.Add(Me.pictureBox)
        Me.pnlSlideShow.Dock = System.Windows.Forms.DockStyle.Top
        Me.pnlSlideShow.Location = New System.Drawing.Point(0, 0)
        Me.pnlSlideShow.Name = "pnlSlideShow"
        Me.pnlSlideShow.Size = New System.Drawing.Size(448, 248)
        Me.pnlSlideShow.TabIndex = 1
        '
        'pictureBox
        '
        Me.pictureBox.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pictureBox.Location = New System.Drawing.Point(0, 0)
        Me.pictureBox.Name = "pictureBox"
        Me.pictureBox.Size = New System.Drawing.Size(448, 248)
        Me.pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom
        Me.pictureBox.TabIndex = 0
        Me.pictureBox.TabStop = False
        '
        'timer
        '
        Me.timer.Interval = 1000
        '
        'gbButtons
        '
        Me.gbButtons.Controls.Add(Me.btnFullScreen)
        Me.gbButtons.Controls.Add(Me.btnSetting)
        Me.gbButtons.Controls.Add(Me.btnImageSlideShow)
        Me.gbButtons.Controls.Add(Me.btnNext)
        Me.gbButtons.Controls.Add(Me.btnOpenFolder)
        Me.gbButtons.Controls.Add(Me.btnPrevious)
        Me.gbButtons.Dock = System.Windows.Forms.DockStyle.Fill
        Me.gbButtons.Location = New System.Drawing.Point(0, 248)
        Me.gbButtons.Name = "gbButtons"
        Me.gbButtons.Size = New System.Drawing.Size(448, 123)
        Me.gbButtons.TabIndex = 2
        Me.gbButtons.TabStop = False
        '
        'btnFullScreen
        '
        Me.btnFullScreen.Location = New System.Drawing.Point(233, 71)
        Me.btnFullScreen.Name = "btnFullScreen"
        Me.btnFullScreen.Size = New System.Drawing.Size(208, 23)
        Me.btnFullScreen.TabIndex = 16
        Me.btnFullScreen.Text = "Full Screen"
        Me.btnFullScreen.UseVisualStyleBackColor = True
        '
        'btnSetting
        '
        Me.btnSetting.Location = New System.Drawing.Point(233, 29)
        Me.btnSetting.Name = "btnSetting"
        Me.btnSetting.Size = New System.Drawing.Size(96, 23)
        Me.btnSetting.TabIndex = 15
        Me.btnSetting.Text = "Settings"
        Me.btnSetting.UseVisualStyleBackColor = True
        '
        'btnImageSlideShow
        '
        Me.btnImageSlideShow.Location = New System.Drawing.Point(8, 71)
        Me.btnImageSlideShow.Name = "btnImageSlideShow"
        Me.btnImageSlideShow.Size = New System.Drawing.Size(209, 23)
        Me.btnImageSlideShow.TabIndex = 14
        Me.btnImageSlideShow.Text = "Start Slideshow"
        '
        'btnNext
        '
        Me.btnNext.Location = New System.Drawing.Point(345, 29)
        Me.btnNext.Name = "btnNext"
        Me.btnNext.Size = New System.Drawing.Size(96, 23)
        Me.btnNext.TabIndex = 13
        Me.btnNext.Text = "Next"
        '
        'btnOpenFolder
        '
        Me.btnOpenFolder.Location = New System.Drawing.Point(121, 29)
        Me.btnOpenFolder.Name = "btnOpenFolder"
        Me.btnOpenFolder.Size = New System.Drawing.Size(96, 23)
        Me.btnOpenFolder.TabIndex = 12
        Me.btnOpenFolder.Text = "Open Folder..."
        '
        'btnPrevious
        '
        Me.btnPrevious.Location = New System.Drawing.Point(7, 29)
        Me.btnPrevious.Name = "btnPrevious"
        Me.btnPrevious.Size = New System.Drawing.Size(96, 23)
        Me.btnPrevious.TabIndex = 11
        Me.btnPrevious.Text = "Previous"
        '
        'MainForm
        '
        Me.ClientSize = New System.Drawing.Size(448, 371)
        Me.Controls.Add(Me.gbButtons)
        Me.Controls.Add(Me.pnlSlideShow)
        Me.DoubleBuffered = True
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "MainForm"
        Me.Text = "VBImageFullScreenSlideShow"
        Me.pnlSlideShow.ResumeLayout(False)
        CType(Me.pictureBox, System.ComponentModel.ISupportInitialize).EndInit()
        Me.gbButtons.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private pnlSlideShow As Panel
    Private pictureBox As PictureBox
    Private imageFolderBrowserDlg As FolderBrowserDialog
    Private WithEvents timer As Timer
    Private gbButtons As GroupBox
    Private WithEvents btnFullScreen As Button
    Private WithEvents btnSetting As Button
    Private WithEvents btnImageSlideShow As Button
    Private WithEvents btnNext As Button
    Private WithEvents btnOpenFolder As Button
    Private WithEvents btnPrevious As Button
End Class


