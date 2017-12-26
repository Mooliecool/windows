' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Threading
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Hosting
Imports Microsoft.VisualBasic

Namespace Microsoft.Rules.Samples
	Class Program
        Shared waitHandle As New AutoResetEvent(False)

        Shared Sub Main(ByVal args As String())

            ' Start the engine
            Using workflowRuntime As New WorkflowRuntime()
                AddHandler workflowRuntime.WorkflowCompleted, AddressOf workflowRuntime_WorkflowCompleted
                AddHandler workflowRuntime.WorkflowTerminated, AddressOf workflowRuntime_WorkflowTerminated


                Dim anotherOrder As String = "y"
                While Not anotherOrder.ToLower().StartsWith("n")
                    ' Prompt the user for inputs
                    Console.Write("" & Chr(10) & "Please enter your name: ")
                    Dim customerName As String = Console.ReadLine()
                    Console.WriteLine("" & Chr(10) & "What would you like to purchase?")
                    Console.WriteLine("" & Chr(9) & "(1) Vista Ultimate DVD")
                    Console.WriteLine("" & Chr(9) & "(2) Vista Ultimate Upgrade DVD")
                    Console.WriteLine("" & Chr(9) & "(3) Vista Home Premium DVD")
                    Console.WriteLine("" & Chr(9) & "(4) Vista Home Premium Upgrade DVD")
                    Console.WriteLine("" & Chr(9) & "(5) Vista Home Basic DVD")
                    Console.WriteLine("" & Chr(9) & "(6) Vista Home Basic Upgrade DVD")
                    Dim itemNum As Integer = 0

                    ' Make sure an integeral value has been entered for the item number
                    Dim validItemNum As Boolean = False
                    While validItemNum = False
                        Try
                            Console.Write("" & Chr(10) & "Please enter an item number: ")
                            itemNum = Int32.Parse(Console.ReadLine()) ' throw if the input is not an integer
                            validItemNum = True
                        Catch ex As FormatException
                            Console.WriteLine(" => Please enter an integer for the item number!")
                        End Try
                    End While

                    ' Make sure an integeral value has been entered for the zip code
                    Dim zipCode As String = ""
                    Dim validZip As Boolean = False
                    While validZip = False
                        Try
                            Console.Write("" & Chr(10) & "Please enter your 5-Digit zip code: ")
                            zipCode = Console.ReadLine()
                            Int32.Parse(zipCode) ' throw if the input is not an integer
                            validZip = True
                        Catch ex As FormatException
                            Console.WriteLine(" => Please enter an integer for the zip code!")
                        End Try
                    End While

                    Console.WriteLine()

                    ' Populate the dictionary with the information the user has just entered
                    Dim parameters As New Dictionary(Of String, Object)()
                    parameters.Add("CustomerName", customerName)
                    parameters.Add("ItemNum", itemNum)
                    parameters.Add("ZipCode", zipCode)

                    ' Load the workflow type and create the workflow
                    Dim instance As WorkflowInstance = workflowRuntime.CreateWorkflow(GetType(Workflow), parameters)
                    waitHandle.Reset()
                    instance.Start()

                    waitHandle.WaitOne()

                    Console.Write("Another Order? (Y/N): ")
                    anotherOrder = Console.ReadLine()
                End While
            End Using
        End Sub
        Private Shared Sub workflowRuntime_WorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
            Console.WriteLine("" & Chr(10) & "Workflow Completed")
            WaitHandle.[Set]()
        End Sub
        Private Shared Sub workflowRuntime_WorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
            Console.WriteLine(e.Exception.Message)
            WaitHandle.[Set]()
        End Sub
	End Class
End Namespace