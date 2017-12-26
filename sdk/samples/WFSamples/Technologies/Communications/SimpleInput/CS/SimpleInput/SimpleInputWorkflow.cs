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
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.SimpleInput
{
    public sealed partial class SimpleInputWorkflow : SequentialWorkflowActivity
    {
        public SimpleInputWorkflow()
        {
            InitializeComponent();
        }

        // The member 'data' is accessed through binding in the dequeueInput activity.
        private object dataValue= null;

        public object Data
        {
            get { return dataValue; }
            set { dataValue = value; }
        }

        // The event handler for the Condition event of the whileActivity activity
        // The while loop will terminate when the string "exit" is found on the queue
        private void LoopCondition(object sender, ConditionalEventArgs e)
        {
            if ((Data == null))
                // no data received yet, so we want to continue the loop
                e.Result = true;
            else
            {
                // go until the data is equals "exit"
                bool checkData = !((Data as string).Equals("exit"));
                e.Result = checkData;
            }
        }

        private void OnPrintInput(object sender, EventArgs e)
        {
            if (Data != null)         
                Console.WriteLine("Input = " + Data as string);
        }
    }
}
