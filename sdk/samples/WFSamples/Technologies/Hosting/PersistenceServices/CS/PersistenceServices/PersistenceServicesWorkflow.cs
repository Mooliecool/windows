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
using System.Globalization;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.PersistenceServices
{
    //This sample demonstrates how to use the out-of-box SqlWorkflowPersistenceService
    // The workflow is a simple sequential workflow with 3 activities: code1, delay1, code2
    // The code activities are used to display status and current UTC DateTime, the delay 
    // causes the workflow to go idle for 5 seconds.
    public partial class PersistenceServicesWorkflow : System.Workflow.Activities.SequentialWorkflowActivity
    {
        public PersistenceServicesWorkflow()
        {
            InitializeComponent();
        }

        // This is the handler for code1
        private void OnCode1ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("OnCode1ExecuteCode event handler - DateTime: " + DateTime.UtcNow.ToString(CultureInfo.CurrentCulture));
        }

        // This is the handler for code2
        private void OnCode2ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("OnCode2ExecuteCode event handler - DateTime: " + DateTime.UtcNow.ToString(CultureInfo.CurrentCulture));
        }
    }
}