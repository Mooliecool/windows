//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Workflow.Activities;

namespace Microsoft.Samples.Rules
{
    public sealed partial class Workflow : SequentialWorkflowActivity
    {
        private string customerName;                // that is,. "John Customer".
        private int itemNum;                        // that is, 1 => for Vista Ultimate DVD.
        private string zipCode;                     // that is, "00999".
        private OrderError invalidOrder;
        private OrderErrorCollection invalidZipCodeErrorCollection;
        private OrderErrorCollection invalidItemNumErrorCollection;
        private OrderErrorCollection invalidOrdersCollection;

        public string CustomerName
        {
            get
            {
                return this.customerName;
            }
            set
            {
                this.customerName = value;
            }
        }

        public int ItemNum
        {
            get
            {
                return this.itemNum;
            }
            set
            {
                this.itemNum = value;
            }
        }

        public string ZipCode
        {
            get
            {
                return this.zipCode;
            }
            set
            {
                this.zipCode = value;
            }
        }

        public Workflow()
        {
            invalidOrder = new OrderError();
            invalidZipCodeErrorCollection = new OrderErrorCollection();
            invalidItemNumErrorCollection = new OrderErrorCollection();
            invalidOrdersCollection = new OrderErrorCollection();
            InitializeComponent();
        }
    }
}

