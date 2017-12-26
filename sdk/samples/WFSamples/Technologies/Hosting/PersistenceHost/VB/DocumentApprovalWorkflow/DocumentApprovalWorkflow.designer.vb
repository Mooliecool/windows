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
Partial Public Class DocumentApprovalWorkflow

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
        Me.DocumentApproved = New System.Workflow.Activities.HandleExternalEventActivity
        Me.SendDocument = New System.Workflow.Activities.CallExternalMethodActivity
        '
        'DocumentApproved
        '
        Me.DocumentApproved.EventName = "DocumentApproved"
        Me.DocumentApproved.InterfaceType = GetType(IDocumentApproval)
        Me.DocumentApproved.Name = "DocumentApproved"
        '
        'SendDocument
        '
        Me.SendDocument.InterfaceType = GetType(IDocumentApproval)
        Me.SendDocument.MethodName = "RequestDocumentApproval"
        Me.SendDocument.Name = "SendDocument"
        activitybind1.Name = "DocumentApprovalWorkflow"
        activitybind1.Path = "Approver"
        workflowparameterbinding1.ParameterName = "approver"
        workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind1, System.Workflow.ComponentModel.ActivityBind))
        activitybind2.Name = "DocumentApprovalWorkflow"
        activitybind2.Path = "InstanceId"
        workflowparameterbinding2.ParameterName = "documentId"
        workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind2, System.Workflow.ComponentModel.ActivityBind))
        Me.SendDocument.ParameterBindings.Add(workflowparameterbinding1)
        Me.SendDocument.ParameterBindings.Add(workflowparameterbinding2)
        '
        'DocumentApprovalWorkflow
        '
        Me.Activities.Add(Me.SendDocument)
        Me.Activities.Add(Me.DocumentApproved)
        Me.Name = "DocumentApprovalWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents SendDocument As System.Workflow.Activities.CallExternalMethodActivity
    Private WithEvents DocumentApproved As System.Workflow.Activities.HandleExternalEventActivity

End Class
