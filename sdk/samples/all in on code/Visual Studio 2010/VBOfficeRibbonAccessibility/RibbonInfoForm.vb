'****************************** Module Header ******************************'
' Module Name:  RibbonInfoForm.vb
' Project:      VBOfficeRibbonAccessibility
' Copyright (c) Microsoft Corporation.
' 
' This example illustrates how to pinvoke the Microsoft Active Accessibilty 
' (MSAA) API to automate Office Ribbon controls. The code calls the following 
' APIs,

'    AccessibleObjectFromWindow, 
'    AccessibleChildren,
'    GetRoleText,
'    GetStateText,

' to display the whole structure of the Office ribbon, including tabs, groups,
' and controls. It also shows how to nagivate to a tab and execute button 
' function programmatically.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports Microsoft.Office.Core
Imports System.Runtime.InteropServices
Imports System.Text.RegularExpressions
Imports System.Diagnostics
Imports System.Windows.Forms


Public Class RibbonInfoForm

    ' Helper Data Structure. It is used to store the list box item.
    Friend Class ListBoxItem

        Public Sub New(ByVal name As String, ByVal item As IAccessible)
            Me.Name = name
            Me.accItem = item
        End Sub

        Public Overrides Function ToString() As String
            Return Me.Name
        End Function

        ' Fields
        Public accItem As IAccessible
        Public Name As String

    End Class


    Private TopWindow As IAccessible ' The top accessible window


    ' In the form load event, get all accessible tab objects and list them in 
    ' the lbTabs list box.
    Private Sub RibbonInfoForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        ' Initialize the IAccessible interface of the top window.
        Globals.ThisAddIn.Application.Activate()
        Me.TopWindow = MSAAHelper.GetAccessibleObjectFromHandle( _
            Process.GetCurrentProcess.MainWindowHandle)

        ' Get the IAccessible object of the Ribbon property page.
        Dim ribbon As IAccessible = MSAAHelper.GetAccessibleObjectByNameAndRole(
            Me.TopWindow, New Regex("Ribbon"), "property page", False)

        ' Find all visible ribbon tabs and show them in the list box. 
        Dim children As IAccessible() = MSAAHelper.GetAccessibleChildren( _
            MSAAHelper.GetAccessibleObjectByNameAndRole(ribbon, _
            New Regex("Ribbon Tabs"), "page tab list", True))

        For Each child As IAccessible In children
            If (child.accChildCount > 0) Then
                Dim tabs As IAccessible() = MSAAHelper.GetAccessibleChildren(child)
                For Each tab As IAccessible In tabs
                    Dim state As String = MSAAHelper.GetStateText(tab.accState(0))
                    If (Not state.Contains("invisible")) Then
                        Me.lbTabs.Items.Add(New ListBoxItem(tab.accName(0), tab))
                    End If
                Next
            End If
        Next
    End Sub


    ' When the selected item in lbTabs changes, the code navigates to the 
    ' selected tab. 
    Private Sub lbTabs_SelectedValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles lbTabs.SelectedValueChanged
        Dim item As ListBoxItem = TryCast(Me.lbTabs.SelectedItem, ListBoxItem)
        item.accItem.accDoDefaultAction(0)
    End Sub


    ' When the btnListChildGroups button is clicked, the code probes all ribbon 
    ' groups in the selected tab and list them in the lbGroups list box.
    Private Sub btnListChildGroups_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnListChildGroups.Click
        Me.lbGroups.Items.Clear()

        ' Find the selected tab.
        Dim item As ListBoxItem = TryCast(Me.lbTabs.SelectedItem, ListBoxItem)
        Dim tab As IAccessible = MSAAHelper.GetAccessibleObjectByNameAndRole( _
            Me.TopWindow, New Regex(item.Name), "property page", True)
        If (tab Is Nothing) Then
            MessageBox.Show("Error: the " & item.Name & " tab cannot be found")
            Return
        End If

        ' Get the groups, and list the groups in the lbGroups control.
        Dim groups As New List(Of IAccessible)
        MSAAHelper.GetAccessibleObjectListByRole(tab, "tool bar", (groups), True)
        For Each group As IAccessible In groups
            Me.lbGroups.Items.Add(New ListBoxItem(group.accName(0), group))
        Next
    End Sub


    ' When the btnListChildControls button is clicked, the code probes all 
    ' controls in the selected group and list them in the lbControls list box.
    Private Sub btnListChildControls_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnListChildControls.Click
        Me.lbControls.Items.Clear()

        ' Find the selected group.
        Dim item As ListBoxItem = TryCast(Me.lbGroups.SelectedItem, ListBoxItem)
        Dim group As IAccessible = MSAAHelper.GetAccessibleObjectByNameAndRole( _
            Me.TopWindow, New Regex(item.Name), "tool bar", True)
        If (group Is Nothing) Then
            MessageBox.Show("Error: the " & item.Name & " group cannot be found")
            Return
        End If

        ' Get the controls, and list the controls in the lbControls control.
        Dim controls As IAccessible() = MSAAHelper.GetAccessibleChildren(group)
        For Each control As IAccessible In controls
            Me.lbControls.Items.Add(New ListBoxItem(control.accName(0), control))
        Next
    End Sub


    ' When the btnExecuteControl is clicked, the code executes the default 
    ' action of the currently selected control.
    Private Sub btnExecuteControl_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExecuteControl.Click
        Dim item As ListBoxItem = TryCast(Me.lbControls.SelectedItem, ListBoxItem)
        If (item IsNot Nothing) Then
            item.accItem.accDoDefaultAction(0)
        End If
    End Sub

End Class