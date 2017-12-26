' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel
Imports System.Threading

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples", SessionMode:=SessionMode.Required)> _
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

    ' Define a service contract to inspect instance state
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples", SessionMode:=SessionMode.Required)> _
    Public Interface ICalculatorInstance
        Inherits ICalculator

        <OperationContract()> _
        Function GetInstanceContextMode() As String
        <OperationContract()> _
        Function GetInstanceId() As Integer
        <OperationContract()> _
        Function GetOperationCount() As Integer

    End Interface

    ' Enable one of the following instance modes to compare instancing behaviors.

    ' PerCall creates a new instance for each operation
    '<ServiceBehavior(InstanceContextMode := InstanceContextMode.PerCall)> _

    ' Singleton creates a single instance for application lifetime
    '<ServiceBehavior(InstanceContextMode := InstanceContextMode.Single)> _

    ' PerSession creates an instance per channel session
    ' This requires a binding that supports session
    <ServiceBehavior(InstanceContextMode := InstanceContextMode.PerSession)> _
    Public Class CalculatorService
        Implements ICalculatorInstance

        Shared syncObject As New Object()
        Shared instanceCount As Integer
        Private instanceId As Integer
        Private operationCount As Integer

        Public Sub New()

            SyncLock syncObject

                instanceCount += 1
                instanceId = instanceCount

            End SyncLock

        End Sub

        Public Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculatorInstance.Add

            operationCount += 1
            Return n1 + n2

        End Function

        Public Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculatorInstance.Subtract

            Interlocked.Increment(operationCount)
            Return n1 - n2

        End Function

        Public Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculatorInstance.Multiply

            Interlocked.Increment(operationCount)
            Return n1 * n2

        End Function

        Public Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculatorInstance.Divide

            Interlocked.Increment(operationCount)
            Return n1 / n2

        End Function

        Public Function GetInstanceContextMode() As String Implements ICalculatorInstance.GetInstanceContextMode

            ' Return the InstanceContextMode of the service
            Dim host As ServiceHost = DirectCast(OperationContext.Current.Host, ServiceHost)
            Dim behavior As ServiceBehaviorAttribute = host.Description.Behaviors.Find(Of ServiceBehaviorAttribute)()
            Return behavior.InstanceContextMode.ToString()

        End Function

        Public Function GetInstanceId() As Integer Implements ICalculatorInstance.GetInstanceId

            ' Return the id for this instance
            Return instanceId

        End Function

        Public Function GetOperationCount() As Integer Implements ICalculatorInstance.GetOperationCount

            ' Return the number of ICalculator operations performed on this instance
            SyncLock syncObject

                Return operationCount

            End SyncLock

        End Function

    End Class

End Namespace
