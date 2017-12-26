'************************************* Module Header **************************************\
' Module Name:	UC_SmartTagSupport.vb
' Project:		VBWinFormDesigner
' Copyright (c) Microsoft Corporation.
' 
' 
' The SmartTagSupport sample demonstrates how to add smart tags for a control.
' 
' Smart tags are menu-like user interface (UI) elements that supply commonly used design-time 
' options. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'


Imports System.ComponentModel.Design
Imports System.Text
Imports System.ComponentModel
Imports System.Windows.Forms.Design
Imports System.Security.Permissions

Namespace SmartTags
    <Designer(GetType(UC_SmartTagSupportDesigner))> _
    Public Class UC_SmartTagSupport

    End Class


    <PermissionSet(SecurityAction.Demand, Name:="FullTrust")> _
    Public Class UC_SmartTagSupportDesigner
        Inherits ControlDesigner

        Private _actionLists As DesignerActionListCollection

        Public Overrides ReadOnly Property ActionLists() As DesignerActionListCollection
            Get
                If (Nothing Is Me._actionLists) Then
                    Me._actionLists = New DesignerActionListCollection
                    Me._actionLists.Add(New UC_SmartTagSupportActionList(MyBase.Component))
                End If
                Return Me._actionLists
            End Get
        End Property
    End Class



    Public Class UC_SmartTagSupportActionList
        Inherits DesignerActionList

        Private control As UC_SmartTagSupport
        Private designerActionUISvc As DesignerActionUIService

        Public Sub New(ByVal component As IComponent)
            MyBase.New(component)
            Me.designerActionUISvc = Nothing
            Me.control = TryCast(component, UC_SmartTagSupport)
            Me.designerActionUISvc = TryCast(MyBase.GetService(GetType(DesignerActionUIService)), DesignerActionUIService)
        End Sub

        Private Function GetPropertyByName(ByVal propName As String) As PropertyDescriptor
            Dim prop As PropertyDescriptor = TypeDescriptor.GetProperties(Me.control).Item(propName)
            If (Nothing Is prop) Then
                Throw New ArgumentException("Matching ColorLabel property not found!", propName)
            End If
            Return prop
        End Function

        Public Overrides Function GetSortedActionItems() As DesignerActionItemCollection
            Dim items As New DesignerActionItemCollection
            items.Add(New DesignerActionHeaderItem("Appearance"))
            items.Add(New DesignerActionHeaderItem("Information"))
            items.Add(New DesignerActionPropertyItem("BackColor", "Back Color", "Appearance", "Selects the background color."))
            items.Add(New DesignerActionPropertyItem("ForeColor", "Fore Color", "Appearance", "Selects the foreground color."))
            Dim location As New StringBuilder("Location: ")
            location.Append(Me.control.Location)
            Dim size As New StringBuilder("Size: ")
            size.Append(Me.control.Size)
            items.Add(New DesignerActionTextItem(location.ToString, "Information"))
            items.Add(New DesignerActionTextItem(size.ToString, "Information"))
            Return items
        End Function

        Public Property BackColor() As Color
            Get
                Return Me.control.BackColor
            End Get
            Set(ByVal value As Color)
                Me.GetPropertyByName("BackColor").SetValue(Me.control, value)
            End Set
        End Property

        Public Property ForeColor() As Color
            Get
                Return Me.control.ForeColor
            End Get
            Set(ByVal value As Color)
                Me.GetPropertyByName("ForeColor").SetValue(Me.control, value)
            End Set
        End Property
    End Class

End Namespace
