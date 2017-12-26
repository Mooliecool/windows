' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Description
Imports System.ServiceModel.Channels
Imports System.ServiceModel
Imports System.Messaging
Imports System.Configuration

Namespace Microsoft.ServiceModel.Samples

    Public Class Program

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

                'Open the ServiceHost to create listeners and start listening for messages.
                serviceHost.Open()

                ' The service can now be accessed.
                Console.WriteLine("The service is ready.")
                Console.WriteLine("Press <ENTER> to terminate service.")
                Console.ReadLine()

            End Using

        End Sub

    End Class

End Namespace
