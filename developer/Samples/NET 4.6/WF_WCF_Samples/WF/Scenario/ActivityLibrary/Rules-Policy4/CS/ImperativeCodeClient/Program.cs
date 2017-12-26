//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Expressions;
using System.Activities.Statements;
using System.Configuration;
using System.Workflow.Activities.Rules;
using System.Workflow.ComponentModel.Serialization;
using System.Xml;
using Microsoft.Samples.Activities.Rules;

namespace Microsoft.Samples.Rules.Client
{
    //--------------------------------------
    // Apply discount to orders using an existing 3.5 RuleSet 
    //
    // Rules:
    // ======
    //     If Value > 500 and CustomerType is Residental => Discount = 5%
    //     If Value > 10000 and CustomerType is Business => Discount = 10%
    //--------------------------------------
    class Program
    {
        static void Main(string[] args)
        {
            // load the ruleset from the ruleset file
            string rulesFilePath = ConfigurationManager.AppSettings["rulesFilePath"];
            WorkflowMarkupSerializer serializer = new WorkflowMarkupSerializer();
            XmlTextReader reader = new XmlTextReader(rulesFilePath);
            RuleDefinitions rules = serializer.Deserialize(reader) as RuleDefinitions;
            RuleSet ruleSet = rules.RuleSets[0];

            // create orders 
            Variable<Order> order1 = new Variable<Order>() { Name = "Order1", Default = new LambdaValue<Order>(c => new Order(650, CustomerType.Residential)) };
            Variable<Order> order2 = new Variable<Order>() { Name = "Order2", Default = new LambdaValue<Order>(c => new Order(15000, CustomerType.Business)) };
            Variable<Order> order3 = new Variable<Order>() { Name = "Order3", Default = new LambdaValue<Order>(c => new Order(650, CustomerType.Business)) };

            // create and run workflow instance
            WorkflowInvoker.Invoke(
                new Sequence
                {
                    Variables = { order1, order2, order3 },
                    Activities =
                    {
                        //---------------------------------------
                        // Rule: Order > 500 and CustomerType is Residential
                        //---------------------------------------
                        new WriteLine { Text = new InArgument<string>("OrderValue > 500 and is Residential customer => discount = 5%") },
                        new WriteLine
                        {
                            Text = new InArgument<string>(c => string.Format("   Before Evaluation: {0}", order1.Get(c).ToString()))
                        },
                        new Policy4<Order>
                        {
                            RuleSet = ruleSet,
                            Input = new InArgument<Order>(order1),
                            Result = new OutArgument<Order>(order1)
                        },
                        new WriteLine
                        {
                            Text = new InArgument<string>(c => string.Format("   After Evaluation: {0}", order1.Get(c).ToString()))
                        },

                        //---------------------------------------
                        // Rule: Order > 10000 and CustomerType is Businesss
                        //---------------------------------------
                        new WriteLine(),
                        new WriteLine { Text = new InArgument<string>("OrderValue > 10000 and is Business customer => discount = 10%") },
                        new WriteLine
                        {
                            Text = new InArgument<string>(c => string.Format("   Before Evaluation: {0}", order2.Get(c).ToString()))
                        },
                        new Policy4<Order>
                        {
                            RuleSet = ruleSet,
                            Input = new InArgument<Order>(order2),
                            Result = new OutArgument<Order>(order2)
                        },
                        new WriteLine
                        {
                            Text = new InArgument<string>(c => string.Format("   After Evaluation: {0}", order2.Get(c).ToString()))
                        },

                        //---------------------------------------
                        // No Rules Applied
                        //---------------------------------------
                        new WriteLine(),
                        new WriteLine { Text = new InArgument<string>("This order does not match any of the rules above") },
                        new WriteLine
                        {
                            Text = new InArgument<string>(c => string.Format("   Before Evaluation: {0}", order3.Get(c).ToString()))
                        },
                        new Policy4<Order>
                        {
                            RuleSet = ruleSet,
                            Input = new InArgument<Order>(order3),
                            Result = new OutArgument<Order>(order3)
                        },
                        new WriteLine
                        {
                            Text = new InArgument<string>(c => string.Format("   After Evaluation: {0}", order3.Get(c).ToString()))
                        }

                    }
                }
            );

            // wait until the user press a key
            Console.WriteLine("Press any key to exit...");
            Console.Read();
        }
    }
}
