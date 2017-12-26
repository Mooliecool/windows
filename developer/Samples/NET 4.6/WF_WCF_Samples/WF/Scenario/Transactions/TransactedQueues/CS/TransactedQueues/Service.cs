//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.Activities.Statements;
using System.ServiceModel;
using System.ServiceModel.Activities;

namespace Microsoft.Samples.TransactedQueue
{
    static class Service
    {
        const int InitialPointBalance = 5000;

        public static Activity Create()
        {
            // Every account has a name and a point balance
            Variable<string> accountId = new Variable<string>();
            Variable<int> pointBalance= new Variable<int>();
            Variable<int> amountAdded = new Variable<int>();
            Variable<int> amountUsed = new Variable<int>();

            Sequence serviceRoot = new Sequence
            {
                // Each RewardsPointsService instance stores an accountId and a total point balance
                Variables = { accountId, pointBalance },

                Activities = 
                {
                    new WriteLine { Text = "Service instance started" },

                    // Dequeue message under a transaction and process the CreateAccount request
                    new TransactedReceiveScope
                    {
                        Request = new Receive
                        {
                            ServiceContractName = Shared.Contract,
                            OperationName = "CreateAccount",
                            
                            // The first message creates the workflow instance and initializes a correlation key based on the accountId
                            CanCreateInstance = true,
                            CorrelatesOn = CreateMessageQuerySet("sm:body()/tempuri:CreateAccount/tempuri:account"),
                            Content = new ReceiveParametersContent
                            {
                                Parameters = 
                                {
                                    { "account", new OutArgument<string>(accountId) }
                                }
                            }
                        },

                        // The body of the TRS will execute under the same transaction that the message was dequeued under
                        Body = new Sequence
                        {
                            Activities =
                            {
                                // All new accounts get some free points!
                                new Assign<int>
                                {
                                    To = new OutArgument<int>(pointBalance),
                                    Value = new InArgument<int>(InitialPointBalance)
                                },

                                new WriteLine 
                                { 
                                    Text = new InArgument<string>((env) => string.Format("Created account for {0} and gave them {1} points", accountId.Get(env), InitialPointBalance.ToString()))
                                }
                            }
                        }
                    },

                    // After the account creation, we listen indefinitely for AddPoints requests and UsePoints requests
                    new Parallel
                    {
                        Branches =
                        {
                            new While
                            {
                                // loop indefinitely
                                Condition = true,

                                Body = new TransactedReceiveScope
                                {
                                    Variables = { amountAdded },

                                    Request = new Receive
                                    {
                                        ServiceContractName = Shared.Contract,
                                        OperationName = "AddPoints",
                                        
                                        CorrelatesOn = CreateMessageQuerySet("sm:body()/tempuri:AddPoints/tempuri:account"),
                                        Content = new ReceiveParametersContent
                                        {
                                            Parameters = 
                                            {
                                                { "account", new OutArgument<string>(accountId) },
                                                { "amount", new OutArgument<int>(amountAdded) }
                                            }
                                        },
                                    },

                                    Body = new Sequence
                                    {
                                        Activities =
                                        {
                                            new WriteLine 
                                            { 
                                                Text = new InArgument<string>((env) => string.Format("Adding {0} points to account {1}", amountAdded.Get(env), accountId.Get(env))) 
                                            },
                                            
                                            // Add the amount specified to the point balance for this account
                                            new Assign<int>
                                            {
                                                To = new OutArgument<int>(pointBalance),
                                                Value = new InArgument<int>((env) => pointBalance.Get(env) + amountAdded.Get(env))
                                            }
                                        }
                                    }
                                }
                            },

                            new While
                            {
                                // loop indefinitely
                                Condition = true,

                                Body = new TransactedReceiveScope
                                {
                                    Variables = { amountUsed },

                                    Request = new Receive
                                    {
                                        ServiceContractName = Shared.Contract,
                                        OperationName = "UsePoints",
                                        
                                        CorrelatesOn = CreateMessageQuerySet("sm:body()/tempuri:UsePoints/tempuri:account"),
                                        Content = new ReceiveParametersContent
                                        {
                                            Parameters = 
                                            {
                                                { "account", new OutArgument<string>(accountId) },
                                                { "amount", new OutArgument<int>(amountUsed) }
                                            }
                                        }
                                    },

                                    Body = new Sequence
                                    {
                                        Activities =
                                        {
                                            new WriteLine 
                                            { 
                                                Text = new InArgument<string>((env) => string.Format("Deducting {0} points from account {1}", amountUsed.Get(env), accountId.Get(env))) 
                                            },
                                            
                                            // Decrement the amount specified from the point balance for this account
                                            // (for the purposes of the sample, we allow them to go negative)
                                            new Assign<int>
                                            {
                                                To = new OutArgument<int>(pointBalance),
                                                Value = new InArgument<int>((env) => pointBalance.Get(env) - amountUsed.Get(env))
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            };

            return serviceRoot;
        }

        // Wrapper code that encapsulates creating a simple message query for the purposes of the sample
        static MessageQuerySet CreateMessageQuerySet(string query)
        {
            XPathMessageQuery xpathQuery = new XPathMessageQuery(query);

            MessageQuerySet mqs = new MessageQuerySet();
            mqs.Add("accountQuery", xpathQuery);

            return mqs;
        }
    }
}
