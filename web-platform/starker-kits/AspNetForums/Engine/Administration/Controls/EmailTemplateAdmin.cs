/******************************* EmailAdmin Web control *************************************
 * 
 * SUMMARY:
 *		This Web control displays the various email templates and allows the user (an admin)
 *		to select one at a time and edit the template.  These are the email templates used
 *		by the WebForums.Emails class to send out notification emails to people when various
 *		events occur.
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
using System.IO;


namespace AspNetForums.Controls.Admin {

    /// <summary>
    /// This Web control displays a list of the available email templates and allows 
    /// the administrator to edit these templates.
    /// </summary>
    [
        ParseChildren(true)
    ]
    public class EmailTemplateAdmin : WebControl, INamingContainer {
        Control controlTemplate;
        DropDownList emailTemplates;
        TextBox subject;
        TextBox body;
        Label status;

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
            emailTemplates = (DropDownList) controlTemplate.FindControl("emailTemplateList");
            emailTemplates.DataTextField = "Description";
            emailTemplates.DataValueField = "EmailTemplateID";
            emailTemplates.DataSource = Emails.GetEmailTemplateList();
            emailTemplates.DataBind();
            emailTemplates.AutoPostBack = true;
            emailTemplates.SelectedIndexChanged += new System.EventHandler(EmailTemplate_Changed);

            // Set up the button used to update the email template
            button = (Button) controlTemplate.FindControl("UpdateTemplate");
            button.Text = " Update Template ";
            button.Click += new System.EventHandler(UpdateTemplate_Click);

            // Find the status control
            status = (Label) controlTemplate.FindControl("Status");
            status.Visible = false;

            // Find the subject text box and the body textbox
            subject = (TextBox) controlTemplate.FindControl("Subject");
            body = (TextBox) controlTemplate.FindControl("Body");

            DisplayEditMode();
        }

        // ****************************************************************
        // UpdateTemplate_Click
        //
        /// <summary>
        /// Logic to handle updating the email message in the database when
        /// the user has made changes.
        /// </summary>
        // ****************************************************************
        private void UpdateTemplate_Click(Object sender, EventArgs e) {
            
            EmailTemplate email = new EmailTemplate();
            email.EmailTemplateID = Convert.ToInt32(emailTemplates.SelectedItem.Value);
            email.Subject = subject.Text;
            email.Body = body.Text;

            // update the email template
            Emails.UpdateEmailTemplate(email);

            // Display that we've updated
            status.Visible = true;
            status.Text = "Email template updated...";

        }
		
        // ****************************************************************
        // EmailTemplate_Changed
        //
        /// <summary>
        /// Raised when the drop down list of available messages changes
        /// </summary>
        // ****************************************************************
        private void EmailTemplate_Changed(Object sender, EventArgs e) {
            DisplayEditMode();
        }

        // ****************************************************************
        // DisplayEditMode
        //
        /// <summary>
        /// Retrieves the values for the subject and body from the database
        /// and dispalys them in textboxs for the user to edit
        /// </summary>
        // ****************************************************************
        private void DisplayEditMode() {

            // Now populate the subject/body with the appropriate values
            // read in the email template's subject/body
            EmailTemplate email = Emails.GetEmailTemplateInfo(Convert.ToInt32(emailTemplates.SelectedItem.Value));

            // Set the values for subject/body
            subject.Text = email.Subject;
            body.Text = email.Body;
        }

        /***********************************************************************
        CreateChildControls Event Handler
        ---------------------------------
            This event handler adds the children controls.
        ************************************************************************/
        protected override void CreateChildControls() {

            // Attempt to load the control. If this fails, we're done
            try {
                controlTemplate = Page.LoadControl(Globals.ApplicationVRoot + "/skins/" + Globals.Skin + "/Skins/Skin-Email.ascx");
            }
            catch (FileNotFoundException) {
                throw new Exception("The user control skins/Skins/Skin-Email.ascx was not found. Please ensure this file exists in your skins directory");
            }

            // Initialize the user control being used as a template
            InitializeControlTemplate();

            this.Controls.Add(controlTemplate);

        }

    }
}
