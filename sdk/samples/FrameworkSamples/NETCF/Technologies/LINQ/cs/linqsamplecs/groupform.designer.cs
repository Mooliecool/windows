namespace Microsoft.Samples.Linq
{
    partial class GroupForm
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
            this.txtDir = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.lvGroup = new System.Windows.Forms.ListView();
            this.colExtension = new System.Windows.Forms.ColumnHeader();
            this.colName = new System.Windows.Forms.ColumnHeader();
            this.colLMD = new System.Windows.Forms.ColumnHeader();
            this.btnGroup = new System.Windows.Forms.Button();
            this.groupMenu = new System.Windows.Forms.MainMenu();
            this.miExit = new System.Windows.Forms.MenuItem();
            this.miSearch = new System.Windows.Forms.MenuItem();
            this.SuspendLayout();
            // 
            // txtDir
            // 
            this.txtDir.Location = new System.Drawing.Point(89, 8);
            this.txtDir.Name = "txtDir";
            this.txtDir.Size = new System.Drawing.Size(148, 21);
            this.txtDir.TabIndex = 0;
            this.txtDir.Text = "\\windows";
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(0, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(83, 20);
            this.label1.Text = "Directory Path";
            // 
            // lvGroup
            // 
            this.lvGroup.Columns.Add(this.colExtension);
            this.lvGroup.Columns.Add(this.colName);
            this.lvGroup.Columns.Add(this.colLMD);
            this.lvGroup.Location = new System.Drawing.Point(3, 59);
            this.lvGroup.Name = "lvGroup";
            this.lvGroup.Size = new System.Drawing.Size(234, 209);
            this.lvGroup.TabIndex = 2;
            this.lvGroup.View = System.Windows.Forms.View.Details;
            this.lvGroup.Visible = false;
            // 
            // colExtension
            // 
            this.colExtension.Text = "Extension";
            this.colExtension.Width = 75;
            // 
            // colName
            // 
            this.colName.Text = "Name";
            this.colName.Width = 100;
            // 
            // colLMD
            // 
            this.colLMD.Text = "Date modified";
            this.colLMD.Width = 75;
            // 
            // btnGroup
            // 
            this.btnGroup.Location = new System.Drawing.Point(4, 33);
            this.btnGroup.Name = "btnGroup";
            this.btnGroup.Size = new System.Drawing.Size(233, 20);
            this.btnGroup.TabIndex = 3;
            this.btnGroup.Text = "Group";
            this.btnGroup.Click += new System.EventHandler(this.btnGroup_Click);
            // 
            // groupMenu
            // 
            this.groupMenu.MenuItems.Add(this.miExit);
            this.groupMenu.MenuItems.Add(this.miSearch);
            // 
            // miExit
            // 
            this.miExit.Text = "Exit";
            this.miExit.Click += new System.EventHandler(this.miExit_Click);
            // 
            // miSearch
            // 
            this.miSearch.Text = "Search View";
            this.miSearch.Click += new System.EventHandler(this.miSearch_Click);
            // 
            // GroupForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 268);
            this.Controls.Add(this.btnGroup);
            this.Controls.Add(this.lvGroup);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtDir);
            this.Menu = this.groupMenu;
            this.Name = "GroupForm";
            this.Text = "Group View";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox txtDir;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ListView lvGroup;
        private System.Windows.Forms.Button btnGroup;
        private System.Windows.Forms.ColumnHeader colExtension;
        private System.Windows.Forms.ColumnHeader colName;
        private System.Windows.Forms.ColumnHeader colLMD;
        private System.Windows.Forms.MainMenu groupMenu;
        private System.Windows.Forms.MenuItem miExit;
        private System.Windows.Forms.MenuItem miSearch;

    }
}