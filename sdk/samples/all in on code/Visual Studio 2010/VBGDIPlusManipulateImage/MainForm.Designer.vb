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

        If disposing AndAlso (_imgManipulator IsNot Nothing) Then
            _imgManipulator.Dispose()
        End If

        MyBase.Dispose(disposing)
    End Sub

#Region "Windows Form Designer generated code"

    ''' <summary>
    ''' Required method for Designer support - do not modify
    ''' the contents of this method with the code editor.
    ''' </summary>
    Private Sub InitializeComponent()
        Me.pnlMenu = New Panel()
        Me.chkDrawBounds = New CheckBox()
        Me.btnReset = New Button()
        Me.btnRotateAngle = New Button()
        Me.tbRotateAngle = New TextBox()
        Me.lbFlip = New Label()
        Me.lbMove = New Label()
        Me.lbRotateAngle = New Label()
        Me.lbRotate = New Label()
        Me.lbSize = New Label()
        Me.pnlImage = New Panel()
        Me.lbInterpolationMode = New Label()
        Me.cmbInterpolationMode = New ComboBox()
        Me.label1 = New Label()
        Me.btnSkewRight = New Button()
        Me.btnFlipVertical = New Button()
        Me.btnAmplify = New Button()
        Me.btnRotateRight = New Button()
        Me.btnMoveLeft = New Button()
        Me.btnMoveUp = New Button()
        Me.btnMoveDown = New Button()
        Me.btnMoveRight = New Button()
        Me.btnSkewLeft = New Button()
        Me.btnFlipHorizontal = New Button()
        Me.btnMicrify = New Button()
        Me.btnRotateLeft = New Button()
        Me.pnlMenu.SuspendLayout()
        Me.SuspendLayout()
        ' 
        ' pnlMenu
        ' 
        Me.pnlMenu.Controls.Add(Me.cmbInterpolationMode)
        Me.pnlMenu.Controls.Add(Me.chkDrawBounds)
        Me.pnlMenu.Controls.Add(Me.btnReset)
        Me.pnlMenu.Controls.Add(Me.btnRotateAngle)
        Me.pnlMenu.Controls.Add(Me.tbRotateAngle)
        Me.pnlMenu.Controls.Add(Me.btnSkewRight)
        Me.pnlMenu.Controls.Add(Me.btnFlipVertical)
        Me.pnlMenu.Controls.Add(Me.btnAmplify)
        Me.pnlMenu.Controls.Add(Me.btnRotateRight)
        Me.pnlMenu.Controls.Add(Me.btnMoveLeft)
        Me.pnlMenu.Controls.Add(Me.btnMoveUp)
        Me.pnlMenu.Controls.Add(Me.btnMoveDown)
        Me.pnlMenu.Controls.Add(Me.btnMoveRight)
        Me.pnlMenu.Controls.Add(Me.btnSkewLeft)
        Me.pnlMenu.Controls.Add(Me.btnFlipHorizontal)
        Me.pnlMenu.Controls.Add(Me.btnMicrify)
        Me.pnlMenu.Controls.Add(Me.btnRotateLeft)
        Me.pnlMenu.Controls.Add(Me.label1)
        Me.pnlMenu.Controls.Add(Me.lbFlip)
        Me.pnlMenu.Controls.Add(Me.lbMove)
        Me.pnlMenu.Controls.Add(Me.lbRotateAngle)
        Me.pnlMenu.Controls.Add(Me.lbRotate)
        Me.pnlMenu.Controls.Add(Me.lbInterpolationMode)
        Me.pnlMenu.Controls.Add(Me.lbSize)
        Me.pnlMenu.Dock = DockStyle.Left
        Me.pnlMenu.Location = New Point(0, 0)
        Me.pnlMenu.Name = "pnlMenu"
        Me.pnlMenu.Size = New Size(269, 541)
        Me.pnlMenu.TabIndex = 0
        ' 
        ' chkDrawBounds
        ' 
        Me.chkDrawBounds.AutoSize = True
        Me.chkDrawBounds.Location = New Point(13, 461)
        Me.chkDrawBounds.Name = "chkDrawBounds"
        Me.chkDrawBounds.Size = New Size(90, 17)
        Me.chkDrawBounds.TabIndex = 6
        Me.chkDrawBounds.Text = "Draw Bounds"
        Me.chkDrawBounds.UseVisualStyleBackColor = True
        '			Me.chkDrawBounds.CheckedChanged += New System.EventHandler(Me.chkDrawBounds_CheckedChanged)
        ' 
        ' btnReset
        ' 
        Me.btnReset.Location = New Point(13, 506)
        Me.btnReset.Name = "btnReset"
        Me.btnReset.Size = New Size(75, 23)
        Me.btnReset.TabIndex = 5
        Me.btnReset.Text = "Reset"
        Me.btnReset.UseVisualStyleBackColor = True
        '			Me.btnReset.Click += New System.EventHandler(Me.btnReset_Click)
        ' 
        ' btnRotateAngle
        ' 
        Me.btnRotateAngle.Location = New Point(213, 307)
        Me.btnRotateAngle.Name = "btnRotateAngle"
        Me.btnRotateAngle.Size = New Size(42, 23)
        Me.btnRotateAngle.TabIndex = 4
        Me.btnRotateAngle.Text = "OK"
        Me.btnRotateAngle.UseVisualStyleBackColor = True
        '			Me.btnRotateAngle.Click += New System.EventHandler(Me.btnRotateAngle_Click)
        ' 
        ' tbRotateAngle
        ' 
        Me.tbRotateAngle.Location = New Point(80, 309)
        Me.tbRotateAngle.Name = "tbRotateAngle"
        Me.tbRotateAngle.Size = New Size(127, 20)
        Me.tbRotateAngle.TabIndex = 3
        Me.tbRotateAngle.Text = "10"
        ' 
        ' lbFlip
        ' 
        Me.lbFlip.AutoSize = True
        Me.lbFlip.Location = New Point(4, 162)
        Me.lbFlip.Name = "lbFlip"
        Me.lbFlip.Size = New Size(23, 13)
        Me.lbFlip.TabIndex = 0
        Me.lbFlip.Text = "Flip"
        ' 
        ' lbMove
        ' 
        Me.lbMove.AutoSize = True
        Me.lbMove.Location = New Point(3, 379)
        Me.lbMove.Name = "lbMove"
        Me.lbMove.Size = New Size(34, 13)
        Me.lbMove.TabIndex = 0
        Me.lbMove.Text = "Move"
        ' 
        ' lbRotateAngle
        ' 
        Me.lbRotateAngle.AutoSize = True
        Me.lbRotateAngle.Location = New Point(4, 313)
        Me.lbRotateAngle.Name = "lbRotateAngle"
        Me.lbRotateAngle.Size = New Size(69, 13)
        Me.lbRotateAngle.TabIndex = 0
        Me.lbRotateAngle.Text = "Rotate Angle"
        ' 
        ' lbRotate
        ' 
        Me.lbRotate.AutoSize = True
        Me.lbRotate.Location = New Point(4, 103)
        Me.lbRotate.Name = "lbRotate"
        Me.lbRotate.Size = New Size(39, 13)
        Me.lbRotate.TabIndex = 0
        Me.lbRotate.Text = "Rotate"
        ' 
        ' lbSize
        ' 
        Me.lbSize.AutoSize = True
        Me.lbSize.Location = New Point(4, 46)
        Me.lbSize.Name = "lbSize"
        Me.lbSize.Size = New Size(27, 13)
        Me.lbSize.TabIndex = 0
        Me.lbSize.Text = "Size"
        ' 
        ' pnlImage
        ' 
        Me.pnlImage.BackColor = SystemColors.ControlDark
        Me.pnlImage.Dock = DockStyle.Fill
        Me.pnlImage.Location = New Point(269, 0)
        Me.pnlImage.Name = "pnlImage"
        Me.pnlImage.Size = New Size(560, 541)
        Me.pnlImage.TabIndex = 1
        '			Me.pnlImage.Paint += New System.Windows.Forms.PaintEventHandler(Me.pnlImage_Paint)
        ' 
        ' lbInterpolationMode
        ' 
        Me.lbInterpolationMode.AutoSize = True
        Me.lbInterpolationMode.Location = New Point(4, 18)
        Me.lbInterpolationMode.Name = "lbInterpolationMode"
        Me.lbInterpolationMode.Size = New Size(92, 13)
        Me.lbInterpolationMode.TabIndex = 0
        Me.lbInterpolationMode.Text = "InterpolationMode"
        ' 
        ' cmbInterpolationMode
        ' 
        Me.cmbInterpolationMode.DropDownStyle = ComboBoxStyle.DropDownList
        Me.cmbInterpolationMode.FormattingEnabled = True
        Me.cmbInterpolationMode.Location = New Point(96, 13)
        Me.cmbInterpolationMode.Name = "cmbInterpolationMode"
        Me.cmbInterpolationMode.Size = New Size(158, 21)
        Me.cmbInterpolationMode.TabIndex = 7
        ' 
        ' label1
        ' 
        Me.label1.AutoSize = True
        Me.label1.Location = New Point(4, 240)
        Me.label1.Name = "label1"
        Me.label1.Size = New Size(34, 13)
        Me.label1.TabIndex = 0
        Me.label1.Text = "Skew"
        ' 
        ' btnSkewRight
        ' 
        Me.btnSkewRight.Image = My.Resources.SkewRight
        Me.btnSkewRight.ImageAlign = ContentAlignment.MiddleLeft
        Me.btnSkewRight.Location = New Point(156, 234)
        Me.btnSkewRight.Name = "btnSkewRight"
        Me.btnSkewRight.Size = New Size(98, 24)
        Me.btnSkewRight.TabIndex = 2
        Me.btnSkewRight.Text = "SkewRight"
        Me.btnSkewRight.TextImageRelation = TextImageRelation.ImageBeforeText
        Me.btnSkewRight.UseVisualStyleBackColor = True
        '			Me.btnSkewRight.Click += New System.EventHandler(Me.btnSkew_Click)
        ' 
        ' btnFlipVertical
        ' 
        Me.btnFlipVertical.Image = My.Resources.FlipVertical
        Me.btnFlipVertical.ImageAlign = ContentAlignment.MiddleLeft
        Me.btnFlipVertical.Location = New Point(156, 156)
        Me.btnFlipVertical.Name = "btnFlipVertical"
        Me.btnFlipVertical.Size = New Size(98, 24)
        Me.btnFlipVertical.TabIndex = 2
        Me.btnFlipVertical.Text = "FlipVertical"
        Me.btnFlipVertical.TextImageRelation = TextImageRelation.ImageBeforeText
        Me.btnFlipVertical.UseVisualStyleBackColor = True
        '			Me.btnFlipVertical.Click += New System.EventHandler(Me.btnRotateFlip_Click)
        ' 
        ' btnAmplify
        ' 
        Me.btnAmplify.Image = My.Resources.Amplify
        Me.btnAmplify.ImageAlign = ContentAlignment.MiddleLeft
        Me.btnAmplify.Location = New Point(156, 40)
        Me.btnAmplify.Name = "btnAmplify"
        Me.btnAmplify.Size = New Size(98, 24)
        Me.btnAmplify.TabIndex = 2
        Me.btnAmplify.Text = "Amplify"
        Me.btnAmplify.TextImageRelation = TextImageRelation.ImageBeforeText
        Me.btnAmplify.UseVisualStyleBackColor = True
        '			Me.btnAmplify.Click += New System.EventHandler(Me.btnAmplify_Click)
        ' 
        ' btnRotateRight
        ' 
        Me.btnRotateRight.Image = My.Resources.RotateRight
        Me.btnRotateRight.ImageAlign = ContentAlignment.MiddleLeft
        Me.btnRotateRight.Location = New Point(156, 97)
        Me.btnRotateRight.Name = "btnRotateRight"
        Me.btnRotateRight.Size = New Size(98, 24)
        Me.btnRotateRight.TabIndex = 2
        Me.btnRotateRight.Text = "RotateRight"
        Me.btnRotateRight.TextImageRelation = TextImageRelation.ImageBeforeText
        Me.btnRotateRight.UseVisualStyleBackColor = True
        '			Me.btnRotateRight.Click += New System.EventHandler(Me.btnRotateFlip_Click)
        ' 
        ' btnMoveLeft
        ' 
        Me.btnMoveLeft.Image = My.Resources.MoveLeft
        Me.btnMoveLeft.ImageAlign = ContentAlignment.MiddleLeft
        Me.btnMoveLeft.Location = New Point(48, 373)
        Me.btnMoveLeft.Name = "btnMoveLeft"
        Me.btnMoveLeft.Size = New Size(86, 24)
        Me.btnMoveLeft.TabIndex = 2
        Me.btnMoveLeft.Text = "MoveLeft"
        Me.btnMoveLeft.TextImageRelation = TextImageRelation.ImageBeforeText
        Me.btnMoveLeft.UseVisualStyleBackColor = True
        '			Me.btnMoveLeft.Click += New System.EventHandler(Me.btnMove_Click)
        ' 
        ' btnMoveUp
        ' 
        Me.btnMoveUp.Image = My.Resources.MoveUp
        Me.btnMoveUp.ImageAlign = ContentAlignment.MiddleLeft
        Me.btnMoveUp.Location = New Point(96, 347)
        Me.btnMoveUp.Name = "btnMoveUp"
        Me.btnMoveUp.Size = New Size(86, 24)
        Me.btnMoveUp.TabIndex = 2
        Me.btnMoveUp.Text = "MoveUp"
        Me.btnMoveUp.TextImageRelation = TextImageRelation.ImageBeforeText
        Me.btnMoveUp.UseVisualStyleBackColor = True
        '			Me.btnMoveUp.Click += New System.EventHandler(Me.btnMove_Click)
        ' 
        ' btnMoveDown
        ' 
        Me.btnMoveDown.Image = My.Resources.MoveDown
        Me.btnMoveDown.ImageAlign = ContentAlignment.MiddleLeft
        Me.btnMoveDown.Location = New Point(96, 398)
        Me.btnMoveDown.Name = "btnMoveDown"
        Me.btnMoveDown.Size = New Size(86, 24)
        Me.btnMoveDown.TabIndex = 2
        Me.btnMoveDown.Text = "MoveDown"
        Me.btnMoveDown.TextImageRelation = TextImageRelation.ImageBeforeText
        Me.btnMoveDown.UseVisualStyleBackColor = True
        '			Me.btnMoveDown.Click += New System.EventHandler(Me.btnMove_Click)
        ' 
        ' btnMoveRight
        ' 
        Me.btnMoveRight.Image = My.Resources.MoveRight
        Me.btnMoveRight.ImageAlign = ContentAlignment.MiddleLeft
        Me.btnMoveRight.Location = New Point(143, 373)
        Me.btnMoveRight.Name = "btnMoveRight"
        Me.btnMoveRight.Size = New Size(85, 24)
        Me.btnMoveRight.TabIndex = 2
        Me.btnMoveRight.Text = "MoveRight"
        Me.btnMoveRight.TextImageRelation = TextImageRelation.ImageBeforeText
        Me.btnMoveRight.UseVisualStyleBackColor = True
        '			Me.btnMoveRight.Click += New System.EventHandler(Me.btnMove_Click)
        ' 
        ' btnSkewLeft
        ' 
        Me.btnSkewLeft.Image = My.Resources.SkewLeft
        Me.btnSkewLeft.ImageAlign = ContentAlignment.MiddleLeft
        Me.btnSkewLeft.Location = New Point(48, 234)
        Me.btnSkewLeft.Name = "btnSkewLeft"
        Me.btnSkewLeft.Size = New Size(102, 24)
        Me.btnSkewLeft.TabIndex = 2
        Me.btnSkewLeft.Text = "SkewLeft"
        Me.btnSkewLeft.TextImageRelation = TextImageRelation.ImageBeforeText
        Me.btnSkewLeft.UseVisualStyleBackColor = True
        '			Me.btnSkewLeft.Click += New System.EventHandler(Me.btnSkew_Click)
        ' 
        ' btnFlipHorizontal
        ' 
        Me.btnFlipHorizontal.Image = My.Resources.FlipHorizontal
        Me.btnFlipHorizontal.ImageAlign = ContentAlignment.MiddleLeft
        Me.btnFlipHorizontal.Location = New Point(48, 156)
        Me.btnFlipHorizontal.Name = "btnFlipHorizontal"
        Me.btnFlipHorizontal.Size = New Size(102, 24)
        Me.btnFlipHorizontal.TabIndex = 2
        Me.btnFlipHorizontal.Text = "FlipHorizontal"
        Me.btnFlipHorizontal.TextImageRelation = TextImageRelation.ImageBeforeText
        Me.btnFlipHorizontal.UseVisualStyleBackColor = True
        '			Me.btnFlipHorizontal.Click += New System.EventHandler(Me.btnRotateFlip_Click)
        ' 
        ' btnMicrify
        ' 
        Me.btnMicrify.Image = My.Resources.Micrify
        Me.btnMicrify.ImageAlign = ContentAlignment.MiddleLeft
        Me.btnMicrify.Location = New Point(48, 40)
        Me.btnMicrify.Name = "btnMicrify"
        Me.btnMicrify.Size = New Size(102, 24)
        Me.btnMicrify.TabIndex = 2
        Me.btnMicrify.Text = "Micrify"
        Me.btnMicrify.TextImageRelation = TextImageRelation.ImageBeforeText
        Me.btnMicrify.UseVisualStyleBackColor = True
        '			Me.btnMicrify.Click += New System.EventHandler(Me.btnAmplify_Click)
        ' 
        ' btnRotateLeft
        ' 
        Me.btnRotateLeft.Image = My.Resources.RotateLeft
        Me.btnRotateLeft.ImageAlign = ContentAlignment.MiddleLeft
        Me.btnRotateLeft.Location = New Point(48, 97)
        Me.btnRotateLeft.Name = "btnRotateLeft"
        Me.btnRotateLeft.Size = New Size(102, 24)
        Me.btnRotateLeft.TabIndex = 2
        Me.btnRotateLeft.Text = "RotateLeft"
        Me.btnRotateLeft.TextImageRelation = TextImageRelation.ImageBeforeText
        Me.btnRotateLeft.UseVisualStyleBackColor = True
        '			Me.btnRotateLeft.Click += New System.EventHandler(Me.btnRotateFlip_Click)
        ' 
        ' MainForm
        ' 
        Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = AutoScaleMode.Font
        Me.ClientSize = New Size(829, 541)
        Me.Controls.Add(Me.pnlImage)
        Me.Controls.Add(Me.pnlMenu)
        Me.Name = "MainForm"
        Me.Text = "GDIPlusManipulateImage"
        Me.pnlMenu.ResumeLayout(False)
        Me.pnlMenu.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private pnlMenu As Panel
    Private WithEvents pnlImage As Panel
    Private lbSize As Label
    Private WithEvents btnRotateLeft As Button
    Private lbRotate As Label
    Private WithEvents btnRotateRight As Button
    Private WithEvents btnFlipVertical As Button
    Private WithEvents btnFlipHorizontal As Button
    Private lbFlip As Label
    Private WithEvents btnRotateAngle As Button
    Private tbRotateAngle As TextBox
    Private lbRotateAngle As Label
    Private lbMove As Label
    Private WithEvents btnMoveLeft As Button
    Private WithEvents btnMoveUp As Button
    Private WithEvents btnMoveDown As Button
    Private WithEvents btnMoveRight As Button
    Private WithEvents btnAmplify As Button
    Private WithEvents btnMicrify As Button
    Private WithEvents btnReset As Button
    Private WithEvents chkDrawBounds As CheckBox
    Private lbInterpolationMode As Label
    Private cmbInterpolationMode As ComboBox
    Private WithEvents btnSkewRight As Button
    Private WithEvents btnSkewLeft As Button
    Private label1 As Label
End Class