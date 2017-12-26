'************************************* Module Header **************************************\
' Module Name:	UC_DetectMouseEvent.vb
' Project:		VBWinFormDesigner
' Copyright (c) Microsoft Corporation.
' 
' 
' The DetectMouseEvent sample demonstrates how to detect mouse events at design time.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.

' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Windows.Forms.Design
Imports System.ComponentModel

Namespace MouseEvents
    <Designer(GetType(UC_DetectMouseEventDesigner))> _
    Public Class UC_DetectMouseEvent

    End Class


    Friend Class UC_DetectMouseEventDesigner
        Inherits ControlDesigner

        Protected Overrides Function GetHitTest(ByVal point As Point) As Boolean
            MessageBox.Show(("Mouse at: " & point.ToString))
            Return MyBase.GetHitTest(point)
        End Function

        Protected Overrides Sub OnMouseEnter()
            MyBase.OnMouseEnter()
        End Sub

        Protected Overrides Sub OnMouseLeave()
            MyBase.OnMouseLeave()
        End Sub

    End Class
End Namespace