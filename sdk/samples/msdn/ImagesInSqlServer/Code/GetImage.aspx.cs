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
	/// GetImage is the file used to actually retrieve the image from SQL Server
	/// and display it in a browser.
	/// </summary>
	public class GetImage : System.Web.UI.Page
	{
		private void Page_Load(object sender, System.EventArgs e)
		{
			/// <summary>
			/// The ImageID variable holds the ImageID passed from ViewImages </summary>
			int ImageId = System.Convert.ToInt32(Request.QueryString["ImageId"]);

			/// <summary>
			/// imageContent will be used to hold the record returned from the database.
			/// This line calls the GetImages function. </summary>
			SqlDataReader imageContent = GetImages(ImageId);
			/// <summary>
			/// The Read method is used to load the record into memory </summary>
			imageContent.Read();

			/// <summary>
			/// The ImageType is retrieved from the database and sent to the browser as the
			/// ContentType. This tells the browser how to display the image. </summary>
			Response.ContentType = imageContent["ImageType"].ToString();
			/// <summary>
			/// The OutputStream.Write is used to write a byte array of the image, starting at
			/// the first byte and proceeding through the entire file. </summary>
			Response.OutputStream.Write((byte[])imageContent["ImageFile"], 0, System.Convert.ToInt32(imageContent["ImageSize"]));

			/// <summary>
			/// Call the End method to indicate to the browser that the entire image has been sent. </summary>
			Response.End();
		}

		/// <summary>
		/// GetImages is used to actually retrieve the record, based on the ImageId that is passed in.
		/// </summary>
		/// <param name="ImageId">ImageId is the ID of the image chosen on ViewImages.aspx.</param>
		/// <returns>GetImages returns a SqlDataReader, which is more efficient than a DataSet in this case.</returns>
		private SqlDataReader GetImages(int imageId)
		{
			/// <summary>
			/// Open a connection to the SQL Server </summary>
			SqlConnection Connection = new SqlConnection("server=localhost;uid=sa;database=ImageUpload");
			/// <summary>
			/// The SqlCommand retrieves a single record from the Images table </summary>
			SqlCommand Command = new SqlCommand("Select * From Images Where ImageId=@ImageId", Connection);

			/// <summary>
			/// Create a SqlParameter that will allow you to pass in the ImageId to the query </summary>
			SqlParameter imageIDParameter = new SqlParameter("@ImageId", SqlDbType.Int);
			/// <summary>
			/// Assign the ImageId to the parameter that was created </summary>
			imageIDParameter.Value = imageId;
			/// <summary>
			/// Add the Parameter to the SqlCommand's Parameters collection </summary>
			Command.Parameters.Add(imageIDParameter);
			/// <summary>
			/// Open the connection in order to retrieve the record </summary>
			Connection.Open();

			/// <summary>
			/// Return a SqlDataReader to the calling procedure. Notice that the connection will
			/// be closed afer the record has been read </summary>
			return Command.ExecuteReader(CommandBehavior.CloseConnection);

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
