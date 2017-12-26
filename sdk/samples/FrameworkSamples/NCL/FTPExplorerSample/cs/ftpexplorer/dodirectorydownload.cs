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
using System.IO;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Microsoft.Samples.FTPExplorer
{
	/// <summary>
	/// Summary description for DoDirectoryDownLoad.
	/// </summary>
	public class DoDirectoryDownLoad : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label lblStatus;
		private System.Windows.Forms.Button cmdCancel;
		private System.Windows.Forms.ProgressBar progressBarDowload;
		private System.Windows.Forms.TextBox txtLocaldir;
		private System.Windows.Forms.Button cmdDownload;
		protected System.Windows.Forms.Label lblDownLoad;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		private Uri _directoryToDownload;
        private FtpClient _ftpSession;

        public DoDirectoryDownLoad(Uri directoryPath)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
            _directoryToDownload = directoryPath;
            _ftpSession = FtpClient.GetFtpClient(_directoryToDownload.Host);
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
            this.lblStatus = new System.Windows.Forms.Label();
            this.cmdCancel = new System.Windows.Forms.Button();
            this.progressBarDowload = new System.Windows.Forms.ProgressBar();
            this.txtLocaldir = new System.Windows.Forms.TextBox();
            this.cmdDownload = new System.Windows.Forms.Button();
            this.lblDownLoad = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // lblStatus
            // 
            this.lblStatus.Location = new System.Drawing.Point(32, 56);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(376, 16);
            this.lblStatus.TabIndex = 11;
            // 
            // cmdCancel
            // 
            this.cmdCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cmdCancel.Location = new System.Drawing.Point(136, 120);
            this.cmdCancel.Name = "cmdCancel";
            this.cmdCancel.TabIndex = 10;
            this.cmdCancel.Text = "Cancel";
            // 
            // progressBarDowload
            // 
            this.progressBarDowload.Location = new System.Drawing.Point(24, 80);
            this.progressBarDowload.Name = "progressBarDowload";
            this.progressBarDowload.Size = new System.Drawing.Size(392, 8);
            this.progressBarDowload.TabIndex = 9;
            // 
            // txtLocaldir
            // 
            this.txtLocaldir.Location = new System.Drawing.Point(136, 16);
            this.txtLocaldir.Name = "txtLocaldir";
            this.txtLocaldir.Size = new System.Drawing.Size(264, 20);
            this.txtLocaldir.TabIndex = 8;
            this.txtLocaldir.Text = Directory.GetCurrentDirectory();
            // 
            // cmdDownload
            // 
            this.cmdDownload.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.cmdDownload.Location = new System.Drawing.Point(224, 120);
            this.cmdDownload.Name = "cmdDownload";
            this.cmdDownload.Size = new System.Drawing.Size(144, 23);
            this.cmdDownload.TabIndex = 7;
            this.cmdDownload.Text = "Start DownLoad";
            this.cmdDownload.Click += new System.EventHandler(this.cmdDownload_Click);
            // 
            // lblDownLoad
            // 
            this.lblDownLoad.Location = new System.Drawing.Point(24, 16);
            this.lblDownLoad.Name = "lblDownLoad";
            this.lblDownLoad.Size = new System.Drawing.Size(96, 16);
            this.lblDownLoad.TabIndex = 6;
            this.lblDownLoad.Text = "Local Directory";
            // 
            // DoDirectoryDownLoad
            // 
            this.ClientSize = new System.Drawing.Size(432, 158);
            this.ControlBox = false;
            this.Controls.Add(this.lblStatus);
            this.Controls.Add(this.cmdCancel);
            this.Controls.Add(this.progressBarDowload);
            this.Controls.Add(this.txtLocaldir);
            this.Controls.Add(this.cmdDownload);
            this.Controls.Add(this.lblDownLoad);
            this.Name = "DoDirectoryDownLoad";
            this.Text = "DoDirectoryDownLoad";
            this.ResumeLayout(false);
            this.PerformLayout();

        }
		#endregion

		private void cmdDownload_Click(object sender, System.EventArgs e)
		{
            if(Directory.Exists(this.txtLocaldir.Text))
            {
			    if(this.txtLocaldir.Text[this.txtLocaldir.Text.Length-1] != '\\')
				    this.txtLocaldir.Text+="\\";
                downloadDirectory(_directoryToDownload.AbsolutePath,this.txtLocaldir.Text,1);			
            }
            else
            {                
                MessageBox.Show("Directory "+this.txtLocaldir.Text + " doesn't exists","Error",MessageBoxButtons.OK,MessageBoxIcon.Error);
            }
		}

		private void downloadDirectory(string serverDirectory,string localDirectory,int depth)
		{
            downloadFiles(serverDirectory,localDirectory);
			if(depth < 1)
			{
                Uri uri = new Uri(serverDirectory );
                foreach (FileStruct fs in _ftpSession.GetDirectoryList(uri.AbsolutePath))
                {
					string newServerDirectory = "";
					string newLocalDirectory="";
                    newServerDirectory = appendSegmentOnDirectoryPath(serverDirectory, fs.Name);
                    newLocalDirectory = appendSegmentOnDirectoryPath(localDirectory, fs.Name);
					int dirDepth = depth+1;
					downloadDirectory(newServerDirectory,newLocalDirectory,dirDepth);							
				}				
			}
			return;
		}		
		private void downloadFiles(string serverDirectory,string localDirectory)
		{	
			if (!Directory.Exists (localDirectory))
			{
				Directory.CreateDirectory(localDirectory);
			}
            Uri uri = new Uri("ftp://" + _directoryToDownload.Host+ "/" + serverDirectory + "/");
            foreach (FileStruct fs in _ftpSession.GetFileList(uri.AbsolutePath))
            {			
				string localFile = localDirectory+"\\"+fs.Name;
				if(!File.Exists(localFile))
				{
                       _ftpSession.DownloadFile(new Uri("ftp://" + _directoryToDownload.Host + "/" + serverDirectory + "/" + fs.Name), localFile);
               }								
			}
		}

        private string appendSegmentOnDirectoryPath(string directoryPath,
                                            string segmentName)
        {
            if (directoryPath.Length > 1 
                && directoryPath[directoryPath.Length - 1] == '\\')
            {
                directoryPath = directoryPath + segmentName;
            }
            else
            {
                directoryPath = directoryPath + "\\" + segmentName;
            }
            return directoryPath;
        }
    }
}
