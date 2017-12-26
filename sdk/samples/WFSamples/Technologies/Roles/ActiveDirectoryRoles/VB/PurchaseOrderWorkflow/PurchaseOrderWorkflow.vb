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
Imports System.Security.Principal

Public Class PurchaseOrderWorkflow
    Inherits SequentialWorkflowActivity

    Private poInitiatorsValue As WorkflowRoleCollection = New WorkflowRoleCollection

    Public ReadOnly Property POInitiators() As WorkflowRoleCollection
        Get
            Return poInitiatorsValue
        End Get
    End Property

    Private Sub OnPOInitiated(ByVal sender As System.Object, ByVal e As ExternalDataEventArgs)
        Console.WriteLine("PO Initiated successfully")
    End Sub

    Private Sub OnSetupRoles(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim poInitiatorsRole As ActiveDirectoryRole = ActiveDirectoryRoleFactory.CreateFromAlias("ADGroup")

        ' Add the role to the WorkflowRoleCollection representing the POInitiators
        POInitiators.Add(poInitiatorsRole)
    End Sub
End Class
