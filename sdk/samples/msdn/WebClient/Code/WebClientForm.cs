using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Net;
using System.IO;


namespace WebClient
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class WebClientForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label Label1;
		private System.Windows.Forms.Label Label2;
		private System.Windows.Forms.TextBox targetURI;
		private System.Windows.Forms.Button GetTargetAtURI;
		private System.Windows.Forms.TextBox saveTargetToAddress;
		private System.Windows.Forms.Button SaveTextToTarget;
		private System.Windows.Forms.TextBox textOutput;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public WebClientForm()
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
			this.targetURI = new System.Windows.Forms.TextBox();
			this.Label1 = new System.Windows.Forms.Label();
			this.GetTargetAtURI = new System.Windows.Forms.Button();
			this.saveTargetToAddress = new System.Windows.Forms.TextBox();
			this.SaveTextToTarget = new System.Windows.Forms.Button();
			this.Label2 = new System.Windows.Forms.Label();
			this.textOutput = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// targetURI
			// 
			this.targetURI.Location = new System.Drawing.Point(8, 32);
			this.targetURI.Name = "targetURI";
			this.targetURI.Size = new System.Drawing.Size(888, 20);
			this.targetURI.TabIndex = 2;
			this.targetURI.Text = "http://";
			// 
			// Label1
			// 
			this.Label1.Location = new System.Drawing.Point(8, 16);
			this.Label1.Name = "Label1";
			this.Label1.Size = new System.Drawing.Size(104, 16);
			this.Label1.TabIndex = 3;
			this.Label1.Text = "Download Address:";
			// 
			// GetTargetAtURI
			// 
			this.GetTargetAtURI.Location = new System.Drawing.Point(904, 32);
			this.GetTargetAtURI.Name = "GetTargetAtURI";
			this.GetTargetAtURI.Size = new System.Drawing.Size(56, 24);
			this.GetTargetAtURI.TabIndex = 4;
			this.GetTargetAtURI.Text = "Go!";
			this.GetTargetAtURI.Click += new System.EventHandler(this.ButtonGo_Click);
			// 
			// saveTargetToAddress
			// 
			this.saveTargetToAddress.Location = new System.Drawing.Point(8, 616);
			this.saveTargetToAddress.Name = "saveTargetToAddress";
			this.saveTargetToAddress.Size = new System.Drawing.Size(880, 20);
			this.saveTargetToAddress.TabIndex = 5;
			this.saveTargetToAddress.Text = "";
			// 
			// SaveTextToTarget
			// 
			this.SaveTextToTarget.Location = new System.Drawing.Point(896, 616);
			this.SaveTextToTarget.Name = "SaveTextToTarget";
			this.SaveTextToTarget.Size = new System.Drawing.Size(56, 24);
			this.SaveTextToTarget.TabIndex = 6;
			this.SaveTextToTarget.Text = "Save";
			this.SaveTextToTarget.Click += new System.EventHandler(this.ButtonSave_Click);
			// 
			// Label2
			// 
			this.Label2.Location = new System.Drawing.Point(8, 592);
			this.Label2.Name = "Label2";
			this.Label2.Size = new System.Drawing.Size(96, 16);
			this.Label2.TabIndex = 7;
			this.Label2.Text = "Save To Address:";
			// 
			// textOutput
			// 
			this.textOutput.Location = new System.Drawing.Point(8, 64);
			this.textOutput.MaxLength = 50000;
			this.textOutput.Multiline = true;
			this.textOutput.Name = "textOutput";
			this.textOutput.ScrollBars = System.Windows.Forms.ScrollBars.Both;
			this.textOutput.Size = new System.Drawing.Size(992, 520);
			this.textOutput.TabIndex = 8;
			this.textOutput.Text = "";
			// 
			// WebClientForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(1008, 656);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.textOutput,
																		  this.Label2,
																		  this.SaveTextToTarget,
																		  this.saveTargetToAddress,
																		  this.GetTargetAtURI,
																		  this.targetURI,
																		  this.Label1});
			this.Name = "WebClientForm";
			this.Text = "Source Viewer";
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new WebClientForm());
		}

		private void ButtonGo_Click(object sender, System.EventArgs e)
		{
			string Address=targetURI.Text;

			if (Address!="")
			{
				//create an instance of the WebClient.
				System.Net.WebClient MyClient = new System.Net.WebClient();
				//create a stream object to get the stream from the OpenRead method.
				System.IO.Stream MyStream = MyClient.OpenRead(Address);
				//create a streamreader to parse through the stream.
				System.IO.StreamReader MyStreamReader = new System.IO.StreamReader(MyStream);
				
				string NewLine;
				
				//loop through the stream and put the output in the textbox.
				while((NewLine=MyStreamReader.ReadLine())!=null)
				{
					textOutput.AppendText(NewLine+"\r\n");
				}

				MyStream.Close();

			}
		}

		private void ButtonSave_Click(object sender, System.EventArgs e)
		{
			string SaveAddress=saveTargetToAddress.Text;

			if (SaveAddress!="")
			{
				//create an instance of the WebClient.
				System.Net.WebClient MyClient = new System.Net.WebClient();
				//create a stream object to hold the stream in the OpenWrite method.
				System.IO.Stream MyStream = MyClient.OpenWrite(SaveAddress, "PUT");
				//create a streamwriter to write the stream to the specified location.
				System.IO.StreamWriter MyStreamWriter = new System.IO.StreamWriter(MyStream);
				
				//grab the stream and write the output.
				MyStreamWriter.Write(textOutput.Text);
				
				//close the writer.
				MyStreamWriter.Close();

				MessageBox.Show("File has been created!");

			}		
		}
	}
}
