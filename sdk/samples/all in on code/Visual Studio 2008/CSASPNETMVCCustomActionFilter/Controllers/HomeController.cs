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

namespace CSASPNETMVCCustomActionFilter.Controllers
{
    [HandleError]
    public class HomeController : Controller
    {
        // Use declarative attribute to registetr the MessageModifier ActionFilter
        [CSASPNETMVCCustomActionFilter.ActionFilters.MessageModifierActionFilter]
        public ActionResult Index()
        {
            // The original value of Message data returned via ViewData collection
            ViewData["Message"] = "Welcome to ASP.NET MVC!";

            return View();
        }

        // Use declarative attribute to registetr the Logging ActionFilter
        [CSASPNETMVCCustomActionFilter.ActionFilters.TextLogActionFilter(LogFileName = @"D:\temp\logs\mvc_action_filter.log")]
        public ActionResult About()
        {
            return View();
        }
    }
}
