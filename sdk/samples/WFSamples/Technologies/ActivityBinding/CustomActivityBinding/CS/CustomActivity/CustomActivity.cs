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

namespace Microsoft.Samples.Workflow.CustomActivityBinding
{
    public partial class CustomActivity : Activity
    {
        public CustomActivity()
        {
            InitializeComponent();
        }

        protected override ActivityExecutionStatus Execute(ActivityExecutionContext executionContext)
        {
            Console.WriteLine("Executing custom activity.  NameToPrint: " + NameToPrint);
            return base.Execute(executionContext);
        }

        public static DependencyProperty NameToPrintProperty = DependencyProperty.Register("NameToPrint", typeof(System.String), typeof(CustomActivity));

        [DescriptionAttribute("String to be passed to the custom activity")]
        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public string NameToPrint
        {
            get
            {
                return ((String)(base.GetValue(CustomActivity.NameToPrintProperty)));
            }
            set
            {
                base.SetValue(CustomActivity.NameToPrintProperty, value);
            }
        }
    }
}
