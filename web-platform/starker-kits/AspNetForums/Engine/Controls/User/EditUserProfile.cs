using System;
using System.Drawing;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using AspNetForums;
using AspNetForums.Controls.Moderation;
using AspNetForums.Components;
using System.ComponentModel;
using System.IO;

namespace AspNetForums.Controls {
    public class EditUserProfile : SkinnedForumWebControl {
        private UserInfoEditMode adminMode = UserInfoEditMode.Edit;
        private User user = null;
        string skinFilename = "Skin-EditUserInfo.ascx";
        bool requirePasswordForUpdate =  false;

        // *********************************************************************
        //  EditUserInfo
        //
        /// <summary>
        /// Constructor
        /// </summary>
        /// 
        // ********************************************************************/
        public EditUserProfile() {
            // Set the default skin
            if (SkinFilename == null)
                SkinFilename = skinFilename;
        }
	
        
        // *********************************************************************
        //  Initializeskin
        //
        /// <summary>
        /// This method populates the user control used to edit a user's information
        /// </summary>
        /// <param name="control">Instance of the user control to populate</param>
        /// 
        // ***********************************************************************/
        override protected void InitializeSkin(Control skin) {
            Label label;
            TextBox textbox;
            DropDownList dropdownlist;
            CheckBox checkbox;
            Button submit;
            System.Web.UI.WebControls.Image image;

            // Set the name
            label = (Label) skin.FindControl("Username");
            label.Text = EditUser.Username;

            // Set the email address
            textbox = (TextBox) skin.FindControl("Email");
            textbox.Text = EditUser.Email;

            // Set the time zone
            dropdownlist = (DropDownList) skin.FindControl("Timezone");
            dropdownlist.Items.FindByValue(EditUser.Timezone.ToString()).Selected = true;

            // Set the date format
            dropdownlist = (DropDownList) skin.FindControl("DateFormat");
            try {
                dropdownlist.Items.FindByValue(EditUser.DateFormat).Selected = true;
            }
            catch (Exception)
            {
                ; // unknown date format, don't set
            }

            // Email tracking option
            checkbox = (CheckBox) skin.FindControl("EmailTracking");
            if (checkbox != null)
                checkbox.Checked = EditUser.TrackPosts;

            // Hide read threads
            checkbox = (CheckBox) skin.FindControl("UnreadThreadsOnly");
            checkbox.Checked = EditUser.HideReadThreads;

            // Occupation
            textbox = (TextBox) skin.FindControl("Occupation");
            if (textbox != null)
                textbox.Text = EditUser.Occupation;

            // Location
            textbox = (TextBox) skin.FindControl("Location");
            if (textbox != null)
                textbox.Text = EditUser.Location;

            // Interests
            textbox = (TextBox) skin.FindControl("Interests");
            if (textbox != null)
                textbox.Text = EditUser.Interests;

            // MsnIm
            textbox = (TextBox) skin.FindControl("MsnIm");
            if (textbox != null)
                textbox.Text = EditUser.MsnIM;

            // YahooIm
            textbox = (TextBox) skin.FindControl("YahooIm");
            if (textbox != null)
                textbox.Text = EditUser.YahooIM;

            // AolIm
            textbox = (TextBox) skin.FindControl("AolIm");
            if (textbox != null)
                textbox.Text = EditUser.AolIM;

            // ICQ
            textbox = (TextBox) skin.FindControl("ICQ");
            if (textbox != null)
                textbox.Text = EditUser.IcqIM;

            // FakeEmail
            textbox = (TextBox) skin.FindControl("FakeEmail");
            textbox.Text = EditUser.PublicEmail;

            // WebSite
            textbox = (TextBox) skin.FindControl("Website");
            if (textbox != null)
                textbox.Text = EditUser.Url;

            // Signature
            textbox = (TextBox) skin.FindControl("Signature");
            if (textbox != null)
                textbox.Text = EditUser.Signature;

            // Style
            dropdownlist = (DropDownList) skin.FindControl("SiteStyle");
            if (dropdownlist != null) {
                foreach (String styleOption in Globals.AvailableSkins) {
                    dropdownlist.Items.Add(styleOption);
                }
            }


            // Attempt to apply the user's style ... but it might not exist
            try {
                dropdownlist.Items.FindByText(EditUser.Skin).Selected = true;
            } 
            catch (Exception)
            {
                ; // Type not found
            }

            // Post view order
            dropdownlist = (DropDownList) skin.FindControl("PostViewOrder");
            dropdownlist.Items.Add(new ListItem("Oldest first", "0"));
            dropdownlist.Items.Add(new ListItem("Newest first", "1"));
            dropdownlist.Items.FindByValue(Convert.ToInt32(EditUser.ShowPostsAscending).ToString()).Selected = true;

            // Has Avatar
            Control hasAvatar = skin.FindControl("HasIcon");

            if ((EditUser.HasAvatar) && (hasAvatar != null)) {
                image = (System.Web.UI.WebControls.Image) skin.FindControl("CurrentIcon");
                image.ImageUrl = EditUser.AvatarUrl;
                hasAvatar.Visible = true;

                ((CheckBox) skin.FindControl("ShowIcon")).Checked = EditUser.ShowAvatar;
            }

            // Perform Role Control initialization
            UpdateRoleControls(skin);

            // Are we in administration mode?
            if (AdminMode == UserInfoEditMode.Admin)
            {
                ((Control) skin.FindControl("Administration")).Visible = true;

                // Is the user's profile approved?
                checkbox = (CheckBox) skin.FindControl("ProfileApproved");
                checkbox.Checked = EditUser.IsProfileApproved;

                // Is the user banned
                checkbox = (CheckBox) skin.FindControl("Banned");
                checkbox.Checked = !EditUser.IsApproved;

                // Is the user trusted
                checkbox = (CheckBox) skin.FindControl("Trusted");
                checkbox.Checked = EditUser.IsTrusted;

                // *** Moderation
                ((Control) skin.FindControl("Moderation")).Visible = true;

                // Can the user globally moderate
                ModeratedForums moderatedForums = (ModeratedForums) skin.FindControl("ModeratedForums");
                moderatedForums.Username = EditUser.Username;
            }

            // Do we require a password for doing an update?
            if (!RequirePasswordForUpdate) {
                // Don't ask for a password when we update
                ((Control) skin.FindControl("PasswordRequired")).Visible = false;

                // Disable the validator
                ((RequiredFieldValidator) skin.FindControl("ValidatePassword")).Enabled = false;
            }

            // Wire-up the button
            submit = (Button) skin.FindControl("Submit");
            submit.Click += new System.EventHandler(UpdateUserInfo_ButtonClick);
        }

        // *********************************************************************
        //  Roles_Command
        //
        /// <summary>
        /// This event is raised whenever the administrator works with the role
        /// based security model.  There isn't any pre-caching or confirmation
        /// when working with role based security, and all changes to the database
        /// occur immediately.
        /// </summary>
        /// 
        // ********************************************************************/
        private void Roles_Command(object sender, DataGridCommandEventArgs e)
        {
            string userName = EditUser.Username;
            string action = e.CommandName;
            string role = e.CommandArgument.ToString();

            switch(action)
            {
                case "AddRole":
                    UserRoles.AddUserToRole(userName, role);
                    UpdateRoleControls(((Control) sender).Parent);
                    break;
                case "RemoveRole":
                    UserRoles.RemoveUserFromRole(userName, role);
                    UpdateRoleControls(((Control) sender).Parent);
                    break;
            }
        }

        // *********************************************************************
        //  UpdateRoleControls
        //
        /// <summary>
        /// Special helper method for update the role based security lists
        /// whenever a user action occurs.  This could be the user being added
        /// to a new security role, or the user being removed from a security
        /// role.
        /// </summary>
        /// 
        // ********************************************************************/
        private void UpdateRoleControls(Control skin)
        {
            if ( AdminMode == UserInfoEditMode.Admin )
            {
                // Show the role-based security information
                Control roleBasedSecurity = skin.FindControl("RoleBasedSecurity");
                if ( roleBasedSecurity != null )
                {
                    roleBasedSecurity.Visible = true;
                }
                else
                {
                    return;
                }

                // Perform role based security work
                string[] userRoles = UserRoles.GetUserRoles(EditUser.Username);
                string[] roles = UserRoles.GetAllRoles();

                DataGrid activeRoles = (DataGrid) skin.FindControl("ActiveRoles");
                if ( userRoles.Length > 0 )
                {
                    activeRoles.DataSource = userRoles;
                    activeRoles.DataBind();
                    activeRoles.ItemCommand += new DataGridCommandEventHandler(Roles_Command);
                    activeRoles.Visible = true;
                }
                else
                {
                    activeRoles.Visible = false;
                }

                DataGrid allRoles = (DataGrid) skin.FindControl("AllRoles");
                if ( roles.Length > 0 )
                {
                    allRoles.DataSource = roles;
                    allRoles.DataBind();
                    allRoles.ItemCommand += new DataGridCommandEventHandler(Roles_Command);
                    allRoles.Visible = true;
                }
                else
                {
                    allRoles.Visible = false;
                }
            }
        }

        // *********************************************************************
        //  UpdateUserInfo_ButtonClick
        //
        /// <summary>
        /// This event is raised when the user clicks the submit button in the user
        /// control loaded in the DisplayEditMode function. This event is responsible
        /// for processing the form values and writing them back to the database if
        /// necessary.
        /// </summary>
        /// 
        // ********************************************************************/
        private void UpdateUserInfo_ButtonClick(Object sender, EventArgs e) 
        {
            // Ensure the page is valid
            if (!Page.IsValid) 
                return;

            String password = null;
            Control skin;
            TextBox textbox;
            CheckBox checkbox;
            DropDownList dropdown;
            Control control;

            // Find the EditUserInformation user control
            skin = ((Control)sender).Parent;

            // First get the user's password
            if (AdminMode == UserInfoEditMode.Admin) {
                password = EditUser.Password;
            } else {
                password = ((TextBox) skin.FindControl("Password")).Text;
            }

            // Get the values from the form
            EditUser.Email = ((TextBox) skin.FindControl("Email")).Text;
            EditUser.PublicEmail = ((TextBox) skin.FindControl("FakeEmail")).Text;
            
            textbox = (TextBox) skin.FindControl("WebSite");
            if (textbox != null)
                EditUser.Url = textbox.Text;

            textbox = (TextBox) skin.FindControl("Signature");
            if (textbox != null)
                EditUser.Signature = textbox.Text;

            checkbox = (CheckBox) skin.FindControl("EmailTracking");
            if (checkbox != null)
                EditUser.TrackPosts = checkbox.Checked;
            
            EditUser.Timezone = Convert.ToInt32(((DropDownList) skin.FindControl("Timezone")).SelectedItem.Value);

            // Do we require a password to perform the update?
            if (RequirePasswordForUpdate) {
                textbox = (TextBox) skin.FindControl("Password");
                if (textbox != null)
                    EditUser.Password = textbox.Text;
            }
            
            textbox = (TextBox) skin.FindControl("Occupation");
            if (textbox != null)
                EditUser.Occupation = textbox.Text;
            
            textbox = (TextBox) skin.FindControl("Location");
            if (textbox != null)
                EditUser.Location = textbox.Text;
            
            textbox = (TextBox) skin.FindControl("Interests");
            if (textbox != null)
                EditUser.Interests = textbox.Text;
            
            textbox = (TextBox) skin.FindControl("MsnIm");
            if (textbox != null)
                EditUser.MsnIM = textbox.Text;
            
            textbox = (TextBox) skin.FindControl("YahooIm");
            if (textbox != null)
                EditUser.YahooIM = textbox.Text;
            
            textbox = (TextBox) skin.FindControl("AolIm");
            if (textbox != null)
                EditUser.AolIM = textbox.Text;
            
            textbox = (TextBox) skin.FindControl("ICQ");
            if (textbox != null)
                EditUser.IcqIM = textbox.Text;
            
            checkbox = (CheckBox) skin.FindControl("UnreadThreadsOnly");
            if (checkbox != null)
                EditUser.HideReadThreads = checkbox.Checked;
            
            checkbox = (CheckBox) skin.FindControl("ShowIcon");
            if (checkbox != null)
                EditUser.HasAvatar = checkbox.Checked;

            dropdown = (DropDownList) skin.FindControl("SiteStyle");
            if (dropdown != null)
                EditUser.Skin = dropdown.SelectedItem.Value;

            dropdown = (DropDownList) skin.FindControl("DateFormat");
            if (dropdown != null)
                EditUser.DateFormat = dropdown.SelectedItem.Value;

            dropdown = (DropDownList) skin.FindControl("PostViewOrder");
            if (dropdown != null)
                EditUser.ShowPostsAscending = Convert.ToBoolean(Convert.ToInt32(dropdown.SelectedItem.Value));

            control = skin.FindControl("HasIcon");
            if (null != control)
            {
                if (control.Visible == true) 
                {
                    EditUser.HasAvatar = true;
                }
            }

            // If we're in admin mode we have a couple other options to handle
            if (AdminMode == UserInfoEditMode.Admin)
            {
                checkbox = (CheckBox) skin.FindControl("ProfileApproved");
                EditUser.IsProfileApproved = checkbox.Checked;
                
                checkbox = (CheckBox) skin.FindControl("Banned");
                EditUser.IsApproved = !checkbox.Checked;

                checkbox = (CheckBox) skin.FindControl("Trusted");
                EditUser.IsTrusted = checkbox.Checked;
            }

            // Did the user send an icon?
            HtmlInputFile postedFile = (HtmlInputFile) skin.FindControl("Icon");
            if (postedFile != null) {

                if (postedFile.PostedFile.ContentLength != 0) {

                    if (postedFile.PostedFile.ContentType == "image/gif") {
                        postedFile.PostedFile.SaveAs(Page.Server.MapPath(Globals.ApplicationVRoot + "/UserIcons/" + EditUser.Username + ".gif"));
                        EditUser.Avatar = AvatarType.gif;
                        EditUser.HasAvatar = true;
                    } else if ((postedFile.PostedFile.ContentType == "image/jpg") || (postedFile.PostedFile.ContentType == "image/jpeg")) {
                        postedFile.PostedFile.SaveAs(Page.Server.MapPath(Globals.ApplicationVRoot + "/UserIcons/" + EditUser.Username + ".jpg"));
                        EditUser.Avatar = AvatarType.jpg;
                        EditUser.HasAvatar = true;
                    } else {
                        Label validatePostedFile = (Label) skin.FindControl("validatePostedFile");
                        validatePostedFile.Text = "File type must be .gif or .jpg";
                        return;
                    }
                }
            }

            // Do update
            try {
                bool updateResult;

                // Perform the actual update
                updateResult = Users.UpdateUserProfile(EditUser);

                // If we're admin do another update
                if (AdminMode == UserInfoEditMode.Admin) {
                    Users.UpdateUserInfoFromAdminPage(EditUser);
                }

                if (updateResult) {
                    // the user was updated successfully, send an email if the password was changed
                    Context.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UserProfileUpdated));
                    Context.Response.End();
                }
                else
                {
                    RequiredFieldValidator validatePassword;
                    validatePassword = (RequiredFieldValidator) skin.FindControl("ValidatePassword");

                    validatePassword.Text = "Password is invalid";
                    validatePassword.IsValid = false;
                }
            }
            catch (Exception)
            {
                RequiredFieldValidator validateEmail;
                validateEmail = (RequiredFieldValidator) skin.FindControl("ValidateEmail");

                validateEmail.Text = "Email address already exists.";
                validateEmail.IsValid = false;
            }
        }

        /// <summary>
        ///  Allow for Administration mode
        /// </summary>
        public UserInfoEditMode AdminMode
        {
            get
            {
                return adminMode;
            }
            set
            {
                adminMode = value;
            }
        }

        /// <summary>
        ///  Dynamically update the editable user to enable admin vs normal
        ///  user mode.
        /// </summary>
        protected User EditUser
        {
            get
            {
                if ( user == null )
                {
                    if ( AdminMode == UserInfoEditMode.Admin )
                    {
                        if ( Context != null )
                        {
                            string userName = Context.Request["Username"];
                            
                            if ( userName != null )
                            {
                                user = Users.GetUserInfo(userName, false);
                            }
                        }
                    }

                    if ( user == null )
                    {
                        user = ForumUser;
                    }
                }

                return user;
            }
        }

        public bool RequirePasswordForUpdate {
            get { return requirePasswordForUpdate; }
            set { requirePasswordForUpdate = value; }
        }
    }
}