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
using System.Runtime.InteropServices;
using System.Globalization;
using System.Diagnostics;
using System.Drawing;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace Microsoft.Samples
{
	public partial class FormMain : System.Windows.Forms.Form
	{
		private StringCollection str;
		private string[] types = new string[4];
		private Stopwatch sw  = new Stopwatch();
		private long parseTime;
		private long tryParseTime;

		private System.ComponentModel.IContainer components = null;

		public FormMain()
		{
			InitializeComponent();
		}

		private void buttonGo_Click(object sender, System.EventArgs e)
		{

			Cursor = Cursors.WaitCursor;
			FillList();

			labelTryParseResultValue.Text = "";
			labelParseResultValue.Text = "";

			PerformTryParse();
			PerformParse();

			if (tryParseTime >= 0 && parseTime >= 0)
			{
				if (parseTime / tryParseTime < 5)
				{
					labelParseResultValue.ForeColor = Color.DarkGreen;
				}
				else if (parseTime / tryParseTime < 15)
				{
					labelParseResultValue.ForeColor = Color.Purple;
				}
				else if (parseTime / tryParseTime < 35)
				{
					labelParseResultValue.ForeColor = Color.DarkRed;
				}
				else
				{
					labelParseResultValue.ForeColor = Color.Red;
				}
			}
			else
			{
				labelTryParseResultValue.ForeColor = Color.Black;
			}


			if (parseTime > 0)
			{
				Double result =(Convert.ToDouble(parseTime) / tryParseTime) - 1;

				if (result < 0.1 && result > -0.1)
				{ //an arbitrary pick, but this means they're roughly the same
					labelPercentResult.Visible = false;
					labelPercentResultValue.Visible = false;
				}
				else
				{
					if (result > 0)
					{
						labelPercentResult.Text = "Faster!";
						labelPercentResult.ForeColor = Color.Black;
					}
					else
					{

						labelPercentResult.ForeColor = Color.Red;
						labelPercentResult.Text = "Slower";
						result = Math.Abs(result);
					}
					labelPercentResult.Visible = true;
					labelPercentResultValue.Visible = true;
				}

				labelPercentResultValue.Text = String.Format(CultureInfo.CurrentCulture, "{0:N2} X", result);
			}
			else
			{
				labelPercentResult.Visible = false;
				labelPercentResultValue.Visible = false;
			}

			Cursor = Cursors.Arrow;
		}


		private void PerformParse()
		{
			switch (comboType.SelectedIndex)
			{
				case 0:
					Int32ParseRoutine();
					break;
				case 1:
					DecimalParseRoutine();
					break;
				case 2:
					DoubleParseRoutine();
					break;
				case 3:
					DateTimeParseRoutine();
					break;
			}

			//parseTime = sw.ElapsedMilliseconds
			parseTime = sw.ElapsedTicks;
			labelParseResultValue.Text = parseTime + " ticks";
		}

		private void PerformTryParse()
		{
			switch (comboType.SelectedIndex)
			{
				case 0:
					Int32TryParseRoutine();
					break;
				case 1:
					DecimalTryParseRoutine();
					break;
				case 2:
					DoubleTryParseRoutine();
					break;
				case 3:
					DateTimeTryParseRoutine();
					break;
			}

			//tryParseTime = sw.ElapsedMilliseconds
			tryParseTime = sw.ElapsedTicks;
			labelTryParseResultValue.Text = tryParseTime + " ticks";
		}

		private void Int32ParseRoutine()
		{
			int i;
			sw = Stopwatch.StartNew();
			for (int j = 1; j <= numericIterations.Value; j++)
			{
				foreach (string s in str)
				{
					try
					{
						i = Int32.Parse(s, NumberStyles.Any, CultureInfo.InvariantCulture);
						// print successful
					}
					catch (FormatException)
					{
						// print unsuccessful
					}
				}
			}
			sw.Stop();
		}

		private void Int32TryParseRoutine()
		{
			int i;
			sw = Stopwatch.StartNew();
			for (int j = 1; j <= numericIterations.Value; j++)
			{
				foreach (string s in str)
				{
					if (Int32.TryParse(s, NumberStyles.Any, CultureInfo.InvariantCulture, out i))
					{
						// print successful
					}
					else
					{
						// print unsuccessful
					}
				}
			}
			sw.Stop();
		}

		private void DecimalParseRoutine()
		{
			Decimal d;
			sw = Stopwatch.StartNew();
			for (int j = 1; j <= numericIterations.Value; j++)
			{
				foreach (string s in str)
				{
					try
					{
						d = Decimal.Parse(s, NumberStyles.Any, CultureInfo.InvariantCulture);
						// print successful
					}
					catch (FormatException)
					{
						// print unsuccessful
					}
				}
			}
			sw.Stop();
		}

		private void DecimalTryParseRoutine()
		{
			Decimal d;
			sw = Stopwatch.StartNew();
			for (int j = 1; j <= numericIterations.Value; j++)
			{
				foreach (string s in str)
				{
					if (Decimal.TryParse(s, NumberStyles.Any, CultureInfo.InvariantCulture, out d))
					{
						// print successful
					}
					else
					{
						// print unsuccessful
					}
				}
			}
			sw.Stop();
		}

		private void DoubleParseRoutine()
		{
			Double d;
			sw = Stopwatch.StartNew();
			for (int j = 1; j <= numericIterations.Value; j++)
			{
				foreach (string s in str)
				{
					try
					{
						d = Double.Parse(s, NumberStyles.Any, CultureInfo.InvariantCulture);
						// print successful
					}
					catch (FormatException)
					{
						// print unsuccessful
					}
				}
			}
			sw.Stop();
		}

		private void DoubleTryParseRoutine()
		{
			Double d;
			sw = Stopwatch.StartNew();
			for (int j = 1; j <= numericIterations.Value; j++)
			{
				foreach (string s in str)
				{
					if (Double.TryParse(s, NumberStyles.Any, CultureInfo.InvariantCulture, out d))
					{
						// print successful
					}
					else
					{
						// print unsuccessful
					}
				}
			}
			sw.Stop();
		}

		private void DateTimeParseRoutine()
		{
			DateTime d;
			sw = Stopwatch.StartNew();
			for (int j = 1; j <= numericIterations.Value; j++)
			{
				foreach (string s in str)
				{
					try
					{
						d = DateTime.Parse(s, CultureInfo.InvariantCulture, DateTimeStyles.None);
						// print successful
					}
					catch (FormatException)
					{
						// print unsuccessful
					}
				}
			}
			sw.Stop();
		}

		private void DateTimeTryParseRoutine()
		{
			DateTime d;
			sw = Stopwatch.StartNew();
			for (int j = 1; j <= numericIterations.Value; j++)
			{
				foreach (string s in str)
				{
					if (DateTime.TryParse(s, CultureInfo.InvariantCulture, DateTimeStyles.None, out d))
					{
						// print successful
					}
					else
					{
						// print unsuccessful
					}
				}
			}
			sw.Stop();
		}

		private void FillList()
		{
			if (comboType.SelectedIndex != 3 ) {
				NumberFill();
			} else {
				DateTimeFill();
			
			}

			Random r = new Random(Environment.TickCount);

			for (int i = 1; i <= (100 - numericPercent.Value); i++)
			{
				int length = r.Next(4, 9);
				string s = "";
				for (int j = 1; j <= length; j++)
				{
					s = s + Convert.ToChar(r.Next(97, 123));
				}

				str.Add(s);
			}
		}

		private void NumberFill()
		{
			Random r = new Random(Environment.TickCount);
			str = new StringCollection();

			for (int i = 1; i <= numericPercent.Value; i++)
			{
				str.Add((r.Next(1, 1000)).ToString(CultureInfo.InvariantCulture));
			}
		}

		private void DateTimeFill()
		{
			Random r = new Random();
			str = new StringCollection();

			for (int i = 1; i <= numericPercent.Value; i++)
			{
				str.Add(DateTime.Now.ToString(CultureInfo.InvariantCulture));
			}
		}

		private void FormMain_Load(object sender, System.EventArgs e)
		{
			labelDesc.Text = "This demo makes 100 strings, of which" + Environment.NewLine +
				"the 'percentage' value determines how many" + Environment.NewLine +
				"can be validly parsed to the specified type. " + Environment.NewLine +
				"The 'iterations' value determines how many " + Environment.NewLine +
				"times the strings are parsed";
			types[0] = "Int32";
			types[1] = "Decimal";
			types[2] = "Double";
			types[3] = "DateTime";

			foreach(string s in types) {
				comboType.Items.Add(s);
			}

			comboType.SelectedIndex = 0;
		}

		private void buttonExit_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}

		private void buttonHelp_Click(object sender, System.EventArgs e)
		{
			FormHelp fh = new FormHelp();
			fh.Show();
		}
	}
}

