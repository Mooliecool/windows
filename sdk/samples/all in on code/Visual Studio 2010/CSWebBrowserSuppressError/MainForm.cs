/****************************** Module Header ******************************\
* Module Name:  MainForm.cs
* Project:	    CSWebBrowserSuppressError
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
using System.Windows.Forms;

namespace CSWebBrowserSuppressError
{
    public partial class MainForm : Form
    {

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            // Initialize the properties ScriptErrorsSuppressed and 
            // HtmlElementErrorsSuppressed of the browser.
            this.wbcSample.ScriptErrorsSuppressed = chkSuppressAllDialog.Checked;
            this.wbcSample.HtmlElementErrorsSuppressed = chkSuppressHtmlElementError.Checked;

            // Add a handler to the NavigateError event of the web browser control.
            this.wbcSample.NavigateError += 
                new EventHandler<WebBrowserNavigateErrorEventArgs>(wbcSample_NavigateError);

            // Get the current value of DisableJITDebugger in the key 
            // HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\Main.
            this.chkSuppressJITDebugger.Checked = CSWebBrowserSuppressError.WebBrowserEx.JITDebuggerDisabled;
            this.chkSuppressJITDebugger.CheckedChanged += 
                new System.EventHandler(this.chkSuppressJITDebugger_CheckedChanged);
        }


        /// <summary>
        /// Handle btnNavigate_Click event.
        /// If the text of tbUrl is not empty, then navigate to the url, else navigate to
        /// the build-in Error.htm.
        /// </summary>
        private void btnNavigate_Click(object sender, EventArgs e)
        {
            try
            {
                if (!string.IsNullOrEmpty(tbUrl.Text.Trim()))
                {
                    wbcSample.Navigate(tbUrl.Text);
                }
                else
                {
                    wbcSample.Navigate(Environment.CurrentDirectory + @"\HTMLPages\Error.htm");
                }
            }
            catch (ArgumentException)
            {
                MessageBox.Show("Please make sure that the url is valid.");
            }
        }


        /// <summary>
        /// Enable or disable JITDebugger of the web browser if 
        /// chkSuppressJITDebugger.Checked changed.
        /// </summary>
        private void chkSuppressJITDebugger_CheckedChanged(object sender, EventArgs e)
        {
            WebBrowserEx.JITDebuggerDisabled = chkSuppressJITDebugger.Checked;
            MessageBox.Show("To disable/enable script debugger, you must restart the application.");
        }

        /// <summary>
        /// Set the property HtmlElementErrorsSuppressed of the web browser if 
        /// chkSuppressHtmlElementError.Checked changed.
        /// </summary>
        private void chkSuppressHtmlElementError_CheckedChanged(object sender, EventArgs e)
        {
            wbcSample.HtmlElementErrorsSuppressed = chkSuppressHtmlElementError.Checked;
        }

        /// <summary>
        /// Set the property ScriptErrorsSuppressed of the web browser if 
        /// chkSuppressAllDialog.Checked changed.
        /// </summary>
        private void chkSuppressAllDialog_CheckedChanged(object sender, EventArgs e)
        {
            wbcSample.ScriptErrorsSuppressed = chkSuppressAllDialog.Checked;
        }

        /// <summary>
        /// Handle the Navigation Error.
        /// </summary>
        void wbcSample_NavigateError(object sender, WebBrowserNavigateErrorEventArgs e)
        {
            // Navigate to the build-in 404.htm if the http status code is 404.
            if (chkSuppressNavigationError.Checked && e.StatusCode == 404)
            {
                wbcSample.Navigate(string.Format(@"{0}\HTMLPages\404.htm", 
                    Environment.CurrentDirectory));
            }
        }

    }
}
