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

namespace Microsoft.Samples.Workflow.SimpleStateMachineWorkflow
{
    public partial class StateMachineWorkflow : StateMachineWorkflowActivity
    {
        public StateMachineWorkflow()
        {
            InitializeComponent();
        }

        private void Code1Handler(object sender, EventArgs e)
        {
            Console.WriteLine("In Start State. Transitioning to State 1");
        }

        private void Code2Handler(object sender, EventArgs e)
        {
            Console.WriteLine("In State 1. Transitioning to Completed State");
        }
    }
}



