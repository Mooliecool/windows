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
using System.ComponentModel;
using System.Workflow.Activities;
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.EventArgsTrackingSample
{
    public sealed partial class SuspendedWorkflow
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
            this.suspend1 = new System.Workflow.ComponentModel.SuspendActivity();
            // 
            // suspend1
            // 
            this.suspend1.Error = "Suspending the workflow";
            this.suspend1.Name = "suspend1";
            // 
            // SuspendedWorkflow
            // 
            this.Activities.Add(this.suspend1);
            this.Name = "SuspendedWorkflow";
            this.CanModifyActivities = false;
        }

        #endregion

        private SuspendActivity suspend1;
    }
}
