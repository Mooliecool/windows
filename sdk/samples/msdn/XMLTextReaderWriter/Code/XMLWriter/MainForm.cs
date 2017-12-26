using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Xml;

namespace XMLTextWriter
{
	/// <summary>
	/// Summary description for MainForm.
	/// </summary>
	public class MainForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox OutputFileName;
		private System.Windows.Forms.StatusBar statusBar1;
		private System.Windows.Forms.Button WriteFile;
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
			this.label1 = new System.Windows.Forms.Label();
			this.OutputFileName = new System.Windows.Forms.TextBox();
			this.statusBar1 = new System.Windows.Forms.StatusBar();
			this.WriteFile = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(16, 24);
			this.label1.Name = "label1";
			this.label1.TabIndex = 0;
			this.label1.Text = "File Name";
			// 
			// OutputFileName
			// 
			this.OutputFileName.Location = new System.Drawing.Point(144, 24);
			this.OutputFileName.Name = "OutputFileName";
			this.OutputFileName.Size = new System.Drawing.Size(464, 20);
			this.OutputFileName.TabIndex = 1;
			this.OutputFileName.Text = "customers.xml";
			// 
			// statusBar1
			// 
			this.statusBar1.Location = new System.Drawing.Point(0, 416);
			this.statusBar1.Name = "statusBar1";
			this.statusBar1.Size = new System.Drawing.Size(704, 22);
			this.statusBar1.TabIndex = 2;
			this.statusBar1.Text = "statusBar1";
			// 
			// WriteFile
			// 
			this.WriteFile.Location = new System.Drawing.Point(144, 56);
			this.WriteFile.Name = "WriteFile";
			this.WriteFile.TabIndex = 3;
			this.WriteFile.Text = "Write File";
			this.WriteFile.Click += new System.EventHandler(this.WriteFile_Click);
			// 
			// MainForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(704, 438);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.WriteFile,
																		  this.statusBar1,
																		  this.OutputFileName,
																		  this.label1});
			this.Name = "MainForm";
			this.Text = "MainForm";
			this.ResumeLayout(false);

		}
		#endregion

		static void Main() 
		{
			Application.Run(new MainForm());
		}
		private void WriteFile_Click(object sender, System.EventArgs e)
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
				XmlFile = directoryXML.FullName + "\\" + OutputFileName.Text;
			}
			else
			{
				XmlFile = directoryInfo.FullName + "\\" + OutputFileName.Text;

			}
			
			//create the xml text writer object by providing the filename to write to
			//and the desired encoding.  If the encoding is left null, then the writer
			//assumes UTF-8.
			XmlTextWriter XmlWtr = new System.Xml.XmlTextWriter(XmlFile,null);
			
			//set the formatting option of the xml file. The default indentation is 2 character spaces.
			//To change the default, use the Indentation property to set the number of IndentChars to use
			//and use the IndentChar property to set the character to use for indentation, such as the 
			//tab character. Here the default is used.
			XmlWtr.Formatting=Formatting.Indented;

			//begin to write the xml document. This creates the xml declaration with the version attribute
			//set to "1.0".
			XmlWtr.WriteStartDocument();

			//start the first element.
			XmlWtr.WriteStartElement("customers");
			
			//create our first customer element.
			//this is a child element of the customers element.
			XmlWtr.WriteStartElement("customer");
			
			//writes the entire element with the specified element name and
			//string value respectively.
			XmlWtr.WriteElementString("name", "Kevin Anders");
			XmlWtr.WriteElementString("phone", "555.555.5555");
			
			//end the customer element.
			XmlWtr.WriteEndElement();
			
			//create another customer.
			XmlWtr.WriteStartElement("customer");
			XmlWtr.WriteElementString("name", "Staci Richard");
			XmlWtr.WriteElementString("phone", "555.122.1552");
			//end the second customer element.
			XmlWtr.WriteEndElement();
			
			//end the customers element.
			XmlWtr.WriteEndElement();

			//now end the document.
			XmlWtr.WriteEndDocument();
			
			//now flush the contents of the stream.
			XmlWtr.Flush();
			
			//close the text writerj and write the xml file.
			XmlWtr.Close();

			statusBar1.Text = "Output file has been written";
		}
	}
}
