/****************************** Module Header ******************************\
* Module Name:	HtmlClient.aspx.cs
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* The code behind for the aspx page hosting the HTML client.
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
    public partial class HtmlClient : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            // Query session data to see if the user has been authenticated.
            // And display sign in link or welcome message based on the information.
            if (Session["User"] != null)
            {
                this.LoginLink.Visible = false;
                this.UserNameLabel.Visible = true;
                this.UserNameLabel.Text = "Welcome, " + (string)Session["User"] + ".";
            }
            else
            {
                this.LoginLink.Visible = true;
                this.UserNameLabel.Visible = false;
            }
        }
    }
}