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
using System.Threading;
using System.Xml;

namespace Microsoft.WorkflowServices.Samples
{
	public sealed partial class SupplierWorkflow: SequentialWorkflowActivity
	{
		public SupplierWorkflow()
		{
			InitializeComponent();
		}

        public PurchaseOrder order = new PurchaseOrder();
        public string supplierAck = default(string);
        public IDictionary<string, string> customerContext = default(IDictionary<string, string>);

        private void AcceptOrder(object sender, EventArgs e)
        {
            Console.WriteLine("Order Received...");
            this.supplierAck = "Order Received on " + DateTime.Now;
            this.SendOrderDetails.Context = this.customerContext;
        }

        public IDictionary<string, string> contextShipper2 = default(IDictionary<string, string>);
        public ShippingQuote quoteShipper2 = new ShippingQuote();

        private void PrepareShipper2Request(object sender, SendActivityEventArgs e)
        {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("RequestShippingQuote from Shipper2");
            Console.ResetColor();
            this.contextShipper2 = this.ReceiveQuoteFromShipper2.Context;
        }

        public IDictionary<string, string> contextShipper1 = default(IDictionary<string, string>);
        public ShippingQuote quoteShipper1 = new ShippingQuote();

        private void PrepareShipper1Request(object sender, SendActivityEventArgs e)
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("RequestShippingQuote from Shipper1");
            Console.ResetColor();
            this.contextShipper1 = this.ReceiveQuoteFromShipper1.Context;
        }

        public IDictionary<string, string> contextShipper3 = default(IDictionary<string, string>);
        public ShippingQuote quoteShipper3 = new ShippingQuote();
        
        private void PrepareShipper3Request(object sender, SendActivityEventArgs e)
        {
            Console.ForegroundColor = ConsoleColor.Blue;
            Console.WriteLine("RequestShippingQuote from Shipper3");
            Console.ResetColor();
            this.contextShipper3 = this.ReceiveQuoteFromShipper3.Context;
        }

        public PurchaseOrder confirmedOrder = new PurchaseOrder();
        public ShippingQuote confirmedQuote = new ShippingQuote();

        private void PrepareOrderConfirmation(object sender, SendActivityEventArgs e)
        {
            Console.WriteLine("Send OrderConfirmation to Customer");
            confirmedQuote = quoteShipper2;
            if (confirmedQuote.ShippingCost > quoteShipper1.ShippingCost)
                confirmedQuote = quoteShipper1;
            if (confirmedQuote.ShippingCost > quoteShipper3.ShippingCost)
                confirmedQuote = quoteShipper3;
        }

        public string ackShipper2 = default(string);
        public string ackShipper1 = default(string);
        public string ackShipper3 = default(string);

        private void ReceiveShipper2ShippingQuote(object sender, EventArgs e)
        {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Received Shipper2 ShippingQuote");
            Console.WriteLine("Cost: $" + quoteShipper2.ShippingCost);
            Console.WriteLine("ShipDate: " + quoteShipper2.EstimatedShippingDate);
            Console.ResetColor();
        }

        private void ReceiveShipper1ShippingQuote(object sender, EventArgs e)
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Received Shipper1 ShippingQuote");
            Console.WriteLine("Cost: $" + quoteShipper1.ShippingCost);
            Console.WriteLine("ShipDate: " + quoteShipper1.EstimatedShippingDate);
            Console.ResetColor();
        }

        private void ReceiveShipper3ShippingQuote(object sender, EventArgs e)
        {
            Console.ForegroundColor = ConsoleColor.Blue;
            Console.WriteLine("Received Shipper3 ShippingQuote");
            Console.WriteLine("Cost: $" + quoteShipper3.ShippingCost);
            Console.WriteLine("ShipDate: $" + quoteShipper3.EstimatedShippingDate);
            Console.ResetColor();
        }





	}

}
