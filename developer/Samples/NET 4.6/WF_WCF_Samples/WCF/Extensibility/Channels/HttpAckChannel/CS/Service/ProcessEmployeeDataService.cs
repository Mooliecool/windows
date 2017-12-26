//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Transactions;

namespace Microsoft.Samples.HttpAckChannel
{
    public class ProcessEmployeeDataService : IProcessEmployeeData
    {
        public void ProcessData(int employeeId)
        {
            // There is not much processing logic here...
            // We are assuming that the organization has 50 employees with sequential Employee Ids
            // If the employeeId  > 50, we are sending an Http Status code of 500 (Internal Server Error) back to the client
            // else, we assume that the processing is successful and send an Http Status code of 200 (Successfull)
            ReceiveContext receiveContext;
            if (!ReceiveContext.TryGet(OperationContext.Current.IncomingMessageProperties, out receiveContext))
            {
                Console.WriteLine("ReceiveContext property was not found in the message...");
                return;
            }

            if (employeeId > 50)
            {
                // Abandon
                receiveContext.Abandon(TimeSpan.MaxValue);
            }
            else
            {
                // Complete in Transaction block.
                CommittableTransaction committableTransaction = new CommittableTransaction();
                Transaction.Current = committableTransaction;
                try
                {
                    receiveContext.Complete(TimeSpan.MaxValue);
                    committableTransaction.Commit();
                }
                catch
                {
                    committableTransaction.Rollback();
                    // If the transaction was not completed we call Abandon explicitly which sends an Http 500 to the client
                    receiveContext.Abandon(TimeSpan.MaxValue);
                }
            }
        }
    }
}
