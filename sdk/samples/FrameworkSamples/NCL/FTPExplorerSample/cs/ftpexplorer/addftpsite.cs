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

namespace Microsoft.Samples.FTPExplorer
{
	/// <summary>
	/// Summary description for AddFtpSite.
	/// </summary>
	public class AddFtpSite : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button cmdCancel;
		private System.Windows.Forms.CheckBox chkAnonymous;
        	private CheckBox chkEnableSSL;
       	 private System.Windows.Forms.TextBox txtPassword;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.TextBox txtUserID;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox txtFtpHost;
		private System.Windows.Forms.Button cmdAdd;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		public string FtpHost="";
       	public string UserID = "";
        	public bool EnableSSL;

        	public string Password="";

		public AddFtpSite()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
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
            this.cmdCancel = new System.Windows.Forms.Button();
            this.cmdAdd = new System.Windows.Forms.Button();
            this.chkAnonymous = new System.Windows.Forms.CheckBox();
            this.txtPassword = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.txtUserID = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.txtFtpHost = new System.Windows.Forms.TextBox();
            this.chkEnableSSL = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
// 
// cmdCancel
// 
            this.cmdCancel.BackColor = System.Drawing.Color.NavajoWhite;
            this.cmdCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cmdCancel.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmdCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cmdCancel.ForeColor = System.Drawing.Color.Black;
            this.cmdCancel.Location = new System.Drawing.Point(32, 142);
            this.cmdCancel.Name = "cmdCancel";
            this.cmdCancel.Size = new System.Drawing.Size(104, 23);
            this.cmdCancel.TabIndex = 25;
            this.cmdCancel.Text = "Cancel";
// 
// cmdAdd
// 
            this.cmdAdd.BackColor = System.Drawing.Color.NavajoWhite;
            this.cmdAdd.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.cmdAdd.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmdAdd.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cmdAdd.ForeColor = System.Drawing.Color.IndianRed;
            this.cmdAdd.Location = new System.Drawing.Point(152, 142);
            this.cmdAdd.Name = "cmdAdd";
            this.cmdAdd.Size = new System.Drawing.Size(120, 23);
            this.cmdAdd.TabIndex = 24;
            this.cmdAdd.Text = "Add Site";
            this.cmdAdd.Click += new System.EventHandler(this.cmdAdd_Click);
// 
// chkAnonymous
// 
            this.chkAnonymous.Checked = true;
            this.chkAnonymous.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkAnonymous.Location = new System.Drawing.Point(8, 57);
            this.chkAnonymous.Name = "chkAnonymous";
            this.chkAnonymous.Size = new System.Drawing.Size(120, 14);
            this.chkAnonymous.TabIndex = 23;
            this.chkAnonymous.Text = "Use Anonymous";
            this.chkAnonymous.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
// 
// txtPassword
// 
            this.txtPassword.Enabled = false;
            this.txtPassword.Location = new System.Drawing.Point(120, 102);
            this.txtPassword.Name = "txtPassword";
            this.txtPassword.PasswordChar = '*';
            this.txtPassword.Size = new System.Drawing.Size(152, 20);
            this.txtPassword.TabIndex = 22;
            this.txtPassword.Text = "a@anony.com";
// 
// label5
// 
            this.label5.Location = new System.Drawing.Point(48, 102);
            this.label5.Margin = new System.Windows.Forms.Padding(3, 0, 3, 3);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(64, 20);
            this.label5.TabIndex = 21;
            this.label5.Text = "Password";
// 
// txtUserID
// 
            this.txtUserID.Enabled = false;
            this.txtUserID.Location = new System.Drawing.Point(120, 78);
            this.txtUserID.Name = "txtUserID";
            this.txtUserID.Size = new System.Drawing.Size(152, 20);
            this.txtUserID.TabIndex = 20;
            this.txtUserID.Text = "anonymous";
// 
// label4
// 
            this.label4.Location = new System.Drawing.Point(48, 78);
            this.label4.Margin = new System.Windows.Forms.Padding(3, 3, 3, 1);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(56, 20);
            this.label4.TabIndex = 19;
            this.label4.Text = "User ID";
// 
// label1
// 
            this.label1.Location = new System.Drawing.Point(8, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(63, 23);
            this.label1.TabIndex = 16;
            this.label1.Text = "Ftp Host";
// 
// txtFtpHost
// 
            this.txtFtpHost.Location = new System.Drawing.Point(78, 8);
            this.txtFtpHost.Name = "txtFtpHost";
            this.txtFtpHost.Size = new System.Drawing.Size(194, 20);
            this.txtFtpHost.TabIndex = 13;
            this.txtFtpHost.Text = "ftp.microsoft.com";
// 
// chkEnableSSL
// 
            this.chkEnableSSL.AutoSize = true;
            this.chkEnableSSL.Location = new System.Drawing.Point(8, 38);
            this.chkEnableSSL.Name = "chkEnableSSL";
            this.chkEnableSSL.Size = new System.Drawing.Size(178, 17);
            this.chkEnableSSL.TabIndex = 26;
            this.chkEnableSSL.Text = "Enable Secure Connection (SSL)";
// 
// AddFtpSite
// 
            this.ClientSize = new System.Drawing.Size(304, 182);
            this.Controls.Add(this.chkEnableSSL);
            this.Controls.Add(this.cmdCancel);
            this.Controls.Add(this.cmdAdd);
            this.Controls.Add(this.chkAnonymous);
            this.Controls.Add(this.txtPassword);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.txtUserID);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtFtpHost);
            this.Name = "AddFtpSite";
            this.Text = "AddFtpSite";
            this.ResumeLayout(false);
            this.PerformLayout();

        }
		#endregion

		private void cmdAdd_Click(object sender, System.EventArgs e)
		{
			FtpHost     = txtFtpHost.Text ;
			UserID      = txtUserID.Text ;
			Password    = txtPassword.Text;
            EnableSSL = chkEnableSSL.Checked;

        }

		private void checkBox1_CheckedChanged(object sender, System.EventArgs e)
		{
			if(this.chkAnonymous.Checked == false) // was false and now becoming checked
			{
				this.txtUserID.Enabled= true;
				this.txtPassword.Enabled= true;
			}
			else
			{
				this.txtUserID.Text = "Anonymous";
				this.txtPassword.Text = "a@Anonymous";
				this.txtUserID.Enabled= false;
				this.txtPassword.Enabled= false;
			}
		}		
	}
}
