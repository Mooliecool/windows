using System.Security.Permissions;
namespace CSWebBrowserWithProxy
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
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        private void InitializeComponent()
        {
            this.pnlMenu = new System.Windows.Forms.Panel();
            this.cmbProxy = new System.Windows.Forms.ComboBox();
            this.radUseProxy = new System.Windows.Forms.RadioButton();
            this.tbUrl = new System.Windows.Forms.TextBox();
            this.radIEProxy = new System.Windows.Forms.RadioButton();
            this.btnNavigate = new System.Windows.Forms.Button();
            this.pnlBrowser = new System.Windows.Forms.Panel();
            this.pnlToolbar = new System.Windows.Forms.Panel();
            this.pnlStatus = new System.Windows.Forms.Panel();
            this.lbStatus = new System.Windows.Forms.Label();
            this.prgBrowserProcess = new System.Windows.Forms.ProgressBar();
            this.wbcSample = new CSWebBrowserWithProxy.WebBrowserWithProxy();
            this.pnlMenu.SuspendLayout();
            this.pnlBrowser.SuspendLayout();
            this.pnlToolbar.SuspendLayout();
            this.pnlStatus.SuspendLayout();
            this.SuspendLayout();
            // 
            // pnlMenu
            // 
            this.pnlMenu.Controls.Add(this.cmbProxy);
            this.pnlMenu.Controls.Add(this.radUseProxy);
            this.pnlMenu.Controls.Add(this.tbUrl);
            this.pnlMenu.Controls.Add(this.radIEProxy);
            this.pnlMenu.Controls.Add(this.btnNavigate);
            this.pnlMenu.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlMenu.Location = new System.Drawing.Point(0, 0);
            this.pnlMenu.Name = "pnlMenu";
            this.pnlMenu.Size = new System.Drawing.Size(824, 54);
            this.pnlMenu.TabIndex = 1;
            // 
            // cmbProxy
            // 
            this.cmbProxy.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbProxy.FormattingEnabled = true;
            this.cmbProxy.Items.AddRange(new object[] {
            "First Browser",
            "Second Browser"});
            this.cmbProxy.Location = new System.Drawing.Point(236, 23);
            this.cmbProxy.Name = "cmbProxy";
            this.cmbProxy.Size = new System.Drawing.Size(121, 21);
            this.cmbProxy.TabIndex = 10;
            // 
            // radUseProxy
            // 
            this.radUseProxy.AutoSize = true;
            this.radUseProxy.Location = new System.Drawing.Point(145, 25);
            this.radUseProxy.Name = "radUseProxy";
            this.radUseProxy.Size = new System.Drawing.Size(85, 17);
            this.radUseProxy.TabIndex = 6;
            this.radUseProxy.Text = "Proxy Server";
            this.radUseProxy.UseVisualStyleBackColor = true;
            // 
            // tbUrl
            // 
            this.tbUrl.Dock = System.Windows.Forms.DockStyle.Top;
            this.tbUrl.Location = new System.Drawing.Point(0, 0);
            this.tbUrl.Name = "tbUrl";
            this.tbUrl.Size = new System.Drawing.Size(824, 20);
            this.tbUrl.TabIndex = 5;
            this.tbUrl.Text = "http://www.whatsmyip.us/";
            // 
            // radIEProxy
            // 
            this.radIEProxy.AutoSize = true;
            this.radIEProxy.Checked = true;
            this.radIEProxy.Location = new System.Drawing.Point(12, 25);
            this.radIEProxy.Name = "radIEProxy";
            this.radIEProxy.Size = new System.Drawing.Size(127, 17);
            this.radIEProxy.TabIndex = 4;
            this.radIEProxy.TabStop = true;
            this.radIEProxy.Text = "Use IE Proxy Settings";
            this.radIEProxy.UseVisualStyleBackColor = true;
            // 
            // btnNavigate
            // 
            this.btnNavigate.Location = new System.Drawing.Point(388, 22);
            this.btnNavigate.Name = "btnNavigate";
            this.btnNavigate.Size = new System.Drawing.Size(79, 23);
            this.btnNavigate.TabIndex = 1;
            this.btnNavigate.Text = "Go";
            this.btnNavigate.UseVisualStyleBackColor = true;
            this.btnNavigate.Click += new System.EventHandler(this.btnNavigate_Click);
            // 
            // pnlBrowser
            // 
            this.pnlBrowser.Controls.Add(this.pnlToolbar);
            this.pnlBrowser.Controls.Add(this.wbcSample);
            this.pnlBrowser.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlBrowser.Location = new System.Drawing.Point(0, 54);
            this.pnlBrowser.Name = "pnlBrowser";
            this.pnlBrowser.Size = new System.Drawing.Size(824, 557);
            this.pnlBrowser.TabIndex = 2;
            // 
            // pnlToolbar
            // 
            this.pnlToolbar.Controls.Add(this.pnlStatus);
            this.pnlToolbar.Controls.Add(this.prgBrowserProcess);
            this.pnlToolbar.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlToolbar.Location = new System.Drawing.Point(0, 530);
            this.pnlToolbar.Name = "pnlToolbar";
            this.pnlToolbar.Size = new System.Drawing.Size(824, 27);
            this.pnlToolbar.TabIndex = 1;
            // 
            // pnlStatus
            // 
            this.pnlStatus.Controls.Add(this.lbStatus);
            this.pnlStatus.Dock = System.Windows.Forms.DockStyle.Left;
            this.pnlStatus.Location = new System.Drawing.Point(0, 0);
            this.pnlStatus.Name = "pnlStatus";
            this.pnlStatus.Size = new System.Drawing.Size(275, 27);
            this.pnlStatus.TabIndex = 14;
            // 
            // lbStatus
            // 
            this.lbStatus.AutoSize = true;
            this.lbStatus.Location = new System.Drawing.Point(12, 10);
            this.lbStatus.Name = "lbStatus";
            this.lbStatus.Size = new System.Drawing.Size(0, 13);
            this.lbStatus.TabIndex = 13;
            // 
            // prgBrowserProcess
            // 
            this.prgBrowserProcess.Dock = System.Windows.Forms.DockStyle.Fill;
            this.prgBrowserProcess.Location = new System.Drawing.Point(0, 0);
            this.prgBrowserProcess.Name = "prgBrowserProcess";
            this.prgBrowserProcess.Size = new System.Drawing.Size(824, 27);
            this.prgBrowserProcess.TabIndex = 12;
            // 
            // wbcSample
            // 
            this.wbcSample.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wbcSample.Location = new System.Drawing.Point(0, 0);
            this.wbcSample.MinimumSize = new System.Drawing.Size(20, 20);
            this.wbcSample.Name = "wbcSample";
            this.wbcSample.ScriptErrorsSuppressed = true;
            this.wbcSample.Size = new System.Drawing.Size(824, 557);
            this.wbcSample.TabIndex = 0;
            this.wbcSample.ProgressChanged += new System.Windows.Forms.WebBrowserProgressChangedEventHandler(this.wbcSample_ProgressChanged);
            // 
            // MainForm
            // 
            this.AcceptButton = this.btnNavigate;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(824, 611);
            this.Controls.Add(this.pnlBrowser);
            this.Controls.Add(this.pnlMenu);
            this.Name = "MainForm";
            this.Text = "CSWebBrowserWithProxy";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.pnlMenu.ResumeLayout(false);
            this.pnlMenu.PerformLayout();
            this.pnlBrowser.ResumeLayout(false);
            this.pnlToolbar.ResumeLayout(false);
            this.pnlStatus.ResumeLayout(false);
            this.pnlStatus.PerformLayout();
            this.ResumeLayout(false);

        }


        #endregion


        private System.Windows.Forms.Panel pnlMenu;
        private System.Windows.Forms.RadioButton radUseProxy;
        private System.Windows.Forms.TextBox tbUrl;
        private System.Windows.Forms.RadioButton radIEProxy;
        private System.Windows.Forms.Button btnNavigate;
        private System.Windows.Forms.ComboBox cmbProxy;
        private System.Windows.Forms.Panel pnlBrowser;
        private WebBrowserWithProxy wbcSample;
        private System.Windows.Forms.Panel pnlToolbar;
        private System.Windows.Forms.ProgressBar prgBrowserProcess;
        private System.Windows.Forms.Panel pnlStatus;
        private System.Windows.Forms.Label lbStatus;
    }
}

