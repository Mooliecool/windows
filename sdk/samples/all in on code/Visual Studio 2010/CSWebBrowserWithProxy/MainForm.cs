/****************************** Module Header ******************************\
 Module Name:  MainForm.cs
 Project:      CSWebBrowserWithProxy
 Copyright (c) Microsoft Corporation.
 
 This is the main form of this application. It is used to initialize the UI and 
 handle the events.
 
 This source is subject to the Microsoft Public License.
 See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 All other rights reserved.
 
 THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


using System;
using System.Windows.Forms;
using System.Security.Permissions;

namespace CSWebBrowserWithProxy
{

    public partial class MainForm : Form
    {
        // Get the current proxy.
        InternetProxy CurrentProxy
        {
            get
            {
                if (radIEProxy.Checked)
                {
                    return InternetProxy.NoProxy;
                }
                else
                {
                    return cmbProxy.SelectedItem as InternetProxy;
                }
            }
        }

        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {

            // Data bind cmbProxy to display the ProxyList.
            cmbProxy.DisplayMember = "ProxyName";
            cmbProxy.DataSource = InternetProxy.ProxyList;
            cmbProxy.SelectedIndex = 0;

            wbcSample.StatusTextChanged += new EventHandler(wbcSample_StatusTextChanged);

        }

        /// <summary>
        /// Handle btnNavigate_Click event.
        /// The method Goto of WebBrowserControl class wraps the Navigate method of
        /// WebBrowser class to set the Proxy-Authorization header if needed.
        /// </summary>
        private void btnNavigate_Click(object sender, EventArgs e)
        {
            try
            {
                wbcSample.Proxy = CurrentProxy;
                wbcSample.Goto(tbUrl.Text);
            }
            catch (ArgumentException)
            {
                MessageBox.Show("Please maske sure that the url is valid.");
            }
        }

        private void wbcSample_ProgressChanged(object sender, WebBrowserProgressChangedEventArgs e)
        {
            prgBrowserProcess.Value = (int)e.CurrentProgress;
            wbcSample.StatusTextChanged += new EventHandler(wbcSample_StatusTextChanged);
        }

        void wbcSample_StatusTextChanged(object sender, EventArgs e)
        {
            lbStatus.Text = wbcSample.StatusText;
        }
    }
}
