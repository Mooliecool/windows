namespace Microsoft.Samples.FTPExplorer
{
    partial class FtpExplorerMainForm
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
            this.menuMain = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addSiteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.createToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.fileActionMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.downloadFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.deleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.directoryActionMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.downloadDirectoryToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.makeDirectoryToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.deleteDirectoryToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.uploadFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.hostActionMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.makeDirectoryToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.uploadFileToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.welComeMessageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.FolderListView = new System.Windows.Forms.TreeView();
            this.FileListView = new System.Windows.Forms.ListView();
            this.FileName = new System.Windows.Forms.ColumnHeader("");
            this.lblNetworkStatus = new System.Windows.Forms.Label();
            this.menuMain.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuMain
            // 
            this.menuMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.fileActionMenu,
            this.directoryActionMenu,
            this.hostActionMenu});
            this.menuMain.Location = new System.Drawing.Point(0, 0);
            this.menuMain.Name = "menuMain";
            this.menuMain.Padding = new System.Windows.Forms.Padding(6, 2, 0, 2);            
            this.menuMain.Size = new System.Drawing.Size(782, 22);
            this.menuMain.TabIndex = 25;
            this.menuMain.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addSiteToolStripMenuItem,
            this.createToolStripMenuItem,
            this.openToolStripMenuItem,
            this.exitToolStripMenuItem1});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // addSiteToolStripMenuItem
            // 
            this.addSiteToolStripMenuItem.Name = "addSiteToolStripMenuItem";
            this.addSiteToolStripMenuItem.Text = "&Add Site";
            this.addSiteToolStripMenuItem.Click += new System.EventHandler(this.menuAddSite_Click);
            // 
            // createToolStripMenuItem
            // 
            this.createToolStripMenuItem.Name = "createToolStripMenuItem";
            this.createToolStripMenuItem.Text = "Save Workspace";
            this.createToolStripMenuItem.Click += new System.EventHandler(this.saveWorkSpaceToolStripMenuItem_Click);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Text = "Open WorkSpace";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openWorkSpaceToolStripMenuItem_Click);
            // 
            // exitToolStripMenuItem1
            // 
            this.exitToolStripMenuItem1.Name = "exitToolStripMenuItem1";
            this.exitToolStripMenuItem1.Text = "Exit";
            this.exitToolStripMenuItem1.Click += new System.EventHandler(this.exitToolStripMenuItem1_Click);
            // 
            // fileActionMenu
            // 
            this.fileActionMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.downloadFileToolStripMenuItem,
            this.deleteToolStripMenuItem});
            this.fileActionMenu.Name = "fileActionMenu";
            this.fileActionMenu.Text = "&File Action";
            this.fileActionMenu.Visible = false;
            // 
            // downloadFileToolStripMenuItem
            // 
            this.downloadFileToolStripMenuItem.Name = "downloadFileToolStripMenuItem";
            this.downloadFileToolStripMenuItem.Text = "Download";
            this.downloadFileToolStripMenuItem.Click += new System.EventHandler(this.menuDownLoad_Click);
            // 
            // deleteToolStripMenuItem
            // 
            this.deleteToolStripMenuItem.Name = "deleteToolStripMenuItem";
            this.deleteToolStripMenuItem.Text = "Delete";
            this.deleteToolStripMenuItem.Click += new System.EventHandler(this.deleteFilesToolStripMenuItem_Click);
            // 
            // directoryActionMenu
            // 
            this.directoryActionMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.downloadDirectoryToolStripMenuItem,
            this.makeDirectoryToolStripMenuItem,
            this.deleteDirectoryToolStripMenuItem,
            this.uploadFileToolStripMenuItem});
            this.directoryActionMenu.Name = "directoryActionMenu";
            this.directoryActionMenu.Text = "&Directory Action";
            this.directoryActionMenu.Visible = false;
            // 
            // downloadDirectoryToolStripMenuItem
            // 
            this.downloadDirectoryToolStripMenuItem.Name = "downloadDirectoryToolStripMenuItem";
            this.downloadDirectoryToolStripMenuItem.Text = "Download Directory";
            this.downloadDirectoryToolStripMenuItem.Click += new System.EventHandler(this.downloadDirectoryToolStripMenuItem_Click);
            // 
            // makeDirectoryToolStripMenuItem
            // 
            this.makeDirectoryToolStripMenuItem.Name = "makeDirectoryToolStripMenuItem";
            this.makeDirectoryToolStripMenuItem.Text = "Make Directory";
            this.makeDirectoryToolStripMenuItem.Click += new System.EventHandler(this.makeDirectoryToolStripMenuItem_Click);
            // 
            // deleteDirectoryToolStripMenuItem
            // 
            this.deleteDirectoryToolStripMenuItem.Name = "deleteDirectoryToolStripMenuItem";
            this.deleteDirectoryToolStripMenuItem.Text = "Delete Directory";
            this.deleteDirectoryToolStripMenuItem.Click += new System.EventHandler(this.deleteDirectoryToolStripMenuItem_Click);
            // 
            // uploadFileToolStripMenuItem
            // 
            this.uploadFileToolStripMenuItem.Name = "uploadFileToolStripMenuItem";
            this.uploadFileToolStripMenuItem.Text = "Upload File";
            this.uploadFileToolStripMenuItem.Click += new System.EventHandler(this.uploadFileToolStripMenuItem_Click);
            // 
            // hostActionMenu
            // 
            this.hostActionMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.makeDirectoryToolStripMenuItem1,
            this.uploadFileToolStripMenuItem1,
            this.welComeMessageToolStripMenuItem});
            this.hostActionMenu.Name = "hostActionMenu";
            this.hostActionMenu.Text = "&Host Action";
            this.hostActionMenu.Visible = false;
            // 
            // makeDirectoryToolStripMenuItem1
            // 
            this.makeDirectoryToolStripMenuItem1.Name = "makeDirectoryToolStripMenuItem1";
            this.makeDirectoryToolStripMenuItem1.Text = "&Make Directory";
            this.makeDirectoryToolStripMenuItem1.Click += new System.EventHandler(this.makeDirectoryToolStripMenuItem_Click);
            // 
            // uploadFileToolStripMenuItem1
            // 
            this.uploadFileToolStripMenuItem1.Name = "uploadFileToolStripMenuItem1";
            this.uploadFileToolStripMenuItem1.Text = "&Upload File";
            this.uploadFileToolStripMenuItem1.Click += new System.EventHandler(this.uploadFileToolStripMenuItem_Click);
            // 
            // welComeMessageToolStripMenuItem
            // 
            this.welComeMessageToolStripMenuItem.Name = "welComeMessageToolStripMenuItem";
            this.welComeMessageToolStripMenuItem.Text = "&Welcome Message";
            this.welComeMessageToolStripMenuItem.Click += new System.EventHandler(this.welComeMessageToolStripMenuItem_Click);
            // 
            // FolderListView
            // 
            this.FolderListView.FullRowSelect = true;
            this.FolderListView.Location = new System.Drawing.Point(0, 24);
            this.FolderListView.Name = "FolderListView";
            this.FolderListView.PathSeparator = "/";
            this.FolderListView.Size = new System.Drawing.Size(248, 439);
            this.FolderListView.TabIndex = 26;
            this.FolderListView.AfterCollapse += new System.Windows.Forms.TreeViewEventHandler(this.FolderListView_AfterCollapse);
            this.FolderListView.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.FolderListView_BeforeExpand);
            this.FolderListView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.FolderListView_AfterSelect);
            this.FolderListView.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.FolderListView_NodeMouseClick);
            // 
            // FileListView
            // 
            this.FileListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.FileName});
            this.FileListView.Location = new System.Drawing.Point(248, 24);
            this.FileListView.Name = "FileListView";
            this.FileListView.Size = new System.Drawing.Size(534, 439);
            this.FileListView.TabIndex = 27;
            this.FileListView.SelectedIndexChanged += new System.EventHandler(this.FileListView_SelectedIndexChanged);
            // 
            // FileName
            // 
            this.FileName.Width = 100;
            // 
            // lblNetworkStatus
            // 
            this.lblNetworkStatus.AutoSize = true;
            this.lblNetworkStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblNetworkStatus.Location = new System.Drawing.Point(0, 469);
            this.lblNetworkStatus.Name = "lblNetworkStatus";
            this.lblNetworkStatus.Size = new System.Drawing.Size(81, 14);
            this.lblNetworkStatus.TabIndex = 28;
            this.lblNetworkStatus.Text = "NetworkStatus";
            // 
            // FtpExplorerMainForm
            // 
            this.ClientSize = new System.Drawing.Size(782, 488);
            this.Controls.Add(this.lblNetworkStatus);
            this.Controls.Add(this.FileListView);
            this.Controls.Add(this.FolderListView);
            this.Controls.Add(this.menuMain);
            this.MaximizeBox = false;
            this.Name = "FtpExplorerMainForm";
            this.Text = ".Net Ftp Explorer";
            this.menuMain.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuMain;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addSiteToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem createToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem fileActionMenu;
        private System.Windows.Forms.ToolStripMenuItem downloadFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem deleteToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem directoryActionMenu;
        private System.Windows.Forms.ToolStripMenuItem downloadDirectoryToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem makeDirectoryToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem deleteDirectoryToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem uploadFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem hostActionMenu;
        private System.Windows.Forms.ToolStripMenuItem makeDirectoryToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem uploadFileToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem welComeMessageToolStripMenuItem;
        private System.Windows.Forms.TreeView FolderListView;
        private System.Windows.Forms.ListView FileListView;
        private System.Windows.Forms.ColumnHeader FileName;
        private System.Windows.Forms.Label lblNetworkStatus;
    }
}