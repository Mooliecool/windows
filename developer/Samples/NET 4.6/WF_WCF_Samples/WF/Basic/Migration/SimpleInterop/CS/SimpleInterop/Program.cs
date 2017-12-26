//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;

namespace Microsoft.Samples.WF.Migration
{

    class Program
    {
        static void Main()
        {
            Variable<string> v = new Variable<string>();
            Sequence s = new Sequence()
            {
                Variables = { v },
                Activities =
                {
                    new Assign<string>()
                    {
                        To = v,
                        Value = "hello, world"
                    },
                    new Interop()
                    {
                        ActivityType = typeof(WriteLine),
                        ActivityProperties =
                        {
                            // Bind the Text property of the WriteLine to the Variable v
                            { "Text", new InArgument<string>(v) }
                        },
                        ActivityMetaProperties =
                        {
                            // Provide a value for the Name meta-property of the WriteLine
                            { "Name", "WriteLine" }
                        }
                    }
                }
            };
            
            WorkflowInvoker.Invoke(s);
            Console.WriteLine("Press [enter] to exit");
            Console.ReadLine();
        }
    }
}
