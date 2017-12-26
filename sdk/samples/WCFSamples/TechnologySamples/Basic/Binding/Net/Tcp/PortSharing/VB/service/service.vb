' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface ICalculator

        <OperationContract()> _
        Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double

    End Interface

    ' Service class that implements the service contract
    Public Class CalculatorService
        Implements ICalculator

        Public Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Add

            Return n1 + n2

        End Function

        Public Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Subtract

            Return n1 - n2

        End Function

        Public Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Multiply

            Return n1 * n2

        End Function

        Public Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Divide

            Return n1 / n2

        End Function

    End Class

    Class service

        Public Shared Sub Main(ByVal args As String())

            ' Configure a binding with TCP port sharing enabled
            Dim binding As New NetTcpBinding()
            binding.PortSharingEnabled = True

            ' Start a service on a fixed TCP port
            Dim host As New ServiceHost(GetType(CalculatorService))
            Dim salt As UShort = CUShort(New Random().Next(1, 65535))
            Dim address As String = [String].Format("net.tcp://localhost:9000/calculator/{0}", salt)
            host.AddServiceEndpoint(GetType(ICalculator), binding, address)
            host.Open()
            Console.WriteLine("Service #{0} listening on {1}.", salt, address)
            Console.WriteLine("Press <ENTER> to terminate service.")
            Console.ReadLine()
            host.Close()

        End Sub

    End Class

End Namespace