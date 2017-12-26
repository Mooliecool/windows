//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;
using System.Threading;

namespace Microsoft.Samples.Workflow.TransactionalServiceSample
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        static void Main(string[] args)
        {

            const string persistenceConnectionString = "Initial Catalog=SqlPersistenceService;Data Source=localhost;Integrated Security=SSPI;";
            const string transactionServiceDataBase = "Initial Catalog=TransactionServiceSampleDB;Data Source=localhost;Integrated Security=SSPI;Enlist=false;";
            bool validInteger = false;
            Int32 transferAmount = 0;

            //Display account balances before requesting transfer amount
            QueryAccountService queryAccounts = new QueryAccountService(transactionServiceDataBase);
            Int32[] accountBalances = queryAccounts.QueryAccount(1);
            Console.WriteLine("The account balances for account number {0} are:  Checking : {1:c} , Savings : {2:c}",
                 1, accountBalances[0], accountBalances[1]);

            Console.WriteLine("Please enter an amount to transfer from Savings to Checking");

            while (!validInteger)
            {
                try
                {
                    transferAmount = Convert.ToInt32(Console.ReadLine());
                    if (transferAmount < 0)
                    {
                        Console.WriteLine("Please enter an amount greater than zero.");
                    }
                    else
                    {
                        validInteger = true;
                    }
                }
                catch (FormatException)
                {
                    Console.WriteLine("Please enter a valid amount and try again.");
                }
            }

            //Initiate the workflow run time
            using (WorkflowRuntime runtime = new WorkflowRuntime())
            {
                // Add the SQL persistence service
                SqlWorkflowPersistenceService persistenceService = new SqlWorkflowPersistenceService(persistenceConnectionString);
                runtime.AddService(persistenceService);

                // Add the query account service. This will be used to query the account balances
                runtime.AddService(queryAccounts);

                // Add the transactional service. This is the service which 
                // does the work of crediting and debiting the amounts
                // This service participates in the work batch of the workflow instance
                TransactionalService transactionService = new TransactionalService(transactionServiceDataBase);
                runtime.AddService(transactionService);
                runtime.WorkflowCompleted += new EventHandler<WorkflowCompletedEventArgs>(wr_OnWorkflowCompleted);
                runtime.WorkflowAborted += new EventHandler<WorkflowEventArgs>(wr_OnWorkflowAborted);
                runtime.WorkflowTerminated += delegate(object sender, WorkflowTerminatedEventArgs e)
                {
                    Console.WriteLine(e.Exception.Message);
                    waitHandle.Set();
                };
                runtime.StartRuntime();
                Dictionary<string, object> parameters = new Dictionary<string, object>();
                parameters.Add("TransferAmount", transferAmount);

                // Initiate the workflow
                runtime.CreateWorkflow(typeof(BalanceTransferWorkflow), parameters).Start();
                Console.WriteLine("Running the workflow");

                // Wait for the workflow to finish
                waitHandle.WaitOne();

                Console.WriteLine("Done running the workflow.");

                runtime.StopRuntime();
            }
        }

        static void wr_OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs instance)
        {
            waitHandle.Set();
        }

        static void wr_OnWorkflowAborted(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow aborted: Please check database connectivity");
            waitHandle.Set();
        }
    }
}
