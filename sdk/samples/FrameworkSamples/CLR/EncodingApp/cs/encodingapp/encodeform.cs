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
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Text;
using System.Globalization;

namespace Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication
{
	/// <summary>
	/// Main form to display the UI
	/// </summary>
	public class EncodeForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Panel optionsPanel;
		private System.Windows.Forms.Label encodingLabel;
		private System.Windows.Forms.Label stringLabel;
		private System.Windows.Forms.Label decodeStringLabel;
		private System.Windows.Forms.Label fileLabel;
		private System.Windows.Forms.Label fallBackLabel;
		private System.Windows.Forms.ComboBox encodingSchemeComboBox;
		private System.Windows.Forms.ComboBox normalizationComboBox;
		private System.Windows.Forms.ComboBox fallbackSchemeComboBox;
		private System.Windows.Forms.Label normalizationLabel;
		private System.Windows.Forms.Label encdingLabel;

		//Variable to contain encoding options
		private EncoderInfo encoderInfo;

		//User controls displaying string encode and file encode
		private Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication.EncodeStringUserControl stringControl;
		private Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication.EncodeFileUserControl fileControl;
		private Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication.DecodeBytesUserControl decodeStringControl;
		private System.Windows.Forms.Panel descriptionPanel;
		private System.Windows.Forms.Label descFallbacklabel;
		private System.Windows.Forms.Label descNormalizationlabel;
		private System.Windows.Forms.Label titleNormalizationLabel;
		private System.Windows.Forms.Label titleFallbackLabel;
		private System.Windows.Forms.Label encDescLabel;
		private System.Windows.Forms.Label titleLabel;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public EncodeForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//Initialize user controls
			InitializeUserControls();
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

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.optionsPanel = new System.Windows.Forms.Panel();
			this.encodingSchemeComboBox = new System.Windows.Forms.ComboBox();
			this.normalizationComboBox = new System.Windows.Forms.ComboBox();
			this.normalizationLabel = new System.Windows.Forms.Label();
			this.fallbackSchemeComboBox = new System.Windows.Forms.ComboBox();
			this.fallBackLabel = new System.Windows.Forms.Label();
			this.encdingLabel = new System.Windows.Forms.Label();
			this.fileLabel = new System.Windows.Forms.Label();
			this.stringLabel = new System.Windows.Forms.Label();
			this.decodeStringLabel = new System.Windows.Forms.Label();
			this.encodingLabel = new System.Windows.Forms.Label();
			this.descriptionPanel = new System.Windows.Forms.Panel();
			this.descFallbacklabel = new System.Windows.Forms.Label();
			this.descNormalizationlabel = new System.Windows.Forms.Label();
			this.titleNormalizationLabel = new System.Windows.Forms.Label();
			this.titleFallbackLabel = new System.Windows.Forms.Label();
			this.encDescLabel = new System.Windows.Forms.Label();
			this.titleLabel = new System.Windows.Forms.Label();
			this.optionsPanel.SuspendLayout();
			this.descriptionPanel.SuspendLayout();
			this.SuspendLayout();

// 
// encodingSchemeComboBox
// 
			this.encodingSchemeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.encodingSchemeComboBox.FormattingEnabled = true;
			this.encodingSchemeComboBox.Location = new System.Drawing.Point(14, 155);
			this.encodingSchemeComboBox.Margin = new System.Windows.Forms.Padding(3, 1, 3, 3);
			this.encodingSchemeComboBox.Name = "encodingSchemeComboBox";
			this.encodingSchemeComboBox.Size = new System.Drawing.Size(182, 21);
			this.encodingSchemeComboBox.TabIndex = 25;
// 
// normalizationComboBox
// 
			this.normalizationComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.normalizationComboBox.FormattingEnabled = true;
			this.normalizationComboBox.Location = new System.Drawing.Point(14, 261);
			this.normalizationComboBox.Margin = new System.Windows.Forms.Padding(3, 0, 3, 3);
			this.normalizationComboBox.Name = "normalizationComboBox";
			this.normalizationComboBox.Size = new System.Drawing.Size(182, 21);
			this.normalizationComboBox.TabIndex = 27;
			this.normalizationComboBox.SelectedIndexChanged += new System.EventHandler(this.normalizationComboBox_SelectedIndexChanged);
// 
// normalizationLabel
// 
			this.normalizationLabel.Location = new System.Drawing.Point(14, 247);
			this.normalizationLabel.Margin = new System.Windows.Forms.Padding(3, 3, 3, 1);
			this.normalizationLabel.Name = "normalizationLabel";
			this.normalizationLabel.Size = new System.Drawing.Size(143, 13);
			this.normalizationLabel.TabIndex = 30;
			this.normalizationLabel.Text = "Normalization:";
// 
// fallbackSchemeComboBox
// 
			this.fallbackSchemeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.fallbackSchemeComboBox.FormattingEnabled = true;
			this.fallbackSchemeComboBox.Location = new System.Drawing.Point(14, 208);
			this.fallbackSchemeComboBox.Margin = new System.Windows.Forms.Padding(3, 1, 3, 3);
			this.fallbackSchemeComboBox.Name = "fallbackSchemeComboBox";
			this.fallbackSchemeComboBox.Size = new System.Drawing.Size(182, 21);
			this.fallbackSchemeComboBox.TabIndex = 26;
			this.fallbackSchemeComboBox.SelectedIndexChanged += new System.EventHandler(this.fallbackSchemeComboBox_SelectedIndexChanged);
// 
// fallBackLabel
// 
			this.fallBackLabel.Location = new System.Drawing.Point(14, 193);
			this.fallBackLabel.Margin = new System.Windows.Forms.Padding(3, 3, 3, 1);
			this.fallBackLabel.Name = "fallBackLabel";
			this.fallBackLabel.Size = new System.Drawing.Size(143, 13);
			this.fallBackLabel.TabIndex = 29;
			this.fallBackLabel.Text = "FallBack:";
// 
// encdingLabel
// 
			this.encdingLabel.Location = new System.Drawing.Point(14, 139);
			this.encdingLabel.Margin = new System.Windows.Forms.Padding(3, 3, 3, 2);
			this.encdingLabel.Name = "encdingLabel";
			this.encdingLabel.Size = new System.Drawing.Size(143, 13);
			this.encdingLabel.TabIndex = 28;
			this.encdingLabel.Text = "Encoding:";
// 
// fileLabel
// 
			this.fileLabel.Cursor = System.Windows.Forms.Cursors.Hand;
			this.fileLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((byte) (0)));
			this.fileLabel.ForeColor = System.Drawing.SystemColors.ControlText;
			this.fileLabel.Location = new System.Drawing.Point(14, 90);
			this.fileLabel.Name = "fileLabel";
			this.fileLabel.Size = new System.Drawing.Size(113, 18);
			this.fileLabel.TabIndex = 2;
			this.fileLabel.Text = "Encode File";
			this.fileLabel.Click += new System.EventHandler(this.fileLabel_Click);
// 
// stringLabel
// 
			this.stringLabel.Cursor = System.Windows.Forms.Cursors.Hand;
			this.stringLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((byte) (0)));
			this.stringLabel.ForeColor = System.Drawing.SystemColors.ControlText;
			this.stringLabel.Location = new System.Drawing.Point(14, 70);
			this.stringLabel.Name = "stringLabel";
			this.stringLabel.Size = new System.Drawing.Size(113, 18);
			this.stringLabel.TabIndex = 1;
			this.stringLabel.Text = "Encode String";
			this.stringLabel.Click += new System.EventHandler(this.stringLabel_Click);
// 
// decodeStringLabel
// 
			this.decodeStringLabel.Cursor = System.Windows.Forms.Cursors.Hand;
			this.decodeStringLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((byte) (0)));
			this.decodeStringLabel.ForeColor = System.Drawing.SystemColors.ControlText;
			this.decodeStringLabel.Location = new System.Drawing.Point(14, 50);
			this.decodeStringLabel.Name = "decodeStringLabel";
			this.decodeStringLabel.Size = new System.Drawing.Size(113, 18);
			this.decodeStringLabel.TabIndex = 3;
			this.decodeStringLabel.Text = "Decode Bytes";
			this.decodeStringLabel.Click += new System.EventHandler(this.decodeStringLabel_Click);
// 
// encodingLabel
// 
			this.encodingLabel.Cursor = System.Windows.Forms.Cursors.Hand;
			this.encodingLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 14F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte) (0)));
			this.encodingLabel.ForeColor = System.Drawing.Color.FromArgb(((int) (((byte) (64)))), ((int) (((byte) (0)))), ((int) (((byte) (64)))));
			this.encodingLabel.Location = new System.Drawing.Point(4, 4);
			this.encodingLabel.Name = "encodingLabel";
			this.encodingLabel.Size = new System.Drawing.Size(109, 28);
			this.encodingLabel.TabIndex = 0;
			this.encodingLabel.Text = "Encoding";
			this.encodingLabel.Click += new System.EventHandler(this.encodingLabel_Click);
// 
// optionsPanel
// 
			this.optionsPanel.BackColor = System.Drawing.Color.FromArgb(((int) (((byte) (128)))), ((int) (((byte) (128)))), ((int) (((byte) (250)))));
			this.optionsPanel.Controls.Add(this.encodingSchemeComboBox);
			this.optionsPanel.Controls.Add(this.normalizationComboBox);
			this.optionsPanel.Controls.Add(this.normalizationLabel);
			this.optionsPanel.Controls.Add(this.fallbackSchemeComboBox);
			this.optionsPanel.Controls.Add(this.fallBackLabel);
			this.optionsPanel.Controls.Add(this.encdingLabel);
			this.optionsPanel.Controls.Add(this.fileLabel);
			this.optionsPanel.Controls.Add(this.stringLabel);
			this.optionsPanel.Controls.Add(this.decodeStringLabel);
			this.optionsPanel.Controls.Add(this.encodingLabel);
			this.optionsPanel.Dock = System.Windows.Forms.DockStyle.Left;
			this.optionsPanel.Location = new System.Drawing.Point(0, 0);
			this.optionsPanel.Name = "optionsPanel";
			this.optionsPanel.Size = new System.Drawing.Size(217, 500);
			this.optionsPanel.TabIndex = 0;
// 
// descriptionPanel
// 
			this.descriptionPanel.Controls.Add(this.descFallbacklabel);
			this.descriptionPanel.Controls.Add(this.descNormalizationlabel);
			this.descriptionPanel.Controls.Add(this.titleNormalizationLabel);
			this.descriptionPanel.Controls.Add(this.titleFallbackLabel);
			this.descriptionPanel.Controls.Add(this.encDescLabel);
			this.descriptionPanel.Controls.Add(this.titleLabel);
			this.descriptionPanel.Dock = System.Windows.Forms.DockStyle.Left;
			this.descriptionPanel.Location = new System.Drawing.Point(217, 0);
			this.descriptionPanel.Name = "descriptionPanel";
			this.descriptionPanel.Size = new System.Drawing.Size(580, 500);
			this.descriptionPanel.TabIndex = 7;
// 
// descFallbacklabel
// 
			this.descFallbacklabel.Location = new System.Drawing.Point(18, 129);
			this.descFallbacklabel.Margin = new System.Windows.Forms.Padding(3, 3, 3, 1);
			this.descFallbacklabel.Name = "descFallbacklabel";
			this.descFallbacklabel.Size = new System.Drawing.Size(510, 84);
			this.descFallbacklabel.TabIndex = 5;
			this.descFallbacklabel.Text = @"Fallback mechanism of the encoding determines how any character that the encoder is unable to 
encode is treated
Replacement fallback replaces that character with a pre-determined string, usually ""?""
Exception fallback throws an exception when it encounters such a character
Best Fit fallback allows the user to implement her own encoding";
// 
// descNormalizationlabel
// 
			this.descNormalizationlabel.Location = new System.Drawing.Point(18, 230);
			this.descNormalizationlabel.Name = "descNormalizationlabel";
			this.descNormalizationlabel.Size = new System.Drawing.Size(510, 46);
			this.descNormalizationlabel.TabIndex = 4;
			this.descNormalizationlabel.Text = "Normalization is done on Unicode strings in order to eliminate ambiguity between " +
				"different representations\r\n of special characters";
// 
// titleNormalizationLabel
// 
			this.titleNormalizationLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, ((System.Drawing.FontStyle) ((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte) (0)));
			this.titleNormalizationLabel.Location = new System.Drawing.Point(18, 213);
			this.titleNormalizationLabel.Margin = new System.Windows.Forms.Padding(3, 1, 3, 3);
			this.titleNormalizationLabel.Name = "titleNormalizationLabel";
			this.titleNormalizationLabel.Size = new System.Drawing.Size(446, 17);
			this.titleNormalizationLabel.TabIndex = 3;
			this.titleNormalizationLabel.Text = "Normalization";
// 
// titleFallbackLabel
// 
			this.titleFallbackLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, ((System.Drawing.FontStyle) ((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte) (0)));
			this.titleFallbackLabel.Location = new System.Drawing.Point(18, 112);
			this.titleFallbackLabel.Margin = new System.Windows.Forms.Padding(3, 1, 3, 3);
			this.titleFallbackLabel.Name = "titleFallbackLabel";
			this.titleFallbackLabel.Size = new System.Drawing.Size(446, 17);
			this.titleFallbackLabel.TabIndex = 2;
			this.titleFallbackLabel.Text = "FallBack";
// 
// encDescLabel
// 
			this.encDescLabel.Location = new System.Drawing.Point(18, 59);
			this.encDescLabel.Margin = new System.Windows.Forms.Padding(3, 3, 3, 2);
			this.encDescLabel.Name = "encDescLabel";
			this.encDescLabel.Size = new System.Drawing.Size(529, 53);
			this.encDescLabel.TabIndex = 1;
			this.encDescLabel.Text = "Encoding is the process of converting a given set of characters to a set of bytes" +
				" that uniquely represent the \r\ncharacters in a particular encoding scheme.\r\nUTF " +
				"-8 is the most popular encoding scheme and also the most recommended one.";
// 
// titleLabel
// 
			this.titleLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, ((System.Drawing.FontStyle) ((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte) (0)));
			this.titleLabel.Location = new System.Drawing.Point(18, 42);
			this.titleLabel.Name = "titleLabel";
			this.titleLabel.Size = new System.Drawing.Size(446, 17);
			this.titleLabel.TabIndex = 0;
			this.titleLabel.Text = "Encoding";
// 
// EncodeForm
// 
			this.BackColor = System.Drawing.Color.White;
			this.ClientSize = new System.Drawing.Size(800, 500);
			this.Controls.Add(this.descriptionPanel);
			this.Controls.Add(this.optionsPanel);
			this.Name = "EncodeForm";
			this.Text = "Encode";
			this.Load += new System.EventHandler(this.EncodeForm_Load);
			this.optionsPanel.ResumeLayout(false);
			this.descriptionPanel.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private void InitializeUserControls()
		{

			this.encoderInfo = new EncoderInfo();
			this.decodeStringControl = new Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication.DecodeBytesUserControl(this);
			this.stringControl = new Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication.EncodeStringUserControl(this);
			this.fileControl = new Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication.EncodeFileUserControl(this);
// 
// stringControl
// 
			this.stringControl.Location = new System.Drawing.Point(217, 0);
			this.stringControl.Name = "stringControl";
			this.stringControl.Size = new System.Drawing.Size(581, 418);
			this.stringControl.TabIndex = 5;
// 
// fileControl
// 
			this.fileControl.Location = new System.Drawing.Point(217, 0);
			this.fileControl.Name = "fileControl";
			this.fileControl.Size = new System.Drawing.Size(409, 420);
			this.fileControl.TabIndex = 4;

//
// decodeStringControl
//
			this.decodeStringControl.Location = new Point(217, 0);
			this.decodeStringControl.Name = "decodeStringControl";


			this.Controls.Add(this.fileControl);
			this.Controls.Add(this.stringControl);
			this.Controls.Add(this.decodeStringControl);
		}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			Application.EnableVisualStyles();
			Application.Run(new EncodeForm());
		}

		//Property to get the object encapsulating the encoding options
		public EncoderInfo EncodeInfo
		{
			get
			{
				return encoderInfo;
			}
		}

		//Display string encode UI
		private void stringLabel_Click(object sender, System.EventArgs e)
		{
			descriptionPanel.Visible = false;
			fileControl.Visible = false;
			stringControl.Visible = true;
			decodeStringControl.Visible = false;
		}

		//Display file encode UI
		private void fileLabel_Click(object sender, System.EventArgs e)
		{
			descriptionPanel.Visible = false;
			fileControl.Visible = true;
			stringControl.Visible = false;
			decodeStringControl.Visible = false;
		}

		//Display string decode UI
		private void decodeStringLabel_Click(object sender, System.EventArgs e)
		{
			descriptionPanel.Visible = false;
			fileControl.Visible = false;
			stringControl.Visible = false;
			decodeStringControl.Visible = true;
		}

		private void EncodeForm_Load(object sender, System.EventArgs e)
		{
			// Initialize encoding schemes
			EncodingInfo[] encodings = Encoding.GetEncodings();
			ArrayList encList = new ArrayList(encodings);
			IComparer encComp = new EncodingSortClass();
			encList.Sort(encComp);
			encodings = (EncodingInfo[]) encList.ToArray(typeof(EncodingInfo));

			encodingSchemeComboBox.DataSource = encodings;
			encodingSchemeComboBox.DisplayMember = "DisplayName";
			encodingSchemeComboBox.ValueMember = "CodePage";
			encodingSchemeComboBox.SelectedValue = Encoding.UTF8.CodePage;
			this.encodingSchemeComboBox.SelectedIndexChanged += new System.EventHandler(this.encodingSchemeComboBox_SelectedIndexChanged);

			// Initialize fallback schemes
			this.fallbackSchemeComboBox.Items.AddRange(EncoderInfo.FallbackSchemes);
			fallbackSchemeComboBox.SelectedIndex = 0;

			// Initialize normalization form
			this.normalizationComboBox.Items.Add(EncoderInfo.NormalizationNone);
			this.normalizationComboBox.Items.AddRange(Enum.GetNames(typeof(NormalizationForm)));
			normalizationComboBox.SelectedIndex = 0;
		}

		//Set the normalization scheme 
		private void normalizationComboBox_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			encoderInfo.SetNormalizationForm((string) (((ComboBox) sender).SelectedItem));
		}

		//Set the fallback scheme
		private void fallbackSchemeComboBox_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			encoderInfo.SetEncoderFallback((string) ((ComboBox) sender).SelectedItem);
		}

		//Set the encoding scheme
		private void encodingSchemeComboBox_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			encoderInfo.SetEncodingCodePage((int.Parse(((ComboBox) sender).SelectedValue.ToString(), CultureInfo.CurrentUICulture)));
		}

		private void encodingLabel_Click(object sender, System.EventArgs e)
		{
			descriptionPanel.Visible = true;
			fileControl.Visible = false;
			stringControl.Visible = false;
			decodeStringControl.Visible = false;
		}
	}

	internal class EncodingSortClass : IComparer 
	{
		int IComparer.Compare(object x, object y)
		{
			if (x is EncodingInfo && y is EncodingInfo)
				return (new CaseInsensitiveComparer().Compare(((EncodingInfo) x).DisplayName, ((EncodingInfo) y).DisplayName));
			else
				throw new ArgumentException("Object passed not EncodingInfo!");
		}
	}
}
