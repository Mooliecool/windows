//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Transactions;

namespace Microsoft.Samples.TransactionRollback
{
    class PrintTransactionInfo : NativeActivity
    {
        protected override void Execute(NativeActivityContext context)
        {
            RuntimeTransactionHandle rth = context.Properties.Find(typeof(RuntimeTransactionHandle).FullName) as RuntimeTransactionHandle;

            if (rth == null)
            {
                Console.WriteLine("There is no ambient RuntimeTransactionHandle");
            }
            
            Transaction t = rth.GetCurrentTransaction(context);

            if (t == null)
            {
                Console.WriteLine("There is no ambient transaction");
            }
            else
            {
                Console.WriteLine("Transaction: {0} is {1}", t.TransactionInformation.LocalIdentifier, t.TransactionInformation.Status);
            }
        }
    }
}
