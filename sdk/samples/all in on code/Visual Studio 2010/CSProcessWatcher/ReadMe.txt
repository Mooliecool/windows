========================================================================
	      Console Application: CSProcessWatcher Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The project illustrates how to watch the process creation/modify/shutdown events
by using the Windows Management Instrumentation(WMI).

/////////////////////////////////////////////////////////////////////////////
Demo: 

Please follow these demonstration steps below.

Step 1. Run CSProcessWatcher.exe in a command prompt. The command
        'CSProcessWatcher.exe <ProcessName>' means CSProcessWatcher.exe will watch
        the events of the <ProcessName>. Here, the <ProcessName> is the name of the
        process which you want to watch. If you run CSProcessWatcher.exe directly
        without any command argument, the default <ProcessName> will be "notepad.exe".

Step 2. Launch the process which you want to watch. Observe the output information 
        from the CSProcesswatcher.exe. 


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1.  Create a Visual C# Console Application, name it "CSProcessWatcher".

Step 2.  Right click the References, choose "Add Reference ...", then select
         .NET reference "System.Management", click "OK".

Step 3. Open the Visual studio 2010 command prompt, navigate to 
		your current project folder and enter the command:  
		
		mgmtclassgen Win32_Process /n root\cimv2 /o WMI.Win32
		
		You will get the Process.CS file. Add it into this project.
		
Step 4.  Add a new class 'ProcessWatcher'. Make it derive from 
         System.Management.ManagementEventWatcher. Add three events for this class:
         ProcessCreated, ProcessDeleted, ProcessModified.
         
         In the constructor of class ProcessWatcher, we need to subscribe to temporary
         event notifications based on a specified event query.
         
        private void Init(string processName)
        {
            this.Query.QueryLanguage = "WQL";
            this.Query.QueryString = string.Format(processEventQuery, processName);
            this.EventArrived += new EventArrivedEventHandler(watcher_EventArrived);
        }
        
         Meanwhile, when we received an event notification, according to  the event
         type, we should raise the corresponding event. 
 
        private void watcher_EventArrived(object sender, EventArrivedEventArgs e)
        {
            string eventType = e.NewEvent.ClassPath.ClassName;
            WMI.Win32.Process proc = new WMI.Win32.Process(e.NewEvent["TargetInstance"] as ManagementBaseObject);

            switch (eventType)
            {
                case "__InstanceCreationEvent":
                    if (ProcessCreated != null)
                    {
                        ProcessCreated(proc);
                    }
                    break;

                case "__InstanceDeletionEvent":
                    if (ProcessDeleted != null)
                    {
                        ProcessDeleted(proc);
                    }
                    break;

                case "__InstanceModificationEvent":
                    if (ProcessModified != null)
                    {
                        ProcessModified(proc);
                    }
                    break;
            }
        }

        
/////////////////////////////////////////////////////////////////////////////
References:

WQL (SQL for WMI)
http://msdn.microsoft.com/en-us/library/aa394606(v=VS.85).aspx

Win32_Process Class
http://msdn.microsoft.com/en-us/library/aa394372(v=VS.85).aspx

__InstanceOperationEvent Class
http://msdn.microsoft.com/en-us/library/aa394652(v=VS.85).aspx

Receiving a WMI Event
http://msdn.microsoft.com/en-us/library/aa393013(VS.85).aspx


/////////////////////////////////////////////////////////////////////////////


