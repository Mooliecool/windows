using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Data.SqlClient;
using System.Drawing;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;

namespace IBuySpy {

    public class OrderList : System.Web.UI.Page {

        protected System.Web.UI.WebControls.Label MyError;
        protected System.Web.UI.WebControls.DataGrid MyList;
    
        public OrderList() {
            Page.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this page is used to obtain
        // from a database a collection of all orders placed
        // by the current customer.  The collection is then
        // databound to a templated asp:datalist control.
        //
        //*******************************************************

        private void Page_Load(object sender, System.EventArgs e) {

            String customerID = User.Identity.Name;

            // Obtain and bind a list of all orders ever placed by visiting customer
            IBuySpy.OrdersDB orderHistory = new IBuySpy.OrdersDB();
        
            MyList.DataSource = orderHistory.GetCustomerOrders(customerID);
            MyList.DataBind();

            // Hide the list and display a message if no orders have ever been made
            if (MyList.Items.Count == 0) {
                MyError.Text = "You have no orders to display.";
                MyList.Visible = false;
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
