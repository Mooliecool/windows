//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.Threading;

namespace Microsoft.Samples.Compensation.CompensableActivitySample
{

    class Program
    {
        // Builds a sequence of two actions that are compensable
        static Activity BuildASequenceofCompensableActions()
        {
            return new Sequence
            {
                Activities = 
                {
                    new WriteLine { Text = " Start of workflow" },

                    // first compensable action
                    new CompensableActivity
                    {
                        // Body contains the work required to execute for the first action
                        Body = new WriteLine { Text = " CompensableActivity: Do First Action" },
                        // Compensation contains the work required to execute if at a later time 
                        // the first action needs to be compensated
                        CompensationHandler = new WriteLine { Text = " CompensableActivity: Compensate First Action" }                        
                    },

                    // second compensable action
                    new CompensableActivity
                    {
                        Body = new WriteLine { Text = " CompensableActivity: Do Second Action" },
                        CompensationHandler = new WriteLine { Text = " CompensableActivity: Compensate Second Action" }                        
                    },

                    new WriteLine { Text = " End of workflow" }
                }
            };
        }

        // Builds a sequence two actions that are compensable
        // The sequence hits an unexpected event after the execution of the first action and
        // it throws System.Exception
        static Activity BuildASequenceofCompensableActionsThatThrows()
        {
            return new Sequence
            {
                Activities = 
                {
                    new WriteLine { Text = " Start of workflow" },

                    new CompensableActivity
                    {
                        Body = new WriteLine { Text = " CompensableActivity: Do First Action" },
                        CompensationHandler = new WriteLine { Text = " CompensableActivity: Compensate First Action" }                        
                    },
                    
                    // an unexpected event is encountered
                    new Throw { Exception = new InArgument<Exception>((env) => new Exception(" Exception of type 'System.Exception' was thrown.")), },
                    
                    // if the workflow application is not resumed after handling the exception, the following code
                    // will never execute
                    new CompensableActivity
                    {
                        Body = new WriteLine { Text = " CompensableActivity: Do Second Action" },
                        CompensationHandler = new WriteLine { Text = " CompensableActivity: Compensate Second Action" }                        
                    },

                    new WriteLine { Text = " End of workflow" }
                }
            };
        }
        
        static void Main()
        {
            WorkflowApplication application;
            
            // Executing the sequence of two actions that complete with success 
            Console.WriteLine("Execution with success:");
            application = new WorkflowApplication(BuildASequenceofCompensableActions());
            ManualResetEvent resetEvent = new ManualResetEvent(false);           
            application.Completed = delegate(WorkflowApplicationCompletedEventArgs e)
            {
                resetEvent.Set();
            };
            application.Run();
            resetEvent.WaitOne();

            // Notice how the first and second actions get executed and the workflow completes with success

            Console.WriteLine();
            Console.WriteLine("Execution with exception:");

            // Executing the sequence of two actions; after the first action is executed an exception will be thrown
            application = new WorkflowApplication(BuildASequenceofCompensableActionsThatThrows());
            resetEvent.Reset();
            application.Completed = delegate(WorkflowApplicationCompletedEventArgs e)
            {
                resetEvent.Set();
            };
            application.OnUnhandledException = delegate(WorkflowApplicationUnhandledExceptionEventArgs e)
            {
                Console.WriteLine("Workflow UnhandledException invoked...");
                Console.WriteLine("Exception Type: {0}", e.UnhandledException.GetType());
                Console.WriteLine("Exception Message: {0}", e.UnhandledException.Message);

                // simulate a case where the exception can't be corrected and decide to cancel the workflow
                return UnhandledExceptionAction.Cancel;
            };
            application.Run();
            resetEvent.WaitOne();
            // Notice how the first action is executed successfully and because the workflow terminates with
            // an exception and gets canceled, the first action gets compensated automatically;
            // Since the second action wasn't executed, no compensation is necessary for it.

            Console.WriteLine();
            Console.WriteLine("Press ENTER to exit");
            Console.ReadLine();
        }
    }
}
