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

namespace AspNetForums.Controls.Admin {

    /// <summary>
    /// This Web control displays a list of the posts for a particular forum and allows the
    /// administrator (hopefully not a regular user) to edit and delete these posts.  Note that
    /// deleting a post deletes the post and all of its replies.
    /// </summary>
    /// <remarks>
    /// Note that only those posts that fall within the first "page" of posts are displayed for
    /// the forum.  No means to navigate backward or forward is present.  This is a bit restrictive, 
    /// since it limits the administrator to being able to only	edit or delete posts in a forum that 
    /// fall within the default timespan for the forum.
    /// </remarks>
    [
        ParseChildren(true),
        Designer("WebForumsControls.Design", "AdminForumPostListing")
    ]
    public class AdminForumPostListing : WebControl, INamingContainer {

        // Create the variables to create the DataList
        DataList dlForumView;
        DataBoundLiteralControl dataBoundLiteralControl;
        int iUserTimezone = Globals.DBTimezone;
        DeletePostStyle deletePostStyle;

        // the default view of the forum posts
        const ViewOptions defaultViewOptions = ViewOptions.Threaded;

        // the default paths to the edit and delete images
        const String defaultEditPostIconUrl = "edit.gif";
        const String defaultDeletePostIconUrl = "delete.gif";

        // whether or not to display the "Reasons for Deleting the Post" textbox
        const bool defaultShowReasonsTextBox = true;			

        const String defaultIndentString = "&nbsp;&nbsp;&nbsp;&nbsp;";
        const String defaultThreadSeparator = "<hr noshade size=\"1px\">\n";

        // *********************************************************************
        //
        //  AdminForumPostListing Constructor
        //
        /// <summary>
        /// The constructor simply creates an instance of the DataList and sets
        /// the ItemTemplate to a new CompiledTemplateBuilder instance.
        /// </summary>
        //
        // ********************************************************************/
        public AdminForumPostListing() {

            deletePostStyle = new DeletePostStyle();

            // create an instance of the datalist and set its properties
            dlForumView = new DataList();
            dlForumView.ID = "dlForumView";

            // set the ItemTemplate property
            dlForumView.ItemTemplate = new CompiledTemplateBuilder(new BuildTemplateMethod(BuildItemTemplate));

        }

        // *********************************************************************
        //
        //  HandleImageButtonDataBind Event Handler
        //
        /// <summary>
        /// This event handler fires when the DataList is databound.  Here we
        /// must set the ImageButton items' CommandArgument property to the
        /// current PostID.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        //
        // ********************************************************************/
        private void HandleImageButtonDataBind(Object sender, EventArgs e) {	

            ImageButton target = (ImageButton) sender;
            DataListItem Container = (DataListItem) target.NamingContainer;

            // Set the ImageButton's CommandArgument
            target.CommandArgument = DataBinder.Eval(Container.DataItem, "PostID").ToString();

        }

        // *********************************************************************
        //
        //  BeginDataBinding Event Handler
        //
        /// <summary>
        /// This event handler is called when the DataList's DataBound method is
        /// called.  This event	handler needs to add the DataBoundStrings to the 
        /// DataListItem.  Since the ImageButtons break up the DataBinding Literal
        /// and DataBound strings, we have two functions: BeginDataBinding and
        /// FinishDataBinding.  This one, begin, is responsible for the databound
        /// content BEFORE the imageButtons.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        //
        // ********************************************************************/
        private void BeginDataBinding(Object sender, EventArgs e) {	

            DataBoundLiteralControl target = (DataBoundLiteralControl) sender;
            DataListItem Container = (DataListItem) target.NamingContainer;

            // Add the DataBoundString
            target.SetDataBoundString(0, DisplayIndent(Convert.ToInt32(DataBinder.Eval(Container.DataItem, "PostLevel"))));

        }

        // *********************************************************************
        //
        //  FinishDataBinding Event Handler
        //
        /// <summary>
        /// This event handler is called when the DataList's DataBound method is
        /// called.  This event	handler needs to add the DataBoundStrings to the 
        ///	DataListItem.  Since the ImageButtons break up the DataBinding Literal
        ///	and DataBound strings, we have two functions: BeginDataBinding and
        ///	FinishDataBinding.  This one, finish, is responsible for the databound
        ///	content AFTER the imageButtons.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        //
        // ********************************************************************/
        private void FinishDataBinding(Object sender, EventArgs e) {	

            DataBoundLiteralControl target = (DataBoundLiteralControl) sender;
            DataListItem Container = (DataListItem) target.NamingContainer;

            // Add the DataBoundStrings
            target.SetDataBoundString(0, Convert.ToString(DataBinder.Eval(Container.DataItem, "PostID")));
            target.SetDataBoundString(1, DataBinder.Eval(Container.DataItem, "Subject").ToString());			

            // we have to add other strings if we need to show the username / dateposted
            if (ShowUsername) {

                // we need to show the username
                target.SetDataBoundString(2, Context.Server.UrlEncode(DataBinder.Eval(Container.DataItem, "Username").ToString()));
                target.SetDataBoundString(3, DataBinder.Eval(Container.DataItem, "Username").ToString());

                // we need to show the username AND the DatePosted
                if (ShowDatePosted)
                    target.SetDataBoundString(4, Users.AdjustForTimezone(Convert.ToDateTime(DataBinder.Eval(Container.DataItem, "PostDate", "{0:g}")), iUserTimezone));

            // we need to show JUST the DatePosted
            } else if (ShowDatePosted) {
                
                target.SetDataBoundString(2, Users.AdjustForTimezone(Convert.ToDateTime(DataBinder.Eval(Container.DataItem, "PostDate", "{0:g}")), iUserTimezone));

            }

        }


        // *********************************************************************
        //
        //  BeginBuildTemplate
        //
        /// <summary>
        /// This function creates a new DataBoundLiteralControl and populates
        ///	the Static Strings.  Additionally, the DataBinding event handler is
        ///	wired up to a local event handler.  Note that there are two BuildItemTemplate
        ///	functions: BeginBuildItemTemplate and FinishBuildItemTemplate.  This
        ///	one, begin, is responsible for constructing the code BEFORE the ImageButtons.
        /// </summary>
        /// <returns></returns>
        //
        // ********************************************************************/
        private Control BeginBuildItemTemplate() {

            // Create a new DataBoundLiteralControl with 8 literal items and 7 databound items
            dataBoundLiteralControl = new DataBoundLiteralControl(1,1);

            // we really don't want to add any static strings, but we need to in order to have the
            // databound string we wish to emit to appear.  Blah.
            dataBoundLiteralControl.SetStaticString(0, "\n");

            // wire up the handleDataBinding event handler to the DataBinding event
            dataBoundLiteralControl.DataBinding += new System.EventHandler(BeginDataBinding);			

            // return the DataBoundLiteralControl
            return dataBoundLiteralControl;		

        }


		
        // *********************************************************************
        //
        //  FinishBuildItemTemplate
        //
        /// <summary>
        /// This function creates a new DataBoundLiteralControl and populates
        ///	the Static Strings.  Additionally, the DataBinding event handler is
        ///	wired up to a local event handler.  Note that there are two BuildItemTemplate
        ///	functions: BeginBuildItemTemplate and FinishBuildItemTemplate.  This
        ///	one, finish, is responsible for constructing the code AFTER the ImageButtons.
        /// </summary>
        /// <returns></returns>
        //
        // ********************************************************************/
        private Control FinishBuildItemTemplate() {

            // Create a new DataBoundLiteralControl with literal items and databound items
            dataBoundLiteralControl = new DataBoundLiteralControl(6,5);

            dataBoundLiteralControl.SetStaticString(0, "<a class=\"postSubject\" href=\"" + Globals.UrlShowPost);
            dataBoundLiteralControl.SetStaticString(1, "\" target=\"" + FrameToOpenPosts + "\">");			

            // show the username, if needed
            if (ShowUsername) {

                // we are showing the username
                dataBoundLiteralControl.SetStaticString(2, "</a>\n<span class=\"postText\">from </span><a class=\"postUsername\" href=\"" + Globals.UrlUserProfile);
                dataBoundLiteralControl.SetStaticString(3, "\">");

                if (ShowDatePosted) {

                    // we are showing the username AND the date posted
                    dataBoundLiteralControl.SetStaticString(4, "</a><span class=\"postText\">, posted </span><span class=\"postDate\">");
                    dataBoundLiteralControl.SetStaticString(5, "</span>\n");

                } else {

                    // we are showing just the username
                    dataBoundLiteralControl.SetStaticString(4, "</a>");

                }

            } else if (ShowDatePosted) {

                // we are showing just the date posted
                dataBoundLiteralControl.SetStaticString(2, "</a><span class=\"postText\">, posted </span><span class=\"postDate\">");
                dataBoundLiteralControl.SetStaticString(3, "</span>\n");

            } else if (!ShowUsername && !this.ShowDatePosted) {

                // we are showing neither the username nor the date posted
                dataBoundLiteralControl.SetStaticString(2, "</a>\n");

            }

            // wire up the handleDataBinding event handler to the DataBinding event
            dataBoundLiteralControl.DataBinding += new System.EventHandler(FinishDataBinding);			

            // return the DataBoundLiteralControl
            return dataBoundLiteralControl;		
        }


        // *********************************************************************
        //
        //  BuildItemTemplate
        //
        /// <summary>
        /// This function is called to create the template for the datalist.
        /// It calls BeginBuildItemTemplate, which creates the DataBoundLiteralControl
        /// needed.  Next, it adds two ImageButtons (the edit/delete images).  Finally,
        /// it calls FinishBuildItemTemplate, which builds up the static/dynamic content
        /// AFTER the imageButtons.
        /// </summary>
        /// <param name="_ctrl"></param>
        //
        // ********************************************************************/
        private void BuildItemTemplate(Control _ctrl) {

            System.Web.UI.IParserAccessor __parser = ((System.Web.UI.IParserAccessor)(_ctrl));

            // build up the first part of the DataList
            BeginBuildItemTemplate();

            // add the DataBoundLiteralControl to the parser			
            __parser.AddParsedSubObject(this.dataBoundLiteralControl);

            // add an edit and delete icon
            ImageButton imgbtnTmp = new ImageButton();
//            imgbtnTmp.ImageUrl = Globals.ImagePath + this.EditPostIconUrl;
            imgbtnTmp.CommandName = "edit";
            imgbtnTmp.DataBinding += new EventHandler(HandleImageButtonDataBind);
            imgbtnTmp.Command += new CommandEventHandler(PostAction_Click);
            imgbtnTmp.ToolTip = "Click to edit the contents of this post.";
            __parser.AddParsedSubObject(imgbtnTmp);

            imgbtnTmp = new ImageButton();
//            imgbtnTmp.ImageUrl = Globals.ImagePath + this.DeletePostIconUrl;
            imgbtnTmp.CommandName = "delete";
            imgbtnTmp.DataBinding += new EventHandler(HandleImageButtonDataBind);
            imgbtnTmp.Command += new CommandEventHandler(PostAction_Click);
            imgbtnTmp.ToolTip = "Click to delete this post.";
            __parser.AddParsedSubObject(imgbtnTmp);

            __parser.AddParsedSubObject(new LiteralControl("&nbsp;"));

            // complete our building up the DataList
            FinishBuildItemTemplate();

            // add the DataBoundLiteralControl to the parser			
            __parser.AddParsedSubObject(this.dataBoundLiteralControl);

        }


        // *********************************************************************
        //
        //  BindData
        //
        /// <summary>
        /// This function binds the posts to show to the DataList.  Note that only
        ///	those posts that fall within the first "page" of posts are displayed for
        ///	the forum.  No means to navigate backward or forward is present.  This is
        ///	a bit restrictive, since it limits the administrator to being able to only
        ///	edit or delete posts in a forum that fall within the default timespan for
        ///	the forum.
        /// </summary>
        //
        // ********************************************************************/		
        private void BindData() {

            PostCollection posts;

            // Make sure all of our child controls have been created
            this.EnsureChildControls();			

            // Get info about the forum
            Forum forum = Forums.GetForumInfo(ForumID);

            int iUserTimeZone = Globals.DBTimezone;

            // Read in user information, if applicable and set iUserTimeZone
            if (Context.User.Identity.Name.Length > 0)
                iUserTimeZone = ((User) Users.GetUserInfo(Context.User.Identity.Name, true)).Timezone;

            // Get all of the messages for the forum
            if (ViewMode == ViewOptions.NotSet)
                posts = Posts.GetAllMessages(ForumID, ViewOptions.Threaded, 0);
            else
                posts = Posts.GetAllMessages(ForumID, ViewMode, 0);


            // Bind the data
            dlForumView.DataSource = posts;
            dlForumView.DataBind();

        }


        // *********************************************************************
        //
        //  CreateChildControls
        //
        /// <summary>
        /// This event handler adds the children controls.
        /// </summary>
        //
        // ********************************************************************/	
        protected override void CreateChildControls() {

            Panel panelTmp;
            Label lblTmp;
            Button btnTmp;
            
            // Is this user an administrator?
            if (this.CheckUserPermissions && !((User) Users.GetUserInfo(Context.User.Identity.Name, true)).IsAdministrator)
                Context.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UnableToAdminister));

            // Make sure we have a valid ForumID
            if (ForumID == -1)
                throw new Exception("The ForumView control requires that you pass in a valid ForumID.  No such value was passed in.");

            // Create the panel
            panelTmp = new Panel();
            panelTmp.ID = "panelPosts";

            // if the user indicated to show the forum title, do that now.
            if (ShowForumTitle) {

                lblTmp = new Label();
                lblTmp.ID = "txtForumTitle";
                lblTmp.CssClass = "head";
                panelTmp.Controls.Add(lblTmp);

            }

            // add a Refresh button
            btnTmp = new Button();
            btnTmp.CssClass = "normalButton";
            btnTmp.Text = "Refresh Post Listings for this Forum";
            btnTmp.ToolTip = "Click to update the below view of posts for this forum.";
            btnTmp.Click += new EventHandler(ButtonRefresh_Click);
            panelTmp.Controls.Add(btnTmp);

            // Add to the control collection of the panel
            panelTmp.Controls.Add(dlForumView);

            // Add the panel to the parent controls collection
            Controls.Add(panelTmp);

            // create the confirm delete panel
            CreateConfirmDeletePanel();

        }

	

        // *********************************************************************
        //
        //  OnPreRender Event Handler
        //
        /// <summary>
        /// This event handler binds the template to the datalist - this action
        ///	is only carried out if the user DID NOT specify an ItemTemplate.
        ///	In such a case, the default databinding is used.
        /// </summary>
        /// <param name="e"></param>
        //
        // ********************************************************************/
        protected override void OnPreRender(EventArgs e) {

            // bind the data on our first load
            if (!Page.IsPostBack) 
                BindData();

        }


        // *********************************************************************
        //
        //  OnPreRender Event Handler
        //
        /// <summary>
        ///	This event handler fires when the user clicks the Refresh button.  All
        ///	we need to do is rebind the data.
        /// </summary>
        //
        // ********************************************************************/
        private void ButtonRefresh_Click(Object sender, EventArgs e) {

			BindData();

		}
		


        // *********************************************************************
        //
        //  PostAction_Click Event Handler
        //
        /// <summary>
        /// This event handler fires when the user clicks one of the two ImageButtons
        /// (edit/delete) or when the user clicks the "Confirm Delete" button in
        ///	the Confirm Delete panel.  Appropriate action is taken based upon
        ///	the user's command.
        ///	</summary>
        //
        // ********************************************************************/
        private void PostAction_Click(Object sender, CommandEventArgs e) {

            // Read in the postID
            int iPostID = Convert.ToInt32(e.CommandArgument);

            // Decide what action to take
            switch (e.CommandName) {

                // Edit the post, so send the user to the proper URL
                case "edit":
                Context.Response.Redirect(Globals.UrlEditPostFromAdmin + iPostID.ToString() + "&ForumID=" + ForumID.ToString());
                break;


                // Delete the post, hide/show the appropriate panels
                case "delete":
                ((Panel) FindControl("panelConfirmDelete")).Visible = true;
                ((Panel) FindControl("panelPosts")).Visible = false;

                // set the Confirm Delete button's CommandArgument to the PostID passed in
                ((Button) FindControl("btnConfirmDelete")).CommandArgument = iPostID.ToString();
                break;

                // Delete the post
                case "confirmdelete":
                if (deletePostStyle.ShowReasonsForDeletingTextBox)
                    Posts.DeletePost(iPostID, ((TextBox) FindControl("txtDeleteReason")).Text, "undone");
                else
                    Posts.DeletePost(iPostID, "undone", "undone");

                // Show the posts
                ((Panel) FindControl("panelConfirmDelete")).Visible = false;
                ((Panel) FindControl("panelPosts")).Visible = true;

                // rebind the data
                BindData();		
                break;
            }
        }

        
        // *********************************************************************
        //
        //  ButtonCancelDelete_Click Event Handler
        //
        /// <summary>
        /// This event handler fires when the user clicks the Cancel Delete button.
        /// It hides/shows the appropriate panels and rebinds the data.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        //
        // ********************************************************************/
        private void ButtonCancelDelete_Click(Object sender, EventArgs e) {

            // flip the panel visibility back
            ((Panel) FindControl("panelConfirmDelete")).Visible = false;
            ((Panel) FindControl("panelPosts")).Visible = true;

            // rebind the data
            this.BindData();

        }


        // *********************************************************************
        //
        //  CreateConfirmDeletePanel
        //
        /// <summary>
        /// This function creates the controls for the delete post panel.  The Delete
        ///	Post panel is displayed when the user clicks on the "Delete Post" link.
        ///	It pops up two buttons, a cancel and confirm, allowing the user to
        ///	ensure that they do, indeed, want to delete the post.
        /// </summary>
        /// 
        // ********************************************************************/
        void CreateConfirmDeletePanel() {

            // Start off by creating the panel control
            Panel panelTmp = new Panel();
            panelTmp.ID = "panelConfirmDelete";
            panelTmp.Visible = false;

            Label lblTmp;
            Button btnTmp;

            panelTmp.Controls.Add(new LiteralControl("<table class=\"postText\" align=\"center\" border=\"0\"><tr><th>\n"));

            // Create the header title
            lblTmp = new Label();
            lblTmp.Text = "DELETING A MESSAGE";
            lblTmp.CssClass = "head";
            panelTmp.Controls.Add(lblTmp);

            panelTmp.Controls.Add(new LiteralControl("\n</th></tr>\n<tr><td align=\"center\">"));

            // Display the reasons why (or why not) to delete the post.  Also notify the user that
            // deleting a post will delete all of its children (replies).
            lblTmp = new Label();
            lblTmp.Text = deletePostStyle.DeletePostText + Globals.HtmlNewLine + Globals.HtmlNewLine +
            "<b>Also, all of this post's replies will be deleted as well.</b>" + Globals.HtmlNewLine + Globals.HtmlNewLine ;
            lblTmp.ApplyStyle(deletePostStyle);
            panelTmp.Controls.Add(lblTmp);

            // Create the cancel button
            btnTmp = new Button();
            btnTmp.ID = "btnCancelDelete";
            btnTmp.Text = "Cancel Delete";	
            btnTmp.CssClass = "normalButton";
            btnTmp.ToolTip = "Click this to cancel the delete, leaving this post in-tact.";
            btnTmp.Click += new EventHandler(ButtonCancelDelete_Click);			
            panelTmp.Controls.Add(btnTmp);

            panelTmp.Controls.Add(new LiteralControl("&nbsp;&nbsp;&nbsp;"));

            // Create the confirm button
            btnTmp = new Button();
            btnTmp.ID = "btnConfirmDelete";
            btnTmp.Text = "Confirm Delete";	
            btnTmp.CssClass = "normalButton";
            btnTmp.ToolTip = "Click this to permanently delete the post.";
            btnTmp.Command += new CommandEventHandler(PostAction_Click);
            btnTmp.CommandName = "confirmdelete";
            panelTmp.Controls.Add(btnTmp);

            // Display a textbox for the reason why a post was deleted, 
            // only if ShowReasonsForDeletingTextBox is true
            if (deletePostStyle.ShowReasonsForDeletingTextBox) {

                // Show the "Reasons for Deleting" message
                lblTmp = new Label();
                lblTmp.Text += Globals.HtmlNewLine + Globals.HtmlNewLine + deletePostStyle.ReasonsTextBoxMessage + Globals.HtmlNewLine;
                lblTmp.ApplyStyle(deletePostStyle);
                panelTmp.Controls.Add(lblTmp);

                // Create the reasons textbox
                TextBox txtTmp = new TextBox();
                txtTmp.ID = "txtDeleteReason";
                txtTmp.TextMode = TextBoxMode.MultiLine;
                txtTmp.ApplyStyle(deletePostStyle);
                txtTmp.Columns = deletePostStyle.ReasonsTextBoxColumns;
                txtTmp.Rows = deletePostStyle.ReasonsTextBoxRows;
                panelTmp.Controls.Add(txtTmp);

                panelTmp.Controls.Add(new LiteralControl("</td></tr><tr><td align=\"center\">\n\t"));

            }

            panelTmp.Controls.Add(new LiteralControl("\n</td></tr></table>"));
            Controls.Add(panelTmp);
        } 


        // *********************************************************************
        //
        //  DisplayIndent
        //
        /// <summary>
        /// This functions simply renders the html needed for the identation for
        ///	a particular item in a post.  Only called if displaying a threaded forum
        /// </summary>
        /// <param name="iPostLevel"></param>
        /// <returns>String</returns>
        //
        // ********************************************************************/
        private String DisplayIndent(int iPostLevel) {

            String retVal = "";			

            // if we're showing a new thread, display the thread separator
            if (iPostLevel == 1)
                retVal += ThreadSeparator;

            // display the appropriate number of idents based on the PostLevel
            for (int iLoop=0; iLoop < iPostLevel-1; iLoop++)
                retVal += IndentString;

            // Add the LiteralControl to the Controls collection
            return retVal;

        }


        // *********************************************************************
        //
        //  ForumID
        //
        /// <summary>
		/// Specifies the forum whose posts you wish to administer.
		/// </summary>
		/// <remarks>
		/// If ForumID is not specified, an Exception will be thrown.
		/// </remarks>
		/// 
		// ********************************************************************/

        [
            Bindable(true),
            Category("Required"),
            DefaultValue(""),
            Description("Specifies the Forum whose posts you wish to administer.")
        ]
        public int ForumID {

            get {
                // the forumID is stuffed in the ViewState so that
                // it is persisted across postbacks.
                if (ViewState["forumID"] == null)
                    return -1;		// if it's not found in the ViewState, return the default value

                return Convert.ToInt32(ViewState["forumID"].ToString());
            }

            set {
                // set the viewstate
                ViewState["forumID"] = value;
            }

        }
		
        // *********************************************************************
        //
        //  ViewOptions
        //
        /// <summary>
        /// Specifies the mode in which to view the forum's posts.
        /// </summary>
        /// <remarks>It is highly recommended that you either choose Flat or leave the default, Threaded,
        /// as the option.  Choosing Mixed will hide some messages.  Also, note that the end user's
        /// Forum View preferences are not taken into consideration when rendering the AdminForumPostListing
        /// Web control. </remarks>
        //
        // ********************************************************************/
        [
            Bindable(true),
            Category("Appearance"),
            DefaultValue("Threaded"),
            Description("Specifies the mode in which to view the forum's posts.")
        ]
        public ViewOptions ViewMode {

            get {
                // the ViewMode is stuffed in the ViewState so that
                // it is persisted across postbacks.
                if (ViewState["viewMode"] == null)
                    return defaultViewOptions;

                return ((ViewOptions) ViewState["viewMode"]);
            }

            set {
                // set the viewstate
                ViewState["viewMode"] = value;
            }

        }

        // *********************************************************************
        //
        //  ViewOptions
        //
        /// <summary>
        /// When a post is clicked, the administrator will be taken to view the post.  Use this property
        /// to specify if the post should be loaded in a particular frame.
        /// </summary>
        /// 
        // ********************************************************************/
        [
            Bindable(true),
            Category("Appearance"),
            DefaultValue(""),
            Description("Specifies if the post should be loaded in a particular frame.")
        ]
        public String FrameToOpenPosts {

            get {
                if (ViewState["frameToOpenPosts"] == null) 
                    return "";

                return (String) ViewState["frameToOpenPosts"];
            }

            set { 
                ViewState["frameToOpenPosts"] = value; 
            }
        }

        // *********************************************************************
        //
        //  ShowForumTitle
        //
        /// <summary>
        /// Indicates whether or not to display the Forum's title.
        /// </summary>
        /// 
        // ********************************************************************/
        [
            Bindable(true),
            Category("Appearance"),
            DefaultValue("True"),
            Description("Indicates whether or not to display the Forum's title.")
        ]
        public bool ShowForumTitle {
            get {
                if (ViewState["showForumTitle"] == null) 
                    return true;
                    
                return (bool) ViewState["showForumTitle"];
            }

            set { 
                ViewState["showForumTitle"] = value; 
            }
        }

        // *********************************************************************
        //
        //  ThreadSeparator
        //
        /// <summary>
        /// Specifies the HTML to use to separate each thread.  Defaults to a horizontal rule.
        /// </summary>
        /// <remarks>This property is only displayed when the ForumView property is set to Threaded (the
        /// default for this Web control).</remarks>
        /// 
        // ********************************************************************/
        [
            Bindable(true),
            Category("Appearance"),
            DefaultValue("<hr noshade size=\"1px\">"),
            Description("Specifies the HTML to use to separate each thread.")
        ]
        public String ThreadSeparator {
            get {
                if (ViewState["threadSeparator"] == null) 
                    return defaultThreadSeparator;
                   
                return (String) ViewState["threadSeparator"];
            }

            set { 
                ViewState["threadSeparator"] = value; 
            }
        }


        // *********************************************************************
        //
        //  IndentString
        //
        /// <summary>
        /// Specifies the HTML to use to indent each child post.
        /// </summary>
        /// <remarks>Only applicable when viewing the forum with the threaded ViewMode display.</remarks>
        //
        // ********************************************************************/
        [
            Bindable(true),
            Category("Appearance"),
            DefaultValue("&nbsp;&nbsp;&nbsp;&nbsp;"),
            Description("Specifies the HTML to use to indent each child post.")
        ]
        public String IndentString {
            get {
                if (ViewState["indentString"] == null) 
                    return defaultIndentString;
                    
                return (String) ViewState["indentString"];
            }

            set { 
                ViewState["indentString"] = value; 
            }

        }

        // *********************************************************************
        //
        //  EditPostIconUrl
        //
        /// <summary>
        /// Specifies the Url to the image to display for the ImageButton that, when clicked, will allow
        /// the end user to edit the post.
        /// </summary>
        /// <remarks>This setting should specify the image's Url path relative to the imagesPath
        /// setting in Web.config.</remarks>
        //
        // ********************************************************************/
        public String EditPostIconUrl {
            get {
                if (ViewState["editPostIconUrl"] == null) 
                    return defaultEditPostIconUrl;

                return (String) ViewState["editPostIconUrl"];
            }

            set { 
                ViewState["editPostIconUrl"] = value; 
            }

        }

        // *********************************************************************
        //
        //  DeletePostIconUrl
        //
        /// <summary>
        /// Specifies the Url to the image to display for the ImageButton that, when clicked, will allow
        /// the end user to delete the post.
        /// </summary>
        /// <remarks>This setting should specify the image's Url path relative to the imagesPath
        /// setting in Web.config.</remarks>
        /// 
        // ********************************************************************/
        public String DeletePostIconUrl {

            get {
                if (ViewState["deletePostIconUrl"] == null) 
                    return defaultDeletePostIconUrl;
                
                return (String) ViewState["deletePostIconUrl"]; 
            }

            set { 
                ViewState["deletePostIconUrl"] = value; 
            }
        }

        // *********************************************************************
        //
        //  ShowUsername
        //
        /// <summary>
        /// Determines whether or not to show the Username of the person who posted the message.
        /// </summary>
        /// 
        // ********************************************************************/
        public bool ShowUsername {
            get {
                if (ViewState["showUsername"] == null) 
                    return true;

                return (bool) ViewState["showUsername"];
            }

            set { 
                ViewState["showUsername"] = value; 
            }

        }

        // *********************************************************************
        //
        //  ShowDatePosted
        //
        /// <summary>
        /// Determines whether or not to show the date and time the messages was posted.
        /// </summary>
        ///
        // ********************************************************************/
        public bool ShowDatePosted {
            get {
                if (ViewState["showDatePosted"] == null) 
                    return true;

                return (bool) ViewState["showDatePosted"];
            }
            set { 
                ViewState["showDatePosted"] = value; 
            }
        }

        // *********************************************************************
        //
        //  DeletePost
        //
        /// <summary>
        /// This property allows the end developer to specify the stylistic and textual properties
        /// of the DeltePostStyle class.  These settings are reflected in the Confirm Delete page.
        /// </summary>
        ///
        // ********************************************************************/
        public DeletePostStyle DeletePost {
            get {	
                return deletePostStyle;	
            }
        }

        // *********************************************************************
        //
        //  DeletePost
        //
        /// <summary>
        /// Indicates if the Web control should check to verify that the user visiting the page
        /// is, indeed, a moderator.
        /// </summary>
        ///
        // ********************************************************************/
        public bool CheckUserPermissions {
            get {
                if (ViewState["checkUserPerm"] == null) 
                    return true;
                return (bool) ViewState["checkUserPerm"];

            }
   
            set { 
                ViewState["checkUserPerm"] = value; 
            }
        }

    }
}
