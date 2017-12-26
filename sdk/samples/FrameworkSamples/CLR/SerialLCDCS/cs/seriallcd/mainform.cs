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
using System.Threading;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Diagnostics;

namespace Microsoft.Samples.SerialTest
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class MainForm : System.Windows.Forms.Form
	{
		PerformanceCounter percentProcessorTimePC;
		PerformanceCounter commitedBytesPC;
		private SerialLCD lcd;
		private bool alwaysChangeLine1;
		private bool alwaysChangeLine2;
		private bool fan1Changed;
		private bool line1Changed;
		private bool line2Changed;
		private bool brightnessChanged;
		private bool contrastChanged;
		private System.Windows.Forms.GroupBox groupDisplay;
		private System.Windows.Forms.Label labelLine1;
		private System.Windows.Forms.Label labelLine2;
		private System.Windows.Forms.ComboBox comboLine1;
		private System.Windows.Forms.ComboBox comboLine2;
		private System.Windows.Forms.Timer timerMain;
		private System.Windows.Forms.Button buttonClosePort;
		private System.Windows.Forms.Button buttonOpenPort;
		private System.Windows.Forms.TrackBar trackBarBrightness;
		private System.Windows.Forms.Label labelBrightness;
		private System.Windows.Forms.Label labelContrast;
		private System.Windows.Forms.TrackBar trackBarContrast;
		private System.Windows.Forms.GroupBox groupFan;
		private System.Windows.Forms.Label labelFan1;
		private System.Windows.Forms.TrackBar trackBarFan1;
		private System.Windows.Forms.Label labelFan1Speed;
		private System.Windows.Forms.Button buttonExit;
		private System.Windows.Forms.GroupBox groupGreeting;
		private System.Windows.Forms.Button buttonSend;
		private System.Windows.Forms.Label labelBrightnessValue;
		private System.Windows.Forms.Label labelContrastValue;
		private System.Windows.Forms.Label labelInputChanges;
		private System.Windows.Forms.ComboBox comboInput;
		private System.Windows.Forms.ComboBox comboGreeting;
		private System.ComponentModel.IContainer components;

		public MainForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.groupDisplay = new System.Windows.Forms.GroupBox();
			this.labelContrastValue = new System.Windows.Forms.Label();
			this.labelBrightnessValue = new System.Windows.Forms.Label();
			this.labelContrast = new System.Windows.Forms.Label();
			this.trackBarContrast = new System.Windows.Forms.TrackBar();
			this.labelBrightness = new System.Windows.Forms.Label();
			this.trackBarBrightness = new System.Windows.Forms.TrackBar();
			this.comboLine2 = new System.Windows.Forms.ComboBox();
			this.comboLine1 = new System.Windows.Forms.ComboBox();
			this.labelLine2 = new System.Windows.Forms.Label();
			this.labelLine1 = new System.Windows.Forms.Label();
			this.components = new System.ComponentModel.Container();
			this.timerMain = new System.Windows.Forms.Timer(this.components);
			this.buttonClosePort = new System.Windows.Forms.Button();
			this.buttonOpenPort = new System.Windows.Forms.Button();
			this.groupFan = new System.Windows.Forms.GroupBox();
			this.labelFan1Speed = new System.Windows.Forms.Label();
			this.trackBarFan1 = new System.Windows.Forms.TrackBar();
			this.labelFan1 = new System.Windows.Forms.Label();
			this.buttonExit = new System.Windows.Forms.Button();
			this.groupGreeting = new System.Windows.Forms.GroupBox();
			this.comboGreeting = new System.Windows.Forms.ComboBox();
			this.buttonSend = new System.Windows.Forms.Button();
			this.labelInputChanges = new System.Windows.Forms.Label();
			this.comboInput = new System.Windows.Forms.ComboBox();
			this.groupDisplay.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.trackBarContrast)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.trackBarBrightness)).BeginInit();
			this.groupFan.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.trackBarFan1)).BeginInit();
			this.groupGreeting.SuspendLayout();
			this.SuspendLayout();
// 
// groupDisplay
// 
			this.groupDisplay.Controls.Add(this.labelContrastValue);
			this.groupDisplay.Controls.Add(this.labelBrightnessValue);
			this.groupDisplay.Controls.Add(this.labelContrast);
			this.groupDisplay.Controls.Add(this.trackBarContrast);
			this.groupDisplay.Controls.Add(this.labelBrightness);
			this.groupDisplay.Controls.Add(this.trackBarBrightness);
			this.groupDisplay.Controls.Add(this.comboLine2);
			this.groupDisplay.Controls.Add(this.comboLine1);
			this.groupDisplay.Controls.Add(this.labelLine2);
			this.groupDisplay.Controls.Add(this.labelLine1);
			this.groupDisplay.Location = new System.Drawing.Point(11, 11);
			this.groupDisplay.Name = "groupDisplay";
			this.groupDisplay.Size = new System.Drawing.Size(460, 210);
			this.groupDisplay.TabIndex = 0;
			this.groupDisplay.TabStop = false;
			this.groupDisplay.Text = "Display Choices";
// 
// labelContrastValue
// 
			this.labelContrastValue.Location = new System.Drawing.Point(394, 163);
			this.labelContrastValue.Name = "labelContrastValue";
			this.labelContrastValue.Size = new System.Drawing.Size(53, 33);
			this.labelContrastValue.TabIndex = 10;
			this.labelContrastValue.Text = "15";
			this.labelContrastValue.TextAlign = System.Drawing.ContentAlignment.TopCenter;
// 
// labelBrightnessValue
// 
			this.labelBrightnessValue.Location = new System.Drawing.Point(394, 116);
			this.labelBrightnessValue.Name = "labelBrightnessValue";
			this.labelBrightnessValue.Size = new System.Drawing.Size(53, 33);
			this.labelBrightnessValue.TabIndex = 9;
			this.labelBrightnessValue.Text = "100";
			this.labelBrightnessValue.TextAlign = System.Drawing.ContentAlignment.TopCenter;
// 
// labelContrast
// 
			this.labelContrast.Location = new System.Drawing.Point(9, 163);
			this.labelContrast.Name = "labelContrast";
			this.labelContrast.Size = new System.Drawing.Size(109, 30);
			this.labelContrast.TabIndex = 7;
			this.labelContrast.Text = "Contrast";
// 
// trackBarContrast
// 
			this.trackBarContrast.Location = new System.Drawing.Point(139, 163);
			this.trackBarContrast.Maximum = 50;
			this.trackBarContrast.Name = "trackBarContrast";
			this.trackBarContrast.Size = new System.Drawing.Size(259, 34);
			this.trackBarContrast.TabIndex = 6;
			this.trackBarContrast.TickFrequency = 5;
			this.trackBarContrast.Value = 15;
			this.trackBarContrast.ValueChanged += new System.EventHandler(this.trackBarContrast_ValueChanged);
// 
// labelBrightness
// 
			this.labelBrightness.Location = new System.Drawing.Point(9, 116);
			this.labelBrightness.Name = "labelBrightness";
			this.labelBrightness.Size = new System.Drawing.Size(109, 30);
			this.labelBrightness.TabIndex = 5;
			this.labelBrightness.Text = "Brightness";
// 
// trackBarBrightness
// 
			this.trackBarBrightness.LargeChange = 20;
			this.trackBarBrightness.Location = new System.Drawing.Point(139, 116);
			this.trackBarBrightness.Maximum = 100;
			this.trackBarBrightness.Name = "trackBarBrightness";
			this.trackBarBrightness.Size = new System.Drawing.Size(259, 34);
			this.trackBarBrightness.TabIndex = 4;
			this.trackBarBrightness.TickFrequency = 20;
			this.trackBarBrightness.Value = 100;
			this.trackBarBrightness.ValueChanged += new System.EventHandler(this.trackBarBrightness_ValueChanged);
// 
// comboLine2
// 
			this.comboLine2.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboLine2.FormattingEnabled = true;
			this.comboLine2.Location = new System.Drawing.Point(149, 72);
			this.comboLine2.Name = "comboLine2";
			this.comboLine2.Size = new System.Drawing.Size(298, 32);
			this.comboLine2.TabIndex = 3;
			this.comboLine2.SelectedIndexChanged += new System.EventHandler(this.comboLine2_SelectedIndexChanged);
// 
// comboLine1
// 
			this.comboLine1.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboLine1.FormattingEnabled = true;
			this.comboLine1.Location = new System.Drawing.Point(149, 28);
			this.comboLine1.Name = "comboLine1";
			this.comboLine1.Size = new System.Drawing.Size(298, 32);
			this.comboLine1.TabIndex = 2;
			this.comboLine1.SelectedIndexChanged += new System.EventHandler(this.comboLine1_SelectedIndexChanged);
// 
// labelLine2
// 
			this.labelLine2.Location = new System.Drawing.Point(9, 72);
			this.labelLine2.Name = "labelLine2";
			this.labelLine2.Size = new System.Drawing.Size(78, 22);
			this.labelLine2.TabIndex = 1;
			this.labelLine2.Text = "Line 2";
// 
// labelLine1
// 
			this.labelLine1.Location = new System.Drawing.Point(9, 28);
			this.labelLine1.Name = "labelLine1";
			this.labelLine1.Size = new System.Drawing.Size(78, 22);
			this.labelLine1.TabIndex = 0;
			this.labelLine1.Text = "Line 1";
// 
// timerMain
// 
			this.timerMain.Tick += new System.EventHandler(this.timerMain_Tick);
// 
// buttonClosePort
// 
			this.buttonClosePort.Location = new System.Drawing.Point(499, 68);
			this.buttonClosePort.Name = "buttonClosePort";
			this.buttonClosePort.Size = new System.Drawing.Size(112, 48);
			this.buttonClosePort.TabIndex = 1;
			this.buttonClosePort.Text = "Close Port";
			this.buttonClosePort.Click += new System.EventHandler(this.buttonClosePort_Click);
// 
// buttonOpenPort
// 
			this.buttonOpenPort.Enabled = false;
			this.buttonOpenPort.Location = new System.Drawing.Point(499, 11);
			this.buttonOpenPort.Name = "buttonOpenPort";
			this.buttonOpenPort.Size = new System.Drawing.Size(112, 48);
			this.buttonOpenPort.TabIndex = 2;
			this.buttonOpenPort.Text = "Open Port";
			this.buttonOpenPort.Click += new System.EventHandler(this.buttonOpenPort_Click);
// 
// groupFan
// 
			this.groupFan.Controls.Add(this.labelFan1Speed);
			this.groupFan.Controls.Add(this.trackBarFan1);
			this.groupFan.Controls.Add(this.labelFan1);
			this.groupFan.Location = new System.Drawing.Point(11, 225);
			this.groupFan.Name = "groupFan";
			this.groupFan.Size = new System.Drawing.Size(461, 74);
			this.groupFan.TabIndex = 3;
			this.groupFan.TabStop = false;
			this.groupFan.Text = "Fan Details";
// 
// labelFan1Speed
// 
			this.labelFan1Speed.Location = new System.Drawing.Point(401, 29);
			this.labelFan1Speed.Name = "labelFan1Speed";
			this.labelFan1Speed.Size = new System.Drawing.Size(53, 33);
			this.labelFan1Speed.TabIndex = 8;
			this.labelFan1Speed.Text = "0";
			this.labelFan1Speed.TextAlign = System.Drawing.ContentAlignment.TopCenter;
// 
// trackBarFan1
// 
			this.trackBarFan1.LargeChange = 20;
			this.trackBarFan1.Location = new System.Drawing.Point(129, 25);
			this.trackBarFan1.Maximum = 100;
			this.trackBarFan1.Name = "trackBarFan1";
			this.trackBarFan1.Size = new System.Drawing.Size(260, 34);
			this.trackBarFan1.TabIndex = 7;
			this.trackBarFan1.TickFrequency = 20;
			this.trackBarFan1.ValueChanged += new System.EventHandler(this.trackBarFan1_ValueChanged);
// 
// labelFan1
// 
			this.labelFan1.Location = new System.Drawing.Point(12, 25);
			this.labelFan1.Name = "labelFan1";
			this.labelFan1.Size = new System.Drawing.Size(121, 29);
			this.labelFan1.TabIndex = 2;
			this.labelFan1.Text = "Fan Speed";
// 
// buttonExit
// 
			this.buttonExit.Location = new System.Drawing.Point(499, 324);
			this.buttonExit.Name = "buttonExit";
			this.buttonExit.Size = new System.Drawing.Size(112, 48);
			this.buttonExit.TabIndex = 4;
			this.buttonExit.Text = "Exit";
			this.buttonExit.Click += new System.EventHandler(this.buttonExit_Click);
// 
// groupGreeting
// 
			this.groupGreeting.Controls.Add(this.comboGreeting);
			this.groupGreeting.Controls.Add(this.buttonSend);
			this.groupGreeting.Location = new System.Drawing.Point(11, 303);
			this.groupGreeting.Name = "groupGreeting";
			this.groupGreeting.Size = new System.Drawing.Size(462, 69);
			this.groupGreeting.TabIndex = 5;
			this.groupGreeting.TabStop = false;
			this.groupGreeting.Text = "Send A Greeting";
// 
// comboGreeting
// 
			this.comboGreeting.FormattingEnabled = true;
			this.comboGreeting.Location = new System.Drawing.Point(10, 25);
			this.comboGreeting.Name = "comboGreeting";
			this.comboGreeting.Size = new System.Drawing.Size(333, 32);
			this.comboGreeting.TabIndex = 1;
// 
// buttonSend
// 
			this.buttonSend.Location = new System.Drawing.Point(353, 23);
			this.buttonSend.Name = "buttonSend";
			this.buttonSend.Size = new System.Drawing.Size(94, 38);
			this.buttonSend.TabIndex = 0;
			this.buttonSend.Text = "Send";
			this.buttonSend.Click += new System.EventHandler(this.buttonSend_Click);
// 
// labelInputChanges
// 
			this.labelInputChanges.Location = new System.Drawing.Point(488, 195);
			this.labelInputChanges.Name = "labelInputChanges";
			this.labelInputChanges.Size = new System.Drawing.Size(141, 26);
			this.labelInputChanges.TabIndex = 6;
			this.labelInputChanges.Text = "Input Changes";
// 
// comboInput
// 
			this.comboInput.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboInput.FormattingEnabled = true;
			this.comboInput.Location = new System.Drawing.Point(488, 225);
			this.comboInput.Name = "comboInput";
			this.comboInput.Size = new System.Drawing.Size(138, 32);
			this.comboInput.TabIndex = 7;
// 
// MainForm
// 
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            		this.ClientSize = new System.Drawing.Size(636, 417);
			this.Controls.Add(this.comboInput);
			this.Controls.Add(this.labelInputChanges);
			this.Controls.Add(this.groupGreeting);
			this.Controls.Add(this.buttonExit);
			this.Controls.Add(this.groupFan);
			this.Controls.Add(this.buttonOpenPort);
			this.Controls.Add(this.buttonClosePort);
			this.Controls.Add(this.groupDisplay);
			this.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.Name = "MainForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Serial LCD";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.MainForm_Closing);
			this.Load += new System.EventHandler(this.MainForm_Load);
			this.groupDisplay.ResumeLayout(false);
			this.groupDisplay.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.trackBarContrast)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.trackBarBrightness)).EndInit();
			this.groupFan.ResumeLayout(false);
			this.groupFan.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.trackBarFan1)).EndInit();
			this.groupGreeting.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		[STAThread]
		static void Main()
		{
			Application.EnableVisualStyles();
			Application.Run(new MainForm());
		}
		private void MainForm_Load(object sender, System.EventArgs e)
		{
			Cursor = Cursors.WaitCursor;
			FillCombo(comboLine1, 0);
			FillCombo(comboLine2, 1);
			comboInput.Items.Add("Brightness");
			comboInput.Items.Add("Contrast");
			comboInput.Items.Add("Fan Speed");
			comboInput.SelectedIndex = 0;
			comboGreeting.Items.Add("[Enter, or select a greeting]");
			comboGreeting.Items.Add("WELCOME, ALL !!!");
			comboGreeting.Items.Add("Hey there TechEd 2004 !!");
			comboGreeting.Items.Add("Hello, World");
			comboGreeting.Items.Add("Welcome to this presentation");
			comboGreeting.SelectedIndex = 0;
			percentProcessorTimePC = new PerformanceCounter("Processor", "% Processor Time", "_Total");
			commitedBytesPC = new PerformanceCounter("Memory", "Committed Bytes", "");
			try
			{
				OpenPort();
				Cursor = Cursors.Arrow;
			}
			catch (TimeoutException)
			{
				Cursor = Cursors.Arrow;
				// because this is JUST on open, we will ignore it this time...
				buttonClosePort.Enabled = false;
				buttonOpenPort.Enabled = true;
			}
		}

		private void FillCombo(ComboBox box, int selected)
		{
			box.Items.Add("Date");
			box.Items.Add("Time");
			box.Items.Add("CPU Usage");
			box.Items.Add("Memory Usage");
			box.Items.Add("Display Size");
			box.SelectedIndex = selected;
		}
		private void MainForm_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			if (lcd != null)
			{
				lcd.Close();
			}
		}
		private void timerMain_Tick(object sender, System.EventArgs e)
		{
			if (lcd.IsOpen)
			{
				if (line1Changed || alwaysChangeLine1)
				{
					WriteEntry(comboLine1, true);
					line1Changed = false;

				}
				if (line2Changed || alwaysChangeLine2)
				{
					WriteEntry(comboLine2, false);
					line2Changed = false;
				}

				if (brightnessChanged)
				{
					lcd.SetLCDBacklight(trackBarBrightness.Value);
					brightnessChanged = false;
				}
				if (contrastChanged)
				{
					lcd.SetLCDContrast(trackBarContrast.Value);
					contrastChanged = false;
				}
				if (fan1Changed)
				{
					lcd.SetFan1Power(trackBarFan1.Value);
					fan1Changed = false;
				}
				if (lcd.TimedOut)
				{
					timerMain.Enabled = false;
					CloseConnection();
					lcd.TimedOut = false;
					timerMain.Enabled = true;
				}
			}

			switch (lcd.Key)
			{
				case LCDKey.Up :
					if (comboInput.SelectedIndex == 0)
						comboInput.SelectedIndex = 2;
					else
						comboInput.SelectedIndex = comboInput.SelectedIndex - 1;

					break;
				case LCDKey.Down :
					comboInput.SelectedIndex = (comboInput.SelectedIndex + 1) % 3;
					break;
					
				case LCDKey.Left :
					if (comboInput.SelectedIndex == 0)
					{
						if (trackBarBrightness.Value > 0)
							trackBarBrightness.Value -= 20;
					}
					else if (comboInput.SelectedIndex == 1)
					{
						if (trackBarContrast.Value > 0)
							trackBarContrast.Value -= 5;
					}
					else if (comboInput.SelectedIndex == 2)
					{
						if (trackBarFan1.Value > 0)
							trackBarFan1.Value -= 20;
					}
					break;
				case LCDKey.Right :
					if (comboInput.SelectedIndex == 0)
					{
						if (trackBarBrightness.Value < 100)
							trackBarBrightness.Value += 20;
					}
					else if (comboInput.SelectedIndex == 1)
					{
						if (trackBarContrast.Value < 50)
							trackBarContrast.Value += 5;
					}
					else if (comboInput.SelectedIndex == 2)
					{
						if (trackBarFan1.Value < 100)
							trackBarFan1.Value += 20;
					}
					break;
					
				case LCDKey.Exit :
					this.Close();
					break;
			}

			lcd.Key = LCDKey.None;
		}

		private void CloseConnection()
		{
			lcd.Close();
			buttonOpenPort.Enabled = true;
			buttonClosePort.Enabled = false;
			timerMain.Enabled = false;
		}

		private void WriteEntry(ComboBox box, bool firstLine)
		{
			string text = null;
			switch (box.SelectedIndex)
			{
				case 0 :
					text = DateTime.Now.ToString("d");
					break;
				case 1 :
					text = DateTime.Now.ToString("T");
					break;
				case 2 :
					text = "CPU: " + (int)percentProcessorTimePC.NextValue() + "%";
					break;
				case 3:
					text = "MEM: " + (int)(commitedBytesPC.NextValue() / (1024 * 1000)) + "MB";
					break;
				case 4:
					text = Screen.PrimaryScreen.Bounds.Width.ToString() + " X " + 
							Screen.PrimaryScreen.Bounds.Height.ToString();
					break;
			}

			if (firstLine)
			{
				lcd.WriteLine1(text);
			}
			else
			{
				lcd.WriteLine2(text);
			}
		}
		private void buttonOpenPort_Click(object sender, System.EventArgs e)
		{
			Cursor.Current = Cursors.WaitCursor;
			OpenPort();
			Cursor.Current = Cursors.Arrow;
		}

		void OpenPort()
		{
			if (lcd == null || lcd.IsOpen == false) lcd = new SerialLCD();
			buttonOpenPort.Enabled = false;
			buttonClosePort.Enabled = true;
			lcd.SetFan1Power(trackBarFan1.Value);
			lcd.SetLCDBacklight(trackBarBrightness.Value);
			lcd.SetLCDContrast(trackBarContrast.Value);
			// force a refresh of the two lines...
			line1Changed = true;
			line2Changed = true;
			timerMain.Enabled = true;
		}

		private void buttonClosePort_Click(object sender, System.EventArgs e)
		{
			Cursor.Current = Cursors.WaitCursor;
			CloseConnection();
			Cursor.Current = Cursors.Arrow;
		}


		private void buttonExit_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}
		private void comboLine1_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			line1Changed = true;
			alwaysChangeLine1 = comboLine1.SelectedIndex != 0;
		}
		private void comboLine2_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			line2Changed = true;
			alwaysChangeLine2 = comboLine2.SelectedIndex != 0;
		}
		private void trackBarFan1_ValueChanged(object sender, System.EventArgs e)
		{
			fan1Changed = true;
			labelFan1Speed.Text = trackBarFan1.Value.ToString();
		}
		private void buttonSend_Click(object sender, System.EventArgs e)
		{
			if (lcd != null && lcd.IsOpen)
			{
				Cursor = Cursors.WaitCursor;
				string temp = comboGreeting.Text.Trim();
				if (temp.Length > 0)
				{
					lcd.WriteLine2("");
					timerMain.Enabled = false;

					int position = 15;

					while (true)
					{
						string s = null;
						if (position > 0)
							s = String.Format("{0," + position + "}", " ");
						s += temp.Substring(0, 16 - position < temp.Length ? 16 - position : temp.Length);
						lcd.WriteLine1(s);
						Thread.Sleep(200);
						if (position > 0)
						{
							position--;
						}
						else
						{
							if (temp.Length > 1)
								temp = temp.Substring(1);
							else
							{
								lcd.WriteLine1("");
								break;
							}
						}
					}
					line1Changed = true;
					line2Changed = true;
					timerMain.Enabled = true;
				}
				Cursor = Cursors.Arrow;
			}
		}
		private void trackBarBrightness_ValueChanged(object sender, System.EventArgs e)
		{
			brightnessChanged = true;
			labelBrightnessValue.Text = trackBarBrightness.Value.ToString();
		}
		private void trackBarContrast_ValueChanged(object sender, System.EventArgs e)
		{
			contrastChanged = true;
			labelContrastValue.Text = trackBarContrast.Value.ToString();
		}
	}
}
