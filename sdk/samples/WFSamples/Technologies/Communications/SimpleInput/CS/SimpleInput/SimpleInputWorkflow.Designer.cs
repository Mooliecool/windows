//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//  This source code is intended only as a supplement to Microsoft
//  Development Tools and/or on-line documentation.  See these other
//  materials for detailed information regarding Microsoft code samples.
// 
//  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.ComponentModel;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.SimpleInput
{
    public sealed partial class SimpleInputWorkflow
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
            System.Workflow.ComponentModel.ActivityBind activitybind1 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.Activities.CodeCondition codecondition1 = new System.Workflow.Activities.CodeCondition();
            this.printInput = new System.Workflow.Activities.CodeActivity();
            this.dequeueInput = new Microsoft.Samples.Workflow.SimpleInput.Input();
            this.sequenceActivity = new System.Workflow.Activities.SequenceActivity();
            this.whileActivity = new System.Workflow.Activities.WhileActivity();
            // 
            // printInput
            // 
            this.printInput.Name = "printInput";
            this.printInput.ExecuteCode += new System.EventHandler(this.OnPrintInput);
            // 
            // dequeueInput
            // 
            activitybind1.Name = "SimpleInputWorkflow";
            activitybind1.Path = "Data";
            this.dequeueInput.Name = "dequeueInput";
            this.dequeueInput.Queue = "Queue";
            this.dequeueInput.SetBinding(Microsoft.Samples.Workflow.SimpleInput.Input.DataProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            // 
            // sequenceActivity
            // 
            this.sequenceActivity.Activities.Add(this.dequeueInput);
            this.sequenceActivity.Activities.Add(this.printInput);
            this.sequenceActivity.Name = "sequenceActivity";
            // 
            // whileActivity
            // 
            this.whileActivity.Activities.Add(this.sequenceActivity);
            codecondition1.Condition += new System.EventHandler<System.Workflow.Activities.ConditionalEventArgs>(this.LoopCondition);
            this.whileActivity.Condition = codecondition1;
            this.whileActivity.Name = "whileActivity";
            // 
            // SimpleInputWorkflow
            // 
            this.Activities.Add(this.whileActivity);
            this.Name = "SimpleInputWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private WhileActivity whileActivity;
        private SequenceActivity sequenceActivity;
        private Input dequeueInput;
        private CodeActivity printInput;





    }
}
