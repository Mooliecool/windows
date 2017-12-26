namespace CSWebBrowserSuppressError
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
            this.pnlMenu = new System.Windows.Forms.Panel();
            this.chkSuppressAllDialog = new System.Windows.Forms.CheckBox();
            this.chkSuppressNavigationError = new System.Windows.Forms.CheckBox();
            this.tbUrl = new System.Windows.Forms.TextBox();
            this.chkSuppressJITDebugger = new System.Windows.Forms.CheckBox();
            this.chkSuppressHtmlElementError = new System.Windows.Forms.CheckBox();
            this.btnNavigate = new System.Windows.Forms.Button();
            this.lbUrl = new System.Windows.Forms.Label();
            this.pnlBrowser = new System.Windows.Forms.Panel();
            this.wbcSample = new CSWebBrowserSuppressError.WebBrowserEx();
            this.pnlMenu.SuspendLayout();
            this.pnlBrowser.SuspendLayout();
            this.SuspendLayout();
            // 
            // pnlMenu
            // 
            this.pnlMenu.Controls.Add(this.chkSuppressAllDialog);
            this.pnlMenu.Controls.Add(this.chkSuppressNavigationError);
            this.pnlMenu.Controls.Add(this.tbUrl);
            this.pnlMenu.Controls.Add(this.chkSuppressJITDebugger);
            this.pnlMenu.Controls.Add(this.chkSuppressHtmlElementError);
            this.pnlMenu.Controls.Add(this.btnNavigate);
            this.pnlMenu.Controls.Add(this.lbUrl);
            this.pnlMenu.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlMenu.Location = new System.Drawing.Point(0, 0);
            this.pnlMenu.Name = "pnlMenu";
            this.pnlMenu.Size = new System.Drawing.Size(824, 65);
            this.pnlMenu.TabIndex = 1;
            // 
            // chkSuppressAllDialog
            // 
            this.chkSuppressAllDialog.AutoSize = true;
            this.chkSuppressAllDialog.Location = new System.Drawing.Point(183, 42);
            this.chkSuppressAllDialog.Name = "chkSuppressAllDialog";
            this.chkSuppressAllDialog.Size = new System.Drawing.Size(119, 17);
            this.chkSuppressAllDialog.TabIndex = 7;
            this.chkSuppressAllDialog.Text = "Suppress all dialogs";
            this.chkSuppressAllDialog.UseVisualStyleBackColor = true;
            this.chkSuppressAllDialog.CheckedChanged += new System.EventHandler(this.chkSuppressAllDialog_CheckedChanged);
            // 
            // chkSuppressNavigationError
            // 
            this.chkSuppressNavigationError.AutoSize = true;
            this.chkSuppressNavigationError.Location = new System.Drawing.Point(308, 42);
            this.chkSuppressNavigationError.Name = "chkSuppressNavigationError";
            this.chkSuppressNavigationError.Size = new System.Drawing.Size(149, 17);
            this.chkSuppressNavigationError.TabIndex = 6;
            this.chkSuppressNavigationError.Text = "Suppress Navigation Error";
            this.chkSuppressNavigationError.UseVisualStyleBackColor = true;
            // 
            // tbUrl
            // 
            this.tbUrl.Dock = System.Windows.Forms.DockStyle.Top;
            this.tbUrl.Location = new System.Drawing.Point(0, 13);
            this.tbUrl.Name = "tbUrl";
            this.tbUrl.Size = new System.Drawing.Size(824, 20);
            this.tbUrl.TabIndex = 5;
            // 
            // chkSuppressJITDebugger
            // 
            this.chkSuppressJITDebugger.AutoSize = true;
            this.chkSuppressJITDebugger.Location = new System.Drawing.Point(463, 42);
            this.chkSuppressJITDebugger.Name = "chkSuppressJITDebugger";
            this.chkSuppressJITDebugger.Size = new System.Drawing.Size(138, 17);
            this.chkSuppressJITDebugger.TabIndex = 2;
            this.chkSuppressJITDebugger.Text = "Suppress JIT Debugger";
            this.chkSuppressJITDebugger.UseVisualStyleBackColor = true;
            // 
            // chkSuppressHtmlElementError
            // 
            this.chkSuppressHtmlElementError.AutoSize = true;
            this.chkSuppressHtmlElementError.Location = new System.Drawing.Point(12, 42);
            this.chkSuppressHtmlElementError.Name = "chkSuppressHtmlElementError";
            this.chkSuppressHtmlElementError.Size = new System.Drawing.Size(165, 17);
            this.chkSuppressHtmlElementError.TabIndex = 2;
            this.chkSuppressHtmlElementError.Text = "Suppress Html Element Errors";
            this.chkSuppressHtmlElementError.UseVisualStyleBackColor = true;
            this.chkSuppressHtmlElementError.CheckedChanged += new System.EventHandler(this.chkSuppressHtmlElementError_CheckedChanged);
            // 
            // btnNavigate
            // 
            this.btnNavigate.Location = new System.Drawing.Point(733, 36);
            this.btnNavigate.Name = "btnNavigate";
            this.btnNavigate.Size = new System.Drawing.Size(79, 23);
            this.btnNavigate.TabIndex = 1;
            this.btnNavigate.Text = "Go";
            this.btnNavigate.UseVisualStyleBackColor = true;
            this.btnNavigate.Click += new System.EventHandler(this.btnNavigate_Click);
            // 
            // lbUrl
            // 
            this.lbUrl.AutoSize = true;
            this.lbUrl.Dock = System.Windows.Forms.DockStyle.Top;
            this.lbUrl.Location = new System.Drawing.Point(0, 0);
            this.lbUrl.Name = "lbUrl";
            this.lbUrl.Size = new System.Drawing.Size(225, 13);
            this.lbUrl.TabIndex = 8;
            this.lbUrl.Text = "URL (leave blank to load the internal test html)";
            // 
            // pnlBrowser
            // 
            this.pnlBrowser.Controls.Add(this.wbcSample);
            this.pnlBrowser.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlBrowser.Location = new System.Drawing.Point(0, 65);
            this.pnlBrowser.Name = "pnlBrowser";
            this.pnlBrowser.Size = new System.Drawing.Size(824, 546);
            this.pnlBrowser.TabIndex = 2;
            // 
            // wbcSample
            // 
            this.wbcSample.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wbcSample.HtmlElementErrorsSuppressed = false;
            this.wbcSample.Location = new System.Drawing.Point(0, 0);
            this.wbcSample.MinimumSize = new System.Drawing.Size(20, 20);
            this.wbcSample.Name = "wbcSample";
            this.wbcSample.Size = new System.Drawing.Size(824, 546);
            this.wbcSample.TabIndex = 0;
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
            this.Text = "CSWebBrowserSuppressError";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.pnlMenu.ResumeLayout(false);
            this.pnlMenu.PerformLayout();
            this.pnlBrowser.ResumeLayout(false);
            this.ResumeLayout(false);

        }


        #endregion


        private System.Windows.Forms.Panel pnlMenu;
        private System.Windows.Forms.TextBox tbUrl;
        private System.Windows.Forms.CheckBox chkSuppressHtmlElementError;
        private System.Windows.Forms.Button btnNavigate;
        private System.Windows.Forms.CheckBox chkSuppressJITDebugger;
        private System.Windows.Forms.Panel pnlBrowser;
        private WebBrowserEx wbcSample;
        private System.Windows.Forms.CheckBox chkSuppressNavigationError;
        private System.Windows.Forms.CheckBox chkSuppressAllDialog;
        private System.Windows.Forms.Label lbUrl;
    }
}

