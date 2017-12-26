'****************************** Module Header ******************************'
' Module Name:  MSAAHelper.vb
' Project:      VBOfficeRibbonAccessibility
' Copyright (c) Microsoft Corporation.
'
' The helper functions for Microsoft Active Accessibility (MSAA).
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Diagnostics
Imports Microsoft.Office.Core
Imports System.Text.RegularExpressions


Friend Class MSAAHelper

    ''' <summary>
    ''' Gets the child accessible objects of the given container object.
    ''' </summary>
    ''' <param name="accContainer">
    ''' The container object's IAccessible interface.
    ''' </param>
    ''' <returns>
    ''' The child accessible objects of the given container object.
    ''' </returns>
    Public Shared Function GetAccessibleChildren(ByVal accContainer As IAccessible) As IAccessible()
        ' Get the number of child interfaces that belong to this object. 
        Dim childNum As Integer = 0
        Try
            childNum = accContainer.accChildCount
        Catch ex As Exception
            childNum = 0
            Debug.Print(ex.Message)
        End Try

        ' Get the child accessible objects.
        Dim accObjects As IAccessible() = New IAccessible(childNum - 1) {}
        Dim count As Integer = 0
        If (childNum <> 0) Then
            NativeMethods.AccessibleChildren(accContainer, 0, childNum, accObjects, (count))
        End If
        Return accObjects
    End Function


    ''' <summary>
    ''' Gets the child accessible object by name and role text.
    ''' </summary>
    ''' <param name="accContainer">
    ''' The container object's IAccessible interface.
    ''' </param>
    ''' <param name="name">The name of the object</param>
    ''' <param name="roleText">The role text of the object</param>
    ''' <param name="ignoreInvisible">
    ''' Specifies if it's required to ignore the invisible objects.
    ''' </param>
    ''' <returns>
    ''' The accessible object in the container that match the specified name 
    ''' and role. 
    ''' </returns>
    Public Shared Function GetAccessibleObjectByNameAndRole( _
        ByVal accContainer As IAccessible, ByVal name As Regex, _
        ByVal roleText As String, ByVal ignoreInvisible As Boolean) As IAccessible

        Dim objToReturn As IAccessible = Nothing
        If (accContainer IsNot Nothing) Then
            ' Get the child accessible objects.
            Dim children As IAccessible() = GetAccessibleChildren(accContainer)
            For Each child As IAccessible In children

                ' Get each child's name, state and role.
                Dim childName As String = Nothing
                Dim childState As String = String.Empty
                Dim childRole As String = String.Empty
                Try
                    childName = child.accName(0)
                    childState = GetStateText(child.accState(0))
                    childRole = GetRoleText(child.accRole(0))
                Catch ex As Exception
                    ' Record the error and continue.
                    Debug.Print(ex.Message)
                    Continue For
                End Try

                ' If the child is invisible and it's required to ignore the 
                ' invisible objects, continue to the next object.
                If (ignoreInvisible AndAlso childState.Contains("invisible")) Then
                    Continue For
                End If

                ' If the name and role match, return the object.
                If (Not String.IsNullOrEmpty(childName) AndAlso _
                    name.Match(childName).Success AndAlso _
                    childRole = roleText) Then
                    Return child
                End If

                ' Recursively look for the object among the children.
                objToReturn = GetAccessibleObjectByNameAndRole(child, name, _
                    roleText, ignoreInvisible)
                If (objToReturn IsNot Nothing) Then
                    Return objToReturn
                End If
            Next
        End If
        Return objToReturn
    End Function


    ''' <summary>
    ''' Get the role text of an accesible object.
    ''' </summary>
    ''' <param name="role">
    ''' One of the object role constants.
    ''' http://msdn.microsoft.com/en-us/library/dd373608.aspx
    ''' </param>
    ''' <returns>The role text of an accessible object</returns>
    Friend Shared Function GetRoleText(ByVal role As UInt32) As String
        Dim roleText As New StringBuilder(&H400)
        NativeMethods.GetRoleText(role, roleText, roleText.Capacity)
        Return roleText.ToString
    End Function


    ''' <summary>
    ''' Get the state text of an accessible object.
    ''' </summary>
    ''' <param name="stateBit">
    ''' One of the object state constants.
    ''' http://msdn.microsoft.com/en-us/library/dd373609.aspx
    ''' </param>
    ''' <returns>The state text of an accessible object</returns>
    Public Shared Function GetStateText(ByVal stateBit As MSAAStateConstants) As String
        Dim maxLength As Integer = &H400
        Dim focusableStateText As New StringBuilder(maxLength)
        Dim sizeableStateText As New StringBuilder(maxLength)
        Dim moveableStateText As New StringBuilder(maxLength)
        Dim invisibleStateText As New StringBuilder(maxLength)
        Dim unavailableStateText As New StringBuilder(maxLength)
        Dim hasPopupStateText As New StringBuilder(maxLength)

        If (stateBit = (MSAAStateConstants.STATE_SYSTEM_FOCUSABLE Or _
            MSAAStateConstants.STATE_SYSTEM_SIZEABLE Or MSAAStateConstants.STATE_SYSTEM_MOVEABLE)) Then
            NativeMethods.GetStateText(MSAAStateConstants.STATE_SYSTEM_FOCUSABLE, _
                focusableStateText, focusableStateText.Capacity)
            NativeMethods.GetStateText(MSAAStateConstants.STATE_SYSTEM_SIZEABLE, _
                sizeableStateText, sizeableStateText.Capacity)
            NativeMethods.GetStateText(MSAAStateConstants.STATE_SYSTEM_MOVEABLE, _
                moveableStateText, moveableStateText.Capacity)
            Return (focusableStateText.ToString & "," & sizeableStateText.ToString & _
                "," & moveableStateText.ToString)
        End If

        If (stateBit = (MSAAStateConstants.STATE_SYSTEM_FOCUSABLE Or _
            MSAAStateConstants.STATE_SYSTEM_INVISIBLE)) Then
            NativeMethods.GetStateText(MSAAStateConstants.STATE_SYSTEM_FOCUSABLE, _
                focusableStateText, focusableStateText.Capacity)
            NativeMethods.GetStateText(MSAAStateConstants.STATE_SYSTEM_INVISIBLE, _
                invisibleStateText, invisibleStateText.Capacity)
            Return (focusableStateText.ToString & "," & invisibleStateText.ToString)
        End If

        If (stateBit = (MSAAStateConstants.STATE_SYSTEM_FOCUSABLE Or _
            MSAAStateConstants.STATE_SYSTEM_UNAVAILABLE)) Then
            NativeMethods.GetStateText(MSAAStateConstants.STATE_SYSTEM_FOCUSABLE, _
                focusableStateText, focusableStateText.Capacity)
            NativeMethods.GetStateText(MSAAStateConstants.STATE_SYSTEM_UNAVAILABLE, _
                unavailableStateText, unavailableStateText.Capacity)
            Return (focusableStateText.ToString & "," & unavailableStateText.ToString)
        End If

        If (stateBit = (MSAAStateConstants.STATE_SYSTEM_HASPOPUP Or _
            MSAAStateConstants.STATE_SYSTEM_UNAVAILABLE)) Then
            NativeMethods.GetStateText(MSAAStateConstants.STATE_SYSTEM_HASPOPUP, _
                hasPopupStateText, hasPopupStateText.Capacity)
            NativeMethods.GetStateText(MSAAStateConstants.STATE_SYSTEM_UNAVAILABLE, _
                unavailableStateText, unavailableStateText.Capacity)
            Return (hasPopupStateText.ToString & "," & unavailableStateText.ToString)
        End If

        Dim stateText As New StringBuilder(maxLength)
        NativeMethods.GetStateText(stateBit, stateText, stateText.Capacity)
        Return stateText.ToString
    End Function


    ''' <summary>
    ''' Gets the list of child accesible objects that match the role text.
    ''' </summary>
    ''' <param name="accContainer">
    ''' The container object's IAccessible interface.
    ''' </param>
    ''' <param name="roleText">The role text of the object</param>
    ''' <param name="accObjList">
    ''' The list of child accesible objects that match the role text.
    ''' </param>
    ''' <param name="ignoreInvisible">
    ''' Specifies if it's required to ignore the invisible objects.
    ''' </param>
    Public Shared Sub GetAccessibleObjectListByRole(ByVal accContainer As IAccessible, _
        ByVal roleText As String, ByRef accObjList As List(Of IAccessible), _
        ByVal ignoreInvisible As Boolean)

        If (accContainer IsNot Nothing) Then
            ' Get the child accessible objects.
            Dim children As IAccessible() = GetAccessibleChildren(accContainer)
            For Each child As IAccessible In children

                ' Get each child's name, state and role.
                Dim childRole As String = Nothing
                Dim childState As String = String.Empty
                Try
                    childRole = GetRoleText(child.accRole(0))
                    childState = GetStateText(child.accState(0))
                Catch ex As Exception
                    ' Record the error and continue.
                    Debug.Print(ex.Message)
                    Continue For
                End Try

                ' If the child is invisible and it's required to ignore the 
                ' invisible objects, continue to the next object.
                If (ignoreInvisible AndAlso childState.Contains("invisible")) Then
                    Continue For
                End If

                ' If the role matches, add the object to the result list.
                If (childRole = roleText) Then
                    accObjList.Add(child)
                End If

                ' Recursively look for the object among the children.
                GetAccessibleObjectListByRole(child, roleText, (accObjList), ignoreInvisible)
            Next
        End If
    End Sub


    ''' <summary>
    ''' Gets the accessible object from a window handle.
    ''' </summary>
    ''' <param name="hWnd">The window handle</param>
    ''' <returns>The accessible object from the window handle</returns>
    Public Shared Function GetAccessibleObjectFromHandle(ByVal hWnd As IntPtr) As IAccessible
        Dim objToReturn As IAccessible = Nothing
        If (hWnd <> IntPtr.Zero) Then
            Dim iid As Guid = GetType(IAccessible).GUID
            objToReturn = NativeMethods.AccessibleObjectFromWindow(hWnd, 0, (iid))
        End If
        Return objToReturn
    End Function

End Class