/****************************** Module Header ******************************\
* Module Name:  OutputCacheWithDuration.aspx.cs
* Project:      CSASPNETOutputCache
* Copyright (c) Microsoft Corporation.
* 
* This page is used to demonstrate OutputCache with Duration attribute. 
* The cache will be expiration after 10s specified in @OutputCache directive.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 8/28/2009 14:36 AM Thomas Sun Created
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
#endregion


namespace CSASPNETOutputCache
{
    public partial class OutputCacheWithDuration : System.Web.UI.Page
    {
        /// <summary>
        /// Display datetime when page is rendering first time.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Page_Load(object sender, EventArgs e)
        {
            lblResult.Text = DateTime.Now.ToString();

        }

    }
}
