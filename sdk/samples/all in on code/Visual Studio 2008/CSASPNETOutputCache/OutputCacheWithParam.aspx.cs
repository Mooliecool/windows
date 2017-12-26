/****************************** Module Header ******************************\
* Module Name:  OutputCacheWithParam.aspx.cs
* Project:      CSASPNETOutputCache
* Copyright (c) Microsoft Corporation.
* 
* This page is used to demonstrate OutputCache with VaryByParam. 
* The cache has different version for each QueryString value.
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
    public partial class OutputCacheWithParam : System.Web.UI.Page
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
