/************************************* Module Header **************************************\
* Module Name:	ThisAddIn.cs
* Project:		CSOutlookUIDesigner
* Copyright (c) Microsoft Corporation.
* 
* The CSOutlookUIDesigner sample demonstrates how to use the Ribbon Designer to create
* customized Ribbon for Outlook 2007 inspectors.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 3/3/2009 11:40 AM Wang Jie Created
\******************************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Outlook = Microsoft.Office.Interop.Outlook;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Tools;
#endregion

namespace CSOutlookUIDesigner
{
    public partial class ThisAddIn
    {
        private void ThisAddIn_Startup(object sender, System.EventArgs e)
        {
            // Register for the NewInspector event
            this.Application.Inspectors.NewInspector += 
                new Outlook.InspectorsEvents_NewInspectorEventHandler(Inspectors_NewInspector);
        }

        private void Inspectors_NewInspector(Outlook.Inspector inspector)
        {
            // We will add our custom task pane to the new inspector.
            if (inspector.CurrentItem is Outlook.MailItem ||
                inspector.CurrentItem is Outlook.AppointmentItem ||
                inspector.CurrentItem is Outlook.TaskItem ||
                inspector.CurrentItem is Outlook.ContactItem)
            {
                // Create the custom task pane and make it initially invisible.
                MyTaskPane mtp = new MyTaskPane();
                CustomTaskPane ctp = this.CustomTaskPanes.Add(mtp, "Browser", inspector);
                ctp.Visible = false;
                ctp.DockPosition = Office.MsoCTPDockPosition.msoCTPDockPositionBottom;
                mtp.ParentCustomTaskPane = ctp;
            }
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
