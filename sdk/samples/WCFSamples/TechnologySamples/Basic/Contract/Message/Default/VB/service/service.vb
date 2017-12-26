' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Channels
Imports System.ServiceModel
Imports System.Runtime.Serialization

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface ICalculator

        <OperationContract(Action:="http://test/MyMessage_action", ReplyAction:="http://test/MyMessage_action")> _
        Function Calculate(ByVal request As MyMessage) As MyMessage

    End Interface

    ' Custom message.
    <MessageContract()> _
    Public Class MyMessage

        Private m_operation As String
        Private m_n1 As Double
        Private m_n2 As Double
        Private m_result As Double

        'Constructor - create an empty message.
        Public Sub New()
        End Sub

        'Constructor - create a message and populate its members.
        Public Sub New(ByVal n1 As Double, ByVal n2 As Double, ByVal operation As String, ByVal result As Double)

            Me.m_n1 = n1
            Me.m_n2 = n2
            Me.m_operation = operation
            Me.m_result = result

        End Sub

        'Constructor - create a message from another message.
        Public Sub New(ByVal message As MyMessage)

            Me.m_n1 = message.N1
            Me.m_n2 = message.N2
            Me.m_operation = message.Operation
            Me.m_result = message.Result

        End Sub

        <MessageHeader()> _
        Public Property Operation() As String

            Get

                Return m_operation

            End Get

            Set(ByVal value As String)

                m_operation = value

            End Set

        End Property

        <MessageBodyMember()> _
        Public Property N1() As Double

            Get

                Return m_n1

            End Get

            Set(ByVal value As Double)

                m_n1 = value

            End Set

        End Property

        <MessageBodyMember()> _
        Public Property N2() As Double

            Get

                Return m_n2

            End Get

            Set(ByVal value As Double)

                m_n2 = value

            End Set

        End Property

        <MessageBodyMember()> _
        Public Property Result() As Double

            Get

                Return m_result

            End Get

            Set(ByVal value As Double)

                m_result = value

            End Set

        End Property

    End Class

    ' Service class which implements the service contract.
    Public Class CalculatorService
        Implements ICalculator

        ' Perform a calculation.
        Public Function Calculate(ByVal request As MyMessage) As MyMessage Implements ICalculator.Calculate

            Dim response As New MyMessage(request)
            Select Case request.Operation
                Case "+"
                    response.Result = request.N1 + request.N2
                    Exit Select
                Case "-"
                    response.Result = request.N1 - request.N2
                    Exit Select
                Case "*"
                    response.Result = request.N1 * request.N2
                    Exit Select
                Case "/"
                    response.Result = request.N1 / request.N2
                    Exit Select
                Case Else
                    response.Result = 0
                    Exit Select
            End Select
            Return response

        End Function

    End Class

End Namespace
