' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Channels
Imports System.Configuration
Imports System.ServiceModel
Imports System.ServiceModel.Dispatcher
Imports System.ServiceModel.Description
Imports System.Transactions
Imports System.Runtime.Serialization
Imports System.Collections.Generic
Imports System.Text
Imports System.Threading
Imports Microsoft.VisualBasic

Namespace Microsoft.ServiceModel.Samples

    ' Define the Purchase Order Line Item
    <DataContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Class PurchaseOrderLineItem

        <DataMember()> _
        Public ProductId As String

        <DataMember()> _
        Public UnitCost As Single

        <DataMember()> _
        Public Quantity As Integer

        Public Overloads Overrides Function ToString() As String

            Dim displayString As String = "Order LineItem: " & Quantity & " of " & ProductId & " @unit price: $" & UnitCost & vbNewLine
            Return displayString

        End Function

        Public ReadOnly Property TotalCost() As Single

            Get

                Return UnitCost * Quantity

            End Get

        End Property

    End Class

    ' Define Purchase Order
    <DataContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Class PurchaseOrder

        Shared ReadOnly OrderStates As String() = {"Pending", "Processed", "Shipped"}
        Shared statusIndexer As New Random(137)

        <DataMember()> _
        Public PONumber As String

        <DataMember()> _
        Public CustomerId As String

        <DataMember()> _
        Public orderLineItems As PurchaseOrderLineItem()

        Public ReadOnly Property TotalCost() As Single

            Get

                Dim tc As Single = 0
                For Each lineItem As PurchaseOrderLineItem In orderLineItems
                    tc += lineItem.TotalCost
                Next

                Return TotalCost

            End Get

        End Property

        Public ReadOnly Property Status() As String

            Get

                Return OrderStates(statusIndexer.[Next](3))

            End Get

        End Property

        Public Overloads Overrides Function ToString() As String

            Dim strbuf As New StringBuilder("Purchase Order: " & PONumber & vbNewLine)
            strbuf.Append(vbTab & "Customer: " & CustomerId & vbNewLine)
            strbuf.Append(vbTab & "OrderDetails" & vbNewLine)

            For Each lineItem As PurchaseOrderLineItem In orderLineItems

                strbuf.Append(vbTab & vbTab & lineItem.ToString())

            Next

            strbuf.Append(vbTab & "Total cost of this order: $" & TotalCost & vbNewLine)
            strbuf.Append(vbTab & "Order status: " & Status.ToString() & vbNewLine)
            Return strbuf.ToString()

        End Function

    End Class

    ' Order Processing Logic
    ' Can replace with transaction-aware resource such as SQL, transacted hashtable to hold orders
    ' This example uses a non-transactional resource
    Public Class Orders

        Shared purchaseOrders As New Dictionary(Of String, PurchaseOrder)()

        Public Shared Sub Add(ByVal po As PurchaseOrder)

            purchaseOrders.Add(po.PONumber, po)

        End Sub

        Public Shared Function GetOrderStatus(ByVal poNumber As String) As String

            Dim po As PurchaseOrder = Nothing
            If purchaseOrders.TryGetValue(poNumber, po) Then
                Return po.Status
            Else
                Return Nothing
            End If

        End Function

        Public Shared Sub DeleteOrder(ByVal poNumber As String)

            If purchaseOrders(poNumber) IsNot Nothing Then
                purchaseOrders.Remove(poNumber)
            End If

        End Sub

    End Class

    Public Class PoisonErrorHandler
        Implements IErrorHandler

        Shared orderProcessingCallback As New WaitCallback(AddressOf OrderProcessorService.StartThreadProc)

        Public Sub ProvideFault(ByVal [error] As Exception, ByVal version As MessageVersion, ByRef fault As Message) Implements IErrorHandler.ProvideFault

            ' no-op -we are not interested in this.

        End Sub

        ' Handle poison message exception by moving the offending message out of the way for regular processing to go on
        Public Function HandleError(ByVal [error] As Exception) As Boolean Implements IErrorHandler.HandleError

            Dim poisonException As MsmqPoisonMessageException = TryCast([error], MsmqPoisonMessageException)
            If poisonException IsNot Nothing Then

                Dim lookupId As Long = poisonException.MessageLookupId
                Console.WriteLine(" Poisoned message -message look up id = {0}", lookupId)

                ' Get MSMQ queue name from app settings in configuration

                Dim orderQueue As New System.Messaging.MessageQueue(OrderProcessorService.QueueName)
                Dim poisonMessageQueue As New System.Messaging.MessageQueue(OrderProcessorService.PoisonQueueName)
                Dim message As System.Messaging.Message = Nothing

                ' use a new transaction scope to remove the message from the main app queue and add it to the poison queue
                ' the poison message service will process messages from the poison queue
                Using txScope As New TransactionScope(TransactionScopeOption.RequiresNew)

                    Dim retryCount As Integer = 0
                    While retryCount < 3

                        retryCount += 1
                        Try

                            ' look up the poison message using the look up id
                            message = orderQueue.ReceiveByLookupId(lookupId)
                            If message IsNot Nothing Then

                                ' send the message to the poison message queue
                                poisonMessageQueue.Send(message, System.Messaging.MessageQueueTransactionType.Automatic)
                                ' complete transaction scope
                                txScope.Complete()
                                Console.WriteLine("Moved poisoned message with look up id: {0} to poison queue: {1} ", lookupId, OrderProcessorService.PoisonQueueName)
                                Exit Try

                            End If

                        Catch generatedExceptionName As InvalidOperationException

                            'code for the case when the message may still not be available in the queue because of a race in transaction or 
                            'another node in the farm may actually have taken the message
                            If retryCount < 3 Then

                                Console.WriteLine("Trying to move poison message but message is not available, sleeping for 10 seconds before retrying")
                                Thread.Sleep(TimeSpan.FromSeconds(10))

                            Else

                                Console.WriteLine("Giving up on trying to move the message")

                            End If

                        End Try

                    End While

                End Using

                ' restart service host
                Console.WriteLine()
                Console.WriteLine("Restarting the service to process rest of the messages in the queue")
                Console.WriteLine("Press <ENTER> to stop the service")
                ThreadPool.QueueUserWorkItem(orderProcessingCallback)
                Return True

            End If

            Return False

        End Function

    End Class

    Public NotInheritable Class PoisonErrorBehaviorAttribute
        Inherits Attribute
        Implements IServiceBehavior

        Private poisonErrorHandler As PoisonErrorHandler

        Public Sub New()

            Me.poisonErrorHandler = New PoisonErrorHandler()

        End Sub

        Private Sub Validate(ByVal description As ServiceDescription, ByVal serviceHostBase As ServiceHostBase) Implements IServiceBehavior.Validate

        End Sub

        Private Sub AddBindingParameters(ByVal description As ServiceDescription, ByVal serviceHostBase As ServiceHostBase, ByVal endpoints As System.Collections.ObjectModel.Collection(Of ServiceEndpoint), ByVal parameters As BindingParameterCollection) Implements IServiceBehavior.AddBindingParameters

        End Sub

        Private Sub ApplyDispatchBehavior(ByVal description As ServiceDescription, ByVal serviceHostBase As ServiceHostBase) Implements IServiceBehavior.ApplyDispatchBehavior

            For Each channelDispatcherBase As ChannelDispatcherBase In serviceHostBase.ChannelDispatchers

                Dim channelDispatcher As ChannelDispatcher = TryCast(channelDispatcherBase, ChannelDispatcher)
                channelDispatcher.ErrorHandlers.Add(poisonErrorHandler)

            Next

        End Sub

    End Class

    ' Define a service contract. 
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IOrderProcessor

        <OperationContract(IsOneWay:=True)> _
        Sub SubmitPurchaseOrder(ByVal po As PurchaseOrder)

    End Interface

    ' Service class which implements the service contract.
    ' Added code to write output to the console window
    <PoisonErrorBehavior()> _
    Public Class OrderProcessorService
        Implements IOrderProcessor

        Shared r As New Random(137)
        Public Shared QueueName As String
        Public Shared PoisonQueueName As String

        <OperationBehavior(TransactionScopeRequired:=True, TransactionAutoComplete:=True)> _
        Public Sub SubmitPurchaseOrder(ByVal po As PurchaseOrder) Implements IOrderProcessor.SubmitPurchaseOrder

            Dim randomNumber As Integer = r.[Next](10)

            If randomNumber Mod 2 = 0 Then

                Orders.Add(po)
                Console.WriteLine("Processing {0} ", po)

            Else

                Console.WriteLine("Aborting transaction, cannot process purchase order: " + po.PONumber)
                Console.WriteLine()
                Throw New Exception("Cannot process purchase order: " + po.PONumber)

            End If

        End Sub

        Public Shared Sub StartThreadProc(ByVal stateInfo As Object)

            StartService()

        End Sub

        Public Shared Sub StartService()

            ' Get MSMQ queue name from app settings in configuration
            QueueName = ConfigurationManager.AppSettings("queueName")

            ' Get MSMQ queue name for the final poison queue
            PoisonQueueName = ConfigurationManager.AppSettings("poisonQueueName")

            ' Create the transacted MSMQ queue if necessary.
            If Not System.Messaging.MessageQueue.Exists(QueueName) Then
                System.Messaging.MessageQueue.Create(QueueName, True)
            End If

            ' Create the transacted poison message MSMQ queue if necessary.
            If Not System.Messaging.MessageQueue.Exists(PoisonQueueName) Then
                System.Messaging.MessageQueue.Create(PoisonQueueName, True)
            End If

            ' Create a ServiceHost for the OrderProcessorService type.
            Dim serviceHost As New ServiceHost(GetType(OrderProcessorService))

            ' Hook on to the service host faulted events
            AddHandler serviceHost.Faulted, AddressOf OnServiceFaulted

            ' Open the ServiceHostBase to create listeners and start listening for messages.
            serviceHost.Open()

        End Sub

        Public Shared Sub OnServiceFaulted(ByVal sender As Object, ByVal e As EventArgs)

            Console.WriteLine("Service Faulted")

        End Sub

        ' Host the service within this EXE console application.
        Public Shared Sub Main()

            OrderProcessorService.StartService()

            ' The service can now be accessed.
            Console.WriteLine("The service is ready.")
            Console.WriteLine("Press <ENTER> to stop the service")
            Console.ReadLine()
            Console.WriteLine("Exiting service")

        End Sub

    End Class

End Namespace
