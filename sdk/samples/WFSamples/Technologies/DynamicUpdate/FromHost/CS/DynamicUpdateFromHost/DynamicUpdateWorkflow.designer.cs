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

namespace Microsoft.Samples.Workflow.DynamicUpdateFromHost
{
    public partial class DynamicUpdateWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        public void InitializeComponent()
        {
            this.CanModifyActivities = true;
            //
            // code and ifelse activities
            //
            this.PORequest = new System.Workflow.Activities.CodeActivity();
            this.POCreated = new System.Workflow.Activities.CodeActivity();
            // 
            // PORequest
            // 
            this.PORequest.Name = "PORequest";
            this.PORequest.ExecuteCode += new EventHandler(OnPORequest);
            // 
            // POCreated
            // 
            this.POCreated.Name = "POCreated";
            this.POCreated.ExecuteCode += new EventHandler(OnPOCreated);
            //
            // Credit check delay
            //
            this.CreditCheckDelay = new System.Workflow.Activities.DelayActivity();
            this.CreditCheckDelay.Name = "CreditCheckDelay";
            this.CreditCheckDelay.TimeoutDuration = new TimeSpan(0, 0, 2);
            this.CreditCheckDelay.InitializeTimeoutDuration += new EventHandler(OnCreditCheckDelay);
            // 
            // DynamicUpdateWorkflow
            // 
            this.Activities.Add(this.PORequest);
            this.Activities.Add(this.CreditCheckDelay);
            this.Activities.Add(this.POCreated);

            this.Name = "DynamicUpdateWorkflow";
            this.CanModifyActivities = false;
        }

        private CodeActivity PORequest = null;
        private CodeActivity POCreated = null;
        private DelayActivity CreditCheckDelay = null;
    }
}
