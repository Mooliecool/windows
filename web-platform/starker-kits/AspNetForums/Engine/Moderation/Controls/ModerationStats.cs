using System;
using System.Drawing;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Controls;
using AspNetForums.Components;
using System.ComponentModel;
using System.IO;

namespace AspNetForums.Controls.Moderation {

    // *********************************************************************
    //  ModerationStats
    //
    /// <summary>
    /// This server control provides statistics about moderators and moderator
    /// actions within the forums.
    /// </summary>
    // ********************************************************************/ 
    public class ModerationStats : SkinnedForumWebControl {
        string skinFilename = "Moderation/Skin-Statistics.ascx";

        // *********************************************************************
        //  ModerationStats
        //
        /// <summary>
        /// Constructor
        /// </summary>
        // ***********************************************************************/
        public ModerationStats() {

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
            Label label;
            Repeater repeater;

            // Get the statistics
            Statistics siteStats = Statistics.GetSiteStatistics();


            // Most active moderator
            repeater = (Repeater) skin.FindControl("TopModerators");
            if (null != repeater) {
                repeater.DataSource = Moderate.GetMostActiveModerators();
                repeater.DataBind();
            }

            // Moderator actions
            repeater = (Repeater) skin.FindControl("ModerationAction");
            if (null != repeater) {
                repeater.DataSource = Moderate.GetModerationAuditSummary();
                repeater.DataBind();
            }

            // Total Moderators
            label = (Label) skin.FindControl("TotalModerators");
            if (null != label)
                label.Text = siteStats.TotalModerators.ToString("n0");

            // Total Moderated Posts
            label = (Label) skin.FindControl("TotalModeratedPosts");
            if (null != label)
                label.Text = siteStats.TotalModeratedPosts.ToString("n0");

        }

    }
}
