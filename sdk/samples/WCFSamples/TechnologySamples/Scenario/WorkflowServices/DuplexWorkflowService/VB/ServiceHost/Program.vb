' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ServiceModel
Imports System.Workflow.Runtime
Imports System.ServiceModel.Description

Namespace Microsoft.WorkflowServices.Samples
	Class Program
        Shared Sub Main(ByVal args As String())
            Dim workflowHost As New WorkflowServiceHost(GetType(Microsoft.WorkflowServices.Samples.ServiceWorkflow))

            AddHandler workflowHost.Description.Behaviors.Find(Of WorkflowRuntimeBehavior)().WorkflowRuntime.WorkflowTerminated, AddressOf WorkflowRuntime_WorkflowTerminated
            AddHandler workflowHost.Description.Behaviors.Find(Of WorkflowRuntimeBehavior)().WorkflowRuntime.WorkflowCompleted, AddressOf WorkflowRuntime_WorkflowCompleted
            workflowHost.Open()
            Console.WriteLine("WorkflowServiceHost is ready.")
            Console.ForegroundColor = ConsoleColor.Red
            Console.WriteLine("Press <enter> to exit.")
            Console.ResetColor()
            Console.ReadLine()
            workflowHost.Close()
        End Sub
        Private Shared Sub WorkflowRuntime_WorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
            Console.WriteLine("WorkflowTerminated: " + e.Exception.Message)
        End Sub
        Private Shared Sub WorkflowRuntime_WorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
            Console.WriteLine("WorkflowCompleted: " + e.WorkflowInstance.InstanceId.ToString())
        End Sub
	End Class
End Namespace