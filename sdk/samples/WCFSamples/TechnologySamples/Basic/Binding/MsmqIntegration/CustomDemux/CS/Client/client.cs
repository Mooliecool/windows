
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Messaging;
using System.Configuration;
using System.Transactions;
namespace Microsoft.ServiceModel.Samples
{
    class Program
    {
        static void Main(string[] args)
        {
            //Connect to the queue
            MessageQueue orderQueue = new MessageQueue("FormatName:Direct=OS:" + ConfigurationManager.AppSettings["orderQueueName"]);

            // Create the purchase order
            PurchaseOrder po = new PurchaseOrder();
            po.customerId = "somecustomer.com";
            po.poNumber = Guid.NewGuid().ToString();

            PurchaseOrderLineItem lineItem1 = new PurchaseOrderLineItem();
            lineItem1.productId = "Blue Widget";
            lineItem1.quantity = 54;
            lineItem1.unitCost = 29.99F;

            PurchaseOrderLineItem lineItem2 = new PurchaseOrderLineItem();
            lineItem2.productId = "Red Widget";
            lineItem2.quantity = 890;
            lineItem2.unitCost = 45.89F;

            po.orderLineItems = new PurchaseOrderLineItem[2];
            po.orderLineItems[0] = lineItem1;
            po.orderLineItems[1] = lineItem2;

            // submit the purchase order
            Message msg = new Message();
            msg.Body = po;
            msg.Label = "SubmitPurchaseOrder";

            //Submit an Order.
            using (TransactionScope scope = new TransactionScope(TransactionScopeOption.Required))
            {
                
                orderQueue.Send(msg, MessageQueueTransactionType.Automatic);
                // Complete the transaction.
                scope.Complete();
                
            }
            Console.WriteLine("Placed the order:{0}", po);

            // submit the purchase order
            Message msg2 = new Message();
            msg2.Body = po.poNumber;
            msg2.Label = "CancelPurchaseOrder";

            //Cancel the Order.
            using (TransactionScope scope2 = new TransactionScope(TransactionScopeOption.Required))
            {
                
                orderQueue.Send(msg2, MessageQueueTransactionType.Automatic);
                // Complete the transaction.
                scope2.Complete();
               
            }
            Console.WriteLine("Cancelled the Order: {0}", po.poNumber);
            Console.WriteLine("Press <ENTER> to terminate client.");
            Console.ReadLine();
        }
    }
}