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

namespace AspNetForums.Controls {

    [
    ParseChildren(true)	
    ]

    /// <summary>
    /// This Web control allows the user to create a new post or edit an existing post.
    /// The Mode property determines what action is being taken.  A value of NewPost, the
    /// default, constructs Web controls for creating a new post; a value of ReplyToPost
    /// assumes the person is replying to an existing post; a value of EditPost allows the
    /// user to edit an existing post.
    /// </summary>
    /// <remarks>
    /// When adding a new post, the ForumID must be specified, which indicates what forum the
    /// new post belongs to.  When replying to a post, the PostID property must be specified, indicating
    /// the post that is being replied to.  When editing a post, the PostID property must be
    /// specified, indicating the post to edit.  Failure to specify these required properties
    /// will cause an Exception to be thrown.
    /// </remarks>
    public class CreateEditPost : WebControl, INamingContainer {

        DropDownList pinnedPost;
        CheckBox allowNoReplies;
        ViewOptions postView = ViewOptions.Threaded;
        User user;

        /*************** Property and Class contants ****************/
        // the default messages for posting a new message, replying to a message, and editing a message
        const String _defaultPostNewMessageText = "Post a New Message";
        const String _defaultEditMessageText = "Edit an Existing Message";
        const String _defaultReplyToMessageText = "Reply to an Existing Message";
        const String _defaultPreviewMessageText = "Message Preview";
        readonly String _defaultRedirectUrl = Globals.UrlModeration;
		
        // define constants
        const int _subjectMaxLength = 50;
        /************************************************************/


        /********** DECLARE PRIVATE VARIABLES **************/
        // Create the more advanced, custom styles
        PostStyle _post = new PostStyle();
        PreviewPostSyle _previewPostStyle = new PreviewPostSyle();
        ReplyToPostStyle _replyToPostStyle = new ReplyToPostStyle();
        /***************************************************/

        public CreateEditPost() {

            // If we have an instance of context, let's attempt to
            // get the ForumID so we can save the user from writing
            // the code
            if (null != Context) {

                // Attempt to get the post id
                if (null != Context.Request.QueryString["PostID"]) {
                    this.PostID = Convert.ToInt32(Context.Request.QueryString["PostID"]);
                    this.Mode = CreateEditPostMode.ReplyToPost;
                } else if (null != Context.Request.Form["PostId"]) {
                    this.PostID = Convert.ToInt32(Context.Request.Form["PostID"]);
                    this.Mode = CreateEditPostMode.ReplyToPost;
                }

                // Attempt to get the forum id
                if (null != Context.Request.QueryString["ForumID"]) {
                    this.ForumID = Convert.ToInt32(Context.Request.QueryString["ForumID"]);
                    this.Mode = CreateEditPostMode.NewPost;
                } else if (null != Context.Request.Form["ForumID"]) {
                    this.ForumID = Convert.ToInt32(Context.Request.Form["ForumID"]);
                    this.Mode = CreateEditPostMode.NewPost;
                }

                // Do we have a redirect URL?
                if (null != Context.Request.QueryString["RedirectURL"]) {
                    RedirectURL = Context.Request.QueryString["RedirectURL"];
                } else if (null != Context.Request.Form["RedirectURL"]) {
                    RedirectURL = Context.Request.Form["RedirectURL"];
                }

                // If we don't have either a forum id or a post id we have an error
                if ((this.ForumID < 0) && (this.PostID < 0)) {
                    Page.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.PostDoesNotExist));
                    Page.Response.End();
                }

                // Is a mode specified?
                if (null != Context.Request.QueryString["Mode"]) {
                    string mode = Context.Request.QueryString["Mode"];

                    if (mode == "flat")
                        postView = ViewOptions.Flat;
                    else
                        postView = ViewOptions.Threaded;

                } else if (null != Context.Request.Form["Mode"]) {
                    string mode = Context.Request.Form["Mode"];

                    if (mode == "flat")
                        postView = ViewOptions.Flat;
                    else
                        postView = ViewOptions.Threaded;
                }
            }
        }

        /***********************************************************************
        // CreateChildControls
        //
        /// <summary>
        /// Create the child controls of this control
        /// </summary>
        ***********************************************************************/
        protected override void CreateChildControls() {

            // Create an instance of the user control used to format the display
            Control postForm;

            // Get the current user
            user = Users.GetLoggedOnUser();

            // Attempt to load the control. If this fails, we're done
            try {
                postForm = Page.LoadControl(Globals.ApplicationVRoot + "/skins/" + Globals.Skin + "/Skins/Skin-Post.ascx");
            }
            catch (FileNotFoundException) {
                throw new Exception("The user control skins/Skins/Skin-Post.ascx was not found. Please ensure this file exists in your skins directory");
            }

            // Set the ID
            postForm.ID = "PostForm";

            // Optionally display reply, post, and preview
            if (Mode != CreateEditPostMode.EditPost) {
                DisplayReply(postForm);
                DisplayPost(postForm);
                DisplayPreview(postForm);
            } else {
                DisplayEdit(postForm);
                DisplayPreview(postForm);
            }

            // All done. Add the control to the control collection
            Controls.Add(postForm);
        }


        private PostDetails GetPostForEdit() {
            PostDetails post = null;

            // Read in information about the post we are replying to
            try {
                post = Posts.GetPostDetails(PostID, Users.GetLoggedOnUser().Username);
            } catch (PostNotFoundException) {
                HttpContext.Current.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.PostDoesNotExist));
                HttpContext.Current.Response.End();
            }

            // If the user attempting to edit is the same user that posted and the user is trusted, or the user is a moderator...
            if (((post.Username.ToLower() == Users.GetLoggedOnUser().Username.ToLower()) && (Users.GetLoggedOnUser().IsTrusted)) || (Users.GetLoggedOnUser().IsModerator)) {
                return post;
            }

            throw new AspNetForums.Components.CannotEditPostException("User cannot edit post.");

        }

        /***********************************************************************
        // DisplayEdit
        //
        /// <summary>
        /// When a user replies to a post, the user control that controls the UI
        /// is loaded and passed to this method. Elements of the form are then wired
        /// up to handle events, such as button clicks
        /// </summary>
        /// <param name="control">Usercontrol used to control UI formatting</param>
        ***********************************************************************/
        private void DisplayEdit(Control controlTemplate) {
            PostDetails post = null;
            Label label;
            TextBox textbox;
            Button button;

            if (Mode != CreateEditPostMode.EditPost)
                return;

            // Get the post to edit
            try {
                post = GetPostForEdit();
            } catch (CannotEditPostException) {
                HttpContext.Current.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UnableToEditPost));
                HttpContext.Current.Response.End();
            }

            // Set the visibility
            ((Control) controlTemplate.FindControl("Edit")).Visible = true;
            ((Control) controlTemplate.FindControl("EditNotes")).Visible = true;

            // Set the title
            ((Label) controlTemplate.FindControl("PostTitle")).Text = Globals.HtmlDecode(EditMessageText);

            // Set the editor of the post
            ((Label) controlTemplate.FindControl("PostEditor")).Text = user.Username;

            // Set the Username
            label = (Label) controlTemplate.FindControl("PostAuthor");
            label.Text = post.Username;

            // Set the Subject
            textbox = (TextBox) controlTemplate.FindControl("PostSubject");
            textbox.Text = post.Subject;

            // Set the Body
            textbox = (TextBox) controlTemplate.FindControl("PostBody");
            textbox.Text = post.Body;

            // Find the checkbox
            allowNoReplies = (CheckBox) controlTemplate.FindControl("AllowReplies");

            // Wireup the preview button
            button = (Button) controlTemplate.FindControl("PreviewButton");
            button.Click += new System.EventHandler(PreviewButton_Click);

            // Wire up the cancel button
            button = (Button) controlTemplate.FindControl("Cancel");
            button.Click += new System.EventHandler(CancelButton_Click);

            // Wire up the post button
            button = (Button) controlTemplate.FindControl("PostButton");
            button.Click += new System.EventHandler(PostButton_Click);
        }

        /***********************************************************************
        // DisplayPost
        //
        /// <summary>
        /// When a user replies to a post, the user control that controls the UI
        /// is loaded and passed to this method. Elements of the form are then wired
        /// up to handle events, such as button clicks
        /// </summary>
        /// <param name="control">Usercontrol used to control UI formatting</param>
        ***********************************************************************/
        private void DisplayPost(Control controlTemplate) {
            Button button;

            // Set the title message
            if (Mode == CreateEditPostMode.NewPost)
                ((Label) controlTemplate.FindControl("PostTitle")).Text = _defaultPostNewMessageText;
            else 
                ((Label) controlTemplate.FindControl("PostTitle")).Text = _defaultReplyToMessageText;

            // Set the subject if necessary
            if (Mode == CreateEditPostMode.ReplyToPost) {
                HyperLink hyperlink;

                // Get the subject of the message we're replying to
                hyperlink = (HyperLink) controlTemplate.FindControl("ReplySubject");

                // Do we need to prepend 'Re: '?
                if (hyperlink.Text.StartsWith("Re: "))
                    ((TextBox) controlTemplate.FindControl("PostSubject")).Text = Globals.HtmlDecode(hyperlink.Text);
                else
                    ((TextBox) controlTemplate.FindControl("PostSubject")).Text = "Re: " + Globals.HtmlDecode(hyperlink.Text);
            }

            // Set the Username
            ((Label) controlTemplate.FindControl("PostAuthor")).Text = user.Username;

            // Allow pinned posts?
            if (((user.IsAdministrator) || (Moderate.CanModerate(user.Username))) && (Mode != CreateEditPostMode.ReplyToPost)) {
                controlTemplate.FindControl("AllowPinnedPosts").Visible = true;
                
                pinnedPost = (DropDownList) controlTemplate.FindControl("PinnedPost");
                pinnedPost.Items.Add(new ListItem("Do not pin post", "0"));
                pinnedPost.Items.Add(new ListItem("1 Day", "1"));
                pinnedPost.Items.Add(new ListItem("3 Days", "3"));
                pinnedPost.Items.Add(new ListItem("1 Week", "7"));
                pinnedPost.Items.Add(new ListItem("2 Weeks", "14"));
                pinnedPost.Items.Add(new ListItem("1 Month", "30"));
                pinnedPost.Items.Add(new ListItem("3 Months", "90"));
                pinnedPost.Items.Add(new ListItem("6 Months", "180"));
                pinnedPost.Items.Add(new ListItem("1 Year", "360"));
                pinnedPost.Items.Add(new ListItem("Announcement", "999"));

                // Do an autopost back incase we need to flip the allowNoReplies checkbox
                pinnedPost.AutoPostBack = true;
                pinnedPost.SelectedIndexChanged += new System.EventHandler(PinnedPost_Changed);
            }

            // Find the checkbox
            allowNoReplies = (CheckBox) controlTemplate.FindControl("AllowReplies");

            // Wireup the preview button
            button = (Button) controlTemplate.FindControl("PreviewButton");
            button.Click += new System.EventHandler(PreviewButton_Click);

            // Wire up the cancel button
            button = (Button) controlTemplate.FindControl("Cancel");
            button.Click += new System.EventHandler(CancelButton_Click);

            // Wire up the post button
            button = (Button) controlTemplate.FindControl("PostButton");
            button.Click += new System.EventHandler(PostButton_Click);
        }

        /***********************************************************************
        // PinnedPost_Changed
        //
        /// <summary>
        /// Event raised when the pinned post drop down list changes. If the user
        /// selected announcemnt we need to find the allow replies check box, check it,
        /// and then disable it.
        /// </summary>
        /// <param name="control">Usercontrol used to control UI formatting</param>
        ***********************************************************************/
        private void PinnedPost_Changed(Object sender, EventArgs e) {

            // Do we have an announcement?
            if (Convert.ToInt32(pinnedPost.SelectedItem.Value) == 999) {
                allowNoReplies.Checked = true;
                allowNoReplies.Enabled = false;
            } else {
                allowNoReplies.Checked = false;
                allowNoReplies.Enabled = true;
            }

        }

        /***********************************************************************
        // DisplayPreview
        //
        /// <summary>
        /// Displays a preview of a user's post to a message.
        /// </summary>
        /// <param name="control">Usercontrol used to control UI formatting</param>
        ***********************************************************************/
        private void DisplayPreview(Control control) {
            Button button;

            // Wire up the back button
            button = (Button) control.FindControl("BackButton");
            button.Click += new System.EventHandler(BackButton_Click);

            // Wire up the post button
            button = (Button) control.FindControl("PreviewPostButton");
            button.Click += new System.EventHandler(PostButton_Click);

        }

        /***********************************************************************
        // BackButton_Click
        //
        /// <summary>
        /// Event handler for the back button click from Preview mode
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        ************************************************************************/
        private void BackButton_Click(Object sender, EventArgs e) {
            Control form;

            // The event was raised by a button in the user control
            // the is the UI for the form -- get the Parent, e.g. the User Control
            form = ((Control)sender).Parent;

            // Find and hide the Preview display
            form.FindControl("Preview").Visible = false;

            if (Mode == CreateEditPostMode.NewPost)
                form.FindControl("ReplyTo").Visible = false;
            else if (Mode == CreateEditPostMode.ReplyToPost)
                form.FindControl("ReplyTo").Visible = true;

            // Find and enable the Post
            form.FindControl("Post").Visible = true;

        }
        
        /***********************************************************************
        // PostButton_Click
        //
        /// <summary>
        /// This event handler fires when the preview button is clicked.  It needs
        /// to show/hide the appropriate panels.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        ************************************************************************/
        private void PostButton_Click (Object sender, EventArgs e) {
            Control form;

            // Only proceed if the post is valid
            if (!Page.IsValid) 
                return;
			
            // Get the user control that the click originated from
            form = ((Control)sender).Parent;

            // When we add a new post, we want to get back the NewPostID, so that we
            // can automagically redirect the user to the page showing the post.
            // If iNewPostID comes back as 0, though, that means that the post needs
            // to be approved first, so the user is taken to a page explaining this.
            Post newPost = null; 
            Post postToAdd = new Post();			
			
            postToAdd.Username = Context.User.Identity.Name;
            postToAdd.ForumID = postToAdd.ParentID = 0;
            postToAdd.Subject = ((TextBox) form.FindControl("PostSubject")).Text;
            postToAdd.Body = ((TextBox) form.FindControl("PostBody")).Text;
            postToAdd.IsLocked = allowNoReplies.Checked;

            // Are we in edit mode?
            /*
            if (Mode == CreateEditPostMode.EditPost) {
                string editNotes = CreateEditNotes(form);
                postToAdd.Body = editNotes + postToAdd.Body;
            }
            */

            // Are we pinning the post?
            if ((pinnedPost != null) && (Convert.ToInt32(pinnedPost.SelectedItem.Value) > 0)) {
                
                switch (Convert.ToInt32(pinnedPost.SelectedItem.Value)) {

                    case 1:
                        postToAdd.PostDate = DateTime.Now.Date.AddDays(1);
                        break;

                    case 3:
                        postToAdd.PostDate = DateTime.Now.Date.AddDays(3);
                        break;

                    case 7:
                        postToAdd.PostDate = DateTime.Now.Date.AddDays(7);
                        break;

                    case 14:
                        postToAdd.PostDate = DateTime.Now.Date.AddDays(14);
                        break;

                    case 30:
                        postToAdd.PostDate = DateTime.Now.Date.AddMonths(1);
                        break;

                    case 90:
                        postToAdd.PostDate = DateTime.Now.Date.AddMonths(3);
                        break;

                    case 180:
                        postToAdd.PostDate = DateTime.Now.Date.AddMonths(6);
                        break;

                    case 360:
                        postToAdd.PostDate = DateTime.Now.Date.AddYears(1);
                        break;

                    case 999:
                        postToAdd.PostDate = DateTime.Now.Date.AddYears(25);
                        break;
                }
            }

            // Are we adding a new post, editing an existing one, or replying to an existing one?
            switch (Mode) {
                case CreateEditPostMode.NewPost:	// adding a new post
                    postToAdd.ForumID = ForumID;			// specify the forum ID that the new post belongs
					
                    try {
                        newPost = Posts.AddPost(postToAdd);
                    }
                    catch (PostDuplicateException) {
                        Context.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.DuplicatePost) + "&ForumId=" + ForumID);
                        Context.Response.End();
                    }
                    break;

                case CreateEditPostMode.ReplyToPost:	// replying to an existing post
                    try {
//                        if (postView == ViewOptions.Threaded) {
                            postToAdd.ParentID = PostID;			// specify the post we are replying to
//                        } else {
//                            postRepliedTo = Posts.GetPost(PostID, user.Username);
//                            postToAdd.ParentID = postRepliedTo.ThreadID;
//                        }

                        newPost = Posts.AddPost(postToAdd);
                    } 
                    catch (Components.PostNotFoundException) {
                        // uh-oh, something is off... are we replying to a message that has been deleted?
                        Context.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.ProblemPosting));
                        Context.Response.End();
                    }
                    catch (PostDuplicateException) {
                        Context.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.DuplicatePost) + "&PostId=" + PostID);
                        Context.Response.End();
                    }
                    break;

                case CreateEditPostMode.EditPost:
                    postToAdd.PostID = PostID;			// specify the ID of the post we are updating
                    string editedBy = Users.GetLoggedOnUser().Username;

                    // update the post
                    Posts.UpdatePost(postToAdd, editedBy);

                    // send the user back to from where they came
                    Context.Response.Redirect(RedirectURL);
                    Context.Response.End();

                    // exit from the event handler
                    return;					
            }

            // now that we've added the post, redirect the user to the post display (if the post 
            // has been approved)
            if (newPost.Approved) {
                if (Mode == CreateEditPostMode.NewPost)
                    Context.Response.Redirect(Globals.UrlShowPost + newPost.ThreadID);
                else
                    Context.Response.Redirect(Globals.UrlShowPost + newPost.ThreadID + "#" + newPost.PostID);
                Context.Response.End();
            } else {
                // if the post HASN'T been approved, send the user to an explanation page, passing along the Post or ForumID
                String strRedirect = Globals.UrlMessage + Convert.ToInt32(Messages.PostPendingModeration);
                
                if (ForumID > 0)
                    Context.Response.Redirect(strRedirect + "&ForumID=" + ForumID.ToString());
                else
                    Context.Response.Redirect(strRedirect + "&PostID=" + PostID.ToString());
            }        

        }

        
        /***********************************************************************
        // CancelButton_Click
        //
        /// <summary>
        /// Event raised when the user decides to cancel the post.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        ************************************************************************/
		private void CancelButton_Click(Object sender, EventArgs e) {
			string redirectUrl = null;
			
			if ( PostID > -1 ) {
				Post post = Posts.GetPost(PostID, Context.User.Identity.Name);
				redirectUrl = Globals.UrlShowPost + post.ThreadID + "#" + PostID;

			} else if ( ForumID > -1 ) {
				redirectUrl = Globals.UrlShowForum + ForumID;
			} else {
				redirectUrl = Globals.UrlHome;
			}
			
			Page.Response.Redirect(Globals.UrlHome);
			Page.Response.End();
		}

        /***********************************************************************
        // PreviewButton_Click
        //
        /// <summary>
        /// This event handler fires when the preview button is clicked.  It needs
        /// to show/hide the appropriate panels.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        ************************************************************************/
        private void PreviewButton_Click(Object sender, EventArgs e) {
            Control form;
            Label label;
            TextBox textbox;

            // only do this stuff if the page is valid
            if (!Page.IsValid) 
                return;
			
            // The event was raised by a button in the user control
            // the is the UI for the form -- get the Parent, e.g. the User Control
            form = ((Control)sender).Parent;

            // Find and enable the Preview display
            form.FindControl("Preview").Visible = true;

            // Find and hide the ReplyTo display and Post
            form.FindControl("ReplyTo").Visible = false;
            form.FindControl("Post").Visible = false;

            // Set the title text
            ((Label) form.FindControl("PostTitle")).Text = "Preview Message";

            // Preview the post subject
            label = (Label) form.FindControl("PreviewSubject");
            textbox = (TextBox) form.FindControl("PostSubject");
            label.Text = Globals.HtmlEncode(textbox.Text);

            // Preview the post body
            label = (Label) form.FindControl("PreviewBody");
            textbox = (TextBox) form.FindControl("PostBody");

            // Are we in edit mode?
            /*
            if (Mode == CreateEditPostMode.EditPost) {
                string editNotes = CreateEditNotes(form);
                label.Text = Globals.FormatPostBody(editNotes + textbox.Text) + Globals.FormatSignature(user.Signature);
            } else {
            }
            */
            label.Text = Globals.FormatPostBody(textbox.Text) + Globals.FormatSignature(user.Signature);

        }
		
        /***********************************************************************
        // CreateEditNotes
        //
        /// <summary>
        /// When a post is edited we add some notes to the post.
        /// </summary>
        ***********************************************************************/
        private string CreateEditNotes(Control form) {
            string editNotes;
            TextBox textbox;

            // Get the edit notes that the editor submitted
            textbox = (TextBox) form.FindControl("EditNotesBody");

            // Create the edit notes string
            editNotes = "[Edit by=\"" + user.Username + "\"]" + textbox.Text + "[/Edit]\n";

            return editNotes;
        }

        /***********************************************************************
        // DisplayReply
        //
        /// <summary>
        /// When a user replies to a post, the user control that controls the UI
        /// is loaded and passed to this method. Details such as the username, subject,
        /// and message are extracted and displayed.
        /// </summary>
        /// <param name="control">Usercontrol used to control UI formatting</param>
        ***********************************************************************/
        private void DisplayReply(Control control) {
            PostDetails post = null;
            HyperLink hyperlink;

            if (Mode == CreateEditPostMode.NewPost)
                return;

            // Set the visibility
            ((Control) control.FindControl("ReplyTo")).Visible = true;

            // Read in information about the post we are replying to
            try {
                post = Posts.GetPostDetails(PostID, Context.User.Identity.Name);
            } catch (Components.PostNotFoundException) {
                HttpContext.Current.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.PostDoesNotExist));
                HttpContext.Current.Response.End();
            }

            // Don't allow replies to locked posts
            if (post.IsLocked) {
                HttpContext.Current.Response.Redirect(Globals.UrlShowPost + PostID);
                HttpContext.Current.Response.End();
            }

            // Set the Username
            hyperlink = (HyperLink) control.FindControl("ReplyPostedBy");
            hyperlink.Text = post.Username;
            hyperlink.NavigateUrl = Globals.UrlUserProfile + post.Username;

            // Set the date
            ((Label) control.FindControl("ReplyPostedByDate")).Text = " on " + post.PostDate.ToString(user.DateFormat + " " + Globals.TimeFormat);

            // Set the Subject
            hyperlink = (HyperLink) control.FindControl("ReplySubject");
            hyperlink.Text = post.Subject;
            hyperlink.NavigateUrl = Globals.UrlShowPost + post.PostID;

            // Set the Body
            ((Label) control.FindControl("ReplyBody")).Text = Globals.FormatPostBody(post.Body);
        }
		

        /************ PROPERTY SET/GET STATEMENTS **************/
        /// <summary>
        /// Indicates the style and UI settings for the post.  Through this property you can set
        /// features like the columns and rows in the post body text box, and other such settings.
        /// <seealso cref="PostStyle"/>
        /// </summary>
        public PostStyle Post {
            get {  return _post;  }
        }

        /// <summary>
        /// Defines the style and UI settings for the Post Preview panel.
        /// <seealso cref="PreviewPostSyle"/>
        /// </summary>
        public PreviewPostSyle PreviewPost {
            get {  return _previewPostStyle;  }
        }

        /// <summary>
        /// Defines the style and settings for the panel that shows the post being replied to.
        /// <seealso cref="ReplyToPostStyle"/>
        /// </summary>
        public ReplyToPostStyle ReplyToPost {
            get {  return _replyToPostStyle;  }
        }

        /// <summary>
        /// Indicates whether the post being made is a new post, a reply to an existing post, or an
        /// editing of an existing post.  The allowable values are NewPost, ReplyToPost, and EditPost.
        /// </summary>
        /// <remarks>When setting the Mode property to NewPost, you must supply a ForumID property.
        /// When setting the Mode to ReplyToPost or EditPost, you must supply a PostID property.</remarks>
        public CreateEditPostMode Mode {
            get {
                if (ViewState["mode"] == null) return CreateEditPostMode.NewPost;
                return (CreateEditPostMode) ViewState["mode"];
            }
            set { ViewState["mode"] = value; }
        }

        /// <summary>
        /// This property determines the ForumID a new post is being posted to.
        /// <seealso cref="Mode"/>
        /// </summary>
        /// <remarks>Failure to pass in a ForumID when setting the Mode property to NewPost will result
        /// in a thrown Exception.</remarks>
        public int ForumID {
            get {
                if (ViewState["forumID"] == null) return -1;
                return (int) ViewState["forumID"];
            }
            set { ViewState["forumID"] = value; }
        }

        /// <summary>
        /// When editing a post, specifies the Url to send the end user once the post has been
        /// updates or the user clicks the Cancel button.
        /// </summary>
        public String RedirectURL {
            get {
                if (ViewState["redirectUrl"] == null) return _defaultRedirectUrl;
                return (String) ViewState["redirectUrl"];
            }
            set { ViewState["redirectUrl"] = value; }
        }

        /// <summary>
        /// This property is required when the Mode property is set to either ReplyToPost or EditPost.
        /// When the Mode is set to ReplyToPost, the PostID indicates the post that the end user is
        /// replying to.  When Mode is set to EditPost, the PostID indicates the post that is being edited.
        /// <seealso cref="Mode"/>
        /// </summary>
        /// <remarks>An Exception will be thrown if the Mode property is set to either EditPost or
        /// ReplyToPost and the PostID property is not set.</remarks>
        public int PostID {
            get {
                if (ViewState["postID"] == null) return -1;
                return (int) ViewState["postID"];
            }
            set { ViewState["postID"] = value; }
        }

        /// <summary>
        /// Specifies the text that appears at the top of the page when posting a new message.
        /// </summary>
        public String PostNewMessageText {
            get {
                if (ViewState["postNewMessageText"] == null) return _defaultPostNewMessageText;
                return (String) ViewState["postNewMessageText"];
            }
            set { ViewState["postNewMessageText"] = value; }
        }

        /// <summary>
        /// Specifies the message that appears at the top of the page when previewing a message.
        /// </summary>
        public String PreviewMessageText {
            get {
                if (ViewState["previewMessageText"] == null) return _defaultPreviewMessageText;
                return (String) ViewState["previewMessageText"];
            }
            set { ViewState["previewMessageText"] = value; }
        }

        /// <summary>
        /// Specifies the message that appears at the top of the page when editing an existing post.
        /// </summary>
        public String EditMessageText {
            get {
                if (ViewState["editMessageText"] == null) return _defaultEditMessageText;
                return (String) ViewState["editMessageText"];
            }
            set { ViewState["editMessageText"] = value; }
        }

        /// <summary>
        /// Specifies the message that appears at the top of the page when replying to an existing post.
        /// </summary>
        public String ReplyToMessageText {
            get {
                if (ViewState["replyToMessageText"] == null) return _defaultReplyToMessageText;
                return (String) ViewState["replyToMessageText"];
            }
            set { ViewState["replyToMessageText"] = value; }
        }
        /********************************************************/
    }



    /************************** PostStyle Class **************************
        This class represents the style and misc. UI options for making the post.
     *********************************************************************/
    /// <summary>
    /// This class represents the style and miscellaneous UI options for making the post.
    /// Using this class, the look and feel of the text boxes and buttons used for making a 
    /// new post can be customized.
    /// </summary>
	
    [ ToolboxItemAttribute(false) ]
    public class PostStyle : Style {
        /********** DECLARE PRIVATE CONSTANTS **************/
        const int _defaultSubjectColumns = 40;					// how many columns in the subject textbox
        const int _defaultBodyColumns = 75;						// how many columns in the body textbox
        const int _defaultBodyRows = 15;						// how many rows in the body textbox
        const String _defaultPostText = "  Post Message ";				// the default text for the Post button
        const String _defaultPreviewText = " Preview > ";			// the default text for the Preview button
        const String _defaultEditModePostText = "Post Altered Message";	// the default text for the Post button in Edit mode
        const String _defaultEditModePreviewText = " Preview ";	// the default text for the Preview button in Edit mode
        /***************************************************/


        /************ PROPERTY SET/GET STATEMENTS **************/
        /// <summary>
        /// Specifies how the Post and Preview buttons should be aligned.  The default is Right aligned.
        /// </summary>
        public HorizontalAlign ButtonHorizontalAlign {
            get {
                if (ViewState["postStyleButtonHZ"] == null) return HorizontalAlign.Right;
                return (HorizontalAlign) ViewState["postStyleButtonHZ"];
            }
            set {  ViewState["postStyleButtonHZ"] = value;  }
        }

        /// <summary>
        /// Indicates how many columns the subject text box should contain.  The default is 40.
        /// </summary>
        public int SubjectColumns {
            get {
                if (ViewState["postStyleSubjectColumns"] == null) return _defaultSubjectColumns;
                return (int) ViewState["postStyleSubjectColumns"];
            }
            set {  ViewState["postStyleSubjectColumns"] = value;  }
        }

        /// <summary>
        /// Indicates how many columns the multi-line Body text box should have.  The default is 75.
        /// </summary>
        public int BodyColumns {
            get {
                if (ViewState["postStyleBodyColumns"] == null) return _defaultBodyColumns;
                return (int) ViewState["postStyleBodyColumns"];
            }
            set {  ViewState["postStyleBodyColumns"] = value;  }
        }

        /// <summary>
        /// Indicates how many rows the multi-line Body text box should have.  The default is 15.
        /// </summary>
        public int BodyRows {
            get {
                if (ViewState["postStyleBodyRows"] == null) return _defaultBodyRows;
                return (int) ViewState["postStyleBodyRows"];
            }
            set {  ViewState["postStyleBodyRows"] = value;  }
        }

        /// <summary>
        /// Specifies the text for the Post button.
        /// </summary>
        public String PostText {
            get {
                if (ViewState["postStylePostText"] == null) return _defaultPostText;
                return (String) ViewState["postStylePostText"];
            }
            set {  ViewState["postStylePostText"] = value;  }
        }

        /// <summary>
        /// Specifies the text for the Preview button.
        /// </summary>
        public String PreviewText {
            get {
                if (ViewState["postStylePreviewText"] == null) return _defaultPreviewText;
                return (String) ViewState["postStylePreviewText"];
            }
            set { ViewState["postStylePreviewText"] = value; }
        }

        /// <summary>
        /// Specifies the text for the Post button when the user is editing a post (as opposed to
        /// posting a new message or replying to an existing message).
        /// </summary>
        public String EditModePostText {
            get {
                if (ViewState["postStyleEditModePostText"] == null) return _defaultEditModePostText;
                return (String) ViewState["postStyleEditModePostText"];
            }
            set {  ViewState["postStyleEditModePostText"] = value;  }
        }

        /// <summary>
        /// Specifies the text for the Preview button when the user is editing a post.
        /// </summary>
        public String EditModePreviewText {
            get {
                if (ViewState["postStyleEditModePreviewText"] == null) return _defaultEditModePreviewText;
                return (String) ViewState["postStyleEditModePreviewText"];
            }
            set { ViewState["postStyleEditModePreviewText"] = value; }
        }
        /********************************************************/
    }
    /**********************************************************************/



    /************************** PreviewPostSyle Class **************************
        This class represents the style and miscellaneous options for the Preview pane,
        which is displayed when the user clicks the Preview button.
     *********************************************************************/
    /// <summary>
    /// This class represents the style and miscellaneous options for the Preview pane,
    /// which is displayed when the user clicks the Preview button.
    /// </summary>
    [ ToolboxItemAttribute(false) ]
    public class PreviewPostSyle : Style {
        /********** DECLARE PRIVATE CONSTANTS **************/
        const String _defaultCancelText = "Cancel";				// the default text for the Cancel button
        const String _defaultPostText = "Post Message >>";		// the default text for the Post button
        const String _defaultReturnToEditText = "<< Edit the Post";	// the default text for the "Edit the Post" button
        const String _defaultEditModePostText = "Post Altered Message >>";		// the default text for the Post button when in Edit mode
        const String _defaultEditModeReturnToEditText = "<< Edit the Post";	// the default text for the "Edit the Post" button when in Edit mode
        /***************************************************/


        /************ PROPERTY SET/GET STATEMENTS **************/
        /// <summary>
        /// Determines how the preview panel should be horizontally aligned.  The default is
        /// Center align.
        /// </summary>
        public HorizontalAlign HorizontalAlign {
            get {
                if (ViewState["previewPostStyleHZ"] == null) return HorizontalAlign.Center;
                return (HorizontalAlign) ViewState["previewPostStyleHZ"];
            }
            set {  ViewState["previewPostStyleHZ"] = value;  }
        }

        /// <summary>
        /// Specifies the text in the Post button on the Preview pane.
        /// </summary>
        public String PostText {
            get {
                if (ViewState["previewPostStylePostText"] == null) return _defaultPostText;
                return (String) ViewState["previewPostStylePostText"];
            }
            set {  ViewState["previewPostStylePostText"] = value;  }
        }

        /// <summary>
        /// Specifies the text in the Cancel button on the Preview pane.
        /// </summary>
        /// <remarks>When the user clicks the Cancel button, she is taken back to the
        /// message she was replying to or the forum she was visiting when she clicked the "Post a
        /// New Message" link.</remarks>
        public String CancelText {
            get {
                if (ViewState["previewPostStyleCancelText"] == null) return _defaultCancelText;
                return (String) ViewState["previewPostStyleCancelText"];
            }
            set { ViewState["previewPostStyleCancelText"] = value; }
        }

        /// <summary>
        /// The text for the button to allow the user to return to edit the post.
        /// </summary>
        public String RetunToEditText {
            get {
                if (ViewState["previewPostStyleReturnToEditText"] == null) return _defaultReturnToEditText;
                return (String) ViewState["previewPostStyleReturnToEditText"];
            }
            set { ViewState["previewPostStyleReturnToEditText"] = value; }
        }

        /// <summary>
        /// The text for the button to allow the user to return to edit the post when the Mode property
        /// is set to EditPost.
        /// </summary>
        public String EditModeRetunToEditText {
            get {
                if (ViewState["previewPostStyleEditModeReturnToEditText"] == null) return _defaultEditModeReturnToEditText;
                return (String) ViewState["previewPostStyleEditModeReturnToEditText"];
            }
            set { ViewState["previewPostStyleEditModeReturnToEditText"] = value; }
        }

        /// <summary>
        /// Specifies the text in the Post button on the Preview pane when the Mode property is set
        /// to EditPost.
        /// </summary>
        public String EditModePostText {
            get {
                if (ViewState["previewEditModePostStylePostText"] == null) return _defaultEditModePostText;
                return (String) ViewState["previewEditModePostStylePostText"];
            }
            set {  ViewState["previewEditModePostStylePostText"] = value;  }
        }
        /********************************************************/
    }
    /**********************************************************************/
}