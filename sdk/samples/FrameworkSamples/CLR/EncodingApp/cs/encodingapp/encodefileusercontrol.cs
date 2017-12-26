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
using System.IO;
using System.Diagnostics;
using System.Globalization;

namespace Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication
{
	/// <summary>
	/// User control to encapsulate file encode UI
	/// </summary>
	public class EncodeFileUserControl : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.Button browseButton;
		private System.Windows.Forms.TextBox fileNameTextBox;
		private System.Windows.Forms.OpenFileDialog openFileDialog;
		private System.Windows.Forms.Button encodeButton;
		private System.Windows.Forms.GroupBox openFilePanel;
		private System.Windows.Forms.GroupBox encodeFileGroupBox;
		private System.Windows.Forms.Label captionLabel;
		private System.Windows.Forms.LinkLabel outputLinkLabel;
		private System.Windows.Forms.ComboBox decodeComboBox;
		private System.Windows.Forms.Label decodeLabel;
		private System.Windows.Forms.Label intermediateLabel;
		private System.Windows.Forms.LinkLabel intermediateLinkLabel;
		private Encoding decoding;
		private EncodingInfo[] encodings;

		//Callback to main form
		private EncodeForm encodeForm;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public EncodeFileUserControl(EncodeForm encodeForm)
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			//Initialize callback 
			this.encodeForm = encodeForm;
			this.decoding = Encoding.Default;
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
			this.browseButton = new System.Windows.Forms.Button();
			this.fileNameTextBox = new System.Windows.Forms.TextBox();
			this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.encodeButton = new System.Windows.Forms.Button();
			this.openFilePanel = new System.Windows.Forms.GroupBox();
			this.encodeFileGroupBox = new System.Windows.Forms.GroupBox();
			this.outputLinkLabel = new System.Windows.Forms.LinkLabel();
			this.captionLabel = new System.Windows.Forms.Label();
			this.intermediateLabel = new System.Windows.Forms.Label();
			this.intermediateLinkLabel = new System.Windows.Forms.LinkLabel();
			this.decodeComboBox = new ComboBox();
			this.decodeLabel = new Label();
			this.openFilePanel.SuspendLayout();
			this.encodeFileGroupBox.SuspendLayout();
			this.SuspendLayout();
// 
// browseButton
// 
			this.browseButton.BackColor = System.Drawing.SystemColors.ButtonFace;
			this.browseButton.Location = new System.Drawing.Point(255, 19);
			this.browseButton.Name = "browseButton";
			this.browseButton.Size = new System.Drawing.Size(60, 23);
			this.browseButton.TabIndex = 19;
			this.browseButton.Text = "Browse...";
			this.browseButton.Click += new System.EventHandler(this.browseButton_Click);
// 
// fileNameTextBox
// 
			this.fileNameTextBox.Location = new System.Drawing.Point(55, 48);
			this.fileNameTextBox.Name = "fileNameTextBox";
			this.fileNameTextBox.Size = new System.Drawing.Size(225, 20);
			this.fileNameTextBox.TabIndex = 18;
// 
// encodeButton
// 
			this.encodeButton.BackColor = System.Drawing.SystemColors.ButtonFace;
			this.encodeButton.Location = new System.Drawing.Point(24, 144);
			this.encodeButton.Name = "encodeButton";
			this.encodeButton.Size = new System.Drawing.Size(90, 25);
			this.encodeButton.TabIndex = 20;
			this.encodeButton.Text = "Encode";
			this.encodeButton.Click += new System.EventHandler(this.encodeButton_Click);
// 
// openFilePanel
// 
			//this.openFilePanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.openFilePanel.Controls.Add(this.browseButton);
			this.openFilePanel.Location = new System.Drawing.Point(39, 27);
			this.openFilePanel.Name = "openFilePanel";
			this.openFilePanel.Size = new System.Drawing.Size(329, 55);
			this.openFilePanel.TabIndex = 21;
// 
// encodeFileGroupBox
// 
			this.encodeFileGroupBox.Controls.Add(this.outputLinkLabel);
			this.encodeFileGroupBox.Controls.Add(this.captionLabel);
			this.encodeFileGroupBox.Controls.Add(this.encodeButton);
			this.encodeFileGroupBox.Controls.Add(this.decodeLabel);
			this.encodeFileGroupBox.Controls.Add(this.decodeComboBox);
			this.encodeFileGroupBox.Controls.Add(this.intermediateLabel);
			this.encodeFileGroupBox.Controls.Add(this.intermediateLinkLabel);
			this.encodeFileGroupBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte) (0)));
			this.encodeFileGroupBox.Location = new System.Drawing.Point(15, 8);
			this.encodeFileGroupBox.Name = "encodeFileGroupBox";
			this.encodeFileGroupBox.Size = new System.Drawing.Size(383, 380);
			this.encodeFileGroupBox.TabIndex = 24;
			this.encodeFileGroupBox.TabStop = false;
			this.encodeFileGroupBox.Text = "Encode file";
// 
// outputLinkLabel
// 
			this.outputLinkLabel.Links.Add(new System.Windows.Forms.LinkLabel.Link(0, 0));
			this.outputLinkLabel.Location = new System.Drawing.Point(17, 200);
			this.outputLinkLabel.Name = "outputLinkLabel";
			this.outputLinkLabel.Size = new System.Drawing.Size(336, 50);
			this.outputLinkLabel.TabIndex = 22;
			this.outputLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.outputLinkLabel_LinkClicked);
// 
// intermediateLinkLabel
// 
			this.intermediateLinkLabel.Links.Add(new System.Windows.Forms.LinkLabel.Link(0, 0));
			this.intermediateLinkLabel.Location = new System.Drawing.Point(17, 272);
			this.intermediateLinkLabel.Name = "intermediateLinkLabel";
			this.intermediateLinkLabel.Size = new System.Drawing.Size(336, 50);
			this.intermediateLinkLabel.TabIndex = 53;
			this.intermediateLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.intermediateLinkLabel_LinkClicked);
// 
// captionLabel
// 
			this.captionLabel.Location = new System.Drawing.Point(17, 180);
			this.captionLabel.Name = "captionLabel";
			this.captionLabel.Size = new System.Drawing.Size(300, 20);
			this.captionLabel.TabIndex = 21;
			this.captionLabel.Text = "The encoded file has been saved at the location: ";
// 
// intermediateLabel
// 
			this.intermediateLabel.Location = new System.Drawing.Point(17, 252);
			this.intermediateLabel.Name = "intermediateLabel";
			this.intermediateLabel.Size = new System.Drawing.Size(300, 20);
			this.intermediateLabel.TabIndex = 51;
			this.intermediateLabel.Text = "The intermediate file has been saved at the location: ";
// 
// decodeLabel
// 
			this.decodeLabel.Location = new System.Drawing.Point(20, 100);
			this.decodeLabel.Name = "decodeLabel";
			this.decodeLabel.Size = new System.Drawing.Size(120, 20);
			this.decodeLabel.TabIndex = 21;
			this.decodeLabel.Text = "Input file's encoding";
// 
// decodeComboBox
// 
			this.decodeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.decodeComboBox.FormattingEnabled = true;
			this.decodeComboBox.Location = new System.Drawing.Point(150, 95);
			this.decodeComboBox.Name = "decodeComboBox";
			this.decodeComboBox.Size = new System.Drawing.Size(202, 21);
			this.decodeComboBox.TabIndex = 25;
// 
// EncodeFileUserControl
// 
			this.Controls.Add(this.fileNameTextBox);
			this.Controls.Add(this.openFilePanel);
			this.Controls.Add(this.encodeFileGroupBox);
			this.Name = "EncodeFileUserControl";
			this.Size = new System.Drawing.Size(409, 400);
			this.Load += new System.EventHandler(this.EncodeFileUserControl_Load);
			this.openFilePanel.ResumeLayout(false);
			this.encodeFileGroupBox.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();
		}
		#endregion

		//File is decoded into an intermediate file with the encoding picked off the dropdown or
		//default setting. Anyhow, this process becomes considerably easier using a StreamReader where
		//the decoding can be passed as a parameter. Same with StreamWriter - it has a parameter using
		//which target encoding can be passed. The following code is to illustrate the usage of the 
		//Convert API
		private string DecodeFile()
		{
			//Check if file name has been entered
			if (fileNameTextBox.Text == "")
			{
				MessageBox.Show("Enter file name");
				return null;
			}
			//Check if file exists
			if (!File.Exists(fileNameTextBox.Text))
			{
				MessageBox.Show("File does not exist. Please try again!");
				//Clear textbox
				fileNameTextBox.Text = "";
				return null;
			}
			StreamWriter fileWriter = null;
			try
			{
				//Extract input filename and add prefix of "Intermediate_" to filename
				string intermediateFileName = Path.GetDirectoryName(fileNameTextBox.Text) + Path.DirectorySeparatorChar + "Intermediate_" + Path.GetFileName(fileNameTextBox.Text);

				fileWriter = new StreamWriter(intermediateFileName, false, Encoding.Unicode, 512);

				using (BinaryReader fileReader = new BinaryReader(new FileStream(fileNameTextBox.Text, FileMode.Open)))
				{
					//Instantiate decoder from the encoding selected
					Decoder dstDecoder = decoding.GetDecoder();
					//buffers
					byte[] dataBlock = new byte[1024];
					char[] decodedData = new char[512];
					//Read the next block spanning 1024 chars
					int bytesRead = 0;
					while ((bytesRead = fileReader.Read(dataBlock, 0, dataBlock.Length)) > 0)
					{
						//Number of bytes used in decoding and number of characters used
						int bytesUsed = 0, charsUsed = 0;
						int byteStart = 0;
						bool completed = false;

						while (!completed)
						{
							//Decode the data block 
							dstDecoder.Convert(dataBlock, byteStart, bytesRead - byteStart, decodedData, 0, 512, true, out bytesUsed, out charsUsed, out completed);
							byteStart += bytesUsed;
							//Write decoded bytes into the intermediate file
							fileWriter.Write(decodedData, 0, bytesUsed);
						}
					}
				}
				return intermediateFileName;
			}
			catch (IOException ioExc)
			{
				MessageBox.Show("IOException:\n" + ioExc.Message);
				return null;
			}
			catch (EncoderFallbackException fbExc)
			{
				MessageBox.Show("EncoderFallbackException:\n" + fbExc.Message);
				return null;
			}
			catch (ArgumentException aExc)
			{
				MessageBox.Show(aExc.Message);
				return null;
			}
			finally 
			{
				fileWriter.Close();
			}
		}

		private void encodeButton_Click(object sender, System.EventArgs e)
		{
			string decodedFileName = DecodeFile();
			if (decodedFileName == null)
				return;

			FileStream fileWriter = null;
			try
			{
				//Extract input filename and add prefix of "Encoded_" to filename
				string outputFileName = Path.GetDirectoryName(fileNameTextBox.Text) +
					Path.DirectorySeparatorChar +
					"Encoded_" +
					Path.GetFileName(fileNameTextBox.Text);

				fileWriter = new FileStream(outputFileName, FileMode.Create);

				using (StreamReader fileReader = new StreamReader(decodedFileName))
				{
					//Get encoding from encoderInfo object
					Encoding dstEncoding = encodeForm.EncodeInfo.Encoding;

					//Instantiate encoder from the encoding selected
					Encoder dstEncoder = dstEncoding.GetEncoder();

					//buffers
					char[] dataBlock = new char[1024];
					byte[] encodedData = new byte[512];

					//Read the next block spanning 1024 chars
					int charsRead = 0;
					while ((charsRead = fileReader.Read (dataBlock,0,dataBlock .Length))>0)
					{
						//Number of characters used in encoding and number of bytes used
						int charsUsed = 0, bytesUsed = 0;
						int charStart = 0;
						bool completed = false;

						while (!completed)
						{
							//Encode the data block 
							dstEncoder.Convert(dataBlock, charStart, charsRead - charStart, encodedData, 0, 512, true, out charsUsed, out bytesUsed, out completed);
							charStart += charsUsed;
							//Write encoded bytes into the output file
							fileWriter.Write(encodedData,0,bytesUsed);
						}
					}
				}

				//Display the intermediate file name
				captionLabel.Visible = true;
				outputLinkLabel.Visible = true;
				intermediateLabel.Visible = true;
				intermediateLinkLabel.Visible = true;
				
				//Display the output file name
				outputLinkLabel.Text = outputFileName;
				outputLinkLabel.Links.Clear();
				outputLinkLabel.Links.Add(0, outputFileName.Length);
				intermediateLinkLabel.Text = decodedFileName;
				intermediateLinkLabel.Links.Clear();
				intermediateLinkLabel.Links.Add(0, decodedFileName.Length);
			}
			catch (IOException ioExc)
			{
				MessageBox.Show("IOException:\n" + ioExc.Message);
			}
			catch (EncoderFallbackException fbExc)
			{
				MessageBox.Show("EncoderFallbackException:\n" + fbExc.Message);
			}
			catch (ArgumentException aExc)
			{
				MessageBox.Show(aExc.Message);
			}
			finally 
			{
				fileWriter.Close();
			}
		}

		//Open file specified
		private void browseButton_Click(object sender, System.EventArgs e)
		{
			captionLabel.Visible = false;
			outputLinkLabel.Visible = false;
			intermediateLabel.Visible = false;
			intermediateLinkLabel.Visible = false;

			openFileDialog.ShowDialog();
			fileNameTextBox.Text = openFileDialog.FileName;
		}

		//Called at startup
		private void EncodeFileUserControl_Load(object sender, System.EventArgs e)
		{
			captionLabel.Visible = false;
			outputLinkLabel.Visible = false;
			intermediateLabel.Visible = false;
			intermediateLinkLabel.Visible = false;

			// Initialize encoding schemes
			encodings = Encoding.GetEncodings();
			ArrayList encList = new ArrayList(encodings);
			IComparer encComp = new EncodingSortClass();
			encList.Sort(encComp);
			encodings = (EncodingInfo[]) encList.ToArray(typeof(EncodingInfo));

			foreach (EncodingInfo ei in encodings)
			{
				decodeComboBox.Items.Add(ei.DisplayName);
			}
			decodeComboBox.Items.Insert(0, "Default");

			decodeComboBox.SelectedIndex = 0;
			this.decodeComboBox.SelectedIndexChanged += new System.EventHandler(this.decodeComboBox_SelectedIndexChanged);
		}

		private void decodeComboBox_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if (((ComboBox)sender).SelectedIndex <= 0)
				decoding = Encoding.Default;
			else
				decoding = Encoding.GetEncoding(encodings[((ComboBox)sender).SelectedIndex-1].CodePage);
		}

		private void intermediateLinkLabel_LinkClicked(object sender, System.Windows.Forms.LinkLabelLinkClickedEventArgs e)
		{
			try
			{
				//Create a new process to open editor to display intermediate file
				Process openFile = new Process();
				openFile.StartInfo.FileName = intermediateLinkLabel.Text;
				openFile.StartInfo.RedirectStandardOutput = false;
				openFile.StartInfo.UseShellExecute = true;
				openFile.Start();
			}
			catch (Exception exc)
			{
				MessageBox.Show("Error trying to open file!\n" + exc.Message);
			}
			
		}

		private void outputLinkLabel_LinkClicked(object sender, System.Windows.Forms.LinkLabelLinkClickedEventArgs e)
		{
			try
			{
				//Create a new process to open editor to display output file
				Process openFile = new Process();
				openFile.StartInfo.FileName = outputLinkLabel.Text;
				openFile.StartInfo.RedirectStandardOutput = false;
				openFile.StartInfo.UseShellExecute = true;
				openFile.Start();
			}
			catch (Exception exc)
			{
				MessageBox.Show("Error trying to open file!\n" + exc.Message);
			}
			
		}
	}
}
