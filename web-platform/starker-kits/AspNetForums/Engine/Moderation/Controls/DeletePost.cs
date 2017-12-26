// TODO: Add logic to ensure only users in the Forum-Moderators group have access

using System;
using System.Drawing;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Components;
using System.ComponentModel;
using System.IO;
using System.Web.Security;

namespace AspNetForums.Controls.Moderation {

    // *********************************************************************
    //  DeletePost
    //
    /// <summary>
    /// This control is used by forum moderators to delete posts
    /// </summary>
    // ***********************************************************************/
    public class DeletePost : SkinnedForumWebControl {

        string skinFilename = "Skin-DeletePost.ascx";
        RequiredFieldValidator reasonValidation;
        TextBox reasonForDelete;
        CheckBox sendEmail;
        Label hasReplies;
        Post postToDelete;
        LinkButton deleteButton;
        HyperLink cancelButton;

        // *********************************************************************
        //  DeletePost
        //
        /// <summary>
        /// Constuctor
        /// </summary>
        // ***********************************************************************/
        public DeletePost() : base() {

            // Assign a default template name
            if (SkinFilename == null)
                SkinFilename = skinFilename;

            if (base.ReturnURL == null) {
                throw new Exception("A ReturnURL value must be passed via QueryString or POST body");
            }
        }


        // *********************************************************************
        //  Initializeskin
        //
        /// <summary>
        /// Initialize the control template and populate the control with values
        /// </summary>
        // ***********************************************************************/
        override protected void InitializeSkin(Control skin) {

            // Get the post we are deleting
            postToDelete = Posts.GetPost(PostID, ForumUser.Username);

            // Text box containing the reason why the post was deleted. This note will be
            // sent to the end user.
            reasonForDelete = (TextBox) skin.FindControl("DeleteReason");

            // Does the post have any replies?
            hasReplies = (Label) skin.FindControl("HasReplies");
            if (null != hasReplies) {
                if (postToDelete.Replies > 0)
                    hasReplies.Text = "true (" + postToDelete.Replies + ") ";
                else
                    hasReplies.Text = "false ";
            }

            // Are we sending email?
            sendEmail = (CheckBox) skin.FindControl("SendUserEmail");

            // Perform the delete
            deleteButton = (LinkButton) skin.FindControl("DeletePost");
            if (null != deleteButton)
                deleteButton.Click += new System.EventHandler(DeletePost_Click);

            // Cancel the delete
            cancelButton = (HyperLink) skin.FindControl("CancelDelete");
            if (null != cancelButton)
                cancelButton.NavigateUrl = base.ReturnURL;

            // Validator for reason to delete
            reasonValidation = (RequiredFieldValidator) skin.FindControl("ValidateReason");

        }

        // *********************************************************************
        //  DeletePost_Click
        //
        /// <summary>
        /// Event handler for deleting a post
        /// </summary>
        // ***********************************************************************/
        private void DeletePost_Click(Object sender, EventArgs e) {

            // Are we valid?
            if (reasonValidation.IsValid) {
				Post post = (Post) Posts.GetPost(PostID, null);
                Moderate.DeletePost(PostID, ForumUser.Username, reasonForDelete.Text, sendEmail.Checked);

                HttpContext.Current.Response.Redirect(Globals.UrlShowForum + post.ForumID);
                HttpContext.Current.Response.End();

            }

        }

        // *********************************************************************
        //  CancelDelete_Click
        //
        /// <summary>
        /// Event handler for canceling deletion of a post
        /// </summary>
        // ***********************************************************************/
        private void CancelDelete_Click(Object sender, EventArgs e) {
            HttpContext.Current.Response.Clear();
            HttpContext.Current.Response.Redirect(ReturnURL);
            HttpContext.Current.Response.End();
        }

    }
}
