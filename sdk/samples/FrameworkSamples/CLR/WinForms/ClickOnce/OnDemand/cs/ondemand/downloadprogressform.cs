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


#region Using directives

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

#endregion

namespace Microsoft.Samples.Windows.Forms.ClickOnce.OnDemand
{
	partial class DownloadProgressForm : System.Windows.Forms.Form
	{
		public DownloadProgressForm()
		{
			InitializeComponent();
		}

		public void SetProgress(int currentProgress, long currentBytes, long totalBytes, string currentState)
		{
			progressBar1.Value = currentProgress;
            totalBytesLabel.Text = totalBytes.ToString();
            currentBytesLabel.Text = currentBytes.ToString();
            currentStateLabel.Text = currentState;
        }

		public void SetProgressCompleted(bool updateSucceeded)
		{
			progressBar1.Value = progressBar1.Maximum;
            closeButton.Text = "Close";

			if (updateSucceeded)
			{
				Text = "Download Completed";
			}
			else
			{
				progressBar1.Value = progressBar1.Minimum;
				Text = "Application Download Failed";
			}
		}

		private void closeButton_Click(object sender, EventArgs e)
		{
			Close();
		}
	}
}
