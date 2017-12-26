namespace Microsoft.Samples.PingClient
{
    partial class PingClientForm
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
            this.pingLabel = new System.Windows.Forms.Label();
            this.addressTextBox = new System.Windows.Forms.TextBox();
            this.sendButton = new System.Windows.Forms.Button();
            this.pingDetailsTextBox = new System.Windows.Forms.TextBox();
            this.cancelButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // pingLabel
            // 
            this.pingLabel.AutoSize = true;
            this.pingLabel.Location = new System.Drawing.Point(12, 17);
            this.pingLabel.Name = "pingLabel";
            this.pingLabel.Size = new System.Drawing.Size(51, 14);
            this.pingLabel.TabIndex = 0;
            this.pingLabel.Text = "Name/IP:";
            // 
            // addressTextBox
            // 
            this.addressTextBox.Location = new System.Drawing.Point(69, 14);
            this.addressTextBox.Name = "addressTextBox";
            this.addressTextBox.Size = new System.Drawing.Size(211, 20);
            this.addressTextBox.TabIndex = 0;
            // 
            // sendButton
            // 
            this.sendButton.Location = new System.Drawing.Point(124, 231);
            this.sendButton.Name = "sendButton";
            this.sendButton.TabIndex = 1;
            this.sendButton.Text = "Send";
            this.sendButton.Click += new System.EventHandler(this.sendButton_Click);
            // 
            // pingDetailsTextBox
            // 
            this.pingDetailsTextBox.AutoSize = false;
            this.pingDetailsTextBox.Location = new System.Drawing.Point(12, 41);
            this.pingDetailsTextBox.Multiline = true;
            this.pingDetailsTextBox.Name = "pingDetailsTextBox";
            this.pingDetailsTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.pingDetailsTextBox.Size = new System.Drawing.Size(268, 184);
            this.pingDetailsTextBox.TabIndex = 3;
            // 
            // cancelButton
            // 
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(205, 231);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.TabIndex = 2;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // PingClientForm
            // 
            this.AcceptButton = this.sendButton;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.pingDetailsTextBox);
            this.Controls.Add(this.sendButton);
            this.Controls.Add(this.addressTextBox);
            this.Controls.Add(this.pingLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Name = "PingClientForm";
            this.Text = "Ping Client";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label pingLabel;
        private System.Windows.Forms.TextBox addressTextBox;
        private System.Windows.Forms.Button sendButton;
		private System.Windows.Forms.TextBox pingDetailsTextBox;
		private System.Windows.Forms.Button cancelButton;
    }
}

