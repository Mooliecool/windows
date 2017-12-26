/****************************** Module Header ******************************\
* Module Name:  Default.aspx.cs
* Project:      CSASPNETuserControlEventsExpose
* Copyright (c) Microsoft Corporation.
* 
* This page load the usercontrol and add the usercontrol into webpage.
* Then subscribe the MyEvent of usercontrol to react the button click of usercontrol in the webpage.
* When click the button of usercontrol, the webpage will display the selected value of dropdownlist.
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
    public partial class _Default : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            MyUserControl myUserControl = LoadControl("MyUserControl.ascx") as MyUserControl;
            if (myUserControl != null)
            {
                myUserControl.MyEvent += new MyUserControl.MyEventHandler(userControlBtnClick);
                this.PlaceHolder1.Controls.Add(myUserControl);
            }
        }
        public void userControlBtnClick(object sender, EventArgs e)
        {
            Response.Write("Main Page Event Handler<BR/>Selected Value:" + ddlTemp.SelectedItem.Text + "<BR/>");

        }
    }
}
