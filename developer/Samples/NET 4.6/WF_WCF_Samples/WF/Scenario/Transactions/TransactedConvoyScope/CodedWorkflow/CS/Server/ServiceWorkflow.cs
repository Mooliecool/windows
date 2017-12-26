//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.Activities.Statements;
using System.ServiceModel;
using System.ServiceModel.Activities;
using System.ServiceModel.Dispatcher;
using Microsoft.Samples.Transaction.TransactedConvoy.Common;

namespace Microsoft.Samples.Transaction.TransactedConvoy.Server
{
    public class ServiceWorkflow : Activity
    {
        public ServiceWorkflow()
        {
            this.Implementation = this.InternalImplementation;
        }

        private Activity InternalImplementation()
        {
            Variable<string> correlationKey = new Variable<string>();

            Receive recvBootstrap = new Receive
            {
                OperationName = "Bootstrap",
                ServiceContractName = Constants.ServiceContractName,
                CanCreateInstance = true,
                CorrelatesOn = BuildCorrelatesOn(),
                Content = ReceiveContent.Create(new OutArgument<string>(correlationKey))
            };

            return new Sequence
            {
                Variables = { correlationKey },
                Activities =
                {
                    new WriteLine { Text = "Server: Workflow begins." },

                    // TransactedReceiveScope requires a single Request to "bootstrap" the transactional scope.
                    // The body of a TransactedReceiveScope can have any number of activities including Receive activities
                    // that will all execute under the same transaction.
                    new TransactedReceiveScope
                    {
                        Request = recvBootstrap,

                        Body = new Sequence
                        {
                            Activities =
                            {
                                new WriteLine { Text = "Server: Transaction started." },

                                new SendReply
                                {
                                    Request = recvBootstrap
                                },

                                // Receives in parallel, often referred to as "parallel convoy"
                                new Parallel
                                {
                                    // all branches must complete
                                    CompletionCondition = false,

                                    // all 3 receives will execute under the context of the same transaction
                                    Branches =
                                    {
                                        new Receive
                                        {
                                            OperationName = "WorkBranch1",
                                            ServiceContractName = Constants.ServiceContractName,
                                            CanCreateInstance = false,
                                            CorrelatesOn = BuildCorrelatesOn(),
                                            Content = ReceiveContent.Create(new OutArgument<string>(correlationKey))
                                        },
                                        new Receive
                                        {
                                            OperationName = "WorkBranch2",
                                            ServiceContractName = Constants.ServiceContractName,
                                            CanCreateInstance = false,
                                            CorrelatesOn = BuildCorrelatesOn(),
                                            Content = ReceiveContent.Create(new OutArgument<string>(correlationKey))
                                        },
                                        new Receive
                                        {
                                            OperationName = "WorkBranch3",
                                            ServiceContractName = Constants.ServiceContractName,
                                            CanCreateInstance = false,
                                            CorrelatesOn = BuildCorrelatesOn(),
                                            Content = ReceiveContent.Create(new OutArgument<string>(correlationKey))
                                        }
                                    }
                                }
                            }
                        }
                    },
                    new WriteLine { Text = "Server: Transaction commits." },

                    new WriteLine { Text = "Server: Workflow ends" },
                }
            };
        }

        // A correlation is initialized in the first Receive activity based on an string identifier from the client.
        // All following receives follow up on this same correlation key.
        private static MessageQuerySet BuildCorrelatesOn()
        {
            XPathMessageContext context = new XPathMessageContext();

            return new MessageQuerySet()
            {
                { "CorrH", new XPathMessageQuery("sm:body()/ser:string", context) },
            };
        }
    }
}
