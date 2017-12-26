' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.EnterpriseServices
Imports System.Runtime.InteropServices

' Enterprise Service attributes used to define the application and component configuration
<Assembly: ApplicationName("ServiceModelHostedSample")> 
<Assembly: ApplicationID("4CDCDB2C-0B19-4534-95CD-FBBFF4D67DD9")> 
<Assembly: ApplicationActivation(ActivationOption.Server)> 
<Assembly: ApplicationAccessControl(False)> 
<Assembly: CLSCompliant(True)> 

Namespace Microsoft.ServiceModel.Samples

    ' Define the component's interface.
    <Guid("8803CCEC-4DFB-49a7-925B-60025C32E5CD")> _
    <ComVisible(True)> _
    Public Interface ICalculator

        Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double
        Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double
        Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double
        Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double

    End Interface

    ' Supporting implementation for the ICalculator interface.
    <Guid("C2B84940-AD54-4a44-B5F7-928130980AB9")> _
    <ProgId("ServiceModelHostedSample.ESCalculator")> _
    <ComVisible(True)> _
    Public Class ESCalculatorService
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