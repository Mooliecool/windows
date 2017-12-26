//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.ServiceModel.Activities;
using System.Threading;
using System.Transactions;
using TransactionScope = System.Activities.Statements.TransactionScope;

namespace Microsoft.Samples.TRSComposabilitySample
{

    class Program
    {
        // Define workflow
        static Activity ClientWorkflow()
        {
            string correlationKey = Guid.NewGuid().ToString();

            Send requestOne = new Send
            {
                Endpoint = Constants.ServerEndpoint,
                ServiceContractName = Constants.ServiceContractName,
                OperationName = "ScenarioOne",
                Content = new SendMessageContent { Message = new InArgument<string>(correlationKey) },
            };

            Send requestTwo = new Send
            {
                Endpoint = Constants.ServerEndpoint,
                ServiceContractName = Constants.ServiceContractName,
                OperationName = "ScenarioTwo",
                Content = new SendMessageContent { Message = new InArgument<string>(correlationKey) },
            };

            return new CorrelationScope
            {
                Body = new Sequence
                {
                    Activities =
                    {
                        new WriteLine { Text = "Begin Client Workflow" },

                        new TransactionScope
                        {
                            // Initiate Transaction for Scenario One
                            Body = new Sequence
                            {
                                Activities =
                                {
                                    new WriteLine { Text = "Scenario One Started" },

                                    // Transaction has not flowed yet, so it remains local
                                    new PrintTxInfo(),

                                    requestOne,

                                    new ReceiveReply
                                    {
                                        Request = requestOne,
                                    },

                                    // Transaction has flowed to service and has promoted to be a distributed transaction
                                    new PrintTxInfo(),

                                    new WriteLine { Text = "Scenario One Complete\n" },
                                }
                            }
                        },

                        new TryCatch
                        {
                            //Set up trycatch as we anticipate Scenario Two to fail
                            Try = new TransactionScope
                            {
                                // Initiate Transaction for Scenario Two
                                // When set to false a TransactionAbortedException will propogate instead of aborting the workflow.
                                AbortInstanceOnTransactionFailure = false,
                                Body = new Sequence
                                {
                                    Activities = 
                                    {
                                        new WriteLine { Text = "Scenario Two Started" },

                                        // Transaction has not flowed yet, so it remains local
                                        new PrintTxInfo(),

                                        requestTwo,

                                        new ReceiveReply
                                        {
                                            Request = requestTwo,
                                        },

                                        // Transaction has flowed to service and has promoted to be a distributed transaction
                                        new PrintTxInfo(),
                                    }
                                }
                            },
                            Catches = 
                            {
                                new Catch<TransactionAbortedException> 
                                {
                                    Action = new ActivityAction<TransactionAbortedException>
                                    {
                                        Handler = new WriteLine { Text = "The transaction was aborted on the server and a TransactionAbortedException was sent." },
                                    },
                                },
                            },
                            Finally = new WriteLine { Text = "Scenario Two Complete\n" },
                        },

                        new WriteLine { Text = "End Client Workflow" },
                    }
                }
            };
        }

        static void Main(string[] args)
        {
            Console.WriteLine("Press [ENTER] to start the client once the service is running.");
            Console.ReadLine();

            Console.WriteLine("Starting the client");
            Console.WriteLine();

            //Start the client
            AutoResetEvent syncEvent = new AutoResetEvent(false);
            WorkflowApplication app = new WorkflowApplication(ClientWorkflow());
            app.Completed = delegate(WorkflowApplicationCompletedEventArgs e)
            {
                 Console.WriteLine("Completed");
                 syncEvent.Set();
            };
            app.Aborted = delegate(WorkflowApplicationAbortedEventArgs e)
            {
                 Console.WriteLine("Aborted");
                 syncEvent.Set(); 
            };
            app.OnUnhandledException = delegate(WorkflowApplicationUnhandledExceptionEventArgs e)
            {
                 Console.WriteLine("UnhandledException: " + e.UnhandledException);
                 return UnhandledExceptionAction.Abort;
            };
            app.Run();
            syncEvent.WaitOne();

            Console.WriteLine("Press [ENTER] to exit");
            Console.ReadLine();
        }
    }
}
