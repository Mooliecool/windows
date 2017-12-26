' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.ServiceModel.MsmqIntegration
Imports System.Runtime.Serialization
Imports System.Transactions

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract. 
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    <ServiceKnownType(GetType(PurchaseOrder))> _
    Public Interface IOrderProcessor

        <OperationContract(IsOneWay:=True, Action:="*")> _
        Sub SubmitPurchaseOrder(ByVal msg As MsmqMessage(Of PurchaseOrder))

    End Interface
    ' Service class which implements the service contract.
    ' Added code to write output to the console window
    Public Class OrderProcessorService
        Implements IOrderProcessor

        <OperationBehavior(TransactionScopeRequired:=True, TransactionAutoComplete:=True)> _
        Public Sub SubmitPurchaseOrder(ByVal ordermsg As MsmqMessage(Of PurchaseOrder)) Implements IOrderProcessor.SubmitPurchaseOrder

            Dim po As PurchaseOrder = DirectCast(ordermsg.Body, PurchaseOrder)
            Dim statusIndexer As New Random()
            po.Status = DirectCast(statusIndexer.[Next](3), OrderStates)
            Console.WriteLine("Processing {0} ", po)
            'Send a response to the client that the order has been received and is pending fullfillment 
            SendResponse(ordermsg)

        End Sub

        Private Sub SendResponse(ByVal ordermsg As MsmqMessage(Of PurchaseOrder))

            Dim client As New OrderResponseClient("OrderResponseEndpoint")

            'Set the correlation ID such that the client can correlate the response to the order
            Dim orderResponsemsg As New MsmqMessage(Of PurchaseOrder)(ordermsg.Body)
            orderResponsemsg.CorrelationId = ordermsg.Id
            Using scope As New TransactionScope(TransactionScopeOption.Required)

                client.SendOrderResponse(orderResponsemsg)
                scope.Complete()

            End Using
            client.Close()

        End Sub

    End Class

End Namespace
