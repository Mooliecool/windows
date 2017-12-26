' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel
Imports System.Diagnostics

Namespace Microsoft.ServiceModel.Samples

    ' Style (OperationFormatStyle): 
    ' This property determines how the WSDL metadata for the service is formatted.
    ' Possible values are Document and Rpc.

    ' This version of the service contract uses Use.Literal and OperationFormatStyle.Rpc
    ' on a DataContractFormat attribute..
    ' [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples"),
    ' DataContractFormat(Style = OperationFormatStyle.Rpc)]

    ' This version of the service contract uses Use.Encoded and OperationFormatStyle.Rpc
    ' on a XmlSerializerFormat attribute.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples"), XmlSerializerFormat(Style:=OperationFormatStyle.Rpc, Use:=OperationFormatUse.Encoded)> _
    Public Interface IUseAndStyleCalculator

        <OperationContract()> _
        Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double

    End Interface

    Public Class CalculatorService
        Implements IUseAndStyleCalculator

        ' Create a static TraceSource to the configured trace listeners
        Shared ts As New TraceSource("CalculatorServiceTraceSource")

        Public Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double Implements IUseAndStyleCalculator.Add

            Dim result As Double = n1 + n2

            ts.TraceInformation("Received Add({0},{1})", n1, n2)
            ts.TraceInformation("Return: {0}", result)
            ts.Flush()

            Return result

        End Function

        Public Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double Implements IUseAndStyleCalculator.Subtract

            Dim result As Double = n1 - n2

            ts.TraceInformation("Received Subtract({0},{1})", n1, n2)
            ts.TraceInformation("Return: {0}", result)
            ts.Flush()

            Return result

        End Function

        Public Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double Implements IUseAndStyleCalculator.Multiply

            Dim result As Double = n1 * n2

            ts.TraceInformation("Received Multiply({0},{1})", n1, n2)
            ts.TraceInformation("Return: {0}", result)
            ts.Flush()

            Return result

        End Function

        Public Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double Implements IUseAndStyleCalculator.Divide

            Dim result As Double = n1 / n2

            ts.TraceInformation("Received Divide({0},{1})", n1, n2)
            ts.TraceInformation("Return: {0}", result)
            ts.Flush()

            Return result

        End Function

    End Class

End Namespace