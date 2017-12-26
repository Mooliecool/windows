'************************** Module Header ******************************'
' Module Name:  IPropertyStore.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' The IPropertyStore Interface is implemented by the CustomPropertyPagePropertyStore
' Class. 
' 
' It is used to store the Properties of a PropertyPage object.
' 
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Namespace PropertyPageBase

    Public Delegate Sub StoreChangedDelegate()

    Public Interface IPropertyStore

        Event StoreChanged As StoreChangedDelegate

        Sub Dispose()

        Sub Initialize(ByVal dataObject() As Object)

        Sub Persist(ByVal propertyName As String, ByVal propertyValue As String)

        Function PropertyValue(ByVal propertyName As String) As String

    End Interface
End Namespace
