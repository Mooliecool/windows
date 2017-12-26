//------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;

namespace Microsoft.Samples.Activities.Statements
{
    public class Program
    {
        public static void Main(string[] args)
        {
            try
            {
                WorkflowInvoker.Invoke(Enumerate(1, 5, 1));
                Console.WriteLine();

                // Out of range with positive step 
                WorkflowInvoker.Invoke(Enumerate(5, 1, 1));
                Console.WriteLine();

                // Raise exception with when entering an infinite loop 
                // Setting step = 0
                WorkflowInvoker.Invoke(Enumerate(1, 5, 0));
                Console.WriteLine();
            }
            catch (InvalidOperationException ex)
            {
                Console.WriteLine(ex.Message);
            }

            Console.WriteLine("Press <return> to exit...");
            Console.Read();
        }

        static RangeEnumeration Enumerate(int start, int stop, int step)
        {
            Console.WriteLine("Starting enumeration of a series of numbers from {0} to {1} using steps of {2}", start, stop, step);
            
            DelegateInArgument<int> loopVariable = new DelegateInArgument<int>();

            return new RangeEnumeration
            {
                Start = start,
                Stop = stop,
                Step = step,
                Body = new ActivityAction<int>
                {
                    Argument = loopVariable,
                    Handler = new WriteLine
                    {
                        Text = new InArgument<string>(context => "This is " + loopVariable.Get(context).ToString()),
                    },
                }
            };           
        }
    }
}
