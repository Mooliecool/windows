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
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.NestedExceptions
{
    public sealed partial class NestedExceptionsWorkflow
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
            this.processThrowException = new System.Workflow.Activities.CodeActivity();
            this.faultHandlerActivity = new System.Workflow.ComponentModel.FaultHandlerActivity();
            this.throwsException = new System.Workflow.Activities.CodeActivity();
            this.faultHandlersActivity = new System.Workflow.ComponentModel.FaultHandlersActivity();
            this.synchronizationScopeActivity = new System.Workflow.ComponentModel.SynchronizationScopeActivity();
            // 
            // processThrowException
            // 
            this.processThrowException.Name = "processThrowException";
            this.processThrowException.ExecuteCode += new System.EventHandler(this.processThrownException_ExecuteCode);
            // 
            // faultHandlerActivity1
            // 
            this.faultHandlerActivity.Activities.Add(this.processThrowException);
            this.faultHandlerActivity.FaultType = typeof(System.Exception);
            this.faultHandlerActivity.Name = "faultHandlerActivity1";
            // 
            // throwsException
            // 
            this.throwsException.Name = "throwsException";
            this.throwsException.ExecuteCode += new System.EventHandler(this.throwsException_ExecuteCode);
            // 
            // faultHandlersActivity
            // 
            this.faultHandlersActivity.Activities.Add(this.faultHandlerActivity);
            this.faultHandlersActivity.Name = "faultHandlersActivity";
            // 
            // synchronizationScopeActivity
            // 
            this.synchronizationScopeActivity.Activities.Add(this.throwsException);
            this.synchronizationScopeActivity.Name = "synchronizationScopeActivity";
            this.synchronizationScopeActivity.SynchronizationHandles = null;
            // 
            // NestedExceptionsWorkflow
            // 
            this.Activities.Add(this.synchronizationScopeActivity);
            this.Activities.Add(this.faultHandlersActivity);
            this.Name = "NestedExceptionsWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private FaultHandlersActivity faultHandlersActivity;
        private SynchronizationScopeActivity synchronizationScopeActivity;
        private FaultHandlerActivity faultHandlerActivity;
        private CodeActivity processThrowException;
        private CodeActivity throwsException;

    }
}
