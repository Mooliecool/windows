//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.Threading;

namespace Microsoft.Samples.TransactionRollback
{
    class Program
    {       
        static void Main(string[] args)
        {            
            AutoResetEvent syncEvent = new AutoResetEvent(false);

            //The workflow consists of a TransactionScope with a sequence of activities for its body. The sequence will print the transaction
            //id and state before and after rolling back the transaction. When the TransactionScope completes it will abort the workflow.
            WorkflowApplication app = new WorkflowApplication(BuildWorkflow());

            //Only the Aborted handler is needed for this sample because the default behavior for the TransactionScope is to abort if the
            //transaction is aborted
            app.Aborted = delegate(WorkflowApplicationAbortedEventArgs e) 
            { 
                Console.WriteLine("Workflow aborted: {0}", e.Reason); 
                syncEvent.Set(); 
            };

            app.Run();
            syncEvent.WaitOne();

            Console.WriteLine("Sample has ended, press [ENTER] to exit");
            Console.ReadLine();
        }

        static Activity BuildWorkflow()
        {
            return new TransactionScope
            {
                Body = new Sequence
                {
                    Activities =
                    {
                        new WriteLine { Text = "Begin Workflow" },

                        //Transaction is active. This activity will access the T:System.Transactions.TransactionInformation for the runtime
                        //transaction and print the local id and status of the transaction.
                        new PrintTransactionInfo(),

                        new Rollback() { Reason = new InArgument<Exception>((env) => new ApplicationException("Something has gone wrong and I need to rollback the transaction.")) },

                        //Transaction is aborted
                        new PrintTransactionInfo(),

                        new WriteLine { Text = "End Workflow" },
                    }
                },
            };
        }
    }
}
