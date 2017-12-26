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
using System.Collections;
using System.Collections.Specialized;
using System.Workflow.Runtime;
using System.Data;
using System.Data.SqlClient;

namespace Microsoft.Samples.Workflow.TransactionalServiceSample
{
    public abstract class AbstractTransactionService
    {
        public virtual void CreditAmount(Int32 amount)
        {
        }
        public virtual void DebitAmount(Int32 amount)
        {
        }
    }
    public class TransactionalService : AbstractTransactionService,IPendingWork
    {
        private string connectionStringValue;

        public TransactionalService(NameValueCollection parameters)
        {
            if (parameters == null)
            {
                throw new ArgumentNullException("parameters", "The name value collection 'parameters' cannot be null");
            }
            string connectionString = parameters["ConnectionString"];
            if (connectionString != null)
            {
                init(connectionString);
            }
            else
            {
                throw new ArgumentNullException("connectionString","Connection string not found in the name-value collection");
            }
        }

        public TransactionalService(string connectionString)
        {
            if (connectionString == null)
            {
                throw new ArgumentNullException("connectionString", "Connection string cannot be null");
            }
            init(connectionString);
        }

        private void init(string connectionString)
        {
            this.connectionStringValue = connectionString;
            // Check if SQL server is available
            using (SqlConnection connection = new SqlConnection(this.connectionStringValue))
            {
                connection.Open();
            }
        }

        // This method will be called by the workflow to credit
        // the amount. This method simply adds the work to be done
        // to the work batch and returns. Note that there is no actual work of
        // crediting being done here
        public override void CreditAmount(int amount)
        {
            Request creditRequest = new Request(OperationType.Credit, amount);
            // Add the work to the batch and return
            WorkflowEnvironment.WorkBatch.Add(this, creditRequest);
        }

        // This method will be called by the workflow to debit
        // the amount. This method simply adds the work to be done
        // to the work batch and returns. Note that there is no actual work of
        // debiting being done here
        public override void DebitAmount(int amount)
        {
            Request debitRequest = new Request(OperationType.Debit, amount);
            WorkflowEnvironment.WorkBatch.Add(this, debitRequest);
        }

        // The actual work of crediting or debiting is done in this method
        // This method is called by the transaction co-ordinator when the
        // work batch is committed.
        void IPendingWork.Commit(System.Transactions.Transaction transaction, ICollection items)
        {
            foreach (Request request in items)
            {
                if (request.Operation == OperationType.Credit)
                {
                    CreditAmountInAccount(transaction,request.Amount);
                }
                if (request.Operation == OperationType.Debit)
                {
                    DebitAmountFromAccount(transaction,request.Amount);
                }
            }
        }

        // The method will ascertain if the class is really ready
        // to commit at this time. The transaction coordinator
        // will call this method before calling the Commit method
        // to find out if the Commit must be really done. In our case
        // we always want to commit, hence we return true
        bool IPendingWork.MustCommit(ICollection items)
        {
            return true;
        }
        internal void CreditAmountInAccount(System.Transactions.Transaction transaction, Int32 amount)
        {
            SqlCommand command = new SqlCommand();
            command.CommandType = CommandType.StoredProcedure;
            command.CommandText = "dbo.CreditAmount";
            command.Connection = new SqlConnection(connectionStringValue);
            
            try
            {
                command.Connection.Open();
                command.Connection.EnlistTransaction(transaction);
                SqlParameter parameter = new SqlParameter();
                parameter.ParameterName = "@Amount";
                parameter.SqlDbType = SqlDbType.Int;
                parameter.SqlValue = amount;
                parameter.Direction = ParameterDirection.Input;
                command.Parameters.Add(parameter);
                command.ExecuteNonQuery();
            }
            finally
            {
                command.Connection.Close();
            }
        }
        internal void DebitAmountFromAccount(System.Transactions.Transaction transaction, Int32 amount)
        {
            SqlCommand command = new SqlCommand();
            command.CommandType = CommandType.StoredProcedure;
            command.CommandText = "dbo.DebitAmount";
            command.Connection = new SqlConnection(connectionStringValue);
            command.Connection.Open();
            command.Connection.EnlistTransaction(transaction);

            try
            {                
                SqlParameter parameter = new SqlParameter();
                parameter.ParameterName = "@Amount";
                parameter.SqlDbType = SqlDbType.Int;
                parameter.SqlValue = amount;
                parameter.Direction = ParameterDirection.Input;
                command.Parameters.Add(parameter);
                int rows = command.ExecuteNonQuery();
            }
            finally
            {
                if ((null != command) && (null != command.Connection) && (ConnectionState.Closed != command.Connection.State))
                    command.Connection.Close();
            }
        }

        // Called after the commit is successful if any additional
        // clean up work needs to be done.
        void IPendingWork.Complete(bool succeeded, ICollection items)
        {
        }
    }
    [Serializable]
    internal class Request
    {
        public OperationType Operation;
        public Int32 Amount;

        public Request()
        {
        }
        public Request(OperationType requestOperation,Int32 requestAmount)
        {
            this.Operation = requestOperation;
            this.Amount = requestAmount;
        }
    }

    internal enum OperationType
    {
        Credit = 1,
        Debit = 2
    }
}
