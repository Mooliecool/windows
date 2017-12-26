//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.ServiceModel.Activities;
using Microsoft.Samples.Transaction.TransactedConvoy.Common;

namespace Microsoft.Samples.Transaction.TransactedConvoy.Client
{
    public class ClientWorkflow :  Activity
    {
        // Endpoing configuration is defined in app.config and referenced by this name
        private const string endpointConfigurationName = "codeServiceEndpoint";

        public ClientWorkflow()
        {
            this.Implementation = this.InternalImplementation;
        }

        private Activity InternalImplementation()
        {
            string correlationKey = Guid.NewGuid().ToString();

            Send sendBootstrap = new Send
            {
                OperationName = "Bootstrap",
                EndpointConfigurationName = endpointConfigurationName,
                Content = SendMessageContent.Create(new InArgument<string>(correlationKey)),
                ServiceContractName = Constants.ServiceContractName,
            };

            // In this sample there is no client side transaction, so transaction flow is not occuring.
            // Instead the service workflow is creating and scoping the transaction.
            return new CorrelationScope()
            {
                Body = new Sequence
                {
                    Activities = 
                    {
                        new WriteLine { Text = "Client: Workflow begins." },

                        new WriteLine { Text = "Client: Bootstrap service." },

                        // The service requires an initial Request represented by this "bootstrap" message.
                        sendBootstrap,

                        new ReceiveReply
                        {
                            Request = sendBootstrap
                        },

                        new WriteLine { Text = "Client: Beginning parallel sends." },

                        // The service workflow is now in a state where it's accepting multiple requests in parallel.
                        // All of these requests will execute under the same server side transaction.
                        new Parallel()
                        {
                            CompletionCondition = false,
                            Branches = 
                            {
                                new Send
                                {
                                    OperationName = "WorkBranch1",
                                    ServiceContractName = Constants.ServiceContractName,
                                    EndpointConfigurationName = endpointConfigurationName,
                                    Content = SendMessageContent.Create(new InArgument<string>(correlationKey))
                                },
                                new Send
                                {
                                    OperationName = "WorkBranch2",
                                    ServiceContractName = Constants.ServiceContractName,
                                    EndpointConfigurationName = endpointConfigurationName,
                                    Content = SendMessageContent.Create(new InArgument<string>(correlationKey)),
                                },
                                new Send
                                {
                                    OperationName = "WorkBranch3",
                                    ServiceContractName = Constants.ServiceContractName,
                                    EndpointConfigurationName = endpointConfigurationName,
                                    Content = SendMessageContent.Create(new InArgument<string>(correlationKey)),
                                },
                            },
                        },

                        new WriteLine { Text = "Client: All sends complete." },

                        new WriteLine { Text = "Client: Workflow ends." }
                    }
                }
            };
        }
    }
}
