=============================================================================
         CONSOLE APPLICATION : CSWF4LongRunningHost
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

In WF3, when a workflow is delayed and persisted in persistence store, 
after the timer expired, workflow will resume from database automatically.
Now in WF4, we have to resume a persisted workflow manually, So can we create 
a long running WF4 that can monitor a delayed workflow and resume a workflow
automatically after the delay timer expired? this sample include a host that 
can do this. 

To run the sample:
1. Sql Server 2005/2008(or express edition);
2. Setup WF4 sql workflow persistence store according to this MSDN document:
   http://msdn.microsoft.com/en-us/library/ee395773.aspx
3. Open CSWF4SequenceWF.sln with Visual Studio 2010
4. Add sql workflow persistence store connection string to App.config file:
    <?xml version="1.0" encoding="utf-8" ?>
    <configuration>
      <appSettings>
        <add key="SqlWF4PersistenceConnectionString"
             value="Data Source=.\sqlexpress;Initial Catalog=WF4PersistenceDB;
             Integrated Security=True"/>
      </appSettings>
        <startup>      
           <supportedRuntime version="v4.0" 
                             sku=".NETFramework,Version=v4.0,Profile=Client" />               
        </startup>
    </configuration>
5. Press Ctrl+F5.


/////////////////////////////////////////////////////////////////////////////
Creation:

1.Create a Workflow Console Application and name it CSWF4LongRunningHost;

2.Add assemble references:
  a. System.Activities.DurableInstancing. 
  b. System.Runtime.DurableInstancing. 
  c. System.Configuration. 

2.Create a workflow as shown in Workflow1.xaml file

3.Add a new code file to the project named LongRunningHost.cs and fill the 
  file with following code:
    using System;
    using System.Collections.Generic;
    using System.Activities;
    using System.Threading;
    using System.Xml.Linq;
    using System.Activities.DurableInstancing;
    using System.Runtime.DurableInstancing;
    using System.Configuration;

    namespace CSWF4LongRunningHost {
        public class LongRunningWFHost {
            Activity workflow = null;
            ManualResetEvent waitHandler = new ManualResetEvent(false);
            static XName wfHostTypeName;
            bool completed = false;
            private static readonly XName WorkflowHostTypePropertyName =
                XNamespace.Get("urn:schemas-microsoft-com:System.Activities/4.0/properties").
                GetName("WorkflowHostType");
            SqlWorkflowInstanceStore instanceStore = null;
            InstanceHandle instanceHandle = null;
            public LongRunningWFHost(Activity workflow) {
                this.workflow = workflow;
            }
            public void Run() {
                wfHostTypeName = XName.Get("Version" + Guid.NewGuid().ToString(),
                                           typeof(Workflow1).FullName);
                this.instanceStore = SetupSqlpersistenceStore();
                this.instanceHandle =
                    CreateInstanceStoreOwnerHandle(instanceStore, wfHostTypeName);
                WorkflowApplication wfApp = CreateWorkflowApp();
                wfApp.Run();
                while (true) {
                    this.waitHandler.WaitOne();
                    if (completed) {
                        break;
                    }
                    WaitForRunnableInstance(this.instanceHandle);
                    wfApp = CreateWorkflowApp();
                    try {
                        wfApp.LoadRunnableInstance();
                        waitHandler.Reset();
                        wfApp.Run();
                    } catch (InstanceNotReadyException) {
                        Console.WriteLine("Handled expected InstanceNotReadyException, retrying...");
                    }
                }
                Console.WriteLine("workflow completed.");
            }
            public void WaitForRunnableInstance(InstanceHandle handle) {
                var events = instanceStore.WaitForEvents(handle, TimeSpan.MaxValue);
                bool foundRunnable = false;
                foreach (var persistenceEvent in events) {
                    if (persistenceEvent.Equals(HasRunnableWorkflowEvent.Value)) {
                        foundRunnable = true;
                        break;
                    }
                }
                if (!foundRunnable) {
                    Console.WriteLine("no runnable instance");
                }
            }

            public WorkflowApplication CreateWorkflowApp() {
                WorkflowApplication wfApp = new WorkflowApplication(workflow);
                wfApp.InstanceStore = this.instanceStore;
                Dictionary<XName, object> wfScope = new Dictionary<XName, object>{
                    { WorkflowHostTypePropertyName, wfHostTypeName }
                };
                wfApp.AddInitialInstanceValues(wfScope);
                wfApp.Unloaded = (e) => {
                    Console.WriteLine("Unloaded");
                    this.waitHandler.Set();
                };
                wfApp.Completed = (e) => {
                    this.completed = true;
                };
                wfApp.PersistableIdle = (e) => {
                    return PersistableIdleAction.Unload;
                };
                wfApp.Aborted = delegate(WorkflowApplicationAbortedEventArgs abortArgs) {
                    Console.WriteLine("Workflow aborted (expected in this sample)");
                };
                return wfApp;
            }

            private SqlWorkflowInstanceStore SetupSqlpersistenceStore() {
                string connectionString =
                    ConfigurationManager.
                    AppSettings["SqlWF4PersistenceConnectionString"].ToString();
                SqlWorkflowInstanceStore sqlWFInstanceStore =
                    new SqlWorkflowInstanceStore(connectionString);
                return sqlWFInstanceStore;
            }

            private static InstanceHandle CreateInstanceStoreOwnerHandle(InstanceStore store,
                                                                         XName wfHostTypeName) {
                InstanceHandle ownerHandle = store.CreateInstanceHandle();
                CreateWorkflowOwnerCommand ownerCommand = 
                    new CreateWorkflowOwnerCommand() {
                    InstanceOwnerMetadata = {
                         { WorkflowHostTypePropertyName, 
                           new InstanceValue(wfHostTypeName) }
                    }
                };
                store.DefaultInstanceOwner = store.Execute(ownerHandle, 
                                                           ownerCommand,
                                                           TimeSpan.FromSeconds(30)).InstanceOwner;
                return ownerHandle;
            }
        }
    }

4. Open Program.cs file, alter its code to:
    using System;
    using System.Activities;
    using System.Activities.Statements;
    namespace CSWF4LongRunningHost {
        class Program {
            static void Main(string[] args) {
                LongRunningWFHost host = new LongRunningWFHost(new Workflow1());
                host.Run();
            }
        }
    }


/////////////////////////////////////////////////////////////////////////////
Reference:

Absolute Delay
http://msdn.microsoft.com/en-us/library/ff522352.aspx


/////////////////////////////////////////////////////////////////////////////