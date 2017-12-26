using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace WebServiceSecurityClient
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.TextBox WebServiceResults;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox Password;
		private System.Windows.Forms.TextBox UserName;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form1()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.button1 = new System.Windows.Forms.Button();
			this.WebServiceResults = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.Password = new System.Windows.Forms.TextBox();
			this.UserName = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(144, 128);
			this.button1.Name = "button1";
			this.button1.TabIndex = 6;
			this.button1.TabStop = false;
			this.button1.Text = "Go";
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// WebServiceResults
			// 
			this.WebServiceResults.Location = new System.Drawing.Point(72, 16);
			this.WebServiceResults.Name = "WebServiceResults";
			this.WebServiceResults.Size = new System.Drawing.Size(136, 20);
			this.WebServiceResults.TabIndex = 2;
			this.WebServiceResults.TabStop = false;
			this.WebServiceResults.Text = "";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(8, 16);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(38, 13);
			this.label1.TabIndex = 3;
			this.label1.Text = "Output";
			// 
			// Password
			// 
			this.Password.Location = new System.Drawing.Point(72, 80);
			this.Password.Name = "Password";
			this.Password.PasswordChar = '*';
			this.Password.Size = new System.Drawing.Size(136, 20);
			this.Password.TabIndex = 1;
			this.Password.Text = "";
			// 
			// UserName
			// 
			this.UserName.Location = new System.Drawing.Point(72, 48);
			this.UserName.Name = "UserName";
			this.UserName.Size = new System.Drawing.Size(136, 20);
			this.UserName.TabIndex = 0;
			this.UserName.Text = "";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(8, 80);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(54, 13);
			this.label2.TabIndex = 5;
			this.label2.Text = "Password";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(8, 48);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(61, 13);
			this.label3.TabIndex = 4;
			this.label3.Text = "User Name";
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(232, 166);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.label3,
																		  this.label2,
																		  this.UserName,
																		  this.Password,
																		  this.label1,
																		  this.WebServiceResults,
																		  this.button1});
			this.Name = "Form1";
			this.Text = "Form1";
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// void Main merely loads the form.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		/// <summary>
		/// The button1_Click event handler runs when the button is clicked. The reference to the
		/// Greetings.asmx Web Service has already been set, so in this procedure you will 
		/// instantiate the Web Service and also create a NetworkCredential object.
		/// </summary>

		private void button1_Click(object sender, System.EventArgs e)
		{
			/// <summary>
			/// The NetworkCredential class is used to instantiate an object named myCred.
			/// This object will be passed to the Credentials property of the Web Service 
			/// object you create.</summary>
			System.Net.NetworkCredential myCred = new System.Net.NetworkCredential();
			/// <summary>
			/// myGreeting is the object you create that points to the Greetings Web Service.
			/// You may need to create a new Web Reference and change this line if the Web Service
			/// is not running on a local machine.</summary>
			localhost.Greetings myGreeting = new localhost.Greetings();
			/// <summary>
			/// Setting the UserName property of the NetworkCredential object. </summary>
			myCred.UserName= UserName.Text;
			/// <summary>
			/// Setting the Password property of the NetworkCredential object. </summary>
			myCred.Password= Password.Text;
			/// <summary>
			/// Here you set the Credentials property of the Web Service variable to the 
			/// NetworkCredential object you created earlier.</summary>
			myGreeting.Credentials=myCred;
			/// <summary>
			/// After the Credentials property is set, you call the Web Service method as you normally
			/// would, and the credential information is passed as part of the SOAP header.</summary>
			WebServiceResults.Text=myGreeting.HelloWorld();
		}
	}
}
