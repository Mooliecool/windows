//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.ServiceModel;

namespace Microsoft.Samples.Queues
{
    [ServiceContract]
    interface IProductCalculator
    {
        [OperationContract(IsOneWay = true)]
        void CalculateProduct(int firstNum, int secondNum);
    }
}
