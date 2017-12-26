/************************************* Module Header **************************************\
* Module Name:  MyFormRegion.cs
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
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Office = Microsoft.Office.Core;
using Outlook = Microsoft.Office.Interop.Outlook;
#endregion

namespace CSOutlookUIDesigner
{
    partial class MyFormRegion
    {
        #region Form Region Factory

        [Microsoft.Office.Tools.Outlook.FormRegionMessageClass(Microsoft.Office.Tools.Outlook.FormRegionMessageClassAttribute.Note)]
        [Microsoft.Office.Tools.Outlook.FormRegionName("CSOutlookUIDesigner.MyFormRegion")]
        public partial class MyFormRegionFactory
        {
            // Occurs before the form region is initialized.
            // To prevent the form region from appearing, set e.Cancel to true.
            // Use e.OutlookItem to get a reference to the current Outlook item.
            private void MyFormRegionFactory_FormRegionInitializing(object sender, Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs e)
            {
            }
        }

        #endregion

        // Occurs before the form region is displayed.
        // Use this.OutlookItem to get a reference to the current Outlook item.
        // Use this.OutlookFormRegion to get a reference to the form region.
        private void MyFormRegion_FormRegionShowing(object sender, System.EventArgs e)
        {
            Outlook.MailItem mail = this.OutlookItem as Outlook.MailItem;
            
            if (mail != null)
            {
                txtContent.Text= mail.HTMLBody;

                // If this is a new mail, we'll update the HTML view every 1 second.
                if (mail.EntryID == null)
                    tmr.Enabled = true;

                Marshal.ReleaseComObject(mail);
            }
        }

        // Occurs when the form region is closed.
        // Use this.OutlookItem to get a reference to the current Outlook item.
        // Use this.OutlookFormRegion to get a reference to the form region.
        private void MyFormRegion_FormRegionClosed(object sender, System.EventArgs e)
        {
        }

        private void tmr_Tick(object sender, EventArgs e)
        {
            // Get the mail item and update the HTML view.
            Outlook.MailItem mail = (Outlook.MailItem)this.OutlookItem;
            txtContent.Text = mail.HTMLBody;
            Marshal.ReleaseComObject(mail);
        }
    }
}
