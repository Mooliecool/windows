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
using System.Data;
using System.Data.SqlClient;

namespace Microsoft.Samples.Workflow.TransactionalServiceSample
{
    // This class is an implementation of the IQueryAccount Service
    // This service will be invoked by the workflow to query the amount
    public class QueryAccountService : IQueryAccountService    
    {
        private string connectionString;

        public QueryAccountService(string accountConnectionString)
        {
            connectionString = accountConnectionString;
        }

        public Int32[] QueryAccount(int accountNumber)
        {
            Int32[] accountBalances = new Int32[] { 0, 0 };
            SqlCommand command = new SqlCommand();
            SqlDataReader reader = null;

            command.CommandType = CommandType.StoredProcedure;
            command.CommandText = "dbo.GetAccountBalances";
            command.Connection = new SqlConnection(connectionString);

            SqlParameter parameter = new SqlParameter();
            parameter.ParameterName = "@AccountNumber";
            parameter.SqlDbType = SqlDbType.Int;
            parameter.SqlValue = accountNumber;
            command.Parameters.Add(parameter);

            try
            {
                command.Connection.Open();
                reader = command.ExecuteReader(CommandBehavior.CloseConnection);
                if (!(reader.HasRows))
                {
                    throw new Exception("There are no rows in the database matching the account number"); ;
                }
                while (reader.Read())
                {
                    // Checking Amount
                    accountBalances[0] = reader.GetInt32(0);
                    // Savings Amount
                    accountBalances[1] = reader.GetInt32(1);
                }
            }
            finally
            {
                if ((null != command) && (null != command.Connection) && (ConnectionState.Closed != command.Connection.State))
                {
                    reader.Close();
                    command.Connection.Close();
                }
            }
            return accountBalances;
        }
    }
    
}
