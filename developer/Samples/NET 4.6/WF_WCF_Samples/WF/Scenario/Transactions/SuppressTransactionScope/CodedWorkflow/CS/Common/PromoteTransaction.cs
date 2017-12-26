//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.Transactions;

namespace Microsoft.Samples.SuppressTransactionSample
{

    public class PromoteTransaction : CodeActivity
    {
        protected override void Execute(CodeActivityContext context)
        {
            TransactionInterop.GetTransmitterPropagationToken(System.Transactions.Transaction.Current);
        }
    };
}
