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
using System.IO;


namespace AspNetForums.Controls.Admin {

    /// <summary>
    /// This Web control displays a list of the available email templates and allows 
    /// the administrator to edit these templates.
    /// </summary>
    [
        ParseChildren(true)
    ]
    public class MessageTemplateAdmin : WebControl, INamingContainer {
        Control controlTemplate;
        DropDownList messageTemplates;
        TextBox title;
        TextBox body;
        Label status;
        HyperLink preview;

        // ****************************************************************
        // InitializeControlTemplate
        //
        /// <summary>
        /// This control uses a user control for a template. This function
        /// initializes the control and attempts to call FindControl on the 
        /// controls that it needs access to in order to render properly
        /// </summary>
        // ****************************************************************
        private void InitializeControlTemplate() {
            Button button;

            // Find and populate the drop down list with the available templates
            messageTemplates = (DropDownList) controlTemplate.FindControl("messageTemplateList");
            messageTemplates.DataTextField = "Title";
            messageTemplates.DataValueField = "MessageID";
            messageTemplates.DataSource = ForumMessages.GetMessageTemplateList();
            messageTemplates.DataBind();
            messageTemplates.AutoPostBack = true;
            messageTemplates.SelectedIndexChanged += new System.EventHandler(MessageTemplate_Changed);

            // Set up the button used to update the email template
            button = (Button) controlTemplate.FindControl("UpdateTemplate");
            button.Text = " Update Template ";
            button.Click += new System.EventHandler(UpdateTemplate_Click);

            // Set up the preview option
            preview = (HyperLink) controlTemplate.FindControl("PreviewMessage");

            // Find the status control
            status = (Label) controlTemplate.FindControl("Status");
            status.Visible = false;

            // Find the subject text box and the body textbox
            title = (TextBox) controlTemplate.FindControl("Title");
            body = (TextBox) controlTemplate.FindControl("Body");

            DisplayEditMode();
        }

        // ****************************************************************
        // UpdateTemplate_Click
        //
        /// <summary>
        /// Logic to handle updating the message in the database when
        /// the user has made changes.
        /// </summary>
        // ****************************************************************
        private void UpdateTemplate_Click(Object sender, EventArgs e) {
            ForumMessage message = new ForumMessage();
            message.MessageID = Convert.ToInt32(messageTemplates.SelectedItem.Value);
            message.Title = title.Text;
            message.Body = body.Text;

            // update the email template
            ForumMessages.UpdateMessageTemplate(message);

            // Display that we've updated
            status.Visible = true;
            status.Text = "Message template updated...";
        }
		
        // ****************************************************************
        // MessageTemplate_Changed
        //
        /// <summary>
        /// Raised when the drop down list of available messages changes
        /// </summary>
        // ****************************************************************
        private void MessageTemplate_Changed(Object sender, EventArgs e) {
            DisplayEditMode();
        }

        // ****************************************************************
        // DisplayEditMode
        //
        /// <summary>
        /// Retrieves the values for the title and body from the database
        /// and dispalys them in textboxs for the user to edit
        /// </summary>
        // ****************************************************************
        private void DisplayEditMode() {

            // Set the preview to the correct message
            preview.NavigateUrl = Globals.UrlMessage + messageTemplates.SelectedItem.Value;

            // Now populate the subject/body with the appropriate values
            // read in the email template's subject/body
            ForumMessage message = ForumMessages.GetMessage(Convert.ToInt32(messageTemplates.SelectedItem.Value));

            // Set the values for subject/body
            title.Text = message.Title;
            body.Text = message.Body;
        }

        /***********************************************************************
        CreateChildControls Event Handler
        ---------------------------------
            This event handler adds the children controls.
        ************************************************************************/
        protected override void CreateChildControls() {

            // Attempt to load the control. If this fails, we're done
            try {
                controlTemplate = Page.LoadControl(Globals.ApplicationVRoot + "/skins/" + Globals.Skin + "/Skins/Skin-MessageAdmin.ascx");
            }
            catch (FileNotFoundException) {
                throw new Exception("The user control skins/Skins/Skin-MessageAdmin.ascx was not found. Please ensure this file exists in your skins directory");
            }

            // Initialize the user control being used as a template
            InitializeControlTemplate();

            this.Controls.Add(controlTemplate);

        }

    }
}
