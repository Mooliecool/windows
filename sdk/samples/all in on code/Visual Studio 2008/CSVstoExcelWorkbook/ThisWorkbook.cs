/************************************* Module Header **************************************\
* Module Name:	ThisWorkbook.cs
* Project:		CSVstoExcelWorkbook
* Copyright (c) Microsoft Corporation.
* 
* The CSVstoExcelWorkbook provides the examples on how to customize Excel 
* Workbooks by using the ListObject and the document Actions Pane.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 4/7/2009 11:00 AM Wang Jie Created
\******************************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml.Linq;
using Microsoft.VisualStudio.Tools.Applications.Runtime;
using Excel = Microsoft.Office.Interop.Excel;
using Office = Microsoft.Office.Core;
#endregion

namespace CSVstoExcelWorkbook
{
    public partial class ThisWorkbook
    {

        private void ThisWorkbook_Startup(object sender, System.EventArgs e)
        {
            // Adds the CourseQueryPane to the ActionsPane.
            // To toggle the ActionsPane on/off, click the View tab in Excel Ribbon,
            // then in the Show/Hide group, toggle the Document Actions button.
            this.ActionsPane.Controls.Add(new CourseQueryPane());
        }

        private void ThisWorkbook_Shutdown(object sender, System.EventArgs e)
        {
        }

        #region VSTO Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InternalStartup()
        {
            this.Startup += new System.EventHandler(ThisWorkbook_Startup);
            this.Shutdown += new System.EventHandler(ThisWorkbook_Shutdown);
        }

        #endregion

    }
}
