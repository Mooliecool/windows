'************************** Module Header ******************************'
' Module Name:  IPropertyPageUI.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' The IPropertyPageUI Interface is implemented by the PageView Class. It
' provides the methods to get / set the value of the Controls on a PageView object. 
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

Imports System.Windows.Forms

Namespace PropertyPageBase
    Public Delegate Sub UserEditCompleteHandler(ByVal control As Control,
                                                ByVal value As String)

    Public Interface IPropertyPageUI

        Event UserEditComplete As UserEditCompleteHandler

        Function GetControlValue(ByVal control As Control) As String

        Sub SetControlValue(ByVal control As Control, ByVal value As String)

    End Interface
End Namespace
