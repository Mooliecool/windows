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
Partial Public NotInheritable Class WebServicePublishWorkflow

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
        Dim activitybind3 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim workflowparameterbinding3 As System.Workflow.ComponentModel.WorkflowParameterBinding = New System.Workflow.ComponentModel.WorkflowParameterBinding
        Me.webServiceResponse1 = New System.Workflow.Activities.WebServiceOutputActivity
        Me.webServiceReceive1 = New System.Workflow.Activities.WebServiceInputActivity
        '
        'webServiceResponse1
        '
        Me.webServiceResponse1.InputActivityName = "webServiceReceive1"
        Me.webServiceResponse1.Name = "webServiceResponse1"
        activitybind1.Name = "WebServicePublishWorkflow"
        activitybind1.Path = "POStatus"
        workflowparameterbinding1.ParameterName = "orderStatus"
        workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind1, System.Workflow.ComponentModel.ActivityBind))
        Me.webServiceResponse1.ParameterBindings.Add(workflowparameterbinding1)
        '
        'webServiceReceive1
        '
        Me.webServiceReceive1.InterfaceType = GetType(IPurchaseOrder)
        Me.webServiceReceive1.IsActivating = True
        Me.webServiceReceive1.MethodName = "CreateOrder"
        Me.webServiceReceive1.Name = "webServiceReceive1"
        activitybind2.Name = "WebServicePublishWorkflow"
        activitybind2.Path = "PurchaseOrderId"
        workflowparameterbinding2.ParameterName = "id"
        workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind2, System.Workflow.ComponentModel.ActivityBind))
        activitybind3.Name = "WebServicePublishWorkflow"
        activitybind3.Path = "POStatus"
        workflowparameterbinding3.ParameterName = "orderStatus"
        workflowparameterbinding3.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind3, System.Workflow.ComponentModel.ActivityBind))
        Me.webServiceReceive1.ParameterBindings.Add(workflowparameterbinding2)
        Me.webServiceReceive1.ParameterBindings.Add(workflowparameterbinding3)
        AddHandler Me.webServiceReceive1.InputReceived, AddressOf Me.OnWebServiceInputReceived
        '
        'WebServicePublishWorkflow
        '
        Me.Activities.Add(Me.webServiceReceive1)
        Me.Activities.Add(Me.webServiceResponse1)
        Me.Name = "WebServicePublishWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents webServiceReceive1 As System.Workflow.Activities.WebServiceInputActivity
    Private WithEvents webServiceResponse1 As System.Workflow.Activities.WebServiceOutputActivity

End Class
