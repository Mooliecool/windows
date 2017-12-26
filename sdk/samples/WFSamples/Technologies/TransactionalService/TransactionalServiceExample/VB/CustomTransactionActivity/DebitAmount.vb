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
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Drawing
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

<ToolboxItemAttribute(GetType(ActivityToolboxItem))> _
Public Class DebitAmount
    Inherits Activity

    Protected Overrides Function Execute(ByVal executionContext As System.Workflow.ComponentModel.ActivityExecutionContext) As System.Workflow.ComponentModel.ActivityExecutionStatus
        ' Fire the Before Invoke Handler
        MyBase.RaiseEvent(OnBeforeInvokeEvent, Me, EventArgs.Empty)

        ' Get reference to the transactional service from the context
        Dim service As AbstractTransactionService = executionContext.GetService(Of AbstractTransactionService)()
        ' Call a method on the service and pass the payload to it
        service.DebitAmount(Me.Amount)

        ' Return the status of the activity as closed
        Return System.Workflow.ComponentModel.ActivityExecutionStatus.Closed
    End Function

    Public Shared AmountProperty As DependencyProperty = DependencyProperty.Register("Amount", GetType(System.Int32), GetType(DebitAmount))

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    Public Property Amount() As System.Int32
        Get
            Return CType(MyBase.GetValue(AmountProperty), Integer)
        End Get
        Set(ByVal value As System.Int32)
            MyBase.SetValue(AmountProperty, value)
        End Set
    End Property
    Public Shared OnBeforeInvokeEvent As DependencyProperty = DependencyProperty.Register("OnBeforeInvoke", GetType(System.EventHandler), GetType(DebitAmount))

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    Public Custom Event OnBeforeInvoke As System.EventHandler
        AddHandler(ByVal value As System.EventHandler)
            MyBase.AddHandler(OnBeforeInvokeEvent, value)
            MyBase.AddHandler(OnBeforeInvokeEvent, value)
        End AddHandler

        RemoveHandler(ByVal value As System.EventHandler)
            MyBase.RemoveHandler(OnBeforeInvokeEvent, value)
            MyBase.RemoveHandler(OnBeforeInvokeEvent, value)
        End RemoveHandler

        RaiseEvent(ByVal sender As Object, ByVal e As System.EventArgs)
        End RaiseEvent
    End Event
End Class
