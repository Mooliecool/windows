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
Imports System.Collections.Generic
Imports System.Text
Imports System.ComponentModel.Design
Imports System.ComponentModel
Imports System.Drawing
Imports System.Windows.Forms
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.ComponentModel

Public Class WorkflowViewWrapper
    Inherits System.Windows.Forms.Panel

    Friend Host As IDesignerHost
    Friend SequentialWorkflow As SequentialWorkflowActivity

    Private workflowView As WorkflowView
    Private loader As Loader
    Private surface As DesignSurface

    Public Sub New()
        Me.loader = New Loader()

        ' Create a Workflow Design Surface
        Me.surface = New DesignSurface()
        Me.surface.BeginLoad(Me.loader)

        ' Get the Workflow Designer Host
        If Me.surface.GetService(GetType(IDesignerHost)) IsNot Nothing AndAlso TypeOf Me.surface.GetService(GetType(IDesignerHost)) Is IDesignerHost Then
            Me.Host = CType(Me.surface.GetService(GetType(IDesignerHost)), IDesignerHost)
        Else
            Return
        End If

        ' Create a Sequential Workflow by using the Workflow Designer Host
        SequentialWorkflow = CType(Host.CreateComponent(GetType(SequentialWorkflowActivity)), SequentialWorkflowActivity)
        SequentialWorkflow.Name = "CustomOutlookWorkflow"

        ' Create a Workflow View on the Workflow Design Surface
        Me.workflowView = New WorkflowView(CType(Me.surface, IServiceProvider))

        ' Add a message filter to the workflow view, to support panning
        Dim filter As MessageFilter = New MessageFilter(CType(Me.surface, IServiceProvider), Me.workflowView)
        Me.workflowView.AddDesignerMessageFilter(Filter)

        ' Activate the Workflow View
        Me.Host.Activate()

        Me.workflowView.Dock = DockStyle.Fill
        Me.Controls.Add(workflowView)
        Me.Dock = DockStyle.Fill
    End Sub

    Public Sub PerformSave()
        If Me.XamlFile.Length <> 0 Then
            Me.SaveExistingWorkflow(Me.XamlFile)
        Else
            Dim saveFileDialog As SaveFileDialog = New SaveFileDialog()
            saveFileDialog.Filter = "Workflow XAML files (*.xoml)|*.xoml|All files (*.*)|*.*"
            saveFileDialog.FilterIndex = 1
            saveFileDialog.RestoreDirectory = True

            If saveFileDialog.ShowDialog() = DialogResult.OK Then
                Me.SaveExistingWorkflow(saveFileDialog.FileName)
                Me.Text = "Designer Hosting Sample -- [" + saveFileDialog.FileName + "]"
            End If
        End If
    End Sub

    Friend Sub SaveExistingWorkflow(ByVal filePath As String)
        If Me.surface IsNot Nothing AndAlso Me.loader IsNot Nothing Then
            Me.loader.XamlFile = filePath
            Me.loader.PerformFlush(Me.Host)
        End If
    End Sub

    Public Property XamlFile() As String
        Get
            Return Me.loader.XamlFile
        End Get
        Set(ByVal value As String)
            Me.loader.XamlFile = value
        End Set
    End Property
End Class

Friend Class MessageFilter
    Inherits WorkflowDesignerMessageFilter

    Private mouseDown As Boolean
    Private serviceProvider As IServiceProvider
    Private workflowView As WorkflowView

    Public Sub New(ByVal provider As IServiceProvider, ByVal view As WorkflowView)
        Me.serviceProvider = provider
        Me.workflowView = View
    End Sub

    Protected Overrides Function OnMouseDown(ByVal eventArgs As System.Windows.Forms.MouseEventArgs) As Boolean
        ' Allow other components to process this event by not returning true.
        Me.mouseDown = True
        Return False
    End Function

    Protected Overrides Function OnMouseMove(ByVal eventArgs As System.Windows.Forms.MouseEventArgs) As Boolean
        ' Allow other components to process this event by not returning true.
        If (Me.mouseDown) Then
            Me.workflowView.ScrollPosition = New Point(eventArgs.X, eventArgs.Y)
        End If
        Return False
    End Function

    Protected Overrides Function OnMouseUp(ByVal eventArgs As System.Windows.Forms.MouseEventArgs) As Boolean
        ' Allow other components to process this event by not returning true.
        Me.mouseDown = False
        Return False
    End Function

    Protected Overrides Function OnMouseDoubleClick(ByVal eventArgs As System.Windows.Forms.MouseEventArgs) As Boolean
        Me.mouseDown = False
        Return True
    End Function

    Protected Overrides Function OnMouseEnter(ByVal eventArgs As System.Windows.Forms.MouseEventArgs) As Boolean
        ' Allow other components to process this event by not returning true.
        Me.mouseDown = False
        Return False
    End Function

    Protected Overrides Function OnMouseHover(ByVal eventArgs As System.Windows.Forms.MouseEventArgs) As Boolean
        ' Allow other components to process this event by not returning true.
        Me.mouseDown = False
        Return False
    End Function

    Protected Overrides Function OnMouseLeave() As Boolean
        ' Allow other components to process this event by not returning true.
        Me.mouseDown = False
        Return False
    End Function

    Protected Overrides Function OnMouseWheel(ByVal eventArgs As System.Windows.Forms.MouseEventArgs) As Boolean
        Me.mouseDown = False
        Return True
    End Function

    Protected Overrides Function OnMouseCaptureChanged() As Boolean
        ' Allow other components to process this event by not returning true.
        Me.mouseDown = False
        Return False
    End Function

    Protected Overrides Function OnDragEnter(ByVal eventArgs As System.Windows.Forms.DragEventArgs) As Boolean
        Return True
    End Function

    Protected Overrides Function OnDragOver(ByVal eventArgs As System.Windows.Forms.DragEventArgs) As Boolean
        Return True
    End Function

    Protected Overrides Function OnDragLeave() As Boolean
        Return True
    End Function

    Protected Overrides Function OnDragDrop(ByVal eventArgs As System.Windows.Forms.DragEventArgs) As Boolean
        Return True
    End Function

    Protected Overrides Function OnGiveFeedback(ByVal eventArgs As System.Windows.Forms.GiveFeedbackEventArgs) As Boolean
        Return True
    End Function

    Protected Overrides Function OnQueryContinueDrag(ByVal eventArgs As System.Windows.Forms.QueryContinueDragEventArgs) As Boolean
        Return True
    End Function

    Protected Overrides Function OnKeyUp(ByVal eventArgs As System.Windows.Forms.KeyEventArgs) As Boolean
        Return True
    End Function

    Protected Overrides Function OnShowContextMenu(ByVal screenMenuPoint As System.Drawing.Point) As Boolean
        Return True
    End Function

    Protected Overrides Function OnKeyDown(ByVal eventArgs As System.Windows.Forms.KeyEventArgs) As Boolean
        If eventArgs.KeyCode = Keys.Delete Then
            Dim selectionService As ISelectionService = CType(Me.serviceProvider.GetService(GetType(ISelectionService)), ISelectionService)
            If selectionService IsNot Nothing AndAlso TypeOf selectionService.PrimarySelection Is CodeActivity Then
                Dim codeActivityComponent As CodeActivity = CType(selectionService.PrimarySelection, CodeActivity)
                Dim parentActivity As CompositeActivity = codeActivityComponent.Parent
                If parentActivity IsNot Nothing Then
                    parentActivity.Activities.Remove(codeActivityComponent)
                    Me.ParentView.Update()
                End If
            End If
        End If
        Return True
    End Function
End Class
