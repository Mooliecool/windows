
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Collections.Generic;
using System.ServiceModel;

namespace Microsoft.ServiceModel.Samples.SupportingTokens
{
    [ServiceContract]
    public interface IEchoService : IDisposable
    {
        [OperationContract]
        string Echo();
    }
}