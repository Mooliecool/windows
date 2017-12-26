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

namespace Microsoft.Samples.Workflow.WorkflowSerialization
{
    public partial class ConsoleActivity : Activity
    {
        public ConsoleActivity()
        {
            InitializeComponent();
        }

        protected override ActivityExecutionStatus Execute(ActivityExecutionContext executionContext)
        {
            Console.WriteLine("Executing custom activity.  Input string: " + StringToWrite);
            return ActivityExecutionStatus.Closed;
        }
        public static DependencyProperty StringToWriteProperty = DependencyProperty.Register("StringToWrite", typeof(System.String), typeof(ConsoleActivity));

        [DescriptionAttribute("String to be written to the console")]
        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public string StringToWrite
        {
            get
            {
                return ((String)(base.GetValue(ConsoleActivity.StringToWriteProperty)));
            }
            set
            {
                base.SetValue(ConsoleActivity.StringToWriteProperty, value);
            }
        }
    }
}
