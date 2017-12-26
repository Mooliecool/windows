/****************************** Module Header ******************************\
* Module Name:    EmailAvailableValidationHandler.cs
* Project:        CSASPNETEmailAddresseValidator
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to send a confirmation Email to check whether an
* Email address is available.
* 
* In this file, we create a HttpHandler which used to update the record in  
* the database and finish the validation. We need to config this Handler in
* the Web.config.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using System.Web;
using System.Web.Configuration;
using System.Configuration;
using CSASPNETEmailAddresseValidator.Module;
using System.Linq;

namespace CSASPNETEmailAddresseValidator.Handler
{
    public class EmailAvailableValidationHandler : IHttpHandler
    {
        public bool IsReusable
        {
            get { return true; }
        }


        public void ProcessRequest(HttpContext context)
        {

            // Get the unique key which is used to compare with
            // the key stored in the database.
            string key = context.Request.QueryString["k"];

            if (!string.IsNullOrEmpty(key))
            {
                using (EmailAddressValidationDataContext service =
                    new EmailAddressValidationDataContext())
                {
                    tblEmailValidation EValidation =
                        service.tblEmailValidations.Where(
                        t => t.ValidateKey.Trim() == key).FirstOrDefault();
                    if (EValidation != null)
                    {

                        // Update the record and make it as validated.
                        EValidation.IsValidated = true;
                        service.SubmitChanges();

                        // We can custom the output of the return message.
                        // Here is just a simple message.
                        context.Response.Write("Congratulation! Your Email Addess: " +
                            EValidation.EmailAddress + " has been validated!");
                    }
                    else
                    {
                        context.Response.Write("Please submit your address first.");
                    }
                }
            }
        }



    }
}
