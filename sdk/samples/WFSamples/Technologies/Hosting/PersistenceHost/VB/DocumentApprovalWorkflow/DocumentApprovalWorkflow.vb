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
Imports System.Workflow.Runtime
Imports Microsoft.VisualBasic

Public Class DocumentApprovalWorkflow
    Inherits SequentialWorkflowActivity

    Private approverValue As String = String.Empty

    Public Property Approver() As String
        Get
            Return approverValue
        End Get

        Set(ByVal value As String)
            approverValue = value
        End Set
    End Property


    Public ReadOnly Property InstanceId() As Guid
        Get
            Try
                Return WorkflowEnvironment.WorkflowInstanceId
            Catch ex As Exception
                Return Guid.Empty
            End Try
        End Get
    End Property
End Class
