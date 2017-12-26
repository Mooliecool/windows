'************************** Module Header ******************************'
' Module Name:  PropertyControlMap.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' The PropertyControlMap class is used to initialize the Controls on a PageView
' Object. 
' 
' The IPageViewSite Interface is implemented by the PropertyPage class, and 
' the IPropertyPageUI Interface is implemented by the PageView Class. With the 
' PropertyControlTable object, the PropertyControlMap object could get a Property
' value from a PropertyPage object, and use it to initialize the related Control
' on the PageView object. 
' 
' It provides the main UI features of a PageView object. Through this interface, 
' the PropertyPage object can show / hide / move a PageView object.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System.Windows.Forms

Namespace PropertyPageBase

    Public Class PropertyControlMap

        ' The IPageViewSite Interface is implemented by the PropertyPage class.
        Private _pageViewSite As IPageViewSite

        ' The IPropertyPageUI Interface is implemented by the PageView Class.
        Private _propertyPageUI As IPropertyPageUI

        ' The PropertyControlTable class stores the Control / Property Name KeyValuePairs. 
        ' A KeyValuePair contains a Control of a PageView object, and a Property Name of
        ' PropertyPage object.
        Private _propertyControlTable As PropertyControlTable


        Public Sub New(ByVal pageViewSite As IPageViewSite, ByVal propertyPageUI As IPropertyPageUI, ByVal propertyControlTable As PropertyControlTable)
            Me._propertyControlTable = propertyControlTable
            Me._pageViewSite = pageViewSite
            Me._propertyPageUI = propertyPageUI
        End Sub

        ''' <summary>
        ''' Initialize the Controls on a PageView Object using the Properties of
        ''' a PropertyPage object. 
        ''' </summary>
        Public Sub InitializeControls()
            RemoveHandler _propertyPageUI.UserEditComplete, AddressOf propertyPageUI_UserEditComplete
            For Each str As String In Me._propertyControlTable.GetPropertyNames()

                Dim valueForProperty As String = Me._pageViewSite.GetValueForProperty(str)

                Dim controlFromPropertyName As Control =
                    Me._propertyControlTable.GetControlFromPropertyName(str)

                Me._propertyPageUI.SetControlValue(controlFromPropertyName, valueForProperty)

            Next str
            AddHandler _propertyPageUI.UserEditComplete, AddressOf propertyPageUI_UserEditComplete
        End Sub

        ''' <summary>
        ''' Notify the PropertyPage object that a Control value is changed.
        ''' </summary>
        Private Sub propertyPageUI_UserEditComplete(ByVal control As Control,
                                                    ByVal value As String)

            Dim propertyNameFromControl As String =
                Me._propertyControlTable.GetPropertyNameFromControl(control)

            Me._pageViewSite.PropertyChanged(propertyNameFromControl, value)
        End Sub

    End Class
End Namespace
