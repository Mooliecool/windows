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

namespace Microsoft.Samples.Workflow.WorkflowThreading
{
    public sealed partial class ThreadingWorkflow
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
            System.Workflow.Activities.CodeCondition codecondition1 = new System.Workflow.Activities.CodeCondition();
            this.codeActivity1 = new System.Workflow.Activities.CodeActivity();
            this.ifElseActivity = new System.Workflow.Activities.IfElseActivity();
            this.codeActivity2 = new System.Workflow.Activities.CodeActivity();
            this.codeActivity3 = new System.Workflow.Activities.CodeActivity();
            this.ifElseBranchActivity = new System.Workflow.Activities.IfElseBranchActivity();
            this.ifElseBranchActivity1 = new System.Workflow.Activities.IfElseBranchActivity();
            this.WaitForMessageActivity = new Microsoft.Samples.Workflow.WorkflowThreading.WaitForMessageActivity();
            this.delayActivity = new System.Workflow.Activities.DelayActivity();
            // 
            // codeActivity1
            // 
            this.codeActivity1.Name = "codeActivity1";
            this.codeActivity1.ExecuteCode += new System.EventHandler(this.OnCodeActivity1ExecuteCode);
            // 
            // ifElseActivity
            // 
            this.ifElseActivity.Activities.Add(this.ifElseBranchActivity);
            this.ifElseActivity.Activities.Add(this.ifElseBranchActivity1);
            this.ifElseActivity.Name = "ifElseActivity";
            // 
            // codeActivity2
            // 
            this.codeActivity2.Name = "codeActivity2";
            this.codeActivity2.ExecuteCode += new System.EventHandler(this.OnCodeActivity2ExecuteCode);
            // 
            // codeActivity3
            // 
            this.codeActivity3.Name = "codeActivity3";
            this.codeActivity3.ExecuteCode += new System.EventHandler(this.OnCodeActivity3ExecuteCode);
            // 
            // ifElseBranchActivity
            // 
            this.ifElseBranchActivity.Activities.Add(this.WaitForMessageActivity);
            codecondition1.Condition += new System.EventHandler<System.Workflow.Activities.ConditionalEventArgs>(this.IfElseBranchActivityCodeCondition);
            this.ifElseBranchActivity.Condition = codecondition1;
            this.ifElseBranchActivity.Name = "ifElseBranchActivity";
            // 
            // ifElseBranchActivity1
            // 
            this.ifElseBranchActivity1.Activities.Add(this.delayActivity);
            this.ifElseBranchActivity1.Condition = null;
            this.ifElseBranchActivity1.Name = "ifElseBranchActivity1";
            // 
            // WaitForMessageActivity
            // 
            this.WaitForMessageActivity.Name = "WaitForMessageActivity";
            // 
            // delayActivity
            // 
            this.delayActivity.Name = "delayActivity";
            this.delayActivity.TimeoutDuration = System.TimeSpan.Parse("00:00:02");
            // 
            // ThreadingWorkflow
            // 
            this.Activities.Add(this.codeActivity1);
            this.Activities.Add(this.ifElseActivity);
            this.Activities.Add(this.codeActivity2);
            this.Activities.Add(this.codeActivity3);
            this.Name = "ThreadingWorkflow";
            this.CanModifyActivities = false;
        }

        #endregion

        private CodeActivity codeActivity1;
        private CodeActivity codeActivity2;
        private CodeActivity codeActivity3;
        private WaitForMessageActivity WaitForMessageActivity;
        private IfElseActivity ifElseActivity;
        private IfElseBranchActivity ifElseBranchActivity;
        private IfElseBranchActivity ifElseBranchActivity1;
        private DelayActivity delayActivity;
    }
}
