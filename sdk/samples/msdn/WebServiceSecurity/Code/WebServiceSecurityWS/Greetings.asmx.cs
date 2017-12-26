using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Web;
using System.Web.Services;

namespace WebServiceSecurity
{
	/// <summary>
	/// Summary description for Service1.
	/// </summary>
	public class Greetings : System.Web.Services.WebService
	{
		public Greetings()
		{
			//CODEGEN: This call is required by the ASP.NET Web Services Designer
			InitializeComponent();
		}

		#region Component Designer generated code
		
		//Required by the Web Services Designer 
		private IContainer components = null;
				
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{

		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if(disposing && components != null)
			{
				components.Dispose();
			}
			base.Dispose(disposing);		
		}
		
		#endregion

		/// <summary>
		/// This is a simple Web Service that just implements the classic "Hello World"
		/// program. Note that no coding is required to make it secure. This will be done
		/// using the built-in capabilities of IIS and some client-side programming.
		/// </summary>
		[WebMethod]
		public string HelloWorld()
		{
			return "Hello World";
		}
	}
}
