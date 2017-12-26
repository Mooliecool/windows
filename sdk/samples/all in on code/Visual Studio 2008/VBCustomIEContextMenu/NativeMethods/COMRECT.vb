'*************************** Module Header ******************************'
' Module Name:  COMRECT.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' The class COMRECT is used to define the outer rectangle of the border.
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

Imports System.Drawing
Imports System.Runtime.InteropServices

Namespace NativeMethods
    <StructLayout(LayoutKind.Sequential)> _
    Public Class COMRECT
        Public left As Integer
        Public top As Integer
        Public right As Integer
        Public bottom As Integer

        Public Overrides Function ToString() As String
            Return String.Concat( _
                New Object() {" Left = ", Me.left, _
                              " Top = ", Me.top, _
                              " Right = ", Me.right, _
                              " Bottom = ", Me.bottom})
        End Function

        Public Sub New()
        End Sub

        Public Sub New(ByVal r As Rectangle)
            Me.left = r.X
            Me.top = r.Y
            Me.right = r.Right
            Me.bottom = r.Bottom
        End Sub

        Public Sub New(ByVal left As Integer, _
                       ByVal top As Integer, _
                       ByVal right As Integer, _
                       ByVal bottom As Integer)
            Me.left = left
            Me.top = top
            Me.right = right
            Me.bottom = bottom
        End Sub

        Public Shared Function FromXYWH(ByVal x As Integer, _
                                        ByVal y As Integer, _
                                        ByVal width As Integer, _
                                        ByVal height As Integer) _
                                    As NativeMethods.COMRECT
            Return New NativeMethods.COMRECT(x, y, x + width, y + height)
        End Function
    End Class
End Namespace
