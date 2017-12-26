' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.EnterpriseServices
Imports System.Runtime.InteropServices

' Enterprise Service attributes used to define the application and component configuration
<Assembly: ApplicationName("ServiceModelSample")> 
<Assembly: ApplicationID("E146E066-D3D1-4e0e-B175-30160BD368DE")> 
<Assembly: ApplicationActivation(ActivationOption.Library)> 
<Assembly: ApplicationAccessControl(True)> 

Namespace Microsoft.ServiceModel.Samples

    ' Define the component's interface.
    <Guid("C551FBA9-E3AA-4272-8C2A-84BD8D290AC7")> _
    <ComVisible(True)> _
    Public Interface ICalculator

        Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double
        Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double
        Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double
        Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double

    End Interface

    ' Supporting implementation for the ICalculator interface.
    <Guid("BE62FF5B-8B53-476b-A385-0F66043049F6")> _
    <ComVisible(True)> _
    <ProgId("ServiceModelSample.ESCalculator")> _
    Public Class esCalculatorService
        Inherits ServicedComponent
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

    End Class

End Namespace