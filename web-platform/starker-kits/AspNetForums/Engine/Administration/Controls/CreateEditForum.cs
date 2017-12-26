using System;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.IO;
using System.Web.UI.WebControls;
using System.ComponentModel;
using AspNetForums;
using AspNetForums.Components;

namespace AspNetForums.Controls.Admin {


    /// <summary>
    /// This Web control allows the end user to create a new forum or edit an existing forum.
    /// This control should be placed on a Web page that only administrative users have
    /// access to.
    /// </summary>
    /// <remarks>
    /// To specify whether to create a new forum or edit an existing one, set the Mode
    /// property accordingly.  Note that if you set the Mode to EditForum and do NOT
    /// set the ForumID property, an Exception will be thrown.
    /// </remarks>
    [
    ParseChildren(true)
    ]
    public class CreateEditForum : WebControl, INamingContainer {

        // The default message header
        const String defaultCreateHeaderMessage = "Create a New Forum";
        const String defaultEditHeaderMessage = "Edit a Forum";

        // Constants for the forum name textbox/reqexpvalidator
        const int defaultForumNameMaxLength = 100;
        const int defaultForumNameCols = 45;
        const String defaultForumNameWarningMessage = "You must supply a name for the forum.";

        // Constants for the forum Description textbox/reqexpvalidator
        const int defaultForumDescriptionMaxLength = 4000;
        const int defaultForumDescriptionCols = 60;
        const int defaultForumDescriptionRows = 10;
        const String defaultForumDescriptionWarningMessage = "You must supply a description for the forum.";

        // the default redirection Url (i.e, where to take the user once they create/update the forum)
        const String defaultRedirectUrl = "./";

        const bool defaultModerationOption = true;
        const bool defaultActiveOption = true;

        // Constructor
        public CreateEditForum() {

            // If we have an instance of context, let's attempt to
            // get the ForumID so we can save the user from writing
            // the code
            if (null != Context) {

                if (null != Context.Request.QueryString["ForumId"])
                    this.ForumID = Convert.ToInt32(Context.Request.QueryString["ForumId"]);
                else if (null != Context.Request.Form["ForumId"])
                    this.ForumID = Convert.ToInt32(Context.Request.Form["ForumId"]);

            }

            // Is this user isn't an administrator?
            if (this.CheckUserPermissions && !((User) Users.GetUserInfo(Context.User.Identity.Name, true)).IsAdministrator)
                Context.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UnableToAdminister));

            // Ensure that if the user wants to edit an existing forum, that they passed in a ForumID
            if (Mode == CreateEditForumMode.EditForum && ForumID == -1)
                throw new Exception("In order to edit a forum, you must pass in a valid ForumID.");

        }

        /// <summary>
        /// Populates the user control containing the form with the initial values for
        /// creating a new forum.
        /// </summary>
        /// <param name="control">An instance of the user control that contains the expected server controls.</param>
        private void PopulateCreateEditForum(Control control) {
            DropDownList dropdownlist;
            Button button;
            Label label;

            // Set the title
            label = (Label) control.FindControl("Title");
            if (Mode == CreateEditForumMode.CreateForum)
                label.Text = "Create a new forum";
            else
                label.Text = "Edit an existing forum";

            // Fill the Forum Group Drop Down
            dropdownlist = (DropDownList) control.FindControl("ForumGroups");
            dropdownlist.DataTextField = "Name";
            dropdownlist.DataValueField = "ForumGroupId";
            dropdownlist.DataSource = ForumGroups.GetAllForumGroups(true, false);
            dropdownlist.DataBind();

            // Wire up the button click
            button = (Button) control.FindControl("CreateUpdate");
            button.Click += new System.EventHandler(CreateUpdateForum_Click);

            // Initialize role based security
            UpdateRoleControls(control);

            if (Mode == CreateEditForumMode.CreateForum)
                button.Text = "Create New Forum";
            else
                button.Text = "Update Forum";
        }

        /// <summary>
        /// Handles the click event for the button when creating a new forum.
        /// </summary>
        private void CreateUpdateForum_Click(Object sender, EventArgs e) {
            Control form;
            Forum forum;

            // if the page is invalid, simply exit the function
            if (!Page.IsValid) 
                return;

            // Get the Edit Form
            form = FindControl("CreateEditForm");

            // Create a new forum
            forum = new Forum();

            forum.Name = ((TextBox) form.FindControl("ForumName")).Text;
            forum.Description = ((TextBox) form.FindControl("Description")).Text;
            forum.Moderated = ((CheckBox) form.FindControl("Moderated")).Checked;
            forum.Active = ((CheckBox) form.FindControl("Active")).Checked;
            forum.ForumGroupId = Convert.ToInt32(((DropDownList) form.FindControl("ForumGroups")).SelectedItem.Value);

            // Set special properties if this is a new forum
            if (Mode == CreateEditForumMode.CreateForum) {
                Forums.AddForum(forum);
            } else {
                // we need to update the forum
                forum.ForumID = ForumID;
                Forums.UpdateForum(forum);
            }

            // send the user back to the forum admin page
            Context.Response.Redirect(this.RedirectUrl);

        }

		// *********************************************************************
		// CreateChildControls
        //
        /// <summary>
		///	This event handler adds the children controls.
		///	</summary>
		//
		// ********************************************************************/
        protected override void CreateChildControls() {

            Control form;

            // Attempt to load the control. If this fails, we're done
            try {
                form = Page.LoadControl(Globals.ApplicationVRoot + "/skins/" + Globals.Skin + "/Skins/Skin-CreateEditForums.ascx");
            }
            catch (FileNotFoundException) {
                throw new Exception("The user control skins/Skins/Skin-CreateEditForums.ascx was not found. Please ensure this file exists in your skins directory");
            }

            // Set the id
            form.ID = "CreateEditForm";

            PopulateCreateEditForum(form);
 
            this.Controls.Add(form);

        }

        // *********************************************************************
        // OnPreRender
        //
        /// <summary>
        /// This event handler fires every time the page is loaded.  In essence, 
        /// we want to bind the data only if the user is editing the forum.
        /// </summary>
        /// <param name="e"></param>
        //
        // ********************************************************************/
        protected override void OnPreRender(EventArgs e) {

            // bind the data only if its the first visit to the page and
            // we are in edit mode
            if (!Page.IsPostBack && Mode == CreateEditForumMode.EditForum)
                BindData();

        }



        // *********************************************************************
        // BindData
        //
        /// <summary>
        ///	This function is called when the end user is editing an existing function.
        ///	It simply reads in the information about the appropriate forum and
        ///	binds the results to the Web controls.
        /// </summary>
        //
        // ********************************************************************/
        private void BindData() {
            Control form;
            Forum forum;

            // get information about the particular forum
            forum = Forums.GetForumInfo(ForumID);

            // Find the form control
            form = FindControl("CreateEditForm");

            // set the textbox/checkbox/listbox Web controls.
            ((TextBox) form.FindControl("ForumName")).Text = forum.Name;
            ((TextBox) form.FindControl("Description")).Text = Forums.ForumDescriptionFormattedToRaw(forum.Description);
            ((CheckBox) form.FindControl("Moderated")).Checked = forum.Moderated;
            ((CheckBox) form.FindControl("Active")).Checked = forum.Active;
            ((DropDownList) form.FindControl("ForumGroups")).Items.FindByValue(forum.ForumGroupId.ToString()).Selected = true;

        }
		

        // *********************************************************************
        //  ButtonSubmit_Click
        //
        /// <summary>
        ///	This event handler fires when the user clicks the submit button.  The
        ///	submit button might mean that we are creating a new forum or updating
        ///	an existing one, so the code must determine what the case is.  In any case,
        ///	static methods in the Forums class handle all of the dirty work.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        //
        // ********************************************************************/
        private void ButtonSubmit_Click(Object sender, EventArgs e) {

            // if the page is invalid, simply exit the function
            if (!Page.IsValid) 
                return;

            Forum forum = new Forum();

            if (Mode == CreateEditForumMode.CreateForum) {
                // ok, we have a valid page, so add the forum to the database
                forum.Name = ((TextBox) FindControl("txtName")).Text;
                forum.Description = ((TextBox) FindControl("txtDescription")).Text;
                forum.Moderated = ((CheckBox) FindControl("chkModerated")).Checked;
                forum.Active = ((CheckBox) FindControl("chkActive")).Checked;
                forum.DaysToView = Convert.ToInt32(((ListBox) FindControl("lstDaysToView")).SelectedItem.Text);
                forum.ForumGroupId = Convert.ToInt32(((DropDownList) FindControl("listForumGroup")).SelectedItem.Value);

                Forums.AddForum(forum);
            } else {
                // we need to update the forum
                forum.ForumID = ForumID;
                forum.Name = ((TextBox) FindControl("txtName")).Text;
                forum.Description = ((TextBox) FindControl("txtDescription")).Text;
                forum.Moderated = ((CheckBox) FindControl("chkModerated")).Checked;
                forum.Active = ((CheckBox) FindControl("chkActive")).Checked;
                forum.DaysToView = Convert.ToInt32(((ListBox) FindControl("lstDaysToView")).SelectedItem.Text);

                Forums.UpdateForum(forum);
            }

            // send the user back to the forum admin page
            Context.Response.Redirect(this.RedirectUrl);
        }



        // *********************************************************************
        //  HeaderMessage
        //
        /// <summary>
        ///	Specifies a header message to be displayed.
        /// </summary>
        //
        // ********************************************************************/
        public String HeaderMessage {
            get {
                if (ViewState["headerMessage"] == null) {
                    if (Mode == CreateEditForumMode.CreateForum)
                        return defaultCreateHeaderMessage;
                    else
                        return defaultEditHeaderMessage;
                }

                return (String) ViewState["headerMessage"];
            }

            set {  
                ViewState["headerMessage"] = value;  
            }

        }

        // *********************************************************************
        //  ForumNameWarningMessage
        //
        /// <summary>
        /// Specifies the warning message to return if the user does not specify
        /// a title for the Forum (which is a required field).
        /// </summary>
        //
        // ********************************************************************/
        public String ForumNameWarningMessage {

            get {
                if (ViewState["forumNameWarningMsg"] == null) 
                    return defaultForumNameWarningMessage;

                return (String) ViewState["forumNameWarningMsg"];
            }

            set {  
                ViewState["forumNameWarningMsg"] = value;  
            }

        }

        // *********************************************************************
        //  ForumNameColumns
        //
        /// <summary>
        /// Specifies how many columns to allow for the forum title text box.
        /// </summary>
        //
        // ********************************************************************/
        public int ForumNameColumns {
            get  {
                if (ViewState["forumNameCols"] == null) 
                    return defaultForumNameCols;

                return (int) ViewState["forumNameCols"];
            }

            set {  
                ViewState["forumNameCols"] = value;  
            }

        }

        // *********************************************************************
        //  ForumDescriptionColumns
        //
        /// <summary>
        /// Indicates how many columns wide to make the forum description text box (a
        /// multi-line text box).  Defaults to 60.
        /// </summary>
        //
        // ********************************************************************/
        public int ForumDescriptionColumns {
            get {
                if (ViewState["forumDescCols"] == null) 
                    return defaultForumDescriptionCols;

                return (int) ViewState["forumDescCols"];
            }

            set {  
                ViewState["forumDescCols"] = value;  
            }

        }
		
        // *********************************************************************
        //  ForumDescriptionRows
        //
        /// <summary>
        /// Specifies how many rows tall to make the forum description text box.  Defaults
        /// to 10.
        /// </summary>
        //
        // ********************************************************************/
        public int ForumDescriptionRows {
            get {
                if (ViewState["forumDescRows"] == null) 
                    return defaultForumDescriptionRows;

                return (int) ViewState["forumDescRows"];
            }

            set {  
                ViewState["forumDescRows"] = value;  
            }

        }

        // *********************************************************************
        //  ForumDescriptionWarningMessage
        //
        /// <summary>
        /// Specifies the warning message to display if the user does not enter a
        /// forum description, which is a required field.
        /// </summary>
        //
        // ********************************************************************/
        public String ForumDescriptionWarningMessage {
            get {
                if (ViewState["forumDescWarningMsg"] == null) 
                    return defaultForumDescriptionWarningMessage;

                return (String) ViewState["forumDescWarningMsg"];
            }

            set {  
                ViewState["forumDescWarningMsg"] = value;  
            }

        }

        // *********************************************************************
        //  RedirectUrl
        //
        /// <summary>
        /// Indicates the Url to send the user to once they create or update the forum.  Defaults
        /// to ./, which sends the user back to the default document in the current directory.
        /// </summary>
        //
        // ********************************************************************/
        public String RedirectUrl {

            get {
                if (ViewState["redirUrl"] == null) 
                    return defaultRedirectUrl;

                return (String) ViewState["redirUrl"];
            }

            set {  
                ViewState["redirUrl"] = value;  
            }

        }

        // *********************************************************************
        //  RedirectUrl
        //
        /// <summary>
        /// Specifies the mode for the Web control.  This property can have one of two
        /// values: CreateForum or EditForum.  When the Mode is set to CreateForum, the
        /// Web control allows the user to create a new forum; when it's set to
        /// EditForum, the user is allowed to edit an existing forum (in which case the
        /// ForumID must be set to the forum to be edited).
        /// <seealso cref="ForumID"/>
        /// </summary>
        /// <remarks>
        /// If Mode is set to EditForum and the ForumID property is not set,
        /// an Exception will be thrown.
        /// </remarks>
        //
        // ********************************************************************/
        public CreateEditForumMode Mode {
            get {
                if (ViewState["mode"] == null) 
                    return CreateEditForumMode.CreateForum;

                return (CreateEditForumMode) ViewState["mode"];
            }

            set {  
                ViewState["mode"] = value;  
            }
        }

        // *********************************************************************
        //  DefaultModerationOption
        //
        /// <summary>
        /// Indicates whether or not forums, when created, are moderated by default or not.
        /// If this property is set to true (the default) when a new forum is created, the
        /// Moderated checkbox will be checked initially.  If this is false, this checkbox will
        /// be unchecked.
        /// </summary>
        //
        // ********************************************************************/
        public bool DefaultModerationOption {
            get {
                if (ViewState["defaultModOption"] == null) 
                    return defaultModerationOption;

                return (bool) ViewState["defaultModOption"];
            }

            set { 
                ViewState["defaultModOption"] = value;  
            }
        }

        // *********************************************************************
        //  DefaultActiveOption
        //
        /// <summary>
        /// Indicates whether or not forums, when created, are active by default or not.
        /// If this property is set to true (the default) when a new forum is created, the
        /// Active checkbox will be checked initially.  If this is false, this checkbox will
        /// be unchecked.
        /// </summary>
        //
        // ********************************************************************/
        public bool DefaultActiveOption {
            get {
                if (ViewState["defaultActiveOption"] == null) 
                    return defaultActiveOption;

                return (bool) ViewState["defaultActiveOption"];
            }

            set { 
                ViewState["defaultActiveOption"] = value;  
            }
        }

        // *********************************************************************
        //  ForumID
        //
        /// <summary>
        /// The ForumID of the forum to edit.  For use when Mode is set to EditForum.
        /// <seealso cref="Mode"/>
        /// </summary>
        /// <remarks>
        /// If Mode is set to EditForum and this property is not set, an Exception
        /// will be thrown.
        /// </remarks>
        //
        // ********************************************************************/
        public int ForumID {
            get {
                if (ViewState["forumID"] == null) 
                    return -1;

                return (int) ViewState["forumID"];
            }

            set {  
                ViewState["forumID"] = value;  
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
            if ( Mode == CreateEditForumMode.EditForum )
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
                string[] userRoles = UserRoles.GetForumRoles(ForumID);
                string[] roles = UserRoles.GetAllRoles();

                DataGrid activeRoles = skin.FindControl("ActiveRoles") as DataGrid;
                if ( activeRoles != null ) {
					if ( userRoles.Length > 0 ) {
						activeRoles.DataSource = userRoles;
						activeRoles.DataBind();
						activeRoles.ItemCommand += new DataGridCommandEventHandler(Roles_Command);
						activeRoles.Visible = true;
					}
					else {
						activeRoles.Visible = false;
					}
				}
				
				Label noActiveRoles = skin.FindControl("NoActiveRoles") as Label;
				if ( noActiveRoles != null ) {
					if ( userRoles.Length > 0 ) {
						noActiveRoles.Visible = false;
					}
					else {
						noActiveRoles.Visible = true;
					}
				}

                DataGrid allRoles = skin.FindControl("AllRoles") as DataGrid;
                if ( allRoles != null ) {
					if ( roles.Length > 0 ) {
						allRoles.DataSource = roles;
						allRoles.DataBind();
						allRoles.ItemCommand += new DataGridCommandEventHandler(Roles_Command);
						allRoles.Visible = true;
					}
					else {
						allRoles.Visible = false;
					}
				}

				Label noAllRoles = skin.FindControl("NoAllRoles") as Label;
				if ( noAllRoles != null ) {
					if ( roles.Length > 0 ) {
						noAllRoles.Visible = false;
					}
					else {
						noAllRoles.Visible = true;
					}
				}
            }
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
            int forumID = ForumID;
            string action = e.CommandName;
            string role = e.CommandArgument.ToString();

            switch(action)
            {
                case "AddRole":
                    UserRoles.AddForumToRole(forumID, role);
                    UpdateRoleControls(((Control) sender).Parent);
                    break;
                case "RemoveRole":
                    UserRoles.RemoveForumFromRole(forumID, role);
                    UpdateRoleControls(((Control) sender).Parent);
                    break;
            }
        }

        // *********************************************************************
        //  CheckUserPermissions
        //
        /// <summary>
        /// Indicates if the Web control should check to verify that the user visiting the page
        /// is, indeed, a moderator.
        /// </summary>
        //
        // ********************************************************************/
        public bool CheckUserPermissions 
        {
            get {
                if (ViewState["checkUserPerm"] == null) 
                    return true;

                return (bool) ViewState["checkUserPerm"];
            }

            set { 
                ViewState["checkUserPerm"] = value; 
            }

        }
    }
}