' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Configuration
Imports System.Messaging
Imports System.ServiceModel
Imports System.Transactions

Namespace Microsoft.ServiceModel.Samples

    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Create a client with
            Dim r As New Random(137)
            Dim client As New StockTickerClient()
            Dim price As Single = 43.23F

            For i As Integer = 0 To 9

                Dim increment As Single = 0.01F * (r.[Next](10))
                client.StockTick("zzz" & i, price + increment)

            Next

            'Closing the client gracefully cleans up resources
            client.Close()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace
