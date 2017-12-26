/**************************** Module Header ********************************\
* Module Name:    HighlightCodePage.aspx.cs
* Project:        CSASPNETHighlightCodeInPage
* Copyright (c) Microsoft Corporation
*
* Sometimes we input code like C# or HTML in our post and we need these code 
* to be highlighted for a better reading experience.The project illustrates how
* to highlight the code in a page. 

* This page is used to let user highlight the code. 

* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\***************************************************************************/

using System;
using System.Web.UI.WebControls;
using System.Collections;

namespace CSASPNETHighlightCodeInPage
{
    public partial class HighlightCodePage : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            this.lbError.Visible = false;
            this.lbResult.Visible = false;
        }

        protected void btnHighLight_Click(object sender, EventArgs e)
        {
            string _error = string.Empty;

            // Check the value of user's input data.
            if (CheckControlValue(this.ddlLanguage.SelectedValue, 
                this.tbCode.Text, out _error))
            {
                // Initialize the Hashtable variable which used to
                // store the different languages of code and their 
                // related regular expressions with matching options.
                Hashtable _htb = CodeManager.Init();

                // Initialize the suitable collection object.
                RegExp _rg = new RegExp();
                _rg = (RegExp)_htb[this.ddlLanguage.SelectedValue];
                this.lbResult.Visible = true;
                if (this.ddlLanguage.SelectedValue != "html")
                {
                    // Display the highlighted code in a label control.
                    this.lbResult.Text = CodeManager.Encode(
                        CodeManager.HighlightCode(
                        Server.HtmlEncode(this.tbCode.Text)
                        .Replace("&quot;", "\""),
                        this.ddlLanguage.SelectedValue, _rg)
                        );
                }
                else
                {
                    // Display the highlighted code in a label control.
                    this.lbResult.Text = CodeManager.Encode(
                        CodeManager.HighlightHTMLCode(this.tbCode.Text, _htb)
                        );
                }
            }
            else
            {
                this.lbError.Visible = true;
                this.lbError.Text = _error;
            }
        }
        public bool CheckControlValue(string selectValue, 
            string inputValue, 
            out string error)
        {
            error = string.Empty;
            if (selectValue == "-1")
            {
                error = "Please choose the language.";
                return false;
            }
            if (string.IsNullOrEmpty(inputValue))
            {
                error = "Please paste your code in the textbox control.";
                return false;
            }
            return true;
        }
    }
}