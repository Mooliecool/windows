using System;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums.Components;
using System.IO;

namespace AspNetForums.Controls {

    // *********************************************************************
    //  Message
    //
    /// <summary>
    /// Renders the appropriate error message passed in by a query string id value.
    /// </summary>
    // ********************************************************************/ 
    public class Message : SkinnedForumWebControl {
        string skinFilename = "Skin-Message.ascx";
        int messageId = -1;
        ForumMessage message;

        // *********************************************************************
        //  Message
        //
        /// <summary>
        /// Constructor
        /// </summary>
        //
        // ********************************************************************/
        public Message() : base() {

            // Assign a default template name
            if (SkinFilename == null)
                SkinFilename = skinFilename;

            // Get the error message id
            if (null != Context) {

                // Get the message id
                if (null != Context.Request.QueryString["MessageId"])
                    MessageID = Convert.ToInt32(Context.Request.QueryString["MessageId"]);
                else if (null != Context.Request.Form["MessageId"])
                    MessageID = Convert.ToInt32(Context.Request.Form["MessageId"]);

            }

            message = ForumMessages.GetMessage(MessageID);

        }

        // *********************************************************************
        //  Initializeskin
        //
        /// <summary>
        /// Initialize the control template and populate the control with values
        /// </summary>
        // ***********************************************************************/
        override protected void InitializeSkin(Control skin) {
            Label title;
            Label body;

            // Do some processing on the messages
            message.Body = message.Body.Replace("<UrlHome>", "<a href=\"" + Globals.UrlHome + "\">" + Globals.SiteName + " Home</a>");
            message.Body = message.Body.Replace("<UrlLogin>", "<a href=\"" + Globals.UrlLogin + "\">" + Globals.SiteName + " Login</a>");
            message.Body = message.Body.Replace("<UrlProfile>", "<a href=\"" + Globals.UrlEditUserProfile + "\">user profile</a>");
            
            // Handle duplicate post messages or moderation messages
            if ((message.Body.IndexOf("<DuplicatePost>") > 0) || (message.Body.IndexOf("<PendingModeration>") > 0)) {

                if (ForumID > 0) {
                    message.Body = message.Body.Replace("<DuplicatePost>", "<a href=\"" + Globals.UrlShowForum + ForumID.ToString() + "\">" + "Return to forum</a>");
                    message.Body = message.Body.Replace("<PendingModeration>", "<a href=\"" + Globals.UrlShowForum + ForumID.ToString() + "\">" + "Return to forum</a>");
                } else if (PostID > 0) {
                    message.Body = message.Body.Replace("<DuplicatePost>", "<a href=\"" + Globals.UrlShowPost + PostID.ToString() + "\">" + "Return to post</a>");
                    message.Body = message.Body.Replace("<PendingModeration>", "<a href=\"" + Globals.UrlShowPost + PostID.ToString() + "\">" + "Return to post</a>");
                }
            }

            // Find the title
            title = (Label) skin.FindControl("MessageTitle");
            if (title != null) {
                title.Text = message.Title;
            }

            // Find the title
            body = (Label) skin.FindControl("MessageBody");
            if (body != null) {
                body.Text = message.Body;
            }

        }

        // *********************************************************************
        //  MessageID
        //
        /// <summary>
        /// Property to control the message id
        /// </summary>
        // ***********************************************************************/
        public int MessageID {
            get { return messageId; }
            set { messageId = value; }
        }

    }
}