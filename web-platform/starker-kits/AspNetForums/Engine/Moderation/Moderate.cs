using System;
using System.Web;
using AspNetForums;
using AspNetForums.Components;


namespace AspNetForums {

    // *********************************************************************
    //  Moderate
    //
    /// <summary>
    /// This class contains methods that are helpful for moderating posts.
    /// </summary>
    /// 
    // ********************************************************************/ 
    public class Moderate {

        // *********************************************************************
        //  CanEditPost
        //
        /// <summary>
        /// This method determines whether or not a user can edit a particular post.
        /// </summary>
        /// <param name="strUsername">The username of the user who you wish to know if he can edit
        /// the post.</param>
        /// <param name="iPostID">To post you wish to know if the user can edit.</param>
        /// <returns>A boolean value: true if the user can edit the post, false otherwise.</returns>
        /// <remarks>Moderators can edit posts that are still waiting for approval in the forum(s) they 
        /// are cleared to moderate.  Forum administrators may edit any post, awaiting approval or not,
        /// at any time.</remarks>
        /// 
        // ********************************************************************/ 
        public static bool CanEditPost(String Username, int PostID) {			
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.CanEditPost(Username, PostID);
        }
		
        // *********************************************************************
        //  GetQueueStatus
        //
        /// <summary>
        /// Returns details about the moderation queue
        /// </summary>
        /// <param name="username">The username making the request</param>
        /// <param name="forumId">Forum to check-on</param>
        /// <returns>A boolean value: true if the user can edit the post, false otherwise.</returns>
        /// <remarks>Moderators can edit posts that are still waiting for approval in the forum(s) they 
        /// are cleared to moderate.  Forum administrators may edit any post, awaiting approval or not,
        /// at any time.</remarks>
        /// 
        // ********************************************************************/ 
        public static ModerationQueueStatus GetQueueStatus(int forumID, string username) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetQueueStatus(forumID, username);
        }

        // *********************************************************************
        //  GetMostActiveModerators
        //
        /// <summary>
        /// Returns a list of the users that have added the most posts
        /// </summary>
        /// <returns>A UserCollection</returns>
        /// 
        // ********************************************************************/
        public static ModeratorCollection GetMostActiveModerators() {
            ModeratorCollection moderators;

            // Only update once every 24 hours
            if (HttpContext.Current.Cache["MostActiveModerators"] == null) {
                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                // Get the collection
                moderators = dp.GetMostActiveModerators();

                // add to the cache
                HttpContext.Current.Cache.Insert("MostActiveModerators", moderators, null, DateTime.Now.AddDays(1), TimeSpan.Zero);

            }

            return (ModeratorCollection) HttpContext.Current.Cache["MostActiveModerators"];

        }

        // *********************************************************************
        //  GetModerationAuditSummary
        //
        /// <summary>
        /// Returns a collection of moderation audit items.
        /// </summary>
        /// 
        // ********************************************************************/ 
        public static ModerationAuditCollection GetModerationAuditSummary() {

            ModerationAuditCollection moderationAudit;

            // Only update once every 24 hours
            if (HttpContext.Current.Cache["ModerationAuditSummary"] == null) {

                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                // Get the audit details
                moderationAudit = dp.GetModerationAuditSummary();

                // Store in Cache
                HttpContext.Current.Cache.Insert("ModerationAuditSummary", moderationAudit, null, DateTime.Now.AddDays(1), TimeSpan.Zero);
            }

            return (ModerationAuditCollection) HttpContext.Current.Cache["ModerationAuditSummary"];
        }
            

        // *********************************************************************
        //  GetForumGroupsForModeration
        //
        /// <summary>
        /// Returns a collection of forums that require moderation.
        /// </summary>
        /// <param name="username">Name of user requesting list</param>
        /// <returns>A collection of forums that require moderation and that the current
        /// user has access to.</returns>
        /// 
        // ********************************************************************/ 
        public static ForumGroupCollection GetForumGroupsForModeration(string username) {
            ForumGroupCollection forumGroups;

            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            // Get the collection
            forumGroups = dp.GetForumGroupsForModeration(username);

            // Sort the collection
            forumGroups.Sort();

            return forumGroups;
        }

        // *********************************************************************
        //  GetAllUnmoderatedThreads
        //
        /// <summary>
        /// A list of threads for a given forum that need to be moderated.
        /// </summary>
        /// <param name="forumID">ID of the forum to return threads for</param>
        /// <param name="pageSize">How many items to return per request</param>
        /// <param name="pageIndex">What location in the page list</param>
        /// <param name="username">Username making the request</param>
        /// <returns>A collection of threads</returns>
        /// 
        // ********************************************************************/ 
        public static ThreadCollection GetAllUnmoderatedThreads(int forumID, int pageSize, int pageIndex, string username) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetAllUnmoderatedThreads(forumID, pageSize, pageIndex, username);
        }

        // *********************************************************************
        //  GetTotalUnModeratedThreadsInForum
        //
        /// <summary>
        /// A count of all the threads requring moderation in a given forum.
        /// </summary>
        /// <param name="forumID">ID of the forum to return threads for</param>
        /// <param name="maxDateTime">Date constraint</param>
        /// <param name="minDateTime">Data constraint</param>
        /// <param name="username">Username making the request</param>
        /// <returns>A collection of threads</returns>
        /// 
        // ********************************************************************/ 
        /* TODO: REMOVE?
        public static int GetTotalUnModeratedThreadsInForum(int ForumID, DateTime maxDateTime, DateTime minDateTime, string username, bool unreadThreadsOnly) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetTotalUnModeratedThreadsInForum(ForumID, maxDateTime, minDateTime, username, unreadThreadsOnly);
        }
        */

        // *********************************************************************
        //  GetForumsForModerationByForumGroupId
        //
        /// <summary>
        /// Returns forums requiring moderation by forum group id
        /// </summary>
        /// <param name="forumGroupId">Id of the forum group to return forums for</param>
        /// <param name="username">Username making the request</param>
        /// 
        // ********************************************************************/ 
        public static ModeratedForumCollection GetForumsForModerationByForumGroupId(int forumGroupId, string username) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetForumsForModerationByForumGroupId(forumGroupId, username);
        }


        // *********************************************************************
        //  MovePost
        //
        /// <summary>
        /// Moves a post from its current forum to another.
        /// </summary>
        /// <param name="iPostID">The post to move.</param>
        /// <param name="MoveToForumID">The forum to move the post to.</param>
        /// <param name="Username">The user attempting to move the post.</param>
        /// <returns>A MovedPostStatus enumeration member indicating the resulting status: 
        /// NotMoved, meaning the post was not moved; MovedButNotApproved, meaning the post was
        /// moved, but not approved in the new forum; or MovedAndApproved, meaning that the post
        /// was moved and approved in the new forum.</returns>
        /// <remarks>A post moved from one forum to another is automatically approved if the person
        /// moving the post is also a moderate in the forum that the post is being moved to.  Moving a
        /// post can fail if the user attempts to move a post that has already been approved.</remarks>
        /// 
        // ********************************************************************/ 
        public static MovedPostStatus MovePost(int postID, int moveToForumID, String approvedBy, bool sendEmail) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            MovedPostStatus status = dp.MovePost(postID, moveToForumID, approvedBy);

            if (sendEmail) {
                if (status == MovedPostStatus.MovedAndApproved) {
                    // the post was moved to a new forum and automatically approved
                    // send an email explaining what happened, and send thread tracking emails
                    Emails.SendThreadTrackingEmails(postID);
                    Emails.SendMessageMovedAndApprovedEmail(postID);
                }
                else if (status == MovedPostStatus.MovedButNotApproved) {
                    // the post was moved, but is still waiting to approval from a moderate in the
                    // forum is was moved to.  Send an email explaining this to the user.
                    Emails.SendMessageMovedAndNotApprovedEmail(postID);
                }
            }

            return status;
        }



        // *********************************************************************
        //  DeletePost
        //
        /// <summary>
        /// This method delets a post and all of its replies.  It also includes a reason on why the
        /// post was deleted.
        /// </summary>
        /// <param name="PostID">The post to delete.</param>
        /// <param name="ReasonForDeleting">The reason why the post was deleted.</param>
        /// <returns>A boolean, indiciating if the post was successfully deleted.</returns>
        /// <remarks>The user of the post being deleted is automatically sent an email
        /// explaining why his or her post was removed.</remarks>
        /// 
        // ********************************************************************/ 
        public static void DeletePost(int postID, string approvedBy, string reasonForDeleting, bool sendEmail) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            // delete the post - if we succeed, send an email to the person who had their
            // post deleted, explaining why it was deleted
            if (reasonForDeleting.Length == 0) reasonForDeleting = "NO REASON GIVEN.";

            // Send email if we delete the post
            if (sendEmail)
                Emails.SendEmail(((Post) Posts.GetPost(postID, null)).Username, EmailTypeEnum.MessageDeleted, postID, null, reasonForDeleting);

            // Delete the post
            dp.DeletePost(postID, approvedBy, reasonForDeleting);

        }


        // *********************************************************************
        //  ApprovePost
        //
        /// <summary>
        /// Approves a moderated post.
        /// </summary>
        /// <param name="postID">The post to approve.</param>
        /// <param name="approvedBy">Username approving the post.</param>
        /// 
        // ********************************************************************/ 
        public static void ApprovePost(int postID, string approvedBy) {
            ApprovePost(postID, approvedBy, null);
        }


        // *********************************************************************
        //  ApprovePost
        //
        /// <summary>
        /// Approves a moderated post.
        /// </summary>
        /// <param name="postID">The post to approve.</param>
        /// <param name="approvedBy">Username approving the post.</param>
        /// <param name="updateUserAsTrusted">Approve the username as no longer requiring moderation</param>
        /// 
        // ********************************************************************/ 
        public static void ApprovePost(int postID, string approvedBy, string updateUserAsTrusted) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            bool firstTimeApproved = dp.ApprovePost(postID, approvedBy, updateUserAsTrusted);

            if (firstTimeApproved) {
                // this means that the post hadn't been approved before, send confirmation
                Emails.SendThreadTrackingEmails(postID);
                Emails.SendMessageApprovedEmail(postID);
            }
        }


        // *********************************************************************
        //  CanModerate
        //
        /// <summary>
        /// Determines whether or not a user can moderate any forums.
        /// </summary>
        /// <param name="Username">The user who you are interested in determining has moderation
        /// capabilities.</param>
        /// <returns>A boolean, indicating whether or not the user can moderate.</returns>
        /// <remarks>This method does not indicate *what* forums a user can moderate, it just serves to
        /// determine IF a user can moderate at all.</remarks>
        /// 
        // ********************************************************************/ 
        // TODO: REMOVE?
        public static bool CanModerate(String Username) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.CanModerate(Username);
        }
		
        // *********************************************************************
        //  CanModerate
        //
        /// <summary>
        /// Determines whether or not a user can moderate any forums.
        /// </summary>
        /// <param name="Username">The user who you are interested in determining has moderation
        /// capabilities.</param>
        /// <returns>A boolean, indicating whether or not the user can moderate.</returns>
        /// <remarks>This method does not indicate *what* forums a user can moderate, it just serves to
        /// determine IF a user can moderate at all.</remarks>
        /// 
        // ********************************************************************/ 
        public static bool CanModerate(String username, int forumId) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.CanModerate(username, forumId);
        }


        // *********************************************************************
        //  GetPostsAwaitingModeration
        //
        /// <summary>
        /// Returns a SqlDataReader full of the posts that are waiting approval in the forum(s)
        /// that a particular user has moderation rights for.
        /// </summary>
        /// <param name="Username">The user name of the individual currently moderating.</param>
        /// <returns>A PostCollection with the unapproved posts that the user can moderate.</returns>
        /// 
        // ********************************************************************/ 
        // TODO: REMOVE?
        /*
        public static PostCollection GetPostsAwaitingModeration(String Username) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetPostsAwaitingModeration(Username);
        }
        */

        // *********************************************************************
        //  GetPostsAwaitingModeration
        //
        /// <summary>
        /// Returns a bool indicating whether the given user has posts to moderate
        /// </summary>
        /// <param name="username">The user name of the individual currently moderating.</param>
        /// 
        // ********************************************************************/ 
        public static bool UserHasPostsAwaitingModeration(String username) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.UserHasPostsAwaitingModeration(username);
        }

        /// <summary>
        /// This method returns a list of users who can moderate a particular forum
        /// </summary>
        /// <param name="ForumID">The Forum you are interested in finding those who can moderate it.</param>
        /// <returns>A ModeratedForumCollection object that lists the users who can moderate the specified
        /// forum.</returns>
        // TODO: REMOVE?
        public static ModeratedForumCollection GetForumModerators(int ForumID) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetForumModerators(ForumID);
        }
    }
}