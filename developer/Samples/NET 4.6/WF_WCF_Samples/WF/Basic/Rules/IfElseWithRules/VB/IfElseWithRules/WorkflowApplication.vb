'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------
Imports System
Imports System.Collections.Generic
Imports System.Threading
Imports System.Workflow.Runtime
Imports Microsoft.VisualBasic

Namespace Microsoft.Samples.Rules.IfElseWithRules
    Class WorkflowApplication
        Shared WaitHandle As New AutoResetEvent(False)

        Shared Sub Main()
            ' Start the engine.
            Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
                currentWorkflowRuntime.StartRuntime()

                AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

                ' Load the workflow type.
                Dim type As System.Type = GetType(IfElseWorkflow)

                ' The "OrderValueParameter" parameter is used to determine which branch of the IfElse should be executed.
                ' A value less than 10,000 will execute branch 1 - Get Manager Approval; any other value will execute branch 2 - Get VP Approval

                Dim parameters As Dictionary(Of String, Object) = New Dictionary(Of String, Object)
                parameters.Add("OrderValue", 14000)
                Dim workflow As WorkflowInstance = currentWorkflowRuntime.CreateWorkflow(type, parameters)
                workflow.Start()

                WaitHandle.WaitOne()

                currentWorkflowRuntime.StopRuntime()
            End Using
        End Sub

        Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
            Console.WriteLine(vbCrLf + "Workflow completed")
            WaitHandle.Set()
        End Sub

        Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
            Console.WriteLine(e.Exception.Message)
            WaitHandle.Set()
        End Sub
    End Class
End Namespace

