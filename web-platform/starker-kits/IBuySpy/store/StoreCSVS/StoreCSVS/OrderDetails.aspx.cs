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

    public class OrderDetailsPage : System.Web.UI.Page {

        protected System.Web.UI.WebControls.Label lblOrderNumber;
        protected System.Web.UI.WebControls.Label lblOrderDate;
        protected System.Web.UI.WebControls.Label lblShipDate;
        protected System.Web.UI.WebControls.DataGrid GridControl1;
        protected System.Web.UI.WebControls.Label MyError;
        protected System.Web.UI.HtmlControls.HtmlTable detailsTable;
        protected System.Web.UI.WebControls.Label lblTotal;

        public OrderDetailsPage() {
            Page.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this page is used to obtain
        // order information from a database and then update
        // UI elements with them.
        //
        //*******************************************************

        private void Page_Load(object sender, System.EventArgs e) {

            // Obtain Order ID from QueryString
            int OrderID = Int32.Parse(Request.Params["OrderID"]);

            // Get the customer ID too
            string CustomerId = User.Identity.Name;

            // Obtain Order Details from Database
            IBuySpy.OrdersDB orderHistory = new IBuySpy.OrdersDB();
            IBuySpy.OrderDetails myOrderDetails = orderHistory.GetOrderDetails(OrderID, CustomerId);

            // if order was found, display it
            if (myOrderDetails != null) {

                // Bind Items to GridControl
                GridControl1.DataSource = myOrderDetails.OrderItems;
                GridControl1.DataBind();

                // Update labels with summary details
                lblTotal.Text = String.Format( "{0:c}", myOrderDetails.OrderTotal);
                lblOrderNumber.Text = OrderID.ToString();
                lblOrderDate.Text = myOrderDetails.OrderDate.ToShortDateString();
                lblShipDate.Text = myOrderDetails.ShipDate.ToShortDateString();
            }
            // otherwise display an error message
            else {

                MyError.Text = "Order not found!";
                detailsTable.Visible = false;
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
