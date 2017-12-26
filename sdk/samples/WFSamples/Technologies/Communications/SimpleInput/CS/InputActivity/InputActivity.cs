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
using System.ComponentModel;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;

namespace Microsoft.Samples.Workflow.SimpleInput
{
    [ToolboxItem(typeof(ActivityToolboxItem))]
    public class Input : Activity
    {
        public static readonly DependencyProperty QueueProperty = DependencyProperty.Register("Queue", typeof(string), typeof(Input), new PropertyMetadata(DependencyPropertyOptions.Metadata));
        public static readonly DependencyProperty DataProperty = DependencyProperty.Register("Data", typeof(object), typeof(Input));

        // This is the name of the queue
        public string Queue
        {
            get
            {
                return (string)GetValue(QueueProperty);
            }
            set
            {
                SetValue(QueueProperty, value);
            }
        }

        // This is the item that is dequeued        
        public object Data
        {
            get
            {
                return (object)GetValue(DataProperty);
            }
            set
            {
                SetValue(DataProperty, value);
            }
        }

        protected override void Initialize(IServiceProvider provider)
        {
            WorkflowQueuingService qService = provider.GetService(typeof(WorkflowQueuingService)) as WorkflowQueuingService;
            IComparable qName = this.Queue;

            if (!qService.Exists(qName))
            {
                bool transactional = true;

                qService.CreateWorkflowQueue(qName, transactional);
            }
        }

        protected override void Uninitialize(IServiceProvider provider)
        {
            WorkflowQueuingService qService = provider.GetService(typeof(WorkflowQueuingService)) as WorkflowQueuingService;
            IComparable qName = this.Queue;

            if (qService.Exists(qName) && qService.GetWorkflowQueue(qName).Count == 0)
            {
                qService.DeleteWorkflowQueue(qName);
            }

            base.Uninitialize(provider);
        }

        protected override ActivityExecutionStatus Execute(ActivityExecutionContext context)
        {
            //WorkflowQueuingService is a component that allows access to queue items
            //enqueued to a particular workflow instance
            // Items are submitted to a workflow instance from the host using the 
            // EnqueueItem method call on the workflowinstance
            WorkflowQueuingService qService = context.GetService<WorkflowQueuingService>() as WorkflowQueuingService;

            // The Queue name is in the activity context and specified in the Enqueue item call
            IComparable qName = this.Queue;


            //Try to dequeue if there are any items available
            // Register the handler OnQueueItemAvailable when a item is queued
            if (!TryDequeueAndComplete(context, qName))
            {
                qService.GetWorkflowQueue(qName).QueueItemAvailable += this.OnQueueItemAvailable;
                return ActivityExecutionStatus.Executing;
            }

            return ActivityExecutionStatus.Closed;
        }

        // Handler when a queue item is available
        private void OnQueueItemAvailable(object sender, QueueEventArgs args)
        {
            ActivityExecutionContext context = sender as ActivityExecutionContext;

            WorkflowQueuingService qService = context.GetService<WorkflowQueuingService>() as WorkflowQueuingService;
            IComparable qName = this.Queue;

            // Dequeue the item and remove the handler 
            if (TryDequeueAndComplete(context, qName))
            {
                qService.GetWorkflowQueue(qName).QueueItemAvailable -= this.OnQueueItemAvailable;
            }
        }

        // Dequeue the item and then process the item
        private bool TryDequeueAndComplete(ActivityExecutionContext context, IComparable queueName)
        {
            WorkflowQueuingService qService = context.GetService<WorkflowQueuingService>() as WorkflowQueuingService;
            IComparable qName = this.Queue;

            if (qService.GetWorkflowQueue(qName).Count > 0)
            {
                this.Data = qService.GetWorkflowQueue(qName).Dequeue();
                return true;
            }

            return false;
        }
    }
}
