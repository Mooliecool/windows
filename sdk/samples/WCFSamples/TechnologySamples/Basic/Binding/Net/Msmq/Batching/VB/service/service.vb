' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Channels
Imports System.Configuration
Imports System.Messaging
Imports System.ServiceModel
Imports System.ServiceModel.Description
Imports System.Transactions
Imports System.Runtime.Serialization
Imports System.Collections.Generic
Imports System.Data
Imports System.Data.SqlClient
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

        Shared OrderStates As String() = {"Pending", "Processed", "Shipped"}
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
                Return tc

            End Get

        End Property

        Public ReadOnly Property Status() As String

            Get

                Return OrderStates(statusIndexer.[Next](3))

            End Get

        End Property

        Public Overloads Overrides Function ToString() As String

            Dim strbuf As New System.Text.StringBuilder("Purchase Order: " & PONumber & vbNewLine)
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
    Public Class Orders

        Public Shared Sub Add(ByVal po As PurchaseOrder)

            ' insert purchase order
            Dim insertPurchaseOrderCommand As New SqlCommand("insert into PurchaseOrders(poNumber, customerId) values(@poNumber, @customerId)")
            insertPurchaseOrderCommand.Parameters.Add("@poNumber", SqlDbType.VarChar, 50)
            insertPurchaseOrderCommand.Parameters.Add("@customerId", SqlDbType.VarChar, 50)

            ' insert product line item
            Dim insertProductLineItemCommand As New SqlCommand("insert into ProductLineItems(productId, unitCost, quantity, poNumber) values(@productId, @unitCost, @quantity, @poNumber)")
            insertProductLineItemCommand.Parameters.Add("@productId", SqlDbType.VarChar, 50)
            insertProductLineItemCommand.Parameters.Add("@unitCost", SqlDbType.Float)
            insertProductLineItemCommand.Parameters.Add("@quantity", SqlDbType.Int)
            insertProductLineItemCommand.Parameters.Add("@poNumber", SqlDbType.VarChar, 50)

            Dim rowsAffected As Integer = 0

            Using scope As New TransactionScope(TransactionScopeOption.Required)

                Using conn As New SqlConnection(ConfigurationManager.AppSettings("connectionString"))

                    conn.Open()

                    ' insert into purchase order table
                    insertPurchaseOrderCommand.Connection = conn
                    insertPurchaseOrderCommand.Parameters("@poNumber").Value = po.PONumber
                    insertPurchaseOrderCommand.Parameters("@customerId").Value = po.CustomerId
                    insertPurchaseOrderCommand.ExecuteNonQuery()

                    ' insert into product line item table
                    insertProductLineItemCommand.Connection = conn
                    For Each orderLineItem As PurchaseOrderLineItem In po.orderLineItems

                        insertProductLineItemCommand.Parameters("@poNumber").Value = po.PONumber
                        insertProductLineItemCommand.Parameters("@productId").Value = orderLineItem.ProductId
                        insertProductLineItemCommand.Parameters("@unitCost").Value = orderLineItem.UnitCost
                        insertProductLineItemCommand.Parameters("@quantity").Value = orderLineItem.Quantity
                        rowsAffected += insertProductLineItemCommand.ExecuteNonQuery()

                    Next
                    scope.Complete()

                End Using

            End Using
            Console.WriteLine("Updated database with {0} product line items for purchase order {1} ", rowsAffected, po.PONumber)

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
    <ServiceBehavior(ReleaseServiceInstanceOnTransactionComplete:=False, TransactionIsolationLevel:=System.Transactions.IsolationLevel.Serializable, ConcurrencyMode:=ConcurrencyMode.Multiple)> _
    Public Class OrderProcessorService
        Implements IOrderProcessor

        <OperationBehavior(TransactionScopeRequired:=True, TransactionAutoComplete:=True)> _
        Public Sub SubmitPurchaseOrder(ByVal po As PurchaseOrder) Implements IOrderProcessor.SubmitPurchaseOrder

            Console.WriteLine("Processing {0} ", po)
            Orders.Add(po)

        End Sub

        ' Host the service within this EXE console application.
        Public Shared Sub Main()

            ' Get MSMQ queue name from app settings in configuration
            Dim queueName As String = ConfigurationManager.AppSettings("queueName")

            ' Create the transacted MSMQ queue if necessary.
            If Not MessageQueue.Exists(queueName) Then
                MessageQueue.Create(queueName, True)
            End If

            ' Get the base address that is used to listen for WS-MetaDataExchange requests
            ' This is useful to generate a proxy for the client
            Dim baseAddress As String = ConfigurationManager.AppSettings("baseAddress")

            ' Create a ServiceHost for the OrderProcessorService type.
            Dim serviceHost As New ServiceHost(GetType(OrderProcessorService))
            AddHandler serviceHost.Faulted, AddressOf OnServiceFaulted

            '  ServiceThrottlingBehavior throttle = new ServiceThrottlingBehavior();
            '            throttle.MaxConcurrentCalls = 5;
            '            serviceHost.Description.Behaviors.Add(throttle);
            ' Hook on to the service host faulted events
            ' Open the ServiceHostBase to create listeners and start listening for messages.
            serviceHost.Open()

            ' The service can now be accessed.
            Console.WriteLine("The service is ready.")
            Console.WriteLine("Press <ENTER> to terminate service.")
            Console.WriteLine()
            Console.ReadLine()

            Try

                serviceHost.Close()

            Catch generatedExceptionName As CommunicationObjectFaultedException

                Console.WriteLine(" Service cannot be closed...it already faulted")

            End Try

        End Sub

        Public Shared Sub OnServiceFaulted(ByVal sender As Object, ByVal e As EventArgs)

            Console.WriteLine("Service Faulted ")

        End Sub

    End Class

End Namespace
