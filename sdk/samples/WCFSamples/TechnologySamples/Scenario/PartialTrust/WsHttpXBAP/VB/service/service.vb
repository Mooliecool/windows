' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ServiceModel
Imports System.Runtime.Serialization
Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.Servicemodel.Samples")> _
    Public Interface ICalcPlus
        <OperationContract()> _
        Function Add(ByVal n1 As Double, ByVal n2 As Double) As CalcResult

        <OperationContract()> _
        Function Divide(ByVal n1 As Double, ByVal n2 As Double) As CalcResult

        <OperationContract()> _
        Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As CalcResult

        <OperationContract()> _
        Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As CalcResult
    End Interface

    <DataContract([Namespace]:="http://Microsoft.Servicemodel.Samples")> _
    Public Class CalcResult
        <DataMember()> _
        Public ReadOnly Number As Double

        <DataMember()> _
        Public ReadOnly Words As String

        ' The constructor is not reconstituted on the client
        ' in the proxy, but it is useful on the service.
        Public Sub New(ByVal number As Double)
            Me.Number = number
            Me.Words = NumberUtilities.NumberConverter.ConvertNumberToWords(number)
        End Sub
    End Class

    ' Service class that implements the service contract.
    Public Class CalcPlusService
        Implements ICalcPlus
        Public Function Add(ByVal n1 As Double, ByVal n2 As Double) As CalcResult Implements ICalcPlus.Add
            Return New CalcResult(n1 + n2)
        End Function

        Public Function Divide(ByVal n1 As Double, ByVal n2 As Double) As CalcResult Implements ICalcPlus.Divide
            Return New CalcResult(n1 / n2)
        End Function

        Public Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As CalcResult Implements ICalcPlus.Multiply
            Return New CalcResult(n1 * n2)
        End Function

        Public Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As CalcResult Implements ICalcPlus.Subtract
            Return New CalcResult(n1 - n2)
        End Function
    End Class

End Namespace
