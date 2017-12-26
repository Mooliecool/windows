/****************************** Module Header ******************************\
* Module Name:	CSASPNETRDLCMainSubReport.aspx.cs
* Project:		CSASPNETRDLCMainSubReport
* Copyright (c) Microsoft Corporation.
* 
* This sample demonstrates how to build a sub-report which get data from
* SQL Server based on a primary key passed from the main-report, then use 
* the ReportViewer control to show that Main/Sub report in the web page.
* 
*  
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* 03/30/2010 13:00 AM Zong-Qing Li Created
\***************************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using Microsoft.Reporting.WebForms;

namespace CSASPNETRDLCMainSubReport
{
    public partial class CSASPNETRDLCMainSubReport : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            // Supply the data for the sub report.
            ReportViewer1.LocalReport.SubreportProcessing += new SubreportProcessingEventHandler(SetSubDataSource);
            this.ReportViewer1.LocalReport.Refresh();

        }
        public void SetSubDataSource(object sender, SubreportProcessingEventArgs e)
        {
            e.DataSources.Add(new ReportDataSource("CourseDataSet_Course", "ObjectDataSource2"));
        }

    }
}
