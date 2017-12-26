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
using System.Web.Security;

namespace AspNetForums.Controls {

    // *********************************************************************
    //  MyForums
    //
    /// <summary>
    /// A helpful control that simply displays common threads the user has 
    /// participated in or threads that the user is tracking.
    /// </summary>
    // ***********************************************************************/
    public class MyForums : SkinnedForumWebControl {

        string skinFilename = "Skin-MyForums.ascx";
        ThreadList threadTracking;
        ThreadList participatedThreads;
        HyperLink search;

        // *********************************************************************
        //  Login
        //
        /// <summary>
        /// Constructor
        /// </summary>
        // ***********************************************************************/
        public MyForums() : base() {

            // Assign a default template name
            if (SkinFilename == null)
                SkinFilename = skinFilename;

        }

        // *********************************************************************
        //  Initializeskin
        //
        /// <summary>
        /// Initialize the control template and populate the control with values
        /// </summary>
        // ***********************************************************************/
        override protected void InitializeSkin(Control skin) {

            // Find the thread tracking thread list control
            threadTracking = (ThreadList) skin.FindControl("ThreadTracking");
            if (null != threadTracking) {
                ThreadCollection trackedThreads = Threads.GetThreadsUserIsTracking(ForumUser.Username);

                if (trackedThreads.Count > 0) {
                    threadTracking.DataSource = trackedThreads;
                    threadTracking.DataBind();
                } else {
                    threadTracking.Visible = false;
                    Label label = (Label) skin.FindControl("NoTrackedThreads");
                    if (null != label) {
                        label.Visible = true;
                    }

                }
            }

            // Find the participated threads control
            participatedThreads = (ThreadList) skin.FindControl("ParticipatedThreads");
            if (null != participatedThreads) {
                ThreadCollection participatedInThreads = Threads.GetThreadsUserMostRecentlyParticipatedIn(ForumUser.Username);

                if (participatedInThreads.Count > 0) {
                    participatedThreads.DataSource = participatedInThreads;
                    participatedThreads.DataBind();
                } else {
                    participatedThreads.Visible = false;
                    Label label = (Label) skin.FindControl("NoParticipatedThreads");
                    if (null != label) {
                        label.Visible = true;
                    }
                }
            }


            // Find the search link
            search = (HyperLink) skin.FindControl("FindMorePosts");
            if (null != search) {
                search.NavigateUrl = Globals.UrlSearchForPostsByUser + ForumUser.Username;
            }

        }


    }
}