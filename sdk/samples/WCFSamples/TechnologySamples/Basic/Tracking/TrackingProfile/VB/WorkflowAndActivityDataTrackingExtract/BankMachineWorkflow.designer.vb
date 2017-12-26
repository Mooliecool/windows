' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Drawing
Imports System.Reflection
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

Namespace Microsoft.Samples.Workflow.WorkflowAndActivityDataTrackingExtract
    Partial Class BankMachineWorkflow
#Region "Designer generated code"

        ''' <summary>
        ''' Required method for Designer support - do not modify
        ''' the contents of this method with the code editor.
        ''' </summary>
        <System.Diagnostics.DebuggerNonUserCode()> _
        Private Sub InitializeComponent()
            Me.CanModifyActivities = True
            Dim codecondition1 As New System.Workflow.Activities.CodeCondition()
            Dim codecondition2 As New System.Workflow.Activities.CodeCondition()
            Dim codecondition3 As New System.Workflow.Activities.CodeCondition()
            Me.withdrawalActivity = New System.Workflow.Activities.CodeActivity()
            Me.depositActivity = New System.Workflow.Activities.CodeActivity()
            Me.withdrawalIfElseBranchActivity2 = New System.Workflow.Activities.IfElseBranchActivity()
            Me.depositIfElseBranchActivity = New System.Workflow.Activities.IfElseBranchActivity()
            Me.depositOrWithdrawalIfElseActivity = New System.Workflow.Activities.IfElseActivity()
            Me.userInputActivity = New System.Workflow.Activities.CodeActivity()
            Me.bankMachineSequenceActivity = New System.Workflow.Activities.SequenceActivity()
            Me.serviceChargeActivity = New Microsoft.Samples.Workflow.ServiceChargeActivityLibrary.ServiceCharge()
            Me.whileNotExitActivity = New System.Workflow.Activities.WhileActivity()
            '
            ' withdrawalActivity
            '
            Me.withdrawalActivity.Name = "withdrawalActivity"
            AddHandler Me.withdrawalActivity.ExecuteCode, AddressOf Me.WithdrawalExecuteCode
            '
            ' depositActivity
            '
            Me.depositActivity.Name = "depositActivity"
            AddHandler Me.depositActivity.ExecuteCode, AddressOf Me.DepositExecuteCode
            '
            ' withdrawalIfElseBranchActivity2
            '
            Me.withdrawalIfElseBranchActivity2.Activities.Add(Me.withdrawalActivity)
            AddHandler codecondition1.Condition, AddressOf Me.CheckWithdrawal
            Me.withdrawalIfElseBranchActivity2.Condition = codecondition1
            Me.withdrawalIfElseBranchActivity2.Name = "withdrawalIfElseBranchActivity2"
            '
            ' depositIfElseBranchActivity
            '
            Me.depositIfElseBranchActivity.Activities.Add(Me.depositActivity)
            AddHandler codecondition2.Condition, AddressOf Me.CheckDeposit
            Me.depositIfElseBranchActivity.Condition = codecondition2
            Me.depositIfElseBranchActivity.Name = "depositIfElseBranchActivity"
            '
            ' depositOrWithdrawalIfElseActivity
            '
            Me.depositOrWithdrawalIfElseActivity.Activities.Add(Me.depositIfElseBranchActivity)
            Me.depositOrWithdrawalIfElseActivity.Activities.Add(Me.withdrawalIfElseBranchActivity2)
            Me.depositOrWithdrawalIfElseActivity.Name = "depositOrWithdrawalIfElseActivity"
            '
            ' userInputActivity
            '
            Me.userInputActivity.Name = "userInputActivity"
            AddHandler Me.userInputActivity.ExecuteCode, AddressOf Me.UserInputExecuteCode
            '
            ' bankMachineSequenceActivity
            '
            Me.bankMachineSequenceActivity.Activities.Add(Me.userInputActivity)
            Me.bankMachineSequenceActivity.Activities.Add(Me.depositOrWithdrawalIfElseActivity)
            Me.bankMachineSequenceActivity.Name = "bankMachineSequenceActivity"
            '
            ' serviceChargeActivity
            '
            Me.serviceChargeActivity.Fee = 2.41
            Me.serviceChargeActivity.Name = "serviceChargeActivity"
            '
            ' whileNotExitActivity
            '
            Me.whileNotExitActivity.Activities.Add(Me.bankMachineSequenceActivity)
            AddHandler codecondition3.Condition, AddressOf Me.CheckOption
            Me.whileNotExitActivity.Condition = codecondition3
            Me.whileNotExitActivity.Name = "whileNotExitActivity"
            '
            ' BankMachineWorkflow
            '
            Me.Activities.Add(Me.whileNotExitActivity)
            Me.Activities.Add(Me.serviceChargeActivity)
            Me.Name = "BankMachineWorkflow"
            Me.CanModifyActivities = False

        End Sub

#End Region

        Private serviceChargeActivity As Microsoft.Samples.Workflow.ServiceChargeActivityLibrary.ServiceCharge
        Private bankMachineSequenceActivity As SequenceActivity
        Private withdrawalActivity As CodeActivity
        Private depositActivity As CodeActivity
        Private withdrawalIfElseBranchActivity2 As IfElseBranchActivity
        Private depositIfElseBranchActivity As IfElseBranchActivity
        Private depositOrWithdrawalIfElseActivity As IfElseActivity
        Private whileNotExitActivity As WhileActivity
        Private userInputActivity As CodeActivity


















    End Class
End Namespace