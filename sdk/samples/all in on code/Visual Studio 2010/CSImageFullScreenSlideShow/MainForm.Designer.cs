namespace CSImageFullScreenSlideShow
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
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.pnlSlideShow = new System.Windows.Forms.Panel();
            this.pictureBox = new System.Windows.Forms.PictureBox();
            this.imageFolderBrowserDlg = new System.Windows.Forms.FolderBrowserDialog();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.gbButtons = new System.Windows.Forms.GroupBox();
            this.btnFullScreen = new System.Windows.Forms.Button();
            this.btnSetting = new System.Windows.Forms.Button();
            this.btnImageSlideShow = new System.Windows.Forms.Button();
            this.btnNext = new System.Windows.Forms.Button();
            this.btnOpenFolder = new System.Windows.Forms.Button();
            this.btnPrevious = new System.Windows.Forms.Button();
            this.pnlSlideShow.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            this.gbButtons.SuspendLayout();
            this.SuspendLayout();
            // 
            // pnlSlideShow
            // 
            this.pnlSlideShow.AutoScroll = true;
            this.pnlSlideShow.BackColor = System.Drawing.Color.Black;
            this.pnlSlideShow.Controls.Add(this.pictureBox);
            this.pnlSlideShow.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlSlideShow.Location = new System.Drawing.Point(0, 0);
            this.pnlSlideShow.Name = "pnlSlideShow";
            this.pnlSlideShow.Size = new System.Drawing.Size(448, 248);
            this.pnlSlideShow.TabIndex = 1;
            // 
            // pictureBox
            // 
            this.pictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pictureBox.Location = new System.Drawing.Point(0, 0);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(448, 248);
            this.pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox.TabIndex = 0;
            this.pictureBox.TabStop = false;
            // 
            // timer
            // 
            this.timer.Interval = 1000;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // gbButtons
            // 
            this.gbButtons.Controls.Add(this.btnFullScreen);
            this.gbButtons.Controls.Add(this.btnSetting);
            this.gbButtons.Controls.Add(this.btnImageSlideShow);
            this.gbButtons.Controls.Add(this.btnNext);
            this.gbButtons.Controls.Add(this.btnOpenFolder);
            this.gbButtons.Controls.Add(this.btnPrevious);
            this.gbButtons.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gbButtons.Location = new System.Drawing.Point(0, 248);
            this.gbButtons.Name = "gbButtons";
            this.gbButtons.Size = new System.Drawing.Size(448, 123);
            this.gbButtons.TabIndex = 2;
            this.gbButtons.TabStop = false;
            // 
            // btnFullScreen
            // 
            this.btnFullScreen.Location = new System.Drawing.Point(233, 71);
            this.btnFullScreen.Name = "btnFullScreen";
            this.btnFullScreen.Size = new System.Drawing.Size(208, 23);
            this.btnFullScreen.TabIndex = 16;
            this.btnFullScreen.Text = "Full Screen";
            this.btnFullScreen.UseVisualStyleBackColor = true;
            this.btnFullScreen.Click += new System.EventHandler(this.btnFullScreen_Click);
            // 
            // btnSetting
            // 
            this.btnSetting.Location = new System.Drawing.Point(233, 29);
            this.btnSetting.Name = "btnSetting";
            this.btnSetting.Size = new System.Drawing.Size(96, 23);
            this.btnSetting.TabIndex = 15;
            this.btnSetting.Text = "Settings";
            this.btnSetting.UseVisualStyleBackColor = true;
            this.btnSetting.Click += new System.EventHandler(this.btnSetting_Click);
            // 
            // btnImageSlideShow
            // 
            this.btnImageSlideShow.Location = new System.Drawing.Point(8, 71);
            this.btnImageSlideShow.Name = "btnImageSlideShow";
            this.btnImageSlideShow.Size = new System.Drawing.Size(209, 23);
            this.btnImageSlideShow.TabIndex = 14;
            this.btnImageSlideShow.Text = "Start Slideshow";
            this.btnImageSlideShow.Click += new System.EventHandler(this.btnImageSlideShow_Click);
            // 
            // btnNext
            // 
            this.btnNext.Location = new System.Drawing.Point(345, 29);
            this.btnNext.Name = "btnNext";
            this.btnNext.Size = new System.Drawing.Size(96, 23);
            this.btnNext.TabIndex = 13;
            this.btnNext.Text = "Next";
            this.btnNext.Click += new System.EventHandler(this.btnNext_Click);
            // 
            // btnOpenFolder
            // 
            this.btnOpenFolder.Location = new System.Drawing.Point(121, 29);
            this.btnOpenFolder.Name = "btnOpenFolder";
            this.btnOpenFolder.Size = new System.Drawing.Size(96, 23);
            this.btnOpenFolder.TabIndex = 12;
            this.btnOpenFolder.Text = "Open Folder...";
            this.btnOpenFolder.Click += new System.EventHandler(this.btnOpenFolder_Click);
            // 
            // btnPrevious
            // 
            this.btnPrevious.Location = new System.Drawing.Point(7, 29);
            this.btnPrevious.Name = "btnPrevious";
            this.btnPrevious.Size = new System.Drawing.Size(96, 23);
            this.btnPrevious.TabIndex = 11;
            this.btnPrevious.Text = "Previous";
            this.btnPrevious.Click += new System.EventHandler(this.btnPrevious_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(448, 371);
            this.Controls.Add(this.gbButtons);
            this.Controls.Add(this.pnlSlideShow);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainForm";
            this.Text = "CSImageFullScreenSlideShow";
            this.pnlSlideShow.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
            this.gbButtons.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel pnlSlideShow;
        private System.Windows.Forms.PictureBox pictureBox;
        private System.Windows.Forms.FolderBrowserDialog imageFolderBrowserDlg;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.GroupBox gbButtons;
        private System.Windows.Forms.Button btnFullScreen;
        private System.Windows.Forms.Button btnSetting;
        private System.Windows.Forms.Button btnImageSlideShow;
        private System.Windows.Forms.Button btnNext;
        private System.Windows.Forms.Button btnOpenFolder;
        private System.Windows.Forms.Button btnPrevious;
    }
}

