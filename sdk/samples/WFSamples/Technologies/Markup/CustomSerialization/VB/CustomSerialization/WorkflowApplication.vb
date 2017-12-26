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
Imports System.ComponentModel.Design.Serialization
Imports System.Threading
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.Runtime
Imports System.Xml

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)
    Const workflowFilename As String = "workflow.xaml"

    Shared Sub Main()
        Dim validator As Validator = CType(Activator.CreateInstance(GetType(Validator)), Validator)
        Dim serializer As New WorkflowMarkupSerializer()
        Dim serializationManager As New DesignerSerializationManager()

        Using workflowRuntime As New WorkflowRuntime()
            AddHandler workflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler workflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

            '
            ' Create queue workflow programmatically
            '
            Console.WriteLine("Creating workflow.")
            Dim workflow As New SequentialWorkflowActivity()
            workflow.Name = "Programmatically created workflow"
            workflow.Description = "Programmatically created workflow for XAML activation"

            ' Add Queue activity
            Dim queueActivity As New QueueActivity()
            queueActivity.Name = "QueueActivity1"

            ' Enqueue data
            queueActivity.NameQueue.Enqueue("Queue item 1")
            queueActivity.NameQueue.Enqueue("Queue item 2")
            queueActivity.NameQueue.Enqueue("Queue item 3")
            workflow.Activities.Add(queueActivity)

            ' Add Stack activity
            Dim stackActivity As New StackActivity()
            stackActivity.Name = "StackActivity1"

            ' Push data
            stackActivity.NameStack.Push("Stack item 1")
            stackActivity.NameStack.Push("Stack item 2")
            stackActivity.NameStack.Push("Stack item 3")
            workflow.Activities.Add(stackActivity)

            '
            ' Serialize workflow to XAML file
            '
            Console.WriteLine("Serializing workflow to file.")
            Using writer As XmlWriter = XmlWriter.Create(workflowFilename)
                Using serializationManager.CreateSession()
                    serializer.Serialize(serializationManager, writer, workflow)
                    If (serializationManager.Errors.Count > 0) Then
                        Console.WriteLine(String.Format("There were {0} errors during serialization", serializationManager.Errors.Count))
                        Return
                    End If
                End Using
            End Using

            '
            ' Create workflow instance from file
            '
            Console.WriteLine("Deserializing workflow from file.")
            Dim workflowInstance As WorkflowInstance
            Using reader As XmlReader = XmlReader.Create(workflowFilename)
                Try
                    workflowInstance = workflowRuntime.CreateWorkflow(reader)
                Catch e As WorkflowValidationFailedException
                    Console.WriteLine("Validation errors found.  Exiting.")
                    Dim validationError As ValidationError
                    For Each validationError In e.Errors
                        Console.WriteLine(validationError.ErrorText)
                    Next validationError
                    Return
                End Try
            End Using

            '
            ' Execute workflow
            '
            Console.WriteLine("Starting workflow.")
            WorkflowInstance.Start()
            WaitHandle.WaitOne()
            workflowRuntime.StopRuntime()
        End Using
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        Console.WriteLine("Workflow completed.")
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

End Class


