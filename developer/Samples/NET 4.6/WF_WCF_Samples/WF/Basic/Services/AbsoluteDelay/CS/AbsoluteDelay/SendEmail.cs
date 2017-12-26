//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Net.Mail;

namespace Microsoft.Samples.AbsoluteDelay
{
    // Defines a new custom CodeActivity called SendEmailActivity. 
    // We are using a base class of CodeActivity<TResult> which will inherit an OutArgument of Result. The value of Result will be determined by what is returned by Execute.
    public class SendEmail : CodeActivity<DateTime>
    {
        //specifies the In/Out arguments for this activity
        [RequiredArgument]
        public InArgument<string> SendMailSubject { get; set; }

        [RequiredArgument]
        public InArgument<string> SendMailBody { get; set; }

        [RequiredArgument]
        public InArgument<string> SendMailFrom { get; set; }

        [RequiredArgument]
        public InArgument<string> SendMailTo { get; set; }

        [RequiredArgument]
        public InArgument<string> SmtpHost { get; set; }

        protected override DateTime Execute(CodeActivityContext context)
        {
            //Creates a new E-mail message and sets mail properties to the values from the InArguments
            MailMessage message = new MailMessage()
            {
                Subject = this.SendMailSubject.Get(context),
                From = new MailAddress(this.SendMailFrom.Get(context)),
                Sender = new MailAddress(this.SendMailFrom.Get(context)),
            };

            message.Body = this.SendMailBody.Get(context);

            message.To.Add(new MailAddress(this.SendMailTo.Get(context)));

            SmtpClient client = new SmtpClient(this.SmtpHost.Get(context));
            client.UseDefaultCredentials = true;

            //retries sending an e-mail if the timeout it hit
            try
            {
                client.Timeout = 300;
                client.Send(message);
            }
            catch(Exception)
            { 
                client.Send(message);
            }
            return DateTime.Now;
        }
    }
}