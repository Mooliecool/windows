namespace Microsoft.Samples.WCF.UptimeMonitor.Client {
	partial class MainForm {
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;
		private System.Windows.Forms.MainMenu mainMenu1;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing) {
			if (disposing && (components != null)) {
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
            this.mainMenu1 = new System.Windows.Forms.MainMenu();
            this.updateButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.uptimeLabel = new System.Windows.Forms.Label();
            this.timestampLabel = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.hostBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // updateButton
            // 
            this.updateButton.Location = new System.Drawing.Point(3, 72);
            this.updateButton.Name = "updateButton";
            this.updateButton.Size = new System.Drawing.Size(233, 20);
            this.updateButton.TabIndex = 0;
            this.updateButton.Text = "Query Server Uptime";
            this.updateButton.Click += new System.EventHandler(this.updateButton_Click);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(3, 29);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(100, 20);
            this.label1.Text = "Server uptime:";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(3, 49);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(100, 20);
            this.label2.Text = "Last updated:";
            // 
            // uptimeLabel
            // 
            this.uptimeLabel.Location = new System.Drawing.Point(110, 28);
            this.uptimeLabel.Name = "uptimeLabel";
            this.uptimeLabel.Size = new System.Drawing.Size(126, 20);
            // 
            // timestampLabel
            // 
            this.timestampLabel.Location = new System.Drawing.Point(110, 48);
            this.timestampLabel.Name = "timestampLabel";
            this.timestampLabel.Size = new System.Drawing.Size(126, 20);
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(4, 4);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(100, 20);
            this.label3.Text = "Server:";
            // 
            // hostBox
            // 
            this.hostBox.Location = new System.Drawing.Point(111, 4);
            this.hostBox.Name = "hostBox";
            this.hostBox.Size = new System.Drawing.Size(125, 21);
            this.hostBox.TabIndex = 6;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 268);
            this.Controls.Add(this.hostBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.timestampLabel);
            this.Controls.Add(this.uptimeLabel);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.updateButton);
            this.Menu = this.mainMenu1;
            this.MinimizeBox = false;
            this.Name = "MainForm";
            this.Text = "Uptime Monitor";
            this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Button updateButton;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label uptimeLabel;
		private System.Windows.Forms.Label timestampLabel;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox hostBox;
	}
}

