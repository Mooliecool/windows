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

namespace Microsoft.Samples.Workflow.WhileAndParallel
{
    public sealed partial class WhileAndParallelWorkflow : SequentialWorkflowActivity
    {
        // number of iterations for While loop
        private int count; 

        public WhileAndParallelWorkflow()
        {
            InitializeComponent();
        }

        private void WhileCondition(object sender, ConditionalEventArgs e)
        {
            ++count;
            e.Result = (count <= 2);
        }

        private void OnConsoleMessage1(object sender, EventArgs e)
        {
            Console.WriteLine("\nIn Sequence1 Activity: While loop # {0}", count);
        }

        private void OnConsoleMessage2(object sender, EventArgs e)
        {
            Console.WriteLine("\nIn Sequence2 Activity: While loop # {0}", count);
        }
    }
}
