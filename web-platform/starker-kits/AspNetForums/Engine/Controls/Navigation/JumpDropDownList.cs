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

    /// <summary>
    /// This Web control displays the posts for a particular forum.  The posts are displayed in
    /// a format either specifically indicated by the programmer utilizing this Web control or by
    /// the Web visitor's Forum Display settings.  The posts shown are the posts for the forum that
    /// fall within a certain date range, which can be specified via the Forum Administration Web page.
    /// </summary>
    /// <remarks>When using this control you must set the ForumID property to the forum's posts you
    /// wish to display.  Failure to set this property will result in an Exception.</remarks>
    [
    ParseChildren(true)
    ]
    public class JumpDropDownList : WebControl, INamingContainer {


        string displayText = "Jump to: ";
        User user;

        // *********************************************************************
        //  CreateChildControls
        //
        /// <summary>
        /// This event handler adds the children controls and is resonsible
        /// for determining the template type used for the control.
        /// </summary>
        /// 
        // ********************************************************************/ 
        protected override void CreateChildControls() {
            DropDownList locations;
            Label description;

            // Do we have a user?
            user = Users.GetLoggedOnUser();

            // Create a new drop down list
            locations = new DropDownList();
            locations.AutoPostBack = true;
            locations.SelectedIndexChanged += new System.EventHandler(Location_Changed);
            locations.DataSource = Locations();
            locations.DataTextField = "Text";
            locations.DataValueField = "Value";
            locations.DataBind();

            // Create a new label
            description = new Label();
            description.CssClass = "normalTextSmallBold";
            description.Text = DisplayText + " ";

            Controls.Add(description);
            Controls.Add(locations);
        }

        // *********************************************************************
        //  Location_Changed
        //
        /// <summary>
        /// User wants to jump to a new location
        /// </summary>
        /// 
        // ********************************************************************/ 
        private void Location_Changed(Object sender, EventArgs e) {

            DropDownList jumpLocation = (DropDownList) sender;
            string jumpValue = jumpLocation.SelectedItem.Value;

            if (jumpValue.StartsWith("/")) {
                Page.Response.Redirect(jumpValue);
            } else if (jumpValue.StartsWith("g")) {
                int forumGroupId = 0;
                forumGroupId = Convert.ToInt32(jumpValue.Substring(jumpValue.IndexOf("-") + 1));
                Page.Response.Redirect(Globals.UrlShowForumGroup + forumGroupId);
            } else if (jumpValue.StartsWith("f")) {
                int forumId = 0;
                forumId = Convert.ToInt32(jumpValue.Substring(jumpValue.IndexOf("-") + 1));
                Page.Response.Redirect(Globals.UrlShowForum + forumId);
            } else {
                Page.Response.Redirect(Globals.ApplicationVRoot);
            }

            // End the response
            Page.Response.End();
        }

        // *********************************************************************
        //  Locations
        //
        /// <summary>
        /// Populates the locations dropdown with various location options
        /// </summary>
        /// 
        // ********************************************************************/ 
        private ListItemCollection Locations() {
            Forums forums = new Forums();
            ListItemCollection options = new ListItemCollection();

            options.Add(new ListItem("Please select"));
            options.Add(new ListItem("---------------------"));
            options.Add(new ListItem("Forums Home", Globals.UrlHome));
            options.Add(new ListItem("Search Forums", Globals.UrlSearch));
            options.Add(new ListItem("Member List", Globals.UrlShowAllUsers));
            if (user != null)
                options.Add(new ListItem("Edit My Profile", Globals.UrlEditUserProfile));
            options.Add(new ListItem("---------------------"));

            if (user != null)
                forums.ForumListItemCollection(user.Username, Forums.ForumListStyle.Nested, options);
            else
                forums.ForumListItemCollection(null, Forums.ForumListStyle.Nested, options);

            return options;
        }

        // *********************************************************************
        //  DisplayText
        //
        /// <summary>
        /// Text preceding the drop down list of options
        /// </summary>
        /// 
        // ********************************************************************/ 
        public string DisplayText {
            get { return displayText;  }
            set { displayText = value; }
        }

    }
}