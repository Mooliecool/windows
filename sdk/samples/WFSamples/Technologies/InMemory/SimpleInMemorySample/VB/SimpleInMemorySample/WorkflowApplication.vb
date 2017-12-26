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
Imports System.IO
Imports System.Reflection
Imports System.Threading
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.Runtime

Class WorkflowApplication

    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        Using workflowRuntime As New WorkflowRuntime()
            Dim compiler As New WorkflowCompiler()
            Dim compilerParameters As New WorkflowCompilerParameters()
            compilerParameters.GenerateInMemory = True
            compilerParameters.LanguageToUse = "VB"

            Dim workflowFilenames As String() = GetWorkflowFilenames()

            Dim results As WorkflowCompilerResults = compiler.Compile(compilerParameters, workflowFilenames)
            If results.Errors.Count > 0 Then
                Console.WriteLine("Errors occurred while building the workflow:")
                For Each compilerError As WorkflowCompilerError In results.Errors
                    Console.WriteLine(compilerError.Line.ToString() + "," + compilerError.Column.ToString() + " : " + compilerError.ErrorText)
                Next
                Return
            End If

            Dim workflowType As Type = results.CompiledAssembly.GetType("SequentialWorkflow")

            AddHandler workflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted

            AddHandler workflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

            Dim parameters As New Dictionary(Of String, Object)
            parameters.Add("Amount", 300)

            Dim instance As WorkflowInstance = workflowRuntime.CreateWorkflow(workflowType, parameters)
            instance.Start()

            WaitHandle.WaitOne()

            workflowRuntime.StopRuntime()
        End Using
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        Dim orderStatus As String = e.OutputParameters("Status").ToString()
        Console.WriteLine("Order was " + orderStatus)
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    Private Shared Function GetWorkflowFilenames() As String()
        Dim workflowFilename As String = "\SequentialWorkflow.vb"
        Dim workflowDesignerFilename As String = "\SequentialWorkflow.designer.vb"
        Dim applicationPath As String = Path.GetDirectoryName(Assembly.GetExecutingAssembly().GetName().CodeBase)
        Dim codeDirectory As String = Directory.GetParent(applicationPath.Substring(6)).ToString()
        Dim workflowFullPath As String = codeDirectory + workflowFilename
        Dim workflowDesignerFullPath As String = codeDirectory + workflowDesignerFilename

        Return New String() {workflowFullPath, workflowDesignerFullPath}


    End Function
End Class


