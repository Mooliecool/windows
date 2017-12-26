
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Configuration;
using System.Messaging;
using System.ServiceModel;
using System.Transactions;

namespace Microsoft.ServiceModel.Samples
{
    //The service contract is defined in generatedClient.cs, generated from the service by the svcutil tool.

    //Client implementation code.
    class Client
    {
        static void Main()
        {
            Random randomGen = new Random();
            for (int i = 0; i < 2500; i++)
            {
                // Create a client with given client endpoint configuration
                OrderProcessorClient client = new OrderProcessorClient("OrderProcessorEndpoint");

                // Create the purchase order
                PurchaseOrder po = new PurchaseOrder();
                po.CustomerId = "somecustomer" + i + ".com";
                po.PONumber = Guid.NewGuid().ToString();

                PurchaseOrderLineItem lineItem1 = new PurchaseOrderLineItem();
                lineItem1.ProductId = "Blue Widget";
                lineItem1.Quantity = randomGen.Next(1, 100);
                lineItem1.UnitCost = (float)randomGen.NextDouble() * 10;

                PurchaseOrderLineItem lineItem2 = new PurchaseOrderLineItem();
                lineItem2.ProductId = "Red Widget";
                lineItem2.Quantity = 890;
                lineItem2.UnitCost = 45.89F;

                po.orderLineItems = new PurchaseOrderLineItem[2];
                po.orderLineItems[0] = lineItem1;
                po.orderLineItems[1] = lineItem2;

                //Create a transaction scope.
                using (TransactionScope scope = new TransactionScope(TransactionScopeOption.Required))
                {
                    // Make a queued call to submit the purchase order
                    client.SubmitPurchaseOrder(po);
                    // Complete the transaction.
                    scope.Complete();
                }

                client.Close();
            }
            Console.WriteLine();
            Console.WriteLine("Press <ENTER> to terminate client.");
            Console.ReadLine();
        }
    }
}
