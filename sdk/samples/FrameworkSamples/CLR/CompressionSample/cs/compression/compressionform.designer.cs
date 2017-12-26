namespace Microsoft.Sample.Compression
{
	public partial class CompressionForm : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CompressionForm));
			this.mainToolStrip = new System.Windows.Forms.ToolStrip();
			this.newGzipToolStripButton = new System.Windows.Forms.ToolStripButton();
			this.newDeflateToolStripButton = new System.Windows.Forms.ToolStripButton();
			this.openToolStripButton = new System.Windows.Forms.ToolStripButton();
			this.newToolStripSeparator = new System.Windows.Forms.ToolStripSeparator();
			this.addToolStripButton = new System.Windows.Forms.ToolStripButton();
			this.removeToolStripButton = new System.Windows.Forms.ToolStripButton();
			this.extractToolStripButton = new System.Windows.Forms.ToolStripButton();
			this.addToolStripSeparator = new System.Windows.Forms.ToolStripSeparator();
			this.newGzipMenuToolStripButton = new System.Windows.Forms.ToolStripMenuItem();
			this.newDeflateMenuToolStripButton = new System.Windows.Forms.ToolStripMenuItem();
			this.fileListView = new System.Windows.Forms.ListView();
			this.mainMenuStrip = new System.Windows.Forms.MenuStrip();
			this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.openMenuStripButton = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
			this.closeMenuStripButton = new System.Windows.Forms.ToolStripMenuItem();
			this.actionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.addMenuStripButton = new System.Windows.Forms.ToolStripMenuItem();
			this.removeMenuStripButton = new System.Windows.Forms.ToolStripMenuItem();
			this.extractMenuStripButton = new System.Windows.Forms.ToolStripMenuItem();
			this.extractAllMenuStripButton = new System.Windows.Forms.ToolStripMenuItem();
			this.fileContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.contextMenuStripButton = new System.Windows.Forms.ToolStripMenuItem();
			this.removeContextMenuStripButton = new System.Windows.Forms.ToolStripMenuItem();
			this.mainStatusStrip = new System.Windows.Forms.StatusStrip();
			this.mainToolStrip.SuspendLayout();
			this.mainMenuStrip.SuspendLayout();
			this.fileContextMenuStrip.SuspendLayout();
			this.SuspendLayout();
			// 
			// mainToolStrip
			// 
			this.mainToolStrip.Anchor = System.Windows.Forms.AnchorStyles.Right;
			this.mainToolStrip.Dock = System.Windows.Forms.DockStyle.None;
			this.mainToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newGzipToolStripButton,
            this.newDeflateToolStripButton,
            this.openToolStripButton,
            this.newToolStripSeparator,
            this.addToolStripButton,
            this.removeToolStripButton,
            this.extractToolStripButton,
            this.addToolStripSeparator});
			this.mainToolStrip.Location = new System.Drawing.Point(0, 0);
			this.mainToolStrip.Name = "mainToolStrip";
			this.mainToolStrip.TabIndex = 1;
			this.mainToolStrip.Text = "toolStrip1";
			// 
			// newGzipToolStripButton
			// 
            this.newGzipToolStripButton.Image = Resource1.NewGzipToolStripMenuItem_Image;
			this.newGzipToolStripButton.Name = "newGzipToolStripButton";
			this.newGzipToolStripButton.Text = "New Gzip";
			this.newGzipToolStripButton.Click += new System.EventHandler(this.newGzipToolStripMenuItem_Click);
			// 
			// newDeflateToolStripButton
			// 
            this.newDeflateToolStripButton.Image = Resource1.NewDeflateToolStripMenuItem_Image;
			this.newDeflateToolStripButton.Name = "newDeflateToolStripButton";
			this.newDeflateToolStripButton.Text = "New Deflate";
			this.newDeflateToolStripButton.Click += new System.EventHandler(this.newDeflateToolStripMenuItem_Click);
			// 
			// openToolStripButton
			// 
            this.openToolStripButton.Image = Resource1.OpenToolStripMenuItem_Image;
			this.openToolStripButton.Name = "openToolStripButton";
			this.openToolStripButton.Text = "Open";
			this.openToolStripButton.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
			// 
			// newToolStripSeparator
			// 
			this.newToolStripSeparator.Name = "newToolStripSeparator";
			// 
			// addToolStripButton
			// 
            this.addToolStripButton.Image = Resource1.OpenToolStripMenuItem_Image;
			this.addToolStripButton.Name = "addToolStripButton";
			this.addToolStripButton.Text = "Add";
			this.addToolStripButton.Click += new System.EventHandler(this.addToolStripMenuItem_Click);
			// 
			// removeToolStripButton
			// 
            this.removeToolStripButton.Image = Resource1.CutToolStripMenuItem_Image;
			this.removeToolStripButton.Name = "removeToolStripButton";
			this.removeToolStripButton.Text = "Remove";
			this.removeToolStripButton.Click += new System.EventHandler(this.removeToolStripMenuItem_Click);
			// 
			// extractToolStripButton
			// 
            this.extractToolStripButton.Image = Resource1.UndoToolStripMenuItem_Image;
			this.extractToolStripButton.Name = "extractToolStripButton";
			this.extractToolStripButton.Text = "Extract";
			this.extractToolStripButton.Click += new System.EventHandler(this.extractToolStripMenuItem_Click);
			// 
			// addToolStripSeparator
			// 
			this.addToolStripSeparator.Name = "addToolStripSeparator";
			// 
			// newGzipMenuToolStripButton
			// 
            this.newGzipMenuToolStripButton.Image = Resource1.NewGzipToolStripMenuItem_Image;
			this.newGzipMenuToolStripButton.Name = "newGzipMenuToolStripButton";
			this.newGzipMenuToolStripButton.Text = "New Gzip";
			this.newGzipMenuToolStripButton.Click += new System.EventHandler(this.newGzipToolStripMenuItem_Click);
			// 
			// newDeflateMenuToolStripButton
			// 
            this.newDeflateMenuToolStripButton.Image = Resource1.NewDeflateToolStripMenuItem_Image;
			this.newDeflateMenuToolStripButton.ImageTransparentColor = System.Drawing.Color.Transparent;
			this.newDeflateMenuToolStripButton.Name = "newDeflateMenuToolStripButton";
			this.newDeflateMenuToolStripButton.Text = "New Deflate";
			this.newDeflateMenuToolStripButton.Click += new System.EventHandler(this.newDeflateToolStripMenuItem_Click);
			// 
			// fileListView
			// 
			this.fileListView.Dock = System.Windows.Forms.DockStyle.Fill;
			this.fileListView.FullRowSelect = true;
			this.fileListView.Location = new System.Drawing.Point(9, 33);
			this.fileListView.Name = "fileListView";
			this.fileListView.ShowItemToolTips = true;
			this.fileListView.Size = new System.Drawing.Size(769, 524);
			this.fileListView.TabIndex = 6;
			this.fileListView.SelectedIndexChanged += new System.EventHandler(this.fileListView_SelectedIndexChanged);
			this.fileListView.MouseUp += new System.Windows.Forms.MouseEventHandler(this.fileListView_MouseUp);
			// 
			// mainMenuStrip
			// 
			this.mainMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.actionsToolStripMenuItem});
			this.mainMenuStrip.Location = new System.Drawing.Point(9, 9);
			this.mainMenuStrip.Name = "mainMenuStrip";
			this.mainMenuStrip.Padding = new System.Windows.Forms.Padding(6, 2, 0, 2);
			this.mainMenuStrip.TabIndex = 7;
			this.mainMenuStrip.Text = "menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newGzipMenuToolStripButton,
            this.newDeflateMenuToolStripButton,
            this.openMenuStripButton,
            this.toolStripSeparator1,
            this.closeMenuStripButton});
			this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
			this.fileToolStripMenuItem.Text = "&File";
			// 
			// openMenuStripButton
			// 
			this.openMenuStripButton.Image = Resource1.OpenToolStripMenuItem_Image;
			this.openMenuStripButton.Name = "openMenuStripButton";
			this.openMenuStripButton.Text = "Open";
			this.openMenuStripButton.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
			// 
			// toolStripSeparator1
			// 
			this.toolStripSeparator1.Name = "toolStripSeparator1";
			// 
			// closeMenuStripButton
			// 
			this.closeMenuStripButton.Name = "closeMenuStripButton";
			this.closeMenuStripButton.Text = "Exit";
			this.closeMenuStripButton.Click += new System.EventHandler(this.closeToolStripMenuItem_Click);
			// 
			// actionsToolStripMenuItem
			// 
			this.actionsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addMenuStripButton,
            this.removeMenuStripButton,
            this.extractMenuStripButton,
            this.extractAllMenuStripButton});
			this.actionsToolStripMenuItem.Name = "actionsToolStripMenuItem";
			this.actionsToolStripMenuItem.Text = "&Actions";
			// 
			// addMenuStripButton
			// 
			this.addMenuStripButton.Image = Resource1.RedoToolStripMenuItem_Image;
			this.addMenuStripButton.Name = "addMenuStripButton";
			this.addMenuStripButton.Text = "Add";
			this.addMenuStripButton.Click += new System.EventHandler(this.addToolStripMenuItem_Click);
			// 
			// removeMenuStripButton
			// 
			this.removeMenuStripButton.Image = Resource1.CutToolStripMenuItem_Image;
			this.removeMenuStripButton.Name = "removeMenuStripButton";
			this.removeMenuStripButton.Text = "Remove";
			this.removeMenuStripButton.Click += new System.EventHandler(this.removeToolStripMenuItem_Click);
			// 
			// extractMenuStripButton
			// 
			this.extractMenuStripButton.Image = Resource1.UndoToolStripMenuItem_Image;
			this.extractMenuStripButton.Name = "extractMenuStripButton";
			this.extractMenuStripButton.Text = "Extract";
			this.extractMenuStripButton.Click += new System.EventHandler(this.extractToolStripMenuItem_Click);
			// 
			// extractAllMenuStripButton
			// 
			this.extractAllMenuStripButton.Image = Resource1.UndoToolStripMenuItem_Image;//((System.Drawing.Image)(resources.GetObject("extractAllMenuStripButton.Image")));
			this.extractAllMenuStripButton.Name = "extractAllMenuStripButton";
			this.extractAllMenuStripButton.Text = "Extract All";
			this.extractAllMenuStripButton.Click += new System.EventHandler(this.extractAllToolStripMenuItem_Click);
			// 
			// fileContextMenuStrip
			// 
			this.fileContextMenuStrip.BackColor = System.Drawing.SystemColors.Menu;
			this.fileContextMenuStrip.Enabled = true;
			this.fileContextMenuStrip.GripMargin = new System.Windows.Forms.Padding(2);
			this.fileContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.contextMenuStripButton,
            this.removeContextMenuStripButton});
			this.fileContextMenuStrip.Location = new System.Drawing.Point(32, 77);
			this.fileContextMenuStrip.Name = "fileContextMenuStrip";
			this.fileContextMenuStrip.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.fileContextMenuStrip.Size = new System.Drawing.Size(103, 48);
			// 
			// contextMenuStripButton
			// 
            this.contextMenuStripButton.Image = Resource1.UndoToolStripMenuItem_Image;
			this.contextMenuStripButton.Name = "contextMenuStripButton";
			this.contextMenuStripButton.Text = "Extract";
			this.contextMenuStripButton.Click += new System.EventHandler(this.contextMenuStripButton_Click);
			// 
			// removeContextMenuStripButton
			// 
			this.removeContextMenuStripButton.Image = Resource1.CutToolStripMenuItem_Image;
			this.removeContextMenuStripButton.Name = "removeContextMenuStripButton";
			this.removeContextMenuStripButton.Text = "Remove";
			this.removeContextMenuStripButton.Click += new System.EventHandler(this.removeToolStripMenuItem_Click);
			// 
			// mainStatusStrip
			// 
			this.mainStatusStrip.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.Table;
			this.mainStatusStrip.Location = new System.Drawing.Point(9, 539);
			this.mainStatusStrip.Name = "mainStatusStrip";
			this.mainStatusStrip.Size = new System.Drawing.Size(769, 18);
			this.mainStatusStrip.TabIndex = 8;
			this.mainStatusStrip.Text = "statusStrip1";
			// 
			// CompressionForm
			// 
			this.ClientSize = new System.Drawing.Size(787, 566);
			this.Controls.Add(this.mainStatusStrip);
			this.Controls.Add(this.fileListView);
			this.Controls.Add(this.mainMenuStrip);
			this.Name = "CompressionForm";
			this.Padding = new System.Windows.Forms.Padding(9);
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "CompressionForm";
			this.mainToolStrip.ResumeLayout(false);
			this.mainMenuStrip.ResumeLayout(false);
			this.fileContextMenuStrip.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (archive != null)
			{
				archive.Close();
			}
			if (disposing)
			{
				if (components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}

		private System.Windows.Forms.ToolStrip mainToolStrip;
		private System.Windows.Forms.ListView fileListView;
		private System.Windows.Forms.ToolStripButton openToolStripButton;
		private System.Windows.Forms.ToolStripButton addToolStripButton;
		private System.Windows.Forms.ToolStripButton extractToolStripButton;
		private System.Windows.Forms.ToolStripSeparator addToolStripSeparator;
		private System.Windows.Forms.ToolStripSeparator newToolStripSeparator;
		private System.Windows.Forms.MenuStrip mainMenuStrip;
		private System.Windows.Forms.ToolStripMenuItem actionsToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem openMenuStripButton;
		private System.Windows.Forms.ToolStripMenuItem closeMenuStripButton;
		private System.Windows.Forms.ToolStripMenuItem addMenuStripButton;
		private System.Windows.Forms.ToolStripMenuItem removeMenuStripButton;
		private System.Windows.Forms.ToolStripMenuItem extractMenuStripButton;
		private System.Windows.Forms.ToolStripMenuItem extractAllMenuStripButton;
		//private System.Windows.Forms.StatusStripPanel mainStatusStripPanel;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
		private System.Windows.Forms.ToolStripButton newGzipToolStripButton;
		private System.Windows.Forms.ToolStripButton newDeflateToolStripButton;
		private System.Windows.Forms.ToolStripMenuItem newGzipMenuToolStripButton;
		private System.Windows.Forms.ToolStripMenuItem newDeflateMenuToolStripButton;
		private System.Windows.Forms.ContextMenuStrip fileContextMenuStrip;
		private System.Windows.Forms.ToolStripMenuItem contextMenuStripButton;
		private System.Windows.Forms.ToolStripMenuItem removeContextMenuStripButton;
		private System.Windows.Forms.ToolStripButton removeToolStripButton;
		private System.Windows.Forms.StatusStrip mainStatusStrip;


	}
}

