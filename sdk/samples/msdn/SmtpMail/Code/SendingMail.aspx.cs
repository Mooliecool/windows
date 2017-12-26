using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using System.Web.Mail;

namespace SendMail
{
	/// <summary>
	/// SendingMail is a simple application that shows two different ways to send email.
	/// One of the examples is repeated in order to show how to make an attachment.
	/// </summary>
	public class SendingMail : System.Web.UI.Page
	{
		protected System.Web.UI.WebControls.TextBox fromAddress;
		protected System.Web.UI.WebControls.TextBox sendToAddress;
		protected System.Web.UI.WebControls.TextBox messageSubject;
		protected System.Web.UI.WebControls.TextBox messageBody;
		protected System.Web.UI.WebControls.TextBox fileNameToAttach;
		protected System.Web.UI.WebControls.Button SendMessageNoMessageObj;
		protected System.Web.UI.WebControls.Button SendMessageWithFile;
		protected System.Web.UI.WebControls.TextBox SMTPServerName;
		protected System.Web.UI.WebControls.Label statusLabel;
		protected System.Web.UI.WebControls.Button SendMessage;
	
		private void Page_Load(object sender, System.EventArgs e)
		{
			


			
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN: This call is required by the ASP.NET Web Form Designer.
			//
			InitializeComponent();
			base.OnInit(e);
		}
		
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{    
			this.SendMessage.Click += new System.EventHandler(this.SendMessage_Click);
			this.SendMessageNoMessageObj.Click += new System.EventHandler(this.SendMessageNoMessageObj_Click);
			this.SendMessageWithFile.Click += new System.EventHandler(this.SendMessageWithFile_Click);
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion

		private void SendMessage_Click(object sender, System.EventArgs e)
		{
			/// <summary>
			/// Create a MailMessage object called myMail</summary>
			MailMessage myMail = new MailMessage();
			/// <summary>
			/// Set the From property for the email</summary>
			/// <value>
			/// The value must be an address that can be relayed by the mail server.</value>
			myMail.From= fromAddress.Text;
			/// <summary>
			/// Set the To property for the email</summary>
			/// <value>
			/// The value is the address to which you want to send the email.</value>
			myMail.To=sendToAddress.Text;
			/// <summary>
			/// Set the Subject property for the email</summary>
			/// <value>
			/// This property sets the subject for the email.</value>
			myMail.Subject=messageSubject.Text;
			/// <summary>
			/// Set the Body property for the email</summary>
			/// <value>
			/// This is the actual message. It can be text or HTML.</value>
			myMail.Body=messageBody.Text;
			/// <summary>
			/// Set the SmtpServer property of the SmtpMail class</summary>
			/// <value>
			/// This server is the outgoing mail server. Make sure it can relay messages matching the address in the From property.</value>
			SmtpMail.SmtpServer=SMTPServerName.Text;
			/// <summary>
			/// The Send method of the SmtpMail class will send the email to the address listed in the To property</summary>
			/// <value>
			/// The mail object is passed to the Send method</value>
			SmtpMail.Send (myMail);
			/// Update the status message
			statusLabel.Text = "Mail has been sent";
		}

		private void SendMessageNoMessageObj_Click(object sender, System.EventArgs e)
		{
		
			/// <summary>
			/// An alternative way to send an email is to use the SmtpMail class without
			/// creating a MailMessage object. Here you just pass some string values to 
			/// the Send method.</summary>
			
			/// <summary>
			/// Set the from string to indicate the sender of the email </summary>
			/// <value>
			/// The value must be an address that can be relayed by the mail server. </value>
			string from = fromAddress.Text;
			/// <summary>
			/// Set the to string to indicate the recipient of the email </summary>
			/// <value>
			/// The value is the address to which you want to send the email.</value>
			string to = sendToAddress.Text;
			/// <summary>
			/// Set the to string to indicate the subject of the email </summary>
			/// <value>
			/// The value is the subject of the email.</value>
			string subject = messageSubject.Text;
			/// <summary>
			/// Set the to string hold the body of the email </summary>
			/// <value>
			/// The value is the address to which you want to send the email.</value>
			string body = messageBody.Text;
			/// <summary>
			/// Set the to string to indicate the recipient of the email </summary>
			/// <value>
			/// The value is the address to which you want to send the email.</value>
			SmtpMail.SmtpServer=SMTPServerName.Text;
			/// Set the SmtpServer property of the SmtpMail class</summary>
			/// <value>
			/// This server is the outgoing mail server. Make sure it can relay messages matching the address in the From property.</value>
			SmtpMail.Send(from, to, subject, body);
			/// Update the status message
			statusLabel.Text = "Mail has been sent";
		}

		private void SendMessageWithFile_Click(object sender, System.EventArgs e)
		{
			/// <summary>
			/// Returning to the first method, you now see how to create an attachment.
			/// This is accomplished by creating a MailAttachment object and adding it
			/// to the Attachments collection of a MailMessage object.</summary>

			if (fileNameToAttach.Text == "" )
			{
				statusLabel.Text = "File name is required to send an attachment";
				return;
			}
			/// <summary>
			/// Create a MailMessage object called mailWithAttachment</summary>
			MailMessage mailWithAttachment = new MailMessage();
			/// <summary>
			/// Create a MailAttachment object called myAttachment</summary>
			/// <value>
			/// The name of the file to attach is passed into the MailAttachment constructor.</value>
			MailAttachment myAttachment = new MailAttachment(fileNameToAttach.Text);
			/// <summary>
			/// Add the file attachment to the MailMessage's Attachments collection</summary>
			/// <value>
			/// A MailAttachment object is added to the MailMessage's Attachments collection</value>
			mailWithAttachment.Attachments.Add(myAttachment);
			/// <summary>
			/// Set the From property for the email</summary>
			/// <value>
			/// The value must be an address that can be relayed by the mail server.</value>
			mailWithAttachment.From=fromAddress.Text;
			/// <summary>
			/// Set the Subject property for the email</summary>
			/// <value>
			/// This property sets the subject for the email.</value>
			mailWithAttachment.To=sendToAddress.Text;
			/// <summary>
			/// Set the Body property for the email</summary>
			/// <value>
			/// This is the actual message. It can be text or HTML.</value>
			mailWithAttachment.Body=messageBody.Text;
			/// <summary>
			/// Set the Subject property for the email</summary>
			/// <value>
			/// This property sets the subject for the email.</value>
			mailWithAttachment.Subject=messageSubject.Text;
			/// <summary>
			/// Set the SmtpServer property of the SmtpMail class</summary>
			/// <value>
			/// This server is the outgoing mail server. Make sure it can relay messages matching the address in the From property.</value>
			SmtpMail.SmtpServer=SMTPServerName.Text;
			/// <summary>
			/// The Send method of the SmtpMail class will send the email to the address listed in the To property</summary>
			/// <value>
			/// The mail object is passed to the Send method</value>
			SmtpMail.Send(mailWithAttachment);
			/// Update the status message
			statusLabel.Text = "Mail has been sent";
		}
	}
}
