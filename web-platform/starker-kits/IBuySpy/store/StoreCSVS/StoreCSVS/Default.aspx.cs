using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;

namespace IBuySpy {

    public class CDefault : System.Web.UI.Page {

        protected System.Web.UI.WebControls.Label WelcomeMsg;
    
        public CDefault() {
            Page.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this page is used to personalize
        // the welcome message seen by returning IBuySpy users.
        // It does this by retrieving a client-side cookie
        // (persisted on the client in the Login.aspx and
        // register.aspx pages) and updating a label control.
        //
        //*******************************************************

        private void Page_Load(object sender, System.EventArgs e) {

            // Customize welcome message if personalization cookie is present
            if (Request.Cookies["IBuySpy_FullName"] != null) {
                WelcomeMsg.Text = "Welcome " + Request.Cookies["IBuySpy_FullName"].Value;
            }
        }

        private void Page_Init(object sender, EventArgs e) {
            //
            // CODEGEN: This call is required by the ASP.NET Web Form Designer.
            //
            InitializeComponent();
        }

		#region Web Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {    
            this.Load += new System.EventHandler(this.Page_Load);

        }
		#endregion

    }
}
