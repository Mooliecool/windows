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

namespace Microsoft.Samples.Workflow.Delay
{
    public sealed partial class DelayWorkflow 
    {
        #region Designer generated code
        
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            this.logBeforeDelay = new System.Workflow.Activities.CodeActivity();
            this.delay = new System.Workflow.Activities.DelayActivity();
            this.logAfterDelay = new System.Workflow.Activities.CodeActivity();
            // 
            // logBeforeDelay
            // 
            this.logBeforeDelay.Name = "logBeforeDelay";
            this.logBeforeDelay.ExecuteCode += new System.EventHandler(this.OnLogBeforeDelay);
            // 
            // delay
            // 
            this.delay.Name = "delay";
            this.delay.TimeoutDuration = System.TimeSpan.Parse("00:00:02");
            // 
            // logAfterDelay
            // 
            this.logAfterDelay.Name = "logAfterDelay";
            this.logAfterDelay.ExecuteCode += new System.EventHandler(this.OnLogAfterDelay);
            // 
            // DelayWorkflow
            // 
            this.Activities.Add(this.logBeforeDelay);
            this.Activities.Add(this.delay);
            this.Activities.Add(this.logAfterDelay);
            this.Name = "DelayWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private DelayActivity delay;
        private CodeActivity logBeforeDelay;
        private CodeActivity logAfterDelay;

    }
}
