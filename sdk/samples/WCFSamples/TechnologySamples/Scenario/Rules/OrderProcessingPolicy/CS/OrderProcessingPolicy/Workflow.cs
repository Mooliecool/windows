//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Drawing;
using System.Text;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Design;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.Runtime;

#pragma warning disable 169     // Ignore fields not used because they are used by Rules.

namespace Microsoft.Rules.Samples
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
            InitializeComponent();
        }
    }
}
