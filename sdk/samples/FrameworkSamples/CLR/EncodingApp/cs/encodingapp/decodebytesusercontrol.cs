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
using Microsoft.VisualBasic;
using System.Globalization;
using System.Text;

namespace Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication
{
	/// <summary>
	/// Summary description for DecodeBytesUserControl.
	/// </summary>
	public class DecodeBytesUserControl : System.Windows.Forms.UserControl
	{
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;
		private DecodeControl inputDecodeControl;
		private DecodeControl outputDecodeControl;
		private System.Windows.Forms.Button decodeButton;

		//Callback to main form
		private EncodeForm encodeForm;

		public DecodeBytesUserControl(EncodeForm encodeForm)
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
			this.encodeForm = encodeForm;
			outputDecodeControl.Visible = false;
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

		private void decodeButton_Click(object sender, System.EventArgs e)
		{
			string[] bytes;
			string byteString = inputDecodeControl.Bytes.Trim();
			
			//Encoded bytes 
			ArrayList encodedBytesList = new ArrayList();

			//Set number styles according to number base selected
			NumberStyles byteFrmt = (inputDecodeControl.ByteFormat[0] == 'X')?NumberStyles.HexNumber:NumberStyles.Integer;
			
			//Separate bytes in byte string
			bytes = byteString.Split(new char[] { ' ' });

			try
			{
				//Parse encoded bytes
				foreach (string s in bytes)
				{
					encodedBytesList.Add(byte.Parse(s, byteFrmt));
				}
				
				Encoding enc = encodeForm.EncodeInfo.Encoding;

				//Get decoder from Encoding object
				Decoder dec = enc.GetDecoder();

				//Convert arraylist of bytes to byte array
				byte[] encodedBytes = (byte[])encodedBytesList.ToArray(typeof (byte ));
				//Allot necessary memory by calling the GetCharCount function to obtain number of characters 
				//that the byte array will be decoded to
				char[] decodedChars = new char[dec.GetCharCount(encodedBytes, 0, encodedBytes.Length)];

				//Get decoded characters from the encoded byte array
				dec.GetChars(encodedBytes, 0, encodedBytes.Length, decodedChars, 0);
				
				//Alternate implementation using Convert API
				/*int b = 0, c = 0;
				bool comp = false;

				//Decode the byte array
				while (comp != true)
				{
					dec.Convert(encodedBytes, 0, encodedBytes.Length, decodedChars, 0, decodedChars.Length, true, out b, out c, out comp);
				}*/

				outputDecodeControl.Text = new string(decodedChars);
				string outputUnicodeChars = "";
				foreach (char c in outputDecodeControl.Text.ToCharArray())
					{
						//Convert each char to short and print 
						outputUnicodeChars += ((short) c).ToString(outputDecodeControl.ByteFormat, CultureInfo.CurrentUICulture);
						outputUnicodeChars += " ";
					}
				outputDecodeControl.Bytes = outputUnicodeChars;
				outputDecodeControl.Visible = true;
			}
			catch (FormatException fexc)
			{
				MessageBox.Show(fexc.Message + "\nPlease enter bytes in right format");
				return;
			}
			catch (OverflowException oexc)
			{
				MessageBox.Show(oexc.Message);
				return;
			}
			catch (DecoderFallbackException dexc)
			{
				MessageBox.Show(dexc.Message);
			}
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.decodeButton = new System.Windows.Forms.Button();
			this.inputDecodeControl = new DecodeControl(false);
			this.outputDecodeControl = new DecodeControl(true);
			this.SuspendLayout();
// 
// inputDecodeControl
// 
			this.inputDecodeControl.Bytes = "";
			this.inputDecodeControl.Location = new System.Drawing.Point(0, 0);
			this.inputDecodeControl.Name = "inputDecodeControl";
			this.inputDecodeControl.Size = new System.Drawing.Size(288, 344);
			this.inputDecodeControl.TabIndex = 0;
// 
// outputDecodeControl
// 
			this.outputDecodeControl.Bytes = "";
			this.outputDecodeControl.Location = new System.Drawing.Point(289, 0);
			this.outputDecodeControl.Name = "outputDecodeControl";
			this.outputDecodeControl.Size = new System.Drawing.Size(288, 344);
			this.outputDecodeControl.TabIndex = 1;
// 
// decodeButton
// 
			this.decodeButton.Location = new System.Drawing.Point(121, 339);
			this.decodeButton.Name = "decodeButton";
			this.decodeButton.TabIndex = 2;
			this.decodeButton.Text = "Decode";
			this.decodeButton.Click += new System.EventHandler(this.decodeButton_Click);
// 
// DecodeBytesUserControl
// 
			this.Controls.Add(this.decodeButton);
			this.Controls.Add(this.outputDecodeControl);
			this.Controls.Add(this.inputDecodeControl);
			this.Name = "DecodeBytesUserControl";
			this.Size = new System.Drawing.Size(600, 404);
			this.ResumeLayout(false);

		}
		#endregion
	}
}
