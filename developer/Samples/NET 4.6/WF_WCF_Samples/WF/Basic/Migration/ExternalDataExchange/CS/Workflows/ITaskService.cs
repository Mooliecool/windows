//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Workflow.Activities;

namespace Microsoft.Samples.InteropDemo
{

    [ExternalDataExchange]
    [CorrelationParameter("taskId")]
    public interface ITaskService
    {
        [CorrelationInitializer]
        void CreateTask(string taskId, string assignee, string text);

        [CorrelationAlias("taskId", "e.Id")]
        event EventHandler<TaskEventArgs> TaskCompleted;
    }
}
