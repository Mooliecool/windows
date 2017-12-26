//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.Activities.Tracking;
using System.ServiceModel;
using System.ServiceModel.Activities;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;
using System.Threading;

namespace Microsoft.Samples.TRSComposabilitySample
{

    class Program
    {
        static void Main(string[] args)
        {
            //Build the service
            AutoResetEvent syncEvent = new AutoResetEvent(false);
            WorkflowServiceHost host = ServiceHostFactory(syncEvent);

            //Start the service
            Console.WriteLine("Starting service.");
            host.Open();
            Console.WriteLine("Service is running.");

            //Wait for incoming messages
            syncEvent.WaitOne();
            host.Close();

            Console.WriteLine("Press ENTER to exit\n");
            Console.ReadLine();
        }

        static MessageQuerySet BuildCorrelatesOn()
        {
            XPathMessageContext context = new XPathMessageContext();

            return new MessageQuerySet()
            {
                { "ch", new XPathMessageQuery("//ser:string", context) },
            };
        }

        //Define the Workflow
        static Activity ServerWorkflow()
        {
            Variable<string> correlationValue = new Variable<string>() { Name = "correlationID" };
            MessageQuerySet mqs = new MessageQuerySet()
            {
                { "ch", new XPathMessageQuery("//ser:string", new XPathMessageContext()) },
            };

            Receive requestOne = new Receive
            {
                OperationName = "ScenarioOne",
                ServiceContractName = Constants.ServiceContractName,
                CanCreateInstance = true,
                CorrelatesOn = mqs,
            };

            Receive requestTwo = new Receive
            {
                OperationName = "ScenarioTwo",
                ServiceContractName = Constants.ServiceContractName,
                CanCreateInstance = false,
                CorrelatesOn = mqs,
            };

            return new Sequence
            {
                Variables = { correlationValue },
                Activities =
                {
                    new WriteLine { Text = "Begin Server Workflow. Begin Scenario One." },

                    new TransactedReceiveScope
                    {
                        // Flow in transaction
                        Request = requestOne,
                        Body = new Sequence
                        {
                            Activities = 
                            {
                                new WriteLine { Text = "Begin Scenario One TransactedReceiveScope Body" },

                                new SendReply { Request = requestOne },

                                new PrintTxInfo(),

                                // Inner TransactionScope scope joins the existing transaction
                                new TransactionScope
                                {
                                    Body = new Sequence
                                    {
                                        Activities = 
                                        {
                                            new WriteLine { Text = "Begin Scenario One Inner TransactionScope Body" },

                                            new PrintTxInfo(),

                                            new WriteLine { Text = "End Scenario One Inner TransactionScope Body" },
                                        },
                                    },

                                },

                                new PrintTxInfo(),

                                new WriteLine { Text = "End Scenario One TransactedReceiveScope Body" },

                            }
                        }
                    },

                    new WriteLine { Text = "End Scenario One. Begin Scenario Two." },

                    new TransactedReceiveScope
                    {
                        // Flow in transaction
                        Request = requestTwo,
                        Body = new Sequence
                        {
                            Activities = 
                            {
                                new WriteLine { Text = "Begin Scenario Two TransactedReceiveScope Body" },

                                new SendReply { Request = requestTwo },

                                new PrintTxInfo(),

                                // Inner TransactionScope joins the existing transaction, but it sets a timeout for the work in its scope.
                                // If the timeout expires the entire transaction will be aborted.
                                new TransactionScope
                                {
                                    Timeout = TimeSpan.FromSeconds(2),
                                    Body = new Sequence
                                    {
                                        Activities = 
                                        {
                                            new WriteLine { Text = "Begin Scenario Two Inner TransactionScope Body" },

                                            new Delay { Duration = TimeSpan.FromSeconds(5) },

                                            new PrintTxInfo(),

                                            new WriteLine { Text = "End Scenario Two Inner TransactionScope Body" },
                                        },
                                    }
                                },

                                new PrintTxInfo(),

                                new WriteLine { Text = "End Scenario Two TransactedReceiveScope Body" },
                            },
                        },
                    },

                    new WriteLine { Text = "End Scenario Two. End Server Workflow\n" },
                },
            };
        }

        //Define WorkflowServiceHost
        static WorkflowServiceHost ServiceHostFactory(AutoResetEvent syncEvent)
        {
            WorkflowServiceHost host = new WorkflowServiceHost(ServerWorkflow());
            host.AddServiceEndpoint(Constants.ServiceContractName, Constants.ServerEndpoint.Binding, Constants.ServerAddress);

            host.Description.Behaviors.Find<ServiceDebugBehavior>().IncludeExceptionDetailInFaults = true;

            host.WorkflowExtensions.Add(new EventTrackingParticipant(syncEvent));

            return host;
        }
    }

    //Define custom tracking participant
    public class EventTrackingParticipant : TrackingParticipant
    {
        AutoResetEvent syncEvent;

        public EventTrackingParticipant(AutoResetEvent syncEvent)
        {
            this.syncEvent = syncEvent;
        }

        protected override void Track(TrackingRecord trackingRecord, TimeSpan timeout)
        {
            WorkflowInstanceRecord record = trackingRecord as WorkflowInstanceRecord;
            if (record != null)
            {
                if (record.State == WorkflowInstanceStates.Aborted || record.State == WorkflowInstanceStates.Completed || record.State == WorkflowInstanceStates.UnhandledException)
                {
                    Console.WriteLine("EventTrackingParticipant: Workflow is {0}", record.State);
                    syncEvent.Set();
                }
            }
        }
    }
}
