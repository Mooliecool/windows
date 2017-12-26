using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
// The System.Xml namespace is required for processing Xml.
using System.Xml;

namespace CustomApplicationFiles
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label lblServer;
		private System.Windows.Forms.Label lblTimeout;
		private System.Windows.Forms.Label lblMaxRecords;
		private System.Windows.Forms.GroupBox grpGeneral;
		private System.Windows.Forms.GroupBox grpSearch;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.CheckBox loggingIsEnabled;
		private System.Windows.Forms.CheckBox toolBarIsVisible;
		private System.Windows.Forms.NumericUpDown maximumRecords;
		private System.Windows.Forms.NumericUpDown timeoutPeriod;
		private System.Windows.Forms.TextBox serverName;
		private System.Windows.Forms.Button LoadXmlTextReader;
		private System.Windows.Forms.Button LoadXmlDocument;
		private System.Windows.Forms.Button CreateConfigurationFile;
		private System.Windows.Forms.Button UpdateXmlDocument;
		private System.Windows.Forms.Button ClearFields;

		/// <summary>
		/// The configurationFile defines the name of the XML file that
		/// will be used throughout this sample to hold the data on the form.
		/// </summary>
		private String configurationFile = "ApplicationConfiguration.xml";

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
			this.lblServer = new System.Windows.Forms.Label();
			this.lblTimeout = new System.Windows.Forms.Label();
			this.lblMaxRecords = new System.Windows.Forms.Label();
			this.grpGeneral = new System.Windows.Forms.GroupBox();
			this.loggingIsEnabled = new System.Windows.Forms.CheckBox();
			this.toolBarIsVisible = new System.Windows.Forms.CheckBox();
			this.grpSearch = new System.Windows.Forms.GroupBox();
			this.maximumRecords = new System.Windows.Forms.NumericUpDown();
			this.timeoutPeriod = new System.Windows.Forms.NumericUpDown();
			this.serverName = new System.Windows.Forms.TextBox();
			this.LoadXmlTextReader = new System.Windows.Forms.Button();
			this.LoadXmlDocument = new System.Windows.Forms.Button();
			this.CreateConfigurationFile = new System.Windows.Forms.Button();
			this.UpdateXmlDocument = new System.Windows.Forms.Button();
			this.ClearFields = new System.Windows.Forms.Button();
			this.grpGeneral.SuspendLayout();
			this.grpSearch.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.maximumRecords)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.timeoutPeriod)).BeginInit();
			this.SuspendLayout();
			// 
			// lblServer
			// 
			this.lblServer.Location = new System.Drawing.Point(16, 24);
			this.lblServer.Name = "lblServer";
			this.lblServer.TabIndex = 0;
			this.lblServer.Text = "Server:";
			// 
			// lblTimeout
			// 
			this.lblTimeout.Location = new System.Drawing.Point(16, 48);
			this.lblTimeout.Name = "lblTimeout";
			this.lblTimeout.TabIndex = 1;
			this.lblTimeout.Text = "Timeout:";
			// 
			// lblMaxRecords
			// 
			this.lblMaxRecords.Location = new System.Drawing.Point(16, 72);
			this.lblMaxRecords.Name = "lblMaxRecords";
			this.lblMaxRecords.TabIndex = 2;
			this.lblMaxRecords.Text = "Max Records:";
			// 
			// grpGeneral
			// 
			this.grpGeneral.Controls.AddRange(new System.Windows.Forms.Control[] {
																					 this.loggingIsEnabled,
																					 this.toolBarIsVisible});
			this.grpGeneral.Location = new System.Drawing.Point(13, 8);
			this.grpGeneral.Name = "grpGeneral";
			this.grpGeneral.Size = new System.Drawing.Size(272, 80);
			this.grpGeneral.TabIndex = 0;
			this.grpGeneral.TabStop = false;
			this.grpGeneral.Text = "General";
			// 
			// loggingIsEnabled
			// 
			this.loggingIsEnabled.Location = new System.Drawing.Point(16, 48);
			this.loggingIsEnabled.Name = "loggingIsEnabled";
			this.loggingIsEnabled.Size = new System.Drawing.Size(136, 24);
			this.loggingIsEnabled.TabIndex = 2;
			this.loggingIsEnabled.Text = "Enable Logging";
			// 
			// toolBarIsVisible
			// 
			this.toolBarIsVisible.Location = new System.Drawing.Point(16, 24);
			this.toolBarIsVisible.Name = "toolBarIsVisible";
			this.toolBarIsVisible.Size = new System.Drawing.Size(136, 24);
			this.toolBarIsVisible.TabIndex = 1;
			this.toolBarIsVisible.Text = "Show Toolbar";
			this.toolBarIsVisible.CheckedChanged += new System.EventHandler(this.toolBarIsVisible_CheckedChanged);
			// 
			// grpSearch
			// 
			this.grpSearch.Controls.AddRange(new System.Windows.Forms.Control[] {
																					this.maximumRecords,
																					this.timeoutPeriod,
																					this.serverName,
																					this.lblMaxRecords,
																					this.lblServer,
																					this.lblTimeout});
			this.grpSearch.Location = new System.Drawing.Point(13, 96);
			this.grpSearch.Name = "grpSearch";
			this.grpSearch.Size = new System.Drawing.Size(272, 104);
			this.grpSearch.TabIndex = 1;
			this.grpSearch.TabStop = false;
			this.grpSearch.Text = "Search";
			// 
			// maximumRecords
			// 
			this.maximumRecords.Increment = new System.Decimal(new int[] {
																			 10,
																			 0,
																			 0,
																			 0});
			this.maximumRecords.Location = new System.Drawing.Point(120, 72);
			this.maximumRecords.Maximum = new System.Decimal(new int[] {
																		   1000,
																		   0,
																		   0,
																		   0});
			this.maximumRecords.Name = "maximumRecords";
			this.maximumRecords.Size = new System.Drawing.Size(72, 20);
			this.maximumRecords.TabIndex = 7;
			// 
			// timeoutPeriod
			// 
			this.timeoutPeriod.Increment = new System.Decimal(new int[] {
																			30,
																			0,
																			0,
																			0});
			this.timeoutPeriod.Location = new System.Drawing.Point(120, 48);
			this.timeoutPeriod.Maximum = new System.Decimal(new int[] {
																		  600,
																		  0,
																		  0,
																		  0});
			this.timeoutPeriod.Minimum = new System.Decimal(new int[] {
																		  30,
																		  0,
																		  0,
																		  0});
			this.timeoutPeriod.Name = "timeoutPeriod";
			this.timeoutPeriod.Size = new System.Drawing.Size(72, 20);
			this.timeoutPeriod.TabIndex = 6;
			this.timeoutPeriod.Value = new System.Decimal(new int[] {
																		30,
																		0,
																		0,
																		0});
			// 
			// serverName
			// 
			this.serverName.Location = new System.Drawing.Point(120, 24);
			this.serverName.Name = "serverName";
			this.serverName.Size = new System.Drawing.Size(144, 20);
			this.serverName.TabIndex = 3;
			this.serverName.Text = "";
			// 
			// LoadXmlTextReader
			// 
			this.LoadXmlTextReader.Location = new System.Drawing.Point(12, 312);
			this.LoadXmlTextReader.Name = "LoadXmlTextReader";
			this.LoadXmlTextReader.Size = new System.Drawing.Size(272, 23);
			this.LoadXmlTextReader.TabIndex = 2;
			this.LoadXmlTextReader.Text = "Load Configuration Data Using XmlTextReader";
			this.LoadXmlTextReader.Click += new System.EventHandler(this.LoadXmlTextReader_Click);
			// 
			// LoadXmlDocument
			// 
			this.LoadXmlDocument.Location = new System.Drawing.Point(12, 344);
			this.LoadXmlDocument.Name = "LoadXmlDocument";
			this.LoadXmlDocument.Size = new System.Drawing.Size(272, 23);
			this.LoadXmlDocument.TabIndex = 3;
			this.LoadXmlDocument.Text = "Load Configuration Data Using XmlDocument";
			this.LoadXmlDocument.Click += new System.EventHandler(this.btnLoadXmlDocument_Click);
			// 
			// CreateConfigurationFile
			// 
			this.CreateConfigurationFile.Location = new System.Drawing.Point(12, 248);
			this.CreateConfigurationFile.Name = "CreateConfigurationFile";
			this.CreateConfigurationFile.Size = new System.Drawing.Size(272, 23);
			this.CreateConfigurationFile.TabIndex = 4;
			this.CreateConfigurationFile.Text = "Create Configuration File Using XmlTextWriter";
			this.CreateConfigurationFile.Click += new System.EventHandler(this.CreateConfigurationFile_Click);
			// 
			// UpdateXmlDocument
			// 
			this.UpdateXmlDocument.Location = new System.Drawing.Point(12, 280);
			this.UpdateXmlDocument.Name = "UpdateXmlDocument";
			this.UpdateXmlDocument.Size = new System.Drawing.Size(272, 23);
			this.UpdateXmlDocument.TabIndex = 5;
			this.UpdateXmlDocument.Text = "Update Configuration File Using XmlDocument";
			this.UpdateXmlDocument.Click += new System.EventHandler(this.UpdateXmlDocument_Click);
			// 
			// ClearFields
			// 
			this.ClearFields.Location = new System.Drawing.Point(104, 208);
			this.ClearFields.Name = "ClearFields";
			this.ClearFields.TabIndex = 6;
			this.ClearFields.Text = "Clear Fields";
			this.ClearFields.Click += new System.EventHandler(this.ClearFields_Click);
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(296, 379);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.ClearFields,
																		  this.UpdateXmlDocument,
																		  this.CreateConfigurationFile,
																		  this.LoadXmlDocument,
																		  this.LoadXmlTextReader,
																		  this.grpSearch,
																		  this.grpGeneral});
			this.Name = "Form1";
			this.Text = "Custom Application Files Sample";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.grpGeneral.ResumeLayout(false);
			this.grpSearch.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.maximumRecords)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.timeoutPeriod)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		private void Form1_Load(object sender, System.EventArgs e)
		{
			// The following line determines if the configuration file exists.  If the file
			// does not exist the Update and Load buttons are disabled since there is no file
			// to update or load.
			if (!System.IO.File.Exists(configurationFile))
			{
				UpdateXmlDocument.Enabled = false;
				LoadXmlTextReader.Enabled = false;
				LoadXmlDocument.Enabled = false;
			}
		}

		/// <summary>
		/// When the btnClear Button control is clicked the data in all of the 
		/// the controls on the form is cleared.  The CheckBox controls are unchecked, the 
		/// TextBox control is cleared and the NumericUpDown controls are set back to
		/// their minimum values.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data.</param>
		private void ClearFields_Click(object sender, System.EventArgs e)
		{
			// Clear all of the controls.
			toolBarIsVisible.Checked = false;
			loggingIsEnabled.Checked = false;
			serverName.Text = "";
			// Set the NumericUpDown controls to their MinimumValue properties to
			// make sure they are within the acceptable range.
			timeoutPeriod.Value = timeoutPeriod.Minimum;
			maximumRecords.Value = maximumRecords.Minimum;
		}
		
		/// <summary>
		/// When the btnCreateConfigureFile Button control is clicked the XmlTextWriter class
		/// is used to create the XML configuration file.  Instead of using the values on the form, 
		/// default values will be used for every field.  The file is created in the directory 
		/// where the application is executed.  Therefore, when running in debug mode the 
		/// configuration file is created in the "bin\Debug" subdirectory.  If the configuration 
		/// file already exists in the directory it is automatically overwritten.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data.</param>
		private void CreateConfigurationFile_Click(object sender, System.EventArgs e)
		{
			// The resulting XML configuration file looks like this:
//			<?xml version="1.0" encoding="utf-8"?>		- Line 1
//			<configuration>														- Line 2
//				<general>																- Line 3
//					<toolbar>true</toolbar>								- Line 4
//					<logging>false</logging>							- Line 5
//				</general>															- Line 6
//				<search>																- Line 7
//					<server>Test Server</server>					- Line 8
//					<timeout>60</timeout>									- Line 9
//					<maxRecords>100</maxRecords>					- Line 10
//				</search>																- Line 11
//			</configuration>													- Line 12

			try
			{
				// Create an instance of the XmlTextWriter class by passing it the name of the file 
				// to write to and the XML encoding to be used.  "UTF-8" is the default encoding.  The
				// XmlTextWriter class allows for writing of XML to the configuration file.  It does this 
				// in a forward-only, non-cached manner.
				XmlTextWriter applicationConfiguration = new XmlTextWriter(configurationFile, System.Text.Encoding.UTF8);

				// Set the Formatting to Indented to make the XML easier to read.
				applicationConfiguration.Formatting = Formatting.Indented;
				
				// Start the Document. This creates Line 1 of the XML above.
				applicationConfiguration.WriteStartDocument();

				// Write the Configuration element (Line 2).
				applicationConfiguration.WriteStartElement("configuration");
				
				// Write the General child element (Line 3).
				applicationConfiguration.WriteStartElement("general");

				// Write the Toolbar and Logging elements (Lines 4 and 5).
				applicationConfiguration.WriteElementString("toolbar", "true");
				applicationConfiguration.WriteElementString("logging", "false");

				// Close the General element (Line 6).
				applicationConfiguration.WriteEndElement();

				// Write the Search child element (Line 7).
				applicationConfiguration.WriteStartElement("search");

				// Write the Server, Timeout, and MaxRecords elements (Lines 8, 9, and 10).
				applicationConfiguration.WriteElementString("server", "Test Server");
				applicationConfiguration.WriteElementString("timeout", "60");
				applicationConfiguration.WriteElementString("maxRecords", "100");

				// Close the Search element (Line 11).
				applicationConfiguration.WriteEndElement();

				// Close the Configuration element (Line 12).
				applicationConfiguration.WriteEndElement();

				// Close the Document.
				applicationConfiguration.WriteEndDocument();

				// Write the XML to the File and close the XmlTextWriter.
				applicationConfiguration.Close();

				// Now that the configuration file exists, enable the Update and Load buttons.
				UpdateXmlDocument.Enabled = true;
				LoadXmlTextReader.Enabled = true;
				LoadXmlDocument.Enabled = true;
			}

			catch (Exception exception)
			{
				MessageBox.Show("Exception: " + exception.ToString());
			}
		}

		/// <summary>
		/// When the btnUpdateXmlDocument Button control is clicked the XML in the configuration
		/// file is updated with the values on the form.  To update the configuration file the
		/// existing file is loaded via a XmlDocument class and then the InnerText of each
		/// Element is set to the values of the controls on the form.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data.</param>
		private void UpdateXmlDocument_Click(object sender, System.EventArgs e)
		{
			try
			{
				// Create an instance of the XmlDocument class.  The XmlDocument class provides read-write 
				// access to the entire configuration file.  Unlike the XmlTextWriter class, the XmlDocument
				// provides direct access to specific elements.
				XmlDocument applicationConfiguration = new XmlDocument();
				
				// Load the configuration file into the XmlDocument by passing it the name of the file.
				applicationConfiguration.Load(configurationFile);

				// To get at the value of each element it's necessary to create an XmlNode for each of the elements.
				// A XmlNode exposes an InnerText property that allows us to get to the text inside the element tags.
				// When using the XmlDocument a XPath expression is used to navigate to the individual nodes/elements.
				// A XPath expression is simply the path to the element starting from the root element down through
				// the children.
				XmlNode toolbarNode = applicationConfiguration.SelectSingleNode("configuration/general/toolbar");
				XmlNode loggingNode = applicationConfiguration.SelectSingleNode("configuration/general/logging");
				XmlNode serverNode = applicationConfiguration.SelectSingleNode("configuration/search/server");
				XmlNode timeoutNode = applicationConfiguration.SelectSingleNode("configuration/search/timeout");
				XmlNode recordsNode = applicationConfiguration.SelectSingleNode("configuration/search/maxRecords");

				// The InnerText property of an XmlNode contains the text within the element tags.
				// Since InnerText requires a string, the ToString() method is used for all the controls
				// that do not return a string value.
				toolbarNode.InnerText = toolBarIsVisible.Checked.ToString(); 
				loggingNode.InnerText = loggingIsEnabled.Checked.ToString(); 
				serverNode.InnerText = serverName.Text; 
				timeoutNode.InnerText = timeoutPeriod.Value.ToString(); 
				recordsNode.InnerText = maximumRecords.Value.ToString(); 

				// The Save method of the XmlDocument saves the XML to the specified XML file.
				applicationConfiguration.Save(configurationFile);
			}
			catch (Exception exception)
			{
				MessageBox.Show("Exception: " + exception.ToString());
			}
		}

		/// <summary>
		/// When the btnLoadXmlTextReader Button control is clicked a XmlTextReader is used 
		/// to read the XML in the configuration file and load the configuration data into
		/// the controls on the form.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data.</param>
		private void LoadXmlTextReader_Click(object sender, System.EventArgs e)
		{
			// Create an instance of the XmlTextReader class by passing it the name of the
			// configuration file.  The XmlTextReader class provides a fast, forward-only, 
			// read-only cursor of the XML Data contained in the configuration file.
			XmlTextReader applicationConfiguration = new XmlTextReader(configurationFile);

			try
			{
				// Loop through the XmlTextReader just like a cursor, by testing for the end of the file.
				while(!applicationConfiguration.EOF)
				{
					// The MoveToContent method of the XmlTextReader checks whether the current node is a 
					// content node (CDATA, Element, EndElement, etc...).  If it is not, the reader skips ahead to the 
					// next content node or end of file.
					// Since the values are contained in elements, we continue reading until we find a node that is
					// an element.
					if (applicationConfiguration.MoveToContent() == XmlNodeType.Element)
						// Once an element is found, set the appropriate control based on the name of that element.
						// Since there are parent elements (i.e., configuration, general, and search) the default
						// reads the next node.
						switch (applicationConfiguration.Name)
						{
							case "toolbar":
								toolBarIsVisible.Checked = Boolean.Parse(applicationConfiguration.ReadElementString());
								break;
							case "logging":
								loggingIsEnabled.Checked = Boolean.Parse(applicationConfiguration.ReadElementString());
								break;
							case "server":
								serverName.Text = applicationConfiguration.ReadElementString();
								break;
							case "timeout":
								timeoutPeriod.Value = int.Parse(applicationConfiguration.ReadElementString());
								break;
							case "maxRecords":
								maximumRecords.Value = int.Parse(applicationConfiguration.ReadElementString());
								break;
							default:
								applicationConfiguration.Read();
								break;
						}
					else
						applicationConfiguration.Read();
				}
			}
			catch (Exception exception)
			{
				MessageBox.Show("Exception: " + exception.ToString());
			}
			finally
			{
				// It is important that the XmlTextReader is always closed.  If not the file is left open
				// and access will be denied.
				if (applicationConfiguration != null)
					applicationConfiguration.Close();
			}
		}

		/// <summary>
		/// When the btnLoadXmlDocument Button control is clicked a XmlDocument is used to 
		/// read the XML in the configuration file and load the configuration data into the
		/// controls on the form.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data.</param>
		private void btnLoadXmlDocument_Click(object sender, System.EventArgs e)
		{
			try
			{
				// Create an instance of the XmlDocument class.  The XmlDocument class provides read-write 
				// access to the entire configuration file.  In addition, the XmlDocument allows us to select
				// specific nodes instead of going through the entire document like the XmlTextReader.
				XmlDocument applicationConfiguration = new XmlDocument();

				// Load the configuration file into the XmlDocument.
				applicationConfiguration.Load(configurationFile);

				// Select the specified nodes by passing an XPath expression that contains the full path,
				// from root to element.
				XmlNode toolbarNode = applicationConfiguration.SelectSingleNode("configuration/general/toolbar");
				XmlNode loggingNode = applicationConfiguration.SelectSingleNode("configuration/general/logging");
				XmlNode serverNode = applicationConfiguration.SelectSingleNode("configuration/search/server");
				XmlNode timeoutNode = applicationConfiguration.SelectSingleNode("configuration/search/timeout");
				XmlNode recordsNode = applicationConfiguration.SelectSingleNode("configuration/search/maxRecords");

				// If the node is not found a null is returned.  If the node is not null, retrieve the InnerText
				// and convert it to the appropriate data type of the associated control.  If the node is null, 
				// set the associated control to a default value.
				toolBarIsVisible.Checked = (toolbarNode != null) ? (Boolean.Parse(toolbarNode.InnerText)) : (false);
				loggingIsEnabled.Checked = (loggingNode != null) ? (Boolean.Parse(loggingNode.InnerText)) : (false);
				serverName.Text = (serverNode != null) ? (serverNode.InnerText) : ("");
				timeoutPeriod.Value = (timeoutNode != null) ? (int.Parse(timeoutNode.InnerText)) : (30);
				maximumRecords.Value = (recordsNode != null) ? (int.Parse(recordsNode.InnerText)) : (100);
			}
			catch (Exception exception)
			{
				MessageBox.Show("Exception: " + exception.ToString());
			}
		}

		private void toolBarIsVisible_CheckedChanged(object sender, System.EventArgs e)
		{
		
		}
	}
}
