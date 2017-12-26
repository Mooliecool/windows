namespace WinFormsSearchApp
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
           this.txtSQL = new System.Windows.Forms.TextBox();
           this.btnRunQuery = new System.Windows.Forms.Button();
           this.cmbQueries = new System.Windows.Forms.ComboBox();
           this.btnSave = new System.Windows.Forms.Button();
           this.statusStrip = new System.Windows.Forms.StatusStrip();
           this.toolStripStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
           this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
           this.btnInsert = new System.Windows.Forms.Button();
           this.tabControl = new System.Windows.Forms.TabControl();
           this.tabPageColumns = new System.Windows.Forms.TabPage();
           this.cmbScope = new System.Windows.Forms.ComboBox();
           this.lvProperties = new System.Windows.Forms.ListView();
           this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader5 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader6 = new System.Windows.Forms.ColumnHeader();
           this.tabPageResults = new System.Windows.Forms.TabPage();
           this.dataGridView1 = new System.Windows.Forms.DataGridView();
           this.groupBox1 = new System.Windows.Forms.GroupBox();
           this.statusStrip.SuspendLayout();
           this.tabControl.SuspendLayout();
           this.tabPageColumns.SuspendLayout();
           this.tabPageResults.SuspendLayout();
           ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
           this.groupBox1.SuspendLayout();
           this.SuspendLayout();
           // 
           // txtSQL
           // 
           this.txtSQL.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.txtSQL.Location = new System.Drawing.Point(7, 45);
           this.txtSQL.Multiline = true;
           this.txtSQL.Name = "txtSQL";
           this.txtSQL.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
           this.txtSQL.Size = new System.Drawing.Size(409, 48);
           this.txtSQL.TabIndex = 2;
           // 
           // btnRunQuery
           // 
           this.btnRunQuery.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.btnRunQuery.Location = new System.Drawing.Point(422, 45);
           this.btnRunQuery.Name = "btnRunQuery";
           this.btnRunQuery.Size = new System.Drawing.Size(62, 23);
           this.btnRunQuery.TabIndex = 2;
           this.btnRunQuery.Text = "Run ";
           this.btnRunQuery.UseVisualStyleBackColor = true;
           this.btnRunQuery.Click += new System.EventHandler(this.OnRunQuery);
           // 
           // cmbQueries
           // 
           this.cmbQueries.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.cmbQueries.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
           this.cmbQueries.FormattingEnabled = true;
           this.cmbQueries.Location = new System.Drawing.Point(7, 18);
           this.cmbQueries.Name = "cmbQueries";
           this.cmbQueries.Size = new System.Drawing.Size(409, 23);
           this.cmbQueries.TabIndex = 6;
           this.cmbQueries.SelectedIndexChanged += new System.EventHandler(this.OnLoadQuery);
           // 
           // btnSave
           // 
           this.btnSave.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.btnSave.Location = new System.Drawing.Point(422, 70);
           this.btnSave.Name = "btnSave";
           this.btnSave.Size = new System.Drawing.Size(62, 23);
           this.btnSave.TabIndex = 8;
           this.btnSave.Text = "Save";
           this.toolTip1.SetToolTip(this.btnSave, "Save query to config file, to enable loading it during future");
           this.btnSave.UseVisualStyleBackColor = true;
           this.btnSave.Click += new System.EventHandler(this.OnSaveQuery);
           // 
           // statusStrip
           // 
           this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel});
           this.statusStrip.Location = new System.Drawing.Point(0, 382);
           this.statusStrip.Name = "statusStrip";
           this.statusStrip.Padding = new System.Windows.Forms.Padding(1, 0, 11, 0);
           this.statusStrip.Size = new System.Drawing.Size(493, 22);
           this.statusStrip.TabIndex = 10;
           this.statusStrip.Text = "statusStrip1";
           // 
           // toolStripStatusLabel
           // 
           this.toolStripStatusLabel.Name = "toolStripStatusLabel";
           this.toolStripStatusLabel.Size = new System.Drawing.Size(0, 17);
           // 
           // btnInsert
           // 
           this.btnInsert.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.btnInsert.Location = new System.Drawing.Point(418, 7);
           this.btnInsert.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
           this.btnInsert.Name = "btnInsert";
           this.btnInsert.Size = new System.Drawing.Size(64, 24);
           this.btnInsert.TabIndex = 5;
           this.btnInsert.Text = "Insert";
           this.toolTip1.SetToolTip(this.btnInsert, "Insert checked properites at insertion point in Query text box");
           this.btnInsert.UseVisualStyleBackColor = true;
           this.btnInsert.Click += new System.EventHandler(this.OnInsertColumns);
           // 
           // tabControl
           // 
           this.tabControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.tabControl.Controls.Add(this.tabPageColumns);
           this.tabControl.Controls.Add(this.tabPageResults);
           this.tabControl.Location = new System.Drawing.Point(0, 102);
           this.tabControl.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
           this.tabControl.Name = "tabControl";
           this.tabControl.SelectedIndex = 0;
           this.tabControl.Size = new System.Drawing.Size(496, 280);
           this.tabControl.TabIndex = 11;
           // 
           // tabPageColumns
           // 
           this.tabPageColumns.Controls.Add(this.cmbScope);
           this.tabPageColumns.Controls.Add(this.btnInsert);
           this.tabPageColumns.Controls.Add(this.lvProperties);
           this.tabPageColumns.Location = new System.Drawing.Point(4, 24);
           this.tabPageColumns.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
           this.tabPageColumns.Name = "tabPageColumns";
           this.tabPageColumns.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
           this.tabPageColumns.Size = new System.Drawing.Size(488, 252);
           this.tabPageColumns.TabIndex = 0;
           this.tabPageColumns.Text = "Properties";
           this.tabPageColumns.UseVisualStyleBackColor = true;
           // 
           // cmbScope
           // 
           this.cmbScope.DropDownHeight = 125;
           this.cmbScope.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
           this.cmbScope.FormattingEnabled = true;
           this.cmbScope.IntegralHeight = false;
           this.cmbScope.Location = new System.Drawing.Point(3, 7);
           this.cmbScope.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
           this.cmbScope.Name = "cmbScope";
           this.cmbScope.Size = new System.Drawing.Size(244, 23);
           this.cmbScope.Sorted = true;
           this.cmbScope.TabIndex = 7;
           this.cmbScope.SelectedIndexChanged += new System.EventHandler(this.OnPropertyScopeChanged);
           // 
           // lvProperties
           // 
           this.lvProperties.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.lvProperties.CheckBoxes = true;
           this.lvProperties.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3,
            this.columnHeader5,
            this.columnHeader6});
           this.lvProperties.FullRowSelect = true;
           this.lvProperties.Location = new System.Drawing.Point(-1, 34);
           this.lvProperties.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
           this.lvProperties.Name = "lvProperties";
           this.lvProperties.Size = new System.Drawing.Size(489, 222);
           this.lvProperties.Sorting = System.Windows.Forms.SortOrder.Ascending;
           this.lvProperties.TabIndex = 4;
           this.lvProperties.UseCompatibleStateImageBehavior = false;
           this.lvProperties.View = System.Windows.Forms.View.Details;
           this.lvProperties.ItemChecked += new System.Windows.Forms.ItemCheckedEventHandler(this.OnColumnChecked);
           this.lvProperties.DoubleClick += new System.EventHandler(this.OnColumnsDoubleClick);
           this.lvProperties.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.OnColumnHeaderClicked);
           // 
           // columnHeader1
           // 
           this.columnHeader1.Text = "Name";
           this.columnHeader1.Width = 325;
           // 
           // columnHeader2
           // 
           this.columnHeader2.Text = "Type";
           this.columnHeader2.Width = 130;
           // 
           // columnHeader3
           // 
           this.columnHeader3.Text = "Multi-valued";
           this.columnHeader3.Width = 150;
           // 
           // columnHeader5
           // 
           this.columnHeader5.Text = "IsViewable";
           this.columnHeader5.Width = 150;
           // 
           // columnHeader6
           // 
           this.columnHeader6.Text = "IsColumn";
           this.columnHeader6.Width = 150;
           // 
           // tabPageResults
           // 
           this.tabPageResults.Controls.Add(this.dataGridView1);
           this.tabPageResults.Location = new System.Drawing.Point(4, 24);
           this.tabPageResults.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
           this.tabPageResults.Name = "tabPageResults";
           this.tabPageResults.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
           this.tabPageResults.Size = new System.Drawing.Size(488, 252);
           this.tabPageResults.TabIndex = 1;
           this.tabPageResults.Text = "Query Results";
           this.tabPageResults.UseVisualStyleBackColor = true;
           // 
           // dataGridView1
           // 
           this.dataGridView1.AllowUserToAddRows = false;
           this.dataGridView1.AllowUserToDeleteRows = false;
           this.dataGridView1.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
           this.dataGridView1.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.AllCells;
           this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
           this.dataGridView1.Dock = System.Windows.Forms.DockStyle.Fill;
           this.dataGridView1.Location = new System.Drawing.Point(3, 2);
           this.dataGridView1.Name = "dataGridView1";
           this.dataGridView1.ReadOnly = true;
           this.dataGridView1.Size = new System.Drawing.Size(482, 248);
           this.dataGridView1.TabIndex = 5;
           // 
           // groupBox1
           // 
           this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.groupBox1.Controls.Add(this.cmbQueries);
           this.groupBox1.Controls.Add(this.txtSQL);
           this.groupBox1.Controls.Add(this.btnSave);
           this.groupBox1.Controls.Add(this.btnRunQuery);
           this.groupBox1.Location = new System.Drawing.Point(0, 4);
           this.groupBox1.Name = "groupBox1";
           this.groupBox1.Size = new System.Drawing.Size(496, 99);
           this.groupBox1.TabIndex = 12;
           this.groupBox1.TabStop = false;
           this.groupBox1.Text = " Query ";
           // 
           // MainForm
           // 
           this.AcceptButton = this.btnRunQuery;
           this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.ClientSize = new System.Drawing.Size(493, 404);
           this.Controls.Add(this.statusStrip);
           this.Controls.Add(this.tabControl);
           this.Controls.Add(this.groupBox1);
           this.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.Name = "MainForm";
           this.Text = "Windows Desktop Search - Test Application";
           this.Load += new System.EventHandler(this.OnFormLoad);
           this.statusStrip.ResumeLayout(false);
           this.statusStrip.PerformLayout();
           this.tabControl.ResumeLayout(false);
           this.tabPageColumns.ResumeLayout(false);
           this.tabPageResults.ResumeLayout(false);
           ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
           this.groupBox1.ResumeLayout(false);
           this.groupBox1.PerformLayout();
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtSQL;
        private System.Windows.Forms.Button btnRunQuery;
        private System.Windows.Forms.ComboBox cmbQueries;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.TabPage tabPageColumns;
        private System.Windows.Forms.ComboBox cmbScope;
        private System.Windows.Forms.Button btnInsert;
        private System.Windows.Forms.ListView lvProperties;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.TabPage tabPageResults;
        private System.Windows.Forms.DataGridView dataGridView1;
       private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ColumnHeader columnHeader5;
        private System.Windows.Forms.ColumnHeader columnHeader6;
    }
}

