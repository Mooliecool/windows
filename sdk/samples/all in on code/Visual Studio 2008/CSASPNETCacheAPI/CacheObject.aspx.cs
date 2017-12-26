/****************************** Module Header ******************************\
* Module Name:  CacheObject.aspx.cs
* Project:      CSASPNETCacheAPI
* Copyright (c) Microsoft Corporation.
* 
* Demonstrate how to save data in Cache and how remove it from Cache. 
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
    public partial class CacheObject : System.Web.UI.Page
    {
        /// <summary>
        /// Save data in Cache when page is loading
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Page_Load(object sender, EventArgs e)
        {
            // Display current datetime which is used to compare with cached datetime.
            lblCurrentTime.Text = DateTime.Now.ToString();

            if (!IsPostBack)
            {
                Cache["Time"] = DateTime.Now.ToString();
            }

            // Check whether Cache object is null befor trying to use it.
            if (Cache["Time"] != null)
            {
                lblCacheTime.Text = Cache["Time"].ToString();
            }
        }

        /// <summary>
        /// Remove data from Cache
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnClearCache_Click(object sender, EventArgs e)
        {
            // Remove cache with key "Time" using Cache's Remove method
            Cache.Remove("Time");
            Cache["Time"] = DateTime.Now.ToString();
        }
    }
}
