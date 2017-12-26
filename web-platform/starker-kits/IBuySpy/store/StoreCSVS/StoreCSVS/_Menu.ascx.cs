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

    public abstract class C_Menu : System.Web.UI.UserControl {

        protected System.Web.UI.WebControls.DataList MyList;
    
        public C_Menu() {
            this.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this page is used to obtain
        // from a database a list of all product categories
        // and databind it to an asp:datalist control.
        //
        // To optimize performance, this user control is output 
        // cached (varying based on the categoryId and selection
        // passed through the querystring.    
        //
        //*******************************************************

        private void Page_Load(object sender, System.EventArgs e) {

            // Set the curent selection of list
            String selectionId = Request.Params["selection"];

            if (selectionId != null) {
                MyList.SelectedIndex = Int32.Parse(selectionId);
            }

            // Obtain list of menu categories and databind to list control
            IBuySpy.ProductsDB products = new IBuySpy.ProductsDB();
        
            MyList.DataSource = products.GetProductCategories();
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
