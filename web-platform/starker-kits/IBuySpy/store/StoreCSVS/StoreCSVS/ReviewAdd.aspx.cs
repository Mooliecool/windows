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

    public class ReviewAdd : System.Web.UI.Page {

        protected System.Web.UI.WebControls.Label ModelName;
        protected System.Web.UI.WebControls.TextBox Name;
        protected System.Web.UI.WebControls.RequiredFieldValidator RequiredFieldValidator1;
        protected System.Web.UI.WebControls.TextBox Email;
        protected System.Web.UI.WebControls.RequiredFieldValidator RequiredFieldValidator2;
        protected System.Web.UI.WebControls.RadioButtonList Rating;
        protected System.Web.UI.WebControls.TextBox Comment;
        protected System.Web.UI.WebControls.RequiredFieldValidator RequiredFieldValidator3;
        protected System.Web.UI.WebControls.ImageButton ReviewAddBtn;
    
        public ReviewAdd() {
            Page.Init += new System.EventHandler(Page_Init);
        }

        //*******************************************************
        //
        // The Page_Load event on this page is used to fetch details
        // about the product to review.  It then updates UI elements
        // with the results.
        //
        // Note that the product to review is specified using
        // a querystring argument to the page.
        //
        //*******************************************************
           
        private void Page_Load(object sender, System.EventArgs e) {

            if (Page.IsPostBack != true) {

                // Obtain ProductID of Product to Review
                int productID = Int32.Parse(Request["productID"]);
        
                // Populate Product Name on Page
                IBuySpy.ProductsDB products = new IBuySpy.ProductsDB();
                ModelName.Text = products.GetProductDetails(productID).ModelName;
            
                // Store ProductID in Page State to use on PostBack
                ViewState["productID"] = productID;
            }
        }

        //*******************************************************
        //
        // The ReviewAddBtn_Click event is used to add a new
        // review into the IBuySpy Reviews database.
        //
        // Note that we are deliberately HtmlEncoding all text
        // values *before* adding them to the database.  This allows
        // us to prevent hackers from adding images or hyperlinks
        // into the message content.
        //
        //*******************************************************

        private void ReviewAddBtn_Click(object sender, System.Web.UI.ImageClickEventArgs e) {

            // Only add the review if all fields on the page are valid
            if (Page.IsValid == true) {
              
                // Obtain ProductID from Page State
                int productID = (int) ViewState["productID"];
            
                // Obtain Rating number of RadioButtonList
                int rating = Int32.Parse(Rating.SelectedItem.Value);
             
                // Add Review to ReviewsDB.  HtmlEncode before entry
                IBuySpy.ReviewsDB review = new IBuySpy.ReviewsDB();
                review.AddReview(productID, Server.HtmlEncode(Name.Text), Server.HtmlEncode(Email.Text), rating, Server.HtmlEncode(Comment.Text));
              
                // Redirect client back to the originating product details page
                Response.Redirect("ProductDetails.aspx?ProductID=" + productID);
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
            this.ReviewAddBtn.Click += new System.Web.UI.ImageClickEventHandler(this.ReviewAddBtn_Click);
            this.Load += new System.EventHandler(this.Page_Load);

        }
		#endregion

    }
}
