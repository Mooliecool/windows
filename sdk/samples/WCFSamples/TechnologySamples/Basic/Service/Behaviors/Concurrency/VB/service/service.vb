' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
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

    ' Define a service contract to inspect concurrency state
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface ICalculatorConcurrency
        Inherits ICalculator

        <OperationContract()> _
        Function GetConcurrencyMode() As String
        <OperationContract()> _
        Function GetOperationCount() As Integer

    End Interface

    ' Concurrency controls whether a service instance can process more than one message at a time.
    ' PerCall instancing creates a service instance per message, so concurrency is not relevant.
    ' Single instancing create a single service instance for all messages.
    ' PerSession instancing creates a service instance per session.
    ' Enable one of the following concurrency modes to compare behavior.

    ' Single allows a single message to be processed sequentially by each service instance.
    '[ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Single, InstanceContextMode = InstanceContextMode.Single)]

    ' Multiple allows concurrent processing of multiple messages by a service instance.
    ' The service implementation should be thread-safe. This can be used to increase throughput.

    ' Uses Thread.Sleep to vary the execution time of each operation
    <ServiceBehavior(ConcurrencyMode:=ConcurrencyMode.Multiple, InstanceContextMode:=InstanceContextMode.[Single])> _
    Public Class CalculatorService
        Implements ICalculatorConcurrency

        Private operationCount As Integer

        Public Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculatorConcurrency.Add

            operationCount += 1
            System.Threading.Thread.Sleep(180)
            Return n1 + n2

        End Function

        Public Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculatorConcurrency.Subtract

            operationCount += 1
            System.Threading.Thread.Sleep(100)
            Return n1 - n2

        End Function

        Public Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculatorConcurrency.Multiply

            operationCount += 1
            System.Threading.Thread.Sleep(150)
            Return n1 * n2

        End Function

        Public Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculatorConcurrency.Divide

            operationCount += 1
            System.Threading.Thread.Sleep(120)
            Return n1 / n2

        End Function

        Public Function GetConcurrencyMode() As String Implements ICalculatorConcurrency.GetConcurrencyMode

            ' Return the ConcurrencyMode of the service
            Dim host As ServiceHost = DirectCast(OperationContext.Current.Host, ServiceHost)
            Dim behavior As ServiceBehaviorAttribute = host.Description.Behaviors.Find(Of ServiceBehaviorAttribute)()
            Return behavior.ConcurrencyMode.ToString()

        End Function

        Public Function GetOperationCount() As Integer Implements ICalculatorConcurrency.GetOperationCount

            ' Return number of operations
            Return operationCount

        End Function

    End Class

End Namespace
