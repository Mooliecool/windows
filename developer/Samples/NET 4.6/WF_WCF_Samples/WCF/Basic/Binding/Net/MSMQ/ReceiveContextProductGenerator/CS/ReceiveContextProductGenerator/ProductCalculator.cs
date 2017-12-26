//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Channels;

namespace Microsoft.Samples.Queues
{
    [ServiceBehavior(InstanceContextMode = InstanceContextMode.Single, ConcurrencyMode = ConcurrencyMode.Single)]
    class ProductCalculator : IProductCalculator
    {
        private static int receiveCount = 0;

        [OperationBehavior(TransactionScopeRequired = true, TransactionAutoComplete = true)]
        [ReceiveContextEnabled(ManualControl = true)]
        public void CalculateProduct(int firstNumber, int secondNumber)
        {
            ReceiveContext receiveContext;
            if (!ReceiveContext.TryGet(OperationContext.Current.IncomingMessageProperties, out receiveContext))
            {
                Console.WriteLine("ReceiveContext not installed/found on this machine.");
                return;
            }

            // Pseudo-randomly decide whether to process the message based on its content.
            if ((firstNumber * secondNumber) % 2 == (receiveCount % 2))
            {
                receiveContext.Complete(TimeSpan.MaxValue);
                Console.WriteLine("{0} x {1} = {2}", firstNumber, secondNumber, firstNumber * secondNumber);
            }
            else
            {
                receiveContext.Abandon(TimeSpan.MaxValue);
                Console.WriteLine("{0} & {1} not processed", firstNumber, secondNumber);
            }
            receiveCount++;
        }

    }
}
