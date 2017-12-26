//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;

namespace Microsoft.Samples.DynamicArguments
{

    class Program
    {
        static void Main(string[] args)
        {
            WorkflowInvoker.Invoke(CreateProgram());
            Console.WriteLine("Press [Enter] to exit");
            Console.ReadLine();
        }

        static Activity CreateProgram()
        {
            Variable<int> result = new Variable<int>("result");

            return new Sequence
            {
                Variables = { result },
                Activities = 
                {
                    new WriteLine { Text = new InArgument<string>("Invoking Math.Max with arguments 5 and 7")},
                    new MethodInvoke
                    {
                        TargetType = typeof(Math),
                        MethodName = "Max",
                        Parameters = { new InArgument<int>(5), new InArgument<int>(7)},
                        Result = new OutArgument<int>(result)
                    },
                    new WriteLine { Text = new InArgument<string>(context=> result.Get(context).ToString() + " is the bigger number")}
                }
            };
        }
    }
}
