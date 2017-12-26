/************************************* Module Header **************************************\
* Module Name:  MyRibbon.cs
* Project:      CSOutlookUIDesigner
* Copyright (c) Microsoft Corporation.
* 
* The CSOutlookRibbonDesigner sample demonstrates how to use the Ribbon Designer to create
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
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Windows.Forms;
using Microsoft.Office.Tools;
using Microsoft.Office.Tools.Ribbon;
using Outlook = Microsoft.Office.Interop.Outlook;
#endregion

namespace CSOutlookUIDesigner
{
    public partial class MyRibbon : OfficeRibbon
    {
        public MyRibbon()
        {
            InitializeComponent();
        }

        private void MyRibbon_Load(object sender, RibbonUIEventArgs e)
        {
            // Trying to get current Inspector object
            Outlook.Inspector inspector = this.Context as Outlook.Inspector;

            if (inspector != null)
            {
                if (inspector.CurrentItem is Outlook.MailItem)
                {
                    // MailItem object, set the Mail group on second tab visible
                    grpMail.Visible = true;

                    Outlook.MailItem item = (Outlook.MailItem)inspector.CurrentItem;
                    // If the EntryID is null, this is a new mail, otherwise
                    // this should be in Reading mode.
                    lblMailMode.Label = string.Format("Current Mode: {0}",
                        item.EntryID == null ? "Drafting" : "Reading");
                    Marshal.ReleaseComObject(item);
                }
                else if (inspector.CurrentItem is Outlook.AppointmentItem)
                {
                    // Appointment object, set the Appointment group on second tab visible
                    grpAppointmentItem.Visible = true;
                }
                else if (inspector.CurrentItem is Outlook.TaskItem)
                {
                    // Task object, set the Task group on second tab visible
                    grpTaskItem.Visible = true;
                }
                else if (inspector.CurrentItem is Outlook.ContactItem)
                {
                    // Contact object, set the Contact group on second tab visible
                    grpContactItem.Visible = true;
                }

                // Release our reference to the inspector
                Marshal.ReleaseComObject(inspector);
                inspector = null;
                GC.Collect();
            }
        }

        private void cboMyList_ItemsLoading(object sender, RibbonControlEventArgs e)
        {
            // Add ComboBox items via code
            RibbonDropDownItem item = new RibbonDropDownItem();
            item.Label = string.Format("Item Added via code at time {0}", DateTime.Now);
            cboMyList.Items.Add(item);
        }

        private void btnWeb_Click(object sender, RibbonControlEventArgs e)
        {
            // Go through the CustomTaskPanes and find out the one on current inspector
            foreach (CustomTaskPane ctp in Globals.ThisAddIn.CustomTaskPanes)
            {
                if (ctp.Window == this.Context)
                {
                    // Make the CustomTaskPane visible.
                    ctp.Visible = true;
                }
            }
        }

        private void tbSecondTab_Click(object sender, RibbonControlEventArgs e)
        {
            // Show / Hide the second customized tab on ribbon
            mySecondTab.Visible = tbSecondTab.Checked;
        }

        private void chkShowGroup_Click(object sender, RibbonControlEventArgs e)
        {
            // Show / Hide the third group on first customized tab
            grpThree.Visible = chkShowGroup.Checked;
        }

        private void splitButton_Click(object sender, RibbonControlEventArgs e)
        {
            // SplitButton itself clicked
            MessageBox.Show("SplitButton itself clicked",
                            "Event",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Information);
                            
        }

        private void btnAlign_Click(object sender, RibbonControlEventArgs e)
        {
            // SplitButton item clicked
            // Get the reference to the clicked button
            RibbonButton rb = sender as RibbonButton;

            if (rb != null)
                MessageBox.Show(rb.Label,
                                "SplitButton Item Clicked",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
        }

        private void myFirstGroup_DialogLauncherClick(object sender, RibbonControlEventArgs e)
        {
            // Dialog launcher clicked
            // Open a dialog...
            if (colorDlg.ShowDialog() == DialogResult.OK)
                MessageBox.Show(colorDlg.Color.ToString(),
                                "Selected Color",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
        }

        private void txtEdit_TextChanged(object sender, RibbonControlEventArgs e)
        {
            // Edit control text changed,
            // set new text to label control (if not empty)
            if (txtEdit.Text.Trim().Length > 0)
                lblSample.Label = txtEdit.Text;
        }
    }
}
