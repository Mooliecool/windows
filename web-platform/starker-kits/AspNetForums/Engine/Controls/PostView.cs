using System;
using System.Drawing;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Controls.Moderation;
using AspNetForums.Components;
using System.ComponentModel;
using System.IO;

namespace AspNetForums.Controls {

    // *********************************************************************
    //  PostView
    //
    /// <summary>
    /// This Web controls how posts are displayed.
    /// </summary>
    /// <remarks>
    /// If the user attempts to view a post that has not yet been approved or a post that
    /// does not exist (perhaps one that has been deleted), they will be taken to the appropriate
    /// error message page. If the user attempts to load the control without specifying a PostID
    /// in the PostID property, an Exception is thrown.
    /// </remarks>
    /// 
    // ********************************************************************/    
    [
    ParseChildren(true)	
    ]
    public class PostView : WebControl, INamingContainer {

        Paging pager;
        PostList postList;
        ViewOptions postView = ViewOptions.Flat;
        Forum forum;
        bool defaultShowSignature = true;
        string defaultPostDateTimeFormat = "MMM d, yyyy - h:mm tt";
        int threshHold = 3;
        Control controlTemplate;
        string skinName;
        User user;
        DropDownList displayMode;
        DropDownList sortOrder;
        string templateName = "Skin-ShowPostList.ascx";
        CheckBox emailTracking;

        // *********************************************************************
        //  PostView
        //
        /// <summary>
        /// Class contructor - read in the PostId that was sent via the
        /// QueryString or Post body of the request
        /// </summary>
        /// 
        // ********************************************************************/ 
        public PostView() {

            // If we have an instance of context, let's attempt to
            // get the ForumID so we can save the user from writing
            // the code
            if (null != Context) {

                try {
                    if (null != Context.Request.QueryString["PostID"]) {
                        string postID = Context.Request.QueryString["PostID"];

                        // Contains a #
                        if (postID.IndexOf("#") > 0)
                            postID = postID.Substring(0, postID.IndexOf("#"));

                        this.PostID = Convert.ToInt32(postID);
                    } else if (null != Context.Request.Form["PostId"]) {
                        this.PostID = Convert.ToInt32(Context.Request.Form["PostID"]);
                    }
                } catch (Exception) {
                    HttpContext.Current.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.PostDoesNotExist));
                    HttpContext.Current.Response.End();
                }

                // Was a view option passed in via the querystring?
                if (null != Context.Request.QueryString["View"])
                    if (Context.Request.QueryString["View"] == "Threaded")
                        ViewMode = ViewOptions.Threaded;

                // Was a threshhold value passed in
                if (null != Context.Request.QueryString["ThreshHold"])
                    threshHold = Convert.ToInt32(Context.Request.QueryString["ThreshHold"]);

            }

            // Populate get details about the forum we are in
            try {
                forum = Forums.GetForumInfoByPostID(PostID);
            } catch (Components.ForumNotFoundException) {
                HttpContext.Current.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UnknownForum));
                HttpContext.Current.Response.End();
            }

        }

        // *********************************************************************
        //  CreateChildControls
        //
        /// <summary>
        /// This event handler adds the children controls. FIrst we apply templates
        /// for this control. We then determine how we render the post items, either threaded
        /// or flat. We also apply templates defined for each of the post item types.
        /// </summary>
        /// 
        // ********************************************************************/ 
        protected override void CreateChildControls() {

            // Attempt to get the user name
            if (Page.Request.IsAuthenticated) {
                user = Users.GetLoggedOnUser();
            }

            // Set the siteStyle for the page
            if (user != null)
                skinName = user.Skin;
            else
                skinName = Globals.Skin;

            // Set the view mode
            SetViewMode();

            // Call CreateChildControls
            base.CreateChildControls();
        }

        // *********************************************************************
        //  OnPreRender
        //
        /// <summary>
        /// Override OnPreRender and databind
        /// </summary>
        /// 
        // ********************************************************************/ 
        private void SetViewMode() {

            // Render a threaded or flat view
            if (ViewMode == ViewOptions.Threaded) {

                // Attempt to load the control. If this fails, we're done
                try {
                    controlTemplate = Page.LoadControl(Globals.ApplicationVRoot + "/skins/" + Globals.Skin + "/Skins/Skin-ShowPostThreaded.ascx");
                } catch (FileNotFoundException) {
                    throw new Exception("The user control skins/Skins/Skin-ShowPostThreaded.ascx was not found. Please ensure this file exists in your skins directory");
                }

                // Initialize the control template
                InitializePostThreadViewControlTemplate();

            } else {

                try {
                    controlTemplate = Page.LoadControl(Globals.ApplicationVRoot + "/skins/" + Globals.Skin + "/Skins/" + TemplateName);
                } catch (FileNotFoundException) {
                    throw new Exception("The user control skins/Skins/" + TemplateName + "was not found. Please ensure this file exists in your skins directory");
                }

                // Initialize the control template
                InitializePostListControlTemplate();

            }

            // Add the control template to the controls collection
            Controls.Add(controlTemplate);

        }

        // *********************************************************************
        //  InitializePostThreadViewControlTemplate
        //
        /// <summary>
        /// Render the post threaded view
        /// </summary>
        /// 
        // ********************************************************************/ 
        private void InitializePostThreadViewControlTemplate() {
            PostThreaded postThreaded;

            // Initialize common display elements
            InitializeCommonTemplateItems();

            // Get the threadview control
            postThreaded = (PostThreaded) controlTemplate.FindControl("ThreadView");
            postThreaded.PostID = PostID;

            // Set the threshhold
            postThreaded.Threshhold = ThreshHoldForThreadedView;

            Controls.Add(controlTemplate);
        }

        private void InitializeCommonTemplateItems() {
            System.Web.UI.WebControls.Image newThreadImage;
            HyperLink forumName;
            HyperLink newThreadLink;

            // Clear the controls collection
            Controls.Clear();

            // Find the forum name link
            forumName = (HyperLink) controlTemplate.FindControl("ForumName");
            if (forumName != null) {
                forumName.Text = forum.Name;
                forumName.NavigateUrl = Globals.UrlShowForum + forum.ForumID;
            }

            // Find the Display Mode control
            displayMode = (DropDownList) controlTemplate.FindControl("DisplayMode");
            if (displayMode != null) {
                displayMode.AutoPostBack = true;
                displayMode.SelectedIndexChanged += new System.EventHandler(DisplayMode_Changed);
            }

            // Find the email tracking option
            emailTracking = (CheckBox) controlTemplate.FindControl("TrackThread");
            if ((user != null) && (emailTracking != null)) {
                emailTracking.Visible = true;

                emailTracking.AutoPostBack = true;
                emailTracking.CheckedChanged += new System.EventHandler(Toggle_ThreadTracking);
                emailTracking.Checked = Posts.IsUserTrackingThread(PostID, user.Username);
            } else {
                emailTracking.Visible = false;
            }

        // Find the new thread link
        newThreadLink = (HyperLink) controlTemplate.FindControl("NewThreadLinkTop");
        if (newThreadLink != null) {

        newThreadLink.NavigateUrl = Globals.UrlAddNewPost + forum.ForumID;

        // Find the new thread iamge
        newThreadImage = (System.Web.UI.WebControls.Image) controlTemplate.FindControl("NewThreadImageTop");
        if (newThreadImage != null) {
        newThreadImage.ImageUrl = Globals.ApplicationVRoot + "/skins/" + skinName + "/images/newthread.gif";

    } else {
                    newThreadLink.Text = " New Thread ";
                }
            }
        }
        
        // *********************************************************************
        //  InitializePostListControlTemplate
        //
        /// <summary>
        /// Render the post list view
        /// </summary>
        /// 
        // ********************************************************************/ 
        private void InitializePostListControlTemplate() {

            // Initialize common display elements
            InitializeCommonTemplateItems();

            // Find the postList control
            postList = (PostList) controlTemplate.FindControl("PostList");

            // Ascending or descending
            sortOrder = (DropDownList) controlTemplate.FindControl("SortOrder");
            if (sortOrder != null) {

                if (user != null)
                    sortOrder.Items.FindByValue(Convert.ToInt32(user.ShowPostsAscending).ToString()).Selected = true;


                sortOrder.AutoPostBack = true;
                sortOrder.SelectedIndexChanged += new System.EventHandler(SortOrder_Changed);
            }

            // Find the pager
            pager = (Paging) controlTemplate.FindControl("Pager");
            pager.PageIndex_Changed += new System.EventHandler(PageIndex_Changed);

            // Get the total records used in the pager
            pager.TotalRecords = Threads.GetTotalPostsForThread(PostID);

            // Set up the email tracking check changed event
            postList.ThreadTracking_Changed += new System.EventHandler(Toggle_ThreadTracking);

            // Set the datasource
            // If we're in a post back someone else probably wants to render the view
            // Set the datasource
            if (user != null) {
                postList.DataSource = Posts.GetThreadByPostID(PostID, pager.PageIndex, pager.PageSize, 0, Convert.ToInt32(user.ShowPostsAscending));
                Posts.MarkPostAsRead(PostID, user.Username);
            } else {
                postList.DataSource = Posts.GetThreadByPostID(PostID, pager.PageIndex, pager.PageSize, 0, Convert.ToInt32(sortOrder.SelectedItem.Value));
            }
            postList.DataBind();

        }

        
        // *********************************************************************
        //  SortOrder_Changed
        //
        /// <summary>
        /// Event handler used when the user wants to toggle between flat and
        /// threaded view.
        /// </summary>
        /// 
        // ********************************************************************/ 
        private void SortOrder_Changed(Object sender, EventArgs e) {
            postList.DataSource = Posts.GetThreadByPostID(PostID, pager.PageIndex, pager.PageSize, 0, Convert.ToInt32(sortOrder.SelectedItem.Value));
        }

        // *********************************************************************
        //  DisplayMode_Changed
        //
        /// <summary>
        /// Event handler used when the user wants to toggle between flat and
        /// threaded view.
        /// </summary>
        /// 
        // ********************************************************************/ 
        private void DisplayMode_Changed(Object sender, EventArgs e) {

            if (displayMode.SelectedItem.Value == "Threaded") {
                ViewMode = ViewOptions.Threaded;
            } else {
                ViewMode = ViewOptions.Flat;
            }


            // Display the appropriate view
            SetViewMode();
        }

        // *********************************************************************
        //  Toggle_ThreadTracking
        //
        /// <summary>
        /// Event raised when the user wants to enable or disable thread tracking
        /// </summary>
        /// 
        // ********************************************************************/
        public void Toggle_ThreadTracking(Object sender, EventArgs e) {
            Posts.ReverseThreadTrackingOptions(user.Username, PostID);
        }

        // *********************************************************************
        //  PageIndex_Changed
        //
        /// <summary>
        /// Event raised when the selected index of the page has changed.
        /// </summary>
        /// 
        // ********************************************************************/
        private void PageIndex_Changed(Object sender, EventArgs e) {

            // Set the datasource
            postList.DataSource = Posts.GetThreadByPostID(PostID, pager.PageIndex, pager.PageSize, 0, Convert.ToInt32(sortOrder.SelectedItem.Value));
            postList.DataBind();

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
//            DataBind();
        }

        /// <summary>
        /// Specifies the ID for the post that you wish to view.
        /// </summary>
        /// <remarks>If this property is not specified, an Exception will be thrown.  If the PostID
        /// passed in refrences a post that does not exist, or one that has been posted to a moderated
        /// forum and has not yet been approved, the user will be automatically redirected to a message
        /// page explaining the situation.</remarks>
        [
        Category("Required"),
        Description("Specifies the ID for the post that you wish to view.")
        ]
        public int PostID {
            get {
                // the postID is stuffed in the ViewState so that
                // it is persisted across postbacks.
                if (ViewState["postID"] == null)
                    return -1;		// if it's not found in the ViewState, return the default value
					
                return Convert.ToInt32(ViewState["postID"].ToString());
            }
            set {
                // set the viewstate
                ViewState["postID"] = value;
            }
        }


        /// <summary>
        /// Specifies whether or not to show the poster's Signature in the post body content.
        /// </summary>
        [
        Category("Style"),
        Description("Specifies whether or not to show the poster's Signature in the post body content."),
        DefaultValue(true)
        ]
        public bool ShowSignature {
            get {
                if (ViewState["showSignature"] == null) 
                    return defaultShowSignature;
                return (bool) ViewState["showSignature"];
            }
            set { ViewState["showSignature"] = value; }
        }
		
        /// <summary>
        /// Indicates whether or not to show all of the messages in this thread.
        /// </summary>
        [
        Category("Style"),
        Description("Indicates whether or not to show all of the messages in this thread.")
        ]
        public ViewOptions ViewMode {
            get {
/*
                if (user != null) {
                    if (user.ViewPostsInFlatView)
                        return ViewOptions.Threaded;
                    else
                        return ViewOptions.Flat;
                } else {
*/
                    if (ViewState["PostViewMode"] == null)
                        return postView; 
                    else
                        return (ViewOptions) ViewState["PostViewMode"];
//                }
            }
            set { 
/*
                if (user != null)
                    Users.ToggleOptions(user.Username, user.HideReadThreads, value);
                else
*/
                    ViewState["PostViewMode"] = value; 
            }
        }

        /// <summary>
        /// Specifies the date/time format to display the post date in.
        /// The string should contain valid DateTimeFormatInfo characters.  For example, to display the
        /// date range as MM/DD/YYYY - HH:MM AM/PM, use the string: MM/dd/yyyy - HH:mm tt
        /// </summary>
        [
        Category("Style"),
        Description("Specifies the date/time format to display the post date in."),
        DefaultValue("MMM d, yyyy - h:mm tt")
        ]
        public String PostDateTimeFormat {
            get { return defaultPostDateTimeFormat; }
            set { defaultPostDateTimeFormat = value; }
        }

        // *********************************************************************
        //  Threshhold
        //
        /// <summary>
        /// Controls the depth at which the body of the thread is no longer displayed
        /// </summary>
        /// 
        // ********************************************************************/
        [
        Description("Controls the depth at which the body of the thread is no longer displayed.")
        ]
        public int ThreshHoldForThreadedView {
            get { return threshHold; }
            set {threshHold = value; }
        }

        public string TemplateName {
            get { return templateName; }
            set { templateName = value; }
        }
        
    }
}