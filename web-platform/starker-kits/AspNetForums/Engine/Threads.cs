using System;
using System.Collections;
using System.Web;
using System.IO;
using System.Web.Caching;
using AspNetForums.Components;

namespace AspNetForums {

    // *********************************************************************
    //  Threads
    //
    /// <summary>
    /// This class contains methods for working with an individual post.  There are methods to
    /// Add a New Post, Update an Existing Post, retrieve a single post, etc.
    /// </summary>
    // ***********************************************************************/
    public class Threads {

        // *********************************************************************
        //  GetAllThreads
        //
        /// <summary>
        /// Returns a collection of threads based on the properties specified
        /// </summary>
        /// <param name="forumID">Id of the forum to retrieve posts from</param>
        /// <param name="pageSize">Number of results to return</param>
        /// <param name="pageIndex">Location in results set to return</param>
        /// <param name="endDate">Results before this date</param>
        /// <param name="username">Username asking for the threads</param>
        /// <param name="unreadThreadsOnly">Return unread threads</param>
        /// <returns>A collection of threads</returns>
        // ***********************************************************************/
        public static ThreadCollection GetAllThreads(int forumID, int pageSize, int pageIndex, DateTime endDate, string username, bool unreadThreadsOnly) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            ThreadCollection threads;

            // If the user is anonymous take some load off the db
            if (username == null) {
                if (HttpContext.Current.Cache["Thread-" + forumID + pageSize.ToString() + pageIndex.ToString() + endDate.ToString()] != null)
                    return (ThreadCollection) HttpContext.Current.Cache["Thread-" + forumID + pageSize.ToString() + pageIndex.ToString() + endDate.ToString()];
            }

            // Get the threads
            threads =  dp.GetAllThreads(forumID, pageSize, pageIndex, endDate, username, unreadThreadsOnly);			

            if (username == null)
                HttpContext.Current.Cache.Insert("Thread-" + forumID + pageSize.ToString() + pageIndex.ToString() + endDate.ToString(), threads, null, DateTime.Now.AddMinutes(1), TimeSpan.Zero);

            return threads;
        }

        // *********************************************************************
        //  GetNextThreadID
        //
        /// <summary>
        /// Returns the id of the next thread.
        /// </summary>
        /// <param name="postID">Current threadid is determined from postsid</param>
        // ***********************************************************************/
        public static int GetNextThreadID(int postID) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetNextThreadID(postID);			
        }

        // *********************************************************************
        //  GetPrevThreadID
        //
        /// <summary>
        /// Returns the id of the previous thread.
        /// </summary>
        /// <param name="postID">Current threadid is determined from postsid</param>
        // ***********************************************************************/
        public static int GetPrevThreadID(int postID) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetPrevThreadID(postID);			
        }

        // *********************************************************************
        //  GetAllThreads
        //
        /// <summary>
        /// Returns a collection of threads based on the properties specified
        /// </summary>
        /// <param name="forumID">Id of the forum to retrieve posts from</param>
        /// <param name="username">Username asking for the threads</param>
        /// <param name="unreadThreadsOnly">Return unread threads</param>
        /// <returns>A collection of threads</returns>
        // ***********************************************************************/
        public static ThreadCollection GetAllThreads(int forumID, string username, bool unreadThreadsOnly) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetAllThreads(forumID, username, unreadThreadsOnly);			
        }



        // *********************************************************************
        //  GetThreadsUserMostRecentlyParticipatedIn
        //
        /// <summary>
        /// Returns threads that the user has recently participated in.
        /// </summary>
        /// <param name="username">Username to get tracked posts for</param>
        /// <returns>Thread collection of threads</returns>
        // ***********************************************************************/
        public static ThreadCollection GetThreadsUserMostRecentlyParticipatedIn(string username) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetThreadsUserMostRecentlyParticipatedIn(username);
        }

        // *********************************************************************
        //  GetThreadsUserIsTracking
        //
        /// <summary>
        /// Returns threads that the user has email tracking enabled for.
        /// </summary>
        /// <param name="username">Username to get tracked posts for</param>
        /// <returns>Thread collection of threads being tracked</returns>
        // ***********************************************************************/
        public static ThreadCollection GetThreadsUserIsTracking(string username) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetThreadsUserIsTracking(username);
        }

        // *********************************************************************
        //  GetTotalPostsForThread
        //
        /// <summary>
        /// Total number of posts in a given thread
        /// </summary>
        /// <param name="postID">Id of the post to find replies</param>
        /// <returns>Total posts in the thread</returns>
        // ***********************************************************************/
        public static int GetTotalPostsForThread(int postID) {

            int postCount = 0;

            // Only do this once per request
            if (HttpContext.Current.Items["TotalPostsForThread-" + postID] != null) {

                postCount = Convert.ToInt32(HttpContext.Current.Items["TotalPostsForThread-" + postID]);

            } 
            else {

                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                postCount = dp.GetTotalPostsForThread(postID);

                HttpContext.Current.Items["TotalPostsForThread-" + postID] = postCount;
            }

            return postCount;
        }
    }
}
