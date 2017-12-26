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

namespace AspNetForums.Controls.Search {

    // *********************************************************************
    //  SearchOptions
    //
    /// <summary>
    /// This server control renders and handles the search UI for the user.
    /// </summary>
    // ***********************************************************************/
    public class SearchOptions : SkinnedForumWebControl {

        string skinFilename = "Skin-SearchOptions.ascx";
        bool allowSearchAllForums = true;
        DropDownList forumList;
        DropDownList searchType;
        DropDownList matchType;
        Button searchButton;

        // *********************************************************************
        //  SearchOptions
        //
        /// <summary>
        /// Constructor
        /// </summary>
        // ***********************************************************************/
        public SearchOptions() : base() {

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

            // Find the forum list control
            forumList = (DropDownList) skin.FindControl("forumList");
            if (null != forumList) {

                forumList.DataSource = Forums.GetAllForums();
                forumList.DataTextField = "Name";
                forumList.DataValueField = "ForumID";
                forumList.DataBind();

                // Add the "All Forums" option if the user is allowed to search all forums
                if (AllowSearchAllForums)
                    forumList.Items.Insert(0, new ListItem("All Forums", "-1"));
            }

            // What are we searching for posts or users?
            searchType = (DropDownList) skin.FindControl("SearchType");
            if (null != searchType) {
                searchType.Items.Add(new ListItem("Posts", "0"));
                searchType.Items.Add(new ListItem("Posted By", "1"));
            }

            matchType = (DropDownList) skin.FindControl("MatchType");
            if (null != matchType) {
                matchType.Items.Add(new ListItem("Match All Words", "0"));
                matchType.Items.Add(new ListItem("Match Any Words", "1"));
                matchType.Items.Add(new ListItem("Match Exact Phrase", "2"));	
            }

            searchButton = (Button) skin.FindControl("Search_Button");
            if (null != searchButton) {
                searchButton.Click += new EventHandler(SearchButton_Click);
            }

        }


        
        // *********************************************************************
        //  SearchButton_Click
        //
        /// <summary>
        /// This event handler is called when the user clicks on the submit button,
        /// firing off the search.  It resets the DataGrid's CurrentPageIndex to 0
        /// and Displays the Search Results.
        /// </summary>
        // ***********************************************************************/
        private void SearchButton_Click(Object sender, EventArgs e) {
            
            // Exit if the page is invalid
            if (!Page.IsValid) 
                return;

        }


        // *********************************************************************
        //  AllowSearchAllForums
        //
        /// <summary>
        /// Does the control allow the searching of all forums
        /// </summary>
        // ***********************************************************************/
        public bool AllowSearchAllForums {
            get { return allowSearchAllForums; }
            set { allowSearchAllForums = value; }
        }
    }
}