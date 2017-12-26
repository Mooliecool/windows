namespace Microsoft.Samples.DeviceNotification 
{
    public partial class DeviceNotification : System.Windows.Forms.Form
    {
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.mainMenu1 = new System.Windows.Forms.MainMenu();
            this.NotifyMenu = new System.Windows.Forms.MenuItem();
            this.NotifyShow = new System.Windows.Forms.MenuItem();
            this.NotifyHide = new System.Windows.Forms.MenuItem();
            this.NewNotificationAdd = new System.Windows.Forms.MenuItem();
            this.NotifyIcon = new System.Windows.Forms.ComboBox();
            this.NotifyVisable = new System.Windows.Forms.Label();
            this.NotifyReponce = new System.Windows.Forms.Label();
            this.Label9 = new System.Windows.Forms.Label();
            this.Label8 = new System.Windows.Forms.Label();
            this.NotifyCritical = new System.Windows.Forms.CheckBox();
            this.Label7 = new System.Windows.Forms.Label();
            this.NotifyText = new System.Windows.Forms.TextBox();
            this.Label6 = new System.Windows.Forms.Label();
            this.NotifyCaption = new System.Windows.Forms.TextBox();
            this.Label5 = new System.Windows.Forms.Label();
            this.Label4 = new System.Windows.Forms.Label();
            this.Label3 = new System.Windows.Forms.Label();
            this.Label2 = new System.Windows.Forms.Label();
            this.Label1 = new System.Windows.Forms.Label();
            this.NotifyDuration = new System.Windows.Forms.NumericUpDown();
            this.SuspendLayout();
            // 
            // mainMenu1
            // 
            this.mainMenu1.MenuItems.Add(this.NotifyMenu);
            this.mainMenu1.MenuItems.Add(this.NewNotificationAdd);
            // 
            // NotifyMenu
            // 
            this.NotifyMenu.MenuItems.Add(this.NotifyShow);
            this.NotifyMenu.MenuItems.Add(this.NotifyHide);
            this.NotifyMenu.Text = "Notification";
            // 
            // NotifyShow
            // 
            this.NotifyShow.Text = "Show";
            this.NotifyShow.Click += new System.EventHandler(this.NotifyShow_Click);
            // 
            // NotifyHide
            // 
            this.NotifyHide.Text = "Hide";
            this.NotifyHide.Click += new System.EventHandler(this.NotifyHide_Click);
            // 
            // NewNotificationAdd
            // 
            this.NewNotificationAdd.Text = "AddNew";
            this.NewNotificationAdd.Click += new System.EventHandler(this.NewNotificationAdd_Click);
            // 
            // NotifyIcon
            // 
            this.NotifyIcon.Items.Add("Icon1");
            this.NotifyIcon.Items.Add("Icon2");
            this.NotifyIcon.Location = new System.Drawing.Point(92, 118);
            this.NotifyIcon.Name = "NotifyIcon";
            this.NotifyIcon.Size = new System.Drawing.Size(125, 22);
            this.NotifyIcon.TabIndex = 42;
            this.NotifyIcon.SelectedIndexChanged += new System.EventHandler(this.NotifyIcon_SelectedIndexChanged);
            // 
            // NotifyVisable
            // 
            this.NotifyVisable.Location = new System.Drawing.Point(95, 239);
            this.NotifyVisable.Name = "NotifyVisable";
            this.NotifyVisable.Size = new System.Drawing.Size(123, 20);
            // 
            // NotifyReponce
            // 
            this.NotifyReponce.Location = new System.Drawing.Point(92, 213);
            this.NotifyReponce.Name = "NotifyReponce";
            this.NotifyReponce.Size = new System.Drawing.Size(123, 20);
            // 
            // Label9
            // 
            this.Label9.Location = new System.Drawing.Point(30, 146);
            this.Label9.Name = "Label9";
            this.Label9.Size = new System.Drawing.Size(61, 22);
            this.Label9.Text = "Duration:";
            // 
            // Label8
            // 
            this.Label8.Location = new System.Drawing.Point(52, 118);
            this.Label8.Name = "Label8";
            this.Label8.Size = new System.Drawing.Size(39, 22);
            this.Label8.Text = "Icon:";
            // 
            // NotifyCritical
            // 
            this.NotifyCritical.Location = new System.Drawing.Point(153, 146);
            this.NotifyCritical.Name = "NotifyCritical";
            this.NotifyCritical.Size = new System.Drawing.Size(65, 22);
            this.NotifyCritical.TabIndex = 40;
            this.NotifyCritical.Text = "Critical";
            this.NotifyCritical.CheckStateChanged += new System.EventHandler(this.NotifyCritical_CheckStateChanged);
            // 
            // Label7
            // 
            this.Label7.Location = new System.Drawing.Point(52, 92);
            this.Label7.Name = "Label7";
            this.Label7.Size = new System.Drawing.Size(39, 20);
            this.Label7.Text = "Text:";
            // 
            // NotifyText
            // 
            this.NotifyText.Location = new System.Drawing.Point(92, 91);
            this.NotifyText.Name = "NotifyText";
            this.NotifyText.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.NotifyText.Size = new System.Drawing.Size(126, 21);
            this.NotifyText.TabIndex = 39;
            this.NotifyText.TextChanged += new System.EventHandler(this.NotifyText_TextChanged);
            // 
            // Label6
            // 
            this.Label6.Location = new System.Drawing.Point(37, 65);
            this.Label6.Name = "Label6";
            this.Label6.Size = new System.Drawing.Size(54, 20);
            this.Label6.Text = "Caption:";
            // 
            // NotifyCaption
            // 
            this.NotifyCaption.Location = new System.Drawing.Point(92, 64);
            this.NotifyCaption.Name = "NotifyCaption";
            this.NotifyCaption.Size = new System.Drawing.Size(126, 21);
            this.NotifyCaption.TabIndex = 38;
            this.NotifyCaption.Text = "Sample Caption";
            this.NotifyCaption.TextChanged += new System.EventHandler(this.NotifyCaption_TextChanged);
            // 
            // Label5
            // 
            this.Label5.Location = new System.Drawing.Point(36, 239);
            this.Label5.Name = "Label5";
            this.Label5.Size = new System.Drawing.Size(50, 20);
            this.Label5.Text = "Visible:";
            // 
            // Label4
            // 
            this.Label4.Location = new System.Drawing.Point(20, 213);
            this.Label4.Name = "Label4";
            this.Label4.Size = new System.Drawing.Size(66, 20);
            this.Label4.Text = "Response:";
            // 
            // Label3
            // 
            this.Label3.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold);
            this.Label3.Location = new System.Drawing.Point(7, 47);
            this.Label3.Name = "Label3";
            this.Label3.Size = new System.Drawing.Size(211, 16);
            this.Label3.Text = "Options";
            // 
            // Label2
            // 
            this.Label2.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold);
            this.Label2.Location = new System.Drawing.Point(5, 193);
            this.Label2.Name = "Label2";
            this.Label2.Size = new System.Drawing.Size(228, 23);
            this.Label2.Text = "Output from Notification";
            // 
            // Label1
            // 
            this.Label1.Location = new System.Drawing.Point(6, 9);
            this.Label1.Name = "Label1";
            this.Label1.Size = new System.Drawing.Size(229, 32);
            this.Label1.Text = "Select Notification options below then use the visible menu";
            // 
            // NotifyDuration
            // 
            this.NotifyDuration.Location = new System.Drawing.Point(92, 146);
            this.NotifyDuration.Maximum = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.NotifyDuration.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.NotifyDuration.Name = "NotifyDuration";
            this.NotifyDuration.Size = new System.Drawing.Size(55, 22);
            this.NotifyDuration.TabIndex = 54;
            this.NotifyDuration.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.NotifyDuration.ValueChanged += new System.EventHandler(this.NotifyDuration_ValueChanged);
            // 
            // DeviceNotification
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 268);
            this.Controls.Add(this.NotifyDuration);
            this.Controls.Add(this.NotifyIcon);
            this.Controls.Add(this.NotifyVisable);
            this.Controls.Add(this.NotifyReponce);
            this.Controls.Add(this.Label9);
            this.Controls.Add(this.Label8);
            this.Controls.Add(this.NotifyCritical);
            this.Controls.Add(this.Label7);
            this.Controls.Add(this.NotifyText);
            this.Controls.Add(this.Label6);
            this.Controls.Add(this.NotifyCaption);
            this.Controls.Add(this.Label5);
            this.Controls.Add(this.Label4);
            this.Controls.Add(this.Label3);
            this.Controls.Add(this.Label2);
            this.Controls.Add(this.Label1);
            this.Menu = this.mainMenu1;
            this.Name = "DeviceNotification";
            this.Text = "CS Notification Sample";
            this.Load += new System.EventHandler(this.DeviceNotification_Load);
            this.ResumeLayout(false);

		}

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
        }

		private System.Windows.Forms.MainMenu mainMenu1;
		private System.Windows.Forms.ComboBox NotifyIcon;
		private System.Windows.Forms.Label NotifyVisable;
		private System.Windows.Forms.Label NotifyReponce;
		private System.Windows.Forms.Label Label9;
		private System.Windows.Forms.Label Label8;
		private System.Windows.Forms.CheckBox NotifyCritical;
		private System.Windows.Forms.Label Label7;
		private System.Windows.Forms.TextBox NotifyText;
		private System.Windows.Forms.Label Label6;
		private System.Windows.Forms.TextBox NotifyCaption;
		private System.Windows.Forms.Label Label5;
		private System.Windows.Forms.Label Label4;
		private System.Windows.Forms.Label Label3;
		private System.Windows.Forms.Label Label2;
		private System.Windows.Forms.Label Label1;
		private System.Windows.Forms.MenuItem NotifyMenu;
		private System.Windows.Forms.MenuItem NotifyShow;
		private System.Windows.Forms.MenuItem NotifyHide;
		private System.Windows.Forms.MenuItem NewNotificationAdd;
		private System.Windows.Forms.NumericUpDown NotifyDuration;
    }
}

