/****************************** Module Header ******************************\
* Module Name: Global.asax.cs
* Project:     CSASPNETControlPermissionForFiles
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to control the permission for protect files and 
* folders on server from being download. Here we give a solution that when the
* web application receive a URL request, we will make a judgment that if the 
* request file's extension name is not .jpg file then redirect to 
* NoPermissionPage page. Also, user can not access the image file via copy URL. 
* 
* This Gload.aspx.cs file can check the user's http request is correct or not.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*****************************************************************************/



using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.SessionState;

namespace CSASPNETControlPermissionForFiles
{
    public class Global : System.Web.HttpApplication
    {

        protected void Application_Start(object sender, EventArgs e)
        {

        }

        protected void Session_Start(object sender, EventArgs e)
        {

        }

        /// <summary>
        /// The Application_BeginRequest method is used to make a judgment whether the request file 
        /// is jpg file, and throw illegal request to NoPermissionPage.aspx page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Application_BeginRequest(object sender, EventArgs e)
        {
            string path = HttpContext.Current.Request.Path; 
            string[] pathElements = path.Split('.');
            string extenseName = pathElements[pathElements.Length - 1];
            if (!extenseName.Equals("aspx",StringComparison.OrdinalIgnoreCase))
            {
                if (!extenseName.Equals("jpg",StringComparison.OrdinalIgnoreCase) || !IsUrl())
                {
                    HttpContext.Current.Response.Redirect("~/NoPermissionPage.aspx");
                }
            }
        }

        /// <summary>
        /// The method is used to check whether the page is opened by typing the URL in browser  
        /// </summary>
        /// <returns></returns>
        protected bool IsUrl()
        {
            string httpReferer = System.Web.HttpContext.Current.Request.ServerVariables["HTTP_REFERER"];
            string serverName = System.Web.HttpContext.Current.Request.ServerVariables["SERVER_NAME"];
            if ((httpReferer != null) && (httpReferer.IndexOf(serverName) == 7))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        protected void Application_AuthenticateRequest(object sender, EventArgs e)
        {

        }

        protected void Application_Error(object sender, EventArgs e)
        {

        }

        protected void Session_End(object sender, EventArgs e)
        {

        }

        protected void Application_End(object sender, EventArgs e)
        {

        }
    }
}