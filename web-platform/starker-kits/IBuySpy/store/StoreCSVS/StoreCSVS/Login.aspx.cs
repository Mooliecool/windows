using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;

namespace IBuySpy {

    public class Login : System.Web.UI.Page {

        protected System.Web.UI.WebControls.Label Message;
        protected System.Web.UI.WebControls.TextBox email;
        protected System.Web.UI.WebControls.RequiredFieldValidator emailRequired;
        protected System.Web.UI.WebControls.RegularExpressionValidator emailValid;
        protected System.Web.UI.WebControls.TextBox password;
        protected System.Web.UI.WebControls.RequiredFieldValidator passwordRequired;
        protected System.Web.UI.WebControls.CheckBox RememberLogin;
        protected System.Web.UI.WebControls.ImageButton LoginBtn;
    
        public Login() {
            Page.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The LoginBtn_Click event is used on this page to
        // authenticate a customer's supplied username/password
        // credentials against a database.
        //
        // If the supplied username/password are valid, then
        // the event handler adds a cookie to the client
        // (so that we can personalize the home page's welcome
        // message), migrates any items stored in the user's
        // temporary (non-persistent) shopping cart to their
        // permanent customer account, and then redirects the browser
        // back to the originating page.
        //
        //*******************************************************

        private void LoginBtn_Click(object sender, System.Web.UI.ImageClickEventArgs e) {

            // Only attempt a login if all form fields on the page are valid
            if (Page.IsValid == true) {

                // Save old ShoppingCartID
                IBuySpy.ShoppingCartDB shoppingCart = new IBuySpy.ShoppingCartDB();
                String tempCartID = shoppingCart.GetShoppingCartId();

                // Attempt to Validate User Credentials using CustomersDB
                IBuySpy.CustomersDB accountSystem = new IBuySpy.CustomersDB();
                String customerId = accountSystem.Login(email.Text, password.Text);

                if (customerId != null) {

                    // Migrate any existing shopping cart items into the permanent shopping cart
                    shoppingCart.MigrateCart(tempCartID, customerId);

                    // Lookup the customer's full account details
                    IBuySpy.CustomerDetails customerDetails = accountSystem.GetCustomerDetails(customerId);

                    // Store the user's fullname in a cookie for personalization purposes
                    Response.Cookies["IBuySpy_FullName"].Value = customerDetails.FullName;

                    // Make the cookie persistent only if the user selects "persistent" login checkbox
                    if (RememberLogin.Checked == true) {
                        Response.Cookies["IBuySpy_FullName"].Expires = DateTime.Now.AddMonths(1);
                    }

                    // Redirect browser back to originating page
                    FormsAuthentication.RedirectFromLoginPage(customerId, RememberLogin.Checked);
                }
                else {
                    Message.Text = "Login Failed!";
                }
            }
        }

        private void Page_Load(object sender, System.EventArgs e) {
            // Put user code to initialize the page here
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
            this.LoginBtn.Click += new System.Web.UI.ImageClickEventHandler(this.LoginBtn_Click);
            this.Load += new System.EventHandler(this.Page_Load);

        }
		#endregion

    }
}
