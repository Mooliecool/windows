
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


Public Class EnumDropDown
    Inherits ToolStripDropDown

    Dim WithEvents toggleAll As ToolStripMenuItem

    Public Sub New(ByVal enumType As Type)
        InitializeComponent()
        toggleAll = New ToolStripMenuItem()
        toggleAll.Text = "Select All"
        toggleAll.Tag = True

        Me.Items.Add(toggleAll)

        For Each value As Object In System.Enum.GetValues(enumType)
            Dim button As New ToolStripButton()
            button.Name = value.ToString()
            button.Text = value.ToString()
            button.CheckOnClick = True
            button.Image = My.Resources.eventIcon

            Me.Items.Add(button)
        Next
    End Sub

    ' Raised when 'Select All' or 'Select None' is displayed.
    Private Sub ToggleAllClick(ByVal sender As Object, ByVal e As EventArgs) Handles toggleAll.Click
        For Each item As ToolStripItem In Me.Items
            Dim button As ToolStripButton = TryCast(item, ToolStripButton)
            If button IsNot Nothing Then
                If CType(toggleAll.Tag, Boolean) Then
                    If Not button.Checked Then button.PerformClick()
                Else
                    If button.Checked Then button.PerformClick()
                End If
            End If
        Next
        UpdateToggleAll()
    End Sub

    ' Update the 'toggle all' button to be either 'select all' or 'select none'
    Public Sub UpdateToggleAll()
        Dim allChecked As Boolean = True
        For Each item As ToolStripItem In Me.Items
            If TypeOf item Is ToolStripButton Then
                If Not (CType(item, ToolStripButton)).Checked Then
                    allChecked = False
                End If
            End If
        Next
        If allChecked Then
            toggleAll.Text = "Select None"
            toggleAll.Tag = False
        Else
        toggleAll.Text = "Select All"
        toggleAll.Tag = True
        End If
    End Sub

    ' Clear all the checked items
    Public Sub Reset()
        UpdateToggleAll()
        For Each button As ToolStripItem In Me.Items
            If TypeOf button Is ToolStripButton Then
                CType(button, ToolStripButton).Checked = False
            End If
        Next
    End Sub
End Class
