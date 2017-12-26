namespace CSGDIPlusManipulateImage
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }

            if (disposing && (imgManipulator != null))
            {
                imgManipulator.Dispose();
            }

            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.pnlMenu = new System.Windows.Forms.Panel();
            this.chkDrawBounds = new System.Windows.Forms.CheckBox();
            this.btnReset = new System.Windows.Forms.Button();
            this.btnRotateAngle = new System.Windows.Forms.Button();
            this.tbRotateAngle = new System.Windows.Forms.TextBox();
            this.lbFlip = new System.Windows.Forms.Label();
            this.lbMove = new System.Windows.Forms.Label();
            this.lbRotateAngle = new System.Windows.Forms.Label();
            this.lbRotate = new System.Windows.Forms.Label();
            this.lbSize = new System.Windows.Forms.Label();
            this.pnlImage = new System.Windows.Forms.Panel();
            this.lbInterpolationMode = new System.Windows.Forms.Label();
            this.cmbInterpolationMode = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.btnSkewRight = new System.Windows.Forms.Button();
            this.btnFlipVertical = new System.Windows.Forms.Button();
            this.btnAmplify = new System.Windows.Forms.Button();
            this.btnRotateRight = new System.Windows.Forms.Button();
            this.btnMoveLeft = new System.Windows.Forms.Button();
            this.btnMoveUp = new System.Windows.Forms.Button();
            this.btnMoveDown = new System.Windows.Forms.Button();
            this.btnMoveRight = new System.Windows.Forms.Button();
            this.btnSkewLeft = new System.Windows.Forms.Button();
            this.btnFlipHorizontal = new System.Windows.Forms.Button();
            this.btnMicrify = new System.Windows.Forms.Button();
            this.btnRotateLeft = new System.Windows.Forms.Button();
            this.pnlMenu.SuspendLayout();
            this.SuspendLayout();
            // 
            // pnlMenu
            // 
            this.pnlMenu.Controls.Add(this.cmbInterpolationMode);
            this.pnlMenu.Controls.Add(this.chkDrawBounds);
            this.pnlMenu.Controls.Add(this.btnReset);
            this.pnlMenu.Controls.Add(this.btnRotateAngle);
            this.pnlMenu.Controls.Add(this.tbRotateAngle);
            this.pnlMenu.Controls.Add(this.btnSkewRight);
            this.pnlMenu.Controls.Add(this.btnFlipVertical);
            this.pnlMenu.Controls.Add(this.btnAmplify);
            this.pnlMenu.Controls.Add(this.btnRotateRight);
            this.pnlMenu.Controls.Add(this.btnMoveLeft);
            this.pnlMenu.Controls.Add(this.btnMoveUp);
            this.pnlMenu.Controls.Add(this.btnMoveDown);
            this.pnlMenu.Controls.Add(this.btnMoveRight);
            this.pnlMenu.Controls.Add(this.btnSkewLeft);
            this.pnlMenu.Controls.Add(this.btnFlipHorizontal);
            this.pnlMenu.Controls.Add(this.btnMicrify);
            this.pnlMenu.Controls.Add(this.btnRotateLeft);
            this.pnlMenu.Controls.Add(this.label1);
            this.pnlMenu.Controls.Add(this.lbFlip);
            this.pnlMenu.Controls.Add(this.lbMove);
            this.pnlMenu.Controls.Add(this.lbRotateAngle);
            this.pnlMenu.Controls.Add(this.lbRotate);
            this.pnlMenu.Controls.Add(this.lbInterpolationMode);
            this.pnlMenu.Controls.Add(this.lbSize);
            this.pnlMenu.Dock = System.Windows.Forms.DockStyle.Left;
            this.pnlMenu.Location = new System.Drawing.Point(0, 0);
            this.pnlMenu.Name = "pnlMenu";
            this.pnlMenu.Size = new System.Drawing.Size(269, 541);
            this.pnlMenu.TabIndex = 0;
            // 
            // chkDrawBounds
            // 
            this.chkDrawBounds.AutoSize = true;
            this.chkDrawBounds.Location = new System.Drawing.Point(13, 461);
            this.chkDrawBounds.Name = "chkDrawBounds";
            this.chkDrawBounds.Size = new System.Drawing.Size(90, 17);
            this.chkDrawBounds.TabIndex = 6;
            this.chkDrawBounds.Text = "Draw Bounds";
            this.chkDrawBounds.UseVisualStyleBackColor = true;
            this.chkDrawBounds.CheckedChanged += new System.EventHandler(this.chkDrawBounds_CheckedChanged);
            // 
            // btnReset
            // 
            this.btnReset.Location = new System.Drawing.Point(13, 506);
            this.btnReset.Name = "btnReset";
            this.btnReset.Size = new System.Drawing.Size(75, 23);
            this.btnReset.TabIndex = 5;
            this.btnReset.Text = "Reset";
            this.btnReset.UseVisualStyleBackColor = true;
            this.btnReset.Click += new System.EventHandler(this.btnReset_Click);
            // 
            // btnRotateAngle
            // 
            this.btnRotateAngle.Location = new System.Drawing.Point(213, 307);
            this.btnRotateAngle.Name = "btnRotateAngle";
            this.btnRotateAngle.Size = new System.Drawing.Size(42, 23);
            this.btnRotateAngle.TabIndex = 4;
            this.btnRotateAngle.Text = "OK";
            this.btnRotateAngle.UseVisualStyleBackColor = true;
            this.btnRotateAngle.Click += new System.EventHandler(this.btnRotateAngle_Click);
            // 
            // tbRotateAngle
            // 
            this.tbRotateAngle.Location = new System.Drawing.Point(80, 309);
            this.tbRotateAngle.Name = "tbRotateAngle";
            this.tbRotateAngle.Size = new System.Drawing.Size(127, 20);
            this.tbRotateAngle.TabIndex = 3;
            this.tbRotateAngle.Text = "10";
            // 
            // lbFlip
            // 
            this.lbFlip.AutoSize = true;
            this.lbFlip.Location = new System.Drawing.Point(4, 162);
            this.lbFlip.Name = "lbFlip";
            this.lbFlip.Size = new System.Drawing.Size(23, 13);
            this.lbFlip.TabIndex = 0;
            this.lbFlip.Text = "Flip";
            // 
            // lbMove
            // 
            this.lbMove.AutoSize = true;
            this.lbMove.Location = new System.Drawing.Point(3, 379);
            this.lbMove.Name = "lbMove";
            this.lbMove.Size = new System.Drawing.Size(34, 13);
            this.lbMove.TabIndex = 0;
            this.lbMove.Text = "Move";
            // 
            // lbRotateAngle
            // 
            this.lbRotateAngle.AutoSize = true;
            this.lbRotateAngle.Location = new System.Drawing.Point(4, 313);
            this.lbRotateAngle.Name = "lbRotateAngle";
            this.lbRotateAngle.Size = new System.Drawing.Size(69, 13);
            this.lbRotateAngle.TabIndex = 0;
            this.lbRotateAngle.Text = "Rotate Angle";
            // 
            // lbRotate
            // 
            this.lbRotate.AutoSize = true;
            this.lbRotate.Location = new System.Drawing.Point(4, 103);
            this.lbRotate.Name = "lbRotate";
            this.lbRotate.Size = new System.Drawing.Size(39, 13);
            this.lbRotate.TabIndex = 0;
            this.lbRotate.Text = "Rotate";
            // 
            // lbSize
            // 
            this.lbSize.AutoSize = true;
            this.lbSize.Location = new System.Drawing.Point(4, 46);
            this.lbSize.Name = "lbSize";
            this.lbSize.Size = new System.Drawing.Size(27, 13);
            this.lbSize.TabIndex = 0;
            this.lbSize.Text = "Size";
            // 
            // pnlImage
            // 
            this.pnlImage.BackColor = System.Drawing.SystemColors.ControlDark;
            this.pnlImage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlImage.Location = new System.Drawing.Point(269, 0);
            this.pnlImage.Name = "pnlImage";
            this.pnlImage.Size = new System.Drawing.Size(560, 541);
            this.pnlImage.TabIndex = 1;
            this.pnlImage.Paint += new System.Windows.Forms.PaintEventHandler(this.pnlImage_Paint);
            // 
            // lbInterpolationMode
            // 
            this.lbInterpolationMode.AutoSize = true;
            this.lbInterpolationMode.Location = new System.Drawing.Point(4, 18);
            this.lbInterpolationMode.Name = "lbInterpolationMode";
            this.lbInterpolationMode.Size = new System.Drawing.Size(92, 13);
            this.lbInterpolationMode.TabIndex = 0;
            this.lbInterpolationMode.Text = "InterpolationMode";
            // 
            // cmbInterpolationMode
            // 
            this.cmbInterpolationMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbInterpolationMode.FormattingEnabled = true;
            this.cmbInterpolationMode.Location = new System.Drawing.Point(96, 13);
            this.cmbInterpolationMode.Name = "cmbInterpolationMode";
            this.cmbInterpolationMode.Size = new System.Drawing.Size(158, 21);
            this.cmbInterpolationMode.TabIndex = 7;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(4, 240);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(34, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Skew";
            // 
            // btnSkewRight
            // 
            this.btnSkewRight.Image = global::CSGDIPlusManipulateImage.Properties.Resources.SkewRight;
            this.btnSkewRight.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btnSkewRight.Location = new System.Drawing.Point(156, 234);
            this.btnSkewRight.Name = "btnSkewRight";
            this.btnSkewRight.Size = new System.Drawing.Size(98, 24);
            this.btnSkewRight.TabIndex = 2;
            this.btnSkewRight.Text = "SkewRight";
            this.btnSkewRight.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnSkewRight.UseVisualStyleBackColor = true;
            this.btnSkewRight.Click += new System.EventHandler(this.btnSkew_Click);
            // 
            // btnFlipVertical
            // 
            this.btnFlipVertical.Image = global::CSGDIPlusManipulateImage.Properties.Resources.FlipVertical;
            this.btnFlipVertical.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btnFlipVertical.Location = new System.Drawing.Point(156, 156);
            this.btnFlipVertical.Name = "btnFlipVertical";
            this.btnFlipVertical.Size = new System.Drawing.Size(98, 24);
            this.btnFlipVertical.TabIndex = 2;
            this.btnFlipVertical.Text = "FlipVertical";
            this.btnFlipVertical.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnFlipVertical.UseVisualStyleBackColor = true;
            this.btnFlipVertical.Click += new System.EventHandler(this.btnRotateFlip_Click);
            // 
            // btnAmplify
            // 
            this.btnAmplify.Image = global::CSGDIPlusManipulateImage.Properties.Resources.Amplify;
            this.btnAmplify.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btnAmplify.Location = new System.Drawing.Point(156, 40);
            this.btnAmplify.Name = "btnAmplify";
            this.btnAmplify.Size = new System.Drawing.Size(98, 24);
            this.btnAmplify.TabIndex = 2;
            this.btnAmplify.Text = "Amplify";
            this.btnAmplify.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnAmplify.UseVisualStyleBackColor = true;
            this.btnAmplify.Click += new System.EventHandler(this.btnAmplify_Click);
            // 
            // btnRotateRight
            // 
            this.btnRotateRight.Image = global::CSGDIPlusManipulateImage.Properties.Resources.RotateRight;
            this.btnRotateRight.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btnRotateRight.Location = new System.Drawing.Point(156, 97);
            this.btnRotateRight.Name = "btnRotateRight";
            this.btnRotateRight.Size = new System.Drawing.Size(98, 24);
            this.btnRotateRight.TabIndex = 2;
            this.btnRotateRight.Text = "RotateRight";
            this.btnRotateRight.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnRotateRight.UseVisualStyleBackColor = true;
            this.btnRotateRight.Click += new System.EventHandler(this.btnRotateFlip_Click);
            // 
            // btnMoveLeft
            // 
            this.btnMoveLeft.Image = global::CSGDIPlusManipulateImage.Properties.Resources.MoveLeft;
            this.btnMoveLeft.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btnMoveLeft.Location = new System.Drawing.Point(48, 373);
            this.btnMoveLeft.Name = "btnMoveLeft";
            this.btnMoveLeft.Size = new System.Drawing.Size(86, 24);
            this.btnMoveLeft.TabIndex = 2;
            this.btnMoveLeft.Text = "MoveLeft";
            this.btnMoveLeft.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnMoveLeft.UseVisualStyleBackColor = true;
            this.btnMoveLeft.Click += new System.EventHandler(this.btnMove_Click);
            // 
            // btnMoveUp
            // 
            this.btnMoveUp.Image = global::CSGDIPlusManipulateImage.Properties.Resources.MoveUp;
            this.btnMoveUp.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btnMoveUp.Location = new System.Drawing.Point(96, 347);
            this.btnMoveUp.Name = "btnMoveUp";
            this.btnMoveUp.Size = new System.Drawing.Size(86, 24);
            this.btnMoveUp.TabIndex = 2;
            this.btnMoveUp.Text = "MoveUp";
            this.btnMoveUp.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnMoveUp.UseVisualStyleBackColor = true;
            this.btnMoveUp.Click += new System.EventHandler(this.btnMove_Click);
            // 
            // btnMoveDown
            // 
            this.btnMoveDown.Image = global::CSGDIPlusManipulateImage.Properties.Resources.MoveDown;
            this.btnMoveDown.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btnMoveDown.Location = new System.Drawing.Point(96, 398);
            this.btnMoveDown.Name = "btnMoveDown";
            this.btnMoveDown.Size = new System.Drawing.Size(86, 24);
            this.btnMoveDown.TabIndex = 2;
            this.btnMoveDown.Text = "MoveDown";
            this.btnMoveDown.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnMoveDown.UseVisualStyleBackColor = true;
            this.btnMoveDown.Click += new System.EventHandler(this.btnMove_Click);
            // 
            // btnMoveRight
            // 
            this.btnMoveRight.Image = global::CSGDIPlusManipulateImage.Properties.Resources.MoveRight;
            this.btnMoveRight.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btnMoveRight.Location = new System.Drawing.Point(143, 373);
            this.btnMoveRight.Name = "btnMoveRight";
            this.btnMoveRight.Size = new System.Drawing.Size(85, 24);
            this.btnMoveRight.TabIndex = 2;
            this.btnMoveRight.Text = "MoveRight";
            this.btnMoveRight.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnMoveRight.UseVisualStyleBackColor = true;
            this.btnMoveRight.Click += new System.EventHandler(this.btnMove_Click);
            // 
            // btnSkewLeft
            // 
            this.btnSkewLeft.Image = global::CSGDIPlusManipulateImage.Properties.Resources.SkewLeft;
            this.btnSkewLeft.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btnSkewLeft.Location = new System.Drawing.Point(48, 234);
            this.btnSkewLeft.Name = "btnSkewLeft";
            this.btnSkewLeft.Size = new System.Drawing.Size(102, 24);
            this.btnSkewLeft.TabIndex = 2;
            this.btnSkewLeft.Text = "SkewLeft";
            this.btnSkewLeft.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnSkewLeft.UseVisualStyleBackColor = true;
            this.btnSkewLeft.Click += new System.EventHandler(this.btnSkew_Click);
            // 
            // btnFlipHorizontal
            // 
            this.btnFlipHorizontal.Image = global::CSGDIPlusManipulateImage.Properties.Resources.FlipHorizontal;
            this.btnFlipHorizontal.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btnFlipHorizontal.Location = new System.Drawing.Point(48, 156);
            this.btnFlipHorizontal.Name = "btnFlipHorizontal";
            this.btnFlipHorizontal.Size = new System.Drawing.Size(102, 24);
            this.btnFlipHorizontal.TabIndex = 2;
            this.btnFlipHorizontal.Text = "FlipHorizontal";
            this.btnFlipHorizontal.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnFlipHorizontal.UseVisualStyleBackColor = true;
            this.btnFlipHorizontal.Click += new System.EventHandler(this.btnRotateFlip_Click);
            // 
            // btnMicrify
            // 
            this.btnMicrify.Image = global::CSGDIPlusManipulateImage.Properties.Resources.Micrify;
            this.btnMicrify.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btnMicrify.Location = new System.Drawing.Point(48, 40);
            this.btnMicrify.Name = "btnMicrify";
            this.btnMicrify.Size = new System.Drawing.Size(102, 24);
            this.btnMicrify.TabIndex = 2;
            this.btnMicrify.Text = "Micrify";
            this.btnMicrify.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnMicrify.UseVisualStyleBackColor = true;
            this.btnMicrify.Click += new System.EventHandler(this.btnAmplify_Click);
            // 
            // btnRotateLeft
            // 
            this.btnRotateLeft.Image = global::CSGDIPlusManipulateImage.Properties.Resources.RotateLeft;
            this.btnRotateLeft.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btnRotateLeft.Location = new System.Drawing.Point(48, 97);
            this.btnRotateLeft.Name = "btnRotateLeft";
            this.btnRotateLeft.Size = new System.Drawing.Size(102, 24);
            this.btnRotateLeft.TabIndex = 2;
            this.btnRotateLeft.Text = "RotateLeft";
            this.btnRotateLeft.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnRotateLeft.UseVisualStyleBackColor = true;
            this.btnRotateLeft.Click += new System.EventHandler(this.btnRotateFlip_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(829, 541);
            this.Controls.Add(this.pnlImage);
            this.Controls.Add(this.pnlMenu);
            this.Name = "MainForm";
            this.Text = "GDIPlusManipulateImage";
            this.pnlMenu.ResumeLayout(false);
            this.pnlMenu.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel pnlMenu;
        private System.Windows.Forms.Panel pnlImage;
        private System.Windows.Forms.Label lbSize;
        private System.Windows.Forms.Button btnRotateLeft;
        private System.Windows.Forms.Label lbRotate;
        private System.Windows.Forms.Button btnRotateRight;
        private System.Windows.Forms.Button btnFlipVertical;
        private System.Windows.Forms.Button btnFlipHorizontal;
        private System.Windows.Forms.Label lbFlip;
        private System.Windows.Forms.Button btnRotateAngle;
        private System.Windows.Forms.TextBox tbRotateAngle;
        private System.Windows.Forms.Label lbRotateAngle;
        private System.Windows.Forms.Label lbMove;
        private System.Windows.Forms.Button btnMoveLeft;
        private System.Windows.Forms.Button btnMoveUp;
        private System.Windows.Forms.Button btnMoveDown;
        private System.Windows.Forms.Button btnMoveRight;
        private System.Windows.Forms.Button btnAmplify;
        private System.Windows.Forms.Button btnMicrify;
        private System.Windows.Forms.Button btnReset;
        private System.Windows.Forms.CheckBox chkDrawBounds;
        private System.Windows.Forms.Label lbInterpolationMode;
        private System.Windows.Forms.ComboBox cmbInterpolationMode;
        private System.Windows.Forms.Button btnSkewRight;
        private System.Windows.Forms.Button btnSkewLeft;
        private System.Windows.Forms.Label label1;
    }
}

