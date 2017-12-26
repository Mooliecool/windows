namespace NAVWebServicesSampleCode
{
    partial class CSDynamicsNAVWebServices
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
            this.btnCodeunitWebService = new System.Windows.Forms.Button();
            this.btnPageWebService = new System.Windows.Forms.Button();
            this.Results = new System.Windows.Forms.ListBox();
            this.cmbType = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.CodeunitURL = new System.Windows.Forms.TextBox();
            this.PageURL = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnCodeunitWebService
            // 
            this.btnCodeunitWebService.Location = new System.Drawing.Point(10, 34);
            this.btnCodeunitWebService.Name = "btnCodeunitWebService";
            this.btnCodeunitWebService.Size = new System.Drawing.Size(332, 23);
            this.btnCodeunitWebService.TabIndex = 0;
            this.btnCodeunitWebService.Text = "Codeunit Web Service";
            this.btnCodeunitWebService.UseVisualStyleBackColor = true;
            this.btnCodeunitWebService.Click += new System.EventHandler(this.btnCodeunitWebService_Click);
            // 
            // btnPageWebService
            // 
            this.btnPageWebService.Location = new System.Drawing.Point(11, 62);
            this.btnPageWebService.Name = "btnPageWebService";
            this.btnPageWebService.Size = new System.Drawing.Size(332, 23);
            this.btnPageWebService.TabIndex = 1;
            this.btnPageWebService.Text = "Page Web Service";
            this.btnPageWebService.UseVisualStyleBackColor = true;
            this.btnPageWebService.Click += new System.EventHandler(this.btnPageWebService_Click);
            // 
            // Results
            // 
            this.Results.FormattingEnabled = true;
            this.Results.Location = new System.Drawing.Point(31, 193);
            this.Results.Name = "Results";
            this.Results.Size = new System.Drawing.Size(332, 212);
            this.Results.TabIndex = 2;
            // 
            // cmbType
            // 
            this.cmbType.FormattingEnabled = true;
            this.cmbType.Items.AddRange(new object[] {
            "Full List",
            "Filtered List Simple",
            "Filtered List Advanced",
            "READ",
            "CREATE",
            "CREATEMULTIPLE",
            "UPDATE",
            "UPDATEMULTIPLE",
            "DELETE"});
            this.cmbType.Location = new System.Drawing.Point(43, 35);
            this.cmbType.Name = "cmbType";
            this.cmbType.Size = new System.Drawing.Size(160, 21);
            this.cmbType.TabIndex = 3;
            this.cmbType.SelectedIndexChanged += new System.EventHandler(this.cmbType_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(8, 38);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(31, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Type";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 18);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "URL";
            // 
            // CodeunitURL
            // 
            this.CodeunitURL.Location = new System.Drawing.Point(42, 11);
            this.CodeunitURL.Name = "CodeunitURL";
            this.CodeunitURL.Size = new System.Drawing.Size(300, 20);
            this.CodeunitURL.TabIndex = 6;
            // 
            // PageURL
            // 
            this.PageURL.Location = new System.Drawing.Point(43, 6);
            this.PageURL.Name = "PageURL";
            this.PageURL.Size = new System.Drawing.Size(300, 20);
            this.PageURL.TabIndex = 8;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(8, 13);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(29, 13);
            this.label3.TabIndex = 7;
            this.label3.Text = "URL";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.CodeunitURL);
            this.panel1.Controls.Add(this.label2);
            this.panel1.Controls.Add(this.btnCodeunitWebService);
            this.panel1.Location = new System.Drawing.Point(21, 2);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(354, 69);
            this.panel1.TabIndex = 9;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.PageURL);
            this.panel2.Controls.Add(this.label3);
            this.panel2.Controls.Add(this.label1);
            this.panel2.Controls.Add(this.cmbType);
            this.panel2.Controls.Add(this.btnPageWebService);
            this.panel2.Location = new System.Drawing.Point(21, 83);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(353, 98);
            this.panel2.TabIndex = 10;
            // 
            // CSDynamicNAVWebServices
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(576, 434);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.Results);
            this.Name = "CSDynamicNAVWebServices";
            this.Text = "CSDynamicNAVWebServices";
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnCodeunitWebService;
        private System.Windows.Forms.Button btnPageWebService;
        private System.Windows.Forms.ListBox Results;
        private System.Windows.Forms.ComboBox cmbType;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox CodeunitURL;
        private System.Windows.Forms.TextBox PageURL;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
    }
}

