'************************************* Module Header **************************************\
' Module Name:	UC_AdornmentPainting.vb
' Project:		VBWinFormDesigner
' Copyright (c) Microsoft Corporation.
' 
' 
' The AdornmentPainting sample demonstrates how to paint additional adornments on top of 
' the control.  
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.

' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Windows.Forms.Design
Imports System.Drawing.Drawing2D
Imports System.ComponentModel

Namespace Adornment
    <Designer(GetType(UC_AdornmentPaintingDesigner))> _
    Public Class UC_AdornmentPainting

    End Class

    Friend Class UC_AdornmentPaintingDesigner
        Inherits ControlDesigner

        Protected Overrides Sub OnPaintAdornments(ByVal pe As PaintEventArgs)
            Dim uc As UC_AdornmentPainting = DirectCast(MyBase.Component, UC_AdornmentPainting)
            Dim clientRectangle As Rectangle = uc.ClientRectangle
            Using pen As Pen = New Pen(Color.Red)
                pen.DashStyle = DashStyle.Dot
                clientRectangle.Width -= 1
                clientRectangle.Height -= 1
                pe.Graphics.DrawRectangle(pen, clientRectangle)
            End Using
            MyBase.OnPaintAdornments(pe)
        End Sub

    End Class
End Namespace

