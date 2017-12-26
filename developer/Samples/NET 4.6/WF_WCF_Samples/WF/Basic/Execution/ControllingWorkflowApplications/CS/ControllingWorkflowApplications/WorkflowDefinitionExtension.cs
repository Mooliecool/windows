//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities.Hosting;
using System.Activities.Persistence;
using System.Collections.Generic;
using System.Data.SqlClient;
using System.Xml.Linq;

namespace Microsoft.Samples.WorkflowApplicationReadLineHost
{

    class WorkflowDefinitionExtension : PersistenceIOParticipant, IWorkflowInstanceExtension
    {
        const string baseNamespace = "urn:schemas-microsoft-com:System.Activities/4.0/properties";
        static readonly XNamespace workflowNamespace = XNamespace.Get(baseNamespace);
        static readonly XName status = workflowNamespace.GetName("Status");

        static readonly XNamespace workflowApplicationsNamespace = XNamespace.Get("urn:schemas-microsoft-com:Microsoft.Samples.WF/WorkflowApplications/properties");
        static readonly XName definitionInfoName = workflowApplicationsNamespace.GetName("DefinitionInfo");

        const string associateDefinitionSql = "DECLARE @countExisting int SELECT @countExisting = COUNT(*) FROM dbo.WorkflowDefinition WHERE id = @id IF @countExisting = 1 BEGIN UPDATE dbo.WorkflowDefinition SET workflowDefinitionPath = @workflowDefinitionPath WHERE id = @id END ELSE INSERT INTO dbo.WorkflowDefinition (id, workflowDefinitionPath) VALUES(@id, @workflowDefinitionPath)";
        const string deleteDefinitionSql = "DELETE FROM dbo.WorkflowDefinition WHERE id = @id";

        WorkflowInstanceProxy host;
        WorkflowDefinitionInfo definitionInfo;
        string connectionString;

        public WorkflowDefinitionExtension(string definitionPath, string connectionString)
            : this(connectionString)
        {
            this.definitionInfo = new WorkflowDefinitionInfo { Path = definitionPath };
        }

        public WorkflowDefinitionExtension(string connectionString)
            : base(false, true)
        {
            this.connectionString = connectionString;
        }

        public WorkflowDefinitionInfo DefinitionInfo
        {
            get
            {
                return this.definitionInfo;
            }
        }

        protected override IAsyncResult BeginOnSave(IDictionary<XName, object> readWriteValues, IDictionary<XName, object> writeOnlyValues, TimeSpan timeout, AsyncCallback callback, object state)
        {
            return new SaveAsyncResult(this.host.Id, writeOnlyValues[status].ToString(), this.definitionInfo, this.connectionString, timeout, callback, state);
        }

        protected override void EndOnSave(IAsyncResult result)
        {
            SaveAsyncResult.End(result);
        }

        protected override void Abort() { }

        public IEnumerable<object> GetAdditionalExtensions()
        {
            return null;
        }

        public void SetInstance(WorkflowInstanceProxy instance)
        {
            this.host = instance;
        }

        static SqlCommand BuildAssociateDefinitionCommand(SqlConnection sqlConnection, Guid id, string definitionPath)
        {
            SqlCommand command = new SqlCommand(associateDefinitionSql, sqlConnection);
            command.Parameters.AddWithValue("@id", id);
            command.Parameters.AddWithValue("@workflowDefinitionPath", definitionPath);
            return command;
        }

        static SqlCommand BuildDeleteDefinitionCommand(SqlConnection sqlConnection, Guid id)
        {
            SqlCommand command = new SqlCommand(deleteDefinitionSql, sqlConnection);
            command.Parameters.AddWithValue("@id", id);
            return command;
        }

        static bool StatusIsComplete(string status)
        {
            return (status == "Closed" || status == "Canceled" || status == "Faulted");
        }

        class SaveAsyncResult : AsyncResult
        {
            SqlConnection connection;
            SqlCommand command;
            WorkflowDefinitionInfo definitionInfo;
            static AsyncCompletion handleEndExecuteNonQuery;

            public SaveAsyncResult(Guid id, string workflowInstanceStatus, WorkflowDefinitionInfo definitionInfo, string connectionString, TimeSpan timeout, AsyncCallback callback, object state)
                : base(callback, state)
            {
                this.definitionInfo = definitionInfo;

                if (handleEndExecuteNonQuery == null)
                {
                    handleEndExecuteNonQuery = new AsyncCompletion(HandleEndExecuteNonQuery);
                }

                this.connection = new SqlConnection(connectionString);
                if (!StatusIsComplete(workflowInstanceStatus))
                {
                    this.command = BuildAssociateDefinitionCommand(this.connection, id, this.definitionInfo.Path);
                }
                else
                {
                    this.command = BuildDeleteDefinitionCommand(this.connection, id);
                }
                this.connection.Open();

                IAsyncResult result = command.BeginExecuteNonQuery(PrepareAsyncCompletion(handleEndExecuteNonQuery), this);

                if (result.CompletedSynchronously)
                {
                    if (HandleEndExecuteNonQuery(result))
                    {
                        Complete(true);
                    }
                }
            }

            static bool HandleEndExecuteNonQuery(IAsyncResult result)
            {
                SaveAsyncResult thisPtr = (SaveAsyncResult)result.AsyncState;

                try
                {
                    thisPtr.command.EndExecuteNonQuery(result);
                }
                finally
                {
                    thisPtr.connection.Close();
                }

                return true;
            }

            public static void End(IAsyncResult result)
            {
                SaveAsyncResult thisPtr = AsyncResult.End<SaveAsyncResult>(result);
            }
        }
    }
}
