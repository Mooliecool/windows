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
using System.Globalization;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.PersistenceServices
{
    public partial class PersistenceServicesWorkflow : System.Workflow.Activities.SequentialWorkflowActivity
    {
        private CodeActivity code1;
        private DelayActivity delay1;
        private CodeActivity code2;

        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            this.code1 = new System.Workflow.Activities.CodeActivity();
            this.delay1 = new System.Workflow.Activities.DelayActivity();
            this.code2 = new System.Workflow.Activities.CodeActivity();
            // 
            // code1
            // 
            this.code1.Name = "code1";
            this.code1.ExecuteCode += new System.EventHandler(this.OnCode1ExecuteCode);
            // 
            // delay1
            // 
            this.delay1.Name = "delay1";
            this.delay1.TimeoutDuration = System.TimeSpan.Parse("00:00:05");
            // 
            // code2
            // 
            this.code2.Name = "code2";
            this.code2.ExecuteCode += new System.EventHandler(this.OnCode2ExecuteCode);
            // 
            // DelayWorkflow
            // 
            this.Activities.Add(this.code1);
            this.Activities.Add(this.delay1);
            this.Activities.Add(this.code2);
            this.Name = "PersistenceServicesWorkflow";
            this.CanModifyActivities = false;
        }
    }
}
