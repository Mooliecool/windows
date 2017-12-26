//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.DurableInstancing;
using System.Activities.Tracking;
using System.Activities.Validation;
using System.Activities.XamlIntegration;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Data.SqlClient;
using System.IO;
using System.Linq;
using System.Runtime.DurableInstancing;
using System.Xaml;
using System.Xml;
using Microsoft.Samples.WorkflowApplicationReadLineHost.HelperClasses;

namespace Microsoft.Samples.WorkflowApplicationReadLineHost
{

    public class WorkflowApplicationManager : IWorkflowApplicationHandler
    {
        const string connectionString = @"Data Source=.\SQLEXPRESS;Initial Catalog=SampleInstanceStore;Integrated Security=True;Asynchronous Processing=True";
        const string findExistingInstancesSql = "SELECT id, workflowDefinitionPath FROM dbo.WorkflowDefinition";
        IHostView hostView;
        Dictionary<Guid, WorkflowApplicationState> instanceStates;
        object instanceStatesLock;

        InstanceStore instanceStore;

        ManagerState state;
        object managerStateLock;

        public WorkflowApplicationManager(IHostView hostView)
        {
            this.hostView = hostView;
            this.hostView.Initialize(this);
            this.instanceStates = new Dictionary<Guid, WorkflowApplicationState>();
            this.instanceStatesLock = new object();
            this.managerStateLock = new object();
        }

        public void Open()
        {
            this.instanceStore = new SqlWorkflowInstanceStore(connectionString);

            CreateWorkflowOwnerCommand createWorkflowOwnerCommand = new CreateWorkflowOwnerCommand();
            InstanceHandle handle = this.instanceStore.CreateInstanceHandle();

            try
            {
                this.instanceStore.BeginExecute(handle, createWorkflowOwnerCommand, TimeSpan.FromSeconds(30), OnInstanceStoreEndExecute, null);
            }
            catch (InstancePersistenceException persistenceException)
            {
                WriteException(persistenceException, "An error has occured setting up the InstanceStore");
            }
        }

        void OnInstanceStoreEndExecute(IAsyncResult ar)
        {
            InstanceView ownerView;

            try
            {
                ownerView = this.instanceStore.EndExecute(ar);
            }
            catch (InstancePersistenceException persistenceException)
            {
                WriteException(persistenceException, "An error has occured setting up the InstanceStore");
                return;
            }

            this.instanceStore.DefaultInstanceOwner = ownerView.InstanceOwner;

            lock (this.managerStateLock)
            {
                this.state = ManagerState.Opened;
            }

            this.hostView.ManagerOpened();
        }

        public void Run(string xamlPath)
        {
            EnsureOpen();

            FileUtil.LoadFile(xamlPath, RunAfterLoadProgram, this.hostView.ErrorWriter);
        }

        void RunAfterLoadProgram(string originalPath, Stream stream)
        {
            Activity program = null;

            if (!TryLoadAndValidateProgram(stream, originalPath, out program))
            {
                return;
            }

            //create a new instance from this definition
            WorkflowApplication instance = new WorkflowApplication(program);

            if (this.hostView.UsePersistence)
            {
                instance.InstanceStore = this.instanceStore;
                instance.Extensions.Add(() => new WorkflowDefinitionExtension(originalPath, connectionString));
            }

            TextWriter instanceWriter = ConfigureExtensions(instance);
            instance.Run();
            AddInstance(new WorkflowApplicationState(instance, this, instanceWriter));

            this.hostView.SelectInstance(instance.Id);
        }

        public void LoadExistingInstances()
        {
            EnsureOpen();
            SqlConnection connection = new SqlConnection(connectionString);
            SqlCommand command = new SqlCommand(findExistingInstancesSql, connection);
            try
            {
                connection.Open();
                command.BeginExecuteReader(new AsyncCallback(OnEndFindExistingInstances), command);
            }
            catch (SqlException exception)
            {
                this.hostView.ErrorWriter.WriteLine("Could not load existing instances due to a Sql Exception: " + exception.ToString());
            }
        }

        void OnEndFindExistingInstances(IAsyncResult result)
        {
            SqlCommand command = result.AsyncState as SqlCommand;
            try
            {
                SqlDataReader dataReader = command.EndExecuteReader(result);
                while (dataReader.Read())
                {
                    string workflowDefinitionPath = dataReader.GetString(dataReader.GetOrdinal("workflowDefinitionPath"));
                    Guid id = dataReader.GetGuid(dataReader.GetOrdinal("id"));

                    FileUtil.LoadFile(
                        workflowDefinitionPath,
                        (path, stream) =>
                        {
                            Activity program;
                            if (TryLoadAndValidateProgram(stream, workflowDefinitionPath, out program))
                            {
                                WorkflowApplication instance = new WorkflowApplication(program);
                                instance.InstanceStore = this.instanceStore;
                                instance.Extensions.Add(() => new WorkflowDefinitionExtension(workflowDefinitionPath, connectionString));
                                TextWriter instanceWriter = ConfigureExtensions(instance);
                                instance.BeginLoad(id, new AsyncCallback(OnEndLoad), new object[] { instance, instanceWriter });
                            }
                        },
                        this.hostView.ErrorWriter);
                }
            }
            catch (SqlException exception)
            {
                this.hostView.ErrorWriter.WriteLine("Could not load existing instances due to a Sql Exception: " + exception.ToString());
            }
            finally
            {
                command.Connection.Close();
            }
        }

        void OnEndLoad(IAsyncResult result)
        {
            WorkflowApplication instance = (WorkflowApplication)((object[])result.AsyncState)[0];
            TextWriter instanceWriter = (TextWriter)((object[])result.AsyncState)[1];

            try
            {
                instance.EndLoad(result);
            }
            catch (InstanceLockedException)
            {
                //don't load any instances for which we cannot aquire the lock
                this.hostView.ErrorWriter.WriteLine("Could not load existing instance because it was Locked.");
                return;
            }
            catch (InstancePersistenceException e)
            {
                this.hostView.ErrorWriter.WriteLine("Could not load existing instance due to an InstancePersistenceException: " + e.ToString());
            }

            instance.Run();
            AddInstance(new WorkflowApplicationState(instance, this, instanceWriter));            
        }

        public void Close()
        {
            lock (managerStateLock)
            {
                if (this.state == ManagerState.Closed)
                {
                    return;
                }
                else
                {
                    this.state = ManagerState.Closed;
                }
            }

            List<WorkflowApplication> instances;

            lock (instanceStatesLock)
            {
                instances = new List<WorkflowApplication>();
                Guid[] keys = this.instanceStates.Keys.ToArray();
                for (int index = 0; index < keys.Length; index++)
                {
                    WorkflowApplicationState instanceState = this.instanceStates[keys[index]];
                    if (instanceState.IsLoaded)
                    {
                        instances.Add(instanceState.WorkflowApplication);
                    }
                    instanceState.Close();
                    this.instanceStates.Remove(keys[index]);
                }
            }

            Collection<IAsyncResult> unloadCalls = new Collection<IAsyncResult>();
            foreach (WorkflowApplication instance in instances)
            {
                if (this.hostView.UsePersistence)
                {
                    try
                    {
                        IAsyncResult result = instance.BeginUnload(null, instance);
                        unloadCalls.Add(result);
                    }
                    //swallow any exception - this is a best effort Close
                    catch (WorkflowApplicationException) { }
                    catch (InstancePersistenceException) { }
                }
                else
                {
                    instance.Abort();
                }
            }

            //can't wait on multiple handles under an STA thread
            foreach (IAsyncResult result in unloadCalls)
            {
                result.AsyncWaitHandle.WaitOne();
                WorkflowApplication instance = result.AsyncState as WorkflowApplication;
                try
                {
                    instance.EndUnload(result);
                }
                //swallow any exception - this is a best effort Close
                catch (WorkflowApplicationException) { }
                catch (InstancePersistenceException) { }
            }
        }

        public void ResumeBookmark(Guid id, string bookmarkName, string value)
        {
            EnsureOpen();

            WorkflowApplicationState workflowApplicationState = null;

            lock (this.instanceStatesLock)
            {
                this.instanceStates.TryGetValue(id, out workflowApplicationState);
            }

            if (workflowApplicationState == null)
            {
                InstanceNoLongerPresent(id);
                return;
            }

            workflowApplicationState.CanResumeBookmarks = false;

            workflowApplicationState.WorkflowApplication.BeginResumeBookmark(bookmarkName, value, new AsyncCallback(OnEndResumeBookmark), new object[] { workflowApplicationState.WorkflowApplication, bookmarkName });
        }

        void OnEndResumeBookmark(IAsyncResult ar)
        {
            object[] asyncState = ar.AsyncState as object[];
            WorkflowApplication instance = asyncState[0] as WorkflowApplication;
            string bookmarkName = asyncState[1] as string;

            BookmarkResumptionResult result = instance.EndResumeBookmark(ar);

            if (result != BookmarkResumptionResult.Success)
            {
                //it is possible the bookmark has been removed by some other event in the workflow
                //but that event will update the host - no need to do it here
                this.hostView.OutputWriter.WriteLine("Could not resume bookmark: {0} on instance {1}", bookmarkName, instance.Id);
            }
        }

        public void Abort(Guid id, string reason)
        {
            EnsureOpen();
            WorkflowApplicationState workflowApplicationState = null;

            lock (this.instanceStatesLock)
            {
                this.instanceStates.TryGetValue(id, out workflowApplicationState);
            }

            if (workflowApplicationState == null)
            {
                //it is possible this instance has been removed by some other event in the workflow
                //nothing to do
                return;
            }

            workflowApplicationState.WorkflowApplication.Abort(reason);
        }

        public void Cancel(Guid id)
        {
            EnsureOpen();
            WorkflowApplicationState workflowApplicationState = null;

            lock (this.instanceStatesLock)
            {
                this.instanceStates.TryGetValue(id, out workflowApplicationState);
            }

            if (workflowApplicationState == null)
            {
                InstanceNoLongerPresent(id);
                return;
            }

            workflowApplicationState.CanResumeBookmarks = false;

            try
            {
                workflowApplicationState.WorkflowApplication.BeginCancel(new AsyncCallback(OnEndCancel), workflowApplicationState.WorkflowApplication);
            }
            catch (WorkflowApplicationException workflowApplicationException)
            {
                RemoveInstanceWithException(workflowApplicationState.WorkflowApplication.Id, "An error occured during Cancelation", workflowApplicationException);
            }
        }

        void OnEndCancel(IAsyncResult ar)
        {
            WorkflowApplication instance = ar.AsyncState as WorkflowApplication;

            try
            {
                instance.EndCancel(ar);
            }
            catch (WorkflowApplicationException workflowApplicationException)
            {
                RemoveInstanceWithException(instance.Id, "An error occured during Cancelation", workflowApplicationException);
            }
        }

        public void Terminate(Guid id, string reason)
        {
            EnsureOpen();
            WorkflowApplicationState workflowApplicationState = null;

            lock (this.instanceStatesLock)
            {
                this.instanceStates.TryGetValue(id, out workflowApplicationState);
            }

            if (workflowApplicationState == null)
            {
                InstanceNoLongerPresent(id);
                return;
            }

            workflowApplicationState.CanResumeBookmarks = false;

            try
            {
                workflowApplicationState.WorkflowApplication.BeginTerminate(reason, new AsyncCallback(OnEndTerminate), workflowApplicationState.WorkflowApplication);
            }
            catch (WorkflowApplicationException workflowApplicationException)
            {
                RemoveInstanceWithException(workflowApplicationState.WorkflowApplication.Id, "An error occured during Termination", workflowApplicationException);
            }
        }

        void OnEndTerminate(IAsyncResult ar)
        {
            WorkflowApplication instance = ar.AsyncState as WorkflowApplication;

            try
            {
                instance.EndTerminate(ar);
            }
            catch (WorkflowApplicationException workflowApplicationException)
            {
                RemoveInstanceWithException(instance.Id, "An error occured during Termination", workflowApplicationException);
            }
        }

        //handle events from the WorkflowApplication
        void IWorkflowApplicationHandler.OnAborted(WorkflowApplicationAbortedEventArgs e)
        {
            lock (managerStateLock)
            {
                if (this.state == ManagerState.Opened)
                {
                    this.hostView.Dispatch(() => RemoveInstance(e.InstanceId, "Instance was Aborted: " + e.Reason));
                }
            }
        }

        void IWorkflowApplicationHandler.OnCompleted(WorkflowApplicationCompletedEventArgs e)
        {
            lock (managerStateLock)
            {
                if (this.state == ManagerState.Opened)
                {
                    this.hostView.Dispatch(() => CompleteInstance(e.InstanceId, "Completed in the " + e.CompletionState.ToString() + " state"));
                }
            }
        }

        UnhandledExceptionAction IWorkflowApplicationHandler.OnUnhandledException(WorkflowApplicationUnhandledExceptionEventArgs e)
        {
            lock (managerStateLock)
            {
                if (this.state == ManagerState.Opened)
                {
                    WriteException(e.UnhandledException, string.Format("Exception encountered in activity {0}", e.ExceptionSource.DisplayName));
                }
            }

            return UnhandledExceptionAction.Terminate;
        }

        void IWorkflowApplicationHandler.OnIdle(WorkflowApplicationIdleEventArgs e)
        {
            lock (managerStateLock)
            {
                if (this.state == ManagerState.Opened)
                {
                    this.hostView.Dispatch(() => UpdateInstanceState(e.InstanceId));
                }
            }
        }

        bool TryLoadAndValidateProgram(Stream stream, string originalPath, out Activity program)
        {
            program = null;

            Exception loadException = null;

            try
            {
                program = ActivityXamlServices.Load(stream);
            }
            catch (XmlException xmlException)
            {
                loadException = xmlException;
            }
            catch (XamlException xamlException)
            {
                loadException = xamlException;
            }
            catch (ArgumentException argumentException)
            {
                loadException = argumentException;
            }

            ValidationResults results = null;

            //If this is a Dynamic activity - a XamlException might occur
            try
            {
                results = ActivityValidationServices.Validate(program);
            }
            catch(XamlException xamlException)
            {
                loadException = xamlException;
            }

            if (loadException != null)
            {
                WriteException(loadException, "An error has occured loading the specified file: " + originalPath);
                return false;
            }


            foreach (ValidationError error in results.Errors)
            {
                this.hostView.ErrorWriter.WriteLine("{0}: {1} Activity: {2}",
                    "error",
                    error.Message,
                    error.Source.DisplayName);
            }

            foreach (ValidationError warning in results.Warnings)
            {
                this.hostView.ErrorWriter.WriteLine("{0}: {1} Activity: {2}",
                    "warning",
                    warning.Message,
                    warning.Source.DisplayName);
            }

            if (results.Errors.Count > 0)
            {
                this.hostView.ErrorWriter.WriteLine("Could not run Workflow: " + originalPath);
                this.hostView.ErrorWriter.WriteLine("There are validation errors");
                return false;
            }

            return true;
        }

        List<WorkflowApplicationInfo> BuildInstanceInfoList()
        {
            var instanceInfos = from instanceState in this.instanceStates.Values
                                select instanceState.AsWorkflowApplicationInfo();

            return new List<WorkflowApplicationInfo>(instanceInfos);
        }

        void RemoveInstance(Guid id, string message)
        {
            List<WorkflowApplicationInfo> instanceInfos = null;

            lock (instanceStatesLock)
            {
                this.instanceStates.Remove(id);
                instanceInfos = BuildInstanceInfoList();
            }

            this.hostView.UpdateInstances(instanceInfos);
        }

        void CompleteInstance(Guid id, string message)
        {
            List<WorkflowApplicationInfo> instanceInfos = null;

            lock (instanceStatesLock)
            {
                this.instanceStates[id].Close();
                instanceInfos = BuildInstanceInfoList();
            }

            this.hostView.UpdateInstances(instanceInfos);
        }

        void AddInstance(WorkflowApplicationState workflowApplicationState)
        {
            List<WorkflowApplicationInfo> instanceInfos = null;

            lock (instanceStatesLock)
            {
                this.instanceStates.Add(workflowApplicationState.WorkflowApplication.Id, workflowApplicationState);
                instanceInfos = BuildInstanceInfoList();
            }

            this.hostView.UpdateInstances(instanceInfos);
        }

        void UpdateInstanceState(Guid id)
        {
            WorkflowApplicationState workflowApplicationState = null;

            lock (this.instanceStatesLock)
            {
                this.instanceStates.TryGetValue(id, out workflowApplicationState);
            }

            if (workflowApplicationState == null)
            {
                //any event that would result in the removal of the instance will have output the reason to the hostView
                return;
            }

            List<WorkflowApplicationInfo> instanceInfos = null;

            lock (this.instanceStatesLock)
            {
                instanceInfos = BuildInstanceInfoList();
            }

            this.hostView.UpdateInstances(instanceInfos);
        }

        TextWriter ConfigureExtensions(WorkflowApplication instance)
        {
            instance.Extensions.Add(() =>
                {
                    TrackingParticipant participant = new TextWriterTrackingParticipant(this.hostView.OutputWriter);
                    participant.TrackingProfile = CreateTrackingProfile();
                    return participant;
                });

            TextWriter textWriter = this.hostView.CreateInstanceWriter();
            instance.Extensions.Add(() => textWriter);
            return textWriter;
        }

        TrackingProfile CreateTrackingProfile()
        {
            TrackingProfile trackingProfile = new TrackingProfile()
                {
                    Name = "CustomTrackingProfile",
                    Queries = 
                    {
                        new CustomTrackingQuery() 
                        {
                         Name = "*",
                         ActivityName = "*"
                        },
                        new WorkflowInstanceQuery()
                        {
                            States = { "*" }
                        } 
                    }
                };

            if (this.hostView.UseActivityTracking)
            {
                trackingProfile.Queries.Add(
                    new ActivityStateQuery()
                        {

                            ActivityName = "*",
                            States = { "*" },
                            Variables = { "*" }
                        });
                trackingProfile.Queries.Add(
                    new ActivityScheduledQuery() { ChildActivityName = "*" });
            }

            return trackingProfile;
        }

        void EnsureOpen()
        {
            lock (this.managerStateLock)
            {
                if (this.state != ManagerState.Opened)
                {
                    throw new InvalidOperationException("This WorkflowApplicationManager is Closed.");
                }
            }
        }

        void WriteException(Exception e)
        {
            WriteException(e, null);
        }

        void WriteException(Exception e, string preamble)
        {
            this.hostView.ErrorWriter.WriteLine(preamble + " " + e.Message);
        }

        void RemoveInstanceWithException(Guid id, string message, Exception e)
        {
            RemoveInstance(id, "Encountered an Exception");
            WriteException(e, message);
        }

        void InstanceNoLongerPresent(Guid id)
        {
            this.hostView.OutputWriter.WriteLine("The instance: " + id.ToString() + " is no longer available");
        }


        enum ManagerState
        {
            Created = 0,
            Opened = 1,
            Closed = 2
        }

    }
}
