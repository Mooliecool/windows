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
using System.ComponentModel;
using System.Workflow.Activities;
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.Synchronized
{
    public sealed partial class SynchronizedActivityWorkflow: SequentialWorkflowActivity
    {
        private int count;

        public SynchronizedActivityWorkflow()
        {
            InitializeComponent();
        }


        void Decrement1_ExecuteCode(object sender, EventArgs e)
        {
            this.count--;
            Console.WriteLine("Decrement1 i = " + this.count);
        }

        void Increment1_ExecuteCode(object sender, EventArgs e)
        {
            this.count++;
            Console.WriteLine("Increment1 i = " + this.count);
        }

        void Decrement2_ExecuteCode(object sender, EventArgs e)
        {
            this.count--;
            Console.WriteLine("Decrement2 i = " + this.count);
        }

        void Increment2_ExecuteCode(object sender, EventArgs e)
        {
            this.count++;
            Console.WriteLine("Increment2 i = " + this.count);
        }

        void End_ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("End i = " + this.count);
        }

        void Begin_ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Begin i = " + this.count);
        }
    
    }
}
