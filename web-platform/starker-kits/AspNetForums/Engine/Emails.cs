using System;
using System.Web;
using System.Web.Mail;
using AspNetForums.Components;
using System.ComponentModel;

namespace AspNetForums {

    // *********************************************************************
    //  Emails
    //
    /// <summary>
    /// This class is responsible for sending out emails to users when certain events occur.  For example,
    /// when a user requests to be emailed their password, a method of this class is called to send the
    /// correct email template populated with the correct data to the correct user.
    /// </summary>
    /// <remarks>There are a number of email templates.  These templates can be viewed/edited via the Email
    /// Administration Web page.  The EmailTypeEnum enumeration contains a member for each of the potential
    /// email templates.</remarks>
    /// 
    // ********************************************************************/
    public class Emails {

        // *********************************************************************
        //  SendEmail
        //
        /// <summary>
        /// Sends a particular email type to a particular user.
        /// </summary>
        /// <param name="username">The user to send the email to.</param>
        /// <param name="emailType">The type of email to send.</param>
        /// <returns>A boolean, indicating whether or not the email was sent successfully.</returns>
        /// 
        // ********************************************************************/
        public static bool SendEmail(String username, EmailTypeEnum emailType) {
            return SendEmail(username, emailType, 0, null, null);
        }

        // *********************************************************************
        //  SendEmail
        //
        /// <summary>
        /// Sends a particular email to a particular user based on the contents of a particular post.
        /// </summary>
        /// <param name="Username">The user to send the email to.</param>
        /// <param name="EmailType">The type of email to send.</param>
        /// <param name="PostID">The particular post to base the email on.</param>
        /// <returns>
        /// A boolean, indicating whether or not the email was sent successfully.
        /// </returns>
        /// 
        // ********************************************************************/
        public static bool SendEmail(String username, EmailTypeEnum emailType, int postID) {
            return SendEmail(username, emailType, postID, null, null);
        }

        // *********************************************************************
        //  SendEmail
        //
        /// <summary>
        /// Sends a particular email to a particular user based on the contents of a particular post.
        /// </summary>
        /// <param name="Username">The user to send the email to.</param>
        /// <param name="EmailType">The type of email to send.</param>
        /// <param name="PostID">The particular post to base the email on.</param>
        /// <returns>
        /// A boolean, indicating whether or not the email was sent successfully.
        /// </returns>
        /// 
        // ********************************************************************/
        public static bool SendEmail(String username, EmailTypeEnum emailType, int postID, string bcc) {
            return SendEmail(username, emailType, postID, bcc, null);
        }


        // *********************************************************************
        //  SendEmail
        //
        /// <summary>
        /// This method sends an email to the user Username - the email template is identified by 
        /// the EmailID parameter.   
        /// </summary>
        /// <param name="Username">The user to send the email to.</param>
        /// <param name="EmailType">The specific email template.</param>
        /// <param name="PostID">Information on a particular post.</param>
        /// <param name="Bcc">A comma-separated list of email addresses to add to the Bcc property.
        /// This is used when there are numerous recipients for a particular email, such as the thread
        /// tracking email template.</param>
        /// <returns>A boolean, indicating whether or not the email was sent successfully.</returns>
        /// <remarks>When sending a deleted email messages, the strBcc parameter holds the
        /// *reason* why the message was deleted.</remarks>
        /// 
        // ********************************************************************/
        public static bool SendEmail(String username, EmailTypeEnum emailType, int postID, string bcc, string deleteReason) {			
    
            // if we don't wish to send emails at all, bail out now
            if (!Globals.SendEmail)
                return false;

            // both the username and bcc field can't be empty
            if (username.Length == 0 && bcc.Length == 0)
                return false;

            if (bcc == null)
                bcc = "";

            // if we were passed in a username, get the username info
            String strEmail = "";
            User user = null;
            int dbTimezoneOffset;
            int iTimezoneOffset = Globals.DBTimezone;

            dbTimezoneOffset = iTimezoneOffset;

            if (username.Length > 0) {

                user = Users.GetUserInfo(username, false);
                strEmail = user.Email;
                iTimezoneOffset = user.Timezone;
            }						

            // read in the email
            EmailTemplate email = GetEmailTemplateInfo((int) emailType);

            // Constrcut the email
            MailMessage msg = new MailMessage();

            if (bcc.Length > 0 && emailType != EmailTypeEnum.MessageDeleted) {
                msg.Bcc = bcc;
            } else {
                msg.To = strEmail;		// assumes strEmail was set
            }

            msg.From = email.From;
            msg.Subject = FormatEmail(email.Subject, user, postID, iTimezoneOffset, dbTimezoneOffset, null);
            msg.Priority = email.Priority;
            msg.Body = FormatEmail(email.Body, user, postID, iTimezoneOffset, dbTimezoneOffset, deleteReason) + "\n";
            SmtpMail.SmtpServer = Globals.SmtpServer;

            try {
                SmtpMail.Send(msg);
            } catch (Exception) {
                return false;
            }

            return true;
        }

        // *********************************************************************
        //  FormatEmail
        //
        /// <summary>
        /// This method formats a given string doing search/replace for markup
        /// </summary>
        /// <param name="messageToFormat">Message to apply formatting to</param>
        /// <param name="user">User the message is being sent to</param>
        /// <param name="timezoneOffset">User's timezone offset</param>
        /// <param name="dbTimezoneOffset">Database's timezone offset</param>
        /// <param name="postID">ID of the post the message is about</param>
        /// 
        // ********************************************************************/
        private static string FormatEmail(string messageToFormat, User user, int postID, int timezoneOffset, int dbTimezoneOffset, string deleteReason) {
            // now, before we send, we need to replace the tags in
            // messageToFormat with the appropriate values

            // we can only set the username/password if we got info on the user
            if (user != null) {
                messageToFormat = messageToFormat.Replace("<Username>", user.Username);
                messageToFormat = messageToFormat.Replace("<Email>", user.Email);
                messageToFormat = messageToFormat.Replace("<PublicEmail>", user.PublicEmail);
                messageToFormat = messageToFormat.Replace("<DateCreated>", user.DateCreated.ToString());
                messageToFormat = messageToFormat.Replace("<LastLogin>", user.LastLogin.ToString());
                messageToFormat = messageToFormat.Replace("<Password>", user.Password);
                messageToFormat = messageToFormat.Replace("<UrlProfile>", Globals.UrlEditUserProfile);
            }


            // set the timesent and sitename
            messageToFormat = messageToFormat.Replace("<TimeSent>", DateTime.Now.AddHours(timezoneOffset - dbTimezoneOffset).ToString());
            messageToFormat = messageToFormat.Replace("<ModerateUrl>", Globals.UrlModeration);
            messageToFormat = messageToFormat.Replace("<SiteName>", Globals.SiteName);
            messageToFormat = messageToFormat.Replace("<UrlLogin>", Globals.UrlLogin);
            messageToFormat = messageToFormat.Replace("<WebSiteUrl>", Globals.UrlWebSite);

            // set information about the post, if a post was referenced
            if (postID > 0) {
                // get information for the post
                Post post = Posts.GetPost(postID, Users.GetLoggedOnUser().Username);

                messageToFormat = messageToFormat.Replace("<PostedBy>", post.Username);
                messageToFormat = messageToFormat.Replace("<Subject>", post.Subject);
                messageToFormat = messageToFormat.Replace("<ForumName>", post.ForumName);
                messageToFormat = messageToFormat.Replace("<PostDate>", post.PostDate.AddHours(timezoneOffset - dbTimezoneOffset).ToString());
                messageToFormat = messageToFormat.Replace("<PostBody>", post.Body);
                messageToFormat = messageToFormat.Replace("<PostUrl>", Globals.UrlShowPost + post.ThreadID);

                // in the case of a deleted message, slap in the strBcc parameter into the <DeleteReasons> tag
                messageToFormat = messageToFormat.Replace("<DeleteReasons>", deleteReason);

            }

            return messageToFormat;
        }


        // *********************************************************************
        //  SendMessageApprovedEmail
        //
        /// <summary>
        /// This method sends an email to the user whose post has just been approved.
        /// </summary>
        /// <param name="PostID">Specifies the ID of the Post that was just approved.</param>
        /// 
        // ********************************************************************/
        public static void SendMessageApprovedEmail(int postID) {
            SendEmail(((Post) Posts.GetPost(postID, null)).Username, EmailTypeEnum.MessageApproved, postID);
        }


        // *********************************************************************
        //  SendMessageMovedAndApprovedEmail
        //
        /// <summary>
        /// This method sends an email to the user whose post has just been moved AND approved.
        /// </summary>
        /// <param name="PostID">Specifies the ID of the Post that was just approved.</param>
        /// 
        // ********************************************************************/
        public static void SendMessageMovedAndApprovedEmail(int postID) {
            // send the email
            SendEmail(((Post) Posts.GetPost(postID, null)).Username, EmailTypeEnum.MessageMovedAndApproved, postID);
        }

        // *********************************************************************
        //  SendMessageMovedAndNotApprovedEmail
        //
        /// <summary>
        /// This method sends an email to the user whose post has just been moved AND NOT approved
        /// </summary>
        /// <param name="PostID">Specifies the ID of the Post that was just approved.</param>
        /// 
        // ********************************************************************/
        public static void SendMessageMovedAndNotApprovedEmail(int postID) {
            // send the email
            SendEmail(((Post) Posts.GetPost(postID, null)).Username, EmailTypeEnum.MessageMovedAndNotApproved, postID);
        }


        // *********************************************************************
        //  SendThreadTrackingEmails
        //
        /// <summary>
        /// This method sends an email to all of those people who have subscribed
        /// to track a particular thread.  This function is called when a new
        /// message is added to the thread.
        /// <seealso cref="SendEmail"/>
        /// </summary>
        /// <param name="postID">The ID of the newly posted message.</param>
        /// <remarks>This method first obtains a list of all of those users who are
        /// subscribed to track the thread that the new email was added to.  It then
        /// calls SendEmail, passing along this information.</remarks>
        /// 
        // ********************************************************************/
        public static void SendThreadTrackingEmails(int postID) {
            string bcc = "";
            string username;

            // Get the username of the currently logged on user
            username = Users.GetLoggedOnUser().Username;

            // Get the list of user's we will send this mail to
            UserCollection users = GetEmailList(postID);

            // Add each user to the bcc list
            for (int i=0; i < users.Count; i++) {

                User user;

                // Get a user object
                user = (User) users[i];

                // ensure we don't send mail to the user that caused
                // this action
                if (username.ToLower() != user.Username.ToLower()) 
                    bcc += user.Email + ",";
            }

            // send the email
            SendEmail("", EmailTypeEnum.NewMessagePostedToThread, postID, bcc);
        }


        // *********************************************************************
        //  GetEmailList
        //
        /// <summary>
        /// Retrieves a list of email addresses from the users who are tracking a particular thread.
        /// </summary>
        /// <param name="PostID">The PostID of the new message.  We really aren't interested in this
        /// Post, specifically, but the thread it belongs to.</param>
        /// <returns>A UserCollection with the email addresses of those who want to receive
        /// notification when a message in this thread is replied to.</returns>
        /// 
        // ********************************************************************/
        private static UserCollection GetEmailList(int postID) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetEmailList(postID);
        }		


        // *********************************************************************
        //  GetEmailTemplateList
        //
        /// <summary>
        /// This method returns a list of all of the email templates in the Emails table.
        /// </summary>
        /// <returns>Returns an EmailTemplateCollection with all of the email templates.</returns>
        /// 
        // ********************************************************************/
        public static EmailTemplateCollection GetEmailTemplateList() {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetEmailTemplateList();
        }

        // *********************************************************************
        //  GetEmailTemplateList
        //
        /// <summary>
        /// This method returns the info for a particular email template.
        /// </summary>
        /// <param name="emailTemplateID">Specifies the numeric value of the EmailID.</param>
        /// <returns>An EmailTemplate containing the information about a particular
        /// email template.</returns>
        /// 
        // ********************************************************************/
        public static EmailTemplate GetEmailTemplateInfo(int emailTemplateID) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetEmailTemplateInfo(emailTemplateID);
        }

        // *********************************************************************
        //  UpdateEmailTemplate
        //
        /// <summary>
        /// This method updates an existing email template.
        /// </summary>
        /// <param name="email">An email template object containing information on the email template to
        /// update.</param>
        /// 
        // ********************************************************************/
        public static void UpdateEmailTemplate(EmailTemplate email) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            dp.UpdateEmailTemplate(email);
        }


        // *********************************************************************
        //  SendModeratorsNotification
        //
        /// <summary>
        /// Sends those moderators who are assigned to receive email notification an email indicating that
        /// a new post has been posted to a forum that they moderate.
        /// </summary>
        /// <param name="postID">The ID of the newly added post.</param>
        /// <remarks>This method should only be called when a new post is added and is awaiting moderation.</remarks>
        /// 
        // ********************************************************************/
        public static void SendModeratorsNotification(int postID) {
            // get a list of moderators who are interested in hearing about the
            // particular post
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            UserCollection users = dp.GetModeratorsInterestedInPost(postID);

            // loop through each record
            String bcc = "";

            for (int i=0; i < users.Count; i++)
                bcc += ((User) users[i]).Email + ",";

            // send the email
            SendEmail("", EmailTypeEnum.ModeratorEmailNotification, postID, bcc);
        }
    }
}