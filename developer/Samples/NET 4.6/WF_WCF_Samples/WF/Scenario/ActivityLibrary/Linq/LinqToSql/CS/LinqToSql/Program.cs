//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------
using System;
using System.Activities;
using System.Activities.Expressions;
using System.Activities.Statements;
using System.Collections.Generic;

namespace Microsoft.Samples.Activities.Data
{

    class Program
    {
        // connection string for the database
        static string cnn = @"Data Source=.\SQLExpress;Initial Catalog=LinqToSqlSample;Integrated Security=True";

        static void Main(string[] args)
        {            
            WorkflowInvoker.Invoke(CreateRetrieveWithoutPredicateWF());

            WorkflowInvoker.Invoke(CreateRetrieveWithSimplePredicateWF());

            WorkflowInvoker.Invoke(CreateRetrieveWithComplexPredicateWF());

            Console.WriteLine("Press [ENTER] to exit...");
            Console.ReadLine();
        }


        // Creates a workflow that uses FindInTable activity without predicate to retrieve entities from a Sql Server Database
        static Activity CreateRetrieveWithoutPredicateWF()
        {
            Variable<IList<Role>> roles = new Variable<IList<Role>>();
            DelegateInArgument<Role> roleIterationVariable = new DelegateInArgument<Role>();
            
            return new Sequence()
            {
                Variables = { roles },
                Activities =
                {                    
                    new WriteLine { Text = "\r\nAll Roles (no predicates)\r\n==============" },                    
                    new FindInSqlTable<Role> 
                    {
                        ConnectionString = cnn,
                        Result = new OutArgument<IList<Role>>(roles)
                    }, 
                    new ForEach<Role>
                    {
                        Values = new InArgument<IEnumerable<Role>>(roles),
                        Body = new ActivityAction<Role>()
                        {
                            Argument = roleIterationVariable ,
                            Handler = new WriteLine { Text = new InArgument<string>(env => roleIterationVariable.Get(env).Name) }
                        }
                    }
                }
            };
        }

        // Creates a workflow that uses FindInSqlTable activity with a simple predicate to retrieve entities from a Sql Server Database
        static Activity CreateRetrieveWithSimplePredicateWF()
        {
            Variable<IList<Employee>> employees = new Variable<IList<Employee>>();
            DelegateInArgument<Employee> employeeIterationVariable = new DelegateInArgument<Employee>();

            return new Sequence()
            {
                Variables = { employees },
                Activities =
                {                                      
                    new WriteLine { Text = "\r\nAll Employees (simple predicate)\r\n==============" },
                    new FindInSqlTable<Employee> 
                    {
                        ConnectionString = cnn,                        
                        Predicate = new LambdaValue<Func<Employee, bool>>(c => new Func<Employee, bool>(emp => emp.Role.Equals("SDE"))),
                        Result = new OutArgument<IList<Employee>>(employees)
                    }, 
                    new ForEach<Employee>
                    {
                        Values = new InArgument<IEnumerable<Employee>>(employees),
                        Body = new ActivityAction<Employee>()
                        {
                            Argument = employeeIterationVariable ,
                            Handler = new WriteLine { Text = new InArgument<string>(env => employeeIterationVariable.Get(env).ToString()) }
                            
                        }
                    }                  
                }
            };
        }

        // Creates a workflow that uses FindInSqlTable activity with a more complex predicate to retrieve entities from a Sql Server Database
        static Activity CreateRetrieveWithComplexPredicateWF()
        {
            Variable<IList<Employee>> employees = new Variable<IList<Employee>>();
            DelegateInArgument<Employee> employeeIterationVariable = new DelegateInArgument<Employee>();
            return new Sequence()
            {
                Variables = { employees },
                Activities =
                {                                        
                    new WriteLine { Text = "\r\nEmployees by Role and Location (more complex predicate)\r\n============================================" },
                    new FindInSqlTable<Employee> 
                    {
                        ConnectionString = cnn,                        
                        Predicate = new LambdaValue<Func<Employee, bool>>(c => new Func<Employee, bool>(emp => emp.Role.Equals("PM") && emp.Location.Equals("Redmond"))),                                   
                        Result = new OutArgument<IList<Employee>>(employees)                        
                    },                                       
                    new ForEach<Employee>
                    {
                        Values = new InArgument<IEnumerable<Employee>>(employees),
                        Body = new ActivityAction<Employee>()
                        {
                            Argument = employeeIterationVariable,
                            Handler = new WriteLine { Text = new InArgument<string>(env => employeeIterationVariable.Get(env).ToString()) }
                            
                        }
                    }                    
                }
            };
        }
    }
}