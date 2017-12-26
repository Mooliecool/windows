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
using System.ComponentModel.Design;
using System.Collections;
using System.Drawing;
using System.Reflection;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace Microsoft.Samples.Workflow.StateMachineCommunication
{
    partial class SimpleStateMachine
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
            this.setCompleted = new System.Workflow.Activities.SetStateActivity();
            this.handleSetState = new System.Workflow.Activities.HandleExternalEventActivity();
            this.startStateEventDriven = new System.Workflow.Activities.EventDrivenActivity();
            this.CompletedState = new System.Workflow.Activities.StateActivity();
            this.StartState = new System.Workflow.Activities.StateActivity();
            // 
            // setCompleted
            // 
            this.setCompleted.Name = "setCompleted";
            this.setCompleted.TargetStateName = "CompletedState";
            // 
            // handleSetState
            // 
            this.handleSetState.EventName = "SetState";
            this.handleSetState.InterfaceType = typeof(Microsoft.Samples.Workflow.StateMachineCommunication.IEventService);
            this.handleSetState.Name = "handleSetState";
            // 
            // startStateEventDriven
            // 
            this.startStateEventDriven.Activities.Add(this.handleSetState);
            this.startStateEventDriven.Activities.Add(this.setCompleted);
            this.startStateEventDriven.Name = "startStateEventDriven";
            // 
            // CompletedState
            // 
            this.CompletedState.Name = "CompletedState";
            // 
            // StartState
            // 
            this.StartState.Activities.Add(this.startStateEventDriven);
            this.StartState.Name = "StartState";
            // 
            // SimpleStateMachine
            // 
            this.Activities.Add(this.StartState);
            this.Activities.Add(this.CompletedState);
            this.CompletedStateName = "CompletedState";
            this.DynamicUpdateCondition = null;
            this.InitialStateName = "StartState";
            this.Name = "SimpleStateMachine";
            this.CanModifyActivities = false;

        }

        #endregion

        private EventDrivenActivity startStateEventDriven;
        private HandleExternalEventActivity handleSetState;
        private StateActivity CompletedState;
        private SetStateActivity setCompleted;
        private StateActivity StartState;
    }
}
