using System;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.Caching;
using System.Web.UI.Design;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Components;
using System.ComponentModel;
using System.ComponentModel.Design;

namespace AspNetForums.Controls {

    // *********************************************************************
    //  NavigationMenu
    //
    /// <summary>
    /// This control renders a navigation menu used to navigate the hierarchy
    /// of the Discussion Board. It's links are generated from paths named
    /// in the web.config file.
    /// </summary>
    // ********************************************************************/ 
    public class NavigationMenu : SkinnedForumWebControl {
        string skinFilename = "Skin-Navigation.ascx";
        string menuItemTextLogin = "Login &nbsp;";
        string menuItemTextLogout = "Logout &nbsp;";
        string menuItemTextAdmin = "Admin &nbsp;";
        string menuItemTextModeration = "Moderation &nbsp;";
        string menuItemTextMyForums = "MyForums &nbsp;";
        string menuItemTextSearch = "Search &nbsp;";
        string menuItemTextRegister = "Register &nbsp;";
        string menuItemTextProfile = "Profile &nbsp;";
        string menuItemTextHome = "Home &nbsp;";
        string menuItemTextMember = "Member List &nbsp;";
        bool displayTitle = true; // by default we display the title

        // *********************************************************************
        //  NavigationMenu
        //
        /// <summary>
        /// Constructor
        /// </summary>
        // ***********************************************************************/
        public NavigationMenu() : base() {
            if (SkinFilename == null)
                SkinFilename = skinFilename;
        }

        // *********************************************************************
        //  InitializeSkin
        //
        /// <summary>
        /// Initialize the control template and populate the control with values
        /// </summary>
        // ***********************************************************************/
        override protected void InitializeSkin(Control skin) {
            HyperLink link;
            HyperLink loginMenu;
            HyperLink profileMenu;
            HyperLink searchMenu;
            HyperLink memberListMenu;
            HyperLink registerMenu;
            HyperLink homeMenu;
            HyperLink adminMenu;
            HyperLink moderationMenu;
            HyperLink myForumsMenu;
            Image image;

            // Find the Home Hyperlink
            link = (HyperLink) skin.FindControl("Home");
            if (DisplayTitle) {
                if (link != null) {
                    link.NavigateUrl = Globals.UrlHome;

                    // Find the home image
                    image = (Image) skin.FindControl("HomeImage");
                    if (image != null)
                        image.ImageUrl = Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/title.gif";
                }
            } else {
                link.Visible = false;
            }


            // Find the controls we need
            loginMenu = (HyperLink) skin.FindControl("LoginMenu");
            profileMenu = (HyperLink) skin.FindControl("ProfileMenu");
            searchMenu = (HyperLink) skin.FindControl("SearchMenu");
            memberListMenu = (HyperLink) skin.FindControl("MemberListMenu");
            registerMenu = (HyperLink) skin.FindControl("RegisterMenu");
            homeMenu = (HyperLink) skin.FindControl("HomeMenu");
            adminMenu = (HyperLink) skin.FindControl("AdminMenu");
            moderationMenu = (HyperLink) skin.FindControl("ModerationMenu");
            myForumsMenu = (HyperLink) skin.FindControl("MyForumsMenu");

            // Search
            searchMenu.Visible = true;
            searchMenu.NavigateUrl = Globals.UrlSearch;
            if (UseIcons) {
                searchMenu.Text = "<img src=\"" + Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/icon_mini_search.gif" + "\" border=\"0\">";
             }
            searchMenu.Text += MenuTextForSearch;

            // Is the user logged on?
            loginMenu.Visible = true;
            if (UseIcons) {
                loginMenu.Text = "<img src=\"" + Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/icon_mini_login.gif" + "\" border=\"0\">";
            }
            if (Context.Request.IsAuthenticated) {
                loginMenu.NavigateUrl = Globals.UrlLogout;
                loginMenu.Text += MenuTextForLogout;
            } else {
                loginMenu.NavigateUrl = Globals.UrlLogin;
                loginMenu.Text+= MenuTextForLogin;
            }

            // Display the user's profile
            if (Context.Request.IsAuthenticated) {
                profileMenu.Visible = true;
                profileMenu.NavigateUrl = Globals.UrlEditUserProfile;
                if (UseIcons) {
                    profileMenu.Text = "<img src=\"" + Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/icon_mini_profile.gif" + "\" border=\"0\">";
                }
                profileMenu.Text += MenuTextForProfile;
            }

            // Display My Forums
            if (Context.Request.IsAuthenticated) {
                myForumsMenu.Visible = true;
                myForumsMenu.NavigateUrl = Globals.UrlMyForums;
                if (UseIcons) {
                    myForumsMenu.Text = "<img src=\"" + Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/icon_mini_myforums.gif" + "\" border=\"0\">";
                }
                myForumsMenu.Text += MenuTextForMyForums;
            }

            // Register
            if (!Context.Request.IsAuthenticated) {
                registerMenu.Visible = true;
                registerMenu.NavigateUrl = Globals.UrlRegister;
                if (UseIcons) {
                    registerMenu.Text = "<img src=\"" + Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/icon_mini_register.gif" + "\" border=\"0\">";
                }
                registerMenu.Text += MenuTextForRegister;
            }

            // Home
            homeMenu.Visible = true;
            homeMenu.NavigateUrl = Globals.UrlHome;
            if (UseIcons) {
                homeMenu.Text = "<img src=\"" + Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/icon_mini_home.gif" + "\" border=\"0\">";
            }
            homeMenu.Text += MenuTextForHome;


            // Members
            memberListMenu.Visible = true;
            memberListMenu.NavigateUrl = Globals.UrlShowAllUsers;
            if (UseIcons) {
                memberListMenu.Text = "<img src=\"" + Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/icon_mini_memberlist.gif" + "\" border=\"0\">";
            }
            memberListMenu.Text += MenuTextForMembers;

            // Moderator?
            if ((null != ForumUser) && (ForumUser.IsModerator)) {
                moderationMenu.Visible = true;

                if (UseIcons) {
                    moderationMenu.Text = "<img src=\"" + Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/icon_mini_moderate.gif" + "\" border=\"0\">";
                }

                // Moderation details
                moderationMenu.Text += MenuTextForModeration;
                moderationMenu.NavigateUrl = Globals.UrlModeration;
            }

            // Admin?
            if ((null != ForumUser) && (ForumUser.IsAdministrator)) {
                adminMenu.Visible = true;

                if (UseIcons) {
                    adminMenu.Text = "<img src=\"" + Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/icon_mini_admin.gif" + "\" border=\"0\">";
                }

                // Admin
                adminMenu.Text += MenuTextForAdmin;
                adminMenu.NavigateUrl = Globals.UrlAdmin;
            }
        }

        // *********************************************************************
        //  Controls
        //
        /// <summary>
        /// Design time hack to get the control to render.
        /// </summary>
        /// 
        // ********************************************************************/ 
        public override ControlCollection Controls {
			get {
				EnsureChildControls();
				return base.Controls;
			}
		}

        public string MenuTextForLogin {
            get { return menuItemTextLogin; }
            set { menuItemTextLogin = value; }
        }

        public string MenuTextForLogout {
            get { return menuItemTextLogout; }
            set { menuItemTextLogout = value; }
        }

        public string MenuTextForSearch {
            get { return menuItemTextSearch; }
            set { menuItemTextSearch = value; }
        }

        public string MenuTextForRegister {
            get { return menuItemTextRegister; }
            set { menuItemTextRegister = value; }
        }

        public string MenuTextForMembers {
            get { return menuItemTextMember; }
            set { menuItemTextMember = value; }
        }

        public string MenuTextForProfile {
            get { return menuItemTextProfile; }
            set { menuItemTextProfile = value; }
        }
        public string MenuTextForHome {
            get { return menuItemTextHome; }
            set { menuItemTextHome = value; }
        }

        public string MenuTextForAdmin {
            get { return menuItemTextAdmin; }
            set { menuItemTextAdmin = value; }
        }

        public string MenuTextForModeration {
            get { return menuItemTextModeration; }
            set { menuItemTextModeration = value; }
        }

        public string MenuTextForMyForums {
            get { return menuItemTextMyForums; }
            set { menuItemTextMyForums = value; }
        }

        public bool UseIcons {
            get { 
                if (ViewState["useIcons"] == null)
                    return true;

                return (bool) ViewState["useIcons"];
            }

            set { 
                ViewState["useIcons"] = value;
            }
        }

        public bool DisplayTitle {
            get {
                return displayTitle;
            }
            set {
                displayTitle = value;
            }
        }
    }
}