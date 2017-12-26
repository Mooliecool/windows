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

namespace Microsoft.Samples.Workflow.SuspendAndTerminate
{
    public sealed partial class SuspendAndTerminateWorkflow 
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
            this.suspend = new System.Workflow.ComponentModel.SuspendActivity();
            this.consoleMessage = new System.Workflow.Activities.CodeActivity();
            this.terminate = new System.Workflow.ComponentModel.TerminateActivity();
            // 
            // suspend
            // 
            this.suspend.Error = null;
            this.suspend.Name = "suspend";
            // 
            // ConsoleMessage
            // 
            this.consoleMessage.Name = "consoleMessage";
            this.consoleMessage.ExecuteCode += new System.EventHandler(this.OnConsoleMessage);
            // 
            // terminate
            // 
            this.terminate.Error = null;
            this.terminate.Name = "terminate";
            // 
            // SuspendAndTerminateWorkflow
            // 
            this.Activities.Add(this.suspend);
            this.Activities.Add(this.consoleMessage);
            this.Activities.Add(this.terminate);
            this.Name = "SuspendAndTerminateWorkflow";
            this.CanModifyActivities = false;
        }

        #endregion

        private SuspendActivity suspend;
        private CodeActivity consoleMessage;
        private TerminateActivity terminate;
    }
}
