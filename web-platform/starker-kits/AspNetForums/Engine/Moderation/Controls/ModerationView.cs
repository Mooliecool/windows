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

namespace AspNetForums.Controls.Moderation {

    // *********************************************************************
    //  ModerationView
    //
    /// <summary>
    /// This server control provides moderators with a view of all the posts
    /// that require moderation for a given forum.
    /// </summary>
    // ********************************************************************/ 
    public class ModerationView : SkinnedForumWebControl {
        string skinFilename = "Moderation/Skin-ModerationView.ascx";
        Forum forum = null;
        HyperLink forumName;
        Label forumDescription;
        ThreadList threadList;

        // *********************************************************************
        // ModerationView
        //
        /// <summary>
        /// Constructor
        /// </summary>
        /// 
        // ********************************************************************/
        public ModerationView() : base() {
            // Assign a default template name
            if (SkinFilename == null)
                SkinFilename = skinFilename;
        }

        // *********************************************************************
        //  CreateChildControls
        //
        /// <summary>
        /// Loads a user control used as the 'template' for the control. A file
        /// not found exception is thrown if the user control is not found.
        /// </summary>
        /// 
        // ********************************************************************/ 
        protected override void CreateChildControls() {
            Control skin;

            // Load the control template
            skin = base.LoadSkin();

            // Initialize the control template
            InitializeSkin(skin);

            // Databind the thread list control
            PerformDataBindingForThreadList();

            // Add the control template to the output
            Controls.Add(skin);

        }

        // *********************************************************************
        //  PerformDataBindingForThreadList()
        //
        /// <summary>
        /// DataBinds the threadlist server control
        /// </summary>
        /// 
        // ********************************************************************/ 
        private void PerformDataBindingForThreadList() {
            ThreadCollection threads;

            // User specific data source ... note although we support paging, it's not being used.
            threads = Moderate.GetAllUnmoderatedThreads(ForumID, 25, 0, ForumUser.Username);

            // Do we have data to display?
            if (threads.Count == 0) {
                HttpContext.Current.Response.Redirect(Globals.UrlModeration);
                HttpContext.Current.Response.End();
            } else {
                threadList.DataSource = threads;
                threadList.DataBind();
            }
        }


        // *********************************************************************
        //  OnPreRender
        //
        /// <summary>
        /// Override OnPreRender and databind
        /// </summary>
        /// 
        // ********************************************************************/ 
        protected override void OnPreRender(EventArgs e) {

            // If we're in a postback the data might have changed
            if (Page.IsPostBack) {
                PerformDataBindingForThreadList();
            }

        }

        // *********************************************************************
        //  InitializeSkin
        //
        /// <summary>
        /// Initializes the user control loaded in CreateChildControls. Initialization
        /// consists of finding well known control names and wiring up any necessary events.
        /// </summary>
        /// 
        // ********************************************************************/ 
        override protected void InitializeSkin(Control skin) {
            // Ensure we have a valid forum
            try {
                forum = Forums.GetForumInfo(ForumID);
            } catch (Components.ForumNotFoundException) {
                Page.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UnknownForum));
                Page.Response.End();
            }

            // Find the forum name
            forumName = (HyperLink) skin.FindControl("ForumName");
            if (forumName != null) {
                forumName.Text = forum.Name;
                forumName.NavigateUrl = Globals.UrlShowForum + ForumID;
            }

            // Find the forum Description
            forumDescription = (Label) skin.FindControl("ForumDescription");
            if (forumDescription != null)
                forumDescription.Text = forum.Description;

            // Find the thread list
            threadList = (ThreadList) skin.FindControl("ThreadList");

        }
    }
}