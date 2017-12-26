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
using System.Windows.Forms;
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility.Dialogs
{
    /// <summary>
    /// Dialog for hosting the WorkflowViewPanel.
    /// </summary>
    public partial class WorkflowDesignerDialog : Form
    {
        /// <summary>
        /// Create a new dialog to display the specified
        /// workflow type.
        /// </summary>
        /// <param name="wfType">Type of the workflow to display.</param>
        public WorkflowDesignerDialog(Type workflowType)
        {
            if (workflowType == null)
            {
                throw new ArgumentNullException("workflowType");
            }
        
            if (!typeof(Activity).IsAssignableFrom(workflowType))
            {
                throw new ArgumentException("Cannot display a type which is not Activity.", "workflowType");
            }

            InitializeComponent();

            workflowViewPanel1.DisplayType(workflowType);
        }
    }
}