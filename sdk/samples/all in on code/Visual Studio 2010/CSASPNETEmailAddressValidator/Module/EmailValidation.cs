/****************************** Module Header ******************************\
* Module Name:    EmailValidation.cs
* Project:        CSASPNETEmailAddresseValidator
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to send a confirmation Email to check whether an
* Email address is available.
* 
* In this file, we create a class which used as the main logic to validate  
* Email address by confirmation Email.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/


using System;
using System.Linq;
using System.Web;
using System.Net.Mail;
using System.Net;

namespace CSASPNETEmailAddresseValidator.Module
{

    // Create an enum class which we used to return the 
    // process status after confirmation Email sent.
    public enum ValidateEmailResult
    {
        EmailValidated,
        EmailValidating,
        EmailNotValidate,
        EmailStartToValidate
    }

    public class EmailValidation
    {

        // Some readonly fields to store the SMTP host infomation.
        // As we all know, if we want to send an Email, we need
        // a mail server which could send message to another mail
        // server, it could be called, SMTP server. Vice versa, 
        // the mail server which receive the message, we call it
        // POP server. Here 'MAILHOST' is the smtp server name or 
        // IP address, 'SENDMAILADDRESS' is the address which used
        // to send the confirmation Email, 'SENDMAILPASSWORD' is the
        // password of the address and 'NEEDSSL' is used that some 
        // SMTP need SSL to process the Email message exchanging, 
        // for example Hotmail.
        private readonly string mailHost;
        private readonly string sendMailAddress;
        private readonly string sendMailPassword;
        private readonly bool needSSL;

        public EmailValidation(string mailAddress, string password,
                                string hostserver, bool enableSSL)
        {
            mailHost = hostserver;
            sendMailAddress = mailAddress;
            sendMailPassword = password;
            needSSL = enableSSL;
        }

        // This method used to sending confirmation Email.
        // We use a simple database table to store the information about the
        // Email addresses which are ready for validation.
        public ValidateEmailResult StartToValidateEmail(string emailaddress)
        {

            // Use Linq to SQL to process the database queries.
            using (EmailAddressValidationDataContext context =
                new EmailAddressValidationDataContext())
            {

                // Check whether the email address has already been recorded
                // into the database.
                tblEmailValidation eval = context.tblEmailValidations.Where(
                    t => t.EmailAddress == emailaddress).FirstOrDefault();

                if (eval != null)
                {

                    // If yes, return the process of the validation.
                    if (eval.IsValidated)
                    {
                        return ValidateEmailResult.EmailValidated;
                    }
                    else
                    {
                        return ValidateEmailResult.EmailValidating;
                    }
                }

                // Generate a unique key to validate the address.
                string querykey = Guid.NewGuid().ToString().Replace("-", "");

                // Send the confirmation Email.
                SendValidationEmail(emailaddress, querykey);

                // If not, create a new record to the database.
                context.tblEmailValidations.InsertOnSubmit(new tblEmailValidation()
                {
                    EmailAddress = emailaddress,
                    IsValidated = false,
                    IsSendCheckEmail = false,
                    ValidatingStartTime = DateTime.Now,
                    ValidateKey = querykey
                });
                context.SubmitChanges();

                return ValidateEmailResult.EmailStartToValidate;

            }
        }

        // This method is used to get the current domain which will be
        // made as a link to send to the address.
        // User click the address from the confirmation Email and
        // the server will fire a Handler ,EmailAvailableValidationHandler, 
        // to update the record in the database to finish the validation.
        private string GetDomainURI()
        {
            if (HttpContext.Current == null)
            {
                throw new NullReferenceException("Need web context");
            }
            HttpRequest request = HttpContext.Current.Request;
            string rsl = "";
            rsl += request.ServerVariables["HTTPS"] == "on" ? "https://" : "http://";
            rsl += request.ServerVariables["SERVER_NAME"];
            rsl += (request.ServerVariables["SERVER_PORT"] != "80") ?
                    (":" + request.ServerVariables["SERVER_PORT"]) : "";
            return rsl;
        }

        // Here we use SmtpClient and MaillMessage classed to send the confirmation
        // Emaill.
        private void SendValidationEmail(string address, string querykey)
        {
            using (SmtpClient smtp = new SmtpClient(mailHost))
            {
                MailAddress from = new MailAddress(sendMailAddress, "Confirmation Email");
                MailAddress to = new MailAddress(address);

                using (MailMessage message = new MailMessage(from, to))
                {
                    message.IsBodyHtml = true;
                    message.Subject = "Confirmation Email";

                    // Here we only send a link for the last step of the validation.
                    // We can also create our own html styles to make it look better.
                    message.Body = string.Format("<a href='{0}/mail.axd?k={1}'>" +
                        "Please click here to finish email address validation.</a>",
                        GetDomainURI(), querykey);

                    smtp.DeliveryMethod = SmtpDeliveryMethod.Network;
                    smtp.UseDefaultCredentials = false;
                    smtp.Credentials = new NetworkCredential(sendMailAddress, sendMailPassword);
                    smtp.Port = 25;
                    smtp.EnableSsl = needSSL;
                    smtp.Send(message);
                }
            }
        }

        // This method is used to re-send the Confirmation Email.
        public void ReSendValidationEmail(string address)
        {
            using (EmailAddressValidationDataContext context =
                    new EmailAddressValidationDataContext())
            {

                tblEmailValidation eval = context.tblEmailValidations.Where(
                            t => t.EmailAddress == address).FirstOrDefault();
                if (eval != null)
                {
                    SendValidationEmail(address, eval.ValidateKey);
                }
            }

        }
    }
}