/************************************* Module Header **************************************\
* Module Name:	ThisAddIn.cs
* Project:		CSVstoGetWrapperObject
* Copyright (c) Microsoft Corporation.
* 
* The CSVstoGetWrapperObject project demonstrates how to get a VSTO wrapper
* object from an existing Office COM object.
*
* This feature requires Visual Studio Tools for Office 3.0 SP1 (included in 
* Visual Studio 2008 SP1) for both design-time and runtime support.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 6/13/2009 3:00 PM Wang Jie Created
\******************************************************************************************/


#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Excel = Microsoft.Office.Interop.Excel;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Tools.Excel;
using Microsoft.Office.Tools.Excel.Extensions;
#endregion

namespace CSVstoGetWrapperObject
{
    public partial class ThisAddIn
    {
        private void ThisAddIn_Startup(object sender, System.EventArgs e)
        {
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
