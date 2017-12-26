//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Transactions;

namespace Microsoft.Samples.TransactionRollback
{
    public class Rollback : NativeActivity
    {
        public InArgument<Exception> Reason { get; set; }

        protected override void Execute(NativeActivityContext context)
        {
            //Gets the ambient RuntimeTransactionHandle which allows access to the runtime transaction
            RuntimeTransactionHandle rth = context.Properties.Find(typeof(RuntimeTransactionHandle).FullName) as RuntimeTransactionHandle;

            if (rth == null)
            {
                throw new InvalidOperationException("There is no RuntimeTransactionHandle");
            }

            //The runtime transaction
            Transaction t = rth.GetCurrentTransaction(context);

            if (t == null)
            {
                throw new InvalidOperationException("There is no transaction to rollback");
            }

            Exception e = Reason.Get(context);

            if (e == null)
            {
                e = new TransactionException("The Rollback activity was used to rollback the transaction");
            }
            
            t.Rollback(e);
        }
    }
}
