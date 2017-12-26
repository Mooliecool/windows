
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;

using System.ServiceModel.Channels;
using System.Configuration;
using System.ServiceModel;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Description;
using System.Transactions;
using System.Runtime.Serialization;
using System.Collections.Generic;
using System.Text;
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
            String displayString = "Order LineItem: " + Quantity + " of " + ProductId + " @unit price: $" + UnitCost + "\n";
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
            StringBuilder strbuf = new StringBuilder("Purchase Order: " + PONumber + "\n");
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
    // Can replace with transaction-aware resource such as SQL, transacted hashtable to hold orders
    // This example uses a non-transactional resource
    public class Orders
    {
        static Dictionary<string, PurchaseOrder> purchaseOrders = new Dictionary<string, PurchaseOrder>();

        public static void Add(PurchaseOrder po)
        {
            purchaseOrders.Add(po.PONumber, po);
        }

        public static string GetOrderStatus(string poNumber)
        {
            PurchaseOrder po;
            if (purchaseOrders.TryGetValue(poNumber, out po))
                return po.Status;
            else
                return null;
        }

        public static void DeleteOrder(string poNumber)
        {
            if (purchaseOrders[poNumber] != null)
                purchaseOrders.Remove(poNumber);
        }
    }
    
    public class PoisonErrorHandler : IErrorHandler
    {
        static WaitCallback orderProcessingCallback = new WaitCallback(OrderProcessorService.StartThreadProc);

        public void ProvideFault(Exception error, MessageVersion version, ref Message fault)
        {
            // no-op -we are not interested in this.
        }
        
        // Handle poison message exception by moving the poison message out of the way for regular processing to continue.
        public bool HandleError(Exception error)
        {
            MsmqPoisonMessageException poisonException = error as MsmqPoisonMessageException;
            if (null != poisonException)
            {
                long lookupId = poisonException.MessageLookupId;
                Console.WriteLine(" Poisoned message -message look up id = {0}", lookupId);

                // Get MSMQ queue name from app settings in configuration.

                System.Messaging.MessageQueue orderQueue = new System.Messaging.MessageQueue(OrderProcessorService.QueueName);
                System.Messaging.MessageQueue poisonMessageQueue = new System.Messaging.MessageQueue(OrderProcessorService.PoisonQueueName);
                System.Messaging.Message message = null;
                
                // Use a new transaction scope to remove the message from the main app queue and add it to the poison queue
                // the poison message service processes messages from the poison queue.
                using(TransactionScope txScope= new TransactionScope(TransactionScopeOption.RequiresNew))
                {
                    int retryCount = 0;
                    while(retryCount < 3) 
                    {
                        retryCount++;
                    
                        try 
                        {
                            // Look up the poison message using the look up id.
                            message = orderQueue.ReceiveByLookupId(lookupId);
                            if(message != null) 
                            {
                                // send the message to the poison message queue.
                                poisonMessageQueue.Send(message, System.Messaging.MessageQueueTransactionType.Automatic);
                                
                                // complete transaction scope
                                txScope.Complete();

                                Console.WriteLine("Moved poisoned message with look up id: {0} to poison queue: {1} ", lookupId, OrderProcessorService.PoisonQueueName);
                                break;
                            }
                        
                        }
                        catch(InvalidOperationException ) 
                        {
                            //Code for the case when the message may still not be available in the queue because of a race condition in the transaction or 
                            //another node in the farm may actually have taken the message.
                            if (retryCount < 3) 
                            {
                                Console.WriteLine("Trying to move poison message but message is not available, sleeping for 10 seconds before retrying");
                                Thread.Sleep(TimeSpan.FromSeconds(10));
                            }
                            else 
                            {
                                Console.WriteLine("Giving up on trying to move the message");
                            }
                        }
                    
                    }
                }

                // restart service host
                Console.WriteLine();
                Console.WriteLine("Restarting the service to process rest of the messages in the queue");
                Console.WriteLine("Press <ENTER> to stop the service");
                ThreadPool.QueueUserWorkItem(orderProcessingCallback);
                return true;
            }

            return false;
        }
    } 

    public sealed class PoisonErrorBehaviorAttribute : Attribute, IServiceBehavior 
    {
        PoisonErrorHandler poisonErrorHandler;

        public PoisonErrorBehaviorAttribute()
        {
            this.poisonErrorHandler = new PoisonErrorHandler();
        }

        void IServiceBehavior.Validate(ServiceDescription description, ServiceHostBase serviceHostBase) 
        {
        }
        
        void IServiceBehavior.AddBindingParameters(ServiceDescription description, ServiceHostBase serviceHostBase, System.Collections.ObjectModel.Collection<ServiceEndpoint> endpoints, BindingParameterCollection parameters)
        {
        }

        void IServiceBehavior.ApplyDispatchBehavior(ServiceDescription description, ServiceHostBase serviceHostBase)
        {
            foreach(ChannelDispatcherBase channelDispatcherBase in serviceHostBase.ChannelDispatchers)
            {
                ChannelDispatcher channelDispatcher = channelDispatcherBase as ChannelDispatcher;
                channelDispatcher.ErrorHandlers.Add(poisonErrorHandler);
            }
        }
    }

    // Define a service contract. 
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public interface IOrderProcessor
    {
        [OperationContract(IsOneWay = true)]
        void SubmitPurchaseOrder(PurchaseOrder po);
    }


    // Service class that implements the service contract.
    // Added code to write output to the console window
    [PoisonErrorBehavior]
    public class OrderProcessorService : IOrderProcessor
    {
        static Random r = new Random(137);
        public static string QueueName;
        public static string PoisonQueueName;

        [OperationBehavior(TransactionScopeRequired = true, TransactionAutoComplete = true)]
        public void SubmitPurchaseOrder(PurchaseOrder po)
        {

            int randomNumber = r.Next(10);

            if (randomNumber % 2 == 0)
            {
                Orders.Add(po);
                Console.WriteLine("Processing {0} ", po);
            }
            else
            {
                Console.WriteLine("Aborting transaction, cannot process purchase order: " + po.PONumber);
                Console.WriteLine();
                throw new Exception("Cannot process purchase order: " + po.PONumber);
            }
        }

        public static void StartThreadProc(object stateInfo)
        {
            StartService();
        }

        public static void StartService()
        {
            // Get MSMQ queue name from app settings in configuration.
            QueueName = ConfigurationManager.AppSettings["queueName"];

            // Get MSMQ queue name for the final poison queue.
            PoisonQueueName = ConfigurationManager.AppSettings["poisonQueueName"];

            // Create the transacted MSMQ queue if necessary.
            if (!System.Messaging.MessageQueue.Exists(QueueName))
                System.Messaging.MessageQueue.Create(QueueName, true);

            // Create the transacted poison message MSMQ queue if necessary.
            if (!System.Messaging.MessageQueue.Exists(PoisonQueueName))
                System.Messaging.MessageQueue.Create(PoisonQueueName, true);

            // Create a ServiceHost for the OrderProcessorService type.
            ServiceHost serviceHost = new ServiceHost(typeof(OrderProcessorService));

            // Hook on to the service host faulted events.
            serviceHost.Faulted += new EventHandler(OnServiceFaulted);

            // Open the ServiceHostBase to create listeners and start listening for messages.
            serviceHost.Open();
            
        }


        public static void OnServiceFaulted(object sender, EventArgs e) 
        {
            Console.WriteLine("Service Faulted");
        }
        

        // Host the service within this EXE console application.
        public static void Main()
        {

            OrderProcessorService.StartService();

            // The service can now be accessed.
            Console.WriteLine("The service is ready.");
            Console.WriteLine("Press <ENTER> to stop the service");
            Console.ReadLine();
            Console.WriteLine("Exiting service");

        }
    }
}
