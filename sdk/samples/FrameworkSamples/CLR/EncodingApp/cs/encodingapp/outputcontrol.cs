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
using System.Globalization;

namespace Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication
{
	/// <summary>
	/// User Control created to display a string and its corresponding representation in bytes
	/// </summary>
	public class OutputControl: System.Windows.Forms.UserControl
	{
		//Format in which the bytes are represented - hexa or decimal
		private String byteStrFormat;

		private System.Windows.Forms.GroupBox dataGroupBox;

		private System.Windows.Forms.Label stringLabel;

		private System.Windows.Forms.Label bytesLabel;

		private System.Windows.Forms.RadioButton hexadecimalRadioButton;

		private System.Windows.Forms.TextBox stringTextBox;

		private System.Windows.Forms.RadioButton decimalRadioButton;

		private System.Windows.Forms.TextBox bytesTextBox;

		private ArrayList encodedByte;

		public ArrayList EncodedBytes
		{
			get
			{
				return encodedByte;
			}
			set
			{
				encodedByte = value;
			}
		}

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public OutputControl()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
			encodedByte = new ArrayList ();
			decimalRadioButton.Checked = true;
		}

		// Property to get/set the text in the textbox
		public override string Text
		{
			set
			{
				stringTextBox.Text = value;
			}
		}

		// Property to get/set title of the control
		public string Title
		{
			set
			{
				dataGroupBox.Text = value;
			}
		}

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
			this.bytesLabel = new System.Windows.Forms.Label();
			this.hexadecimalRadioButton = new System.Windows.Forms.RadioButton();
			this.decimalRadioButton = new System.Windows.Forms.RadioButton();
			this.bytesTextBox = new System.Windows.Forms.TextBox();
			this.stringTextBox = new System.Windows.Forms.TextBox();
			this.stringLabel = new System.Windows.Forms.Label();
			this.dataGroupBox.SuspendLayout();
			this.SuspendLayout();
// 
// dataGroupBox
// 
			this.dataGroupBox.Controls.Add(this.bytesLabel);
			this.dataGroupBox.Controls.Add(this.hexadecimalRadioButton);
			this.dataGroupBox.Controls.Add(this.decimalRadioButton);
			this.dataGroupBox.Controls.Add(this.bytesTextBox);
			this.dataGroupBox.Controls.Add(this.stringTextBox);
			this.dataGroupBox.Controls.Add(this.stringLabel);
			this.dataGroupBox.Location = new System.Drawing.Point(6, 2);
			this.dataGroupBox.Name = "dataGroupBox";
			this.dataGroupBox.Size = new System.Drawing.Size(272, 286);
			this.dataGroupBox.TabStop = false;
// 
// bytesLabel
// 
			this.bytesLabel.Location = new System.Drawing.Point(20, 106);
			this.bytesLabel.Name = "bytesLabel";
			this.bytesLabel.Size = new System.Drawing.Size(213, 20);
			this.bytesLabel.Text = "Bytes";
// 
// hexadecimalRadioButton
// 
			this.hexadecimalRadioButton.Location = new System.Drawing.Point(157, 241);
			this.hexadecimalRadioButton.Name = "hexadecimalRadioButton";
			this.hexadecimalRadioButton.Size = new System.Drawing.Size(90, 20);
			this.hexadecimalRadioButton.TabIndex = 4;
			this.hexadecimalRadioButton.Text = "Hexadecimal";
			this.hexadecimalRadioButton.CheckedChanged += new System.EventHandler(this.radioButton_CheckedChanged);
// 
// decimalRadioButton
// 
			this.decimalRadioButton.Location = new System.Drawing.Point(20, 241);
			this.decimalRadioButton.Name = "decimalRadioButton";
			this.decimalRadioButton.Size = new System.Drawing.Size(63, 19);
			this.decimalRadioButton.TabIndex = 3;
			this.decimalRadioButton.Text = "Decimal";
			this.decimalRadioButton.CheckedChanged += new System.EventHandler(this.radioButton_CheckedChanged);
// 
// bytesTextBox
// 
			this.bytesTextBox.BackColor = System.Drawing.Color.White;
			this.bytesTextBox.Location = new System.Drawing.Point(20, 127);
			this.bytesTextBox.Multiline = true;
			this.bytesTextBox.Name = "bytesTextBox";
			this.bytesTextBox.ReadOnly = true;
			this.bytesTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.bytesTextBox.Size = new System.Drawing.Size(227, 99);
			this.bytesTextBox.TabIndex = 2;
// 
// stringTextBox
// 
			this.stringTextBox.Location = new System.Drawing.Point(20, 39);
			this.stringTextBox.Multiline = true;
			this.stringTextBox.Name = "stringTextBox";
			this.stringTextBox.Size = new System.Drawing.Size(227, 41);
			this.stringTextBox.TabIndex = 1;
			this.stringTextBox.TextChanged += new System.EventHandler(this.stringTextBox_TextChanged);
// 
// stringLabel
// 
			this.stringLabel.Location = new System.Drawing.Point(20, 19);
			this.stringLabel.Name = "stringLabel";
			this.stringLabel.Size = new System.Drawing.Size(213, 20);
			this.stringLabel.Text = "String";
// 
// EncodingUserControl
// 
			this.Controls.Add(this.dataGroupBox);
			this.Name = "EncodingUserControl";
			this.Size = new System.Drawing.Size(286, 299);
			this.dataGroupBox.ResumeLayout(false);
			this.dataGroupBox.PerformLayout();
			this.ResumeLayout(false);
		}
		#endregion

		private void stringTextBox_TextChanged(object sender, System.EventArgs e)
		{
			string text = ((TextBox)sender).Text;

			bytesTextBox.Text = "";
			foreach (byte b in encodedByte)
			{
				//Convert each char to bytes and print 
				bytesTextBox.Text += b.ToString(byteStrFormat, CultureInfo.CurrentUICulture);
				bytesTextBox.Text += " ";
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

			bytesTextBox.Text = "";
			foreach (byte b in encodedByte)
			{
				bytesTextBox.Text += (b).ToString(byteStrFormat, CultureInfo.CurrentUICulture);
				bytesTextBox.Text += " ";
			}
		}
	}
}
