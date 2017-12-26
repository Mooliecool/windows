using System;
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
    /// This Web control displays the textboxes needed to create a new user account.
    /// The user can enter their username and email, and a warning message is displayed if
    /// either the username or email message is already taken.  Once the user enters an available
    /// username and password, they will be sent a confirmation email with their password.
    /// </summary>
    [
        ParseChildren(true)
    ]
    public class CreateUser : SkinnedForumWebControl {

        const bool defaultInstantSignup = true;	// instant signup is on by default
        bool redirect = true;
        string skinFilename = "Skin-CreateNewAccount.ascx";
        TextBox username;
        TextBox password;
        TextBox emailAddress;
        RequiredFieldValidator usernameValidator;
        RequiredFieldValidator emailValidator;

        // *********************************************************************
        //  CreateUser
        //
        /// <summary>
        /// Constructor
        /// </summary>
        // ***********************************************************************/
        public CreateUser() : base() {

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

            // Is Instant sign-up enabled?
            if (InstantSignup)
                skin.FindControl("InstantSignUp").Visible = true;
            else
                skin.FindControl("InstantSignUp").Visible = false;

            // Find the button on the user control and wire-it up to the CreateUser_Click event in this class
            Button button = (Button) skin.FindControl("CreateAccount");
            button.Text = "Create User";
            button.Click += new System.EventHandler(CreateUser_Click);

            // Find the other controls
            username = (TextBox) skin.FindControl("Username");
            password = (TextBox) skin.FindControl("Password");
            emailAddress = (TextBox) skin.FindControl("Email");
            usernameValidator = (RequiredFieldValidator) skin.FindControl("usernameValidator");
            emailValidator = (RequiredFieldValidator) skin.FindControl("emailValidator");
 
        }

        // *********************************************************************
        //  CreateUser_Click
        //
        /// <summary>
        /// This event handler fires when the submit button is clicked and the
        /// form posts back.  It is responsible for updating the user's info
        /// </summary>
        //
        // ********************************************************************/
        private void CreateUser_Click(Object sender, EventArgs e) {

            // make sure the page is valid
            if (!Page.IsValid) 
                return;

            // try to create the new user account
            User user = new User();
            user.Username = username.Text;			
            user.Email = emailAddress.Text;

            if (InstantSignup)
                user.Password = password.Text;
            else
                user.Password = String.Empty;

            CreateUserStatus status = Users.CreateNewUser(user, true);

            // Determine if the account was created successfully
            switch (status) {

                // Username already exists!
                case CreateUserStatus.DuplicateUsername:
                    usernameValidator.Text = "A user with this username already exists.";
                    usernameValidator.IsValid = false;
                    break;

                // Email already exists!
                case CreateUserStatus.DuplicateEmailAddress:
                    emailValidator.Text = "A user with this email address already exists.";
                    emailValidator.IsValid = false;
                    break;

                // Unknown failure has occurred!
                case CreateUserStatus.UnknownFailure:
                    usernameValidator.Text = "An unexpected failure has occurred.  Please notify the Web site administrator of this error." + Globals.HtmlNewLine + Globals.HtmlNewLine;
                    usernameValidator.IsValid = false;
                    break;

                // Everything went off fine, good
                case CreateUserStatus.Created:

                    if (!redirect) {
                        usernameValidator.Text = "User created";
                        usernameValidator.IsValid = false;

                    } else {
                        // everything looks good, redirect to the correct message page
                        if (this.InstantSignup)
                            Context.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.NewAccountCreated));
                        else
                            Context.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.NewAccountCreated));
                    }
                    break;
            }			
        }

        // *********************************************************************
        //  InstantSignup
        //
        /// <summary>
        /// Specifies whether the user should be able to instantly create an account.  If this
        /// value is True, the user is prompted for a username/email/and password.  Upon selecting
        /// a unique email and username, the user account is created with the specified password.
        /// If InstantSignup is false, a random password is assigned to the user, and they are emailed
        /// this password.
        /// </summary>
        //
        // ********************************************************************/
        public bool InstantSignup {
            get {
                if (ViewState["instantSignup"] == null) 
                    return defaultInstantSignup;

                return (bool) ViewState["instantSignup"];
            }

            set { 
                ViewState["instantSignup"] = value; 
            }
        }

        // *********************************************************************
        //  Redirect
        //
        /// <summary>
        /// Optionally don't perform redirect when creating a new user
        /// </summary>
        // ***********************************************************************/
        public bool Redirect {
            get {
                return redirect;
            }
            set {
                redirect = value;
            }
        }
    }
}