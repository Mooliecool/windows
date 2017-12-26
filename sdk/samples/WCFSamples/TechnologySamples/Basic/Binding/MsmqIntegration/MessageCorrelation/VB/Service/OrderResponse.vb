' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.ServiceModel.MsmqIntegration
Imports Microsoft.ServiceModel.Samples

Namespace Microsoft.ServiceModel.Samples

    <System.ServiceModel.ServiceContractAttribute([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IOrderResponse

        <System.ServiceModel.OperationContractAttribute(IsOneWay:=True, Action:="*")> _
        Sub SendOrderResponse(ByVal msg As MsmqMessage(Of PurchaseOrder))

    End Interface

    Partial Public Class OrderResponseClient
        Inherits System.ServiceModel.ClientBase(Of IOrderResponse)
        Implements IOrderResponse

        Public Sub New()

        End Sub

        Public Sub New(ByVal configurationName As String)

            MyBase.New(configurationName)

        End Sub

        Public Sub New(ByVal binding As System.ServiceModel.Channels.Binding, ByVal address As System.ServiceModel.EndpointAddress)

            MyBase.New(binding, address)

        End Sub

        Public Sub SendOrderResponse(ByVal msg As MsmqMessage(Of PurchaseOrder)) Implements IOrderResponse.SendOrderResponse

            MyBase.Channel.SendOrderResponse(msg)

        End Sub

    End Class

End Namespace
