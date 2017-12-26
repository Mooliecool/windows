'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Collections.Generic
Imports System.Drawing
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

Public Class BalanceTransferWorkflow
    Inherits SequentialWorkflowActivity

    Private accountBalancesValue As IList(Of Int32) = New Int32(1) {0, 0}
    Private accountNumberValue As Int32 = 1
    Private workflowExceptionValue As Exception = New System.Exception

    Public Property AccountBalances() As IList(Of Int32)
        Get
            Return accountBalancesValue
        End Get
        Set(ByVal value As IList(Of Int32))
            accountBalancesValue = value
        End Set
    End Property

    Public Property AccountNumber() As Int32
        Get
            Return accountNumberValue
        End Get
        Set(ByVal value As Int32)
            accountNumberValue = value
        End Set
    End Property

    Public Property WorkflowException() As Exception
        Get
            Return workflowExceptionValue
        End Get
        Set(ByVal value As Exception)
            workflowExceptionValue = value
        End Set
    End Property

    Private Sub onBeforeTransactionInquiry(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("Inquiring account number {0} to find the balance before the transaction", AccountNumber)
    End Sub

    Private Sub onDisplayAccountBalances(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("The Account Balances for account number {0} are Checking : {1:c} , Savings {2:c}", AccountNumber, AccountBalances(0), AccountBalances(1))
    End Sub

    Private Sub onBeforeCreditAmount(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("Crediting the Checking amount in account number {0}", AccountNumber)
        'Me.TransferAmount = CType((Me.Parameters("TransferAmount").Value), Int32)
        Me.CreditAmount1.Amount = TransferAmount
    End Sub

    Private Sub onBeforeDebitAmount(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("Debiting the Savings amount in account number {0}", AccountNumber)
        'Me.TransferAmount = CType((Me.Parameters("TransferAmount").Value), Int32)
        Me.DebitAmount1.Amount = TransferAmount
    End Sub

    Private Sub onAfterTransactionInquiry(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("Inquiring account number {0} to find the balance after the transaction", AccountNumber)
    End Sub

    Private Sub onExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        WorkflowException = exceptionHandler1.Fault
        Console.WriteLine("The transaction failed due to an exception.  Aborting the transaction ....")
        Console.WriteLine(WorkflowException.Message)
    End Sub

    Private transferAmountValue As Integer
    Property TransferAmount() As Integer
        Get
            Return transferAmountValue
        End Get
        Set(ByVal value As Integer)
            transferAmountValue = value
        End Set
    End Property
End Class
