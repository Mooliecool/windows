/****************************** Module Header ******************************\
Module Name:    Program.cs
Project:        CSSMTPSendEmail
Copyright (c) Microsoft Corporation.

CSSMTPSendEmail demonstrates sending email with attachment and embedded 
image in the message body using SMTP server from a C# program.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Net.Mail;
using System.Net;


class Program
{
    static void Main(string[] args)
    {
        //
        // Build a mail object.
        //

        Console.WriteLine("Build email object");

        MailMessage mail = new MailMessage();
        mail.To.Add("anyreceiver@anydomain.com");
        mail.From = new MailAddress("anyaddress@anydomain.com");
        mail.Subject = "Test email of All-In-One Code Framework - CSSMTPSendEmail";
        mail.Body = "Welcome to <a href='http://1code.codeplex.com'>All-In-One Code Framework</a>!";
        mail.IsBodyHtml = true;
        
        // Attachments
        Console.WriteLine("Add attachment");
        string attachedFile = "<attached file path>";
        mail.Attachments.Add(new Attachment(attachedFile));

        // Embedded image in the message body
        Console.WriteLine("Embed image");
        mail.Body += "<br/><img alt=\"\" src=\"cid:image1\">";

        string imgFile = "<image file path>";
        AlternateView htmlView = AlternateView.CreateAlternateViewFromString(
            mail.Body, null, "text/html");
        LinkedResource imgLink = new LinkedResource(imgFile, "image/jpg");
        imgLink.ContentId = "image1";
        imgLink.TransferEncoding = System.Net.Mime.TransferEncoding.Base64;
        htmlView.LinkedResources.Add(imgLink);
        mail.AlternateViews.Add(htmlView);

        // 
        // Configure the SMTP client and send the email.
        // 

        // Configure the SMTP client
        SmtpClient smtp = new SmtpClient();
        smtp.Host = "smtp.live.com";
        smtp.Credentials = new NetworkCredential(
            "myaccount@live.com", "mypassword");
        smtp.EnableSsl = true;

        // Send the email
        Console.WriteLine("Sending email...");
        smtp.Send(mail);
    }
}