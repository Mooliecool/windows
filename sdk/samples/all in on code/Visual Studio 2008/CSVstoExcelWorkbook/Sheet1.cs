/************************************* Module Header **************************************\
* Module Name:	Sheet1.cs
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
    public partial class Sheet1
    {
        private void Sheet1_Startup(object sender, System.EventArgs e)
        {
        }

        internal void UpdateCourseList(string studentName)
        {
            // Update the title.
            this.Range["A1", "A1"].Value2 = "Course List for " + studentName;

            // Update the DataTable.
            courseListTableAdapter.FillCourseList(schoolDataSet.CourseList, studentName);
        }

        private void Sheet1_Shutdown(object sender, System.EventArgs e)
        {
        }

        #region VSTO Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InternalStartup()
        {
            this.Startup += new System.EventHandler(Sheet1_Startup);
            this.Shutdown += new System.EventHandler(Sheet1_Shutdown);
        }

        #endregion

    }
}
