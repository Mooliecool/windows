/***************************** ModeratedForums Web control **********************************
 * 
 * SUMMARY:
 *      This Web control has two purposes: it can display the forums that a particular user can
 *      moderate, and provide a means for the end user to add and remove from this list of forums
 *      that can be moderated by a particular user; also, it can list the users that can moderate
 *      a particular forum.  However, this control does not allow for users to be added and removed
 *      as moderators for a particular forum.
 *
 * GENERAL COMMENTS:
 *      Chances are, the end developer will never need to use this control from a page itself.
 *      An instance of this control is created in both the UserAdmin and EditForum user
 *      controls.
 *
 ******************************************************************************************/


using System;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Configuration;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Components;
using System.ComponentModel;

namespace AspNetForums.Controls.Moderation {
    [ ToolboxItemAttribute(false) ]

    /// <summary>
    /// This Web control has two purposes: it can display the forums that a particular user can
    /// moderate, and provide a means for the end user to add and remove from this list of forums
    /// that can be moderated by a particular user; also, it can list the users that can moderate
    /// a particular forum.  However, this control does not allow for users to be added and removed
    /// as moderators for a particular forum.
    /// </summary>
    /// <remarks>Chances are, the end developer will never need to use this control from a page itself.
    /// An instance of this control is created in both the UserAdmin and EditForum user
    /// controls.</remarks>
    public class ModeratedForums : SkinnedForumWebControl {
        const ModeratedForumMode _defaultMode = ModeratedForumMode.ViewForUser;
        const string skinFilename = "Moderation/Skin-ModeratedForums.ascx";
        
        private DataGrid moderatedForums = null;
        private DataGrid moderatingUsers = null;
        private DropDownList unmoderatedForums = null;
        private Button addForumButton = null;
        private CheckBox emailNotification = null;
        private Panel moderatedForumsPanel = null;
        private Panel unmoderatedForumsPanel = null;

        // *********************************************************************
        //  EditUserInfo
        //
        /// <summary>
        /// Constructor
        /// </summary>
        /// 
        // ********************************************************************/
        public ModeratedForums() {

            // Set the default skin
            if (SkinFilename == null) {
                SkinFilename = skinFilename;
            }
            
            if ( Context != null ) {
                if ( Context.Request["Username"] != null ) {
                    Username = Context.Request["Username"];
                }
                if ( Context.Request["ForumID"] != null ) {
                    ForumID = Int32.Parse(Context.Request["ForumID"].ToString());
                }
            }

            this.PreRender += new EventHandler(PreRenderDataBinding);
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
            // make sure we have a username/forumID
            if (Mode == ModeratedForumMode.ViewForUser && Username.Length == 0)
                throw new Exception("When specifying Mode as ViewForUser you must pass in the Username of the user whose forum moderation informaion you wish to view.");

            if (Mode == ModeratedForumMode.ViewForForum && ForumID == -1)
                throw new Exception("When specifying Mode as ViewForForum you must pass in the ForumID of the forum whose moderators you wish to view.");

            // Find all skinned controls
            moderatedForums = skin.FindControl("UserModeratedForums") as DataGrid;
            moderatingUsers = skin.FindControl("ForumModeratedForums") as DataGrid;
            unmoderatedForums = skin.FindControl("UnmoderatedForums") as DropDownList;
            addForumButton = skin.FindControl("AddForum") as Button;
            emailNotification = skin.FindControl("EmailNotification") as CheckBox;
            
            // Find all control panels
            moderatedForumsPanel = skin.FindControl("ModeratedForumsPanel") as Panel;
            unmoderatedForumsPanel = skin.FindControl("UnmoderatedForumsPanel") as Panel;
            
            SkinLogic();
        }
        
        private void SkinLogic() {
            if ( Mode == ModeratedForumMode.ViewForForum ) {
                ModeratedForumCollection moderators = Moderate.GetForumModerators(ForumID);
                
                if ( moderators.Count > 0 ) {
                    if ( moderatedForumsPanel != null ) {
                        moderatedForumsPanel.Visible = true;
                    }
                }
                else {
                    moderatedForumsPanel.Visible = false;
                }
                
                if ( moderatedForums != null ) {
                    moderatedForums.Visible = false;
                }

                if ( unmoderatedForumsPanel != null ) {
                    unmoderatedForumsPanel.Visible = false;
                }
            }
            
            if ( Mode == ModeratedForumMode.ViewForUser ) {
                // Get the collections of forums this user can moderate/not moderate
                ModeratedForumCollection moderated = Users.GetForumsModeratedByUser(Username);
                ModeratedForumCollection notmoderated = Users.GetForumsNotModeratedByUser(Username);

                if ( moderated.Count > 0 ) {
                    if ( moderatedForumsPanel != null ) {
                        moderatedForumsPanel.Visible = true;
                    }
                }
                else {
                    if ( moderatedForumsPanel != null ) {
                        moderatedForumsPanel.Visible = false;
                    }
                }

                if ( notmoderated.Count > 0 ) {
                    if ( unmoderatedForumsPanel != null ) {
                        unmoderatedForumsPanel.Visible = true;

                    }
                }
                else {
                    if ( unmoderatedForumsPanel != null ) {
                        unmoderatedForumsPanel.Visible = false;
                    }
                }

            }

            // Hook up the Add Forum Button
            if ( addForumButton != null ) {
                addForumButton.Click += new EventHandler(AddForum_Click);
            }

            // Hook up the Moderated Forums Removal Handler
            if ( moderatedForums != null ) {
                moderatedForums.ItemCommand += new DataGridCommandEventHandler(Moderated_Command);
            }
        }

        private void PreRenderDataBinding(object sender, EventArgs e) {
            InternalDataBind();
        }
        
        public void RebindData() {
        }
        
        private void InternalDataBind() {
            if ( Mode == ModeratedForumMode.ViewForUser ) {
                // Get the collections of forums this user can moderate/not moderate
                ModeratedForumCollection moderated = Users.GetForumsModeratedByUser(Username);
                ModeratedForumCollection notmoderated = Users.GetForumsNotModeratedByUser(Username);

                if ( moderatedForums != null ) {
                    moderatedForums.DataSource = moderated;
                    moderatedForums.DataBind();
                }

                if ( unmoderatedForums != null ) {
                    unmoderatedForums.DataSource = notmoderated;
                    unmoderatedForums.DataBind();
                }
            }
            
            if ( Mode == ModeratedForumMode.ViewForForum ) {
                ModeratedForumCollection moderators = Moderate.GetForumModerators(ForumID);
                
                if ( moderatingUsers != null ) {
                    moderatingUsers.DataSource = moderators;
                    moderatingUsers.DataBind();
                }
            }
        }
        
        private void AddForum_Click(object sender, EventArgs e) {
            if ( unmoderatedForums != null ) {
                int forumID = -1;
                try {
                    forumID = Int32.Parse(unmoderatedForums.SelectedItem.Value);
                }
                catch {}
                bool notify = (emailNotification == null) ? false : emailNotification.Checked;
                
                if ( forumID > -1 ) {
                    ModeratedForum forum = new ModeratedForum();
                    forum.Username = Username;
                    forum.ForumID = forumID;
                    forum.EmailNotification = notify;
                    Users.AddModeratedForumForUser(forum);
                    SkinLogic();
                }
            }
        }
        
        private void Moderated_Command(object sender, DataGridCommandEventArgs e) {
            string command = e.CommandName;
            int forumID = Int32.Parse(e.CommandArgument.ToString());
            
            switch(command) {
                case "Remove":
                    ModeratedForum forum = new ModeratedForum();
                    forum.Username = Username;
                    forum.ForumID = forumID;
                    Users.RemoveModeratedForumForUser(forum);
                    SkinLogic();
                    break;
            }
        }

        /// <summary>
        /// When Mode is set to ViewForUser, you must specify the Username of the
        /// user whose list of moderated forums you wish to view.
        /// <seealso cref="Mode"/>
        /// </summary>
        /// <remarks>If Mode is set to ViewForUser and Username is not specified, an
        /// Exception will be thrown.</remarks>
        public String Username {
            get {
                if (ViewState["username"] == null) return "";
                return (String) ViewState["username"];
            }
            set { ViewState["username"] = value; }
        }
        
        /// <summary>
        /// Specifies the Mode for the Web control.  This property can have one of two values:
        /// ViewForUser or ViewForForum.  If ViewForUser is selected, a Username must be passed in
        /// and the list of forums that the particular user moderates is displayed, along with the
        /// option to add and remove forums from this list.  If Mode is set to ViewForForum, a
        /// ForumID must be passed in and the list of users that moderate the particular forum are
        /// shown.  The default is ViewForUser.
        /// <seealso cref="Username"/>
        /// <seealso cref="ForumID"/>
        /// </summary>
        /// <remarks>If Mode is set to ViewForUser and the Username property is not set, an
        /// Exception will be thrown.  Likewise, if Mode is set to ViewForForum and the ForumID
        /// property is not set, an Exception will be thrown.</remarks>
        public ModeratedForumMode Mode {
            get {
                if (ViewState["mode"] == null) return _defaultMode;
                return (ModeratedForumMode) ViewState["mode"];
            }
            set { ViewState["mode"] = value; }
        }

        /// <summary>
        /// Indicates if the Web control should check to verify that the user visiting the page
        /// is, indeed, a moderator.
        /// </summary>
        public bool CheckUserPermissions {
            get {
                if (ViewState["checkUserPerm"] == null) return true;
                return (bool) ViewState["checkUserPerm"];
            }
            set { ViewState["checkUserPerm"] = value; }
        }
        /*******************************************************/
    }
}