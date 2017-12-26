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

    public abstract class C_PopularItems : System.Web.UI.UserControl {

        protected System.Web.UI.WebControls.Repeater productList;
        public int ProductID;

        public C_PopularItems() {
            this.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this page is used to obtain
        // from a database a list of all popular items this
        // weel and then databind it to an asp:datalist control.
        //
        // To optimize performance, this user control is output 
        // cached for a period of 1 hour. 
        //
        //*******************************************************
        private void Page_Load(object sender, System.EventArgs e) {

            // Obtain list of favorite items
            IBuySpy.ProductsDB products = new IBuySpy.ProductsDB();
         
            // Databind and display the list of favorite product items
            productList.DataSource = products.GetMostPopularProductsOfWeek();
            productList.DataBind();

            // Hide the list if no items are in it
            if (productList.Items.Count == 0) {
                productList.Visible = false;
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
