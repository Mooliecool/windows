

namespace Microsoft.Samples.StopWatchCS
{
	public partial class FormMain : System.Windows.Forms.Form
	{

		private void InitializeComponent()
		{
			this.labelTopLeft = new System.Windows.Forms.Label();
			this.LabelTimePrompt = new System.Windows.Forms.Label();
			this.labelLapPrompt = new System.Windows.Forms.Label();
			this.labelLap = new System.Windows.Forms.Label();
			this.labelTime = new System.Windows.Forms.Label();
			this.labelTopRight = new System.Windows.Forms.Label();
			this.labelBottomRight = new System.Windows.Forms.Label();
			this.labelBottomLeft = new System.Windows.Forms.Label();
			this.buttonLap = new System.Windows.Forms.Button();
			this.buttonStart = new System.Windows.Forms.Button();
			this.labelExit = new System.Windows.Forms.Label();
			this.components = new System.ComponentModel.Container();
			this.timerMain = new System.Windows.Forms.Timer(this.components);
			this.SuspendLayout();
// 
// labelTopLeft
// 
			this.labelTopLeft.BackColor = System.Drawing.Color.Black;
			this.labelTopLeft.ForeColor = System.Drawing.Color.Black;
			this.labelTopLeft.Location = new System.Drawing.Point(8, 8);
			this.labelTopLeft.Margin = new System.Windows.Forms.Padding(3, 3, 3, 0);
			this.labelTopLeft.Name = "labelTopLeft";
			this.labelTopLeft.Size = new System.Drawing.Size(16, 17);
			this.labelTopLeft.TabIndex = 11;
			this.labelTopLeft.Click += new System.EventHandler(this.labelTopLeft_Click);
// 
// LabelTimePrompt
// 
			this.LabelTimePrompt.Font = new System.Drawing.Font("Comic Sans MS", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.LabelTimePrompt.ForeColor = System.Drawing.Color.Yellow;
			this.LabelTimePrompt.Location = new System.Drawing.Point(18, 26);
			this.LabelTimePrompt.Margin = new System.Windows.Forms.Padding(3, 0, 3, 3);
			this.LabelTimePrompt.Name = "LabelTimePrompt";
			this.LabelTimePrompt.Size = new System.Drawing.Size(88, 26);
			this.LabelTimePrompt.TabIndex = 15;
			this.LabelTimePrompt.Text = "Time";
			this.LabelTimePrompt.Click += new System.EventHandler(this.LabelTimePrompt_Click);
// 
// labelLapPrompt
// 
			this.labelLapPrompt.Font = new System.Drawing.Font("Comic Sans MS", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelLapPrompt.ForeColor = System.Drawing.Color.Yellow;
			this.labelLapPrompt.Location = new System.Drawing.Point(26, 87);
			this.labelLapPrompt.Margin = new System.Windows.Forms.Padding(3, 1, 3, 3);
			this.labelLapPrompt.Name = "labelLapPrompt";
			this.labelLapPrompt.Size = new System.Drawing.Size(56, 25);
			this.labelLapPrompt.TabIndex = 14;
			this.labelLapPrompt.Text = "Lap";
			this.labelLapPrompt.Visible = false;
			this.labelLapPrompt.Click += new System.EventHandler(this.labelLapPrompt_Click);
// 
// labelLap
// 
			this.labelLap.Font = new System.Drawing.Font("Microsoft Sans Serif", 20.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelLap.ForeColor = System.Drawing.Color.Yellow;
			this.labelLap.Location = new System.Drawing.Point(10, 112);
			this.labelLap.Name = "labelLap";
			this.labelLap.Size = new System.Drawing.Size(191, 34);
			this.labelLap.TabIndex = 13;
			this.labelLap.Visible = false;
			this.labelLap.Click += new System.EventHandler(this.labelLap_Click);
// 
// labelTime
// 
			this.labelTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelTime.ForeColor = System.Drawing.Color.Yellow;
			this.labelTime.Location = new System.Drawing.Point(10, 52);
			this.labelTime.Margin = new System.Windows.Forms.Padding(3, 3, 3, 1);
			this.labelTime.Name = "labelTime";
			this.labelTime.Size = new System.Drawing.Size(188, 33);
			this.labelTime.TabIndex = 12;
			this.labelTime.Text = "00:00:00.00";
			this.labelTime.Click += new System.EventHandler(this.labelTime_Click);
// 
// labelTopRight
// 
			this.labelTopRight.BackColor = System.Drawing.Color.Black;
			this.labelTopRight.ForeColor = System.Drawing.Color.Black;
			this.labelTopRight.Location = new System.Drawing.Point(176, 8);
			this.labelTopRight.Name = "labelTopRight";
			this.labelTopRight.Size = new System.Drawing.Size(16, 17);
			this.labelTopRight.TabIndex = 21;
			this.labelTopRight.Click += new System.EventHandler(this.labelTopRight_Click);
// 
// labelBottomRight
// 
			this.labelBottomRight.BackColor = System.Drawing.Color.Black;
			this.labelBottomRight.ForeColor = System.Drawing.Color.Black;
			this.labelBottomRight.Location = new System.Drawing.Point(176, 177);
			this.labelBottomRight.Margin = new System.Windows.Forms.Padding(1, 3, 3, 3);
			this.labelBottomRight.Name = "labelBottomRight";
			this.labelBottomRight.Size = new System.Drawing.Size(16, 17);
			this.labelBottomRight.TabIndex = 20;
			this.labelBottomRight.Click += new System.EventHandler(this.labelBottomRight_Click);
// 
// labelBottomLeft
// 
			this.labelBottomLeft.BackColor = System.Drawing.Color.Black;
			this.labelBottomLeft.ForeColor = System.Drawing.Color.Black;
			this.labelBottomLeft.Location = new System.Drawing.Point(8, 177);
			this.labelBottomLeft.Margin = new System.Windows.Forms.Padding(3, 3, 1, 3);
			this.labelBottomLeft.Name = "labelBottomLeft";
			this.labelBottomLeft.Size = new System.Drawing.Size(16, 17);
			this.labelBottomLeft.TabIndex = 19;
			this.labelBottomLeft.Click += new System.EventHandler(this.labelBottomLeft_Click);
// 
// buttonLap
// 
			this.buttonLap.Font = new System.Drawing.Font("Comic Sans MS", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.buttonLap.ForeColor = System.Drawing.Color.Yellow;
			this.buttonLap.Location = new System.Drawing.Point(102, 153);
			this.buttonLap.Margin = new System.Windows.Forms.Padding(2, 3, 1, 3);
			this.buttonLap.Name = "buttonLap";
			this.buttonLap.Size = new System.Drawing.Size(72, 34);
			this.buttonLap.TabIndex = 18;
			this.buttonLap.Text = "Lap";
			this.buttonLap.Click += new System.EventHandler(this.buttonLap_Click);
// 
// buttonStart
// 
			this.buttonStart.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.buttonStart.Font = new System.Drawing.Font("Comic Sans MS", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.buttonStart.ForeColor = System.Drawing.Color.Yellow;
			this.buttonStart.Location = new System.Drawing.Point(26, 153);
			this.buttonStart.Margin = new System.Windows.Forms.Padding(1, 3, 2, 3);
			this.buttonStart.Name = "buttonStart";
			this.buttonStart.Size = new System.Drawing.Size(72, 34);
			this.buttonStart.TabIndex = 17;
			this.buttonStart.Text = "Start";
			this.buttonStart.Click += new System.EventHandler(this.buttonStart_Click);
// 
// labelExit
// 
			this.labelExit.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.labelExit.Location = new System.Drawing.Point(91, 3);
			this.labelExit.Margin = new System.Windows.Forms.Padding(3, 3, 3, 1);
			this.labelExit.Name = "labelExit";
			this.labelExit.Size = new System.Drawing.Size(20, 21);
			this.labelExit.TabIndex = 16;
			this.labelExit.Text = "x";
			this.labelExit.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.labelExit.Click += new System.EventHandler(this.labelExit_Click);
// 
// timerMain
// 
			this.timerMain.Enabled = true;
			this.timerMain.Interval = 50;
			this.timerMain.Tick += new System.EventHandler(this.timerMain_Tick);
// 
// FormMain
// 
			this.AutoScaleDimensions = new System.Drawing.Size(9, 23);
			this.BackColor = System.Drawing.Color.Navy;
			this.ClientSize = new System.Drawing.Size(200, 200);
			this.Controls.Add(this.labelTopRight);
			this.Controls.Add(this.labelBottomRight);
			this.Controls.Add(this.labelBottomLeft);
			this.Controls.Add(this.buttonLap);
			this.Controls.Add(this.buttonStart);
			this.Controls.Add(this.labelExit);
			this.Controls.Add(this.LabelTimePrompt);
			this.Controls.Add(this.labelLapPrompt);
			this.Controls.Add(this.labelLap);
			this.Controls.Add(this.labelTime);
			this.Controls.Add(this.labelTopLeft);
			this.Font = new System.Drawing.Font("Microsoft Sans Serif", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Name = "FormMain";
			this.Padding = new System.Windows.Forms.Padding(9);
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "StopWatch Demo";
			this.TopMost = true;
			this.Click += new System.EventHandler(this.FormMain_Click);
			this.Load += new System.EventHandler(this.FormMain_Load);
			this.ResumeLayout(false);

		}

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

		private System.Windows.Forms.Label labelTopLeft;
		private System.Windows.Forms.Label LabelTimePrompt;
		private System.Windows.Forms.Label labelLapPrompt;
		private System.Windows.Forms.Label labelLap;
		private System.Windows.Forms.Label labelTime;
		private System.Windows.Forms.Label labelTopRight;
		private System.Windows.Forms.Label labelBottomRight;
		private System.Windows.Forms.Label labelBottomLeft;
		private System.Windows.Forms.Button buttonLap;
		private System.Windows.Forms.Button buttonStart;
		private System.Windows.Forms.Label labelExit;
		private System.Windows.Forms.Timer timerMain;
	}
}

