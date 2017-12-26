' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Channels
Imports System.ServiceModel
Imports System.Runtime.Serialization
Imports System.Xml
Imports System.Globalization

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface ICalculator

        <OperationContract(Action:=CalculatorService.RequestAction, ReplyAction:=CalculatorService.ReplyAction)> _
        Function ComputeSum(ByVal request As Message) As Message

    End Interface

    ' Service class which implements the service contract.
    Public Class CalculatorService
        Implements ICalculator

        ' Perform a calculation.
        Public Const ReplyAction As String = "http://test/Message_ReplyAction"
        Public Const RequestAction As String = "http://test/Message_RequestAction"

        Public Function ComputeSum(ByVal request As Message) As Message Implements ICalculator.ComputeSum

            'The body of the message contains a list of numbers which will be read as a int[] using GetBody<T>
            Dim result As Integer = 0

            Dim inputs As Integer() = request.GetBody(Of Integer())()
            For Each i As Integer In inputs

                result += i

            Next

            Dim response As Message = Message.CreateMessage(request.Version, ReplyAction, result)
            Return response

        End Function

    End Class

End Namespace
