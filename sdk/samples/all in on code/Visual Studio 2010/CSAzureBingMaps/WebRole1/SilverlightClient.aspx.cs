/****************************** Module Header ******************************\
* Module Name:	SilverlightClient.aspx.cs
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* The code behind for the aspx page hosting the Silverlight client.
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

namespace AzureBingMaps.WebRole
{
    public partial class SilverlightClient : System.Web.UI.Page
    {
        // Those properties will be passed to Silverlight as initial parameters.
        public bool IsAuthenticated { get; set; }
        public string WelcomeMessage { get; set; }

        protected void Page_Load(object sender, EventArgs e)
        {
            // Query session data to see if the user has been authenticated.
            if (Session["User"] != null)
            {
                this.IsAuthenticated = true;
                this.WelcomeMessage = "Welcome: " + (string)Session["User"] + ".";
            }
            else
            {
                this.IsAuthenticated = false;
                this.WelcomeMessage = null;
            }
        }
    }
}