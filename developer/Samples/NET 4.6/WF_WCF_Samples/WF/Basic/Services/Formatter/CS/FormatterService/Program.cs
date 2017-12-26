//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.Linq;
using System.ServiceModel;
using System.ServiceModel.Activities;
using Microsoft.Samples.WorkflowServicesSamples.Common;

namespace Microsoft.Samples.WorkflowServicesSamples.EchoWorkflowService
{
    class Program
    {
        static WorkflowService service;

        private static Activity GetApproveExpense(Variable<Expense> expense, Variable<bool> reply)
        {
            Receive approveExpense = new Receive
            {
                OperationName = "ApproveExpense",
                CanCreateInstance = true,
                ServiceContractName = "FinanceService",
                SerializerOption = SerializerOption.DataContractSerializer,
                Content = ReceiveContent.Create(new OutArgument<Expense>(expense))
            };
            approveExpense.KnownTypes.Add(typeof(Travel));
            approveExpense.KnownTypes.Add(typeof(Meal));

            Activity workflow = new CorrelationScope()
            {
                Body = new Sequence
                {
                    Variables = { expense, reply },                    
                    Activities = 
                    {
                        approveExpense,
                        new WriteLine
                        {
                            Text = new InArgument<string>(env =>("Expense approval request received"))
                        },
                        new If
                        {
                           Condition = new InArgument<bool> (env => (expense.Get(env).Amount <= 100)),
                           Then =                         
                               new Assign<bool>
                               {
                                   Value = true,
                                   To = new OutArgument<bool>(reply)
                               },
                           Else =                         
                               new Assign<bool>
                               {
                                   Value = false,
                                   To = new OutArgument<bool>(reply)
                               },                                                       
                        },

                        new If
                        {
                           Condition = new InArgument<bool> (reply),
                           Then =                         
                                new WriteLine
                                {
                                    Text = new InArgument<string>("Expense Approved")
                                },
                           Else =                         
                                new WriteLine
                                {
                                    Text = new InArgument<string>("Expense Cannot be Approved")
                                },
                        },
                        new SendReply
                        {                           
                            Request = approveExpense,
                            Content = SendContent.Create(new InArgument<bool>(reply)),                            
                        },
                    },
                }
            };
            return workflow;
        }
        private static Activity GetApprovePO(Variable<PurchaseOrder> po, Variable<bool> replyPO)
        {
            Receive approvePO = new Receive
            {
                OperationName = "ApprovePurchaseOrder",
                CanCreateInstance = true,
                ServiceContractName = "FinanceService",
                SerializerOption = SerializerOption.XmlSerializer,
                Content = ReceiveContent.Create(new OutArgument<PurchaseOrder>(po))
            };

            Activity workflow = new CorrelationScope()
            {
                Body = new Sequence
                {
                    Variables = { po, replyPO },                    
                    Activities = 
                    {
                        approvePO,
                        new WriteLine
                        {
                            Text = new InArgument<string>(env =>("Purchase order approval request received"))
                        },
                        new If
                        {
                           Condition = new InArgument<bool> (env => (po.Get(env).RequestedAmount <= 100)),
                           Then =                         
                               new Assign<bool>
                               {
                                   Value = true,
                                   To = new OutArgument<bool>(replyPO)
                               },
                           Else =                         
                               new Assign<bool>
                               {
                                   Value = false,
                                   To = new OutArgument<bool>(replyPO)
                               },                                                       
                        },

                        new If
                        {
                           Condition = new InArgument<bool> (replyPO),
                           Then =                         
                                new WriteLine
                                {
                                    Text = new InArgument<string>("Purchase Order Approved")
                                },
                           Else =                         
                                new WriteLine
                                {
                                    Text = new InArgument<string>("Purchase Order Cannot be Approved")
                                },
                        },
                        new SendReply
                        {                           
                            Request = approvePO,
                            Content = SendContent.Create(new InArgument<bool>(replyPO)),                            
                        },
                    }
                }
            };
            return workflow;
        }
        private static Activity GetApprovedVendor(Variable<VendorRequest> vendor, Variable<VendorResponse> replyVendor)
        {
            Receive approvedVendor = new Receive
            {
                OperationName = "ApprovedVendor",
                CanCreateInstance = true,
                ServiceContractName = "FinanceService",
                Content = ReceiveContent.Create(new OutArgument<VendorRequest>(vendor))
            };

            Activity workflow = new CorrelationScope()
            {
                Body = new Sequence
                {
                    Variables = { vendor, replyVendor },                    
                    Activities =
                    {
                        approvedVendor,
                        new WriteLine
                        {
                            Text = new InArgument<string>(env =>("Query for approved vendor received"))
                        },
                        new If
                        {
                           Condition = new InArgument<bool> (env => ((vendor.Get(env).requestingDepartment == "Finance"))||Constants.vendors.Contains(vendor.Get(env).Name)),
                           Then =                         
                               new Assign<VendorResponse>
                               {
                                   Value = new InArgument<VendorResponse>( (e) => new VendorResponse{isPreApproved = true} ),
                                   To = new OutArgument<VendorResponse>(replyVendor)
                               },
                           Else =                         
                               new Assign<VendorResponse>
                               {
                                   Value = new InArgument<VendorResponse>( (e) => new VendorResponse{isPreApproved = false} ),
                                   To = new OutArgument<VendorResponse>(replyVendor)
                               }, 
                        },

                        new If
                        {
                           Condition = new InArgument<bool> (env => replyVendor.Get(env).isPreApproved),
                           Then =                         
                                new WriteLine
                                {
                                    Text = new InArgument<string>("Vendor is pre-approved")
                                },
                           Else =                         
                                new WriteLine
                                {
                                    Text = new InArgument<string>("Vendor is not pre-approved")
                                },
                        },
                        new SendReply
                        {                           
                            Request = approvedVendor,
                            Content = SendContent.Create(new InArgument<VendorResponse>(replyVendor)),                     
                        },
                    }
                }
            };
            return workflow;
        }


        private static void CreateService()
        {
            Variable<Expense> expense = new Variable<Expense> { Name = "expense" };
            Variable<VendorRequest> vendor = new Variable<VendorRequest> { Name = "vendor" };
            Variable<PurchaseOrder> po = new Variable<PurchaseOrder> { Name = "po" };
            Variable<bool> reply = new Variable<bool> { Name = "reply" };
            Variable<bool> replyPO = new Variable<bool> { Name = "reply" };
            Variable<VendorResponse> replyVendor = new Variable<VendorResponse> { Name = "reply" };

            Parallel workflow = new Parallel
            {
                Branches =
                {
                    GetApproveExpense(expense, reply),
                    GetApprovePO(po, replyPO),
                    GetApprovedVendor(vendor, replyVendor),
                }
            };
            service = new WorkflowService
            {
                Name = "FinanceService",
                Body = workflow,

                Endpoints =
                    {
                        new Endpoint
                        {
                            ServiceContractName="FinanceService",
                            AddressUri = new Uri(Constants.EchoServiceAddress),
                            Binding = new BasicHttpBinding(),
                        }
                    }
            };
        }


        static void Main(string[] args)
        {

            Console.WriteLine("Starting up...");
            CreateService();

            Uri address = new Uri(Constants.ServiceBaseAddress);
            System.ServiceModel.Activities.WorkflowServiceHost host = new System.ServiceModel.Activities.WorkflowServiceHost(service, address);

            try
            {
                Console.WriteLine("Opening service...");
                host.Open();

                Console.WriteLine("Service is listening on {0}...", address);
                Console.WriteLine("To terminate press ENTER");
                Console.ReadLine();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Service treminated with exception {0}", ex.ToString());
            }
            finally
            {
                host.Close();
            }
        }


        private static void OnCompleteInstanceExecution(object sender, WorkflowApplicationCompletedEventArgs e)
        {
            if (e.TerminationException != null)
            {
                Console.WriteLine("Workflow completed with {0}: {1}.", e.TerminationException.GetType().FullName, e.TerminationException.Message);
            }
            else
            {
                Console.WriteLine("Workflow completed successfully.");
            }
        }
    }
}

