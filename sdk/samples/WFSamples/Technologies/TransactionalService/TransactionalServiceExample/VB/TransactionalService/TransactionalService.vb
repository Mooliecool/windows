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
Imports System.Data
Imports System.Data.SqlClient
Imports System.Collections.Specialized
Imports System.Workflow.Runtime

Public Class TransactionService
    Inherits AbstractTransactionService
    Implements IPendingWork

    Private connectionStringValue As String

    Public Sub New(ByVal parameters As NameValueCollection)
        If parameters Is Nothing Then
            Throw New ArgumentNullException("parameters", "The name value collection parameters cannot be Nothing")

        End If
        Dim connectionString As String = parameters("ConnectionString")
        If Not connectionString Is Nothing Then
            init(connectionString)
        Else
            Throw New ArgumentNullException("ConnectionString", "Connection String not found in the name value collection")
        End If
    End Sub

    Public Sub New(ByVal connectionString As String)
        If connectionString Is Nothing Then
            Dim e As ArgumentNullException = New ArgumentNullException("connectionString", "Connection String cannot be nothing")
            Throw e
        End If

        init(connectionString)
    End Sub

    Private Sub init(ByVal connectionString As String)
        connectionStringValue = connectionString

        ' Check if SQL server is available
        Using connection As SqlConnection = New SqlConnection(Me.connectionStringValue)
            connection.Open()
        End Using
    End Sub

    ' This method will be called by the workflow to credit
    ' the amount. This method simply adds the work to be done
    ' to the work batch and returns. Note that there is no actual work of
    ' crediting being done here
    Public Overrides Sub CreditAmount(ByVal amount As Integer)
        Dim request As Request = New Request(OperationType.Credit, amount)

        ' Add the work to the batch and return
        WorkflowEnvironment.WorkBatch.Add(Me, request)
    End Sub

    ' This method will be called by the workflow to debit
    ' the amount. This method simply adds the work to be done
    ' to the work batch and returns. Note that there is no actual work of
    ' debiting being done here
    Public Overrides Sub DebitAmount(ByVal amount As Integer)
        Dim request As Request = New Request(OperationType.Debit, amount)
        WorkflowEnvironment.WorkBatch.Add(Me, request)
    End Sub

    Friend Sub CreditAmountInAccount(ByVal transaction As System.Transactions.Transaction, ByVal amount As Int32)
        Dim command As SqlCommand = New SqlCommand()

        command.CommandType = CommandType.StoredProcedure
        command.CommandText = "dbo.CreditAmount"
        command.Connection = New SqlConnection(connectionStringValue)
        
        Try
            command.Connection.Open()
            command.Connection.EnlistTransaction(transaction)
            Dim parameter As SqlParameter = New SqlParameter()
            parameter.ParameterName = "@Amount"
            parameter.SqlDbType = SqlDbType.Int
            parameter.SqlValue = amount
            parameter.Direction = ParameterDirection.Input
            command.Parameters.Add(parameter)
            command.ExecuteNonQuery()
        Finally
            command.Connection.Close()
        End Try
    End Sub

    Friend Sub DebitAmountFromAccount(ByVal transaction As System.Transactions.Transaction, ByVal amount As Int32)
        Dim command As SqlCommand = New SqlCommand()
        command.CommandType = CommandType.StoredProcedure
        command.CommandText = "dbo.DebitAmount"
        command.Connection = New SqlConnection(connectionStringValue)
        command.Connection.Open()
        command.Connection.EnlistTransaction(transaction)
        Try
            Dim parameter As SqlParameter = New SqlParameter()
            parameter.ParameterName = "@Amount"
            parameter.SqlDbType = SqlDbType.Int
            parameter.SqlValue = amount
            parameter.Direction = ParameterDirection.Input
            command.Parameters.Add(parameter)
            command.ExecuteNonQuery()
        Finally
            If (command IsNot Nothing) AndAlso (command.Connection IsNot Nothing) AndAlso (ConnectionState.Closed <> command.Connection.State) Then
                command.Connection.Close()
            End If
        End Try
    End Sub

    Private Sub Commit(ByVal transaction As System.Transactions.Transaction, ByVal items As ICollection) Implements IPendingWork.Commit
        Dim request As Request
        For Each request In items
            If (request.Operation = OperationType.Credit) Then
                Me.CreditAmountInAccount(transaction, request.Amount)
            End If
            If (request.Operation = OperationType.Debit) Then
                Me.DebitAmountFromAccount(transaction, request.Amount)
            End If
        Next
    End Sub

    Private Sub Complete(ByVal succeeded As Boolean, ByVal items As ICollection) Implements IPendingWork.Complete
    End Sub

    Private Function MustCommit(ByVal items As ICollection) As Boolean Implements IPendingWork.MustCommit
        Return True
    End Function


End Class


Public MustInherit Class AbstractTransactionService
    Protected Sub New()
    End Sub
    Public Overridable Sub CreditAmount(ByVal amount As Int32)
    End Sub

    Public Overridable Sub DebitAmount(ByVal amount As Int32)
    End Sub
End Class

<Serializable()> _
Friend Class Request
    Public ReadOnly Operation As OperationType
    Public ReadOnly Amount As Int32

    Public Sub New()
    End Sub

    Public Sub New(ByVal operation As OperationType, ByVal amount As Int32)
        Me.Operation = operation
        Me.Amount = amount
    End Sub
End Class

Friend Enum OperationType
    Credit = 1
    Debit = 2
End Enum