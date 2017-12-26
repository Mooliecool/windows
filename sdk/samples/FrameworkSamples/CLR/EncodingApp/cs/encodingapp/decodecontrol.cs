//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
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
//---------------------------------------------------------------------

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Text;
using Microsoft.VisualBasic;
using System.Globalization;


namespace Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication
{
	/// <summary>
	/// UI for decoding string
	/// </summary>
	public class DecodeControl : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.GroupBox dataGroupBox;
		private System.Windows.Forms.Label bytesLabel;
		private System.Windows.Forms.RadioButton hexadecimalRadioButton;
		private System.Windows.Forms.RadioButton decimalRadioButton;
		private System.Windows.Forms.TextBox bytesTextBox;
		private System.Windows.Forms.TextBox stringTextBox;
		private System.Windows.Forms.Label stringLabel;
		private string byteStrFormat;
		
		// Property to get/set the text in the textbox
		public override string Text
		{
			get
			{
				return stringTextBox.Text;
			}
			set
			{
				stringTextBox.Text = value;
			}
		}

		public string Bytes
		{
			get
			{
				return bytesTextBox.Text;
			}
			set
			{
				bytesTextBox.Text = value;
			}
		}

		public string ByteFormat
		{
			get
			{
				return byteStrFormat;
			}
		}

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		
			/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (components != null)
					components.Dispose();
			}

			base.Dispose(disposing);
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.dataGroupBox = new System.Windows.Forms.GroupBox();
			this.stringTextBox = new System.Windows.Forms.TextBox();
			this.stringLabel = new System.Windows.Forms.Label();
			this.bytesLabel = new System.Windows.Forms.Label();
			this.hexadecimalRadioButton = new System.Windows.Forms.RadioButton();
			this.decimalRadioButton = new System.Windows.Forms.RadioButton();
			this.bytesTextBox = new System.Windows.Forms.TextBox();
			this.dataGroupBox.SuspendLayout();
			this.SuspendLayout();
// 
// dataGroupBox
// 
			this.dataGroupBox.Controls.Add(this.stringTextBox);
			this.dataGroupBox.Controls.Add(this.stringLabel);
			this.dataGroupBox.Controls.Add(this.bytesLabel);
			this.dataGroupBox.Controls.Add(this.hexadecimalRadioButton);
			this.dataGroupBox.Controls.Add(this.decimalRadioButton);
			this.dataGroupBox.Controls.Add(this.bytesTextBox);
			this.dataGroupBox.Location = new System.Drawing.Point(15, 8);
			this.dataGroupBox.Name = "dataGroupBox";
			this.dataGroupBox.Size = new System.Drawing.Size(272, 284);
			this.dataGroupBox.TabIndex = 0;
			this.dataGroupBox.TabStop = false;
			this.dataGroupBox.Text = "Decode";
// 
// stringTextBox
// 
			this.stringTextBox.Location = new System.Drawing.Point(23, 220);
			this.stringTextBox.MaxLength = 256;
			this.stringTextBox.Multiline = true;
			this.stringTextBox.Name = "stringTextBox";
			this.stringTextBox.Size = new System.Drawing.Size(227, 41);
			this.stringTextBox.TabIndex = 6;
			this.stringTextBox.ReadOnly = true;
			this.stringTextBox.BackColor = Color.White;
// 
// stringLabel
// 
			this.stringLabel.Location = new System.Drawing.Point(23, 200);
			this.stringLabel.Name = "stringLabel";
			this.stringLabel.Size = new System.Drawing.Size(213, 20);
			this.stringLabel.TabIndex = 7;
			this.stringLabel.Text = "String";
// 
// bytesLabel
// 
			this.bytesLabel.Location = new System.Drawing.Point(23, 25);
			this.bytesLabel.Name = "bytesLabel";
			this.bytesLabel.Size = new System.Drawing.Size(213, 20);
			this.bytesLabel.TabIndex = 0;
			this.bytesLabel.Text = "Bytes";
// 
// hexadecimalRadioButton
// 
			this.hexadecimalRadioButton.Location = new System.Drawing.Point(160, 160);
			this.hexadecimalRadioButton.Name = "hexadecimalRadioButton";
			this.hexadecimalRadioButton.Size = new System.Drawing.Size(90, 20);
			this.hexadecimalRadioButton.TabIndex = 4;
			this.hexadecimalRadioButton.Text = "Hexadecimal";
			this.hexadecimalRadioButton.CheckedChanged +=new EventHandler(radioButton_CheckedChanged);
// 
// decimalRadioButton
// 
			this.decimalRadioButton.Location = new System.Drawing.Point(23, 160);
			this.decimalRadioButton.Name = "decimalRadioButton";
			this.decimalRadioButton.Size = new System.Drawing.Size(63, 19);
			this.decimalRadioButton.TabIndex = 3;
			this.decimalRadioButton.Text = "Decimal";
			this.decimalRadioButton.CheckedChanged +=new EventHandler(radioButton_CheckedChanged);
// 
// bytesTextBox
// 
			this.bytesTextBox.BackColor = System.Drawing.Color.White;
			this.bytesTextBox.Location = new System.Drawing.Point(23, 46);
			this.bytesTextBox.Multiline = true;
			this.bytesTextBox.Name = "bytesTextBox";
			this.bytesTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.bytesTextBox.Size = new System.Drawing.Size(227, 99);
			this.bytesTextBox.TabIndex = 2;
// 
// EncodingUserControl
// 
			this.Controls.Add(this.dataGroupBox);
			this.Name = "EncodingUserControl";
			this.Size = new System.Drawing.Size(300, 344);
			this.dataGroupBox.ResumeLayout(false);
			this.dataGroupBox.PerformLayout();
			this.ResumeLayout(false);
		}
		#endregion		
		
		public DecodeControl(bool isOutputControl)
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
			hexadecimalRadioButton.Checked = true;

			if (isOutputControl)
			{
				dataGroupBox.Text = "Decoded Output";
				bytesLabel.Text = "Unicode Chars";
			}
			else
			{
				dataGroupBox.Text = "Input";
				bytesLabel.Text = "Bytes";
				stringTextBox.Visible = false;
				stringLabel.Visible = false;
			}
		}

		private void radioButton_CheckedChanged(object sender, System.EventArgs e)
		{
			if (decimalRadioButton.Checked == true)
			{
				byteStrFormat = "00000";
			}
			else
			{
				byteStrFormat = "X4";
			}
			
			if(stringTextBox.Visible)
			{
				string outputUnicodeChars = string.Empty;
				foreach (char c in stringTextBox.Text.ToCharArray())
				{
					//Convert each char to short and print 
					outputUnicodeChars += ((short) c).ToString(byteStrFormat,CultureInfo.CurrentUICulture);
					outputUnicodeChars += " ";
				}
				bytesTextBox.Text = outputUnicodeChars;
			}

		}
	}
}
