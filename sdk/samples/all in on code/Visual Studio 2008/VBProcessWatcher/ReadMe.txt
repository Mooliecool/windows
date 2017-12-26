========================================================================
	      Console Application: VBProcessWatcher Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The project illustrates how to watch the process creation/modify/shutdown events
by using the Windows Management Instrumentation(WMI).

/////////////////////////////////////////////////////////////////////////////
Demo: 

Please follow these demonstration steps below.

Step 1. Run VBProcessWatcher.exe in a command prompt. The command
        'VBProcessWatcher.exe <ProcessName>' means VBProcessWatcher.exe will watch
        the events of the <ProcessName>. Here, the <ProcessName> is the name of the
        process which you want to watch. If you run VBProcessWatcher.exe directly
        without any command argument, the default <ProcessName> will be "notepad.exe".

Step 2. Launch the process which you want to watch. Observe the output information 
        from the VBProcesswatcher.exe. 


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1.  Create a Visual Basic Console Application, name it "VBProcessWatcher".

Step 2.  Right click the Project name, choose "Add Reference ...", then select
         .NET reference "System.Management", click "OK".

Step 3. Open the Visual studio 2008 command prompt, navigate to 
		your current project folder and enter the command:  
		
		mgmtclassgen Win32_Process /n root\cimv2 /o WMI.Win32 /l VB
		
		You will get the Process.VB file. Add it into this project.
		
Step 4.  Add a new class 'ProcessWatcher'. Make it derive from 
         System.Management.ManagementEventWatcher. Add three events for this class:
         ProcessCreated, ProcessDeleted, ProcessModified.
         
         In the constructor of class ProcessWatcher, we need to subscribe to temporary
         event notifications based on a specified event query.
         
         Private Sub Init(ByVal processName As String)
             Me.Query.QueryLanguage = "WQL"
             Me.Query.QueryString = String.Format(WMI_OPER_EVENT_QUERY, processName)
             AddHandler Me.EventArrived, AddressOf Me.watcher_EventArrived
         End Sub
        
         Meanwhile, when we received an event notification, according to  the event
         type, we should raise the corresponding event. 
 
         Private Sub watcher_EventArrived(ByVal sender As Object, ByVal e As EventArrivedEventArgs)
            Dim eventType As String = e.NewEvent.ClassPath.ClassName
            Dim proc As New Win32.Process(TryCast(e.NewEvent("TargetInstance"), ManagementBaseObject))

            Select Case eventType
                Case "__InstanceCreationEvent"
                    RaiseEvent ProcessCreated(proc)
                    Exit Select

                Case "__InstanceDeletionEvent"
                    RaiseEvent ProcessDeleted(proc)
                    Exit Select

                Case "__InstanceModificationEvent"
                    RaiseEvent ProcessModified(proc)
                    Exit Select
            End Select
        End Sub
        
        
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


