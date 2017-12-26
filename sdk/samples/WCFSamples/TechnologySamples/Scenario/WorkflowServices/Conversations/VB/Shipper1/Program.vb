' Copyright (c) Microsoft Corporation. All rights reserved.

#Region "Using directives"

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Threading
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Hosting
Imports System.ServiceModel
Imports System.ServiceModel.Description


#End Region

Namespace Microsoft.WorkflowServices.Samples
	Class Program
        Shared Sub Main(ByVal args As String())
            Dim host As New WorkflowServiceHost(GetType(ShipperWorkflow))
            AddHandler host.Description.Behaviors.Find(Of WorkflowRuntimeBehavior)().WorkflowRuntime.WorkflowTerminated, AddressOf WorkflowRuntime_WorkflowTerminated
            AddHandler host.Description.Behaviors.Find(Of WorkflowRuntimeBehavior)().WorkflowRuntime.WorkflowCompleted, AddressOf WorkflowRuntime_WorkflowCompleted
            host.Open()

            Console.ForegroundColor = ConsoleColor.White
            Console.WriteLine("Press <enter> to exit")
            Console.ResetColor()
            Console.ReadLine()
            host.Close()
        End Sub
        Private Shared Sub WorkflowRuntime_WorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
            Console.WriteLine("WorkflowTerminated: " + e.Exception.Message)
        End Sub
        Private Shared Sub WorkflowRuntime_WorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
            Console.WriteLine("WorkflowCompleted.")
        End Sub
	End Class
End Namespace