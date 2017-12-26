namespace CSWebBrowserLoadComplete
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
            this.pnlUrl = new System.Windows.Forms.Panel();
            this.tbURL = new System.Windows.Forms.TextBox();
            this.btnGo = new System.Windows.Forms.Button();
            this.stsLoad = new System.Windows.Forms.StatusStrip();
            this.lbStatus = new System.Windows.Forms.ToolStripStatusLabel();
            this.webEx = new CSWebBrowserLoadComplete.WebBrowserEx();
            this.lstActivities = new System.Windows.Forms.ListBox();
            this.pnlUrl.SuspendLayout();
            this.stsLoad.SuspendLayout();
            this.SuspendLayout();
            // 
            // pnlUrl
            // 
            this.pnlUrl.Controls.Add(this.tbURL);
            this.pnlUrl.Controls.Add(this.btnGo);
            this.pnlUrl.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlUrl.Location = new System.Drawing.Point(0, 0);
            this.pnlUrl.Name = "pnlUrl";
            this.pnlUrl.Size = new System.Drawing.Size(797, 21);
            this.pnlUrl.TabIndex = 0;
            // 
            // tbURL
            // 
            this.tbURL.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbURL.Location = new System.Drawing.Point(0, 0);
            this.tbURL.Name = "tbURL";
            this.tbURL.Size = new System.Drawing.Size(722, 20);
            this.tbURL.TabIndex = 2;
            this.tbURL.Text = "http://msdn.microsoft.com";
            // 
            // btnGo
            // 
            this.btnGo.Dock = System.Windows.Forms.DockStyle.Right;
            this.btnGo.Location = new System.Drawing.Point(722, 0);
            this.btnGo.Name = "btnGo";
            this.btnGo.Size = new System.Drawing.Size(75, 21);
            this.btnGo.TabIndex = 1;
            this.btnGo.Text = "GO";
            this.btnGo.UseVisualStyleBackColor = true;
            this.btnGo.Click += new System.EventHandler(this.btnGo_Click);
            // 
            // stsLoad
            // 
            this.stsLoad.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lbStatus});
            this.stsLoad.Location = new System.Drawing.Point(0, 396);
            this.stsLoad.Name = "stsLoad";
            this.stsLoad.Size = new System.Drawing.Size(797, 22);
            this.stsLoad.TabIndex = 1;
            // 
            // lbStatus
            // 
            this.lbStatus.Name = "lbStatus";
            this.lbStatus.Size = new System.Drawing.Size(0, 17);
            // 
            // webEx
            // 
            this.webEx.Dock = System.Windows.Forms.DockStyle.Fill;
            this.webEx.Location = new System.Drawing.Point(0, 21);
            this.webEx.MinimumSize = new System.Drawing.Size(20, 20);
            this.webEx.Name = "webEx";
            this.webEx.Size = new System.Drawing.Size(797, 241);
            this.webEx.TabIndex = 2;
            // 
            // lstActivities
            // 
            this.lstActivities.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.lstActivities.FormattingEnabled = true;
            this.lstActivities.Location = new System.Drawing.Point(0, 262);
            this.lstActivities.Name = "lstActivities";
            this.lstActivities.Size = new System.Drawing.Size(797, 134);
            this.lstActivities.TabIndex = 3;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(797, 418);
            this.Controls.Add(this.webEx);
            this.Controls.Add(this.lstActivities);
            this.Controls.Add(this.stsLoad);
            this.Controls.Add(this.pnlUrl);
            this.Name = "MainForm";
            this.Text = "CSWebBrowserLoadComplete";
            this.pnlUrl.ResumeLayout(false);
            this.pnlUrl.PerformLayout();
            this.stsLoad.ResumeLayout(false);
            this.stsLoad.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel pnlUrl;
        private System.Windows.Forms.TextBox tbURL;
        private System.Windows.Forms.Button btnGo;
        private System.Windows.Forms.StatusStrip stsLoad;
        private System.Windows.Forms.ToolStripStatusLabel lbStatus;
        private WebBrowserEx webEx;
        private System.Windows.Forms.ListBox lstActivities;
    }
}

