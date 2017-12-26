' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Messaging
Imports System.Configuration

Namespace Microsoft.ServiceModel.Samples

    Class Program

        Public Shared Sub Main(ByVal args As String())

            ' Create a transaction queue using System.Messaging API
            ' You could also choose to not do this and instead create the
            ' queue using MSMQ MMC -make sure you create a transactional queue
            If Not MessageQueue.Exists(ConfigurationManager.AppSettings("queueName")) Then
                MessageQueue.Create(ConfigurationManager.AppSettings("queueName"), True)
            End If

            'Connect to the queue
            Dim Queue As New MessageQueue(ConfigurationManager.AppSettings("queueName"))

            AddHandler Queue.ReceiveCompleted, AddressOf ProcessOrder
            Queue.BeginReceive()
            Console.WriteLine("Order Service is running")
            Console.WriteLine("Press <ENTER> to terminate service.")
            Console.ReadLine()

        End Sub

        Public Shared Sub ProcessOrder(ByVal source As Object, ByVal asyncResult As ReceiveCompletedEventArgs)

            Try

                ' Connect to the queue.
                Dim Queue As MessageQueue = DirectCast(source, MessageQueue)
                ' End the asynchronous receive operation.
                Dim msg As System.Messaging.Message = Queue.EndReceive(asyncResult.AsyncResult)
                msg.Formatter = New System.Messaging.XmlMessageFormatter(New Type() {GetType(PurchaseOrder)})
                Dim po As PurchaseOrder = DirectCast(msg.Body, PurchaseOrder)
                Dim statusIndexer As New Random()
                po.Status = DirectCast(statusIndexer.[Next](3), OrderStates)
                Console.WriteLine("Processing {0} ", po)
                Queue.BeginReceive()

            Catch ex As System.Exception

                Console.WriteLine(ex.Message)

            End Try

        End Sub

    End Class

End Namespace
