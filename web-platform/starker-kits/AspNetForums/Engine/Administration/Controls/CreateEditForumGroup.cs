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
    /// This control is used by administrators to edit existing or delete existing forum groups.
    /// </remarks>
    [
    ParseChildren(true)
    ]
    public class CreateEditForumGroup : SkinnedForumWebControl {

        Label title;
        TextBox forumGroupName;
        RequiredFieldValidator validateForumGroupName;
        ListBox forumGroupList;
        ImageButton upButton;
        ImageButton downButton;
        string skinFilename = "Skin-CreateEditForumGroup.ascx";
        CreateEditForumMode mode = CreateEditForumMode.CreateForum;

        // Default redirection Url (i.e, where to take the user once they create/update the forum)
        String defaultRedirectUrl = Globals.UrlAdmin;
        

        // *********************************************************************
        // CreateEditForumGroup
        //
        /// <summary>
        ///	Constructor
        ///	</summary>
        //
        // ********************************************************************/
        public CreateEditForumGroup() : base() {

            if (SkinFilename == null)
                SkinFilename = skinFilename;

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
            title = (Label) skin.FindControl("title");

            // Find the validator
            validateForumGroupName = (RequiredFieldValidator) skin.FindControl("forumGroupNameValidator");

            // Find the forum group name textbox
            forumGroupName = (TextBox) skin.FindControl("forumGroupName");
            forumGroupName.Text = "";

            // Based on the mode we're in..
            if (Mode == CreateEditForumMode.CreateForum) {

                // Set title text
                title.Text = "Create new forum group";

                // Hide the drop down list
                skin.FindControl("EditMode").Visible = false;
            } else {

                // Display the drop down list
                skin.FindControl("EditMode").Visible = true;

                // Set title text
                title.Text = "Edit an existing forum group";

                // Set the validation text
                validateForumGroupName.Text = "Please select a forum group name from the drop down list.";

                // Set the textbox to readonly
                forumGroupName.ReadOnly = true;

                // Find the drop down list
                forumGroupList = (ListBox) skin.FindControl("ForumGroups");
                PopulateListBox(0);

                // Allow the text box to be edited
                forumGroupName.ReadOnly = false;

                forumGroupName.Text = forumGroupList.SelectedItem.Text;

                // Find the image buttons
                upButton = (ImageButton) skin.FindControl("MoveUpButton");
                if (null != upButton) {
                    upButton.ImageUrl = Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/up.gif";
                    upButton.Click += new System.Web.UI.ImageClickEventHandler(MoveForumGroupUpInSortOrder_Click);
                }

                downButton = (ImageButton) skin.FindControl("MoveDownButton");
                if (null != downButton) {
                    downButton.ImageUrl = Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/dn.gif";
                    downButton.Click += new System.Web.UI.ImageClickEventHandler(MoveForumGroupDownInSortOrder_Click);
                }
            }

            // Wire up the button click
            button = (Button) skin.FindControl("CreateForumGroup");
            if (Mode == CreateEditForumMode.CreateForum) {
                button.Text = "Create Forum Group ";
                button.Click += new System.EventHandler(CreateForumGroup_Click);
            } else {
                button.Text = " Update Forum Group Name ";
                button.Click += new System.EventHandler(UpdateForumGroup_Click);
            }

            // Wire up the delete button click
            button = (Button) skin.FindControl("DeleteForumGroup");
            if (Mode == CreateEditForumMode.CreateForum) {
                button.Visible = false;
            } else {
                button.Visible = true;
                button.Text = " Delete Forum Group ";
                button.Click += new System.EventHandler(DeleteForumGroup_Click);
            }        
        }

        /*******************************************************************
        // PopulateListBox
        //
        /// <summary>
        /// Used to populate the list box that lists the available forum groups
        /// </summary>
        //
        ********************************************************************/
        public void PopulateListBox(int selectedForumGroupById) {
            ForumGroupCollection forums = ForumGroups.GetAllForumGroups(true, false);
            forumGroupList.DataTextField = "Name";
            forumGroupList.DataValueField = "ForumGroupId";
            forumGroupList.DataSource = forums;
            forumGroupList.Rows = forums.Count;
            forumGroupList.DataBind();
            forumGroupList.AutoPostBack = true;
            forumGroupList.SelectedIndexChanged += new System.EventHandler(ForumGroupList_SelectionChange);

            if (selectedForumGroupById > 0)
                forumGroupList.Items.FindByValue(selectedForumGroupById.ToString()).Selected = true;
            else
                forumGroupList.Items[0].Selected = true;
        }

        /*******************************************************************
        // MoveForumGroupDownInSortOrder_Click
        //
        /// <summary>
        /// Used to control moving Forum Groups up and down in the sort order
        /// </summary>
        //
        ********************************************************************/
        private void MoveForumGroupDownInSortOrder_Click(Object sender, ImageClickEventArgs e) {

            int selectedForumGroup = Convert.ToInt32(forumGroupList.SelectedItem.Value);

            ForumGroups.ChangeForumGroupSortOrder(selectedForumGroup, false);

            PopulateListBox(selectedForumGroup);
        }

        /*******************************************************************
        // MoveForumGroupUpInSortOrder_Click
        //
        /// <summary>
        /// Used to control moving Forum Groups up and down in the sort order
        /// </summary>
        //
        ********************************************************************/
        private void MoveForumGroupUpInSortOrder_Click(Object sender, ImageClickEventArgs e) {
            int selectedForumGroup = Convert.ToInt32(forumGroupList.SelectedItem.Value);

            ForumGroups.ChangeForumGroupSortOrder(selectedForumGroup, true);

            PopulateListBox(selectedForumGroup);
        }

        /*******************************************************************
        // ForumGroupList_SelectionChange
        //
        /// <summary>
        /// This event is raised when the drop down that allows the user to 
        /// either edit an existing forum group name or create a new forum group
        /// name
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        //
        ********************************************************************/
        private void ForumGroupList_SelectionChange(Object sender, EventArgs e) {
            ListBox forumGroupList;

            // Is the validator currently on? If so, turn it off
            validateForumGroupName.IsValid = true;

            // Get the control that raised teh event
            forumGroupList = (ListBox) sender;

            // Allow the text box to be edited
            forumGroupName.ReadOnly = false;

            // If the selection is -1 that means we want to create a new forum group
            if (Convert.ToInt32(forumGroupList.SelectedItem.Value) != -1)
                forumGroupName.Text = forumGroupList.SelectedItem.Text;
            else
                forumGroupName.Text = "";

        }

        /*******************************************************************
        // UpdateForumGroup_Click
        //
        /// <summary>
        /// Event raised when the user is ready to update forum group name
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        //
        ********************************************************************/
        private void UpdateForumGroup_Click(Object sender, EventArgs e) {

            // If the page is invalid, simply exit the function
            if (!Page.IsValid) 
                return;

            // Update
            ForumGroups.UpdateForumGroup(forumGroupName.Text, Convert.ToInt32(forumGroupList.SelectedItem.Value));

        }

        /*******************************************************************
        // DeleteForumGroup_Click
        //
        /// <summary>
        /// Event raised when the user deletes a forum group name
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        //
        ********************************************************************/
        private void DeleteForumGroup_Click(Object sender, EventArgs e) {

            // If the page is invalid, simply exit the function
            if (!Page.IsValid) 
                return;

            // Update
            ForumGroups.UpdateForumGroup(null, Convert.ToInt32(forumGroupList.SelectedItem.Value));
        }

        /*******************************************************************
        // CreateForumGroup_Click
        //
        /// <summary>
        /// Event raised when the user is ready to create a new forum
        /// group name
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        //
        ********************************************************************/
        private void CreateForumGroup_Click(Object sender, EventArgs e) {

            // If the page is invalid, simply exit the function
            if (!Page.IsValid) 
                return;

            try {
                if (Mode == CreateEditForumMode.CreateForum) {
                    ForumGroups.AddForumGroup(forumGroupName.Text);
                }
            } catch (Exception) {
                validateForumGroupName.Text = "Forum group name already exists.";
                validateForumGroupName.IsValid = false;
                return;
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
        public CreateEditForumMode Mode {
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
                    return defaultRedirectUrl;

                return (String) ViewState["redirUrl"];
            }

            set {  
                ViewState["redirUrl"] = value;  
            }

        }

   }
}