' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Create a client
            Dim client As New CalculatorInstanceClient()
            Dim instanceMode As String = client.GetInstanceContextMode()
            Console.WriteLine("InstanceContextMode: {0}", instanceMode)
            DoCalculations(client)

            ' Create a second client
            Dim client2 As New CalculatorInstanceClient()

            DoCalculations(client2)

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

        Private Shared Sub DoCalculations(ByVal client As CalculatorInstanceClient)

            ' Call the Add service operation.
            Dim value1 As Double = 100
            Dim value2 As Double = 15.99
            Dim result As Double = client.Add(value1, value2)
            Console.WriteLine("Add({0},{1}) = {2}", value1, value2, result)
            Console.Write("InstanceId: {0}", client.GetInstanceId())
            Console.WriteLine(" , OperationCount: {0}", client.GetOperationCount())

            ' Call the Subtract service operation.
            value1 = 145
            value2 = 76.54
            result = client.Subtract(value1, value2)
            Console.WriteLine("Subtract({0},{1}) = {2}", value1, value2, result)
            Console.Write("InstanceId: {0}", client.GetInstanceId())
            Console.WriteLine(" , OperationCount: {0}", client.GetOperationCount())

            ' Call the Multiply service operation.
            value1 = 9
            value2 = 81.25
            result = client.Multiply(value1, value2)
            Console.WriteLine("Multiply({0},{1}) = {2}", value1, value2, result)
            Console.Write("InstanceId: {0}", client.GetInstanceId())
            Console.WriteLine(" , OperationCount: {0}", client.GetOperationCount())

            ' Call the Divide service operation.
            value1 = 22
            value2 = 7
            result = client.Divide(value1, value2)
            Console.WriteLine("Divide({0},{1}) = {2}", value1, value2, result)
            Console.Write("InstanceId: {0}", client.GetInstanceId())
            Console.WriteLine(" , OperationCount: {0}", client.GetOperationCount())

        End Sub

    End Class

End Namespace
