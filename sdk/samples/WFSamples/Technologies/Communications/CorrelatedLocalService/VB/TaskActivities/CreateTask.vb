'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'  This source code is intended only as a supplement to Microsoft
'  Development Tools and/or on-line documentation.  See these other
'  materials for detailed information regarding Microsoft code samples.
' 
'  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'  PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.ComponentModel
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.ComponentModel.Compiler

<ToolboxItemAttribute(GetType(ActivityToolboxItem))> _
Public Class CreateTask
    Inherits System.Workflow.Activities.CallExternalMethodActivity
    Public Sub New()
        MyBase.New()
        Me.InterfaceType = GetType(ITaskService)
        Me.MethodName = "CreateTask"
        InitializeComponent()
    End Sub

    Protected Overrides Sub OnMethodInvoking(ByVal e As EventArgs)
        Me.ParameterBindings("taskId").Value = Me.TaskId
        Me.ParameterBindings("assignee").Value = Me.Assignee
        Me.ParameterBindings("text").Value = Me.Text
    End Sub

    Public Shared AssigneeProperty As DependencyProperty = DependencyProperty.Register("Assignee", GetType(System.String), GetType(CreateTask))

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    Public Property Assignee() As System.String
        Get
            Return CType(MyBase.GetValue(AssigneeProperty), String)

        End Get
        Set(ByVal value As System.String)
            MyBase.SetValue(AssigneeProperty, value)

        End Set
    End Property
    Public Shared TaskIdProperty As DependencyProperty = DependencyProperty.Register("TaskId", GetType(System.String), GetType(CreateTask))

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    Public Property TaskId() As System.String
        Get
            Return CType(MyBase.GetValue(TaskIdProperty), String)

        End Get
        Set(ByVal value As System.String)
            MyBase.SetValue(TaskIdProperty, value)

        End Set
    End Property
    Public Shared TextProperty As DependencyProperty = DependencyProperty.Register("Text", GetType(System.String), GetType(CreateTask))

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    Public Property Text() As System.String
        Get
            Return CType(MyBase.GetValue(TextProperty), String)

        End Get
        Set(ByVal value As System.String)
            MyBase.SetValue(TextProperty, value)

        End Set
    End Property
End Class
