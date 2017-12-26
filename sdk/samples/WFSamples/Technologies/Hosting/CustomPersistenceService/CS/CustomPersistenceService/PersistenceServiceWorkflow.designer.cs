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
using System.Threading;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.CustomPersistenceService
{
    public partial class PersistenceServiceWorkflow : SequentialWorkflowActivity
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        public void InitializeComponent()
        {
            this.CanModifyActivities = true;
            // set delay id and 10 second timeout
            this.delay = new DelayActivity();
            this.delay.Name = "delay";
            this.delay.TimeoutDuration = new TimeSpan(0, 0, 5);

            // add delay activity to workflow
            this.Activities.Add(this.delay);

            // set workflow name
            this.Name = "PersistenceServiceWorkflow";
            this.CanModifyActivities = false;
        }

        private DelayActivity delay;
    }
}
