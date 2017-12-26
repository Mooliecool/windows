'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.Threading
Imports System.Activities
Imports System.Activities.Statements
Imports System.Activities.Expressions

Namespace Microsoft.Samples.Compensation.CompensableActivitySample

	Friend Class Program
		' Builds a sequence of two actions that are compensable
		Private Shared Function BuildASequenceofCompensableActions() As Activity

            Dim s As New Sequence
            s.Activities.Add(New WriteLine With {.Text = " Start of workflow"})

            ' first compensable action
            ' Body          - contains the work required to execute for the first action
            ' Compensation  - contains the work required to execute if at a later time 
            ' the first action needs to be compensated
            s.Activities.Add(New CompensableActivity With {.Body = New WriteLine With {.Text = " CompensableActivity: Do First Action"}, .CompensationHandler = New WriteLine With {.Text = " CompensableActivity: Compensate First Action"}})

            ' second compensable action
            s.Activities.Add(New CompensableActivity With {.Body = New WriteLine With {.Text = " CompensableActivity: Do Second Action"}, .CompensationHandler = New WriteLine With {.Text = " CompensableActivity: Compensate Second Action"}})
            s.Activities.Add(New WriteLine With {.Text = " End of workflow"})

            Return s
        End Function

		' Builds a sequence two actions that are compensable
		' The sequence hits an unexpected event after the execution of the first action and
		' it throws an ApplicationException
		Private Shared Function BuildASequenceofCompensableActionsThatThrows() As Activity
            Dim s As New Sequence

            s.Activities.Add(New WriteLine With {.Text = " Start of workflow"})
            s.Activities.Add(New CompensableActivity With {.Body = New WriteLine With {.Text = " CompensableActivity: Do First Action"}, .CompensationHandler = New WriteLine With {.Text = " CompensableActivity: Compensate First Action"}})

            ' an unexpected event is encountered
            s.Activities.Add(New [Throw] With {.Exception = New InArgument(Of Exception)(New [New](Of Exception))})

            ' if the workflow instance is not resumed after handling the exception, the following code
            ' will never execute
            s.Activities.Add(New CompensableActivity With {.Body = New WriteLine With {.Text = " CompensableActivity: Do Second Action"}, .CompensationHandler = New WriteLine With {.Text = " CompensableActivity: Compensate Second Action"}})
            s.Activities.Add(New WriteLine With {.Text = " End of workflow"})

            Return s
		End Function

		Shared Sub Main()
			Dim instance As WorkflowApplication

			' Executing the sequence of two actions that complete with success 
			Console.WriteLine("Execution with success:")
            instance = New WorkflowApplication(BuildASequenceofCompensableActions())

            Dim resetEvent As New ManualResetEvent(False)


            instance.Completed = Function(e As WorkflowApplicationCompletedEventArgs) resetEvent.Set()
            instance.Run()
            resetEvent.WaitOne()

            ' Notice how the first and second actions get executed and the workflow completes with success
            Console.WriteLine(Constants.vbLf & "Execution with exception:")

            ' Executing the sequence of two actions; after the first action is executed an exception will be thrown
            instance = New WorkflowApplication(BuildASequenceofCompensableActionsThatThrows())
            resetEvent.Reset()

            instance.Completed = Function(e As WorkflowApplicationCompletedEventArgs) resetEvent.Set()
            instance.OnUnhandledException = AddressOf HandleException

            instance.Run()
            resetEvent.WaitOne()

            ' Notice how the first action is executed successfully and because the workflow terminates with
            ' an exception and gets canceled, the first action gets compensated automatically;
            ' Since the second action wasn't executed, no compensation is necessary for it.
            Console.WriteLine(Constants.vbLf & "Press ENTER to exit")
            Console.ReadLine()
        End Sub

        Private Shared Function HandleException(ByVal e As WorkflowApplicationUnhandledExceptionEventArgs) As UnhandledExceptionAction
            Console.WriteLine("Workflow UnhandledException invoked...")
            Console.WriteLine("Exception Type: {0}", e.UnhandledException.GetType())
            Console.WriteLine("Exception Message:  {0}", e.UnhandledException.Message)

            ' Simulate a case where the exception can't be corrected and decide to cancel the workflow
            Return UnhandledExceptionAction.Cancel
        End Function
    End Class
End Namespace
