/**************************** Module Header ********************************\
* Module Name:    Default.aspx.cs
* Project:        CSASPNETAddControlDynamically
* Copyright (c) Microsoft Corporation
*
* This project shows how to add controls dynamically in ASP.NET pages. It 
* imitates a scenario that customers need to input more than one addr info 
* without max limit. So we use button to add new address input TextBoxes.
* After a customer inputs all the addresses, we also use button to update 
* these info into database, which is run as displaying these addresses in 
* this sample.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\***************************************************************************/

using System;
using System.Web.UI.WebControls;

namespace CSASPNETAddControlDynamically
{
    public partial class Default : System.Web.UI.Page
    {
        protected void Page_PreLoad(object sender, EventArgs e)
        {
            // Use ViewState to store the number of the addresses.
            if (ViewState["AddressCount"] == null)
            {
                ViewState["AddressCount"] = 0;
            }

            // Get the the number of the addresses.
            int addresscount = (int)ViewState["AddressCount"];

            // Iterat adding addresses input component.
            for (int i = 0; i < addresscount; i++)
            {
                AddAdress((i + 1).ToString());
            }
        }
        
        protected void btnAddAddress_Click(object sender, EventArgs e)
        {
            if (ViewState["AddressCount"] != null)
            {
                int btncount = (int)ViewState["AddressCount"];

                // Add a new component to pnlAddressContainer.
                AddAdress((btncount + 1).ToString());
                ViewState["AddressCount"] = btncount + 1;
            }
            else
            {
                Response.Write("ERROR");
                Response.End();
            }
        }

        protected void btnSave_Click(object sender, EventArgs e)
        {
            int addresscount = (int)ViewState["AddressCount"];

            // Display all the addresses on the page.
            // This is an imitation that we update these addresses into database.
            for (int i = 0; i < addresscount; i++)
            {
                TextBox tb = pnlAddressContainer.FindControl("TextBox" + (i + 1).ToString()) as TextBox;
                string address = tb.Text == "" ? "Empty" : tb.Text;
                Response.Write("Address" + (i + 1).ToString() + " is " + address + ".<br />");
            }

            // Clear the ViewState.
            ViewState["AddressCount"] = null;
        }

        protected void AddAdress(string id)
        {
            // Label to display address No.
            Label lb = new Label();
            lb.Text = "Address" + id + ": ";

            // TextBox for inputting the address.
            TextBox tb = new TextBox();
            tb.ID = "TextBox" + id;

            if (id != "1")
            {
                // Have a try on the code without this condition.
                // We will get a odd behaviour on this after clicking Save button.
                tb.Text = Request.Form[tb.ID];
            }

            // Button to check the Address.
            // Also to illustrate how to bind events to a dynamic control.
            Button btn = new Button();
            btn.Text = "Check";
            btn.ID = "Button" + id;

            // Bind event using += operator.
            btn.Click += new EventHandler(ClickEvent);

            Literal lt = new Literal() { Text = "<br />" };

            // Add these controls to pnlAddressContainer as a component.
            pnlAddressContainer.Controls.Add(lb);
            pnlAddressContainer.Controls.Add(tb);
            pnlAddressContainer.Controls.Add(btn);
            pnlAddressContainer.Controls.Add(lt);
        }

        protected void ClickEvent(object sender, EventArgs e)
        {
            // Get button instance from sender.
            Button btn = sender as Button;

            // Get TextBox instance and its value via FindControl() method.
            TextBox tb = pnlAddressContainer.FindControl(btn.ID.Replace("Button", "TextBox")) as TextBox;
            string address = tb.Text == "" ? "Empty" : tb.Text;

            // Alert a msg to show the address in the corresponding TextBox.
            System.Text.StringBuilder sb = new System.Text.StringBuilder();
            sb.Append("<script type=\"text/javascript\">");
            sb.Append("alert(\"Address" + btn.ID.Replace("Button", "") + " is " + address + ".\");");
            sb.Append("</script>");

            if (!ClientScript.IsClientScriptBlockRegistered(this.GetType(), "AlertClick"))
            {
                ClientScript.RegisterClientScriptBlock(this.GetType(), "AlertClick", sb.ToString());
            }
        }
    }
}