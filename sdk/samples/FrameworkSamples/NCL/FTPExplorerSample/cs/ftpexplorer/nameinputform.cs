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

namespace Microsoft.Samples.FTPExplorer
{
	/// <summary>
	/// Summary description for form.
	/// </summary>
	public partial class NameInputForm : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		public NameInputForm()
		{
			InitializeComponent();			
		}

		public string Title
		{
			get
			{
				return this.Text;
			}
			set
			{
				this.Text = value;
			}
		}
		public string InputParameterLabel
		{
			get
			{
				if (this.lblInputString != null)
				{
					return this.lblInputString.Text;
			}
				else
				{
					return "";
				}
			}
			set
			{
				if (this.lblInputString != null)
				{
					this.lblInputString.Text = value;
				}
			}
		}
		public string InputValue
		{
			get
			{
				return this.txtInputValue.Text;
			}
		}

		private void NameInputdlg_Load(object sender, System.EventArgs e)
		{
			//this.lblInputString.Text = _inputParameterLabel;
		}

		private void NameInputdlg_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			
		}

		private void cmdOK_Click(object sender, System.EventArgs e)
		{
			
		}

		private void NameInputdlg_FormClosing(object sender, System.Windows.Forms.FormClosingEventArgs e)
		{
			//_inputParamValue = this.txtInputValue.Text;
		}


	}
}
