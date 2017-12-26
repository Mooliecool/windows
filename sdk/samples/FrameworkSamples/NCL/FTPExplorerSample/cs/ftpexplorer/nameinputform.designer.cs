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

namespace Microsoft.Samples.FTPExplorer
{
	public partial class NameInputForm : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.lblInputString = new System.Windows.Forms.Label();
            this.txtInputValue = new System.Windows.Forms.TextBox();
            this.cmdOK = new System.Windows.Forms.Button();
            this.cmdCancel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // lblInputString
            // 
            this.lblInputString.Location = new System.Drawing.Point(18, 9);
            this.lblInputString.Margin = new System.Windows.Forms.Padding(3, 3, 2, 3);
            this.lblInputString.Name = "lblInputString";
            this.lblInputString.Size = new System.Drawing.Size(174, 23);
            this.lblInputString.TabIndex = 0;
            this.lblInputString.Text = "Input String";
            // 
            // txtInputValue
            // 
            this.txtInputValue.Location = new System.Drawing.Point(189, 9);
            this.txtInputValue.Margin = new System.Windows.Forms.Padding(1, 3, 3, 3);
            this.txtInputValue.Name = "txtInputValue";
            this.txtInputValue.Size = new System.Drawing.Size(191, 20);
            this.txtInputValue.TabIndex = 1;
            // 
            // cmdOK
            // 
            this.cmdOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.cmdOK.Location = new System.Drawing.Point(242, 53);
            this.cmdOK.Name = "cmdOK";
            this.cmdOK.TabIndex = 2;
            this.cmdOK.Text = "&OK";
            this.cmdOK.Click += new System.EventHandler(this.cmdOK_Click);
            // 
            // cmdCancel
            // 
            this.cmdCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cmdCancel.Location = new System.Drawing.Point(117, 53);
            this.cmdCancel.Name = "cmdCancel";
            this.cmdCancel.TabIndex = 3;
            this.cmdCancel.Text = "&Cancel";
            // 
            // NameInputForm
            // 
            this.AutoSize = true;
            this.ClientSize = new System.Drawing.Size(401, 97);
            this.ControlBox = false;
            this.Controls.Add(this.cmdCancel);
            this.Controls.Add(this.cmdOK);
            this.Controls.Add(this.txtInputValue);
            this.Controls.Add(this.lblInputString);
            this.Name = "NameInputForm";
            this.Padding = new System.Windows.Forms.Padding(9);
            this.Text = "NameInputForm";
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.NameInputdlg_Paint);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.NameInputdlg_FormClosing);
            this.Load += new System.EventHandler(this.NameInputdlg_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

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

		//private System.Windows.Forms.Label label1;
		//private System.Windows.Forms.TextBox textBox1;
		//private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Label lblInputString;
		private System.Windows.Forms.TextBox txtInputValue;
		//private System.Windows.Forms.Button OK;
		//private System.Windows.Forms.Button button2;
		private System.Windows.Forms.Button cmdCancel;
		private System.Windows.Forms.Button cmdOK;
	}
}
