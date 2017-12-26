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
Public Class TaskCompleted
    Inherits System.Workflow.Activities.HandleExternalEventActivity
    Public Sub New()
        MyBase.New()
        InitializeComponent()

        Me.EventName = "TaskCompleted"
        Me.InterfaceType = GetType(ITaskService)
    End Sub

    Protected Overrides Sub OnInvoked(ByVal e As System.EventArgs)
        Me.EventArgs = CType(e, TaskEventArgs)
    End Sub

    Public Shared SenderProperty As DependencyProperty = DependencyProperty.Register("Sender", GetType(System.Object), GetType(TaskCompleted))

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    Public Property Sender() As System.Object
        Get
            Return CType(MyBase.GetValue(SenderProperty), Object)

        End Get
        Set(ByVal value As System.Object)
            MyBase.SetValue(SenderProperty, value)

        End Set
    End Property
    Public Shared EventArgsProperty As DependencyProperty = DependencyProperty.Register("EventArgs", GetType(TaskEventArgs), GetType(TaskCompleted))

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    Public Property EventArgs() As TaskEventArgs
        Get
            Return CType(MyBase.GetValue(EventArgsProperty), TaskEventArgs)

        End Get
        Set(ByVal value As TaskEventArgs)
            MyBase.SetValue(EventArgsProperty, value)

        End Set
    End Property
End Class
