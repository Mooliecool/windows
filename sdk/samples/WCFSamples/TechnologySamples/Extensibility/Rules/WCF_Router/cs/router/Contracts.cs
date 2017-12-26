//  Copyright (c) Microsoft Corporation. All rights reserved.

using System.ServiceModel;
using System.ServiceModel.Channels;

namespace Microsoft.ServiceModel.Samples
{
    [ServiceContract(SessionMode = SessionMode.Allowed)]
    public interface ISimplexDatagramRouter
    {
        [OperationContract(IsOneWay = true, Action = "*")]
        void ProcessMessage(Message message);
    }

    [ServiceContract(SessionMode = SessionMode.Allowed)]
    public interface IRequestReplyDatagramRouter
    {
        [OperationContract(IsOneWay = false, Action = "*", ReplyAction = "*")]
        Message ProcessMessage(Message message);
    }

    [ServiceContract(SessionMode=SessionMode.Required)]
    public interface ISimplexSessionRouter
    {
        [OperationContract(IsOneWay = true, Action = "*")]
        void ProcessMessage(Message message);
    }

    [ServiceContract(SessionMode=SessionMode.Required, CallbackContract = typeof(ISimplexSessionRouter))]
    public interface IDuplexSessionRouter
    {
        [OperationContract(IsOneWay = true, Action = "*")]
        void ProcessMessage(Message message);
    }
}
