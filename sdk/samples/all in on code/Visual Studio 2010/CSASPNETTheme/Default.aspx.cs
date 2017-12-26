/****************************** Module Header ******************************\
* Module Name:  Default.aspx.cs
* Project:      CSASPNETTheme
* Copyright (c) Microsoft Corporation.
* 
* This page populates a sample control and two buttons which can lead to reload 
* the page in different themes during the page initial period.
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
using System.Collections.Generic;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace CSASPNETTheme
{
    public partial class _Default : System.Web.UI.Page
    {
        //Set page theme before initialization.
        protected void Page_PreInit(object sender, EventArgs e)
        {
            switch (Request.QueryString["theme"])
            {
                case "Blue":
                    Page.Theme = "BlueTheme";
                    break;
                case "Pink":
                    Page.Theme = "PinkTheme";
                    break;
            }
        }        
        
        protected void Page_Load(object sender, EventArgs e)
        {

        }

        protected void Button1_Click(object sender, EventArgs e)
        {
            Response.Redirect("~/default.aspx?theme=Blue");
        }

        protected void Button2_Click(object sender, EventArgs e)
        {
            Response.Redirect("~/default.aspx?theme=Pink");
        }
    }
}
