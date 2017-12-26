using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.ServiceProcess;


public class WSControllerForm : System.Windows.Forms.Form
{
	private System.ServiceProcess.ServiceController WSController;
	private System.Windows.Forms.Label Label1;
	private System.Windows.Forms.Button ButtonStart;
	private System.Windows.Forms.Button ButtonStop;
	private System.Windows.Forms.Button ButtonPause;
	private System.Windows.Forms.Label controlledService;
	/// <summary>
	/// Required designer variable.
	/// </summary>
	private System.ComponentModel.Container components = null;

	public WSControllerForm()
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
		this.WSController = new System.ServiceProcess.ServiceController();
		this.Label1 = new System.Windows.Forms.Label();
		this.ButtonStart = new System.Windows.Forms.Button();
		this.ButtonStop = new System.Windows.Forms.Button();
		this.ButtonPause = new System.Windows.Forms.Button();
		this.controlledService = new System.Windows.Forms.Label();
		this.SuspendLayout();
		// 
		// Label1
		// 
		this.Label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
		this.Label1.Location = new System.Drawing.Point(48, 8);
		this.Label1.Name = "Label1";
		this.Label1.Size = new System.Drawing.Size(232, 23);
		this.Label1.TabIndex = 0;
		this.Label1.Text = "Windows Service Controller";
		this.Label1.TextAlign = System.Drawing.ContentAlignment.TopCenter;
		// 
		// ButtonStart
		// 
		this.ButtonStart.Location = new System.Drawing.Point(120, 48);
		this.ButtonStart.Name = "ButtonStart";
		this.ButtonStart.TabIndex = 1;
		this.ButtonStart.Text = "Start";
		this.ButtonStart.Click += new System.EventHandler(this.ButtonStart_Click);
		// 
		// ButtonStop
		// 
		this.ButtonStop.Location = new System.Drawing.Point(120, 112);
		this.ButtonStop.Name = "ButtonStop";
		this.ButtonStop.TabIndex = 2;
		this.ButtonStop.Text = "Stop";
		this.ButtonStop.Click += new System.EventHandler(this.ButtonStop_Click);
		// 
		// ButtonPause
		// 
		this.ButtonPause.Location = new System.Drawing.Point(120, 80);
		this.ButtonPause.Name = "ButtonPause";
		this.ButtonPause.TabIndex = 3;
		this.ButtonPause.Text = "Pause";
		this.ButtonPause.Click += new System.EventHandler(this.ButtonPause_Click);
		// 
		// controlledService
		// 
		this.controlledService.AutoSize = true;
		this.controlledService.Location = new System.Drawing.Point(96, 144);
		this.controlledService.Name = "controlledService";
		this.controlledService.Size = new System.Drawing.Size(0, 13);
		this.controlledService.TabIndex = 4;
		// 
		// WSControllerForm
		// 
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		this.ClientSize = new System.Drawing.Size(336, 174);
		this.Controls.AddRange(new System.Windows.Forms.Control[] {
																	  this.controlledService,
																	  this.ButtonPause,
																	  this.ButtonStop,
																	  this.ButtonStart,
																	  this.Label1});
		this.MinimumSize = new System.Drawing.Size(264, 160);
		this.Name = "WSControllerForm";
		this.Text = "Windows Service Controller";
		this.Load += new System.EventHandler(this.WSControllerForm_Load);
		this.ResumeLayout(false);

	}
	#endregion

	private void WSControllerForm_Load(object sender, System.EventArgs e)
	{
		ServiceController[] AvailableServices = ServiceController.GetServices(".");
		
		foreach(ServiceController AvailableService in AvailableServices)
		{
			//Check the service name for IIS.
			if(AvailableService.ServiceName=="IISADMIN")
			{
				WSController.ServiceName="IISADMIN";
				SetButtonStatus();
				return;
			}
		}

		MessageBox.Show("The IIS Admin Service is not installed on this Machine", "IIS Admin Service is not available");
		this.Close();
		Application.Exit();
	}


	private void SetButtonStatus()
	{
		//get the status of the service.
		string strServerStatus = WSController.Status.ToString();

		//check the status of the service and enable the 
		//command buttons accordingly.

		controlledService.Text =  WSController.ServiceName;

		if (strServerStatus == "Running")
		{
			//check to see if the service can be paused
			if (WSController.CanPauseAndContinue == true)
			{
				ButtonPause.Enabled = true;
			}
			else
			{
				ButtonPause.Enabled = false;
			}

			ButtonStop.Enabled = true;
			ButtonStart.Enabled = false;
		}
		else if(strServerStatus == "Paused")
		{
			ButtonStart.Enabled = true;
			ButtonPause.Enabled = false;
			ButtonStop.Enabled = true;
		}
		else if (strServerStatus == "Stopped")
		{
			ButtonStart.Enabled = true;
			ButtonPause.Enabled = false;
			ButtonStop.Enabled = false;
		}

	}


	private void ButtonStart_Click(object sender, System.EventArgs e)
	{
		//check the status of the service
		if(WSController.Status.ToString() == "Paused")
		{
			WSController.Continue();
		}
		else if(WSController.Status.ToString() == "Stopped")
		{

			//get an array of services this service depends upon, loop through 
			//the array and prompt the user to start all required services.
			ServiceController[] ParentServices = WSController.ServicesDependedOn;
			
			//if the length of the array is greater than or equal to 1.
			if (ParentServices.Length >= 1)
			{
				foreach(ServiceController ParentService in ParentServices)
				{	
					//make sure the parent service is running or at least paused.
					if(ParentService.Status.ToString() != "Running" || ParentService.Status.ToString() != "Paused")
					{

						if (MessageBox.Show("This service is required. Would you like to also start this service?\n"+ParentService.DisplayName, "Required Service", MessageBoxButtons.YesNo).ToString() == "Yes")
						{
							//if the user chooses to start the service

							ParentService.Start();
							ParentService.WaitForStatus(ServiceControllerStatus.Running);
						}
						else
						{
							//otherwise just return.
							return;
						}
					}
				}
			}
			
			WSController.Start();
		}

		WSController.WaitForStatus(System.ServiceProcess.ServiceControllerStatus.Running);
		SetButtonStatus();
	}

	private void ButtonStop_Click(object sender, System.EventArgs e)
	{
		//check to see if the service can be stopped.
		if (WSController.CanStop == true)
		{
			
			//get an array of dependent services, loop through the array and 
			//prompt the user to stop all dependent services.
			ServiceController[] DependentServices = WSController.DependentServices;
			
			//if the length of the array is greater than or equal to 1.
			if (DependentServices.Length >= 1)
			{
				foreach(ServiceController DependentService in DependentServices)
				{
					//make sure the dependent service is not already stopped.
					if(DependentService.Status.ToString() != "Stopped")
					{
						if (MessageBox.Show("Would you like to also stop this dependent service?\n"+DependentService.DisplayName, "Dependent Service", MessageBoxButtons.YesNo).ToString() == "Yes")
						{
							// not checking at this point whether the dependent service can be stopped.
							// developer may want to include this check to avoid exception.
							DependentService.Stop();
							DependentService.WaitForStatus(ServiceControllerStatus.Stopped);
						}
						else
						{
							return;
						}
					}
				}
			}
			
			//check the status of the service
			if(WSController.Status.ToString() == "Running" || WSController.Status.ToString() == "Paused")
			{
				WSController.Stop();
			}
			WSController.WaitForStatus(System.ServiceProcess.ServiceControllerStatus.Stopped);
			SetButtonStatus();
		}
	}

	private void ButtonPause_Click(object sender, System.EventArgs e)
	{
		//check to see if the service can be paused and continue
		if (WSController.CanPauseAndContinue == true)
		{
			//check the status of the service
			if(WSController.Status.ToString() == "Running")
			{
				WSController.Pause();
			}

			WSController.WaitForStatus(System.ServiceProcess.ServiceControllerStatus.Paused);
			SetButtonStatus();
		}

	}

}