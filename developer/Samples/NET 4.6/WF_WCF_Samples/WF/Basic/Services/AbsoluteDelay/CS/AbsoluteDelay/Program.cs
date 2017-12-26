//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.DurableInstancing;
using System.Collections.Generic;
using System.Runtime.DurableInstancing;
using System.Threading;
using System.Xml.Linq;

namespace Microsoft.Samples.AbsoluteDelay
{
    // This sample demonstrates how to use the durable timers feature of WF 4.0 with WorkflowApplication. A lot of the steps below are handled automatically by WorkflowServiceHost, so
    // we recommend that you consider using WorkflowServiceHost if you require durable timers support. However, there are scenarios where it doesn't make sense to use  WorkfowServiceHost 
    // and durable timers are still required. This sample can be used as a starting point for those scenarios.
    class Program
    {
        // The connection string to the pre-configured instance store
        private const string persistenceConn = "Data Source=localhost\\sqlexpress;Initial Catalog=AbsoluteDelaySampleDB;Integrated Security=True;Asynchronous Processing=True";

        // A well known property that is needed by WorkflowApplication and the InstanceStore
        private static readonly XName WorkflowHostTypePropertyName = XNamespace.Get("urn:schemas-microsoft-com:System.Activities/4.0/properties").GetName("WorkflowHostType");

        // events to help synchronize the stages of the sample 
        private static ManualResetEvent workflowUnloadedEvent = new ManualResetEvent(false);

        private static bool workflowCompleted;

        static void Main(string[] args)
        {
            SqlWorkflowInstanceStore store = new SqlWorkflowInstanceStore(persistenceConn);

            // Create a unique name that is used to associate instances in the instance store hosts that can load them. This is needed to prevent a Workflow host from loading
            // instances that have different implementations. The unique name should change whenever the implementation of the workflow changes to prevent workflow load exceptions.
            // For the purposes of the demo we create a unique name every time the program is run.
            XName wfHostTypeName = XName.Get("Version" + Guid.NewGuid().ToString(), typeof(Workflow1).FullName);

            // Create an InstanceStore owner that is associated with the workflow type
            InstanceHandle ownerHandle = CreateInstanceStoreOwner(store, wfHostTypeName);

            // Create a WorkflowApplication instance to start a new workflow
            WorkflowApplication wfApp = CreateWorkflowApplication(new Workflow1(), store, wfHostTypeName);

            // This will create the workflow and execute it until the delay is executed, the workflow goes idle, and is unloaded
            wfApp.Run();

            // This loop handles re-loading workflows that have been unloaded due to durable timers
            while (true)
            {
                // Wait for workflow to unload
                workflowUnloadedEvent.WaitOne();

                // Break the loop once the workflow has completed
                if (workflowCompleted)
                {
                    break;
                }

                // Wait for a timer registered by the delay to expire and the workflow instance to become "runnable" again
                WaitForRunnableInstance(store, ownerHandle);

                // Create a new WorkflowApplication instance to host the re-loaded workflow
                wfApp = CreateWorkflowApplication(new Workflow1(), store, wfHostTypeName);

                try
                {
                    // Re-load the runnable workflow instance and run it
                    wfApp.LoadRunnableInstance();

                    workflowUnloadedEvent.Reset();

                    wfApp.Run();
                }
                catch (InstanceNotReadyException)
                {
                    Console.WriteLine("Handled expected InstanceNotReadyException, retrying...");
                }
            }

            Console.WriteLine("Sample finished, hit enter to close");
            Console.ReadLine();
        }

        // Configure a Default Owner for the instance store so instances can be re-loaded from WorkflowApplication
        private static InstanceHandle CreateInstanceStoreOwner(InstanceStore store, XName wfHostTypeName)
        {
            InstanceHandle ownerHandle = store.CreateInstanceHandle();

            CreateWorkflowOwnerCommand ownerCommand = new CreateWorkflowOwnerCommand()
            {
                InstanceOwnerMetadata =
                {
                    { WorkflowHostTypePropertyName, new InstanceValue(wfHostTypeName) }
                }
            };

            store.DefaultInstanceOwner = store.Execute(ownerHandle, ownerCommand, TimeSpan.FromSeconds(30)).InstanceOwner;

            return ownerHandle;
        }

        // Creates and configures a new instance of WorkflowApplication
        private static WorkflowApplication CreateWorkflowApplication(Activity rootActivity, InstanceStore store, XName wfHostTypeName)
        {
            WorkflowApplication wfApp = new WorkflowApplication(rootActivity);

            wfApp.InstanceStore = store;

            Dictionary<XName, object> wfScope = new Dictionary<XName, object> 
            {
                 { WorkflowHostTypePropertyName, wfHostTypeName }
            };

            // Add the WorkflowHostType value to workflow application so that it stores this data in the instance store when persisted
            wfApp.AddInitialInstanceValues(wfScope);            

            // This statement is optional (see the comments in AbsoluteDelay.CacheMetadata details for more info).
            // wfApp.Extensions.Add<DurableTimerExtension>(() => new DurableTimerExtension());

            // For demonstration purposes the workflow is unloaded as soon as it is idle (and able to persist)
            wfApp.PersistableIdle = delegate(WorkflowApplicationIdleEventArgs idleArgs)
            { 
                Console.WriteLine("Workflow unloading..."); 
                return PersistableIdleAction.Unload;
            };
            
            // Configure some tracing and synchronization for the other WorkflowApplication events

            wfApp.Unloaded = delegate(WorkflowApplicationEventArgs eargs)
            {
                if (!workflowCompleted)
                {
                    Console.WriteLine("Workflow unloaded");
                }
                else
                {
                    Console.WriteLine("Workflow unloaded after completing");
                }

                workflowUnloadedEvent.Set();
            };

            wfApp.Completed = delegate
            { 
                Console.WriteLine("Workflow completed");
                workflowCompleted = true;
            };
            
            wfApp.Aborted = delegate(WorkflowApplicationAbortedEventArgs abortArgs)
            { 
                Console.WriteLine("Workflow aborted (expected in this sample)");
            };

            return wfApp;
        }

        private static void WaitForRunnableInstance(InstanceStore store, InstanceHandle ownerHandle)
        {
            IEnumerable<InstancePersistenceEvent> events = store.WaitForEvents(ownerHandle, TimeSpan.MaxValue);

            bool foundRunnable = false;

            // Loop through the persistence events looking for the HasRunnableWorkflow event (in this sample, it corresponds with
            // the workflow instance whose timer has expired and is ready to be resumed by the host). 
            foreach (InstancePersistenceEvent persistenceEvent in events)
            {
                if (persistenceEvent.Equals(HasRunnableWorkflowEvent.Value))
                {
                    foundRunnable = true;
                    break;
                }
            }

            if (!foundRunnable)
            {
                throw new ApplicationException("Unexpected: No runnable instances found in the instance store");
            }
        }
    }
}
