' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Globalization
Imports System.ServiceModel.Channels
Imports System.IO
Imports System.ServiceModel
Imports System.Xml

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
        <OperationContract()> _
        Function Sum(ByVal request As Message) As Message

    End Interface

    ' Service class which implements the service contract.
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

        'The Sum operation operates on the WCF Message object directly
        Public Function Sum(ByVal request As Message) As Message Implements ICalculator.Sum

            Dim s As Integer = 0
            Dim text As String = ""

            'The body of the message contains a list of numbers which will be read directly using an XmlReader
            Dim body As XmlReader = request.GetReaderAtBodyContents()

            While body.Read()

                text = body.ReadString().Trim()
                If text.Length > 0 Then

                    s += Convert.ToInt32(text, CultureInfo.InvariantCulture)

                End If

            End While
            body.Close()

            Dim response As Message = Message.CreateMessage(request.Version, "http://Microsoft.ServiceModel.Samples/ICalculator/SumResponse", s)
            Return response

        End Function

    End Class

End Namespace