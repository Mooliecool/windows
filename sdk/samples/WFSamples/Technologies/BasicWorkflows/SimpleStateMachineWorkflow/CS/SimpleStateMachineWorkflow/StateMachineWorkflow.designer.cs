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

namespace Microsoft.Samples.Workflow.SimpleStateMachineWorkflow
{
    public partial class StateMachineWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            this.setCompletedState = new System.Workflow.Activities.SetStateActivity();
            this.code2 = new System.Workflow.Activities.CodeActivity();
            this.state1Delay = new System.Workflow.Activities.DelayActivity();
            this.setState1 = new System.Workflow.Activities.SetStateActivity();
            this.code1 = new System.Workflow.Activities.CodeActivity();
            this.startStateDelay = new System.Workflow.Activities.DelayActivity();
            this.eventDriven2 = new System.Workflow.Activities.EventDrivenActivity();
            this.eventDriven1 = new System.Workflow.Activities.EventDrivenActivity();
            this.CompletedState = new System.Workflow.Activities.StateActivity();
            this.state1 = new System.Workflow.Activities.StateActivity();
            this.StartState = new System.Workflow.Activities.StateActivity();
            // 
            // setCompletedState
            // 
            this.setCompletedState.Name = "setCompletedState";
            this.setCompletedState.TargetStateName = "CompletedState";
            // 
            // code2
            // 
            this.code2.Name = "code2";
            this.code2.ExecuteCode += new System.EventHandler(this.Code2Handler);
            // 
            // state1Delay
            // 
            this.state1Delay.Name = "state1Delay";
            this.state1Delay.TimeoutDuration = System.TimeSpan.Parse("00:00:02");
            // 
            // setState1
            // 
            this.setState1.Name = "setState1";
            this.setState1.TargetStateName = "state1";
            // 
            // code1
            // 
            this.code1.Name = "code1";
            this.code1.ExecuteCode += new System.EventHandler(this.Code1Handler);
            // 
            // startStateDelay
            // 
            this.startStateDelay.Name = "startStateDelay";
            this.startStateDelay.TimeoutDuration = System.TimeSpan.Parse("00:00:05");
            // 
            // eventDriven2
            // 
            this.eventDriven2.Activities.Add(this.state1Delay);
            this.eventDriven2.Activities.Add(this.code2);
            this.eventDriven2.Activities.Add(this.setCompletedState);
            this.eventDriven2.Name = "eventDriven2";
            // 
            // eventDriven1
            // 
            this.eventDriven1.Activities.Add(this.startStateDelay);
            this.eventDriven1.Activities.Add(this.code1);
            this.eventDriven1.Activities.Add(this.setState1);
            this.eventDriven1.Name = "eventDriven1";
            // 
            // CompletedState
            // 
            this.CompletedState.Name = "CompletedState";
            // 
            // state1
            // 
            this.state1.Activities.Add(this.eventDriven2);
            this.state1.Name = "state1";
            // 
            // StartState
            // 
            this.StartState.Activities.Add(this.eventDriven1);
            this.StartState.Name = "StartState";
            // 
            // StateMachineWorkflow
            // 
            this.Activities.Add(this.StartState);
            this.Activities.Add(this.state1);
            this.Activities.Add(this.CompletedState);
            this.CompletedStateName = "CompletedState";
            this.DynamicUpdateCondition = null;
            this.InitialStateName = "StartState";
            this.Name = "StateMachineWorkflow";
            this.CanModifyActivities = false;

        }
        private StateActivity StartState;
        private EventDrivenActivity eventDriven1;
        private SetStateActivity setState1;
        private StateActivity state1;
        private EventDrivenActivity eventDriven2;
        private DelayActivity state1Delay;
        private SetStateActivity setCompletedState;
        private CodeActivity code1;
        private CodeActivity code2;
        private DelayActivity startStateDelay;
        private StateActivity CompletedState;
    }
}
