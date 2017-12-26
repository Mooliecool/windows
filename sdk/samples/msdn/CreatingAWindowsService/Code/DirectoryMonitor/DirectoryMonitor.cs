using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.ServiceProcess;
using System.Configuration;
using System.IO;
using System.Threading;

namespace DirectoryMonitorService
{
	public class DirectoryMonitor : System.ServiceProcess.ServiceBase
	{

		private static string Path;
		private static string Filter;
		private static bool IncludeSubs;
		private System.IO.FileSystemWatcher FileMonitor;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public DirectoryMonitor()
		{
			// This call is required by the Windows.Forms Component Designer.
			InitializeComponent();
		}

		// The main entry point for the process
		static void Main()
		{
			System.ServiceProcess.ServiceBase[] ServicesToRun;
	
			// More than one user Service may run within the same process. To add
			// another service to this process, change the following line to
			// create a second service object. For example,
			//
			//   ServicesToRun = New System.ServiceProcess.ServiceBase[] {new Service1(), new MySecondUserService()};
			//

			//alter this code to create an instance of this service.
			ServicesToRun = new System.ServiceProcess.ServiceBase[] { new DirectoryMonitor() };

			System.ServiceProcess.ServiceBase.Run(ServicesToRun);
		
		}

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.FileMonitor = new System.IO.FileSystemWatcher();
			((System.ComponentModel.ISupportInitialize)(this.FileMonitor)).BeginInit();
			// 
			// FileMonitor
			// 
			this.FileMonitor.EnableRaisingEvents = true;
			this.FileMonitor.Deleted += new System.IO.FileSystemEventHandler(this.FileMonitor_Changed);
			this.FileMonitor.Renamed += new System.IO.RenamedEventHandler(this.FileMonitor_OnRenamed);
			this.FileMonitor.Changed += new System.IO.FileSystemEventHandler(this.FileMonitor_Changed);
			this.FileMonitor.Created += new System.IO.FileSystemEventHandler(this.FileMonitor_Changed);
			// 
			// DirectoryMonitor
			// 
			this.CanPauseAndContinue = true;
			this.CanShutdown = true;
			this.ServiceName = "DirectoryMonitor";
			((System.ComponentModel.ISupportInitialize)(this.FileMonitor)).EndInit();

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

		/// <summary>
		/// Set things in motion so your service can do its work.
		/// </summary>
		protected override void OnStart(string[] args)
		{
			//retrieve the configuration properties from the Machine.config file.
			Path = ConfigurationSettings.AppSettings.Get("DirectoryMonitorPath").ToString();
			Filter = ConfigurationSettings.AppSettings.Get("DirectoryMonitorFilter").ToString();
			IncludeSubs = (Convert.ToBoolean(ConfigurationSettings.AppSettings.Get("DirectoryMonitorIncludeSubs").ToString()));

			//Set the path for the FileSystemWatcher to monitor.
			FileMonitor.Path = Path.ToString();
			//Set the file type filter for the FileSystemWatcher to monitor.
			FileMonitor.Filter = Filter.ToString();
			//Determine if the FileSystemWatcher should monitor subdirectories.
			FileMonitor.IncludeSubdirectories = IncludeSubs;
			
			//Set the NotifyFilters for raising events.
			FileMonitor.NotifyFilter = NotifyFilters.LastAccess | NotifyFilters.LastWrite 
				| NotifyFilters.FileName | NotifyFilters.DirectoryName;

			//Begin monitoring.
			FileMonitor.EnableRaisingEvents = true;
		}
 
		/// <summary>
		/// Stop this service.
		/// </summary>
		protected override void OnStop()
		{
			//When the service is stopped the FileSystemMonitor should stop raising events.
			FileMonitor.EnableRaisingEvents = false;
		}

		protected override void OnPause()
		{
			//When the service is paused the FileSystemMonitor should stop raising events.
			FileMonitor.EnableRaisingEvents = false;
		}

		protected override void OnContinue()
		{
			//When the service is continued, then retrieve the configuration properties again.
			Path = ConfigurationSettings.AppSettings.Get("DirectoryMonitorPath").ToString();
			Filter = ConfigurationSettings.AppSettings.Get("DirectoryMonitorFilter").ToString();
			IncludeSubs = (Convert.ToBoolean(ConfigurationSettings.AppSettings.Get("DirectoryMonitorIncludeSubs").ToString()));

			//Set the properties on the monitor.
			FileMonitor.Path = Path.ToString();
			FileMonitor.Filter = Filter.ToString();
			FileMonitor.IncludeSubdirectories = IncludeSubs;
		}

		private void FileMonitor_OnRenamed(object source, RenamedEventArgs e)
		{
			// Specify what is done when a file is newName.
			// Here the Service makes an entry to the EventLog.
			string originalName = e.OldFullPath;
			string newName = e.FullPath;
			EventLog.WriteEntry("Directory Monitor", "File: "+originalName+" newName to "+e.OldName+" newName");
		}

		private void FileMonitor_Changed(object sender, System.IO.FileSystemEventArgs e)
		{
			string ChangeType = e.ChangeType.ToString();

			//write a log entry for the appropriate changetype.
			if (ChangeType=="Created")
			{
				EventLog.WriteEntry("Directory Monitor", "File: " +  e.FullPath + " " + e.Name+" Created");
			}
			else if(ChangeType=="Deleted")
			{
				EventLog.WriteEntry("Directory Monitor", "File: " +  e.FullPath + " " +e.Name+" Deleted");
			}
			else if(ChangeType=="Changed")
			{
				EventLog.WriteEntry("Directory Monitor", "File: "+e.FullPath+" "+e.Name+" Changed");
			}
		
		}

	}
}
