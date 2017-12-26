
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

        If disposing AndAlso (documentManipulator IsNot Nothing) Then
            documentManipulator.Dispose()
        End If

        MyBase.Dispose(disposing)
    End Sub

#Region "Windows Form Designer generated code"

    ''' <summary>
    ''' Required method for Designer support - do not modify
    ''' the contents of this method with the code editor.
    ''' </summary>
    Private Sub InitializeComponent()
        Me.pnlBrowse = New Panel()
        Me.lbFileName = New Label()
        Me.btnOpenFile = New Button()
        Me.pnlImageList = New Panel()
        Me.lstImage = New ListBox()
        Me.pnlOperation = New Panel()
        Me.btnReplace = New Button()
        Me.btnDelete = New Button()
        Me.picView = New PictureBox()
        Me.btnExport = New Button()
        Me.pnlBrowse.SuspendLayout()
        Me.pnlImageList.SuspendLayout()
        Me.pnlOperation.SuspendLayout()
        CType(Me.picView, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        ' 
        ' pnlBrowse
        ' 
        Me.pnlBrowse.Controls.Add(Me.lbFileName)
        Me.pnlBrowse.Controls.Add(Me.btnOpenFile)
        Me.pnlBrowse.Dock = DockStyle.Top
        Me.pnlBrowse.Location = New Point(0, 0)
        Me.pnlBrowse.Name = "pnlBrowse"
        Me.pnlBrowse.Size = New Size(783, 37)
        Me.pnlBrowse.TabIndex = 0
        ' 
        ' lbFileName
        ' 
        Me.lbFileName.AutoSize = True
        Me.lbFileName.Location = New Point(143, 13)
        Me.lbFileName.Name = "lbFileName"
        Me.lbFileName.Size = New Size(0, 13)
        Me.lbFileName.TabIndex = 2
        ' 
        ' btnOpenFile
        ' 
        Me.btnOpenFile.Location = New Point(12, 8)
        Me.btnOpenFile.Name = "btnOpenFile"
        Me.btnOpenFile.Size = New Size(124, 23)
        Me.btnOpenFile.TabIndex = 1
        Me.btnOpenFile.Text = "Open the word doc"
        Me.btnOpenFile.UseVisualStyleBackColor = True
        '			Me.btnOpenFile.Click += New System.EventHandler(Me.btnOpenFile_Click)
        ' 
        ' pnlImageList
        ' 
        Me.pnlImageList.Controls.Add(Me.lstImage)
        Me.pnlImageList.Dock = DockStyle.Left
        Me.pnlImageList.Location = New Point(0, 37)
        Me.pnlImageList.Name = "pnlImageList"
        Me.pnlImageList.Size = New Size(269, 431)
        Me.pnlImageList.TabIndex = 1
        ' 
        ' lstImage
        ' 
        Me.lstImage.Dock = DockStyle.Fill
        Me.lstImage.FormattingEnabled = True
        Me.lstImage.Location = New Point(0, 0)
        Me.lstImage.Name = "lstImage"
        Me.lstImage.Size = New Size(269, 431)
        Me.lstImage.TabIndex = 0
        '			Me.lstImage.SelectedIndexChanged += New System.EventHandler(Me.lstImage_SelectedIndexChanged)
        ' 
        ' pnlOperation
        ' 
        Me.pnlOperation.Controls.Add(Me.btnExport)
        Me.pnlOperation.Controls.Add(Me.btnReplace)
        Me.pnlOperation.Controls.Add(Me.btnDelete)
        Me.pnlOperation.Dock = DockStyle.Top
        Me.pnlOperation.Location = New Point(269, 37)
        Me.pnlOperation.Name = "pnlOperation"
        Me.pnlOperation.Size = New Size(514, 35)
        Me.pnlOperation.TabIndex = 2
        ' 
        ' btnReplace
        ' 
        Me.btnReplace.Location = New Point(218, 5)
        Me.btnReplace.Name = "btnReplace"
        Me.btnReplace.Size = New Size(75, 23)
        Me.btnReplace.TabIndex = 1
        Me.btnReplace.Text = "Replace"
        Me.btnReplace.UseVisualStyleBackColor = True
        '			Me.btnReplace.Click += New System.EventHandler(Me.btnReplace_Click)
        ' 
        ' btnDelete
        ' 
        Me.btnDelete.Location = New Point(125, 5)
        Me.btnDelete.Name = "btnDelete"
        Me.btnDelete.Size = New Size(75, 23)
        Me.btnDelete.TabIndex = 0
        Me.btnDelete.Text = "Delete"
        Me.btnDelete.UseVisualStyleBackColor = True
        '			Me.btnDelete.Click += New System.EventHandler(Me.btnDelete_Click)
        ' 
        ' picView
        ' 
        Me.picView.Dock = DockStyle.Fill
        Me.picView.Location = New Point(269, 72)
        Me.picView.Name = "picView"
        Me.picView.Size = New Size(514, 396)
        Me.picView.TabIndex = 3
        Me.picView.TabStop = False
        ' 
        ' btnExport
        ' 
        Me.btnExport.Location = New Point(33, 5)
        Me.btnExport.Name = "btnExport"
        Me.btnExport.Size = New Size(75, 23)
        Me.btnExport.TabIndex = 2
        Me.btnExport.Text = "Export"
        Me.btnExport.UseVisualStyleBackColor = True
        '			Me.btnExport.Click += New System.EventHandler(Me.btnExport_Click)
        ' 
        ' MainForm
        ' 
        Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = AutoScaleMode.Font
        Me.ClientSize = New Size(783, 468)
        Me.Controls.Add(Me.picView)
        Me.Controls.Add(Me.pnlOperation)
        Me.Controls.Add(Me.pnlImageList)
        Me.Controls.Add(Me.pnlBrowse)
        Me.Name = "MainForm"
        Me.Text = "VBManipulateImagesInWordDocument"
        Me.pnlBrowse.ResumeLayout(False)
        Me.pnlBrowse.PerformLayout()
        Me.pnlImageList.ResumeLayout(False)
        Me.pnlOperation.ResumeLayout(False)
        CType(Me.picView, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private pnlBrowse As Panel
    Private lbFileName As Label
    Private WithEvents btnOpenFile As Button
    Private pnlImageList As Panel
    Private WithEvents lstImage As ListBox
    Private pnlOperation As Panel
    Private WithEvents btnReplace As Button
    Private WithEvents btnDelete As Button
    Private picView As PictureBox
    Private WithEvents btnExport As Button
End Class

