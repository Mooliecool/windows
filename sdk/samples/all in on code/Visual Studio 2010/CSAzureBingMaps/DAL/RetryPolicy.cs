/****************************** Module Header ******************************\
* Module Name:	RetryPolicy.cs
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* A SQL Azure retry policy.
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
using System.Data.SqlClient;
using System.Threading;

namespace AzureBingMaps.DAL
{
    /// <summary>
    /// A SQL Azure retry policy.
    /// </summary>
    public class RetryPolicy
    {
        public int RetryNumber { get; set; }
        public TimeSpan WaitTime { get; set; }
        public Action Task { get; set; }

        public RetryPolicy()
        {
            this.RetryNumber = 3;
        }

        public RetryPolicy(int retryNumber)
        {
            this.RetryNumber = retryNumber;
            this.WaitTime = TimeSpan.FromSeconds(5d);
        }

        /// <summary>
        /// If the execution fails, retry it after a specific time,
        /// until the maximum retry policy exceeds.
        /// </summary>
        public void Execute()
        {
            for (int i = 0; i < this.RetryNumber; i++)
            {
                try
                {
                    this.Task();
                    break;
                }
                catch (SqlException ex)
                {
                    if (i == this.RetryNumber - 1)
                    {
                        throw new SqlExceptionWithRetry(
                            "Maximum retry reached. Still unable to process the request. See inner exception for more details.",
                            ex);
                    }
                    Thread.Sleep(this.WaitTime);
                }
            }
        }
    }

    public class SqlExceptionWithRetry : Exception
    {
        public SqlExceptionWithRetry(string message, SqlException innerException)
            : base(message, innerException)
        {
        }
    }
}
