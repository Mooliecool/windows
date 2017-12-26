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
    //  SiteStats
    //
    /// <summary>
    /// This server control display statistic information for the forums.
    /// </summary>
    // ***********************************************************************/
    public class SiteStats : SkinnedForumWebControl {
        string skinFilename = "Skin-Statistics.ascx";


        // *********************************************************************
        //  SiteStats
        //
        /// <summary>
        /// Constructor
        /// </summary>
        // ***********************************************************************/
        public SiteStats() {

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
            Repeater repeater;

            // Get the statistics
            Statistics siteStats = Statistics.GetSiteStatistics();

            // Find the stats image
            img = (System.Web.UI.WebControls.Image) skin.FindControl("StatsImg");
            if (null != img)
                img.ImageUrl = Globals.ApplicationVRoot + "/skins/" + Globals.Skin + "/images/icon_stats.gif";

            // Find the total Users
            label = (Label) skin.FindControl("TotalUsers");
            if (null != label)
                label.Text = siteStats.TotalUsers.ToString("n0");

            // Find the total threads
            label = (Label) skin.FindControl("TotalThreads");
            if (null != label)
                label.Text = siteStats.TotalThreads.ToString("n0");
        
            // Find the total posts
            label = (Label) skin.FindControl("TotalPosts");
            if (null != label)
                label.Text = siteStats.TotalPosts.ToString("n0");

            // New threads in past 24 hours
            label = (Label) skin.FindControl("NewThreadsInPast24Hours");
            if (null != label)
                label.Text = siteStats.NewThreadsInPast24Hours.ToString("n0");

            // New posts in past 24 hours
            label = (Label) skin.FindControl("NewPostsInPast24Hours");
            if (null != label)
                label.Text = siteStats.NewPostsInPast24Hours.ToString("n0");

            // New users in past 24 hours
            label = (Label) skin.FindControl("NewUsersInPast24Hours");
            if (null != label)
                label.Text = siteStats.NewUsersInPast24Hours.ToString("n0");

            // Most viewed
            link = (HyperLink) skin.FindControl("MostViewedThread");
            if (null != link) {
                link.Text = siteStats.MostViewsSubject;

                // Don't link if there are no posts yet
                if (siteStats.MostViewsPostID > 0)
                    link.NavigateUrl = Globals.UrlShowPost + siteStats.MostViewsPostID;
            }

            // Most posts
            link = (HyperLink) skin.FindControl("MostActiveThread");
            if (null != link) {
                link.Text = siteStats.MostActiveSubject;

                // Don't link if there are no posts yet
                if (siteStats.MostActivePostID > 0)
                    link.NavigateUrl = Globals.UrlShowPost + siteStats.MostActivePostID;
            }

            // Most read
            link = (HyperLink) skin.FindControl("MostReadThread");
            if (null != link) {
                link.Text = siteStats.MostReadPostSubject;

                // Don't link if there are no posts yet
                if (siteStats.MostReadPostID > 0)
                    link.NavigateUrl = Globals.UrlShowPost + siteStats.MostReadPostID;
            }

            // Most active user
            repeater = (Repeater) skin.FindControl("TopUsers");
            if (null != repeater) {
                repeater.DataSource = Users.GetMostActiveUsers();
                repeater.DataBind();
            }

            // Newest user
            link = (HyperLink) skin.FindControl("NewestUser");
            if (null != link) {
                link.Text = siteStats.NewestUser;

                // Don't enable link if there are no posts yet
                if (siteStats.MostReadPostID > 0)
                    link.NavigateUrl = Globals.UrlUserProfile + siteStats.NewestUser;
            }
        }

    }
}
