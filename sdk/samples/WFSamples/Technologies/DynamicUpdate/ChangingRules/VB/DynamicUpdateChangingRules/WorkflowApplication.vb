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
Imports System.Threading
Imports System.Workflow.Runtime
Imports System.Workflow.Activities.Rules
Imports System.Workflow.ComponentModel
Imports System.CodeDom

Class WorkflowApplication
    Shared wasChanged As Boolean = False
    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
            Try
                Dim orderAmount As Int32 = 14000
                Console.WriteLine("Order amount = {0:c}, approved amount = {1:c}", orderAmount, 10000)

                ' Fire up the engine.
                currentWorkflowRuntime.StartRuntime()

                AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler currentWorkflowRuntime.WorkflowIdled, AddressOf OnWorkflowIdled
                AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated
                AddHandler currentWorkflowRuntime.ServicesExceptionNotHandled, AddressOf OnExceptionNotHandled

                ' Load the workflow type.
                Dim type As System.Type = GetType(DynamicRulesWorkflow)
                ' The "Amount" parameter is used to determine which branch of the IfElse should be executed
                ' a value less than 10,000 will execute branch 1 - Get Manager Approval;
                ' any other value will execute branch 2 - Get VP Approval
                Dim parameters As New Dictionary(Of String, Object)
                parameters.Add("Amount", orderAmount)
                Dim workflowInstance As WorkflowInstance = currentWorkflowRuntime.CreateWorkflow(type, parameters)
                workflowInstance.Start()

                WaitHandle.WaitOne()
            Catch ex As Exception
                Console.WriteLine("Exception")
                Console.WriteLine("     Source: {0}", ex.Source)
                Console.WriteLine("     Message: {0}", ex.Message)
            Finally
                currentWorkflowRuntime.StopRuntime()
                Console.WriteLine("Workflow runtime stopped, program exiting...")
            End Try
        End Using
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        Console.WriteLine("Workflow completed.")
        WaitHandle.Set()
    End Sub

    Shared Sub OnExceptionNotHandled(ByVal sender As Object, ByVal e As ServicesExceptionNotHandledEventArgs)
        Console.WriteLine("Unhandled Workflow Exception ")
        Console.WriteLine("  Type: " + e.GetType().ToString())
        Console.WriteLine("  Message: " + e.Exception.Message)
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowIdled(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        If wasChanged Then
            Return
        End If
        wasChanged = True

        Dim workflowInstance As WorkflowInstance = e.WorkflowInstance

        Dim NewAmount As Int32 = 15000
        Console.WriteLine("Dynamically change approved amount to {0:c}", NewAmount)

        ' Dynamic update of order rule
        Dim workflowchanges As WorkflowChanges = New WorkflowChanges(workflowInstance.GetWorkflowDefinition)

        Dim transient As CompositeActivity = workflowchanges.TransientWorkflow
        Dim ruleDefinitions As RuleDefinitions = CType(transient.GetValue(ruleDefinitions.RuleDefinitionsProperty), RuleDefinitions)
        Dim conditions As RuleConditionCollection = ruleDefinitions.Conditions
        Dim condition1 As RuleExpressionCondition = CType(conditions("Check"), RuleExpressionCondition)
        CType(condition1.Expression, CodeBinaryOperatorExpression).Right = New CodePrimitiveExpression(NewAmount)

        workflowInstance.ApplyWorkflowChanges(workflowchanges)
    End Sub
End Class


