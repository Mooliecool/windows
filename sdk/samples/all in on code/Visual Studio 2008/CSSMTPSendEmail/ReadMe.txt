========================================================================
    CONSOLE APPLICATION : CSSMTPSendEmail Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

CSSMTPSendEmail demonstrates sending email with attachment and embedded image 
in the message body using SMTP server from a C# program.

With the introduction of .NET 2.0, the classes for sending email are packed 
in the System.Net.Mail namespace. In the example, we use the MailMessage and 
the SmtpClient classes.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Build an email object and set the basic email properties.

	MailMessage mail = new MailMessage();
	mail.To.Add("anyreceiver@anydomain.com");
	mail.From = new MailAddress("anyaddress@anydomain.com");
	...

2. Add an attachment of the email.

	mail.Attachments.Add(new Attachment(attachedFile));

3. Embed an image in the message body.

	AlternateView htmlView = AlternateView.CreateAlternateViewFromString(
		mail.Body, null, "text/html");
	LinkedResource imgLink = new LinkedResource(imgFile, "image/jpg");
	imgLink.ContentId = "image1";
	imgLink.TransferEncoding = System.Net.Mime.TransferEncoding.Base64;
	htmlView.LinkedResources.Add(imgLink);
	mail.AlternateViews.Add(htmlView);

4. Configure the SMTP client.

	SmtpClient smtp = new SmtpClient();
	smtp.Host = "smtp.live.com";
	smtp.Credentials = new NetworkCredential(
		"myaccount@live.com", "mypassword");
	smtp.EnableSsl = true;

5. Send the email.

	smtp.Send(mail);


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: SmtpClient Class
http://msdn.microsoft.com/en-us/library/system.net.mail.smtpclient.aspx

Sending Emails from C# Application using default SMTP
http://www.codeproject.com/KB/cs/Sending_Mails_From_C_.aspx


/////////////////////////////////////////////////////////////////////////////