=============================================================================
    WF4 APPLICATION : VBWF4ActivitiesCorrelation
=============================================================================
/////////////////////////////////////////////////////////////////////////////
Summary:
Consider such a workflow:
      start
	    |
 Receive activity
	    |
 Receive activity
        |
There are two Receive activities. The first Receive activity will create a 
workflow instance. The second Receive activity functions as a bookmark 
activity. Imagine that there are two such workflow instances:
      start                 start
	    |                     |
 Receive activity      Receive activity
	    |                     |
 Receive activity      Receive activity
        |                     |
A WCF request comes to call the second Receive activity.
Which one should take care of the request? We can use correlation to solve
the problem.


/////////////////////////////////////////////////////////////////////////////
Demo:
1. Setup WF4 persistence store according to:
   http://msdn.microsoft.com/en-us/library/ee395773.aspx
2. Open VBWF4ActivitiesCorrelation.sln with Visual Studio 2010(run as administrator). 
3. Open App.config file and alter the sql connection string. 
4. Press Ctrl+F5 to start the WF service. 
5. Use WCF Test Client to test the WF service.


/////////////////////////////////////////////////////////////////////////////
Prerequisite

1. Visual Studio 2010
2. .NET Framework 4.0


/////////////////////////////////////////////////////////////////////////////
Implementation:
1. Create a new Workflow Console Application project and name it: 
   VBWF4ActivitiesCorrelation. To create a Workflow Console Application:
   Click File|New|Visual Basic|Workflow|Workflow Console Application. 
2. Add references:
       System.Activities.DurableInstancing
       System.Configuation
	   System.Runtime.DurableInstancing
3. Setup WF4 persistence store according to:
   http://msdn.microsoft.com/en-us/library/ee395773.aspx
4. Open App.config file and alter its code to: 
   <?xml version="1.0" encoding="utf-8" ?>
   <configuration>
      <startup>       
         <supportedRuntime version="v4.0" 
                           sku=".NETFramework,Version=v4.0,Profile=Client" />       
      </startup>
	      <system.serviceModel>
			<bindings />
			<client />
			<behaviors>
			  <serviceBehaviors>
				<behavior>
				  <serviceDebug includeExceptionDetailInFaults="True"
								httpHelpPageEnabled="True"/>
				  <serviceMetadata httpGetEnabled="True"/>
				</behavior>
			  </serviceBehaviors>
			</behaviors>
		  </system.serviceModel>
	  <appSettings>
		<add key="SqlWF4PersistenceConnectionString" 
			 value="Data Source=.\sqlexpress;Initial Catalog=WF4PersistenceDB;Integrated Security=True" />
	  </appSettings>
   </configuration>
5. Open the default created file Workflow1.xaml,Create a workflow shown 
   in the Workflow1.xaml. 
6. Open MainModule.vb file and alter its code to: 
Imports System.Activities
Imports System.Activities.Statements
Imports System.Linq
Imports System.Activities.DurableInstancing
Imports System.Threading
Imports System.Configuration
Imports System.ServiceModel.Activities

Module MainModule

    Sub Main()
        Dim wfAddress As New Uri("http://localhost:8000/WFServices")
        Dim waitHandler As New AutoResetEvent(False)
        Using host As New WorkflowServiceHost(New Workflow1(), wfAddress)
            host.WorkflowExtensions.Add(SetupSimplySqlPersistenceStore())
            AddHandler host.Closed, Function(obj, arg) waitHandler.Set()
            host.Open()
            Console.WriteLine("http://localhost:8000/WFServices is opening")
            waitHandler.WaitOne()
        End Using
    End Sub

    Private Function SetupSimplySqlPersistenceStore() As SqlWorkflowInstanceStore
        Dim connectionString As String =
            ConfigurationManager.AppSettings("SqlWF4PersistenceConnectionString").ToString()
        Dim sqlInstanceStore As SqlWorkflowInstanceStore =
            New SqlWorkflowInstanceStore(connectionString)
        sqlInstanceStore.HostLockRenewalPeriod = TimeSpan.FromSeconds(30)
        Return sqlInstanceStore
    End Function

End Module

7. Press Ctrl+F5 to start the service. 
8. Use WCF Test Client to test the service.
   Add service address http://localhost:8000/WFServices to the WCF Test Client 
   then, Double Click StartWF(), invoke with any string, say, "aaa". after
   pressing the Invoke button, you shall see "Hello" printed in the service 
   console window. next, click ResumeWorkflow() in WCF Test Client. Invoke
   the method with the string "aaa". and only by "aaa" you then can resume 
   the persisted workflow. 


/////////////////////////////////////////////////////////////////////////////
Reference:

A Developer's Introduction to Windows Workflow Foundation (WF) in .NET 4
http://msdn.microsoft.com/en-us/library/ee342461.aspx

