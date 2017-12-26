using System;
using System.Collections;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Web;
using System.Web.UI;
using System.IO;
using System.Web.UI.WebControls;
using System.ComponentModel;
using AspNetForums;
using AspNetForums.Components;


namespace AspNetForums.Controls.Admin {


    /// <summary>
    /// This control can be used by adminsitrators to create, edit, or delete roles.
    /// </remarks>
    [
    ParseChildren(true)
    ]
    public class CreateEditRole : SkinnedForumWebControl {
        // Skin based Controls
        Label title;
        RequiredFieldValidator validateRoleName;
        TextBox roleName;
        DropDownList roleNames;
        TextBox roleDescription;

        string skinFilename = "Skin-CreateEditRole.ascx";
        CreateEditRoleMode mode = CreateEditRoleMode.EditRole;
        

        // *********************************************************************
        // CreateEditRole
        //
        /// <summary>
        ///	Constructor
        ///	</summary>
        //
        // ********************************************************************/
        public CreateEditRole() : base() {
            if (SkinFilename == null)
                SkinFilename = skinFilename;

            if ( Context != null )
            {
                if ( Context.Request["Rolename"] != null &&
                     Context.Request["Rolename"].Length > 0 )
                {
                    this.Rolename = Context.Request["Rolename"];
                }

                if ( Context.Request["Action"] != null &&
                     Context.Request["Action"].Length > 0 )
                {
                    this.Mode = (CreateEditRoleMode) Enum.Parse(
                        typeof(CreateEditRoleMode),
                        Context.Request["Action"].ToString(),
                        true);
                }
            }
        }

        // *********************************************************************
        //  InitializeSkin
        //
        /// <summary>
        /// Initialize the skin to be applied to this server control.
        /// </summary>
        // ***********************************************************************/
        override protected void InitializeSkin(Control skin) {
            Button button;

            // Find and set the title
            title = skin.FindControl("title") as Label;

            // Find the validator
            validateRoleName = skin.FindControl("roleNameValidator") as RequiredFieldValidator;

            // Find the Role Name and Description Text boxes
            roleName = skin.FindControl("roleName") as TextBox;
            if ( roleName != null ) {
            	roleName.Text = "";
			}

			// Find the TextArea for role descriptions
            roleDescription = skin.FindControl("roleDescription") as TextBox;
            if ( roleDescription != null ) {
	            roleDescription.Text = "";
			}

			// Find the drop down list for roles			
			roleNames = skin.FindControl("roleNames") as DropDownList;

            // Based on the mode we're in..
            switch(Mode)
            {
                case CreateEditRoleMode.CreateRole:
                    // Set title text
                    if ( title != null ) {
	                    title.Text = "Create a new user security role";
					}
					if ( roleName != null ) {
						roleName.Visible = true;
					}
					if ( roleNames != null ) {
						roleNames.Visible = false;
					}
                    break;
                case CreateEditRoleMode.EditRole:
                    // Set title text
                    if ( title != null ) {
                    	title.Text = "Edit an existing role";
					}
					if ( roleName != null ) {
						roleName.Visible = false;
					}
					if ( roleNames != null ) {
						roleNames.Visible = true;
						roleNames.AutoPostBack = true;
						roleNames.DataSource = UserRoles.GetAllRoles();
						roleNames.DataBind();

						if ( Rolename == null || Rolename.Length == 0 ) {
							if ( roleNames.Items.Count > 0 ) {
								Rolename = roleNames.Items[0].Text;
							}
						}
						else {
							ListItem selected = roleNames.Items.FindByValue(Rolename);
							if ( selected != null ) {
								selected.Selected = true;
							}
						}

						roleNames.SelectedIndexChanged += new EventHandler(RoleNames_SelectedIndexChanged);
					}
					
                    if ( Rolename != null && Rolename.Length > 0 ) {
                        roleDescription.Text = UserRoles.GetRoleDescription(Rolename);
                    }
                    else {
                        roleDescription.Text = "You need to define roles to edit.";
                    }
                    break;
            }
            // Wire up the button click
            button = skin.FindControl("CreateRole") as Button;
            if ( button != null ) {
				if (Mode == CreateEditRoleMode.CreateRole) {
					button.Text = "Create Role";
					button.Click += new System.EventHandler(CreateRole_Click);
				}
				else {
					if ( roleNames == null || roleNames.Items.Count == 0 ) {
						button.Visible = false;
					}
					button.Text = "Update Role";
					button.Click += new System.EventHandler(UpdateRole_Click);
				}
			}

            // Wire up the delete button click
            button = (Button) skin.FindControl("DeleteRole");
            if (Mode == CreateEditRoleMode.CreateRole || roleNames == null || roleNames.Items.Count == 0) {
                button.Visible = false;
            } else {
                button.Visible = true;
                button.Text = "Delete Role";
                button.Click += new System.EventHandler(DeleteRole_Click);
            }        
        }

        private void RoleNames_SelectedIndexChanged(Object sender, EventArgs e)
        {
			if ( roleNames != null ) {
				Rolename = roleNames.Items[roleNames.SelectedIndex].Text;
				
				if ( Rolename != null && Rolename.Length > 0 ) {
					roleDescription.Text = UserRoles.GetRoleDescription(Rolename);
				}
				else {
					roleDescription.Text = "You need to define roles to edit.";
				}
			}
        }
    
        /*******************************************************************
        // UpdateRole_Click
        //
        /// <summary>
        /// Event raised when the user is ready to update a user security role
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        //
        ********************************************************************/
        private void UpdateRole_Click(Object sender, EventArgs e) {

            // If the page is invalid, simply exit the function
            if (!Page.IsValid) 
                return;

            if ( Mode == CreateEditRoleMode.EditRole )
            {
                // Update
                UserRoles.UpdateRole(Rolename, roleDescription.Text);
            }

            // send the user back to the forum admin page
            Context.Response.Redirect(this.RedirectUrl);
        }

        /*******************************************************************
        // DeleteRole_Click
        //
        /// <summary>
        /// Event raised when the user deletes a user security role
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        //
        ********************************************************************/
        private void DeleteRole_Click(Object sender, EventArgs e) {

            // If the page is invalid, simply exit the function
            if (!Page.IsValid) 
                return;

            if ( Mode == CreateEditRoleMode.EditRole )
            {
                // Update the role's description
                UserRoles.DeleteRole(Rolename);
            }

            // send the user back to the forum admin page
            Context.Response.Redirect(this.RedirectUrl);
        }

        /*******************************************************************
        // CreateRole_Click
        //
        /// <summary>
        /// Event raised when the user is ready to create a new user
        /// security role.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        //
        ********************************************************************/
        private void CreateRole_Click(Object sender, EventArgs e) {

            // If the page is invalid, simply exit the function
            if (!Page.IsValid) 
                return;

            if (Mode == CreateEditRoleMode.CreateRole)
            {
                UserRoles.CreateNewRole(roleName.Text, roleDescription.Text);
            }

            // send the user back to the forum admin page
            Context.Response.Redirect(this.RedirectUrl);
        }

        // *********************************************************************
        //  Mode
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
        public CreateEditRoleMode Mode {
            get {
                return mode;
            }

            set {  
                mode = value;  
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
                    return Globals.UrlAdmin;

                return (String) ViewState["redirUrl"];
            }
            set {
                ViewState["redirUrl"] = value;  
            }
        }

        public string Rolename
        {
            get
            {
                return (String) ViewState["roleName"];
            }

            set
            {
                ViewState["roleName"] = value;
            }
        }
   }
}