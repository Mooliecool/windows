//------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Expressions;
using System.Activities.Statements;

namespace Microsoft.Samples.Activities.Statements
{
    public class Program
    {
        public static void Main(string[] args)
        {
            // Normal iteration
            Console.WriteLine("Emulate: for(i=0; i<10; i++)");
            WorkflowInvoker.Invoke(Loop());
            Console.WriteLine("Run completed");
            Console.WriteLine();

            Console.WriteLine("Press <return> to exit...");
            Console.Read();
        }

        static For Loop()
        {
            Variable<int> loopVariable = new Variable<int>();

            return new For()
            {
                Variables = { loopVariable },

                InitAction = new Assign<int>()
                {
                    To = loopVariable,
                    Value = 0,
                },

                IterationAction = new Assign<int>()
                {
                    To = loopVariable,
                    Value = new InArgument<int>(ctx => loopVariable.Get(ctx) + 1)
                },
                
                // ExpressionServices.Convert is called to convert LINQ expression to a
                // serializable Expression Activity.
                Condition = ExpressionServices.Convert<bool>(ctx => loopVariable.Get(ctx) < 10),
                Body = new WriteLine
                {
                    Text = new InArgument<string>(ctx => "Value of item is: " + loopVariable.Get(ctx).ToString()),
                },
            };
        }
    }
}
