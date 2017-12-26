namespace Microsoft.Samples.Windows.Forms.ClickOnce.OnDemand
{
	public partial class DownloadProgressForm : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
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
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.closeButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.currentBytesLabel = new System.Windows.Forms.Label();
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.totalBytesLabel = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.currentStateLabel = new System.Windows.Forms.Label();
            this.flowLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
// 
// progressBar1
// 
            this.progressBar1.Location = new System.Drawing.Point(27, 26);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(338, 21);
            this.progressBar1.TabIndex = 0;
// 
// closeButton
// 
            this.closeButton.Location = new System.Drawing.Point(291, 109);
            this.closeButton.Name = "closeButton";
            this.closeButton.Size = new System.Drawing.Size(74, 26);
            this.closeButton.TabIndex = 1;
            this.closeButton.Text = "Cancel";
            this.closeButton.Click += new System.EventHandler(this.closeButton_Click);
// 
// label1
// 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(96, 3);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(14, 14);
            this.label1.TabIndex = 2;
            this.label1.Text = "of";
// 
// label2
// 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(3, 3);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(70, 14);
            this.label2.TabIndex = 3;
            this.label2.Text = "Downloaded:  ";
// 
// label3
// 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(133, 3);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(31, 14);
            this.label3.TabIndex = 4;
            this.label3.Text = "bytes";
// 
// currentBytesLabel
// 
            this.currentBytesLabel.AutoSize = true;
            this.currentBytesLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.currentBytesLabel.Location = new System.Drawing.Point(79, 3);
            this.currentBytesLabel.Name = "currentBytesLabel";
            this.currentBytesLabel.Size = new System.Drawing.Size(11, 14);
            this.currentBytesLabel.TabIndex = 5;
            this.currentBytesLabel.Text = "0";
// 
// flowLayoutPanel1
// 
            this.flowLayoutPanel1.Controls.Add(this.label2);
            this.flowLayoutPanel1.Controls.Add(this.currentBytesLabel);
            this.flowLayoutPanel1.Controls.Add(this.label1);
            this.flowLayoutPanel1.Controls.Add(this.totalBytesLabel);
            this.flowLayoutPanel1.Controls.Add(this.label3);
            this.flowLayoutPanel1.Location = new System.Drawing.Point(27, 54);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(280, 23);
            this.flowLayoutPanel1.TabIndex = 6;
// 
// totalBytesLabel
// 
            this.totalBytesLabel.AutoSize = true;
            this.totalBytesLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.totalBytesLabel.Location = new System.Drawing.Point(116, 3);
            this.totalBytesLabel.Name = "totalBytesLabel";
            this.totalBytesLabel.Size = new System.Drawing.Size(11, 14);
            this.totalBytesLabel.TabIndex = 7;
            this.totalBytesLabel.Text = "0";
// 
// label4
// 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(30, 84);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(98, 14);
            this.label4.TabIndex = 7;
            this.label4.Text = "Current Operation:";
// 
// currentStateLabel
// 
            this.currentStateLabel.AutoSize = true;
            this.currentStateLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.currentStateLabel.Location = new System.Drawing.Point(131, 84);
            this.currentStateLabel.Name = "currentStateLabel";
            this.currentStateLabel.Size = new System.Drawing.Size(18, 14);
            this.currentStateLabel.TabIndex = 8;
            this.currentStateLabel.Text = "<>";
// 
// DownloadProgressForm
// 
            this.AutoSize = true;
            this.ClientSize = new System.Drawing.Size(388, 156);
            this.Controls.Add(this.currentStateLabel);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.flowLayoutPanel1);
            this.Controls.Add(this.closeButton);
            this.Controls.Add(this.progressBar1);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "DownloadProgressForm";
            this.Padding = new System.Windows.Forms.Padding(9);
            this.Text = "Downloading Application Component";
            this.flowLayoutPanel1.ResumeLayout(false);
            this.flowLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

		#endregion

		private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Button closeButton;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label currentBytesLabel;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.Label totalBytesLabel;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label currentStateLabel;
    }
}
