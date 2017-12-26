using System;
using System.Text.RegularExpressions;
using AspNetForums.Components;
using System.Web;
using System.Web.Caching;
using System.Collections;

namespace AspNetForums {

    // *********************************************************************
    //  Users
    //
    /// <summary>
    /// This class encapsulates all data operations for managing forum users.
    /// </summary>
    // ***********************************************************************/
    public class Users {

        // *********************************************************************
        //  GetUserInfo
        //
        /// <summary>
        /// Return information about a particular user.
        /// </summary>
        /// <param name="username">The user whose information you are interested in.</param>
        /// <param name="updateIsOnline">Updates user's online datetime stamp.</param>
        /// <returns>Instance of User with details about a given forum user.</returns>
        /// <remarks>
        /// If the specified user is not found, a UserNotFoundException exception is thrown. Feel
        /// free to call this multiple times during the request as the value is stored in Context once
        /// read from the data source.
        /// </remarks>
        // ***********************************************************************/
        public static User GetUserInfo(String username, bool updateIsOnline) {
            string userKey = "UserInfo-" + username;

            // Attempt to return the user from Cache for users not online to save
            // us a trip to the database.
            if (updateIsOnline == false) {
                if (HttpContext.Current.Cache[userKey] != null)
                    return (User) HttpContext.Current.Cache[userKey];
            }

            // Let's not go to the database each time we need the user's info
            if (HttpContext.Current.Items[userKey] == null) {
                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                User user = dp.GetUserInfo(username, updateIsOnline);

                // Hang on to the data for this request only
                HttpContext.Current.Items[userKey] = user;
            }

            // Do we need to add the user into the Cache
            if (updateIsOnline == false) {
                if (HttpContext.Current.Cache[userKey] == null)
                    HttpContext.Current.Cache.Insert(userKey, HttpContext.Current.Items[userKey], null, DateTime.Now.AddMinutes(1), TimeSpan.Zero);
            }

            return (User) HttpContext.Current.Items[userKey];
        }

        // *********************************************************************
        //  ChangePasswordForLoggedOnUser
        //
        /// <summary>
        /// Changes the password for the currently logged on user.
        /// </summary>
        /// <param name="password">User's current password.</param>
        /// <param name="newPassword">User's new password.</param>
        /// <returns>Indicates whether or not the password change succeeded</returns>
        // ***********************************************************************/
        public static bool ChangePasswordForLoggedOnUser(string password, string newPassword) {
            User user;

            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            // Get the current user
            user = Users.GetUserInfo(HttpContext.Current.User.Identity.Name, false);

            // Check to ensure the passwords match
            if (password != user.Password)
                return false;
            else
                dp.ChangePasswordForLoggedOnUser(user.Username, newPassword);

            user.Password = newPassword;

            // Email the user their password
            Emails.SendEmail(user.Username, EmailTypeEnum.ChangedPassword);

            return true;
        }

        // *********************************************************************
        //  WhoIsOnline
        //
        /// <summary>
        /// Returns a user collection of all the user's online. Lookup is only
        /// performed every 30 seconds.
        /// </summary>
        /// <param name="pastMinutes">How many minutes in time we should go back to return users.</param>
        /// <returns>A collection of user.</returns>
        /// 
        // ********************************************************************/
        public static UserCollection WhoIsOnline(int pastMinutes) {
            UserCollection users;

            // Read from the cache if available
            if (HttpContext.Current.Cache["WhoIsOnline"] == null) {
                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                // Get the users
                users = dp.WhoIsOnline(pastMinutes);

                // Add to the Cache
                HttpContext.Current.Cache.Insert("WhoIsOnline", users, null, DateTime.Now.AddMinutes(5), TimeSpan.Zero);

            }

            return (UserCollection) HttpContext.Current.Cache["WhoIsOnline"];

        }

        // *********************************************************************
        //  GetUsernameByEmail
        //
        /// <summary>
        /// Returns a username given a user's email address.
        /// </summary>
        /// <param name="emailAddress">Email address to look up username by.</param>
        /// <returns>Username</returns>
        /// 
        // ********************************************************************/
        public static string GetUsernameByEmail(string emailAddress) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetUsernameByEmail(emailAddress);
        }

        // *********************************************************************
        //  FindUsersByName
        //
        /// <summary>
        /// Returns a user collection of users that match the string provided
        /// </summary>
        /// <param name="emailAddress">String to match on.</param>
        /// <returns>Username</returns>
        /// 
        // ********************************************************************/
        public static UserCollection FindUsersByName(int pageIndex, int pageSize, string usernameToMatch) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.FindUsersByName(pageIndex, pageSize, usernameToMatch);
        }

        // *********************************************************************
        //  AdjustForTimezone
        //
        /// <summary>
        /// Adjusts a date/time for a user's particular timezone offset.
        /// </summary>
        /// <param name="dtAdjust">The time to adjust.</param>
        /// <param name="user">The user viewing the time.</param>
        /// <returns>A datetime adjusted for the user's timezone offset.</returns>
        /// 
        // ********************************************************************/
        public static DateTime AdjustForTimezone(DateTime dtAdjust, User user) {
            return dtAdjust.AddHours(user.Timezone - Globals.DBTimezone);
        }

        // *********************************************************************
        //  AdjustForTimezone
        //
        /// <summary>
        /// Adjusts a date/time for a specified timezone offset.
        /// </summary>
        /// <param name="dtAdjust">The time to adjust.</param>
        /// <param name="TimezoneOffset">The timezone offset to adjust the date/time to (0 = GMT)</param>
        /// <returns>A string containing the date and time, adjusted for the specified timezone offset</returns>
        /// 
        // ********************************************************************/
        public static String AdjustForTimezone(DateTime dtAdjust, int TimezoneOffset) {
            return dtAdjust.AddHours(TimezoneOffset - Globals.DBTimezone).ToString();
        }

        // *********************************************************************
        //  AdjustForTimezone
        //
        /// <summary>
        /// Adjusts a date/time for a user's particular timezone offset applying a particular formatting.
        /// </summary>
        /// <param name="dtAdjust">The time to adjust.</param>
        /// <param name="user">The user viewing the time.</param>
        /// <param name="format">A string representing the desired date/time format.</param>
        /// <returns>A string containing the date and time, adjusted for the user's timezone offset.</returns>
        /// 
        // ********************************************************************/
        public static String AdjustForTimezone(DateTime dtAdjust, User user, String format) {
            return dtAdjust.AddHours(user.Timezone - Globals.DBTimezone).ToString(format);
        }

        // *********************************************************************
        //  AdjustForTimezone
        //
        /// <summary>
        /// Adjusts a date/time for a specified timezone offset applying a particular formatting.
        /// </summary>
        /// <param name="dtAdjust">The time to adjust.</param>
        /// <param name="TimezoneOffset">The timezone offset to adjust the date/time to (0 = GMT)</param>
        /// <param name="format">A string representing the desired date/time format.</param>
        /// <returns>A string containing the date and time, adjusted for the specified timezone offset</returns>
        /// 
        // ********************************************************************/
        public static String AdjustForTimezone(DateTime dtAdjust, int TimezoneOffset, String format) {
            return dtAdjust.AddHours(TimezoneOffset - Globals.DBTimezone).ToString(format);
        }

        // *********************************************************************
        //  GetAllUsers
        //
        /// <summary>
        /// Returns all the users currently in the system.
        /// </summary>
        /// <param name="pageIndex">Page position in which to return user's for, e.g. position of result set</param>
        /// <param name="pageSize">Size of a given page, e.g. size of result set.</param>
        /// <param name="sortBy">How the returned user's are to be sorted.</param>
        /// <param name="sortOrder">Direction in which to sort</param>
        /// <returns>A collection of user.</returns>
        /// 
        // ********************************************************************/
        public static UserCollection GetAllUsers(int pageIndex, int pageSize, SortUsersBy sortBy, int sortOrder, string usernameBeginsWith) {
            UserCollection users;

            // Build a cache key
            string usersKey = pageIndex.ToString() + pageSize.ToString() + sortBy + sortOrder.ToString() + usernameBeginsWith;

            // Serve from the cache when possible
            users = (UserCollection) HttpContext.Current.Cache[usersKey];

            if (users == null) {
                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                users =  dp.GetAllUsers(pageIndex, pageSize, sortBy, sortOrder, usernameBeginsWith);

                // Insert the user collection into the cache for 120 seconds
                HttpContext.Current.Cache.Insert(usersKey, users, null, DateTime.Now.AddSeconds(1800), TimeSpan.Zero);
            }

            return users;
        }

        // *********************************************************************
        //  TrackAnonymousUsers
        //
        /// <summary>
        /// Used to keep track of the number of anonymous users on the system
        /// </summary>
        /// <returns>A collection of user.</returns>
        /// 
        // ********************************************************************/
        public static void TrackAnonymousUsers() {
            string userId;
            HttpCookie cookie;
            string cookieName = "AspNetForumsAnonymousUser";

            // Check if the Tracking cookie exists
            cookie = HttpContext.Current.Request.Cookies[cookieName];

            // Anonymous users are tracking in 15 minute intervals
            if (null == cookie) {
                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                userId = Guid.NewGuid().ToString();
        
                HttpContext.Current.Response.Cookies[cookieName].Expires = DateTime.Now.AddMinutes(15);
                HttpContext.Current.Response.Cookies[cookieName].Value = userId;

                // If it's a new user only...
                dp.TrackAnonymousUsers(userId);
            }
        }

        // *********************************************************************
        //  GetAnonymousUsersOnline
        //
        /// <summary>
        /// Returns total number of anonymous users currently online.
        /// </summary>
        /// <returns>A numerical value of for the number online</returns>
        /// 
        // ********************************************************************/
        public static int GetAnonymousUsersOnline() {
            int anonymousUsersOnline = 0;

            // Read from the cache if available
            if (HttpContext.Current.Cache["AnonymousUsersOnlineCount"] == null) {
                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                // Get the number of anonymous users online
                anonymousUsersOnline = dp.TotalAnonymousUsersOnline();

                // Add to the Cache
                HttpContext.Current.Cache.Insert("AnonymousUsersOnlineCount", anonymousUsersOnline, null, DateTime.Now.AddSeconds(30), TimeSpan.Zero);

            }

            return (int) HttpContext.Current.Cache["AnonymousUsersOnlineCount"];

        }

        // *********************************************************************
        //  GetMostActiveUsers
        //
        /// <summary>
        /// Returns a list of the users that have added the most posts
        /// </summary>
        /// <returns>A UserCollection</returns>
        /// 
        // ********************************************************************/
        public static UserCollection GetMostActiveUsers() {
            UserCollection users;

            // Only update once every 24 hours
            if (HttpContext.Current.Cache["MostActiveUsers"] == null) {
                // Create Instance of the IDataProviderBase
                IDataProviderBase dp = DataProvider.Instance();

                // Get the collection
                users = dp.GetMostActiveUsers();

                // add to the cache
                HttpContext.Current.Cache.Insert("MostActiveUsers", users, null, DateTime.Now.AddDays(1), TimeSpan.Zero);

            }

            return (UserCollection) HttpContext.Current.Cache["MostActiveUsers"];

       }

        // *********************************************************************
        //  GetLoggedOnUser
        //
        /// <summary>
        /// Short-cut for getting back a user instance
        /// </summary>
        /// <returnsA User instance based off the value of User.Identity.Name</returns>
        /// 
        // ********************************************************************/
        public static User GetLoggedOnUser() {
        
            if (!HttpContext.Current.Request.IsAuthenticated)
                return null;

            return Users.GetUserInfo(HttpContext.Current.User.Identity.Name, true);
        }


        // *********************************************************************
        //  UpdateUserInfo
        //
        /// <summary>
        /// Updates a user's personal information.
        /// </summary>
        /// <param name="user">The user to update.  The Username indicates what user to update.</param>
        /// <param name="NewPassword">If the user is changing their password, the user's new password.
        /// Otherwise, this should be the user's existing password.</param>
        /// <returns>This method returns a boolean: it returns True if
        /// the update succeeds, false otherwise.  (The update might fail if the user enters an
        /// incorrect password.)</returns>
        /// <remarks>For the user to update their information, they must supply their password.  Therefore,
        /// the Password property of the user object passed in should be set to the user's existing password.
        /// The NewPassword parameter should contain the user's new password (if they are changing it) or
        /// existing password if they are not.  From this method, only the user's personal information can
        /// be updated (the user's password, forum view settings, email address, etc.); to update the user's
        /// system-level settings (whether or not they are banned, their trusted status, etc.), use the
        /// UpdateUserInfoFromAdminPage method.  <seealso cref="UpdateUserInfoFromAdminPage"/></remarks>
        /// 
        // ********************************************************************/
        public static bool UpdateUserProfile(User user) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();
            bool updatePasswordSucceded = false;

            // we need to strip the <script> tags from input forms
            user.Signature = StripScriptTagsFromInput(user.Signature);
            user.AolIM = Globals.HtmlEncode(user.AolIM);
            user.Email = Globals.HtmlEncode(user.Email);
            user.PublicEmail = Globals.HtmlEncode(user.PublicEmail);
            user.IcqIM = Globals.HtmlEncode(user.IcqIM);
            user.Interests = Globals.HtmlEncode(user.Interests);
            user.Location = Globals.HtmlEncode(user.Location);
            user.MsnIM = Globals.HtmlEncode(user.MsnIM);
            user.Occupation = Globals.HtmlEncode(user.Occupation);
            user.Url = Globals.HtmlEncode(user.Url);
            user.Username = StripScriptTagsFromInput(user.Username);
            user.YahooIM = Globals.HtmlEncode(user.YahooIM);

            // Call the underlying update
            updatePasswordSucceded = dp.UpdateUserProfile(user);

            // Remove from the cache if it exists
            HttpContext.Current.Cache.Remove("UserInfo-" + user.Username);

            return updatePasswordSucceded;
        }

        // *********************************************************************
        //  StripScriptTagsFromInput
        //
        /// <summary>
        /// Helper function used to ensure we don't inject script into the db.
        /// </summary>
        /// <param name="dirtyText">Text to be cleaned for script tags</param>
        /// <returns>Clean text with no script tags.</returns>
        /// 
        // ********************************************************************/
        public static string StripScriptTagsFromInput(string dirtyText) {
            string cleanText;

            // Perform RegEx
            cleanText = Regex.Replace(dirtyText, "<script((.|\n)*?)</script>", "", RegexOptions.IgnoreCase | RegexOptions.Multiline);

            return cleanText;
        }

        // *********************************************************************
        //  GetUsersByFirstCharacter
        //
        /// <summary>
        /// Returns a list of users whose username starts with FirstCharacter.
        /// </summary>
        /// <param name="FirstCharacter">The starting letter of users you are interested in
        /// viewing.</param>
        /// <returns>A UserCollection populated with the users whose Username begins with FirstCharacter.</returns>
        /// 
        // ********************************************************************/
        public static UserCollection GetUsersByFirstCharacter(String FirstCharacter) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetUsersByFirstCharacter(FirstCharacter);			
        }

        // *********************************************************************
        //  UpdateUserInfoFromAdminPage
        //
        /// <summary>
        /// Updates a user's system-level information.
        /// </summary>
        /// <param name="user">A user object containing information to be updated.  The Username
        /// property specifies what user should be updated.</param>
        /// <remarks>This method updates a user's system-level information: their approved status, their
        /// trusted status, etc.  To update a user's personal information (such as their password,
        /// signature, homepage Url, etc.), use the UpdateUserInfo method.  <seealso cref="UpdateUserInfo"/></remarks>
        /// 
        // ********************************************************************/
        public static void UpdateUserInfoFromAdminPage(User user) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            dp.UpdateUserInfoFromAdminPage(user);			
        }


        // *********************************************************************
        //  GetForumsModeratedByUser
        //
        /// <summary>
        /// Returns a list of forums moderated by a particular user.
        /// </summary>
        /// <param name="Username">The user whose list of moderated forums you are interested in viewing.</param>
        /// <returns>A ModeratedForumCollection containing a listing of the forums moderated by the specified
        /// user.</returns>
        /// 
        // ********************************************************************/
        public static ModeratedForumCollection GetForumsModeratedByUser(String Username) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetForumsModeratedByUser(Username);			
        }


        // *********************************************************************
        //  GetForumsNotModeratedByUser
        //
        /// <summary>
        /// Returns a list of forums that are NOT moderated by the specified user.
        /// </summary>
        /// <param name="Username">The Username of the user whose list of non-moderated forums you
        /// are interested in.</param>
        /// <returns>A ModeratedForumColelction containing a listing of the forums NOT moderated by the 
        /// specified user.</returns>
        /// 
        // ********************************************************************/
        public static ModeratedForumCollection GetForumsNotModeratedByUser(String Username) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.GetForumsNotModeratedByUser(Username);			
        }

        // *********************************************************************
        //  ToggleOptions
        //
        /// <summary>
        /// Toggle various user options
        /// </summary>
        /// <param name="username">Name of user we're updating</param>
        /// <param name="hideReadThreads">Hide threads that the user has already read</param>
        /// 
        // ********************************************************************/
        public static void ToggleOptions(string username, bool hideReadThreads) {
            ToggleOptions(username, hideReadThreads, ViewOptions.NotSet);			
        }

        // *********************************************************************
        //  ToggleOptions
        //
        /// <summary>
        /// Toggle various user options
        /// </summary>
        /// <param name="username">Name of user we're updating</param>
        /// <param name="hideReadThreads">Hide threads that the user has already read</param>
        /// <param name="viewOptions">How the user views posts</param>
        /// 
        // ********************************************************************/
        public static void ToggleOptions(string username, bool hideReadThreads, ViewOptions viewOptions) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            dp.ToggleOptions(username, hideReadThreads, viewOptions);			
        }

        // *********************************************************************
        //  AddModeratedForumForUser
        //
        /// <summary>
        /// Adds a forum to the user's list of moderated forums.
        /// </summary>
        /// <param name="forum">A ModeratedForum object containing information on the forum to add.</param>
        /// 
        // ********************************************************************/
        public static void AddModeratedForumForUser(ModeratedForum forum) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            dp.AddModeratedForumForUser(forum);			
        }


        // *********************************************************************
        //  RemoveModeratedForumForUser
        //
        /// <summary>
        /// Removes a forum from the user's list of moderated forums.
        /// </summary>
        /// <param name="forum">A ModeratedForum object specifying the forum to remove.</param>
        /// 
        // ********************************************************************/
        public static void RemoveModeratedForumForUser(ModeratedForum forum) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            dp.RemoveModeratedForumForUser(forum);			
        }


        // *********************************************************************
        //  ValidUser
        //
        /// <summary>
        /// Determines if the user is a valid user.
        /// </summary>
        /// <param name="user">The user to check.  Note that the Username and Password properties of the
        /// User object must be set.</param>
        /// <returns>A boolean: true if the user's Username/password are valid; false if they are not,
        /// or if the user has been banned.</returns>
        /// 
        // ********************************************************************/
        public static bool ValidUser(User user) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.ValidUser(user);
        }


        // *********************************************************************
        //  CreateNewUser
        //
        /// <summary>
        /// Creates a new user.
        /// </summary>
        /// <param name="user">A User object containing information about the user to create.  Only the
        /// Username and Email properties are used here.</param>
        /// <returns></returns>
        /// <remarks>This method chooses a random password for the user and emails the user his new Username/password.
        /// From that point on, the user can configure their settings.</remarks>
        /// 
        // ********************************************************************/
        public static CreateUserStatus CreateNewUser(User user, bool needToSendEmail) {

            // Make sure the username begins with an alpha character
            if (!Regex.IsMatch(user.Username, "^[A-Za-z].*"))
                return CreateUserStatus.InvalidFirstCharacter;

            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            // do we have a password?
            if (user.Password == String.Empty) {
                // assign a temporary password
                const int passwordLength = 10;
                user.Password = Globals.CreateTemporaryPassword(passwordLength);

                needToSendEmail = true;
            }

            CreateUserStatus status = dp.CreateNewUser(user);			// create the user account

            if (status == CreateUserStatus.Created && needToSendEmail)
                // send an email to the user with their new logon info
                Emails.SendEmail(user.Username, EmailTypeEnum.NewUserAccountCreated);

            return status;
        }

        
        // *********************************************************************
        //  TotalNumberOfUserAccounts
        //
        /// <summary>
        /// Calculates and returns the total number of user accounts.
        /// </summary>
        /// <returns>The total number of user accounts created.</returns>
        /// 
        // ********************************************************************/
        public static int TotalNumberOfUserAccounts() {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.TotalNumberOfUserAccounts(null, null);
        }

        
        // *********************************************************************
        //  TotalNumberOfUserAccounts
        //
        /// <summary>
        /// Calculates and returns the total number of user accounts.
        /// </summary>
        /// <returns>The total number of user accounts created.</returns>
        /// 
        // ********************************************************************/
        public static int TotalNumberOfUserAccounts(string usernameBeginsWith, string usernameToFind) {
            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            return dp.TotalNumberOfUserAccounts(usernameBeginsWith, usernameToFind);
        }

        // *********************************************************************
        //  SortUsersBy
        //
        /// <summary>
        /// Enum for control how user's are sorted - Note the sort is performed
        /// in the database.
        /// </summary>
        /// 
        // ********************************************************************/
        public enum SortUsersBy {
            JoinedDate = 0,
            Username = 1,
            Website = 2,
            LastActiveDate = 3,
            Posts = 4
        }

    }
}
