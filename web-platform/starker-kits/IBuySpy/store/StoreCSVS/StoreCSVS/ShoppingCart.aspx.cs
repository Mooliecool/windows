using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Data.SqlClient;
using System.Drawing;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;

namespace IBuySpy {

    public class ShoppingCart : System.Web.UI.Page {

        protected System.Web.UI.WebControls.Label MyError;
        protected System.Web.UI.WebControls.DataGrid MyList;
        protected System.Web.UI.WebControls.Label lblTotal;
        protected System.Web.UI.WebControls.ImageButton UpdateBtn;
        protected System.Web.UI.WebControls.ImageButton CheckoutBtn;
        protected System.Web.UI.WebControls.Panel DetailsPanel;

        public ShoppingCart() {
            Page.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this page is used to load the
        // ShoppingCart DataGrid *the first time* the page is
        // accessed.
        //
        // Note that subsequent postbacks to the page *do not*
        // reload the Datagrid.  Instead, we rely on the control's
        // built-in viewstate management to rebuild the control
        // on the server.
        //
        //*******************************************************

        private void Page_Load(object sender, System.EventArgs e) {

            // Populate the shopping cart the first time the page is accessed.
            if (Page.IsPostBack == false) {
                PopulateShoppingCartList();
            }
        }

        //*******************************************************
        //
        // The UpdateBtn_Click event is raised when a user clicks
        // the "update" button on the client.  The event handler
        // updates all items in the cart back to the database,
        // and then repopulates the datagrid with the current
        // cart contents.
        //
        //*******************************************************

        private void UpdateBtn_Click(object sender, System.Web.UI.ImageClickEventArgs e) {

            // Update the Shopping Cart and then Repopulate the List
            UpdateShoppingCartDatabase();
            PopulateShoppingCartList();
        }

        //*******************************************************
        //
        // The CheckoutBtn_Click event is raised when a user clicks
        // the "checkout" button on the client.  The event handler
        // updates all items in the cart back to the database,
        // and then redirects the user to the checkout.aspx page
        //
        //*******************************************************

        private void CheckoutBtn_Click(object sender, System.Web.UI.ImageClickEventArgs e) {

            // Update Shopping Cart
            UpdateShoppingCartDatabase();

            // If cart is not empty, proceed on to checkout page
            IBuySpy.ShoppingCartDB cart = new IBuySpy.ShoppingCartDB();

            // Calculate shopping cart ID
            String cartId = cart.GetShoppingCartId();

            // If the cart isn't empty, navigate to checkout page
            if (cart.GetItemCount(cartId) !=0) {
                Response.Redirect("Checkout.aspx");
            }
            else {
                MyError.Text = "You can't proceed to the Check Out page with an empty cart.";
            }
        }

        //*******************************************************
        //
        // The PopulateShoppingCartList helper method is used to
        // dynamically populate a GridControl with the contents of
        // the current user's shopping cart.
        //
        //*******************************************************

        void PopulateShoppingCartList() {

            IBuySpy.ShoppingCartDB cart = new IBuySpy.ShoppingCartDB();

            // Obtain current user's shopping cart ID
            String cartId = cart.GetShoppingCartId();

            // If no items, hide details and display message
            if (cart.GetItemCount(cartId) == 0) {
                DetailsPanel.Visible = false;
                MyError.Text = "There are currently no items in your shopping cart.";
            }
            else {

                // Databind Gridcontrol with Shopping Cart Items
                MyList.DataSource = cart.GetItems(cartId);
                MyList.DataBind();

                //Update Total Price Label
                lblTotal.Text = String.Format( "{0:c}", cart.GetTotal(cartId));
            }
        }

        //*******************************************************
        //
        // The UpdateShoppingCartDatabase helper method is used to
        // update a user's items within the shopping cart database
        // using client input from the GridControl.
        //
        //*******************************************************

        void UpdateShoppingCartDatabase() {

            IBuySpy.ShoppingCartDB cart = new IBuySpy.ShoppingCartDB();

            // Obtain current user's shopping cart ID
            String cartId = cart.GetShoppingCartId();

            // Iterate through all rows within shopping cart list
            for (int i=0; i < MyList.Items.Count; i++) {

                // Obtain references to row's controls
                TextBox quantityTxt = (TextBox) MyList.Items[i].FindControl("Quantity");
                CheckBox remove = (CheckBox) MyList.Items[i].FindControl("Remove");

                // Wrap in try/catch block to catch errors in the event that someone types in
                // an invalid value for quantity
                int quantity;
                try {
                    quantity = Int32.Parse(quantityTxt.Text);

                    // If the quantity field is changed or delete is checked
                    if (quantity != (int)MyList.DataKeys[i] || remove.Checked == true) {

                        Label lblProductID = (Label) MyList.Items[i].FindControl("ProductID");

                        if (quantity == 0 || remove.Checked == true) {
                            cart.RemoveItem(cartId, Int32.Parse(lblProductID.Text));
                        }
                        else {
                            cart.UpdateItem(cartId, Int32.Parse(lblProductID.Text),quantity);
                        }
                    }
                }
                catch {
                    MyError.Text = "There has been a problem with one or more of your inputs.";
                }
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
            this.UpdateBtn.Click += new System.Web.UI.ImageClickEventHandler(this.UpdateBtn_Click);
            this.CheckoutBtn.Click += new System.Web.UI.ImageClickEventHandler(this.CheckoutBtn_Click);
            this.Load += new System.EventHandler(this.Page_Load);

        }
        #endregion

    }
}
