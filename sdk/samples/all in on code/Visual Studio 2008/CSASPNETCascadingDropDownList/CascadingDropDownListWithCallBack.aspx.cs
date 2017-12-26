/****************************** Module Header ******************************\
* Module Name:  CascadingDropDownListWithCallBack.aspx.cs
* Project:      CSASPNETCascadingDropDown
* Copyright (c) Microsoft Corporation.
* 
* This page is used to show the Cascading DropDownList with callback
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/24/2009 10:33 AM Thomas Sun Created
\***************************************************************************/

#region Using directives
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
using System.Collections.Generic;
#endregion


namespace CSASPNETCascadingDropDownList
{
    public partial class CascadingDropDownListWithCallBack : System.Web.UI.Page
    {
        /// <summary>
        /// Page Load event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Page_Load(object sender, EventArgs e)
        {
            // Register client onclick event for submit button
            btnSubmit.Attributes.Add("onclick", "SaveSelectedData();");
            if (!IsPostBack)
            {
                // Bind country dropdownlist
                BindddlCountry();
                // Initialize hide field value
                hdfResult.Value = "";
                hdfCity.Value = "";
                hdfCitySelectValue.Value = "0";
                hdfRegion.Value = "";
                hdfRegionSelectValue.Value = "";
            }
        }


        /// <summary>
        /// Bind country dropdownlist
        /// </summary>
        public void BindddlCountry()
        {
            List<string> list = RetrieveDataFromXml.GetAllCountry();
            ddlCountry.DataSource = list;
            ddlCountry.DataBind();
        }


        /// <summary>
        /// Submit button click event and show select result
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnSubmit_Click(object sender, EventArgs e)
        {
            // Get result from hide field saved in client
            string strResult = hdfResult.Value;
            lblResult.Text = strResult;
        }
    }
}
