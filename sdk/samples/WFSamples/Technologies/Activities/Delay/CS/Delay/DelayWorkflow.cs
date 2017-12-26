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

namespace Microsoft.Samples.Workflow.Delay
{
    public sealed partial class DelayWorkflow : SequentialWorkflowActivity
    {
        DateTime before;
        DateTime after;
        TimeSpan delta;

        public DelayWorkflow()
        {
            InitializeComponent();
        }

        // The event handler that executes on ExecuteCode event of the logBeforeDelay activity
        private void OnLogBeforeDelay(object sender, EventArgs e)
        {
            Console.WriteLine("\nExecuting code activity before the delay");
            before = DateTime.Now;
            Console.WriteLine("Current Time: {0}", before.ToString(CultureInfo.CurrentCulture));
        }

        // The event handler that executes on ExecuteCode event of the logAfterDelay activity
        private void OnLogAfterDelay(object sender, EventArgs e)
        {
            Console.WriteLine("\nExecuting code activity after the delay");
            after = DateTime.Now;
            Console.WriteLine("Current Time: {0}", after.ToString(CultureInfo.CurrentCulture));
            delta = after.Subtract(before);
            Console.WriteLine("\nWorkflow completed after the following delay: {0}", delta.ToString());
        }
      }
}
