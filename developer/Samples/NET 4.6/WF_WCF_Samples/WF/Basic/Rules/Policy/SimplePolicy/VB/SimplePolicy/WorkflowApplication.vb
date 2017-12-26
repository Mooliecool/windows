'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports System
Imports System.Threading
Imports System.Workflow.Runtime
Imports Microsoft.VisualBasic

Namespace Microsoft.Samples.Workflow.SimplePolicy
    Class WorkflowApplication
        Shared WaitHandle As New AutoResetEvent(False)

        Shared Sub Main()
            ' Start the engine.
            Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
                currentWorkflowRuntime.StartRuntime()

                AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

                ' Load the workflow type.
                Dim type As System.Type = GetType(SimplePolicyWorkflow)
                currentWorkflowRuntime.CreateWorkflow(type).Start()

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
