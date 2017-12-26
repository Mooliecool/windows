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
using System.Workflow.ComponentModel.Design;
using System.Workflow.ComponentModel.Compiler;


namespace Microsoft.Samples.Workflow.TransactionalServiceSample
{
    [ToolboxItem(typeof(ActivityToolboxItem))]
    public partial class DebitAmount : Activity
    {
        public DebitAmount()
        {
            InitializeComponent();
        }
        public static DependencyProperty AmountProperty = System.Workflow.ComponentModel.DependencyProperty.Register("Amount", typeof(Int32), typeof(DebitAmount));

        public Int32 Amount
        {
            get
            {
                return ((Int32)(base.GetValue(DebitAmount.AmountProperty)));
            }
            set
            {
                base.SetValue(DebitAmount.AmountProperty, value);
            }
        }

        public static DependencyProperty OnBeforeInvokeEvent = System.Workflow.ComponentModel.DependencyProperty.Register("OnBeforeInvoke", typeof(EventHandler), typeof(DebitAmount));
        [MergableProperty(false)]
        public event EventHandler OnBeforeInvoke
        {
            add
            {
                base.AddHandler(OnBeforeInvokeEvent, value);
            }
            remove
            {
                base.RemoveHandler(OnBeforeInvokeEvent, value);
            }
        }

        protected override sealed ActivityExecutionStatus Execute(ActivityExecutionContext context)
        {
            // Fire the Before Invoke Handler
            base.RaiseEvent(OnBeforeInvokeEvent, this, EventArgs.Empty);

            // Get reference to the transactional service from the context
            AbstractTransactionService service = context.GetService<AbstractTransactionService>();
            // Call a method on the service and pass the payload to it
            service.DebitAmount(this.Amount);

            // Return the status of the activity as closed
            return ActivityExecutionStatus.Closed;
        }
    }
}
