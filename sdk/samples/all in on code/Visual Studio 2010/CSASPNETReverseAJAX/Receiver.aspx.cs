/****************************** Module Header ******************************\
* Module Name:    Receiver.aspx.cs
* Project:        CSASPNETReverseAJAX
* Copyright (c) Microsoft Corporation
*
* The user will use this page to log in with a unique user name. Then when there is a 
* new message on the server, the server will immediately push the message to the client.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;

namespace CSASPNETReverseAJAX
{
    public partial class Receiver : System.Web.UI.Page
    {
        protected void btnLogin_Click(object sender, EventArgs e)
        {
            string userName = tbUserName.Text.Trim();

            // Join into the recipient list.
            if (!string.IsNullOrEmpty(userName))
            {
                ClientAdapter.Instance.Join(userName);

                Session["userName"] = userName;
            }
        }

        protected void Page_PreRender(object sender, EventArgs e)
        {
            // Activate the JavaScript waiting loop.
            if (Session["userName"] != null)
            {
                string userName = (string)Session["userName"];

                // Call JavaScript method waitEvent to start the wait loop.
                ClientScript.RegisterStartupScript(this.GetType(), "ActivateWaitingLoop", "waitEvent();", true);

                lbNotification.Text = string.Format("Your user name is <b>{0}</b>. It is waiting for new message now.", userName);

                // Disable the login.
                tbUserName.Visible = false;
                btnLogin.Visible = false;
            }
        }
    }
}