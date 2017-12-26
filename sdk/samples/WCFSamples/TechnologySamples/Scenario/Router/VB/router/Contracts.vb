' Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.

Imports System
Imports System.ServiceModel.Channels
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    <ServiceContract(SessionMode:=SessionMode.Allowed)> _
    Public Interface ISimplexDatagramRouter

        <OperationContract(IsOneWay:=True, Action:="*")> _
        Sub ProcessMessage(ByVal message As Message)

    End Interface

    <ServiceContract(SessionMode:=SessionMode.Allowed)> _
    Public Interface IRequestReplyDatagramRouter

        <OperationContract(IsOneWay:=False, Action:="*", ReplyAction:="*")> _
        Function ProcessMessage(ByVal message As Message) As Message

    End Interface

    <ServiceContract(SessionMode:=SessionMode.Required)> _
    Public Interface ISimplexSessionRouter

        <OperationContract(IsOneWay:=True, Action:="*")> _
        Sub ProcessMessage(ByVal message As Message)

    End Interface

    <ServiceContract(SessionMode:=SessionMode.Required, CallbackContract:=GetType(ISimplexSessionRouter))> _
    Public Interface IDuplexSessionRouter

        <OperationContract(IsOneWay:=True, Action:="*")> _
        Sub ProcessMessage(ByVal message As Message)

    End Interface

End Namespace
