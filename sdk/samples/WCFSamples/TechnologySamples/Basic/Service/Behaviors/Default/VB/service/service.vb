' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples", SessionMode:=SessionMode.Required)> _
    Public Interface ICalculator

        <OperationContract()> _
        Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double

    End Interface

    <ServiceBehavior(AutomaticSessionShutdown:=True, ConcurrencyMode:=ConcurrencyMode.[Single], InstanceContextMode:=InstanceContextMode.PerSession, IncludeExceptionDetailInFaults:=False, UseSynchronizationContext:=True, ValidateMustUnderstand:=True)> _
    Public Class CalculatorService
        Implements ICalculator

        <OperationBehavior(TransactionAutoComplete:=True, TransactionScopeRequired:=False, Impersonation:=ImpersonationOption.NotAllowed)> _
        Public Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Add

            System.Threading.Thread.Sleep(1600)
            Return n1 + n2

        End Function

        <OperationBehavior(TransactionAutoComplete:=True, TransactionScopeRequired:=False, Impersonation:=ImpersonationOption.NotAllowed)> _
        Public Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Subtract

            System.Threading.Thread.Sleep(800)
            Return n1 - n2

        End Function

        <OperationBehavior(TransactionAutoComplete:=True, TransactionScopeRequired:=False, Impersonation:=ImpersonationOption.NotAllowed)> _
        Public Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Multiply

            System.Threading.Thread.Sleep(400)
            Return n1 * n2

        End Function

        <OperationBehavior(TransactionAutoComplete:=True, TransactionScopeRequired:=False, Impersonation:=ImpersonationOption.NotAllowed)> _
        Public Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Divide

            System.Threading.Thread.Sleep(100)
            Return n1 / n2

        End Function

    End Class

End Namespace
