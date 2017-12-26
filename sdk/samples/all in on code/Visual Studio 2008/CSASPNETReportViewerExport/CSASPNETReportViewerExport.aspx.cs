/****************************** Module Header ******************************\
* Module Name:    CSASPNETReportViewerExport.cs
* Project:        CSASPNETReportViewerExport
* Copyright (c) Microsoft Corporation.
*
* The CSASPNETReportViewerExport sample demonstrates how to export the rdlc 
* as PDF and EXCEL programmatically.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* History:
* * 10/8/2009 11:00 AM Zong-Qing Li Created
\***************************************************************************/
using System;
using System.Collections;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Xml.Linq;
using System.IO;

namespace CSASPNETReportViewerExport
{
    public partial class CSASPNETReportViewerExport : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {

        }
        public void Export(string format)
        {
            string mimeType, encoding, extension;
            string[] streamids; Microsoft.Reporting.WebForms.Warning[] warnings;
            // Get report byte data
            byte[] bytes = ReportViewer1.LocalReport.Render(format, "", out mimeType, out encoding, out extension, out streamids, out warnings);
            string path = string.Empty;
            // Judge which format will be used
            if (format == "PDF")
            {
                path = Server.MapPath("SampleReport.pdf");
            }
            else
            {
                path = Server.MapPath("SampleReport.xls");
            }
            // Load the report byte data into pdf file or excel
            FileStream fs = new FileStream(path, FileMode.OpenOrCreate);
            byte[] data = new byte[fs.Length];
            fs.Write(bytes, 0, bytes.Length);
            fs.Close();
        }

        protected void btn_ExportExcel_Click(object sender, EventArgs e)
        {
            Export("EXCEL");
        }

        protected void btn_ExportPDF_Click(object sender, EventArgs e)
        {
            Export("PDF");
        }
    }
}
