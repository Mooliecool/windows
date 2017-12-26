//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.IO;

namespace Microsoft.Samples.WorkflowApplicationReadLineHost
{

    public interface IHostView
    {
        bool UsePersistence { get; }

        bool UseActivityTracking { get; }

        TextWriter OutputWriter { get; }

        TextWriter ErrorWriter { get; }

        TextWriter CreateInstanceWriter();

        void Initialize(WorkflowApplicationManager manager);

        void ManagerOpened();

        //rebuild the view of instances and refresh current view state
        void UpdateInstances(List<WorkflowApplicationInfo> instanceInfos);

        void SelectInstance(Guid id);

        void Dispatch(Action work);
    }
}
