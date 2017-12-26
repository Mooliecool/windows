/****************************** Module Header ******************************\
* Module Name:    Sender.aspx.cs
* Project:        CSASPNETReverseAJAX
* Copyright (c) Microsoft Corporation
*
* The user will use this page to send a message to a particular recipient.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;

namespace CSASPNETReverseAJAX
{
    public partial class Sender : System.Web.UI.Page
    {
        protected void btnSend_Click(object sender, EventArgs e)
        {
            // Create a message entity to contain all necessary data.
            Message message = new Message();
            message.RecipientName = tbRecipientName.Text.Trim();
            message.MessageContent = tbMessageContent.Text.Trim();

            if (!string.IsNullOrWhiteSpace(message.RecipientName) && !string.IsNullOrEmpty(message.MessageContent))
            {
                // Call the client adapter to send the message to the particular recipient instantly.
                ClientAdapter.Instance.SendMessage(message);

                // Display a timestamp.
                lbNotification.Text += DateTime.Now.ToLongTimeString() + ": Message sent!<br/>";
            }
        }
    }
}