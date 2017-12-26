
Partial Public Class ImageListExplorerBar
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

#Region "Component Designer generated code"

    ''' <summary> 
    ''' Required method for Designer support - do not modify 
    ''' the contents of this method with the code editor.
    ''' </summary>
    Private Sub InitializeComponent()
        Me.pnlCmd = New Panel()
        Me.btnGetImg = New Button()
        Me.pnlImgList = New Panel()
        Me.lstImg = New ListBox()
        Me.pnlCmd.SuspendLayout()
        Me.pnlImgList.SuspendLayout()
        Me.SuspendLayout()
        ' 
        ' pnlCmd
        ' 
        Me.pnlCmd.Controls.Add(Me.btnGetImg)
        Me.pnlCmd.Dock = DockStyle.Top
        Me.pnlCmd.Location = New Point(0, 0)
        Me.pnlCmd.Name = "pnlCmd"
        Me.pnlCmd.Size = New Size(366, 36)
        Me.pnlCmd.TabIndex = 0
        ' 
        ' btnGetImg
        ' 
        Me.btnGetImg.Location = New Point(16, 4)
        Me.btnGetImg.Name = "btnGetImg"
        Me.btnGetImg.Size = New Size(129, 23)
        Me.btnGetImg.TabIndex = 0
        Me.btnGetImg.Text = "Get all images"
        Me.btnGetImg.UseVisualStyleBackColor = True
        '			Me.btnGetImg.Click += New System.EventHandler(Me.btnGetImg_Click)
        ' 
        ' pnlImgList
        ' 
        Me.pnlImgList.Controls.Add(Me.lstImg)
        Me.pnlImgList.Dock = DockStyle.Fill
        Me.pnlImgList.Location = New Point(0, 36)
        Me.pnlImgList.Name = "pnlImgList"
        Me.pnlImgList.Size = New Size(366, 412)
        Me.pnlImgList.TabIndex = 1
        ' 
        ' lstImg
        ' 
        Me.lstImg.Dock = DockStyle.Fill
        Me.lstImg.FormattingEnabled = True
        Me.lstImg.Location = New Point(0, 0)
        Me.lstImg.Name = "lstImg"
        Me.lstImg.Size = New Size(366, 412)
        Me.lstImg.TabIndex = 0
        '			Me.lstImg.DoubleClick += New System.EventHandler(Me.lstImg_DoubleClick)
        ' 
        ' BookmarkExplorerBar
        ' 
        Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = AutoScaleMode.Font
        Me.Controls.Add(Me.pnlImgList)
        Me.Controls.Add(Me.pnlCmd)
        Me.Name = "BookmarkExplorerBar"
        Me.Size = New Size(366, 448)
        Me.pnlCmd.ResumeLayout(False)
        Me.pnlImgList.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private pnlCmd As Panel
    Private WithEvents btnGetImg As Button
    Private pnlImgList As Panel
    Private WithEvents lstImg As ListBox

End Class

