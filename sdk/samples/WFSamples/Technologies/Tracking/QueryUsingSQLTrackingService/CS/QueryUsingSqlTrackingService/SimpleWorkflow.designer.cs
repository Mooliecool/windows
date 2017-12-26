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

namespace Microsoft.Samples.Workflow.QueryUsingSqlTrackingService
{
    public partial class SimpleWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            this.codeConsoleNotify = new System.Workflow.Activities.CodeActivity();
            // 
            // codeConsoleNotify
            // 
            this.codeConsoleNotify.Name = "codeConsoleNotify";
            this.codeConsoleNotify.ExecuteCode += new EventHandler(Code1Handler);
            // 
            // SimpleWorkflow
            // 
            this.Activities.Add(this.codeConsoleNotify);
            this.Name = "SimpleWorkflow";
            this.CanModifyActivities = false;
        }
        private CodeActivity codeConsoleNotify;
    }
}
