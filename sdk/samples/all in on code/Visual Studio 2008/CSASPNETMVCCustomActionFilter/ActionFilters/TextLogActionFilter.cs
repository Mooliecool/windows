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
using System.Text;
using System.Web.Mvc;
using System.IO;

namespace CSASPNETMVCCustomActionFilter.ActionFilters
{
    public class TextLogActionFilter : ActionFilterAttribute
    {
        // Property for specifying log file path
        public string LogFileName { get; set; }

        // Default constructor
        public TextLogActionFilter() { LogFileName = "MVC_ACTION_FILTER_LOG.TXT"; }

        public override void OnActionExecuted(ActionExecutedContext filterContext)
        {
            StreamWriter sw = File.AppendText(LogFileName);
            sw.WriteLine(">>>TextLogActionFilter.OnActionExecuted, {0}", DateTime.Now);
            sw.Close(); 
        }

        public override void OnActionExecuting(ActionExecutingContext filterContext)
        {
            StreamWriter sw = File.AppendText(LogFileName);
            sw.WriteLine(">>>TextLogActionFilter.OnActionExecuting, {0}", DateTime.Now);
            sw.Close();
        }

        public override void OnResultExecuted(ResultExecutedContext filterContext)
        {
            StreamWriter sw = File.AppendText(LogFileName);
            sw.WriteLine(">>>TextLogActionFilter.OnResultExecuted, {0}", DateTime.Now);
            sw.Close();
        }

        public override void OnResultExecuting(ResultExecutingContext filterContext)
        { 
            StreamWriter sw = File.AppendText(LogFileName);
            sw.WriteLine(">>>TextLogActionFilter.OnResultExecuting, {0}", DateTime.Now);
            sw.Close();
        }

    }
}
