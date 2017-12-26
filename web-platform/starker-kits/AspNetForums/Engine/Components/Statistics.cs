using System;
using System.Web;
using System.Web.Caching;

namespace AspNetForums.Components {

    // *********************************************************************
    //  Statistics
    //
    /// <summary>
    /// This class contains is used to get statistics about the running ASP.NET Forum
    /// </summary>
    // ***********************************************************************/
    public class Statistics {
        int totalUsers;
        int totalPosts;
        int totalThreads;
        int totalModerators;
        int totalModeratedPosts;
        int newPostsInPast24Hours;
        int newThreadsInPast24Hours;
        int newUsersInPast24Hours;
        int mostViewsPostId;
        string mostViewsSubject;
        int mostActivePostId;
        string mostActiveSubject;
        int mostReadPostId;
        string mostReadPostSubject;
        string mostActiveUser;
        string newestUser;

        // *********************************************************************
        //  Statistics
        //
        /// <summary>
        /// This class contains is used to get statistics about the running ASP.NET Forum.
        /// This is a really expensive SQL query, so we'll take advantage of caching to only
        /// update it once every 2 hours.
        /// </summary>
        // ***********************************************************************/
        public static Statistics GetSiteStatistics() {

            // Cached lookup
            if (HttpContext.Current.Cache["SiteStatistics"] != null) {
                return (Statistics) HttpContext.Current.Cache["SiteStatistics"];
            } else {
                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                Statistics stats = dp.GetSiteStatistics();
                HttpContext.Current.Cache.Insert("SiteStatistics", stats, null, DateTime.Now.AddHours(2), TimeSpan.Zero);
                return stats;
            }

        }

        public int TotalUsers {
            get { return totalUsers; }
            set {
                if (value < 0)
                    totalUsers = 0;
                else
                    totalUsers = value;
            }
        }

        public int TotalModerators {
            get { return totalModerators; }
            set {
                if (value < 0)
                    totalModerators = 0;
                else
                    totalModerators = value;
            }
        }

        public int TotalModeratedPosts {
            get { return totalModeratedPosts; }
            set {
                if (value < 0)
                    totalModeratedPosts = 0;
                else
                    totalModeratedPosts = value;
            }
        }

        // *********************************************************************
        //  TotalPosts
        //
        /// <summary>
        /// Specifies the total number of posts made to the board.
        /// </summary>
        // ***********************************************************************/
        public int TotalPosts {
            get { return totalPosts; }
            set {
                if (value < 0)
                    totalPosts = 0;
                else
                    totalPosts = value;
            }
        }

        // *********************************************************************
        //  TotalThreads
        //
        /// <summary>
        /// Specifies the total number of threads (top-level posts) made.
        /// </summary>
        // ***********************************************************************/
        public int TotalThreads {
            get { return totalThreads; }
            set {
                if (value < 0)
                    totalThreads = 0;
                else
                    totalThreads = value;
            }
        }

        // *********************************************************************
        //  NewPostsInPast24Hours
        //
        /// <summary>
        /// Specifies the number of posts made in the last 24 hours.
        /// </summary>
        // ***********************************************************************/
        public int NewPostsInPast24Hours {
            get { return newPostsInPast24Hours; }
            set {
                if (value < 0)
                    newPostsInPast24Hours = 0;
                else
                    newPostsInPast24Hours = value;
            }
        }

        // *********************************************************************
        //  NewUsersInPast24Hours
        //
        /// <summary>
        /// Specifies the number of users added in the last 24 hours.
        /// </summary>
        // ***********************************************************************/
        public int NewUsersInPast24Hours {
            get { return newUsersInPast24Hours; }
            set {
                if (value < 0)
                    newUsersInPast24Hours = 0;
                else
                    newUsersInPast24Hours = value;
            }
        }

        // *********************************************************************
        //  NewThreadsInPast24Hours
        //
        /// <summary>
        /// Specifies the number of threads (top-level posts) made in the last 24 hours.
        /// </summary>
        // ***********************************************************************/
        public int NewThreadsInPast24Hours {
            get { return newThreadsInPast24Hours; }
            set {
                if (value < 0)
                    newThreadsInPast24Hours = 0;
                else
                    newThreadsInPast24Hours = value;
            }
        }

        // *********************************************************************
        //  MostViewsPostID
        //
        /// <summary>
        /// The Post with the most number of views in the past 3 days
        /// </summary>
        // ***********************************************************************/
        public int MostViewsPostID {
            get { return mostViewsPostId; }
            set {
                if (value < 0)
                    mostViewsPostId = 0;
                else
                    mostViewsPostId = value;
            }
        }

        // *********************************************************************
        //  MostViewsSubject
        //
        /// <summary>
        /// The Post with the most number of views in the past 3 days
        /// </summary>
        // ***********************************************************************/
        public String MostViewsSubject {
            get { return mostViewsSubject; }
            set {
                mostViewsSubject = value;
            }
        }

        // *********************************************************************
        //  MostActivePostID
        //
        /// <summary>
        /// The Post with the most replies in the past 3 days.
        /// </summary>
        // ***********************************************************************/
        public int MostActivePostID {
            get { return mostActivePostId; }
            set {
                if (value < 0)
                    mostActivePostId = 0;
                else
                    mostActivePostId = value;
            }
        }

        // *********************************************************************
        //  MostViewsSubject
        //
        /// <summary>
        /// The Post with the most replies in the past 3 days.
        /// </summary>
        // ***********************************************************************/
        public String MostActiveSubject {
            get { return mostActiveSubject; }
            set {
                mostActiveSubject = value;
            }
        }

        // *********************************************************************
        //  MostReadPostID
        //
        /// <summary>
        /// The Post the most number of users have read in the past 3 days
        /// </summary>
        // ***********************************************************************/
        public int MostReadPostID {
            get { return mostReadPostId; }
            set {
                if (value < 0)
                    mostReadPostId = 0;
                else
                    mostReadPostId = value;
            }
        }

        // *********************************************************************
        //  MostReadPostSubject
        //
        /// <summary>
        /// The Post the most number of users have read in the past 3 days
        /// </summary>
        // ***********************************************************************/
        public String MostReadPostSubject {
            get { return mostReadPostSubject; }
            set {
                mostReadPostSubject = value;
            }
        }

        // *********************************************************************
        //  MostActiveUser
        //
        /// <summary>
        /// The most active user
        /// </summary>
        // ***********************************************************************/
        public String MostActiveUser {
            get { return mostActiveUser; }
            set {
                mostActiveUser = value;
            }
        }

        // *********************************************************************
        //  NewestUser
        //
        /// <summary>
        /// The newest user to join
        /// </summary>
        // ***********************************************************************/
        public String NewestUser {
            get { return newestUser; }
            set {
                newestUser = value;
            }
        }
    }
}
