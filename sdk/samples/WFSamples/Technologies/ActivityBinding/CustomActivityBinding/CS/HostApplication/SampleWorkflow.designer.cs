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

namespace Microsoft.Samples.Workflow.CustomActivityBinding
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
            System.Workflow.ComponentModel.ActivityBind activitybind1 = new System.Workflow.ComponentModel.ActivityBind();
            this.Print = new CustomActivity();
            // 
            // Print
            // 
            this.Print.Name = "Print";
            activitybind1.Name = "SampleWorkflow";
            activitybind1.Path = "NameToPrint";
            this.Print.SetBinding(CustomActivity.NameToPrintProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            // 
            // SampleWorkflow
            // 
            this.Activities.Add(this.Print);
            this.Name = "SampleWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private CustomActivity Print;


    }
}
