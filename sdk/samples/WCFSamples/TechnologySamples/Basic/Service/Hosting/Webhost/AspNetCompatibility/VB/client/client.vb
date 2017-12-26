' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Create a client
            Dim client As New CalculatorSessionClient()

            client.Clear()
            client.AddTo(100)
            client.SubtractFrom(50)
            client.MultiplyBy(17.65)
            client.DivideBy(2)
            Dim result As Double = client.Result()
            Console.WriteLine("0, + 100, - 50, * 17.65, / 2 = {0}", result)

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace
