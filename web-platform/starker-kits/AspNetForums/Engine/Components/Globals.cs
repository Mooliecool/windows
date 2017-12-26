using System;
using System.Collections;
using System.Collections.Specialized;
using System.Drawing;
using System.Configuration;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.IO;
using System.Xml;
using System.Web.Caching;
using System.Text.RegularExpressions;


namespace AspNetForums.Components {

    /************* DECLARE ENUMERATIONS ****************/
    /// <summary>
    /// The NextPrevMessagesPosition enumeration is used with the ForumView Web control to indicate the position of the
    /// Next/Prev Messages links.  The available options are Top, Bottom, and Both.
    /// </summary>
    public enum NextPrevMessagesPosition { 
        /// <summary>
        /// Places the Next/Prev Messages bar just at the top of the forum post listing.
        /// </summary>
        Top, 
        
        /// <summary>
        /// Places the Next/Prev Messages bar just at the bottom of the forum post listing.
        /// </summary>
        Bottom, 
        
        /// <summary>
        /// Places the Next/Prev Messages bar at the top and the bottom of the forum post listing.
        /// </summary>
        Both,
    
        /// <summary>
        /// Does not display the Next/Prev Messages bar.
        /// </summary>
        None
    }

    /// <summary>
    /// The ViewOptions enumeration determines how the posts for a particular forum are displayed.
    /// The options are NotSet, meaning the default is used; Flat; Mixed; and Threaded.
    /// </summary>
    public enum ViewOptions { 
        /// <summary>
        /// When the forum is visited by an anonymous user, their ViewOptions are NotSet.
        /// Pass this value in to have the default forum view setting used.
        /// </summary>
        NotSet = -1, 
        
        /// <summary>
        /// Specifies to display the forum in a Flat mode.
        /// </summary>
        Flat = 0, 
        
        /// <summary>
        /// Specifies to display the forum in the Mixed mode.
        /// </summary>
        Mixed = 1, 
        
        /// <summary>
        /// Specifies to display the forum in Threaded mode.
        /// </summary>
        Threaded = 2 }

    /// <summary>
    /// The CreateEditPostMode enumeration determines what mode the PostDisplay Web control assumes.
    /// The options are NewPost, ReplyToPost, and EditPost.
    /// </summary>
    public enum CreateEditPostMode { 
        /// <summary>
        /// Specifies that the user is creating a new post.
        /// </summary>
        NewPost, 
        
        /// <summary>
        /// Specifies that the user is replying to an existing post.
        /// </summary>
        ReplyToPost, 
        
        /// <summary>
        /// Specifies that a  moderator or administrator is editing an existing post.
        /// </summary>
        EditPost }

    /// <summary>
    /// The CreateEditForumMode enumeration determines what mode the CreateEditForum Web control assumes.
    /// The options are CreateForum and EditForum.
    /// </summary>
    public enum CreateEditForumMode { 
        /// <summary>
        /// Specifies that a new forum is being created.
        /// </summary>
        CreateForum, 
        
        /// <summary>
        /// Specifies that an existing forum is being edited.
        /// </summary>
        EditForum }

    /// <summary>
    ///  The CreateEditRoleMode enumeration defines what mode the CreateEditRole Web control assumes.
    ///  The options are CreateRole, and EditRole.
    /// </summary>
    public enum CreateEditRoleMode {
        /// <summary>
        ///  Specifies that a new role is being created.
        /// </summary>
        CreateRole,
        /// <summary>
        ///  Specifies that an existing role is being edited.
        /// </summary>
        EditRole
    }

    /// <summary>
    /// The DateTimeFormatEnum enumeration determines the date/time format returned by the AccountForTimezone
    /// functions.  The available options are: ShortTimeString; ShortDateString; LongTimeString;
    /// LongDateString, and CompleteDate.
    /// </summary>
    public enum DateTimeFormatEnum { ShortTimeString, ShortDateString, LongTimeString, LongDateString, CompleteDate }

    /// <summary>
    /// The ModeratedForumMode enumeration determines how the ModeratedForums Web control works.
    /// A value of ViewForForum shows all of the moderators for a particular forum; a value of ViewForUser
    /// shows all of the forums a particular user moderates.
    /// </summary>
    public enum ModeratedForumMode { 
        /// <summary>
        /// Specifies to view the list of moderators for a particular forum.
        /// </summary>
        ViewForForum,
 
        /// <summary>
        /// Specifies to view a list of moderated forums for a particular user.
        /// </summary>
        ViewForUser 
    }

    /// <summary>
    /// The UserInfoEditMode enumeration determines the role the UserInfo Web control assumes.  The
    /// available options are Edit and View.
    /// </summary>
    public enum UserInfoEditMode { 
        /// <summary>
        /// Indicates that the user is editing his or her personal user information.
        /// </summary>
        Edit, 
        
        /// <summary>
        /// Indicates that a user is viewing a user's information (not necessarily his or her own).
        /// </summary>
        View,

        /// <summary>
        /// Indicates that the user is being edited by the admin or moderator.
        /// </summary>
        Admin

    }


    /// <summary>
    /// Indicates how to apply the search query terms.
    /// </summary>
    public enum ToSearchEnum { 
        /// <summary>
        /// Specifies that the PerformSearch method should apply the search query to the post body.
        /// </summary>
        PostsSearch, 
        
        /// <summary>
        /// Specifies that the PerformSearch method should apply the search query to the post's author's 
        /// Username.
        /// </summary>
        PostsBySearch 
    }

    /// <summary>
    /// Indicates the return status for creating a new user.
    /// </summary>
    public enum CreateUserStatus { 
        /// <summary>
        /// The user was not created for some unknown reason.
        /// </summary>
        UnknownFailure, 
        
        /// <summary>
        /// The user's account was successfully created.
        /// </summary>
        Created, 
        
        /// <summary>
        /// The user's account was not created because the user's desired username is already being used.
        /// </summary>
        DuplicateUsername, 
        
        /// <summary>
        /// The user's account was not created because the user's email address is already being used.
        /// </summary>
        DuplicateEmailAddress, 
        
        /// <summary>
        /// The user's account was not created because the user's desired username did not being with an
        /// alphabetic character.
        /// </summary>
        InvalidFirstCharacter }

    /// <summary>
    /// Indicates how to interpret the search terms.
    /// </summary>
    public enum SearchWhatEnum { 
        /// <summary>
        /// Searches for all words entered into the search terms.
        /// </summary>
        SearchAllWords, 
        
        /// <summary>
        /// Searches for any word entered as search terms.
        /// </summary>
        SearchAnyWord, 
        
        /// <summary>
        /// Searches for the EXACT search phrase entered in the search terms.
        /// </summary>
        SearchExactPhrase }

    /// <summary>
    /// Returns the status of a moved post operation.
    /// </summary>
    public enum MovedPostStatus { 
        /// <summary>
        /// The post was not moved; this could happen due to the post having been already deleted or
        /// already approved by another moderator.
        /// </summary>
        NotMoved, 
        
        /// <summary>
        /// The post was moved successfully to the specified forum, but is still waiting approval, since
        /// the moderator who moved the post lacked moderation rights to the forum the post was moved to.
        /// </summary>
        MovedButNotApproved, 
        
        /// <summary>
        /// The post was moved successfully to the specified forum and approved.
        /// </summary>
        MovedAndApproved}


    /// <summary>
    /// The EmailTypeEnum enumeration determines what type of message is to be displayed
    /// </summary>
    public enum Messages {
        UnableToAdminister = 1,
        UnableToEditPost = 2,
        UnableToModerate = 3,
        DuplicatePost = 4,
        FileNotFound = 5,
        UnknownForum = 6,
        NewAccountCreated = 7,
        PostPendingModeration = 8,
        PostDoesNotExist = 9,
        PostIdParameterNotSpecified = 10,
        ProblemPosting = 11,
        UnableToViewMessage = 12,
        UserProfileUpdated = 13,
        UserDoesNotExist = 14,
        UserPasswordChangeSuccess = 15,
        UserPasswordChangeFailed = 16
    }

    /// <summary>
    /// The EmailTypeEnum enumeration determines what type of email template is used to send an email.
    /// The available options are: ForgottenPassword, ChangedPassword, NewMessagePostedToThread,
    /// NewUserAccountCreated, MessageApproved, MessageMovedAndApproved, MessageMovedAndNotApproved,
    /// MessageDeleted, and ModeratorEmailNotification.
    /// </summary>
    public enum EmailTypeEnum {
        /// <summary>
        /// Sends a user their username and password to the email address on file.
        /// </summary>
        ForgottenPassword = 1,

        /// <summary>
        /// Sends an email to the user when he changes his password.
        /// </summary>
        ChangedPassword = 2,

        /// <summary>
        /// Sends a mass emailing when a new post is added to a thread.  Those who receive the email are those
        /// who have email thread tracking turned on for the particular thread that the new post was added to.
        /// </summary>
        NewMessagePostedToThread = 3,

        /// <summary>
        /// When a user creates a new account, this email template sends their UrlShowPost information (username/password).
        /// </summary>
        NewUserAccountCreated = 4,

        /// <summary>
        /// When a user's post that was awaiting moderation is approved, they are sent this email.
        /// </summary>
        MessageApproved = 5,

        /// <summary>
        /// If a user's post is moved from one forum to another, this email indicates this fact.
        /// </summary>
        MessageMovedAndApproved = 6,

        /// <summary>
        /// If a user's post was moved to another forum but is still waiting moderator approval, this
        /// email template informs them of the situation.
        /// </summary>
        MessageMovedAndNotApproved = 7,

        /// <summary>
        /// If a user's post is deleted, this email explains why their post was deleted.
        /// </summary>
        MessageDeleted = 8,

        /// <summary>
        /// When a new post needs to be approved, those moderators of the posted-to forum who have email
        /// notification turned on are sent this email to instruct them that there is a post waiting moderation.
        /// </summary>
        ModeratorEmailNotification = 9
    }
    /***************************************************/

    public class Globals {
        // the HTML newline character
        public const String HtmlNewLine = "<br />";
        public const String _appSettingsPrefix = "AspNetForumsSettings.";

        // *********************************************************************
        //  LoadSkinnedTemplate
        //
        /// <summary>
        /// Attempts to load a template from the skin defined for the application.
        /// If no template is found, or if an error occurs, a maker is added to the
        /// cache to indicate that we won't try the code path again. Otherwise the
        /// template is added to the cache and loaded from memory.
        /// </summary>
        /// 
        // ********************************************************************/
        public static ITemplate LoadSkinnedTemplate(string virtualPathToTemplate, string templateKey, Page page) {
            ITemplate _template;
            CacheDependency fileDep;

            // Get the instance of the Cache
            Cache cache = HttpContext.Current.Cache;

            // Attempt to load template from Cache
            if ((cache[templateKey] == null) || (cache[templateKey] != typeof(FileNotFoundException))) {
                try {
                    // Create a file dependency
                    fileDep = new CacheDependency(page.Server.MapPath(virtualPathToTemplate));

                    // Load the template
                    _template = page.LoadTemplate(virtualPathToTemplate);

                    // Add to cache
                    cache.Insert(templateKey, _template, fileDep);

                } catch (FileNotFoundException fileNotFound) {

                    // Add a marker we can check for to skip this in the future
                    cache.Insert(templateKey, fileNotFound);

                    return null;
                } catch (HttpException httpException) {

                    // Add a marker we can check for to skip this in the future
                    if (httpException.ErrorCode == -2147467259)
                        cache.Insert(templateKey, new FileNotFoundException("Template not found"));
                    else
                        throw httpException;

                    return null;
                }
            } else {
                return null;
            }

            // return the template from Cache
            return (ITemplate) cache[templateKey];
        }

        public static string FormatSignature(string userSignature) {
            if ( userSignature != null && userSignature.Length > 0 )
                return "<hr size=\"1\" align=\"left\" width=\"15%\">" + Globals.FormatPostBody(userSignature);

            return null;
        }

        /// <summary>
        /// Converts the raw, database form of a post's body to an HTML-friendly formatted version.
        /// </summary>
        /// <param name="RawPostBody">The raw format of the post body text.</param>
        /// <returns>The formatted version of the raw text.</returns>
        /// <remarks>When a post is saved into a database, it is the raw text that is saved.  (The literal
        /// text entered by the user.)  However, due to the way a browser views certain text characters,
        /// we need to format this text before we display it to the user.  This method removes breaking
        /// HTML characters and applies the text transformations specified by the transformation file.</remarks>
        public static String FormatPostBody(String stringToTransform) {
            return Transforms.TransformString(stringToTransform);
        }


        /// <summary>
        /// Converts a prepared subject line back into a raw text subject line.
        /// </summary>
        /// <param name="FormattedMessageSubject">The prepared subject line.</param>
        /// <returns>A raw text subject line.</returns>
        /// <remarks>This function is only needed when editing an existing message or when replying to
        /// a message - it turns the HTML escaped characters back into their pre-escaped status.</remarks>
        public static String HtmlDecode(String FormattedMessageSubject) {       
            // strip the HTML - i.e., turn < into &lt;, > into &gt;
            return HttpContext.Current.Server.HtmlDecode(FormattedMessageSubject);
        } 

        /// <summary>
        /// Converts a prepared subject line back into a raw text subject line.
        /// </summary>
        /// <param name="FormattedMessageSubject">The prepared subject line.</param>
        /// <returns>A raw text subject line.</returns>
        /// <remarks>This function is only needed when editing an existing message or when replying to
        /// a message - it turns the HTML escaped characters back into their pre-escaped status.</remarks>
        public static String HtmlEncode(String FormattedMessageSubject) {       
            // strip the HTML - i.e., turn < into &lt;, > into &gt;
            return HttpContext.Current.Server.HtmlEncode(FormattedMessageSubject);
        } 

        /************ PROPERTY SET/GET STATEMENTS **************/
        /// <summary>
        /// Returns the default view to use for viewing the forum posts, as specified in the AspNetForumsSettings
        /// section of Web.config.
        /// </summary>
        static public int DefaultForumView {
            get {
                const int _defaultForumView = 2;
                const String _settingName = "defaultForumView";

                String _str = (String) HttpContext.Current.Cache.Get("webForums." + _settingName);
                int iValue = _defaultForumView;
                if (_str == null) {
                    // we need to get the string and place it in the cache
                    String prefix = "";
                    NameValueCollection context = (NameValueCollection)ConfigurationSettings.GetConfig("AspNetForumsSettings");
                    if (context == null) {
                        // get the appSettings context
                        prefix = Globals._appSettingsPrefix;;
                        context = (NameValueCollection)ConfigurationSettings.GetConfig("appSettings");
                    }

                    _str = context[prefix + _settingName];

                    // determine what forum view to show
                    if (_str == null)
                        // choose the default
                        iValue = _defaultForumView;
                    else
                        switch(_str.ToUpper()) {
                            case "THREADED":
                                iValue = 2;
                                break;

                            case "MIXED":
                                iValue = 1;
                                break;

                            case "FLAT":
                                iValue = 0;
                                break;

                            default:
                                iValue = _defaultForumView;
                                break;
                        }
                    
                    _str = iValue.ToString();
                    HttpContext.Current.Cache.Insert("webForums." + _settingName, _str);
                }

                return Convert.ToInt32(_str);
            }
        }


        /// <summary>
        /// Returns a boolean value indicating whether or not duplicate posts are allowed on the forum.
        /// </summary>
        static public bool AllowDuplicatePosts {
            get {
                return SafeConfigBoolean("AspNetForumsSettings", "allowDuplicatePosts", false);
            }
        }

        /// <summary>
        /// Specifies the SMTP Mail Server to use to send email information.  If no value is specified, or
        /// a value of "default" is specified, the default SMTP Mail Server is used.
        /// </summary>
        static public String SmtpServer {
            get {
                string smtpServer = SafeConfigString("AspNetForumsSettings", "smtpServer", string.Empty);

                if (smtpServer.Length == 0 || smtpServer.ToUpper() == "DEFAULT")
                    smtpServer = "";

                return smtpServer;
            }
        }

        /// <summary>
        ///  The base URL for this web site
        /// </summary>
        static public String UrlWebSite {
            get {
                return SafeConfigString("AspNetForumsSettings", "urlWebSite", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// A read-only property that determines if WebForums.NET should send automated email messages
        /// or not.  Simply, this property returns false if the smtpServer property is set to NONE, true
        /// otherwise.
        /// </summary>
        static public bool SendEmail {
            get { return SmtpServer.ToUpper() != "NONE"; }
        }
        
        /// <summary>
        /// Url path to the page implementing search features
        /// </summary>
        static public String UrlSearch {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlSearch", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Url path to the page used for new user registration
        /// </summary>
        static public String UrlRegister {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlRegister", string.Empty).Replace("^", "&");
            }
        }
        
        /// <summary>
        /// Url path to the user profile page
        /// </summary>
        static public String UrlEditUserProfile {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlEditUserProfile", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Name of the skin to be applied
        /// </summary>
        static public String Skin {
            get {
                if ( AvailableSkins.Length > 0 ) {
                    return AvailableSkins[0];
                }
                
                return "default";
            }
        }

        /// <summary>
        /// Available skins that can be used
        /// </summary>
        static public String[] AvailableSkins {
            get {
                return SafeConfigString("AspNetForumsSettings", "availableSkins", string.Empty).Split(';');
            }
        }

        /// <summary>
        /// TODO: Necessary?
        /// </summary>
        static public String ApplicationVRoot {
            get {
                if (HttpContext.Current.Request.ApplicationPath == "/")
                    return ForumsDirectory;
                else
                    return HttpContext.Current.Request.ApplicationPath;
            }
        }

        static private string ForumsDirectory {
            get {
                return SafeConfigString("AspNetForumsSettings", "forumsDirectory", string.Empty);
            }
        }
        
        /// <summary>
        /// Returns the database connection string
        /// </summary>
        static public String DatabaseConnectionString {
            get {
                return SafeConfigString("AspNetForumsSettings", "connectionString", string.Empty);
            }
        }

        /// <summary>
        /// Returns the default page size used in paging
        /// </summary>
        static public int PageSize {
            get {
                return SafeConfigNumber("AspNetForumsSettings", "defaultPageSize", 25);
            }
        }

        /// <summary>
        /// Default date format
        /// </summary>
        static public string DateFormat {
            get {
                return SafeConfigString("AspNetForumsSettings", "defaultDateFormat", string.Empty);
            }
        }

        /// <summary>
        /// Default time format
        /// </summary>
        static public string TimeFormat {
            get {
                return SafeConfigString("AspNetForumsSettings", "defaultTimeFormat", string.Empty);
            }
        }

        /// <summary>
        ///  Enables the use of full-text searches on the site.  This changes how the
        ///  search criterion is built and which options are available to users.
        /// </summary>
        static public bool EnableFullTextSearching {
            get {
                return SafeConfigBoolean("AspNetForumsSettings", "enableFullTextSearching", false);
            }
        }

        /// <summary>
        /// Returns the offset of the timezone of the database server
        /// </summary>
        static public int DBTimezone {
            get {
                return SafeConfigNumber("AspNetForumsSettings", "dbTimeZoneOffset", -5);
            }
        }
        
        /// <summary>
        /// Returns the Url to view a User's information
        /// </summary>
        static public String UrlUserProfile {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlShowUserProfile", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to the home of the forums app
        /// </summary>
        static public String UrlHome {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlHome", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to my forums
        /// </summary>
        static public String UrlMyForums {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlMyForums", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to change the user's password
        /// </summary>
        static public String UrlChangePassword {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlChangePassword", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to change the user's password
        /// </summary>
        static public String UrlForgotPassword {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlForgotPassword", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to search for all posts by a given user
        /// </summary>
        static public String UrlSearchForPostsByUser {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlSearchForPostsByUser", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to search for all posts by a given user
        /// </summary>
        static public String UrlModerationHistory {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlModerationHistory", string.Empty).Replace("^", "&");
            }
        }
        

        /// <summary>
        /// Returns the Url to search for all posts by a given user
        /// </summary>
        static public String UrlQuickSearch {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlQuickSearch", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to view all users
        /// </summary>
        static public String UrlShowAllUsers {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlShowAllUsers", string.Empty).Replace("^", "&");
            }
        }
        
        /// <summary>
        /// Returns the Url to edit an existing post from the post moderation page
        /// </summary>
        static public String UrlEditPost {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlEditPost", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to edit an existing post from the post moderation page
        /// </summary>
        static public String UrlUserEditPost {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlUserEditPost", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to edit an existing post from the post moderation page
        /// </summary>
        static public String UrlDeletePost {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlDeletePost", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to edit an existing post from the post moderation page
        /// </summary>
        static public String UrlMovePost {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlMovePost", string.Empty).Replace("^", "&");
            }
        }

        static public String UrlModerateForumPosts {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlModerateForumPosts", string.Empty).Replace("^", "&");
            }
        }

        static public String UrlModerateThread {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlModerateThread", string.Empty).Replace("^", "&");
            }
        }

        static public String UrlManageForumPosts {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlManageForumPosts", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to edit an existing post from the administration page
        /// </summary>
        static public String UrlEditPostFromAdmin {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlEditExistingPostFromAdmin", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to login to the forum site
        /// </summary>
        static public String UrlLogin {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlLogin", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to login to the forum site
        /// </summary>
        static public String UrlLogout {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlLogout", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to show a particular forum
        /// </summary>
        static public String UrlShowForum {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlShowForum", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// <summary>   
        /// Returns the Url to show a particular forum group
        /// </summary>
        static public String UrlShowForumGroup {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlShowForumGroup", string.Empty).Replace("^", "&");
            }
        }

        /// Returns the Url to show a particular post
        /// </summary>
        static public String UrlShowPost {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlShowPost", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the Url to the post moderation page
        /// </summary>
        static public String UrlModeration {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlModeration", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Returns the path to the location of the various message Web pages.  The 
        /// message pages are pages that are automatically shown at certain events, such
        /// as when a user posts a message to a moderated forum, or when a user attempts 
        /// to view a post that doesn't exist.
        /// </summary>
        static public String UrlMessage {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlMessage", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Specifies the Url to reply to a post
        /// </summary>
        static public String UrlReplyToPost {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlReplyToPost", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Indicates the Url to add a new post
        /// </summary>
        static public String UrlAddNewPost {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlAddNewPost", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Indicates the Url to edit a forum
        /// </summary>
        static public String UrlEditForum {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlEditForum", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Indicates the Url to create a new forum
        /// </summary>
        static public String UrlCreateForum {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlCreateForum", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Indicates the Url to show a forum's posts for editing and deleting purposes
        /// </summary>
        static public String UrlShowForumPostsForAdmin {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlShowForumPostsForAdmin", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// The Url to use for Admin
        /// </summary>
        static public String UrlAdmin {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlAdmin", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// The Url to use for Admins to edit users
        /// </summary>
        static public String UrlAdminEditUser {
            get {
                return SafeConfigUrl("AspNetForumsSettings", "urlAdminEditUser", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// The Url to use for Admins to remove roles
        /// </summary>
        static public String UrlAdminRemoveRole
        {
            get 
            {
                return SafeConfigUrl("AspNetForumsSettings", "urlAdminRemoveRole", string.Empty).Replace("^", "&");
            }
        }

        /// <summary>
        /// Indicates the name of the Web site.
        /// </summary>
        static public String SiteName {
            get {
                return SafeConfigString("AspNetForumsSettings", "siteName", string.Empty);
            }
        }

        /// <summary>
        /// Indicates the physical path to the transformation text file.
        /// </summary>
        static public String PhysicalPathToTransformationFile {
            get {

                if (HttpContext.Current.Cache["pathToTransformationFile"] == null) {
                    string path;
                    NameValueCollection configSettings = (NameValueCollection) ConfigurationSettings.GetConfig("AspNetForumsSettings");
                    path = Globals.ApplicationVRoot + configSettings["pathToTransformationFile"];

                    if (path.Substring(0,1) == "/")
                        // using virtual path, must convert to physical path
                        path = HttpContext.Current.Server.MapPath(path);

                    HttpContext.Current.Cache.Insert("pathToTransformationFile", path);

                    return path;
                } else {
                    return (string) HttpContext.Current.Cache["pathToTransformationFile"];
                }

            }
        }

        /// <summary>
        /// Creates a temporary password of a specified length.
        /// </summary>
        /// <param name="length">The maximum length of the temporary password to create.</param>
        /// <returns>A temporary password less than or equal to the length specified.</returns>
        public static String CreateTemporaryPassword(int length) {
            string strTempPassword = Guid.NewGuid().ToString("N");
            for(int i = 0; i < (length / 32); i++) {
                strTempPassword += Guid.NewGuid().ToString("N");
            }
            return strTempPassword.Substring(0, length);
        }

        private static string SafeConfigUrl(string configSection, string configKey, string defaultValue) {
            NameValueCollection configSettings = ConfigurationSettings.GetConfig(configSection) as NameValueCollection;
            if ( configSettings != null ) {
                string configValue = configSettings[configKey] as string;
                if ( configValue != null ) {
                    return Globals.ApplicationVRoot + configValue;
                }
            }

            return defaultValue;
        }

        private static string SafeConfigString(string configSection, string configKey, string defaultValue) {
            NameValueCollection configSettings = ConfigurationSettings.GetConfig(configSection) as NameValueCollection;
            if ( configSettings != null ) {
                string configValue = configSettings[configKey] as string;
                if ( configValue != null ) {
                    return configValue;
                }
            }

            return defaultValue;
        }

        private static int SafeConfigNumber(string configSection, string configKey, int defaultValue)
        {
            NameValueCollection configSettings = ConfigurationSettings.GetConfig(configSection) as NameValueCollection;
            if ( configSettings != null ) {
                try {
                    int configValue = Int32.Parse(configSettings[configKey]);
                    return configValue;
                } catch {}
            }

            return defaultValue;
        }

        private static bool SafeConfigBoolean(string configSection, string configKey, bool defaultValue)
        {
            NameValueCollection configSettings = ConfigurationSettings.GetConfig(configSection) as NameValueCollection;
            if ( configSettings != null ) {
                try {
                    bool configValue = bool.Parse(configSettings[configKey]);
                    return configValue;
                } catch {}
            }

            return defaultValue;
        }
    }
}