using System;
using System.Data;
using System.Data.SqlClient;
using AspNetForums.Components;
using System.Web;
using System.Web.Mail;
using System.IO;
using System.Text.RegularExpressions;
using System.Collections;

namespace AspNetForums.Data {


    /// <summary>
    /// Summary description for WebForumsDataProvider.
    /// </summary>
    public class SqlDataProvider : IDataProviderBase {

        /****************************************************************
        // GetNextThreadID
        //
        /// <summary>
        /// Gets the next threadid based on the postid
        /// </summary>
        //
        ****************************************************************/
        public ModerationQueueStatus GetQueueStatus(int forumID, string username) {
            ModerationQueueStatus moderationQueue = new ModerationQueueStatus();

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetUnmoderatedPostStatus", myConnection);
            SqlDataReader reader;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@ForumID", SqlDbType.Int).Value = forumID;
            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();

            reader = myCommand.ExecuteReader();

            while (reader.Read()) {
                moderationQueue.AgeInMinutes = (int) reader["OldestPostAgeInMinutes"];
                moderationQueue.Count = (int) reader["TotalPostsInModerationQueue"];
            }

            reader.Close();
            myConnection.Close();

            return moderationQueue;
        }

        /****************************************************************
        // GetNextThreadID
        //
        /// <summary>
        /// Gets the next threadid based on the postid
        /// </summary>
        //
        ****************************************************************/
        public int GetNextThreadID(int postID) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetPrevNextThreadID", myConnection);
            SqlDataReader reader;
            int threadID = postID;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@PostID", SqlDbType.Int).Value = postID;
            myCommand.Parameters.Add("@NextThread", SqlDbType.Bit).Value = 1;

            // Execute the command
            myConnection.Open();

            reader = myCommand.ExecuteReader();

            while (reader.Read())
                threadID = (int) reader["ThreadID"];

            reader.Close();
            myConnection.Close();

            return threadID;
        }

        /****************************************************************
        // GetPrevThreadID
        //
        /// <summary>
        /// Gets the prev threadid based on the postid
        /// </summary>
        //
        ****************************************************************/
        public int GetPrevThreadID(int postID) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetPrevNextThreadID", myConnection);
            SqlDataReader reader;
            int threadID = postID;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@PostID", SqlDbType.Int).Value = postID;
            myCommand.Parameters.Add("@NextThread", SqlDbType.Bit).Value = 0;

            // Execute the command
            myConnection.Open();

            reader = myCommand.ExecuteReader();

            while (reader.Read())
                threadID = (int) reader["ThreadID"];

            reader.Close();
            myConnection.Close();

            return threadID;
        }

        /****************************************************************
        // Vote
        //
        /// <summary>
        /// Votes for a poll
        /// </summary>
        //
        ****************************************************************/
        public void Vote(int postID, string selection) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_Vote", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@PostID", SqlDbType.Int).Value = postID;
            myCommand.Parameters.Add("@Vote", SqlDbType.NVarChar, 2).Value = selection;

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }
        
        /****************************************************************
        // GetVoteResults
        //
        /// <summary>
        /// Returns a collection of threads that the user has recently partipated in.
        /// </summary>
        //
        ****************************************************************/
        public VoteResultCollection GetVoteResults(int postID) {
            VoteResult voteResult;
            VoteResultCollection voteResults = new VoteResultCollection();

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetVoteResults", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@PostID", SqlDbType.Int).Value = postID;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            
            // Read the values
            //
            while (dr.Read()) {
                voteResult = new VoteResult();
                voteResult.Vote = (string) dr["Vote"];
                voteResult.VoteCount = (int) dr["VoteCount"];

                voteResults.Add(voteResult.Vote,voteResult);
            }
            
            // Close the conneciton
            myConnection.Close();

            return voteResults;
        }

        /****************************************************************
        // GetThreadsUserMostRecentlyParticipatedIn
        //
        /// <summary>
        /// Returns a collection of threads that the user has recently partipated in.
        /// </summary>
        //
        ****************************************************************/
        public ThreadCollection GetThreadsUserMostRecentlyParticipatedIn(string username) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetTopicsUserMostRecentlyParticipatedIn", myConnection);
            ThreadCollection threads;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            threads = new ThreadCollection();

            while (dr.Read()) {
                threads.Add(PopulateThreadFromSqlDataReader(dr));
            }
            dr.Close();
            myConnection.Close();

            // Only return the posts specified through paging

            return threads;
        }


        /****************************************************************
        // GetThreadsUserIsTracking
        //
        /// <summary>
        /// Returns a collection of threads that the user is tracking
        /// </summary>
        //
        ****************************************************************/
        public ThreadCollection GetThreadsUserIsTracking(string username) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetTopicsUserIsTracking", myConnection);
            ThreadCollection threads;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            threads = new ThreadCollection();

            while (dr.Read()) {
                threads.Add(PopulateThreadFromSqlDataReader(dr));
            }
            dr.Close();
            myConnection.Close();

            // Only return the posts specified through paging

            return threads;
        }


        /****************************************************************
        // FindUsersByName
        //
        /// <summary>
        /// Returns a collection of users matching the name value provided.
        /// </summary>
        //
        ****************************************************************/
        public UserCollection FindUsersByName(int pageIndex, int pageSize, string usernameToMatch) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_FindUsersByName", myConnection);
            SqlDataReader reader;
            UserCollection users = new UserCollection();
            User user;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            myCommand.Parameters.Add("@PageIndex", SqlDbType.Int).Value = pageIndex;
            myCommand.Parameters.Add("@PageSize", SqlDbType.Int).Value = pageSize;
            myCommand.Parameters.Add("@UsernameToFind", SqlDbType.NVarChar, 50).Value = usernameToMatch;

            // Execute the command
            myConnection.Open();
            reader = myCommand.ExecuteReader();

            while (reader.Read()) {
                user = this.PopulateUserFromSqlDataReader(reader);
                users.Add(user);
            }

            reader.Close();
            myConnection.Close();

            return users;
        }

        
        
        /****************************************************************
        // GetTopNPopularPosts
        //
        /// <summary>
        /// TODO
        /// </summary>
        //
        ****************************************************************/
        public PostCollection GetTopNPopularPosts(string username, int postCount, int days) {
            return GetTopNPosts(username, postCount, days, "TotalViews");
        }
        
        /****************************************************************
        // GetTopNPopularPosts
        //
        /// <summary>
        /// ToDO
        /// </summary>
        //
        ****************************************************************/
        public PostCollection GetTopNNewPosts(string username, int postCount) {
            return GetTopNPosts(username, postCount, 0, "ThreadDate");
        }
        
        /****************************************************************
        // GetTopNPopularPosts
        //
        /// <summary>
        /// TODO
        /// </summary>
        //
        ****************************************************************/
        private PostCollection GetTopNPosts(string username, int postCount, int days, string sort) {
            PostCollection posts = new PostCollection();
            SqlConnection myConnection;

            using(myConnection = new SqlConnection(Globals.DatabaseConnectionString)) {
                SqlCommand myCommand = new SqlCommand("dbo.forums_GetTopNPosts", myConnection);
                myCommand.CommandType = CommandType.StoredProcedure;
                myCommand.Parameters.Add("@UserName", SqlDbType.NVarChar, 50).Value = username;
                myCommand.Parameters.Add("@SortType", SqlDbType.NVarChar, 50).Value = sort;
                myCommand.Parameters.Add("@PostCount", SqlDbType.Int, 4).Value = postCount;
                myCommand.Parameters.Add("@DaysToCount", SqlDbType.Int, 4).Value = days;
                
                myConnection.Open();
                using(SqlDataReader dr = myCommand.ExecuteReader()) {
                    Post post = null;
                    
                    while(dr.Read()) {
                        post = PopulatePostFromSqlDataReader(dr);
                        posts.Add(post);
                    }
                }
            }

            // Close the connection
            myConnection.Close();
            return posts;
        }

        /****************************************************************
        // GetModerationAuditSummary
        //
        /// <summary>
        /// Returns a summary of moderation audit details.
        /// </summary>
        //
        ****************************************************************/
        public ModerationAuditCollection GetModerationAuditSummary() {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("Statistics_GetModerationActions", myConnection);
            SqlDataReader reader;
            ModerationAuditCollection moderationAudits = new ModerationAuditCollection();
            ModerationAuditSummary moderationAudit;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Execute the command
            myConnection.Open();
            reader = myCommand.ExecuteReader();

            while (reader.Read()) {
                moderationAudit = new ModerationAuditSummary();
                moderationAudit.Action = (string) reader["Description"];
                moderationAudit.ActionSummary = Convert.ToInt32(reader["TotalActions"]);

                moderationAudits.Add(moderationAudit);
            }

            reader.Close();
            myConnection.Close();

            return moderationAudits;

        }

        /****************************************************************
        // GetMostActiveModerators
        //
        /// <summary>
        /// Returns a collection of the most active moderators.
        /// </summary>
        //
        ****************************************************************/
        public ModeratorCollection GetMostActiveModerators() {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("Statistics_GetMostActiveModerators", myConnection);
            SqlDataReader reader;
            ModeratorCollection moderators = new ModeratorCollection();
            Moderator moderator;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Execute the command
            myConnection.Open();
            reader = myCommand.ExecuteReader();

            while (reader.Read()) {
                moderator = new Moderator();
                moderator.Username = (string) reader["Username"];
                moderator.TotalPostsModerated = Convert.ToInt32(reader["PostsModerated"]);

                moderators.Add(moderator);
            }

            reader.Close();
            myConnection.Close();

            return moderators;

        }


        /****************************************************************
        // GetMostActiveUsers
        //
        /// <summary>
        /// Returns a collection of the most active users.
        /// </summary>
        //
        ****************************************************************/
        public UserCollection GetMostActiveUsers() {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("Statistics_GetMostActiveUsers", myConnection);
            SqlDataReader reader;
            UserCollection users = new UserCollection();
            User user;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Execute the command
            myConnection.Open();
            reader = myCommand.ExecuteReader();

            while (reader.Read()) {
                user = new User();
                user.Username = (string) reader["Username"];
                user.TotalPosts = Convert.ToInt32(reader["TotalPosts"]);

                users.Add(user);
            }

            reader.Close();
            myConnection.Close();

            return users;

        }


        /****************************************************************
        // GetAllUnmoderatedThreads
        //
        /// <summary>
        /// Returns a collection of all posts that have yet to be approved.
        /// </summary>
        //
        ****************************************************************/
        public ThreadCollection GetAllUnmoderatedThreads(int forumID, int pageSize, int pageIndex, string username) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetAllUnmoderatedTopicsPaged", myConnection);
            ThreadCollection threads;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@ForumId", SqlDbType.Int, 4).Value = forumID;
            myCommand.Parameters.Add("@PageSize", SqlDbType.Int, 4).Value = pageSize;
            myCommand.Parameters.Add("@PageIndex", SqlDbType.Int, 4).Value = pageIndex;
            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            threads = new ThreadCollection();

            while (dr.Read()) {
                threads.Add(PopulateThreadFromSqlDataReader(dr));
            }
            dr.Close();
            myConnection.Close();

            // Only return the posts specified through paging
            return threads;
        }

        /****************************************************************
        // GetTotalUnModeratedThreadsInForum
        //
        /// <summary>
        /// Returns a count of all posts that have yet to be approved.
        /// </summary>
        //
        ****************************************************************/
        public int GetTotalUnModeratedThreadsInForum(int ForumID, DateTime maxDateTime, DateTime minDateTime, string username, bool unreadThreadsOnly) {
            return 0;
        }

        /****************************************************************
        // GetForumsForModerationByForumGroupId
        //
        /// <summary>
        /// Returns forums in a given forum group for moderation
        /// </summary>
        //
        ****************************************************************/
        public ModeratedForumCollection GetForumsForModerationByForumGroupId(int forumGroupId, string username) {

            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetForumsForModerationByForumGroupId", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@ForumGroupId", SqlDbType.Int).Value = forumGroupId;
            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            // populate a ForumCollection object
            ModeratedForumCollection forums = new ModeratedForumCollection();
            ModeratedForum moderatedForum;

            while (dr.Read()) {

                moderatedForum = PopulateModeratedForumFromSqlDataReader(dr);

                moderatedForum.TotalPosts = Convert.ToInt32(dr["TotalPosts"]);
                moderatedForum.TotalThreads = Convert.ToInt32(dr["TotalTopics"]);
                moderatedForum.ForumGroupId = (int) dr["ForumGroupId"];
                moderatedForum.TotalPostsAwaitingModeration = (int) dr["TotalPostsAwaitingModeration"];

                // Handle Nulls in the case that we don't have a 'most recent post...'
                if (Convert.IsDBNull(dr["MostRecentPostAuthor"]))
                    moderatedForum.MostRecentPostAuthor = null;
                else
                    moderatedForum.MostRecentPostAuthor = Convert.ToString(dr["MostRecentPostAuthor"]);

                if (Convert.IsDBNull(dr["MostRecentPostId"])) {
                    moderatedForum.MostRecentPostId = 0;
                    moderatedForum.MostRecentThreadId = 0;
                } else {
                    moderatedForum.MostRecentPostId = Convert.ToInt32(dr["MostRecentPostId"]);
                    moderatedForum.MostRecentThreadId = Convert.ToInt32(dr["MostRecentThreadId"]);
                }

                if (Convert.IsDBNull(dr["MostRecentPostDate"]))
                    moderatedForum.MostRecentPostDate = DateTime.MinValue.AddMonths(1);
                else
                    moderatedForum.MostRecentPostDate = Convert.ToDateTime(dr["MostRecentPostDate"]);

                // Last time the user was active in the forum
                if (username != null) {
                    if (Convert.IsDBNull(dr["LastUserActivity"]))
                        moderatedForum.LastUserActivity = DateTime.MinValue.AddMonths(1);
                    else
                        moderatedForum.LastUserActivity = Convert.ToDateTime(dr["LastUserActivity"]);
                } else {
                    moderatedForum.LastUserActivity = DateTime.MinValue;
                }
            
                forums.Add(moderatedForum);

            }
            dr.Close();
            myConnection.Close();

            return forums;   
        }

        /****************************************************************
        // GetForumGroupsForModeration
        //
        /// <summary>
        /// Returns a forum group collection of all the forum groups containing
        /// forums that the current user has moderation abilities on.
        /// </summary>
        //
        ****************************************************************/
        public ForumGroupCollection GetForumGroupsForModeration(string username) {
            // Connect to the database
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetAllForumGroupsForModeration", myConnection);

            myCommand.CommandType = CommandType.StoredProcedure;

            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;
    
            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            // populate a ForumGroupCollection object
            ForumGroupCollection forumGroups = new ForumGroupCollection();
            ForumGroup forumGroup;

            while (dr.Read()) {
                forumGroup = PopulateForumGroupFromSqlDataReader(dr);
                forumGroups.Add(forumGroup);
            }
            dr.Close();
            myConnection.Close();

            return forumGroups;
        }

        /****************************************************************
        // ToggleOptions
        //
        /// <summary>
        /// Allows use to change various settings without updating the profile directly
        /// </summary>
        //
        ****************************************************************/
        public void ToggleOptions(string username, bool hideReadThreads, ViewOptions viewOptions) {
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_ToggleOptions", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Pass sproc parameters
            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;
            myCommand.Parameters.Add("@HideReadThreads", SqlDbType.Bit).Value = hideReadThreads;

            if (ViewOptions.NotSet == viewOptions)
                myCommand.Parameters.Add("@FlatView", SqlDbType.Bit).Value = System.DBNull.Value;
            else if (ViewOptions.Threaded == viewOptions)
                myCommand.Parameters.Add("@FlatView", SqlDbType.Bit).Value = false;
            else
                myCommand.Parameters.Add("@FlatView", SqlDbType.Bit).Value = true;

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();

            myConnection.Close();
        }

        /****************************************************************
        // ChangeForumGroupSortOrder
        //
        /// <summary>
        /// Used to move forums group sort order up or down
        /// </summary>
        //
        ****************************************************************/
        public void ChangeForumGroupSortOrder(int forumGroupID, bool moveUp) {
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_ChangeForumGroupSortOrder", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Pass sproc parameters
            myCommand.Parameters.Add("@ForumGroupID", SqlDbType.Int).Value = forumGroupID;
            myCommand.Parameters.Add("@MoveUp", SqlDbType.Bit).Value = moveUp;

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();

            myConnection.Close();
        }

        /****************************************************************
        // UpdateMessageTemplate
        //
        /// <summary>
        /// update the message in the database
        /// </summary>
        //
        ****************************************************************/
        public void UpdateMessageTemplate(ForumMessage message) {
            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_UpdateMessageTemplateList", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Pass sproc parameters
            myCommand.Parameters.Add("@MessageID", SqlDbType.Int).Value = message.MessageID;
            myCommand.Parameters.Add("@Title", SqlDbType.NVarChar, 128).Value = message.Title;
            myCommand.Parameters.Add("@Body", SqlDbType.NVarChar, 4000).Value = message.Body;

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();

            myConnection.Close();

        }


        /****************************************************************
        // GetMessageTemplateList
        //
        /// <summary>
        /// Returns a collection of ForumMessages
        /// </summary>
        //
        ****************************************************************/
        public ForumMessageTemplateCollection GetMessageTemplateList() {
            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetForumMessageTemplateList", myConnection);
            SqlDataReader reader;
            ForumMessageTemplateCollection messages = new ForumMessageTemplateCollection();
            ForumMessage message;


            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Execute the command
            myConnection.Open();
            reader = myCommand.ExecuteReader();

            while (reader.Read()) {
                message = new ForumMessage();

                message.MessageID = Convert.ToInt32(reader["MessageID"]);
                message.Title = (string) reader["Title"];
                message.Body = (string) reader["Body"];

                messages.Add(message);

            }

            myConnection.Close();

            return messages;
        }

        /****************************************************************
        // GetUsernameByEmail
        //
        /// <summary>
        /// Returns the username given an email address
        /// </summary>
        //
        ****************************************************************/
        public string GetUsernameByEmail(string emailAddress) {
            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetUsernameByEmail", myConnection);
            SqlDataReader reader;
            string username = null;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Pass sproc parameters
            myCommand.Parameters.Add("@Email", SqlDbType.NVarChar, 50).Value = emailAddress;

            // Execute the command
            myConnection.Open();
            reader = myCommand.ExecuteReader();

            while (reader.Read()) {
                username = (String) reader["username"];
            }

            myConnection.Close();

            return username;
        }


        /****************************************************************
        // ChangePasswordForLoggedOnUser
        //
        /// <summary>
        /// Change the password for the logged on user.
        /// </summary>
        //
        ****************************************************************/
        public void ChangePasswordForLoggedOnUser(string username, string newPassword) {
            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_ChangeUserPassword", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Pass sproc parameters
            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;
            myCommand.Parameters.Add("@NewPassword", SqlDbType.NVarChar, 50).Value = newPassword;

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();

        }


        /****************************************************************
        // GetMessage
        //
        /// <summary>
        /// Returns a message to display to the user.
        /// </summary>
        //
        ****************************************************************/
        public ForumMessage GetMessage(int messageId) {
            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetMessage", myConnection);
            ForumMessage message;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Pass sproc parameters
            myCommand.Parameters.Add("@MessageId", SqlDbType.Int).Value = messageId;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            message = new ForumMessage();

            while (dr.Read()) {

                message.Title = Convert.ToString(dr["Title"]);
                message.Body = Convert.ToString(dr["Body"]);

            }
            dr.Close();
            myConnection.Close();

            return message;

        }

        
        /****************************************************************
        // GetModeratedPostsByForumId
        //
        /// <summary>
        /// Returns all the posts in a given forum that require moderation.
        /// </summary>
        //
        ****************************************************************/
        private  PostCollection GetModeratedPostsByForumId(int forumId) {
            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetModeratedPostsByForumId", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Pass sproc parameters
            myCommand.Parameters.Add("@Username", SqlDbType.Int).Value = forumId;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            PostCollection posts = new PostCollection();
            Post post = null;

            while (dr.Read()) {
                post = PopulatePostFromSqlDataReader(dr);
                post.ForumName = Convert.ToString(dr["ForumName"]);

                posts.Add(post);
            }
            dr.Close();
            myConnection.Close();

            return posts;
        }

        /****************************************************************
        // GetForumsRequiringModeration
        //
        /// <summary>
        /// Returns a Moderated Foru
        /// </summary>
        //
        ****************************************************************/
        public ModeratedForumCollection GetForumsRequiringModeration(string username) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetModeratedForums", myConnection);
            SqlDataReader reader;
            ModeratedForumCollection moderatedForums = new ModeratedForumCollection();
            ModeratedForum moderatedForum;
            PostCollection posts;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Pass sproc parameters
            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();
            reader = myCommand.ExecuteReader();

            // Loop through the returned results
            while (reader.Read()) {

                // Populate all the forum details
                moderatedForum = new ModeratedForum();
                moderatedForum = (ModeratedForum) PopulateForumFromSqlDataReader(reader);

                // Get all the posts in the forum awaiting moderation
                posts = new PostCollection();
                posts = GetModeratedPostsByForumId(moderatedForum.ForumID);
                moderatedForum.PostsAwaitingModeration = posts;
            }

            myConnection.Close();

            return moderatedForums;

        }


        /****************************************************************
        // MarkPostAsRead
        //
        /// <summary>
        /// Flags a post a 'read' in the database
        /// </summary>
        //
        ****************************************************************/
        public void MarkPostAsRead(int postID, string username) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_MarkPostAsRead", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Pass sproc parameters
            myCommand.Parameters.Add("@PostID", SqlDbType.Int).Value = postID;
            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
 
        }

        /****************************************************************
        // GetTotalPostsForThread
        //
        /// <summary>
        /// Returns the total number of posts in a given thread. This call
        /// is used mainly by paging when viewing posts.
        /// </summary>
        //
        ****************************************************************/
        public int GetTotalPostsForThread(int postID) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetTotalPostsForThread", myConnection);
            int postCount = 0;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Pass sproc parameters
            myCommand.Parameters.Add("@PostID", SqlDbType.Int).Value = postID;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            // Populate the colleciton of users
            while (dr.Read())
                postCount = Convert.ToInt32(dr["TotalPostsForThread"]);

            dr.Close();
            myConnection.Close();
 
            return postCount;
        }


        /****************************************************************
        // GetAllUsers
        //
        /// <summary>
        /// Returns a collection of all users.
        /// </summary>
        //
        ****************************************************************/
        public UserCollection GetAllUsers(int pageIndex, int pageSize, Users.SortUsersBy sortBy, int sortOrder, string usernameBeginsWith) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetAllUsers", myConnection);
            UserCollection users = new UserCollection();

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Pass sproc parameters
            myCommand.Parameters.Add("@PageIndex", SqlDbType.Int).Value = pageIndex;
            myCommand.Parameters.Add("@PageSize", SqlDbType.Int).Value = pageSize;
            myCommand.Parameters.Add("@SortBy", SqlDbType.Int).Value = sortBy;
            myCommand.Parameters.Add("@SortOrder", SqlDbType.Bit).Value = Convert.ToBoolean(sortOrder);

            if ((usernameBeginsWith == "All") || (usernameBeginsWith == null))
                myCommand.Parameters.Add("@UsernameBeginsWith", SqlDbType.NVarChar, 1).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@UsernameBeginsWith", SqlDbType.NVarChar, 1).Value = usernameBeginsWith;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            // Populate the colleciton of users
            while (dr.Read())
                users.Add(PopulateUserFromSqlDataReader(dr));

            dr.Close();
            myConnection.Close();

            return users;
        }

        /****************************************************************
        // GetTotalThreadsInForum
        //
        /// <summary>
        /// Returns the total number of threads in a given forum
        /// </summary>
        /// <param name="username">forum id to look up</param>
        //
        ****************************************************************/
        public int GetTotalThreadsInForum(int ForumID, DateTime maxDateTime, DateTime minDateTime, string username, bool unreadThreadsOnly) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_TopicCountForForum", myConnection);
            int totalThreads = 0;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@ForumID", SqlDbType.Int).Value = ForumID;

            // Control the returned values by DateTime
            myCommand.Parameters.Add("@MaxDate", SqlDbType.DateTime).Value = maxDateTime;
            myCommand.Parameters.Add("@MinDate", SqlDbType.DateTime).Value = minDateTime;

            // Do we have a username?
            if (username == null)
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Return unread threads
            myCommand.Parameters.Add("@UnReadTopicsOnly", SqlDbType.Bit).Value = unreadThreadsOnly;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            while (dr.Read())
                totalThreads = Convert.ToInt32(dr["TotalTopics"]);

            dr.Close();
            myConnection.Close();

            return totalThreads;
        }

        /****************************************************************
        // AddUserToRole
        //
        /// <summary>
        /// Adds a user to a role to elevate their permissions
        /// </summary>
        /// <param name="username">The username of the user to add to the role</param>
        /// <param name="role">The role the user will be added to</param>
        //
        ****************************************************************/
        public void AddUserToRole(string username, string role)
        {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_AddUserToRole", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@UserName", SqlDbType.NVarChar, 50).Value = username;
            myCommand.Parameters.Add("@Rolename", SqlDbType.NVarChar, 256).Value = role;

            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }

        /****************************************************************
        // AddForumToRole
        //
        /// <summary>
        /// Adds a forum to a given role
        /// </summary>
        /// <param name="forumID">The id for the forum to be added to the role</param>
        /// <param name="role">The role the user will be added to</param>
        //
        ****************************************************************/
        public void AddForumToRole(int forumID, string role)
        {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_AddForumToRole", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@ForumID", SqlDbType.Int, 4).Value = forumID;
            myCommand.Parameters.Add("@Rolename", SqlDbType.NVarChar, 256).Value = role;

            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }

        /****************************************************************
        // RemoveUserFromRole
        //
        /// <summary>
        /// Removes a user from a permissions role.
        /// </summary>
        /// <param name="username">The username of the user to remove from the role</param>
        /// <param name="role">The role the user will be removed from</param>
        //
        ****************************************************************/
        public void RemoveUserFromRole(string username, string role)
        {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_RemoveUserFromRole", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@UserName", SqlDbType.NVarChar, 50).Value = username;
            myCommand.Parameters.Add("@Rolename", SqlDbType.NVarChar, 256).Value = role;

            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }

        /****************************************************************
        // RemoveForumFromRole
        //
        /// <summary>
        /// Removes a forum from a given permissions role.
        /// </summary>
        /// <param name="forumID">The forum ID for the forum to remove from the role.</param>
        /// <param name="role">The role the user will be removed from</param>
        //
        ****************************************************************/
        public void RemoveForumFromRole(int forumID, string role)
        {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_RemoveForumFromRole", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@ForumID", SqlDbType.Int, 4).Value = forumID;
            myCommand.Parameters.Add("@Rolename", SqlDbType.NVarChar, 256).Value = role;

            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }

        /****************************************************************
        // CreateNewRole
        //
        /// <summary>
        /// Creates a new permissions role
        /// </summary>
        /// <param name="role">The name for the new permissions role</param>
        /// <param name="description">The description of the new role useful for administration</param>
        //
        ****************************************************************/
        public void CreateNewRole(string role, string description)
        {
            // Create Instance of Connection and Command Object
            using(SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString))
            {
                SqlCommand myCommand = new SqlCommand("dbo.forums_CreateNewRole", myConnection);

                // Mark the Command as a SPROC
                myCommand.CommandType = CommandType.StoredProcedure;

                // Add Parameters to SPROC
                myCommand.Parameters.Add("@Rolename", SqlDbType.VarChar, 256).Value = role;
                myCommand.Parameters.Add("@Description", SqlDbType.VarChar, 512).Value = description;

                myConnection.Open();
                myCommand.ExecuteNonQuery();
                myConnection.Close();
            }
        }

        /****************************************************************
        // DeleteRole
        //
        /// <summary>
        /// Deletes an existing role
        /// </summary>
        /// <param name="role">The name for the role to be deleted</param>
        //
        ****************************************************************/
        public void DeleteRole(string role)
        {
            // Create Instance of Connection and Command Object
            using(SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString))
            {
                SqlCommand myCommand = new SqlCommand("dbo.forums_DeleteRole", myConnection);

                // Mark the Command as a SPROC
                myCommand.CommandType = CommandType.StoredProcedure;

                // Add Parameters to SPROC
                myCommand.Parameters.Add("@Rolename", SqlDbType.VarChar, 256).Value = role;

                myConnection.Open();
                myCommand.ExecuteNonQuery();
                myConnection.Close();
            }
        }

        /****************************************************************
        // GetRoleDescription
        //
        /// <summary>
        /// Gets the description string for a role
        /// </summary>
        /// <param name="role">The name for the role a description is needed for</param>
        //
        ****************************************************************/
        public string GetRoleDescription(string role)
        {
            string roleDescription;

            // Create Instance of Connection and Command Object
            using(SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString))
            {
                SqlCommand myCommand = new SqlCommand("dbo.forums_GetRoleDescription", myConnection);

                // Mark the Command as a SPROC
                myCommand.CommandType = CommandType.StoredProcedure;

                // Add Parameters to SPROC
                myCommand.Parameters.Add("@Rolename", SqlDbType.VarChar, 256).Value = role;

                myConnection.Open();
                roleDescription = (string) myCommand.ExecuteScalar();
                myConnection.Close();
            }

            return roleDescription;
        }

        /****************************************************************
        // UpdateRoleDescription
        //
        /// <summary>
        /// Updates the description of a given role for administration purposes
        /// </summary>
        /// <param name="role">The name of the permissions role to be updated</param>
        /// <param name="description">The new description of the role useful for administration</param>
        //
        ****************************************************************/
        public void UpdateRoleDescription(string role, string description)
        {
            // Create Instance of Connection and Command Object
            using(SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString))
            {
                SqlCommand myCommand = new SqlCommand("dbo.forums_UpdateRoleDescription", myConnection);

                // Mark the Command as a SPROC
                myCommand.CommandType = CommandType.StoredProcedure;

                // Add Parameters to SPROC
                myCommand.Parameters.Add("@RoleName", SqlDbType.VarChar, 256).Value = role;
                myCommand.Parameters.Add("@Description", SqlDbType.VarChar, 512).Value = description;

                myConnection.Open();
                myCommand.ExecuteNonQuery();
                myConnection.Close();
            }
        }

        /****************************************************************
        // GetUserRoles
        //
        /// <summary>
        /// Returns a string array of role names
        /// </summary>
        //
        ****************************************************************/
        public String[] GetAllRoles() 
        {
            string[] roles;

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetAllRoles", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Open the database connection and execute the command
            SqlDataReader dr;

            myConnection.Open();
            dr = myCommand.ExecuteReader();

            // create a String array from the data
            ArrayList userRoles = new ArrayList();

            while (dr.Read()) 
            {
                userRoles.Add(dr["RoleName"]);
            }

            dr.Close();

            // Return the String array of roles
            roles = (String[]) userRoles.ToArray(typeof(String));

            // Close the connection
            myConnection.Close();

            return roles;
        }
            
        /****************************************************************
        // GetUserRoles
        //
        /// <summary>
        /// Returns a string array of role names that the user belongs to
        /// </summary>
        /// <param name="username">username to find roles for</param>
        //
        ****************************************************************/
        public String[] GetUserRoles(string username) 
        {
            string[] roles;

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetRolesByUser", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@username", SqlDbType.NVarChar, 50).Value = username;

            // Open the database connection and execute the command
            SqlDataReader dr;

            myConnection.Open();
            dr = myCommand.ExecuteReader();

            // create a String array from the data
            ArrayList userRoles = new ArrayList();

            while (dr.Read()) {
                userRoles.Add(dr["RoleName"]);
            }

            dr.Close();

            // Return the String array of roles
            roles = (string[]) userRoles.ToArray(typeof(String));

            myConnection.Close();

            return roles;
        }

        /****************************************************************
        // GetUserRoles
        //
        /// <summary>
        /// Returns a string array of role names that the user belongs to
        /// </summary>
        /// <param name="username">username to find roles for</param>
        //
        ****************************************************************/
        public String[] GetForumRoles(int forumID) 
        {
            string[] roles;

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetRolesByForum", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@ForumID", SqlDbType.Int, 4).Value = forumID;

            // Open the database connection and execute the command
            SqlDataReader dr;

            myConnection.Open();
            dr = myCommand.ExecuteReader();

            // create a String array from the data
            ArrayList forumRoles = new ArrayList();

            while (dr.Read()) 
            {
                forumRoles.Add(dr["RoleName"]);
            }

            dr.Close();

            // Return the String array of roles
            roles = (string[]) forumRoles.ToArray(typeof(String));

            myConnection.Close();

            return roles;
        }

        /****************************************************************
        // TrackAnonymousUsers
        //
        /// <summary>
        /// Keep track of anonymous users.
        /// </summary>
        /// <param name="userId">user id to uniquely identify the user</param>
        //
        ****************************************************************/
        public void TrackAnonymousUsers(string userId) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_TrackAnonymousUsers", myConnection);
            SqlParameter param;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            param = new SqlParameter("@UserId", SqlDbType.Char, 36);
            param.Value = userId;
            myCommand.Parameters.Add(param);

            // Open the connection
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }

        /****************************************************************
        // GetForumGroupByForumId
        //
        /// <summary>
        /// Returns the name of a forum group based on the id of the forum.
        /// </summary>
        /// <param name="forumGroupName">ID of the forum group to lookup</param>
        //
        ****************************************************************/
        public ForumGroup GetForumGroupByForumId(int forumID) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetForumGroupByForumID", myConnection);
            SqlDataReader dr;
            ForumGroup forumGroup = null;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@ForumID", SqlDbType.Int).Value = forumID;

            // Open the connection
            myConnection.Open();

            dr = myCommand.ExecuteReader();

            if (dr.Read())
                forumGroup = PopulateForumGroupFromSqlDataReader(dr);

            myConnection.Close();

            return forumGroup;
        }

        /****************************************************************
        // AddForumGroup
        //
        /// <summary>
        /// Creates a new forum group, and exception is raised if the
        /// forum group already exists.
        /// </summary>
        /// <param name="forumGroupName">Name of the forum group to create</param>
        //
        ****************************************************************/
        public void AddForumGroup(string forumGroupName) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_AddForumGroup", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter param = new SqlParameter("@ForumGroupName", SqlDbType.NVarChar, 256);
            param.Value = forumGroupName;
            myCommand.Parameters.Add(param);

            // Open the connection
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();

        }

        /****************************************************************
        // MarkAllThreadsRead
        //
        /// <summary>
        /// Marks all threads from Forum ID and below as read
        /// </summary>
        //
        *****************************************************************/
        public void MarkAllThreadsRead(int forumID, string username) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_MarkAllThreadsRead", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter param = new SqlParameter("@ForumID", SqlDbType.Int);
            param.Value = forumID;
            myCommand.Parameters.Add(param);

            param = new SqlParameter("@Username", SqlDbType.NVarChar, 50);
            param.Value = username;
            myCommand.Parameters.Add(param);


            // Open the connection
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }


        /****************************************************************
        // UpdateForumGroup
        //
        /// <summary>
        /// Updates the name of an existing forum group
        /// </summary>
        /// <param name="forumGroupName">New name for the forum group</param>
        /// <param name="forumGroupId">Unique identifier for the forum group to update</param>
        //
        *****************************************************************/
        public void UpdateForumGroup(string forumGroupName, int forumGroupId) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_UpdateForumGroup", myConnection);
            SqlParameter param;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            param = new SqlParameter("@ForumGroupName", SqlDbType.NVarChar, 256);
            
            // If forumGroupName is null we want to delete
            if (null == forumGroupName)
                param.Value = System.DBNull.Value;
            else
                param.Value = forumGroupName;
            
            myCommand.Parameters.Add(param);

            // Add Parameters to SPROC
            param = new SqlParameter("@ForumGroupId", SqlDbType.Int);
            param.Value = forumGroupId;
            myCommand.Parameters.Add(param);

            // Open the connection
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }


        /// <summary>
        /// Builds and returns an instance of the Post class based on the current row of an
        /// aptly populated SqlDataReader object.
        /// </summary>
        /// <param name="dr">The SqlDataReader object that contains, at minimum, the following
        /// columns: PostID, ParentID, Body, ForumID, PostDate, PostLevel, SortOrder, Subject,
        /// ThreadDate, ThreadID, Replies, Username, and Approved.</param>
        /// <returns>An instance of the Post class that represents the current row of the passed 
        /// in SqlDataReader, dr.</returns>
        private Post PopulatePostFromSqlDataReader(SqlDataReader dr) {

            Post post = new Post();
            post.PostID = Convert.ToInt32(dr["PostID"]);
            post.ParentID = Convert.ToInt32(dr["ParentID"]);
            post.Body = Convert.ToString(dr["Body"]);
            post.ForumName = Convert.ToString(dr["ForumName"]);
            post.ForumID = Convert.ToInt32(dr["ForumID"]);
            post.PostDate = Convert.ToDateTime(dr["PostDate"]);
            post.PostLevel = Convert.ToInt32(dr["PostLevel"]);
            post.SortOrder = Convert.ToInt32(dr["SortOrder"]);
            post.Subject = Convert.ToString(dr["Subject"]);
            post.ThreadDate = Convert.ToDateTime(dr["ThreadDate"]);
            post.ThreadID = Convert.ToInt32(dr["ThreadID"]);
            post.Replies = Convert.ToInt32(dr["Replies"]);
            post.Username = Convert.ToString(dr["Username"]);
            post.Approved = Convert.ToBoolean(dr["Approved"]);
            post.IsLocked = Convert.ToBoolean(dr["IsLocked"]);
            post.Views = Convert.ToInt32(dr["TotalViews"]);
            post.HasRead = Convert.ToBoolean(dr["HasRead"]);
            
            return post;
        }

        
        // *********************************************************************
        //
        //  PopulateThreadFromSqlDataReader
        //
        /// <summary>
        /// This private method accepts a datareader and attempts to create and
        /// populate a thread class instance which is returned to the caller. For
        /// all practical purposes, a thread is simply a lightweigh version of a 
        /// post - no details, such as the body, are provided though and a thread is
        /// always considered the first post in a thread.
        /// </summary>
        //
        // ********************************************************************/
        private Thread PopulateThreadFromSqlDataReader(SqlDataReader reader) {
            Thread thread = new Thread();

            thread.PostID = Convert.ToInt32(reader["PostID"]);
            thread.PostDate = Convert.ToDateTime(reader["PostDate"]);
            thread.Subject = Convert.ToString(reader["Subject"]);
            thread.Body = Convert.ToString(reader["Body"]);
            thread.ThreadDate = Convert.ToDateTime(reader["ThreadDate"]);
            thread.PinnedDate = Convert.ToDateTime(reader["PinnedDate"]);
            thread.Replies = Convert.ToInt32(reader["Replies"]);
            thread.Username = Convert.ToString(reader["Username"]);
            thread.IsLocked = Convert.ToBoolean(reader["IsLocked"]);
            thread.IsPinned = Convert.ToBoolean(reader["IsPinned"]);
            thread.Views = Convert.ToInt32(reader["TotalViews"]);
            thread.HasRead = Convert.ToBoolean(reader["HasRead"]);
            thread.MostRecentPostAuthor = Convert.ToString(reader["MostRecentPostAuthor"]);
            thread.MostRecentPostID = Convert.ToInt32(reader["MostRecentPostID"]);
            thread.ThreadID = Convert.ToInt32(reader["ThreadID"]);

            return thread;
        }


        /// <summary>
        /// Builds and returns an instance of the Forum class based on the current row of an
        /// aptly populated SqlDataReader object.
        /// </summary>
        /// <param name="dr">The SqlDataReader object that contains, at minimum, the following
        /// columns: ForumID, DateCreated, Description, Name, Moderated, and DaysToView.</param>
        /// <returns>An instance of the Forum class that represents the current row of the passed 
        /// in SqlDataReader, dr.</returns>
        private  Forum PopulateForumFromSqlDataReader(SqlDataReader dr) {
            Forum forum = new Forum();
            forum.ForumID = Convert.ToInt32(dr["ForumID"]);
            forum.ParentId = Convert.ToInt32(dr["ParentID"]);
            forum.ForumGroupId = Convert.ToInt32(dr["ForumGroupId"]);
            forum.DateCreated = Convert.ToDateTime(dr["DateCreated"]);
            forum.Description = Convert.ToString(dr["Description"]);
            forum.Name = Convert.ToString(dr["Name"]);
            forum.Moderated = Convert.ToBoolean(dr["Moderated"]);
            forum.DaysToView = Convert.ToInt32(dr["DaysToView"]);
            forum.Active = Convert.ToBoolean(dr["Active"]);
            forum.SortOrder = Convert.ToInt32(dr["SortOrder"]);
            forum.IsPrivate = Convert.ToBoolean(dr["IsPrivate"]);
            forum.DisplayMask = (byte[]) dr["DisplayMask"];

            return forum;
        }

        private  ModeratedForum PopulateModeratedForumFromSqlDataReader(SqlDataReader dr) {
            ModeratedForum forum = new ModeratedForum();
            forum.ForumID = Convert.ToInt32(dr["ForumID"]);
            forum.ForumGroupId = Convert.ToInt32(dr["ForumGroupId"]);
            forum.DateCreated = Convert.ToDateTime(dr["DateCreated"]);
            forum.Description = Convert.ToString(dr["Description"]);
            forum.Name = Convert.ToString(dr["Name"]);
            forum.Moderated = Convert.ToBoolean(dr["Moderated"]);
            forum.DaysToView = Convert.ToInt32(dr["DaysToView"]);
            forum.Active = Convert.ToBoolean(dr["Active"]);
            forum.SortOrder = Convert.ToInt32(dr["SortOrder"]);
            forum.IsPrivate = Convert.ToBoolean(dr["IsPrivate"]);

            return forum;
        }
        
        private ForumGroup PopulateForumGroupFromSqlDataReader(SqlDataReader dr) {

            ForumGroup forumGroup = new ForumGroup();
            forumGroup.ForumGroupID = (int) dr["ForumGroupId"];
            forumGroup.Name = (string) dr["Name"];
            forumGroup.SortOrder = Convert.ToInt32(dr["SortOrder"]);

            return forumGroup;
        }
    

        /// <summary>
        /// Builds and returns an instance of the User class based on the current row of an
        /// aptly populated SqlDataReader object.
        /// </summary>
        /// <param name="dr">The SqlDataReader object that contains, at minimum, the following
        /// columns: Signature, Email, FakeEmail, Url, Password, Username, Administrator, Approved,
        /// Trusted, Timezone, DateCreated, LastLogin, and ForumView.</param>
        /// <returns>An instance of the User class that represents the current row of the passed 
        /// in SqlDataReader, dr.</returns>
        private User PopulateUserFromSqlDataReader(SqlDataReader dr) {
            User user = new User();
            user.Signature = Convert.ToString(dr["Signature"]);
            user.Email = Convert.ToString(dr["Email"]);
            user.PublicEmail = Convert.ToString(dr["FakeEmail"]);
            user.Url = Convert.ToString(dr["URL"]);
            user.Password = Convert.ToString(dr["Password"]);
            user.Username = Convert.ToString(dr["Username"]);
            user.IsApproved = Convert.ToBoolean(dr["Approved"]);
            user.IsProfileApproved = Convert.ToBoolean(dr["ProfileApproved"]);
            user.IsTrusted = Convert.ToBoolean(dr["Trusted"]);
            user.Timezone = Convert.ToInt32(dr["Timezone"]);
            user.DateCreated = Convert.ToDateTime(dr["DateCreated"]);
            user.LastLogin = Convert.ToDateTime(dr["LastLogin"]);
            user.LastActivity = Convert.ToDateTime(dr["LastActivity"]);
            user.TrackPosts = Convert.ToBoolean(dr["TrackYourPosts"]);
            user.Location = (string) dr["Location"];
            user.Occupation = (string) dr["Occupation"];
            user.Interests = (string) dr["Interests"];
            user.MsnIM = (string) dr["MSN"];
            user.AolIM = (string) dr["AIM"];
            user.YahooIM = (string) dr["Yahoo"];
            user.IcqIM = (string) dr["ICQ"];
            user.TotalPosts = (int) dr["TotalPosts"];
            user.HasAvatar = (bool) dr["HasAvatar"];
            user.HideReadThreads = (bool) dr["ShowUnreadTopicsOnly"];
            user.Skin = (string) dr["Style"];
            user.Avatar = (AspNetForums.Components.AvatarType) (int) dr["AvatarType"];
            user.AvatarUrl = (string) dr["AvatarUrl"];
            user.ShowAvatar = (bool) dr["ShowAvatar"];
            user.DateFormat = (string) dr["DateFormat"];
            user.ShowPostsAscending = (bool) dr["PostViewOrder"];
            user.ViewPostsInFlatView = (bool) dr["FlatView"];
            user.IsModerator = Convert.ToBoolean(dr["IsModerator"]);
            user.Attributes = (byte[]) dr["Attributes"];
        
            switch (Convert.ToInt32(dr["ForumView"])) {
                case 0:
                    user.ForumView = ViewOptions.Flat;
                    break;

                case 1:
                    user.ForumView = ViewOptions.Mixed;
                    break;

                case 2:
                    user.ForumView = ViewOptions.Threaded;
                    break;

                default:
                    user.ForumView = ViewOptions.NotSet;
                    break;
            }
            
            return user;
        }


        
        /// <summary>
        /// Builds and returns an instance of the EmailTemplate class based on the current row of an
        /// aptly populated SqlDataReader object.
        /// </summary>
        /// <param name="dr">The SqlDataReader object that contains, at minimum, the following
        /// columns: EmailID, Subject, Message, FromAddress, Importance, and Description.</param>
        /// <returns>An instance of the EmailTemplate class that represents the current row of the passed 
        /// in SqlDataReader, dr.</returns>
        private  EmailTemplate PopulateEmailTemplateFromSqlDataReader(SqlDataReader dr) {
            EmailTemplate email = new EmailTemplate();
            
            email.EmailTemplateID = Convert.ToInt32(dr["EmailID"]);
            email.Subject = Convert.ToString(dr["Subject"]);
            email.Body = Convert.ToString(dr["Message"]);
            email.From = Convert.ToString(dr["FromAddress"]);
            email.Description = Convert.ToString(dr["Description"]);

            switch (Convert.ToInt32(dr["Importance"])) {
                case 0:
                    email.Priority = MailPriority.Low;
                    break;

                case 2:
                    email.Priority = MailPriority.High;
                    break;

                default:        // the default
                    email.Priority = MailPriority.Normal;
                    break;
            }

            return email;
        }
        /*********************************************************************************/


        /*********************************************************************************/

        /************************ POST FUNCTIONS ***********************
                 * These functions return information about a post or posts.  They
                 * are called from the WebForums.Posts class.
                 * *************************************************************/
        public ThreadCollection GetAllThreads(int forumID, string username, bool unreadThreadsOnly) {

            // TODO - might want to do some more work here
            return GetAllThreads(forumID, 9999, 0, DateTime.Now.AddYears(-20), username, unreadThreadsOnly);

        }


        public ThreadCollection GetAllThreads(int forumID, int pageSize, int pageIndex, DateTime endDate, string username, bool unreadThreadsOnly) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetAllTopicsPaged", myConnection);
            ThreadCollection threads;

            // Ensure DateTime is min value for SQL
            if (endDate == DateTime.MinValue)
                endDate = (DateTime) System.Data.SqlTypes.SqlDateTime.MinValue;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@ForumId", SqlDbType.Int, 4).Value = forumID;
            myCommand.Parameters.Add("@PageSize", SqlDbType.Int, 4).Value = pageSize;
            myCommand.Parameters.Add("@PageIndex", SqlDbType.Int, 4).Value = pageIndex;
            myCommand.Parameters.Add("@DateFilter", SqlDbType.DateTime).Value = endDate;
            
            // Only pass username if it's not null
            if (username == null)
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            myCommand.Parameters.Add("@UnReadTopicsOnly", SqlDbType.Bit).Value = unreadThreadsOnly;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            threads = new ThreadCollection();

            while (dr.Read()) {
                threads.Add(PopulateThreadFromSqlDataReader(dr));
            }
            dr.Close();
            myConnection.Close();

            // Only return the posts specified through paging

            return threads;

        }

        /// <summary>
        /// Returns all of the messages for a particular page of posts for a paticular forum in a
        /// particular ForumView mode.
        /// </summary>
        /// <param name="ForumID">The ID of the Forum whose posts you wish to display.</param>
        /// <param name="ForumView">How to display the Forum posts.  The ViewOptions enumeration
        /// supports one of three values: Flat, Mixed, and Threaded.</param>
        /// <param name="PagesBack">How many pages back of data to display.  A value of 0 displays
        /// the posts from the current time to a time that is the Forum's DaysToView days prior to the
        /// current day.</param>
        /// <returns>A PostCollection object containing all of the posts.</returns>
        public  PostCollection GetAllMessages(int ForumID, ViewOptions ForumView, int PagesBack) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetAllMessages", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterForumId = new SqlParameter("@ForumId", SqlDbType.Int, 4);
            parameterForumId.Value = ForumID;
            myCommand.Parameters.Add(parameterForumId);

            SqlParameter parameterViewType = new SqlParameter("@ViewType", SqlDbType.Int, 4);
            parameterViewType.Value = (int) ForumView;
            myCommand.Parameters.Add(parameterViewType);

            SqlParameter parameterPagesBack = new SqlParameter("@PagesBack", SqlDbType.Int, 4);
            parameterPagesBack.Value = PagesBack;
            myCommand.Parameters.Add(parameterPagesBack);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            PostCollection posts = new PostCollection();
            while (dr.Read()) {
                posts.Add(PopulatePostFromSqlDataReader(dr));
            }
            dr.Close();
            myConnection.Close();

            return posts;
        }

        /// <summary>
        /// Returns all of the messages for a particular page of posts for a paticular forum in a
        /// particular ForumView mode.
        /// </summary>
        /// <param name="ForumID">The ID of the Forum whose posts you wish to display.</param>
        /// <param name="ForumView">How to display the Forum posts.  The ViewOptions enumeration
        /// supports one of three values: Flat, Mixed, and Threaded.</param>
        /// <param name="PagesBack">How many pages back of data to display.  A value of 0 displays
        /// the posts from the current time to a time that is the Forum's DaysToView days prior to the
        /// current day.</param>
        /// <returns>A PostCollection object containing all of the posts.</returns>
        public  PostCollection GetSubjectsByForum(int ForumID, ViewOptions ForumView, int PagesBack) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetAllMessages", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterForumId = new SqlParameter("@ForumId", SqlDbType.Int, 4);
            parameterForumId.Value = ForumID;
            myCommand.Parameters.Add(parameterForumId);

            SqlParameter parameterViewType = new SqlParameter("@ViewType", SqlDbType.Int, 4);
            parameterViewType.Value = (int) ForumView;
            myCommand.Parameters.Add(parameterViewType);

            SqlParameter parameterPagesBack = new SqlParameter("@PagesBack", SqlDbType.Int, 4);
            parameterPagesBack.Value = PagesBack;
            myCommand.Parameters.Add(parameterPagesBack);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            PostCollection posts = new PostCollection();
            while (dr.Read()) {
                posts.Add(PopulatePostFromSqlDataReader(dr));
            }
            dr.Close();
            myConnection.Close();

            return posts;
        }

        /// is the user tracking this thread?
        public bool IsUserTrackingThread(int threadID, string username) {

            bool userIsTrackingPost = false; 

            // If username is null, don't continue
            if (username == null)
                return userIsTrackingPost;

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_IsUserTrackingPost", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@ThreadID", SqlDbType.Int).Value = threadID;
            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            if (!dr.Read())
                return userIsTrackingPost;

            userIsTrackingPost = Convert.ToBoolean(dr["IsUserTrackingPost"]);

            dr.Close();
            myConnection.Close();

            return userIsTrackingPost;
        }

        /// <summary>
        /// Gets the details for a particular post.  These details include the IDs of the next/previous
        /// post and the next/prev thread, along with information about the user who posted the post.
        /// </summary>
        /// <param name="PostID">The ID of the Post to get the information from.</param>
        /// <param name="Username">The Username of the person viewing the post.  Used to determine if
        /// the particular user has email tracking turned on for the thread that this message resides.</param>
        /// <returns>A PostDetails instance with rich information about the particular post.</returns>
        /// <remarks>If a PostID is passed in that is NOT found in the database, a PostNotFoundException
        /// exception is thrown.</remarks>
        public  PostDetails GetPostDetails(int postID, String username) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetSingleMessage", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@PostID", SqlDbType.Int).Value = postID;

            if ( (username == null) || (username == String.Empty))
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            if (!dr.Read())
                // we did not get back a post
                throw new Components.PostNotFoundException("Did not get back a post for PostID " + postID.ToString());

            // we have a post to work with
            PostDetails post = new PostDetails();
            post.PostID = postID;
            post.ParentID = Convert.ToInt32(dr["ParentID"]);
            post.Body = Convert.ToString(dr["Body"]);
            post.ForumID = Convert.ToInt32(dr["ForumID"]);
            post.PostDate = Convert.ToDateTime(dr["PostDate"]);
            post.PostLevel = Convert.ToInt32(dr["PostLevel"]);
            post.SortOrder = Convert.ToInt32(dr["SortOrder"]);
            post.Subject = Convert.ToString(dr["Subject"]);
            post.ThreadDate = Convert.ToDateTime(dr["ThreadDate"]);
            post.ThreadID = Convert.ToInt32(dr["ThreadID"]);
            post.Replies = Convert.ToInt32(dr["Replies"]);
            post.Username = Convert.ToString(dr["Username"]);
            post.NextPostID = Convert.ToInt32(dr["NextPostID"]);
            post.PrevPostID = Convert.ToInt32(dr["PrevPostID"]);
            post.NextThreadID = Convert.ToInt32(dr["NextThreadID"]);
            post.PrevThreadID = Convert.ToInt32(dr["PrevThreadID"]);
            post.ThreadTracking = Convert.ToBoolean(dr["UserIsTrackingThread"]);
            post.ForumName = Convert.ToString(dr["ForumName"]);
            post.IsLocked = Convert.ToBoolean(dr["IsLocked"]);

            // populate information about the User
            User user = new User();
            user.Username = post.Username;
            user.PublicEmail = Convert.ToString(dr["FakeEmail"]);
            user.Url = Convert.ToString(dr["URL"]);
            user.Signature = Convert.ToString(dr["Signature"]);
            
            post.UserInfo = user;

            dr.Close();
            myConnection.Close();

            return post;
        }


        /// <summary>
        /// Returns count of all posts in system
        /// </summary>
        /// <returns></returns>
        public int GetTotalPostCount() {
            int totalPostCount;

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetTotalPostCount", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            dr.Read();
            
            totalPostCount = (int) dr[0];

            dr.Close();
            myConnection.Close();

            return totalPostCount;
        }

        /// <summary>
        /// Get basic information about a single post.  This method returns an instance of the Post class,
        /// which contains less information than the PostDeails class, which is what is returned by the
        /// GetPostDetails method.
        /// </summary>
        /// <param name="PostID">The ID of the post whose information we are interested in.</param>
        /// <returns>An instance of the Post class.</returns>
        /// <remarks>If a PostID is passed in that is NOT found in the database, a PostNotFoundException
        /// exception is thrown.</remarks>
        public  Post GetPost(int postID, string username, bool trackViews) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetPostInfo", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@PostID", SqlDbType.Int).Value = postID;
            myCommand.Parameters.Add("@TrackViews", SqlDbType.Bit).Value = trackViews;

            if (username == null)
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            if (!dr.Read()) {
                dr.Close();
                myConnection.Close();
                // we did not get back a post
                throw new Components.PostNotFoundException("Did not get back a post for PostID " + postID.ToString());
            }

            Post p = PopulatePostFromSqlDataReader(dr);
            dr.Close();
            myConnection.Close();


            // we have a post to work with  
            return p;
        }


    
        /// <summary>
        /// Reverses a particular user's email thread tracking options for the thread that contains
        /// the post specified by PostID.  That is, if a User has email thread tracking turned on for
        /// a particular thread, a call to this method will turn off the email thread tracking; conversely,
        /// if a user has thread tracking turned off for a particular thread, a call to this method will
        /// turn it on.
        /// </summary>
        /// <param name="Username">The User whose email thread tracking options we wish to reverse.</param>
        /// <param name="PostID"></param>
        public  void ReverseThreadTracking(String Username, int PostID) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_ReverseTrackingOption", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterUsername = new SqlParameter("@Username", SqlDbType.NVarChar, 50);
            parameterUsername.Value = Username;
            myCommand.Parameters.Add(parameterUsername);

            SqlParameter parameterPostId = new SqlParameter("@PostID", SqlDbType.Int, 4);
            parameterPostId.Value = PostID;
            myCommand.Parameters.Add(parameterPostId);

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }



        /// <summary>
        /// Returns a collection of Posts that make up a particular thread.
        /// </summary>
        /// <param name="ThreadID">The ID of the Thread to retrieve the posts of.</param>
        /// <returns>A PostCollection object that contains the posts in the thread specified by
        /// ThreadID.</returns>
        public  PostCollection GetThread(int ThreadID) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetThread", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterThreadId = new SqlParameter("@ThreadID", SqlDbType.Int, 4);
            parameterThreadId.Value = ThreadID;
            myCommand.Parameters.Add(parameterThreadId);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            // loop through the results
            PostCollection posts = new PostCollection();
            while (dr.Read()) {
                posts.Add(PopulatePostFromSqlDataReader(dr));
            }
            dr.Close();
            myConnection.Close();

            return posts;
        }


        /// <summary>
        /// Returns a collection of Posts that make up a particular thread with paging
        /// </summary>
        /// <param name="PostID">The ID of a Post in the thread that you are interested in retrieving.</param>
        /// <returns>A PostCollection object that contains the posts in the thread.</returns>
        public PostCollection GetThreadByPostID(int postID, int currentPageIndex, int pageSize, int sortBy, int sortOrder, string username) {
            SqlParameter param;

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetThreadByPostIDPaged", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // PostID Parameter
            param = new SqlParameter("@PostID", SqlDbType.Int, 4);
            param.Value = postID;
            myCommand.Parameters.Add(param);

            // CurrentPage Parameter
            param = new SqlParameter("@PageIndex", SqlDbType.Int);
            param.Value = currentPageIndex;
            myCommand.Parameters.Add(param);

            // PageSize Parameter
            param = new SqlParameter("@PageSize", SqlDbType.Int, 4);
            param.Value = pageSize;
            myCommand.Parameters.Add(param);

            // Username
            if ( (username == null) || (username == String.Empty))
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Sort by
            myCommand.Parameters.Add("@SortBy", SqlDbType.Int).Value = sortBy;

            // Sort order
            myCommand.Parameters.Add("@SortOrder", SqlDbType.Int).Value = sortOrder;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            // loop through the results
            PostCollection posts = new PostCollection();
            while (dr.Read()) {
                Post p = PopulatePostFromSqlDataReader(dr);
                p.PostType = (Posts.PostType) dr["PostType"];
                posts.Add(p);
            }

            dr.Close();
            myConnection.Close();

            return posts;
        }

        
        /// <summary>
        /// Returns a collection of Posts that make up a particular thread.
        /// </summary>
        /// <param name="PostID">The ID of a Post in the thread that you are interested in retrieving.</param>
        /// <returns>A PostCollection object that contains the posts in the thread.</returns>
        public  PostCollection GetThreadByPostID(int postID, string username) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetThreadByPostID", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@PostID", SqlDbType.Int).Value = postID;

            if ( (username == null) || (username == String.Empty))
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            // loop through the results
            PostCollection posts = new PostCollection();
            while (dr.Read()) {
                posts.Add(PopulatePostFromSqlDataReader(dr));
            }

            dr.Close();
            myConnection.Close();

            return posts;
        }




        /// <summary>
        /// Adds a new Post.  This method checks the allowDuplicatePosts settings to determine whether
        /// or not to allow for duplicate posts.  If allowDuplicatePosts is set to false and the user
        /// attempts to enter a duplicate post, a PostDuplicateException exception is thrown.
        /// </summary>
        /// <param name="PostToAdd">A Post object containing the information needed to add a new
        /// post.  The essential fields of the Post class that must be set are: the Subject, the
        /// Body, the Username, and a ForumID or a ParentID (depending on whether the post to add is
        /// a new post or a reply to an existing post, respectively).</param>
        /// <returns>A Post object with information on the newly inserted post.  This returned Post
        /// object includes the ID of the newly added Post (PostID) as well as if the Post is
        /// Approved or not.</returns>
        public Post AddPost(Post postToAdd, string username) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlParameter param;
            myConnection.Open();

            SqlParameter parameterUserName = new SqlParameter("@Username", SqlDbType.NVarChar, 50);
            parameterUserName.Value = postToAdd.Username;

            SqlParameter parameterBody = new SqlParameter("@Body", SqlDbType.NText);
            parameterBody.Value = postToAdd.Body;

            if (!Globals.AllowDuplicatePosts) {
                SqlCommand checkForDupsCommand = new SqlCommand("dbo.forums_IsDuplicatePost", myConnection);
                checkForDupsCommand.CommandType = CommandType.StoredProcedure;  // Mark the Command as a SPROC
                checkForDupsCommand.Parameters.Add(parameterUserName);
                checkForDupsCommand.Parameters.Add(parameterBody);

                if (((int) checkForDupsCommand.ExecuteScalar()) > 0) {
                    myConnection.Close();
                    throw new PostDuplicateException("Attempting to insert a duplicate post.");
                }

                checkForDupsCommand.Parameters.Clear();         // clear the parameters
            }

            SqlCommand myCommand = new SqlCommand("dbo.forums_AddPost", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterForumId = new SqlParameter("@ForumID", SqlDbType.Int, 4);
            parameterForumId.Value = postToAdd.ForumID;
            myCommand.Parameters.Add(parameterForumId);

            SqlParameter parameterPostId = new SqlParameter("@ReplyToPostID", SqlDbType.Int, 4);
            parameterPostId.Value = postToAdd.ParentID;
            myCommand.Parameters.Add(parameterPostId);

            SqlParameter parameterSubject = new SqlParameter("@Subject", SqlDbType.NVarChar, 256);
            parameterSubject.Value = postToAdd.Subject;
            myCommand.Parameters.Add(parameterSubject);

            param = new SqlParameter("@IsLocked", SqlDbType.Bit);
            param.Value = postToAdd.IsLocked;
            myCommand.Parameters.Add(param);

            param = new SqlParameter("@Pinned", SqlDbType.DateTime);
            if (postToAdd.PostDate > DateTime.Now)
                param.Value = postToAdd.PostDate;
            else
                param.Value = System.DBNull.Value;
            myCommand.Parameters.Add(param);

            myCommand.Parameters.Add(parameterUserName);
            myCommand.Parameters.Add(parameterBody);

            // Execute the command
            int iNewPostID = 0;
            
            try {
                // Get the new PostID
                iNewPostID = Convert.ToInt32(myCommand.ExecuteScalar().ToString());
            } 
            catch (Exception e) {
                myConnection.Close();
                // if an exception occurred, throw it back
                throw new Exception(e.Message);
            }

            myConnection.Close();
            
            // Return a Post instance with info from the newly inserted post.
            return GetPost(iNewPostID, username, false);
        }

        

        /// <summary>
        /// Updates a post.
        /// </summary>
        /// <param name="UpdatedPost">The Post data used to update the Post.  The ID of the UpdatedPost
        /// Post object corresponds to what post is to be updated.  The only other fields used to update
        /// the Post are the Subject and Body.</param>
        public void UpdatePost(Post post, string editedBy) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_UpdatePost", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@PostID", SqlDbType.Int, 4).Value = post.PostID;
            myCommand.Parameters.Add("@Subject", SqlDbType.NVarChar, 256).Value = post.Subject;
            myCommand.Parameters.Add("@Body", SqlDbType.NText).Value = post.Body;
            myCommand.Parameters.Add("@IsLocked", SqlDbType.Bit).Value = post.IsLocked;
            myCommand.Parameters.Add("@EditedBy", SqlDbType.NVarChar, 50).Value = editedBy;

            // Execute the command
            myConnection.Open();
            try {
                myCommand.ExecuteNonQuery();
            } 
            catch (Exception e) {
                myConnection.Close();
                // oops, something went wrong
                throw new Exception(e.Message);
            }
            myConnection.Close();
        }


        
        /// <summary>
        /// This method deletes a particular post and all of its replies.
        /// </summary>
        /// <param name="postID">The PostID that you wish to delete.</param>
        public void DeletePost(int postID, string approvedBy, string reason) {
            // Delete the post
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_DeleteModeratedPost", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@PostID", SqlDbType.Int, 4).Value = postID;
            myCommand.Parameters.Add("@ApprovedBy", SqlDbType.NVarChar, 50).Value = approvedBy;
            myCommand.Parameters.Add("@Reason", SqlDbType.NVarChar, 1024).Value = reason;

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }




        /*********************************************************************************/

        /************************ FORUM FUNCTIONS ***********************
                 * These functions return information about a forum.
                 * are called from the WebForums.Forums class.
                 * **************************************************************/
    
        /// <summary>
        /// Returns information about a particular forum that contains a particular thread.
        /// </summary>
        /// <param name="ThreadID">The ID of the thread that is contained in the Forum you wish to
        /// retrieve information about.</param>
        /// <returns>A Forum object instance containing the information about the Forum that the
        /// specified thread exists in.</returns>
        /// <remarks>If a ThreadID is passed in that is NOT found in the database, a ForumNotFoundException
        /// exception is thrown.</remarks>
        public  Forum GetForumInfoByThreadID(int ThreadID) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetForumByThreadID", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterThreadId = new SqlParameter("@ThreadID", SqlDbType.Int, 4);
            parameterThreadId.Value = ThreadID;
            myCommand.Parameters.Add(parameterThreadId);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            if (!dr.Read()) {
                dr.Close();
                myConnection.Close();
                // we didn't get a forum, handle it
                throw new Components.ForumNotFoundException("Did not get back a forum for ThreadID " + ThreadID.ToString());
            } 

            Forum f = PopulateForumFromSqlDataReader(dr);
            dr.Close();
            myConnection.Close();

            return f;
        }



        /// <summary>
        /// Returns a Forum object with information on a particular forum.
        /// </summary>
        /// <param name="ForumID">The ID of the Forum you are interested in.</param>
        /// <returns>A Forum object.</returns>
        /// <remarks>If a ForumID is passed in that is NOT found in the database, a ForumNotFoundException
        /// exception is thrown.</remarks>
        public  Forum GetForumInfo(int forumID, string username) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetForumInfo", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@ForumID", SqlDbType.Int).Value = forumID;
            if (( username == null) || (username == String.Empty))
                myCommand.Parameters.Add("@username", SqlDbType.NVarChar, 50).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            if (!dr.Read()) {
                dr.Close();
                myConnection.Close();
                // we didn't get a forum, handle it
                throw new Components.ForumNotFoundException("Did not get back a forum for ForumID " + forumID.ToString());
            }

            Forum f = PopulateForumFromSqlDataReader(dr);
            f.TotalThreads = Convert.ToInt32(dr["TotalTopics"]);

            dr.Close();
            myConnection.Close();

            return f;
        }




        /// <summary>
        /// Returns information about a particular forum that contains a particular post.
        /// </summary>
        /// <param name="PostID">The ID of the post that is contained in the Forum you wish to
        /// retrieve information about.</param>
        /// <returns>A Forum object instance containing the information about the Forum that the
        /// specified thread exists in.</returns>
        /// <remarks>If a Post is passed in that is NOT found in the database, a ForumNotFoundException
        /// exception is thrown.</remarks>
        public  Forum GetForumInfoByPostID(int PostID) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetForumByPostID", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterPostId = new SqlParameter("@PostID", SqlDbType.Int, 4);
            parameterPostId.Value = PostID;
            myCommand.Parameters.Add(parameterPostId);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            if (!dr.Read()) {
                dr.Close();
                myConnection.Close();
                // we didn't get a forum, handle it
                throw new Components.ForumNotFoundException("Did not get back a forum for PostID " + PostID.ToString());
            }

            Forum f = PopulateForumFromSqlDataReader(dr);
            dr.Close();
            myConnection.Close();
            return f;
        }

        public ForumCollection GetForumsByForumGroupId(int forumGroupId, string username) {

            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetForumsByForumGroupId", myConnection);
            SqlParameter param;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter forumGroups = new SqlParameter("@ForumGroupId", SqlDbType.Int, 1);
            forumGroups.Value = forumGroupId;
            myCommand.Parameters.Add(forumGroups);

            // Add Parameters to SPROC
            param = new SqlParameter("@GetAllForums", SqlDbType.Bit, 1);
            param.Value = 1; // TODO
            myCommand.Parameters.Add(param);

            // Add Parameters to SPROC
            param = new SqlParameter("@Username", SqlDbType.NVarChar, 50);
            if (username == null)
                param.Value = System.DBNull.Value;
            else
                param.Value = username;
            myCommand.Parameters.Add(param);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            // populate a ForumCollection object
            ForumCollection forums = new ForumCollection();
            Forum forum;

            while (dr.Read()) {

                forum = PopulateForumFromSqlDataReader(dr);

                forum.TotalPosts = Convert.ToInt32(dr["TotalPosts"]);
                forum.TotalThreads = Convert.ToInt32(dr["TotalTopics"]);
                forum.ForumGroupId = (int) dr["ForumGroupId"];

                // Handle Nulls in the case that we don't have a 'most recent post...'
                if (Convert.IsDBNull(dr["MostRecentPostAuthor"]))
                    forum.MostRecentPostAuthor = null;
                else
                    forum.MostRecentPostAuthor = Convert.ToString(dr["MostRecentPostAuthor"]);

                if (Convert.IsDBNull(dr["MostRecentPostId"])) {
                    forum.MostRecentPostId = 0;
                    forum.MostRecentThreadId = 0;
                } else {
                    forum.MostRecentPostId = Convert.ToInt32(dr["MostRecentPostId"]);
                    forum.MostRecentThreadId = Convert.ToInt32(dr["MostRecentThreadId"]);
                }

                if (Convert.IsDBNull(dr["MostRecentPostDate"]))
                    forum.MostRecentPostDate = DateTime.MinValue.AddMonths(1);
                else
                    forum.MostRecentPostDate = Convert.ToDateTime(dr["MostRecentPostDate"]);

                // Last time the user was active in the forum
                if (username != null) {
                    if (Convert.IsDBNull(dr["LastUserActivity"]))
                        forum.LastUserActivity = DateTime.MinValue.AddMonths(1);
                    else
                        forum.LastUserActivity = Convert.ToDateTime(dr["LastUserActivity"]);
                } else {
                    forum.LastUserActivity = DateTime.MinValue;
                }
            
                forums.Add(forum);

            }
            dr.Close();
            myConnection.Close();

            return forums;        
        }

        public ForumGroupCollection GetAllForumGroups(bool displayAllForumGroups, string username) {

            // Connect to the database
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetAllForumGroups", myConnection);

            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@GetAllForumsGroups", SqlDbType.Int).Value = displayAllForumGroups;
            if ( (username == null) || (username == String.Empty))
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;
    
            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            // populate a ForumGroupCollection object
            ForumGroupCollection forumGroups = new ForumGroupCollection();
            ForumGroup forumGroup;

            while (dr.Read()) {
                forumGroup = PopulateForumGroupFromSqlDataReader(dr);

                forumGroups.Add(forumGroup);
            }
            dr.Close();
            myConnection.Close();

            return forumGroups;
        }

        /// <summary>
        /// Returns a list of all Forums.
        /// </summary>
        /// <returns>A ForumCollection object.</returns>
        public  ForumCollection GetAllForums(bool showAllForums, string username) {
            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetAllForums", myConnection);
            SqlParameter param;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            param = new SqlParameter("@GetAllForums", SqlDbType.Bit, 1);
            param.Value = showAllForums;
            myCommand.Parameters.Add(param);

            // Add Parameters to SPROC
            param = new SqlParameter("@Username", SqlDbType.NVarChar, 50);
            if (username == null)
                param.Value = System.DBNull.Value;
            else
                param.Value = username;

            myCommand.Parameters.Add(param);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            // populate a ForumCollection object
            ForumCollection forums = new ForumCollection();
            Forum forum;
            while (dr.Read()) {
                forum = PopulateForumFromSqlDataReader(dr);

                forum.TotalPosts = Convert.ToInt32(dr["TotalPosts"]);
                forum.TotalThreads = Convert.ToInt32(dr["TotalTopics"]);
                forum.ForumGroupId = (int) dr["ForumGroupId"];
                
                // Handle Nulls
                if (Convert.IsDBNull(dr["MostRecentPostAuthor"]))
                    forum.MostRecentPostAuthor = null;
                else
                    forum.MostRecentPostAuthor = Convert.ToString(dr["MostRecentPostAuthor"]);

                if (Convert.IsDBNull(dr["MostRecentPostId"])) {
                    forum.MostRecentPostId = 0;
                    forum.MostRecentThreadId = 0;
                } else {
                    forum.MostRecentPostId = Convert.ToInt32(dr["MostRecentPostId"]);
                    forum.MostRecentThreadId = Convert.ToInt32(dr["MostRecentThreadId"]);
                }

                if (Convert.IsDBNull(dr["MostRecentPostDate"]))
                    forum.MostRecentPostDate = DateTime.MinValue.AddMonths(1);
                else
                    forum.MostRecentPostDate = Convert.ToDateTime(dr["MostRecentPostDate"]);

                // Last time the user was active in the forum
                if (username != null) {
                    if (Convert.IsDBNull(dr["LastUserActivity"]))
                        forum.LastUserActivity = DateTime.MinValue.AddMonths(1);
                    else
                        forum.LastUserActivity = Convert.ToDateTime(dr["LastUserActivity"]);
                } else {
                    forum.LastUserActivity = DateTime.MinValue;
                }

                forums.Add(forum);
            }
            dr.Close();
            myConnection.Close();

            return forums;
        }


    

        /// <summary>
        /// Deletes a forum and all of its posts.
        /// </summary>
        /// <param name="ForumID">The ID of the forum to delete.</param>
        /// <remarks>Be very careful when using this method.  It will permanently delete ALL of the
        /// posts associated with the forum.</remarks>
        public  void DeleteForum(int ForumID) {
            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_DeleteForum", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            SqlParameter parameterForumID = new SqlParameter("@ForumID", SqlDbType.Int, 4);
            parameterForumID.Value = ForumID;
            myCommand.Parameters.Add(parameterForumID);

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }


        /// <summary>
        /// Adds a new forum.
        /// </summary>
        /// <param name="forum">A Forum object instance that defines the variables for the new forum to
        /// be added.  The Forum object properties used to create the new forum are: Name, Description,
        /// Moderated, and DaysToView.</param>
        public void AddForum(Forum forum) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_AddForum", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterForumName = new SqlParameter("@Name", SqlDbType.NVarChar, 100);
            parameterForumName.Value = forum.Name;
            myCommand.Parameters.Add(parameterForumName);

            SqlParameter parameterForumDesc = new SqlParameter("@Description", SqlDbType.NVarChar, 5000);
            parameterForumDesc.Value = forum.Description;
            myCommand.Parameters.Add(parameterForumDesc);

            SqlParameter parameterForumGroupId = new SqlParameter("@ForumGroupId", SqlDbType.Int);
            parameterForumGroupId.Value = forum.ForumGroupId;
            myCommand.Parameters.Add(parameterForumGroupId);
            
            SqlParameter parameterModerated = new SqlParameter("@Moderated", SqlDbType.Bit, 1);
            parameterModerated.Value = forum.Moderated;
            myCommand.Parameters.Add(parameterModerated);

            SqlParameter parameterViewToDays = new SqlParameter("@DaysToView", SqlDbType.Int, 4);
            parameterViewToDays.Value = forum.DaysToView;
            myCommand.Parameters.Add(parameterViewToDays);

            SqlParameter parameterActive = new SqlParameter("@Active", SqlDbType.Bit, 1);
            parameterActive.Value = forum.Active;
            myCommand.Parameters.Add(parameterActive);


            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }



        /// <summary>
        /// Updates an existing forum.
        /// </summary>
        /// <param name="forum">A Forum object with the new, updated properties.  The ForumID property
        /// specifies what forum to update, while hte Name, Description, Moderated, and DaysToView
        /// properties indicate the new values.</param>
        public  void UpdateForum(Forum forum) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_UpdateForum", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@ForumGroupID", SqlDbType.Int).Value = forum.ForumGroupId;

            SqlParameter parameterForumId = new SqlParameter("@ForumID", SqlDbType.Int, 4);
            parameterForumId.Value = forum.ForumID;
            myCommand.Parameters.Add(parameterForumId);

            SqlParameter parameterForumName = new SqlParameter("@Name", SqlDbType.NVarChar, 100);
            parameterForumName.Value = forum.Name;
            myCommand.Parameters.Add(parameterForumName);

            SqlParameter parameterForumDesc = new SqlParameter("@Description", SqlDbType.NVarChar, 5000);
            parameterForumDesc.Value = forum.Description;
            myCommand.Parameters.Add(parameterForumDesc);

            SqlParameter parameterModerated = new SqlParameter("@Moderated", SqlDbType.Bit, 1);
            parameterModerated.Value = forum.Moderated;
            myCommand.Parameters.Add(parameterModerated);

            SqlParameter parameterViewToDays = new SqlParameter("@DaysToView", SqlDbType.Int, 4);
            parameterViewToDays.Value = forum.DaysToView;
            myCommand.Parameters.Add(parameterViewToDays);

            SqlParameter parameterActive = new SqlParameter("@Active", SqlDbType.Bit, 1);
            parameterActive.Value = forum.Active;
            myCommand.Parameters.Add(parameterActive);


            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }


        /// <summary>
        /// Returns the total number of forums.
        /// </summary>
        /// <returns>The total number of forums.</returns>
        public int TotalNumberOfForums() {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetTotalNumberOfForums", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            myConnection.Open();
            int totalForums = Convert.ToInt32(myCommand.ExecuteScalar());
            myConnection.Close();

            return totalForums;

        }
        /*********************************************************************************/




        /*********************************************************************************/

        /************************ USER FUNCTIONS ***********************
                 * These functions return information about a user.
                 * are called from the WebForums.Users class.
                 * *************************************************************/
    
        /// <summary>
        /// Retrieves information about a particular user.
        /// </summary>
        /// <param name="Username">The name of the User whose information you are interested in.</param>
        /// <returns>A User object.</returns>
        /// <remarks>If a Username is passed in that is NOT found in the database, a UserNotFoundException
        /// exception is thrown.</remarks>
        public  User GetUserInfo(String username, bool updateIsOnline) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetUserInfo", myConnection);
            SqlParameter param;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            param = new SqlParameter("@UserName", SqlDbType.NVarChar, 50);
            param.Value = username;
            myCommand.Parameters.Add(param);

            // Add Parameters to SPROC
            param = new SqlParameter("@UpdateIsOnline", SqlDbType.Bit);
            param.Value = updateIsOnline;
            myCommand.Parameters.Add(param);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            if (!dr.Read()) {
                dr.Close();
                myConnection.Close();
                // we didn't get a user, handle it
                throw new Components.UserNotFoundException("User not found for Username " + username);
            }

            User u = PopulateUserFromSqlDataReader(dr);
            dr.Close();
            myConnection.Close();

            return u;
        }

        /// <summary>
        /// Returns a depricated user collection of the user's currently online
        /// for the specified minutes. Only the username and whether they are an
        /// administrator is returned.
        /// </summary>
        /// <param name="pastMinutes">Minutes back in time</param>
        /// <returns></returns>
        public int TotalAnonymousUsersOnline() {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetAnonymousUsersOnline", myConnection);
            int anonymousUserCount = 0;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            if (dr.Read())
                anonymousUserCount = Convert.ToInt32(dr["AnonymousUserCount"]);

            dr.Close();
            myConnection.Close();

            return anonymousUserCount;
        }

        /// <summary>
        /// Returns a depricated user collection of the user's currently online
        /// for the specified minutes. Only the username and whether they are an
        /// administrator is returned.
        /// </summary>
        /// <param name="pastMinutes">Minutes back in time</param>
        /// <returns></returns>
        public UserCollection WhoIsOnline(int pastMinutes) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetUsersOnline", myConnection);
            UserCollection users = new UserCollection();

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterUsername = new SqlParameter("@PastMinutes", SqlDbType.Int);
            parameterUsername.Value = pastMinutes;
            myCommand.Parameters.Add(parameterUsername);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            while (dr.Read()) {
                OnlineUser u = new OnlineUser();
                u.Username = Convert.ToString(dr["Username"]);
                u.IsAdministrator = Convert.ToBoolean(dr["Administrator"]);
                u.IsModerator = Convert.ToBoolean(dr["IsModerator"]);

                users.Add(u);
            }

            dr.Close();
            myConnection.Close();

            return users;
        }
    
        /// <summary>
        /// Updates a user's information.
        /// </summary>
        /// <param name="user">A User object that contains information about the existing user.</param>
        /// <param name="NewPassword">The new password for the User.  (If the user has not changed their
        /// password, this property should be their existing password.)</param>
        /// <returns>A boolean - true if the user's password was correct, false otherwise.  In the case
        /// of an incorrect password being entered, the update is not performed.</returns>
        public  bool UpdateUserProfile(User user) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_UpdateUserInfo", myConnection);
            SqlParameter param;
            bool succeded = false;

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Username
            param = new SqlParameter("@UserName", SqlDbType.NVarChar, 50);
            param.Value = user.Username;
            myCommand.Parameters.Add(param);

            // Email
            param = new SqlParameter("@Email", SqlDbType.NVarChar, 75);
            param.Value = user.Email;
            myCommand.Parameters.Add(param);

            // Fake Email
            param = new SqlParameter("@FakeEmail", SqlDbType.NVarChar, 75);
            param.Value = user.PublicEmail;
            myCommand.Parameters.Add(param);

            // Website
            param = new SqlParameter("@URL", SqlDbType.NVarChar, 100);
            param.Value = user.Url;
            myCommand.Parameters.Add(param);

            // Occupation
            param = new SqlParameter("@Occupation", SqlDbType.NVarChar, 100);
            param.Value = user.Occupation;
            myCommand.Parameters.Add(param);

            // Location
            param = new SqlParameter("@Location", SqlDbType.NVarChar, 100);
            param.Value = user.Location;
            myCommand.Parameters.Add(param);

            // Interests
            param = new SqlParameter("@Interests", SqlDbType.NVarChar, 200);
            param.Value = user.Interests;
            myCommand.Parameters.Add(param);

            // MSN IM
            param = new SqlParameter("@MsnIm", SqlDbType.NVarChar, 50);
            param.Value = user.MsnIM;
            myCommand.Parameters.Add(param);

            // AOL IM
            param = new SqlParameter("@AolIm", SqlDbType.NVarChar, 50);
            param.Value = user.AolIM;
            myCommand.Parameters.Add(param);

            // Yahoo IM
            param = new SqlParameter("@YahooIm", SqlDbType.NVarChar, 50);
            param.Value = user.YahooIM;
            myCommand.Parameters.Add(param);

            // ICQ
            param = new SqlParameter("@IcqIm", SqlDbType.NVarChar, 50);
            param.Value = user.IcqIM;
            myCommand.Parameters.Add(param);

            // Signature
            param = new SqlParameter("@Signature", SqlDbType.NVarChar, 256);
            param.Value = user.Signature;
            myCommand.Parameters.Add(param);

            // Forum View
            param = new SqlParameter("@ForumView", SqlDbType.Int, 4);
            param.Value = (int) user.ForumView;
            myCommand.Parameters.Add(param);

            // Thread tracking
            param = new SqlParameter("@ThreadTracking", SqlDbType.Bit, 1);
            param.Value = user.TrackPosts;
            myCommand.Parameters.Add(param);

            // Timezone
            param = new SqlParameter("@Timezone", SqlDbType.Int, 4);
            param.Value = user.Timezone;
            myCommand.Parameters.Add(param);

            // Date Format
            param = new SqlParameter("@DateFormat", SqlDbType.NVarChar, 10);
            param.Value = user.DateFormat;
            myCommand.Parameters.Add(param);

            // HasAvatar
            myCommand.Parameters.Add("@HasAvatar", SqlDbType.Bit).Value = user.HasAvatar;

            // ShowAvatar
            myCommand.Parameters.Add("@ShowAvatar", SqlDbType.Bit).Value = user.ShowAvatar;

            // Post display order
            myCommand.Parameters.Add("@PostViewOrder", SqlDbType.Bit).Value = Convert.ToBoolean(user.ShowPostsAscending);

            // Password
            param = new SqlParameter("@Password", SqlDbType.NVarChar, 20);
            param.Value = user.Password;
            myCommand.Parameters.Add(param);

            // ShowUnreadThreadsOnly
            param = new SqlParameter("@ShowUnreadTopicsOnly", SqlDbType.Bit);
            param.Value = user.HideReadThreads;
            myCommand.Parameters.Add(param);

            // Site Style
            param = new SqlParameter("@SiteStyle", SqlDbType.NVarChar, 20);
            param.Value = user.Skin;
            myCommand.Parameters.Add(param);

            // AvatarType
            param = new SqlParameter("@AvatarType", SqlDbType.Int);
            param.Value = user.Avatar;
            myCommand.Parameters.Add(param);

            // Execute the command
            myConnection.Open();

            succeded = Convert.ToBoolean(Convert.ToInt32(myCommand.ExecuteScalar().ToString()));

            myConnection.Close();
            return succeded;
        }

        /// <summary>
        /// Returns a collection of users whose Username begins with a specified character.
        /// </summary>
        /// <param name="FirstCharacter">The starting character.</param>
        /// <returns>A UserCollection object with Users whose Username begins with the specified
        /// FirstCharacter letter.</returns>
        public  UserCollection GetUsersByFirstCharacter(String FirstCharacter) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetUsersByFirstCharacter", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            SqlParameter parameterUsername = new SqlParameter("@FirstLetter", SqlDbType.Char, 1);
            parameterUsername.Value = FirstCharacter;
            myCommand.Parameters.Add(parameterUsername);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            UserCollection users = new UserCollection();
            while (dr.Read()) {
                users.Add(PopulateUserFromSqlDataReader(dr));
            }

            dr.Close();
            myConnection.Close();
            return users;
        }

        

        /// <summary>
        /// Updates a user's information via the administration page.
        /// </summary>
        /// <param name="user">The information about the User you wish to update.  Note that the
        /// Username of the User identifies the user you wish to update, while the Approved,
        /// Trusted, and Administrator properties specify the new settings for those fields.</param>
        public void UpdateUserInfoFromAdminPage(User user) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_UpdateUserFromAdminPage", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = user.Username;
            myCommand.Parameters.Add("@ProfileApproved", SqlDbType.Bit, 1).Value = user.IsProfileApproved;
            myCommand.Parameters.Add("@Approved", SqlDbType.Bit, 1).Value = user.IsApproved;
            myCommand.Parameters.Add("@Trusted", SqlDbType.Bit, 1).Value = user.IsTrusted;

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }

        
        /// <summary>
        /// This method creates a new user if possible.  If the username or
        /// email addresses already exist, an appropriate CreateUserStatus message is
        /// returned.
        /// </summary>
        /// <param name="user">The email for the new user account.</param>
        /// <returns>An CreateUserStatus enumeration value, indicating if the user was created successfully
        /// (CreateUserStatus.Created) or if the new user couldn't be created because of a duplicate
        /// Username (CreateUserStatus.DuplicateUsername) or duplicate email address (CreateUserStatus.DuplicateEmailAddress).</returns>
        /// <remarks>All User accounts created must consist of a unique Username and a unique
        /// Email address.</remarks>
        public CreateUserStatus CreateNewUser(User user) {
            // make sure the user's name begins with an alphabetic character
            if (!Regex.IsMatch(user.Username, "^[A-Za-z].*"))
                return CreateUserStatus.InvalidFirstCharacter;

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_CreateNewUser", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterUsername = new SqlParameter("@Username", SqlDbType.NVarChar, 50);
            parameterUsername.Value = user.Username;
            myCommand.Parameters.Add(parameterUsername);

            SqlParameter parameterEmail = new SqlParameter("@Email", SqlDbType.NVarChar, 75);
            parameterEmail.Value = user.Email;
            myCommand.Parameters.Add(parameterEmail);

            SqlParameter parameterPassword = new SqlParameter("@RandomPassword", SqlDbType.NVarChar, 20);
            parameterPassword.Value = user.Password;
            myCommand.Parameters.Add(parameterPassword);
            

            // Execute the command
            myConnection.Open();
            
            int iStatusCode = Convert.ToInt32(myCommand.ExecuteScalar());
            
            CreateUserStatus status;
            switch (iStatusCode) {
                case 1:     // user created successfully
                    status = CreateUserStatus.Created;
                    break;

                case 2:     // username duplicate
                    status = CreateUserStatus.DuplicateUsername;
                    break;

                case 3:     // email address duplicate
                    status = CreateUserStatus.DuplicateEmailAddress;
                    break;

                default:    // oops, something bad happened
                    status = CreateUserStatus.UnknownFailure;
                    break;
            }

            myConnection.Close();

            return status;      // return the status result
        }

        
        /// <summary>
        /// This method determines whether or not a particular username/password combo
        /// is valid.
        /// </summary>
        /// <param name="user">The User to determine if he/she is valid.</param>
        /// <returns>A boolean value, indicating if the username and password are valid.</returns>
        public bool ValidUser(User user) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_CheckUserCredentials", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterUsername = new SqlParameter("@Username", SqlDbType.NVarChar, 50);
            parameterUsername.Value = user.Username;
            myCommand.Parameters.Add(parameterUsername);

            SqlParameter parameterPassword = new SqlParameter("@Password", SqlDbType.NVarChar, 20);
            parameterPassword.Value = user.Password;
            myCommand.Parameters.Add(parameterPassword);

            // Execute the command
            myConnection.Open();
            bool retVal = Convert.ToBoolean(myCommand.ExecuteScalar());
            myConnection.Close();
            return retVal;
        }


        /// <summary>
        /// Calculates and returns the total number of user accounts.
        /// </summary>
        /// <returns>The total number of user accounts created.</returns>
        public int TotalNumberOfUserAccounts(string usernameBeginsWith, string usernameToFind) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetTotalUsers", myConnection);

            // Set the command type to stored procedure
            myCommand.CommandType = CommandType.StoredProcedure;

            if ((usernameBeginsWith == "All") || (usernameBeginsWith == null))
                myCommand.Parameters.Add("@UsernameBeginsWith", SqlDbType.NVarChar, 1).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@UsernameBeginsWith", SqlDbType.NVarChar, 1).Value = usernameBeginsWith;

            if (usernameToFind == null)
                myCommand.Parameters.Add("@UsernameToFind", SqlDbType.NVarChar, 50).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@UsernameToFind", SqlDbType.NVarChar, 50).Value = usernameToFind;

            // Execute the command
            myConnection.Open();
            int retVal = Convert.ToInt32(myCommand.ExecuteScalar());
            myConnection.Close();
            return retVal;
        }
        /*********************************************************************************/




        /*********************************************************************************/




        /// <summary>
        /// Performs a search, returning a PostCollection object with appropriate posts.
        /// </summary>
        /// <param name="ToSearch">Specifies what to search, specifically.  Must be set to a valid
        /// ToSearchEnum value, which supports two possible values: PostsSearch and PostsBySearch.</param>
        /// <param name="SearchWhat">A SearchWhatEnum value, this parameter specifies what to search. 
        /// Acceptable values are: SearchAllWords, SearchAnyWord, and SearchExactPhrase.</param>
        /// <param name="ForumToSearch">Specifies what forum to search.  To search all forums, pass in a
        /// value of 0.</param>
        /// <param name="SearchTerms">Specifies the terms to search on.</param>
        /// <param name="Page">Specifies what page of the search results to display.</param>
        /// <param name="RecsPerPage">Specifies how many records per page to show on the search
        /// results.</param>
        /// <returns>A PostCollection object, containing the posts to display for the particular page
        /// of the search results.</returns>
        public  PostCollection GetSearchResults(ToSearchEnum ToSearch, SearchWhatEnum SearchWhat, int ForumToSearch, String SearchTerms, int Page, int RecsPerPage, string username) {

            // return all of the forums and their total and daily posts
            // first, though, we've got to put our search phrase in the right order
            String strColumnName = "";
            String strWhereClause = " WHERE (";
            String [] aTerms = null;
			
            
            // Are we searching for a particular user?
            if (ToSearch == ToSearchEnum.PostsSearch) {
                strColumnName = "Body";

                // depending on the search style, our WHERE clause will differ
                switch(SearchWhat) {
                    case SearchWhatEnum.SearchExactPhrase:
                        // easy, we want to search for the exact search term
                        strWhereClause += strColumnName + " LIKE '%" + SearchTerms + "%' ";
                        break;
					
                    case SearchWhatEnum.SearchAllWords:
                        // allrighty, we want to find rows where each word is found
                        // split up the search term string into an array
                        aTerms = SearchTerms.Split(new char[]{' '});
					
                        // now, loop through the aTerms array
                        strWhereClause += strColumnName + " LIKE '%" + String.Join("%' AND " + strColumnName + " LIKE '%", aTerms) + "%'";
                        break;

                    case SearchWhatEnum.SearchAnyWord:
                        // allrighty, we want to find rows where each word is found
                        // split up the search term string into an array
                        aTerms = SearchTerms.Split(new char[]{' '});
					
                        // now, loop through the aTerms array
                        strWhereClause += strColumnName + " LIKE '%" + String.Join("%' OR " + strColumnName + " LIKE '%", aTerms) + "%'";
                        break;
                }
			
                strWhereClause += ") AND Approved=1 ";


            }
            else if (ToSearch == ToSearchEnum.PostsBySearch) {
                strColumnName = "UserName";

                strWhereClause += strColumnName + " = '" + SearchTerms + "') AND Approved = 1 ";
            }
			
            // see if we need to add a restriction on the ForumID
            if (ForumToSearch > 0)
                strWhereClause += " AND P.ForumID = " + ForumToSearch.ToString() + " ";
				
			
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetSearchResults", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;
            
            SqlParameter parameterPage = new SqlParameter("@Page", SqlDbType.Int, 4);
            parameterPage.Value = Page;
            myCommand.Parameters.Add(parameterPage);

            SqlParameter parameterRecsPerPage = new SqlParameter("@RecsPerPage", SqlDbType.Int, 4);
            parameterRecsPerPage.Value = RecsPerPage;
            myCommand.Parameters.Add(parameterRecsPerPage);

            SqlParameter parameterSearchTerms = new SqlParameter("@SearchTerms", SqlDbType.NVarChar, 500);
            parameterSearchTerms.Value = strWhereClause;
            myCommand.Parameters.Add(parameterSearchTerms);
            
            if ( (username == null) || (username == String.Empty))
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            // populate the Posts collection
            PostCollection posts = new PostCollection();
            if (!dr.Read()) {
                dr.Close();
                myConnection.Close();
                // we have an empty result, return the empty post collection
                return posts;
            } else {
                // we have to populate our postcollection
                posts.TotalRecordCount = Convert.ToInt32(dr["MoreRecords"]);

                do {
                    posts.Add(PopulatePostFromSqlDataReader(dr));
                    ((Post) posts[posts.Count - 1]).ForumName = Convert.ToString(dr["ForumName"]);
                } while (dr.Read());

                dr.Close();
                myConnection.Close();

                return posts;
            }
        }
        /*********************************************************************************/





        /*********************************************************************************/

        /********************* MODERATION FUNCTIONS *********************
                 * These functions are used to perform moderation.  They are called
                 * from the WebForums.Moderate class.
                 * **************************************************************/


        // **********************************************************************
        /// <summary>
        /// Given a username, returns a boolean indicating whether or not the user has
        /// posts awaiting moderation.
        /// </summary>
        /// <param name="Username"></param>
        /// <returns></returns>
        // **********************************************************************
        public bool UserHasPostsAwaitingModeration(String username) {

            //TODO 

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_UserHasPostsAwaitingModeration", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@username", SqlDbType.NVarChar, 50).Value = username;

            // Open the database connection and execute the command
            SqlDataReader dr;

            myConnection.Open();
            dr = myCommand.ExecuteReader(CommandBehavior.CloseConnection);

            // create a String array from the data
            ArrayList userRoles = new ArrayList();


            dr.Close();

            // Return the String array of roles
            return false;
        }

        /// <summary>
        /// Gets a list of posts that are awaiting moderation that the current user has rights to moderate.
        /// </summary>
        /// <param name="Username">The User who is interested in viewing a list of posts awaiting
        /// moderation.</param>
        /// <returns>A PostCollection containing the posts the user can view that are awaiting moderation.</returns>
        public  PostCollection GetPostsAwaitingModeration(String Username) {
            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetModeratedPosts", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterUsername = new SqlParameter("@UserName", SqlDbType.NVarChar, 50);
            parameterUsername.Value = Username;
            myCommand.Parameters.Add(parameterUsername);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            PostCollection posts = new PostCollection();
            Post post = null;

            while (dr.Read()) {
                post = PopulatePostFromSqlDataReader(dr);
                post.ForumName = Convert.ToString(dr["ForumName"]);

                posts.Add(post);
            }
            dr.Close();
            myConnection.Close();

            return posts;
        }



        /// <summary>
        /// Approves a particular post that is waiting to be moderated.
        /// </summary>
        /// <param name="PostID">The ID of the post to approve.</param>
        /// <returns>A boolean indicating if the post has already been approved.</returns>
        /// <remarks>Keep in mind that multiple moderators may be working on approving/moving/editing/deleting
        /// posts at the same time.  Hence, these moderation functions may not perform the desired task.
        /// For example, if one opts to delete a post that has already been approved, the deletion will
        /// fail.</remarks>
        public bool ApprovePost(int postID, string approvedBy, string updateUserAsTrusted) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_ApproveModeratedPost", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@PostID", SqlDbType.Int, 4).Value = postID;
            myCommand.Parameters.Add("@ApprovedBy", SqlDbType.NVarChar, 50).Value = approvedBy;

            if (updateUserAsTrusted == null)
                myCommand.Parameters.Add("@Trusted", SqlDbType.NVarChar, 50).Value = System.DBNull.Value;
            else
                myCommand.Parameters.Add("@Trusted", SqlDbType.NVarChar, 50).Value = updateUserAsTrusted;

            // Execute the command
            myConnection.Open();
            int iResult = Convert.ToInt32(myCommand.ExecuteScalar().ToString());
            myConnection.Close();

            return iResult == 1;        // was the post previously approved?
        }


        /// <summary>
        /// Deletes a post that is currently waiting to be moderated.
        /// </summary>
        /// <param name="PostID">The ID of the post to delete.</param>
        /// <returns>A boolean, true if the post has been deleted, false otherwise.  The post might not
        /// be deleted if someone else has already approved the post.</returns>
        /// <remarks>Keep in mind that multiple moderators may be working on approving/moving/editing/deleting
        /// posts at the same time.  Hence, these moderation functions may not perform the desired task.
        /// For example, if one opts to delete a post that has already been approved, the deletion will
        /// fail.</remarks>
        public  bool DeleteModeratedPost(int postID, string approvedBy) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_DeleteNonApprovedPost", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@PostID", SqlDbType.Int, 4).Value = postID;
            myCommand.Parameters.Add("@ApprovedBy", SqlDbType.NVarChar, 50).Value = approvedBy;

            // Execute the command
            myConnection.Open();
            int iRowsAffectedCount = Convert.ToInt32(myCommand.ExecuteScalar().ToString());
            myConnection.Close();
            
            return iRowsAffectedCount != 0;     
        }


        /// <summary>
        /// Indicates if a particular user can moderate posts.
        /// </summary>
        /// <param name="Username">The User to check.</param>
        /// <returns>True if the user can moderate, False otherwise.</returns>
        public  bool CanModerate(String Username) {
            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_CanModerate", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterUsername = new SqlParameter("@UserName", SqlDbType.NVarChar, 50);
            parameterUsername.Value = Username;
            myCommand.Parameters.Add(parameterUsername);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();
            if (!dr.Read())
                throw new Components.UserNotFoundException("User not found for Username " + Username);
            
            // check to see if the user can moderate
            bool bolCanModerate = dr["CanModerate"].ToString() == "1";
            
            dr.Close();
            myConnection.Close();
            
            return bolCanModerate;
        }

        /// <summary>
        /// Indicates if a particular user can moderate posts.
        /// </summary>
        /// <param name="Username">The User to check.</param>
        /// <returns>True if the user can moderate, False otherwise.</returns>
        public  bool CanModerate(String username, int forumID) {
            // return all of the forums and their total and daily posts
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_CanModerateForum", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = username;
            myCommand.Parameters.Add("@ForumID", SqlDbType.Int).Value = forumID;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();
            if (!dr.Read())
                throw new Components.UserNotFoundException("User not found for Username " + username);
            
            // check to see if the user can moderate
            bool boolCanModerate = dr["CanModerate"].ToString() == "1";
            
            dr.Close();
            myConnection.Close();
            
            return boolCanModerate;
        }

        /// <summary>
        /// Determines if a user can edit a particular post.
        /// </summary>
        /// <param name="Username">The name of the User.</param>
        /// <param name="PostID">The Post the User wants to edit.</param>
        /// <returns>A boolean value - True if the user can edit the Post, False otherwise.</returns>
        /// <remarks>An Administrator can edit any post.  Moderators may edit posts from forums that they
        /// have moderation rights to and that are awaiting approval.</remarks>
        public  bool CanEditPost(String Username, int PostID) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_CanEditPost", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterPostID = new SqlParameter("@PostID", SqlDbType.Int, 4);
            parameterPostID.Value = PostID;
            myCommand.Parameters.Add(parameterPostID);

            SqlParameter parameterUsername = new SqlParameter("@Username", SqlDbType.NVarChar, 50);
            parameterUsername.Value = Username;
            myCommand.Parameters.Add(parameterUsername);

            // Execute the command
            myConnection.Open();
            int iResponse = Convert.ToInt32(myCommand.ExecuteScalar().ToString());
            myConnection.Close();
            
            return iResponse == 1;
        }


        /// <summary>
        /// Moves a post awaiting moderation from one Forum to another.
        /// </summary>
        /// <param name="PostID">The ID of the Post to move.</param>
        /// <param name="MoveToForumID">The ID of the forum to move the post to.</param>
        /// <param name="Username">The name of the User who is attempting to move the post.</param>
        /// <returns>A MovedPostStatus enumeration value that indicates the status of the attempted move.
        /// This enumeration has three values: NotMoved, MovedButNotApproved, and MovedAndApproved.</returns>
        /// <remarks>A value of NotMoved means the post was not moved (either it has been approved already
        /// or deleted); a value of MovedButNotApproved indicates that the post has been moved to a new
        /// forum, but the user moving the post was NOT a moderator for the forum it was moved to, hence
        /// the moved post is still waiting to be approved; a value of MovedAndApproved indicates that the
        /// moderator moved to post to a forum he moderates, hence the post is automatically approved.</remarks>
        public  MovedPostStatus MovePost(int postID, int moveToForumID, String approvedBy) {

            // moves a post to a specified forum
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_MovePost", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            myCommand.Parameters.Add("@PostID", SqlDbType.Int, 4).Value = postID;
            myCommand.Parameters.Add("@MoveToForumID", SqlDbType.Int, 4).Value = moveToForumID;
            myCommand.Parameters.Add("@Username", SqlDbType.NVarChar, 50).Value = approvedBy;

            // Execute the command
            myConnection.Open();
            int iStatus = Convert.ToInt32(myCommand.ExecuteScalar().ToString());
            myConnection.Close();

            // Determine the status of the moved post
            switch (iStatus) {
                case 0:
                    return MovedPostStatus.NotMoved;
                    
                case 1:
                    return MovedPostStatus.MovedButNotApproved;
                    
                default:
                    return MovedPostStatus.MovedAndApproved;
            }
        }
        /*********************************************************************************/




        /*********************************************************************************/

        /********************* EMAIL FUNCTIONS *********************
                 * These functions are used to perform email functionality.
                 * They are called from the WebForums.Email class
                 * *********************************************************/

        /// <summary>
        /// Returns a list of Users that have email thread tracking turned on for a particular post
        /// in a particular thread.
        /// </summary>
        /// <param name="PostID">The ID of the Post of the thread to send a notification to.</param>
        /// <returns>A UserCollection listing the users who have email thread tracking turned on for
        /// the specified thread.</returns>
        public  UserCollection GetEmailList(int PostID) {
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetTrackingEmailsForThread", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterPostId = new SqlParameter("@PostID", SqlDbType.Int, 4);
            parameterPostId.Value = PostID;
            myCommand.Parameters.Add(parameterPostId);

    
            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            UserCollection users = new UserCollection();
            User u;
            while (dr.Read()) {
                u = new User();
                u.Email = dr["Email"].ToString();
                users.Add(u);
            }

            dr.Close();
            myConnection.Close();

            return users;
        }


        /// <summary>
        /// Returns information about a particular Email Template.
        /// </summary>
        /// <param name="EmailTemplateID">The ID of the Email Template.</param>
        /// <returns>An EmailTemplate class instance.</returns>
        /// <remarks>If the passed in EmailTemplateID does not match to a database entry, a
        /// EmailTemplateNotFoundException exception is thrown.</remarks>
        public EmailTemplate GetEmailTemplateInfo(int EmailTemplateID) {
            EmailTemplate template;

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetEmailInfo", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterEmailId = new SqlParameter("@EmailId", SqlDbType.Int, 4);
            parameterEmailId.Value = EmailTemplateID;
            myCommand.Parameters.Add(parameterEmailId);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();
            if (!dr.Read())
                throw new Components.EmailTemplateNotFoundException("Email template not found for EmailTemplateID " + EmailTemplateID.ToString());
            
            template = PopulateEmailTemplateFromSqlDataReader(dr);

            myConnection.Close();

            return template;
        }



        /// <summary>
        /// Returns a list of all of the Email Templates.
        /// </summary>
        /// <returns>An EmailTemplateCollection instance, that contains a listing of all of the available
        /// Email Templates.</returns>
        public  EmailTemplateCollection GetEmailTemplateList() {
            // Get the username from the approved Post
            
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetEmailList", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            EmailTemplateCollection emails = new EmailTemplateCollection();
            while (dr.Read()) {
                emails.Add(PopulateEmailTemplateFromSqlDataReader(dr));
            }
            dr.Close();
            myConnection.Close();

            return emails;
        }


        /// <summary>
        /// Updates an existing Email Template.
        /// </summary>
        /// <param name="email">An EmailTemplate object instance that contains information on the
        /// Email Template to update.  The EmailID property of the passed in EmailTemplate instance
        /// specifies the Email Template to update, while the Subject and Message properties indicate
        /// the updated Subject and Message values for the Email Template.</param>
        public void UpdateEmailTemplate(EmailTemplate email) {
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_UpdateEmailTemplate", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterEmailId = new SqlParameter("@EmailId", SqlDbType.Int, 4);
            parameterEmailId.Value = email.EmailTemplateID;
            myCommand.Parameters.Add(parameterEmailId);

            SqlParameter parameterSubject = new SqlParameter("@Subject", SqlDbType.NVarChar, 50);
            parameterSubject.Value = email.Subject;
            myCommand.Parameters.Add(parameterSubject);

            SqlParameter parameterMessage = new SqlParameter("@Message", SqlDbType.NText);
            parameterMessage.Value = email.Body;
            myCommand.Parameters.Add(parameterMessage);

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }
        /*********************************************************************************/





        /*********************************************************************************/

        /**************** MODERATOR LISTING FUNCTIONS **************
                 * These functions are used to edit/update/work with the list
                 * of forums a user can moderate.
                 * *********************************************************/

        /// <summary>
        /// Retrieves a list of the Forums moderated by a particular user.
        /// </summary>
        /// <param name="user">The User whose list of moderated forums we are interested in.</param>
        /// <returns>A ModeratedForumCollection.</returns>
        public  ModeratedForumCollection GetForumsModeratedByUser(String Username) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetForumsModeratedByUser", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterUsername = new SqlParameter("@UserName", SqlDbType.NVarChar, 50);
            parameterUsername.Value = Username;
            myCommand.Parameters.Add(parameterUsername);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            ModeratedForumCollection forums = new ModeratedForumCollection();
            ModeratedForum forum;
            while (dr.Read()) {
                forum = new ModeratedForum();
                forum.ForumID = Convert.ToInt32(dr["ForumID"]);
                forum.Name = Convert.ToString(dr["ForumName"]);
                forum.DateCreated = Convert.ToDateTime(dr["DateCreated"]);
                forum.EmailNotification = Convert.ToBoolean(dr["EmailNotification"]);

                forums.Add(forum);
            }
            dr.Close();
            myConnection.Close();

            return forums;
        }


        /// <summary>
        /// Returns a list of forms NOT moderated by the user.
        /// </summary>
        /// <param name="user">The User whose list of non-moderated forums we are interested in
        /// viewing.</param>
        /// <returns>A ModeratedForumCollection containing the list of forums NOT moderated by
        /// the particular user.</returns>
        public ModeratedForumCollection GetForumsNotModeratedByUser(String Username) {          
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetForumsNotModeratedByUser", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterUsername = new SqlParameter("@UserName", SqlDbType.NVarChar, 50);
            parameterUsername.Value = Username;
            myCommand.Parameters.Add(parameterUsername);

            // Execute the command
            myConnection.Open();

            SqlDataReader dr = myCommand.ExecuteReader();

            ModeratedForumCollection forums = new ModeratedForumCollection();
            ModeratedForum forum;
            while (dr.Read()) {
                forum = new ModeratedForum();
                forum.ForumID = Convert.ToInt32(dr["ForumID"]);
                forum.Name = Convert.ToString(dr["ForumName"]);

                forums.Add(forum);
            }
            dr.Close();
            myConnection.Close();

            return forums;
        }


        /// <summary>
        /// Adds a forum to the list of moderatable forums for a particular user.
        /// </summary>
        /// <param name="forum">A ModeratedForum instance that contains the settings for the new user's
        /// moderatable forum.  The Username property indicates the user who can moderate the forum,
        /// the ForumID property indicates what forum the user can moderate, and the
        /// EmailNotification property indicates whether or not the moderator receives email
        /// notification when a new post has been made to the forum.</param>
        public void AddModeratedForumForUser(ModeratedForum forum) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_AddModeratedForumForUser", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterUsername = new SqlParameter("@UserName", SqlDbType.NVarChar, 50);
            parameterUsername.Value = forum.Username;
            myCommand.Parameters.Add(parameterUsername);

            SqlParameter parameterForumID = new SqlParameter("@ForumID", SqlDbType.Int, 4);
            parameterForumID.Value = forum.ForumID;
            myCommand.Parameters.Add(parameterForumID);

            SqlParameter parameterEmailNotification = new SqlParameter("@EmailNotification", SqlDbType.Bit, 1);
            parameterEmailNotification.Value = forum.EmailNotification;
            myCommand.Parameters.Add(parameterEmailNotification);

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }


        /// <summary>
        /// Removes a moderated forum for a particular user.  
        /// </summary>
        /// <param name="forum">A ModeratedForum instance.  The Username and ForumID properties specify
        /// what Forum to remove from what User's list of moderatable forums.</param>
        public  void RemoveModeratedForumForUser(ModeratedForum forum) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_RemoveModeratedForumForUser", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterUsername = new SqlParameter("@UserName", SqlDbType.NVarChar, 50);
            parameterUsername.Value = forum.Username;
            myCommand.Parameters.Add(parameterUsername);

            SqlParameter parameterForumID = new SqlParameter("@ForumID", SqlDbType.Int, 4);
            parameterForumID.Value = forum.ForumID;
            myCommand.Parameters.Add(parameterForumID);

            // Execute the command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }


        /// <summary>
        /// Returns a list of users who are interested in receiving email notification for a newly
        /// added post.
        /// </summary>
        /// <param name="PostID">The ID of the newly added post.</param>
        /// <returns>A UserCollection instance containing the users who want to be emailed when a new
        /// post is added to the moderation system.</returns>
        public  UserCollection GetModeratorsInterestedInPost(int PostID) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetModeratorsForEmailNotification", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterPostID = new SqlParameter("@PostID", SqlDbType.Int, 4);
            parameterPostID.Value = PostID;
            myCommand.Parameters.Add(parameterPostID);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            UserCollection users = new UserCollection();
            User user;
            while (dr.Read()) {
                user = new User();
                user.Username = Convert.ToString(dr["Username"]);
                user.Email = Convert.ToString(dr["Email"]);
                users.Add(user);
            }
            dr.Close();
            myConnection.Close();

            return users;
        }

    

        /// <summary>
        /// Returns a list of the moderators of a particular forum.
        /// </summary>
        /// <param name="ForumID">The ID of the Forum whose moderators we are interested in listing.</param>
        /// <returns>A ModeratedForumCollection containing the moderators of a particular Forum.</returns>
        public  ModeratedForumCollection GetForumModerators(int ForumID) {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetForumModerators", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterForumId = new SqlParameter("@ForumID", SqlDbType.Int, 4);
            parameterForumId.Value = ForumID;
            myCommand.Parameters.Add(parameterForumId);

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            ModeratedForumCollection forums = new ModeratedForumCollection();
            ModeratedForum forum;
            while (dr.Read()) {
                forum = new ModeratedForum();
                forum.Username = Convert.ToString(dr["Username"]);
                forum.ForumID = ForumID;
                forum.EmailNotification = Convert.ToBoolean(dr["EmailNotification"]);
                forum.DateCreated = Convert.ToDateTime(dr["DateCreated"]);

                forums.Add(forum);
            }
            dr.Close();
            myConnection.Close();

            return forums;
        }
        /*********************************************************************************/



        /*********************************************************************************/

        /**************** SUMMARY FUNCTIONS **************
                 * This function is used to get Summary information about WebForums.NET
                 * *********************************************************/

    
        /// <summary>
        /// Returns a SummaryObject object with summary information about the message board.
        /// </summary>
        /// <returns>A SummaryObject object populated with the summary information.</returns>
        public Statistics GetSiteStatistics() {
            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(Globals.DatabaseConnectionString);
            SqlCommand myCommand = new SqlCommand("dbo.forums_GetStatistics", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Execute the command
            myConnection.Open();
            SqlDataReader dr = myCommand.ExecuteReader();

            Statistics statistics = new Statistics();           

            dr.Read();

            statistics.TotalUsers = Convert.ToInt32(dr["TotalUsers"]);
            statistics.TotalPosts = Convert.ToInt32(dr["TotalPosts"]);
            statistics.TotalModerators = Convert.ToInt32(dr["TotalModerators"]);
            statistics.TotalModeratedPosts = Convert.ToInt32(dr["TotalModeratedPosts"]);
            statistics.TotalThreads = Convert.ToInt32(dr["TotalTopics"]);
            statistics.NewPostsInPast24Hours = Convert.ToInt32(dr["NewPostsInPast24Hours"]);
            statistics.NewThreadsInPast24Hours = Convert.ToInt32(dr["NewThreadsInPast24Hours"]);
            statistics.NewUsersInPast24Hours = Convert.ToInt32(dr["NewUsersInPast24Hours"]);
            statistics.MostViewsPostID = Convert.ToInt32(dr["MostViewsPostID"]);
            statistics.MostViewsSubject = Convert.ToString(dr["MostViewsSubject"]);
            statistics.MostActivePostID = Convert.ToInt32(dr["MostActivePostID"]);
            statistics.MostActiveSubject = Convert.ToString(dr["MostActiveSubject"]);
            statistics.MostReadPostID = Convert.ToInt32(dr["MostReadPostID"]);
            statistics.MostReadPostSubject = Convert.ToString(dr["MostReadSubject"]);
            statistics.MostActiveUser = Convert.ToString(dr["MostActiveUser"]);
            statistics.NewestUser = Convert.ToString(dr["NewestUser"]);

            dr.Close();
            myConnection.Close();

            return statistics;
        }

        /*********************************************************************************/
    }
}
