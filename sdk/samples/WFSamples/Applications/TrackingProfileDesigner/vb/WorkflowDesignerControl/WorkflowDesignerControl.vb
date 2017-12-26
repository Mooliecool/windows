
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

Imports System.ComponentModel.Design
Imports System.IO
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.ComponentModel.Serialization
Imports System.XML

Public Class WorkflowDesignerControl
    Inherits UserControl
    Implements IDisposable, IServiceProvider

    Dim workflowView As WorkflowView
    Dim designSurface As DesignSurface
    Dim loader As WorkflowLoader

    Public Sub New()
        InitializeComponent()

        WorkflowTheme.CurrentTheme.ReadOnly = False
        WorkflowTheme.CurrentTheme.AmbientTheme.ShowConfigErrors = False
        WorkflowTheme.CurrentTheme.ReadOnly = True
    End Sub

    Dim xamlValue As String

    Public Property Xaml() As String
        Get
            Dim xamlScratch As String = String.Empty
            If Me.loader IsNot Nothing Then
                Me.loader.Flush()
                xamlScratch = Me.loader.Xaml
            End If
            Return xamlScratch
        End Get

        Set(ByVal value As String)
            Me.xamlValue = value
            If Not (String.IsNullOrEmpty(value)) Then
                LoadWorkflow()
            End If
        End Set
    End Property

    Dim workflowTypeValue As Type = Nothing

    Public Property WorkflowType() As Type
        Get
            Return workflowTypeValue
        End Get
        Set(ByVal value As Type)
            workflowTypeValue = value
            If value IsNot Nothing Then
                LoadWorkflow()
            End If
        End Set
    End Property

    Public Shadows Function GetService(ByVal serviceType As System.Type) As Object Implements System.IServiceProvider.GetService
        If Me.workflowView IsNot Nothing Then
            Return CType(Me.workflowView, IServiceProvider).GetService(serviceType)
        Else
            Return Nothing
        End If
    End Function

    Protected Overrides Sub OnLoad(ByVal e As EventArgs)
        MyBase.OnLoad(e)
    End Sub

    Private Sub LoadWorkflow()
        SuspendLayout()
        Dim designSurface As New DesignSurface()
        Dim loader As New WorkflowLoader()
        loader.WorkflowType = workflowTypeValue
        loader.Xaml = xamlValue
        designSurface.BeginLoad(loader)
        Dim designerHost As IDesignerHost = TryCast(designSurface.GetService(GetType(IDesignerHost)), IDesignerHost)
        If designerHost IsNot Nothing AndAlso designerHost.RootComponent IsNot Nothing Then
            Dim rootDesigner As IRootDesigner = TryCast(designerHost.GetDesigner(designerHost.RootComponent), IRootDesigner)
            If rootDesigner IsNot Nothing Then
                UnloadWorkflow()
                Me.designSurface = designSurface
                Me.loader = loader
                Me.workflowView = TryCast(rootDesigner.GetView(ViewTechnology.Default), WorkflowView)
                Me.panel1.Controls.Add(Me.workflowView)
                Me.workflowView.Dock = DockStyle.Fill
                Me.workflowView.TabIndex = 1
                Me.workflowView.TabStop = True
                Me.workflowView.HScrollBar.TabStop = False
                Me.workflowView.VScrollBar.TabStop = False
                Me.workflowView.Focus()
                Me.workflowView.FitToScreenSize()
            End If
        End If
        ResumeLayout(True)
    End Sub



    Custom Event SelectionChanged As EventHandler
        AddHandler(ByVal value As EventHandler)
            Dim selectionService As ISelectionService = TryCast(GetService(GetType(ISelectionService)), ISelectionService)
            If selectionService IsNot Nothing Then
                AddHandler selectionService.SelectionChanged, value
            End If
        End AddHandler

        RemoveHandler(ByVal value As EventHandler)
            Dim selectionService As ISelectionService = TryCast(GetService(GetType(ISelectionService)), ISelectionService)
            If selectionService IsNot Nothing Then
                RemoveHandler selectionService.SelectionChanged, value
            End If
        End RemoveHandler

        RaiseEvent(ByVal sender As Object, ByVal e As System.EventArgs)
        End RaiseEvent
    End Event

    Private Sub UnloadWorkflow()
        Dim designerHost As IDesignerHost = TryCast(GetService(GetType(IDesignerHost)), IDesignerHost)
        If designerHost IsNot Nothing AndAlso designerHost.Container.Components.Count > 0 Then
            WorkflowLoader.DestroyObjectGraphFromDesignerHost(designerHost, TryCast(designerHost.RootComponent, Activity))
        End If
        If Me.designSurface IsNot Nothing Then
            Me.designSurface.Dispose()
            Me.designSurface = Nothing
        End If

        If Me.workflowView IsNot Nothing Then
            Controls.Remove(Me.workflowView)
            Me.workflowView.Dispose()
            Me.workflowView = Nothing
        End If
    End Sub

    Public Sub ShowDefaultWorkflow()

        Dim workflow As New SequentialWorkflowActivity()
        workflow.Name = "Workflow1"

        Using stringWriter As New StringWriter()
            Using xmlWriter As XmlWriter = Xml.XmlWriter.Create(stringWriter)
                Dim serializer As New WorkflowMarkupSerializer()
                serializer.Serialize(xmlWriter, workflow)
                Me.Xaml = stringWriter.ToString()
            End Using
        End Using
    End Sub
End Class
