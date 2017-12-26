/****************************** Module Header ******************************\
* Module Name:    CSASPNETPageValidationServerSide.cs
* Project:        CSASPNETPageValidation
* Copyright (c) Microsoft Corporation.
*
* The CSASPNETPageValidationServerSide page demonstrates the 
* ASP.NET validation controls to validate the value at the server side.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* History:
* * 7/30/2009 11:00 AM Zong-Qing Li Created
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
#endregion


namespace CSASPNETPageValidation
{
    public partial class CSASPNETPageValidationServerSide : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {

        }


        protected void RequiredFieldValidator1_Load(object sender, EventArgs e)
        {
            if (IsPostBack)
            {
                // Get which input TextBox will be validated.
                TextBox tx = (TextBox)this.FindControl(RequiredFieldValidator1.ControlToValidate);
                if (string.IsNullOrEmpty(tx.Text))
                {
                    RequiredFieldValidator1.ErrorMessage = "Required field cannot be left blank.";
                }
            }
        }

        protected void CustomValidator1_ServerValidate(object source, ServerValidateEventArgs args)
        {
            // Test whether the length of the value is more than 6 characters
            if (args.Value.Length >= 6)
            {
                args.IsValid = true;
            }
            else
            {
                args.IsValid = false;
            }
        }
    }
}
