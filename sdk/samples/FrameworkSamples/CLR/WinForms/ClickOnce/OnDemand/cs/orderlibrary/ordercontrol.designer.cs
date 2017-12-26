namespace Microsoft.Samples.Windows.Forms.ClickOnce.OnDemand
{
	public partial class OrderControl: System.Windows.Forms.UserControl
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
			base.Dispose( disposing );
		}

		#region Component Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.label1 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.panel1 = new System.Windows.Forms.Panel();
			this.orderListControl1 = new Microsoft.Samples.Windows.Forms.ClickOnce.OnDemand.OrderListControl();
			this.panel1.SuspendLayout();
			this.SuspendLayout();
// 
// label1
// 
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.Location = new System.Drawing.Point(62, 26);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(213, 34);
			this.label1.TabIndex = 0;
			this.label1.Text = "Order Module";
// 
// label5
// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(71, 57);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(160, 14);
			this.label5.TabIndex = 9;
			this.label5.Text = "This is a mutli-assembly group. ";
// 
// panel1
// 
			this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.panel1.Controls.Add(this.orderListControl1);
			this.panel1.Location = new System.Drawing.Point(17, 97);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(275, 185);
			this.panel1.TabIndex = 10;
// 
// orderListControl1
// 
			this.orderListControl1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.orderListControl1.Location = new System.Drawing.Point(0, 0);
			this.orderListControl1.Name = "orderListControl1";
			this.orderListControl1.Size = new System.Drawing.Size(273, 183);
			this.orderListControl1.TabIndex = 0;
// 
// OrderControl
// 
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.label1);
			this.Name = "OrderControl";
			this.Size = new System.Drawing.Size(327, 315);
			this.panel1.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Panel panel1;
		private OrderListControl orderListControl1;
	}
}
