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
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.Drawing
Imports System.Drawing.Drawing2D
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Activities
Imports System.Windows.Forms

<Designer(GetType(CustomActivityDesigner), GetType(IDesigner))> _
<ToolboxItem(GetType(ActivityToolboxItem))> _
Public Class CustomActivity
    Inherits SequenceActivity
End Class

<ActivityDesignerThemeAttribute(GetType(CustomActivityDesignerTheme))> _
Public Class CustomActivityDesigner
    Inherits ActivityDesigner

    Protected Overrides ReadOnly Property DesignerActions() As ReadOnlyCollection(Of DesignerAction)
        Get
            Dim DesignerActionList As New List(Of DesignerAction)

            'This is for the configuration error on an activity
            DesignerActionList.Add(New DesignerAction(Me, 1, "Insert Text Here!"))
            Return DesignerActionList.AsReadOnly()
        End Get
    End Property

    Protected Overrides Sub OnExecuteDesignerAction(ByVal designerAction As DesignerAction)
        If designerAction.ActionId = 1 Then
            MessageBox.Show(designerAction.Text)
        End If
    End Sub

    ' Callback for a context Menu item when a user right clicks on the activity
    Private Sub CustomContextMenuEvent(ByVal sender As Object, ByVal e As EventArgs)
        MessageBox.Show("This is the action from my Context Menu")
    End Sub

    Protected Overrides ReadOnly Property Verbs() As ActivityDesignerVerbCollection
        Get
            Dim newVerbs As ActivityDesignerVerbCollection = New ActivityDesignerVerbCollection()
            newVerbs.AddRange(MyBase.Verbs)
            newVerbs.Add(New ActivityDesignerVerb(Me, DesignerVerbGroup.General, "Custom Context Menu", New EventHandler(AddressOf CustomContextMenuEvent)))
            Return newVerbs
        End Get
    End Property
End Class

Friend NotInheritable Class CustomActivityDesignerTheme
    Inherits ActivityDesignerTheme
    Public Sub New(ByVal theme As WorkflowTheme)
        MyBase.New(theme)
        Me.BorderColor = Color.Red
        Me.BorderStyle = DashStyle.Dot
        Me.BackColorStart = Color.LightYellow
        Me.BackColorEnd = Color.Yellow
        Me.BackgroundStyle = LinearGradientMode.Horizontal
    End Sub
End Class
