//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Expressions;
using System.Activities.Statements;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;

namespace Microsoft.Samples.Activities.Data
{

    class Program
    {
        // this sample shows how to implement some scenarios with Entity Framework Activities
        static void Main(string[] args)
        {
            string connStr = ConfigurationManager.ConnectionStrings["NorthwindEntities"].ConnectionString;

            string choice;
            Activity activity;

            DisplayMenu();
            while ((choice = Console.ReadLine().Trim()) != null)
            {
                switch (choice)
                {
                    case "1":
                        activity = EntitySqlQueryExample(connStr);
                        WorkflowInvoker.Invoke(activity);
                        break;
                    case "2":
                        activity = EntityLinqQueryExample(connStr);
                        WorkflowInvoker.Invoke(activity);
                        break;
                    case "3":
                        activity = EntityAddExample(connStr);
                        WorkflowInvoker.Invoke(activity);
                        break;
                    case "4":
                        activity = EntityDeleteExample(connStr);
                        WorkflowInvoker.Invoke(activity);
                        break;
                    case "5":
                        return;
                }
                DisplayMenu();
            }          
        }        

        // This scenario shows how to query for a set of entities from the database using EntitySql
        private static Activity EntitySqlQueryExample(string connStr)
        {            
            Variable<IEnumerable<Customer>> londonCustomers = new Variable<IEnumerable<Customer>>();
            DelegateInArgument<Customer> iterationVariable = new DelegateInArgument<Customer>();
            
            // create and return the workflow
            return new ObjectContextScope
            {
                ConnectionString = new InArgument<string>(connStr),
                ContainerName = "NorthwindEntities",
                Variables = { londonCustomers },
                Body = new Sequence
                    {
                        Activities = 
                            {             
                                new WriteLine { Text = "Executing query" },                          
                                // query for all customers that are in london 
                                new EntitySqlQuery<Customer>
                                {
                                    EntitySql =  @"SELECT VALUE Customer 
                                                     FROM NorthwindEntities.Customers AS Customer 
                                                    WHERE Customer.City = 'London' ORDER BY Customer.ContactName",
                                    Result = londonCustomers
                                },
                                new WriteLine { Text = "Query executed - printing results: " },
                                new WriteLine { },

                                // iterate through the list of customers, and display them in the console
                                new ForEach<Customer>
                                {                                    
                                    Values = londonCustomers,
                                    Body = new ActivityAction<Customer>
                                    {
                                        Argument = iterationVariable,
                                        Handler = new WriteLine { Text = new InArgument<String>(e => iterationVariable.Get(e).ContactName) }
                                    }
                                }
                            }
                    }               
            };          
        }

        // This scenario shows how to query for a set of entities from the database using a Linq predicate
        private static Activity EntityLinqQueryExample(string connStr)
        {
            Variable<IEnumerable<Customer>> londonCustomers = new Variable<IEnumerable<Customer>>() { Name = "LondonCustomers" };
            DelegateInArgument<Customer> iterationVariable = new DelegateInArgument<Customer>() { Name = "iterationVariable" };

            return new ObjectContextScope
            {
                ConnectionString = new InArgument<string>(connStr),
                ContainerName = "NorthwindEntities",
                Variables = { londonCustomers },
                Body = new Sequence
                {
                    Activities = 
                    {             
                        new WriteLine { Text = "Executing query" },                     
                        // return all the customers that match with the provided Linq predicate
                        new EntityLinqQuery<Customer>
                        { 
                            Predicate = new LambdaValue<Func<Customer, bool>>(ctx => new Func<Customer, bool>(c => c.City.Equals("London"))),                            
                            Result = londonCustomers
                        },
                        new WriteLine { Text = "Query executed - printing results" },
                        new WriteLine { },

                        // iterate through the list of customers and display in the console
                        new ForEach<Customer>
                        {                                    
                            Values = londonCustomers,
                            Body = new ActivityAction<Customer>
                            {
                                Argument = iterationVariable,
                                Handler = new WriteLine { Text = new InArgument<String>(e => iterationVariable.Get(e).ContactName) }
                            }
                        }
                    }
                }
            };
        }
        
        // This scenario shows how to add an entity to the database
        private static Activity EntityAddExample(string connStr)
        {   
            Variable<IEnumerable<Order>> orders = new Variable<IEnumerable<Order>>();
            Variable<IEnumerable<OrderDetail>> orderDetails = new Variable<IEnumerable<OrderDetail>>();
            Variable<Order> order = new Variable<Order>();
            Variable<OrderDetail> orderDetail = new Variable<OrderDetail>();            
            
            return new ObjectContextScope
            {
                Variables = { order, orders, orderDetail, orderDetails },
                ContainerName = "NorthwindEntities",
                ConnectionString = new InArgument<string>(connStr),                
                Body = new Sequence
                {                    
                    Activities = 
                    {               
                        // get the order detail that we want to add (if exists, then we can't continue)
                        new EntitySqlQuery<OrderDetail>
                        {
                            EntitySql = @"SELECT VALUE OrderDetail
                                           FROM NorthwindEntities.OrderDetails as OrderDetail
                                          WHERE OrderDetail.OrderID == 10249 and OrderDetail.ProductID == 11",
                            Result = orderDetails
                        },

                        // if the order detail exists, we display an error message, otherwise we add it
                        new If
                        {
                            Condition = new InArgument<bool>(c=>orderDetails.Get(c).Count() > 0),
                            Then = new WriteLine{ Text = "OrderDetail already exists, cannot add to database. Run Delete Example first" },
                            Else = new Sequence
                            {
                                Activities = 
                                {
                                    // get the order where we want to add the detail
                                    new EntitySqlQuery<Order>
                                    {
                                        EntitySql =  @"SELECT VALUE [Order]
                                                         FROM NorthwindEntities.Orders as [Order]
                                                        WHERE Order.OrderID == 10249",
                                        Result = orders
                                    },

                                    // store the order in a variable
                                    new Assign<Order> 
                                    {
                                        To = new OutArgument<Order>(order),
                                        Value = new InArgument<Order>(c => orders.Get(c).First<Order>())
                                    },
                                    new WriteLine { Text = "Executing add" },       

                                    // add the detail to the order
                                    new EntityAdd<OrderDetail>
                                    {
                                        Entity = new InArgument<OrderDetail>(c => new OrderDetail { OrderID=10249, ProductID=11, Quantity=1, UnitPrice = 15, Discount = 0, Order = order.Get(c) })
                                    },
                                    new WriteLine { Text = "Add Executed" }, 
                                }
                            }
                        }                        
                    }
                }
            };
        }

        // This scenario shows how to delete an entity from the database
        private static Activity EntityDeleteExample(string connStr)
        {
            Variable<IEnumerable<OrderDetail>> orderDetails = new Variable<IEnumerable<OrderDetail>>();            

            return new ObjectContextScope
            {
                Variables = { orderDetails },
                ConnectionString = new InArgument<string>(connStr),
                ContainerName = "NorthwindEntities",
                Body = new Sequence
                {
                    Activities = 
                    {             
                        // find the entitiy to be deleted (order detail for product 11 in order 10249)
                        new EntitySqlQuery<OrderDetail>
                        {
                            EntitySql = @"SELECT VALUE OrderDetail
                                            FROM NorthwindEntities.OrderDetails as OrderDetail
                                           WHERE OrderDetail.OrderID == 10249 and OrderDetail.ProductID == 11",
                            Result = orderDetails
                        },

                        // if the order detail is found, delete it, otherwise, display a message
                        new If
                        {
                            Condition = new InArgument<bool>(c=>orderDetails.Get(c).Count() > 0),
                            Then = new Sequence
                            { 
                                Activities = 
                                {                                    
                                    new WriteLine { Text = "Order Detail found for Deleting - about to delete" },  

                                    //delete it!
                                    new EntityDelete<OrderDetail>
                                    {
                                        Entity = new InArgument<OrderDetail>(c => orderDetails.Get(c).First<OrderDetail>())
                                    },
                                    new WriteLine { Text = "Delete Executed" }
                                }
                            },                            
                            Else = new WriteLine { Text = "Order Detail for Deleting not found" }                            
                        }                                                
                    }
                }
            };
        }

        private static void DisplayMenu()
        {
            Console.WriteLine();
            Console.WriteLine("Please choose an example you would like to run:");
            Console.WriteLine("1. Entity SQL Query Activity");
            Console.WriteLine("2. Entity LINQ Query Activity");
            Console.WriteLine("3. Entity Add Activity");
            Console.WriteLine("4. Entity Delete Activity");
            Console.WriteLine("5. Exit Sample");
            Console.WriteLine();
            Console.Write(">> ");
        }
    }
}
