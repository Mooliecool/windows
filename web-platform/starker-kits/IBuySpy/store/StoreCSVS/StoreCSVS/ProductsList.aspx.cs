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

    public class ProductsList : System.Web.UI.Page {

        protected System.Web.UI.WebControls.DataList MyList;
    
        public ProductsList() {
            Page.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this page is used to obtain
        // from a database a collection of all products within a
        // specified product category.  The collection is then
        // databound to a templated asp:datalist control.
        //
        // The product category to obtain is specified using
        // a querystring argument to the page.
        //
        // Note that this page is output cached at 1 hour
        // intervals.  This eliminates the need to hit the database
        // on each request to the page.
        //
        //*******************************************************

        private void Page_Load(object sender, System.EventArgs e) {

            // Obtain categoryId from QueryString
            int categoryId = Int32.Parse(Request.Params["CategoryID"]);

            // Obtain products and databind to an asp:datalist control
            IBuySpy.ProductsDB productCatalogue = new IBuySpy.ProductsDB();
        
            MyList.DataSource = productCatalogue.GetProducts(categoryId);
            MyList.DataBind();
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
