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

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility.Dialogs
{
    using System;
    using System.Windows.Forms;

    public partial class AssemblyResolveExplanationDialog : Form
    {
        private bool doNotShowExplanationValue;

        public AssemblyResolveExplanationDialog()
        {
            InitializeComponent();
        }

        public bool DoNotShowExplanation
        {
            get
            {
                return doNotShowExplanationValue;
            }
            set
            {
                doNotShowExplanationValue = value;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            doNotShowExplanationValue = checkBox1.Checked;
        }
    }
}