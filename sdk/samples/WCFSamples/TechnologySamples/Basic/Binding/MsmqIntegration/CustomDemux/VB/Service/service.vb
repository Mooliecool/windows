' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Dispatcher
Imports System.ServiceModel.Description
Imports System.ServiceModel
Imports System.ServiceModel.MsmqIntegration
Imports System.Collections.Generic
Imports System.ServiceModel.Channels
Imports System.Runtime.Serialization
Imports System.Messaging
Imports System.Threading
Imports System.Configuration

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract()> _
    <ServiceKnownType(GetType(PurchaseOrder))> _
    <ServiceKnownType(GetType(String))> _
    Public Interface IOrderProcessor

        <OperationContract(IsOneWay:=True, Name:="SubmitPurchaseOrder")> _
        Sub SubmitPurchaseOrder(ByVal msg As MsmqMessage(Of PurchaseOrder))
        <OperationContract(IsOneWay:=True, Name:="CancelPurchaseOrder")> _
        Sub CancelPurchaseOrder(ByVal ponumber As MsmqMessage(Of String))

    End Interface

    Public Class MatchAllFilterBehavior
        Implements IEndpointBehavior

#Region "IEndpointBehavior Members"
        Public Sub Validate(ByVal serviceEndpoint As ServiceEndpoint) Implements IEndpointBehavior.Validate

        End Sub

        Public Sub ApplyClientBehavior(ByVal serviceEndpoint As ServiceEndpoint, ByVal proxy As ClientRuntime) Implements IEndpointBehavior.ApplyClientBehavior

        End Sub

        Public Sub AddBindingParameters(ByVal serviceEndpoint As ServiceEndpoint, ByVal parameters As BindingParameterCollection) Implements IEndpointBehavior.AddBindingParameters

        End Sub

        Public Sub ApplyDispatchBehavior(ByVal serviceEndpoint As ServiceEndpoint, ByVal endpointDispatcher As EndpointDispatcher) Implements IEndpointBehavior.ApplyDispatchBehavior

            endpointDispatcher.ContractFilter = New MatchAllMessageFilter()

        End Sub
#End Region

    End Class

    Public Class OperationSelector
        Implements IDispatchOperationSelector

        Public Function SelectOperation(ByRef message As System.ServiceModel.Channels.Message) As String Implements IDispatchOperationSelector.SelectOperation

            Dim [property] As MsmqIntegrationMessageProperty = MsmqIntegrationMessageProperty.[Get](message)
            Return [property].Label

        End Function

    End Class

    ' Service class which implements the service contract.
    ' Added code to write output to the console window
    Public Class OrderProcessorService
        Implements IOrderProcessor
        Implements IContractBehavior

        <OperationBehavior(TransactionScopeRequired:=True, TransactionAutoComplete:=True)> _
        Public Sub SubmitPurchaseOrder(ByVal msg As MsmqMessage(Of PurchaseOrder)) Implements IOrderProcessor.SubmitPurchaseOrder

            Dim po As PurchaseOrder = DirectCast(msg.Body, PurchaseOrder)
            Dim statusIndexer As New Random()
            po.Status = DirectCast(statusIndexer.[Next](3), OrderStates)
            Console.WriteLine("Processing {0} ", po)

        End Sub

        <OperationBehavior(TransactionScopeRequired:=True, TransactionAutoComplete:=True)> _
        Public Sub CancelPurchaseOrder(ByVal msg As MsmqMessage(Of String)) Implements IOrderProcessor.CancelPurchaseOrder

            Dim ponumber As String = DirectCast(msg.Body, String)
            Console.WriteLine("Purchase Order {0} is cancelled ", ponumber)

        End Sub

        ' Host the service within this EXE console application.
        <STAThread()> _
        Public Shared Sub Main()

            ' Get MSMQ queue name from app settings in configuration
            Dim queueName As String = ConfigurationManager.AppSettings("orderQueueName")

            ' Create the transacted MSMQ queue if necessary.
            If Not MessageQueue.Exists(queueName) Then
                MessageQueue.Create(queueName, True)
            End If

            ' Create a ServiceHost for the CalculatorService type.
            Using serviceHost As New ServiceHost(GetType(OrderProcessorService))

                Dim endpoint As ServiceEndpoint = serviceHost.Description.Endpoints(0)
                endpoint.Behaviors.Add(New MatchAllFilterBehavior())

                'Open the ServiceHost to create listeners and start listening for messages.
                serviceHost.Open()

                ' The service can now be accessed.
                Console.WriteLine("The service is ready.")
                Console.WriteLine("Press <ENTER> to terminate service.")
                Console.ReadLine()

            End Using

        End Sub

        Private Sub Validate(ByVal description As ContractDescription, ByVal endpoint As ServiceEndpoint) Implements IContractBehavior.Validate

        End Sub

        Private Sub ApplyDispatchBehavior(ByVal description As ContractDescription, ByVal endpoint As ServiceEndpoint, ByVal dispatch As DispatchRuntime) Implements IContractBehavior.ApplyDispatchBehavior

            dispatch.OperationSelector = New OperationSelector()

        End Sub

        Private Sub AddBindingParameters(ByVal description As ContractDescription, ByVal endpoint As ServiceEndpoint, ByVal parameters As BindingParameterCollection) Implements IContractBehavior.AddBindingParameters

        End Sub

        Private Sub ApplyClientBehavior(ByVal description As ContractDescription, ByVal endpoint As ServiceEndpoint, ByVal proxy As ClientRuntime) Implements IContractBehavior.ApplyClientBehavior

        End Sub

    End Class

End Namespace
