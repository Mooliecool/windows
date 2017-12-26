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
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.SimpleReplicator
{
    public sealed partial class SimpleReplicatorWorkflow : SequentialWorkflowActivity
    {
        private ArrayList childData = new ArrayList();
        public ArrayList ChildData
        {
            get
            {
                return childData;
            }
        }
        public SimpleReplicatorWorkflow()
        {
            InitializeComponent();
            childData.Add("Child Instance 1");
            childData.Add("Child Instance 2");
        }


        private void ChildInitializer(object sender, ReplicatorChildEventArgs args)
        {
            // Using the InstanceData passed in (which comes from the "childData" ArrayList above)
            // populate a property on the SampleReplicatorChildActivity
            (args.Activity as SampleReplicatorChildActivity).InstanceData = args.InstanceData as string;
        }
    }
}
