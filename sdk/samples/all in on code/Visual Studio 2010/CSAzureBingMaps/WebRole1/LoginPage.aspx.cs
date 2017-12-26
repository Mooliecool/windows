/****************************** Module Header ******************************\
* Module Name:	LoginPage.cs
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* The code behind for LoginPage.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Web;
using Microsoft.Live;

namespace AzureBingMaps.WebRole
{
    public partial class LoginPage : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            // Store the return page in session.
            if (Request.QueryString["returnpage"] != null)
            {
                Session["ReturnPage"] = Request.QueryString["returnpage"];
            }
        }

        /// <summary>
        /// Windows Live Messenger Connect session ID.
        /// </summary>
        public string SessionId
        {
            get
            {
                SessionIdProvider oauth = new SessionIdProvider();
                return "wl_session_id=" + oauth.GetSessionId(HttpContext.Current);
            }
        }
    }
}