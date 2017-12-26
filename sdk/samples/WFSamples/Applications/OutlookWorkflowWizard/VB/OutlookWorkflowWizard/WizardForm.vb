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
Imports System.Drawing
Imports System.Text
Imports System.IO
Imports System.Windows.Forms
Imports System.Workflow.ComponentModel
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.Runtime
Imports System.CodeDom.Compiler
Imports System.Xml
Imports Microsoft.VisualBasic

Public Class WizardForm
    Dim folderTypeSelection As FolderType
    Dim ifConditionTypeSelection As IfConditionType
    Dim actionTypeSelection As ActionTypes
    Dim workflowDesigner As WorkflowViewWrapper
    Dim eventActivity As Activity
    Dim results As WorkflowCompilerResults
    Dim xamlFile As String

    Public Sub New()
        ' This call is required by the Windows Form Designer.
        InitializeComponent()
        folderSelectionLabel.Text = String.Empty
        conditionSelectionLabel.Text = String.Empty
        ifParameterLinkLabel.Text = String.Empty
        actionsSelectionLabel.Text = String.Empty
        workflowDesigner = New WorkflowViewWrapper()
        Me.splitContainer1.Panel2.Controls.Add(workflowDesigner)
        workflowDesigner.Show()
    End Sub

    Private Sub FolderSelectionChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles sentRadioButton.CheckedChanged, outboxRadioButton.CheckedChanged, inboxRadioButton.CheckedChanged
        If (inboxRadioButton.Checked) Then
            Me.folderSelectionLabel.Text = "Look inside Inbox Folder"
            folderTypeSelection = FolderType.Inbox
        ElseIf (sentRadioButton.Checked) Then
            Me.folderSelectionLabel.Text = "Look inside Sent Items Folder"
            folderTypeSelection = FolderType.Sent
        ElseIf (outboxRadioButton.Checked) Then
            Me.folderSelectionLabel.Text = "Look inside Outbox Folder"
            folderTypeSelection = FolderType.Outbox
        Else
            Me.folderSelectionLabel.Text = String.Empty
            folderTypeSelection = FolderType.None
        End If
        BuildWorkflow(WizardStep.None)
        ResetButtons()
    End Sub

    Private Sub ConditionSelectionChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles toRadioButton.CheckedChanged, subjectRadioButton.CheckedChanged, fromRadioButton.CheckedChanged, ccRadioButton.CheckedChanged, bccRadioButton.CheckedChanged
        If (subjectRadioButton.Checked) Then
            Me.conditionSelectionLabel.Text = "If Subject Equals to"
            ifConditionTypeSelection = IfConditionType.Subject
        ElseIf (fromRadioButton.Checked) Then
            Me.conditionSelectionLabel.Text = "If From Equals to"
            ifConditionTypeSelection = IfConditionType.FromEmail
        ElseIf (toRadioButton.Checked) Then
            Me.conditionSelectionLabel.Text = "If To Equals to"
            ifConditionTypeSelection = IfConditionType.To
        ElseIf (ccRadioButton.Checked) Then
            Me.conditionSelectionLabel.Text = "If CC Equals to"
            ifConditionTypeSelection = IfConditionType.CC
        ElseIf (bccRadioButton.Checked) Then
            Me.conditionSelectionLabel.Text = "If BCC Equals to"
            ifConditionTypeSelection = IfConditionType.Bcc
        Else
            Me.conditionSelectionLabel.Text = String.Empty
            ifConditionTypeSelection = IfConditionType.None
        End If

        Dim size As Size = Me.conditionSelectionLabel.Size
        Me.ifParameterLinkLabel.Location = New Point(conditionSelectionLabel.Location.X + size.Width, conditionSelectionLabel.Location.Y)
        If String.IsNullOrEmpty(Me.ifParameterLinkLabel.Text) Then
            Me.ifParameterLinkLabel.Text = "Enter Text Here"
        End If
        BuildWorkflow(WizardStep.Folder)
        ResetButtons()
    End Sub

    Private Sub ActionSelectionChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles actionsCheckedListBox.SelectedIndexChanged
        Dim indexCollection As CheckedListBox.CheckedIndexCollection = actionsCheckedListBox.CheckedIndices
        Me.actionsSelectionLabel.Text = String.Empty
        Me.actionTypeSelection = ActionTypes.None
        For Each index As Integer In indexCollection
            Select Case index
                Case (0)
                    Me.actionsSelectionLabel.Text += "Send Auto-Reply Email to Sender" + vbLf
                    Me.actionTypeSelection = Me.actionTypeSelection Or ActionTypes.SendAutoReply
                Case (1)
                    Me.actionsSelectionLabel.Text += "Create Outlook Note Using Subject as Title" + vbLf
                    Me.actionTypeSelection = Me.actionTypeSelection Or ActionTypes.CreateNote
                Case (2)
                    Me.actionsSelectionLabel.Text += "Create Outlook Task Using Subject as Title" + vbLf
                    Me.actionTypeSelection = Me.actionTypeSelection Or ActionTypes.CreateTask
            End Select
        Next
        BuildWorkflow(WizardStep.Condition)
        ResetButtons()
    End Sub

    Private Sub BuildWorkflow(ByVal wizardStep As WizardStep)
        If ValidateSelections(wizardStep) Then
            If workflowDesigner.SequentialWorkflow.Activities.Count > 0 Then
                workflowDesigner.SequentialWorkflow.Activities.Clear()
            End If

            ProcessEventSelection(Me.folderTypeSelection)
            ProcessFilterSelection(Me.ifConditionTypeSelection)
            ProcessActivitySelection(Me.actionTypeSelection)

            generateWorkflowButton.Enabled = False
            compileWorkflowButton.Enabled = True
        End If
    End Sub

    Private Sub ProcessActivitySelection(ByVal actionType As ActionTypes)
        If actionType = ActionTypes.None Then
            Return
        End If

        Dim parallelActivity As ParallelActivity = Nothing
        ' Check for multi activity selection
        If actionType <> ActionTypes.SendAutoReply And actionType <> ActionTypes.CreateNote And actionType <> ActionTypes.CreateTask Then
            parallelActivity = New ParallelActivity()
        End If
        ' Process each selected activity
        If (actionType And ActionTypes.SendAutoReply) = ActionTypes.SendAutoReply Then
            ProcessAutoReplyEmailActivity(parallelActivity)
        End If
        If (actionType And ActionTypes.CreateNote) = ActionTypes.CreateNote Then
            ProcessOutlookNoteActivity(parallelActivity)
        End If
        If (actionType And ActionTypes.CreateTask) = ActionTypes.CreateTask Then
            ProcessOutlookTaskActivity(parallelActivity)
        End If
        ' Add ParallelActivity to the workflow
        If parallelActivity IsNot Nothing Then
            workflowDesigner.SequentialWorkflow.Activities.Add(parallelActivity)
            workflowDesigner.Host.RootComponent.Site.Container.Add(parallelActivity)
        End If
    End Sub

    Private Sub ProcessOutlookTaskActivity(ByVal activityList As ParallelActivity)
        Dim outlookActivity As OutlookTask = New OutlookTask()
        outlookActivity.Name = "CreateOutlookTask"
        Dim branch As SequenceActivity = New SequenceActivity()
        branch.Name = "branch3"
        branch.Activities.Add(outlookActivity)

        If activityList IsNot Nothing Then
            activityList.Activities.Add(branch)
        Else
            workflowDesigner.SequentialWorkflow.Activities.Add(branch)
        End If

        workflowDesigner.Host.RootComponent.Site.Container.Add(outlookActivity)
        workflowDesigner.Host.RootComponent.Site.Container.Add(branch)
    End Sub

    Private Sub ProcessOutlookNoteActivity(ByVal activityList As ParallelActivity)
        Dim outlookNoteActivity As OutlookNote = New OutlookNote()
        outlookNoteActivity.Name = "CreateOutlookNote"
        Dim branch As SequenceActivity = New SequenceActivity()
        branch.Name = "branch2"
        branch.Activities.Add(outlookNoteActivity)

        If activityList IsNot Nothing Then
            activityList.Activities.Add(branch)
        Else
            workflowDesigner.SequentialWorkflow.Activities.Add(branch)
        End If

        workflowDesigner.Host.RootComponent.Site.Container.Add(outlookNoteActivity)
        workflowDesigner.Host.RootComponent.Site.Container.Add(branch)
    End Sub

    Private Sub ProcessAutoReplyEmailActivity(ByVal activityList As ParallelActivity)
        Dim emailActivity As AutoReplyEmail = New AutoReplyEmail()
        emailActivity.Name = "AutoReplyEmail"
        Dim branch As SequenceActivity = New SequenceActivity()
        branch.Name = "branch1"
        branch.Activities.Add(emailActivity)

        If activityList IsNot Nothing Then
            activityList.Activities.Add(branch)
        Else
            workflowDesigner.SequentialWorkflow.Activities.Add(branch)
        End If

        workflowDesigner.Host.RootComponent.Site.Container.Add(emailActivity)
        workflowDesigner.Host.RootComponent.Site.Container.Add(branch)
    End Sub

    Private Sub ProcessFilterSelection(ByVal ifConditionType As IfConditionType)
        Dim emailActivity As BaseMailbox = CType(eventActivity, BaseMailbox)
        Select Case ifConditionType
            Case ifConditionType.Subject
                emailActivity.Filter = FilterOption.Subject
                emailActivity.FilterValue = Me.ifParameterField.Text
            Case ifConditionType.FromEmail
                emailActivity.Filter = FilterOption.FromEmail
                emailActivity.FilterValue = Me.ifParameterField.Text
            Case ifConditionType.To
                emailActivity.Filter = FilterOption.To
                emailActivity.FilterValue = Me.ifParameterField.Text
            Case ifConditionType.CC
                emailActivity.Filter = FilterOption.CC
                emailActivity.FilterValue = Me.ifParameterField.Text
            Case ifConditionType.Bcc
                emailActivity.Filter = FilterOption.Bcc
                emailActivity.FilterValue = Me.ifParameterField.Text
        End Select
    End Sub

    Private Sub ProcessEventSelection(ByVal folderType As FolderType)
        Select Case folderType
            Case folderType.Inbox
                eventActivity = New EvaluateInboxItems()
                workflowDesigner.SequentialWorkflow.Activities.Add(eventActivity)
                workflowDesigner.SequentialWorkflow.Activities.Add(New DummyActivity())
                workflowDesigner.Host.RootComponent.Site.Container.Add(eventActivity)
            Case folderType.Sent
                eventActivity = New EvaluateSentItems()
                workflowDesigner.sequentialWorkflow.Activities.Add(eventActivity)
                workflowDesigner.SequentialWorkflow.Activities.Add(New DummyActivity())
                workflowDesigner.Host.RootComponent.Site.Container.Add(eventActivity)
            Case folderType.Outbox
                eventActivity = New EvaluateOutboxItems()
                workflowDesigner.SequentialWorkflow.Activities.Add(eventActivity)
                workflowDesigner.SequentialWorkflow.Activities.Add(New DummyActivity())
                workflowDesigner.Host.RootComponent.Site.Container.Add(eventActivity)
        End Select
    End Sub

    Private Sub ifParameterLinkLabel_LinkClicked(ByVal sender As System.Object, ByVal e As System.Windows.Forms.LinkLabelLinkClickedEventArgs) Handles ifParameterLinkLabel.LinkClicked
        Me.ifParameterLinkLabel.Visible = False
        Dim ifParameterLocation As Point = Me.ifParameterLinkLabel.Location
        Me.ifParameterField.Visible = True
        Me.ifParameterField.Text = ifParameterLinkLabel.Text
        Me.ifParameterField.Location = ifParameterLocation
        Me.ifParameterField.Focus()
    End Sub

    Private Sub ifParameterField_Leave(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ifParameterField.Leave
        Me.ifParameterField.Visible = False
        Me.ifParameterLinkLabel.Text = ifParameterField.Text
        Me.ifParameterLinkLabel.Visible = True
        Me.ifParameterLinkLabel.Focus()
    End Sub

    Private Sub ifParameterField_KeyDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles ifParameterField.KeyDown
        If e.KeyCode = Keys.Enter Then
            Me.ifParameterField_Leave(Nothing, Nothing)
        End If
    End Sub

    Private Sub ResetButtons()
        generateWorkflowButton.Enabled = True
        compileWorkflowButton.Enabled = False
        startWorkflowButton.Enabled = False
    End Sub

    Private Sub nextStepButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nextStepButton.Click
        If Me.ValidateSelections(CType(tabControl1.SelectedIndex + 1, WizardStep)) Then
            If tabControl1.SelectedIndex <> tabControl1.TabCount - 1 Then
                tabControl1.SelectedIndex = tabControl1.SelectedIndex + 1
            End If
        End If
    End Sub

    Private Sub tabControl1_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tabControl1.SelectedIndexChanged
        nextStepButton.Enabled = (tabControl1.SelectedIndex <> 3)
    End Sub

    Private Function ValidateSelections(ByVal wizardStep As WizardStep) As Boolean
        If Me.folderTypeSelection = FolderType.None And CInt(wizardStep) > 0 Then
            MessageBox.Show(Me, "You need to select the Outlook folder.", Me.Text, MessageBoxButtons.OK, MessageBoxIcon.Error)
            tabControl1.SelectTab(0)
            inboxRadioButton.Checked = False
            Return False
        End If
        If Me.ifConditionTypeSelection = IfConditionType.None And CInt(wizardStep) > 1 Then
            MessageBox.Show(Me, "You need to select the IF condition.", Me.Text, MessageBoxButtons.OK, MessageBoxIcon.Error)
            tabControl1.SelectTab(1)
            Return False
        End If
        If Me.actionTypeSelection = ActionTypes.None And CInt(wizardStep) > 2 Then
            MessageBox.Show(Me, "You need to select at least one action.", Me.Text, MessageBoxButtons.OK, MessageBoxIcon.Error)
            tabControl1.SelectTab(2)
            Return False
        End If
        Return True
    End Function

    Private Sub generateWorkflowButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles generateWorkflowButton.Click
        If Not ValidateSelections(WizardStep.Action) Then
            Return
        End If
        generateWorkflowButton.Enabled = False
        Dim cursor As Cursor = Me.Cursor
        Me.Cursor = Cursors.WaitCursor
        Try
            ' Save the workflow first, and capture the filePath of the workflow
            Me.workflowDesigner.XamlFile = "CustomOutlookWorkflow.xoml"
            Me.workflowDesigner.PerformSave()
            Me.xamlFile = Me.workflowDesigner.XamlFile

            Dim doc As XmlDocument = New XmlDocument()
            doc.Load(Me.workflowDesigner.XamlFile)
            Dim attrib As XmlAttribute = doc.CreateAttribute("x", "Class", "http://schemas.microsoft.com/winfx/2006/xaml")
            attrib.Value = Me.GetType().Namespace + ".CustomOutlookWorkflow"
            doc.DocumentElement.Attributes.Append(attrib)
            doc.Save(Me.workflowDesigner.XamlFile)

            compileWorkflowButton.Enabled = True
            MessageBox.Show(Me, "Workflow generated successfully. Generated Workflow XAML file:" + vbLf + Path.Combine(Path.GetDirectoryName(Me.GetType().Assembly.Location), xamlFile), Me.Text, MessageBoxButtons.OK, MessageBoxIcon.Information)
        Catch
            generateWorkflowButton.Enabled = True
        Finally
            Me.Cursor = cursor
        End Try
    End Sub

    Private Sub compileWorkflowButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles compileWorkflowButton.Click
        If String.IsNullOrEmpty(Me.xamlFile) Then
            Return
        End If
        If Not File.Exists(Me.xamlFile) Then
            MessageBox.Show(Me, "Cannot locate Workflow XAML file: " + Path.Combine(Path.GetDirectoryName(Me.GetType().Assembly.Location), xamlFile), Me.Text, MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If
        compileWorkflowButton.Enabled = False
        Dim cursor As Cursor = Me.Cursor
        Me.Cursor = Cursors.WaitCursor
        Try
            ' Compile the workflow
            Dim assemblyNames() As String = {"ReadEmailActivity.dll"}

            Dim compiler As WorkflowCompiler = New WorkflowCompiler()
            Dim parameters As WorkflowCompilerParameters = New WorkflowCompilerParameters(assemblyNames)
            parameters.LibraryPaths.Add(Path.GetDirectoryName(GetType(BaseMailbox).Assembly.Location))
            parameters.OutputAssembly = "CustomOutlookWorkflow" + Guid.NewGuid().ToString() + ".dll"
            results = compiler.Compile(parameters, Me.xamlFile)

            Dim errors As StringBuilder = New StringBuilder()
            Dim compilerError As CompilerError
            For Each compilerError In results.Errors
                errors.Append(compilerError.ToString() + vbLf)
            Next

            If errors.Length <> 0 Then
                MessageBox.Show(Me, errors.ToString(), Me.Text, MessageBoxButtons.OK, MessageBoxIcon.Error)
                compileWorkflowButton.Enabled = True
            Else
                MessageBox.Show(Me, "Workflow compiled successfully. Compiled assembly:" + vbLf + results.CompiledAssembly.GetName().ToString(), Me.Text, MessageBoxButtons.OK, MessageBoxIcon.Information)
                startWorkflowButton.Enabled = True
            End If
        Finally
            Me.Cursor = cursor
        End Try
    End Sub

    Private Sub startWorkflowButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles startWorkflowButton.Click
        startWorkflowButton.Enabled = False
        ' Start the runtime engine
        Dim workflowRuntime As WorkflowRuntime = New WorkflowRuntime()
        workflowRuntime.StartRuntime()
        ' Add workflow event handlers
        AddHandler workflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
        ' Start the workflow
        workflowRuntime.CreateWorkflow(results.CompiledAssembly.GetType(Me.GetType().Namespace + ".CustomOutlookWorkflow")).Start()
    End Sub

    Private Sub OnWorkflowCompleted(ByVal sender As System.Object, ByVal e As WorkflowCompletedEventArgs)
        MessageBox.Show("The Workflow Completed")
    End Sub

    Enum FolderType
        None = 0
        Inbox = 1
        Sent = 2
        Outbox = 3
    End Enum

    Enum IfConditionType
        None = 0
        Subject = 1
        FromEmail = 2
        [To] = 3
        CC = 4
        Bcc = 5
    End Enum

    <Flags()> _
    Enum ActionTypes
        None = 0
        SendAutoReply = 1
        CreateNote = 2
        CreateTask = 4
    End Enum

    Enum WizardStep
        None = 0
        Folder = 1
        Condition = 2
        Action = 3
    End Enum
End Class
