
'  Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System

Imports System.ServiceModel.Channels
Imports System.ServiceModel
Imports System.Diagnostics

Namespace Microsoft.ServiceModel.Samples

    ' Style (DataContractFormat, XmlSerializerFormat): 
    ' This property determines how the WSDL metadata for the service is formatted.
    ' Possible values are OperationFormatStyle.Document and OperationFormatStyle.Rpc.

    <ServiceContract(Namespace:="http://Microsoft.ServiceModel.Samples"), DataContractFormat(Style := OperationFormatStyle.Document)> _
    Public Interface ICalculator
        <OperationContract()> _
        Function Add(ByVal request As RequestMessage) As ResponseMessage
        <OperationContract()> _
        Function Subtract(ByVal request As RequestMessage) As ResponseMessage
        <OperationContract()> _
        Function Multiply(ByVal request As RequestMessage) As ResponseMessage
        <OperationContract()> _
        Function Divide(ByVal request As RequestMessage) As ResponseMessage
    End Interface
     
    <MessageContract(IsWrapped := false)> _
    Public Class RequestMessage
        <MessageBodyMember()> _
        Private n1 As Double
        <MessageBodyMember()> _
        Private n2 As Double
        Public Property N1Value() As Double
            Get
                Return n1
            End Get
            Set(ByVal value As Double)
                n1 = value
            End Set
        End Property
        Public Property N2Value() As Double
            Get
                Return n2
            End Get
            Set(ByVal value As Double)
                n2 = value
            End Set
        End Property
    End Class

    <MessageContract(IsWrapped := false)> _
    Public Class ResponseMessage
        <MessageBodyMember()> _
        Private vResult As Double
        Public Property Result() As Double
            Get
                Return vResult
            End Get
            Set(ByVal value As Double)
                vResult = value
            End Set
        End Property

    End Class

    Public Class CalculatorService
        Implements ICalculator

        ' Create a static TraceSource to the configured trace listeners
         Dim ts As TraceSource = New TraceSource("CalculatorServiceTraceSource")


        Public Function Add(ByVal request As RequestMessage) As ResponseMessage _
 Implements ICalculator.Add
            Dim n1 As Double = request.N1Value
            Dim n2 As Double = request.N2Value
            Dim vResult As Double = n1 + n2

            ts.TraceInformation("Received Add({0},{1})", n1, n2)
            ts.TraceInformation("Return: {0}", vResult)
            ts.Flush()

            Dim response As ResponseMessage = New ResponseMessage()
            response.Result = vResult
            Return response

        End Function

        Public Function Subtract(ByVal request As RequestMessage) As ResponseMessage _
 Implements ICalculator.Subtract
            Dim n1 As Double = request.N1Value
            Dim n2 As Double = request.N2Value
            Dim vResult As Double = n1 - n2

            ts.TraceInformation("Received Subtract({0},{1})", n1, n2)
            ts.TraceInformation("Return: {0}", vResult)
            ts.Flush()

            Dim response As ResponseMessage = New ResponseMessage()
            response.Result = vResult
            Return response
        End Function

        Public Function Multiply(ByVal request As RequestMessage) As ResponseMessage _
 Implements ICalculator.Multiply

            Dim n1 As Double = request.N1Value
            Dim n2 As Double = request.N2Value
            Dim vResult As Double = n1 * n2

            ts.TraceInformation("Received Multiply({0},{1})", n1, n2)
            ts.TraceInformation("Return: {0}", vResult)
            ts.Flush()

            Dim response As ResponseMessage = New ResponseMessage()
            response.Result = vResult
            Return response
        End Function

        Public Function Divide(ByVal request As RequestMessage) As ResponseMessage _
 Implements ICalculator.Divide
            Dim n1 As Double = request.N1Value
            Dim n2 As Double = request.N2Value
            Dim vResult As Double = n1 / n2

            ts.TraceInformation("Received Divide({0},{1})", n1, n2)
            ts.TraceInformation("Return: {0}", vResult)
            ts.Flush()

            Dim response As ResponseMessage = New ResponseMessage()
            response.Result = vResult
            Return response
        End Function
    End Class

End Namespace
