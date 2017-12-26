'---------------------------------------------------------------------
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
'---------------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System.ComponentModel.Design
Imports System.IO
Imports System.Reflection
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime.Tracking


Friend Class WorkflowTrackingProfileDesignerForm
    Dim profileManager As New TrackingProfileManager()

    Public Sub New()
        InitializeComponent()

        Me.conditionEqualityButton.Tag = System.Workflow.Runtime.Tracking.ComparisonOperator.Equals
        Me.toolStrip1.Enabled = False
        Me.conditionToolStrip.Visible = False
        Me.annotationToolStrip.Visible = False
        eventsDropDown.DropDown = New EnumDropDown(GetType(ActivityExecutionStatus))
        workflowEventsDropDown.DropDown = New EnumDropDown(GetType(TrackingWorkflowEvent))

        AddHandler workflowEventsDropDown.DropDown.ItemClicked, AddressOf Me.OnWorkflowEventClicked
        AddHandler eventsDropDown.DropDown.ItemClicked, AddressOf Me.OnActivityEventClicked

        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf CurrentDomainAssemblyResolve)
    End Sub

#Region "Assembly Resolve"

    ' This assembly resolver will get called when the user tries to load an assembly 
    ' that references other assemblies, and the other assemblies cannot be found 
    ' in the default search paths.  A file open dialog is shown to allow the user 
    ' to select the referenced assemblies.

    Private Function CurrentDomainAssemblyResolve(ByVal sender As Object, ByVal args As ResolveEventArgs) As Assembly
        Try
            MessageBox.Show(String.Format("The required assembly either cannot be found, or references additional assemblies.  " + vbCrLf + " Please find and select the assembly '{0}'.", args.Name), String.Format("Referenced Assembly '{0}' not found", args.Name))

            openAssemblyDialog.Title = String.Format("Open assembly {0}", args.Name)
            Dim res As DialogResult = openAssemblyDialog.ShowDialog()
            If res = Windows.Forms.DialogResult.OK Then
                Dim asm As Assembly = Assembly.LoadFrom(openAssemblyDialog.FileName)
                If Not asm.FullName.Equals(args.Name) Then
                    MessageBox.Show(String.Format("The assembly specified does not match the assembly requested, '{0}'", args.Name), "Error Loading Assembly")
                    Return CurrentDomainAssemblyResolve(sender, args)
                End If
                Return asm
            End If

            If res = Windows.Forms.DialogResult.Cancel Then
                Return Nothing
            End If


        Catch ex As Exception
            MessageBox.Show("There was an error loading the assembly: " + ex.Message, "Error Loading Assembly")
        End Try
        Return Nothing

    End Function
#End Region
#Region "Menu Events"
    Private Sub ExitToolStripMenuItemClick(ByVal sender As Object, ByVal e As EventArgs) Handles exitToolStripMenuItem.Click
        Application.Exit()
    End Sub

    Private Sub SaveProfileToolStripMenuItemClick(ByVal sender As Object, ByVal e As EventArgs) Handles profileAsFileToolStripMenuItem.Click
        Dim result As DialogResult = saveFileDialog1.ShowDialog()
        If Not result = Windows.Forms.DialogResult.OK Then
            Return
        End If
        Try
            Using writer As New StreamWriter(saveFileDialog1.FileName, False)
                profileManager.SerializeProfile(writer)
            End Using
        Catch ex As Exception
            MessageBox.Show("There were errors serializing the tracking profile. " + ex.Message)
            Return
        End Try
    End Sub

    Private Sub LoadProfileToolStripMenuItemClick(ByVal sender As Object, ByVal e As EventArgs) Handles profileToolStripMenuItem.Click
        If profileManager Is Nothing Or profileManager.TrackingProfile Is Nothing Then
            MessageBox.Show("Please load a workflow before attempting to load a profile.")
            Return
        End If

        Dim result As DialogResult = openProfileDialog.ShowDialog()
        If Not result = Windows.Forms.DialogResult.OK Or Not openProfileDialog.CheckFileExists Then
            Return
        End If

        Try
            profileManager.ReadProfile(openProfileDialog.FileName)
        Catch ex As Exception
            MessageBox.Show("Error reading tracking profile: " + ex.Message)
            Return
        End Try
    End Sub

    Private Sub LoadWorkflowToolStripMenuItemClick(ByVal sender As Object, ByVal e As EventArgs) Handles fromFileToolStripMenuItem.Click
        Dim assemblyToLoad As Assembly
        Dim result As DialogResult = openWorkflowDialog.ShowDialog()
        If Not result = Windows.Forms.DialogResult.OK Or Not openWorkflowDialog.CheckPathExists Or openWorkflowDialog.FileName Is Nothing Or openWorkflowDialog.FileName.Length <= 0 Then
            Return
        End If
        Try
            assemblyToLoad = Assembly.LoadFrom(openWorkflowDialog.FileName)
        Catch
            MessageBox.Show("Error loading assembly " + openWorkflowDialog.FileName)
            Return
        End Try
        Dim workflows As New List(Of Type)()
        Dim selectedWorkflow As Type

        For Each t As Type In assemblyToLoad.GetTypes()
            If (t.IsSubclassOf(GetType(Activity))) Then
                workflows.Add(t)
            End If
        Next
        If (workflows.Count = 0) Then
            MessageBox.Show("No workflows found!")
            Return
        ElseIf (workflows.Count = 1) Then
            selectedWorkflow = workflows(0)
        Else 'Multiple workflows to choose from - show selection dialog
            Dim selectionForm As New WorkflowSelectionForm()
            selectionForm.SetWorkflowTypes(workflows)
            selectionForm.ShowDialog()
            selectedWorkflow = selectionForm.SelectedWorkflow
        End If

        If (selectedWorkflow IsNot Nothing) Then
            InitializeProfileDesigner(selectedWorkflow, Nothing)
        End If

    End Sub

    Sub InitializeProfileDesigner(ByVal workflowType As Type, ByVal profile As TrackingProfile)

        Me.toolStrip1.Visible = True
        If workflowType IsNot Nothing Then
            profileManager = New TrackingProfileManager()
            If profile IsNot Nothing Then
                profileManager.TrackingProfile = profile
            End If
            Me.WorkflowDesignerControl1.WorkflowType = workflowType
        End If

        AddHandler Me.WorkflowDesignerControl1.SelectionChanged, AddressOf Me.OnActivitySelected

        'Once a workflow is loaded, ensure our glyph provider is added
        Dim glyphManager As IDesignerGlyphProviderService = _
            TryCast(Me.WorkflowDesignerControl1.GetService(GetType(IDesignerGlyphProviderService)), IDesignerGlyphProviderService)
        If glyphManager Is Nothing Then
            MessageBox.Show("There was an error loading the workflow type " + workflowType.AssemblyQualifiedName)
            Return
        End If
        glyphManager.AddGlyphProvider(New TrackingGlyphProvider(profileManager))

        workflowTypeValue = workflowType
    End Sub


#End Region
#Region "Properties"
    Dim selectedActivityValue As Activity = Nothing


    ' The currently selected activity

    Public Property SelectedActivity() As Activity
        Get
            Return selectedActivityValue
        End Get
        Set(ByVal value As Activity)
            selectedActivityValue = value
        End Set
    End Property

    ' Gets the current root workflow activity
    Public ReadOnly Property RootActivity() As Activity
        Get
            Dim root As Activity = SelectedActivity
            While root IsNot Nothing AndAlso root.Parent IsNot Nothing
                root = root.Parent
            End While
            Return root
        End Get
    End Property

    Dim workflowTypeValue As Type

    ' The current workflow
    Public Property WorkflowType() As Type
        Get
            Return workflowTypeValue
        End Get
        Set(ByVal value As Type)
            workflowTypeValue = Value
        End Set
    End Property

#End Region
#Region "Refresh"
    Public Sub RefreshView()
        workflowDesignerControl1.Refresh()
        RefreshToolStrip()
        RefreshExtractDropDown()
        RefreshEventsDropDown()
        RefreshConditionsDropDown()
        RefreshMarkupTab()
    End Sub

    Sub RefreshToolStrip()
        If SelectedActivity Is Nothing Then
            Me.toolStrip1.Enabled = False
            Return
        End If

        Me.toolStrip1.Enabled = True
        trackButton.Visible = True
        workflowEventsDropDown.Visible = False
        trackButton.Checked = profileManager.IsTracked(SelectedActivity)
        matchDerivedTypes.Checked = profileManager.MatchesDerivedTypes(SelectedActivity)
        conditionsDropDown.Visible = trackButton.Checked
        matchDerivedTypes.Visible = trackButton.Checked
        eventsSeparator.Visible = trackButton.Checked
        eventsDropDown.Visible = trackButton.Checked
        annotateButton.Visible = trackButton.Checked
        extractDropDown.Visible = trackButton.Checked
        conditionMemberDropDown.DropDown = GetExtractableMembers(SelectedActivity, New EventHandler(AddressOf OnConditionMemberClicked), False)
        conditionMemberDropDown.DropDown.Text = "Choose member..."
        trackButton.Text = IIf(SelectedActivity Is Nothing, "Track", String.Format("Track {0}", SelectedActivity.GetType().Name))
        If SelectedActivity Is RootActivity Then
            workflowEventsDropDown.Visible = True
            RefreshWorkflowEvents()
        End If
    End Sub
#End Region
#Region "Conditions"
    Private Sub OnConditionMemberClicked(ByVal sender As Object, ByVal e As EventArgs)
        Dim button As ToolStripButton = TryCast(sender, ToolStripButton)
        button.Owner.Name = button.Owner.Text = button.Text
    End Sub

    Private Sub ConditionCancelButtonClick(ByVal sender As Object, ByVal e As EventArgs) Handles conditionCancelButton.Click
        conditionToolStrip.Visible = False
        conditionsDropDown.Enabled = True
        ResetConditionStrip()
    End Sub

    Private Sub ConditionMemberDropDownDropDownItemClicked(ByVal sender As Object, ByVal e As ToolStripItemClickedEventArgs) Handles conditionMemberDropDown.DropDownItemClicked
        conditionMemberDropDown.Name = e.ClickedItem.Text
        conditionMemberDropDown.Text = e.ClickedItem.Text
        ValidateCondition()
    End Sub

    Private Sub ConditionEqualityButtonClick(ByVal sender As Object, ByVal e As EventArgs) Handles conditionEqualityButton.Click
        Dim button As ToolStripButton = TryCast(sender, ToolStripButton)
        If button IsNot Nothing Then
            If button.Text.Equals("==") Then
                button.Tag = ComparisonOperator.NotEquals
                button.Text = "!="
            Else
                button.Tag = ComparisonOperator.Equals
                button.Text = "=="
            End If
        End If
    End Sub

    Sub ValidateCondition()
        conditionSaveButton.Enabled = True
        If conditionMemberDropDown.Text.Contains(" ") Then conditionSaveButton.Enabled = False
    End Sub

    Private Sub ConditionSaveButtonClick(ByVal sender As Object, ByVal e As EventArgs) Handles conditionSaveButton.Click
        Dim condition As ActivityTrackingCondition = TryCast(conditionToolStrip.Tag, ActivityTrackingCondition)
        profileManager.SaveTrackingCondition( _
            SelectedActivity, _
            condition, _
            conditionMemberDropDown.Text, _
            CType(conditionEqualityButton.Tag, ComparisonOperator), _
            conditionValue.Text)

        conditionToolStrip.Tag = condition
        conditionToolStrip.Visible = False
        conditionsDropDown.Enabled = True
        ResetConditionStrip()
        RefreshConditionsDropDown()
    End Sub


    Private Sub ConditionsDropDownDropDownItemClicked(ByVal sender As Object, ByVal e As ToolStripItemClickedEventArgs) Handles conditionsDropDown.DropDownItemClicked
        conditionToolStrip.Tag = e.ClickedItem.Tag
        conditionsDropDown.Enabled = False
        conditionToolStrip.Visible = True
        ResetConditionStrip()
        If TypeOf conditionToolStrip.Tag Is ActivityTrackingCondition Then
            conditionMemberDropDown.Name = CType(conditionToolStrip.Tag, ActivityTrackingCondition).Member
            conditionMemberDropDown.Text = CType(conditionToolStrip.Tag, ActivityTrackingCondition).Member
            conditionValue.Text = CType(conditionToolStrip.Tag, ActivityTrackingCondition).Value
        End If
        ValidateCondition()
    End Sub

    Private Sub ConditionDeleteButtonClick(ByVal sender As Object, ByVal e As EventArgs) Handles conditionDeleteButton.Click
        profileManager.DeleteTrackingCondition(SelectedActivity, CType(conditionToolStrip.Tag, ActivityTrackingCondition))
        conditionsDropDown.Enabled = True
        conditionToolStrip.Visible = False
        Me.RefreshConditionsDropDown()
    End Sub

    Sub ResetConditionStrip()
        conditionMemberDropDown.Text = "Select member"
        conditionEqualityButton.Name = "=="
        conditionEqualityButton.Text = "=="
        conditionValue.Text = String.Empty
    End Sub

    Sub RefreshConditionsDropDown()
        conditionsDropDown.DropDownItems.Clear()
        conditionsDropDown.DropDownItems.Add("Add new condition...")
        Dim conditions As TrackingConditionCollection = profileManager.GetTrackingConditions(SelectedActivity)
        If conditions IsNot Nothing Then
            conditionsDropDown.DropDownItems.Add(New ToolStripSeparator())
            For Each condition As ActivityTrackingCondition In conditions
                Dim button As New ToolStripButton()
                Dim buttonText As String = String.Format("{0} {1} {2}", _
                        condition.Member, _
                        IIf(condition.Operator = ComparisonOperator.Equals, "==", "!="), _
                        condition.Value)
                button.Name = buttonText
                button.Text = buttonText
                Button.Tag = condition
                conditionsDropDown.DropDownItems.Add(Button)
            Next
        End If
    End Sub
#End Region
#Region "Events"
    Private Sub WorkflowEventsDropDownClick(ByVal sender As Object, ByVal e As EventArgs) Handles workflowEventsDropDown.Click
        TryCast(workflowEventsDropDown.DropDown, EnumDropDown).UpdateToggleAll()
    End Sub

    Private Sub EventsDropDownClick(ByVal sender As Object, ByVal e As EventArgs) Handles eventsDropDown.Click
        TryCast(eventsDropDown.DropDown, EnumDropDown).UpdateToggleAll()
    End Sub

    Private Sub OnActivityEventClicked(ByVal sender As Object, ByVal e As ToolStripItemClickedEventArgs)
        Dim button As ToolStripButton = TryCast(e.ClickedItem, ToolStripButton)
        If button IsNot Nothing Then
            profileManager.ToggleEventStatus( _
                SelectedActivity, _
                CType(System.Enum.Parse(GetType(ActivityExecutionStatus), button.Name), ActivityExecutionStatus))
        End If
        WorkflowDesignerControl1.Refresh()
    End Sub

    Private Sub OnWorkflowEventClicked(ByVal sender As Object, ByVal e As ToolStripItemClickedEventArgs)
        Dim button As ToolStripButton = TryCast(e.ClickedItem, ToolStripButton)
        If button IsNot Nothing Then
            profileManager.ToggleEventStatus(CType(System.Enum.Parse(GetType(TrackingWorkflowEvent), button.Name), TrackingWorkflowEvent))
        End If
        WorkflowDesignerControl1.Refresh()
    End Sub

    Sub RefreshWorkflowEvents()
        If profileManager.WorkflowTrackPoint IsNot Nothing Then
            Dim wtl As WorkflowTrackingLocation = profileManager.WorkflowTrackPoint.MatchingLocation
            For Each we As TrackingWorkflowEvent In wtl.Events
                If Me.eventsDropDown.DropDownItems.ContainsKey(we.ToString()) Then
                    CType(Me.eventsDropDown.DropDownItems(we.ToString()), ToolStripButton).Checked = True
                End If
            Next
        End If
    End Sub


    Public Sub RefreshEventsDropDown()
        Dim trackPoint As ActivityTrackPoint = profileManager.GetTrackPointForActivity(SelectedActivity)
        If trackPoint IsNot Nothing Then
            If (trackPoint.MatchingLocations.Count > 0) Then
                Dim atl As ActivityTrackingLocation = trackPoint.MatchingLocations(0)
                For Each status As ActivityExecutionStatus In atl.ExecutionStatusEvents
                    If Me.eventsDropDown.DropDownItems.ContainsKey(status.ToString()) Then
                        CType(Me.eventsDropDown.DropDownItems(status.ToString()), ToolStripButton).Checked = True
                    End If
                Next
            End If
        End If
    End Sub

    Sub RefreshMarkupTab()
        If profileManager IsNot Nothing AndAlso profileManager.TrackingProfile IsNot Nothing Then
            Try
                If profileManager.TrackingProfile IsNot Nothing Then
                    profileMarkup.Text = profileManager.SerializeProfile()
                End If
            Catch ex As Exception
                profileMarkup.Text = String.Format("Could not serialize tracking profile: {0}", ex.Message)
            End Try
        End If
    End Sub

    Private Sub TabControl1Selected(ByVal sender As Object, ByVal e As TabControlEventArgs) Handles tabControl1.Selected
        RefreshMarkupTab()
    End Sub

#End Region
#Region "Extracts"

    Public Function GetExtractableMembers(ByVal activity As Activity, ByVal onClick As EventHandler, ByVal checkOnClick As Boolean) As ToolStripDropDown
        Return GetExtractableMembers(activity, onClick, checkOnClick, True)
    End Function

    Public Function SortMembers(ByVal lhs As MemberInfo, ByVal rhs As MemberInfo) As Integer
        Return lhs.Name.CompareTo(rhs.Name)
    End Function

    ' Gets a tool strip drop down containing a clickable set of extract members
    Public Function GetExtractableMembers(ByVal activity As Activity, ByVal onClick As EventHandler, ByVal checkOnClick As Boolean, ByVal activityExtract As Boolean) As ToolStripDropDown
        Dim dropDown As New ToolStripDropDown()

        Dim properties As PropertyInfo() = activity.GetType().GetProperties()
        Dim fields As FieldInfo() = activity.GetType().GetFields()
        Dim members As New List(Of MemberInfo)(properties)
        members.AddRange(fields)
        members.Sort(AddressOf SortMembers)

        For Each member As MemberInfo In members
            Dim memberItem As New ToolStripButton()
            memberItem.Name = member.Name
            memberItem.Text = member.Name
            memberItem.CheckOnClick = checkOnClick
            memberItem.Tag = activityExtract
            If onClick IsNot Nothing Then
                AddHandler memberItem.Click, onClick
            End If
            dropDown.Items.Add(memberItem)
        Next
        Return dropDown
    End Function


    Private Sub OnExtractItemClicked(ByVal sender As Object, ByVal e As EventArgs)
        Dim button As ToolStripButton = TryCast(sender, ToolStripButton)
        profileManager.ToggleExtract(SelectedActivity, CType(button.Tag, Boolean), button.Name)
    End Sub

    Sub RefreshExtractDropDown()
        Me.activityMembersToolStripMenuItem.DropDown = GetExtractableMembers(SelectedActivity, New EventHandler(AddressOf OnExtractItemClicked), True, True)
        Me.activityMembersToolStripMenuItem.Text = SelectedActivity.GetType().Name
        Me.workflowMembersToolStripMenuItem.DropDown = GetExtractableMembers(RootActivity, New EventHandler(AddressOf OnExtractItemClicked), True, False)
        'Only display both activity and workflow members if Me is not the root activity; otherwise only display the activity members
        Me.workflowMembersToolStripMenuItem.Visible = (SelectedActivity IsNot RootActivity)

        Dim trackPoint As ActivityTrackPoint = profileManager.GetTrackPointForActivity(SelectedActivity)
        If trackPoint IsNot Nothing Then
            For Each te As TrackingExtract In trackPoint.Extracts
                If (Me.extractDropDown.DropDownItems.ContainsKey(te.Member)) Then
                    CType(Me.extractDropDown.DropDownItems(te.Member), ToolStripButton).Checked = True
                End If
            Next
        End If
    End Sub
#End Region
#Region "Annotation"
    Private Sub AnnotateSaveButtonClick(ByVal sender As Object, ByVal e As EventArgs) Handles annotateSaveButton.Click
        profileManager.SetAnnotation(SelectedActivity, annotationText.Text)
        annotateButton.Enabled = True
        annotationToolStrip.Visible = False
    End Sub

    Private Sub AnnotateCancelButtonClick(ByVal sender As Object, ByVal e As EventArgs) Handles annotateCancelButton.Click
        annotateButton.Enabled = True
        annotationToolStrip.Visible = False
    End Sub
#End Region
#Region "Activity Selection"
    Sub CleanupLastActivity()
        'Cleanup Annotation
        Me.annotationText.Text = String.Empty
        Me.annotationToolStrip.Visible = False
        Me.annotateButton.Enabled = True

        'Cleanup Conditions
        Me.conditionToolStrip.Visible = False
        Me.conditionsDropDown.Enabled = True

        'Reset the drop downs            
        TryCast(workflowEventsDropDown.DropDown, EnumDropDown).Reset()
        TryCast(eventsDropDown.DropDown, EnumDropDown).Reset()
    End Sub

    Private Sub OnActivitySelected(ByVal sender As Object, ByVal e As EventArgs)
        Dim selectionService As ISelectionService = TryCast(Me.WorkflowDesignerControl1.GetService(GetType(ISelectionService)), ISelectionService)
        If selectionService IsNot Nothing Then
            SelectedActivity = TryCast(selectionService.PrimarySelection, Activity)
            If SelectedActivity IsNot Nothing Then
                CleanupLastActivity()
                RefreshView()
            End If
        End If
    End Sub
#End Region
#Region "Toolstrip"
    Private Sub TrackButtonClick(ByVal sender As Object, ByVal e As EventArgs) Handles trackButton.Click
        If SelectedActivity IsNot Nothing Then
            CleanupLastActivity()
            profileManager.ToggleActivityTrackPoint(SelectedActivity)
            RefreshView()
        End If
    End Sub

    Private Sub AnnotateButtonClick(ByVal sender As Object, ByVal e As EventArgs) Handles annotateButton.Click
        annotationText.Text = profileManager.GetAnnotation(SelectedActivity)
        annotationToolStrip.Visible = True
        annotateButton.Enabled = False
    End Sub

    Private Sub MatchDerivedTypesClick(ByVal sender As Object, ByVal e As EventArgs) Handles matchDerivedTypes.Click
        Dim trackPoint As ActivityTrackPoint = profileManager.GetTrackPointForActivity(SelectedActivity)
        If trackPoint IsNot Nothing AndAlso trackPoint.MatchingLocations.Count > 0 Then
            matchDerivedTypes.Checked = Not trackPoint.MatchingLocations(0).MatchDerivedTypes
            trackPoint.MatchingLocations(0).MatchDerivedTypes = Not trackPoint.MatchingLocations(0).MatchDerivedTypes
            RefreshView()
        End If
    End Sub
#End Region

#Region "Sql Events"
    ' Saves a tracking profile to the Sql store
    Private Sub SaveProfileToSqlClick(ByVal sender As Object, ByVal e As EventArgs) Handles toSqlTrackingStoreToolStripMenuItem.Click
        Dim profileStore As New TrackingProfileStore()
        Try
            If profileManager Is Nothing Then Throw New ApplicationException("Need to load a profile first")
            profileManager.SerializeProfile()
        Catch ex As Exception
            MessageBox.Show(ex.Message, "Error serializing profile")
            Return
        End Try
        Dim errorMessage As String = Nothing
        If Not profileStore.ValidateConnection(errorMessage) Then
            MessageBox.Show("Error establishing connection to SQL: " + errorMessage, "Error connecting to SQL")
            Return
        End If
        profileStore.SaveProfile(WorkflowType, profileManager.TrackingProfile)
    End Sub


    Private Sub FromSqlToolStripMenuItemClick(ByVal sender As Object, ByVal e As EventArgs) Handles fromSqlToolStripMenuItem.Click
        Dim profileStore As New TrackingProfileStore()
        Dim errorMessage As String = Nothing
        If Not profileStore.ValidateConnection(errorMessage) Then
            MessageBox.Show("Error establishing connection to SQL: " + errorMessage, "Error connecting to SQL")
            Return
        End If
        Dim workflowType As Type = Nothing
        Dim profile As TrackingProfile = Nothing
        profileStore.LoadWorkflowAndProfile(workflowType, profile)
        If workflowType IsNot Nothing AndAlso profile IsNot Nothing Then
            workflowType = workflowType
            profileManager.TrackingProfile = profile
            InitializeProfileDesigner(workflowType, profile)
        End If
    End Sub

#End Region
End Class
