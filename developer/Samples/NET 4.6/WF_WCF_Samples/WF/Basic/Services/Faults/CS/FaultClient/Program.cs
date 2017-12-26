//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.ServiceModel;
using System.ServiceModel.Activities;
using Microsoft.Samples.Faults.SharedTypes;
using Microsoft.VisualBasic.Activities;

namespace Microsoft.Samples.Faults.FaultClient
{
    class Program
    {
        static void Main(string[] args)
        {
            WorkflowInvoker.Invoke(GetClientWorkflow());

            Console.WriteLine("Press [ENTER] to exit");
            Console.ReadLine();

        }

        static Activity GetClientWorkflow()
        {
            Variable<PurchaseOrder> po = new Variable<PurchaseOrder>();
            Variable<bool> invalidorder = new Variable<bool>() { Default = true };
            Variable<string> replytext = new Variable<string>();
            DelegateInArgument<FaultException<POFault>> poFault = new DelegateInArgument<FaultException<POFault>>();
            DelegateInArgument<FaultException<ExceptionDetail>> unexpectedFault = new DelegateInArgument<FaultException<ExceptionDetail>>();

            // submits a purchase order for the part and quantity stored in the po variable
            Send submitPO = new Send
            {
                Endpoint = new Endpoint
                {
                    Binding = Constants.Binding,
                    AddressUri = new Uri(Constants.ServiceAddress)
                },
                ServiceContractName = Constants.POContractName,
                OperationName = Constants.SubmitPOName,
                KnownTypes = { typeof(POFault) },
                Content = SendContent.Create(new InArgument<PurchaseOrder>(po))              
            };

            return new CorrelationScope
            {
                Body = new Sequence
                {
                    Variables = { invalidorder, po },                    
                    Activities = 
                    {
                        // defines the original desired parts and quantity: 155 pencils
                        new Assign<PurchaseOrder>
                        {
                            To = po,
                            Value = new InArgument<PurchaseOrder>( (e) => new PurchaseOrder() { PartName = "Pencil", Quantity = 155 } )
                        },
                        new While 
                        {
                            // loop until a valid order is submitted
                            Condition = invalidorder,
                            Variables = { replytext },                            
                            Body = new Sequence
                            {
                                Activities = 
                                {
                                    // print out the order that will be submitted
                                    new WriteLine { Text = new InArgument<string>((env) => string.Format("Submitting Order: {0} {1}s", po.Get(env).Quantity, po.Get(env).PartName)) },
                                    // submit the order
                                    submitPO,                                    
                                    new TryCatch
                                    {
                                        Try = new Sequence
                                        {
                                            Activities = 
                                            {
                                                // receive the result of the order
                                                // if ReceiveReply gets a Fault message, then we will handle those faults below
                                                new ReceiveReply
                                                {
                                                    Request = submitPO,
                                                    Content = ReceiveContent.Create(new OutArgument<string>(replytext))
                                                },
                                                new WriteLine { Text = replytext },
                                                // this order must be valid, so set invalidorder to false
                                                new Assign<bool> 
                                                {
                                                    To = invalidorder,
                                                    Value = false
                                                }
                                            }
                                        },
                                        Catches =                         
                                        {
                                            // catch a known Fault type: POFault
                                            new Catch<FaultException<POFault>>
                                            {
                                                Action = new ActivityAction<FaultException<POFault>>
                                                {
                                                    Argument = poFault,
                                                    Handler = new Sequence
                                                    {
                                                        Activities = 
                                                        {
                                                            // print out the details of the POFault
                                                            new WriteLine { Text = new InArgument<string>((env) => string.Format("\tReceived POFault: {0}", poFault.Get(env).Reason.ToString())) },
                                                            new WriteLine { Text = new InArgument<string>((env) => string.Format("\tPOFault Problem: {0}", poFault.Get(env).Detail.Problem)) },
                                                            new WriteLine { Text = new InArgument<string>((env) => string.Format("\tPOFault Solution: {0}", poFault.Get(env).Detail.Solution)) },
                                                            // update the order to buy Widgets instead
                                                            new Assign<string> 
                                                            {
                                                                To = new OutArgument<string>( (e) => po.Get(e).PartName ),
                                                                Value = "Widget"
                                                            }
                                                        }
                                                    }
                                                }
                                            },
                                            // catch any unknown fault types
                                            new Catch<FaultException<ExceptionDetail>>
                                            {
                                                Action = new ActivityAction<FaultException<ExceptionDetail>>
                                                {
                                                    Argument = unexpectedFault,
                                                    Handler = new Sequence
                                                    {
                                                        Activities = 
                                                        {
                                                            // print out the details of the fault
                                                            new WriteLine
                                                            {
                                                                Text = new InArgument<string>((e) => string.Format("\tReceived Fault: {0}",
                                                                    unexpectedFault.Get(e).Message
                                                                    ))
                                                            },
                                                            // update the order to buy 10 less of the item
                                                            new Assign<int> 
                                                            {
                                                                To = new OutArgument<int>( (e) => po.Get(e).Quantity ),
                                                                Value = new InArgument<int>( (e) => po.Get(e).Quantity - 10 )
                                                            }
                                                        }
                                                    }
                                                }
                                            },

                                        }
                                    }
                                }
                            }

                        },
                        new WriteLine { Text = "Order successfully processed." }
                    }
                }                
            };
        }
    }
}
