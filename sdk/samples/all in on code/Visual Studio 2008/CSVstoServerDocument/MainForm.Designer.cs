namespace CSVstoServerDocument
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
            this.lblFileName = new System.Windows.Forms.Label();
            this.txtFileName = new System.Windows.Forms.TextBox();
            this.btnSelectFile = new System.Windows.Forms.Button();
            this.ofd = new System.Windows.Forms.OpenFileDialog();
            this.lstDocInfo = new System.Windows.Forms.ListBox();
            this.toolTip = new System.Windows.Forms.ToolTip(this.components);
            this.txtManifest = new System.Windows.Forms.TextBox();
            this.txtAssembly = new System.Windows.Forms.TextBox();
            this.btnAdd = new System.Windows.Forms.Button();
            this.btnRemove = new System.Windows.Forms.Button();
            this.grpCustomization = new System.Windows.Forms.GroupBox();
            this.lblAssembly = new System.Windows.Forms.Label();
            this.lblManifest = new System.Windows.Forms.Label();
            this.grpCustomization.SuspendLayout();
            this.SuspendLayout();
            // 
            // lblFileName
            // 
            this.lblFileName.AutoSize = true;
            this.lblFileName.Location = new System.Drawing.Point(12, 15);
            this.lblFileName.Name = "lblFileName";
            this.lblFileName.Size = new System.Drawing.Size(120, 13);
            this.lblFileName.TabIndex = 0;
            this.lblFileName.Text = "Document / Workbook:";
            // 
            // txtFileName
            // 
            this.txtFileName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtFileName.Location = new System.Drawing.Point(138, 12);
            this.txtFileName.Name = "txtFileName";
            this.txtFileName.ReadOnly = true;
            this.txtFileName.Size = new System.Drawing.Size(227, 20);
            this.txtFileName.TabIndex = 1;
            this.txtFileName.TextChanged += new System.EventHandler(this.txtFileName_TextChanged);
            // 
            // btnSelectFile
            // 
            this.btnSelectFile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnSelectFile.Location = new System.Drawing.Point(371, 10);
            this.btnSelectFile.Name = "btnSelectFile";
            this.btnSelectFile.Size = new System.Drawing.Size(75, 23);
            this.btnSelectFile.TabIndex = 2;
            this.btnSelectFile.Text = "Select...";
            this.btnSelectFile.UseVisualStyleBackColor = true;
            this.btnSelectFile.Click += new System.EventHandler(this.btnSelectFile_Click);
            // 
            // ofd
            // 
            this.ofd.Filter = "Word Douments|*.doc;*.dot;*.docx;*.dotx;*.docm;*.dotm|Excel Workbooks|*.xls;*.xlt" +
                ";*.xlsx;*.xltx;*.xlsm;*.xltm";
            // 
            // lstDocInfo
            // 
            this.lstDocInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lstDocInfo.FormattingEnabled = true;
            this.lstDocInfo.HorizontalScrollbar = true;
            this.lstDocInfo.Location = new System.Drawing.Point(15, 51);
            this.lstDocInfo.Name = "lstDocInfo";
            this.lstDocInfo.Size = new System.Drawing.Size(431, 121);
            this.lstDocInfo.TabIndex = 3;
            // 
            // txtManifest
            // 
            this.txtManifest.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtManifest.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest;
            this.txtManifest.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.AllUrl;
            this.txtManifest.Enabled = false;
            this.txtManifest.Location = new System.Drawing.Point(121, 19);
            this.txtManifest.Name = "txtManifest";
            this.txtManifest.Size = new System.Drawing.Size(307, 20);
            this.txtManifest.TabIndex = 1;
            this.toolTip.SetToolTip(this.txtManifest, "Deployment manifest URI");
            // 
            // txtAssembly
            // 
            this.txtAssembly.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtAssembly.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest;
            this.txtAssembly.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.AllUrl;
            this.txtAssembly.Enabled = false;
            this.txtAssembly.Location = new System.Drawing.Point(121, 45);
            this.txtAssembly.Name = "txtAssembly";
            this.txtAssembly.Size = new System.Drawing.Size(307, 20);
            this.txtAssembly.TabIndex = 3;
            this.toolTip.SetToolTip(this.txtAssembly, "VSTO SE customization assembly URI");
            // 
            // btnAdd
            // 
            this.btnAdd.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnAdd.Enabled = false;
            this.btnAdd.Location = new System.Drawing.Point(298, 76);
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(130, 23);
            this.btnAdd.TabIndex = 4;
            this.btnAdd.Text = "Add Customization...";
            this.btnAdd.UseVisualStyleBackColor = true;
            this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // btnRemove
            // 
            this.btnRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRemove.Enabled = false;
            this.btnRemove.Location = new System.Drawing.Point(298, 105);
            this.btnRemove.Name = "btnRemove";
            this.btnRemove.Size = new System.Drawing.Size(130, 23);
            this.btnRemove.TabIndex = 5;
            this.btnRemove.Text = "Remove Customization";
            this.btnRemove.UseVisualStyleBackColor = true;
            this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
            // 
            // grpCustomization
            // 
            this.grpCustomization.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.grpCustomization.Controls.Add(this.txtAssembly);
            this.grpCustomization.Controls.Add(this.lblAssembly);
            this.grpCustomization.Controls.Add(this.txtManifest);
            this.grpCustomization.Controls.Add(this.lblManifest);
            this.grpCustomization.Controls.Add(this.btnRemove);
            this.grpCustomization.Controls.Add(this.btnAdd);
            this.grpCustomization.Location = new System.Drawing.Point(12, 192);
            this.grpCustomization.Name = "grpCustomization";
            this.grpCustomization.Size = new System.Drawing.Size(434, 134);
            this.grpCustomization.TabIndex = 4;
            this.grpCustomization.TabStop = false;
            this.grpCustomization.Text = "Document Customization";
            // 
            // lblAssembly
            // 
            this.lblAssembly.AutoSize = true;
            this.lblAssembly.Enabled = false;
            this.lblAssembly.Location = new System.Drawing.Point(6, 48);
            this.lblAssembly.Name = "lblAssembly";
            this.lblAssembly.Size = new System.Drawing.Size(79, 13);
            this.lblAssembly.TabIndex = 2;
            this.lblAssembly.Text = "Assembly Path:";
            // 
            // lblManifest
            // 
            this.lblManifest.AutoSize = true;
            this.lblManifest.Enabled = false;
            this.lblManifest.Location = new System.Drawing.Point(6, 22);
            this.lblManifest.Name = "lblManifest";
            this.lblManifest.Size = new System.Drawing.Size(109, 13);
            this.lblManifest.TabIndex = 0;
            this.lblManifest.Text = "Deployment Manifest:";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(458, 342);
            this.Controls.Add(this.grpCustomization);
            this.Controls.Add(this.lstDocInfo);
            this.Controls.Add(this.btnSelectFile);
            this.Controls.Add(this.txtFileName);
            this.Controls.Add(this.lblFileName);
            this.MinimumSize = new System.Drawing.Size(449, 380);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "ServerDocument Demo";
            this.grpCustomization.ResumeLayout(false);
            this.grpCustomization.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lblFileName;
        private System.Windows.Forms.TextBox txtFileName;
        private System.Windows.Forms.Button btnSelectFile;
        private System.Windows.Forms.OpenFileDialog ofd;
        private System.Windows.Forms.ListBox lstDocInfo;
        private System.Windows.Forms.ToolTip toolTip;
        private System.Windows.Forms.Button btnAdd;
        private System.Windows.Forms.Button btnRemove;
        private System.Windows.Forms.GroupBox grpCustomization;
        private System.Windows.Forms.TextBox txtManifest;
        private System.Windows.Forms.Label lblManifest;
        private System.Windows.Forms.TextBox txtAssembly;
        private System.Windows.Forms.Label lblAssembly;
    }
}

