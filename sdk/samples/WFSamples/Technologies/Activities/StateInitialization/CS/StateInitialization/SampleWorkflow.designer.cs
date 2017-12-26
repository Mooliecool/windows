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

using System.Workflow.Activities;

namespace Microsoft.Workflow.Samples.StateInitialization
{
    partial class SampleWorkflow
    {
        #region Designer generated code
        
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            this.setCompletedState = new System.Workflow.Activities.SetStateActivity();
            this.ReportStateStart = new System.Workflow.Activities.CodeActivity();
            this.stateInitialization = new System.Workflow.Activities.StateInitializationActivity();
            this.CompletedState = new System.Workflow.Activities.StateActivity();
            this.StartState = new System.Workflow.Activities.StateActivity();
            // 
            // setCompletedState
            // 
            this.setCompletedState.Name = "setCompletedState";
            this.setCompletedState.TargetStateName = "CompletedState";
            // 
            // ReportStateStart
            // 
            this.ReportStateStart.Name = "ReportStateStart";
            this.ReportStateStart.ExecuteCode += new System.EventHandler(this.ReportStateStart_ExecuteCode);
            // 
            // stateInitialization
            // 
            this.stateInitialization.Activities.Add(this.ReportStateStart);
            this.stateInitialization.Activities.Add(this.setCompletedState);
            this.stateInitialization.Name = "stateInitialization";
            // 
            // CompletedState
            // 
            this.CompletedState.Name = "CompletedState";
            // 
            // StartState
            // 
            this.StartState.Activities.Add(this.stateInitialization);
            this.StartState.Name = "StartState";
            // 
            // SampleWorkflow
            // 
            this.Activities.Add(this.StartState);
            this.Activities.Add(this.CompletedState);
            this.CompletedStateName = "CompletedState";
            this.DynamicUpdateCondition = null;
            this.InitialStateName = "StartState";
            this.Name = "SampleWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private StateActivity CompletedState;
        private StateInitializationActivity stateInitialization;
        private CodeActivity ReportStateStart;
        private SetStateActivity setCompletedState;
        private StateActivity StartState;











    }
}
