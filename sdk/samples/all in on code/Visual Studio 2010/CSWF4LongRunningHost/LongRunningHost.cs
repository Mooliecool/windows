/****************************** Module Header ******************************\
* Module Name:  LongRunningHost.cs
* Project:      CSWF4LongRunningHost
* Copyright (c) Microsoft Corporation.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Activities;
using System.Threading;
using System.Xml.Linq;
using System.Activities.DurableInstancing;
using System.Runtime.DurableInstancing;
using System.Configuration;


namespace CSWF4LongRunningHost
{
    public class LongRunningWFHost
    {
        Activity workflow = null;
        ManualResetEvent waitHandler = new ManualResetEvent(false);
        static XName wfHostTypeName;
        bool completed = false;
        private static readonly XName WorkflowHostTypePropertyName =
            XNamespace.Get("urn:schemas-microsoft-com:System.Activities/4.0/properties").
            GetName("WorkflowHostType");
        SqlWorkflowInstanceStore instanceStore = null;
        InstanceHandle instanceHandle = null;

        public LongRunningWFHost(Activity workflow)
        {
            this.workflow = workflow;
        }

        public void Run()
        {
            wfHostTypeName = XName.Get("Version" + Guid.NewGuid().ToString(),
                typeof(Workflow1).FullName);
            this.instanceStore = SetupSqlpersistenceStore();
            this.instanceHandle = CreateInstanceStoreOwnerHandle(
                instanceStore, wfHostTypeName);
            WorkflowApplication wfApp = CreateWorkflowApp();
            wfApp.Run();
            while (true)
            {
                this.waitHandler.WaitOne();
                if (completed)
                {
                    break;
                }
                WaitForRunnableInstance(this.instanceHandle);
                wfApp = CreateWorkflowApp();
                try
                {
                    wfApp.LoadRunnableInstance();
                    waitHandler.Reset();
                    wfApp.Run();
                }
                catch (InstanceNotReadyException)
                {
                    Console.WriteLine("Handled expected InstanceNotReadyException, retrying...");
                }
            }
            Console.WriteLine("workflow completed.");
        }

        public void WaitForRunnableInstance(InstanceHandle handle)
        {
            var events = instanceStore.WaitForEvents(handle, TimeSpan.MaxValue);
            bool foundRunnable = false;
            foreach (var persistenceEvent in events)
            {
                if (persistenceEvent.Equals(HasRunnableWorkflowEvent.Value))
                {
                    foundRunnable = true;
                    break;
                }
            }
            if (!foundRunnable)
            {
                Console.WriteLine("no runnable instance");
            }
        }

        public WorkflowApplication CreateWorkflowApp()
        {
            WorkflowApplication wfApp = new WorkflowApplication(workflow);
            wfApp.InstanceStore = this.instanceStore;
            
            Dictionary<XName, object> wfScope = new Dictionary<XName, object>
            {
                { WorkflowHostTypePropertyName, wfHostTypeName }
            };
            wfApp.AddInitialInstanceValues(wfScope);
            wfApp.Unloaded = (e) =>
            {
                Console.WriteLine("Unloaded");
                this.waitHandler.Set();
            };
            wfApp.Completed = (e) =>
            {
                this.completed = true;
            };
            wfApp.PersistableIdle = (e) =>
            {
                return PersistableIdleAction.Unload;
            };
            wfApp.Aborted = delegate(WorkflowApplicationAbortedEventArgs abortArgs)
            {
                Console.WriteLine("Workflow aborted (expected in this sample)");
            };
            return wfApp;
        }

        private SqlWorkflowInstanceStore SetupSqlpersistenceStore()
        {
            string connectionString =
                ConfigurationManager.
                AppSettings["SqlWF4PersistenceConnectionString"].ToString();
            SqlWorkflowInstanceStore sqlWFInstanceStore =
                new SqlWorkflowInstanceStore(connectionString);
            return sqlWFInstanceStore;
        }

        private static InstanceHandle CreateInstanceStoreOwnerHandle(
            InstanceStore store, XName wfHostTypeName)
        {
            InstanceHandle ownerHandle = store.CreateInstanceHandle();
            CreateWorkflowOwnerCommand ownerCommand =
                new CreateWorkflowOwnerCommand()
                {
                    InstanceOwnerMetadata = {
                    { WorkflowHostTypePropertyName, 
                       new InstanceValue(wfHostTypeName) }
                    }
                };
            store.DefaultInstanceOwner = store.Execute(ownerHandle, ownerCommand, 
                TimeSpan.FromSeconds(30)).InstanceOwner;
            return ownerHandle;
        }
    }
}
