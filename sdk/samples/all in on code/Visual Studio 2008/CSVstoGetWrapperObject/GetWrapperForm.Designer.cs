namespace CSVstoGetWrapperObject
{
    partial class GetWrapperForm
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
            this.lblWorkbooks = new System.Windows.Forms.Label();
            this.cboWorkbooks = new System.Windows.Forms.ComboBox();
            this.btnRefreshWb = new System.Windows.Forms.Button();
            this.lblWorksheets = new System.Windows.Forms.Label();
            this.cboWorksheets = new System.Windows.Forms.ComboBox();
            this.btnRefreshWs = new System.Windows.Forms.Button();
            this.btnAddListObject = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // lblWorkbooks
            // 
            this.lblWorkbooks.AutoSize = true;
            this.lblWorkbooks.Location = new System.Drawing.Point(12, 15);
            this.lblWorkbooks.Name = "lblWorkbooks";
            this.lblWorkbooks.Size = new System.Drawing.Size(65, 13);
            this.lblWorkbooks.TabIndex = 0;
            this.lblWorkbooks.Text = "Workbooks:";
            // 
            // cboWorkbooks
            // 
            this.cboWorkbooks.DisplayMember = "Name";
            this.cboWorkbooks.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboWorkbooks.FormattingEnabled = true;
            this.cboWorkbooks.Location = new System.Drawing.Point(85, 12);
            this.cboWorkbooks.Name = "cboWorkbooks";
            this.cboWorkbooks.Size = new System.Drawing.Size(181, 21);
            this.cboWorkbooks.TabIndex = 1;
            this.cboWorkbooks.SelectedIndexChanged += new System.EventHandler(this.cboWorkbooks_SelectedIndexChanged);
            // 
            // btnRefreshWb
            // 
            this.btnRefreshWb.Location = new System.Drawing.Point(272, 10);
            this.btnRefreshWb.Name = "btnRefreshWb";
            this.btnRefreshWb.Size = new System.Drawing.Size(75, 23);
            this.btnRefreshWb.TabIndex = 2;
            this.btnRefreshWb.Text = "Refresh";
            this.btnRefreshWb.UseVisualStyleBackColor = true;
            this.btnRefreshWb.Click += new System.EventHandler(this.btnRefreshWb_Click);
            // 
            // lblWorksheets
            // 
            this.lblWorksheets.AutoSize = true;
            this.lblWorksheets.Location = new System.Drawing.Point(12, 62);
            this.lblWorksheets.Name = "lblWorksheets";
            this.lblWorksheets.Size = new System.Drawing.Size(67, 13);
            this.lblWorksheets.TabIndex = 3;
            this.lblWorksheets.Text = "Worksheets:";
            // 
            // cboWorksheets
            // 
            this.cboWorksheets.DisplayMember = "Name";
            this.cboWorksheets.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboWorksheets.FormattingEnabled = true;
            this.cboWorksheets.Location = new System.Drawing.Point(85, 59);
            this.cboWorksheets.Name = "cboWorksheets";
            this.cboWorksheets.Size = new System.Drawing.Size(181, 21);
            this.cboWorksheets.TabIndex = 4;
            // 
            // btnRefreshWs
            // 
            this.btnRefreshWs.Location = new System.Drawing.Point(272, 57);
            this.btnRefreshWs.Name = "btnRefreshWs";
            this.btnRefreshWs.Size = new System.Drawing.Size(75, 23);
            this.btnRefreshWs.TabIndex = 5;
            this.btnRefreshWs.Text = "Refresh";
            this.btnRefreshWs.UseVisualStyleBackColor = true;
            this.btnRefreshWs.Click += new System.EventHandler(this.btnRefreshWs_Click);
            // 
            // btnAddListObject
            // 
            this.btnAddListObject.Enabled = false;
            this.btnAddListObject.Location = new System.Drawing.Point(195, 104);
            this.btnAddListObject.Name = "btnAddListObject";
            this.btnAddListObject.Size = new System.Drawing.Size(152, 23);
            this.btnAddListObject.TabIndex = 6;
            this.btnAddListObject.Text = "Add VSTO ListObject";
            this.btnAddListObject.UseVisualStyleBackColor = true;
            this.btnAddListObject.Click += new System.EventHandler(this.btnAddListObject_Click);
            // 
            // GetWrapperForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(373, 144);
            this.Controls.Add(this.btnAddListObject);
            this.Controls.Add(this.btnRefreshWs);
            this.Controls.Add(this.cboWorksheets);
            this.Controls.Add(this.lblWorksheets);
            this.Controls.Add(this.btnRefreshWb);
            this.Controls.Add(this.cboWorkbooks);
            this.Controls.Add(this.lblWorkbooks);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "GetWrapperForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "GetVstoObject demo";
            this.Load += new System.EventHandler(this.GetWrapperForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lblWorkbooks;
        private System.Windows.Forms.ComboBox cboWorkbooks;
        private System.Windows.Forms.Button btnRefreshWb;
        private System.Windows.Forms.Label lblWorksheets;
        private System.Windows.Forms.ComboBox cboWorksheets;
        private System.Windows.Forms.Button btnRefreshWs;
        private System.Windows.Forms.Button btnAddListObject;

    }
}