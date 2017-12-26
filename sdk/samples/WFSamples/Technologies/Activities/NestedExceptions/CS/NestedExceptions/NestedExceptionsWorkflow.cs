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

namespace Microsoft.Samples.Workflow.NestedExceptions
{
    public sealed partial class NestedExceptionsWorkflow : SequentialWorkflowActivity
    {
        public NestedExceptionsWorkflow()
        {
            InitializeComponent();
        }

        // Throws an exception from user code - could also use the ThrowActivity
        private void throwsException_ExecuteCode(object sender, EventArgs e)
        {
            Exception exception = new Exception("Workflow custom generated exception.");
            Console.WriteLine("Throwing {0}; Message = {1}\n", exception.GetType().ToString(), exception.Message);
            throw (exception);  
        }

        // This where the code to process the exception takes place
        private void processThrownException_ExecuteCode(object sender, EventArgs e)
        {
            Exception thrownException = faultHandlerActivity.Fault;
            Console.WriteLine("Caught {0}; Message = {1}", thrownException.GetType().ToString(), thrownException.Message);
        }
    }
}
