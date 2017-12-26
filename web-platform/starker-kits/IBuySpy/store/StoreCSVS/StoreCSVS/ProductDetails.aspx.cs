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

    public class ProductDetailsPage : System.Web.UI.Page {

        protected System.Web.UI.WebControls.Label ModelName;
        protected System.Web.UI.WebControls.Image ProductImage;
        protected System.Web.UI.WebControls.Label UnitCost;
        protected System.Web.UI.WebControls.Label ModelNumber;
        protected System.Web.UI.WebControls.HyperLink addToCart;
        protected System.Web.UI.WebControls.Label desc;
        protected C_ReviewList ReviewList;
        protected C_AlsoBought AlsoBoughtList;
    
        public ProductDetailsPage() {
            Page.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this page is used to obtain
        // product information from a database and then update
        // UI elements with them.
        //
        // Note that this page is output cached at 1 minute
        // intervals.  This eliminates the need to hit the database
        // on each request to the page.
        //
        //*******************************************************

        private void Page_Load(object sender, System.EventArgs e) {

            // Obtain ProductID from QueryString
            int ProductID = Int32.Parse(Request.Params["ProductID"]);

            // Obtain Product Details
            IBuySpy.ProductsDB products = new IBuySpy.ProductsDB();
            IBuySpy.ProductDetails myProductDetails = products.GetProductDetails(ProductID);

            // Update Controls with Product Details
            desc.Text = myProductDetails.Description;
            UnitCost.Text = String.Format("{0:c}", myProductDetails.UnitCost);
            ModelName.Text = myProductDetails.ModelName;
            ModelNumber.Text = myProductDetails.ModelNumber.ToString();
            ProductImage.ImageUrl = "ProductImages/" + myProductDetails.ProductImage;
            addToCart.NavigateUrl = "AddToCart.aspx?ProductID=" + ProductID;
            ReviewList.ProductID = ProductID;
            AlsoBoughtList.ProductID = ProductID;
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
