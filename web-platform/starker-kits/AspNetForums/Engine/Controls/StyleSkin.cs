using System;
using System.Web;
using System.Web.UI;
using AspNetForums.Components;

namespace AspNetForums.Controls {

    // *********************************************************************
    //  StyleSkin
    //
    /// <summary>
    /// Encapsulated rendering of style based on the selected skin.
    /// </summary>
    // ********************************************************************/ 
    public class StyleSkin : LiteralControl {
        string title = Globals.SiteName;
        bool displayTitle = true;

        // Controls the style applied to the site
        public StyleSkin() {

            User user = null;
            string skinName = Globals.Skin;

            // Get the user if available we'll personalize the style
            if (HttpContext.Current.Request.IsAuthenticated) {
                user = Users.GetUserInfo(HttpContext.Current.User.Identity.Name, true);
                skinName = user.Skin;
            }

            if (DisplayTitle) {

                if (user != null)
                    base.Text = "<title>" + Title + " (Logged in as: " + user.Username + ")</title>\n";
                else
                    base.Text = "<title>" + Title + "</title>\n";
            }

            // Add the style sheet
            base.Text += "    <link rel=\"stylesheet\" href=\"" + Globals.ApplicationVRoot + "/skins/" + skinName + "/style/default.css\" type=\"text/css\" />";

        }

        // Used to set the title of the page the control is rendered on
        public string Title {
            get { return title; }
            set { title = title + ": " + value; }
        }

        public bool DisplayTitle {
            get { return displayTitle; }
            set { displayTitle = value; }
        }
    }
}
