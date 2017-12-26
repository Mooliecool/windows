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

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class WebServiceInvokeWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim activitybind1 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim workflowparameterbinding1 As System.Workflow.ComponentModel.WorkflowParameterBinding = New System.Workflow.ComponentModel.WorkflowParameterBinding
        Dim activitybind2 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim workflowparameterbinding2 As System.Workflow.ComponentModel.WorkflowParameterBinding = New System.Workflow.ComponentModel.WorkflowParameterBinding
        Me.code1 = New System.Workflow.Activities.CodeActivity
        Me.invokeWebService1 = New System.Workflow.Activities.InvokeWebServiceActivity
        '
        'code1
        '
        Me.code1.Name = "code1"
        AddHandler Me.code1.ExecuteCode, AddressOf Me.OnExecuteCode
        '
        'invokeWebService1
        '
        Me.invokeWebService1.MethodName = "CreateOrder"
        Me.invokeWebService1.Name = "invokeWebService1"
        activitybind1.Name = "WebServiceInvokeWorkflow"
        activitybind1.Path = "PurchaseOrderId"
        workflowparameterbinding1.ParameterName = "id"
        workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind1, System.Workflow.ComponentModel.ActivityBind))
        activitybind2.Name = "WebServiceInvokeWorkflow"
        activitybind2.Path = "POStatus"
        workflowparameterbinding2.ParameterName = "orderStatus"
        workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind2, System.Workflow.ComponentModel.ActivityBind))
        Me.invokeWebService1.ParameterBindings.Add(workflowparameterbinding1)
        Me.invokeWebService1.ParameterBindings.Add(workflowparameterbinding2)
        Me.invokeWebService1.ProxyClass = GetType(Microsoft.Samples.Workflow.WebService.localhost.WebServicePublishWorkflow_WebService)
        AddHandler Me.invokeWebService1.Invoking, AddressOf Me.OnWebServiceInvoking
        '
        'WebServiceInvokeWorkflow
        '
        Me.Activities.Add(Me.invokeWebService1)
        Me.Activities.Add(Me.code1)
        Me.Name = "WebServiceInvokeWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private invokeWebService1 As System.Workflow.Activities.InvokeWebServiceActivity
    Private code1 As System.Workflow.Activities.CodeActivity

End Class
