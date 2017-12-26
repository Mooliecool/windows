using System;
using System.Web;

namespace AspNetForums.Components {

    // *********************************************************************
    //  User
    //
    /// <summary>
    /// This class contains the properties for a User.
    /// </summary>
    /// 
    // ********************************************************************/
    public class User {
        String username = "";				// the user's Username (unique identifier)
        String password = "";				// the user's password
        String email = "";					// the user's real email address (used to send info)
        String fakeEmail = "";				// the user's fake email address (shown in postings)
        String url = "";					// the user's homepage (appears on posts)
        String signature = "";				// the user's signature (appears on posts)
        String location = "";               // the user's location
        String occupation = "";             // the user's occupation
        String interests = "";              // the user's interests
        String msn = "";                    // MSN Instant messenger
        String yahoo = "";                  // Yahoo IM
        String aim = "";                    // AOL IM
        String icq = "";                    // ICQ IM
        String skin = "";                   // Default skin used in the site
        AvatarType avatarType;              // Such as gif, jpg, etc.
        string dateFormat = "";             // Format for how the user views the date
        string avatarUrl = "";              // Url to the Avatar user has provided
        int totalPosts;                     // Total posts by this user
        byte[] attributes;                  // Bit field stored in db for special read-only user attribute switches

        ViewOptions _forumView;		        // how the user wishes to view the forums (flat/mixed/threaded)

        bool hasAvatar;                     // if the user has an icon
        bool showAvatar;                    // control whether or not a user's icon is shown
        bool trusted;						// if the user is a trusted user or not
        bool approved;						// if the user is an approved user or not
        bool isModerator;                   // if the user is a moderator
        bool isProfileApproved;             // if the user's profile is approved
        bool trackPosts;					// if the user wants to automatically sign up for email tracking for his/her own posts
        bool showUnreadThreadsOnly;          // If true only shows unread threads in forum view
        bool sortPostsAscending;            // Order that posts are displayed
        bool flatView;                      // Controls how views are seen in Post View

        DateTime dateCreated;				// the date the user account was created
        DateTime lastLogin;				    // the date the user last logged in
        DateTime lastActive;              // Last time the user was active

        int timezone;						// the user's specified timezone


        /// <summary>
        /// Returns the user's Username.
        /// </summary>
        /// <remarks>The Username is what uniquely identifies each user.</remarks>
        public String Username {
            get { return username; }
            set { username = value; }			
        }

        public bool ViewPostsInFlatView {
            get { return flatView; }
            set { flatView = value; }
        }

        public byte[] Attributes {
            get { return attributes; }
            set { attributes = value; }
        }

        /// <summary>
        /// Returns the user's password.
        /// </summary>
        public String Password {
            get { return password; }
            set { password = value; }
        }

        /// <summary>
        /// Controls views in forums
        /// </summary>
        public bool HideReadThreads {
            get { return showUnreadThreadsOnly; }
            set { showUnreadThreadsOnly = value; }
        }

        /// <summary>
        /// Controls views in posts
        /// </summary>
        public bool ShowPostsAscending {
            get { return sortPostsAscending; }
            set { sortPostsAscending = value; }
        }
        
        /// <summary>
        /// Returns the image type to be displayed
        /// </summary>
        public AvatarType Avatar {
            get { return avatarType; }
            set { avatarType = value; }
        }

        /// <summary>
        /// Controls whether or not a user's icon is shown
        /// </summary>
        public bool ShowAvatar {
            get { return showAvatar; }
            set { showAvatar = value; }
        }

        /// <summary>
        /// Controls the skin the user views the site with
        /// </summary>
        public string Skin {
            get { return skin; }
            set { skin = value; }
        }

        /// <summary>
        /// Path to the user's image url
        /// </summary>
        public String AvatarUrl {
            get {
                // Do we have an avatar for this user?
                if ((HasAvatar) && (ShowAvatar)) {

                    // Is the avatar a URL?
                    if (Avatar == AvatarType.url) {
                        return avatarUrl;
                    } else {
                        string fileName = Username.Replace(" ", "_");
                        return Globals.ApplicationVRoot + "/UserIcons/" + fileName + "." + Avatar.ToString();
                    }
                } else {
                    return null;
                }
            }
            set {
                avatarUrl = value;
            }
        }

        /// <summary>
        /// Format for how the user wishes to view date values
        /// </summary>
        public String DateFormat {
            get {
                return dateFormat;
            }
            set {
                dateFormat = value;
            }
        }

        /// <summary>
        /// Returns the user's real email address.  It is this email address that the user is sent
        /// email notifications.
        /// </summary>
        public String Email {
            get { return email; }
            set { email = value; }
        }

        /// <summary>
        /// Specifies the user's fake email address.  This email address, if supplied, is the one
        /// that is displayed when showing a post posted by the user.
        /// </summary>
        public String PublicEmail {
            get { return fakeEmail; }
            set { fakeEmail = value; }
        }

        /// <summary>
        /// The user's homepage or favorite Url.  This Url is shown at the end of each of the user's posts.
        /// </summary>
        public String Url {
            get { return url; }
            set { url = value; }
        }

        /// <summary>
        /// The user's signature.  If specified, this signature is shown at the end of each of the user's posts.
        /// </summary>
        public String Signature {
            get { return signature; }
            set { signature = value; }
        }

        /// <summary>
        /// Icon for the user
        /// </summary>
        public bool HasAvatar {
            get { return hasAvatar; }
            set { hasAvatar = value; }
        }

        /// <summary>
        /// The user's location
        /// </summary>
        public String Location {
            get { return location; }
            set { location = value; }
        }

        /// <summary>
        /// The user's occupation
        /// </summary>
        public String Occupation {
            get { return occupation; }
            set { occupation = value; }
        }

        /// <summary>
        /// The user's interests
        /// </summary>
        public String Interests {
            get { return interests; }
            set { interests = value; }
        }
        
        /// <summary>
        /// MSN IM address
        /// </summary>
        public String MsnIM {
            get { return msn; }
            set { msn = value; }
        }

        /// <summary>
        /// Yahoo IM address
        /// </summary>
        public String YahooIM {
            get { return yahoo; }
            set { yahoo = value; }
        }

        /// <summary>
        /// AOL IM Address
        /// </summary>
        public String AolIM {
            get { return aim; }
            set { aim = value; }
        }

        /// <summary>
        /// ICQ address
        /// </summary>
        public String IcqIM {
            get { return icq; }
            set { icq = value; }
        }

        /// <summary>
        /// Total posts by this user
        /// </summary>
        public int TotalPosts {
            get { return totalPosts; }
            set { totalPosts = value; }
        }
        
        /// <summary>
        /// The date/time the user's account was created.
        /// </summary>
        public DateTime DateCreated {
            get { return dateCreated; }
            set { dateCreated = value; }
        }

        /// <summary>
        /// The date/time the user last logged in.
        /// </summary>
        public DateTime LastLogin {
            get { return lastLogin; }
            set { lastLogin = value; }
        }

        /// <summary>
        /// The date/time the user last logged in.
        /// </summary>
        public DateTime LastActivity {
            get { return lastActive; }
            set { lastActive = value; }
        }

        /// <summary>
        /// Specifies whether a user is Approved or not.  Non-approved users cannot log into the system
        /// and, therefore, cannot post messages.
        /// </summary>
        public bool IsApproved {
            get { return approved; }
            set { approved = value; }
        }

        /// <summary>
        /// Specifies whether a user's profiles is Approved or not.
        /// </summary>
        public bool IsProfileApproved {
            get { return isProfileApproved; }
            set { isProfileApproved = value; }
        }
        
        /// <summary>
        /// Returns if a user is trusted or not.  A trusted user is one whose messages do not require
        /// any sort of moderation approval.
        /// </summary>
        public bool IsTrusted {
            get { return trusted; }
            set { trusted = value; }
        }

        /// <summary>
        /// Specifies if a user in an administrator or not.
        /// </summary>
        public bool IsAdministrator {
            get { 
                return IsInRole("Forum-Administrators");
            }                    
        }

        /// <summary>
        /// Specifies if a user in an administrator or not.
        /// </summary>
        public bool IsModerator {
            get { 
                return isModerator;
            }                    
            set {
                isModerator = value;
            }
        }

        public static bool IsInRole(string rolename) {
            return HttpContext.Current.User.IsInRole(rolename);
        }

        /// <summary>
        /// Specifies if the user wants to automatically turn on email tracking for threads that 
        /// he/she posts to.
        /// </summary>
        public bool TrackPosts {
            get { return trackPosts; }
            set { trackPosts = value; }
        }

        /// <summary>
        /// Specifies the user's timezone offset.
        /// </summary>
        public int Timezone {
            get { return timezone; }
            set {
                if (value < -12 || value > 12)
                    timezone = 0;
                else
                    timezone = value;
            }
        }

        /// <summary>
        /// Indicates the user's ViewOptions setting.  This can be set to Flat, Mixed, or Threaded.
        /// </summary>
        public ViewOptions ForumView {
            get { return this._forumView; }
            set { _forumView = value;  }
        }

    }

    public enum AvatarType {
        url,
        gif,
        jpg
    }
}
