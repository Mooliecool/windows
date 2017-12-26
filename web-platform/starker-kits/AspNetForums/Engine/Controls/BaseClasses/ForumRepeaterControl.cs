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
    public abstract class ForumRepeaterControl : Repeater {
        User user = null;
		int forumID = -1;
        int forumGroupID = -1;
		int postID = -1;
        int threadID = -1;
        string skinName = null;

        // *********************************************************************
        //  ForumRepeaterControl
        //
        /// <summary>
        /// Constructor
        /// </summary>
        // ***********************************************************************/
        public ForumRepeaterControl() {

            // If we're in design-time we simply return
            if (null == HttpContext.Current)
                return;

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
            catch 
            {
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
                if (null != Context.Request.QueryString["ForumID"])
                    ForumID = Convert.ToInt32(Context.Request.QueryString["ForumID"]);
                else if (null != Context.Request.Form["ForumID"])
                    ForumID = Convert.ToInt32(Context.Request.Form["ForumID"]);

            }
            catch 
            {
                HttpContext.Current.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UnknownForum));
                HttpContext.Current.Response.End();
            }
        }

        
        // *********************************************************************
        //  GetForumGroupIDFromRequest
        //
        /// <summary>
        /// Retrieves the ForumID from the request querystring/post.
        /// </summary>
        // ***********************************************************************/
        private void GetForumGroupIDFromRequest() {

            // Attempt to get the forum id, throw if it is invalid
            try {
                if (null != Context.Request.QueryString["ForumGroupID"])
                    ForumGroupID = Convert.ToInt32(Context.Request.QueryString["ForumGroupID"]);
                else if (null != Context.Request.Form["ForumGroupID"])
                    ForumGroupID = Convert.ToInt32(Context.Request.Form["ForumGroupID"]);
            } 
            catch 
            {
                HttpContext.Current.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UnknownForum));
                HttpContext.Current.Response.End();
            }
        }

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

    }
}
