using System;
using System.Drawing;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using AspNetForums;
using AspNetForums.Components;
using System.ComponentModel;
using System.IO;

namespace AspNetForums.Controls {

    public class UserProfile : SkinnedForumWebControl {

        string skinFilename = "Skin-DisplayUserInfo.ascx";
        User user;

        // *********************************************************************
        //  UserProfile
        //
        /// <summary>
        /// Constructor
        /// </summary>
        /// 
        // ********************************************************************/
        public UserProfile() {
            string username = null;

            // Set the default skin
            if (SkinFilename == null)
                SkinFilename = skinFilename;

            // If we have an instance of context, let's attempt to
            // get the ForumID so we can save the user from writing
            // the code
            if (null != Context) {

                // Attempt the query string or post body
                if (null != Context.Request.QueryString["UserName"])
                    username = Context.Request.QueryString["UserName"];
                else if (null != Context.Request.Form["UserName"])
                    username = Context.Request.Form["UserName"];

            }

            // Attempt to get the user class
            try {
                user = Users.GetUserInfo(username, false);
            }
            catch (Exception) {
                // user not found
                HttpContext.Current.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UserDoesNotExist));
            }
        }
	
        // *********************************************************************
        //  Initializeskin
        //
        /// <summary>
        /// This method populates the user control used to edit a user's information
        /// </summary>
        /// <param name="control">Instance of the user control to populate</param>
        /// 
        // ***********************************************************************/
        override protected void InitializeSkin(Control skin) {
            int totalSitePosts = Posts.GetTotalPostCount();
            Label label;
            HyperLink link;

            // Joined
            label = (Label) skin.FindControl("Joined");
            label.Text = user.DateCreated.ToString(user.DateFormat + " " + Globals.TimeFormat);

            // Set the name
            label = (Label) skin.FindControl("LastLogin");
            label.Text = user.LastLogin.ToString(user.DateFormat + " " + Globals.TimeFormat);

            // Set the name
            label = (Label) skin.FindControl("Username");
            label.Text = user.Username;

            // Set the email address
            link = (HyperLink) skin.FindControl("Email");

            // is the current user anonymous?
            if (null != ForumUser) {
                link.Text = user.PublicEmail;
                link.NavigateUrl = "mailto:" + user.PublicEmail;
            } else {
                link.Text = "Unavailable to anonymous users.";
            }

            // Occupation
            label = (Label) skin.FindControl("Occupation");
            if (null != ForumUser)
                label.Text = user.Occupation;
            else
                label.Text = "Unavailable to anonymous users.";

            // Location
            label = (Label) skin.FindControl("Location");
            if (null != ForumUser)
                label.Text = user.Location;
            else
                label.Text = "Unavailable to anonymous users.";

            // Interests
            label = (Label) skin.FindControl("Interests");
            if (null != ForumUser)
                label.Text = user.Interests;
            else
                label.Text = "Unavailable to anonymous users.";

            // MsnIm
            label = (Label) skin.FindControl("MsnIm");
            if (null != ForumUser)
                label.Text = user.MsnIM;
            else
                label.Text = "Unavailable to anonymous users.";

            // YahooIm
            label = (Label) skin.FindControl("YahooIm");
            if (null != ForumUser)
                label.Text = user.YahooIM;
            else
                label.Text = "Unavailable to anonymous users.";

            // AolIm
            label = (Label) skin.FindControl("AolIm");
            if (null != ForumUser)
                label.Text = user.AolIM;
            else
                label.Text = "Unavailable to anonymous users.";

            // ICQ
            label = (Label) skin.FindControl("ICQ");
            if (null != ForumUser)
                label.Text = user.IcqIM;
            else
                label.Text = "Unavailable to anonymous users.";

            // WebSite
            link = (HyperLink) skin.FindControl("Website");
            link.Text = user.Url;
            link.NavigateUrl = user.Url;

            // Signature
            label = (Label) skin.FindControl("Signature");
            label.Text = Globals.FormatPostBody(user.Signature);

            // Avatar
            if (user.HasAvatar) {

                // Set visiblity to true
                Control c = (Control) skin.FindControl("HasIcon");
                c.Visible = true;

                System.Web.UI.WebControls.Image img = (System.Web.UI.WebControls.Image) skin.FindControl("Icon");
                img.ImageUrl = user.AvatarUrl;
            }

            // Total Posts
            label = (Label) skin.FindControl("PostStats");
            label.Text = user.Username + " has contributed to " + user.TotalPosts.ToString("n0") + " out of " + totalSitePosts.ToString("n0") + " total posts (" + (((double) user.TotalPosts / totalSitePosts) * 100).ToString("N") + "% of total)" + ".";

            // Search for more posts
            link = (HyperLink) skin.FindControl("MorePosts");
            if (null != link) {
                link.NavigateUrl = Globals.UrlSearchForPostsByUser + user.Username;
            }

            // Most Recent Posts
            DisplayMostRecentPost(10, skin);

        }

        // *********************************************************************
        //  RenderTopPosts
        //
        /// <summary>
        /// Renders the top n posts used in the DisplayViewControl
        /// </summary>
        /// <param name="control">Table of posts</param>
        /// 
        // ********************************************************************/
        private void DisplayMostRecentPost(int postsToRender, Control skin) {
            PostCollection posts;
            PostList postList;

            // Get the user's posts
            posts = AspNetForums.Search.PerformSearch(ToSearchEnum.PostsBySearch, SearchWhatEnum.SearchAllWords, -1, user.Username, 0, postsToRender);

            // Find the Post List control
            postList = (PostList) skin.FindControl("PostList");
            postList.DataSource = posts;
            postList.DataBind();

        }

    }
}