using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Xml;

namespace XMLTextReader
{
	/// <summary>
	/// Summary description for MainForm.
	/// </summary>
	public class MainForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label fileName;
		private System.Windows.Forms.TextBox XMLOutput;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public MainForm()
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
				if(components != null)
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
			this.fileName = new System.Windows.Forms.Label();
			this.XMLOutput = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// fileName
			// 
			this.fileName.AutoSize = true;
			this.fileName.Location = new System.Drawing.Point(24, 16);
			this.fileName.Name = "fileName";
			this.fileName.Size = new System.Drawing.Size(47, 13);
			this.fileName.TabIndex = 1;
			this.fileName.Text = "fileName";
			// 
			// XMLOutput
			// 
			this.XMLOutput.Anchor = ((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right);
			this.XMLOutput.Location = new System.Drawing.Point(8, 48);
			this.XMLOutput.Multiline = true;
			this.XMLOutput.Name = "XMLOutput";
			this.XMLOutput.Size = new System.Drawing.Size(776, 312);
			this.XMLOutput.TabIndex = 2;
			this.XMLOutput.Text = "";
			// 
			// MainForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(792, 374);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.XMLOutput,
																		  this.fileName});
			this.Name = "MainForm";
			this.Text = "MainForm";
			this.Load += new System.EventHandler(this.MainForm_Load);
			this.ResumeLayout(false);

		}
		#endregion

		static void Main() 
		{
			Application.Run(new MainForm());
		}
		private void MainForm_Load(object sender, System.EventArgs e)
		{
			string XmlFile;
			System.IO.DirectoryInfo directoryInfo;
			System.IO.DirectoryInfo directoryXML;
			
			//Get the applications startup path
			directoryInfo = System.IO.Directory.GetParent(Application.StartupPath);

			//Set the output path
			if (directoryInfo.Name.ToString() == "bin")
			{
				directoryXML = System.IO.Directory.GetParent(directoryInfo.FullName);
				XmlFile = directoryXML.FullName + "\\customers.xml";
			}
			else
			{
				XmlFile = directoryInfo.FullName + "\\customers.xml";

			}
            fileName.Text = XmlFile;

			//load the xml file into the XmlTextReader object.

			XmlTextReader XmlRdr = new System.Xml.XmlTextReader(XmlFile);
			
			//while moving through the xml document.
			while(XmlRdr.Read())
			{
				//check the node type and look for the element type
				//whose Name property is equal to name.
				if (XmlRdr.NodeType==XmlNodeType.Element&&XmlRdr.Name=="name")
				{
					XMLOutput.Text += XmlRdr.ReadString() + "\r\n";

				}
			}
		}
	}
}
