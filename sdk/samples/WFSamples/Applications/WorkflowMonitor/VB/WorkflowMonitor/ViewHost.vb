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
Imports System.ComponentModel.Design.Serialization
Imports System.Windows.Forms
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design

' ViewHost "hosts" the workflow designer and graphically displays the workflow definition
Namespace WorkflowMonitor
    Public Class ViewHost
        Inherits Control

        Private loader As Loader = Nothing
        Private surface As WorkflowDesignSurface = Nothing
        Private parentValue As MonitorForm

        Private workflowViewValue As WorkflowView = Nothing

        Friend Event ZoomChanged As EventHandler(Of ZoomChangedEventArgs)

        Friend Sub New(ByVal parent As MonitorForm)
            Me.parentValue = parent
            Me.BackColor = System.Drawing.SystemColors.Window
            Me.Dock = DockStyle.Fill
            Me.Name = "viewHost"

            Initialize()

            SuspendLayout()
            ResumeLayout(True)

            Me.BackColor = SystemColors.Control
        End Sub

        ' Expand or collapse all composite activities
        Friend Sub Expand(ByVal expand As Boolean)
            Dim host As IDesignerHost = GetService(GetType(IDesignerHost))
            If host Is Nothing Then
                Return
            End If

            Me.SuspendLayout()

            Dim root As CompositeActivity = host.RootComponent
            Dim activity As Activity
            For Each activity In root.Activities
                Dim compositeActivityDesigner As CompositeActivityDesigner = host.GetDesigner(CType(activity, IComponent))
                If compositeActivityDesigner IsNot Nothing Then
                    compositeActivityDesigner.Expanded = expand
                End If
            Next

            Me.ResumeLayout(True)
        End Sub

        Friend ReadOnly Property WorkflowView() As WorkflowView
            Get
                Return Me.workflowViewValue
            End Get
        End Property

        Protected Overrides Sub Dispose(ByVal disposing As Boolean)
            If disposing Then
                Clear()
            End If
            MyBase.Dispose(disposing)
        End Sub

        Protected Overrides Function GetService(ByVal service As System.Type) As Object
            If Me.surface IsNot Nothing Then
                Return Me.surface.GetService(service)
            Else
                Return Nothing
            End If
        End Function

        ' Loads the workflow definition into the designer
        Friend Sub OpenWorkflow(ByVal workflowDefinition As Activity)
            Initialize()

            Dim host As IDesignerHost = CType(GetService(GetType(IDesignerHost)), IDesignerHost)

            If (host Is Nothing) Then Return

            If Me.WorkflowView IsNot Nothing Then
                If MyBase.Controls.Contains(Me.WorkflowView) Then
                    MyBase.Controls.Remove(Me.WorkflowView)
                End If
                Me.WorkflowView.Dispose()
                Me.workflowViewValue = Nothing
            End If

            Me.loader.WorkflowDefinition = workflowDefinition
            Me.surface.BeginLoad(Me.loader)

            Me.workflowViewValue = New WorkflowView(Me.surface)
            Dim glyphService As IDesignerGlyphProviderService = CType(Me.surface.GetService(GetType(IDesignerGlyphProviderService)), IDesignerGlyphProviderService)
            Dim glyphProvider As WorkflowMonitor.WorkflowMonitorDesignerGlyphProvider = New WorkflowMonitorDesignerGlyphProvider(parentValue.ActivityStatusList)
            glyphService.AddGlyphProvider(glyphProvider)

            workflowViewValue.Dock = DockStyle.Fill
            MyBase.Controls.Add(workflowViewValue)

            CType(host, IDesignerLoaderHost).EndLoad(host.RootComponent.Site.Name, True, Nothing)
        End Sub

        ' Initializes the designer setting up the services, surface, and loader
        Friend Sub Initialize()
            If Me.surface IsNot Nothing Then
                Me.surface.Dispose()
            End If

            Me.loader = New Loader()
            Me.surface = New WorkflowDesignSurface(New MemberCreationService())
        End Sub

        Friend Sub Clear()
            If Me.surface Is Nothing Then
                Return
            End If

            If Me.WorkflowView IsNot Nothing Then
                If MyBase.Controls.Contains(Me.WorkflowView) Then
                    MyBase.Controls.Remove(Me.WorkflowView)
                End If
                Me.WorkflowView.Dispose()
                Me.workflowViewValue = Nothing
            End If

            Me.surface.Dispose()
            Me.surface = Nothing
            Me.loader = Nothing
        End Sub

        Friend Sub HighlightActivity(ByVal activityName As String)
            Dim selectionService As ISelectionService = CType(surface.GetService(GetType(ISelectionService)), ISelectionService)
            Dim referenceService As IReferenceService = CType(surface.GetService(GetType(IReferenceService)), IReferenceService)
            If Not (selectionService Is Nothing) And Not (referenceService Is Nothing) Then
                Dim activityComponent As Activity = CType(referenceService.GetReference(activityName), Activity)
                If Not (activityComponent Is Nothing) Then
                    Dim components As List(Of IComponent) = New List(Of IComponent)
                    components.Add(activityComponent)
                    selectionService.SetSelectedComponents(components)
                End If
            End If
        End Sub
    End Class

    Public Class ZoomChangedEventArgs
        Inherits EventArgs
        Private zoomValue As Int32

        Property Zoom() As Int32
            Get
                Return zoomValue
            End Get
            Set(ByVal value As Int32)
                zoomValue = value
            End Set
        End Property
        Public Sub New(ByVal zoom As Int32)
            zoomValue = zoom
        End Sub
    End Class

    Public Class MemberCreationService
        Implements IMemberCreationService

        Public Sub CreateEvent(ByVal className As String, ByVal eventName As String, ByVal eventType As System.Type, ByVal attributes() As System.Workflow.ComponentModel.Compiler.AttributeInfo, ByVal emitDependencyProperty As Boolean) Implements System.Workflow.ComponentModel.Design.IMemberCreationService.CreateEvent

        End Sub

        Public Sub CreateField(ByVal className As String, ByVal fieldName As String, ByVal fieldType As System.Type, ByVal genericParameterTypes() As System.Type, ByVal attributes As System.CodeDom.MemberAttributes, ByVal initializationExpression As System.CodeDom.CodeSnippetExpression, ByVal overwriteExisting As Boolean) Implements System.Workflow.ComponentModel.Design.IMemberCreationService.CreateField

        End Sub

        Public Sub CreateProperty(ByVal className As String, ByVal propertyName As String, ByVal propertyType As System.Type, ByVal attributes() As System.Workflow.ComponentModel.Compiler.AttributeInfo, ByVal emitDependencyProperty As Boolean, ByVal isMetaProperty As Boolean, ByVal isAttached As Boolean, ByVal ownerType As System.Type, ByVal isReadOnly As Boolean) Implements System.Workflow.ComponentModel.Design.IMemberCreationService.CreateProperty

        End Sub

        Public Sub RemoveEvent(ByVal className As String, ByVal eventName As String, ByVal eventType As System.Type) Implements System.Workflow.ComponentModel.Design.IMemberCreationService.RemoveEvent

        End Sub

        Public Sub RemoveProperty(ByVal className As String, ByVal propertyName As String, ByVal propertyType As System.Type) Implements System.Workflow.ComponentModel.Design.IMemberCreationService.RemoveProperty

        End Sub

        Public Sub ShowCode() Implements System.Workflow.ComponentModel.Design.IMemberCreationService.ShowCode

        End Sub

        Public Sub ShowCode(ByVal activity As System.Workflow.ComponentModel.Activity, ByVal methodName As String, ByVal delegateType As System.Type) Implements System.Workflow.ComponentModel.Design.IMemberCreationService.ShowCode

        End Sub

        Public Sub UpdateBaseType(ByVal className As String, ByVal baseType As System.Type) Implements System.Workflow.ComponentModel.Design.IMemberCreationService.UpdateBaseType

        End Sub

        Public Sub UpdateEvent(ByVal className As String, ByVal oldEventName As String, ByVal oldEventType As System.Type, ByVal newEventName As String, ByVal newEventType As System.Type, ByVal attributes() As System.Workflow.ComponentModel.Compiler.AttributeInfo, ByVal emitDependencyProperty As Boolean, ByVal isMetaProperty As Boolean) Implements System.Workflow.ComponentModel.Design.IMemberCreationService.UpdateEvent

        End Sub

        Public Sub UpdateProperty(ByVal className As String, ByVal oldPropertyName As String, ByVal oldPropertyType As System.Type, ByVal newPropertyName As String, ByVal newPropertyType As System.Type, ByVal attributes() As System.Workflow.ComponentModel.Compiler.AttributeInfo, ByVal emitDependencyProperty As Boolean, ByVal isMetaProperty As Boolean) Implements System.Workflow.ComponentModel.Design.IMemberCreationService.UpdateProperty

        End Sub

        Public Sub UpdateTypeName(ByVal oldClassName As String, ByVal newClassName As String) Implements System.Workflow.ComponentModel.Design.IMemberCreationService.UpdateTypeName

        End Sub
    End Class

End Namespace

