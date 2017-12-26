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
using System.Data.SqlClient;

namespace UploadImages1
{
	/// <summary>
	/// ViewImages is used to display a list of the images in the database. Each 
	/// image is listed by description and is a link that will call GetImage, passing
	/// in the ID of the image.
	/// </summary>
	public class ViewImages : System.Web.UI.Page
	{
	
		private void Page_Load(object sender, System.EventArgs e)
		{
			/// <summary>
			/// Open a connection to the SQL Server </summary>
			SqlConnection Connection = new SqlConnection("server=localhost;uid=sa;database=ImageUpload;pwd=;");
			/// <summary>
			/// The SqlCommand retrieves a list of description and ImageID's from the Images table </summary>
			SqlCommand Command = new SqlCommand("Select ImageID, Description From Images", Connection);

			/// <summary>
			/// Open the connection in order to retrieve the record </summary>
			Connection.Open();
			/// <summary>
			/// Output header text
			Response.Write("<H2 align='center'><FONT color='blue'>Image List from SQL Server</FONT></H2><BR><BR>");
			/// <summary>
			/// Create a SqlDataReader that contains a list of the images in the database </summary>
			SqlDataReader myDR = Command.ExecuteReader();
			/// <summary>
			/// Loop through the records using the Read method </summary>
			while (myDR.Read())
			{
				/// <summary>
				/// Write out hyperlinks that show the descriptions and include the ImageID in a querystring </summary>
				Response.Write("<a href='GetImage.aspx?imageid=" + myDR["ImageID"] + "'>" + myDR["Description"] + "</a><br>");
			}
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN: This call is required by the ASP.NET Web Form Designer.
			//
			InitializeComponent();
			base.OnInit(e);
		}
		
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{    
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion
	}
}
