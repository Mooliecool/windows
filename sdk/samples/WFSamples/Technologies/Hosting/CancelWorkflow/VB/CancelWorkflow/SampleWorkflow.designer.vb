'----------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
'  
'   Copyright (C) Microsoft Corporation.  All rights reserved.
'  
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
'  
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'----------------------------------------------------------------------

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class SampleWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.ExpenseApproval = New System.Workflow.Activities.HandleExternalEventActivity
        Me.SubmitExpense = New System.Workflow.Activities.CallExternalMethodActivity
        '
        'ExpenseApproval
        '
        Me.ExpenseApproval.EventName = "ExpenseApproval"
        Me.ExpenseApproval.InterfaceType = GetType(IExpenseReportService)
        Me.ExpenseApproval.Name = "ExpenseApproval"
        AddHandler Me.ExpenseApproval.Executing, AddressOf Me.ExpenseApproval_Executing

        '
        'SubmitExpense
        '
        Me.SubmitExpense.InterfaceType = GetType(IExpenseReportService)
        Me.SubmitExpense.MethodName = "SubmitExpense"
        Me.SubmitExpense.Name = "SubmitExpense"
        AddHandler Me.SubmitExpense.MethodInvoking, AddressOf Me.SubmitExpense_MethodInvoking
        '
        'SampleWorkflow
        '
        Me.Activities.Add(Me.SubmitExpense)
        Me.Activities.Add(Me.ExpenseApproval)
        Me.Name = "SampleWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents ExpenseApproval As System.Workflow.Activities.HandleExternalEventActivity
    Private WithEvents SubmitExpense As System.Workflow.Activities.CallExternalMethodActivity

End Class
