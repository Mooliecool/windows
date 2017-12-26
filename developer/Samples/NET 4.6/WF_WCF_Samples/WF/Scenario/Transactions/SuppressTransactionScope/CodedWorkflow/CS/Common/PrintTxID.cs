//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Transactions;

namespace Microsoft.Samples.SuppressTransactionSample
{

    public class PrintTxID : NativeActivity
    {
        protected override void Execute(NativeActivityContext context)
        {
            RuntimeTransactionHandle rth = context.Properties.Find(typeof(RuntimeTransactionHandle).FullName) as RuntimeTransactionHandle;
            Transaction runtimeTransaction = rth.GetCurrentTransaction(context);

            if (runtimeTransaction != null)
            {
                TransactionInformation tx = runtimeTransaction.TransactionInformation;

                Console.WriteLine("    Runtime transaction DistroID: " + tx.DistributedIdentifier);
            }
            else
            {
                Console.WriteLine("  No runtime transaction");
            }
        }
    }
}
