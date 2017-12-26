//  Copyright (c) Microsoft Corporation. All rights reserved.

using System.ServiceModel;
using System.ServiceModel.Channels;

namespace Microsoft.ServiceModel.Samples
{
    [ServiceContract]
    interface IWSTrust13
    {
        [OperationContract(Action = Constants.Trust13.Actions.Issue, ReplyAction = Constants.Trust13.Actions.IssueReply)]
        Message Issue(Message request);
    }
}
