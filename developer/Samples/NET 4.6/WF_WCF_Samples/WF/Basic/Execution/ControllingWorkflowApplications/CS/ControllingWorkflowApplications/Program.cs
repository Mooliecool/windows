//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Windows;

namespace Microsoft.Samples.WorkflowApplicationReadLineHost
{

    class Program
    {
        [STAThread]
        static void Main()
        {
            HostView hostView = new HostView();
            WorkflowApplicationManager manager = new WorkflowApplicationManager(hostView);
            Application application = new Application();
            application.Run(hostView);
        }
    }
}
