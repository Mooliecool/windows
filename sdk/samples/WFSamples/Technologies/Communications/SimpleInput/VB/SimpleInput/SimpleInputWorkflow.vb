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
Imports System.ComponentModel
Imports System.Workflow.Activities

Public Class SimpleInputWorkflow
    Inherits SequentialWorkflowActivity

    ' The member 'data' is accessed through binding in the dequeueInput activity.
    Private dataValue As Object = Nothing

    Public Property Data() As Object
        Get
            Return dataValue
        End Get
        Set(ByVal value As Object)
            dataValue = value
        End Set
    End Property


    Private Sub LoopCondition(ByVal sender As System.Object, ByVal e As System.Workflow.Activities.ConditionalEventArgs)
        If (Data Is Nothing) Then
            ' no data received yet, so we want to continue the loop
            e.Result = True
        Else
            Dim checkData As Boolean = Not (CType(Data, String).Equals("exit"))
            ' go until the data is equals "exit"
            e.Result = checkData
        End If
    End Sub

    Private Sub OnPrintInput(ByVal sender As System.Object, ByVal e As System.EventArgs)
        If Data IsNot Nothing Then
            Console.WriteLine("Input = " + Data.ToString())
        End If
    End Sub
End Class
