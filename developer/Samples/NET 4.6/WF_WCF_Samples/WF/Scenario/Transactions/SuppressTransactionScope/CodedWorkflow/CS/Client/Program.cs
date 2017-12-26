//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.ServiceModel.Activities;
using System.Threading;
using System.Xml.Linq;

namespace Microsoft.Samples.SuppressTransactionSample
{

    class Program
    {
        static void Main(string[] args)
        {
            AutoResetEvent serverResetEvent = new AutoResetEvent(false);

            Console.WriteLine("Press [ENTER] once the server is running.");
            Console.ReadLine();

            //Start the client
            Console.WriteLine("Starting the client.");
            new WorkflowInvoker(BuildWF()).Invoke();

            Console.WriteLine();
            Console.WriteLine("Sample complete. Press [ENTER] to close the client.");
            Console.ReadLine();
        }

        static Activity BuildRequestReply()
        {
            Send send = new Send
            {
                OperationName = "DoRequestReply",
                EndpointConfigurationName = "clientEndpoint",
                ServiceContractName = XName.Get("ISuppressSample", "http://tempuri.org/"),
            };

            return new CorrelationScope
            {
                Body = new Sequence
                {
                    Activities = 
                    {
                        new WriteLine { Text = "Beginning RequestReply sequence" },

                        send,
                        new PrintTxID(),
                        new ReceiveReply
                        {
                            Request = send,
                        },

                        new WriteLine { Text = "End RequestReply sequence" },
                    },
                },
            };
        }

        static Activity BuildWF()
        {
            return new TransactionScope
            {
                Body = new Sequence
                {
                    Activities =
                    {
                        new WriteLine { Text = "Beginning TSA sequence" },

                        // Promote the transaction so that it's distributed ID can be used for comparison on client and service
                        new PromoteTransaction(),

                        new PrintTxID(),

                        // Transaction will flow to service
                        BuildRequestReply(),

                        new SuppressTransactionScope
                        {
                            Body = new Sequence
                            {
                                Activities = 
                                {
                                    new WriteLine { },
                                    new WriteLine { Text = "Beginning SuppressTransactionScope" },

                                    // No Transaction will flow to service
                                    BuildRequestReply(),

                                    new WriteLine { Text = "End of SuppressTransactionScope" },
                                    new WriteLine { },
                                },
                            },
                        },

                        // Transaction will flow to service
                        BuildRequestReply(),

                        new WriteLine { Text = "End TSA sequence" },
                    }
                },
            };
        }
    }
}
