//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.ServiceModel;
using System.ServiceModel.Activities;

namespace Microsoft.Samples.TransactedQueue
{
    static class Client
    {
        public static Activity Create()
        {
            string userId = Guid.NewGuid().ToString();

            Sequence clientRoot = new Sequence
            {
                Activities = 
                {
                    new WriteLine { Text = "Creating account for " + userId },

                    // Enqueue the CreateAccount message under a transaction
                    new TransactionScope
                    {
                        Body = new Send
                        {
                            ServiceContractName = Shared.Contract,
                            OperationName = "CreateAccount",
                            Endpoint = new Endpoint
                            {
                                AddressUri = Shared.Address,
                                Binding = Shared.Binding,
                            },

                            Content = new SendParametersContent
                            {
                                Parameters = 
                                {
                                    { "account", new InArgument<string>(userId) }
                                }
                            }
                        }
                    },
                    new WriteLine { Text = "Account " + userId + " earned 1575 points" },

                    // Enqueue the AddPoints message under a transaction
                    new TransactionScope
                    {
                        Body = new Send
                        {
                            ServiceContractName = Shared.Contract,
                            OperationName = "AddPoints",
                            Endpoint = new Endpoint
                            {
                                AddressUri = Shared.Address,
                                Binding = Shared.Binding
                            },

                            Content = new SendParametersContent
                            {
                                Parameters = 
                                {
                                    { "account", new InArgument<string>(userId) },
                                    { "amount", new InArgument<int>(1575) }
                                }
                            }
                        }
                    },

                    new WriteLine { Text = "Account " + userId + " used 4025 points" },

                    // Enqueue the UsePoints message under a transaction
                    new TransactionScope
                    {
                        Body = new Send
                        {
                            ServiceContractName = Shared.Contract,
                            OperationName = "UsePoints",
                            Endpoint = new Endpoint
                            {
                                AddressUri = Shared.Address,
                                Binding = Shared.Binding
                            },

                            Content = new SendParametersContent
                            {
                                Parameters = 
                                {
                                    { "account", new InArgument<string>(userId) },
                                    { "amount", new InArgument<int>(4025) }
                                }
                            }
                        }
                    }
                }
            };

            return clientRoot;
        }
    }
}
