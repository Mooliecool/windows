' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel
Imports System.ServiceModel.Channels

Namespace Microsoft.Samples.WSFederationHttpBinding

    <ServiceContract()> _
    Interface IWSTrust

        <OperationContract(Action:=Constants.Trust.Actions.Issue, ReplyAction:=Constants.Trust.Actions.IssueReply)> _
        Function Issue(ByVal request As Message) As Message

    End Interface

End Namespace

