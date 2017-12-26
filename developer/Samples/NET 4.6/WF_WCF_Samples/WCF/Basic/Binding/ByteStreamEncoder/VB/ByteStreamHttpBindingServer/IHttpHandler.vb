'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System.ServiceModel
Imports System.ServiceModel.Channels

Namespace Microsoft.Samples.ByteStreamEncoder

    <ServiceContract()> _
    Friend Interface IHttpHandler
        <OperationContract(Action:="*", ReplyAction:="*")> _
        Function ProcessRequest(ByVal request As Message) As Message
    End Interface
End Namespace
