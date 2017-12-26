/****************************** Module Header ******************************\ 
* Module Name:  ThisAddIn.cs
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
using System.Text;
using System.Xml.Linq;
using Outlook = Microsoft.Office.Interop.Outlook;
using Office = Microsoft.Office.Core;
#endregion


namespace CSOutlookImportedFormRegion
{
    public partial class ThisAddIn
    {
        private Office.CommandBarButton btnCreateCustomItem;
        internal Outlook.MailItem mail;

        private void ThisAddIn_Startup(object sender, System.EventArgs e)
        {
            //Create a new button on Explorer to show the form region.
            Office.CommandBar CustomBar = Application.ActiveExplorer().CommandBars.Add("CSImportedFormRegion", Office.MsoBarPosition.msoBarTop, false, true);
            CustomBar.Visible = true;
            btnCreateCustomItem = CustomBar.Controls.Add(Office.MsoControlType.msoControlButton, missing, missing, missing, true) as Office.CommandBarButton;
            btnCreateCustomItem.Caption = "Create A Custom Item";
            btnCreateCustomItem.Click += new Microsoft.Office.Core._CommandBarButtonEvents_ClickEventHandler(btnCreateCustomItem_Click);
        }
       
        private void btnCreateCustomItem_Click(Microsoft.Office.Core.CommandBarButton Ctrl, ref bool CancelDefault)
        {
            mail = Application.CreateItem(Microsoft.Office.Interop.Outlook.OlItemType.olMailItem) as Outlook.MailItem;
            mail.MessageClass = "IPM.Note.ImportedFormRegion";
            mail.Display(missing);
        }

        private void ThisAddIn_Shutdown(object sender, System.EventArgs e)
        {

        }

        #region VSTO generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InternalStartup()
        {
            this.Startup += new System.EventHandler(ThisAddIn_Startup);
            this.Shutdown += new System.EventHandler(ThisAddIn_Shutdown);
        }

        #endregion
    }
}
