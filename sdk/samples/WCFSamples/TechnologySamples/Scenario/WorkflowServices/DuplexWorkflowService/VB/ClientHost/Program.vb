' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ServiceModel
Imports System.ServiceModel.Description
Imports System.Workflow.Runtime

Namespace Microsoft.WorkflowServices.Samples
	<ServiceBehavior(InstanceContextMode := InstanceContextMode.[Single])> _
	Class ClientHost
		Implements IReverseContract
        Shared Sub Main(ByVal args As String())
            Dim localHost As New LocalWorkflowServiceHost(GetType(ClientWorkflow), New ClientHost())
            AddHandler localHost.Description.Behaviors.Find(Of WorkflowRuntimeBehavior)().WorkflowRuntime.WorkflowTerminated, AddressOf WorkflowRuntime_WorkflowTerminated
            AddHandler localHost.Description.Behaviors.Find(Of WorkflowRuntimeBehavior)().WorkflowRuntime.WorkflowCompleted, AddressOf WorkflowRuntime_WorkflowCompleted

            localHost.Open()
            Console.WriteLine("LocalWorkflowServiceHost is ready.")

            Dim channel As IHostForwardContract = localHost.CreateLocalChannel(Of IHostForwardContract)()
            If Not localHost.RecoveredContext Then
                ' new Workflow
                Dim returnUri As String = localHost.BaseAddresses(0).ToString()
                channel.BeginWork(returnUri)
                localHost.MaintainContext(DirectCast(channel, IClientChannel))
                Console.WriteLine("Press <enter> to submit first WorkItem.")
            Else
                ' continuation of existing Workflow
                Console.WriteLine("Press <enter> to submit another WorkItem.")
            End If
            Console.ReadLine()

            Dim generator As New Random()
            While True
                Dim id As Integer = generator.[Next](100, 999)
                channel.SubmitWorkItem("WorkItem" + id.ToString())
                Console.WriteLine("Submitting: WorkItem" + id.ToString())
                Console.WriteLine("Submit another WorkItem? (Y)es or (N)o")
                Dim input As String = Console.ReadLine()
                If input.Length = 0 OrElse input(0) <> "Y"c Then
                    Exit While
                End If
            End While

            Console.WriteLine("Completing Work...")

            channel.WorkComplete()
            DirectCast(channel, IClientChannel).Close()

            Console.ForegroundColor = ConsoleColor.Red
            Console.WriteLine("Press <enter> to exit.")
            Console.ResetColor()
            Console.ReadLine()

            localHost.Close()
        End Sub

        Private Shared Sub WorkflowRuntime_WorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
            Console.WriteLine("WorkflowTerminated: " + e.Exception.Message)
        End Sub
        Private Shared Sub WorkflowRuntime_WorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
            Console.WriteLine("WorkflowCompleted: " + e.WorkflowInstance.InstanceId.ToString())
        End Sub


#Region "IReverseContract Members"

        Public Sub WorkItemComplete(ByVal item As WorkItem) Implements IReverseContract.WorkItemComplete
            Console.ForegroundColor = ConsoleColor.Green
            Console.WriteLine("FirstPart: " + item.FirstPart)
            Console.Write("PartsList: ")
            For i As Integer = 0 To item.PartsList.Count - 1
                If i > 0 Then
                    Console.Write(",")
                End If
                Console.Write(item.PartsList(i).ToString())
            Next
            Console.WriteLine()
            Console.WriteLine("LastPart: " + item.LastPart)
            Console.WriteLine()
            Console.ResetColor()
        End Sub

#End Region

	End Class
End Namespace