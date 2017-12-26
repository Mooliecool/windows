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
        Me.SendDocument = New System.Workflow.Activities.CallExternalMethodActivity
        Me.DocumentApproved = New System.Workflow.Activities.HandleExternalEventActivity
        '
        'SendDocument
        '
        Me.SendDocument.Name = "SendDocument"
        Me.SendDocument.InterfaceType = GetType(IDocumentApproval)
        Me.SendDocument.MethodName = "RequestDocumentApproval"
        AddHandler Me.SendDocument.MethodInvoking, AddressOf Me.OnMethodInvoking
        '
        'DocumentApproved
        '
        Me.DocumentApproved.EventName = "DocumentApproved"
        Me.DocumentApproved.Name = "DocumentApproved"
        Me.DocumentApproved.InterfaceType = GetType(IDocumentApproval)
        Me.DocumentApproved.Roles = Nothing
        AddHandler Me.DocumentApproved.Invoked, AddressOf Me.OnInvoked
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
