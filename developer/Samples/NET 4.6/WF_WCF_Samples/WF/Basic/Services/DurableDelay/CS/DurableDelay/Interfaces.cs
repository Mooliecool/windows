//------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------
using System.ServiceModel;

namespace Microsoft.Samples.DurableDelay
{
    [ServiceContract]
    public interface IWorkflow
    {
        [OperationContract(IsOneWay = true)]
        void Start();
    }
}
