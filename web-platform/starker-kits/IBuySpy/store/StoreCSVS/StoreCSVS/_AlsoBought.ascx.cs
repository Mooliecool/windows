using System;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;

namespace IBuySpy {

    public abstract class C_AlsoBought : System.Web.UI.UserControl {

        protected System.Web.UI.WebControls.Repeater alsoBoughtList;
        public int ProductID;

        public C_AlsoBought() {
            this.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this page is used to obtain
        // from a database a collection of other products
        // that customers who purchased a product "also bought".
        //
        //*******************************************************
        private void Page_Load(object sender, System.EventArgs e) {

            // Obtain list of products that people who "also bought" an item have purchased.  Databind to list control
            IBuySpy.ProductsDB productCatalogue = new IBuySpy.ProductsDB();
        
            alsoBoughtList.DataSource = productCatalogue.GetProductsAlsoPurchased(ProductID);
            alsoBoughtList.DataBind();
                
            // Hide the list if no items are in it
            if (alsoBoughtList.Items.Count == 0) {
                alsoBoughtList.Visible = false;
            }
        }

        private void Page_Init(object sender, EventArgs e) {
            //
            // CODEGEN: This call is required by the ASP.NET Web Form Designer.
            //
            InitializeComponent();
        }

		#region Web Form Designer generated code
        ///		Required method for Designer support - do not modify
        ///		the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this.Load += new System.EventHandler(this.Page_Load);

        }
		#endregion
    }
}
