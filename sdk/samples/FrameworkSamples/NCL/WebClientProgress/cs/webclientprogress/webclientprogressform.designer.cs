namespace Microsoft.Samples.WebClientProgress
{
    public partial class WebClientProgressForm : System.Windows.Forms.Form
    {
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.downloadProgressBar = new System.Windows.Forms.ProgressBar();
            this.downloadButton = new System.Windows.Forms.Button();
            this.progressBarPanel = new System.Windows.Forms.Panel();
            this.urlTextBox = new System.Windows.Forms.TextBox();
            this.downloadUrlLabel = new System.Windows.Forms.Label();
            this.progressBarPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // downloadProgressBar
            // 
            this.downloadProgressBar.Location = new System.Drawing.Point(13, 21);
            this.downloadProgressBar.Name = "downloadProgressBar";
            this.downloadProgressBar.Size = new System.Drawing.Size(432, 18);
            this.downloadProgressBar.TabIndex = 0;
            // 
            // downloadButton
            // 
            this.downloadButton.Location = new System.Drawing.Point(367, 21);
            this.downloadButton.Name = "downloadButton";
            this.downloadButton.Size = new System.Drawing.Size(105, 25);
            this.downloadButton.TabIndex = 1;
            this.downloadButton.Text = "Download";
            this.downloadButton.Click += new System.EventHandler(this.btnDownload_Click);
            // 
            // progressBarPanel
            // 
            this.progressBarPanel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.progressBarPanel.Controls.Add(this.downloadProgressBar);
            this.progressBarPanel.Location = new System.Drawing.Point(18, 52);
            this.progressBarPanel.Name = "progressBarPanel";
            this.progressBarPanel.Size = new System.Drawing.Size(456, 58);
            this.progressBarPanel.TabIndex = 2;
            // 
            // urlTextBox
            // 
            this.urlTextBox.Location = new System.Drawing.Point(104, 24);
            this.urlTextBox.Name = "urlTextBox";
            this.urlTextBox.Size = new System.Drawing.Size(247, 20);
            this.urlTextBox.TabIndex = 3;
            // 
            // downloadUrlLabel
            // 
            this.downloadUrlLabel.Location = new System.Drawing.Point(18, 24);
            this.downloadUrlLabel.Name = "downloadUrlLabel";
            this.downloadUrlLabel.Size = new System.Drawing.Size(86, 18);
            this.downloadUrlLabel.TabIndex = 4;
            this.downloadUrlLabel.Text = "Download URL:";
            // 
            // Form1
            // 
            this.AcceptButton = this.downloadButton;
            this.AutoSize = true;
            this.ClientSize = new System.Drawing.Size(497, 143);
            this.Controls.Add(this.progressBarPanel);
            this.Controls.Add(this.downloadUrlLabel);
            this.Controls.Add(this.urlTextBox);
            this.Controls.Add(this.downloadButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Padding = new System.Windows.Forms.Padding(9);
            this.Text = "WebClient Download Sample";
            this.progressBarPanel.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

		/// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
		}

        private System.ComponentModel.IContainer components = null;
		private System.Windows.Forms.Button downloadButton;
		private System.Windows.Forms.ProgressBar downloadProgressBar;
		private System.Windows.Forms.Panel progressBarPanel;
		private System.Windows.Forms.TextBox urlTextBox;
		private System.Windows.Forms.Label downloadUrlLabel;
	}
}

