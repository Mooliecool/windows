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
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.SimpleReplicator
{
    public sealed partial class SimpleReplicatorWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            System.Workflow.ComponentModel.ActivityBind activityBind1 = new System.Workflow.ComponentModel.ActivityBind();
            this.SampleReplicatorChildActivity1 = new Microsoft.Samples.Workflow.SimpleReplicator.SampleReplicatorChildActivity();
            this.ReplicatorWork = new System.Workflow.Activities.ReplicatorActivity();
            // 
            // SampleReplicatorChildActivity1
            // 
            this.SampleReplicatorChildActivity1.InstanceData = null;
            this.SampleReplicatorChildActivity1.Name = "SampleReplicatorChildActivity1";
            activityBind1.Name = "SimpleReplicatorWorkflow";
            activityBind1.Path = "ChildData";
            // 
            // ReplicatorWork
            // 
            this.ReplicatorWork.Activities.Add(this.SampleReplicatorChildActivity1);
            this.ReplicatorWork.ExecutionType = System.Workflow.Activities.ExecutionType.Sequence;
            this.ReplicatorWork.Name = "ReplicatorWork";
            this.ReplicatorWork.ChildInitialized += new System.EventHandler<System.Workflow.Activities.ReplicatorChildEventArgs>(this.ChildInitializer);
            this.ReplicatorWork.SetBinding(System.Workflow.Activities.ReplicatorActivity.InitialChildDataProperty, ((System.Workflow.ComponentModel.ActivityBind)(activityBind1)));
            // 
            // SimpleReplicatorWorkflow
            // 
            this.Activities.Add(this.ReplicatorWork);
            this.Name = "SimpleReplicatorWorkflow";
            this.CanModifyActivities = false;

        }

        private SampleReplicatorChildActivity SampleReplicatorChildActivity1;
        private ReplicatorActivity ReplicatorWork;
    }
}
