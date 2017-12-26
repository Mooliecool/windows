//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Description;
using System.ServiceModel;
using System.ServiceModel.MsmqIntegration;
using System.Collections.Generic;
using System.ServiceModel.Channels;
using System.Runtime.Serialization;
using System.Messaging;
using System.Threading;
using System.Configuration;
namespace Microsoft.ServiceModel.Samples
{

    // Define a service contract.
    [ServiceContract]
    [ServiceKnownType(typeof(PurchaseOrder))]
    [ServiceKnownType(typeof(String))]
    public interface IOrderProcessor
    {
        [OperationContract(IsOneWay = true, Name = "SubmitPurchaseOrder")]
        void SubmitPurchaseOrder(MsmqMessage<PurchaseOrder> msg);

        [OperationContract(IsOneWay = true, Name = "CancelPurchaseOrder")]
        void CancelPurchaseOrder(MsmqMessage<string> ponumber);

    }

    public class MatchAllFilterBehavior : IEndpointBehavior
    {

        #region IEndpointBehavior Members

        public void Validate(ServiceEndpoint serviceEndpoint)
        {
        }

        public void ApplyClientBehavior(ServiceEndpoint serviceEndpoint, ClientRuntime proxy)
        {
        }

        public void AddBindingParameters(ServiceEndpoint serviceEndpoint, BindingParameterCollection parameters)
        {
    	}

        public void ApplyDispatchBehavior(ServiceEndpoint serviceEndpoint, EndpointDispatcher endpointDispatcher)
        {
            
            endpointDispatcher.ContractFilter = new MatchAllMessageFilter();
        }

        #endregion
    }

    public class OperationSelector : IDispatchOperationSelector
    {

        public string SelectOperation(ref System.ServiceModel.Channels.Message message)
        {

            MsmqIntegrationMessageProperty property = MsmqIntegrationMessageProperty.Get(message);
            return property.Label;

        }

    }

    // Service class which implements the service contract.
    // Added code to write output to the console window
    public class OrderProcessorService : IOrderProcessor, IContractBehavior
    {
        [OperationBehavior(TransactionScopeRequired = true, TransactionAutoComplete = true)]
        public void SubmitPurchaseOrder(MsmqMessage<PurchaseOrder> msg)
        {
            PurchaseOrder po = (PurchaseOrder)msg.Body;
            Random statusIndexer = new Random();
            po.Status = (OrderStates)statusIndexer.Next(3);
            Console.WriteLine("Processing {0} ", po);

        }

        [OperationBehavior(TransactionScopeRequired = true, TransactionAutoComplete = true)]
        public void CancelPurchaseOrder(MsmqMessage<string> msg)
        {
            string ponumber = (string)msg.Body;
            Console.WriteLine("Purchase Order {0} is cancelled ", ponumber);

        }

        // Host the service within this EXE console application.
        [STAThread]
        public static void Main()
        {
            // Get MSMQ queue name from app settings in configuration
            string queueName = ConfigurationManager.AppSettings["orderQueueName"];

            // Create the transacted MSMQ queue if necessary.
            if (!MessageQueue.Exists(queueName))
                MessageQueue.Create(queueName, true);


            // Create a ServiceHost for the CalculatorService type.
            using (ServiceHost serviceHost = new ServiceHost(typeof(OrderProcessorService)))
            {               
                ServiceEndpoint endpoint = serviceHost.Description.Endpoints[0];
                endpoint.Behaviors.Add(new MatchAllFilterBehavior());
   
                //Open the ServiceHost to create listeners and start listening for messages.
                serviceHost.Open();

                // The service can now be accessed.
                Console.WriteLine("The service is ready.");
                Console.WriteLine("Press <ENTER> to terminate service.");
                Console.ReadLine();
            }
        }

        void IContractBehavior.Validate(ContractDescription description, ServiceEndpoint endpoint)
        {
        }

        void IContractBehavior.ApplyDispatchBehavior(ContractDescription description, ServiceEndpoint endpoint, DispatchRuntime dispatch)
        {
            dispatch.OperationSelector = new OperationSelector();
        }

        void IContractBehavior.AddBindingParameters(ContractDescription description, ServiceEndpoint endpoint, BindingParameterCollection parameters)
        {
        }

        void IContractBehavior.ApplyClientBehavior(ContractDescription description, ServiceEndpoint endpoint, ClientRuntime proxy)
        {
        }

                
    }
}
