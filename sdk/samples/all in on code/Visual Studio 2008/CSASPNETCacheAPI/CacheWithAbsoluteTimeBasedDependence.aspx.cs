/****************************** Module Header ******************************\
* Module Name:  CacheWithAbsoluteTimeBasedDependence.aspx.cs
* Project:      CSASPNETCacheAPI
* Copyright (c) Microsoft Corporation.
* 
* Demonstrate how to use Absolute Time-Based Dependence. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/22/2009 10:33 AM Thomas Sun Created
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
#endregion


namespace CSASPNETCacheAPI
{
    public partial class CacheWithAbsoluteTimeBasedDependence : System.Web.UI.Page
    {
        /// <summary>
        /// Save data in Cache when page is loading
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                Cache.Remove("Time");
            }

            string strDateTime = DateTime.Now.ToString();
            lblCurrentTime.Text = strDateTime;

            if (Cache["Time"] == null)
            {
                // Save cache with key "Time" using Cache's Insert method and set its absolute time to 10s  
                Cache.Insert("Time", strDateTime, null, DateTime.Now.AddSeconds(10), TimeSpan.Zero);
            }

            lblCacheTime.Text = Cache["Time"].ToString();

        }
    }
}
