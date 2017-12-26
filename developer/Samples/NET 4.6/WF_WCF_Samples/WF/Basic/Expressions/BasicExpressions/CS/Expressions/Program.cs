//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------
using System;
using System.Activities;
using System.Activities.Expressions;
using System.Activities.Statements;
using System.IO;
using System.Xaml;

namespace Microsoft.Samples.Workflow.Expressions
{
    class Program
    {
        static void Main(string[] args)
        {
            // First, load a workflow from compiled XAML.  
            // Open SalaryCalculation.xaml in workflow designer to see the usage of expressions.
            // Open SalaryCalculation.xaml in an XML editor to see these expressions represented in XAML.
            Activity workflow1 = new SalaryCalculation();
            
            // Second, create workflow in code.
            Activity workflow2 = CreateCodeOnlyWorkflow();

            // Finally, create a workflow in code with the purpose of saving it to XAML.
            Activity workflow3 = CreateXamlSerializableCodeWorkflow();

            Console.WriteLine("Starting workflow defined in designer...");
            WorkflowInvoker.Invoke(workflow1);

            Console.WriteLine();
            Console.WriteLine("Starting workflow defined in code...");
            WorkflowInvoker.Invoke(workflow2);

            Console.WriteLine();
            Console.WriteLine("Saving workflow defined in code to XAML...");
            string workflowXamlString = XamlServices.Save(workflow3);

            Console.WriteLine();
            Console.WriteLine("Starting workflow loaded from XAML...");
            Activity workflow4 = (Activity)XamlServices.Load(new StringReader(workflowXamlString));
            WorkflowInvoker.Invoke(workflow4);

            Console.WriteLine();
            Console.WriteLine("Press ENTER to exit...");
            Console.ReadLine();
        }

        static Activity CreateCodeOnlyWorkflow()
        {
            Variable<Employee> e1 = new Variable<Employee>("Employee1", ctx => new Employee("John", "Doe", 55000.0));
            Variable<Employee> e2 = new Variable<Employee>("Employee2", ctx => new Employee("Frank", "Kimono", 89000.0));
            Variable<SalaryStats> stats = new Variable<SalaryStats>("SalaryStats", ctx => new SalaryStats());
            Variable<Double> v1 = new Variable<double>();

            // The most efficient way of defining expressions in code is via LambdaValue and LambdaReference activities.
            // LambdaValue represents an expression that evaluates to an r-value and cannot be assigned to.
            // LambdaReference represents an expression that evaluates to an l-value and can be the target of an assignment.
            Sequence workflow = new Sequence()
            {
                Variables =
                {
                    e1, e2, stats, v1,
                },

                Activities =
                {
                    new WriteLine()
                    {
                        Text = new LambdaValue<string>(ctx => e1.Get(ctx).FirstName + " " + e1.Get(ctx).LastName + " earns " + e1.Get(ctx).Salary.ToString("$0.00")),
                    },
                    new WriteLine()
                    {
                        Text = new LambdaValue<string>(ctx => e2.Get(ctx).FirstName + " " + e2.Get(ctx).LastName + " earns " + e2.Get(ctx).Salary.ToString("$0.00")),
                    },
                    new Assign<double>()
                    {
                        To = new LambdaReference<double>(ctx => stats.Get(ctx).MinSalary),
                        Value = new LambdaValue<double>(ctx => Math.Min(e1.Get(ctx).Salary, e2.Get(ctx).Salary))
                    },
                    new Assign<double>()
                    {
                        To = new LambdaReference<double>(ctx => stats.Get(ctx).MaxSalary),
                        Value = new LambdaValue<double>(ctx => Math.Max(e1.Get(ctx).Salary, e2.Get(ctx).Salary))
                    },
                    new Assign<double>()
                    {
                        To = new LambdaReference<double>(ctx => stats.Get(ctx).AvgSalary),
                        Value = new LambdaValue<double>(ctx => (e1.Get(ctx).Salary + e2.Get(ctx).Salary) / 2.0)
                    },
                    new WriteLine()
                    {
                        Text = new LambdaValue<string>(ctx => String.Format(
                            "Salary statistics: minimum salary is {0:$0.00}, maximum salary is {1:$0.00}, average salary is {2:$0.00}",
                            stats.Get(ctx).MinSalary, stats.Get(ctx).MaxSalary, stats.Get(ctx).AvgSalary))
                    }
                },
            };

            return workflow;
        }

        static Activity CreateXamlSerializableCodeWorkflow()
        {
            Variable<Employee> e1 = new Variable<Employee> { Name = "Employee1", Default = ExpressionServices.Convert<Employee>(ctx => new Employee("John", "Doe", 55000.0)) };
            Variable<Employee> e2 = new Variable<Employee> { Name = "Employee2", Default = ExpressionServices.Convert<Employee>(ctx => new Employee("Frank", "Kimono", 89000.0)) };
            Variable<SalaryStats> stats = new Variable<SalaryStats> { Name = "SalaryStats", Default = ExpressionServices.Convert<SalaryStats>(ctx => new SalaryStats()) };
            Variable<Double> v1 = new Variable<double>();

            // Lambda expressions do not serialize to XAML.  ExpressionServices utility class can be used to 
            // convert them to operator activities, which do serialize to XAML.
            // ExpressionServices.Convert applies to r-values, which cannot be assigned to.
            // ExpressionServices.ConvertReference applies to l-values, which can be the target of an assignment.
            // Note that conversion is supported for a limited set of lambda expressions only.
            Sequence workflow = new Sequence()
            {
                Variables =
                {
                    e1, e2, stats, v1,
                },

                Activities =
                {
                    new WriteLine()
                    {
                        Text = ExpressionServices.Convert<string>(ctx => e1.Get(ctx).FirstName + " " + e1.Get(ctx).LastName + " earns " + e1.Get(ctx).Salary.ToString("$0.00")),
                    },
                    new WriteLine()
                    {
                        Text = ExpressionServices.Convert<string>(ctx => e2.Get(ctx).FirstName + " " + e2.Get(ctx).LastName + " earns " + e2.Get(ctx).Salary.ToString("$0.00")),
                    },
                    new Assign<double>()
                    {
                        To = ExpressionServices.ConvertReference<double>(ctx => stats.Get(ctx).MinSalary),
                        Value = ExpressionServices.Convert<double>(ctx => Math.Min(e1.Get(ctx).Salary, e2.Get(ctx).Salary))
                    },
                    new Assign<double>()
                    {
                        To = ExpressionServices.ConvertReference<double>(ctx => stats.Get(ctx).MaxSalary),
                        Value = ExpressionServices.Convert<double>(ctx => Math.Max(e1.Get(ctx).Salary, e2.Get(ctx).Salary))
                    },
                    new Assign<double>()
                    {
                        To = ExpressionServices.ConvertReference<double>(ctx => stats.Get(ctx).AvgSalary),
                        Value = ExpressionServices.Convert<double>(ctx => (e1.Get(ctx).Salary + e2.Get(ctx).Salary) / 2.0)
                    },
                    new WriteLine()
                    {
                        Text = ExpressionServices.Convert<string>(ctx => String.Format(
                            "Salary statistics: minimum salary is {0:$0.00}, maximum salary is {1:$0.00}, average salary is {2:$0.00}",
                            stats.Get(ctx).MinSalary, stats.Get(ctx).MaxSalary, stats.Get(ctx).AvgSalary))
                    }
                },
            };

            return workflow;
        }
    }
}
