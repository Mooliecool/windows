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

namespace Microsoft.Samples.Workflow.WorkflowDesignerRehosting
{
    public partial class ActivityProperties : Form
    {
        private DialogResult result;

        public ActivityProperties()
        {
            InitializeComponent();
            result = DialogResult.Cancel; 
        }

        public string Description
        {
            get
            {
                return this.descriptionTextBox.Text;
            }
        }

        public string ActivityName
        {
            get
            {
                return this.nameTextBox.Text;
            }
        }

        public DialogResult Result
        {
            get
            {
                return this.result;
            }
        }

        private void okButton_Click(object obj, EventArgs e)
        {
            this.result = DialogResult.OK;
            this.Close();
        }
    }
}