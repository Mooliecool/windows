using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

// The following directives were added to make the code 
// in the LoadColumns() and DisplayAppSettings() functions more readable.
// This allows classes in these namespaces to be referenced without
// a fully-qualified name.
using System.Collections.Specialized;
using System.Configuration;

namespace SystemConfigurationFiles
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private System.Windows.Forms.ListView listView1;
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
			this.listView1 = new System.Windows.Forms.ListView();
			this.SuspendLayout();
			// 
			// listView1
			// 
			this.listView1.Anchor = (((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right);
			this.listView1.FullRowSelect = true;
			this.listView1.Location = new System.Drawing.Point(8, 8);
			this.listView1.Name = "listView1";
			this.listView1.Size = new System.Drawing.Size(328, 208);
			this.listView1.TabIndex = 1;
			this.listView1.View = System.Windows.Forms.View.Details;
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(344, 228);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																																	this.listView1});
			this.Name = "Form1";
			this.Text = "System Files Configuration Sample";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.Form1_Closing);
			this.Load += new System.EventHandler(this.Form1_Load);
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

		/// <summary>
		/// This event fires when the form is loaded.  At this time, the
		/// form size will be restored to that of the previous session as 
		/// recorded in the registry.  Grid columns will also be loaded from a
		/// custom section of a configuration file.  Finally, some applications settings 
		/// are displayed from the standard <appSettings> section of the config file.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data. </param>
		private void Form1_Load(object sender, System.EventArgs e)
		{
			// Restore the form size from the last session.
			RestoreFormSize();

			// Load the grid columns dynamically from a config file.
			LoadColumns();

			// Display some custom applications settings in the grid.
			DisplayAppSettings();

		}

		/// <summary>
		/// This event fires when a form is closing.  This will be used to persist
		/// the form size for use in future sessions.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">A CancelEventArgs that contains the event data.</param>
		private void Form1_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			// Persist the form size for the next session.
			SaveFormSize();
		}

		/// <summary>
		/// This function demonstrates the use of the Registry class
		/// to retrieve settings from the CurrentUser section of the registry.
		/// </summary>
		private void RestoreFormSize()
		{
			// Get a reference to the desired registry hive.
			// This example uses the CurrentUser hive in order to store user preferences
			// for each user on this machine separately.
			Microsoft.Win32.RegistryKey hklm = Microsoft.Win32.Registry.CurrentUser;

			// The following line creates the desired sub key where the settings are be stored.
			// If the sub key already exists, a reference to the existing key is returned.
			Microsoft.Win32.RegistryKey regKey = hklm.CreateSubKey("Software\\SystemFilesConfigSample");

			// The following lines retrieve the width and height settings from the sub key.
			// The second parameter indicates a default value if the setting
			// does not exist in the registry.
			int width = (int)regKey.GetValue("Width", 344);
			int height = (int)regKey.GetValue("Height", 228);

			// Restores the form size to persisted dimensions.
			this.ClientSize = new System.Drawing.Size(width, height);
		}

		/// <summary>
		/// This function demonstrates the use of the Registry class
		/// to save settings in the CurrentUser section of the registry.
		/// </summary>
		private void SaveFormSize()
		{
			// Get a reference to the desired registry hive.
			// This example uses the CurrentUser hive in order to store user preferences
			// for each user on this machine separately.
			Microsoft.Win32.RegistryKey hklm = Microsoft.Win32.Registry.CurrentUser;

			// The following line creates the desired sub key where the settings will be stored.
			// If the sub key already exists, a reference to the existing key is returned.
			Microsoft.Win32.RegistryKey regKey = hklm.CreateSubKey("Software\\SystemFilesConfigSample");

			// The following lines save the width and height of the form
			// in the specified registry keys.  The settings will be created
			// if they don't already exist.
			regKey.SetValue("Width", this.ClientSize.Width);
			regKey.SetValue("Height", this.ClientSize.Height);
		}

		/// <summary>
		/// This function reads a custom configuration file section to determine
		/// the columns that should be displayed in the grid.  The purpose of this function is
		/// to demonstrate how to read data from a custom section using the GetConfig() method.
		/// </summary>
		private void LoadColumns()
		{

			// This code will return a reference to a custom section in the application config file.
			// This reference is cast to a NameValueCollection data type.
			// The NameValueCollection class is in the System.Collections.Specialized namespace.
			// See the "using System.Collections.Specialized" directive above.
			// The ConfigurationSettings class is in the System.Configuration namespace.
			// See the "using System.Configuration" directive above.
			NameValueCollection config = (NameValueCollection)ConfigurationSettings.GetConfig("gridSettings/gridColumns");

			// Because the config section is cast as a collection, it can be traversed.
			// In this example, the collection contains the text for the grid column headers.
			if (config != null)
			{
				for (int i = 0; i < config.Count; i++)
				{
					// For each entry in the config section, add a grid row.
					listView1.Columns.Add(config[i], 200, HorizontalAlignment.Left);
				}
			}
		}

		private void DisplayAppSettings()
		{
			// The assumption is made that these settings are used in code to control
			// the behavior of the application.  Therefore, the number and names of the
			// settings are known, and they are accessed directly.  Since this sample has
			// no true application logic, the settings are just displayed in a list.
			// The ConfigurationSettings class is in the System.Configuration namespace.
			// See the "using System.Configuration" directive above.
			listView1.Items.Add(new ListViewItem(new string[] {"Host", ConfigurationSettings.AppSettings["Host"]}));
			listView1.Items.Add(new ListViewItem(new string[] {"ServicePath", ConfigurationSettings.AppSettings["ServicePath"]}));
			listView1.Items.Add(new ListViewItem(new string[] {"Port", ConfigurationSettings.AppSettings["Port"]}));
			listView1.Items.Add(new ListViewItem(new string[] {"Timeout", ConfigurationSettings.AppSettings["Timeout"]}));
			listView1.Items.Add(new ListViewItem(new string[] {"UseSSL", ConfigurationSettings.AppSettings["UseSSL"]}));
			listView1.Items.Add(new ListViewItem(new string[] {"WSDLFile", ConfigurationSettings.AppSettings["WSDLFile"]}));
			listView1.Items.Add(new ListViewItem(new string[] {"WSMLFile", ConfigurationSettings.AppSettings["WSMLFile"]}));
			listView1.Items.Add(new ListViewItem(new string[] {"WebServiceName", ConfigurationSettings.AppSettings["WebServiceName"]}));
			listView1.Items.Add(new ListViewItem(new string[] {"WebServicePort", ConfigurationSettings.AppSettings["WebServicePort"]}));
		}
	}
}
