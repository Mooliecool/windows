namespace CSWin7TaskbarThumbnail
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
            System.Windows.Forms.ListViewItem listViewItem1 = new System.Windows.Forms.ListViewItem("", 0);
            System.Windows.Forms.ListViewItem listViewItem2 = new System.Windows.Forms.ListViewItem("", 1);
            System.Windows.Forms.ListViewItem listViewItem3 = new System.Windows.Forms.ListViewItem("", 2);
            System.Windows.Forms.ListViewItem listViewItem4 = new System.Windows.Forms.ListViewItem("", 3);
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.addThumbnailButton = new System.Windows.Forms.Button();
            this.removeThumbnailButton = new System.Windows.Forms.Button();
            this.addThumbnailButton2 = new System.Windows.Forms.Button();
            this.removeThumbnailButton2 = new System.Windows.Forms.Button();
            this.thumbnailTabControl = new System.Windows.Forms.TabControl();
            this.thumbnailPreviewTabPage = new System.Windows.Forms.TabPage();
            this.changePreviewOrderButton = new System.Windows.Forms.Button();
            this.previewPictureBox1 = new System.Windows.Forms.PictureBox();
            this.previewPictureBox2 = new System.Windows.Forms.PictureBox();
            this.thumbnailToolbarTabPage = new System.Windows.Forms.TabPage();
            this.imagePictureBox = new System.Windows.Forms.PictureBox();
            this.imageListView = new System.Windows.Forms.ListView();
            this.imageList = new System.Windows.Forms.ImageList(this.components);
            this.thumbnailTabControl.SuspendLayout();
            this.thumbnailPreviewTabPage.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.previewPictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.previewPictureBox2)).BeginInit();
            this.thumbnailToolbarTabPage.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.imagePictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // addThumbnailButton
            // 
            this.addThumbnailButton.Location = new System.Drawing.Point(42, 191);
            this.addThumbnailButton.Name = "addThumbnailButton";
            this.addThumbnailButton.Size = new System.Drawing.Size(121, 23);
            this.addThumbnailButton.TabIndex = 1;
            this.addThumbnailButton.Text = "Add as Thumbnail";
            this.addThumbnailButton.UseVisualStyleBackColor = true;
            this.addThumbnailButton.Click += new System.EventHandler(this.addThumbnailButton_Click);
            // 
            // removeThumbnailButton
            // 
            this.removeThumbnailButton.Location = new System.Drawing.Point(42, 220);
            this.removeThumbnailButton.Name = "removeThumbnailButton";
            this.removeThumbnailButton.Size = new System.Drawing.Size(121, 23);
            this.removeThumbnailButton.TabIndex = 2;
            this.removeThumbnailButton.Text = "Remove Thumbnail";
            this.removeThumbnailButton.UseVisualStyleBackColor = true;
            this.removeThumbnailButton.Click += new System.EventHandler(this.removeThumbnailButton_Click);
            // 
            // addThumbnailButton2
            // 
            this.addThumbnailButton2.Location = new System.Drawing.Point(337, 191);
            this.addThumbnailButton2.Name = "addThumbnailButton2";
            this.addThumbnailButton2.Size = new System.Drawing.Size(121, 23);
            this.addThumbnailButton2.TabIndex = 4;
            this.addThumbnailButton2.Text = "Add as Thumbnail";
            this.addThumbnailButton2.UseVisualStyleBackColor = true;
            this.addThumbnailButton2.Click += new System.EventHandler(this.addThumbnailButton2_Click);
            // 
            // removeThumbnailButton2
            // 
            this.removeThumbnailButton2.Location = new System.Drawing.Point(337, 220);
            this.removeThumbnailButton2.Name = "removeThumbnailButton2";
            this.removeThumbnailButton2.Size = new System.Drawing.Size(121, 23);
            this.removeThumbnailButton2.TabIndex = 5;
            this.removeThumbnailButton2.Text = "Remove Thumbnail";
            this.removeThumbnailButton2.UseVisualStyleBackColor = true;
            this.removeThumbnailButton2.Click += new System.EventHandler(this.removeThumbnailButton2_Click);
            // 
            // thumbnailTabControl
            // 
            this.thumbnailTabControl.Controls.Add(this.thumbnailPreviewTabPage);
            this.thumbnailTabControl.Controls.Add(this.thumbnailToolbarTabPage);
            this.thumbnailTabControl.Location = new System.Drawing.Point(29, 22);
            this.thumbnailTabControl.Name = "thumbnailTabControl";
            this.thumbnailTabControl.SelectedIndex = 0;
            this.thumbnailTabControl.Size = new System.Drawing.Size(587, 301);
            this.thumbnailTabControl.TabIndex = 6;
            this.thumbnailTabControl.SelectedIndexChanged += new System.EventHandler(this.thumbnailTabControl_SelectedIndexChanged);
            // 
            // thumbnailPreviewTabPage
            // 
            this.thumbnailPreviewTabPage.Controls.Add(this.changePreviewOrderButton);
            this.thumbnailPreviewTabPage.Controls.Add(this.removeThumbnailButton);
            this.thumbnailPreviewTabPage.Controls.Add(this.addThumbnailButton2);
            this.thumbnailPreviewTabPage.Controls.Add(this.removeThumbnailButton2);
            this.thumbnailPreviewTabPage.Controls.Add(this.addThumbnailButton);
            this.thumbnailPreviewTabPage.Controls.Add(this.previewPictureBox1);
            this.thumbnailPreviewTabPage.Controls.Add(this.previewPictureBox2);
            this.thumbnailPreviewTabPage.Location = new System.Drawing.Point(4, 22);
            this.thumbnailPreviewTabPage.Name = "thumbnailPreviewTabPage";
            this.thumbnailPreviewTabPage.Padding = new System.Windows.Forms.Padding(3);
            this.thumbnailPreviewTabPage.Size = new System.Drawing.Size(579, 275);
            this.thumbnailPreviewTabPage.TabIndex = 0;
            this.thumbnailPreviewTabPage.Text = "Thumbnail Preview";
            this.thumbnailPreviewTabPage.UseVisualStyleBackColor = true;
            // 
            // changePreviewOrderButton
            // 
            this.changePreviewOrderButton.Location = new System.Drawing.Point(198, 191);
            this.changePreviewOrderButton.Name = "changePreviewOrderButton";
            this.changePreviewOrderButton.Size = new System.Drawing.Size(122, 23);
            this.changePreviewOrderButton.TabIndex = 6;
            this.changePreviewOrderButton.Text = "Change Preview Order";
            this.changePreviewOrderButton.UseVisualStyleBackColor = true;
            this.changePreviewOrderButton.Click += new System.EventHandler(this.changePreviewOrderButton_Click);
            // 
            // previewPictureBox1
            // 
            this.previewPictureBox1.Image = global::CSWin7TaskbarThumbnail.Properties.Resources.DotNet2;
            this.previewPictureBox1.Location = new System.Drawing.Point(33, 6);
            this.previewPictureBox1.Name = "previewPictureBox1";
            this.previewPictureBox1.Size = new System.Drawing.Size(164, 161);
            this.previewPictureBox1.TabIndex = 0;
            this.previewPictureBox1.TabStop = false;
            // 
            // previewPictureBox2
            // 
            this.previewPictureBox2.Image = global::CSWin7TaskbarThumbnail.Properties.Resources.DotNet1;
            this.previewPictureBox2.Location = new System.Drawing.Point(260, 64);
            this.previewPictureBox2.Name = "previewPictureBox2";
            this.previewPictureBox2.Size = new System.Drawing.Size(248, 63);
            this.previewPictureBox2.TabIndex = 3;
            this.previewPictureBox2.TabStop = false;
            // 
            // thumbnailToolbarTabPage
            // 
            this.thumbnailToolbarTabPage.Controls.Add(this.imagePictureBox);
            this.thumbnailToolbarTabPage.Controls.Add(this.imageListView);
            this.thumbnailToolbarTabPage.Location = new System.Drawing.Point(4, 22);
            this.thumbnailToolbarTabPage.Name = "thumbnailToolbarTabPage";
            this.thumbnailToolbarTabPage.Padding = new System.Windows.Forms.Padding(3);
            this.thumbnailToolbarTabPage.Size = new System.Drawing.Size(579, 275);
            this.thumbnailToolbarTabPage.TabIndex = 1;
            this.thumbnailToolbarTabPage.Text = "Thumbnail Toolbar";
            this.thumbnailToolbarTabPage.UseVisualStyleBackColor = true;
            // 
            // imagePictureBox
            // 
            this.imagePictureBox.Location = new System.Drawing.Point(238, 57);
            this.imagePictureBox.Name = "imagePictureBox";
            this.imagePictureBox.Size = new System.Drawing.Size(45, 50);
            this.imagePictureBox.TabIndex = 1;
            this.imagePictureBox.TabStop = false;
            // 
            // imageListView
            // 
            this.imageListView.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem1,
            listViewItem2,
            listViewItem3,
            listViewItem4});
            this.imageListView.LargeImageList = this.imageList;
            this.imageListView.Location = new System.Drawing.Point(115, 154);
            this.imageListView.MultiSelect = false;
            this.imageListView.Name = "imageListView";
            this.imageListView.Size = new System.Drawing.Size(306, 56);
            this.imageListView.TabIndex = 0;
            this.imageListView.UseCompatibleStateImageBehavior = false;
            this.imageListView.SelectedIndexChanged += new System.EventHandler(this.imageListView_SelectedIndexChanged);
            // 
            // imageList
            // 
            this.imageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList.ImageStream")));
            this.imageList.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList.Images.SetKeyName(0, "C.JPG");
            this.imageList.Images.SetKeyName(1, "O.JPG");
            this.imageList.Images.SetKeyName(2, "D.JPG");
            this.imageList.Images.SetKeyName(3, "E.JPG");
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(653, 357);
            this.Controls.Add(this.thumbnailTabControl);
            this.Name = "MainForm";
            this.Text = "Win7 Taskbar Thumbnail";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.thumbnailTabControl.ResumeLayout(false);
            this.thumbnailPreviewTabPage.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.previewPictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.previewPictureBox2)).EndInit();
            this.thumbnailToolbarTabPage.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.imagePictureBox)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PictureBox previewPictureBox1;
        private System.Windows.Forms.Button addThumbnailButton;
        private System.Windows.Forms.Button removeThumbnailButton;
        private System.Windows.Forms.PictureBox previewPictureBox2;
        private System.Windows.Forms.Button addThumbnailButton2;
        private System.Windows.Forms.Button removeThumbnailButton2;
        private System.Windows.Forms.TabControl thumbnailTabControl;
        private System.Windows.Forms.TabPage thumbnailPreviewTabPage;
        private System.Windows.Forms.TabPage thumbnailToolbarTabPage;
        private System.Windows.Forms.Button changePreviewOrderButton;
        private System.Windows.Forms.ListView imageListView;
        private System.Windows.Forms.ImageList imageList;
        private System.Windows.Forms.PictureBox imagePictureBox;
    }
}

