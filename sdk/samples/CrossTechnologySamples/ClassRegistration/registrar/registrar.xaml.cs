using System;
using System.Collections.Generic;
using System.Threading;
using System.Windows;
using System.Workflow.Activities;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;
using System.Workflow.Runtime.Tracking;


namespace Microsoft.Samples.ClassRegistration
{
    public partial class Registrar : Window, IDisposable
    {
        // Workflow runtime components
        WorkflowRuntime workflowRuntime = new WorkflowRuntime();
        SqlWorkflowPersistenceService sqlService;
        RegistrationApprovalService registrationApprover = new RegistrationApprovalService();
        
        // List of pending registrations
        List<RegistrationInformation> PendingRegistrations = new List<RegistrationInformation>();
        
        public Registrar()
        {
            InitializeComponent();
            InitializeWorkflowRuntime();
        }

        private void PageLoad(object sender, EventArgs args)
        {
            LoadActiveWorkflows();
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                // Dispose managed resources
                workflowRuntime.Dispose();
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(true);
        }

        private void InitializeWorkflowRuntime()
        {
            // Get tracking database connection string from application settings
            String connectionString = Properties.Settings.Default.ClassRegistrationTrackingConnectionString;

            // Add External Data Service to process approval/rejection
            ExternalDataExchangeService dataService = new ExternalDataExchangeService();
            workflowRuntime.AddService(dataService);
            dataService.AddService(registrationApprover);

            // Add Sql Persistence service
            sqlService = new SqlWorkflowPersistenceService(connectionString);
            workflowRuntime.AddService(sqlService);
            
            // Start runtime
            workflowRuntime.StartRuntime();
        }

        private void OnGetRegistrations(object sender, EventArgs args)
        {
            LoadActiveWorkflows();
        }
        
        private void LoadActiveWorkflows()
        {
            // Refresh list by clearing out previous entries
            PendingRegistrations.Clear();
            
            // Process all workflows that are persisted in the tracking database
            foreach (SqlPersistenceWorkflowInstanceDescription desc in sqlService.GetAllWorkflows())
            {
                SqlTrackingQuery query = new SqlTrackingQuery(Properties.Settings.Default.ClassRegistrationTrackingConnectionString);
                SqlTrackingWorkflowInstance instance;
                
                RegistrationInformation info = new RegistrationInformation();
                info.Guid = desc.WorkflowInstanceId;

                // Get latest data from tracking database
                if (query.TryGetWorkflow(desc.WorkflowInstanceId, out instance) == true)
                {
                    foreach (ActivityTrackingRecord activityRecord in instance.ActivityEvents)
                    {
                        // Only get properties from the 'listenForRegistrarApproval' activity
                        if (activityRecord.QualifiedName == "listenForRegistrarApproval")
                        {
                            foreach (TrackingDataItem dataItem in activityRecord.Body)
                            {
                                switch (dataItem.FieldName)
                                {
                                    case "UserId":
                                        info.UserId = dataItem.Data as String;
                                        break;
                                    case "SessionId":
                                        info.SessionId = dataItem.Data as String;
                                        break;
                                }
                            }
                        }
                    }

                }

                // Add registration information to list box
                PendingRegistrations.Add(info);
            }

            // Bind list box to list
            RegistrationList.DataContext = PendingRegistrations;
        }

        private void OnRegistrationApproved(object sender, EventArgs args)
        {
            if (RegistrationList.SelectedItem != null)
            {
                Guid registrationId = ((RegistrationInformation)RegistrationList.SelectedItem).Guid;

                // Load workflow from persistence database
                WorkflowInstance workflowInstance = workflowRuntime.GetWorkflow(registrationId);
                
                // Approve registration
                registrationApprover.ApproveRegistration(workflowInstance.InstanceId);

                // Remove current registration from list
                PendingRegistrations.Remove(RegistrationList.SelectedItem as RegistrationInformation);
                
                // Update data context
                RegistrationList.DataContext = null;
                RegistrationList.DataContext = PendingRegistrations;
            }
        }

        private void OnRegistrationRejected(object sender, EventArgs args)
        {
            if (RegistrationList.SelectedItem != null)
            {
                Guid registrationId = ((RegistrationInformation)RegistrationList.SelectedItem).Guid;

                // Load workflow from persistence database
                WorkflowInstance workflowInstance = workflowRuntime.GetWorkflow(registrationId);

                // Reject registration
                registrationApprover.RejectRegistration(workflowInstance.InstanceId);

                // Remove current registration from list
                PendingRegistrations.Remove(RegistrationList.SelectedItem as RegistrationInformation);

                // Update data context
                RegistrationList.DataContext = null;
                RegistrationList.DataContext = PendingRegistrations;
            }
        }
    }

    public class RegistrationInformation
    {
        private Guid   _guid;
        private String _userId;
        private String _sessionId;

        public Guid Guid
        {
            get { return _guid; }
            set { _guid = value; }
        }

        public String UserId
        {
            get { return _userId; }
            set { _userId = value; }
        }

        public String SessionId
        {
            get { return _sessionId; }
            set { _sessionId = value; }
        }
    }

    [Serializable]
    class RegistrationApprovalService : IRegistrarService
    {
        private Guid registrationId;

        public void ApproveRegistration(Guid registrationId)
        {
            this.registrationId = registrationId;
            ThreadPool.QueueUserWorkItem(new WaitCallback(RaiseRegistrationApprovedEvent), (RegistrationApprovalService)this);
        }

        public void RejectRegistration(Guid registrationId)
        {
            this.registrationId = registrationId;
            ThreadPool.QueueUserWorkItem(new WaitCallback(RaiseRegistrationRejectedEvent), (RegistrationApprovalService)this);
        }

        private void RaiseRegistrationApprovedEvent(object stateInfo)
        {
            RegistrationApprovalService registrationApprovalService = stateInfo as RegistrationApprovalService;
            EventHandler<RegistrationEventArgs> registrationApproved = this.Approved;

            if (registrationApproved != null)
            {
                registrationApproved(this, new RegistrationEventArgs(registrationId));
            }
        }

        private void RaiseRegistrationRejectedEvent(object stateInfo)
        {
            RegistrationApprovalService registrationApprovalService = stateInfo as RegistrationApprovalService;
            EventHandler<RegistrationEventArgs> registrationRejected = this.Rejected;

            if (registrationRejected != null)
            {
                registrationRejected(this, new RegistrationEventArgs(registrationId));
            }
        }

        public event EventHandler<RegistrationEventArgs> Approved;
        public event EventHandler<RegistrationEventArgs> Rejected;
    }
}