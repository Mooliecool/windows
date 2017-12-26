/****************************** Module Header ******************************\
 * Module Name:  MainForm.cs
 * Project:      CSWebBrowserLoadComplete
 * Copyright (c) Microsoft Corporation.
 * 
 * This is the main form of this application. It is used to initialize the UI and 
 * handle the events.
 * 
 * This source is subject to the Microsoft Public License.
 * See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 * All other rights reserved.
 * 
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Security.Permissions;
using System.Windows.Forms;

namespace CSWebBrowserLoadComplete
{
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    [PermissionSetAttribute(SecurityAction.InheritanceDemand, Name = "FullTrust")]
    public partial class MainForm : Form
    {

        // The count of how many times DocumentCompleted event were fired.
        int documentCompletedCount = 0;

        // The count of how many times LoadCompleted event were fired.
        int loadCompletedCount = 0;

        public MainForm()
        {
            InitializeComponent();

            // Register the events of the System.Windows.Forms.WebBrowser.
            webEx.DocumentCompleted += webEx_DocumentCompleted;           
            webEx.Navigating += webEx_Navigating;
            webEx.Navigated += webEx_Navigated;

            // Register the events of the WebBrowserEx.
            webEx.StartNavigating += webEx_StartNavigating;
            webEx.LoadCompleted += webEx_LoadCompleted;
            
            this.tbURL.Text = string.Format("{0}\\Resource\\FramesPage.htm",
                Environment.CurrentDirectory);

        }

        /// <summary>
        /// Navigate to an URL.
        /// </summary>
        private void btnGo_Click(object sender, EventArgs e)
        {
            try
            {
                Uri url = new Uri(tbURL.Text);
                webEx.Navigate(url);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void webEx_Navigating(object sender, WebBrowserNavigatingEventArgs e)
        {
            DisplayStatus("Navigating : " + e.Url);
        }

        private void webEx_Navigated(object sender, WebBrowserNavigatedEventArgs e)
        {
            DisplayStatus("Navigated : " + e.Url);
        }

        private void webEx_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            documentCompletedCount++;
            DisplayStatus("Document Completed : " + e.Url);
        }

        private void webEx_LoadCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            loadCompletedCount++;
            DisplayStatus("Load Completed : " + e.Url);
        }


        private void webEx_StartNavigating(object sender, WebBrowserNavigatingEventArgs e)
        {
            documentCompletedCount = 0;
            loadCompletedCount = 0;
            DisplayStatus("Start Navigating : " + e.Url);
        }

        /// <summary>
        /// Display the message.
        /// </summary>
        private void DisplayStatus(string msg)
        {
            DateTime now = DateTime.Now;

            lstActivities.Items.Insert(0,
                string.Format("{0:HH:mm:ss}:{1:000} {2}", 
                now,now.Millisecond,msg));

            lstActivities.SelectedIndex = 0;

            this.lbStatus.Text = string.Format(
                "DocumentCompleted:{0} LoadCompleted:{1}",
                documentCompletedCount, loadCompletedCount);
        }

    }
}
