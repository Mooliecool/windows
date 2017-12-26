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
    //  Login
    //
    /// <summary>
    /// This server control renders and handles the login UI for the user.
    /// </summary>
    // ***********************************************************************/
    [
    ParseChildren(true)
    ]
    public class Login : SkinnedForumWebControl {

        string skinFilename = "Skin-Login.ascx";
        TextBox username;
        TextBox password;
        Button loginButton;
        CheckBox autoLogin;

        // *********************************************************************
        //  Login
        //
        /// <summary>
        /// Constructor
        /// </summary>
        // ***********************************************************************/
        public Login() : base() {

            // Assign a default template name
            if (SkinFilename == null)
                SkinFilename = skinFilename;

        }

        // *********************************************************************
        //  CreateChildControls
        //
        /// <summary>
        /// This event handler adds the children controls.
        /// </summary>
        // ***********************************************************************/
        protected override void CreateChildControls() {

            // If the user is already authenticated we have no work to do
            if (Page.Request.IsAuthenticated)
                return;

            base.CreateChildControls();
        }

        // *********************************************************************
        //  Initializeskin
        //
        /// <summary>
        /// Initialize the control template and populate the control with values
        /// </summary>
        // ***********************************************************************/
        override protected void InitializeSkin(Control skin) {

            // Find the username control
            username = (TextBox) skin.FindControl("username");

            // Find the password control
            password = (TextBox) skin.FindControl("password");

            // Find the login button
            loginButton = (Button) skin.FindControl("loginButton");
            loginButton.Click += new System.EventHandler(LoginButton_Click);

            // Find the autologin checkbox
            autoLogin = (CheckBox) skin.FindControl("autoLogin");
        }


        // *********************************************************************
        //  LoginButton_Click
        //
        /// <summary>
        /// Event handler to handle the login button click event
        /// </summary>
        // ***********************************************************************/
        public void LoginButton_Click(Object sender, EventArgs e) {
            User userToLogin = new User();
            string redirectUrl = null;

            userToLogin.Username = username.Text;
            userToLogin.Password = password.Text;

            if (Users.ValidUser(userToLogin)) {
                FormsAuthentication.SetAuthCookie(userToLogin.Username, autoLogin.Checked);

                redirectUrl = Page.Request.QueryString["ReturnUrl"];

                if (redirectUrl != null) {
                    Page.Response.Redirect(redirectUrl);
                    Page.Response.End();
                } else {
                    Page.Response.Redirect(Globals.UrlHome);
                    Page.Response.End();
                }
            }
        }
    }
}