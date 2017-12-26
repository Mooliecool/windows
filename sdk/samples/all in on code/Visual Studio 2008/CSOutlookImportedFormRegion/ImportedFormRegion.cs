/****************************** Module Header ******************************\ 
* Module Name:  ImportedFormRegion.cs
* Project:      CSOutlookImportedFormRegion
* Copyright (c) Microsoft Corporation.
* 
* The CSOutlookImportedFormRegion provides the examples on how to built a
* Outlook Custom Form with Outlook style look&feel.
 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Resources;
using System.Text;
using Office = Microsoft.Office.Core;
using Outlook = Microsoft.Office.Interop.Outlook;
using System.Windows.Forms;
#endregion


namespace CSOutlookImportedFormRegion
{
    public partial class ImportedFormRegion
    {
        #region Form Region Factory

        [Microsoft.Office.Tools.Outlook.FormRegionMessageClass("IPM.Note.ImportedFormRegion")]
        [Microsoft.Office.Tools.Outlook.FormRegionName("CSOutlookImportedFormRegion.ImportedFormRegion")]
        public partial class ImportedFormRegionFactory
        {
            private void InitializeManifest()
            {
                ResourceManager resources = new ResourceManager(typeof(ImportedFormRegion));
                this.Manifest.FormRegionType = Microsoft.Office.Tools.Outlook.FormRegionType.ReplaceAll;
                this.Manifest.Title = resources.GetString("Title");
                this.Manifest.FormRegionName = resources.GetString("FormRegionName");
                this.Manifest.Description = resources.GetString("Description");
                this.Manifest.ShowInspectorCompose = true;
                this.Manifest.ShowInspectorRead = true;
                this.Manifest.ShowReadingPane = true;

            }

            // Occurs before the form region is initialized.
            // To prevent the form region from appearing, set e.Cancel to true.
            // Use e.OutlookItem to get a reference to the current Outlook item.
            private void ImportedFormRegionFactory_FormRegionInitializing(object sender, Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs e)
            {
            }
        }

        #endregion

        // Occurs before the form region is displayed.
        // Use this.OutlookItem to get a reference to the current Outlook item.
        // Use this.OutlookFormRegion to get a reference to the form region.


        private void ImportedFormRegion_FormRegionShowing(object sender, System.EventArgs e)
        {
            btnDisable.Click += new Outlook.OlkCommandButtonEvents_ClickEventHandler(btnDisable_Click);
            btnAdd.Click += new Outlook.OlkCommandButtonEvents_ClickEventHandler(btnAdd_Click);
            btnRemove.Click += new Outlook.OlkCommandButtonEvents_ClickEventHandler(btnRemove_Click);
            btnSend.Click += new Outlook.OlkCommandButtonEvents_ClickEventHandler(btnSend_Click);
        }

        #region Event Handlers

        private void btnSend_Click()
        {
            Globals.ThisAddIn.mail.Send();
        }

        private void btnRemove_Click()
        {
            if (comboBox1.ListIndex == -1)
            {
                MessageBox.Show("Please choose an item first.");
                return;
            }
            comboBox1.RemoveItem(comboBox1.ListIndex);
        }

        private void btnAdd_Click()
        {
            int i = comboBox1.ListCount;
            comboBox1.AddItem("Item" + i.ToString(), i);
        }

        private void btnDisable_Click()
        {
            if (btnTo.Enabled)
            {
                btnDisable.Caption = "Enable Recipient Box";
                btnTo.Enabled = false;
                _RecipientControl2.Enabled = 0;
            }
            else
            {
                btnDisable.Caption = "Disable Recipient Box";
                btnTo.Enabled = true;
                _RecipientControl2.Enabled = -1;
            }
        }

        #endregion

        // Occurs when the form region is closed.
        // Use this.OutlookItem to get a reference to the current Outlook item.
        // Use this.OutlookFormRegion to get a reference to the form region.
        private void ImportedFormRegion_FormRegionClosed(object sender, System.EventArgs e)
        {
        }
    }
}
