//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Expressions;
using System.Activities.Statements;
using Microsoft.VisualBasic.Activities;

namespace Microsoft.Samples.Compensation.ConfirmationSample
{
    class Program
    {
        static void Main()
        {
            //Executing a sequence of two CompensableActivities. After the second CA completes the first is
            //explicitly confirmed using the Confirm activity and the CompensationToken result of the first CA
            Console.WriteLine("\nExplicit confirmation:");            
            new WorkflowInvoker(ConfirmACompensableActivity()).Invoke();

            //Executing a sequence of two CompensableActivities. After the second CA completes the first is
            //explicitly compensated using the Compensate activity and the CompensationToken result of the first CA
            Console.WriteLine("\nExplicit compensation:");
            new WorkflowInvoker(CompensateACompensableActivity()).Invoke();

            //Executing a CompensableActivity whose body is a sequence of two CompensableActivities. When the 
            //workflow completes it confirms the outer CA which has a confirmation handler defined. The confirmation
            //handler then compensates the first CA and confirms the second. Note that by default the second CA would
            //have been confirmed then the first.
            Console.WriteLine("\nCustom confirmation handler:");
            new WorkflowInvoker(SpecifyCustomOrderInCompensationHandler()).Invoke();

            //Executing a sequence of two CompensableActivities. In the body of the first CA the two ints are summed and stored
            //in a variable which was added to the sequence's variables collection. The second CA then adds another int to the sum.
            //When the workflow completes default confirmation is invoked confirming CA2 then CA1. The first CA has a custom
            //confirmation handler which then subtracts an int from the sum. Notice that the value of the sum when the CA1 confirmation
            //handler runs is the value after both activities have modified the sum.
            Console.WriteLine("\nVariable access in a confirmation handler:");
            new WorkflowInvoker(VariableAccessVignette()).Invoke();

            Console.WriteLine("\nPress ENTER to exit");
            Console.ReadLine();
        }

        // Builds a sequence of two CompensableActivites where the first is explicitly confirmed 
        static Activity ConfirmACompensableActivity()
        {
            Variable<CompensationToken> token = new Variable<CompensationToken>();
            
            return new Sequence
            {
                Variables = { token },
                Activities = 
                {
                    new WriteLine { Text = "Start of workflow" },

                    new CompensableActivity()
                    {
                        Body = new WriteLine() { Text = "CompensableActivity1: Body" },
                        CompensationHandler = new WriteLine() { Text = "CompensableActivity1: Compensation Handler" },
                        ConfirmationHandler = new WriteLine() { Text = "CompensableActivity1: Confirmation Handler" },
                        //The compensation token is used to specify a completed CompensableActivity to be compensated or confirmed
                        Result = token,
                    },

                    new CompensableActivity()
                    {
                        Body = new WriteLine() { Text = "CompensableActivity2: Body" },
                        CompensationHandler = new WriteLine() { Text = "CompensableActivity2: Compensation Handler" },
                        ConfirmationHandler = new WriteLine() { Text = "CompensableActivity2: Confirmation Handler" },
                    },

                    //This will explicitly confirm the first CompensableActivity, the second will be confirmed when the workflow completes
                    new Confirm()
                    {
                        Target = token,
                    },

                    new WriteLine { Text = "End of workflow" }
                }
            };
        }

        // Builds a sequence of two CompensableActivites where the first is explicitly compensated 
        static Activity CompensateACompensableActivity()
        {
            Variable<CompensationToken> token = new Variable<CompensationToken>();
            
            return new Sequence
            {
                Variables = { token },
                Activities = 
                {
                    new WriteLine { Text = "Start of workflow" },

                    new CompensableActivity()
                    {
                        Body = new WriteLine() { Text = "CompensableActivity1: Body" },
                        CompensationHandler = new WriteLine() { Text = "CompensableActivity1: Compensation Handler" },
                        ConfirmationHandler = new WriteLine() { Text = "CompensableActivity1: Confirmation Handler" },
                        Result = token,
                    },

                    new CompensableActivity()
                    {
                        Body = new WriteLine() { Text = "CompensableActivity2: Body" },
                        CompensationHandler = new WriteLine() { Text = "CompensableActivity2: Compensation Handler" },
                        ConfirmationHandler = new WriteLine() { Text = "CompensableActivity2: Confirmation Handler" },
                    },

                    new Compensate()
                    {
                        Target = token,
                    },

                    new WriteLine { Text = "End of workflow" }
                }
            };
        }

        // Builds a sequence of three CompensableActivities with a custom confirmation order
        static Activity SpecifyCustomOrderInCompensationHandler()
        {
            Variable<CompensationToken> tokenOne = new Variable<CompensationToken>();
            Variable<CompensationToken> tokenTwo = new Variable<CompensationToken>();
            
            return new CompensableActivity()
            {
                Variables = { tokenOne, tokenTwo },
                Body = new Sequence()
                {
                    Activities =
                    {
                        new WriteLine { Text = "Start of workflow" },

                        new CompensableActivity()
                        {
                            Body = new WriteLine() { Text = "CompensableActivity1: Body" },
                            CompensationHandler = new WriteLine() { Text = "CompensableActivity1: Compensation Handler" },
                            ConfirmationHandler = new WriteLine() { Text = "CompensableActivity1: Confirmation Handler" },
                            Result = tokenOne,
                        },

                        new CompensableActivity()
                        {
                            Body = new WriteLine() { Text = "CompensableActivity2: Body" },
                            CompensationHandler = new WriteLine() { Text = "CompensableActivity2: Compensation Handler" },
                            ConfirmationHandler = new WriteLine() { Text = "CompensableActivity2: Confirmation Handler" },
                            Result = tokenTwo,
                        },

                        new CompensableActivity()
                        {
                            Body = new WriteLine() { Text = "CompensableActivity3: Body" },
                            CompensationHandler = new WriteLine() { Text = "CompensableActivity3: Compensation Handler" },
                            ConfirmationHandler = new WriteLine() { Text = "CompensableActivity3: Confirmation Handler" },
                        },
                        
                        new WriteLine { Text = "End of workflow" },
                    },
                },
                //The default confirmation order would be reverse of successful execution. Notice in this case the order is set explicitly
                ConfirmationHandler = new Sequence()
                {
                    Activities =
                    {
                        new Confirm() { Target = tokenOne }, //Confirm CA1 first
                        new Confirm() { Target = tokenTwo }, //Confirm CA2 second
                        //Even though CA3 is not specified it will still be confirmed. All CompensableActivities which are not compensated or confirmed will be confirmed in the default order
                    },
                },
            };
        }

        // Builds a sequence of two CompensableActivities which access a variable in the confirmation handler
        static Activity VariableAccessVignette()
        {
            Variable<int> mySum = new Variable<int> { Name = "mySum" };
            
            return new Sequence
            {
                Variables = { mySum },
                Activities = 
                {
                    new WriteLine { Text = "Start of workflow" },

                    new CompensableActivity()
                    {
                        Body = new Sequence
                        {
                            Activities = 
                            {
                                new WriteLine() { Text = "CompensableActivity1: Body" },

                                new Add<int, int, int> { Left = 5, Right = 10, Result = mySum},

                                new WriteLine() { Text = new InArgument<string>(new VisualBasicValue<string> { ExpressionText = "\"CompensableActivity1: The sum is: \" + mySum.toString()" }) },
                            },
                        },
                        CompensationHandler = new WriteLine() { Text = "CompensableActivity1: Compensation Handler" },
                        ConfirmationHandler = new Sequence
                        {
                            Activities = 
                            {
                                new WriteLine() { Text = "CompensableActivity1: Confirmation Handler" },
                                                                
                                new WriteLine() { Text = new InArgument<string>(new VisualBasicValue<string> { ExpressionText = "\"CompensableActivity1: The sum is: \" + mySum.toString()" }) },

                                new Subtract<int, int, int> { Left = mySum, Right = 12, Result = mySum},

                                new WriteLine() { Text = new InArgument<string>(new VisualBasicValue<string> { ExpressionText = "\"CompensableActivity1: After subtracting 12 the sum is now: \" + mySum.toString()" }) },
                            },
                        },
                    },

                    new CompensableActivity()
                    {
                        Body = new Sequence
                        {
                            Activities = 
                            {
                                new WriteLine() { Text = "CompensableActivity2: Body" },
                                                                
                                new WriteLine() { Text = "CompensableActivity2: Adding 7 to the sum" },

                                new Add<int, int, int> { Left = mySum, Right = 7, Result = mySum},

                                new WriteLine() { Text = new InArgument<string>(new VisualBasicValue<string> { ExpressionText = "\"CompensableActivity2: The sum is now: \" + mySum.toString()" }) },
                            },
                        },
                        CompensationHandler = new WriteLine() { Text = "CompensableActivity2: Compensation Handler" },
                        ConfirmationHandler = new WriteLine() { Text = "CompensableActivity2: Confirmation Handler" },
                    },

                    new WriteLine { Text = "End of workflow" }
                }
            };
        }
    }
}
