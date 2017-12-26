<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class MainForm
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Dim ListViewItem1 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("", 0)
        Dim ListViewItem2 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("", 1)
        Dim ListViewItem3 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("", 2)
        Dim ListViewItem4 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("", 3)
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(MainForm))
        Me.thumbnailTabControl = New System.Windows.Forms.TabControl
        Me.thumbnailPreviewTabPage = New System.Windows.Forms.TabPage
        Me.changePreviewOrderButton = New System.Windows.Forms.Button
        Me.removeThumbnailButton = New System.Windows.Forms.Button
        Me.addThumbnailButton2 = New System.Windows.Forms.Button
        Me.removeThumbnailButton2 = New System.Windows.Forms.Button
        Me.addThumbnailButton = New System.Windows.Forms.Button
        Me.pictureBox1 = New System.Windows.Forms.PictureBox
        Me.pictureBox2 = New System.Windows.Forms.PictureBox
        Me.thumbnailToolbarTabPage = New System.Windows.Forms.TabPage
        Me.imagePictureBox = New System.Windows.Forms.PictureBox
        Me.imageListView = New System.Windows.Forms.ListView
        Me.imageList = New System.Windows.Forms.ImageList(Me.components)
        Me.thumbnailTabControl.SuspendLayout()
        Me.thumbnailPreviewTabPage.SuspendLayout()
        CType(Me.pictureBox1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pictureBox2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.thumbnailToolbarTabPage.SuspendLayout()
        CType(Me.imagePictureBox, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'thumbnailTabControl
        '
        Me.thumbnailTabControl.Controls.Add(Me.thumbnailPreviewTabPage)
        Me.thumbnailTabControl.Controls.Add(Me.thumbnailToolbarTabPage)
        Me.thumbnailTabControl.Location = New System.Drawing.Point(30, 32)
        Me.thumbnailTabControl.Name = "thumbnailTabControl"
        Me.thumbnailTabControl.SelectedIndex = 0
        Me.thumbnailTabControl.Size = New System.Drawing.Size(608, 331)
        Me.thumbnailTabControl.TabIndex = 7
        '
        'thumbnailPreviewTabPage
        '
        Me.thumbnailPreviewTabPage.Controls.Add(Me.changePreviewOrderButton)
        Me.thumbnailPreviewTabPage.Controls.Add(Me.removeThumbnailButton)
        Me.thumbnailPreviewTabPage.Controls.Add(Me.addThumbnailButton2)
        Me.thumbnailPreviewTabPage.Controls.Add(Me.removeThumbnailButton2)
        Me.thumbnailPreviewTabPage.Controls.Add(Me.addThumbnailButton)
        Me.thumbnailPreviewTabPage.Controls.Add(Me.pictureBox1)
        Me.thumbnailPreviewTabPage.Controls.Add(Me.pictureBox2)
        Me.thumbnailPreviewTabPage.Location = New System.Drawing.Point(4, 22)
        Me.thumbnailPreviewTabPage.Name = "thumbnailPreviewTabPage"
        Me.thumbnailPreviewTabPage.Padding = New System.Windows.Forms.Padding(3)
        Me.thumbnailPreviewTabPage.Size = New System.Drawing.Size(600, 305)
        Me.thumbnailPreviewTabPage.TabIndex = 0
        Me.thumbnailPreviewTabPage.Text = "Thumbnail Preview"
        Me.thumbnailPreviewTabPage.UseVisualStyleBackColor = True
        '
        'changePreviewOrderButton
        '
        Me.changePreviewOrderButton.Location = New System.Drawing.Point(224, 202)
        Me.changePreviewOrderButton.Name = "changePreviewOrderButton"
        Me.changePreviewOrderButton.Size = New System.Drawing.Size(122, 23)
        Me.changePreviewOrderButton.TabIndex = 6
        Me.changePreviewOrderButton.Text = "Change Preview Order"
        Me.changePreviewOrderButton.UseVisualStyleBackColor = True
        '
        'removeThumbnailButton
        '
        Me.removeThumbnailButton.Location = New System.Drawing.Point(68, 234)
        Me.removeThumbnailButton.Name = "removeThumbnailButton"
        Me.removeThumbnailButton.Size = New System.Drawing.Size(121, 23)
        Me.removeThumbnailButton.TabIndex = 2
        Me.removeThumbnailButton.Text = "Remove Thumbnail"
        Me.removeThumbnailButton.UseVisualStyleBackColor = True
        '
        'addThumbnailButton2
        '
        Me.addThumbnailButton2.Location = New System.Drawing.Point(372, 202)
        Me.addThumbnailButton2.Name = "addThumbnailButton2"
        Me.addThumbnailButton2.Size = New System.Drawing.Size(121, 23)
        Me.addThumbnailButton2.TabIndex = 4
        Me.addThumbnailButton2.Text = "Add as Thumbnail"
        Me.addThumbnailButton2.UseVisualStyleBackColor = True
        '
        'removeThumbnailButton2
        '
        Me.removeThumbnailButton2.Location = New System.Drawing.Point(372, 234)
        Me.removeThumbnailButton2.Name = "removeThumbnailButton2"
        Me.removeThumbnailButton2.Size = New System.Drawing.Size(121, 23)
        Me.removeThumbnailButton2.TabIndex = 5
        Me.removeThumbnailButton2.Text = "Remove Thumbnail"
        Me.removeThumbnailButton2.UseVisualStyleBackColor = True
        '
        'addThumbnailButton
        '
        Me.addThumbnailButton.Location = New System.Drawing.Point(68, 202)
        Me.addThumbnailButton.Name = "addThumbnailButton"
        Me.addThumbnailButton.Size = New System.Drawing.Size(121, 23)
        Me.addThumbnailButton.TabIndex = 1
        Me.addThumbnailButton.Text = "Add as Thumbnail"
        Me.addThumbnailButton.UseVisualStyleBackColor = True
        '
        'pictureBox1
        '
        Me.pictureBox1.Image = Global.VBWin7TaskbarThumbnail.My.Resources.Resources.DotNet2
        Me.pictureBox1.Location = New System.Drawing.Point(43, 35)
        Me.pictureBox1.Name = "pictureBox1"
        Me.pictureBox1.Size = New System.Drawing.Size(156, 161)
        Me.pictureBox1.TabIndex = 0
        Me.pictureBox1.TabStop = False
        '
        'pictureBox2
        '
        Me.pictureBox2.Image = Global.VBWin7TaskbarThumbnail.My.Resources.Resources.DotNet1
        Me.pictureBox2.Location = New System.Drawing.Point(261, 79)
        Me.pictureBox2.Name = "pictureBox2"
        Me.pictureBox2.Size = New System.Drawing.Size(243, 75)
        Me.pictureBox2.TabIndex = 3
        Me.pictureBox2.TabStop = False
        '
        'thumbnailToolbarTabPage
        '
        Me.thumbnailToolbarTabPage.Controls.Add(Me.imagePictureBox)
        Me.thumbnailToolbarTabPage.Controls.Add(Me.imageListView)
        Me.thumbnailToolbarTabPage.Location = New System.Drawing.Point(4, 22)
        Me.thumbnailToolbarTabPage.Name = "thumbnailToolbarTabPage"
        Me.thumbnailToolbarTabPage.Padding = New System.Windows.Forms.Padding(3)
        Me.thumbnailToolbarTabPage.Size = New System.Drawing.Size(600, 305)
        Me.thumbnailToolbarTabPage.TabIndex = 1
        Me.thumbnailToolbarTabPage.Text = "Thumbnail Toolbar"
        Me.thumbnailToolbarTabPage.UseVisualStyleBackColor = True
        '
        'imagePictureBox
        '
        Me.imagePictureBox.Location = New System.Drawing.Point(243, 62)
        Me.imagePictureBox.Name = "imagePictureBox"
        Me.imagePictureBox.Size = New System.Drawing.Size(45, 50)
        Me.imagePictureBox.TabIndex = 1
        Me.imagePictureBox.TabStop = False
        '
        'imageListView
        '
        Me.imageListView.Items.AddRange(New System.Windows.Forms.ListViewItem() {ListViewItem1, ListViewItem2, ListViewItem3, ListViewItem4})
        Me.imageListView.LargeImageList = Me.imageList
        Me.imageListView.Location = New System.Drawing.Point(111, 165)
        Me.imageListView.MultiSelect = False
        Me.imageListView.Name = "imageListView"
        Me.imageListView.Size = New System.Drawing.Size(313, 57)
        Me.imageListView.TabIndex = 0
        Me.imageListView.UseCompatibleStateImageBehavior = False
        '
        'imageList
        '
        Me.imageList.ImageStream = CType(resources.GetObject("imageList.ImageStream"), System.Windows.Forms.ImageListStreamer)
        Me.imageList.TransparentColor = System.Drawing.Color.Transparent
        Me.imageList.Images.SetKeyName(0, "C.JPG")
        Me.imageList.Images.SetKeyName(1, "O.JPG")
        Me.imageList.Images.SetKeyName(2, "D.JPG")
        Me.imageList.Images.SetKeyName(3, "E.JPG")
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(682, 417)
        Me.Controls.Add(Me.thumbnailTabControl)
        Me.Name = "MainForm"
        Me.Text = "Win7 Taskbar Thumbnail"
        Me.thumbnailTabControl.ResumeLayout(False)
        Me.thumbnailPreviewTabPage.ResumeLayout(False)
        CType(Me.pictureBox1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pictureBox2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.thumbnailToolbarTabPage.ResumeLayout(False)
        CType(Me.imagePictureBox, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents thumbnailTabControl As System.Windows.Forms.TabControl
    Private WithEvents thumbnailPreviewTabPage As System.Windows.Forms.TabPage
    Private WithEvents changePreviewOrderButton As System.Windows.Forms.Button
    Private WithEvents removeThumbnailButton As System.Windows.Forms.Button
    Private WithEvents addThumbnailButton2 As System.Windows.Forms.Button
    Private WithEvents removeThumbnailButton2 As System.Windows.Forms.Button
    Private WithEvents addThumbnailButton As System.Windows.Forms.Button
    Private WithEvents pictureBox1 As System.Windows.Forms.PictureBox
    Private WithEvents pictureBox2 As System.Windows.Forms.PictureBox
    Private WithEvents thumbnailToolbarTabPage As System.Windows.Forms.TabPage
    Private WithEvents imagePictureBox As System.Windows.Forms.PictureBox
    Private WithEvents imageListView As System.Windows.Forms.ListView
    Private WithEvents imageList As System.Windows.Forms.ImageList

End Class
