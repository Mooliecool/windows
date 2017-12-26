/****************************** Module Header ******************************\
* Module Name:    Default.aspx.cs
* Project:        CSASPNETEmailAddresseValidator
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to send a confirmation Email to check whether an
* Email address is available.
* 
* In this file, we create some controls to call the codes which will send the 
* confirmation Email.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using CSASPNETEmailAddresseValidator.Module;

namespace CSASPNETEmailAddresseValidator
{
    public partial class Default : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
        }

        // If we click the Validate button, it will start to
        // send the confirmation Email.
        protected void btnValidate_Click(object sender, EventArgs e)
        {
            btnSendAgain.Visible = false;
            try
            {

                // Make an instance of the EmailValidation to be 
                // ready for sending confirmation Email.
                EmailValidation validator = new EmailValidation(
                    tbSendMail.Text,
                    Session["password"].ToString(),
                    tbHost.Text,
                    chkUseSSL.Checked);

                // Use StartToValidateEmail method to start validate
                // the Email address and send the confirmation Email.
                ValidateEmailResult rsl =
                    validator.StartToValidateEmail(tbValidateEmail.Text);
                switch (rsl)
                {
                    case ValidateEmailResult.EmailStartToValidate:
                        lbMessage.Text =
                            "The validation mail has send successfully." +
                            "Please check the email box.";
                        btnSendAgain.Visible = true;
                        break;
                    case ValidateEmailResult.EmailValidated:
                        lbMessage.Text = "This address has been validated.";
                        break;
                    case ValidateEmailResult.EmailValidating:
                        lbMessage.Text = "This address is waiting for user " +
                            "to click the validation link in his mail box";
                        btnSendAgain.Visible = true;
                        break;
                }
            }
            catch (Exception err)
            {
                lbMessage.Text = "Error:" + err.Message;
            }
        }

        // If we have send the confirmation Email, but the user still 
        // not receive the Email, we could let him send again.
        protected void btnSendEmailAgain_Click(object sender, EventArgs e)
        {
            try
            {

                // Make an instance of the EmailValidation to be 
                // ready for sending confirmation Email again.
                EmailValidation validator = new EmailValidation(
                    tbSendMail.Text,
                    Session["password"].ToString(),
                    tbHost.Text,
                    chkUseSSL.Checked);

                // Use ReSendValidationEmail method to re-send the 
                // confirmation Email.
                validator.ReSendValidationEmail(tbValidateEmail.Text);
                lbMessage.Text = "Email has been re-sent. Please check it again.";
            }
            catch (Exception err)
            {
                lbMessage.Text = "Error:" + err.Message;
            }
        }

        protected void ValidationWizard_OnNextButtonClick(object sender,
                                                WizardNavigationEventArgs e)
        {
            if (e.CurrentStepIndex == 0)
            {
                Session["password"] = tbSendMailPassword.Text;
            }
        }
    }
}