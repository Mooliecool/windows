// TODO: Remove code that display help...

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
    //  ThreadView
    //
    /// <summary>
    /// This server control is used to display top level threads. Note, a thread
    /// is a post with more information. The Thread class inherits from the Post
    /// class.
    /// </summary>
    /// 
    // ********************************************************************/
    public class ThreadView : SkinnedForumWebControl {
        protected DateTime threadMinValue = new DateTime(1753, 1, 1);
        protected DateTime threadMaxValue = new DateTime(9999, 12, 31);

        string skinFilename = "Skin-ShowForum.ascx";
        Forum forum = null;
        HyperLink forumName;
        Label forumDescription;
        LinkButton markAllRead;
        System.Web.UI.WebControls.Image newThreadTop;
        String username = null;
        TextBox search;
        Button searchButton;
        DropDownList daysToDisplay;
        CheckBox hideReadThreads;
        ThreadList threadList;
        Paging pager;
        Label noThreads;
        Label noPostsDueToFilter;

        // *********************************************************************
        //  ThreadView
        //
        /// <summary>
        /// The constructor simply checks for a ForumID value passed in via the
        /// HTTP POST or GET.
        /// properties.
        /// </summary>
        /// 
        // ********************************************************************/
        public ThreadView() {

            // Assign a default template name
            if (SkinFilename == null)
                SkinFilename = skinFilename;

        }

        // *********************************************************************
        //  OnPreRender
        //
        /// <summary>
        /// Override OnPreRender and databind
        /// </summary>
        /// 
        // ********************************************************************/ 
        protected override void OnPreRender(EventArgs e) {
            ThreadCollection threads;
            string username = null;

            if (null != ForumUser)
                username = ForumUser.Username;

            // Get the total records in the pager
            if (hideReadThreads != null)
                pager.TotalRecords = Forums.GetTotalThreadsInForum(ForumID, threadMaxValue, CalculateDateTimeFilter(), username, hideReadThreads.Checked);
            else
                pager.TotalRecords = Forums.GetTotalThreadsInForum(ForumID, threadMaxValue, CalculateDateTimeFilter(), username, false);

            // User specific data source
            if ( (ForumUser != null)  && (hideReadThreads != null)) {
                threads = Threads.GetAllThreads(ForumID, pager.PageSize, pager.PageIndex, CalculateDateTimeFilter(), username, hideReadThreads.Checked);
            } else {
                threads = Threads.GetAllThreads(ForumID, pager.PageSize, pager.PageIndex, CalculateDateTimeFilter(), null, false);
            }

            // Do we have data to display?
            if ((threads.Count == 0) && (forum.TotalThreads > 0)) {
                noPostsDueToFilter.Visible = true;
            } else if (threads.Count == 0) {
                noThreads.Visible = true;
                noPostsDueToFilter.Visible = false;
            } else {
                noThreads.Visible = false;
                noPostsDueToFilter.Visible = false;
                markAllRead.Visible = true;
                threadList.DataSource = threads;
                threadList.DataBind();
            }

        }

        // *********************************************************************
        //  Initializeskin
        //
        /// <summary>
        /// Initializes the user control loaded in CreateChildControls. Initialization
        /// consists of finding well known control names and wiring up any necessary events.
        /// </summary>
        /// 
        // ********************************************************************/ 
        protected override void InitializeSkin(Control skin) {
            // Images
            HyperLink link;

            // Ensure we have a valid forum
            try {
                forum = Forums.GetForumInfo(ForumID);
            } catch (Components.ForumNotFoundException) {
                Page.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UnknownForum));
                Page.Response.End();
            }

            // Find the label that we use when there are no threads available
            noThreads = (Label) skin.FindControl("NoThreads");
            noPostsDueToFilter = (Label) skin.FindControl("NoPostsDueToFilter");

            // Find the forum name
            forumName = (HyperLink) skin.FindControl("ForumName");
            if (forumName != null) {
                forumName.Text = forum.Name;
                forumName.NavigateUrl = Globals.UrlShowForum + ForumID;
            }

            // Find the forum Description
            forumDescription = (Label) skin.FindControl("ForumDescription");
            if (forumDescription != null)
                forumDescription.Text = forum.Description;

            // Find the thread list
            threadList = (ThreadList) skin.FindControl("ThreadList");

            // Find the link button to mark all as read
            markAllRead = (LinkButton) skin.FindControl("MarkAllRead");
            if (markAllRead != null) {
                if (username != null) {
                    markAllRead.Visible = true;
                    markAllRead.Click += new System.EventHandler(MarkAllRead_Click);
                } else {
                    markAllRead.Visible = false;
                }
            }

            // Find the search text box
            search = (TextBox) skin.FindControl("Search");

            // Find the search button
            searchButton = (Button) skin.FindControl("SearchButton");
            if (searchButton != null) {
                searchButton.Click += new System.EventHandler(Search_Click);
            }

            // This allows the user to control the total number of threads displayed
            daysToDisplay = (DropDownList) skin.FindControl("DisplayByDays");
            if (daysToDisplay != null) {
                daysToDisplay.SelectedIndexChanged += new System.EventHandler(SelectedDays_Changed);
                daysToDisplay.Items.Add(new ListItem("All Days", "0"));
                daysToDisplay.Items.Add(new ListItem("Today", "1"));
                daysToDisplay.Items.Add(new ListItem("Past 3 Days", "3"));
                daysToDisplay.Items.Add(new ListItem("Past Week", "7"));
                daysToDisplay.Items.Add(new ListItem("Past 2 Weeks", "14"));
                daysToDisplay.Items.Add(new ListItem("Past Month", "30"));
                daysToDisplay.Items.Add(new ListItem("Past 3 Months", "90"));
                daysToDisplay.Items.Add(new ListItem("Past 6 Months", "180"));
                daysToDisplay.Items.Add(new ListItem("Past Year", "360"));
                daysToDisplay.AutoPostBack = true;
            }

            // Display posts that the user has already read
            hideReadThreads = (CheckBox) skin.FindControl("HideReadThreads");
            if (hideReadThreads != null) {
                if (ForumUser != null) {

                    hideReadThreads.Checked = ForumUser.HideReadThreads;
                    hideReadThreads.AutoPostBack = true;
                    hideReadThreads.Visible = true;
                    hideReadThreads.CheckedChanged += new System.EventHandler(HideReadThreads_Changed);
                } else {
                    hideReadThreads.Visible = false;
                }
            }

            // Find the new thread button(s)
            newThreadTop = (System.Web.UI.WebControls.Image) skin.FindControl("NewThreadImageTop");
            if (newThreadTop != null)
                newThreadTop.ImageUrl = Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/newtopic.gif";

            // Set the anchor
            link = (HyperLink) skin.FindControl("NewThreadLinkTop");
            if (link != null)
                link.NavigateUrl = Globals.UrlAddNewPost + ForumID;

            link = (HyperLink) skin.FindControl("NewThreadLinkBottom");
            if (link != null)
                link.NavigateUrl = Globals.UrlAddNewPost + ForumID;

            // Find the pager
            pager = (Paging) skin.FindControl("Pager");
            // Get the total records used in the pager
            if (ForumUser != null) {
                if (!Page.IsPostBack)
                    pager.TotalRecords = Forums.GetTotalThreadsInForum(ForumID, threadMaxValue, CalculateDateTimeFilter(), username, ForumUser.HideReadThreads);
                else
                    pager.TotalRecords = Forums.GetTotalThreadsInForum(ForumID, threadMaxValue, CalculateDateTimeFilter(), username, hideReadThreads.Checked);
            } else {
                pager.TotalRecords = Forums.GetTotalThreadsInForum(ForumID, threadMaxValue, CalculateDateTimeFilter(), null, false);
            }

        }

        // *********************************************************************
        //  Search_Click
        //
        /// <summary>
        /// Handles the Click event when the search button is pressed.
        /// </summary>
        /// 
        // ********************************************************************/
        public void Search_Click(Object sender, EventArgs e) {
            Page.Response.Redirect(Globals.UrlSearch + "&searchText=" + Page.Server.HtmlEncode(search.Text) + "&forum=" + ForumID);
            Page.Response.End();
        }

        // *********************************************************************
        //  HideReadThreads_Changed
        //
        /// <summary>
        /// User checked to either hide or display read threads, refetch data.
        /// </summary>
        /// 
        // ********************************************************************/
        private void HideReadThreads_Changed(Object sender, EventArgs e) {

            // Toggle the hide read threads option
            if (ForumUser != null)
                Users.ToggleOptions(ForumUser.Username, hideReadThreads.Checked);

            // Set the total records used in the pager
            pager.PageIndex = 0;

        }

        // *********************************************************************
        //  CalculateDateTimeFilter
        //
        /// <summary>
        /// Returns a date time value based on the currently selected date time
        /// filter value.
        /// </summary>
        /// 
        // ********************************************************************/
        protected DateTime CalculateDateTimeFilter() {
            DateTime today = DateTime.Now.Date;

            if (daysToDisplay == null)
                return threadMinValue;

            int selectedRange = Convert.ToInt32(daysToDisplay.SelectedItem.Value);

            switch (selectedRange) {

                    // All Days
                case 0:
                    today = threadMinValue;
                    break;

                    // Past 3 Days
                case 3:
                    today = today.AddDays(-3);
                    break;

                    // Past Week
                case 7:
                    today = today.AddDays(-7);
                    break;

                    // Past 2 weeks
                case 14:
                    today = today.AddDays(-14);
                    break;

                    // Past Month
                case 30:
                    today = today.AddMonths(-1);
                    break;

                    // Past 3 months
                case 90:
                    today = today.AddMonths(-3);
                    break;

                    // Past 6 months
                case 180:
                    today = today.AddMonths(-6);
                    break;

                    // Past year
                case 360:
                    today = today.AddYears(-1);
                    break;

            }

            return today;
        }


        // *********************************************************************
        //  SelectedDays_Changed
        //
        /// <summary>
        /// Handle the event raised when the user filters the threads available by days
        /// </summary>
        /// 
        // ********************************************************************/
        private void SelectedDays_Changed(Object sender, EventArgs e) {

            // Set the total records used in the pager
            pager.PageIndex = 0;

        }

        // *********************************************************************
        //  NavigateForum_Changed
        //
        /// <summary>
        /// Raised when the navigation forums drop down list value changes
        /// </summary>
        /// 
        // ********************************************************************/ 
        private void NavigateForum_Changed (Object sender, EventArgs e) {

            DropDownList navForums = (DropDownList) sender;

            Page.Response.Redirect(Globals.UrlShowForum + navForums.SelectedItem.Value);

            Page.Response.End();
        }

        // *********************************************************************
        //  MarkAllRead_Click
        //
        /// <summary>
        /// User wants to mark all threads as read
        /// </summary>
        /// 
        // ********************************************************************/ 
        private void MarkAllRead_Click (Object sender, EventArgs e) {
            try {
                Forums.MarkAllThreadsRead(ForumID, username);
            } catch (Exception) {
                ; // ignore
            }

            Page.Server.Transfer(Globals.UrlShowForum + ForumID);
        }

		
    }
}