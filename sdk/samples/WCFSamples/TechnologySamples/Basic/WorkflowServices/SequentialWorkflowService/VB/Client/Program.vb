' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ServiceModel
Imports System.Threading
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Hosting

Namespace Microsoft.WorkflowServices.Samples
    Class Program
    Shared waitHandle As AutoResetEvent

        Shared Sub Main(ByVal args As String())
            Using workflowRuntime As New WorkflowRuntime()
                ' Add ChannelManager
                Dim channelmgr As New ChannelManagerService()
                workflowRuntime.AddService(channelmgr)

                waitHandle = New AutoResetEvent(False)
                AddHandler workflowRuntime.WorkflowCompleted, AddressOf workflowRuntime_WorkflowCompleted
                AddHandler workflowRuntime.WorkflowTerminated, AddressOf workflowRuntime_WorkflowTerminated

                While True
                    Dim instance As WorkflowInstance = workflowRuntime.CreateWorkflow(GetType(Microsoft.WorkflowServices.Samples.SequentialCalculatorClient))
                    Console.WriteLine("Start SequentialCalculatorClient.")
                    instance.Start()
                    waitHandle.WaitOne()
                    Console.ForegroundColor = ConsoleColor.Green
                    Console.WriteLine("Do another calculation? (Y)")
                    Console.ForegroundColor = ConsoleColor.Red
                    Console.WriteLine("Press <enter> to exit.")
                    Console.ResetColor()
                    Dim input As String = Console.ReadLine()
                    If input.Length = 0 OrElse input(0) <> "Y"c Then
                        Exit While
                    End If
                    waitHandle.Reset()
                End While
            End Using
        End Sub
        Private Shared Sub workflowRuntime_WorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
            Console.WriteLine("WorkflowCompleted: " + e.WorkflowInstance.InstanceId.ToString())
            waitHandle.[Set]()
        End Sub
        Private Shared Sub workflowRuntime_WorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
            Console.WriteLine("WorkflowTerminated: " + e.Exception.Message)
            waitHandle.[Set]()
        End Sub
    End Class
End Namespace