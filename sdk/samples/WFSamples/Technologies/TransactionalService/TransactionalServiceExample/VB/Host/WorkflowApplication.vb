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
Imports System.Collections.Generic
Imports System.Globalization
Imports System.Threading
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Hosting
Imports System.Data.SqlClient

Class WorkflowApplication
    Shared waitHandle As New AutoResetEvent(False)
    Const persistenceConnectionString As String = "Initial Catalog=SqlPersistenceService;Data Source=.;Integrated Security=SSPI;"
    Const transactionServiceDataBase As String = "Initial Catalog=TransactionServiceSampleDB;Data Source=.;Integrated Security=SSPI;Enlist=false;"

    Shared Sub Main()
        Dim validInteger As Boolean = False
        Dim transferAmount As Int32 = 0

        'Display account balances before requesting transfer amount
        Dim queryAccounts As QueryAccountService = New QueryAccountService(transactionServiceDataBase)
        Dim accountBalances As Int32() = queryAccounts.QueryAccount(1)
        Console.WriteLine("The account balances for account number {0} are:  Checking : {1:c} , Savings : {2:c}", _
             1, accountBalances(0), accountBalances(1))

        Console.WriteLine("Please enter an amount to transfer from Savings to Checking")
        While Not validInteger
            Try
                transferAmount = Convert.ToInt32(Console.ReadLine(), New NumberFormatInfo())
                If transferAmount < 0 Then
                    Console.WriteLine("Please enter an amount greater than zero.")
                Else
                    validInteger = True
                End If
            Catch
                Console.WriteLine("Please enter a valid amount and try again.")
            End Try
        End While

        ' Initiate the workflow run time
        Using currentWorkflowRuntime As New WorkflowRuntime()

            ' Add the SQL persistence service
            Dim persistenceService As SqlWorkflowPersistenceService = New SqlWorkflowPersistenceService(persistenceConnectionString)
            currentWorkflowRuntime.AddService(persistenceService)

            ' Add the query account service. This will be used to query the account balances
            currentWorkflowRuntime.AddService(queryAccounts)

            ' Add the transational service. This is the service which 
            ' does the work of crediting and debiting the amounts
            ' This service participates in the work batch of the workflow instance
            Dim transactionalService As TransactionService = New TransactionService(transactionServiceDataBase)
            currentWorkflowRuntime.AddService(transactionalService)
            currentWorkflowRuntime.StartRuntime()
            AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated
            AddHandler currentWorkflowRuntime.WorkflowAborted, AddressOf OnWorkflowAborted
            Dim parameters As Dictionary(Of String, Object) = New Dictionary(Of String, Object)
            parameters.Add("TransferAmount", transferAmount)
            ' Initiate the workflow
            Dim type As System.Type = GetType(BalanceTransferWorkflow)
            currentWorkflowRuntime.CreateWorkflow(type, parameters).Start()
            Console.WriteLine("Running the workflow")
            ' Wait for the workflow to finish
            waitHandle.WaitOne()
            currentWorkflowRuntime.StopRuntime()
            Console.WriteLine("Done running The workflow")
        End Using
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        waitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        waitHandle.Set()
    End Sub

    Shared Sub OnWorkflowAborted(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Console.WriteLine("Ending workflow...")
        waitHandle.Set()
    End Sub
End Class

