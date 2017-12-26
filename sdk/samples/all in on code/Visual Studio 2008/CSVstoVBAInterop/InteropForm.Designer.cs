namespace CSVstoVBAInterop
{
    partial class InteropForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(InteropForm));
            this.chkEnableVbaAccess = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.cboProjects = new System.Windows.Forms.ComboBox();
            this.btnRefresh = new System.Windows.Forms.Button();
            this.txtVbaSub = new System.Windows.Forms.TextBox();
            this.btnInsertRun = new System.Windows.Forms.Button();
            this.btnShowVBE = new System.Windows.Forms.Button();
            this.lblCode = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // chkEnableVbaAccess
            // 
            this.chkEnableVbaAccess.AutoSize = true;
            this.chkEnableVbaAccess.Location = new System.Drawing.Point(12, 12);
            this.chkEnableVbaAccess.Name = "chkEnableVbaAccess";
            this.chkEnableVbaAccess.Size = new System.Drawing.Size(188, 17);
            this.chkEnableVbaAccess.TabIndex = 0;
            this.chkEnableVbaAccess.Text = "Enable Access to VBA project OM";
            this.chkEnableVbaAccess.UseVisualStyleBackColor = true;
            this.chkEnableVbaAccess.CheckedChanged += new System.EventHandler(this.chkEnableVbaAccess_CheckedChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 51);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(94, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Available Projects:";
            // 
            // cboProjects
            // 
            this.cboProjects.DisplayMember = "Name";
            this.cboProjects.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboProjects.FormattingEnabled = true;
            this.cboProjects.Location = new System.Drawing.Point(112, 48);
            this.cboProjects.Name = "cboProjects";
            this.cboProjects.Size = new System.Drawing.Size(178, 21);
            this.cboProjects.TabIndex = 2;
            // 
            // btnRefresh
            // 
            this.btnRefresh.Location = new System.Drawing.Point(296, 46);
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Size = new System.Drawing.Size(75, 23);
            this.btnRefresh.TabIndex = 3;
            this.btnRefresh.Text = "Refresh";
            this.btnRefresh.UseVisualStyleBackColor = true;
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            // 
            // txtVbaSub
            // 
            this.txtVbaSub.Location = new System.Drawing.Point(15, 97);
            this.txtVbaSub.Multiline = true;
            this.txtVbaSub.Name = "txtVbaSub";
            this.txtVbaSub.ReadOnly = true;
            this.txtVbaSub.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtVbaSub.Size = new System.Drawing.Size(356, 176);
            this.txtVbaSub.TabIndex = 5;
            this.txtVbaSub.Text = resources.GetString("txtVbaSub.Text");
            // 
            // btnInsertRun
            // 
            this.btnInsertRun.Location = new System.Drawing.Point(229, 279);
            this.btnInsertRun.Name = "btnInsertRun";
            this.btnInsertRun.Size = new System.Drawing.Size(142, 23);
            this.btnInsertRun.TabIndex = 7;
            this.btnInsertRun.Text = "Insert VBA Code && Run";
            this.btnInsertRun.UseVisualStyleBackColor = true;
            this.btnInsertRun.Click += new System.EventHandler(this.btnInsertRun_Click);
            // 
            // btnShowVBE
            // 
            this.btnShowVBE.Location = new System.Drawing.Point(111, 279);
            this.btnShowVBE.Name = "btnShowVBE";
            this.btnShowVBE.Size = new System.Drawing.Size(112, 23);
            this.btnShowVBE.TabIndex = 6;
            this.btnShowVBE.Text = "Show VBA IDE";
            this.btnShowVBE.UseVisualStyleBackColor = true;
            this.btnShowVBE.Click += new System.EventHandler(this.btnShowVBE_Click);
            // 
            // lblCode
            // 
            this.lblCode.AutoSize = true;
            this.lblCode.Location = new System.Drawing.Point(12, 81);
            this.lblCode.Name = "lblCode";
            this.lblCode.Size = new System.Drawing.Size(119, 13);
            this.lblCode.TabIndex = 4;
            this.lblCode.Text = "VBA Code to be added:";
            // 
            // InteropForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(383, 312);
            this.Controls.Add(this.lblCode);
            this.Controls.Add(this.btnShowVBE);
            this.Controls.Add(this.btnInsertRun);
            this.Controls.Add(this.txtVbaSub);
            this.Controls.Add(this.btnRefresh);
            this.Controls.Add(this.cboProjects);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.chkEnableVbaAccess);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "InteropForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "VBA Interop Demo";
            this.Load += new System.EventHandler(this.InteropForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.CheckBox chkEnableVbaAccess;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox cboProjects;
        private System.Windows.Forms.Button btnRefresh;
        private System.Windows.Forms.TextBox txtVbaSub;
        private System.Windows.Forms.Button btnInsertRun;
        private System.Windows.Forms.Button btnShowVBE;
        private System.Windows.Forms.Label lblCode;
    }
}