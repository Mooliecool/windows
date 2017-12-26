/****************************** Module Header ******************************\
* Module Name:  SqlAzureHelper.cs
* Project: CSSqlAzurePartitioning
* Copyright (c) Microsoft Corporation.
* 
* This file is from Microsoft SQL Azure team's blog.
* http://blogs.msdn.com/b/sqlazure/archive/2010/05/17/10014011.aspx
* 
* 1. Implements forward read only cursors for performance.
* 2. Support IEnumerable and LINQ
* 3. Disposes of the connection and the data reader when the result set is no longer needed.
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Data.SqlClient;
using System.Data.Common;
using System.Configuration;
using System.Threading;

namespace SQLAzure
{
    internal delegate SqlDataReader ExecuteReaderDelegate(SqlConnection sqlConnection);
    internal delegate void ExecuteNonQueryDelegate(SqlConnection sqlConnection);

    sealed class SQLAzureHelper
    {
        internal static IEnumerable<DbDataRecord> ExecuteReader(String connectionString, ExecuteReaderDelegate executeDelegate)
        {
            using (SqlConnection sqlConnection = new SqlConnection(connectionString))
            {
                // Open the connection
                sqlConnection.Open();

                using (SqlDataReader sqlDataReader = executeDelegate(sqlConnection))
                {
                    foreach (DbDataRecord dbDataRecord in sqlDataReader.Cast<DbDataRecord>())
                        yield return dbDataRecord;
                }
            }
        }

        internal static void ExecuteNonQuery(String connectionString, ExecuteNonQueryDelegate executeDelegate)
        {
            // This is the retry loop, handling the retries session
            // is done in the catch for performance reasons
            for (Int32 attempt = 1; ; )
            {
                try
                {
                    using (SqlConnection sqlConnection = new SqlConnection(connectionString))
                    {
                        // Open the connection
                        sqlConnection.Open();
                        executeDelegate(sqlConnection);
                    }
                    // Success Break Out Of Attempt Loop
                    break;
                }
                catch (SqlException sqlException)
                {
                    // Increment Trys
                    attempt++;

                    // Find Maximum Trys
                    Int32 maxRetryCount = Int32.Parse(
                        ConfigurationManager.AppSettings["ConnectionRetrys"]);

                    // Throw Error if we have reach the maximum number of retries
                    if (attempt == maxRetryCount)
                        throw;

                    // Determine if we should retry or abort.
                    if (!SQLAzureHelper.RetryLitmus(sqlException))
                        throw;
                    else
                        Thread.Sleep(SQLAzureHelper.ConnectionRetryWaitSeconds(attempt));
                }
            }
        }
 
        /// <summary>
        /// Number of seconds to wait before retrying the connection
        /// </summary>
        /// <param name="attempt"></param>
        /// <returns></returns>
        public static Int32 ConnectionRetryWaitSeconds(Int32 attempt)
        {
            Int32 connectionRetryWaitSeconds = Int32.Parse(ConfigurationManager.
                AppSettings["ConnectionRetryWaitSeconds"])
                * 1000;

            // Backoff Throttling, here we slow the retries, based on the Number of 
            // Attempts
            connectionRetryWaitSeconds = connectionRetryWaitSeconds *
                (Int32)Math.Pow(2, attempt);

            return (connectionRetryWaitSeconds);
        }

        /// <summary>
        /// Determine from the exception if the execution
        /// of the connection should Be attempted again
        /// </summary>
        /// <param name="exception">Generic Exception</param>
        /// <returns>True if a a retry is needed, false if not</returns>
        public static Boolean RetryLitmus(SqlException sqlException)
        {
            switch (sqlException.Number)
            {
                // The service has encountered an error
                // processing your request. Please try again.
                // Error code %d.
                case 40197:
                // The service is currently busy. Retry
                // the request after 10 seconds. Code: %d.
                case 40549:
                //A transport-level error has occurred when
                // receiving results from the server. (provider:
                // TCP Provider, error: 0 - An established connection
                // was aborted by the software in your host machine.)
                case 10053:
                    return (true);
            }

            return (false);
        }

        /// <summary>
        /// Names of the Databases In Horizontal Partition
        /// </summary>
        public static String[] ConnectionStringNames = { "Database001ConnectionString", "Database002ConnectionString" };

        /// <summary>
        /// Connections Strings In the Horizontal Partition
        /// </summary>
        /// <returns></returns>
        public static IEnumerable<String> ConnectionStrings()
        {
            foreach (String connectionStringName in ConnectionStringNames)
                yield return ConfigurationManager.ConnectionStrings[connectionStringName].ConnectionString;
        }

        /// <summary>
        /// Return the Index to the Database For the Primary Key
        /// </summary>
        /// <param name="primaryKey"></param>
        /// <returns></returns>
        private static int DatabaseIndex(Guid primaryKey)
        {
            uint hash = 0;

            foreach (byte b in primaryKey.ToByteArray())
            {
                hash += b;
                hash += (hash << 10);
                hash ^= (hash >> 6);
            }

            // Final avalanche 
            hash += (hash << 3);
            hash ^= (hash >> 11);
            hash += (hash << 15);

            return ((int)(hash % 100000000));
        }

        /// <summary>
        /// Returns the Connection String Name for the Primary Key
        /// </summary>
        /// <param name="primaryKey"></param>
        /// <returns></returns>
        private static String ConnectionStringName(Guid primaryKey)
        {
            return (ConnectionStringNames[DatabaseIndex(primaryKey) % ConnectionStringNames.Length]);
        }

        /// <summary>
        /// Returns the Connection String For the Primary Key
        /// </summary>
        /// <param name="primaryKey"></param>
        /// <returns></returns>
        public static String ConnectionString(Guid primaryKey)
        {
            return (ConfigurationManager.ConnectionStrings[ConnectionStringName(primaryKey)].ConnectionString);
        }
    }
}
