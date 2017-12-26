//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.ServiceModel;

namespace Microsoft.Samples.NetHttpBinding
{
    [ServiceContract]
    public interface IEchoService
    {
        [OperationContract]
        string Echo(string message);
    }
}
