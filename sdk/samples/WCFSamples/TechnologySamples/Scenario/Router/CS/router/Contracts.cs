// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------
namespace Microsoft.ServiceModel.Samples
{
    using System;
    using System.ServiceModel.Channels;
    using System.ServiceModel;

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
