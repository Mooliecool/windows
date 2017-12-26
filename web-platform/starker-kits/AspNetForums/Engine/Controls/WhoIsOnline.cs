/* Feature:
 * It would be really cool to have support for updating the db with what the user is currently doing.
 */
using System;
using System.Drawing;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Components;
using System.ComponentModel;
using System.IO;

namespace AspNetForums.Controls {

    // *********************************************************************
    //  WhoIsOnline
    //
    /// <summary>
    /// This server control is used to display a list of users currently online.
    /// </summary>
    // ***********************************************************************/
    public class WhoIsOnline : SkinnedForumWebControl {
        int minutesToCheckForUsersOnline = 30;
        bool displayColonForUserListing = true;
        string skinFilename = "Skin-WhoIsOnline.ascx";

        // *********************************************************************
        //  WhoIsOnline
        //
        /// <summary>
        /// Constructor
        /// </summary>
        // ***********************************************************************/
        public WhoIsOnline() : base() {

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
            System.Web.UI.WebControls.Image img;
            Label label;
            HyperLink link;
            UserCollection users;

            // Get the statistics
            Statistics siteStats = Statistics.GetSiteStatistics();

            // Get the users for the past n minutes
            users = Users.WhoIsOnline(Minutes);

            // Find the users image
            img = (System.Web.UI.WebControls.Image) skin.FindControl("StatsImg");
            if (null != img)
                img.ImageUrl = Globals.ApplicationVRoot + "/skins/" + base.SkinName + "/images/icon_stats.gif";

            // Find the Total Users
            label = (Label) skin.FindControl("TotalUsers");
            if (null != label)
                label.Text = siteStats.TotalUsers.ToString("n0");

            // Find the Total users online
            label = (Label) skin.FindControl("UsersOnline");
            if (null != label)
                label.Text = users.Count.ToString("n0");

            // Find the Users label
            label = (Label) skin.FindControl("Users");

            // Display users
            if (null != label) {

                if (users.Count > 0) {
                    if (DisplayColon)
                        label.Controls.Add(new LiteralControl(": "));

                    // Display all the users online
                    for (int i = 0; i < users.Count; i++) {
                        string username = ((OnlineUser)users[i]).Username;

                        link = new HyperLink();
                        link.Text = username ;
                        link.NavigateUrl = Globals.UrlUserProfile + username;
                        label.Controls.Add(link);

                        // Moderators or Admin?
                        if (((OnlineUser)users[i]).IsModerator)
                            link.CssClass = "moderatorOnlineLinkBold";
                        else if (((OnlineUser)users[i]).IsAdministrator)
                            link.CssClass = "adminOnlineLinkBold";
                        else
                            link.CssClass = "userOnlineLinkBold";

                        if (i < (users.Count -1))
                            label.Controls.Add(new LiteralControl(", "));
                    }

                } else {
                    if (DisplayColon)
                        label.Text = ".";
                }
            }

            // Anonymous users
            label = (Label) skin.FindControl("AnonymousUsers");
            if (null != label)
                label.Text = Users.GetAnonymousUsersOnline().ToString("n0");

            // Moderators
            label = (Label) skin.FindControl("Moderators");
            if (null != label) {
                int moderatorCount = 0;

                for (int i = 0; i < users.Count; i++) {
                    if ( ((OnlineUser)users[i]).IsModerator )
                        moderatorCount++;
                }

                label.Text = moderatorCount.ToString("n0");
            }

            // Moderator user list
            label = (Label) skin.FindControl("ModeratorList");
            if (null != label) {
                if (users.Count > 0) {
                    if (DisplayColon)
                        label.Controls.Add(new LiteralControl(": "));

                    // Display all the users online
                    for (int i = 0; i < users.Count; i++) {
                        if ( ((OnlineUser)users[i]).IsModerator) {
                            string username = ((OnlineUser)users[i]).Username;

                            link = new HyperLink();
                            link.Text = username ;
                            link.NavigateUrl = Globals.UrlUserProfile + username;
                            label.Controls.Add(link);

                            // Moderators or Admin?
                            if (((OnlineUser)users[i]).IsModerator)
                                link.CssClass = "moderatorOnlineLinkBold";
                            else if (((OnlineUser)users[i]).IsAdministrator)
                                link.CssClass = "adminOnlineLinkBold";
                            else
                                link.CssClass = "userOnlineLinkBold";

                            if (i < (users.Count -1))
                                label.Controls.Add(new LiteralControl(", "));
                        }
                    }

                } else {
                    if (DisplayColon)
                        label.Text = ".";
                }
            }

        }


        // *********************************************************************
        //  Minutes
        //
        /// <summary>
        /// Controls have often we poll for updates.
        /// </summary>
        // ***********************************************************************/
        public int Minutes {
            get { return minutesToCheckForUsersOnline; }
            set { minutesToCheckForUsersOnline = value; }
        }


        // *********************************************************************
        //  DisplayColon
        //
        /// <summary>
        /// Determines if the colon ':' is used when listing users
        /// </summary>
        // ***********************************************************************/
        public bool DisplayColon {
            get { return displayColonForUserListing; }
            set { displayColonForUserListing = value; }
        }
    }
}
