========================================================================
    CONSOLE APPLICATION : VBSMTPSendEmail Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

VBSMTPSendEmail demonstrates sending email with attachment and embedded image 
in the message body using SMTP server from a VB.NET program.

With the introduction of .NET 2.0, the classes for sending email are packed 
in the System.Net.Mail namespace. In the example, we use the MailMessage and 
the SmtpClient classes.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Build an email object and set the basic email properties.

	Dim mail As New MailMessage
	mail.To.Add("anyreceiver@anydomain.com")
	mail.From = New MailAddress("anyaddress@anydomain.com")
	...

2. Add an attachment of the email.

	mail.Attachments.Add(New Attachment(attachedFile))

3. Embed an image in the message body.

	Dim htmlView As AlternateView = _
	AlternateView.CreateAlternateViewFromString(mail.Body, Nothing, "text/html")
	Dim imgLink As LinkedResource = New LinkedResource(imgFile, "image/jpg")
	imgLink.ContentId = "image1"
	imgLink.TransferEncoding = System.Net.Mime.TransferEncoding.Base64
	htmlView.LinkedResources.Add(imgLink)
	mail.AlternateViews.Add(htmlView)

4. Configure the SMTP client.

	Dim smtp As New SmtpClient
	smtp.Host = "smtp.live.com"
	smtp.Credentials = New NetworkCredential( _
	"myaccount@live.com", "mypassword")
	smtp.EnableSsl = True

5. Send the email.

	smtp.Send(mail)


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: SmtpClient Class
http://msdn.microsoft.com/en-us/library/system.net.mail.smtpclient.aspx


/////////////////////////////////////////////////////////////////////////////