//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System.Activities;

namespace Microsoft.Samples.WorkflowApplicationReadLineHost
{

    interface IWorkflowApplicationHandler
    {
        void OnAborted(WorkflowApplicationAbortedEventArgs e);

        void OnCompleted(WorkflowApplicationCompletedEventArgs e);

        UnhandledExceptionAction OnUnhandledException(WorkflowApplicationUnhandledExceptionEventArgs e);

        void OnIdle(WorkflowApplicationIdleEventArgs e);

    }
}
