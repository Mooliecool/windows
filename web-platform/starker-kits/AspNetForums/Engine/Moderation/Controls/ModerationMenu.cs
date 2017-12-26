// TODO: Move post should not also approve

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
    //  ModerationMenu
    //
    /// <summary>
    /// This server control provides moderators with options for managing posts.
    /// </summary>
    // ********************************************************************/ 
    public class ModerationMenu : SkinnedForumWebControl {

        string skinFilename = "Moderation/Skin-ModerationMenu.ascx";
        LinkButton approvePost;
        LinkButton approvePostAndTurnOffModeration;
        LinkButton approvePostAndReply;
        LinkButton approvePostAndEdit;
        LinkButton turnOffModerationForUser;
        HyperLink moderationHistory;
        HyperLink movePost;
        HyperLink moderateThread;
        HyperLink deletePost;
        HyperLink editPost;
        DropDownList approvePostAndMove;
        Label usernameLabel;
        Label postIDLabel;
        string usernamePostedBy;

        // *********************************************************************
        //  ModerationMenu
        //
        /// <summary>
        /// Constructor for moderation menu.
        /// </summary>
        // ***********************************************************************/
        public ModerationMenu() : base() {
            // Assign a default template name
            if (SkinFilename == null)
                SkinFilename = skinFilename;
        }

        // *********************************************************************
        //  Initializeskin
        //
        /// <summary>
        /// Initialize the control template and populate the control with values
        /// </summary>
        // ***********************************************************************/
        override protected void InitializeSkin(Control skin) {

            // We require a post id value
            if (PostID == -1)
                throw new AspNetForums.Components.PostNotFoundException("You must set the PostID property of this control to the post being moderated.");

            // Find the controls in our control template.

            // Approve Post
            approvePost = (LinkButton) skin.FindControl("ApprovePost");
            if (null != approvePost)
                approvePost.Command += new CommandEventHandler(ApprovePost_Click);

            // Approve Post and turn off moderation for the user
            approvePostAndTurnOffModeration = (LinkButton) skin.FindControl("ApprovePostAndTurnOffModeration");
            if (null != approvePostAndTurnOffModeration)
                approvePostAndTurnOffModeration.Command += new CommandEventHandler(ApprovePostAndTurnOffModeration_Click);

            // Approve Post and reply to the message
            approvePostAndReply = (LinkButton) skin.FindControl("ApprovePostAndReply");
            if (null != approvePostAndReply)
                approvePostAndReply.Command += new CommandEventHandler(ApprovePostAndReply_Click);

            // Approve Post and Edit
            approvePostAndEdit = (LinkButton) skin.FindControl("ApprovePostAndEdit");
            if (null != approvePostAndEdit)
                approvePostAndEdit.Command += new CommandEventHandler(ApprovePostAndEdit_Click);

            // Turn off moderation for this user
            turnOffModerationForUser = (LinkButton) skin.FindControl("TurnOffModeration");
            if (null != turnOffModerationForUser) {
                turnOffModerationForUser.CommandArgument = UsernamePostedBy;
                turnOffModerationForUser.Command += new CommandEventHandler(TurnOffModerationForUser_Command);
            }

            // Delete the post/thread
            deletePost = (HyperLink) skin.FindControl("DeletePost");
            if (null != deletePost) {
                // Go to the delete view
                deletePost.NavigateUrl = Globals.UrlDeletePost + PostID + "&ReturnURL=" + Page.Server.UrlEncode(Page.Request.RawUrl);
            }

            // Moderation History
            moderationHistory = (HyperLink) skin.FindControl("ModerationHistory");
            if (null != moderationHistory) {
                // Go to the delete view
                moderationHistory.NavigateUrl = Globals.UrlModerationHistory + PostID + "&ReturnURL=" + Page.Server.UrlEncode(Page.Request.RawUrl);
            }

            // Move the post/thread
            movePost = (HyperLink) skin.FindControl("MovePost");
            if (null != movePost) {

                if (ThreadID == PostID)
                    movePost.NavigateUrl = Globals.UrlMovePost + PostID + "&ReturnURL=" + Page.Server.UrlEncode(HttpContext.Current.Request.RawUrl);
                else
                    movePost.Enabled = false;
            }

            // Edit the post
            editPost = (HyperLink) skin.FindControl("EditPost");
            if (null != editPost) {
                editPost.NavigateUrl = Globals.UrlEditPost + PostID;
            }

            // Moderate Thread
            moderateThread = (HyperLink) skin.FindControl("ModerateThread");
            if (null != moderateThread) {
                moderateThread.NavigateUrl = Globals.UrlModerateThread + PostID;
            }

            // Approve and move the post to a new forum
            approvePostAndMove = (DropDownList) skin.FindControl("ApprovePostAndMove");
            if (null != approvePostAndMove) {
                Forums forums = new Forums();
                string username = base.ForumUser.Username;
                approvePostAndMove.AutoPostBack = true;
                approvePostAndMove.SelectedIndexChanged += new System.EventHandler(ApprovePostAndMove_Changed);
                approvePostAndMove.DataValueField = "Value";
                approvePostAndMove.DataTextField = "Text";
                approvePostAndMove.DataSource = forums.ForumListItemCollection(username, Forums.ForumListStyle.Flat);
                approvePostAndMove.DataBind();
                approvePostAndMove.Items.FindByValue("f-" + ForumID.ToString()).Selected = true;
            }

            // Username of user that created the post
            usernameLabel = (Label) skin.FindControl("Username");
            if (null != usernameLabel)
                usernameLabel.Text = UsernamePostedBy;

            // Display the PostID for the moderator
            postIDLabel = (Label) skin.FindControl("PostID");
            if (null != postIDLabel)
                postIDLabel.Text = PostID.ToString();
        }


        // *********************************************************************
        //  TurnOffModerationForUser_Command
        //
        /// <summary>
        /// Turn off moderation for a given user
        /// </summary>
        // ***********************************************************************/
        public void TurnOffModerationForUser_Command(Object sender, CommandEventArgs e) {

            LinkButton linkButton = (LinkButton) sender;

            // TODO Moderate.ToggleModeration(linkButton.CommandArgument);

        }

        // *********************************************************************
        //  ApprovePost_Click
        //
        /// <summary>
        /// Event handler to approve posts
        /// </summary>
        // ***********************************************************************/
        public void ApprovePost_Click(Object sender, CommandEventArgs e) {

            // Approve the post
            Moderate.ApprovePost(PostID, ForumUser.Username);

        }

        // *********************************************************************
        //  ApprovePostAndTurnOffModeration_Click
        //
        /// <summary>
        /// Event handler to approve a post and turn off moderation
        /// </summary>
        // ***********************************************************************/
        public void ApprovePostAndTurnOffModeration_Click(Object sender, CommandEventArgs e) {

            // Approve the post and turn off moderation for this user
            Moderate.ApprovePost(PostID, ForumUser.Username, UsernamePostedBy);

        }

        
        // *********************************************************************
        //  ApprovePostAndReply_Click
        //
        /// <summary>
        /// Event handler to approve a post and turn off moderation
        /// </summary>
        // ***********************************************************************/
        public void ApprovePostAndReply_Click(Object sender, CommandEventArgs e) {

            // Approve the post
            Moderate.ApprovePost(PostID, ForumUser.Username);

            // Now go the reply view
            HttpContext.Current.Response.Redirect(Globals.UrlReplyToPost + PostID);
            HttpContext.Current.Response.End();

        }

        // *********************************************************************
        //  ApprovePostAndEdit_Click
        //
        /// <summary>
        /// Event handler to edit a post
        /// </summary>
        // ***********************************************************************/
        public void ApprovePostAndEdit_Click(Object sender, CommandEventArgs e) {

            // Approve the post
            Moderate.ApprovePost(PostID, ForumUser.Username);

            // Now go the edit view
            HttpContext.Current.Response.Redirect(Globals.UrlEditPost + PostID);
            HttpContext.Current.Response.End();

        }

        // *********************************************************************
        //  ApprovePostAndMove_Changed
        //
        /// <summary>
        /// Event handler to approve and move a post
        /// </summary>
        // ***********************************************************************/
        public void ApprovePostAndMove_Changed(Object sender, EventArgs e) {
            MovedPostStatus status;

            // Move the post
            status = Moderate.MovePost(PostID, Convert.ToInt32(approvePostAndMove.SelectedItem.Value.Replace("f-", "")), ForumUser.Username, true);

            // Approve the post
            if (status != MovedPostStatus.NotMoved)
                Moderate.ApprovePost(PostID, ForumUser.Username);

        }

        // *********************************************************************
        //  Username
        //
        /// <summary>
        /// Name of the user that the post this moderation menu is applied to.
        /// </summary>
        // ***********************************************************************/
        public string UsernamePostedBy {
            get { return usernamePostedBy; }
            set { usernamePostedBy = value; }
        }

    }
}
