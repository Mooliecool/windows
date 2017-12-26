' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.ServiceModel
Imports System.ServiceModel.Security.Tokens
Imports System.IdentityModel
Imports System.IdentityModel.Claims
Imports System.IdentityModel.Policy

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface ISecureCalculator

        <OperationContract()> _
        Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function GetIdentity() As String

    End Interface

    ' Service class which implements the service contract.
    Public Class CalculatorService
        Implements ISecureCalculator

        Public Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ISecureCalculator.Add

            Return n1 + n2

        End Function

        Public Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ISecureCalculator.Subtract

            Return n1 - n2

        End Function

        Public Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ISecureCalculator.Multiply

            Return n1 * n2

        End Function

        Public Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ISecureCalculator.Divide

            Return n1 / n2

        End Function

        Public Function GetIdentity() As String Implements ISecureCalculator.GetIdentity

            Dim identity As String = [String].Empty

            Dim ctx As AuthorizationContext = OperationContext.Current.ServiceSecurityContext.AuthorizationContext

            For Each claimSet As ClaimSet In ctx.ClaimSets

                For Each claim As Claim In claimSet

                    identity += "[" + TryCast(claim.Resource.ToString(), String) + "] "
                Next

            Next
            Return identity

        End Function

    End Class

End Namespace