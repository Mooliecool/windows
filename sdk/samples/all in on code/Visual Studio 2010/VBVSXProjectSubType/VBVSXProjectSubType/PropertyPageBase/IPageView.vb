'************************** Module Header ******************************'
' Module Name:  IPageView.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' The IPageView Interface is implemented by the PageView Class. 
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
Imports System.Drawing

Namespace PropertyPageBase

    Public Interface IPageView
        Inherits IDisposable
        Sub HideView()

        Sub Initialize(ByVal parentControl As Control, ByVal rectangle As Rectangle)

        Sub MoveView(ByVal rectangle As Rectangle)

        Function ProcessAccelerator(ByRef message As Message) As Integer

        Sub RefreshPropertyValues()

        Sub ShowView()

        ReadOnly Property ViewSize() As Size
    End Interface

End Namespace
