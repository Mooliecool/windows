using System;
using System.Collections;
using System.Web;
using System.IO;
using System.Web.Caching;
using AspNetForums.Components;

namespace AspNetForums {

    // *********************************************************************
    //  Posts
    //
    /// <summary>
    /// This class contains methods for working with an individual post.  There are methods to
    /// Add a New Post, Update an Existing Post, retrieve a single post, etc.
    /// </summary>
    // ***********************************************************************/
    public class Posts {

        // *********************************************************************
        //  GetPost
        //
        /// <summary>
        /// Returns information about a particular post.
        /// </summary>
        /// <param name="PostID">The ID of the Post to return.</param>
        /// <returns>A Post object with the spcified Post's information.</returns>
        /// <remarks>This method returns information about a particular post.  If the post specified is
        /// not found, a PostNotFoundException exception is thrown.  If you need more detailed
        /// information, such as the PostID of the next/prev posts in the thread, or if the current user
        /// has email tracking enabled for the thread the post appears in, use the GetPostDetails
        /// method.<seealso cref="GetPostDetails"/></remarks>
        /// 
        // ***********************************************************************/
        public static Post GetPost(int postID, string username) {

            // We only want to call this code once per request
            if (HttpContext.Current.Items["Post" + postID] != null) {
                return (Post) HttpContext.Current.Items["Post" + postID];
            } else {
                Post post;

                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                post = dp.GetPost(postID, username, true);

                // Store in context of current request
                HttpContext.Current.Items["Post" + postID] = post;

                return post;
            }
        }


        // *********************************************************************
        //  GetPostDetails
        //
        /// <summary>
        /// Returns more detailed information about a Post than GetPost.
        /// </summary>
        /// <param name="PostID">The ID of the Post to obtain information about.</param>
        /// <param name="Username">The Username of the user viewing the post.</param>
        /// <param name="ConvertPostBodyFromRawToFormatted">A Boolean indicating whether or not
        /// to convert the post's raw database format into the HTML-ready formatted output.</param>
        /// <returns>A PostDetails object with detailed information about the post</returns>
        /// <remarks>GetPostDetails returns five bits of information that the GetPost method fails to:
        /// the PostID of the next/prev posts in the thread, the PostID of the first post in the
        /// next/prev threads, and whether or not the user viewing the post has email thread tracking
        /// turned on for the thread that the post belongs in.  If you don't need this extra information,
        /// call the less resource intensive GetPost. <seealso cref="GetPost"/></remarks>
        /// 
        // ***********************************************************************/
        public static PostDetails GetPostDetails(int postID, String username, bool convertPostBodyFromRawToFormatted) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            if (convertPostBodyFromRawToFormatted) {
                PostDetails retPost = dp.GetPostDetails(postID, username);
                retPost.Body = Globals.FormatPostBody(retPost.Body);
                return retPost;
            }
            else
                return dp.GetPostDetails(postID, username);
        }


        // *********************************************************************
        //  IsUserTrackingThread
        //
        /// <summary>
        /// Returns a boolean to indicate whether the user is tracking the thread.
        /// </summary>
        /// <param name="PostID">The ID of the Post to obtain information about.</param>
        /// <param name="Username">The Username of the user viewing the post.</param>
        /// 
        // ***********************************************************************/
        public static bool IsUserTrackingThread(int threadID, String username) {
            if (username == null)
                return false;

            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.IsUserTrackingThread(threadID, username);
        }

        // *********************************************************************
        //  GetPostDetails
        //
        /// <summary>
        /// Returns more detailed information about a Post than GetPost.
        /// </summary>
        /// <param name="PostID">The ID of the Post to obtain information about.</param>
        /// <param name="Username">The Username of the user viewing the post.</param>
        /// <returns>A PostDetails object with detailed information about the post</returns>
        /// <remarks>GetPostDetails returns five bits of information that the GetPost method fails to:
        /// the PostID of the next/prev posts in the thread, the PostID of the first post in the
        /// next/prev threads, and whether or not the user viewing the post has email thread tracking
        /// turned on for the thread that the post belongs in.  If you don't need this extra information,
        /// call the less resource intensive GetPost. <seealso cref="GetPost"/></remarks>
        /// 
        // ***********************************************************************/
        public static PostDetails GetPostDetails(int postID, String username) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetPostDetails(postID, username);
        }


        // *********************************************************************
        //  ReverseThreadTrackingOptions
        //
        /// <summary>
        /// This method reverses a user's thread tracking options for the thread containing a
        /// particular Post.
        /// </summary>
        /// <param name="Username">The user whose thread tracking options you wish to change.</param>
        /// <param name="PostID">The post of the thread whose tracking option you wish to reverse for
        /// the specified user.</param>
        /// 
        // ***********************************************************************/
        public static void ReverseThreadTrackingOptions(String username, int postID) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            dp.ReverseThreadTracking(username, postID);
        }


        // *********************************************************************
        //  MarkPostAsRead
        //
        /// <summary>
        /// Given a post id, marks it as read in the database for a user.
        /// </summary>
        /// <param name="postID">Id of post to mark as read</param>
        /// <param name="username">Mark read for this user</param>
        /// 
        // ********************************************************************/ 
        public static void MarkPostAsRead(int postID, string username) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            dp.MarkPostAsRead(postID, username);
        }

        // *********************************************************************
        //  GetThreadByPostID
        //
        /// <summary>
        /// This method returns a listing of the messages in a given thread.
        /// </summary>
        /// <param name="PostID">Specifies the PostID of a post that belongs to the thread that we are 
        /// interested in grabbing the messages from.</param>
        /// <returns>A PostCollection containing the posts in the thread.</returns>
        /// 
        // ********************************************************************/ 
        public static PostCollection GetThreadByPostID(int postID) {

            // This method may be called multiple times for a single request
            // we only want to call it once and use the same results for 
            // subsequent calls in the same request chaing
            if (HttpContext.Current.Items["Thread" + postID] != null) {
                return (PostCollection) HttpContext.Current.Items["Thread" + postID];
            } else {
                PostCollection posts;

                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                // Get the post collection
                posts = dp.GetThreadByPostID(postID, HttpContext.Current.User.Identity.Name);

                // Add to the context
                HttpContext.Current.Items["Thread" + postID] = posts;

                // Return the posts
                return posts;
            }
        }
        
        // *********************************************************************
        //  GetTop25NewPosts
        //
        /// <summary>
        /// This method returns the top 25 new posts.  These are the 25 posts
        /// most recently posted to on the boards.
        /// </summary>
        /// <param name="PostID">Specifies the PostID of a post that belongs to the thread that we are 
        /// interested in grabbing the messages from.</param>
        /// <returns>A PostCollection containing the posts in the thread.</returns>
        /// 
        // ********************************************************************/
        public static PostCollection GetTopNPopularPosts(string username, int postCount, int days)
        {
			return DataProvider.Instance().GetTopNPopularPosts(username, postCount, days);
		}

        public static PostCollection GetTopNNewPosts(string username, int postCount)
        {
            return DataProvider.Instance().GetTopNNewPosts(username, postCount);
        }
	
        // *********************************************************************
        //  GetThreadByPostID
        //
        /// <summary>
        /// This method returns a listing of the messages in a given thread using paging.
        /// </summary>
        /// <param name="PostID">Specifies the PostID of a post that belongs to the thread that we are 
        /// interested in grabbing the messages from.</param>
        /// <returns>A PostCollection containing the posts in the thread.</returns>
        /// 
        // ********************************************************************/ 
        public static PostCollection GetThreadByPostID(int postID, int currentPageIndex, int defaultPageSize, int sortBy, int sortOrder) {
            PostCollection posts;
            string postCollectionKey = postID.ToString();

            // Attempt to retrieve from Cache
            posts = (PostCollection) HttpContext.Current.Cache[postCollectionKey];

            if (posts == null) {
                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                posts = dp.GetThreadByPostID(postID, currentPageIndex, defaultPageSize, sortBy, sortOrder, HttpContext.Current.User.Identity.Name);			
            }

            return posts;
        }

        // *********************************************************************
        //  GetThread
        //
        /// <summary>
        /// This method returns a listing of the messages in a given thread.
        /// </summary>
        /// <param name="ThreadID">Specifies the ThreadID that we are interested in grabbing the
        /// messages from.</param>
        /// <returns>A PostCollection containing the posts in the thread.</returns>
        /// 
        // ********************************************************************/ 
        public static PostCollection GetThread(int threadID) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetThread(threadID);			
        }

	
        // *********************************************************************
        //  GetAllMessages
        //
        /// <summary>
        /// This method returns all of the messages for a particular forum 
        /// (specified by ForumID) and returns the messages in a particular
        /// format (specified by ForumView).
        /// </summary>
        /// <param name="ForumID">The ID of the Forum whose posts you are interested in retrieving.</param>
        /// <param name="ForumView">How to view the posts.  The three options are: Flat, Mixed, and Threaded.</param>
        /// <param name="PagesBack">How many pages back of posts to view.  Each forum has a 
        /// parameter indicating how many days worth of posts to show per page.</param>
        /// <returns>A PostCollection object containing the posts for the particular forum that fall within
        /// the particular page specified by PagesBack.</returns>
        /// 
        // ********************************************************************/ 
        public static PostCollection GetAllMessages(int forumID, ViewOptions forumView, int pagesBack) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            // make sure ForumView is set
            if (forumView == ViewOptions.NotSet)
                forumView = (ViewOptions) Globals.DefaultForumView;

            return dp.GetAllMessages(forumID, forumView, pagesBack);			
        }


        // *********************************************************************
        //  GetTotalPostCount
        //
        /// <summary>
        /// Returns the total count of all posts in the system
        /// </summary>
        /// <returns>A count of the total posts</returns>
        /// 
        // ********************************************************************/ 
        public static int GetTotalPostCount() {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetTotalPostCount();

        }


        // *********************************************************************
        //  AddPost
        //
        /// <summary>
        /// This method Adds a new post and returns a Post object containing information about the
        /// newly added post.
        /// </summary>
        /// <param name="PostToAdd">A Post object containing information about the post to add.
        /// This Post object need only have the following properties set: Subject, Body, Username,
        /// and ParentID or ForumID.  If the post is a new post, set ForumID; if it is a reply to
        /// an existing post, set the ParentID to the ID of the Post that is being replied to.</param>
        /// <returns>A Post object containing information about the newly added post.</returns>
        /// <remarks>The Post object being returned by the AddPost method indicates the PostID of the
        /// newly added post and specifies if the post is approved for viewing or not.</remarks>
        /// 
        // ********************************************************************/ 
        public static Post AddPost(Post postToAdd) {
            // convert the subject to the formatted version before adding the post
            postToAdd.Subject = PostSubjectRawToFormatted(postToAdd.Subject);

            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            Post newPost = dp.AddPost(postToAdd, HttpContext.Current.User.Identity.Name);

            // send emails to those that are tracking the thread (if it was an approved post)
            if (newPost.Approved)
                Emails.SendThreadTrackingEmails(newPost.PostID);
            else
                Emails.SendModeratorsNotification(newPost.ThreadID);

            return newPost;
        }

        // *********************************************************************
        //  UpdatePost
        //
        /// <summary>
        /// This method updates a post (called from the admin/moderator editing the post).
        /// </summary>
        /// <param name="UpdatedPost">Changes needing to be made to a particular post.  The PostID
        /// represents to post to update.</param>
        /// 
        // ********************************************************************/ 
        public static void UpdatePost(Post post, string editedBy) {
            post.Subject = PostSubjectRawToFormatted(post.Subject);

            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            dp.UpdatePost(post, editedBy);
        }

	
        // *********************************************************************
        //  PostSubjectRawToFormatted
        //
        /// <summary>
        /// Converts the subject line from raw text to the proper display.
        /// </summary>
        /// <param name="RawMessageSubject">The raw text of the subject line.</param>
        /// <returns>A prepared subject line.</returns>
        /// <remarks>PostSubjectRawToFormatted simply strips out any HTML tags from the subject.  It is this
        /// prepared subject line that is stored in the database.</remarks>
        /// 
        // ********************************************************************/ 
        public static String PostSubjectRawToFormatted(String rawMessageSubject) {		
            String strSubject = rawMessageSubject;
			
            // strip the HTML - i.e., turn < into &lt;, > into &gt;
            //strSubject = strSubject.Replace("<", "&lt;");
            //strSubject = strSubject.Replace(">", "&gt;");				

            strSubject = HttpContext.Current.Server.HtmlEncode(strSubject);
			
            return strSubject;
        } 
		

        // *********************************************************************
        //  DeletePost
        //
        /// <summary>
        /// Permanently deletes a post and all of its replies.
        /// </summary>
        /// <param name="PostID">The ID of the Post to delete.</param>
        /// <remarks>Use with care; keep in mind that this method deletes the specified post *and*
        /// all of the posts that are replies to this post.</remarks>
        /// 
        // ********************************************************************/ 
        public static void DeletePost(int postID, string approvedBy, string reason) {
            // send the reason why the post was deleted
            if (reason.Length == 0) reason = "NO REASON GIVEN";
            
            Emails.SendEmail(((Post) Posts.GetPost(postID, null)).Username, EmailTypeEnum.MessageDeleted, postID, reason);

            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            dp.DeletePost(postID, approvedBy, reason);
        }

        public enum PostType {
            Post,
            Vote
        }
    }
}
