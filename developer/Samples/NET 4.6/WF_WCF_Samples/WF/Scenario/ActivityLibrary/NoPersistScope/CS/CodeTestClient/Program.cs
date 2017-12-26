//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.DurableInstancing;
using System.Activities.Expressions;
using System.Activities.Statements;
using System.IO;
using System.Runtime.DurableInstancing;
using System.Threading;

namespace Microsoft.Samples.Activities.Statements
{
    class Program
    {
        const string connectionString = @"Data Source=.\SQLExpress;Initial Catalog=DefaultSampleStore;Integrated Security=True;Asynchronous Processing=True";

        // start the WF and interact with the user
        static void Main()
        {
            AutoResetEvent resetEvent = new AutoResetEvent(false);            
            
            // create the instance of the workflow
            Activity program = CreateProgram();

            // create and start a workflow application
            WorkflowApplication application = StartWorkflow(program, resetEvent);
           
            // interact with the application
            Interact(application, resetEvent);
        }

        // create the workflow
        static Activity CreateProgram()
        {
            Variable<TextWriter> writer = new Variable<TextWriter>() { Name = "writer" };
            Variable<string> input = new Variable<string> { Name = "input", Default = "" };            

            return new Sequence()
            {
                Activities = 
                { 
                    new NoPersistScope
                    {
                        Body = new TryCatch()
                        {
                            Variables = { writer },
                            Try = new Sequence() 
                            {
                                Activities =
                                {
                                    // Output will appear in the out.txt file in the directory where the program runs
                                    new CreateTextWriter()
                                    {                                        
                                        Filename = "out.txt",
                                        Result = writer
                                    },

                                    // run until the user enters "exit"
                                    new While()
                                    {
                                        Variables = { input },
                                        Condition =  ExpressionServices.Convert(ctx => !input.Get(ctx).Equals("exit")),                                        
                                        Body = new Sequence()
                                        {
                                            Activities =
                                            {
                                                // if the user enters a string, write it in the file
                                                new If()
                                                {                                                    
                                                    Condition = new InArgument<bool>(ctx => !string.IsNullOrEmpty(input.Get(ctx))),
                                                    Then = new WriteLine
                                                    {
                                                        Text = new InArgument<string>(ctx => string.Format("Echo: {0}", input.Get(ctx))),
                                                        TextWriter = writer
                                                    }
                                                },

                                                // now, wait for a new string from the user
                                                new ReadLine()
                                                {
                                                    BookmarkName = "inputBookmark",
                                                    Result = input
                                                },
                                            }
                                        }
                                    }
                                }
                            },
                            Finally = new Dispose() { Target = writer }
                        }
                    }
                }
            };
        }

        // start a workflow instance and configure OOB persistence
        static WorkflowApplication StartWorkflow(Activity program, AutoResetEvent resetEvent)
        {
            // create the application 
            WorkflowApplication application = new WorkflowApplication(program);

            // configure the instance store (in this case we use the Sql OOB instance store)
            InstanceStore store = new SqlWorkflowInstanceStore(connectionString);
            application.InstanceStore = store;
            application.PersistableIdle = (e) => PersistableIdleAction.Persist;

            // run the workflow
            RunWorkflow(application, resetEvent);

            return application;
        }

        // resume execution of a workflow instance
        static void RunWorkflow(WorkflowApplication application, AutoResetEvent resetEvent)
        {
            application.Completed = delegate(WorkflowApplicationCompletedEventArgs e)
            {
                Console.WriteLine("Workflow completed in state " + e.CompletionState);

                if (e.TerminationException != null)
                {
                    Console.WriteLine("Termination exception: " + e.TerminationException);
                }

                Console.WriteLine("-------------------------------");
                Console.WriteLine("- Input has been written to {0}\\out.txt", Environment.CurrentDirectory);
                Console.WriteLine("-------------------------------");

                resetEvent.Set();
            };
            application.Unloaded = delegate(WorkflowApplicationEventArgs e)
            {
                Console.WriteLine("Workflow unloaded");
                resetEvent.Set();
            };

            application.Run();
        }

        // single interaction with the user. The user enters a string in the console and that
        // string is used to resume the ReadLine activity bookmark
        static void Interact(WorkflowApplication application, AutoResetEvent resetEvent)
        {
            Console.WriteLine("Workflow is ready for input");
            Console.WriteLine("Special commands: 'unload', 'exit'");

            bool done = false;
            while (!done)
            {
                Console.Write("> ");
                string s = Console.ReadLine();
                if (s.Equals("unload"))
                {
                    try
                    {
                        // attempt to unload will fail if the workflow is idle within a NoPersistZone
                        application.Unload(TimeSpan.FromSeconds(5));
                        done = true;
                    }
                    catch (TimeoutException e)
                    {
                        Console.WriteLine(e.Message);
                    }
                }
                else if (s.Equals("exit"))
                {
                    application.ResumeBookmark("inputBookmark", s);
                    done = true;
                }
                else
                {
                    application.ResumeBookmark("inputBookmark", s);
                }
            }
            resetEvent.WaitOne();
        }
    }
}
