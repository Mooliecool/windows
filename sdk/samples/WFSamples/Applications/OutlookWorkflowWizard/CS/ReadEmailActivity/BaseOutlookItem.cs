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
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.OutlookWorkflowWizard
{
    public partial class BaseOutlookItem: Activity
    {
        public BaseOutlookItem()
        {
            InitializeComponent();
        }

        private String to;
        public string To
        {
            get { return to; }
            set { to = value; }
        }

        private String subject;
        public string Subject
        {
            get { return subject; }
            set { subject = value; }
        }

        private String body;
        public string Body
        {
            get { return body; }
            set { body = value; }
        }
    }
}
