//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections.Generic;
using System.Drawing;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;
using System.Xml;

namespace Microsoft.WorkflowServices.Samples
{
	public sealed partial class CustomerWorkflow : SequentialWorkflowActivity
	{
        public CustomerWorkflow()
		{
			InitializeComponent();
		}

        public PurchaseOrder order = new PurchaseOrder();
        public string supplierAck = default(string);
        public IDictionary<string, string> contextToSend = default(IDictionary<string, string>);

        private void PrepareOrder(object sender, SendActivityEventArgs e)
        {
            this.order.Amount = 1000;
            this.order.OrderId = 1234;
            this.contextToSend = this.ReceiveOrderDetails.Context;
        }

        public PurchaseOrder orderDetails = new PurchaseOrder();
        public ShippingQuote shippingQuote = new ShippingQuote();
        public string customerAck = default(string);

        private void ReviewOrder(object sender, EventArgs e)
        {
            this.customerAck = "Order Details Received";
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Order Accepted.");
            Console.WriteLine("Cost: $" + this.shippingQuote.ShippingCost);
            Console.WriteLine("ShipDate: " + this.shippingQuote.EstimatedShippingDate);
            Console.ResetColor();
        }

        private void DisplayStatus(object sender, EventArgs e)
        {
            Console.WriteLine(this.supplierAck);
        }
	}

}
