//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ServiceModel;

namespace Microsoft.WorkflowServices.Samples
{
    [ServiceContract(Namespace = "http://Microsoft.WorkflowServices.Samples")]
    public interface ICalculator
    {
        [OperationContract()]
        int PowerOn();
        [OperationContract()]
        int Add(int value);
        [OperationContract()]
        int Subtract(int value);
        [OperationContract()]
        int Multiply(int value);
        [OperationContract()]
        int Divide(int value);
        [OperationContract()]
        void PowerOff();
    }  
}
