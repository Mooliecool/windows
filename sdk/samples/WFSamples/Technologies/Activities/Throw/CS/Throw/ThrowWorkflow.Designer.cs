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
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.ThrowSample
{
    public sealed partial class ThrowWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            System.Workflow.ComponentModel.ActivityBind activitybind1 = new System.Workflow.ComponentModel.ActivityBind();
            this.throwActivity1 = new System.Workflow.ComponentModel.ThrowActivity();
            activitybind1.Name = "ThrowWorkflow";
            activitybind1.Path = "ThrownException";
            // 
            // throwActivity1
            // 
            this.throwActivity1.Name = "throwActivity1";
            this.throwActivity1.SetBinding(System.Workflow.ComponentModel.ThrowActivity.FaultProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            // 
            // ThrowWorkflow
            // 
            this.Activities.Add(this.throwActivity1);
            this.Name = "ThrowWorkflow";
            this.CanModifyActivities = false;

        }

        private Exception thrownExceptionValue = new System.Exception("My Exception Message.");

        public Exception ThrownException
        {
            get { return thrownExceptionValue; }
            set { thrownExceptionValue = value; }
        }

        private ThrowActivity throwActivity1;
	
    }
}
