using System;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums.Components;

namespace AspNetForums.Controls {

    // *********************************************************************
    //  StyleSkin
    //
    /// <summary>
    /// Encapsulated rendering of style based on the selected skin.
    /// </summary>
    // ********************************************************************/ 
    public class CurrentTime : Control {

        public CurrentTime() {
            Label label;

            // Add display text
            label = new Label();
            label.CssClass = "normalTextSmallBold";
            label.Text = "Current time: ";
            Controls.Add(label);

            // Add formatted time
            label = new Label();
            label.CssClass = "normalTextSmall";
            if (HttpContext.Current.Request.IsAuthenticated) {

                // Get the current user
                User user = Users.GetLoggedOnUser();

                // Adjust the current time
                label.Text = DateTime.Now.AddHours(user.Timezone - Globals.DBTimezone).ToString("MMM d, h:mm tt");

            } else {

                label.Text = DateTime.Now.ToString("MMM d, h:mm tt");
            }

            Controls.Add(label);

        }
    }
}
