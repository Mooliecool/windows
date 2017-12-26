using System;
using System.Drawing;
using System.Resources;
using System.ComponentModel;
using System.Windows.Forms;

public class SystemTray : System.Windows.Forms.Form
{
	private System.Windows.Forms.NotifyIcon WSNotifyIcon;
	private System.ComponentModel.IContainer components;
	private Icon mDirIcon = new Icon(typeof(SystemTray).Assembly.GetManifestResourceStream("SystemTrayExample.FLGUSA02.ICO"));
	
	public SystemTray()
	{
		//constructor for the form
		InitializeComponent();
		
		//keep the form hidden
		this.Hide();
		InitializeNotifyIcon();

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
		this.components = new System.ComponentModel.Container();
		this.WSNotifyIcon = new System.Windows.Forms.NotifyIcon(this.components);
		// 
		// WSNotifyIcon
		// 
		this.WSNotifyIcon.Text = "";
		this.WSNotifyIcon.Visible = true;
		// 
		// SysTray
		// 
		this.AccessibleRole = System.Windows.Forms.AccessibleRole.None;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		this.ClientSize = new System.Drawing.Size(8, 7);
		this.ControlBox = false;
		this.Enabled = false;
		this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
		this.MaximizeBox = false;
		this.MinimizeBox = false;
		this.Name = "SystemTray";
		this.Opacity = 0;
		this.ShowInTaskbar = false;
		this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;

	}
	#endregion

	/// <summary>
	/// The main entry point for the application.
	/// </summary>
	[STAThread]
	static void Main() 
	{
		Application.Run(new SystemTray());
	}

	public void ShowControlForm(object sender, EventArgs e)
	{
		//show the Control form.
		WSControllerForm controlForm = new WSControllerForm();
		controlForm.Show();
	}


	public void ExitControlForm(object sender, EventArgs e)
	{
		//Hide the NotifyIcon.
		WSNotifyIcon.Visible=false;

		this.Close();

	}

	private void InitializeNotifyIcon()
	{
		//setup the Icon
		NotifyIcon WSNotifyIcon = new NotifyIcon();
		WSNotifyIcon.Icon = mDirIcon;
		WSNotifyIcon.Text = "Right Click to Configure";
		WSNotifyIcon.Visible = true;
		
		//Create the MenuItem objects and add them to
		//the context menu of the NotifyIcon.
		MenuItem[] mnuItems = new MenuItem[3];
		
		//create the menu items array
		mnuItems[0] = new MenuItem("Show Control Form...", new EventHandler(this.ShowControlForm));
		mnuItems[0].DefaultItem=true;
		mnuItems[1] = new MenuItem("-");
		mnuItems[2] = new MenuItem("Exit", new EventHandler(this.ExitControlForm));
		
		//add the menu items to the context menu of the NotifyIcon
		ContextMenu notifyIconMenu = new ContextMenu(mnuItems);
		WSNotifyIcon.ContextMenu=notifyIconMenu;
	}

}

