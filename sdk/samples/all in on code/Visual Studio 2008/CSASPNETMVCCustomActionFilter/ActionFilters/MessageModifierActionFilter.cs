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

namespace CSASPNETMVCCustomActionFilter.ActionFilters
{
    public class MessageModifierActionFilter : ActionFilterAttribute
    {
        public override void OnResultExecuted(ResultExecutedContext filterContext)
        {
            // If we modify the ViewData here, changes will not be reflected in the final PageView since it is too late here( Result has already been executed)

        }

        public override void OnResultExecuting(ResultExecutingContext filterContext)
        {
            // This event is a preprocessing event where we can customize the ViewData collection

            ViewResult view = filterContext.Result as ViewResult;
            if (view != null)
            {
                view.ViewData["Message"] = "[Modified by MessageModifierActionFilter.OnResultExecuting]" + view.ViewData["Message"].ToString();
            }
        }
    }
}
