' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Channels
Imports System.Configuration
Imports System.Messaging
Imports System.ServiceModel
Imports System.Transactions

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract. 
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IStockTicker

        <OperationContract(IsOneWay:=True)> _
        Sub StockTick(ByVal symbol As String, ByVal price As Single)

    End Interface

    ' Service class which implements the service contract.
    ' Added code to write output to the console window
    Public Class StockTickerService
        Implements IStockTicker

        Public Sub StockTick(ByVal symbol As String, ByVal price As Single) _
            Implements IStockTicker.StockTick

            Console.WriteLine("Stock Tick {0}:{1} ", symbol, price)

        End Sub

        ' Host the service within this EXE console application.
        Public Shared Sub Main()

            ' Get MSMQ queue name from app settings in configuration
            Dim queueName As String = ConfigurationManager.AppSettings("queueName")

            ' Create the transacted MSMQ queue if necessary.
            If Not MessageQueue.Exists(queueName) Then
                MessageQueue.Create(queueName)
            End If

            ' Create a ServiceHost for the StockTickerService type.
            Using serviceHost As New ServiceHost(GetType(StockTickerService))

                ' Open the ServiceHost to create listeners and start listening for messages.
                serviceHost.Open()

                ' The service can now be accessed.
                Console.WriteLine("The service is ready.")
                Console.WriteLine("Press <ENTER> to terminate service.")
                Console.WriteLine()
                Console.ReadLine()

            End Using

        End Sub

    End Class

End Namespace