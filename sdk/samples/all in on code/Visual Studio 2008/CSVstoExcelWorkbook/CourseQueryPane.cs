/************************************* Module Header **************************************\
* Module Name:	CourseQueryPane.cs
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
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
#endregion

namespace CSVstoExcelWorkbook
{
    public partial class CourseQueryPane : UserControl
    {
        public CourseQueryPane()
        {
            InitializeComponent();
        }

        private void CourseQueryPane_Load(object sender, EventArgs e)
        {
            // Fill the student names list.
            studentListTableAdapter.FillStudentList(schoolDataSet.StudentList);
        }

        private void cmdQuery_Click(object sender, EventArgs e)
        {
            // Update course list for selected student.
            Globals.Sheet1.UpdateCourseList(cboName.Text.Trim());
        }
    }
}
