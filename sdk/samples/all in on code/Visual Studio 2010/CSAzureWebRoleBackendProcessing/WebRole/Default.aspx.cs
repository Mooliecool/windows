/****************************** Module Header ******************************\
* Project Name:   CSAzureWebRoleBackendProcessing
* Module Name:    WebRole
* File Name:      Default.aspx.cs
* Copyright (c) Microsoft Corporation
*
* This page submits words to Table storage and displays result.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using CSAzureWebRoleBackendProcessing.Common;

namespace CSAzureWebRoleBackendProcessing.WebRole
{
    public partial class Default : System.Web.UI.Page
    {
        protected void btnProcess_Click(object sender, EventArgs e)
        {
            // Add a new record.
            WordEntry entry = new WordEntry() { Content = tbContent.Text};

            DataSources ds = new DataSources();
            ds.AddWordEntry(entry);
            ds.QueueMessage(String.Format("{0},{1}", entry.PartitionKey, entry.RowKey));
            
            tbContent.Text = string.Empty;
        }

        protected void Page_PreRender(object sender, EventArgs e)
        {
            // Display results.
            DataSources ds = new DataSources();
            dlResult.DataSource = ds.GetWordEntries();
            dlResult.DataBind();
        }
    }
}