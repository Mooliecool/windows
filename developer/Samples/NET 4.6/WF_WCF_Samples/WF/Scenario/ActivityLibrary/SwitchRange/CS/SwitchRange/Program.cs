//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------
using System;
using System.Activities;
using System.Activities.Statements;
using System.Collections.Generic;

namespace Microsoft.Samples.Activities.Statements
{

    class Program
    {
        // Call SwitchRange activity with an input value to switch against
        static void Main(string[] args)
        {
            Console.WriteLine("Calling SwitchRangeActivity with 3 definitions:");
            Console.WriteLine();
       
            // Invoke a valid SwitchRange workflow
            Activity example1 = CreateValidSwitchRangeWorkflow();
            InvokeSwitchRangeWorkflow(example1, "b", "1. Correctly defined SwitchRange with all the ranges correctly defined");            
            ShowSeparator();

            // Invoke a SwitchRange with wrong Ranges
            Activity example2 = CreateInvalidSwitchRangeInvalidRanges();
            InvokeSwitchRangeWorkflow(example2, "b", "2. Incorrectly defined SwitchRange (ranges incorrectly defined)");
            ShowSeparator();            

            // Invoke a SwitchRange without providing an Expression
            InvokeSwitchRangeWorkflow(example1, null, "3. Correclty defined SwitchRange with arguments incorrectly defined (no Expression provided)");
            ShowSeparator();

            Console.WriteLine();
            Console.WriteLine("Press Enter to exit...");
            Console.ReadLine();
        }

        static void ShowSeparator()
        {
            Console.WriteLine("----------------------------");
        }

        // Invoke an instance of a SwitchRange Workflow
        static void InvokeSwitchRangeWorkflow(Activity activity, string expression, string description)
        {
            try
            {
                // Show description in the Console
                Console.WriteLine(description);

                // If a value for Expression is provided...
                if (expression != null)
                {
                    // Show Expression in the Console
                    Console.WriteLine("Expression Value: {0}", expression);

                    // Create dictionary with input arguments
                    IDictionary<string, object> input = new Dictionary<string, object>() { { "Expression", expression } };

                    // Invoke the workflow passing the arguments
                    WorkflowInvoker.Invoke(activity, input);
                }
                else // No value for Expression...
                {
                    // Show message in the Console
                    Console.WriteLine("Expression Value: {null}");

                    // Invoke the workflow without parameters
                    WorkflowInvoker.Invoke(activity);
                }
            }
            catch (Exception ve)
            {
                Console.WriteLine(string.Format("Validation Error: {0}", ve.Message));                
            }
        }
  
        // Create a correctly constructed SwitchRange activity
        static Activity CreateValidSwitchRangeWorkflow()
        {            
            return new SwitchRange<string>()
            {
                DisplayName = "TestSwitchRange",                
                Cases = 
                {
                    new CaseRange<string>                    
                    {
                        From = "a",
                        To = "c",
                        Action = new WriteLine
                        {
                            Text = "Case a-c selected",
                        }
                    },
                    new CaseRange<string>
                    {
                        From = "d",
                        To = "g",
                        Action = new WriteLine
                        {
                            Text = "Case d-g selected",
                        }
                    },
                    new CaseRange<string>
                    {
                        From = "h",
                        To = "z",
                        Action = new WriteLine
                        {
                            Text = "Case h-z selected",
                        }
                    }
                },
                Default = new WriteLine { Text = "Default Case selected" }
            };
        }

        // Create a SwitchRange activity with invalid Ranges
        static Activity CreateInvalidSwitchRangeInvalidRanges()
        {            
            return new SwitchRange<string>()
            {
                DisplayName = "TestSwitchRange",                
                Cases = 
                {
                    new CaseRange<string>                    
                    {
                        From = "a",
                        Action = new WriteLine
                        {
                            Text = "Case a-c selected",
                        }
                    },
                    new CaseRange<string>
                    {
                        From = "d",
                        To = "g",
                        Action = new WriteLine
                        {
                            Text = "Case d-g selected",
                        }
                    },
                    new CaseRange<string>
                    {
                        From = "h",
                        To = "z",
                        Action = new WriteLine
                        {
                            Text = "Case h-z selected",
                        }
                    }
                },
                Default = new WriteLine { Text = "Default Case selected" }
            };
        }
    }
}
