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

using System.Workflow.ComponentModel;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.TerminationTrackingServiceSample
{
    partial class SampleWorkflow
    {
        #region Designer generated code
        
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            this.TerminateWorkflow = new System.Workflow.ComponentModel.TerminateActivity();
            this.Track = new System.Workflow.Activities.CodeActivity();
            // 
            // TerminateWorkflow
            // 
            this.TerminateWorkflow.Error = "Workflow explicitly terminated";
            this.TerminateWorkflow.Name = "TerminateWorkflow";
            // 
            // Track
            // 
            this.Track.Name = "Track";
            this.Track.ExecuteCode += new System.EventHandler(this.Track_ExecuteCode);
            // 
            // SampleWorkflow
            // 
            this.Activities.Add(this.Track);
            this.Activities.Add(this.TerminateWorkflow);
            this.Name = "SampleWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private TerminateActivity TerminateWorkflow;
        private CodeActivity Track;




    }
}
