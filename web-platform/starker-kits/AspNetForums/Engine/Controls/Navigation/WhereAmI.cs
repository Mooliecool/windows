using System;
using System.Drawing;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Components;
using System.ComponentModel;
using System.IO;

namespace AspNetForums.Controls {
    
    [ParseChildren(true)]
    public class WhereAmI : SkinnedForumWebControl {
        private const string skinFilename = "Skin-WhereAmI.ascx";
        private const bool dynamicMenu = false;
		
        private bool showHome = false;
        private bool enableLinks = true;
        
        private ForumGroup forumGroup = null;
        private Forum forum = null;
        private Post post = null;

        // *********************************************************************
        //  WhereAmI
        //
        /// <summary>
        /// Constructor
        /// </summary>
        /// 
        // ***********************************************************************/
        public WhereAmI() : base() {
            // Set-up our skin
            if (SkinFilename == null ) {
                SkinFilename = skinFilename;
            }

            // If the browser isn't IE 6+ we don't enable dynamic menus
            if ((HttpContext.Current.Request.Browser.Browser == "IE") && (HttpContext.Current.Request.Browser.MajorVersion >= 6))
                DynamicMenu = true;
        }
			
        // *********************************************************************
        //  Initializeskin
        //
        /// <summary>
        /// This method initializes the control with the named skin.
        /// </summary>
        /// <param name="control">Instance of the user control to populate</param>
        /// 
        // ***********************************************************************/
        protected override void InitializeSkin(Control skin) {
            // Get the navigation links
            HyperLink linkHome = skin.FindControl("LinkHome") as HyperLink;
            HyperLink linkForumGroup = skin.FindControl("LinkForumGroup") as HyperLink;
            HyperLink linkForum = skin.FindControl("LinkForum") as HyperLink;
            HyperLink linkPost = skin.FindControl("LinkPost") as HyperLink;

            // Get the separators
            Control forumGroupSep = skin.FindControl("ForumGroupSeparator") as Control;
            if ( forumGroupSep != null ) {
                forumGroupSep.Visible = false;
            }
            Control forumSep = skin.FindControl("ForumSeparator") as Control;
            if ( forumSep != null ) {
                forumSep.Visible = false;
            }
            Control postSep = skin.FindControl("PostSeparator") as Control;
            if ( postSep != null ) {
                postSep.Visible = false;
            }
			
            // Get the Menu Control areas
            HtmlControl forumGroupMenu = skin.FindControl("ForumGroupMenu") as HtmlControl;
            HtmlControl forumMenu = skin.FindControl("ForumMenu") as HtmlControl;
            HtmlControl postMenu = skin.FindControl("PostMenu") as HtmlControl;
            HtmlGenericControl menuScript = skin.FindControl("MenuScript") as HtmlGenericControl;
			
            if ( showHome && linkHome != null ) {
                linkHome.Visible = true;
                if ( enableLinks )
                    linkHome.NavigateUrl = UrlHome;
                linkHome.Text = RootLabel;
            }
            else {
                linkHome.Visible = false;
            }
			
            if ( ForumGroup != null && linkForumGroup != null ) {
                if ( showHome && linkHome != null && forumGroupSep != null ) {
                    forumGroupSep.Visible = true;
                }

                linkForumGroup.Visible = true;
                if ( enableLinks )
                    linkForumGroup.NavigateUrl = UrlShowForumGroup + ForumGroup.ForumGroupID;
                linkForumGroup.Text = ForumGroup.Name;
				
                if ( forumGroupMenu != null && DynamicMenu ) {
                    forumGroupMenu.Attributes["onmouseover"] = "menuOver('" + this.UniqueID + ":groupMenu');";
                    forumGroupMenu.Attributes["onmouseleave"] = "menuSourceLeave('" + this.UniqueID + ":groupMenu');";
                    RenderGroupsMenu();
                }
            }
            else {
                linkForumGroup.Visible = false;
            }
			
            if ( Forum != null && linkForum != null ) {
                if ( linkForumGroup != null && forumSep != null ) {
                    forumSep.Visible = true;
                }

                linkForum.Visible = true;
                if ( enableLinks )
                    linkForum.NavigateUrl = UrlShowForum + Forum.ForumID;
                linkForum.Text = Forum.Name;

                if ( forumMenu != null && DynamicMenu ) {
                    forumMenu.Attributes["onmouseover"] = "menuOver('" + this.UniqueID + ":forumMenu');";
                    forumMenu.Attributes["onmouseleave"] = "menuSourceLeave('" + this.UniqueID + ":forumMenu');";
                    RenderForumsMenu();
                }
            }
            else {
                linkForum.Visible = false;
            }
			
            if ( Post != null && linkPost != null ) {
                if ( linkForum != null && postSep != null ) {
                    postSep.Visible = true;
                }
                
                linkPost.Visible = true;
                if ( enableLinks )
                    linkPost.NavigateUrl = UrlShowPost + Post.ThreadID;
                linkPost.Text = Post.Subject;

                if ( postMenu != null && DynamicMenu ) {
                    postMenu.Attributes["onmouseover"] = "menuOver('" + this.UniqueID + ":postMenu');";
                    postMenu.Attributes["onmouseleave"] = "menuSourceLeave('" + this.UniqueID + ":postMenu');";
                    RenderPostsMenu();
                }
            }
            else {
                linkPost.Visible = false;
            }

            if ( menuScript != null ) {
                Page.RegisterClientScriptBlock("MenuScript", menuScript.InnerText);
                menuScript.InnerText = string.Empty;
            }
        }
		
        private void RenderGroupsMenu() {
            ForumGroupCollection groups = ForumGroups.GetAllForumGroups(false, true);
            string groupMenu = "<div class='popupMenu' style='position: absolute; display: none;' id='" + this.UniqueID + ":groupMenu'>";
            groupMenu += "<div class='popupTitle'>Forum Groups</div>";
            for(int i = 0; i < groups.Count; i++) {
                groupMenu += "<div class='popupItem'> <a href='" + UrlShowForumGroup + ((ForumGroup) groups[i]).ForumGroupID + 
                    "'>" + ((ForumGroup) groups[i]).Name + "</a> </div>";
            }
            groupMenu += "</div>";
            Page.RegisterClientScriptBlock(this.UniqueID + ":groupMenu", groupMenu);
        }
		
        private void RenderForumsMenu() {
            ForumCollection forums = DataProvider.Instance().GetForumsByForumGroupId(Forum.ForumGroupId, Context.User.Identity.Name);
            forums.Sort();
            string forumMenu = "<div class='popupMenu' style='position: absolute; display: none;' id='" + this.UniqueID + ":forumMenu'>";
            forumMenu += "<div class='popupTitle'>Forums</div>";
            for(int i = 0; i < forums.Count; i++) {
                forumMenu += "<div class='popupItem'> <a href='" + UrlShowForum + ((Forum) forums[i]).ForumID +
                    "'>" + ((Forum) forums[i]).Name + "</a> </div>";
            }
            forumMenu += "</div>";
            Page.RegisterClientScriptBlock(this.UniqueID + ":forumMenu", forumMenu);
        }
		
        private void RenderPostsMenu() {
            ThreadCollection posts = Threads.GetAllThreads(Post.ForumID, 20, 0, DateTime.MinValue, Context.User.Identity.Name, false);
            string postMenu = "<div class='popupMenu' style='position: absolute; display: none;' id='" + this.UniqueID + ":postMenu'>";
            postMenu += "<div class='popupTitle'>Posts</div>";
            for(int i = 0; i < posts.Count && i < 20; i++) {
                postMenu += "<div class='popupItem'> <a href='" + UrlShowPost + ((Post) posts[i]).PostID +
                    "'>" + ((Post) posts[i]).Subject + "</a> </div>";
            }
            postMenu += "</div>";
            Page.RegisterClientScriptBlock(this.UniqueID + ":postMenu", postMenu);
        }
		
        private ForumGroup ForumGroup {
            get {
                if ( forumGroup == null ) {
                    if ( ForumGroupID > -1 ) {
                        forumGroup = ForumGroups.GetForumGroup(ForumGroupID);
                    }
                    else {
                        if ( ForumID > -1 ) {
                            forumGroup = ForumGroups.GetForumGroup(Forum.ForumGroupId);
                        }
                        else {
                            if ( PostID > -1 ) {
                                forumGroup = ForumGroups.GetForumGroupByForumID(Post.ForumID);
                            }
                        }
                    }
                }
				
                return forumGroup;
            }
        }
		
        private Forum Forum {
            get {
                if ( forum == null ) {
                    if ( ForumID > -1 ) {
                        forum = Forums.GetForumInfo(ForumID);
                    }
                    else {
                        if ( PostID > -1 ) {
                            forum = Forums.GetForumInfo(Post.ForumID);
                        }
                    }
                }
				
                return forum;
            }
        }
		
        private Post Post {
            get {
                if ( post == null ) {
                    if ( PostID > -1 ) {
                        post = Posts.GetPost(PostID, Context.User.Identity.Name);
                    }
                }
				
                return post;
            }
        }

        public string UrlHome {
            get {
                if (ViewState["urlHome"] == null)
                    return Globals.UrlHome;
					
                return ViewState["urlHome"].ToString();
            }
            set {
                // set the viewstate
                ViewState["urlHome"] = value;
            }
        }

        public string RootLabel {
            get {
                if (ViewState["rootLabel"] == null)
                    return Globals.SiteName;
					
                return ViewState["rootLabel"].ToString();
            }
            set {
                // set the viewstate
                ViewState["rootLabel"] = value;
            }
        }

        public string UrlShowForumGroup {
            get {
                if (ViewState["urlShowForumGroup"] == null)
                    return Globals.UrlShowForumGroup;
					
                return ViewState["urlShowForumGroup"].ToString();
            }
            set {
                // set the viewstate
                ViewState["urlShowForumGroup"] = value;
            }
        }

        public string UrlShowForum {
            get {
                if (ViewState["urlShowForum"] == null)
                    return Globals.UrlShowForum;
					
                return ViewState["urlShowForum"].ToString();
            }
            set {
                // set the viewstate
                ViewState["urlShowForum"] = value;
            }
        }

        public string UrlShowPost {
            get {
                if (ViewState["urlShowPost"] == null)
                    return Globals.UrlShowPost;
					
                return ViewState["urlShowPost"].ToString();
            }
            set {
                // set the viewstate
                ViewState["urlShowPost"] = value;
            }
        }

        public bool DynamicMenu {
            get {
                if (ViewState["dynamicMenu"] == null)
                    return dynamicMenu;
					
                return bool.Parse(ViewState["dynamicMenu"].ToString());
            }
            set {
                // set the viewstate
                ViewState["dynamicMenu"] = value;
            }
        }

        /****************************************************************
        // ShowHome
        //
        /// <summary>
        /// Controls whether or not the root element for the home is shown
        /// </summary>
        //
        ****************************************************************/
        public bool ShowHome {
            get {return showHome; }
            set {showHome = value; }
        }

        /****************************************************************
        // EnableLinks
        //
        /// <summary>
        ///  Determines whether or not links are hook-ed up.
        /// </summary>
        //
        ****************************************************************/
        public bool EnableLinks {
            get {return enableLinks; }
            set {enableLinks = value; }
        }
    }
}