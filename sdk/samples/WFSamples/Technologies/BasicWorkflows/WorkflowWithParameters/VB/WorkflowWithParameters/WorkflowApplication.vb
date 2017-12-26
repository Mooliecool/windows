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

Class WorkflowApplication
    'A workflow is always run asychronously; the main thread waits on this event so the program
    ' does not exit before the workflow completes.
    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main(ByVal args As String())
        Try
            ' The program passes the one and only argument into the workflow
            '  as the order amount.
            If args.Length < 1 Then
                Console.WriteLine("Usage: SequentialWorkflowWithParameters [amount]")
                Return
            End If

            ' Create the WorkflowRuntime
            Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
                Console.WriteLine("Runtime Started.")

                ' Listen for the workflow events
                AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

                ' Set up the parameters
                ' "amount" is an "in" parameter and specifies the order amount.
                '  If the amount is < 500 the status is "approved"; "rejected" otherwise.
                Dim parameters As Dictionary(Of String, Object) = New Dictionary(Of String, Object)()
                parameters.Add("Amount", Convert.ToInt32(args(0), CultureInfo.InvariantCulture))

                ' Get the workflow type.
                Dim type As Type = GetType(SequentialWorkflow)

                ' Create and start an instance of the workflow.
                currentWorkflowRuntime.CreateWorkflow(type, parameters).Start()
                Console.WriteLine("Workflow Started.")

                ' Wait for the event to be signaled.
                WaitHandle.WaitOne()

                currentWorkflowRuntime.StopRuntime()
                Console.WriteLine("Program Complete.")
            End Using
        Catch exception As Exception
            Console.WriteLine("Exception occurred: " + exception.Message)
        End Try
    End Sub

    ' This method is called when a workflow instance is completed; because only a single
    ' instance is started, the event arguments are ignored and the waitHandle is signaled so 
    ' the main thread can continue and exit the program.
    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        'The order status is stored in the "status" "in" parameter.
        Dim orderStatus As String = e.OutputParameters("Status").ToString()
        Console.WriteLine("Order was " + orderStatus)
        WaitHandle.Set()
    End Sub


    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub
End Class



