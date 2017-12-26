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

    Shared Sub Main()
        Const workflowFilename As String = "workflow.xoml"

        Using workflowRuntime As New WorkflowRuntime()
            AddHandler workflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler workflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

            '
            ' Create workflow programmatically
            '
            Console.WriteLine("Creating workflow.")
            Dim workflow As New SequentialWorkflowActivity()
            workflow.Name = "Programmatically created workflow"
            workflow.Description = "Programmatically created workflow for XAML activation"
            Dim activity As New ConsoleActivity()
            activity.Name = "ConsoleActivity1"
            activity.StringToWrite = "Sample String"
            workflow.Activities.Add(activity)

            '
            ' Serialize workflow to XAML file
            '
            Console.WriteLine("Serializing workflow to file.")
            Dim serializer As New WorkflowMarkupSerializer()
            Using writer As XmlWriter = XmlWriter.Create(workflowFilename)
                Dim serializationManager As New DesignerSerializationManager()
                Using serializationManager.CreateSession()
                    serializer.Serialize(serializationManager, writer, workflow)
                    If serializationManager.Errors.Count > 0 Then
                        Throw New Exception(String.Format("There were {0} errors during serialization", serializationManager.Errors.Count))
                    End If
                End Using
            End Using

            '
            ' Deserialize workflow from file
            '
            Dim deserializedWorkflow As WorkflowInstance = Nothing
            Console.WriteLine("Deserializing workflow from file.")
            Try
                Using reader As XmlReader = XmlReader.Create(workflowFilename)
                    deserializedWorkflow = workflowRuntime.CreateWorkflow(reader)
                End Using
            Catch exp As WorkflowValidationFailedException
                Dim list As ValidationErrorCollection = exp.Errors
                For Each err As ValidationError In list
                    Console.WriteLine(err.ErrorText)
                Next
                Return
            End Try

            '
            ' Start workflow
            '
            Console.WriteLine("Starting workflow.")
            deserializedWorkflow.Start()

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


