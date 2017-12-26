'*************************** Module Header ******************************'
' Module Name:  CONTEXT_MENU_CONST.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' The class CONTEXT_MENU_CONST defines the constants that specify the identifier
' of the shortcut menu to be displayed. These values are defined in Mshtmhst.h. 
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Namespace NativeMethods
    Public NotInheritable Class CONTEXT_MENU_CONST
        ' The default shortcut menu for a Web page.
        Public Const CONTEXT_MENU_DEFAULT As Integer = 0

        ' Shortcut menu for images. 
        Public Const CONTEXT_MENU_IMAGE As Integer = 1

        ' Shortcut menu for scrollbars and select elements. 
        Public Const CONTEXT_MENU_CONTROL As Integer = 2

        ' Not used. 
        Public Const CONTEXT_MENU_TABLE As Integer = 3

        ' Shortcut menu for selected text. 
        Public Const CONTEXT_MENU_TEXTSELECT As Integer = 4

        ' Shortcut menu for hyperlinks. 
        Public Const CONTEXT_MENU_ANCHOR As Integer = 5

        ' Not used. 
        Public Const CONTEXT_MENU_UNKNOWN As Integer = 6

        ' Internal used.
        Public Const CONTEXT_MENU_IMGDYNSRC As Integer = 7
        Public Const CONTEXT_MENU_IMGART As Integer = 8
        Public Const CONTEXT_MENU_DEBUG As Integer = 9

        ' Shortcut menu for vertical scroll bar. 
        Public Const CONTEXT_MENU_VSCROLL As Integer = 10

        ' Shortcut menu for horizontal scroll bar. 
        Public Const CONTEXT_MENU_HSCROLL As Integer = 11
    End Class
End Namespace

