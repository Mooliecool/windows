=============================================================================
    WF4 APPLICATION : CSWF4ActivitiesCorrelation
=============================================================================
/////////////////////////////////////////////////////////////////////////////
Use:
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


To run the sample:
1. Setup WF4 persistence store according to:
   http://msdn.microsoft.com/en-us/library/ee395773.aspx
2. Open CSWF4ActivitiesCorrelation.sln with Visual Studio 2010(run as administrator). 
3. Open App.config file and alter the sql connection string. 
4. Press Ctrl+F5 to start the WF service. 
5. Use WCF Test Client to test the WF service.


/////////////////////////////////////////////////////////////////////////////
Prerequisite

1. Visual Studio 2010
2. .NET Framework 4.0


/////////////////////////////////////////////////////////////////////////////
Creation:
1. Create a new Workflow Console Application project and name it: 
   CSWF4ActivitiesCorrelation. To create a Workflow Console Application:
   Click File|New|Visual C#|Workflow|Workflow Console Application. 
2. Add assembly references:
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
6. Open Program.cs file and alter its code to: 
using System;
using System.Activities;
using System.Activities.Statements;
using System.Activities.DurableInstancing;
using System.Configuration;
using System.Threading;
using System.ServiceModel.Activities;

namespace CSWF4ActivitiesCorrelation
{
    class Program
    {
        static void Main(string[] args)
        {
            Uri wfAddress = new Uri(@"http://localhost:8000/WFServices");
            AutoResetEvent waitHandler = new AutoResetEvent(false);
            using (WorkflowServiceHost host =
                new WorkflowServiceHost(new Workflow1(), wfAddress))
            {
                host.WorkflowExtensions.Add(SetupSimplySqlPersistenceStore());
                host.Closed += (obj, arg) =>
                {
                    waitHandler.Set();
                };
                host.Open();
                Console.WriteLine(@"http://localhost:8000/WFServices is opening");
                waitHandler.WaitOne();
            }
        }

        private static SqlWorkflowInstanceStore SetupSimplySqlPersistenceStore()
        {
            string connectionString =
                ConfigurationManager.AppSettings["SqlWF4PersistenceConnectionString"].ToString();
            SqlWorkflowInstanceStore sqlInstanceStore =
                new SqlWorkflowInstanceStore(connectionString);
            sqlInstanceStore.HostLockRenewalPeriod = TimeSpan.FromSeconds(30);
            return sqlInstanceStore;
        }
    }
}

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
http://sharepoint/sites/cfx/workspace/Lists/Samples/AllItems.aspx

