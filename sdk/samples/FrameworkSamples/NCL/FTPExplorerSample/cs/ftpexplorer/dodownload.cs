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
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Microsoft.Samples.FTPExplorer
{
	/// <summary>
	/// Summary description for DoDownLoad.
	/// </summary>
	public class DoDownLoad : System.Windows.Forms.Form
	{
		protected System.Windows.Forms.Label lblDownLoad;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		public string DownloadMessage = "";
		public string[] DownloadFileList = null;
		private System.Windows.Forms.Button cmdDownload;
		private System.Windows.Forms.Button cmdCancel;
		private System.Windows.Forms.ProgressBar progressBarDowload;
		private System.Windows.Forms.Label lblStatus;
		private System.Windows.Forms.TextBox txtLocaldir;
		public string[] DownloadDirectoryList = null;

		public DoDownLoad()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			//this.lblDownLoad.Text = DownloadMessage + "(NYI),try later";
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
			this.lblDownLoad = new System.Windows.Forms.Label();
			this.cmdDownload = new System.Windows.Forms.Button();
			this.txtLocaldir = new System.Windows.Forms.TextBox();
			this.progressBarDowload = new System.Windows.Forms.ProgressBar();
			this.cmdCancel = new System.Windows.Forms.Button();
			this.lblStatus = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// lblDownLoad
			// 
			this.lblDownLoad.Location = new System.Drawing.Point(24, 24);
			this.lblDownLoad.Name = "lblDownLoad";
			this.lblDownLoad.Size = new System.Drawing.Size(96, 16);
			this.lblDownLoad.TabIndex = 0;
			this.lblDownLoad.Text = "Local Directory";
			// 
			// cmdDownload
			// 
			this.cmdDownload.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.cmdDownload.Location = new System.Drawing.Point(224, 128);
			this.cmdDownload.Name = "cmdDownload";
			this.cmdDownload.Size = new System.Drawing.Size(144, 23);
			this.cmdDownload.TabIndex = 1;
			this.cmdDownload.Text = "Start DownLoad";
			this.cmdDownload.Click += new System.EventHandler(this.cmdDownload_Click);
			// 
			// txtLocaldir
			// 
			this.txtLocaldir.Location = new System.Drawing.Point(136, 24);
			this.txtLocaldir.Name = "txtLocaldir";
			this.txtLocaldir.Size = new System.Drawing.Size(264, 20);
			this.txtLocaldir.TabIndex = 2;
			this.txtLocaldir.Text = "e:\\adarsh\\ftptest";
			// 
			// progressBarDowload
			// 
			this.progressBarDowload.Location = new System.Drawing.Point(24, 88);
			this.progressBarDowload.Name = "progressBarDowload";
			this.progressBarDowload.Size = new System.Drawing.Size(392, 8);
			this.progressBarDowload.TabIndex = 3;
			// 
			// cmdCancel
			// 
			this.cmdCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cmdCancel.Location = new System.Drawing.Point(136, 128);
			this.cmdCancel.Name = "cmdCancel";
			this.cmdCancel.TabIndex = 4;
			this.cmdCancel.Text = "Cancel";
			// 
			// lblStatus
			// 
			this.lblStatus.Location = new System.Drawing.Point(32, 64);
			this.lblStatus.Name = "lblStatus";
			this.lblStatus.Size = new System.Drawing.Size(376, 16);
			this.lblStatus.TabIndex = 5;
			// 
			// DoDownLoad
			// 
			this.ClientSize = new System.Drawing.Size(424, 158);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.lblStatus,
																		  this.cmdCancel,
																		  this.progressBarDowload,
																		  this.txtLocaldir,
																		  this.cmdDownload,
																		  this.lblDownLoad});
			this.Name = "DoDownLoad";
			this.Text = "DoDownLoad";
			this.ResumeLayout(false);

		}
		#endregion

		private void cmdDownload_Click(object sender, System.EventArgs e)
		{
			if(this.txtLocaldir.Text[this.txtLocaldir.Text.Length-1] != '\\')
				this.txtLocaldir.Text+="\\";
			string LocalFile="";
			if(DownloadFileList != null)
			{//Download all files
				this.progressBarDowload.Maximum = DownloadFileList.Length;
				progressBarDowload.Value = 0;
				foreach(String fs in DownloadFileList)
				{						
					LocalFile = this.txtLocaldir.Text+fs.Substring(fs.LastIndexOf("/")+1);
					this.lblStatus.Text = "Downloading "+fs.Substring(fs.LastIndexOf("/")+1) + "To "+LocalFile;
					progressBarDowload.Value ++;
					Application.DoEvents();
					if(!System.IO.File.Exists(LocalFile))
					{
						Uri uri = new Uri("ftp://" + fs);
						FtpClient mySession = (FtpClient)FtpClient.GetFtpClient(uri.Host);
						mySession.DownloadFile(uri, LocalFile);
					}
				}
			}
		}
	}
}
