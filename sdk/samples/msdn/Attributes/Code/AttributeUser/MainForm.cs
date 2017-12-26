using System;
using System.Drawing;
using System.Diagnostics;
using System.Collections;
using System.ComponentModel;
using System.Configuration;
using System.Windows.Forms;
using System.Data;

namespace AttributeUser
{
	/// <summary>
	/// The MainForm class provides the client interface for this sample.
	/// </summary>
	public class MainForm : System.Windows.Forms.Form
	{
    private System.Windows.Forms.Label ItemNameLabel;
    private System.Windows.Forms.TextBox ItemNameTextBox;
    private System.Windows.Forms.Button AddItemButton;
    private System.Windows.Forms.ListBox ItemsToProcessListBox;
    private System.Windows.Forms.Button ProcessItemsButton;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// Creates a new instance of the main form.
    /// </summary>
		public MainForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
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
      this.ItemNameLabel = new System.Windows.Forms.Label();
      this.ItemNameTextBox = new System.Windows.Forms.TextBox();
      this.AddItemButton = new System.Windows.Forms.Button();
      this.ItemsToProcessListBox = new System.Windows.Forms.ListBox();
      this.ProcessItemsButton = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // ItemNameLabel
      // 
      this.ItemNameLabel.FlatStyle = System.Windows.Forms.FlatStyle.System;
      this.ItemNameLabel.Location = new System.Drawing.Point(12, 12);
      this.ItemNameLabel.Name = "ItemNameLabel";
      this.ItemNameLabel.Size = new System.Drawing.Size(60, 16);
      this.ItemNameLabel.TabIndex = 0;
      this.ItemNameLabel.Text = "Item Name";
      // 
      // ItemNameTextBox
      // 
      this.ItemNameTextBox.Location = new System.Drawing.Point(76, 8);
      this.ItemNameTextBox.Name = "ItemNameTextBox";
      this.ItemNameTextBox.Size = new System.Drawing.Size(228, 20);
      this.ItemNameTextBox.TabIndex = 1;
      this.ItemNameTextBox.Text = "";
      this.ItemNameTextBox.TextChanged += new System.EventHandler(this.ItemNameTextBox_TextChanged);
      // 
      // AddItemButton
      // 
      this.AddItemButton.Enabled = false;
      this.AddItemButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
      this.AddItemButton.Location = new System.Drawing.Point(316, 8);
      this.AddItemButton.Name = "AddItemButton";
      this.AddItemButton.Size = new System.Drawing.Size(52, 20);
      this.AddItemButton.TabIndex = 2;
      this.AddItemButton.Text = "&Add";
      this.AddItemButton.Click += new System.EventHandler(this.AddItemButton_Click);
      // 
      // ItemsToProcessListBox
      // 
      this.ItemsToProcessListBox.Items.AddRange(new object[] {
                                                               "Alabama",
                                                               "Alaska",
                                                               "Arizona",
                                                               "Arkansas",
                                                               "California",
                                                               "Colorado",
                                                               "Connecticut",
                                                               "Delaware",
                                                               "District of Columbia",
                                                               "Florida",
                                                               "Georgia",
                                                               "Hawaii",
                                                               "Idaho",
                                                               "Illinois",
                                                               "Indiana",
                                                               "Iowa",
                                                               "Kansas",
                                                               "Kentucky",
                                                               "Louisiana",
                                                               "Maine",
                                                               "Maryland",
                                                               "Massachusetts",
                                                               "Michigan",
                                                               "Minnesota",
                                                               "Mississippi",
                                                               "Missouri",
                                                               "Montana",
                                                               "Nebraska",
                                                               "Nevada",
                                                               "New Hampshire",
                                                               "New Jersey",
                                                               "New Mexico",
                                                               "New York",
                                                               "North Carolina",
                                                               "North Dakota",
                                                               "Ohio",
                                                               "Oklahoma",
                                                               "Oregon",
                                                               "Pennsylvania",
                                                               "Rhode Island",
                                                               "South Carolina",
                                                               "South Dakota",
                                                               "Tennessee",
                                                               "Texas",
                                                               "Utah",
                                                               "Vermont",
                                                               "Virginia",
                                                               "Washington",
                                                               "West Virginia",
                                                               "Wisconsin",
                                                               "Wyoming"});
      this.ItemsToProcessListBox.Location = new System.Drawing.Point(76, 32);
      this.ItemsToProcessListBox.Name = "ItemsToProcessListBox";
      this.ItemsToProcessListBox.Size = new System.Drawing.Size(228, 69);
      this.ItemsToProcessListBox.TabIndex = 3;
      // 
      // ProcessItemsButton
      // 
      this.ProcessItemsButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
      this.ProcessItemsButton.Location = new System.Drawing.Point(76, 108);
      this.ProcessItemsButton.Name = "ProcessItemsButton";
      this.ProcessItemsButton.Size = new System.Drawing.Size(228, 24);
      this.ProcessItemsButton.TabIndex = 4;
      this.ProcessItemsButton.Text = "&Process Items";
      this.ProcessItemsButton.Click += new System.EventHandler(this.ProcessItemsButton_Click);
      // 
      // MainForm
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(380, 141);
      this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                  this.ProcessItemsButton,
                                                                  this.ItemsToProcessListBox,
                                                                  this.AddItemButton,
                                                                  this.ItemNameTextBox,
                                                                  this.ItemNameLabel});
      this.MaximizeBox = false;
      this.Name = "MainForm";
      this.Text = "Attribute Sample";
      this.ResumeLayout(false);

    }
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new MainForm());
		}

    /// <summary>
    /// This method enables the Add button if the text box contains any non-empty
    /// data, or disables the Add button otherwise.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void ItemNameTextBox_TextChanged(object sender, System.EventArgs e)
    {
      if (ItemNameTextBox.Text.Trim().Length > 0)
      {
        if (!AddItemButton.Enabled)
          AddItemButton.Enabled = true;
      }
      else
      {
        if (AddItemButton.Enabled)
          AddItemButton.Enabled = false;
      }
    }

    /// <summary>
    /// This method adds the entry in the item name text box to the list of
    /// items to be processed.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void AddItemButton_Click(object sender, System.EventArgs e)
    {
      ItemsToProcessListBox.Items.Add(ItemNameTextBox.Text.Trim());
      ItemNameTextBox.Text = string.Empty;
    }

    /// <summary>
    /// This method loops through the list of items to be processed and calls
    /// the ItemProcessor's ProcessItem method for each, handling any
    /// exception that may have been generated.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void ProcessItemsButton_Click(object sender, System.EventArgs e)
    {
      ProcessItemsButton.Enabled = false;

      ItemProcessor processor = new ItemProcessor();
      int runningTotal = 0;

      for (int index = 0; index < ItemsToProcessListBox.Items.Count; index++)
      {
        string itemName = ItemsToProcessListBox.Items[index].ToString();
        try
        {
          processor.ProcessItem(itemName, ref runningTotal);
        }
        catch (ApplicationException)
        {
        }
      }

      ProcessItemsButton.Enabled = true;
      string traceLog = Process.GetCurrentProcess().MainModule.FileName + ".tracelog";
      string traceLogSetting = ConfigurationSettings.AppSettings["TraceLog"];
      if (traceLogSetting != null)
        traceLog = traceLogSetting;

      // When processing is complete, allow the user to see the trace log,
      // generated by the custom attribute applied to the ItemProcessor class.
      if (MessageBox.Show(this, "Would you like to see the trace log?", 
        "Processing Complete", MessageBoxButtons.YesNo) == DialogResult.Yes)
        Process.Start(new System.Diagnostics.ProcessStartInfo("notepad.exe", traceLog));
    }
	}
}
