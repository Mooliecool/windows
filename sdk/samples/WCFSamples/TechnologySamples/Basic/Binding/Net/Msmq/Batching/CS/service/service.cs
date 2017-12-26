
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;

using System.ServiceModel.Channels;
using System.Configuration;
using System.Messaging;
using System.ServiceModel;
using System.ServiceModel.Description;
using System.Transactions;
using System.Runtime.Serialization;
using System.Collections.Generic;
using System.Data;
using System.Data.SqlClient;
using System.Threading;

namespace Microsoft.ServiceModel.Samples
{
    // Define the Purchase Order Line Item
    [DataContract(Namespace="http://Microsoft.ServiceModel.Samples")]
    public class PurchaseOrderLineItem
    {
        [DataMember]
        public string ProductId;

        [DataMember]
        public float UnitCost;

        [DataMember]
        public int Quantity;

        public override string ToString()
        {
            String displayString = "Order LineItem: " + Quantity + " of "  + ProductId + " @unit price: $" + UnitCost + "\n";
            return displayString;
        }

        public float TotalCost
        {
            get { return UnitCost * Quantity; }
        }
    }

    // Define Purchase Order
    [DataContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public class PurchaseOrder
    {
        static string[] OrderStates = { "Pending", "Processed", "Shipped" };
        static Random statusIndexer = new Random(137);

        [DataMember]
        public string PONumber;

        [DataMember]
        public string CustomerId;

        [DataMember]
        public PurchaseOrderLineItem[] orderLineItems;

        public float TotalCost
        {
            get
            {
                float totalCost = 0;
                foreach (PurchaseOrderLineItem lineItem in orderLineItems)
                    totalCost += lineItem.TotalCost;
                return totalCost;
            }
        }

        public string Status
        {
            get 
            {
                return OrderStates[statusIndexer.Next(3)];
            }
        }

        public override string ToString()
        {
            System.Text.StringBuilder strbuf = new System.Text.StringBuilder("Purchase Order: " + PONumber + "\n");
            strbuf.Append("\tCustomer: " + CustomerId + "\n");
            strbuf.Append("\tOrderDetails\n");

            foreach (PurchaseOrderLineItem lineItem in orderLineItems)
            {
                strbuf.Append("\t\t" + lineItem.ToString());
            }

            strbuf.Append("\tTotal cost of this order: $" + TotalCost + "\n");
            strbuf.Append("\tOrder status: " + Status + "\n");
            return strbuf.ToString();
        }
    }

    // Order Processing Logic
    public class Orders
    {
        public static void Add(PurchaseOrder po)
        {
            // insert purchase order
            SqlCommand insertPurchaseOrderCommand = new SqlCommand("insert into PurchaseOrders(poNumber, customerId) values(@poNumber, @customerId)");
            insertPurchaseOrderCommand.Parameters.Add("@poNumber", SqlDbType.VarChar, 50);
            insertPurchaseOrderCommand.Parameters.Add("@customerId", SqlDbType.VarChar, 50);

            // insert product line item
            SqlCommand insertProductLineItemCommand = new SqlCommand("insert into ProductLineItems(productId, unitCost, quantity, poNumber) values(@productId, @unitCost, @quantity, @poNumber)");
            insertProductLineItemCommand.Parameters.Add("@productId", SqlDbType.VarChar, 50);
            insertProductLineItemCommand.Parameters.Add("@unitCost", SqlDbType.Float);
            insertProductLineItemCommand.Parameters.Add("@quantity", SqlDbType.Int);
            insertProductLineItemCommand.Parameters.Add("@poNumber", SqlDbType.VarChar, 50);
 
            int rowsAffected = 0;

            using (TransactionScope scope = new TransactionScope(TransactionScopeOption.Required))
            {
                using (SqlConnection conn = new SqlConnection(ConfigurationManager.AppSettings["connectionString"]))
                {
                    conn.Open();

                    // insert into purchase order table
                    insertPurchaseOrderCommand.Connection = conn;
                    insertPurchaseOrderCommand.Parameters["@poNumber"].Value = po.PONumber;
                    insertPurchaseOrderCommand.Parameters["@customerId"].Value = po.CustomerId;
                    insertPurchaseOrderCommand.ExecuteNonQuery();

                    // insert into product line item table
                    insertProductLineItemCommand.Connection = conn;
                    foreach (PurchaseOrderLineItem orderLineItem in po.orderLineItems)
                    {
                        insertProductLineItemCommand.Parameters["@poNumber"].Value = po.PONumber;
                        insertProductLineItemCommand.Parameters["@productId"].Value = orderLineItem.ProductId;
                        insertProductLineItemCommand.Parameters["@unitCost"].Value = orderLineItem.UnitCost;
                        insertProductLineItemCommand.Parameters["@quantity"].Value = orderLineItem.Quantity;
                        rowsAffected += insertProductLineItemCommand.ExecuteNonQuery();
                    }
                    scope.Complete();
                }
            }
            Console.WriteLine("Updated database with {0} product line items for purchase order {1} ", rowsAffected, po.PONumber);
        }
    }

    // Define a service contract. 
    [ServiceContract(Namespace="http://Microsoft.ServiceModel.Samples")]
    public interface IOrderProcessor
    {
        [OperationContract(IsOneWay = true)]
        void SubmitPurchaseOrder(PurchaseOrder po);
    }

    // Service class which implements the service contract.
    // Added code to write output to the console window
    [ServiceBehavior(ReleaseServiceInstanceOnTransactionComplete=false, TransactionIsolationLevel=System.Transactions.IsolationLevel.Serializable, ConcurrencyMode=ConcurrencyMode.Multiple)]
    public class OrderProcessorService : IOrderProcessor
    {
        [OperationBehavior(TransactionScopeRequired = true, TransactionAutoComplete = true)]
        public void SubmitPurchaseOrder(PurchaseOrder po)
        {
            Console.WriteLine("Processing {0} ", po);        
            Orders.Add(po);
        }


        // Host the service within this EXE console application.
        public static void Main()
        {
            // Get MSMQ queue name from app settings in configuration
            string queueName = ConfigurationManager.AppSettings["queueName"];

            // Create the transacted MSMQ queue if necessary.
            if (!MessageQueue.Exists(queueName))
                MessageQueue.Create(queueName, true);


            // Get the base address that is used to listen for WS-MetaDataExchange requests
            // This is useful to generate a proxy for the client
            string baseAddress = ConfigurationManager.AppSettings["baseAddress"];

            // Create a ServiceHost for the OrderProcessorService type.
            ServiceHost serviceHost = new ServiceHost(typeof(OrderProcessorService));

          /*  ServiceThrottlingBehavior throttle = new ServiceThrottlingBehavior();
            throttle.MaxConcurrentCalls = 5;
            serviceHost.Description.Behaviors.Add(throttle);*/

            // Hook on to the service host faulted events
            serviceHost.Faulted += new EventHandler(OnServiceFaulted);
            
            // Open the ServiceHostBase to create listeners and start listening for messages.
            serviceHost.Open();

            // The service can now be accessed.
            Console.WriteLine("The service is ready.");
            Console.WriteLine("Press <ENTER> to terminate service.");
            Console.WriteLine();
            Console.ReadLine();

            try
            {
                serviceHost.Close();
            }
            catch (CommunicationObjectFaultedException)
            {
                Console.WriteLine(" Service cannot be closed...it already faulted");
            }   
        }

        public static void OnServiceFaulted(object sender, EventArgs e)
        {
            Console.WriteLine("Service Faulted ");
        }

    }
}
