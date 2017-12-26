//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System.ServiceModel;

namespace Microsoft.Samples.HttpAckChannel
{
    [ServiceContract]
    interface IProcessEmployeeData
    {
        [OperationContract(Action="*", IsOneWay=true)]
        [ReceiveContextEnabled(ManualControl=true)]
        void ProcessData(int employeeId);
    }
}
