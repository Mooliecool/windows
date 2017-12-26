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

namespace Microsoft.Samples.Workflow.BasicActivityDesigner
{
    public sealed partial class BasicActivityDesignerWorkflow
    {
        #region Designer generated code
        
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            this.customActivity1 = new Microsoft.Samples.Workflow.BasicActivityDesigner.CustomActivity();
            // 
            // customActivity1
            // 
            this.customActivity1.Name = "customActivity1";
            // 
            // WorkflowApplication
            // 
            this.Activities.Add(this.customActivity1);
            this.Name = "WorkflowApplication";
            this.CanModifyActivities = false;
        }

        #endregion

        private Microsoft.Samples.Workflow.BasicActivityDesigner.CustomActivity customActivity1;
    }
}
