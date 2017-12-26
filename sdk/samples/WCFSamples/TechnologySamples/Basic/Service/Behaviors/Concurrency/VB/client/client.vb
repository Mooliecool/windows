' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples
    ' The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    ' Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Create a client
            Dim client As New CalculatorConcurrencyClient()

            Console.WriteLine("Press <ENTER> to terminate client once the output is displayed.")
            Console.WriteLine()

            ' Communicate with service using asynchronous methods.
            client.BeginGetConcurrencyMode(AddressOf GetConcurrencyModeCallback, client)

            ' BeginAdd
            Dim value1 As Double = 100
            Dim value2 As Double = 15.99
            Dim arAdd As IAsyncResult = client.BeginAdd(value1, value2, AddressOf AddCallback, client)
            Console.WriteLine("Add({0},{1})", value1, value2)

            ' BeginSubtract
            value1 = 145
            value2 = 76.54
            Dim arSubtract As IAsyncResult = client.BeginSubtract(value1, value2, AddressOf SubtractCallback, client)
            Console.WriteLine("Subtract({0},{1})", value1, value2)

            ' BeginMultiply
            value1 = 9
            value2 = 81.25
            Dim arMultiply As IAsyncResult = client.BeginMultiply(value1, value2, AddressOf MultiplyCallback, client)
            Console.WriteLine("Multiply({0},{1})", value1, value2)

            ' BeginDivide
            value1 = 22
            value2 = 7
            Dim arDivide As IAsyncResult = client.BeginDivide(value1, value2, AddressOf DivideCallback, client)
            Console.WriteLine("Divide({0},{1})", value1, value2)

            client.BeginGetOperationCount(AddressOf GetOperationCountCallback, client)

            Console.ReadLine()

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

        End Sub

        ' Asynchronous callbacks for displaying results.
        Private Shared Sub GetConcurrencyModeCallback(ByVal ar As IAsyncResult)

            Dim result As String = (DirectCast(ar.AsyncState, CalculatorConcurrencyClient)).EndGetConcurrencyMode(ar)
            Console.WriteLine("ConcurrencyMode : {0}", result)

        End Sub

        Private Shared Sub GetOperationCountCallback(ByVal ar As IAsyncResult)

            Dim result As Integer = (DirectCast(ar.AsyncState, CalculatorConcurrencyClient)).EndGetOperationCount(ar)
            Console.WriteLine("OperationCount : {0}", result)

        End Sub

        Private Shared Sub AddCallback(ByVal ar As IAsyncResult)

            Dim result As Double = (DirectCast(ar.AsyncState, CalculatorConcurrencyClient)).EndAdd(ar)
            Console.WriteLine("Add Result : " & result)

        End Sub

        Private Shared Sub SubtractCallback(ByVal ar As IAsyncResult)

            Dim result As Double = (DirectCast(ar.AsyncState, CalculatorConcurrencyClient)).EndSubtract(ar)
            Console.WriteLine("Subtract Result : " & result)

        End Sub

        Private Shared Sub MultiplyCallback(ByVal ar As IAsyncResult)

            Dim result As Double = (DirectCast(ar.AsyncState, CalculatorConcurrencyClient)).EndMultiply(ar)
            Console.WriteLine("Multiply Result : " & result)

        End Sub

        Private Shared Sub DivideCallback(ByVal ar As IAsyncResult)

            Dim result As Double = (DirectCast(ar.AsyncState, CalculatorConcurrencyClient)).EndDivide(ar)
            Console.WriteLine("Divide Result : " & result)

        End Sub

    End Class

End Namespace