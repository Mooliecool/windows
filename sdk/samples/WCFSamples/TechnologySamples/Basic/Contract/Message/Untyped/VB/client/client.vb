' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.Xml
Imports System.Text

Namespace Microsoft.ServiceModel.Samples

    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Create a client
            Dim client As New CalculatorClient()
            Dim RequestAction As String = "http://test/Message_RequestAction"

            Using New OperationContextScope(client.InnerChannel)

                ' Call the Sum service operation.
                Dim values As Integer() = {1, 2, 3, 4, 5}
                Dim request As Message = Message.CreateMessage(OperationContext.Current.OutgoingMessageHeaders.MessageVersion, RequestAction, values)
                Dim reply As Message = client.ComputeSum(request)
                Dim response As Integer = reply.GetBody(Of Integer)()

                Console.WriteLine("Sum of numbers passed (1,2,3,4,5) = {0}", response)

            End Using

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace
