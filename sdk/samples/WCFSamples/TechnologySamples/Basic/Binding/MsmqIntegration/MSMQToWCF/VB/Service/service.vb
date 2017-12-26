' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.ServiceModel.MsmqIntegration
Imports System.Runtime.Serialization
Imports System.Configuration
Imports System.Messaging

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

            Dim po As PurchaseOrder = ordermsg.Body
            Dim statusIndexer As New Random()
            po.Status = DirectCast(statusIndexer.[Next](3), OrderStates)
            Console.WriteLine("Processing {0} ", po)

        End Sub

        ' Host the service within this EXE console application.
        Public Shared Sub Main()

            ' Get MSMQ queue name from app settings in configuration
            Dim queueName As String = ConfigurationManager.AppSettings("orderQueueName")

            ' Create the transacted MSMQ queue if necessary.
            If Not MessageQueue.Exists(queueName) Then
                MessageQueue.Create(queueName, True)
            End If

            ' Create a ServiceHost for the OrderProcessorService type.
            Using serviceHost As New ServiceHost(GetType(OrderProcessorService))

                serviceHost.Open()

                ' The service can now be accessed.
                Console.WriteLine("The service is ready.")
                Console.WriteLine("Press <ENTER> to terminate service.")
                Console.ReadLine()

            End Using

        End Sub

    End Class

End Namespace
