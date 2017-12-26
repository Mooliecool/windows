/************************************* Module Header **************************************\
* Module Name:  MainForm.cs
* Project:      CSVstoServerDocument
* Copyright (c) Microsoft Corporation.
* 
* The CSVstoServerDocument project demonstrates how to use the ServerDocument
* class to extract information from a VSTO customized Word document or Excel
* Workbook; and also how to programmatically add / remove VSTO customizations.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 4/13/2009 11:00 AM Wang Jie Created
\******************************************************************************************/


#region using directives
using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SERuntime = Microsoft.VisualStudio.Tools.Applications.Runtime;
using V3Runtime = Microsoft.VisualStudio.Tools.Applications;
#endregion

namespace CSVstoServerDocument
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        #region Control event handlers

        private void btnSelectFile_Click(object sender, EventArgs e)
        {
            ofd.FileName = string.Empty;

            // Select an Word document / Excel workbook.
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                txtFileName.Text = ofd.FileName;
            }
        }

        private void txtFileName_TextChanged(object sender, EventArgs e)
        {
            toolTip.SetToolTip(txtFileName, txtFileName.Text);
            // New file selected, show doc info.
            ShowDocInfo(txtFileName.Text);
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            AddCustomization(txtFileName.Text);
            ShowDocInfo(txtFileName.Text);
        }

        private void btnRemove_Click(object sender, EventArgs e)
        {
            RemoveCustomization(txtFileName.Text);
            ShowDocInfo(txtFileName.Text);
        }

        #endregion

        #region Document Info

        private void ShowDocInfo(string docPath)
        {
            // Get the version of the customization associated with the document.
            int rtVer = V3Runtime.ServerDocument.GetCustomizationVersion(docPath);

            // Update UI elements
            lstDocInfo.Items.Clear();
            btnAdd.Enabled = rtVer == 0;
            btnRemove.Enabled = !btnAdd.Enabled;
            txtAssembly.Text = string.Empty;
            txtManifest.Text = string.Empty;
            txtAssembly.Enabled = btnAdd.Enabled && !Is2007Doc(docPath);
            lblAssembly.Enabled = txtAssembly.Enabled;
            txtManifest.Enabled = btnAdd.Enabled;
            lblManifest.Enabled = txtManifest.Enabled;

            switch (rtVer)
            {
                case 0:
                    lstDocInfo.Items.Add("No customization in this document");
                    break;

                case 1:
                    lstDocInfo.Items.Add("The document has a customization that was " +
                        "created by using Visual Studio Tools for Office, Version 2003.");
                    break;

                case 2:
                    ShowDocInfoSE(docPath);
                    break;

                case 3:
                    ShowDocInfoV3(docPath);
                    break;

                default:
                    lstDocInfo.Items.Add(string.Format("Version {0} is not supported in this demo.", rtVer));
                    break;
            }
        }

        private void ShowDocInfoSE(string docPath)
        {
            bool cacheEnabled = SERuntime.ServerDocument.IsCacheEnabled(docPath);

            using (SERuntime.ServerDocument doc = new SERuntime.ServerDocument(docPath))
            {
                lstDocInfo.Items.Add("The document has a customization that was created by " +
                    "using the Visual Studio 2005 Tools for Office Second Edition runtime.");
                lstDocInfo.Items.Add("This is the version of the runtime that is used to create " +
                    "customizations for Microsoft Office 2003.");

                // Deployment manifest info
                lstDocInfo.Items.Add(string.Empty);
                lstDocInfo.Items.Add("This URL of the deployment manifest is: " + doc.AppManifest.DeployManifestPath);
                lstDocInfo.Items.Add("AppManifest Idenetiy Name: " + doc.AppManifest.Identity.Name);
                lstDocInfo.Items.Add("AppManifest Idenetiy PublicKeyToken: " + doc.AppManifest.Identity.PublicKeyToken.ToString());
                lstDocInfo.Items.Add("AppManifest Idenetiy Version: " + doc.AppManifest.Identity.Version.ToString());

                // Cached data
                lstDocInfo.Items.Add(string.Empty);
                if (cacheEnabled)
                {
                    lstDocInfo.Items.Add("Data cache enabled");
                    foreach (SERuntime.CachedDataHostItem hostItem in doc.CachedData.HostItems)
                    {
                        lstDocInfo.Items.Add("Namespace and class: " + hostItem.Id);
                        foreach (SERuntime.CachedDataItem dataItem in hostItem.CachedData)
                        {
                            lstDocInfo.Items.Add("     Data item: " + dataItem.Id);
                        }
                    }
                }
                else
                {
                    lstDocInfo.Items.Add("Data cache not enabled");
                }
            }
        }

        private void ShowDocInfoV3(string docPath)
        {
            bool cacheEnabled = V3Runtime.ServerDocument.IsCacheEnabled(docPath);

            using (V3Runtime.ServerDocument doc = new V3Runtime.ServerDocument(docPath))
            {
                lstDocInfo.Items.Add("The document has a customization that was created by " +
                    "using the Microsoft Visual Studio Tools for the Microsoft Office system " +
                    "(version 3.0 Runtime).");
                lstDocInfo.Items.Add("This is the version of the runtime that is used to create " +
                    "customizations for the 2007 Microsoft Office system.");

                // Deployment manifest & solution ID
                lstDocInfo.Items.Add(string.Empty);
                lstDocInfo.Items.Add("The URL of the deployment manifest is: " + doc.DeploymentManifestUrl.ToString());
                lstDocInfo.Items.Add("The Solution ID is: " + doc.SolutionId.ToString());

                // Cached data
                lstDocInfo.Items.Add(string.Empty);
                if (cacheEnabled)
                {
                    lstDocInfo.Items.Add("Data cache enabled");
                    foreach (V3Runtime.CachedDataHostItem hostItem in doc.CachedData.HostItems)
                    {
                        lstDocInfo.Items.Add("Namespace and class: " + hostItem.Id);
                        foreach (V3Runtime.CachedDataItem dataItem in hostItem.CachedData)
                        {
                            lstDocInfo.Items.Add("     Data item: " + dataItem.Id);
                        }
                    }
                }
                else
                {
                    lstDocInfo.Items.Add("Data cache not enabled");
                }
            }
        }

        private void AddCustomization(string docPath)
        {
            try
            {
                // Call the corresponding ServerDocument class to add the customization.
                if (Is2007Doc(docPath))
                {
                    V3Runtime.ServerDocument.AddCustomization(docPath, new Uri(txtManifest.Text));
                }
                else
                {
                    SERuntime.ServerDocument.AddCustomization(docPath, txtAssembly.Text, txtManifest.Text, "1.0.0.0", true);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message,
                    this.Text,
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
            }
        }

        private void RemoveCustomization(string docPath)
        {
            // Get the version of the customization associated with the document.
            int rtVer = V3Runtime.ServerDocument.GetCustomizationVersion(docPath);

            // Call the corresponding ServerDocument class to remove the customization.
            switch (rtVer)
            {
                case 2:
                    SERuntime.ServerDocument.RemoveCustomization(docPath);
                    break;

                case 3:
                    V3Runtime.ServerDocument.RemoveCustomization(docPath);
                    break;

                default:
                    MessageBox.Show(string.Format("Version {0} is not supported in this demo.", rtVer),
                        this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                    break;
            }
        }

        private bool Is2007Doc(string docPath)
        {
            // How to get the document version correctly is not in the scope of this demo.
            // Here we just use file extension to make a fast and easy implementation.
            string ext = Path.GetExtension(docPath).ToLowerInvariant();

            return ext == ".docx" || ext == ".dotx" || ext == ".docm" || ext == ".dotm" ||
                   ext == ".xlsx" || ext == ".xltx" || ext == ".xlsm" || ext == ".xltm";
        }

        #endregion
    }
}
