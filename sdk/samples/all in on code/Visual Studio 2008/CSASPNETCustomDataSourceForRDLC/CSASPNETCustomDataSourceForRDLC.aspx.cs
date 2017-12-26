/****************************** Module Header ******************************\
* Module Name:    CSASPNETCustomDataSourceForRDLC.cs
* Project:        CSASPNETCustomDataSourceForRDLC
* Copyright (c) Microsoft Corporation.
*
* The CSASPNETCustomDataSourceForRDLC sample demonstrates how to use custom  
* class as the datasource for client report.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* History:
* * 11/23/2009 11:00 AM Zong-Qing Li Created
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
using Microsoft.Reporting.WebForms;

namespace CSASPNETCustomDataSourceForRDLC
{
    public partial class CSASPNETCustomDataSourceForRDLC : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {

        }

        protected void btn_search_Click(object sender, EventArgs e)
        {
            // Filter the records via report parameter "CountryName".
            ReportParameter rp = new ReportParameter("CountryName", txt_country.Text);
            ReportViewer1.LocalReport.SetParameters(new ReportParameter[] { rp });
            ReportViewer1.LocalReport.Refresh(); 
        }
    }
}
