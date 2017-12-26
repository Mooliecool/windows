/****************************** Module Header ******************************\
* Module Name:  GetDataForCallBack.aspx.cs
* Project:      CSASPNETCascadingDropDown
* Copyright (c) Microsoft Corporation.
* 
* This page is used to retrieve data in callback and write data to client.
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
using System.Text;
#endregion


namespace CSASPNETCascadingDropDownList
{
    public partial class GetDataForCallBack : System.Web.UI.Page
    {
        /// <summary>
        /// Page Load event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Page_Load(object sender, EventArgs e)
        {
            // Get querystring from URL and retrieve data basing on it
            if (Request.QueryString.Count > 0)
            {
                string strValue = Request.QueryString[0];
                if (Request.QueryString["country"] != null)
                {
                    RetrieveRegionByCountry(strValue);
                }
                else
                {
                    RetrieveCityByRegion(strValue);
                }
            }

        }

        /// <summary>
        /// Get region basing on country value
        /// </summary>
        /// <param name="strValue">The country value</param>
        public void RetrieveRegionByCountry(string strValue)
        {
            List<string> list = RetrieveDataFromXml.GetRegionByCountry(strValue);
            WriteData(list);
        }

        /// <summary>
        /// Get city basing on region value
        /// </summary>
        /// <param name="strValue">The region value</param>
        public void RetrieveCityByRegion(string strValue)
        {
            List<string> list = RetrieveDataFromXml.GetCityByRegion(strValue);
            WriteData(list);
        }

        /// <summary>
        /// Write data to client
        /// </summary>
        /// <param name="list">The list contains value </param>
        public void WriteData(List<string> list)
        {
            int iCount = list.Count;
            StringBuilder builder = new StringBuilder();
            if (iCount > 0)
            {
                for (int i = 0; i < iCount - 1; i++)
                {
                    builder.Append(list[i] + ",");
                }
                builder.Append(list[iCount - 1]);
            }

            Response.ContentType = "text/xml";
            // Write data in string format "###,###,###" to client
            Response.Write(builder.ToString());
            Response.End();
        }
    }
}
