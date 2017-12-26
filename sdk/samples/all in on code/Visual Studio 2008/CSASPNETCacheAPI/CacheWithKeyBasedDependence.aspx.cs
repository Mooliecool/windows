/****************************** Module Header ******************************\
* Module Name:  CacheWithKeyBasedDependence.aspx.cs
* Project:      CSASPNETCacheAPI
* Copyright (c) Microsoft Corporation.
* 
* Demonstrate how to set Key-based dependence for Cache.. 
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
using System.Web.Caching;
#endregion

namespace CSASPNETCacheAPI
{
    public partial class CacheWithKeyBasedDependence : System.Web.UI.Page
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

            if (Cache["Trigger"] == null)
            {
                // Set dependence Cache
                Cache.Insert("Trigger", strDateTime, null, DateTime.Now.AddSeconds(10), TimeSpan.Zero);
            }

            string [] strKeys = {"Trigger"};

            // Set dependence
            CacheDependency dependucy = new CacheDependency(null, strKeys);

            if (Cache["Time"] == null)
            {
                Cache.Insert("Time", strDateTime,dependucy);
            }

            lblCacheTime.Text = Cache["Time"].ToString();
        }
    }
}
