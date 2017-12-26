using System;
using System.Data;
using System.Data.SqlClient;
using System.Drawing;
using System.Web;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;

namespace IBuySpy {

    public abstract class C_ReviewList : System.Web.UI.UserControl {

        protected System.Web.UI.WebControls.HyperLink AddReview;
        protected System.Web.UI.WebControls.DataList MyList;
        public int ProductID;

        /// <summary>
        public C_ReviewList() {
            this.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this user control is used to obtain
        // from a database a list of reviews about a specified
        // product and then databind it to an asp:datalist control.
        //
        //*******************************************************

        private void Page_Load(object sender, System.EventArgs e) {

            // Obtain and databind a list of all reviews of a product
            IBuySpy.ReviewsDB productReviews = new IBuySpy.ReviewsDB();
        
            MyList.DataSource = productReviews.GetReviews(ProductID);
            MyList.DataBind();

            // Update navigation link for users to add a new review
            AddReview.NavigateUrl = "ReviewAdd.aspx?productID=" + ProductID.ToString();
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
