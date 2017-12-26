using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;

namespace WindowsApplication27
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button CreateFileNoErrorHandling;
		private System.Windows.Forms.Button CreateFileWithErrorHandling;
		private System.Windows.Forms.StatusBar statusBar1;
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
			this.CreateFileNoErrorHandling = new System.Windows.Forms.Button();
			this.CreateFileWithErrorHandling = new System.Windows.Forms.Button();
			this.statusBar1 = new System.Windows.Forms.StatusBar();
			this.SuspendLayout();
			// 
			// CreateFileNoErrorHandling
			// 
			this.CreateFileNoErrorHandling.Location = new System.Drawing.Point(128, 48);
			this.CreateFileNoErrorHandling.Name = "CreateFileNoErrorHandling";
			this.CreateFileNoErrorHandling.Size = new System.Drawing.Size(112, 23);
			this.CreateFileNoErrorHandling.TabIndex = 0;
			this.CreateFileNoErrorHandling.Text = "No Error Handling";
			this.CreateFileNoErrorHandling.Click += new System.EventHandler(this.CreateFileNoErrorHandling_Click);
			// 
			// CreateFileWithErrorHandling
			// 
			this.CreateFileWithErrorHandling.Location = new System.Drawing.Point(304, 48);
			this.CreateFileWithErrorHandling.Name = "CreateFileWithErrorHandling";
			this.CreateFileWithErrorHandling.Size = new System.Drawing.Size(112, 23);
			this.CreateFileWithErrorHandling.TabIndex = 1;
			this.CreateFileWithErrorHandling.Text = "With Error Handling";
			this.CreateFileWithErrorHandling.Click += new System.EventHandler(this.CreateFileWithErrorHandling_Click);
			// 
			// statusBar1
			// 
			this.statusBar1.Location = new System.Drawing.Point(0, 232);
			this.statusBar1.Name = "statusBar1";
			this.statusBar1.Size = new System.Drawing.Size(608, 22);
			this.statusBar1.TabIndex = 2;
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(608, 254);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.statusBar1,
																		  this.CreateFileWithErrorHandling,
																		  this.CreateFileNoErrorHandling});
			this.Name = "Form1";
			this.Text = "Code Access Security";
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// This application attempts to create a file in the root of the C: drive.
		/// If it is downloaded via HTTP, this will fail. The first button fails 
		/// ungracefully; the second button traps the error.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		private void CreateFileNoErrorHandling_Click(object sender, System.EventArgs e)
		{
			/// <summary>
			/// This single line simply attempts to create a file called test.txt in
			/// the root of the C: drive. If this is run after the assembly has been
			/// downloaded with HTTP, it will fail and .NET will display a rather
			/// verbose error message. </summary>
			System.IO.File.Create("c:\\test1.txt");
			statusBar1.Text = "File (test1.txt) Created Successfully";

		}

		private void CreateFileWithErrorHandling_Click(object sender, System.EventArgs e)
		{
			try
			{
				/// <summary>
				/// This single line simply attempts to create a file called test.txt in
				/// the root of the C: drive. If this is run after the assembly has been
				/// downloaded with HTTP, it will generate an error, but the error will be
				/// caught and handled by the catch portion of the try...catch block. </summary>
				System.IO.File.Create("c:\\test2.txt");
				statusBar1.Text = "File (test2.txt) Created Successfully";

			}
			catch (Exception ex)
			{
				/// <summary>
				/// If there was an error creating the file, display that message to the user.
				/// While the message might not be too friendly, this does show that you have
				/// generated a trappable error. </summary>
				statusBar1.Text = "The following error occurred " + ex.Message + " creating file (test2.txt)";
			}
		}
	}
}
