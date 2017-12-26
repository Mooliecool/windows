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

namespace Microsoft.Samples.Workflow.SimpleSequentialWorkflow
{
    public sealed partial class SequentialWorkflow
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
            this.IsUnderLimitIfElseActivity = new System.Workflow.Activities.IfElseActivity();
            this.YesIfElseBranch = new System.Workflow.Activities.IfElseBranchActivity();
            this.NoIfElseBranch = new System.Workflow.Activities.IfElseBranchActivity();
            this.ApprovePO = new System.Workflow.Activities.CodeActivity();
            this.RejectPO = new System.Workflow.Activities.CodeActivity();
            // 
            // IsUnderLimitIfElseActivity
            // 
            this.IsUnderLimitIfElseActivity.Activities.Add(this.YesIfElseBranch);
            this.IsUnderLimitIfElseActivity.Activities.Add(this.NoIfElseBranch);
            this.IsUnderLimitIfElseActivity.Name = "IsUnderLimitIfElseActivity";
            // 
            // YesIfElseBranch
            // 
            this.YesIfElseBranch.Activities.Add(this.ApprovePO);
            codecondition1.Condition += new System.EventHandler<System.Workflow.Activities.ConditionalEventArgs>(this.IsUnderLimit);
            this.YesIfElseBranch.Condition = codecondition1;
            this.YesIfElseBranch.Name = "YesIfElseBranch";
            // 
            // NoIfElseBranch
            // 
            this.NoIfElseBranch.Activities.Add(this.RejectPO);
            this.NoIfElseBranch.Name = "NoIfElseBranch";
            // 
            // ApprovePO
            // 
            this.ApprovePO.Name = "ApprovePO";
            this.ApprovePO.ExecuteCode += new System.EventHandler(this.OnApproved);
            // 
            // RejectPO
            // 
            this.RejectPO.Name = "RejectPO";
            this.RejectPO.ExecuteCode += new System.EventHandler(this.OnRejected);
            // 
            // SequentialWorkflow
            // 
            this.Activities.Add(this.IsUnderLimitIfElseActivity);
            this.Name = "SequentialWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private IfElseActivity IsUnderLimitIfElseActivity;
        private IfElseBranchActivity YesIfElseBranch;
        private CodeActivity ApprovePO;
        private IfElseBranchActivity NoIfElseBranch;
        private CodeActivity RejectPO;
    }
}
