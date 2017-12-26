namespace CSCodeDOM
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.txtSource = new System.Windows.Forms.TextBox();
            this.btnRun = new System.Windows.Forms.Button();
            this.lblLang = new System.Windows.Forms.Label();
            this.cboLang = new System.Windows.Forms.ComboBox();
            this.chkSpDomain = new System.Windows.Forms.CheckBox();
            this.gbNamespaces = new System.Windows.Forms.GroupBox();
            this.btnAddNamespace = new System.Windows.Forms.Button();
            this.btnRemoveNamespace = new System.Windows.Forms.Button();
            this.txtNamespace = new System.Windows.Forms.TextBox();
            this.lstNamespaces = new System.Windows.Forms.ListBox();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.gbAssemblyRef = new System.Windows.Forms.GroupBox();
            this.btnAddAsmRef = new System.Windows.Forms.Button();
            this.btnRemoveAsmRef = new System.Windows.Forms.Button();
            this.txtAssemblyRef = new System.Windows.Forms.TextBox();
            this.lstAssemblyRef = new System.Windows.Forms.ListBox();
            this.gbNamespaces.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.gbAssemblyRef.SuspendLayout();
            this.SuspendLayout();
            // 
            // txtSource
            // 
            this.txtSource.AcceptsReturn = true;
            this.txtSource.AcceptsTab = true;
            this.txtSource.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtSource.Font = new System.Drawing.Font("Consolas", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtSource.Location = new System.Drawing.Point(12, 12);
            this.txtSource.Multiline = true;
            this.txtSource.Name = "txtSource";
            this.txtSource.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtSource.Size = new System.Drawing.Size(600, 238);
            this.txtSource.TabIndex = 0;
            this.txtSource.Text = resources.GetString("txtSource.Text");
            this.txtSource.WordWrap = false;
            this.txtSource.TextChanged += new System.EventHandler(this.txtSource_TextChanged);
            // 
            // btnRun
            // 
            this.btnRun.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRun.Location = new System.Drawing.Point(537, 407);
            this.btnRun.Name = "btnRun";
            this.btnRun.Size = new System.Drawing.Size(75, 23);
            this.btnRun.TabIndex = 5;
            this.btnRun.Text = "&Run";
            this.btnRun.UseVisualStyleBackColor = true;
            this.btnRun.Click += new System.EventHandler(this.btnRun_Click);
            // 
            // lblLang
            // 
            this.lblLang.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lblLang.AutoSize = true;
            this.lblLang.Location = new System.Drawing.Point(12, 412);
            this.lblLang.Name = "lblLang";
            this.lblLang.Size = new System.Drawing.Size(58, 13);
            this.lblLang.TabIndex = 2;
            this.lblLang.Text = "&Language:";
            // 
            // cboLang
            // 
            this.cboLang.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.cboLang.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboLang.FormattingEnabled = true;
            this.cboLang.Items.AddRange(new object[] {
            "C#",
            "Visual Basic",
            "JScript"});
            this.cboLang.Location = new System.Drawing.Point(76, 409);
            this.cboLang.Name = "cboLang";
            this.cboLang.Size = new System.Drawing.Size(151, 21);
            this.cboLang.TabIndex = 3;
            // 
            // chkSpDomain
            // 
            this.chkSpDomain.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.chkSpDomain.AutoSize = true;
            this.chkSpDomain.Checked = true;
            this.chkSpDomain.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkSpDomain.Location = new System.Drawing.Point(264, 411);
            this.chkSpDomain.Name = "chkSpDomain";
            this.chkSpDomain.Size = new System.Drawing.Size(187, 17);
            this.chkSpDomain.TabIndex = 4;
            this.chkSpDomain.Text = "Run script in separate App&Domain";
            this.chkSpDomain.UseVisualStyleBackColor = true;
            // 
            // gbNamespaces
            // 
            this.gbNamespaces.Controls.Add(this.btnAddNamespace);
            this.gbNamespaces.Controls.Add(this.btnRemoveNamespace);
            this.gbNamespaces.Controls.Add(this.txtNamespace);
            this.gbNamespaces.Controls.Add(this.lstNamespaces);
            this.gbNamespaces.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gbNamespaces.Location = new System.Drawing.Point(3, 3);
            this.gbNamespaces.Name = "gbNamespaces";
            this.gbNamespaces.Size = new System.Drawing.Size(294, 139);
            this.gbNamespaces.TabIndex = 0;
            this.gbNamespaces.TabStop = false;
            this.gbNamespaces.Text = "Import Namespaces";
            // 
            // btnAddNamespace
            // 
            this.btnAddNamespace.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnAddNamespace.Enabled = false;
            this.btnAddNamespace.Location = new System.Drawing.Point(262, 105);
            this.btnAddNamespace.Name = "btnAddNamespace";
            this.btnAddNamespace.Size = new System.Drawing.Size(23, 23);
            this.btnAddNamespace.TabIndex = 2;
            this.btnAddNamespace.Text = "+";
            this.btnAddNamespace.UseVisualStyleBackColor = true;
            this.btnAddNamespace.Click += new System.EventHandler(this.btnAddNamespace_Click);
            // 
            // btnRemoveNamespace
            // 
            this.btnRemoveNamespace.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRemoveNamespace.Enabled = false;
            this.btnRemoveNamespace.Location = new System.Drawing.Point(262, 78);
            this.btnRemoveNamespace.Name = "btnRemoveNamespace";
            this.btnRemoveNamespace.Size = new System.Drawing.Size(23, 23);
            this.btnRemoveNamespace.TabIndex = 3;
            this.btnRemoveNamespace.Text = "-";
            this.btnRemoveNamespace.UseVisualStyleBackColor = true;
            this.btnRemoveNamespace.Click += new System.EventHandler(this.btnRemoveNamespace_Click);
            // 
            // txtNamespace
            // 
            this.txtNamespace.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtNamespace.Location = new System.Drawing.Point(6, 107);
            this.txtNamespace.Name = "txtNamespace";
            this.txtNamespace.Size = new System.Drawing.Size(250, 20);
            this.txtNamespace.TabIndex = 1;
            this.txtNamespace.TextChanged += new System.EventHandler(this.txtNamespace_TextChanged);
            // 
            // lstNamespaces
            // 
            this.lstNamespaces.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lstNamespaces.FormattingEnabled = true;
            this.lstNamespaces.Items.AddRange(new object[] {
            "System",
            "System.IO",
            "System.Data",
            "System.Linq",
            "System.Xml",
            "System.Text",
            "System.Collections.Generic",
            "System.Drawing",
            "System.ComponentModel",
            "System.Windows.Forms",
            "Microsoft.Win32",
            "Microsoft.VisualBasic",
            "Microsoft.JScript"});
            this.lstNamespaces.Location = new System.Drawing.Point(6, 19);
            this.lstNamespaces.Name = "lstNamespaces";
            this.lstNamespaces.Size = new System.Drawing.Size(250, 82);
            this.lstNamespaces.TabIndex = 0;
            this.lstNamespaces.SelectedIndexChanged += new System.EventHandler(this.lstNamespaces_SelectedIndexChanged);
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanel1.AutoSize = true;
            this.tableLayoutPanel1.ColumnCount = 2;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Controls.Add(this.gbNamespaces, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.gbAssemblyRef, 1, 0);
            this.tableLayoutPanel1.Location = new System.Drawing.Point(12, 256);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 1;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(600, 145);
            this.tableLayoutPanel1.TabIndex = 1;
            // 
            // gbAssemblyRef
            // 
            this.gbAssemblyRef.Controls.Add(this.btnAddAsmRef);
            this.gbAssemblyRef.Controls.Add(this.btnRemoveAsmRef);
            this.gbAssemblyRef.Controls.Add(this.txtAssemblyRef);
            this.gbAssemblyRef.Controls.Add(this.lstAssemblyRef);
            this.gbAssemblyRef.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gbAssemblyRef.Location = new System.Drawing.Point(303, 3);
            this.gbAssemblyRef.Name = "gbAssemblyRef";
            this.gbAssemblyRef.Size = new System.Drawing.Size(294, 139);
            this.gbAssemblyRef.TabIndex = 1;
            this.gbAssemblyRef.TabStop = false;
            this.gbAssemblyRef.Text = "Assembly References";
            // 
            // btnAddAsmRef
            // 
            this.btnAddAsmRef.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnAddAsmRef.Enabled = false;
            this.btnAddAsmRef.Location = new System.Drawing.Point(262, 105);
            this.btnAddAsmRef.Name = "btnAddAsmRef";
            this.btnAddAsmRef.Size = new System.Drawing.Size(23, 23);
            this.btnAddAsmRef.TabIndex = 2;
            this.btnAddAsmRef.Text = "+";
            this.btnAddAsmRef.UseVisualStyleBackColor = true;
            this.btnAddAsmRef.Click += new System.EventHandler(this.btnAddAsmRef_Click);
            // 
            // btnRemoveAsmRef
            // 
            this.btnRemoveAsmRef.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRemoveAsmRef.Enabled = false;
            this.btnRemoveAsmRef.Location = new System.Drawing.Point(262, 78);
            this.btnRemoveAsmRef.Name = "btnRemoveAsmRef";
            this.btnRemoveAsmRef.Size = new System.Drawing.Size(23, 23);
            this.btnRemoveAsmRef.TabIndex = 3;
            this.btnRemoveAsmRef.Text = "-";
            this.btnRemoveAsmRef.UseVisualStyleBackColor = true;
            this.btnRemoveAsmRef.Click += new System.EventHandler(this.btnRemoveAsmRef_Click);
            // 
            // txtAssemblyRef
            // 
            this.txtAssemblyRef.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtAssemblyRef.Location = new System.Drawing.Point(6, 108);
            this.txtAssemblyRef.Name = "txtAssemblyRef";
            this.txtAssemblyRef.Size = new System.Drawing.Size(250, 20);
            this.txtAssemblyRef.TabIndex = 1;
            this.txtAssemblyRef.TextChanged += new System.EventHandler(this.txtAssemblyRef_TextChanged);
            // 
            // lstAssemblyRef
            // 
            this.lstAssemblyRef.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lstAssemblyRef.FormattingEnabled = true;
            this.lstAssemblyRef.Items.AddRange(new object[] {
            "mscorlib.dll",
            "System.dll",
            "System.Core.dll",
            "System.Drawing.dll",
            "System.Data.dll",
            "System.Xml.dll",
            "System.Xml.Linq.dll",
            "System.Windows.Forms.dll",
            "Microsoft.VisualBasic.dll",
            "Microsoft.JScript.dll"});
            this.lstAssemblyRef.Location = new System.Drawing.Point(6, 19);
            this.lstAssemblyRef.Name = "lstAssemblyRef";
            this.lstAssemblyRef.Size = new System.Drawing.Size(250, 82);
            this.lstAssemblyRef.TabIndex = 0;
            this.lstAssemblyRef.SelectedIndexChanged += new System.EventHandler(this.lstAssemblyRef_SelectedIndexChanged);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(624, 442);
            this.Controls.Add(this.tableLayoutPanel1);
            this.Controls.Add(this.chkSpDomain);
            this.Controls.Add(this.cboLang);
            this.Controls.Add(this.lblLang);
            this.Controls.Add(this.btnRun);
            this.Controls.Add(this.txtSource);
            this.MinimumSize = new System.Drawing.Size(640, 480);
            this.Name = "MainForm";
            this.Text = "CSCodeDOM";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.gbNamespaces.ResumeLayout(false);
            this.gbNamespaces.PerformLayout();
            this.tableLayoutPanel1.ResumeLayout(false);
            this.gbAssemblyRef.ResumeLayout(false);
            this.gbAssemblyRef.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtSource;
        private System.Windows.Forms.Button btnRun;
        private System.Windows.Forms.Label lblLang;
        private System.Windows.Forms.ComboBox cboLang;
        private System.Windows.Forms.CheckBox chkSpDomain;
        private System.Windows.Forms.GroupBox gbNamespaces;
        private System.Windows.Forms.TextBox txtNamespace;
        private System.Windows.Forms.ListBox lstNamespaces;
        private System.Windows.Forms.Button btnAddNamespace;
        private System.Windows.Forms.Button btnRemoveNamespace;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.GroupBox gbAssemblyRef;
        private System.Windows.Forms.ListBox lstAssemblyRef;
        private System.Windows.Forms.TextBox txtAssemblyRef;
        private System.Windows.Forms.Button btnRemoveAsmRef;
        private System.Windows.Forms.Button btnAddAsmRef;
    }
}

