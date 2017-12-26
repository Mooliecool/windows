namespace Microsoft.Samples
{
	public partial class FormHelp : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.GroupUsageParse = new System.Windows.Forms.GroupBox();
			this.labelUsagePatternParse = new System.Windows.Forms.Label();
			this.GroupUsageTryParse = new System.Windows.Forms.GroupBox();
			this.labelUsagePatternTryParse = new System.Windows.Forms.Label();
			this.labelQuestion = new System.Windows.Forms.Label();
			this.labelDesc = new System.Windows.Forms.Label();
			this.GroupUsageParse.SuspendLayout();
			this.GroupUsageTryParse.SuspendLayout();
			this.SuspendLayout();
// 
// GroupUsageParse
// 
			this.GroupUsageParse.Controls.Add(this.labelUsagePatternParse);
			this.GroupUsageParse.Location = new System.Drawing.Point(431, 9);
			this.GroupUsageParse.Name = "GroupUsageParse";
			this.GroupUsageParse.Size = new System.Drawing.Size(325, 140);
			this.GroupUsageParse.TabIndex = 24;
			this.GroupUsageParse.TabStop = false;
			this.GroupUsageParse.Text = "Usage Pattern (Parse)";
// 
// labelUsagePatternParse
// 
			this.labelUsagePatternParse.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelUsagePatternParse.Location = new System.Drawing.Point(7, 19);
			this.labelUsagePatternParse.Name = "labelUsagePatternParse";
			this.labelUsagePatternParse.Size = new System.Drawing.Size(312, 114);
			this.labelUsagePatternParse.TabIndex = 19;
// 
// GroupUsageTryParse
// 
			this.GroupUsageTryParse.Controls.Add(this.labelUsagePatternTryParse);
			this.GroupUsageTryParse.Location = new System.Drawing.Point(430, 176);
			this.GroupUsageTryParse.Name = "GroupUsageTryParse";
			this.GroupUsageTryParse.Size = new System.Drawing.Size(326, 195);
			this.GroupUsageTryParse.TabIndex = 23;
			this.GroupUsageTryParse.TabStop = false;
			this.GroupUsageTryParse.Text = "Usage Pattern (TryParse)";
// 
// labelUsagePatternTryParse
// 
			this.labelUsagePatternTryParse.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelUsagePatternTryParse.Location = new System.Drawing.Point(7, 17);
			this.labelUsagePatternTryParse.Name = "labelUsagePatternTryParse";
			this.labelUsagePatternTryParse.Size = new System.Drawing.Size(312, 161);
			this.labelUsagePatternTryParse.TabIndex = 15;
// 
// labelQuestion
// 
			this.labelQuestion.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelQuestion.Location = new System.Drawing.Point(9, 7);
			this.labelQuestion.Name = "labelQuestion";
			this.labelQuestion.Size = new System.Drawing.Size(397, 21);
			this.labelQuestion.TabIndex = 22;
			this.labelQuestion.Text = "How does TryParse differ from Parse?";
// 
// labelDesc
// 
			this.labelDesc.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelDesc.Location = new System.Drawing.Point(9, 45);
			this.labelDesc.Name = "labelDesc";
			this.labelDesc.Size = new System.Drawing.Size(414, 343);
			this.labelDesc.TabIndex = 21;
// 
// FormHelp
// 
			this.ClientSize = new System.Drawing.Size(765, 397);
			this.Controls.Add(this.GroupUsageParse);
			this.Controls.Add(this.GroupUsageTryParse);
			this.Controls.Add(this.labelQuestion);
			this.Controls.Add(this.labelDesc);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "FormHelp";
			this.Padding = new System.Windows.Forms.Padding(9);
			this.Text = "Parse Vs. TryParse";
			this.TopMost = true;
			this.Load += new System.EventHandler(this.FormHelp_Load);
			this.GroupUsageParse.ResumeLayout(false);
			this.GroupUsageTryParse.ResumeLayout(false);
			this.ResumeLayout(false);

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

		private System.Windows.Forms.GroupBox GroupUsageParse;
		private System.Windows.Forms.Label labelUsagePatternParse;
		private System.Windows.Forms.GroupBox GroupUsageTryParse;
		private System.Windows.Forms.Label labelUsagePatternTryParse;
		private System.Windows.Forms.Label labelQuestion;
		private System.Windows.Forms.Label labelDesc;
	}
}
