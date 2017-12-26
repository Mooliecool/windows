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
using System.Net;

namespace Microsoft.Samples.WebClientProgress
{
    /// <summary>
    /// WebClientProgress demonstrates how to download a file with a progress
    /// indicator without blocking any of the user interface.
    /// </summary>
    public partial class WebClientProgressForm : System.Windows.Forms.Form
    {
        private bool isBusy;
        private WebClient client;

        public WebClientProgressForm()
        {
            InitializeComponent();
            client = new WebClient();

            // Wire up the download complete event handler.  
            // The operation will either complete successfully or with an 
            // error code.
            client.DownloadFileCompleted += client_DownloadFileCompleted;
            // Wire up the download progress event handler.
            client.DownloadProgressChanged += client_DownloadProgressChanged;
        }

        private void btnDownload_Click(object sender, System.EventArgs e)
        {
            // If an operation is pending, then the user has clicked cancel.
            if (isBusy)
            {
                client.CancelAsync();
                isBusy = false;
                this.downloadButton.Text = "Download";
            }
            // Otherwise go ahead and start the download
            else
            {
                try {
                    Uri uri = new Uri(urlTextBox.Text);
                    this.downloadProgressBar.Value = 0;
                    client.DownloadFileAsync(uri, "localfile.htm");
                    this.downloadButton.Text = "Cancel";
                    isBusy = true;
                }
                catch (UriFormatException ex) {
                    MessageBox.Show(ex.Message);
                }
            }
        }

        // Displays a message indicating that the download completed 
        // successfully, or if there was an error it will be displayed instead.
        private void client_DownloadFileCompleted(
            object sender, AsyncCompletedEventArgs e)
        {
            isBusy = false;
            this.downloadButton.Text = "Download";
            if (e.Error == null)
                MessageBox.Show("Download Complete");
            else
                MessageBox.Show("Download Not Complete: " + e.Error.Message);
        }

        // Updates the progress indicator with the latest progress
        private void client_DownloadProgressChanged(
            object sender, DownloadProgressChangedEventArgs e)
        {
            this.downloadProgressBar.Value = e.ProgressPercentage;
        }
    }
}

