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
using System.ComponentModel.Design;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Design;


namespace Microsoft.Samples.Workflow.WorkflowMonitor
{
    internal sealed class WorkflowDesignSurface : DesignSurface
    {
        internal WorkflowDesignSurface(IMemberCreationService memberCreationService)
        {
            this.ServiceContainer.AddService(typeof(ITypeProvider), new TypeProvider(this.ServiceContainer), true);
            this.ServiceContainer.AddService(typeof(IMemberCreationService), memberCreationService);
            this.ServiceContainer.AddService(typeof(IMenuCommandService), new MenuCommandService(this.ServiceContainer));
        }
    }
}
