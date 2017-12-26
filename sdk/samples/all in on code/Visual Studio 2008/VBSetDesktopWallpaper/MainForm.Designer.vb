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
        Me.radFit = New System.Windows.Forms.RadioButton
        Me.btnSetWallpaper = New System.Windows.Forms.Button
        Me.radFill = New System.Windows.Forms.RadioButton
        Me.label2 = New System.Windows.Forms.Label
        Me.pctPreview = New System.Windows.Forms.PictureBox
        Me.toolTip = New System.Windows.Forms.ToolTip(Me.components)
        Me.tbWallpaperFileName = New System.Windows.Forms.TextBox
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.radStretch = New System.Windows.Forms.RadioButton
        Me.radCenter = New System.Windows.Forms.RadioButton
        Me.radTile = New System.Windows.Forms.RadioButton
        Me.btnBrowseWallpaper = New System.Windows.Forms.Button
        Me.wallpaperOpenFileDialog = New System.Windows.Forms.OpenFileDialog
        Me.label1 = New System.Windows.Forms.Label
        CType(Me.pctPreview, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'radFit
        '
        Me.radFit.AutoSize = True
        Me.radFit.Location = New System.Drawing.Point(7, 92)
        Me.radFit.Name = "radFit"
        Me.radFit.Size = New System.Drawing.Size(36, 17)
        Me.radFit.TabIndex = 3
        Me.radFit.Text = "Fit"
        Me.radFit.UseVisualStyleBackColor = True
        '
        'btnSetWallpaper
        '
        Me.btnSetWallpaper.Location = New System.Drawing.Point(349, 283)
        Me.btnSetWallpaper.Name = "btnSetWallpaper"
        Me.btnSetWallpaper.Size = New System.Drawing.Size(88, 23)
        Me.btnSetWallpaper.TabIndex = 14
        Me.btnSetWallpaper.Text = "Set Wallpaper"
        Me.btnSetWallpaper.UseVisualStyleBackColor = True
        '
        'radFill
        '
        Me.radFill.AutoSize = True
        Me.radFill.Location = New System.Drawing.Point(7, 116)
        Me.radFill.Name = "radFill"
        Me.radFill.Size = New System.Drawing.Size(37, 17)
        Me.radFill.TabIndex = 4
        Me.radFill.Text = "Fill"
        Me.radFill.UseVisualStyleBackColor = True
        '
        'label2
        '
        Me.label2.AutoSize = True
        Me.label2.Location = New System.Drawing.Point(7, 52)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(80, 13)
        Me.label2.TabIndex = 13
        Me.label2.Text = "Image Preview:"
        '
        'pctPreview
        '
        Me.pctPreview.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.pctPreview.Location = New System.Drawing.Point(10, 68)
        Me.pctPreview.Name = "pctPreview"
        Me.pctPreview.Size = New System.Drawing.Size(333, 239)
        Me.pctPreview.TabIndex = 12
        Me.pctPreview.TabStop = False
        '
        'tbWallpaperFileName
        '
        Me.tbWallpaperFileName.Location = New System.Drawing.Point(71, 10)
        Me.tbWallpaperFileName.Name = "tbWallpaperFileName"
        Me.tbWallpaperFileName.ReadOnly = True
        Me.tbWallpaperFileName.Size = New System.Drawing.Size(272, 20)
        Me.tbWallpaperFileName.TabIndex = 8
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.radFill)
        Me.groupBox1.Controls.Add(Me.radFit)
        Me.groupBox1.Controls.Add(Me.radStretch)
        Me.groupBox1.Controls.Add(Me.radCenter)
        Me.groupBox1.Controls.Add(Me.radTile)
        Me.groupBox1.Location = New System.Drawing.Point(349, 38)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(88, 146)
        Me.groupBox1.TabIndex = 11
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Style"
        '
        'radStretch
        '
        Me.radStretch.AutoSize = True
        Me.radStretch.Checked = True
        Me.radStretch.Location = New System.Drawing.Point(7, 68)
        Me.radStretch.Name = "radStretch"
        Me.radStretch.Size = New System.Drawing.Size(59, 17)
        Me.radStretch.TabIndex = 2
        Me.radStretch.TabStop = True
        Me.radStretch.Text = "Stretch"
        Me.radStretch.UseVisualStyleBackColor = True
        '
        'radCenter
        '
        Me.radCenter.AutoSize = True
        Me.radCenter.Location = New System.Drawing.Point(7, 44)
        Me.radCenter.Name = "radCenter"
        Me.radCenter.Size = New System.Drawing.Size(56, 17)
        Me.radCenter.TabIndex = 1
        Me.radCenter.Text = "Center"
        Me.radCenter.UseVisualStyleBackColor = True
        '
        'radTile
        '
        Me.radTile.AutoSize = True
        Me.radTile.Location = New System.Drawing.Point(7, 20)
        Me.radTile.Name = "radTile"
        Me.radTile.Size = New System.Drawing.Size(42, 17)
        Me.radTile.TabIndex = 0
        Me.radTile.Text = "Tile"
        Me.radTile.UseVisualStyleBackColor = True
        '
        'btnBrowseWallpaper
        '
        Me.btnBrowseWallpaper.Location = New System.Drawing.Point(349, 8)
        Me.btnBrowseWallpaper.Name = "btnBrowseWallpaper"
        Me.btnBrowseWallpaper.Size = New System.Drawing.Size(88, 23)
        Me.btnBrowseWallpaper.TabIndex = 10
        Me.btnBrowseWallpaper.Text = "Browse..."
        Me.btnBrowseWallpaper.UseVisualStyleBackColor = True
        '
        'wallpaperOpenFileDialog
        '
        Me.wallpaperOpenFileDialog.Filter = """All Picture Files |*.bmp;*.gif;*.jpg;*.png;*.tif"""
        Me.wallpaperOpenFileDialog.Title = "Select Wallpaper"
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(7, 13)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(55, 13)
        Me.label1.TabIndex = 9
        Me.label1.Text = "Wallpaper"
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(445, 315)
        Me.Controls.Add(Me.btnSetWallpaper)
        Me.Controls.Add(Me.label2)
        Me.Controls.Add(Me.pctPreview)
        Me.Controls.Add(Me.tbWallpaperFileName)
        Me.Controls.Add(Me.groupBox1)
        Me.Controls.Add(Me.btnBrowseWallpaper)
        Me.Controls.Add(Me.label1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.Name = "MainForm"
        Me.Text = "VBSetDesktopWallpaper"
        CType(Me.pctPreview, System.ComponentModel.ISupportInitialize).EndInit()
        Me.groupBox1.ResumeLayout(False)
        Me.groupBox1.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents radFit As System.Windows.Forms.RadioButton
    Private WithEvents btnSetWallpaper As System.Windows.Forms.Button
    Private WithEvents radFill As System.Windows.Forms.RadioButton
    Private WithEvents label2 As System.Windows.Forms.Label
    Private WithEvents pctPreview As System.Windows.Forms.PictureBox
    Private WithEvents toolTip As System.Windows.Forms.ToolTip
    Private WithEvents tbWallpaperFileName As System.Windows.Forms.TextBox
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents radStretch As System.Windows.Forms.RadioButton
    Private WithEvents radCenter As System.Windows.Forms.RadioButton
    Private WithEvents radTile As System.Windows.Forms.RadioButton
    Private WithEvents btnBrowseWallpaper As System.Windows.Forms.Button
    Private WithEvents wallpaperOpenFileDialog As System.Windows.Forms.OpenFileDialog
    Private WithEvents label1 As System.Windows.Forms.Label

End Class
