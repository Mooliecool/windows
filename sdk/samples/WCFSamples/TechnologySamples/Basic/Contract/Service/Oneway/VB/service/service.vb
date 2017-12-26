' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Configuration
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract. 
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IOneWayCalculator

        <OperationContract(IsOneWay:=True)> _
        Sub Add(ByVal n1 As Double, ByVal n2 As Double)
        <OperationContract(IsOneWay:=True)> _
        Sub Subtract(ByVal n1 As Double, ByVal n2 As Double)
        <OperationContract(IsOneWay:=True)> _
        Sub Multiply(ByVal n1 As Double, ByVal n2 As Double)
        <OperationContract(IsOneWay:=True)> _
        Sub Divide(ByVal n1 As Double, ByVal n2 As Double)

    End Interface

    ' Service class which implements the service contract.
    ' Added code to write output to the console window
    <ServiceBehavior(ConcurrencyMode:=ConcurrencyMode.Multiple, InstanceContextMode:=InstanceContextMode.PerCall)> _
    Public Class CalculatorService
        Implements IOneWayCalculator

        Public Sub Add(ByVal n1 As Double, ByVal n2 As Double) Implements IOneWayCalculator.Add

            Console.WriteLine("Received Add({0},{1}) - sleeping", n1, n2)
            System.Threading.Thread.Sleep(1000 * 5)
            Dim result As Double = n1 + n2
            Console.WriteLine("Processing Add({0},{1}) - result: {2}", n1, n2, result)

        End Sub

        Public Sub Subtract(ByVal n1 As Double, ByVal n2 As Double) Implements IOneWayCalculator.Subtract

            Console.WriteLine("Received Subtract({0},{1}) - sleeping", n1, n2)
            System.Threading.Thread.Sleep(1000 * 5)
            Dim result As Double = n1 - n2
            Console.WriteLine("Processing Subtract({0},{1}) - result: {2}", n1, n2, result)

        End Sub

        Public Sub Multiply(ByVal n1 As Double, ByVal n2 As Double) Implements IOneWayCalculator.Multiply

            Console.WriteLine("Received Multiply({0},{1}) - sleeping", n1, n2)
            System.Threading.Thread.Sleep(1000 * 5)
            Dim result As Double = n1 * n2
            Console.WriteLine("Processing Multiply({0},{1}) - result: {2}", n1, n2, result)

        End Sub

        Public Sub Divide(ByVal n1 As Double, ByVal n2 As Double) Implements IOneWayCalculator.Divide

            Console.WriteLine("Received Divide({0},{1}) - sleeping", n1, n2)
            System.Threading.Thread.Sleep(1000 * 5)
            Dim result As Double = n1 / n2
            Console.WriteLine("Processing Divide({0},{1}) - result: {2}", n1, n2, result)

        End Sub

        ' Host the service within this EXE console application.
        Public Shared Sub Main()

            ' Create a ServiceHost for the CalculatorService type and provide the base address.
            Using serviceHost As New ServiceHost(GetType(CalculatorService))

                ' Open the ServiceHostBase to create listeners and start listening for messages.
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