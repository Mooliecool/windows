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

namespace Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication
{
	/// <summary>
	/// User control encapsulating the controls for encoding a string
	/// </summary>
	public class EncodeStringUserControl : System.Windows.Forms.UserControl
	{
		private EncodingUserControl inputControl;

		private EncodingUserControl outputControl;

		private System.Windows.Forms.Button encodeButton;

		private System.ComponentModel.Container components = null;

		//Callback to main form
		private EncodeForm encodeForm;

		public EncodeStringUserControl(EncodeForm encodeForm)
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
			this.encodeForm = encodeForm;
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

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.inputControl = new EncodingUserControl(false);
			this.outputControl = new EncodingUserControl(true);
			this.encodeButton = new System.Windows.Forms.Button();
			this.SuspendLayout();
// 
// inputControl
// 
			this.inputControl.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte) (0)));
			this.inputControl.Location = new System.Drawing.Point(15, 8);
			this.inputControl.Name = "inputControl";
			this.inputControl.Size = new System.Drawing.Size(285, 296);
			this.inputControl.TabIndex = 0;
// 
// encodeButton
// 
			this.encodeButton.BackColor = System.Drawing.SystemColors.ButtonFace;
			this.encodeButton.Location = new System.Drawing.Point(101, 325);
			this.encodeButton.Name = "encodeButton";
			this.encodeButton.TabIndex = 1;
			this.encodeButton.Text = "Encode";
			this.encodeButton.Click += new System.EventHandler(this.okButton_Click);
// 
// outputControl
// 
			this.outputControl.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte) (0)));
			this.outputControl.Location = new System.Drawing.Point(298, 8);
			this.outputControl.Name = "outputControl";
			this.outputControl.Size = new System.Drawing.Size(285, 296);
			this.outputControl.TabIndex = 2;
// 
// EncodeStringUserControl
// 
			this.Controls.Add(this.encodeButton);
			this.Controls.Add(this.inputControl);
			this.Controls.Add(this.outputControl);
			this.Name = "EncodeStringUserControl";
			this.Size = new System.Drawing.Size(596, 418);
			this.Load += new System.EventHandler(this.EncodeStringUserControl_Load);
			this.ResumeLayout(false);

		}
		#endregion

		private void okButton_Click(object sender, System.EventArgs e)
		{
			try
			{
				// Receive encoding from the encoderInfo object in the main form
				Encoding dstEncoding = encodeForm.EncodeInfo.Encoding;
				//Normalize string according to normalization form chosen
				string inputString = encodeForm.EncodeInfo.GetNormalizedString(inputControl.Text);
				string encodedString = "";

				//GetBytes returns the byte array after encoding
				byte[] encodedBytes = dstEncoding.GetBytes(inputString);
				
				foreach (byte b in encodedBytes)
				{
					if (b < 32)
					{
						//add a space to show the empty byte
						encodedString += " ";
					}
					else
						encodedString += (char)b;
					//outputControl.EncodedBytes.Add(b);
				}

				outputControl.EncodedBytes = encodedBytes;
				outputControl.Visible = true;
				outputControl.Text = encodedString;
			}
			catch (EncoderFallbackException exc)
			{
				MessageBox.Show("EncoderFallbackException:\n" + exc.Message);
			}
		}

		private void EncodeStringUserControl_Load(object sender, System.EventArgs e)
		{
			encodeButton.Visible = true;
			outputControl.Visible = false;
		}
	}
}
