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

    public class SearchResults : System.Web.UI.Page {

        protected System.Web.UI.WebControls.DataList MyList;
        protected System.Web.UI.WebControls.Label ErrorMsg;
    
        public SearchResults() {
            Page.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this page is used to obtain
        // from a database a collection of all products whose
        // description or name meets a specified search criteria.
        //
        // Note that the search string to use is specified using
        // a querystring argument to the page.
        //
        //*******************************************************

        private void Page_Load(object sender, System.EventArgs e) {

            // Search database using the supplied "txtSearch" parameter
            IBuySpy.ProductsDB productCatalogue = new IBuySpy.ProductsDB();
        
            MyList.DataSource = productCatalogue.SearchProductDescriptions(Request.Params["txtSearch"]);
            MyList.DataBind();

            // Display a message if no results are found
            if (MyList.Items.Count == 0) {
                ErrorMsg.Text = "No items matched your query.";
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
