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
Imports System.Workflow.Activities

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class PurchaseOrderWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim activitybind1 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Me.InitiatePO = New System.Workflow.Activities.HandleExternalEventActivity
        Me.SetupRoles = New System.Workflow.Activities.CodeActivity
        activitybind1.Name = "PurchaseOrderWorkflow"
        activitybind1.Path = "POInitiators"
        '
        'InitiatePO
        '
        Me.InitiatePO.EventName = "InitiatePurchaseOrderEventHandler"
        Me.InitiatePO.InterfaceType = GetType(IStartPurchaseOrder)
        Me.InitiatePO.Name = "InitiatePO"
        AddHandler Me.InitiatePO.Invoked, AddressOf Me.OnPOInitiated
        Me.InitiatePO.SetBinding(System.Workflow.Activities.HandleExternalEventActivity.RolesProperty, CType(activitybind1, System.Workflow.ComponentModel.ActivityBind))
        '
        'SetupRoles
        '
        Me.SetupRoles.Name = "SetupRoles"
        AddHandler Me.SetupRoles.ExecuteCode, AddressOf Me.OnSetupRoles
        '
        'PurchaseOrderWorkflow
        '
        Me.Activities.Add(Me.SetupRoles)
        Me.Activities.Add(Me.InitiatePO)
        Me.Name = "PurchaseOrderWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private InitiatePO As System.Workflow.Activities.HandleExternalEventActivity
    Private SetupRoles As System.Workflow.Activities.CodeActivity

End Class
