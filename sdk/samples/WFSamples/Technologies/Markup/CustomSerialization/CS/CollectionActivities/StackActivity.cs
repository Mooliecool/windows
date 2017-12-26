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
using System.Collections;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.Activities;
using System.ComponentModel.Design.Serialization;

namespace  Microsoft.Samples.Workflow.CustomSerialization
{
    [DesignerSerializer(typeof(StackActivitySerializer), typeof(WorkflowMarkupSerializer))]
    public partial class StackActivity : Activity
    {
        private Stack nameStackValue;

        public Stack NameStack
        {
            get { return nameStackValue; }
            set { nameStackValue = value; }
        }

        public StackActivity()
        {
            InitializeComponent();
            nameStackValue = new Stack();
        }

        protected override ActivityExecutionStatus Execute(ActivityExecutionContext executionContext)
        {
            while (nameStackValue.Count > 0)
                Console.WriteLine("Stack item: " + nameStackValue.Pop().ToString());
            return ActivityExecutionStatus.Closed;
        }
    }

    

    

}
