/************************************* Module Header **************************************\
* Module Name:  ThisAddIn.cs
* Project:      CSVstoVBAInterop
* Copyright (c) Microsoft Corporation.
* 
* The CSVstoVBAInterop project demonstrates how to interop with VBA project object model in 
* VSTO projects. Including how to programmatically add Macros (or VBA UDF in Excel) into an
* Office document; how to call Macros / VBA UDFs from VSTO code; and how to call VSTO code
* from VBA code. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 5/24/2009 1:00 PM Wang Jie Created
\******************************************************************************************/

#region Using directives
using System;
using System.Text;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using Excel = Microsoft.Office.Interop.Excel;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Tools.Excel;
using Microsoft.Office.Tools.Excel.Extensions;
#endregion


namespace CSVstoVBAInterop
{
    public partial class ThisAddIn
    {
        /// <summary>
        /// The COM Add-in automation service object.
        /// </summary>
        private VstoClass vstoClass = null;

        private void ThisAddIn_Startup(object sender, System.EventArgs e)
        {
        }

        private void ThisAddIn_Shutdown(object sender, System.EventArgs e)
        {
        }

        /// <summary>
        /// Returns an instance of <see cref="VstoClass"/> that can be used by the sample
        /// Excel VBA macro included in this project.
        /// </summary>
        protected override object RequestComAddInAutomationService()
        {
            if (vstoClass == null)
            {
                vstoClass = new VstoClass();
            }

            return vstoClass;
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
