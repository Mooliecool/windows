using System;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Components;
using AspNetForums.Controls.Moderation;
using System.ComponentModel;

namespace AspNetForums.Controls.Admin {

    /// <summary>
    /// This Web control displays a forum for the end user to edit.  The end user can edit the
    /// forum's properties (Title, Description, etc.) from here, as well as view the list of
    /// users who moderate the particular forum.  Additionally the users can edit or delete posts
    /// for this forum.
    /// </summary>
    [
        ParseChildren(true)
    ]
    public class EditForum : WebControl, INamingContainer {

        const String defaultSeparator = "<hr noshade size=\"1px\" />\n";		// the default content separator
        const String defaultPostDisplayTitle = "Posts in this Forum";
        const bool defaultShowPostsInForum = false;

        CreateEditForum editForum = new CreateEditForum();
        ModeratedForums moderatedForums = new ModeratedForums();
        AdminForumPostListing postListing = new AdminForumPostListing();


        public EditForum() {
            // If we have an instance of context, let's attempt to
            // get the ForumID so we can save the user from writing
            // the code
            if (null != Context) {

                if (null != Context.Request.QueryString["ForumId"])
                    this.ForumID = Convert.ToInt32(Context.Request.QueryString["ForumId"]);
                else if (null != Context.Request.Form["ForumId"])
                    this.ForumID = Convert.ToInt32(Context.Request.Form["ForumId"]);

            }        
        }

        // *********************************************************************
        //  CreateChildControls
        //
        /// <summary>
        /// This event handler adds the children controls.
        /// </summary>
        //
        // ********************************************************************/
        protected override void CreateChildControls() {

            if (this.CheckUserPermissions && !((User) Users.GetUserInfo(Context.User.Identity.Name, true)).IsAdministrator)
                // this user isn't an administrator
                Context.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UnableToAdminister));

            // make sure we have a ForumID
            if (ForumID == -1)
                throw new Exception("You must pass in a valid ForumID in order to use the EditForum control.");

            // create an instance of the CreateEditForum Web control
            editForum.ForumID = ForumID;
            editForum.Mode = CreateEditForumMode.EditForum;
            Controls.Add(editForum);

            Panel panelModerators = new Panel();
            panelModerators.ID = "panelModerators";

            // add a separator
            panelModerators.Controls.Add(new LiteralControl(this.Separator));

            // add the list of users who moderate this forum
            moderatedForums.Mode = ModeratedForumMode.ViewForForum;
            moderatedForums.ForumID = ForumID;
            panelModerators.Controls.Add(moderatedForums);

            // add the panel
            Controls.Add(panelModerators);

            // add a separator
            Controls.Add(new LiteralControl(this.Separator));

            // do we want to show the posts in the forum?
            if (ShowPostsInForum) {
                // add the label
                Label lblTmp = new Label();
                lblTmp.CssClass = "head";
                lblTmp.Text = PostDisplayTitle + Globals.HtmlNewLine;
                Controls.Add(lblTmp);

                // add the listing of posts
                postListing.ForumID = ForumID;
                Controls.Add(this.postListing);
            } else {

                // add a hyperlink to the posts to edit
                HyperLink lnkTmp = new HyperLink();
                lnkTmp.Text = "View the posts for this forum...";
                lnkTmp.CssClass = "normalItalic";
                lnkTmp.NavigateUrl = Globals.UrlShowForumPostsForAdmin + ForumID.ToString();
                Controls.Add(lnkTmp);
            }
        }


        // *********************************************************************
        //  OnPreRender
        //
        /// <summary>
        /// This event handler fires every time the page is loaded.  In it, we
        /// want to determine if the forum we are editing is moderated.  If it is,
        /// we want to display the list of forum moderators, else we want to hide
        /// the list.
        /// </summary>
        //
        // ********************************************************************/
        protected override void OnPreRender(EventArgs e) {
            this.EnsureChildControls();

            if (!Page.IsPostBack)				
                ((Panel) FindControl("panelModerators")).Visible = Forums.GetForumInfo(ForumID).Moderated;
        }


        // *********************************************************************
        //  PostDisplayTitle
        //
        /// <summary>
        /// Specifies the title to display above the forum's posts.
        /// <seealso cref="ShowPostsInForum"/>
        /// </summary>
        /// <remarks>
        /// This setting is only displayed when the property ShowPostsInForum
        /// is set to true.
        /// </remarks>
        /// 
        // ********************************************************************/
        public String PostDisplayTitle {

            get {
                if (ViewState["postDisplayTitle"] == null) 
                    return defaultPostDisplayTitle;

                return (String) ViewState["postDisplayTitle"];
            }

            set { 
                ViewState["postDisplayTitle"] = value;  
            }

        }

        // *********************************************************************
        //  ShowPostsInForum
        //
        /// <summary>
        /// Indicates whether or not a list of the forum's current posts should be shown
        /// on the Web page.  If this is set to false, which is the default, a link is automatically
        /// provided to an ASP.NET Web page that will list the forum's posts.
        /// </summary>
        /// <remarks>
        /// Developers may wish to separate the ability to edit and delete  the existing
        /// posts since there may be many posts, making the EditForum Web control render a long,
        /// difficult to read Web page.
        /// </remarks>
        /// 
        // ********************************************************************/
        public bool ShowPostsInForum {
            get {
                if (ViewState["showPosts"] == null) 
                    return defaultShowPostsInForum;

                return (bool) ViewState["showPosts"];
            }

            set {
                ViewState["showPosts"] = value;  
            }

        }


        // *********************************************************************
        //  ShowPostsInForum
        //
        /// <summary>
        /// This property indicates what HTML should appear between each section of the forum
        /// edit page.  The sections that are separated out are: the Forum properties (Title,
        /// Description, etc.); the list of users who moderate the particular forum; and (optionally)
        /// the forum's posts, which can be edited and deleted.
        /// </summary>
        /// 
        // ********************************************************************/
        public String Separator {

            get {
                if (ViewState["separator"] == null) 
                    return defaultSeparator;

                return (String) ViewState["separator"];
            }

            set {  
                ViewState["separator"] = value;  
            }

        }

        // *********************************************************************
        //  CreateEditForum
        //
        /// <summary>
        /// An instance of the CreateEditForum Web control, which the EditForum
        /// Web control uses internally to allow the end user to edit the forum's
        /// properties (forum Title, Description, etc.).  This exposed property allows
        /// end developers to customize the look and feel of these options.
        /// </summary>
        /// 
        // ********************************************************************/
        public CreateEditForum CreateEditForum {
            get { 
                return editForum; 
            }
        }


        // *********************************************************************
        //  AdminForumPostListing
        //
        /// <summary>
        /// If the end developer opts to display the forum's posts on this page, an AdminForumPostListing
        /// Web control is added to the EditForums Web child control collection.  This property
        /// provides a means for the end developer to set properties of the AdminForumPostListing
        /// Web control.
        /// </summary>
        /// 
        // ********************************************************************/
        public AdminForumPostListing AdminForumPostListing {
            get { 
                return postListing; 
            }
        }
		
        // *********************************************************************
        //  ModeratedForums
        //
        /// <summary>
        /// The EditForum Web control displays a list of users who can moderate the forum.
        /// This list is generated by including an instance of the ModeratedForums Web control.
        /// An end developer can alter the properties of this list by using the ModeratedForums
        /// property.
        /// </summary>
        /// 
        // ********************************************************************/
        public ModeratedForums ModeratedForums {
            get { 
                return this.moderatedForums; 
            }
        }

        // *********************************************************************
        //  ForumID
        //
        /// <summary>
        /// The ForumID of the Forum to edit.
        /// </summary>
        /// <remarks>
        /// If ForumID is not specified, an Exception is thrown.
        /// </remarks>
        /// 
        // ********************************************************************/
        public int ForumID {
            get {
                if (ViewState["forumID"] == null) 
                    return -1;

                return (int) ViewState["forumID"];
            }

            set { 
                ViewState["forumID"] = value;  
            }
        }

        // *********************************************************************
        //  CheckUserPermissions
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
