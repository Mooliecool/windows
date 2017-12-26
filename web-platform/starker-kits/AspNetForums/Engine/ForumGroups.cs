using System;
using System.Web;
using AspNetForums.Components;

namespace AspNetForums {

    // *********************************************************************
    //  ForumGroups
    //
    /// <summary>
    /// This class contains methods for working with the Forum Groups.
    /// </summary>
    /// 
    // ********************************************************************/ 
    public class ForumGroups {


        // *********************************************************************
        //  GetForumGroupByForumID
        //
        /// <summary>
        /// Returns the forum group for a given forum id.
        /// </summary>
        /// <param name="forumID">Forum id used to lookup forum</param>
        /// 
        // ********************************************************************/ 
        public static ForumGroup GetForumGroupByForumID(int forumID) {
            
            if (null == HttpContext.Current.Items["ForumGroupByForumID" + forumID]) {
                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                HttpContext.Current.Items["ForumGroupByForumID" + forumID] = dp.GetForumGroupByForumId(forumID);
            }

            return (ForumGroup) HttpContext.Current.Items["ForumGroupByForumID" + forumID];
            
        }

        // *********************************************************************
        //  AddForumGroup
        //
        /// <summary>
        /// Adds a new forum group
        /// </summary>
        /// <param name="forumGroupName">Name of new forum group to create.</param>
        /// 
        // ********************************************************************/ 
        public static void AddForumGroup(string forumGroupName) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            dp.AddForumGroup(forumGroupName);
        }

        // *********************************************************************
        //  GetForumGroup
        //
        /// <summary>
        /// Returns the forum group for a given forum group id.
        /// </summary>
        /// <param name="forumGroupID">Forum group id to return a forum group for.</param>
        /// <returns>The total number of forums.</returns>
        /// 
        // ********************************************************************/ 
        public static ForumGroup GetForumGroup(int forumGroupID) {
            string cacheKey = "ForumGroup-" + forumGroupID;

            // This doesn't change a whole lot so we'll cache values
            if (HttpContext.Current.Cache[cacheKey] == null) {
                GetAllForumGroups(false, true);
            }

            if (HttpContext.Current.Cache[cacheKey] == null)
                throw new Components.ForumGroupNotFoundException("Did not get back a forum group for forum group id " + forumGroupID.ToString());
            else
                return (ForumGroup) HttpContext.Current.Cache[cacheKey];

        }

        // *********************************************************************
        //  ChangeForumGroupSortOrder
        //
        /// <summary>
        /// Used to change the sort order of forum groups
        /// </summary>
        /// <param name="forumGroupID">Id of forum group to move</param>
        /// <param name="moveUp">True to indicate that the forum is moving up.</param>
        /// 
        // ********************************************************************/ 
        public static void ChangeForumGroupSortOrder(int forumGroupID, bool moveUp) {
            IDataProviderBase dp = DataProvider.Instance();

            dp.ChangeForumGroupSortOrder(forumGroupID, moveUp);
        }

        // *********************************************************************
        //  GetAllForumGroups
        //
        /// <summary>
        /// Returns a list of all forum groups
        /// </summary>
        /// <param name="displayForumGroupsNotApproved">If true returns all forum groups</param>
        /// <param name="allowFromCache">Whether or not the request can be satisfied from the Cache</param>
        /// 
        // ********************************************************************/ 
        public static ForumGroupCollection GetAllForumGroups(bool displayForumGroupsNotApproved, bool allowFromCache) {
            string cacheKey = "AllForumGroups-" + displayForumGroupsNotApproved + HttpContext.Current.User.Identity.Name;
            ForumGroupCollection forums;

            // This doesn't change a whole lot so we'll cache values
            if ((HttpContext.Current.Cache[cacheKey] != null) && (allowFromCache)) {
                forums = (ForumGroupCollection) HttpContext.Current.Cache[cacheKey];
            } else {
                IDataProviderBase dp = DataProvider.Instance();

                forums = dp.GetAllForumGroups(displayForumGroupsNotApproved, HttpContext.Current.User.Identity.Name);

                // Sort the forum groups
                forums.Sort();

                HttpContext.Current.Cache.Insert(cacheKey, forums, null, DateTime.Now.AddMinutes(5), TimeSpan.Zero);

                // Add all the forums to the cache as well
                foreach (ForumGroup f in forums) {
                    HttpContext.Current.Cache.Insert("ForumGroup-" + f.ForumGroupID, f, null, DateTime.Now.AddMinutes(5), TimeSpan.Zero);
                }
            }

            forums.Sort();

            return forums;

        }

        
        // *********************************************************************
        //  UpdateForumGroup
        //
        /// <summary>
        /// Update a forum group name
        /// </summary>
        /// <param name="forumGroupName">new name value</param>
        /// <param name="forumGroupId">id of forum group to replace new name value with</param>
        // ********************************************************************/ 
        public static void UpdateForumGroup(string forumGroupName, int forumGroupId) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            dp.UpdateForumGroup(forumGroupName, forumGroupId);
        }
    }
}
