namespace Microsoft.Samples
{
	public partial class FormMain : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.buttonHelp = new System.Windows.Forms.Button();
			this.comboType = new System.Windows.Forms.ComboBox();
			this.labelType = new System.Windows.Forms.Label();
			this.labelDesc = new System.Windows.Forms.Label();
			this.numericIterations = new System.Windows.Forms.NumericUpDown();
			this.labelIterations = new System.Windows.Forms.Label();
			this.buttonExit = new System.Windows.Forms.Button();
			this.buttonGo = new System.Windows.Forms.Button();
			this.labelPercent = new System.Windows.Forms.Label();
			this.numericPercent = new System.Windows.Forms.NumericUpDown();
			this.groupResults = new System.Windows.Forms.GroupBox();
			this.labelPercentResultValue = new System.Windows.Forms.Label();
			this.labelPercentResult = new System.Windows.Forms.Label();
			this.labelParseResult = new System.Windows.Forms.Label();
			this.labelTryParseResult = new System.Windows.Forms.Label();
			this.labelParseResultValue = new System.Windows.Forms.Label();
			this.labelTryParseResultValue = new System.Windows.Forms.Label();
			((System.ComponentModel.ISupportInitialize)(this.numericIterations)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numericPercent)).BeginInit();
			this.groupResults.SuspendLayout();
			this.SuspendLayout();
// 
// buttonHelp
// 
			this.buttonHelp.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.buttonHelp.Location = new System.Drawing.Point(202, 52);
			this.buttonHelp.Name = "buttonHelp";
			this.buttonHelp.Size = new System.Drawing.Size(80, 34);
			this.buttonHelp.TabIndex = 23;
			this.buttonHelp.Text = "Help";
			this.buttonHelp.Click += new System.EventHandler(this.buttonHelp_Click);
// 
// comboType
// 
			this.comboType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboType.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.comboType.FormattingEnabled = true;
			this.comboType.Location = new System.Drawing.Point(64, 95);
			this.comboType.Name = "comboType";
			this.comboType.Size = new System.Drawing.Size(129, 24);
			this.comboType.TabIndex = 22;
// 
// labelType
// 
			this.labelType.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelType.Location = new System.Drawing.Point(8, 93);
			this.labelType.Name = "labelType";
			this.labelType.Size = new System.Drawing.Size(56, 25);
			this.labelType.TabIndex = 21;
			this.labelType.Text = "Type:";
// 
// labelDesc
// 
			this.labelDesc.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelDesc.Location = new System.Drawing.Point(8, 271);
			this.labelDesc.Name = "labelDesc";
			this.labelDesc.Size = new System.Drawing.Size(272, 106);
			this.labelDesc.TabIndex = 20;
// 
// numericIterations
// 
			this.numericIterations.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numericIterations.Location = new System.Drawing.Point(136, 54);
			this.numericIterations.Maximum = new System.Decimal(new int[] {
            70,
            0,
            0,
            0});
			this.numericIterations.Minimum = new System.Decimal(new int[] {
            20,
            0,
            0,
            0});
			this.numericIterations.Name = "numericIterations";
			this.numericIterations.Size = new System.Drawing.Size(56, 23);
			this.numericIterations.TabIndex = 19;
			this.numericIterations.Value = new System.Decimal(new int[] {
            40,
            0,
            0,
            0});
// 
// labelIterations
// 
			this.labelIterations.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelIterations.Location = new System.Drawing.Point(16, 54);
			this.labelIterations.Name = "labelIterations";
			this.labelIterations.Size = new System.Drawing.Size(96, 25);
			this.labelIterations.TabIndex = 18;
			this.labelIterations.Text = "Iterations:";
// 
// buttonExit
// 
			this.buttonExit.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.buttonExit.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.buttonExit.Location = new System.Drawing.Point(201, 95);
			this.buttonExit.Name = "buttonExit";
			this.buttonExit.Size = new System.Drawing.Size(80, 34);
			this.buttonExit.TabIndex = 17;
			this.buttonExit.Text = "Exit";
			this.buttonExit.Click += new System.EventHandler(this.buttonExit_Click);
// 
// buttonGo
// 
			this.buttonGo.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.buttonGo.Location = new System.Drawing.Point(201, 12);
			this.buttonGo.Name = "buttonGo";
			this.buttonGo.Size = new System.Drawing.Size(80, 33);
			this.buttonGo.TabIndex = 16;
			this.buttonGo.Text = "Run";
			this.buttonGo.Click += new System.EventHandler(this.buttonGo_Click);
// 
// labelPercent
// 
			this.labelPercent.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelPercent.Location = new System.Drawing.Point(8, 12);
			this.labelPercent.Name = "labelPercent";
			this.labelPercent.Size = new System.Drawing.Size(127, 25);
			this.labelPercent.TabIndex = 15;
			this.labelPercent.Text = "Success Rate";
// 
// numericPercent
// 
			this.numericPercent.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.numericPercent.Location = new System.Drawing.Point(136, 12);
			this.numericPercent.Name = "numericPercent";
			this.numericPercent.Size = new System.Drawing.Size(56, 23);
			this.numericPercent.TabIndex = 14;
			this.numericPercent.Value = new System.Decimal(new int[] {
            100,
            0,
            0,
            0});
// 
// groupResults
// 
			this.groupResults.Controls.Add(this.labelPercentResultValue);
			this.groupResults.Controls.Add(this.labelPercentResult);
			this.groupResults.Controls.Add(this.labelParseResult);
			this.groupResults.Controls.Add(this.labelTryParseResult);
			this.groupResults.Controls.Add(this.labelParseResultValue);
			this.groupResults.Controls.Add(this.labelTryParseResultValue);
			this.groupResults.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.groupResults.Location = new System.Drawing.Point(8, 129);
			this.groupResults.Name = "groupResults";
			this.groupResults.Size = new System.Drawing.Size(274, 133);
			this.groupResults.TabIndex = 13;
			this.groupResults.TabStop = false;
			this.groupResults.Text = "Results";
// 
// labelPercentResultValue
// 
			this.labelPercentResultValue.Location = new System.Drawing.Point(50, 93);
			this.labelPercentResultValue.Name = "labelPercentResultValue";
			this.labelPercentResultValue.Size = new System.Drawing.Size(132, 24);
			this.labelPercentResultValue.TabIndex = 5;
			this.labelPercentResultValue.TextAlign = System.Drawing.ContentAlignment.TopRight;
			this.labelPercentResultValue.Visible = false;
// 
// labelPercentResult
// 
			this.labelPercentResult.Location = new System.Drawing.Point(191, 92);
			this.labelPercentResult.Name = "labelPercentResult";
			this.labelPercentResult.Size = new System.Drawing.Size(68, 25);
			this.labelPercentResult.TabIndex = 4;
			this.labelPercentResult.Text = "Faster!";
			this.labelPercentResult.Visible = false;
// 
// labelParseResult
// 
			this.labelParseResult.Location = new System.Drawing.Point(8, 25);
			this.labelParseResult.Name = "labelParseResult";
			this.labelParseResult.Size = new System.Drawing.Size(96, 25);
			this.labelParseResult.TabIndex = 3;
			this.labelParseResult.Text = "Parse:";
// 
// labelTryParseResult
// 
			this.labelTryParseResult.Location = new System.Drawing.Point(8, 59);
			this.labelTryParseResult.Name = "labelTryParseResult";
			this.labelTryParseResult.Size = new System.Drawing.Size(96, 25);
			this.labelTryParseResult.TabIndex = 2;
			this.labelTryParseResult.Text = "TryParse:";
// 
// labelParseResultValue
// 
			this.labelParseResultValue.Location = new System.Drawing.Point(107, 25);
			this.labelParseResultValue.Name = "labelParseResultValue";
			this.labelParseResultValue.Size = new System.Drawing.Size(152, 24);
			this.labelParseResultValue.TabIndex = 1;
			this.labelParseResultValue.TextAlign = System.Drawing.ContentAlignment.TopRight;
// 
// labelTryParseResultValue
// 
			this.labelTryParseResultValue.Location = new System.Drawing.Point(103, 59);
			this.labelTryParseResultValue.Name = "labelTryParseResultValue";
			this.labelTryParseResultValue.Size = new System.Drawing.Size(156, 24);
			this.labelTryParseResultValue.TabIndex = 0;
			this.labelTryParseResultValue.TextAlign = System.Drawing.ContentAlignment.TopRight;
// 
// FormMain
// 
			this.AcceptButton = this.buttonGo;
			this.CancelButton = this.buttonExit;
			this.ClientSize = new System.Drawing.Size(290, 389);
			this.Controls.Add(this.buttonHelp);
			this.Controls.Add(this.comboType);
			this.Controls.Add(this.labelType);
			this.Controls.Add(this.labelDesc);
			this.Controls.Add(this.numericIterations);
			this.Controls.Add(this.labelIterations);
			this.Controls.Add(this.buttonExit);
			this.Controls.Add(this.buttonGo);
			this.Controls.Add(this.labelPercent);
			this.Controls.Add(this.numericPercent);
			this.Controls.Add(this.groupResults);
			this.Font = new System.Drawing.Font("Microsoft Sans Serif", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "FormMain";
			this.Padding = new System.Windows.Forms.Padding(9);
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "TryParse Demo";
			this.Load += new System.EventHandler(this.FormMain_Load);
			((System.ComponentModel.ISupportInitialize)(this.numericIterations)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numericPercent)).EndInit();
			this.groupResults.ResumeLayout(false);
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

		private System.Windows.Forms.Button buttonHelp;
		private System.Windows.Forms.ComboBox comboType;
		private System.Windows.Forms.Label labelType;
		private System.Windows.Forms.Label labelDesc;
		private System.Windows.Forms.NumericUpDown numericIterations;
		private System.Windows.Forms.Label labelIterations;
		private System.Windows.Forms.Button buttonExit;
		private System.Windows.Forms.Button buttonGo;
		private System.Windows.Forms.Label labelPercent;
		private System.Windows.Forms.NumericUpDown numericPercent;
		private System.Windows.Forms.GroupBox groupResults;
		private System.Windows.Forms.Label labelPercentResultValue;
		private System.Windows.Forms.Label labelPercentResult;
		private System.Windows.Forms.Label labelParseResult;
		private System.Windows.Forms.Label labelTryParseResult;
		private System.Windows.Forms.Label labelParseResultValue;
		private System.Windows.Forms.Label labelTryParseResultValue;
	}
}

