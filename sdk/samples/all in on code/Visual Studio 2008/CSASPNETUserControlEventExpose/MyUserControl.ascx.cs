/****************************** Module Header ******************************\
* Module Name:  MyUserControl.ascx.cs
* Project:      CSASPNETuserControlEventsExpose 
* Copyright (c) Microsoft Corporation.
* 
*This usercontrol declare the delagate and event as public.
*Thus the event will be subsribed by the webpage.
*When btnTest button is clicked, it will fire the event named 'MyEvent'.
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

namespace CSASPNETUserControlEventExpose
{
    public partial class MyUserControl : System.Web.UI.UserControl
    {
        public delegate void MyEventHandler(object sender, EventArgs e);
        public event MyEventHandler MyEvent;

        protected void Page_Load(object sender, EventArgs e)
        {

        }

        protected void btnTest_Click(object sender, EventArgs e)
        {
            if (MyEvent != null)
            {
                this.MyEvent(sender, e);
            }
            Response.Write("User Control’s Button Click <BR/>");
        }
    }
}