/****************************** Module Header ******************************\
* Module Name:  CustomHttpModule
* Project:      CustomHandlerandModuleProject
* Copyright (c) Microsoft Corporation.
* 
* 
* This module implements IHttpModule interface to write custom Http Module
* to send response for the extension with .aspx resource.
*
*  
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* 11/27/2009 11:16 AM Thomas Sun Created
\***************************************************************************/
#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
#endregion Using directives

namespace CustomHandlerandModuleProject
{
    /// <summary>
    /// Inherit IHttpModule 
    /// </summary>
    public  class CustomHttpModule : IHttpModule
    {
        /// <summary>
        /// Implement the Init method
        /// </summary>
        /// <param name="application">Current HttpApplication object</param>
        public void Init(HttpApplication application)
        {
            // Register BeginRequest event
            application.BeginRequest += (new EventHandler(this.Application_BeginRequest));
            // Register EndRequest event
            application.EndRequest += (new EventHandler(this.Application_EndRequest));
        }

        /// <summary>
        /// Write content to Response object in BeginRequest event
        /// </summary>
        /// <param name="source">Current HttpApplication object</param>
        /// <param name="e"></param>
        private void Application_BeginRequest(Object source, EventArgs e)
        {
            // Get current HttpApplication object
            HttpApplication application = (HttpApplication)source;
            // Get current Context object
            HttpContext context = application.Context;
            // Get request file path
            string filePath = context.Request.FilePath;
            // Get request file's extension
            string fileExtension = VirtualPathUtility.GetExtension(filePath);

            // Write content if extension is .aspx
            if (fileExtension.Equals(".aspx"))
            {
                context.Response.Write("<h1><font color=red>Beginning of Request: added by Custom Module</font></h1><hr>");
            }
        }

        /// <summary>
        /// Write content to Response object in EndRequest event
        /// </summary>
        /// <param name="source">Current HttpApplication object</param>
        /// <param name="e"></param>
        private void Application_EndRequest(Object source, EventArgs e)
        {
            // Get current HttpApplication object
            HttpApplication application = (HttpApplication)source;
            // Get current Context object
            HttpContext context = application.Context;
            // Get request file path
            string filePath = context.Request.FilePath;
            // Get request file's extension
            string fileExtension = VirtualPathUtility.GetExtension(filePath);

            // Write content if extension is .aspx
            if (fileExtension.Equals(".aspx"))
            {
                context.Response.Write("<hr><h1><font color=red>End of Request: added by Custom Module</font></h1>");
            }
        }

        public void Dispose() { }

    }
}
