using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace ResourceFiles
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private System.Windows.Forms.GroupBox gbLanguage;
		private System.Windows.Forms.Label lblLanguage;
		private System.Windows.Forms.ComboBox cboLanguage;
		private System.Windows.Forms.GroupBox gbFields;
		private System.Windows.Forms.TextBox txtWorkPhone;
		private System.Windows.Forms.Label lblWorkPhone;
		private System.Windows.Forms.TextBox txtHomePhone;
		private System.Windows.Forms.Label lblHomePhone;
		private System.Windows.Forms.TextBox txtLastName;
		private System.Windows.Forms.Label lblLastName;
		private System.Windows.Forms.TextBox txtFirstName;
		private System.Windows.Forms.Label lblFirstName;

		/// <summary>
		/// The ResourceManager is used to get strings for form text from the
		/// appropriate resource file based on the culture of the current machine.
		/// </summary>
		System.Resources.ResourceManager rm;


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
			this.gbLanguage = new System.Windows.Forms.GroupBox();
			this.lblLanguage = new System.Windows.Forms.Label();
			this.cboLanguage = new System.Windows.Forms.ComboBox();
			this.gbFields = new System.Windows.Forms.GroupBox();
			this.txtWorkPhone = new System.Windows.Forms.TextBox();
			this.lblWorkPhone = new System.Windows.Forms.Label();
			this.txtHomePhone = new System.Windows.Forms.TextBox();
			this.lblHomePhone = new System.Windows.Forms.Label();
			this.txtLastName = new System.Windows.Forms.TextBox();
			this.lblLastName = new System.Windows.Forms.Label();
			this.txtFirstName = new System.Windows.Forms.TextBox();
			this.lblFirstName = new System.Windows.Forms.Label();
			this.gbLanguage.SuspendLayout();
			this.gbFields.SuspendLayout();
			this.SuspendLayout();
			// 
			// gbLanguage
			// 
			this.gbLanguage.Controls.AddRange(new System.Windows.Forms.Control[] {
																																						 this.lblLanguage,
																																						 this.cboLanguage});
			this.gbLanguage.Location = new System.Drawing.Point(8, 8);
			this.gbLanguage.Name = "gbLanguage";
			this.gbLanguage.Size = new System.Drawing.Size(352, 56);
			this.gbLanguage.TabIndex = 7;
			this.gbLanguage.TabStop = false;
			// 
			// lblLanguage
			// 
			this.lblLanguage.Location = new System.Drawing.Point(16, 24);
			this.lblLanguage.Name = "lblLanguage";
			this.lblLanguage.Size = new System.Drawing.Size(136, 16);
			this.lblLanguage.TabIndex = 8;
			this.lblLanguage.Text = "languageText";
			// 
			// cboLanguage
			// 
			this.cboLanguage.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cboLanguage.Location = new System.Drawing.Point(160, 24);
			this.cboLanguage.Name = "cboLanguage";
			this.cboLanguage.Size = new System.Drawing.Size(176, 21);
			this.cboLanguage.TabIndex = 7;
			this.cboLanguage.SelectedIndexChanged += new System.EventHandler(this.cboLanguage_SelectedIndexChanged);
			// 
			// gbFields
			// 
			this.gbFields.Controls.AddRange(new System.Windows.Forms.Control[] {
																																					 this.txtWorkPhone,
																																					 this.lblWorkPhone,
																																					 this.txtHomePhone,
																																					 this.lblHomePhone,
																																					 this.txtLastName,
																																					 this.lblLastName,
																																					 this.txtFirstName,
																																					 this.lblFirstName});
			this.gbFields.Location = new System.Drawing.Point(8, 72);
			this.gbFields.Name = "gbFields";
			this.gbFields.Size = new System.Drawing.Size(352, 160);
			this.gbFields.TabIndex = 6;
			this.gbFields.TabStop = false;
			// 
			// txtWorkPhone
			// 
			this.txtWorkPhone.Location = new System.Drawing.Point(160, 128);
			this.txtWorkPhone.Name = "txtWorkPhone";
			this.txtWorkPhone.Size = new System.Drawing.Size(176, 20);
			this.txtWorkPhone.TabIndex = 19;
			this.txtWorkPhone.Text = "";
			// 
			// lblWorkPhone
			// 
			this.lblWorkPhone.Location = new System.Drawing.Point(16, 128);
			this.lblWorkPhone.Name = "lblWorkPhone";
			this.lblWorkPhone.Size = new System.Drawing.Size(136, 16);
			this.lblWorkPhone.TabIndex = 18;
			this.lblWorkPhone.Text = "homePhoneText";
			// 
			// txtHomePhone
			// 
			this.txtHomePhone.Location = new System.Drawing.Point(160, 96);
			this.txtHomePhone.Name = "txtHomePhone";
			this.txtHomePhone.Size = new System.Drawing.Size(176, 20);
			this.txtHomePhone.TabIndex = 17;
			this.txtHomePhone.Text = "";
			// 
			// lblHomePhone
			// 
			this.lblHomePhone.Location = new System.Drawing.Point(16, 96);
			this.lblHomePhone.Name = "lblHomePhone";
			this.lblHomePhone.Size = new System.Drawing.Size(136, 16);
			this.lblHomePhone.TabIndex = 16;
			this.lblHomePhone.Text = "workPhoneText";
			// 
			// txtLastName
			// 
			this.txtLastName.Location = new System.Drawing.Point(160, 64);
			this.txtLastName.Name = "txtLastName";
			this.txtLastName.Size = new System.Drawing.Size(176, 20);
			this.txtLastName.TabIndex = 15;
			this.txtLastName.Text = "";
			// 
			// lblLastName
			// 
			this.lblLastName.Location = new System.Drawing.Point(16, 64);
			this.lblLastName.Name = "lblLastName";
			this.lblLastName.Size = new System.Drawing.Size(136, 16);
			this.lblLastName.TabIndex = 14;
			this.lblLastName.Text = "lastNameText";
			// 
			// txtFirstName
			// 
			this.txtFirstName.Location = new System.Drawing.Point(160, 32);
			this.txtFirstName.Name = "txtFirstName";
			this.txtFirstName.Size = new System.Drawing.Size(176, 20);
			this.txtFirstName.TabIndex = 13;
			this.txtFirstName.Text = "";
			// 
			// lblFirstName
			// 
			this.lblFirstName.Location = new System.Drawing.Point(16, 32);
			this.lblFirstName.Name = "lblFirstName";
			this.lblFirstName.Size = new System.Drawing.Size(136, 16);
			this.lblFirstName.TabIndex = 12;
			this.lblFirstName.Text = "firstNameText";
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(368, 244);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																																	this.gbLanguage,
																																	this.gbFields});
			this.Name = "Form1";
			this.Text = "form1Text";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.gbLanguage.ResumeLayout(false);
			this.gbFields.ResumeLayout(false);
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
			// The following line tells the resource manager to look for resource files
			// with a base name of "MyStrings" in the same directory as the executable file.
			// Resource manager will look for resource files in the specified location that have
			// the base name followed by the culture.  For example, if the culture is set to Spanish (es-ES)
			// ResourceManager will look for a resource file called "MyStrings.es-ES.resources".  If no
			// match is found, the default file "MyStrings.resources" will be used.
			rm = System.Resources.ResourceManager.CreateFileBasedResourceManager("MyStrings", "..\\..\\..\\Misc", null);

			// This function sets the text on the form to the appropriate values
			// based on the current culture.  This would normally happen only once,
			// but in this example, the culture can be changed on the fly, so this code
			// is pulled out into a separate function that can be called both at startup
			// and from the cboLanguage drop-down.
			SetCulture();
		}

		/// <summary>
		/// The cboLanguage control is used to simulate different cultures.
		/// The SelectedIndexChanged event fires whenever the current selection
		/// changes, whether changed programmatically or interactively.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data.</param>
		private void cboLanguage_SelectedIndexChanged(object sender, System.EventArgs e)
		{

			// Used to store the simulated culture setting.
			string culture;

			// Get the simulated culture setting based on the user's selection.
			switch (cboLanguage.SelectedIndex)
			{
				case 0:
					// US English.
					// This will look for a MyStrings.en-US.resources file.
					// Since there is none, it will revert to the default of MyStrings.resources.
					culture = "en-US";
					break;
				case 1:
					// Spanish (Spain).
					// This will use the MyStrings.es-ES.resources file.
					culture = "es-ES";
					break;
				default:
					// Catch all.  This shouldn't happen, but...
					// This will use the default of MyStrings.resources.
					culture = "";
					break;
			}

			// Set the culture of the current thread to the specified culture to simulate
			// different cultures.
			System.Threading.Thread.CurrentThread.CurrentUICulture = new System.Globalization.CultureInfo(culture);

			// Reset the form and control text using the new culture setting.
			SetCulture();
		}

		/// <summary>
		/// This function sets the text on the form to the appropriate values
		/// based on the current culture.  This would normally happen only once,
		/// but in this example, the culture can be changed on the fly, so this code
		/// is pulled out into a separate function that can be called both at startup
		/// and from the cboLanguage drop-down.
		/// </summary>
		private void SetCulture()
		{

			// Store the current selected index so it can be restored after the
			// list is repopulated.
			int selectedIndex = this.cboLanguage.SelectedIndex;

			// A drop-down list is used in this sample to allow the user to simulate
			// different cultures.  The following lines are used to populate the list
			// using the current culture.
			this.cboLanguage.Items.Clear();
			this.cboLanguage.Items.Add(rm.GetString("englishText"));
			this.cboLanguage.Items.Add(rm.GetString("spanishText"));

			// Temporarily disable the SelectedIndexChanged event handler
			// while updating the SelectedIndex property in order to keep from 
			// going into an endless loop.
			this.cboLanguage.SelectedIndexChanged -= new System.EventHandler(this.cboLanguage_SelectedIndexChanged);

			// Now restore the selected index.
			this.cboLanguage.SelectedIndex = (selectedIndex == -1 ? 0 : selectedIndex);

			// Re-enable the SelectedIndexChanged event handler.
			this.cboLanguage.SelectedIndexChanged += new System.EventHandler(this.cboLanguage_SelectedIndexChanged);

			// Set the text properties for the form and the form labels
			// to the localized versions of the strings.
			this.Text = rm.GetString("form1Text");
			this.lblLanguage.Text = rm.GetString("languageText");
			this.lblFirstName.Text = rm.GetString("firstNameText");
			this.lblLastName.Text = rm.GetString("lastNameText");
			this.lblHomePhone.Text = rm.GetString("homePhoneText");
			this.lblWorkPhone.Text = rm.GetString("workPhoneText");

		}

	}
}
