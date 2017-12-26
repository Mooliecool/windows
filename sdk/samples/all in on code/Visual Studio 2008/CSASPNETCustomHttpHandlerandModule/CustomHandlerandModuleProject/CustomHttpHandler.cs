/****************************** Module Header ******************************\
* Module Name:  CustomHttpHandler
* Project:      CustomHandlerandModuleProject
* Copyright (c) Microsoft Corporation.
* 
* 
* This module implements IHttpHandler interface to write custom Http Handler
* to send response for the extension with .demo resource.
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
    /// Inherit IHttpHandler
    /// </summary>
    public class CustomHttpHandler : IHttpHandler
    {
        /// <summary>
        /// Implement the ProcessRequest method
        /// </summary>
        /// <param name="context">Current HttpContext</param>
        public void ProcessRequest(HttpContext context)
        {
            //// Get Current Context Request object
            //HttpRequest Request = context.Request;
            // Get Current Context Response object
            HttpResponse Response = context.Response;
            // Write content to client 
            Response.Write("<html>");
            Response.Write("<body>");
            Response.Write("<form><h1><font color=red>This is the response for the .demo file.</font></h1>");
            Response.Write("<br />");
            Response.Write("<br />");
            Response.Write("<br />");
            Response.Write("<br />");
            Response.Write("<br />");
            Response.Write("<br />");
            Response.Write("<br />");
            Response.Write("<a href=\"default.htm\">Go Back to Default page</a>");
            Response.Write("</form>");
            Response.Write("</body>");
            Response.Write("</html>");
        }

        /// <summary>
        /// Indicate whether this custom handler is reusable in pool
        /// </summary>
        public bool IsReusable
        {
            get { return false; }
        }

    }
}
