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

namespace AspNetForums.Controls {

    [
    ParseChildren(true)	
    ]
    /// <summary>
    /// Summary description for Summary.
    /// </summary>
    public abstract class SkinnedForumWebControl : WebControl, INamingContainer {

        User user = null;
        int forumID = -1;
        int postID = -1;
        int threadID = -1;
        int forumGroupID = -1;
        string skinFilename = null;
        string skinName = null;
        string returnURL = null;

        // *********************************************************************
        //  SkinnedForumWebControl
        //
        /// <summary>
        /// Constructor
        /// </summary>
        // ***********************************************************************/
        public SkinnedForumWebControl() {

            // Attempt to get the current user
            user = Users.GetLoggedOnUser();

            // Is the user not availabe - must be anonymous
            if (user == null)
                Users.TrackAnonymousUsers();

            // Set the siteStyle for the page
            if (user != null)
                skinName = user.Skin;
            else
                skinName = Globals.Skin;

            // If we have an instance of context, let's attempt to
            // get the ForumID so we can save the user from writing
            // the code
            if (null != Context) {
                GetPostIDFromRequest();
                GetForumIDFromRequest();
                GetForumGroupIDFromRequest();
                GetReturnURLFromRequest();
            }            
        }

        // *********************************************************************
        //  GetPostIDFromRequest
        //
        /// <summary>
        /// Retrieves the PostID from the request querystring/post.
        /// </summary>
        // ***********************************************************************/
        private void GetPostIDFromRequest() {
            // Attempt to get the post id, throw if it is invalid
            try {
                if (null != Context.Request.QueryString["PostID"]) {
                    string postID = Context.Request.QueryString["PostID"];

                    // Contains a #
                    if (postID.IndexOf("#") > 0)
                        postID = postID.Substring(0, postID.IndexOf("#"));

                    PostID = Convert.ToInt32(postID);
                } else if (null != Context.Request.Form["PostId"]) {
                    PostID = Convert.ToInt32(Context.Request.Form["PostID"]);
                }
            } 
            catch {
                HttpContext.Current.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.PostDoesNotExist));
                HttpContext.Current.Response.End();
            }
        }


        // *********************************************************************
        //  GetForumIDFromRequest
        //
        /// <summary>
        /// Retrieves the ForumID from the request querystring/post.
        /// </summary>
        // ***********************************************************************/
        private void GetForumIDFromRequest() {

            // Attempt to get the forum id, throw if it is invalid
            try {
                if (null != Context.Request.QueryString["ForumID"]) {
                    string forumID = Context.Request.QueryString["ForumID"];

                    // Contains a #
                    if (forumID.IndexOf("#") > 0)
                        forumID = forumID.Substring(0, forumID.IndexOf("#"));

                    ForumID = Convert.ToInt32(forumID);
                } else if (null != Context.Request.Form["ForumId"]) {
                    ForumID = Convert.ToInt32(Context.Request.Form["ForumID"]);
                }
            } 
            catch {
                HttpContext.Current.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UnknownForum));
                HttpContext.Current.Response.End();
            }
        }


        // *********************************************************************
        //  GetForumGroupIDFromRequest
        //
        /// <summary>
        /// Retrieves the ForumGroupID from the request querystring/post.
        /// </summary>
        // ***********************************************************************/
        private void GetForumGroupIDFromRequest() {

            // Attempt to get the forum id, throw if it is invalid
            try {
                if (null != Context.Request.QueryString["ForumGroupID"]) {
                    string forumGroupID = Context.Request.QueryString["ForumGroupID"];

                    // Contains a #
                    if (forumGroupID.IndexOf("#") > 0)
                        forumGroupID = forumGroupID.Substring(0, forumGroupID.IndexOf("#"));

                    ForumGroupID = Convert.ToInt32(forumGroupID);
                } else if (null != Context.Request.Form["ForumGroupId"]) {
                    ForumGroupID = Convert.ToInt32(Context.Request.Form["ForumGroupID"]);
                }
            } 
            catch {
                HttpContext.Current.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UnknownForum));
                HttpContext.Current.Response.End();
            }
        }

        // *********************************************************************
        //  GetReturnURLFromRequest
        //
        /// <summary>
        /// Retrieves the return url from the request querystring/post.
        /// </summary>
        // ***********************************************************************/
        private void GetReturnURLFromRequest() {

            // Attempt to get the return url
            try {
                if (null != Context.Request.QueryString["ReturnURL"]) {
                    ReturnURL = Context.Request.QueryString["ReturnURL"];
                } else if (null != Context.Request.Form["ReturnURL"]) {
                    ReturnURL = Context.Request.Form["ReturnURL"];
                }
            } 
            catch {
                returnURL = null;
            }
        }

        
        // *********************************************************************
        //  CreateChildControls
        //
        /// <summary>
        /// This event handler adds the children controls.
        /// </summary>
        // ***********************************************************************/
        protected override void CreateChildControls() {
            Control skin;

            // Load the skin
            skin = LoadSkin();

            // Initialize the skin
            InitializeSkin(skin);

            Controls.Add(skin);
        }

        // *********************************************************************
        //  LoadControlSkin
        //
        /// <summary>
        /// Loads the names control template from disk.
        /// </summary>
        // ***********************************************************************/
        protected Control LoadSkin() {
            Control skin;
            string skinPath = Globals.ApplicationVRoot + "/skins/" + SkinName + "/Skins/" + SkinFilename;

            // Do we have a skin?
            if (SkinFilename == null)
                throw new Exception("The SkinName property of the control must be set.");

            // Attempt to load the control. If this fails, we're done
            try {
                skin = Page.LoadControl(skinPath);
            }
            catch (FileNotFoundException) {

                // Ok we couldn't find the skin, let's attempt to load the default skin instead
                try {
                    skin = Page.LoadControl(Globals.ApplicationVRoot + "/skins/default/Skins/" + SkinFilename);
                } 
                catch (FileNotFoundException) {
                    // Can't load a skin
                    throw new Exception("The skin: '" + skinPath + "' was not found. Please ensure this file exists in your skins directory");
                }
            }

            return skin;
        }

        // *********************************************************************
        //  InitializeSkin
        //
        /// <summary>
        /// Initialize the control template and populate the control with values
        /// </summary>
        // ***********************************************************************/
        protected abstract void InitializeSkin(Control skin);


        // *********************************************************************
        //  ForumUser
        //
        /// <summary>
        /// Returns an instance of User or null if the user is not logged in.
        /// </summary>
        // ***********************************************************************/
        protected User ForumUser {
            get {
                return user;
            }
        }
        
        // *********************************************************************
        //  SkinName
        //
        /// <summary>
        /// Allows the default control template to be overridden
        /// </summary>
        // ***********************************************************************/
        public string SkinFilename {
            get { 
                return skinFilename; 
            }
            set { 
                skinFilename = value; 
            }
        }

        // *********************************************************************
        //  ForumID
        //
        /// <summary>
        /// If available returns the forum id value read from the querystring.
        /// </summary>
        /// 
        // ********************************************************************/ 
        public int ForumID  {
            get  {
                return forumID;
            }
            set  {
                forumID = value;
            }
        }

        
        // *********************************************************************
        //  ForumGroupID
        //
        /// <summary>
        /// If available returns the forum group id value read from the querystring.
        /// </summary>
        /// 
        // ********************************************************************/ 
        public int ForumGroupID  {
            get  {
                return forumGroupID;
            }
            set  {
                forumGroupID = value;
            }
        }

        // *********************************************************************
        //  PostID
        //
        /// <summary>
        /// If available returns the post id value read from the querystring.
        /// </summary>
        /// 
        // ********************************************************************/ 
        public int PostID {
            get {
                return postID;
            }
            set {
                postID = value;
            }
        }

        // *********************************************************************
        //  ThreadID
        //
        /// <summary>
        /// If available indicates a top level post
        /// </summary>
        /// 
        // ********************************************************************/ 
        public int ThreadID {
            get {
                return threadID;
            }
            set {
                threadID = value;
            }
        }

        // *********************************************************************
        //  SkinName
        //
        /// <summary>
        /// Used to construct paths to images, etc. within controls.
        /// </summary>
        /// 
        // ********************************************************************/ 
        protected string SkinName {
            get {
                return skinName;
            }
            set {
                skinName = value;
            }
        }

    
        // *********************************************************************
        //  ReturnURL
        //
        /// <summary>
        /// When set allows access to the previous page.
        /// </summary>
        /// 
        // ********************************************************************/ 
        protected string ReturnURL {
            get {
                return returnURL;
            }
            set {
                returnURL = value;
            }
        }
    }
}
