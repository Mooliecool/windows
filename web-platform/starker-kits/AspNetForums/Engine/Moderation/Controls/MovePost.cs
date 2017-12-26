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
    //  Movepost
    //
    /// <summary>
    /// This sever control is used to move posts from forum to forum and is
    /// only available to forum moderators.
    /// </summary>
    /// 
    // ********************************************************************/ 
    public class MovePost : SkinnedForumWebControl {

        string skinFilename = "Skin-MovePost.ascx";
        Label subject;
        Label hasReplies;
        Label postedBy;
        Label body;
        DropDownList moveTo;
        CheckBox sendEmail;
        HyperLink cancelMove;
        LinkButton move;

        // *********************************************************************
        //  MovePost
        //
        /// <summary>
        /// Constuctor
        /// </summary>
        /// 
        // ***********************************************************************/
        public MovePost() : base() {

            // Set the skin file
            if (SkinFilename == null)
                SkinFilename = skinFilename;


            // TODO: Add check to see if the user is a moderator

            if (base.ReturnURL == null) {
                throw new Exception("A ReturnURL value must be passed via QueryString or POST body");
            }
        }


        // *********************************************************************
        //  InitializeSkin
        //
        /// <summary>
        /// Initialize the control template and populate the control with values
        /// </summary>
        /// <param name="skin">Control instance of the skin</param>
        /// 
        // ***********************************************************************/
        override protected void InitializeSkin(Control skin) {

            // Get the post we want to move
            Post postToMove = Posts.GetPost(PostID, ForumUser.Username);

            // Display subject
            subject = (Label) skin.FindControl("Subject");
            if (null != subject) {
                subject.Text = postToMove.Subject;
            }

            // Has Replies?
            hasReplies = (Label) skin.FindControl("HasReplies");
            if (null != hasReplies) {
                
                if (postToMove.Replies > 0) {
                    hasReplies.Text = "true (" + postToMove.Replies + ") ";
                } else {
                    hasReplies.Text = "false ";
                }
            }

            // POsted By
            postedBy = (Label) skin.FindControl("PostedBy");
            if (null != postedBy) {
                postedBy.Text = postToMove.Username + " ";
            }

            // Body
            body = (Label) skin.FindControl("Body");
            if (null != body) {
                body.Text = Globals.FormatPostBody(postToMove.Body) + " ";
            }

            // Display the move to drop down list
            moveTo = (DropDownList) skin.FindControl("MoveTo");
            if (null != moveTo) {
                Forums forums = new Forums();
                string username = base.ForumUser.Username;
                moveTo.DataValueField = "Value";
                moveTo.DataTextField = "Text";
                moveTo.DataSource = forums.ForumListItemCollection(username, Forums.ForumListStyle.Nested);
                moveTo.DataBind();
                moveTo.Items.FindByValue("f-" + postToMove.ForumID).Selected = true;
            }

            // Cancel
            cancelMove = (HyperLink) skin.FindControl("CancelMove");
            if (null != cancelMove) {
                cancelMove.NavigateUrl = base.ReturnURL;
            }

            // Move Post
            move = (LinkButton) skin.FindControl("MovePost");
            if (null != move) {
                move.Click += new System.EventHandler(MovePost_Click);
            }

            // Send email
            sendEmail = (CheckBox) skin.FindControl("SendUserEmail");
        }

        // *********************************************************************
        //  MovePost_Click
        //
        /// <summary>
        /// Event handler for deleting a post
        /// </summary>
        /// 
        // ***********************************************************************/
        private void MovePost_Click(Object sender, EventArgs e) {

            // Are we valid?
            Moderate.MovePost(PostID, Convert.ToInt32(moveTo.SelectedItem.Value.Replace("f-", "")), ForumUser.Username, sendEmail.Checked);

            HttpContext.Current.Server.Transfer(ReturnURL);

        }
    }
}
