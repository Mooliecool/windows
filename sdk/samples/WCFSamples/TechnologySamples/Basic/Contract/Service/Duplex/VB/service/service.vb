' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    ' Define a duplex service contract.
    ' A duplex contract consists of two interfaces.
    ' The primary interface is used to send messages from the client to the service.
    ' The callback interface is used to send messages from the service back to the client.
    ' ICalculatorDuplex allows one to perform multiple operations on a running result.
    ' The result is sent back after each operation on the ICalculatorCallback interface.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples", SessionMode:=SessionMode.Required, CallbackContract:=GetType(ICalculatorDuplexCallback))> _
    Public Interface ICalculatorDuplex

        <OperationContract(IsOneWay:=True)> _
        Sub Clear()
        <OperationContract(IsOneWay:=True)> _
        Sub AddTo(ByVal n As Double)
        <OperationContract(IsOneWay:=True)> _
        Sub SubtractFrom(ByVal n As Double)
        <OperationContract(IsOneWay:=True)> _
        Sub MultiplyBy(ByVal n As Double)
        <OperationContract(IsOneWay:=True)> _
        Sub DivideBy(ByVal n As Double)
    End Interface

    ' The callback interface is used to send messages from service back to client.
    ' The Result operation will return the current result after each operation.
    ' The Equation opertion will return the complete equation after Clear() is called.
    Public Interface ICalculatorDuplexCallback
        <OperationContract(IsOneWay:=True)> _
        Sub Result(ByVal result As Double)
        <OperationContract(IsOneWay:=True)> _
        Sub Equation(ByVal eqn As String)
    End Interface

    ' Service class which implements a duplex service contract.
    ' Use an InstanceContextMode of PerSession to store the result
    ' An instance of the service will be bound to each duplex session
    <ServiceBehavior(InstanceContextMode:=InstanceContextMode.PerSession)> _
    Public Class CalculatorService
        Implements ICalculatorDuplex

        Private result As Double = 0
        Private equation As String

        Public Sub New()

            equation = result.ToString()

        End Sub

        Public Sub Clear() Implements ICalculatorDuplex.Clear

            Callback.Equation(equation + " = " + result.ToString())
            equation = result.ToString()

        End Sub

        Public Sub AddTo(ByVal n As Double) Implements ICalculatorDuplex.AddTo

            result += n
            equation += " + " + n.ToString()
            Callback.Result(result)

        End Sub

        Public Sub SubtractFrom(ByVal n As Double) Implements ICalculatorDuplex.SubtractFrom

            result -= n
            equation += " - " + n.ToString()
            Callback.Result(result)

        End Sub

        Public Sub MultiplyBy(ByVal n As Double) Implements ICalculatorDuplex.MultiplyBy

            result *= n
            equation += " * " + n.ToString()
            Callback.Result(result)

        End Sub

        Public Sub DivideBy(ByVal n As Double) Implements ICalculatorDuplex.DivideBy

            result /= n
            equation += " / " + n.ToString()
            Callback.Result(result)

        End Sub

        Private ReadOnly Property Callback() As ICalculatorDuplexCallback

            Get

                Return OperationContext.Current.GetCallbackChannel(Of ICalculatorDuplexCallback)()

            End Get

        End Property

    End Class

End Namespace