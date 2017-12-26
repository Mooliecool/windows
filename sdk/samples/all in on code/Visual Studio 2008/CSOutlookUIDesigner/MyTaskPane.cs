/************************************* Module Header **************************************\
* Module Name:  MyTaskPane.cs
* Project:      CSOutlookUIDesigner
* Copyright (c) Microsoft Corporation.
* 
* The CSOutlookUIDesigner sample demonstrates how to use the Ribbon Designer to create
* customized Ribbon for Outlook 2007 inspectors.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Microsoft.Office.Tools;
using Outlook = Microsoft.Office.Interop.Outlook;
using Word = Microsoft.Office.Interop.Word;
#endregion

namespace CSOutlookUIDesigner
{
    public partial class MyTaskPane : UserControl
    {
        public MyTaskPane()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Gets or sets the parent CTP object.
        /// </summary>
        public CustomTaskPane ParentCustomTaskPane
        {
            get;
            set;
        }

        private void txtAddress_TextChanged(object sender, EventArgs e)
        {
            // Updates the Go button state.
            btnGo.Enabled = txtAddress.Text.Trim().Length > 0;
        }

        private void txtAddress_KeyDown(object sender, KeyEventArgs e)
        {
            // If Enter is pressed, do the navigation by clicking
            // the Go button.
            if (e.KeyCode == Keys.Enter && btnGo.Enabled)
                btnGo.PerformClick();
        }

        private void btnGo_Click(object sender, EventArgs e)
        {
            // Start navigation.
            WebBrowser.Navigate(txtAddress.Text);
        }

        private void WebBrowser_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            // Update the address bar.
            txtAddress.Text = WebBrowser.Document.Url.ToString();
        }

        private void btnPaste_Click(object sender, EventArgs e)
        {
            // Get current inspector.
            Outlook.Inspector inspector = this.ParentCustomTaskPane.Window as Outlook.Inspector;

            if (inspector != null)
            {
                // Copy the selected content into clipboard.
                WebBrowser.Document.ExecCommand("Copy", false, null);
                // Get the document reference.
                Word.Document doc = inspector.WordEditor as Word.Document;
                // Paste the content into the document.
                doc.Application.Selection.Paste();
            }
        }
    }
}
