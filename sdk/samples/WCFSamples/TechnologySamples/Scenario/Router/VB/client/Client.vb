' Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    Class Client

        Public Shared Sub Main(ByVal args As String())

            Dim echoClient As New EchoServiceClient()

            Console.WriteLine("Echo(""Is anyone there?"") returned: " + echoClient.Echo("Is anyone there?"))

            echoClient.Close()

            Dim calculatorClient As New CalculatorServiceClient()

            Console.WriteLine("Add(5) returned: " & calculatorClient.Add(5))
            Console.WriteLine("Add(-3) returned: " & calculatorClient.Add(-3))

            calculatorClient.Close()

            Console.WriteLine()
            Console.WriteLine("Press Enter to exit...")
            Console.ReadLine()

        End Sub

    End Class

End Namespace
