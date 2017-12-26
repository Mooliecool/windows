//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//  This source code is intended only as a supplement to Microsoft
//  Development Tools and/or on-line documentation.  See these other
//  materials for detailed information regarding Microsoft code samples.
// 
//  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Threading;
using System.Windows.Forms;
using System.Workflow.ComponentModel;
using System.Workflow.Runtime;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.CorrelatedLocalService
{
    [Serializable]
    public class TaskEventArgs : ExternalDataEventArgs
    {
        string idValue;
        string assigneeValue;
        string textValue;

        public TaskEventArgs(Guid instanceId, string id, string assignee, string text)
            :base(instanceId)
        {
            this.idValue = id;
            this.assigneeValue = assignee;
            this.textValue = text;
        }

        public string Id
        {
            get { return this.idValue; }
            set { this.idValue = value; }
        }

        public string Assignee
        {
            get { return this.assigneeValue; }
            set { this.assigneeValue = value; }
        }

        public string Text
        {
            get { return this.textValue; }
            set { this.textValue = value; }
        }
    }

    // To identify the corrected TaskCompleted activity after the CreateTask, 
    // the CorrelationParameter is set on taskID parameter


    // The ExternalDataExchange attribute is a required attribute 
    // indicating that the local service participates in data exchange with a workflow
    [ExternalDataExchange]
    [CorrelationParameter("taskId")]
    public interface ITaskService
    {
        // The CorrelationInitializer attribute indicates that the 
        // attributed method is the one that initializes the correlation value 
        // and hence must appear first in the workflow (before other operations defined on the 
        // local service interface) in order for validation to succeed.
        
        [CorrelationInitializer]
        void CreateTask(string taskId, string assignee, string text);


        // The CorrelationAlias attribute overrides the 
        // CorrelationParameter setting for a specific method or event
        // when the correlation value must be obtained from a location other 
        // than that indicated by the CorrelationParameter attribute.
        [CorrelationAlias("taskId", "e.Id")]
        event EventHandler<TaskEventArgs> TaskCompleted;
    }

    public class TaskService : ITaskService
    {
        public void CreateTask(string taskId, string assignee, string text)
        {
            Console.WriteLine("task " + taskId + " created for " + assignee);
            ThreadPool.QueueUserWorkItem(ShowDialog, new TaskEventArgs(WorkflowEnvironment.WorkflowInstanceId, taskId, assignee, text));
        }

        public void RaiseEvent(TaskEventArgs args)
        {
            EventHandler<TaskEventArgs> taskCompleted = this.TaskCompleted;
            if (taskCompleted != null)
                taskCompleted(null, args);
        }

        public void ShowDialog(object state)
        {
            TaskEventArgs taskEventArgs = state as TaskEventArgs;

            MessageBox.Show(string.Format("{0}, click OK when '{1}' completed.", taskEventArgs.Assignee, taskEventArgs.Text), string.Format("Task {0}", taskEventArgs.Id), MessageBoxButtons.OK);
            
            RaiseEvent(taskEventArgs);
        }

        public event EventHandler<TaskEventArgs> TaskCompleted;
    }
}
