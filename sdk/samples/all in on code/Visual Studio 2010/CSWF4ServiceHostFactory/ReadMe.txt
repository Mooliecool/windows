=============================================================================
	WF4/WCF APPLICATION : CSWF4ServiceHostFactory
=============================================================================
/////////////////////////////////////////////////////////////////////////////
Use:

By using WorkflowServiceHost class, we can create a WF4 service host in code. 
The advantage of using WorkflowServiceHost is that we can add our own workflow 
extensions, tracking participant and persistence store.

So question is: can we use our own WorkflowServiceHost in IIS7 like we did in 
the console application? This sample is for answering this question.

To run the sample:
1. Open CSWF4ServiceHostFactory.sln with Visual Studio 2010
2. Press Ctrl+F5.


/////////////////////////////////////////////////////////////////////////////
Prerequisite

1. Visual Studio 2010
2. .NET Framework 4.0
3. Sql Server


/////////////////////////////////////////////////////////////////////////////
Creation:

1.Create a Workflow Service project named CSWF4ServiceHostFactory.
2.Add assembly references to:
  System.Workflow.Runtime
  System.Workflow.Activities
  System.ServiceModel.Activation
  System.Configuration
  System.Activities.DurableInstancing
  System.Runtime.DurableInstancing
3.Setup WF4 sql persistence store. 
  http://msdn.microsoft.com/en-us/library/ee395773.aspx
  name the persistence database:WF4PersistenceDB.
4.add the following configuration to web.config file right under <configuration> node. 
	<appSettings>
	   <add key="SqlWF4PersistenceConnectionString" 
			value="Data Source=.\sqlexpress;Initial Catalog=WF4PersistenceDB;Integrated Security=True" />
	</appSettings>
5.Create a code file named MyServiceHostFactory.cs
	using System;
	using System.Collections.Generic;
	using System.Workflow.Activities;
	using System.Activities;
	using System.Workflow.Runtime;
	using System.ServiceModel.Activities.Activation;
	using System.ServiceModel.Activities;
	using System.ServiceModel.Description;
	using System.Activities.DurableInstancing;
	using System.Configuration;

	namespace CSWF4ServiceHostFactory
	{
		public class MyServiceHostFactory : 
				System.ServiceModel.Activities.Activation.WorkflowServiceHostFactory 
		{
			protected override WorkflowServiceHost CreateWorkflowServiceHost(WorkflowService service,
																			 Uri[] baseAddresses) 
			{
				WorkflowServiceHost host = 
					base.CreateWorkflowServiceHost(service, baseAddresses);
				string connectionString = 
					ConfigurationManager.AppSettings["SqlWF4PersistenceConnectionString"].ToString();
				SqlWorkflowInstanceStore instanceStore = 
					new SqlWorkflowInstanceStore(connectionString);
				instanceStore.InstanceCompletionAction = 
					InstanceCompletionAction.DeleteNothing;
				host.DurableInstancingOptions.InstanceStore = instanceStore;
				return host;
			}
		}
	}

6.Open web.config file and add a ServiceHostingEnvironment node under the 
  System.serviceModel. 
  	<serviceHostingEnvironment multipleSiteBindingsEnabled="true">
		<serviceActivations>
			<add relativeAddress="~/Service1.xamlx"
						service="Service1.xamlx"
						factory="CSWF4ServiceHostFactory.MyServiceHostFactory"/>
		</serviceActivations>
	</serviceHostingEnvironment>
7.Open the default created Service1.xaml, select SendResponse activity
  check its PersistBeforeSend property. 
8.Build the project and then create a IIS7 WebSite(Framework 4.0), map the physical path 
  to the project path. 
  Note: To prevent that the default Application Pool identity have no permission to access the 
  Web.config file and database, you can try shift the identity to LocalSystem. 
9.Call Service1.xamlx. you can call it in WcfTestClient.exe.（usually, you can 
  find WcfTestClient.exe in C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE）


/////////////////////////////////////////////////////////////////////////////
Reference:

http://xhinker.com/post/WF4Create-Your-Own-ServiceHostFactory.aspx
