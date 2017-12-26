//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//-----------------------------------------------------------------------
using System;
using System.Globalization;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Runtime.InteropServices;

namespace Microsoft.Samples.StopWatchCS
{
	public partial class FormMain : System.Windows.Forms.Form
	{
        Stopwatch sw  = new Stopwatch();
        bool paused = false;

		private System.ComponentModel.IContainer components = null;

		public FormMain()
		{
			InitializeComponent();
		}

		private void labelBottomRight_Click(object sender, System.EventArgs e)
		{
			this.Location = new Point(Screen.PrimaryScreen.WorkingArea.Width - this.Width, Screen.PrimaryScreen.WorkingArea.Height - this.Height);
		}

		private void labelLap_Click(object sender, System.EventArgs e)
		{
			this.Location = new Point((Screen.PrimaryScreen.WorkingArea.Width - this.Width) / 2, (Screen.PrimaryScreen.WorkingArea.Height - this.Height) / 2);
		}

		private void labelLapPrompt_Click(object sender, System.EventArgs e)
		{
			this.Location = new Point((Screen.PrimaryScreen.WorkingArea.Width - this.Width) / 2, (Screen.PrimaryScreen.WorkingArea.Height - this.Height) / 2);
		}

		private void labelTime_Click(object sender, System.EventArgs e)
		{
			this.Location = new Point((Screen.PrimaryScreen.WorkingArea.Width - this.Width) / 2, (Screen.PrimaryScreen.WorkingArea.Height - this.Height) / 2);
		}

		private void LabelTimePrompt_Click(object sender, System.EventArgs e)
		{
			this.Location = new Point((Screen.PrimaryScreen.WorkingArea.Width - this.Width) / 2, (Screen.PrimaryScreen.WorkingArea.Height - this.Height) / 2);
		}

		private void FormMain_Load(object sender, System.EventArgs e)
		{
			DrawForm();
		}

		private void DrawForm()
		{
			//Shape the viewer form to look like a windshield
			GraphicsPath gp = new GraphicsPath();

			gp.AddArc(0, 0, 100, 100, 180, 90);
            gp.AddArc(100, 0, 100, 100, 270, 90);
            gp.AddArc(100, 100, 100, 100, 0, 90);
            gp.AddArc(0, 100, 100, 100, 90, 90);

            this.Region = new Region(gp);
		}

		private void FormMain_Click(object sender, System.EventArgs e)
		{
			this.Location = new Point((Screen.PrimaryScreen.WorkingArea.Width - this.Width) / 2, (Screen.PrimaryScreen.WorkingArea.Height - this.Height) / 2);
		}

		private void labelExit_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}

		private void buttonStart_Click(object sender, System.EventArgs e)
		{
			if (sw.IsRunning) {
                sw.Stop();
                buttonStart.Text = "Start";
                buttonLap.Text = "Reset";
			}
            else {
                sw.Start();
                buttonStart.Text = "Stop";
                buttonLap.Text = "Lap";
				labelLap.Visible = false;
				labelLapPrompt.Visible = false;
			}
		}

		private void buttonLap_Click(object sender, System.EventArgs e)
		{
			if (buttonLap.Text == "Lap")
			{
				if (sw.IsRunning)
				{
					paused = true;
					labelLap.Visible = true;
					labelLapPrompt.Visible = true;
				}
			}
			else
			{
				labelLap.Visible = false;
				labelLapPrompt.Visible = false;
				labelTime.Text = "00:00:00.00";
				buttonLap.Text = "Lap";
				sw.Reset();
			}
		}

		private void timerMain_Tick(object sender, System.EventArgs e)
		{
			if (sw.IsRunning)
			{
				TimeSpan ts = TimeSpan.FromMilliseconds(sw.ElapsedMilliseconds);
				labelTime.Text = String.Format(CultureInfo.CurrentCulture, "{0:00}:{1:00}:{2:00}.{3:00}", ts.Hours, ts.Minutes, ts.Seconds, ts.Milliseconds / 10);

				if (paused)
				{
					labelLap.Text = labelTime.Text;
					labelLapPrompt.Text = "Lap";
					paused = false;
				}
			}
		}

		private void labelTopLeft_Click(object sender, System.EventArgs e)
		{
			this.Location = new Point(0, 0);
		}

		private void labelTopRight_Click(object sender, System.EventArgs e)
		{
			this.Location = new Point(Screen.PrimaryScreen.WorkingArea.Width - this.Width, 0);
		}

		private void labelBottomLeft_Click(object sender, System.EventArgs e)
		{
			this.Location = new Point(0, Screen.PrimaryScreen.WorkingArea.Height - this.Height);
		}
	}
}

