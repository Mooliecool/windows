' Copyright (c) Microsoft Corporation.  All Rights Reserved.
Imports System
Imports System.Web.Services

Namespace Microsoft.ServiceModel.Samples

    ''' <summary>
    ''' Simple ASMX Calculator Web Service.	
    ''' </summary>
    <WebService([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Class CalculatorService
        Inherits System.Web.Services.WebService

        <WebMethod()> _
        Public Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double

            Return n1 + n2

        End Function

        <WebMethod()> _
        Public Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double

            Return n1 - n2

        End Function

        <WebMethod()> _
        Public Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double

            Return n1 * n2

        End Function

        <WebMethod()> _
        Public Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double

            Return n1 / n2

        End Function

    End Class

End Namespace
