/****************************** Module Header ******************************\
 * Module Name:  MessageModifierActionFilter.cs
 * Project:              CSASPNETMVCCustomActionFilter
 * Copyright (c) Microsoft Corporation.
 * 
 * The CSASPNETMVCCustomActionFilter sample demonstrates how to use C# codes to 
 * create custom ActionFilters for ASP.NET MVC web application. In this sample,
 * there are two custom ActionFilters, one is used for customizing ViewData
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



using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.Web.Routing;

namespace CSASPNETMVCCustomActionFilter
{
    // Note: For instructions on enabling IIS6 or IIS7 classic mode, 
    // visit http://go.microsoft.com/?LinkId=9394801

    public class MvcApplication : System.Web.HttpApplication
    {
        public static void RegisterRoutes(RouteCollection routes)
        {
            // Default code for registering MVC Url routering rules

            routes.IgnoreRoute("{resource}.axd/{*pathInfo}");

            routes.MapRoute(
                "Default",                                              // Route name
                "{controller}/{action}/{id}",                           // URL with parameters
                new { controller = "Home", action = "Index", id = "" }  // Parameter defaults
            );

        }

        protected void Application_Start()
        {
            RegisterRoutes(RouteTable.Routes);
        }
    }
}