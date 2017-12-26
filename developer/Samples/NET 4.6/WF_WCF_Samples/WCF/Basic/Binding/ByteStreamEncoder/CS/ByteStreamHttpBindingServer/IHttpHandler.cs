//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.ServiceModel;
using System.ServiceModel.Channels;

namespace Microsoft.Samples.ByteStreamEncoder
{

    [ServiceContract]
    interface IHttpHandler
    {
        [OperationContract(Action = "*", ReplyAction = "*")]
        Message ProcessRequest(Message request);
    }
}
