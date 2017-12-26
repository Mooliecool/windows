/****************************** Module Header ******************************\
 * Module Name:  MessageModifierActionFilter.cs
 * Project:              CSASPNETMVCCustomActionFilter
 * Copyright (c) Microsoft Corporation.
 * 
 * The CSASPNETMVCCustomActionFilter sample demonstrates how to use C# codes to 
 * create custom ActionFilters for ASP.NET MVC web application. In this sample,
 * there are two custom ActionFilters, one is used for customizing ViewData(
 * before page view result get executed and rendered; another is used for 
 * perform logging within the various events during the processing of custom 
 * ActionFilters.
 * 
 * 
 * This source is subject to the Microsoft Public License.
 * See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 * All other rights reserved.
 * 
 * History:
 * * 10/10/2009 1:35 PM Steven Cheng Created
 ***************************************************************************/

using System.Web;
using System.Web.Mvc;
using System.Web.UI;

namespace CSASPNETMVCCustomActionFilter
{
    public partial class _Default : Page
    {
        public void Page_Load(object sender, System.EventArgs e)
        {
            // Change the current path so that the Routing handler can correctly interpret
            // the request, then restore the original path so that the OutputCache module
            // can correctly process the response (if caching is enabled).

            string originalPath = Request.Path;
            HttpContext.Current.RewritePath(Request.ApplicationPath, false);
            IHttpHandler httpHandler = new MvcHttpHandler();
            httpHandler.ProcessRequest(HttpContext.Current);
            HttpContext.Current.RewritePath(originalPath, false);
        }
    }
}
