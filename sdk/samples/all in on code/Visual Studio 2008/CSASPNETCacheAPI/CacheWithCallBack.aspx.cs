/****************************** Module Header ******************************\
* Module Name:  CacheWithCallBack.aspx.cs
* Project:      CSASPNETCacheAPI
* Copyright (c) Microsoft Corporation.
* 
* Demonstrate how to use CallBack method when Cache is expiration. 
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
    public partial class CacheWithCallBack : System.Web.UI.Page
    {
        // Declare string variable
        private static string strResult = "";

        /// <summary>
        /// Save data in Cache when page is loading
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Page_Load(object sender, EventArgs e)
        {
            string strDateTime = DateTime.Now.ToString();
            lblCurrentTime.Text = strDateTime;

            if (!IsPostBack)
            {
                Cache.Remove("Time");
                lblCacheTime.Text = strDateTime;
            }

            if (strResult != "")
            {
                lblCacheTime.Text = strResult;
            }

            //Register CallBack method
            CacheItemRemovedCallback onRemove = new CacheItemRemovedCallback(this.RemovedCallback);

            if (Cache["Time"] == null)
            {
                // Save cache with key "Time" using Cache's Insert method and set its absolute time to 10s, and set CacheItemRemovedCallback  
                Cache.Insert("Time", strDateTime, null, DateTime.Now.AddSeconds(10), TimeSpan.Zero, CacheItemPriority.High, onRemove);
            }
        }


        /// <summary>
        /// Call Back method
        /// </summary>
        /// <param name="k">The key that is removed from the cache</param>
        /// <param name="v">The Object item associated with the key removed from the cache</param>
        /// <param name="r">The reason the item was removed from the cache</param>
        public void RemovedCallback(String k, Object v, CacheItemRemovedReason r)
        {
            // Modify strResult value
            strResult = " (CallBack event is fired on) " + DateTime.Now.ToString();
        }
    }
}
