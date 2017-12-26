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
	public sealed partial class ShipperWorkflow: SequentialWorkflowActivity
	{
		public ShipperWorkflow()
		{
			InitializeComponent();
		}

        public PurchaseOrder order = new PurchaseOrder();
        public IDictionary<string, string> supplierContext = default(IDictionary<string, string>);
        public string supplierAck = default(string);

        private void AcceptQuoteRequest(object sender, EventArgs e)
        {
            Console.ForegroundColor = ConsoleColor.White;
            Console.WriteLine("Received ShippingQuote Request");
            this.supplierAck = "Working on quote...";
            Console.ResetColor();
            this.SendShippingQuote.Context = this.supplierContext;
        }

        public ShippingQuote quote = new ShippingQuote();

        private void PrepareQuote(object sender, SendActivityEventArgs e)
        {
            Random rand = new Random();
            quote.ShippingCost = rand.Next(45, 90);
            quote.EstimatedShippingDate = DateTime.Now.AddDays(rand.Next(2,5));
            Console.ForegroundColor = ConsoleColor.White;
            Console.WriteLine("Sending ShippingQuote ");
            Console.WriteLine("Cost: $" + quote.ShippingCost);
            Console.WriteLine("ShipDate: " + quote.EstimatedShippingDate);
            Console.ForegroundColor = ConsoleColor.Gray;
            foreach (KeyValuePair<string, string> item in supplierContext)
                Console.WriteLine(item.Key + ": " + item.Value.Substring(0,13));
            Console.ResetColor();
        }

	}

}
