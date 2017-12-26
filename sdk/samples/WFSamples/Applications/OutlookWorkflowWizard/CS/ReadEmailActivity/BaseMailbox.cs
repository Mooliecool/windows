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
using System.ComponentModel;
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.OutlookWorkflowWizard
{
    public partial class BaseMailbox: Activity
    {
        public BaseMailbox()
        {
            InitializeComponent();
        }

        private FilterOption filter;
        public FilterOption Filter
        {
            get { return filter; }
            set { filter = value; }
        }

        private String filterValue;
        public string FilterValue
        {
            get { return filterValue; }
            set { filterValue = value; }
        }
    }

    public enum FilterOption
    {
        [Description("Subject")]
        Subject = 0,
        [Description("From")]
        From = 1,
        [Description("To")]
        To = 2,
        [Description("CC")]
        CC = 3,
        [Description("BCC")]
        Bcc = 4,
    }
}
