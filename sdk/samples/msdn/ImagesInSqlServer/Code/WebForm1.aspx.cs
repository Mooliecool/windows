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
	/// WebForm1 allows you to upload images into the Images table. It uses standard
	/// HTML upload methods to get the file to the server.
	/// </summary>
	public class WebForm1 : System.Web.UI.Page
	{
		protected System.Web.UI.WebControls.TextBox imageDescription;
		protected System.Web.UI.WebControls.Button UploadImage;
		protected System.Web.UI.HtmlControls.HtmlInputFile ImageToUpload;
	
		private void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here
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
			this.UploadImage.Click += new System.EventHandler(this.UploadImage_Click);
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion

		private void UploadImage_Click(object sender, System.EventArgs e)
		{
			/// <summary>
			/// ContentType holds the content type of the file that was added to the ImageToUpload
			/// control, and HTML input control. </summary>
         string ContentType = ImageToUpload.PostedFile.ContentType;
			/// <summary>
			/// Length holds the size of the file that was added to the ImageToUpload control </summary>
			int Length = System.Convert.ToInt32(ImageToUpload.PostedFile.InputStream.Length);

			/// <summary>
			/// Content will hold the image. It is a byte array of size Length </summary>
			byte[] Content = new byte[Length];

			/// <summary>
			/// The Read method is used to read the file from the ImageToUpload control </summary>
			ImageToUpload.PostedFile.InputStream.Read(Content,0,Length);

			/// <summary>
			/// Open a connection to the SQL Server </summary>
			SqlConnection Connection = new SqlConnection("server=localhost;uid=sa;pwd=;database=ImageUpload");
			/// <summary>
			/// The SqlCommand will be used to insert the image into the Images table </summary>
			SqlCommand Command = new SqlCommand("INSERT Into Images(Description, ImageFile, ImageSize, ImageType) Values(@Description, @ImageFile, @ImageSize, @ImageType)", Connection);

			/// <summary>
			/// The Description parameter is used to add the image file description to the database
			SqlParameter imageDescriptionParameter = new SqlParameter("@Description", SqlDbType.NVarChar);
			imageDescriptionParameter.Value = imageDescription.Text;
			Command.Parameters.Add(imageDescriptionParameter);

			/// <summary>
			/// The ImageFile parameter is used to add the image file to the database
			SqlParameter imageFileParameter = new SqlParameter("@ImageFile", SqlDbType.Image);
			imageFileParameter.Value = Content;
			Command.Parameters.Add(imageFileParameter);

			/// <summary>
			/// The ImageSize parameter is used to add the image file size to the database
			SqlParameter imageSizeParameter = new SqlParameter("@ImageSize", SqlDbType.Int);
			imageSizeParameter.Value = Length;
			Command.Parameters.Add(imageSizeParameter);

			/// <summary>
			/// The ImageType parameter is used to add the image file type to the database
	      SqlParameter imageTypeParameter = new SqlParameter("@ImageType", SqlDbType.NVarChar);
	      imageTypeParameter.Value = ContentType;
	      Command.Parameters.Add(imageTypeParameter);

			/// <summary>
			/// Open the connection in order to retrieve the record </summary>
			Connection.Open();
			/// <summary>
			/// The SQL statement is executed. ExecuteNonQuery is used since no records
			/// will be returned. </summary>
			Command.ExecuteNonQuery();
			/// <summary>
			/// The connection is closed </summary>
			Connection.Close();
		
		}
	}
}
