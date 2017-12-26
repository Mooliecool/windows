using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;

namespace IBuySpy {

    public class AddToCart : System.Web.UI.Page {

        public AddToCart() {
            Page.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this page is used to add the
        // identified product to the user's shopping cart, and then immediately
        // redirect to the shoppingcart page (this avoids problems were a user hits 
        // "refresh" and accidentally adds another product to the cart)  
        //    
        // The product to add to the cart is specified using
        // a querystring argument to the page.
        //
        //*******************************************************

        private void Page_Load(object sender, System.EventArgs e) {
                      
            if (Request.Params["ProductID"] != null) {
        
                IBuySpy.ShoppingCartDB cart = new IBuySpy.ShoppingCartDB();
            
                // Obtain current user's shopping cart ID  
                String cartId = cart.GetShoppingCartId();   
            
                // Add Product Item to Cart
                cart.AddItem(cartId, Int32.Parse(Request.Params["ProductID"]), 1);
            }  
        
            Response.Redirect("ShoppingCart.aspx");
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
