//*************************** Module Header ********************************\
// Module Name:    Default.aspx.cs
// Project:        CSASPNETServerClock
// Copyright (c) Microsoft Corporation
//
// This project illustrates how to get the time of the server side and show 
// it to the client page. Sometimes a website need to show an unified clock 
// on pages to all the visitors. However, if we use JavaScript to handle this
// target, the time will be different from each client. So we need the server
// to return the server time and refresh the clock per second via AJAX. 
//
// This page is used to get and show the time. Please refer to the Clock.aspx
// page to view the code which responses the time.
// 
// This source is subject to the Microsoft Public License.
// See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
// All other rights reserved.
//
//**************************************************************************\

using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace CSASPNETServerClock
{
    public partial class Default : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {

        }
    }
}