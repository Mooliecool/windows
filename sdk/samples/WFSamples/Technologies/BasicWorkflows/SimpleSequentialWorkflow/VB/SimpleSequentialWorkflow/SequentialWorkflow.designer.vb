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
Partial Public Class SequentialWorkflow
    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim codecondition1 As System.Workflow.Activities.CodeCondition = New System.Workflow.Activities.CodeCondition
        Me.IsUnderLimitIfElseActivity = New System.Workflow.Activities.IfElseActivity
        Me.YesIfElseBranch = New System.Workflow.Activities.IfElseBranchActivity
        Me.NoIfElseBranch = New System.Workflow.Activities.IfElseBranchActivity
        Me.ApprovePO = New System.Workflow.Activities.CodeActivity
        Me.RejectPO = New System.Workflow.Activities.CodeActivity
        '
        'IsUnderLimitIfElseActivity
        '
        Me.IsUnderLimitIfElseActivity.Activities.Add(Me.YesIfElseBranch)
        Me.IsUnderLimitIfElseActivity.Activities.Add(Me.NoIfElseBranch)
        Me.IsUnderLimitIfElseActivity.Name = "IsUnderLimitIfElseActivity"
        '
        'YesIfElseBranch
        '
        Me.YesIfElseBranch.Activities.Add(Me.ApprovePO)
        AddHandler codecondition1.Condition, AddressOf Me.IsUnderLimit
        Me.YesIfElseBranch.Condition = codecondition1
        Me.YesIfElseBranch.Name = "YesIfElseBranch"
        '
        'NoIfElseBranch
        '
        Me.NoIfElseBranch.Activities.Add(Me.RejectPO)
        Me.NoIfElseBranch.Name = "NoIfElseBranch"
        '
        'ApprovePO
        '
        Me.ApprovePO.Name = "ApprovePO"
        AddHandler Me.ApprovePO.ExecuteCode, AddressOf Me.OnApproved
        '
        'RejectPO
        '
        Me.RejectPO.Name = "RejectPO"
        AddHandler Me.RejectPO.ExecuteCode, AddressOf Me.OnRejected
        '
        'SequentialWorkflow
        '
        Me.Activities.Add(Me.IsUnderLimitIfElseActivity)
        Me.Name = "SequentialWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents IsUnderLimitIfElseActivity As System.Workflow.Activities.IfElseActivity
    Private WithEvents YesIfElseBranch As System.Workflow.Activities.IfElseBranchActivity
    Private WithEvents ApprovePO As System.Workflow.Activities.CodeActivity
    Private WithEvents NoIfElseBranch As System.Workflow.Activities.IfElseBranchActivity
    Private WithEvents RejectPO As System.Workflow.Activities.CodeActivity

End Class
