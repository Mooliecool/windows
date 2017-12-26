//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Transactions;

namespace Microsoft.Samples.TRSComposabilitySample
{

    public class PrintTxInfo : NativeActivity
    {
        protected override void Execute(NativeActivityContext context)
        {
            //Access to the current transaction in Workflow is through the runtime transaction handle
            //Because workflow is not guaranteed to always run on the same thread the runtime maintains a clone
            //of the current transaction. The thread on which the workflow is currently executing will be set
            //up so that the transaction may be accessed using Transaction.Current the best practice is to use
            //the runtime's transaction by calling GetCurrentTransaction on the runtime transaction handle.
            RuntimeTransactionHandle rth = new RuntimeTransactionHandle();

            rth = context.Properties.Find(rth.ExecutionPropertyName) as RuntimeTransactionHandle;
            if (rth != null)
            {
                TransactionInformation ti = rth.GetCurrentTransaction(context).TransactionInformation;
                Console.WriteLine("Transaction: {0} is {1}.", ti.DistributedIdentifier.ToString(), ti.Status);
            }
            else
            {
                Console.WriteLine("There is no runtime transaction.");
            }
        }
    }    
}
