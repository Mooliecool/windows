using System;
using System.Web.Caching;
using System.Collections.Specialized;
using System.Reflection;
using System.Configuration;
using System.Web;

namespace AspNetForums.Components {

    /// <summary>
    /// The DataProvider class contains a single method, Instance(), which returns an instance of the
    /// user-specified data provider class.
    /// </summary>
    /// <remarks>  The data provider class must inherit the IDataProviderBase
    /// interface.</remarks>
    public class DataProvider {
        /// <summary>
        /// Returns an instance of the user-specified data provider class.
        /// </summary>
        /// <returns>An instance of the user-specified data provider class.  This class must inherit the
        /// IDataProviderBase interface.</returns>
        public static IDataProviderBase Instance() {
            //use the cache because the reflection used later is expensive
            Cache cache = System.Web.HttpContext.Current.Cache;

            if ( cache["IDataProviderBase"] == null ) {
                //get the assembly path and class name from web.config
                String prefix = "";
                NameValueCollection context = (NameValueCollection) ConfigurationSettings.GetConfig("AspNetForumsSettings");
                if (context == null) {
                    // get the appSettings context
                    prefix = Globals._appSettingsPrefix;
                    context = (NameValueCollection)ConfigurationSettings.GetConfig("appSettings");
                }

                String assemblyPath = context[prefix + "DataProviderAssemblyPath"];
                String className = context[prefix + "DataProviderClassName"];

                // assemblyPath presented in virtual form, must convert to physical path
                assemblyPath = HttpContext.Current.Server.MapPath(HttpContext.Current.Request.ApplicationPath + "/bin/" + assemblyPath);					

                // Uuse reflection to store the constructor of the class that implements IWebForumDataProvider
                try {
                    cache.Insert( "IDataProviderBase", Assembly.LoadFrom( assemblyPath).GetType( className ).GetConstructor(new Type[0]), new CacheDependency( assemblyPath ) );
                }
                catch (Exception) {

                    // could not locate DLL file
                    HttpContext.Current.Response.Write("<b>ERROR:</b> Could not locate file: <code>" + assemblyPath + "</code> or could not locate class <code>" + className + "</code> in file.");
                    HttpContext.Current.Response.End();
                }
            }
            return (IDataProviderBase)(  ((ConstructorInfo)cache["IDataProviderBase"]).Invoke(null) );
        }
    }




    public interface IDataProviderBase {
        /*************************** POST METHODS ******************************/
        PostCollection GetAllMessages(int ForumID, ViewOptions ForumView, int PagesBack);
        PostDetails GetPostDetails(int PostID, String Username);
        Post GetPost(int postID, string username, bool trackViews);		
        void ReverseThreadTracking(String Username, int PostID);
        PostCollection GetThread(int ThreadID);
        PostCollection GetThreadByPostID(int postID, string username);
        PostCollection GetThreadByPostID(int postID, int currentPageIndex, int pageSize, int sortBy, int sortOrder, string username);
        PostCollection GetTopNNewPosts(string username, int postCount);
        PostCollection GetTopNPopularPosts(string username, int postCount, int days);
        Post AddPost(Post postToAdd, string username);
        void UpdatePost(Post post, string editedBy);
        void DeletePost(int postID, string approvedBy, string reason);
        int GetTotalPostCount();
        void MarkPostAsRead(int postID, string username);
        bool IsUserTrackingThread(int threadID, string username);
        /***********************************************************************/

        /*************************** Thread METHODS ******************************/
        ThreadCollection GetAllThreads(int forumID, string username, bool unreadThreadsOnly);
        ThreadCollection GetAllThreads(int forumID, int pageSize, int pageIndex, DateTime endDate, string username, bool unreadThreadsOnly);
        int GetTotalPostsForThread(int postID);
        ThreadCollection GetThreadsUserIsTracking(string username);
        ThreadCollection GetThreadsUserMostRecentlyParticipatedIn(string username);
        int GetNextThreadID(int postID);
        int GetPrevThreadID(int postID);
        /***********************************************************************/

        /*************************** FORUM MESSAGES ******************************/
        ForumMessage GetMessage(int messageId);
        ForumMessageTemplateCollection GetMessageTemplateList();
        void UpdateMessageTemplate(ForumMessage message);
        /***********************************************************************/


        /*************************** FORUM METHODS ******************************/
        Forum GetForumInfoByThreadID(int ThreadID);
        void MarkAllThreadsRead(int forumID, string username);
        Forum GetForumInfo(int ForumID, string username);
        Forum GetForumInfoByPostID(int PostID);
        ForumCollection GetAllForums(bool showAllForums, string username);
        ForumCollection GetForumsByForumGroupId(int forumGroupId, string username);
        void AddForumGroup(string forumGroupName);
        void DeleteForum(int ForumID);
        void AddForum(Forum forum);
        void UpdateForum(Forum forum);
        int TotalNumberOfForums();
        int GetTotalThreadsInForum(int ForumID, DateTime maxDateTime, DateTime minDateTime, string username, bool unreadThreadsOnly);
        /***********************************************************************/

        /*************************** FORUM GROUP METHODS ******************************/
        ForumGroup GetForumGroupByForumId(int forumID);
        ForumGroupCollection GetAllForumGroups(bool displayAllForumGroups, string username);
        void UpdateForumGroup(string forumGroupName, int forumGroupId);
        void ChangeForumGroupSortOrder(int forumGroupID, bool moveUp);
        /***********************************************************************/

        /*************************** USER METHODS *****************************/
        User GetUserInfo(String Username, bool updateIsOnline);
        bool UpdateUserProfile(User user);
        UserCollection GetUsersByFirstCharacter(String FirstCharacter);
        void UpdateUserInfoFromAdminPage(User user);
        CreateUserStatus CreateNewUser(User user);
        bool ValidUser(User user);
        UserCollection WhoIsOnline(int pastMinutes);
        int TotalNumberOfUserAccounts(string usernameBeginsWith, string usernameToFind);
        void TrackAnonymousUsers(string userId);
        int TotalAnonymousUsersOnline();
        UserCollection GetAllUsers(int pageIndex, int pageSize, Users.SortUsersBy sortBy, int sortOrder, string usernameBeginsWith);
        void ChangePasswordForLoggedOnUser(string username, string newPassword);
        string GetUsernameByEmail(string emailAddress);
        void ToggleOptions(string username, bool hideReadThreads, ViewOptions viewOptions);
        UserCollection GetMostActiveUsers();
        UserCollection FindUsersByName(int pageIndex, int pageSize, string usernameToMatch);
        /**********************************************************************/

        /************************** SEARCH METHODS *****************************/
        PostCollection GetSearchResults(ToSearchEnum ToSearch, SearchWhatEnum SearchWhat, int ForumToSearch, String SearchTerms, int Page, int RecsPerPage, string username);
        /***********************************************************************/
	
	
        /*********************** MODERATION METHODS ***************************/
        PostCollection GetPostsAwaitingModeration(String Username);
        bool ApprovePost(int postID, string approvedBy, string updateUserAsTrusted);
        bool DeleteModeratedPost(int postID, string approvedBy);
        bool CanModerate(String username);		
        bool CanModerate(String username, int forumId);		
        bool CanEditPost(String Username, int PostID);
        MovedPostStatus MovePost(int postID, int moveToForumID, String approvedBy);
        bool UserHasPostsAwaitingModeration(String Username);
        ForumGroupCollection GetForumGroupsForModeration(string username);
        ModeratedForumCollection GetForumsForModerationByForumGroupId(int forumGroupId, string username);
        ThreadCollection GetAllUnmoderatedThreads(int forumID, int pageSize, int pageIndex, string username);
        int GetTotalUnModeratedThreadsInForum(int ForumID, DateTime maxDateTime, DateTime minDateTime, string username, bool unreadThreadsOnly);
        ModeratorCollection GetMostActiveModerators();
        ModerationAuditCollection GetModerationAuditSummary();
        ModerationQueueStatus GetQueueStatus(int forumID, string username);
        /**********************************************************************/


        /************************** EMAIL METHODS *****************************/
        UserCollection GetEmailList(int PostID);
        EmailTemplate GetEmailTemplateInfo(int EmailTemplateID);
        EmailTemplateCollection GetEmailTemplateList();
        void UpdateEmailTemplate(EmailTemplate email);
        /**********************************************************************/


        /********************* MODERATOR LISTING METHODS **********************/
        ModeratedForumCollection GetForumsModeratedByUser(String Username);
        ModeratedForumCollection GetForumsNotModeratedByUser(String Username);
        void AddModeratedForumForUser(ModeratedForum forum);
        void RemoveModeratedForumForUser(ModeratedForum forum);
        UserCollection GetModeratorsInterestedInPost(int PostID);
        ModeratedForumCollection GetForumModerators(int ForumID);
        /**********************************************************************/

        /********************* USER ROLES METHODS **********************/
        String[] GetUserRoles(string username);
        void AddUserToRole(string username, string role);
        void RemoveUserFromRole(string username, string role);

        String[] GetForumRoles(int forumID);
        void AddForumToRole(int forumID, string role);
        void RemoveForumFromRole(int forumID, string role);

        String[] GetAllRoles();
        void CreateNewRole(string role, string description);
        void DeleteRole(string role);
        string GetRoleDescription(string role);
        void UpdateRoleDescription(string role, string description);
        /**********************************************************************/

        /************************** VOTE METHODS ***************************/
        void Vote(int postID, string selection);
        VoteResultCollection GetVoteResults(int postID);
        /**********************************************************************/


        /************************** SUMMARY METHODS ***************************/
        Statistics GetSiteStatistics();
        /**********************************************************************/
    }
}
