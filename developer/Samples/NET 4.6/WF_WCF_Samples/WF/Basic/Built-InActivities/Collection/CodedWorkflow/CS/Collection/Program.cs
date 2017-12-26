//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Expressions;
using System.Activities.Statements;
using System.Collections.Generic;

namespace Microsoft.Samples.Collection
{
   public class Program
    {
        public static void Main(string[] args)
        {
            WorkflowInvoker.Invoke(CreateWf());

            Console.WriteLine("Press <return> to continue...");
            Console.Read();
        }

        private static Activity CreateWf()
        {
            Random random = new Random();

            // Create a collection and insert some numbers into it
            Variable<ICollection<int>> numbers = new Variable<ICollection<int>>()
            {
                Name = "Numbers",
                Default = new LambdaValue<ICollection<int>>(ctx => new List<int> { 1, 2, 3, 4 }),
            };

            // Body
            Sequence body = new Sequence()
            {
                Variables = { numbers },
                Activities = 
		        {
					// Print out the initial collection
					new WriteLine { Text = "Initial collection:" },
					new PrintCollection<int>() 
					{
		            	Collection = numbers
					},
                    new WriteLine { Text = "-----------------" },
		              
                    // Clear collection
                    new WriteLine { Text = "Clearing the collection" },
                    new ClearCollection<int>() 
                    {
                        Collection = numbers,
                    },			            
                    new PrintCollection<int>() 
                    {
                        Collection = numbers,
                    },
                    new WriteLine { Text = "-----------------" },            
		  
                    // Add 3 random numbers to the collection
                    new AddToCollection<int>() 
                    {
                        Collection = numbers,
                        Item = random.Next(1, 10)
                    },
		                
                    new AddToCollection<int>() 
                    {
                        Collection = numbers,
                        Item = random.Next(1, 10)
                    },
		        			
                    new AddToCollection<int>() 
                    {
                        Collection = numbers,
                        Item = random.Next(1, 10)
                    },

                    // Print out of collection 
                    new WriteLine 
                    {
                        Text = "Collection with three random numbers:"
                    },
                    new PrintCollection<int>() 
                    {
                        Collection = numbers
                    },
                    new WriteLine { Text = "-----------------" },

                    // Check if the number 5 is in the collection
                    new If() 
                    {
                        Condition = new ExistsInCollection<int>()
                        {
                            Collection = numbers,
                            Item = 5
                        },
                        Then = new RemoveFromCollection<int>() 
                        {
                            // Remove 5 to the collection
                            Collection = numbers,
                            Item = 5
                        }
                    },

                    // Print out of collection without 5 in it
                    new WriteLine { Text = "Collection without a 5:" },
                    new PrintCollection<int>() 
                    {
                        Collection = numbers
                    }
		        }
            };

            return body;
        }
    }
}
