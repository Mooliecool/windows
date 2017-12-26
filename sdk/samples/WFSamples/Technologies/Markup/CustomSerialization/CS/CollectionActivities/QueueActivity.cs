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
using System.Collections;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.Activities;
using System.ComponentModel.Design.Serialization;

namespace  Microsoft.Samples.Workflow.CustomSerialization
{
    [DesignerSerializer(typeof(QueueActivitySerializer), typeof(WorkflowMarkupSerializer))]
    public partial class QueueActivity: Activity
    {
        private Queue nameQueueValue;
        public QueueActivity()
        {
            InitializeComponent();
            nameQueueValue = new Queue();
        }

        public Queue NameQueue
        {
            get { return nameQueueValue; }
            set { nameQueueValue = value; }
        }

        protected override ActivityExecutionStatus Execute(ActivityExecutionContext executionContext)
        {
            while (nameQueueValue.Count > 0)
                Console.WriteLine("Queue item: " + nameQueueValue.Dequeue().ToString());

            return ActivityExecutionStatus.Closed;
        }
    }
}
