namespace Microsoft.Samples.Windows.Forms.ClickOnce.OnDemand
{
	public partial class MainForm : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.panel1 = new System.Windows.Forms.Panel();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.orderButton = new System.Windows.Forms.Button();
            this.customerButton = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.downloadAsyncBox = new System.Windows.Forms.CheckBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.customerLocalBox = new System.Windows.Forms.TextBox();
            this.orderLocalBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.isDeployedBox = new System.Windows.Forms.TextBox();
            this.MainPanel = new System.Windows.Forms.Panel();
            this.label4 = new System.Windows.Forms.Label();
            this.panel1.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.groupBox3);
            this.panel1.Controls.Add(this.groupBox2);
            this.panel1.Controls.Add(this.groupBox1);
            this.panel1.Location = new System.Drawing.Point(9, 9);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(203, 320);
            this.panel1.TabIndex = 5;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.orderButton);
            this.groupBox3.Controls.Add(this.customerButton);
            this.groupBox3.Location = new System.Drawing.Point(17, 179);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(173, 126);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "On Demand Components";
            // 
            // orderButton
            // 
            this.orderButton.Location = new System.Drawing.Point(16, 73);
            this.orderButton.Name = "orderButton";
            this.orderButton.Size = new System.Drawing.Size(142, 32);
            this.orderButton.TabIndex = 1;
            this.orderButton.Text = "&Order Tracking";
            this.orderButton.Click += new System.EventHandler(this.orderButton_Click);
            // 
            // customerButton
            // 
            this.customerButton.Location = new System.Drawing.Point(16, 32);
            this.customerButton.Name = "customerButton";
            this.customerButton.Size = new System.Drawing.Size(142, 32);
            this.customerButton.TabIndex = 0;
            this.customerButton.Text = "&Customer Tracking";
            this.customerButton.Click += new System.EventHandler(this.customerButton_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.downloadAsyncBox);
            this.groupBox2.Location = new System.Drawing.Point(16, 120);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(174, 52);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Deployment Config";
            // 
            // downloadAsyncBox
            // 
            this.downloadAsyncBox.AutoSize = true;
            this.downloadAsyncBox.Location = new System.Drawing.Point(16, 20);
            this.downloadAsyncBox.Name = "downloadAsyncBox";
            this.downloadAsyncBox.Size = new System.Drawing.Size(106, 17);
            this.downloadAsyncBox.TabIndex = 0;
            this.downloadAsyncBox.Text = "&Download Async";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.customerLocalBox);
            this.groupBox1.Controls.Add(this.orderLocalBox);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.isDeployedBox);
            this.groupBox1.Location = new System.Drawing.Point(15, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(175, 98);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Deployment State";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(15, 71);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(65, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Order Local:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(15, 46);
            this.label2.Margin = new System.Windows.Forms.Padding(3, 1, 3, 3);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(83, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Customer Local:\r\n";
            // 
            // customerLocalBox
            // 
            this.customerLocalBox.Enabled = false;
            this.customerLocalBox.Location = new System.Drawing.Point(108, 43);
            this.customerLocalBox.Margin = new System.Windows.Forms.Padding(3, 3, 3, 0);
            this.customerLocalBox.Name = "customerLocalBox";
            this.customerLocalBox.Size = new System.Drawing.Size(52, 20);
            this.customerLocalBox.TabIndex = 4;
            // 
            // orderLocalBox
            // 
            this.orderLocalBox.Enabled = false;
            this.orderLocalBox.Location = new System.Drawing.Point(108, 68);
            this.orderLocalBox.Margin = new System.Windows.Forms.Padding(3, 1, 3, 3);
            this.orderLocalBox.Name = "orderLocalBox";
            this.orderLocalBox.Size = new System.Drawing.Size(52, 20);
            this.orderLocalBox.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(15, 21);
            this.label1.Margin = new System.Windows.Forms.Padding(3, 3, 3, 2);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(74, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "App Deployed";
            // 
            // isDeployedBox
            // 
            this.isDeployedBox.Enabled = false;
            this.isDeployedBox.Location = new System.Drawing.Point(108, 18);
            this.isDeployedBox.Name = "isDeployedBox";
            this.isDeployedBox.Size = new System.Drawing.Size(52, 20);
            this.isDeployedBox.TabIndex = 2;
            // 
            // MainPanel
            // 
            this.MainPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.MainPanel.Location = new System.Drawing.Point(212, 9);
            this.MainPanel.Name = "MainPanel";
            this.MainPanel.Size = new System.Drawing.Size(318, 320);
            this.MainPanel.TabIndex = 6;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(11, 341);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(496, 65);
            this.label4.TabIndex = 7;
            this.label4.Text = resources.GetString("label4.Text");
            // 
            // MainForm
            // 
            this.AutoSize = true;
            this.ClientSize = new System.Drawing.Size(539, 419);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.MainPanel);
            this.Controls.Add(this.panel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.Padding = new System.Windows.Forms.Padding(9);
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "ClickOnce - OnDemand Sample";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.panel1.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
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
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Button customerButton;
		private System.Windows.Forms.Panel MainPanel;
		private System.Windows.Forms.Button orderButton;
		private System.Windows.Forms.CheckBox downloadAsyncBox;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox isDeployedBox;
		private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.TextBox orderLocalBox;
        private System.Windows.Forms.TextBox customerLocalBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
	}
}

