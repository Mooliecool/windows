' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Drawing
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules


Namespace Microsoft.Samples.Workflow.WorkflowAndActivityDataTrackingExtract
    Partial Public NotInheritable Class BankMachineWorkflow
        Inherits SequentialWorkflowActivity
        Public Sub New()
            InitializeComponent()
        End Sub

        Private depositValue As Double
        Private withdrawalValue As Double
        Private balanceValue As Double
        Private serviceValue As Integer

        Public Property Deposit() As Double
            Get
                Return depositValue
            End Get
            Set(ByVal value As Double)
                depositValue = value
            End Set
        End Property

        Public Property Withdrawal() As Double
            Get
                Return withdrawalValue
            End Get
            Set(ByVal value As Double)
                withdrawalValue = value
            End Set
        End Property

        Public Property Balance() As Double
            Get
                Return balanceValue
            End Get
            Set(ByVal value As Double)
                balanceValue = value
            End Set
        End Property

        Public Property Service() As Integer
            Get
                Return serviceValue
            End Get
            Set(ByVal value As Integer)
                serviceValue = value
            End Set
        End Property

        Private Sub UserInputExecuteCode(ByVal sender As Object, ByVal e As EventArgs)
            Console.WriteLine("Please enter '1' to deposit, '2' to withdraw, or any other number to Exit")
            serviceValue = 0
            Try
                serviceValue = Integer.Parse(System.Console.ReadLine())
            Catch ex As FormatException
                Console.WriteLine(ex.Message)
            End Try

        End Sub

        Private Sub CheckOption(ByVal sender As Object, ByVal e As ConditionalEventArgs)
            e.Result = False
            If serviceValue = 1 OrElse serviceValue = 2 OrElse serviceValue = 0 Then
                e.Result = True
            End If
        End Sub

        Private Sub CheckDeposit(ByVal sender As Object, ByVal e As ConditionalEventArgs)
            ' Go into the deposit activity if user selects 1.
            e.Result = (serviceValue = 1)


        End Sub

        Private Sub DepositExecuteCode(ByVal sender As Object, ByVal e As EventArgs)
            Console.WriteLine("Enter desired deposit amount:")
            Try
                depositValue = Double.Parse(System.Console.ReadLine())
                If depositValue > 0 Then
                    balanceValue += depositValue
                Else
                    Console.WriteLine("Negative deposit amount is not permitted.")
                End If
            Catch generatedExceptionName As FormatException
                Console.WriteLine("Deposit attempt failed because deposit amount entered was not a numeric value.")
            End Try
        End Sub

        Private Sub CheckWithdrawal(ByVal sender As Object, ByVal e As ConditionalEventArgs)
            ' Go into the withdrawal activity if user selects 2.
            e.Result = (serviceValue = 2)

        End Sub

        Private Sub WithdrawalExecuteCode(ByVal sender As Object, ByVal e As EventArgs)
            Console.WriteLine("Enter desired withdrawal amount:")
            Try
                withdrawalValue = Double.Parse(System.Console.ReadLine())
                If balanceValue >= withdrawalValue AndAlso withdrawalValue > 0 Then
                    balanceValue -= withdrawalValue
                Else
                    Console.WriteLine("Withdrawal attempt failed: the withdrawal amount specified was either greater than the account balance or a negative value")
                End If
            Catch generatedExceptionName As FormatException
                Console.WriteLine("Withdrawal attempt failed because withdrawal amount entered was not a numeric value.")
            End Try
        End Sub
    End Class

End Namespace