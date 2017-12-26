/****************************** Module Header ******************************\
* Module Name: UrlRoutingHandlers.cs
* Project:     CSASPNETEmbedLanguageInUrl
* Copyright (c) Microsoft Corporation
*
* The UrlRoutingHandlers will check the request url. This interface
* break the url string, check the file name and throw them to 
* the InvalidPage.aspx page if it not existence.
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
using System.Web;
using System.Web.Routing;
using System.Text;
using System.Web.Compilation;
using System.Web.UI;

namespace CSASPNETEmbedLanguageInUrl
{
    public class UrlRoutingHandlers : IRouteHandler
    {
        /// <summary>
        /// Create this RoutingHandler to check the HttpRequest and
        /// return correct url path.
        /// </summary>
        /// <param name="context"></param>
        /// <returns></returns>
        public IHttpHandler GetHttpHandler(RequestContext context)
        {
            string language = context.RouteData.Values["language"].ToString().ToLower();
            string pageName = context.RouteData.Values["pageName"].ToString();
            if (pageName == "ShowMe.aspx")
            {
                return BuildManager.CreateInstanceFromVirtualPath("~/ShowMe.aspx", typeof(Page)) as Page;
            }
            else
            {
                return BuildManager.CreateInstanceFromVirtualPath("~/InvalidPage.aspx", typeof(Page)) as Page;
            }
        }
    }
}