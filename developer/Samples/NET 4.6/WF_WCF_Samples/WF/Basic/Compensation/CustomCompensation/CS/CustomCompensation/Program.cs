//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using Microsoft.VisualBasic.Activities;

namespace Microsoft.Samples.Compensation.CustomCompensation
{
    class Program
    {
        // A truck rental agency processing an order
        static Activity TruckRentalAgency()
        {
            // declaring a set of handles that can be used to refer to a particular unit of compensable work
            Variable<CompensationToken> handleCompanyA = new Variable<CompensationToken>
            {
                Name = "Handle to company A",
            };

            Variable<CompensationToken> handleCompanyC = new Variable<CompensationToken>
            {
                Name = "Handle to company C",
            };

            Variable<CompensationToken> handleServiceFee = new Variable<CompensationToken>
            {
                Name = "Handle for service fee",
            };

            Variable<CompensationToken> scopeHandle = new Variable<CompensationToken>
            {
                Name = "ScopeHandle",
            };

            Variable<int> truckRentalRate = new Variable<int>("truckRentalRate", 500);
            
            return new Parallel
            {
                // ends when either the customer cancels the order or the reservation date is now
                CompletionCondition = true,
                
                Branches =  
                {
                    // Branch waiting on customer input
                    new Sequence
                    {
                        Activities = 
                        {  
                            new Delay
                            {
                                Duration = TimeSpan.FromSeconds(3)
                            },
                            // Simulate customer deciding to cancel the order
                            new WriteLine { Text = "Received cancelation from customer" }
                        }
                    },

                    // Branch processing the order
                    // Using CompensableActivity to delimit a compensation zone that confirms if Body completes with success
                    new TryCatch
                    {
                        Variables = { scopeHandle },
                        Try = new CompensableActivity
                        {
                            Result = scopeHandle,
                            Body = new Sequence
                            {
                                Activities = 
                                {
                                    new WriteLine { Text = "Simulate a truck rental order" },

                                    // wrap everything in a compensable activity that will be used to define custom compensation;
                                    // instead of using the default compensation which compensates all completed compensable activities
                                    // in reverse order of execution
                                    new CompensableActivity
                                    {
                                        Variables = { handleServiceFee },

                                        Body = new Sequence
                                        {
                                            Variables =
                                            {
                                                handleCompanyA,
                                                handleCompanyC
                                            },

                                            Activities = 
                                            {
                                                new WriteLine { Text = "Customer requested truck quotes for 4/14/2009" },
                                                                    
                                                new CompensableActivity
                                                {
                                                    Body = new WriteLine { Text = "Request truck reservation from company A" },
                                                    CompensationHandler = new WriteLine { Text = "Cancel truck reservation from company A" },
                                                    ConfirmationHandler= new WriteLine { Text = "Confirm truck reservation from company A" },
                                                    Result = handleCompanyA
                                                },

                                                new CompensableActivity
                                                {
                                                    Body = new WriteLine { Text = "Request truck reservation from company B" },
                                                    CompensationHandler = new WriteLine { Text = "Cancel truck reservation from company B" },
                                                    ConfirmationHandler= new WriteLine { Text = "Confirm truck reservation from company B" }
                                                },

                                                new CompensableActivity
                                                {
                                                    Body = new WriteLine { Text = "Request truck reservation from company C" },
                                                    CompensationHandler = new WriteLine { Text = "Cancel truck reservation from company C" },
                                                    ConfirmationHandler= new WriteLine { Text = "Confirm truck reservation from company C" },
                                                    Result = handleCompanyC
                                                },

                                                new WriteLine { Text = "Customer picked the truck from company B" },

                                                // cancel the reservations for the trucks from the other companies
                                                new Compensate
                                                {
                                                    Target = handleCompanyC
                                                },
                                                new Compensate
                                                {
                                                    Target = handleCompanyA
                                                },

                                                // Charge the customer on the credit card
                                                // Service Fee charge
                                                new CompensableActivity
                                                {
                                                    Body = new WriteLine { Text = "Charge agency service fee on customer credit card" },
                                                    CompensationHandler = new WriteLine { Text = "Refund agency service fee on customer credit card" },
                                                    ConfirmationHandler= new WriteLine { Text = "Confirm service fee charge notification" },
                                                    Result = handleServiceFee
                                                },

                                                // Truck Rental charge
                                                // Notice how using Variables, one can save information that will be available 
                                                // during Compensation or Confirmation
                                                new CompensableActivity
                                                {
                                                    Variables =
                                                    {
                                                        truckRentalRate
                                                    },
                                                    Body = new Sequence
                                                    {
                                                        Activities = 
                                                        {
                                                            new Assign<int>
                                                            {
                                                                To = new OutArgument<int>(truckRentalRate),
                                                                Value = new InArgument<int>(600)
                                                            },
                                                            new WriteLine 
                                                            { 
                                                                Text = new InArgument<string>(env => 
                                                                    ("Charge truck rental cost on customer credit card: $" + truckRentalRate.Get(env).ToString())) 
                                                            },
                                                        }
                                                    },
                                                    CompensationHandler = 
                                                        new WriteLine 
                                                        { 
                                                            Text = new InArgument<string>(env =>
                                                                ("Refund truck rental cost on customer credit card: $" +  truckRentalRate.Get(env).ToString()))
                                                        },
                                                    ConfirmationHandler= new WriteLine { Text = "Confirm truck rental charge notification" }
                                                }
                                            }
                                        },
                                        CompensationHandler = new Sequence // used to define the custom compensation logic
                                        {
                                            Activities = 
                                            {
                                                new WriteLine { Text = "Customer canceled the order" },
                                                new WriteLine { Text = "If customer is not a premium customer and there are less than 10 days" +
                                                                       " to reservation date, then the service fee is not refundable" },
                                                new WriteLine { Text = "This customer is not a premium customer" },
                                                new WriteLine { Text = "There are less than 10 days until reservation date" },
                                                new If // simulate a non-premium customer with less than 10 days to reservation date
                                                {
                                                    Condition = true,
                                                    Then = new Confirm
                                                    {
                                                        Target = handleServiceFee
                                                    }
                                                },
                                                
                                                // invoke default compensation (compensate in reverse order of completion) for the rest 
                                                // of compensable activities
                                                new Compensate()
                                            }
                                        }
                                    },
                                    new WriteLine { Text = "Truck rental order completed" },

                                    new WriteLine { Text = "Wait until reservation date or until cancelation, whichever comes first\n" },
                                    new Delay 
                                    {
                                        Duration = TimeSpan.FromSeconds(5)
                                    }
                                }
                            }
                        },
                        Finally = new If()
                        {
                            //Checks to see if the CompensableActivity completed and has not already been compensated
                            Condition = new VisualBasicValue<bool> { ExpressionText = "ScopeHandle Is Nothing" },
                            Else = new Confirm() { Target = scopeHandle },
                        },
                    }
                }
            };
        }

        static void Main()
        {
            WorkflowInvoker.Invoke(TruckRentalAgency());

            Console.WriteLine("\nPress ENTER to exit");
            Console.ReadLine();
        }
    }
}
