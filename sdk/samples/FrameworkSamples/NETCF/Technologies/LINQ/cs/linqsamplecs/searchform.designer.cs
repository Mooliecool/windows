namespace Microsoft.Samples.Linq
{
    partial class SearchForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.MainMenu searchMenu;

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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SearchForm));
            this.searchMenu = new System.Windows.Forms.MainMenu();
            this.miExit = new System.Windows.Forms.MenuItem();
            this.miView = new System.Windows.Forms.MenuItem();
            this.treeView = new System.Windows.Forms.TreeView();
            this.chkHighlight = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.txtSearch = new System.Windows.Forms.TextBox();
            this.tvImages = new System.Windows.Forms.ImageList();
            this.SuspendLayout();
            // 
            // searchMenu
            // 
            this.searchMenu.MenuItems.Add(this.miExit);
            this.searchMenu.MenuItems.Add(this.miView);
            // 
            // miExit
            // 
            this.miExit.Text = "Exit";
            this.miExit.Click += new System.EventHandler(this.miExit_Click);
            // 
            // miView
            // 
            this.miView.Text = "Group View";
            this.miView.Click += new System.EventHandler(this.miView_Click);
            // 
            // treeView
            // 
            this.treeView.ImageIndex = 0;
            this.treeView.ImageList = this.tvImages;
            this.treeView.Location = new System.Drawing.Point(3, 65);
            this.treeView.Name = "treeView";
            this.treeView.SelectedImageIndex = 0;
            this.treeView.Size = new System.Drawing.Size(234, 200);
            this.treeView.TabIndex = 1;
            this.treeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView_AfterSelect);
            // 
            // chkHighlight
            // 
            this.chkHighlight.Location = new System.Drawing.Point(7, 36);
            this.chkHighlight.Name = "chkHighlight";
            this.chkHighlight.Size = new System.Drawing.Size(233, 20);
            this.chkHighlight.TabIndex = 6;
            this.chkHighlight.Text = "Highlight All while searching";
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(3, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(77, 20);
            this.label1.Text = "SpotSearch";
            // 
            // txtSearch
            // 
            this.txtSearch.Location = new System.Drawing.Point(86, 12);
            this.txtSearch.Name = "txtSearch";
            this.txtSearch.Size = new System.Drawing.Size(151, 21);
            this.txtSearch.TabIndex = 5;
            this.txtSearch.KeyUp += new System.Windows.Forms.KeyEventHandler(this.txtSearch_KeyUp);
            this.tvImages.Images.Clear();
            this.tvImages.Images.Add(((System.Drawing.Image)(resources.GetObject("resource"))));
            this.tvImages.Images.Add(((System.Drawing.Image)(resources.GetObject("resource1"))));
            // 
            // SearchForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 268);
            this.Controls.Add(this.chkHighlight);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtSearch);
            this.Controls.Add(this.treeView);
            this.KeyPreview = true;
            this.Menu = this.searchMenu;
            this.Name = "SearchForm";
            this.Text = "Search View";
            this.Load += new System.EventHandler(this.SearchForm_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView treeView;
        private System.Windows.Forms.CheckBox chkHighlight;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtSearch;
        private System.Windows.Forms.MenuItem miExit;
        private System.Windows.Forms.MenuItem miView;
        private System.Windows.Forms.ImageList tvImages;
    }
}