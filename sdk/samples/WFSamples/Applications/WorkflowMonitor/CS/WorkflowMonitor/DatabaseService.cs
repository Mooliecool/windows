//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Workflow.Runtime.Tracking;
using System.Workflow.Runtime;
using System.Globalization;

namespace Microsoft.Samples.Workflow.WorkflowMonitor
{
    //This class provides database query services against the TrackingService using SqlTrackingQuery APIs
    internal sealed class DatabaseService
    {
        private string connectionString;
        private string serverNameValue = String.Empty;
        private string databaseNameValue = String.Empty;

        internal DatabaseService()
        {
            connectionString = "Persist Security Info=False;Integrated Security=SSPI;database=" + databaseNameValue + ";server=" + serverNameValue;
        }

        internal string ServerName
        {
            get
            {
                return serverNameValue;
            }
            set
            {
                serverNameValue = value;
                connectionString = "Persist Security Info=False;Integrated Security=SSPI;database=" + databaseNameValue + ";server=" + serverNameValue;
            }
        }

        internal string DatabaseName
        {
            get
            {
                return databaseNameValue;
            }
            set
            {
                databaseNameValue = value;
                connectionString = "Persist Security Info=False;Integrated Security=SSPI;database=" + databaseNameValue + ";server=" + serverNameValue;
            }
        }

        internal List<SqlTrackingWorkflowInstance> GetWorkflows(string workflowEvent, DateTime from, DateTime until, TrackingDataItemValue trackingDataItemValue)
        {
            try
            {
                List<SqlTrackingWorkflowInstance> queriedWorkflows = new List<SqlTrackingWorkflowInstance>();
                SqlTrackingQuery sqlTrackingQuery = new SqlTrackingQuery(connectionString);
                SqlTrackingQueryOptions sqlTrackingQueryOptions = new SqlTrackingQueryOptions();
                sqlTrackingQueryOptions.StatusMinDateTime = from.ToUniversalTime();
                sqlTrackingQueryOptions.StatusMaxDateTime = until.ToUniversalTime();
                // If QualifiedName, FieldName, or DataValue is not supplied, we will not query since they are all required to match
                if (!((string.Empty == trackingDataItemValue.QualifiedName) || (string.Empty == trackingDataItemValue.FieldName) || ((string.Empty == trackingDataItemValue.DataValue))))
                    sqlTrackingQueryOptions.TrackingDataItems.Add(trackingDataItemValue);

                queriedWorkflows.Clear();

                if ("created" == workflowEvent.ToLower(CultureInfo.InvariantCulture))
                {
                    sqlTrackingQueryOptions.WorkflowStatus = WorkflowStatus.Created;
                    queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions));
                }
                else if ("completed" == workflowEvent.ToLower(CultureInfo.InvariantCulture))
                {
                    sqlTrackingQueryOptions.WorkflowStatus = WorkflowStatus.Completed;
                    queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions));
                }
                else if ("running" == workflowEvent.ToLower(CultureInfo.InvariantCulture))
                {
                    sqlTrackingQueryOptions.WorkflowStatus = WorkflowStatus.Running;                    
                    queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions));
                }
                else if ("suspended" == workflowEvent.ToLower(CultureInfo.InvariantCulture))
                {
                    sqlTrackingQueryOptions.WorkflowStatus = WorkflowStatus.Suspended;
                    queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions));
                }
                else if ("terminated" == workflowEvent.ToLower(CultureInfo.InvariantCulture))
                {
                    sqlTrackingQueryOptions.WorkflowStatus = WorkflowStatus.Terminated;
                    queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions));
                }
                else if (("all" == workflowEvent.ToLower(CultureInfo.InvariantCulture)) || (string.Empty == workflowEvent) || (null == workflowEvent))
                {
                    sqlTrackingQueryOptions.WorkflowStatus = WorkflowStatus.Created;
                    queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions));

                    sqlTrackingQueryOptions.WorkflowStatus = WorkflowStatus.Completed;
                    queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions));

                    sqlTrackingQueryOptions.WorkflowStatus = WorkflowStatus.Running;
                    queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions));

                    sqlTrackingQueryOptions.WorkflowStatus = WorkflowStatus.Suspended;
                    queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions));

                    sqlTrackingQueryOptions.WorkflowStatus = WorkflowStatus.Terminated;
                    queriedWorkflows.AddRange(sqlTrackingQuery.GetWorkflows(sqlTrackingQueryOptions));

                }
                return queriedWorkflows;

            }
            catch (Exception exception)
            {
                throw new Exception("Exception in GetWorkflows", exception);
            }
        }

        internal bool TryGetWorkflow(Guid workflowInstanceId, out SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance)
        {
            SqlTrackingQuery sqlTrackingQuery = new SqlTrackingQuery(connectionString);
            return sqlTrackingQuery.TryGetWorkflow(workflowInstanceId, out sqlTrackingWorkflowInstance);
        }
    }
}
