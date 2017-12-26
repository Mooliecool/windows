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
using System.Workflow.Activities;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Design;

namespace Microsoft.Samples.Workflow.SimpleReplicator
{
    [ToolboxItemAttribute(typeof(ActivityToolboxItem))]
    public partial class SampleReplicatorChildActivity : SequenceActivity
    {
        public SampleReplicatorChildActivity()
        {
            InitializeComponent();
        }

        private static DependencyProperty InstanceDataProperty = DependencyProperty.Register("InstanceData", typeof(System.String), typeof(SampleReplicatorChildActivity));

        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public string InstanceData
        {
            get
            {
                return ((string)(base.GetValue(SampleReplicatorChildActivity.InstanceDataProperty)));
            }
            set
            {
                base.SetValue(SampleReplicatorChildActivity.InstanceDataProperty, value);
            }
        }

        private void CodeHandler(object sender, EventArgs e)
        {
            // Use the Name property which was populated in the child initialization method on the Replicator
            Console.WriteLine("This is " + this.InstanceData);
        }
    }
}
