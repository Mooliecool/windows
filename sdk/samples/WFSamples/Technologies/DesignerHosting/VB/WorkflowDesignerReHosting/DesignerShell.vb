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
Imports System.ComponentModel.Design
Imports System.Drawing
Imports System.Windows.Forms
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.ComponentModel.Compiler
Imports Microsoft.VisualBasic

Public Class DesignerShell
    Private workflowPanel As WorkflowViewPanel

    Public Sub New()
        ' This call is required by the Windows Form Designer.
        InitializeComponent()

        ' Now initialize the contained components
        Me.workflowPanel = New WorkflowViewPanel(Me)
        Me.workflowPanel.Dock = DockStyle.Fill
        Me.workflowPanel.Name = "WorkflowViewPanel"

        Me.Panel2.Controls.Add(Me.workflowPanel)

        ' Load the default workflow to start with
        Me.workflowPanel.LoadDefaultWorkflow()
    End Sub

    Private Sub zoomNumericUpDown_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles zoomNumericUpDown.ValueChanged
        If Me.workflowPanel IsNot Nothing Then
            Me.workflowPanel.OnZoomChanged(CInt(Me.zoomNumericUpDown.Value))
        End If
    End Sub

    Private Sub addButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles addButton.Click
        Dim viewId As Integer
        Dim rootDesigner As SequentialWorkflowRootDesigner
        rootDesigner = Me.workflowPanel.GetWorkflowView().RootDesigner
        viewId = rootDesigner.ActiveView.ViewId
        If viewId = 1 Then
            Me.workflowPanel.OnCodeActivityAdded()
        Else
            Dim resultBox As DialogResult
            resultBox = MessageBox.Show("This sample supports adding a code activity only in workflow view")
        End If
    End Sub

    Private Sub updateButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles updateButton.Click
        Me.workflowPanel.OnCodeActivityUpdated()
    End Sub
End Class

#Region "Class WorkflowViewPanel"

Public NotInheritable Class WorkflowViewPanel
    Inherits Panel
    Implements IServiceProvider

#Region "Members and Constructor"

    Private designSurface As WorkflowDesignSurface
    Private workflowView As WorkflowView
    Private rootActivity As SequentialWorkflowActivity
    Private designerHost As IDesignerHost
    Private parentValue As DesignerShell

    Public Sub New(ByVal parentValue As DesignerShell)
        Me.parentValue = parentValue
    End Sub

#End Region

#Region "Methods"

    Public Function GetWorkflowView() As WorkflowView
        Return Me.workflowView
    End Function

    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            Clear()
        End If
        MyBase.Dispose(disposing)
    End Sub

    Protected Overrides Function GetService(ByVal serviceType As System.Type) As Object
        If Me.designSurface IsNot Nothing Then
            Return Me.designSurface.GetService(serviceType)
        Else
            Return Nothing
        End If
    End Function

    ' Loads precreated workflow. The function is similar to the above function except
    ' instead of creating empty workflow we create workflow with contents

    Friend Sub LoadDefaultWorkflow()
        Clear()

        Me.designSurface = New WorkflowDesignSurface(Me)
        Dim loader As WorkflowLoader = New WorkflowLoader()
        designSurface.BeginLoad(loader)

        designerHost = CType(GetService(GetType(IDesignerHost)), IDesignerHost)
        If Not designerHost Is Nothing Then
            rootActivity = CType(designerHost.CreateComponent(GetType(SequentialWorkflowActivity)), SequentialWorkflowActivity)
            rootActivity.Name = "Service1"

            Dim codeActivity1 As CodeActivity = New CodeActivity()
            rootActivity.Activities.Add(codeActivity1)
            designerHost.RootComponent.Site.Container.Add(codeActivity1)

            Me.workflowView = New WorkflowView(CType(Me.designSurface, IServiceProvider))
            Me.workflowView.AddDesignerMessageFilter(New CustomMessageFilter(CType(Me.designSurface, IServiceProvider), Me.workflowView, loader))

            Controls.Add(Me.workflowView)
            designerHost.Activate()

            Dim selectionService As ISelectionService = CType(GetService(GetType(ISelectionService)), ISelectionService)
            If Not selectionService Is Nothing Then
                AddHandler selectionService.SelectionChanged, New EventHandler(AddressOf OnSelectionChanged)
                Dim selection() As IComponent = New IComponent() {rootActivity}
                selectionService.SetSelectedComponents(selection)
            End If
        End If
    End Sub

    Private Function GetSelectedActivity() As CodeActivity
        Dim selectionService As ISelectionService = CType(GetService(GetType(ISelectionService)), ISelectionService)

        If selectionService IsNot Nothing AndAlso selectionService.SelectionCount <> 0 Then
            Dim selection() As Object = New Object(selectionService.SelectionCount) {}
            selectionService.GetSelectedComponents().CopyTo(selection, 0)

            If selection(0) IsNot Nothing AndAlso TypeOf selection(0) Is IComponent Then
                If TypeOf selection(0) Is CodeActivity Then
                    Return CType(selection(0), CodeActivity)
                End If
            End If
        End If

        Return Nothing
    End Function

    Private Sub OnSelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        parentValue.nameTextBox.Clear()
        parentValue.descriptionTextBox.Clear()
        parentValue.nameTextBox.ReadOnly = True
        parentValue.descriptionTextBox.ReadOnly = True
        parentValue.updateButton.Enabled = False

        Dim codeActivity As CodeActivity = GetSelectedActivity()
        If Not codeActivity Is Nothing Then
            parentValue.nameTextBox.Text = codeActivity.Name
            parentValue.descriptionTextBox.Text = codeActivity.Description
            parentValue.nameTextBox.ReadOnly = False
            parentValue.descriptionTextBox.ReadOnly = False
            parentValue.updateButton.Enabled = True
        End If
    End Sub

    Public Sub OnCodeActivityUpdated()
        Dim codeActivity As CodeActivity = GetSelectedActivity()
        If Not codeActivity Is Nothing Then
            Try
                ' Using PropertyDescriptor to let a designer update the component name
                Dim propertyDescriptor As PropertyDescriptor = TypeDescriptor.GetProperties(codeActivity)("Name")
                propertyDescriptor.SetValue(codeActivity, Me.parentValue.nameTextBox.Text)
                codeActivity.Description = Me.parentValue.descriptionTextBox.Text
            Catch ex As Exception
                MessageBox.Show(Me.Parent, ex.Message, Me.Parent.Text, MessageBoxButtons.OK, MessageBoxIcon.Error)
            End Try
        End If
    End Sub

    Public Function OnCodeActivityAdded() As Boolean
        Dim codeActivity As CodeActivity = New CodeActivity()

        Dim activityPropertiesForm As ActivityProperties = New ActivityProperties()
        activityPropertiesForm.ShowDialog()
        If activityPropertiesForm.Result = DialogResult.OK Then
            For Each activity As Activity In Me.rootActivity.Activities()
                If activity.Name.Equals(activityPropertiesForm.ActivityName, StringComparison.OrdinalIgnoreCase) Then
                    MessageBox.Show(Me.Parent, "Cannot add new CodeActivity. The CodeActivity with name '" + activityPropertiesForm.ActivityName + "' already exists.", Me.Parent.Text, MessageBoxButtons.OK, MessageBoxIcon.Error)
                    Return False
                End If
            Next

            codeActivity.Name = activityPropertiesForm.ActivityName
            codeActivity.Description = activityPropertiesForm.Description

            Me.rootActivity.Activities.Add(codeActivity)
            Me.designerHost.RootComponent.Site.Container.Add(codeActivity)
            Me.workflowView.Update()
        End If

        Return True
    End Function

    Public Sub OnZoomChanged(ByVal zoomFactor As Integer)
        Me.workflowView.Zoom = zoomFactor
        Me.workflowView.Update()
    End Sub

    Private Sub Clear()
        If Me.designSurface Is Nothing Then
            Return
        Else
            Me.designSurface.Dispose()
        End If

        If Me.workflowView IsNot Nothing AndAlso Controls.Contains(Me.workflowView) Then
            Controls.Remove(Me.workflowView)
        End If
        Me.workflowView = Nothing
    End Sub

    Protected Overrides Sub OnLayout(ByVal levent As LayoutEventArgs)
        MyBase.OnLayout(levent)
        If Me.workflowView IsNot Nothing AndAlso Not Me.workflowView.IsDisposed Then
            Me.workflowView.Width = Me.Width
            Me.workflowView.Height = Me.Height
        End If
    End Sub

    Public Sub InvokeStandardCommand(ByVal cmd As CommandID)
        Try
            Dim menuService As IMenuCommandService = CType(GetService(GetType(IMenuCommandService)), IMenuCommandService)
            If Not menuService Is Nothing Then
                menuService.GlobalInvoke(cmd)
            End If
        Catch
            'We eat exceptions as some of the operations are not supported in samples
        End Try
    End Sub
#End Region

#Region "IServiceProvider Members"

    Private Function IServiceProvider_GetService(ByVal serviceType As System.Type) As Object Implements System.IServiceProvider.GetService
        Return GetService(serviceType)
    End Function

#End Region

End Class

#End Region

#Region "Class WorkflowDesignSurface"

' Design Surface is used to provide services.
Friend NotInheritable Class WorkflowDesignSurface
    Inherits DesignSurface

    Friend Sub New(ByVal serviceProvider As IServiceProvider)
        Me.ServiceContainer.AddService(GetType(IMenuCommandService), New MenuCommandService(Me.ServiceContainer))

        Dim typeProvider As TypeProvider = New TypeProvider(serviceProvider)
        typeProvider.AddAssemblyReference(GetType(String).Assembly.Location)
        Me.ServiceContainer.AddService(GetType(ITypeProvider), typeProvider, True)
    End Sub
End Class

#End Region

#Region "Class CustomMessageFilter"
' All Coordinates passed in physical coordinate system
' Some of the functions will have coordinates in screen coordinates ie ShowContextMenu
Friend NotInheritable Class CustomMessageFilter
    Inherits WorkflowDesignerMessageFilter

#Region "Members and Constructor"

    Private mouseDown As Boolean
    Private serviceProvider As IServiceProvider
    Private workflowView As WorkflowView
    Private loader As WorkflowDesignerLoader

    Public Sub New(ByVal provider As IServiceProvider, ByVal workflowView As WorkflowView, ByVal loader As WorkflowDesignerLoader)
        Me.serviceProvider = provider
        Me.workflowView = workflowView
        Me.loader = loader
    End Sub

#End Region

#Region "WorkflowDesignerMessageFilter Overridables"

    Protected Overrides Function OnMouseDown(ByVal eventArgs As System.Windows.Forms.MouseEventArgs) As Boolean
        ' Allow other components to process this event by not returning true.
        mouseDown = True
        Return False
    End Function

    Protected Overrides Function OnMouseMove(ByVal eventArgs As System.Windows.Forms.MouseEventArgs) As Boolean
        ' Allow other components to process this event by not returning true.
        If mouseDown Then
            workflowView.ScrollPosition = New Point(eventArgs.X, eventArgs.Y)
        End If
        Return False
    End Function

    Protected Overrides Function OnMouseUp(ByVal eventArgs As MouseEventArgs) As Boolean
        ' Allow other components to process this event by not returning true.
        mouseDown = False
        Return False
    End Function

    Protected Overrides Function OnMouseDoubleClick(ByVal eventArgs As MouseEventArgs) As Boolean
        mouseDown = False
        Return True
    End Function

    Protected Overrides Function OnMouseEnter(ByVal eventArgs As MouseEventArgs) As Boolean
        ' Allow other components to process this event by not returning true.
        mouseDown = False
        Return False
    End Function

    Protected Overrides Function OnMouseHover(ByVal eventArgs As MouseEventArgs) As Boolean
        ' Allow other components to process this event by not returning true.
        mouseDown = False
        Return False
    End Function

    Protected Overrides Function OnMouseLeave() As Boolean
        ' Allow other components to process this event by not returning true.
        mouseDown = False
        Return False
    End Function

    Protected Overrides Function OnMouseWheel(ByVal eventArgs As MouseEventArgs) As Boolean
        mouseDown = False
        Return True
    End Function

    Protected Overrides Function OnMouseCaptureChanged() As Boolean
        ' Allow other components to process this event by not returning true.
        mouseDown = False
        Return False
    End Function

    Protected Overrides Function OnDragEnter(ByVal eventArgs As DragEventArgs) As Boolean
        Return True
    End Function

    Protected Overrides Function OnDragOver(ByVal eventArgs As DragEventArgs) As Boolean
        Return True
    End Function

    Protected Overrides Function OnDragLeave() As Boolean
        Return True
    End Function

    Protected Overrides Function OnDragDrop(ByVal eventArgs As DragEventArgs) As Boolean
        Return True
    End Function

    Protected Overrides Function OnGiveFeedback(ByVal gfbevent As GiveFeedbackEventArgs) As Boolean
        Return True
    End Function

    Protected Overrides Function OnQueryContinueDrag(ByVal qcdevent As QueryContinueDragEventArgs) As Boolean
        Return True
    End Function

    Protected Overrides Function OnKeyDown(ByVal eventArgs As KeyEventArgs) As Boolean
        If eventArgs.KeyCode = Keys.Delete Then
            Dim selectionService As ISelectionService = CType(serviceProvider.GetService(GetType(ISelectionService)), ISelectionService)
            If selectionService IsNot Nothing AndAlso TypeOf selectionService.PrimarySelection Is CodeActivity Then
                Dim codeActivityComponent As CodeActivity = CType(selectionService.PrimarySelection, CodeActivity)
                Dim parentActivity As CompositeActivity = codeActivityComponent.Parent
                If parentActivity IsNot Nothing Then
                    parentActivity.Activities.Remove(codeActivityComponent)
                    Me.ParentView.Update()
                End If
                loader.RemoveActivityFromDesigner(codeActivityComponent)
            End If
        End If
        Return True
    End Function

    Protected Overrides Function OnKeyUp(ByVal eventArgs As KeyEventArgs) As Boolean
        Return True
    End Function

    Protected Overrides Function OnShowContextMenu(ByVal menuPoint As Point) As Boolean
        Return True
    End Function

#End Region

End Class

#End Region