//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.ServiceModel.Activities;
using System.Xml.Linq;

namespace Microsoft.Samples.SuppressTransactionSample
{

    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Starting the service.");

            using (WorkflowServiceHost wsh = new WorkflowServiceHost(new ServiceWorkflow(), new Uri("net.tcp://localhost:8001/SuppressSample")))
            {
                wsh.Open();
                Console.WriteLine("Service is now running. Press [ENTER] to close.");
                Console.ReadLine();
            }
        }
    }

    public class ServiceWorkflow : Activity
    {
        public ServiceWorkflow()
        {
            this.Implementation = InternalImplementation;
        }

        private Activity InternalImplementation()
        {
            Receive receive = new Receive
            {
                OperationName = "DoRequestReply",
                CanCreateInstance = true,
                ServiceContractName = XName.Get("ISuppressSample", "http://tempuri.org/"),
            };

            return new Sequence
            {
                Activities = 
                {
                    new TransactedReceiveScope
                    {
                        Request = receive,
                        Body = new Sequence
                        {
                            Activities =
                            {
                                //If the distributed ID does not match that which is printed out on the client, then it was
                                // created locally on the server and no transaction was flowed by the client. 
                                new PrintTxID(),
                                new SendReply
                                {
                                    Request = receive,
                                },
                            },
                        },
                    },
                },
            };
        }
    }
}
