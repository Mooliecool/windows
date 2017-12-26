//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;

namespace Microsoft.Samples.ExtendedProtectionPolicy
{
    [ServiceContract]
    public interface IGetKey
    {
        [OperationContract]
        string GetKeyFromPasscode(string passCode);
    }
}
