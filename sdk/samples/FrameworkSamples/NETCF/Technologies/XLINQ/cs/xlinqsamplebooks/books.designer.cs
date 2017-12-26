namespace Microsoft.Samples.XLinq
{
    partial class Books
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.MainMenu menu;
        
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
            this.menu = new System.Windows.Forms.MainMenu();
            this.miExit = new System.Windows.Forms.MenuItem();
            this.tabControl = new System.Windows.Forms.TabControl();
            this.tpBookSearch = new System.Windows.Forms.TabPage();
            this.lblBookSearchCount = new System.Windows.Forms.Label();
            this.dgResults = new System.Windows.Forms.DataGrid();
            this.dgTableStyle = new System.Windows.Forms.DataGridTableStyle();
            this.dgcolumnTitle = new System.Windows.Forms.DataGridTextBoxColumn();
            this.dgcolumnAuthor = new System.Windows.Forms.DataGridTextBoxColumn();
            this.dgcolumnPrice = new System.Windows.Forms.DataGridTextBoxColumn();
            this.btnSearch = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.txtBookName = new System.Windows.Forms.TextBox();
            this.tpGroupView = new System.Windows.Forms.TabPage();
            this.cbGroupBy = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.btnGo = new System.Windows.Forms.Button();
            this.wbGroupByResults = new System.Windows.Forms.WebBrowser();
            this.tpUnifiedSearch = new System.Windows.Forms.TabPage();
            this.cbUnifiedSearch = new System.Windows.Forms.ComboBox();
            this.wbUnifiedSearchResults = new System.Windows.Forms.WebBrowser();
            this.btnAction = new System.Windows.Forms.Button();
            this.lblMessage = new System.Windows.Forms.Label();
            this.tabControl.SuspendLayout();
            this.tpBookSearch.SuspendLayout();
            this.tpGroupView.SuspendLayout();
            this.tpUnifiedSearch.SuspendLayout();
            this.SuspendLayout();
            // 
            // menu
            // 
            this.menu.MenuItems.Add(this.miExit);
            // 
            // miExit
            // 
            this.miExit.Text = "Exit";
            this.miExit.Click += new System.EventHandler(this.miExit_Click);
            // 
            // tabControl
            // 
            this.tabControl.Controls.Add(this.tpBookSearch);
            this.tabControl.Controls.Add(this.tpGroupView);
            this.tabControl.Controls.Add(this.tpUnifiedSearch);
            this.tabControl.Location = new System.Drawing.Point(0, 0);
            this.tabControl.Name = "tabControl";
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(240, 268);
            this.tabControl.TabIndex = 0;
            // 
            // tpBookSearch
            // 
            this.tpBookSearch.Controls.Add(this.lblBookSearchCount);
            this.tpBookSearch.Controls.Add(this.dgResults);
            this.tpBookSearch.Controls.Add(this.btnSearch);
            this.tpBookSearch.Controls.Add(this.label1);
            this.tpBookSearch.Controls.Add(this.txtBookName);
            this.tpBookSearch.Location = new System.Drawing.Point(0, 0);
            this.tpBookSearch.Name = "tpBookSearch";
            this.tpBookSearch.Size = new System.Drawing.Size(240, 245);
            this.tpBookSearch.Text = "Book Search";
            // 
            // lblBookSearchCount
            // 
            this.lblBookSearchCount.Location = new System.Drawing.Point(8, 54);
            this.lblBookSearchCount.Name = "lblBookSearchCount";
            this.lblBookSearchCount.Size = new System.Drawing.Size(224, 20);
            this.lblBookSearchCount.Text = "lblRecCount";
            this.lblBookSearchCount.Visible = false;
            // 
            // dgResults
            // 
            this.dgResults.BackgroundColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(128)))), ((int)(((byte)(128)))));
            this.dgResults.Location = new System.Drawing.Point(8, 80);
            this.dgResults.Name = "dgResults";
            this.dgResults.Size = new System.Drawing.Size(225, 162);
            this.dgResults.TabIndex = 3;
            this.dgResults.TableStyles.Add(this.dgTableStyle);
            this.dgResults.Visible = false;
            // 
            // dgTableStyle
            // 
            this.dgTableStyle.GridColumnStyles.Add(this.dgcolumnTitle);
            this.dgTableStyle.GridColumnStyles.Add(this.dgcolumnAuthor);
            this.dgTableStyle.GridColumnStyles.Add(this.dgcolumnPrice);
            // 
            // dgcolumnTitle
            // 
            this.dgcolumnTitle.Format = "";
            this.dgcolumnTitle.FormatInfo = null;
            this.dgcolumnTitle.HeaderText = "Title";
            this.dgcolumnTitle.MappingName = "Title";
            this.dgcolumnTitle.Width = 100;
            // 
            // dgcolumnAuthor
            // 
            this.dgcolumnAuthor.Format = "";
            this.dgcolumnAuthor.FormatInfo = null;
            this.dgcolumnAuthor.HeaderText = "Author";
            this.dgcolumnAuthor.MappingName = "Author";
            // 
            // dgcolumnPrice
            // 
            this.dgcolumnPrice.Format = "c";
            this.dgcolumnPrice.FormatInfo = null;
            this.dgcolumnPrice.HeaderText = "Price";
            this.dgcolumnPrice.MappingName = "Price";
            // 
            // btnSearch
            // 
            this.btnSearch.Location = new System.Drawing.Point(7, 31);
            this.btnSearch.Name = "btnSearch";
            this.btnSearch.Size = new System.Drawing.Size(225, 20);
            this.btnSearch.TabIndex = 2;
            this.btnSearch.Text = "Search";
            this.btnSearch.Click += new System.EventHandler(this.btnSearch_Click);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(8, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(42, 20);
            this.label1.Text = "Title";
            // 
            // txtBookName
            // 
            this.txtBookName.Location = new System.Drawing.Point(56, 7);
            this.txtBookName.Name = "txtBookName";
            this.txtBookName.Size = new System.Drawing.Size(177, 21);
            this.txtBookName.TabIndex = 0;
            // 
            // tpGroupView
            // 
            this.tpGroupView.Controls.Add(this.cbGroupBy);
            this.tpGroupView.Controls.Add(this.label2);
            this.tpGroupView.Controls.Add(this.btnGo);
            this.tpGroupView.Controls.Add(this.wbGroupByResults);
            this.tpGroupView.Location = new System.Drawing.Point(0, 0);
            this.tpGroupView.Name = "tpGroupView";
            this.tpGroupView.Size = new System.Drawing.Size(240, 245);
            this.tpGroupView.Text = "Group View";
            // 
            // cbGroupBy
            // 
            this.cbGroupBy.Items.Add("Author");
            this.cbGroupBy.Items.Add("Subject");
            this.cbGroupBy.Items.Add("Style");
            this.cbGroupBy.Location = new System.Drawing.Point(71, 14);
            this.cbGroupBy.Name = "cbGroupBy";
            this.cbGroupBy.Size = new System.Drawing.Size(121, 22);
            this.cbGroupBy.TabIndex = 4;
            // 
            // label2
            // 
            this.label2.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold);
            this.label2.Location = new System.Drawing.Point(4, 16);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(61, 20);
            this.label2.Text = "Group by";
            // 
            // btnGo
            // 
            this.btnGo.Location = new System.Drawing.Point(198, 14);
            this.btnGo.Name = "btnGo";
            this.btnGo.Size = new System.Drawing.Size(39, 20);
            this.btnGo.TabIndex = 1;
            this.btnGo.Text = "Go";
            this.btnGo.Click += new System.EventHandler(this.btnGo_Click);
            // 
            // wbGroupByResults
            // 
            this.wbGroupByResults.Location = new System.Drawing.Point(0, 58);
            this.wbGroupByResults.Name = "wbGroupByResults";
            this.wbGroupByResults.Size = new System.Drawing.Size(240, 187);
            // 
            // tpUnifiedSearch
            // 
            this.tpUnifiedSearch.Controls.Add(this.cbUnifiedSearch);
            this.tpUnifiedSearch.Controls.Add(this.wbUnifiedSearchResults);
            this.tpUnifiedSearch.Controls.Add(this.btnAction);
            this.tpUnifiedSearch.Controls.Add(this.lblMessage);
            this.tpUnifiedSearch.Location = new System.Drawing.Point(0, 0);
            this.tpUnifiedSearch.Name = "tpUnifiedSearch";
            this.tpUnifiedSearch.Size = new System.Drawing.Size(232, 242);
            this.tpUnifiedSearch.Text = "Unified Search";
            // 
            // cbUnifiedSearch
            // 
            this.cbUnifiedSearch.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDown;
            this.cbUnifiedSearch.Location = new System.Drawing.Point(8, 30);
            this.cbUnifiedSearch.Name = "cbUnifiedSearch";
            this.cbUnifiedSearch.Size = new System.Drawing.Size(216, 22);
            this.cbUnifiedSearch.TabIndex = 6;
            this.cbUnifiedSearch.KeyUp += new System.Windows.Forms.KeyEventHandler(this.cbUnifiedSearch_KeyUp);
            // 
            // wbUnifiedSearchResults
            // 
            this.wbUnifiedSearchResults.Location = new System.Drawing.Point(0, 83);
            this.wbUnifiedSearchResults.Name = "wbUnifiedSearchResults";
            this.wbUnifiedSearchResults.Size = new System.Drawing.Size(240, 159);
            // 
            // btnAction
            // 
            this.btnAction.Location = new System.Drawing.Point(8, 57);
            this.btnAction.Name = "btnAction";
            this.btnAction.Size = new System.Drawing.Size(216, 20);
            this.btnAction.TabIndex = 2;
            this.btnAction.Text = "btnAction";
            this.btnAction.Click += new System.EventHandler(this.btnAction_Click);
            // 
            // lblMessage
            // 
            this.lblMessage.Location = new System.Drawing.Point(8, 8);
            this.lblMessage.Name = "lblMessage";
            this.lblMessage.Size = new System.Drawing.Size(216, 18);
            this.lblMessage.Text = "Type in keywords";
            // 
            // Books
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 268);
            this.Controls.Add(this.tabControl);
            this.Menu = this.menu;
            this.Name = "Books";
            this.Text = "Books";
            this.Load += new System.EventHandler(this.Books_Load);
            this.tabControl.ResumeLayout(false);
            this.tpBookSearch.ResumeLayout(false);
            this.tpGroupView.ResumeLayout(false);
            this.tpUnifiedSearch.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.TabPage tpBookSearch;
        private System.Windows.Forms.TabPage tpGroupView;
        private System.Windows.Forms.TextBox txtBookName;
        private System.Windows.Forms.Button btnSearch;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.DataGrid dgResults;
        private System.Windows.Forms.Button btnGo;
        private System.Windows.Forms.WebBrowser wbGroupByResults;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cbGroupBy;
        private System.Windows.Forms.MenuItem miExit;
        private System.Windows.Forms.TabPage tpUnifiedSearch;
        private System.Windows.Forms.Button btnAction;
        private System.Windows.Forms.Label lblMessage;
        private System.Windows.Forms.WebBrowser wbUnifiedSearchResults;
        private System.Windows.Forms.ComboBox cbUnifiedSearch;
        private System.Windows.Forms.Label lblBookSearchCount;
        private System.Windows.Forms.DataGridTableStyle dgTableStyle;
        private System.Windows.Forms.DataGridTextBoxColumn dgcolumnTitle;
        private System.Windows.Forms.DataGridTextBoxColumn dgcolumnAuthor;
        private System.Windows.Forms.DataGridTextBoxColumn dgcolumnPrice;
    }
}

